// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using spidev driver)
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
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define TRANSFER_FULL_DUPLEX 0
#define TRANSFER_TX_ONLY     1
#define TRANSFER_RX_ONLY     2

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev0.0";
static const char *device_slave = "/dev/spidev1.0";
static const char *spi_role = "MASTER";
static uint32_t mode;
static uint8_t bits = 8;
static uint8_t transfer_type = 0;
static char *input_file;
static char *output_file;
static uint32_t speed = 7000000;
static uint16_t delay = 100;
static int verbose;
static int transfer_size;
static int interval = 5; /* interval in seconds for showing transfer rate */

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
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (transfer_type == TRANSFER_TX_ONLY) {
		tr.rx_buf = 0;
	}

	if (transfer_type == TRANSFER_RX_ONLY) {
		tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) {
		pabort("can't send spi message");
	}

	if (verbose && (transfer_type != TRANSFER_RX_ONLY)) {
		printf("SPI-%s\n", spi_role);
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
		printf("SPI-%s\n", spi_role);
		hex_dump(rx, len, 32, "RX");
	}

}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-RMSTsbdHOvplI]\n", prog);
	puts("  -R --role(MASTER/SLAVE) SPI Master or Slave (default MASTER)\n"
		 "  -M --masterdevice       device to use for SPI Master (default /dev/spidev0.0)\n"
		 "  -S --slavedevice        device to use for SPI Slave (default /dev/spidev1.0)\n"
		 "  -T --transfertype       0 for fullduplex(default); 1 for tx-only; 2 for rx-only\n"
		 "  -s --speed              max speed (Hz) (default 1MHz)\n"
		 "  -d --delay              delay (usec) (default 100usec)\n"
		 "  -b --bpw                bits per word (default 8)\n"
		 "  -i --input              input data from a file (e.g. \"test.bin\")\n"
		 "  -o --output             output data to a file (e.g. \"results.bin\")\n"
		 "  -H --cpha               clock phase (default 0)\n"
		 "  -O --cpol               clock polarity (default 0)\n"
		 "  -v --verbose            Verbose (show tx buffer) (default OFF)\n"
		 "  -p                      Send data (e.g. \"1234\\xde\\xad\")\n"
		 "  -l --size               transfer size/length of buf to transfer\n");

	printf("Examples:\n");
	puts("Execute SLAVE full duplex TX/RX test with 64 bytes as transfer length 'rtk_spi_test -R SLAVE -T 0 -l 64'\n"
		 "Execute MASTER full duplex TX/RX test with 64 bytes as transfe length 'rtk_spi_test -R MASTER -T 0 -l 64'\n"
		 "For more examples, please refer to spi_test_script.sh included in spi tests src code.\n");

	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "role(MASTER/SLAVE)",  1, 0, 'R' },
			{ "masterdevice",        1, 0, 'M' },
			{ "slavedevice",         1, 0, 'S' },
			{ "transfertype",        1, 0, 'T' },
			{ "speed",               1, 0, 's' },
			{ "delay",               1, 0, 'd' },
			{ "bpw",                 1, 0, 'b' },
			{ "input",               1, 0, 'i' },
			{ "output",              1, 0, 'o' },
			{ "cpha",                0, 0, 'H' },
			{ "cpol",                0, 0, 'O' },
			{ "verbose",             0, 0, 'v' },
			{ "size",                1, 0, 'l' },
			{ NULL,                  0, 0, 0 },
		};
		int c;

		if (argc == 1) {
			print_usage(argv[0]);
			break;
		}

		c = getopt_long(argc, argv, "R:M:S:T:s:d:b:i:o:HOp:vl:I:",
						lopts, NULL);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 'R':
			spi_role = optarg;
			break;
		case 'M':
			device = optarg;
			break;
		case 'S':
			device_slave = optarg;
			break;
		case 'T':
			transfer_type = atoi(optarg);
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'p':
			input_tx = optarg;
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

	if (memcmp(tx, rx, size)) {
		fprintf(stderr, "transfer error !\n");
		hex_dump(tx, size, 32, "TX");
		hex_dump(rx, size, 32, "RX");
		exit(1);
	} else {
		printf("%d bytes transfer success !\n", size);
	}

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

	if (memcmp(tx, rx, sb.st_size)) {
		fprintf(stderr, "transfer error !\n");
		hex_dump(tx, sb.st_size, 32, "TX");
		hex_dump(rx, sb.st_size, 32, "RX");
		exit(1);
	} else {
		printf("%d bytes transfier success !\n", sb.st_size);
	}

	free(rx);
	free(tx);
	close(tx_fd);
}

