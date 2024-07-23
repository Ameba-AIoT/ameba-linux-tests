#!/bin/bash

echo -e "\n--------------- TWO BOARDS TEST AVAILABLE TEST CASES --------------------------"
echo -e "1. I2C MASTER TX TEST to slave 0x23(7-bit mode) by default data, using /dev/i2c-0."
echo -e "2. I2C MASTER RX TEST to slave 0x23(7-bit mode) by default data, using /dev/i2c-0."
echo -e "3. I2C MASTER RX TEST to slave 0x23(7-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
echo -e "4. I2C MASTER RX TEST to slave 0x23(7-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
echo -e "5. I2C MASTER TX TEST to slave 0x23(10-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
echo -e "6. I2C MASTER RX TEST to slave 0x23(10-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
echo -e "----------------------------------------------------------------------------------"

echo -e "\nPlease select 1~6 for the default test cases:"
read TESTCASE

case $TESTCASE in
	"1")
		echo "I2C MASTER TX TEST to slave 0x23(7-bit mode) by default data, using /dev/i2c-0."
		rtk_i2c_test -W -v
		sleep 1
		;;
    "2")
		echo "I2C MASTER RX TEST to slave 0x23(7-bit mode) by default data, using /dev/i2c-0."
		rtk_i2c_test -R -v
		sleep 1
		;;
    "3")
		echo "I2C MASTER RX TEST to slave 0x23(7-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
		rtk_i2c_test -R -v -l 256
		sleep 1
		;;
    "4")
		echo "I2C MASTER RX TEST to slave 0x23(7-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
		rtk_i2c_test -R -v -l 256
		sleep 1
		;;
    "5")
		echo "I2C MASTER TX TEST to slave 0x23(10-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
		rtk_i2c_test -W -v -l 256 -b
		sleep 1
		;;
    "6")
		echo "I2C MASTER RX TEST to slave 0x23(10-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0."
		rtk_i2c_test -R -v -l 256 -b
		sleep 1
		;;
    *)
        echo "Invalid Option"
        ;;
esac
