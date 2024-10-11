[Description]

User-space test case for Realtek Ameba GPIO driver, test  based on UIO.
The device file will be called /dev/uio0 for the first device, and /dev/uio1, /dev/uio2 and so on for subsequent devices.
Interrupts are handled by reading from /dev/uioX. A blocking read() from /dev/uioX will return as soon as an interrupt occurs.
You can also use poll() on /dev/uioX to wait for an interrupt.The integer value read from /dev/uioX represents the total interrupt count.
You can use this number to figure out if you missed some interrupts.

[HW Setup]
 test pin specified in DTS:
	gpio-uio-test {
		compatible = "generic-uio";
		status = "okay";
		interrupt-parent = <&gpioa>;
		interrupts = <15 IRQ_TYPE_EDGE_BOTH>;
	};

[SW Setup]
1. Lunch with tests option.

[Parameters]
# rtk_gpio_uio_test -d uio0 -i
# rtk_gpio_uio_test -d uio0 -p

[Result]
	# rtk_gpio_uio_test -d uio0 -i
	GPIO interrupt test start...
	Interrupt # 1
	Interrupt # 2
	...