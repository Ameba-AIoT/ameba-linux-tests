#!/bin/bash
#
# Copyright (C) 2023 Realtek
#

function sdioh_read_test() {
    echo "Info: sdioh_read_test start"

    local cnt=0
    local num=0
    while [ $num -le 5 ]
    do
        time dd if=${BLOCK_PARTITION} of=/dev/null bs=${BLOCK_SIZE} count=${BLOCK_COUNT} skip=${cnt}
        cnt=$(( $cnt + $BLOCK_COUNT ))
        num=$(( $num + 1 ))
        echo
    done

    echo "Info: sdioh_read_test success"
    echo
}

function sdioh_write_test() {
    echo "Info: sdioh_write_test start"

    dd if=${BLOCK_PARTITION} of=/mnt/sd_test.img bs=${BLOCK_SIZE} count=${BLOCK_COUNT}
    dd if=/mnt/sd_test.img of=/mnt/storage/mmcblk0p1/sd_test.img bs=${BLOCK_SIZE}

    ls -al /mnt/storage/mmcblk0p1/sd_test.img
    rm -fr /mnt/sd_test.img

    echo "Info: sdioh_write_test success"
    echo
}

function sdioh_mount_test() {
    echo "Info: sdioh_mount_test start"

    umount  /mnt/storage/mmcblk0p1
    echo "Info: sdioh_mount_test umount sdcard success"

    mount -t vfat ${BLOCK_PARTITION} /mnt/storage/mmcblk0p1
    mount | grep sdcard
    echo "Info: sdioh_mount_test mount sdcard to /mnt/storage/mmcblk0p1 success"
    echo
}

function usage() {
cat <<EOF
SYNOPSIS
    $PROGNAME [OPTION]

OPTIONS
    -h --help
        print this help and exit.

    -b, --blocksize
        set the block size of dd.

    -c, --blockcount
        set the block count.

    -p, --blockpartition
        set the sdcard block partition, use mount|grep sdcard to get.

EOF
}

SHORTOPTS="hb:c:p:"
LONGOPTS="help,blocksize:,blockcount:,blockpartition:"

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
        -b|--blocksize)
            BLOCK_SIZE=$2
            shift 2
            ;;
        -c|--blockcount)
            BLOCK_COUNT=$2
            shift 2
            ;;
        -p|--blockpartition)
            BLOCK_PARTITION=$2
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

if [ -z "$BLOCK_SIZE" ]; then
    echo "Error: block size is not spefied."
    usage
    exit 1
fi

if [ -z "$BLOCK_COUNT" ]; then
    echo "Error: block count is not spefied."
    usage
    exit 1
fi

if [ -z "$BLOCK_PARTITION" ]; then
    echo "Error: sdcard block partition is not spefied, please use mount | grep mmcblk to find sdcard partition"
    usage
    exit 1
fi

sdioh_mount_test

sdioh_read_test

sdioh_write_test


