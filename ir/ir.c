// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek IR test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <linux/input.h>
#include <linux/lirc.h>

#define IR_TX_TEST_SCANCODE	0xEA158A75U

int main(int argc, char **argv)
{
	int ret;
	int fd = 0;
	struct lirc_scancode scan;
	int to_set = LIRC_MODE_SCANCODE;

	/* IR rx is default open, triggered by rx signal interrupt. */

	/* The testcase is for ir tx only. */
	printf("IR TX test started\n");

	printf("Open IR device node\n");
	fd = open("/dev/lirc0", O_RDWR);
	if (fd < 0) {
		printf("Fail to open IR device node\n");
		return -1;
	}

	scan.rc_proto = RC_PROTO_NEC;
	scan.scancode = IR_TX_TEST_SCANCODE;
	scan.keycode = 0;
	scan.timestamp = 0;
	scan.flags = 0;

	printf("Set send mode to scancode\n");
	ret = ioctl(fd, LIRC_SET_SEND_MODE, &to_set);
	if (ret < 0) {
		printf("Fail to set send mode: %d\n", ret);
		return -1;
	}

	printf("TX scancode: 0x%llX\n", scan.scancode);
	ret = write(fd, &scan, sizeof(scan));
	if (ret < 0) {
		printf("Fail to TX scancode: %d\n", ret);
		return -1;
	}

	printf("Check the IR receiver for RX scancode\n");

	printf("Close IR device node\n");
	close(fd);

	printf("IR TX test done\n");

	return 0;
}
