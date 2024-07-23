[Description]

User-space test case for Realtek Ameba USB UVC host driver.

[HW Setup]

Attach an UVC camera and a SD card to the Ameba board.

[SW Setup]

Lunch with tests option.

[Parameters]

None

[Result]

	# rtk_usbh_uvc_test /dev/video0

The test will capture images and save it to the attached SD card.
Check the captured images on the SD card.
