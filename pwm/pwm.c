// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek PWM test
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

#define DUTY              "duty"
#define PERIOD            "20000000"
#define DUTYCYCLE         "8000000"

int main(int argc, char *argv[])
{
	int fd;
	int ch;
	int period_in_ns;
	int duty_cycle_in_ns;
	int ret;
	int fd_period;
	int fd_duty;
	int fd_enable;
	char buf[64];

	if (argc < 4) {
		printf("Invalid arguments\n");
		printf("Usage: rtk_pwm_test <channel(0~5)> <period_in_ns> <duty_cycle_in_ns>\n");
		exit(1);
	} else {
		ch = atoi(argv[1]);
		period_in_ns = atoi(argv[2]);
		duty_cycle_in_ns = atoi(argv[3]);
	}

	printf("Open PWM sysfs node\n");
	fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
	if (fd < 0) {
		printf("Failed to open pwm device node\n");
		return -1;
	}

	printf("Create PWM%d sysfs nodes\n", ch);
	sprintf(buf, "%d", ch);
	ret = write(fd, buf, strlen(buf));
	if (ret < 0) {
		printf("Failed to creat PWM%d node\n", ch);
		return -1;
	}

	sprintf(buf, "/sys/class/pwm/pwmchip0/pwm%d/period", ch);
	fd_period = open(buf, O_RDWR);
	if (fd_period < 0) {
		printf("Failed to open PWM%d period node\n", ch);
		return -1;
	}

	sprintf(buf, "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", ch);
	fd_duty = open(buf, O_RDWR);
	if (fd_duty < 0) {
		printf("Failed to open PWM%d duty_cycle node\n", ch);
		return -1;
	}

	sprintf(buf, "/sys/class/pwm/pwmchip0/pwm%d/enable", ch);
	fd_enable = open(buf, O_RDWR);
	if (fd_enable < 0) {
		printf("Failed to open PWM%d enable node\n", ch);
		return -1;
	}

	printf("Set period to %dns\n", period_in_ns);
	sprintf(buf, "%d", period_in_ns);
	ret = write(fd_period, buf, strlen(buf));
	if (ret < 0) {
		printf("Failed to set period: %d\n", ret);
		return -1;
	}

	printf("Set duty cycle to %dns/%dns\n", duty_cycle_in_ns, period_in_ns);
	sprintf(buf, "%d", duty_cycle_in_ns);
	ret = write(fd_duty, buf, strlen(buf));
	if (ret < 0) {
		printf("Failed to set duty_cycle: %d\n", ret);
		return -1;
	}

	printf("Enable PWM%d\n", ch);
	ret = write(fd_enable, "1", strlen("1"));
	if (ret < 0) {
		printf("Failed to enable PWM: %d\n", ret);
		return -1;
	}

	printf("Please check the PWM%d output via LA\n", ch);

	sleep(10);

	printf("Stop PWM%d output\n", ch);
	printf("Disable PWM%d\n", ch);
	ret = write(fd_enable, "0", strlen("0"));
	if (ret < 0) {
		printf("Failed to disable PWM: %d\n", ret);
		return -1;
	}

	fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
	if (fd < 0) {
		printf("Failed to open pwm device node\n");
		return -1;
	}

	printf("Delete PWM%d sysfs nodes\n", ch);
	sprintf(buf, "%d", ch);
	ret = write(fd, buf, strlen(buf));
	if (ret < 0) {
		printf("Failed to delete PWM%d node\n", ch);
		return -1;
	}

	close(fd_enable);
	close(fd_duty);
	close(fd_period);
	close(fd);

	printf("PWM test done\n");

	return 0;
}


