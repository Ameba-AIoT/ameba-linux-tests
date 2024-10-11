[Description]
User-space test case for Realtek Ameba GPIO driver, test is based on chardev.
GPIO controller chardev is /dev/gpiochipX, Ioctl operation could get gpiochip info and test specific GPIO pin.
GPIO test: input/output/interrupt.

[HW Setup]

None

[SW Setup]

Lunch with tests option.

[Parameters]
	# rtk_gpio_cdev_test  -c <gpio_chip> -n [gpio_offset] <mode>
		-c <0|1|2>: select GPIO chip,/dev/gpiochipX, use with other option
		-l: print gpio chip info, only use with -c option
		-n <gpio_offset>: select GPIO offset,0~31,must use with mode option

		mode option:
			-r :Read GPIO value at offset (INPUT mode)
			-w <0|1>: Write GPIO value (OUTPUT mode)
			-i <1|2|3>: value that set interrupt trigger type(INT mode): 1 for rising edge, 2 for falling edge, 3 for both edge.used only with option -p
			-p <1|2|3>: value that set interrupt trigger type (INT mode+poll)
[Result]

	# rtk_gpio_cdev_test -c 0  -l
	Chip name: gpiochip0
	Chip label: GPIO0
	Number of lines: 32
	offset:0, nameL ,consumer: ,Flags:[INPUT][ACTIVE_HIGHT]
	...

	#rtk_gpio_cdev_test -c 0 -n 15 -r
	Test: Read value from GPIO at offset 15 on chip /dev/gpiochip0 (INPUT mode)
	Read value of /dev/gpiochip0-15 success, value:1

	#rtk_gpio_cdev_test -c 0 -n 15 -w 1
	Test: Write value 1 to GPIO at offset 15 on chip /dev/gpiochip0 (OUTPUT mode)
	Write value 1 to /dev/gpiochip0-15 success!

	#rtk_gpio_cdev_test -c 0 -n 15 -p 1
	Test: Interrupt at GPIO offset 15 (INT mode) on chip /dev/gpiochip0
	event_data.timestamp:73506751080, event_data.id:1
	event_data.timestamp:73696054380, event_data.id:1
	event_data.timestamp:74892426640, event_data.id:1




