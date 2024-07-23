#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <sys/timerfd.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: rtk_system_timer_test interval_time_value repeat_count_value\n");
        printf("for example: rtk_system_timer_test 2 10\n");
        return -1;
    }
    struct timespec now;
    now.tv_sec = now.tv_nsec = 0;
    clock_gettime(CLOCK_REALTIME, &now);

    int tmfd;
    int ret;
    struct itimerspec new_value;

    new_value.it_value.tv_sec = now.tv_sec;
    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_interval.tv_sec = atoi(argv[1]);
    new_value.it_interval.tv_nsec = 0;

    tmfd = timerfd_create(CLOCK_REALTIME, 0);
    if (tmfd < 0) {
        printf("timerfd_create error, Error:[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    ret = timerfd_settime(tmfd, TFD_TIMER_ABSTIME, &new_value, NULL);
    if (ret < 0) {
        printf("timerfd_settime error, Error:[%d:%s]\n", errno, strerror(errno));
        close(tmfd);
        return -1;
    }

    uint64_t exp = 0;
    uint64_t tot_exp = 0;
    uint64_t max_exp = atoi(argv[2]);

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    struct timespec curr;
    int secs, nsecs;

    for (; tot_exp < max_exp;) {
        read(tmfd, &exp, sizeof(uint64_t));
        tot_exp += exp;
        clock_gettime(CLOCK_REALTIME, &curr);
        secs = curr.tv_sec - start.tv_sec;
        nsecs = curr.tv_nsec - start.tv_nsec;
        printf("%d.%03d:read: %llu, total: %llu\n", secs, nsecs / 1000000, (unsigned long long)exp, (unsigned long long)tot_exp);
    }

    close(tmfd);

    return 0;
}