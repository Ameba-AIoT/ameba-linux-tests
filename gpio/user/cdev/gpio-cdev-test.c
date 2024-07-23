// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek GPIO test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/gpio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/poll.h>

typedef enum {
	APP_MODE_GPIO_LIST,
	APP_MODE_GPIO_READ,
	APP_MODE_GPIO_WRITE,
	APP_MODE_GPIO_INT,
	APP_MODE_GPIO_POLL,
	APP_MODE_UNKNOWN
} app_mode_t;

typedef struct {
	int offset;
	uint8_t val;
	app_mode_t mode;
} app_opt_t;

static char gpio_chip_path[50];

static void gpio_list(const char *dev_name)
{
	struct gpiochip_info info;
	struct gpioline_info line_info;
	int fd, ret;

	printf("Test: List gpio chip info  on chip %s\n", dev_name);

	fd = open(dev_name, O_RDONLY);
	if (fd < 0) {
		printf("Unabled to open %s: %s", dev_name, strerror(errno));
		return;
	}
	ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info);
	if (ret == -1) {
		printf("Unable to get chip info from ioctl: %s", strerror(errno));
		close(fd);
		return;
	}
	printf("Chip name: %s\n", info.name);
	printf("Chip label: %s\n", info.label);
	printf("Number of lines: %d\n", info.lines);

	for (int i = 0; i < info.lines; i++) {
		line_info.line_offset = i;
		ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &line_info);
		if (ret == -1) {
			printf("Unable to get line info from offset %d: %s\n", i, strerror(errno));
		} else {
			printf("offset: %d, name: %s, consumer: %s. Flags:\t[%s]\t[%s]\t[%s]\t[%s]\t[%s]\n",
				   i,
				   line_info.name,
				   line_info.consumer,
				   (line_info.flags & GPIOLINE_FLAG_IS_OUT) ? "OUTPUT" : "INPUT",
				   (line_info.flags & GPIOLINE_FLAG_ACTIVE_LOW) ? "ACTIVE_LOW" : "ACTIVE_HIGHT",
				   (line_info.flags & GPIOLINE_FLAG_OPEN_DRAIN) ? "OPEN_DRAIN" : "...",
				   (line_info.flags & GPIOLINE_FLAG_OPEN_SOURCE) ? "OPENSOURCE" : "...",
				   (line_info.flags & GPIOLINE_FLAG_KERNEL) ? "KERNEL" : "");
		}
	}
	if (close(fd) == -1) {
		printf("%s Failed to close GPIO character device file", __FUNCTION__);
	}

}

static void gpio_write(const char *dev_name, int offset, uint8_t value)
{
	struct gpiohandle_request rq;
	struct gpiohandle_data data;
	int fd, ret;

	printf("Test: Write value %d to GPIO at offset %d on chip %s (OUTPUT mode) \n", value, offset, dev_name);

	fd = open(dev_name, O_RDONLY);
	if (fd < 0) {
		printf("Unabled to open %s: %s\n", dev_name, strerror(errno));
		return;
	}
	rq.lineoffsets[0] = offset;
	rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
	rq.lines = 1;
	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
	close(fd);
	if (ret == -1) {
		printf("Unable to line handle from ioctl : %s\n", strerror(errno));
		return;
	}
	data.values[0] = value;
	ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	if (ret == -1) {
		printf("Unable to set line value using ioctl : %n", strerror(errno));
	} else {

		printf("Write value %d to %s-%d success! \n", data.values[0], dev_name, offset);
	}
	if (close(rq.fd) == -1) {
		printf("%s Failed to close GPIO device file", __FUNCTION__);
	}

}

static void gpio_read(const char *dev_name, int offset)
{
	struct gpiohandle_request rq;
	struct gpiohandle_data data;
	int fd, ret;

	printf("Test: Read value from GPIO at offset %d on chip %s (INPUT mode)\n", offset, dev_name);

	fd = open(dev_name, O_RDONLY);
	if (fd < 0) {
		printf("Unabled to open %s: %s\n", dev_name, strerror(errno));
		return;
	}
	rq.lineoffsets[0] = offset;
	rq.flags = GPIOHANDLE_REQUEST_INPUT;
	rq.lines = 1;
	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
	close(fd);
	if (ret == -1) {
		printf("Unable to get line handle from ioctl : %s\n", strerror(errno));
		return;
	}
	ret = ioctl(rq.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
	if (ret == -1) {
		printf("Unable to get line value using ioctl : %s\n", strerror(errno));
	} else {
		printf("Read value of %s-%d success, value:%d\n", dev_name, offset, data.values[0]);
	}
	if (close(rq.fd) == -1) {
		printf("%s Failed to close GPIO device file", __FUNCTION__);
	}
}

static void gpio_int(const char *dev_name, int offset, uint8_t value)
{
	struct gpioevent_request event_req;
	struct gpioevent_data event_data;
	int fd, ret;

	printf("Test: Interrupt at GPIO offset %d (INT mode) on chip %s\n", offset, dev_name);

	fd = open(dev_name, O_RDONLY);
	if (fd < 0) {
		printf("Unabled to open %s: %s\n", dev_name, strerror(errno));
		return;
	}
	event_req.lineoffset = offset;
	event_req.handleflags  = GPIOHANDLE_REQUEST_INPUT;//must set
	event_req.eventflags = value;
	/*eventflags:
#define	GPIOEVENT_EVENT_RISING_EDGE 0x1
#define	GPIOEVENT_EVENT_FALLING_EDGE 0x2
#define	GPIOEVENT_EVENT_RISING_EDGE|GPIOEVENT_EVENT_FALLING_EDGE 0x3 //for both edge*/
	ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);
	close(fd);
	if (ret == -1) {
		printf("Unable to get line event from ioctl : %s\n", strerror(errno));
		close(fd);
		return;
	}
	while (1) {
			event_data.timestamp = 0;
			event_data.id = 0;
			read(event_req.fd, &event_data, sizeof(event_data));
			printf("event_data.timestamp:%llu, event_data.id:%d \n", event_data.timestamp, event_data.id);
	}

	/* release line */
	close(event_req.fd);
}

