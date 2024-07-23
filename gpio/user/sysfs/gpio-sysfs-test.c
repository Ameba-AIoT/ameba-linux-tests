// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek GPIO test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

char *setting;
static char gpio_path[100];
typedef enum {
	APP_MODE_GPIO_READ,
	APP_MODE_GPIO_WRITE,
	APP_MODE_GPIO_INT,
	APP_MODE_GPIO_POLL,
	APP_MODE_UNKNOWN
} app_mode_t;

static void gpio_export(char *gpio)
{
	int fd;
	int len;

	printf("GPIO test export pin %s\n", gpio);

	sprintf(gpio_path, "/sys/class/gpio/gpio%s", gpio);
	/*export pin*/
	if (access(gpio_path, F_OK)) {
		if (0 > (fd = open("/sys/class/gpio/export", O_WRONLY))) {
			printf("open export error\n");
			exit(-1);
		}
		len = strlen(gpio);
		if (len != write(fd, gpio, len)) {
			printf("write export error\n");
			close(fd);
			exit(-1);
		}

		close(fd);
	}

}
static void gpio_unexport(char *gpio)//GPIOX attribute config
{
	int fd;
	int len;

	printf("GPIO test unexport pin %s\n", gpio);
	/*unexport pin*/
	if (0 > (fd = open("/sys/class/gpio/unexport", O_WRONLY))) {
		printf("open unexport error\n");
		exit(-1);
	}
	len = strlen(gpio);
	if (len != write(fd, gpio, len)) {
		printf("write unexport error\n");
		close(fd);
		exit(-1);
	}

	close(fd);
}

static int gpio_config(const char *attr, const char *val)//GPIOX attribute config
{
	char file_path[100];
	int fd;
	int len;

	sprintf(file_path, "%s/%s", gpio_path, attr);
	printf("gpio_config: set attr %s:%s\n", attr, val);
	if (0 > (fd = open(file_path, O_WRONLY))) {
		printf("open %s error", file_path);
		return fd;
	}

	len = strlen(val);
	if (len > write(fd, val, len)) {
		printf("write %s error\n", file_path);
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}


static char gpio_get_value(void)
{

	char val;
	int fd;
	char file_path[100];

	sprintf(file_path, "%s/%s", gpio_path, "value");
	if (0 > (fd = open(file_path, O_RDONLY))) {
		printf("open value error\n");
		exit(-1);
	}

	if (0 > read(fd, &val, 1)) {
		printf("read value error\n");
		close(fd);
		exit(-1);
	}

	close(fd);

	return val;
}

static void gpio_read(void)
{

	printf("GPIO read test start...\n");

	if (gpio_config("direction", "in")) {
		exit(-1);
	}

	if (gpio_config("active_low", "0")) {
		exit(-1);
	}

	if (gpio_config("edge", "none")) {
		exit(-1);
	}

	printf("%s value: %c\n", __FUNCTION__, gpio_get_value());

}

static void gpio_write(char *set)
{
	printf("GPIO write test start...\n");

	if (gpio_config("direction", "out")) {
		exit(-1);
	}

	if (gpio_config("active_low", "0")) {
		exit(-1);
	}
	if (gpio_config("edge", "none")) {
		exit(-1);
	}
	if (gpio_config("value", set)) {
		exit(-1);
	}
	printf("%s value: %c\n", __FUNCTION__, gpio_get_value());

}


static void   gpio_edge_int(char *set)
{
	struct pollfd pfd;
	char file_path[100];
	int ret;
	char val;

	printf("GPIO edge interrupt test start...\n");

	if (gpio_config("direction", "in")) {
		exit(-1);
	}

	if (gpio_config("edge", set)) {
		exit(-1);
	}

	sprintf(file_path, "%s/%s", gpio_path, "value");
	if (0 > (pfd.fd = open(file_path, O_RDONLY))) {
		printf("open error\n");
		exit(-1);
	}
	pfd.events = POLLPRI;
	read(pfd.fd, &val, 1);
	printf("GPIO int <value=%c>\n", val);

	while (1) {
		ret = poll(&pfd, 1, -1);
		if (0 > ret) {
			printf("poll error\n");
			exit(-1);
		} else if (0 == ret) {
			fprintf(stderr, "poll timeout.\n");
			continue;
		}

		if (pfd.revents & POLLPRI) {
			if (0 > lseek(pfd.fd, 0, SEEK_SET)) {
				printf("lseek error\n");
				exit(-1);
			}
			if (0 > read(pfd.fd, &val, 1)) {
				printf("read error\n");
				exit(-1);
			}
			printf("GPIO int <value=%c>\n", val);
		}
	}

	close(pfd.fd);
}

static void help(const char *app)
{
	fprintf(stderr, "usage: %s -n <gpio> <mode>\n"
			"\t -n <gpio>: select GPIO pin num,must use with mode option\n"
			"mode:\n"
			"\t\t -r :Read GPIO value at offset (INPUT mode)\n"
			"\t\t -w <0|1>: Write GPIO value (OUTPUT mode). \n"
			"\t\t -i <rising |falling|both>: value that set interrupt trigger type (INT mode)\n",
			app);
}

int main(int argc, char **argv)
{
	int fd;
	int len;
	char *gpio;
	int ret;
	app_mode_t mode = APP_MODE_UNKNOWN;

	while ((ret = getopt(argc, argv, "n:rw:i:")) != -1) {
		switch (ret) {
		case 'n':
			gpio = optarg;
			break;
		case 'r':
			mode = APP_MODE_GPIO_READ;
			break;
		case 'w':
			mode = APP_MODE_GPIO_WRITE;
			setting = optarg;
			break;
		case 'i':
			mode = APP_MODE_GPIO_INT;
			setting = optarg;
			break;
		default:
			help(argv[0]);
			return -1;
		}
	}

	if (mode == APP_MODE_UNKNOWN) {
		help(argv[0]);
		return -1;
	}

	gpio_export(gpio);
	switch (mode) {
	case APP_MODE_GPIO_WRITE:
		gpio_write(setting);
		break;
	case APP_MODE_GPIO_READ:
		gpio_read();
		break;
	case APP_MODE_GPIO_INT:
		gpio_edge_int(setting);
		break;
	default:
		gpio_unexport(gpio);
		help(argv[0]);
		return -1;
	}

	gpio_unexport(gpio);
	printf("Test GPIO SYSFS over!!\n");

	return 0;
}


