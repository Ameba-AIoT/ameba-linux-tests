export PATH

KERNEL_VERSION=`cat /proc/version | awk '{print $3}'`
built_in="false"
role=""

function insmod_ko()
{
    if [ ! -e "$1" ]; then
        echo "No such file: $1"
        exit 1
    fi
    ko=`basename $1 .ko`
    lsmod | grep ${ko//-/_} > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        insmod $1
    fi
}

function insmod_otg_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/storage/usb-storage.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_accessory.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_hid.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbh_msc_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/storage/usb-storage.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbd_adb_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_accessory.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_hid.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbd_acm_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/u_serial.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_acm.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbd_hid_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_hid.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbd_msc_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_mass_storage.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbd_uac2_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/u_audio.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_uac2.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbh_uvc_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/media/usb/uvc/uvcvideo.ko
}

function insmod_usbh_hid_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/hid/usbhid/usbhid.ko
}

function insmod_usbh_uac_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/sound/core/snd-pcm.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/sound/core/snd-hwdep.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/sound/core/snd-rawmidi.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/sound/usb/snd-usbmidi-lib.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/sound/usb/snd-usb-audio.ko
}

function insmod_usbh_vendor_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/misc/usbtest.ko
}

function insmod_usbd_vendor_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}
function insmod_usbh_acm_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/roles/roles.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/class/cdc-acm.ko
}

function mount_configfs()
{
    if [ -e "/mnt/config/usb_gadget" ]; then
        umount -f /mnt/config > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "WARNING: Fail to re-mount USB configfs, following operations may result in failure"
            echo "If USB does not work, try:"
        echo "# umount -f /mnt/config"
        echo "# mount none /mnt/config/ -t configfs"
        echo "Or reboot and try again"
            return 0
        fi
    else
        mkdir /mnt/config/ > /dev/null 2>&1
    fi

    mount none /mnt/config/ -t configfs

    if [ ! -e "/mnt/config/usb_gadget" ]; then
        echo "Fail to mount USB configfs"
        return 1
    fi

    return 0
}

function usb_force_mode()
{
  [ "$1" = 1 ] || [ "$1" = 2 ] || { echo "usage: usb_force_mode 1|2" >&2; return 2; }
  local f=/sys/module/phy_rtk_usb/parameters/usb_force_mode
  [ -w "$f" ] || { echo "not writable: $f" >&2; return 1; }
  echo "$1" > "$f"
}

function usbd_acc_init()
{
    local arg=${1:-0}
    echo "ACC init"

    mount_configfs

    cd /mnt/config/usb_gadget
    mkdir acc > /dev/null 2>&1
    cd acc
    echo 0x0200 > bcdUSB

    echo 64 > bMaxPacketSize0

    #adb will filter devices according to bDeviceClass, bDeviceSubClass and bDeviceProtocol
    echo 0xff > bDeviceClass
    echo 0x42 > bDeviceSubClass
    echo 0x01 > bDeviceProtocol

    echo 0x0BDA > idVendor
    echo 0x8730 > idProduct

    mkdir strings/0x409 > /dev/null 2>&1
    echo "Realtek" > strings/0x409/manufacturer
    echo "ADB Interface" > strings/0x409/product
    cat /proc/realtek/uuid > strings/0x409/serialnumber

    mkdir configs/c.1 > /dev/null 2>&1
    echo 120 > configs/c.1/MaxPower
    mkdir configs/c.1/strings/0x409 > /dev/null 2>&1
    echo "accessary" > configs/c.1/strings/0x409/configuration

    mkdir functions/accessory.adb > /dev/null 2>&1
    ln -sf functions/accessory.adb configs/c.1/
}

function usbd_acm_init()
{
    echo "CDC ACM device init"

    mount_configfs

    cd /mnt/config/usb_gadget
    mkdir cdc > /dev/null 2>&1
    cd cdc

    echo 0x0200 > bcdUSB
    echo 0x02 > bDeviceClass
    echo 0x02 > bDeviceSubClass
    echo 64 > bMaxPacketSize0
    echo 0x0BDA > idVendor
    echo 0x8730 > idProduct

    mkdir strings/0x409 > /dev/null 2>&1
    echo "Realtek" > strings/0x409/manufacturer
    echo "CDC ACM device" > strings/0x409/product
    cat /proc/realtek/uuid > strings/0x409/serialnumber

    mkdir configs/c.1 > /dev/null 2>&1
    echo 120 > configs/c.1/MaxPower
    mkdir configs/c.1/strings/0x409 > /dev/null 2>&1
    echo "acm" > configs/c.1/strings/0x409/configuration

    mkdir functions/acm.ttyS1 > /dev/null 2>&1
    ln -sf functions/acm.ttyS1 configs/c.1/
}

