#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#include "sound/rtk_audio_pll.h"

#define AUDIO_PLL_DEV "/dev/audio_pll_clk"

int main(int argc, char **argv) {
    unsigned int clock = PLL_98P304M;
    unsigned int action = PLL_FASTER;
    unsigned int ppm = 800;
    struct micro_adjust_params params;
    struct audio_clock_info info;
    int fd;

    if (argc < 2) {
        fprintf(stderr, "[-c clock (0:24.576M, 1:45.158M, 2:98.304M)]"
                " [-a action (0:auto, 1:faster, 2:slower)]"
                " [-p ppm (0-1000)] \n");
        return 1;
    }

    while (*argv) {
        if (strcmp(*argv, "-c") == 0) {
            argv++;
            if (*argv)
                clock = atoi(*argv);
        }
        if (strcmp(*argv, "-a") == 0) {
            argv++;
            if (*argv)
                action = atoi(*argv);
        }
        if (strcmp(*argv, "-p") == 0) {
            argv++;
            if (*argv)
                ppm = atoi(*argv);
        }
        if (*argv)
            argv++;
    }

    params.clock = clock;
    params.action = action;
    params.ppm = ppm;

    info.sport_index = 2;

    fd = open(AUDIO_PLL_DEV, O_RDWR);
    if(fd == -1) {
        printf("open %s fail \n", AUDIO_PLL_DEV);
    }

    ioctl(fd, AUDIO_PLL_GET_CLOCK_FOR_SPORT, &info);
    printf("[pll test] clock for sport2:%d (0:24M, 1:45M, 2:98M, 3:xtal-40M)\n", info.clock);

    info.sport_index = 3;
    ioctl(fd, AUDIO_PLL_GET_CLOCK_FOR_SPORT, &info);
    printf("[pll test] clock for sport3:%d (0:24M, 1:45M, 2:98M, 3:xtal-40M)\n", info.clock);

    printf("[pll test] adjust cmd, clock:%d, action:%d, ppm:%d\n", params.clock, params.action, params.ppm);
    ioctl(fd, AUDIO_PLL_MICRO_ADJUST, &params);

    close(fd);
    printf("[pll test] done\n");
    return 0;
}
