// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek SDIOH test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>


int main(int argc, char **argv)
{
	int cnt, ret;
	int fd = -1;
	char *wbuf = "hello";
	char rbuf[10];
	char *path = "/mnt/storage/mmcblk0p1/sd_test.txt";

	printf("SDIOH test started\n");

	fd = open(path, O_RDWR|O_CREAT|O_SYNC|O_RSYNC);
	if (fd < 0){
		printf("Fail to open test file %s: %d\n", path, fd);
		return 1;
	} else {
		printf("Open test file %s\n", path);
	}

	ret = write(fd, wbuf, 6);
	if (ret < 0){
		printf("Fail to write: %d\n", ret);
		return 1;
	} else {
		printf("Write success\n");
		printf("Context: %s\n", wbuf);
	}

	ret = lseek(fd, 0, 0);
	if (ret < 0){
		printf("Fail to seek: %d\n", ret);
		return 1;
	} else {
		printf("Seek success\n");
	}

	ret = read(fd, rbuf, 6);
	if(ret < 0){
		printf("Fail to read: %d\n", ret);
		return 0;
	}else{
		printf("Read success\n");
		printf("Context: %s\n", rbuf);
	}

	close(fd);
	
	printf("SDIOH test PASS\n");

	return 0;
}



