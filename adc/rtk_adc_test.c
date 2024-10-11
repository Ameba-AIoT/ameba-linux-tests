// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek ADC test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd = -1, ret;
    int adc_ch, value, raw_cnt;
    char buffer[15];
    char dev_name[50];
    float scale, millivolts;
    int is_diff_ch = 0;
    int vinp_ch, vinn_ch;


    if (argc == 2) {
        adc_ch = atoi(argv[1]);
        if ((adc_ch < 0) || (adc_ch > 6)) {
            printf("Error: invalid channel %d, please select a valid channel specified in DTS\n", adc_ch);
            exit(1);
        }
        printf("Single ended mode test for channel = %d\n", adc_ch);
    } else if ((argc == 4) && (!strcmp(argv[1], "diff"))) {
        is_diff_ch = 1;
        vinp_ch = atoi(argv[2]);
        vinn_ch = atoi(argv[3]);

        if (!( ((vinp_ch == 0) && (vinn_ch == 1)) ||
             ((vinp_ch == 1) && (vinn_ch == 0)) ||
             ((vinp_ch == 2) && (vinn_ch == 3)) ||
             ((vinp_ch == 3) && (vinn_ch == 2)) ||
             ((vinp_ch == 4) && (vinn_ch == 5)) ||
             ((vinp_ch == 5) && (vinn_ch == 4)) )) {
            printf("Invalid differential channel pair <%d %d>. Please note that only differential channel pairs <vinp vinn> that are supported are below:\n <0 1> <1 0> <2 3> <3 2> <4 5> <5 4>\n", vinp_ch, vinn_ch);
            exit(1);
        }
        printf("Differential mode test for channel pair <vinp vinn> = <%d %d>\n", vinp_ch, vinn_ch);
    } else {
        printf("Usage: For single ended mode: 'rtk_adc_test <0~6>'; For differential mode: 'rtk_adc_test diff <0~5> <0~5>\n");
        exit(1);
    }

    if (is_diff_ch)
        sprintf(dev_name, "/sys/bus/iio/devices/iio:device0/in_voltage%d-voltage%d_raw", vinp_ch, vinn_ch);
    else
        sprintf(dev_name, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", adc_ch);


    if ((fd = open(dev_name, O_RDONLY)) < 0) {
        printf("Error: failed to open requested ADC channel, please check whether it is enabled in DTS\n");
        exit(1);
    }

    ret = read(fd, buffer, 5);
    if (ret < 0) {
        printf("Error: fail to read ADC channel\n");
        exit(1);
    }
    value = atoi(buffer);
    raw_cnt = value;
    close(fd);

    millivolts = (float)raw_cnt;
    if (is_diff_ch)
        printf("CH[%d-%d] Raw ADC count: %d\n", vinp_ch, vinn_ch, raw_cnt);
    else
        printf("CH[%d] Raw ADC count: %d\n", adc_ch, raw_cnt);


    if (is_diff_ch)
        sprintf(dev_name, "/sys/bus/iio/devices/iio:device0/in_voltage%d-voltage%d_offset", vinp_ch, vinn_ch);
    else
        sprintf(dev_name, "/sys/bus/iio/devices/iio:device0/in_voltage%d_offset", adc_ch);

    if ((fd = open(dev_name, O_RDONLY)) < 0) {
        printf("Error: fail to open ADC channel offset attribute\n");
        exit(1);
    }
    ret = read(fd, buffer, 5);
    if (ret < 0) {
        printf("Error: fail to read ADC channel offset attribute\n");
        exit(1);
    }

    value = atoi(buffer);
    millivolts += (float)value;
    if (is_diff_ch)
        printf("CH[%d-%d] Calibration offset: %d\n", vinp_ch, vinn_ch, value);
    else
        printf("CH[%d] Calibration offset: %d\n", adc_ch, value);
    close(fd);

    if (is_diff_ch)
        sprintf(dev_name, "/sys/bus/iio/devices/iio:device0/in_voltage%d-voltage%d_scale", vinp_ch, vinn_ch);
    else
        sprintf(dev_name, "/sys/bus/iio/devices/iio:device0/in_voltage%d_scale", adc_ch);

    if ((fd = open(dev_name, O_RDONLY)) < 0) {
        printf("Error: fail to open ADC channel scale attribute, please check whether it is enabled in DTS\n");
        exit(1);
    }
    ret = read(fd, buffer, 11);
    if (ret < 0) {
        printf("Error: fail to read ADC channel scale attribute\n");
        exit(1);
    }

    scale = strtod(buffer, NULL);
    millivolts *= scale;
    if (is_diff_ch)
        printf("CH[%d-%d] Scale: %.9f\n", vinp_ch, vinn_ch, scale);
    else
        printf("CH[%d] Scale: %.9f\n", adc_ch, scale);
    close(fd);

    /* Final channel voltage in millivolts = (raw_adc_count + offset) * scale */
    if (is_diff_ch)
        printf("CH[%d-%d] Calibrated voltage: %.4f millivolts\n", vinp_ch, vinn_ch, millivolts);
    else
        printf("CH[%d] Calibrated voltage: %.4f millivolts\n", adc_ch, millivolts);

    return 0;
}
