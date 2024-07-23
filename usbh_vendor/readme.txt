[Description]

User-space test case for Realtek Ameba USB vendor host driver.

[HW Setup]

Attach a USB vendor device to the Ameba board.

[SW Setup]

Lunch with tests option.

[Parameters]

None

[Result]

For loopback test(case 30, -t30, other parameters can be modified):
	$ rtk_usbh_vendor_test -a -c1 -t30 -s256 -g32 -v1

For sourcesink test(other case,parameters can be modified, -t can be used to select test case):
	$ testusb -a