static uint64_t _read_count;
static uint64_t _write_count;

static void show_transfer_rate(void)
{
	static uint64_t prev_read_count, prev_write_count;
	double rx_rate, tx_rate;

	rx_rate = ((_read_count - prev_read_count) * 8) / (interval * 1000.0);
	tx_rate = ((_write_count - prev_write_count) * 8) / (interval * 1000.0);

	printf("rate: tx %.1fkbps, rx %.1fkbps\n", rx_rate, tx_rate);

	prev_read_count = _read_count;
	prev_write_count = _write_count;
}

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

	if ((transfer_type == TRANSFER_FULL_DUPLEX) || (transfer_type == TRANSFER_RX_ONLY)) {
		if (memcmp(tx, rx, len)) {
			fprintf(stderr, "transfer error !\n");
			hex_dump(tx, len, 32, "TX");
			hex_dump(rx, len, 32, "RX");
			exit(1);
		} else {
			printf("SPI-%s %d bytes transfer success !\n", spi_role, len);
		}
	}

	free(rx);
	free(tx);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;

	parse_opts(argc, argv);

	if (!strcmp(spi_role, "MASTER")) {
		printf("SPI Master test with device %s\n", device);
		fd = open(device, O_RDWR);
	} else if (!strcmp(spi_role, "SLAVE")) {
		printf("SPI Slave test with device %s\n", device_slave);
		fd = open(device_slave, O_RDWR);
	} else {
		printf("-R --role must be MASTER or SLAVE\n");
		pabort("invalid spi role");
	}

	if (fd < 0) {
		pabort("can't open device");
	}

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1) {
		pabort("can't set spi mode");
	}

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1) {
		pabort("can't get spi mode");
	}

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		pabort("can't set bits per word");
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1) {
		pabort("can't get bits per word");
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		pabort("can't set max speed hz");
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		pabort("can't get max speed hz");
	}

	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);
	printf("transfer_type: %s\n", ((transfer_type == 0) ? "FULL DUPLEX" :
								   ((transfer_type == 1) ? "TX_ONLY" : "RX_ONLY")));

	if (input_tx && input_file) {
		pabort("only one of -p and --input may be selected");
	}

	if (input_tx) {
		transfer_escaped_string(fd, input_tx);
	} else if (input_file) {
		transfer_file(fd, input_file);
	} else if (transfer_size) {
		struct timespec last_stat;
		struct timespec current;

		clock_gettime(CLOCK_MONOTONIC, &last_stat);

		transfer_buf(fd, transfer_size);

		clock_gettime(CLOCK_MONOTONIC, &current);
		if (current.tv_sec - last_stat.tv_sec > interval) {
			show_transfer_rate();
			last_stat = current;
		}
		printf("total: tx %.1fKB, rx %.1fKB\n",
			   _write_count / 1024.0, _read_count / 1024.0);
	} else {
		transfer(fd, default_tx, default_rx, sizeof(default_tx));

		if ((transfer_type == TRANSFER_RX_ONLY) || (transfer_type == TRANSFER_FULL_DUPLEX)) {

			if (memcmp(default_tx, default_rx, sizeof(default_tx))) {
				fprintf(stderr, "transfer error !\n");
				hex_dump(default_tx, sizeof(default_tx), 32, "TX");
				hex_dump(default_rx, sizeof(default_tx), 32, "RX");
				exit(1);
			} else {
				printf("SPI-%s: %d bytes transfer success !\n", spi_role, sizeof(default_tx));
			}
		}
	}

	close(fd);

	return ret;
}
