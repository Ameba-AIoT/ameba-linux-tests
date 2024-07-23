[Description]

Test case for Realtek Ameba ADC driver.

[HW Setup]

Wire the pins of ADC channels to specific voltage sources, e.g. 3.3V on the board.
ATTENTION: for smart c-cut, voltage for test cannot be larger than 1.8V.

[SW Setup]

1. Enable the pinmux group for Captouch in DTS for tests, since ADC share pins
   with Captouch. Enable captouch DTS node & disable nodes which share the same
   pins (e.g. i2c0 shares PA_0 & PA_1 pins, disble i2c0 node if testing these
   pins).
2. Lunch with tests option.

[DTS Illustrate]

Use rtl8730elh-va7-tests-adc-sig.dtb or rtl8730elh-va7-tests-adc-diff.dtb for the corresponding test.

Meanings of parameters are explaned as following:

    &adc {
        rtk,adc-mode = <1>;
        rtk,adc-timer-period = <200000000>;
        rtk,adc-channels = <0>, <1>, <6>;
        //rtk,adc-diff-channels = <0 1>;
        status = "okay";
    };

NOTE: If single channel is for test, use "rtk,adc-channels = <0>, <1>, <6>" to enable channel 0, 1 and 6.
If diff channel is for test, use "rtk,adc-diff-channels = <0 1>" to enable the differential test of channel 0 and 1.
Do not use these two configuration at the same time.

NOTE: rtk,adc-mode = <0>; // 0: Software-trigger mode, 1: Automatic mode, 2: Timer-trigger mode
rtk,adc-timer-period = <200000000>; // Useful only in timer-trigger mode. Unit: ns. Range: 100ms~2s

[Test Process]

1   Download rtl8730elh-va7-tests-adc-sig.dtb
1)  Single read channel 0:

    # rtk_adc_test 0
    Single ended mode test for channel = 0
    CH[0] Raw ADC count: 818
    CH[0] Calibration offset: -23
    CH[0] Scale: 0.849334717
    CH[0] Calibrated voltage: 675.2211 millivolts

2) Buffer read channel 0:

    # sh /bin/rtk_adc_test.sh -l 16 -n 1 -c 0
    iio iio:device0: Update scan mode chan 0 to seq 1
    Trigger 1
    Wait 1s...
    Data type of channel 0:
    le:u12/16>>0
    Data available:
    1

    Get data from buffer:
    / # cat /dev/iio:device0 | xxd -c 2
    00000000: 5c0f  \.
    ^C (Press Ctrl+C to exit.)
    / # ^C

    NOTE: Data appear in /dev/iio:device0 then PASS.
    For software mode: 1 data. For automatic and timer mode: data length is equal to buffer length (e.g. 16) if time is sufficient.
    0x5c0f is a little edian raw-data. Use (raw + offset) * scale in practical application.
    Offset and Scale is in testcase 1-1), e.g. (0xf5c - 23) * 0.849334717 = 3320.049408753mV.

2   Download rtl8730elh-va7-tests-adc-diff.dtb
    Single read differential ended <2, 3>:

    # rtk_adc_test diff 2 3
    Differential mode test for channel pair <vinp vinn> = <2 3>
    CH[2-3] Raw ADC count: 3235
    CH[2-3] Calibration offset: -2016
    CH[2-3] Scale: 0.849334717
    CH[2-3] Calibrated voltage: 1035.3390 millivolts

Note that the final Calibrated voltage is calculated using below std IIO
formula:
Calibrated voltage = (Raw ADC count + Calibration offset) * Scale

[Usage-Guidance of ADC Buffer Mode]

Buffer trigger mode is an offical interface in file-system. Command lines for adc will be recommended as following.

1) Add trigger source:
echo 0 > /sys/devices/iio_sysfs_trigger/add_trigger
echo sysfstrig0 > /sys/bus/iio/devices/iio:device0/trigger/current_trigger

2) Add scan list:
echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage0_en
echo 1 > /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage1_en

3) Set the maxmum length of buffer:
echo 16 > /sys/devices/platform/ocp/42012000.adc/iio:device0/buffer/length

4) Set watermark and enable
echo 2 > /sys/devices/platform/ocp/42012000.adc/iio:device0/buffer/watermark
echo 1 > /sys/bus/iio/devices/iio:device0/buffer/enable

5) Trigger for data:
echo 1 > /sys/bus/iio/devices/trigger0/trigger_now

6) Get data parameters:
cat /sys/devices/platform/ocp/42012000.adc/iio:device0/buffer/data_available
cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage0_type
cat /sys/devices/platform/ocp/42012000.adc/iio:device0/scan_elements/in_voltage1_type

7) Get data in buffer:
cat /dev/iio:device0 | xxd -c 2

8) Disable buffer mode:
Settings can only be changed when buffer-mode is disabled.
echo 0 > /sys/bus/iio/devices/iio:device0/buffer/enable