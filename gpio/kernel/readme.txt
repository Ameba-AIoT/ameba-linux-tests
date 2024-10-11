[Description]

Test case for Realtek Ameba GPIO driver, implemented as kernel module.

[HW Setup]

Wire the two GPIO pins under test specified in DTS together:
	gpio-test {
		compatible = "realtek,ameba-gpio-test";
		device_type = "gpio-dev";
		test-gpios = <&gpiob 7 0>,<&gpiob 8 0>;  // GPIO pins under test: IN, OUT
	};
Adjust the GPIO pins under test in DTS, avoid the pinmux conflicts.

[SW Setup]

1. Enable the pinmux group for GPIO in DTS for tests.
2. Lunch with tests option.

[Parameters]

# insmod /lib/modules/5.4.63/kmod-gpio-test.ko test_case=x

please select the test case number x 0~8.
0: PB-7 high level interrupt
1: PB-7 low level interrupt
2: PB-7 rise edge interrupt
3: PB-7 fall edge interrupt
4: PB-7 both edge interrupt
6: PB-7 output test
7: PB-7 input test
8: PB-7 as input, PB-8 as output, connect the two pins to each other.

[Result]

	# insmod /lib/modules/5.4.63/kmod-gpio-test.ko test_case=x
	...
	GPIO test PASS