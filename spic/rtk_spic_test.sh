#!/bin/bash
#
# Copyright (C) 2023 Realtek
#

function spic_nand_read_test() {
    echo "Info: spic_nand_read_test start"

    local cnt=0
    local num=0
    while [ $num -le 5 ]
    do
        time dd if=/dev/mtdblock8 of=/dev/null bs=${BLOCK_SIZE} count=${BLOCK_COUNT} skip=${cnt}
        cnt=$(( $cnt + $BLOCK_COUNT ))
        num=$(( $num + 1 ))
        echo
    done

    echo "Info: spic_nand_read_test success"
    echo
}

function spic_nand_write_test() {
    echo "Info: spic_nand_write_test start"

    dd if=/dev/mtdblock8 of=/dev/dd.img bs=${BLOCK_SIZE} count=${BLOCK_COUNT}
    dd if=/dev/dd.img of=/rom/mnt/dd.img bs=${BLOCK_SIZE}

    ls -al /rom/mnt/dd.img
    rm -fr /dev/dd.img
    rm -fr /rom/mnt/dd.img

    echo "Info: spic_nand_write_test success"
    echo
}

function spic_nand_mount_test() {
    echo "Info: spic_nand_mount_test start"

    umount /rom/mnt
    echo "Info: spic_nand_mount_test umount userdata success"

    mount -t ubifs ubi1_0 /rom/mnt
    mount | grep ubi1_0
    echo "Info: spic_nand_mount_test mount userdata to /rom/mnt success"
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

EOF
}

SHORTOPTS="hb:c:"
LONGOPTS="help,blocksize:,blockcount:"

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

spic_nand_mount_test

spic_nand_read_test

spic_nand_write_test


