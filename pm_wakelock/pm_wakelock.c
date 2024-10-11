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
	int fd_wake_lock;
	int fd_wake_unlock;
	int fd_autosleep;
	int ret;

	fd_wake_lock = open("/sys/power/wake_lock", O_RDWR);
	if (fd_wake_lock < 0) {
		printf("Fail to open /sys/power/wake_lock node\n");
		return -1;
	}

	fd_wake_unlock = open("/sys/power/wake_unlock", O_RDWR);
	if (fd_wake_unlock < 0) {
		printf("Fail to open /sys/power/wake_unlock node\n");
		return -1;
	}

	fd_autosleep = open("/sys/power/autosleep", O_RDWR);
	if (fd_autosleep < 0) {
		printf("Fail to open /sys/power/autosleep node\n");
		return -1;
	}

	printf("Create first wake_lock: wakelock1\n");
	ret = write(fd_wake_lock, "wakelock1", strlen("wakelock1"));
	if (ret < 0) {
		printf("Fail to create wakelock: %d\n", ret);
		return -1;
	}

	sleep(1);
	printf("Create second wake_lock: wakelock2\n");
	ret = write(fd_wake_lock, "wakelock2", strlen("wakelock2"));
	if (ret < 0) {
		printf("Fail to create wakelock: %d\n", ret);
		return -1;
	}

	sleep(1);
	printf("Enable autosleep, sleep level: standby\n");
	ret = write(fd_autosleep, "mem", strlen("mem"));
	if (ret < 0) {
		printf("Fail to enable autosleep: %d\n", ret);
		return -1;
	}

	printf("Should not sleep now because of wakelock1 and wakelock2\n");

	sleep(1);
	printf("Release wakelock1 here. Should not sleep because wakelock2 is not released\n");
	ret = write(fd_wake_unlock, "wakelock1", strlen("wakelock1"));
	if (ret < 0) {
		printf("Fail to release wakelock1: %d\n", ret);
		return -1;
	}

	sleep(1);
	printf("Release wakelock2 here. Should sleep because all wakelocks are released\n");
	ret = write(fd_wake_unlock, "wakelock2", strlen("wakelock2"));
	if (ret < 0) {
		printf("Fail to release wakelock2: %d\n", ret);
		return -1;
	}

	close(fd_wake_unlock);
	close(fd_autosleep);
	close(fd_wake_lock);

	return 0;
}


