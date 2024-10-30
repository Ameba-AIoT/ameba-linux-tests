// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Misc test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* Define new affairs in realtek-misc.h and implement relevant functions in Realtek misc driver. */
#include <ameba/realtek-misc.h>

#define READ_DATA_SIZE	1

int main(int argc, char **argv)
{
	int ret;
	int fd = 0;
	char buf[READ_DATA_SIZE];
	uint32_t val;

	printf("Misc test started\n");

	fd = open("/dev/misc-ioctl", O_RDWR);
	if (fd < 0) {
		printf("Fail to open misc device node.\n");
		return 1;
	}

	/* Example: Set affair then read rom info. */
	ret = ioctl(fd, RTK_MISC_IOC_RLV, &val);
	if (ret < 0) {
		printf("Fail to get MISC_IOC_RLV.\n");
		return 1;
	}

	printf("\nRL version: %d\n", val);

	ret = ioctl(fd, RTK_MISC_IOC_UUID, &val);
	if (ret < 0) {
		printf("Fail to get MISC_IOC_UUID.\n");
		return 1;
	}

	printf("\nUUID: %u\n", val);

	while (1) {
		printf("Disable DDRC Auto Gating.\n");
		ret = ioctl(fd, RTK_MISC_IOC_DDRC_DISGATE, &val);
		sleep(10);
		printf("Enable DDRC Auto Gating.\n");
		ret = ioctl(fd, RTK_MISC_IOC_DDRC_AUTO_GATE, &val);
		sleep(10);
	}

	close(fd);

	printf("Misc test PASS\n");

	return 0;
}
