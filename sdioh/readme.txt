[Description]

User-space test case for Realtek Ameba SDIOH driver.

[HW Setup]

Insert a FAT32-formatted micro-SD card to the SDIOH slot on the board.

[SW Setup]

1. Enable the pinmux group for SDIOH in DTS for tests.
2. Lunch with tests option.

[Parameters]

None

[Result]

	# rtk_sdioh_test
	SDIOH test started
	Open test file /mnt/storage/mmcblk0p1/sd_test.txt
	Write success
	Context: hello
	Seek success
	Read success
	Context: hello
	SDIOH test PASS
