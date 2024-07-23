#!/bin/bash
#
# Copyright (C) 2023 Realtek
#

function gpio_sysfs_output_test() {
    echo "Info: gpio_sysfs_output_test start"

    for pin in 9 10 39 40
    do
        rtk_gpio_sysfs_test -n ${pin} -w 1
        sleep 1
        rtk_gpio_sysfs_test -n ${pin} -w 0
        sleep 1
    done

    echo "Info: gpio_sysfs_output_test success"
    echo
}

function gpio_sysfs_input_test() {
    echo "Info: gpio_sysfs_input_test start"

    for pin in 49 50 51 52
    do
        rtk_gpio_sysfs_test -n ${pin} -r
        sleep 1
    done

    echo "Info: gpio_sysfs_input_test success"
    echo
}

gpio_sysfs_output_test

gpio_sysfs_input_test