static void gpio_int_poll(const char *dev_name, int offset, uint8_t value)
{
	struct gpioevent_request event_req;
	struct gpioevent_data event_data;
	struct pollfd pfd;
	int fd, ret;

	printf("Test: Interrupt at GPIO offset %d (INT mode) on chip %s\n", offset, dev_name);

	fd = open(dev_name, O_RDONLY);
	if (fd < 0) {
		printf("Unabled to open %s: %s\n", dev_name, strerror(errno));
		return;
	}
	event_req.lineoffset = offset;
	event_req.handleflags  = GPIOHANDLE_REQUEST_INPUT;//must set
	event_req.eventflags = value;
	/*eventflags:
#defiene GPIOEVENT_EVENT_RISING_EDGE 0x1
#defiene GPIOEVENT_EVENT_FALLING_EDGE 0x2
#defiene GPIOEVENT_EVENT_RISING_EDGE|GPIOEVENT_EVENT_FALLING_EDGE 0x3 for both edge
	*/
	ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);
	close(fd);
	if (ret == -1) {
		printf("Unable to get line event from ioctl : %s\n", strerror(errno));
		close(fd);
		return;
	}
	pfd.fd = event_req.fd;
	pfd.events = POLLIN;
	while (1) {
		ret = poll(&pfd, 1, -1);
		if (ret == -1) {
			printf("Error while polling event from GPIO: %s\n", strerror(errno));
		} else if (pfd.revents & POLLIN) {
			event_data.timestamp = 0;
			event_data.id = 0;
			read(event_req.fd, &event_data, sizeof(event_data));
			printf("event_data.timestamp:%llu, event_data.id:%d \n", event_data.timestamp, event_data.id);
		}
	}

	/* release line */
	close(event_req.fd);
}

static void help(const char *app)
{
	fprintf(stderr,	"\r\nUsage: %s -c <gpio_chip> -n [gpio_offset] <mode>.\n"
			"\t -c <0|1|2>: select GPIO chip,/dev/gpiochip*, use with other option\n"
			"\t -l: print gpio chip info, only use with -c option\n"
			"\t -n <gpio_offset>: select GPIO offset,0~31,must use with mode option\n"
			"\tmode option:\n"
			"\t\t -r :Read GPIO value at offset (INPUT mode)\n"
			"\t\t -w <0|1>: Write GPIO value (OUTPUT mode)\n"
			"\t\t -i <1|2|3>: value that set interrupt trigger type(INT mode): 1 for rising edge, 2 for falling edge, 3 for both edge.\n"
			"\t\t -p <1|2|3>: value that set interrupt trigger type (INT mode+poll)\n",
			app);
}

int main(int argc, char *const *argv)
{
	int ret;
	app_opt_t opt;
	char *chip;
	opt.val = 0;
	opt.mode = APP_MODE_UNKNOWN;
	while ((ret = getopt(argc, argv, "c:n:lrw:i:p:")) != -1) {
		switch (ret) {
		case 'c':
			chip = optarg;
			break;
		case 'n':
			opt.offset = atoi(optarg);
			break;
		case 'l':
			opt.mode = APP_MODE_GPIO_LIST;
			break;
		case 'r':
			opt.mode = APP_MODE_GPIO_READ;
			break;
		case 'w':
			opt.mode = APP_MODE_GPIO_WRITE;
			opt.val = (uint8_t)atoi(optarg);
			break;
		case 'i':
			opt.mode = APP_MODE_GPIO_INT;
			opt.val = (uint8_t)atoi(optarg);
			break;
		case 'p':
			opt.mode = APP_MODE_GPIO_POLL;
			opt.val = (uint8_t)atoi(optarg);
			break;
		default:
			help(argv[0]);
			return -1;
		}
	}

	if (opt.mode == APP_MODE_UNKNOWN) {
		help(argv[0]);
		return -1;
	}

	sprintf(gpio_chip_path, "/dev/gpiochip%s", chip);
	switch (opt.mode) {
	case APP_MODE_GPIO_LIST:
		gpio_list(gpio_chip_path);
		break;
	case APP_MODE_GPIO_WRITE:
		gpio_write(gpio_chip_path, opt.offset, opt.val);
		break;
	case APP_MODE_GPIO_READ:
		gpio_read(gpio_chip_path, opt.offset);
		break;
	case APP_MODE_GPIO_INT:
		gpio_int(gpio_chip_path, opt.offset, opt.val);
		break;
	case APP_MODE_GPIO_POLL:
		gpio_int_poll(gpio_chip_path, opt.offset, opt.val);
		break;
	default:
		help(argv[0]);
		return -1;
	}
	return 0;
}

