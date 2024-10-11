#!/bin/bash
#
# Copyright (C) 2023 Realtek
#

function adc_buffer_test() {
    CH_0="0"
    CH_1="1"
    CH_2="2"
    CH_3="3"
    CH_4="4"
    CH_5="5"
    CH_6="6"
    RESULT_0=$(echo $ADC_CHS | grep "${CH_0}")
    RESULT_1=$(echo $ADC_CHS | grep "${CH_1}")
    RESULT_2=$(echo $ADC_CHS | grep "${CH_2}")
    RESULT_3=$(echo $ADC_CHS | grep "${CH_3}")
    RESULT_4=$(echo $ADC_CHS | grep "${CH_4}")
    RESULT_5=$(echo $ADC_CHS | grep "${CH_5}")
    RESULT_6=$(echo $ADC_CHS | grep "${CH_6}")

    echo 0 > "/sys/devices/iio_sysfs_trigger/add_trigger"
    echo sysfstrig0 > "/sys/bus/iio/devices/iio:device0/trigger/current_trigger"

    if [[ "$RESULT_0" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage0_en
    fi
    if [[ "$RESULT_1" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage1_en
    fi
    if [[ "$RESULT_2" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage2_en
    fi
    if [[ "$RESULT_3" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage3_en
    fi
    if [[ "$RESULT_4" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage4_en
    fi
    if [[ "$RESULT_5" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage5_en
    fi
    if [[ "$RESULT_6" != "" ]]; then
        echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage6_en
    fi
    echo $BUFF_LENGTH > /sys/devices/platform/ocp/42012000.adc/iio:device0/buffer/length
    echo 2 > /sys/devices/platform/ocp/42012000.adc/iio:device0/buffer/watermark
    echo 1 > /sys/bus/iio/devices/iio:device0/buffer/enable
    sleep 1s
    for i in $(seq 1 $TRIGGER_NUM)
    do
        echo "Trigger $i"
        echo 1 > /sys/bus/iio/devices/trigger0/trigger_now
        echo "Wait 1s..."
        sleep 1s
    done
    if [[ "$RESULT_0" != "" ]]; then
        echo "Data type of channel 0:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage0_type
    fi
    if [[ "$RESULT_1" != "" ]]; then
        echo "Data type of channel 1:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage1_type
    fi
    if [[ "$RESULT_2" != "" ]]; then
        echo "Data type of channel 2:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage2_type
    fi
    if [[ "$RESULT_3" != "" ]]; then
        echo "Data type of channel 3:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage3_type
    fi
    if [[ "$RESULT_4" != "" ]]; then
        echo "Data type of channel 4:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage4_type
    fi
    if [[ "$RESULT_5" != "" ]]; then
        echo "Data type of channel 5:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage5_type
    fi
    if [[ "$RESULT_6" != "" ]]; then
        echo "Data type of channel 6:"
        cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage6_type
    fi
    echo "Data available:"
    cat /sys/devices/platform/ocp/42012000.adc/iio:device0/buffer/data_available

    exit 0
}

function usage() {
cat << EOF
SYNOPSIS
    $PROGNAME [OPTION]

OPTIONS
    -h --help
        print this help and exit.

    -l, --buffer_len
        set the length of buffer in file system.

    -n, --trigger_num
        set the number of triggers.

    -c, --channels
        set channels to enable, e.g. 0, 016.

EOF
}

SHORTOPTS="hl:n:c:"
LONGOPTS="help,buffer_len:,trigger_num:,channels:"

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
        -l|--buffer_len)
            BUFF_LENGTH=$2
            shift 2
            ;;
        -n|--trigger_num)
            TRIGGER_NUM=$2
            shift 2
            ;;
        -c|--channels)
            ADC_CHS=$2
            shift 2
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

adc_buffer_test
