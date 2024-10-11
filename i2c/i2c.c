// SPDX-License-Identifier: GPL-2.0-only
/*
 * I2C testing utility (using i2cdev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define TRANSFER_TX_ONLY     1
#define TRANSFER_RX_ONLY     2

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static char *device = "/dev/i2c-0";
static uint16_t address = 0x23;
static uint8_t transfer_type = 0; /* TRANSFER_TX_ONLY/TRANSFER_RX_ONLY */
static char *input_file;
static char *output_file;
static int verbose;
static int transfer_size = 0;
static int ten_bit_mode = 0;

uint8_t default_tx[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0x0D,
};

uint8_t default_rx[ARRAY_SIZE(default_tx)] = {0, };
char *input_tx;

static void hex_dump(const void *src, size_t length, size_t line_size,
					 char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size) {
					printf("__ ");
				}
			}
			printf(" |");
			while (line < address) {
				c = *line++;
				printf("%c", (c < 32 || c > 126) ? '.' : c);
			}
			printf("|\n");
			if (length > 0) {
				printf("%s | ", prefix);
			}
		}
	}
}

/*
 *  Unescape - process hexadecimal escape character
 *      converts shell input "\x23" -> 0x23
 */
static int unescape(char *_dst, char *_src, size_t len)
{
	int ret = 0;
	int match;
	char *src = _src;
	char *dst = _dst;
	unsigned int ch;

	while (*src) {
		if (*src == '\\' && *(src + 1) == 'x') {
			match = sscanf(src + 2, "%2x", &ch);
			if (!match) {
				pabort("malformed input string");
			}

			src += 4;
			*dst++ = (unsigned char)ch;
		} else {
			*dst++ = *src++;
		}
		ret++;
	}
	return ret;
}

static void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;
	int out_fd;
	struct i2c_msg test_msgs[1] = {
		{
			.addr = address,
			.flags = ((transfer_type == TRANSFER_RX_ONLY) ? 1 : 0) | (ten_bit_mode ? I2C_M_TEN : 0), // write
			.len = len,
			.buf = (transfer_type == TRANSFER_RX_ONLY) ? rx : tx,
		},
	};
	struct i2c_rdwr_ioctl_data ioctl_data = {
		.msgs = test_msgs,
		.nmsgs = 1,
	};

	ret = ioctl(fd, I2C_RDWR, &ioctl_data);
	if (ret < 1) {
		pabort("Call i2c transfer fail.");
	}

	if (verbose && (transfer_type != TRANSFER_RX_ONLY)) {
		printf("I2C Master TX content:\n");
		hex_dump(tx, len, 32, "TX");
	}

	if (output_file && (transfer_type != TRANSFER_TX_ONLY)) {
		out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (out_fd < 0) {
			pabort("could not open output file");
		}

		ret = write(out_fd, rx, len);
		if (ret != len) {
			pabort("not all bytes written to output file");
		}

		close(out_fd);
	}

	if (verbose && (transfer_type != TRANSFER_TX_ONLY)) {
		printf("I2C Master RX content: \n");
		hex_dump(rx, len, 32, "RX");
	}
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-RMSTsbdHOvplI]\n", prog);
	puts("	    -M --masterdevice       device to use for I2C Master (default /dev/i2c-0)\n"
		 "  -a --address            send data to slave address (default 0x23)\n"
		 "  -b --addr10bit          slave address is in ten-bit mode\n"
		 "  -R --read               master read data form slave device\n"
		 "  -W --write              master write data to slave device\n"
		 "  -i --input              input data from a file (e.g. \"test.bin\")\n"
		 "  -o --output             output data to a file (e.g. \"results.bin\")\n"
		 "  -v --verbose            Verbose (show tx buffer) (default OFF)\n"
		 "  -l --size               transfer size/length of buf to transfer\n");

	printf("Examples:\n");
	puts("Execute I2C MASTER TX by default 'rtk_i2c_test -W -v'\n"
		 "Execute I2C MASTER RX by default 'rtk_i2c_test -R -v'\n"
		 "For more examples, please refer to i2c_test_script.sh included in i2c tests src code.\n");

	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "masterdevice",	1, 0, 'M' },
			{ "address",		1, 0, 'a' },
			{ "addr10bit",		0, 0, 'b' },
			{ "read",		0, 0, 'R' },
			{ "write",		0, 0, 'W' },
			{ "input",		1, 0, 'i' },
			{ "output",		1, 0, 'o' },
			{ "verbose",		0, 0, 'v' },
			{ "size",		1, 0, 'l' },
			{ NULL,			0, 0, 0 },
		};
		int c;

		if (argc == 1) {
			print_usage(argv[0]);
			break;
		}

		c = getopt_long(argc, argv, "M:a:bRWt:r:i:o:vl:n:", lopts, NULL);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 'M':
			device = optarg;
			break;
		case 'a':
			address = atoi(optarg);
			break;
		case 'b':
			ten_bit_mode = 1;
			break;
		case 'R':
			if (!transfer_type) {
				transfer_type = TRANSFER_RX_ONLY;
			} else {
				pabort("I2C master can only rx or tx for one transfer.");
			}
			break;
		case 'W':
			if (!transfer_type) {
				transfer_type = TRANSFER_TX_ONLY;
			} else {
				pabort("I2C master can only rx or tx for one transfer.");
			}
			break;
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'l':
			transfer_size = atoi(optarg);
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

