#!/bin/bash
#
# Copyright (C) 2023 Realtek
#

function clear_leds() {
    CLEAR_DESC="0 0"
    for i in $(seq 1 $LED_CLEAR)
    do
        OUTPUT_SIG="${OUTPUT_SIG} ${CLEAR_DESC}";
    done
    echo "$OUTPUT_SIG" > /sys/devices/platform/ocp/41008000.led-controller/leds/rtk_ws28xxx:common/hw_pattern
    OUTPUT_SIG=""
}

function color_show() {
    if [ "$LED_COLOR" = "red" ]; then
        echo "-- LEDC TEST -- red data length: $LED_COUNT."
        COLOR_DESC="32768 0"
    fi

    if [ "$LED_COLOR" = "green" ]; then
        echo "-- LEDC TEST -- green data length: $LED_COUNT."
        COLOR_DESC="16711680 0"
    fi

    if [ "$LED_COLOR" = "blue" ]; then
        echo "-- LEDC TEST -- blue data length: $LED_COUNT."
        COLOR_DESC="60 0"
    fi

    if [ "$LED_COLOR" = "yellow" ]; then
        echo "-- LEDC TEST -- yellow data length: $LED_COUNT."
        COLOR_DESC="16776960 0"
    fi

    for i in $(seq 1 $LED_COUNT)
    do
        OUTPUT_SIG="${OUTPUT_SIG} ${COLOR_DESC}";
    done
    echo "$OUTPUT_SIG" > /sys/devices/platform/ocp/41008000.led-controller/leds/rtk_ws28xxx:common/hw_pattern
    OUTPUT_SIG=""
}

function pattern_show() {
    PATEERN_PATH="/sys/devices/platform/ocp/41008000.led-controller/leds/rtk_ws28xxx:common/hw_pattern"
    echo 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32768 0  > $PATEERN_PATH
    sleep 0.5s
    echo 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0 0 0 > $PATEERN_PATH
    sleep 0.5s
    echo 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16776960 0  > $PATEERN_PATH
    sleep 0.5s
    echo 32768 0 16776960 0 32768 0 16776960 0 32768 0 16776960 0 32768 0 16776960 0 32768 0 16776960 0 32768 0 16776960 0 32768 0 16776960 0 32768 0 16776960 0  > $PATEERN_PATH
}

function usage() {
cat << EOF
SYNOPSIS
    $PROGNAME [OPTION]

OPTIONS
    -h --help
        print this help and exit.

    -n, --lednums
        set the numbers of led, not more than 16.

    -c, --color
        set the color of leds, supporting for red, blue, green, yellow in testcase.

    -p, --pattern
        pattern mode for leds.

    -e, --empty
        clear up leds.

EOF
}

SHORTOPTS="hn:c:pe:"
LONGOPTS="help,lednums:,color:,pattern,empty:"

ARGS=`getopt -o $SHORTOPTS --long $LONGOPTS -n "$0" -- "$@"`
if [ $? != 0 ]; then
    usage
    exit 1
fi

eval set -- "${ARGS}"

while true
do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -n|--lednums)
            LED_COUNT=$2
            shift 2
            ;;
        -c|--color)
            LED_COLOR=$2
            shift 2
            ;;
        -p|--pattern)
            pattern_show
            shift 2
            exit 0
            ;;
        -e|--empty)
            LED_CLEAR=$2
            clear_leds
            shift 2
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Error: parse error!"
            exit 1
            ;;
    esac
done

if [ -z "$LED_COLOR" ]; then
    echo "Error: the color of leds is not sepecified."
    usage
    exit 1
else
if [ -z "$LED_COUNT" ]; then
    LED_COUNT=1
    echo "Warning: number of leds is not specified. Use defult value 1 led."
fi
    color_show
fi
