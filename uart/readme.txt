[Description]

User-space test case for Realtek Ameba UART driver.
    
[HW Setup]

Connect UART2 pins to PC via USB-UART dongle:
UART2_RXD <-> USB-UART dongle TXD
UART2_TXD <-> USB-UART dongle RXD
GND       <-> USB-UART dongle GND

[SW Setup]

1. Enable the pinmux group for UART0/1/2 in DTS for tests.
2. Lunch with tests option.

[Parameters]

Adjust the UART0/1/2 pinmux in pinctrl DTS:
e.g. UART2:
	uart2_pins: uart2@0 {
		pins {
			pinmux = <REALTEK_PINMUX('A', 11, UART_DATA)>,        // UART2_RXD
				<REALTEK_PINMUX('A', 12, UART_DATA)>;         // UART2_TXD
			bias-pull-up;
			slew-rate = <0>;
			drive-strength = <0>;
		};
	};

Use [-P 0/1/2] to test the three UART ports, e.g. UART2: rtk_uart_test -P 2

[Result]

	$ rtk_uart_test -P 2
	Start loopback...

Open the serial port corresponding to UART2 via serial tool (e.g. Realtek Ameba Trace Tool) on PC,
send any characters to UART2, UART2 will echo back.
