// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek RTC test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

static int rtc_read(int fd, struct rtc_time *p_tm)
{
    int ret = ioctl(fd, RTC_RD_TIME, p_tm);

    if (ret < 0) {
        printf("RTC_RD_TIME error: %d\n", ret);
        return -1;
    }

    printf("Current RTC date/time is %04d-%02d-%02d %02d:%02d:%02d\n",
           p_tm->tm_year + 1900, p_tm->tm_mon + 1, p_tm->tm_mday,
           p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
    return 0;
}

static int rtc_write(int fd, struct rtc_time *p_tm)
{
    p_tm->tm_year = 2024 - 1900;
    p_tm->tm_mon = 2;
    p_tm->tm_mday = 23;
    p_tm->tm_hour = 8;
    p_tm->tm_min = 8;
    p_tm->tm_sec = 8;

    int ret = ioctl(fd, RTC_SET_TIME, p_tm);

    if (ret < 0) {
        printf("RTC_SET_TIME error: %d\n", ret);
        return -1;
    }

    return 0;
}

static int rtc_set_alarm(int fd, struct rtc_time *p_tm)
{
    unsigned long data = 0;
    int ret = 0;
    ret = ioctl(fd, RTC_RD_TIME, p_tm);

    if (ret < 0) {
        printf("RTC_RD_TIME error: %d\n", ret);
        return -1;
    }

    p_tm->tm_sec += 10;

    if (p_tm->tm_sec >= 60) {
        p_tm->tm_sec %= 60;
        p_tm->tm_min++;
    }

    if (p_tm->tm_min == 60) {
        p_tm->tm_min = 0;
        p_tm->tm_hour++;
    }

    if (p_tm->tm_hour == 24) {
        p_tm->tm_hour = 0;
    }

    ret = ioctl(fd, RTC_ALM_SET, p_tm);

    if (ret < 0) {
        printf("RTC_ALM_SET error: %d\n", ret);
        return -1;
    }

    ret = ioctl(fd, RTC_ALM_READ, p_tm);

    if (ret < 0) {
        printf("RTC_ALM_READ error: %d\n", ret);
        return -1;
    }

    printf("Set alarm time to %d:%d:%d\n",
           p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);

    ret = ioctl(fd, RTC_AIE_ON, 0);

    if (ret == -1) {
        printf("RTC_AIE_ON error: %d\n", ret);
        return -1;
    }

    printf("Wait for 10 seconds for alarm...\n");

    /* This blocks until the alarm ring causes an interrupt */
    ret = read(fd, &data, sizeof(unsigned long));

    if (ret < 0) {
        printf("RTC read error: %d\n", ret);
        return -1;
    }

    printf("RTC alarm triggered\n");
    printf("RTC test PASS\n");
    return 0;
}

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s [options]\n", program);
    fprintf(stderr, "\n");
    fprintf(stderr, "  -r read rtc time test\n");
    fprintf(stderr, "  -w write rtc time test\n");
    fprintf(stderr, "  -s set alarm rtc time test\n");
}

int main(int argc, char **argv)
{
    const char *optstring = "rws";
    int fd;
    int ret;
    int c;
    unsigned long data;
    struct rtc_time tm;

    fd = open ("/dev/rtc0", O_RDONLY);

    if (fd < 0) {
        printf("Fail to open /dev/rtc0: %d\n", fd);
        return fd;
    }

    while ((c = getopt(argc, argv, optstring)) != -1) {
        switch (c) {
        case 'r':
            rtc_read(fd, &tm);
            break;

        case 'w':
            rtc_write(fd, &tm);
            break;

        case 's':
            rtc_set_alarm(fd, &tm);
            break;

        default:
            usage(argv[0]);
            return 0;
        }
    }

    close(fd);
    return ret;
}
