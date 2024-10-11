[Description]

Test case for Realtek Ameba Captouch driver.

[HW Setup]

It is suggested to connect the Captouch ext-board to the FPC interface of the board.

[SW Setup]

1. Enable the pinmux group for Captouch in DTS for tests.
2. Lunch with tests option.

[Parameters]
Adjust the Captouch properties in DTS file as required:
	&captouch {
		pinctrl-names = "default";
		pinctrl-0 = <&captouch_pins_a>;
		rtk,ctc-diffthr = <800>, <800>, <800>, <800>, <1000>, <1000>, <1000>, <1000>, <1000>;
		rtk,ctc-mbias = <0x18>, <0x17>, <16>, <0x1B>, <0x00>, <0x00>, <0x00>, <0x00>, <0x00>;
		rtk,ctc-nnoise = <400>, <400>, <400>, <400>, <1000>, <1000>, <1000>, <1000>, <1000>;
		rtk,ctc-pnoise = <400>, <400>, <400>, <400>, <1000>, <1000>, <1000>, <1000>, <1000>;
		rtk,ctc-ch-status = <1>, <1>, <0>, <0>, <0>, <0>, <1>, <1>, <1>;
		status = "okay";
	};
Where:
	pinctrl-0: pinmux, adjust as per hw implementations
	rtk,ctc-diffthr: diff threshold
	rtk,ctc-mbias: mbias
	rtk,ctc-nnoise: n-noise
	rtk,ctc-pnoise: p-noise
	ctc-ch-status: channel status, 1-enabled, 0-disabled, sync with pinmux settings
Refer to Captouch AN for details.

[Result]

    # rtk_captouch_test

Press the buttons on Captouch ext-board or directly touch the Captouch pins on the board, following
message will be printed:

    Keycode xxx pressed
    Keycode xxx released