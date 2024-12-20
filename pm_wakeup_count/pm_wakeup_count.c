// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek PM test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

int main(int argc, char *argv[])
{
	int fd_wakeup_count;
	int fd_state;
	int n_read;
	int ret;
	char buf[64];

	fd_wakeup_count = open("/sys/power/wakeup_count", O_RDWR);
	if (fd_wakeup_count < 0) {
		printf("Fail to open /sys/power/wakeup_count node\n");
		return -1;
	}

	fd_state = open("/sys/power/state", O_RDWR);
	if (fd_state < 0) {
		printf("Fail to open /sys/power/state node\n");
		return -1;
	}

	do {
		n_read = read(fd_wakeup_count, buf, 64);
		printf("Wakeup_count is %s\n", buf);
		if (n_read > 0) {
			printf("Write it back now\n");
			ret = write(fd_wakeup_count, buf, strlen(buf));
		} else {
			continue;
		}
	} while (ret < 0);

	printf("Write success. Ready to enter sleep mode\n");
	ret = write(fd_state, "mem", strlen("mem"));

	close(fd_state);
	close(fd_wakeup_count);

	return 0;
}


