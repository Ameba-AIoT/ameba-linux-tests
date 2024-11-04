export PATH

KERNEL_VERSION=`cat /proc/version | awk '{print $3}'`

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
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbh_msc_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/storage/usb-storage.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
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
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
}

function insmod_usbd_cdc_acm_ko()
{
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/rtkdrivers/usb_phy/phy-rtk-usb.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/common/usb-common.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/core/usbcore.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/udc/udc-core.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/fs/configfs/configfs.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/libcomposite.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/u_serial.ko
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/gadget/function/usb_f_acm.ko
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
    insmod_ko /lib/modules/$KERNEL_VERSION/kernel/drivers/usb/dwc2/dwc2.ko
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

function usbh_msc_init()
{
    #Force host mode
    echo 1 > /sys/module/phy_rtk_usb/parameters/usb_force_mode
}

function usbd_acc_init()
{
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
    #Force device mode
    echo 2 > /sys/module/phy_rtk_usb/parameters/usb_force_mode
}

function usbd_cdc_acm_init()
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
    #Force device mode
    echo 2 > /sys/module/phy_rtk_usb/parameters/usb_force_mode
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
    #Force device mode
    echo 2 > /sys/module/phy_rtk_usb/parameters/usb_force_mode
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
    #Force device mode
    echo 2 > /sys/module/phy_rtk_usb/parameters/usb_force_mode
}

function active_device()
{
    echo "Activate device"
    # Activate device
    ls /sys/class/udc > UDC
}

function deactive_device()
{
    echo "Deactivate device"
    # Activate device
    echo "" > UDC
}

function usb_usage()
{
    echo "Usage: $0 <mode>"
    echo "Where mode:"
    echo "    otg      - OTG mode"
    echo "    usbh_msc - MSC host"
    echo "    usbd_acm - CDC ACM device"
    echo "    usbd_adb - ADB device"
    echo "    usbd_hid - HID device"
    echo "    usbd_msc - MSC device"
}

if [ $# -lt 1 ]; then
    usb_usage
    exit 1
fi

case $1 in
    "otg")
        insmod_otg_ko
        deactive_device
        usbd_acc_init
        active_device
        /bin/adbd &
        ;;
    "usbh_msc")
        insmod_usbh_msc_ko
        usbh_msc_init
        ;;
    "usbd_acm")
        insmod_usbd_cdc_acm_ko
        deactive_device
        usbd_cdc_acm_init
        active_device
        ;;
    "usbd_adb")
        insmod_usbd_adb_ko
        deactive_device
        usbd_acc_init
        active_device
        /bin/adbd &
        ;;
    "usbd_hid")
        insmod_usbd_hid_ko
        deactive_device
        usbd_hid_init
        active_device
        ;;
    "usbd_msc")
        insmod_usbd_msc_ko
        deactive_device
        usbd_msc_init
        active_device
        ;;
    *)
        usb_usage
        ;;
esac