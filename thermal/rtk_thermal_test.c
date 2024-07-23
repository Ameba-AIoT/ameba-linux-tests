// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Thermal test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	char buffer[9];	// temp range(7bytes: -256000~256000) + "\0"(1byte) + "\n"(1byte)
	char type[20];
	int ret;
	float temp;

	fd = open("/sys/class/thermal/thermal_zone0/type", O_RDONLY);
	if (fd < 0) {
		printf("Open error\n");
		exit(1);
	}

	ret = read(fd, type, sizeof(type));
	if (ret < 0) {
		printf("Read error: %d\n", ret);
		exit(1);
	}
	type[ret] = '\0';
	printf("Thermal type is %s\n", type);

	close(fd);

	fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
	if (fd < 0) {
		printf("Open error\n");
		exit(1);
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0) {
		printf("Read error: %d\n", ret);
		exit(1);
	}
	temp = atoi(buffer)/1000.0;
	printf("Thermal temperature is %.1f degrees centigrade\n", temp);

	printf("Thermal test PASS\n");

	close(fd);

	return 0;
}

