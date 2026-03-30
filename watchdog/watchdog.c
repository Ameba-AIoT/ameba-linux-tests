// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Watchdog test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/timerfd.h>

#define WDG_FEED_PRE_TIMEOUT	3
#define WDG_FEED_COUNT			4

int main(int argc, char **argv)
{
	int i = 0;
	int ret = 0;
	int fd = -1;
	int timeout = 20;
	struct timespec gettime0;
	struct timespec gettime1;

	printf("Watchdog test started\n");

	fd = open("/dev/watchdog0", O_WRONLY);
	if (fd < 0) {
		printf("Fail to open watchdog device node: %d\n", fd);
		return 1;
	}

	ret = ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	if (ret < 0) {
		printf("Fail to get watchdog timeout value: %d\n", ret);
		return 1;
	}

	printf("Watchdog timeout: %ds\n", timeout);

	while(1) {
		clock_gettime(CLOCK_MONOTONIC, &gettime0);
		sleep(timeout - WDG_FEED_PRE_TIMEOUT);

		clock_gettime(CLOCK_MONOTONIC, &gettime1);
		printf("Feed watchdog. %.9fs passed.\n", gettime1.tv_sec - gettime0.tv_sec + (double)(gettime1.tv_nsec - gettime0.tv_nsec) / 1000000000.0);
		ret = ioctl(fd, WDIOC_KEEPALIVE, NULL);
		if (ret < 0) {
			printf("Fail to feed watchdog: %d\n", ret);
			return 1;
		}
		i++;
		if (i == WDG_FEED_COUNT) {
			printf("Stop feeding watchdog, expected to reboot in %ds\n", timeout);
			printf("To reboot, please ensure rtk,wdg-interrupt-mode = <0>\n");
			clock_gettime(CLOCK_MONOTONIC, &gettime0);
			while(1) {
				clock_gettime(CLOCK_MONOTONIC, &gettime1);
		        printf("%.9fs passed.\n", gettime1.tv_sec - gettime0.tv_sec + (double)(gettime1.tv_nsec - gettime0.tv_nsec) / 1000000000.0);
				sleep(1);
			}
		}
	}
}
