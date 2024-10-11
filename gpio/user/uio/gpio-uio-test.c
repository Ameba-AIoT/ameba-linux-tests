// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek GPIO test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <poll.h>

typedef enum {
	APP_MODE_GPIO_INT,
	APP_MODE_GPIO_POLL,
	APP_MODE_UNKNOWN
} app_mode_t;

int uio_fd;

static void   gpio_int(void)
{
	int ret;
	int i;

	printf("GPIO interrupt test start...\n");

	for (i = 0; i < 10 ; i++) {
		uint32_t uio_int = 1; /* unmask */

		ret = write(uio_fd, &uio_int, sizeof(uio_int));
		if (ret != sizeof(uio_int)) {
			perror("write");
			close(uio_fd);
			exit(EXIT_FAILURE);
		}

		/* Wait for interrupt */
		ret = read(uio_fd, &uio_int, sizeof(uio_int));
		if (ret == sizeof(uio_int)) {
			/* Do something in response to the interrupt. */
			printf("Interrupt #%u!\n", uio_int);
		}

	}

	close(uio_fd);

}

static void   gpio_int_poll(void)
{
	struct pollfd pfd;
	int ret;

	printf("GPIO interrupt poll test start...\n");
	pfd.fd = uio_fd;
	pfd.events = POLLIN;

	while (1) {
		uint32_t uio_int = 1; /* unmask */
		ret = write(uio_fd, &uio_int, sizeof(uio_int));//must write before once read
		if (ret != sizeof(uio_int)) {
			perror("write");
			close(uio_fd);
			exit(EXIT_FAILURE);
		}

		ret = poll(&pfd, 1, -1);
		printf("poll ret=%d!\n",ret);

		if (ret >= 1) {
			ret = read(uio_fd, &uio_int, sizeof(uio_int));
			if (ret == sizeof(uio_int)) {
				/* Do something in response to the interrupt. */
				printf("POLL Interrupt #%u!\n", uio_int);
			}
		} else {
			perror("poll()");
			close(uio_fd);
			exit(EXIT_FAILURE);
		}

	}

	close(uio_fd);

}

void usage(void)
{
	printf("*argv[0] -d <UIO_DEV_FILE> -i|-p\n");
	printf("    -d       UIO device file. e.g. uio0\n");
	printf("    -i        interrupt of GPIO\n");
	printf("    -p       poll interrupt of GPIO\n");
	return;
}

int main(int argc, char *argv[])
{
	char dev_path[50];
	char *uiod;
	int ret;
	app_mode_t mode = APP_MODE_UNKNOWN;

	printf("GPIO UIO interrupt test.\n");
	while ((ret = getopt(argc, argv, "d:ip")) != -1) {
		switch (ret) {
		case 'd':
			uiod = optarg; //uio0/1/2
			break;
		case 'i':
			mode = APP_MODE_GPIO_INT;
			break;
		case 'p':
			mode = APP_MODE_GPIO_POLL;
			break;
		default:
			usage();
			return -1;
		}

	}

	if (mode == APP_MODE_UNKNOWN) {
		usage();
		return -1;
	}

	/* Open the UIO device file */
	sprintf(dev_path, "/dev/%s", uiod);
	uio_fd = open(dev_path, O_RDWR);
	if (uio_fd < 0) {
		perror("open dev_path");
		exit(EXIT_FAILURE);
	}

	switch (mode) {
	case APP_MODE_GPIO_INT:
		gpio_int();
		break;
	case APP_MODE_GPIO_POLL:
		gpio_int_poll();
		break;
	default:
		usage();
		return -1;
	}

	printf("GPIO UIO test over.\n");

	close(uio_fd);

	return 0;
}