function usbd_hid_init()
{
    echo "HID device init"

    mount_configfs

    cd /mnt/config/usb_gadget
    mkdir hid > /dev/null 2>&1
    cd hid

    echo 0x0200 > bcdUSB
    echo 0x03 > bDeviceClass
    echo 0x00 > bDeviceSubClass
    echo 0x01 > bDeviceProtocol
    echo 64 > bMaxPacketSize0
    echo 0x0BDA > idVendor
    echo 0x8730 > idProduct

    mkdir strings/0x409 > /dev/null 2>&1
    echo "Realtek" > strings/0x409/manufacturer
    echo "HID" > strings/0x409/product
    cat /proc/realtek/uuid > strings/0x409/serialnumber

    mkdir configs/c.1 > /dev/null 2>&1
    echo 120 > configs/c.1/MaxPower
    mkdir configs/c.1/strings/0x409 > /dev/null 2>&1
    echo "hid" > configs/c.1/strings/0x409/configuration

    mkdir functions/hid.usb0 > /dev/null 2>&1
    echo 1 > functions/hid.usb0/subclass
    echo 1 > functions/hid.usb0/protocol
    echo 8 > functions/hid.usb0/report_length
    echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 > functions/hid.usb0/report_desc
    ln -sf functions/hid.usb0 configs/c.1/
}

function usbd_msc_init()
{
    echo "MSC device init"

    mount_configfs

    cd /mnt/config/usb_gadget
    mkdir msc > /dev/null 2>&1
    cd msc

    echo 0x0200 > bcdUSB
    echo 0x0BDA > idVendor
    echo 0x8730 > idProduct

    mkdir strings/0x409 > /dev/null 2>&1
    echo "Realtek" > strings/0x409/manufacturer
    echo "MSC device" > strings/0x409/product
    cat /proc/realtek/uuid > strings/0x409/serialnumber

    mkdir configs/c.1 > /dev/null 2>&1
    echo 120 > configs/c.1/MaxPower
    mkdir configs/c.1/strings/0x409 > /dev/null 2>&1
    echo "msc" > configs/c.1/strings/0x409/configuration

    mkdir functions/mass_storage.0 > /dev/null 2>&1
    echo /dev/mmcblk0 >functions/mass_storage.0/lun.0/file
    echo 1 > functions/mass_storage.0/lun.0/removable
    echo 0 > functions/mass_storage.0/lun.0/nofua
    ln -sf functions/mass_storage.0 configs/c.1/
}

function usbd_uac2_init()
{
    echo "UAC2 device init"

    mount_configfs

    cd /mnt/config/usb_gadget
    mkdir uac2 > /dev/null 2>&1
    cd uac2

    # Batch C1: basic USB descriptors
    echo 0x0200 > bcdUSB
    echo 0x00   > bDeviceClass
    echo 0x00   > bDeviceSubClass
    echo 0x00   > bDeviceProtocol
    echo 64     > bMaxPacketSize0
    echo 0x0BDA > idVendor
    echo 0x8730 > idProduct

    # Batch C2: strings
    mkdir strings/0x409 > /dev/null 2>&1
    echo "Realtek" > strings/0x409/manufacturer
    echo "UAC2 device" > strings/0x409/product
    cat /proc/realtek/uuid > strings/0x409/serialnumber 2>/dev/null || echo "01234567" > strings/0x409/serialnumber

    # Batch C3: config
    mkdir configs/c.1 > /dev/null 2>&1
    echo 120 > configs/c.1/MaxPower
    mkdir configs/c.1/strings/0x409 > /dev/null 2>&1
    echo "uac2" > configs/c.1/strings/0x409/configuration

    # Batch C4: UAC2 function attributes
    mkdir functions/uac2.0 > /dev/null 2>&1
    echo 3        > functions/uac2.0/c_chmask
    echo 48000    > functions/uac2.0/c_srate
    echo 2        > functions/uac2.0/c_ssize
    echo 3        > functions/uac2.0/p_chmask
    echo 48000    > functions/uac2.0/p_srate
    echo 2        > functions/uac2.0/p_ssize
    echo adaptive > functions/uac2.0/c_sync
    ln -sf functions/uac2.0 configs/c.1/
}

function usbd_vendor_init()
{
    echo "Vendor device init"

    mount_configfs

    cd /mnt/config/usb_gadget
    mkdir vendor > /dev/null 2>&1
    cd vendor

    echo 0x0200 > bcdUSB
    echo 0xa4a0 > idProduct
    echo 0x0525 > idVendor

    mkdir strings/0x409 > /dev/null 2>&1
    echo "Realtek" > strings/0x409/manufacturer
    echo "Vendor" > strings/0x409/product
    echo "123456789AB" > strings/0x409/serialnumber

    mkdir configs/c.1 > /dev/null 2>&1
    mkdir configs/c.2 > /dev/null 2>&1

    echo 120 > configs/c.1/MaxPower
    echo 120 > configs/c.2/MaxPower

    mkdir configs/c.1/strings/0x409 > /dev/null 2>&1
    mkdir configs/c.2/strings/0x409 > /dev/null 2>&1

    echo "vendor1" > configs/c.1/strings/0x409/configuration
    echo "vendor2" > configs/c.2/strings/0x409/configuration

    mkdir functions/SourceSink.0 > /dev/null 2>&1
    mkdir functions/Loopback.0 > /dev/null 2>&1

    ln -sf functions/SourceSink.0 configs/c.1/
    ln -sf functions/Loopback.0 configs/c.2/
}


