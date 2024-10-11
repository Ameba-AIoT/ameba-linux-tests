#!/bin/bash

echo -e "\n----------------------------------------"

echo -e "Write Time by cmd: date -s "2023-11-18 02:02:10"."
date -s "2023-11-18 02:02:10"
hwclock -w

echo -e "Read RTC Time:"
rtk_rtc_test -r

echo -e "Write RTC Time 2024-02-23 08:08:08 ."
rtk_rtc_test -w
hwclock -s

echo -e "Read Time by date:"
date +%F\ %H:%M:%S

echo -e "Set Alarm time."
rtk_rtc_test -s

