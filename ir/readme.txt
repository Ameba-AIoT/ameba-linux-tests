[Description]

User-space test case for Realtek Ameba IR driver.

[HW Setup]

For IR TX test: 
Prepare an IR receiver (e.g. another Ameba board) which can receive and display the IR scancode.

For IR RX test: 
1. Wire rx_en / tx_pin to GND;
2. Prepare an IR transmitter (e.g. IR remote controller).

[SW Setup]

1. Enable the pinmux group for IR in DTS for tests.
2. Lunch with tests option.

[Parameters]

Change the TX scancode in test code as required.

[Result]

For IR TX test: 
	# rtk_ir_test
	IR TX test started
	Open IR device node
	Set send mode to scancode
	TX scancode: 0xEA158A75
	Check the IR receiver for RX scancode
	Close IR device node
	IR TX test done
Then check the received scancode via IR receiver.

For IR RX test:
	# getevent -l &
	add device 1: /dev/input/event0
	  name:     "ir_rtk"
Then send scancode via IR transmitter, Ameba will catch the IR RX events, e.g.:
	/dev/input/event0: EV_REP       REP_DELAY            00000000
	/dev/input/event0: EV_REP       REP_PERIOD           00000000
	/dev/input/event0: EV_MSC       MSC_SCAN             00eac40a
	/dev/input/event0: EV_SYN       SYN_REPORT           00000000
	/dev/input/event0: EV_MSC       MSC_SCAN             00eac48a
	/dev/input/event0: EV_SYN       SYN_REPORT           00000000
	/dev/input/event0: EV_MSC       MSC_SCAN             00eac48a

[Explanation]

The tx code in ir test is 0xEA158A75, the code type is RC_PROTO_NEC (Normal NEC), the valide-bits is only 0x8A75 according to the type.
The valide bits will be encoded by ir_raw_encode_scancode to kernel space.
Code sent in air will be 0x51AEAE51 (little edian to big edian).
The signal is received by ir receiver and decoded by ir-nec-decoder, and the code delivered to userspace will be 0x8A75 finally, which is equeal to the send code.

If two boards are used, and wire tx to rx end. Run ir test to tx, the logs prefer is:
/dev/input/event1: EV_REP       REP_DELAY            00000000
/dev/input/event1: EV_REP       REP_PERIOD           00000000
/dev/input/event1: EV_MSC       MSC_SCAN             00008a75
/dev/input/event1: EV_SYN       SYN_REPORT           00000000