function activate_device()
{
    echo "Activate device"
    # Activate device
    ls /sys/class/udc > UDC
}

function deactivate_device()
{
    echo "Deactivate device"
    if [ -d /mnt/config/usb_gadget ]; then
        for gadget in /mnt/config/usb_gadget/*/; do
            [ -f "${gadget}UDC" ] && echo "" > "${gadget}UDC" 2>/dev/null || true
        done
    fi
}

function usb_usage()
{
    echo "Usage: $0 [-b] [-r <role>]"
    echo
    echo "Description:"
    echo "  -b,                  Optional: Use built-in mode."
    echo "  -r <role>,           Mandatory: Specify the role to execute."
    echo
    echo "Where role:"
    echo "  otg                  OTG mode"
    echo "  usbh_acm             CDC ACM host"
    echo "  usbh_uvc             UVC host"
    echo "  usbh_msc             MSC host"
    echo "  usbh_hid             HID host"
    echo "  usbh_uac             UAC host"
    echo "  usbh_vendor          Vendor host"
    echo "  usbd_acm             CDC ACM device"
    echo "  usbd_adb             ADB device"
    echo "  usbd_hid             HID device"
    echo "  usbd_msc             MSC device"
    echo "  usbd_uac2            UAC2 audio device"
    echo "  usbd_vendor          Vendor device"
    echo
    echo "Example:"
    echo "  $0 -b -r otg"
    echo "  $0 -r usbd_adb"
    exit 1
}

while getopts 'br:h' OPT; do
    case $OPT in
        b)
            built_in=true
            ;;
        r)
            role="$OPTARG"
            ;;
        h)
            usb_usage
            ;;
        ?)
            usb_usage
            ;;
    esac
done

case "$role" in
    "otg")
        if [ "$built_in" != "true" ]; then
            insmod_otg_ko
        fi
        deactivate_device
        usbd_acc_init
        activate_device
        /bin/adbd &
        ;;
    "usbh_acm")
        if [ "$built_in" != "true" ]; then
            insmod_usbh_acm_ko
        fi
        usb_force_mode 1
        ;;
    "usbh_uvc")
        if [ "$built_in" != "true" ]; then
            insmod_usbh_uvc_ko
        fi
        usb_force_mode 1
        ;;

    "usbh_msc")
        if [ "$built_in" != "true" ]; then
            insmod_usbh_msc_ko
        fi
        usb_force_mode 1
        ;;
    "usbh_hid")
        if [ "$built_in" != "true" ]; then
            insmod_usbh_hid_ko
        fi
        usb_force_mode 1
        ;;
    "usbh_uac")
        if [ "$built_in" != "true" ]; then
            insmod_usbh_uac_ko
        fi
        usb_force_mode 1
        ;;
    "usbh_vendor")
        if [ "$built_in" != "true" ]; then
            insmod_usbh_vendor_ko
        fi
        usb_force_mode 1
        ;;
    "usbd_acm")
        if [ "$built_in" != "true" ]; then
            insmod_usbd_acm_ko
        fi
        deactivate_device
        usbd_acm_init
        usb_force_mode 2
        activate_device
        ;;
    "usbd_adb")
        if [ "$built_in" != "true" ]; then
            insmod_usbd_adb_ko
        fi
        deactivate_device
        usbd_acc_init
        usb_force_mode 2
        activate_device
        /bin/adbd &
        ;;
    "usbd_hid")
        if [ "$built_in" != "true" ]; then
            insmod_usbd_hid_ko
        fi
        deactivate_device
        usbd_hid_init
        usb_force_mode 2
        activate_device
        ;;
    "usbd_msc")
        if [ "$built_in" != "true" ]; then
            insmod_usbd_msc_ko
        fi
        deactivate_device
        usbd_msc_init
        usb_force_mode 2
        activate_device
        ;;
    "usbd_uac2")
        if [ "$built_in" != "true" ]; then
            insmod_usbd_uac2_ko
        fi
        deactivate_device
        usbd_uac2_init
        usb_force_mode 2
        activate_device
        ;;
    "usbd_vendor")
        if [ "$built_in" != "true" ]; then
            insmod_usbd_vendor_ko
        fi
        deactivate_device
        usbd_vendor_init
        usb_force_mode 2
        activate_device
        ;;
    *)
        usb_usage
        ;;
esac
