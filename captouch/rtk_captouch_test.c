// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Captouch test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	struct input_event event;
	int ret;

	fd = open("/dev/input/event0", O_RDWR);
	if (fd <= 0) {
		printf("Fail to open input file /dev/input/event0.\n");
	}

	while (1) {
		ret = read(fd, &event, sizeof(struct input_event));
		if (ret <= 0) {
			printf("Fail to read input event: %d\n", ret);
			continue;
		}
		if (event.type == EV_KEY) {
			if (event.value == 1) {
				printf("Keycode %d pressed\n", event.code);
			} else if (event.value == 0) {
				printf("Keycode %d released\n", event.code);
			}
		}
	}
	return 0;
}

