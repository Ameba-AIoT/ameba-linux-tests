[Description]

User-space testcase for Realtek Ameba GPIO driver, test is based on sysfs.
Read and write files in /sys/class/gpio/ and /sys/class/gpio/gpioN/ could test GPIO pin.
GPIO test: input/output/interrupt.

[HW Setup]
None

[SW Setup]
1. open option in menuconfig: Device Drivers > GPIO support
 [*]   /sys/class/gpio/... (sysfs interface)
2. Lunch with tests option.

[Test]
 # rtk_gpio_sysfs_test -n <gpio> <mode>
	<gpio>: select GPIO pin num, must use with mode option
	<mode>:
		-r :Read GPIO value at offset (INPUT mode)
		-w <0|1>: Write GPIO value (OUTPUT mode)
		 -i <rising |falling|both>: value that set interrupt trigger type (INT mode)
[Result]
	# rtk_gpio_sysfs_test -n 15 -r
	GPIO test export pin 15
	GPIO read test start...
	...
	gpio_read  value: 1


	# rtk_gpio_sysfs_test -n 15 -w 1
	GPIO test export pin 15
	GPIO write test start...
	...
	gpio_write value: 0

	#  rtk_gpio_sysfs_test -n 15 -i rising
	GPIO test export pin 15
	GPIO edge interrupt test start...
	gpio_config: set attr direction:in
	gpio_config: set attr edge:rising
	GPIO int <value=1>
	...