static void transfer_escaped_string(int fd, char *str)
{
	size_t size = strlen(str);
	uint8_t *tx;
	uint8_t *rx;

	tx = malloc(size);
	if (!tx) {
		pabort("can't allocate tx buffer");
	}

	rx = malloc(size);
	if (!rx) {
		pabort("can't allocate rx buffer");
	}

	size = unescape((char *)tx, str, size);
	transfer(fd, tx, rx, size);

	free(rx);
	free(tx);
}

static void transfer_file(int fd, char *filename)
{
	ssize_t bytes;
	struct stat sb;
	int tx_fd;
	uint8_t *tx;
	uint8_t *rx;

	if (stat(filename, &sb) == -1) {
		pabort("can't stat input file");
	}

	tx_fd = open(filename, O_RDONLY);
	if (tx_fd < 0) {
		pabort("can't open input file");
	}

	tx = malloc(sb.st_size);
	if (!tx) {
		pabort("can't allocate tx buffer");
	}

	rx = malloc(sb.st_size);
	if (!rx) {
		pabort("can't allocate rx buffer");
	}

	bytes = read(tx_fd, tx, sb.st_size);
	if (bytes != sb.st_size) {
		pabort("failed to read input file");
	}

	transfer(fd, tx, rx, sb.st_size);

	free(rx);
	free(tx);
	close(tx_fd);
}

static uint64_t _read_count;
static uint64_t _write_count;

static void transfer_buf(int fd, int len)
{
	uint8_t *tx;
	uint8_t *rx;
	int i;

	tx = malloc(len);
	if (!tx) {
		pabort("can't allocate tx buffer");
	}
	for (i = 0; i < len; i++) {
		tx[i] = i;
	}

	rx = malloc(len);
	if (!rx) {
		pabort("can't allocate rx buffer");
	}

	transfer(fd, tx, rx, len);

	_write_count += len;
	_read_count += len;

	free(rx);
	free(tx);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;

	parse_opts(argc, argv);

	if (!transfer_type) {
		pabort("Must assign TX or RX mode by using option -W or -R.\n");
	}

	printf("Start I2C-Master %s TEST on device %s to slave address 0x%x.\n",
		(transfer_type == TRANSFER_RX_ONLY) ? "RX" : "TX", device, address);
	if (!verbose) {
		printf("Use -v to show TRX transfer buf.\n");
	}

	fd = open(device, 2); // O_RDWR
	if (fd == -1){
		printf("open %s test error\n", device);
		pabort("Can't open device");
		return ret;
	}

	if (input_tx && input_file) {
		pabort("only one of -p and --input may be selected");
	}

	if (input_tx) {
		transfer_escaped_string(fd, input_tx);
	} else if (input_file) {
		transfer_file(fd, input_file);
	} else if (transfer_size) {
		transfer_buf(fd, transfer_size);
	} else {
		transfer(fd, default_tx, default_rx, sizeof(default_tx));
	}

	close(fd);
	return ret;
}
