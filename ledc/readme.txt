[Description]

User-space test case for Realtek Ameba LEDC driver.
Special type for testcase: WS2812C-2020. This test use 64 leds.
LED Specific Parameters:
LEDs are in GRB mode.
|-------------------|
| Color	Brightness  |
| Red	100 - 150   |
| Green	250 - 350   |
| Blue	30 - 60     |
|-------------------|
Data Transfer Time
T0H 0， high level time: 220ns~380ns
T1H 1， high level time: 580ns~1μs
T0L 0， low level time: 580ns~1μs
T1L 1， low level time: 580ns~1μs
RES Frame low level time: above 280μs
(The unit of T0H, T1H, T0L, T1L, RESET is 25ns)

[HW Setup]

Connect a LED to Ameba board, e.g.:
LED 5V	 <-> Ameba 5V
LED GND	 <-> Ameba GND
LED DATA <-> Ameba LEDC pin

[SW Setup]

1. Enable the pinmux group for LEDC in DTS for tests.
2. Lunch with tests option.

[Parameters]
For WS2812C-2020 LEDC, configuration parameters are as following:
	&ledc{
		pinctrl-names = "default";
		pinctrl-0 = <&ledc_pins>;
		rtk,led-nums = <64>;
		rtk,output-RGB-mode = <0>; // Refer to spec for details
		rtk,data-tx-time0h = <0xC>;
		rtk,data-tx-time0l = <0x18>;
		rtk,data-tx-time1h = <0x18>;
		rtk,data-tx-time1l = <0x18>;
		rtk,refresh-time = <0x3FFF>;
		rtk,auto-dma-thred = <64>;
		dmas = <&dma 5>;
		dma-names = "dma_leds_tx";
		status = "okay";
	};

[Test Process]
LEDC function can be support by system control.
rtk_ledc_test.sh is provided to control ledc easilier by pre-defined instructions.

1. sh /bin/rtk_ledc_test.sh -n 16 -c red
   Check: 16 leds are brigtened with red color. This is a test for CPU mode.
   Use /bin/rtk_ledc_test.sh -e 16 to clear leds.

2. sh /bin/rtk_ledc_test.sh -n 8 -c green
   Check: 8 leds are brigtened with green color. This is a test for CPU mode.
   Use /bin/rtk_ledc_test.sh -e 8 to clear leds.

3. sh /bin/rtk_ledc_test.sh -n 12 -c yellow
   Check: 12 leds are brigtened with yellow color. This is a test for CPU mode.
   Use /bin/rtk_ledc_test.sh -e 12 to clear leds.

4. sh /bin/rtk_ledc_test.sh -n 32 -c blue
   Check: 32 leds are brigtened with blue color. This is a test for CPU mode.
   Use /bin/rtk_ledc_test.sh -e 32 to clear leds.

5. sh /bin/rtk_ledc_test.sh -p
   Check: Firstly show a red trotting horse lamp. Then show a yellow trotting horse lamp.
   End with an alternating pattern with red and yellow. This is a test for CPU mode.
   Use /bin/rtk_ledc_test.sh -e 16 to clear leds.

6. sh /bin/rtk_ledc_test.sh -n 64 -c blue
   Check: 64 leds are brigtened with blue color. Length above threshold-32 will use dma transfer automatically.
   Use /bin/rtk_ledc_test.sh -e 64 to clear leds.

7. sh /bin/rtk_ledc_test.sh -n 56 -c green
   Check: 56 leds are brigtened with green color. Length above threshold-32 will use dma transfer automatically.
   Use /bin/rtk_ledc_test.sh -e 56 to clear leds.

8. sh /bin/rtk_ledc_test.sh -n 128 -c yellow
   Check: 64 leds are brigtened with yellow color. Length above threshold-32 will use dma transfer automatically.
   The number of using-leds is set as 64, output 128 data means refresh 64 leds twice. The refresh cannot be discovered by eyes.
   Use /bin/rtk_ledc_test.sh -e 64 to clear leds.

NOTE: when dma used, the data length can only be a multiple of 8, such as 40, 48, 56, 64. Otherwise, only 32 leds will bright if 33~39 data provided.
