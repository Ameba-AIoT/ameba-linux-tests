#!/bin/bash
#
# Copyright (C) 2023 Realtek
#
function pm_timer_wakeup_test() {
    echo "Info: pm_timer_wakeup_test start"

    echo 5000 > /sys/devices/platform/ocp/4200b600.timer/time_ms
    echo 1 > /sys/devices/platform/ocp/4200b600.timer/enable
    rtk_pm_wakeup_count_test

    echo "Info: pm_timer_wakeup_test success"
    echo
}

function pm_captouch_wakeup_test() {
    echo "Info: pm_timer_wakeup_test start"

    rtk_pm_wakeup_count_test

    echo "Info: pm_timer_wakeup_test success"
    echo
}

function pm_wifi_wakeup_test() {
    echo "Info: pm_timer_wakeup_test start"

    wpa_supplicant -D nl80211 -i wlan0 -c /etc/wifi/wpa_supplicant.conf -B &
    dhcpcd wlan0 &
    sleep 30
    rtk_pm_wakeup_count_test

    echo "Info: pm_timer_wakeup_test success"
    echo
}

#!/bin/bash

echo -e "\n--------------- TWO BOARDS TEST AVAILABLE TEST CASES --------------------------"
echo -e "1. Timer Wakeup Test."
echo -e "2. CapTouch Wakeup Test."
echo -e "3. Wifi Wakeup Test."
echo -e "----------------------------------------------------------------------------------"

read TESTCASE

case $TESTCASE in
    "1")
        echo "Timer Wakeup Test."
        pm_timer_wakeup_test
        sleep 1
        ;;
    "2")
        echo "CapTouch Wakeup Test."
        pm_captouch_wakeup_test
        sleep 1
        ;;
    "3")
        echo "Wifi Wakeup Test."
        pm_wifi_wakeup_test
        sleep 1
        ;;
    *)
        echo "Invalid Option"
        ;;
esac

