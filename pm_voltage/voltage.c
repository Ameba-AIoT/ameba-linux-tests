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
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* Define new affairs in realtek-misc.h and implement relevant functions in Realtek misc driver. */
#include <realtek-misc.h>

#define READ_DATA_SIZE	1
#define COMMAND_BUFFER_SIZE 128
#define TARGET_FREQ 660000

void run_command_and_get_output(const char* command, char* result, size_t result_size)
{
	FILE *fp;
	char buffer[COMMAND_BUFFER_SIZE];

	fp = popen(command, "r");
	if (fp == NULL) {
		perror("popen failed");
		exit(EXIT_FAILURE);
	}

	if (fgets(buffer, sizeof(buffer), fp) != NULL) {
		snprintf(result, result_size, "%s", buffer);
	}

	pclose(fp);
}

int main(int argc, char **argv)
{
	int ret;
	int fd = 0;
	char buf[READ_DATA_SIZE];
	uint32_t val;
	char freq_str[COMMAND_BUFFER_SIZE] = {0};
	int current_freq = 0;
	uint32_t vol_before = 0;
	unsigned long to_adjust = 0;

	printf("Voltage test started.\n"
		"Please set WAKE_SRC_Timer3 to wake up AP in "
		"<sdk>\\sources\\firmware\\source\\component\\soc\\amebasmart\\usrcfg\\ameba_sleepcfg.c\n"
		"Please set the NPPLL in SocClk_Info to 920M, div(4) to support the 0.9V for km4 in"
		"<sdk>\\sources\\firmware\\source\\component\\soc\\amebasmart\\usrcfg\\ameba_bootcfg.c\n");

	/* Step 1: adjust AP frequency to 660000. */
	system("echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
	system("echo 660000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed");
	system("echo 660000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed");
	run_command_and_get_output("cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", freq_str, sizeof(freq_str));
	current_freq = atoi(freq_str);
	if (current_freq == TARGET_FREQ) {
		printf("[SUCCESS] Current CPU0 frequency is %d kHz\n", TARGET_FREQ);
	} else {
		printf("[FAILURE] Current CPU0 frequency is %d kHz, expected %d kHz\n", current_freq, TARGET_FREQ);
		return 1;
	}
	run_command_and_get_output("cat /sys/devices/system/cpu/cpu1/cpufreq/cpuinfo_cur_freq", freq_str, sizeof(freq_str));
	current_freq = atoi(freq_str);
	if (current_freq == TARGET_FREQ) {
		printf("[SUCCESS] Current CPU1 frequency is %d kHz\n", TARGET_FREQ);
	} else {
		printf("[FAILURE] Current CPU1 frequency is %d kHz, expected %d kHz\n", current_freq, TARGET_FREQ);
		return 1;
	}

	fd = open("/dev/misc-ioctl", O_RDWR);
	if (fd < 0) {
		printf("Fail to open misc device node.\n");
		return 1;
	}

try_again:
	ret = ioctl(fd, RTK_MISC_IOC_VOLTAGE_GET, &vol_before);
	if (ret < 0) {
		printf("Fail to get RTK_MISC_IOC_VOLTAGE_GET.\n");
		return 1;
	}
	if (vol_before == 0) {
		printf("[FAILURE] Cannot get current voltage!");
		return 1;
	}
	printf("Before adjust, voltage is %s.\n", (vol_before == VOL_09) ? "0.9V" : "1.0V");

	/* Step 2: set the voltage when wake up. */
	to_adjust = ((vol_before == VOL_10) ? VOL_09 : VOL_10);
	ret = ioctl(fd, RTK_MISC_IOC_VOLTAGE_SET, to_adjust);
	if (ret < 0) {
		printf("Fail to set voltage to %sV.\n", (to_adjust == VOL_10) ? "1.0" : "0.9");
		return 1;
	}

	/* Step 3: set wake up source. */
	system("echo 5000 > /sys/devices/platform/ocp/4200b600.timer/time_ms"); // sleep 1s for wake up voltage
	system("echo 1 > /sys/devices/platform/ocp/4200b600.timer/enable");

	/* Step 4: enter CG. */
	system("echo cg > /sys/power/state");
	sleep(1);

	/* Step 5: when wake up, read the actual voltage. */
	ret = ioctl(fd, RTK_MISC_IOC_VOLTAGE_GET, &val);
	if (ret < 0) {
		printf("Fail to get RTK_MISC_IOC_VOLTAGE_GET.\n");
		return 1;
	}

	/* Step 6: Check the current voltage. Decide to switch the frequency or not. */
	if (val != vol_before) {
		printf("[SUCCESS] Adjust system voltage to %s\n", (val == VOL_09) ? "0.9V" : "1.0V");
	} else {
		printf("[RETRY] CG and Adjust voltage failed, try again!\n");
		goto try_again;
	}

	close(fd);
	printf("Voltage test PASS\n");
	return 0;
}
