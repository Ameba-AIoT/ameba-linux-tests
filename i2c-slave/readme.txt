[Description]

This is a test case for Ameba I2C slave, implemented as kernel module.

[HW Setup]

Prepare two Ameba boards:
* One for I2C master, burned with I2C master image
* The other for I2C slave, burned with I2C slave image
Wire the two boards via I2C interface: SDA, SCL and GND.
Refer to the tests DTS file for I2C pinmux.

Attention: If I2C master and slave are running on the same board and
communicate with each other, the I2C slave and master will generate
interrupts at the same time and the master interrupts will be disturbed.

[SW Setup]

1. Enable the pinmux group for I2C slave in DTS for tests.
2. Lunch with tests option.

[Parameters]

Adjust the I2C slave pins under test in DTS, avoid the pinmux conflicts:
	&i2c1 {
		pinctrl-names = "default";
		pinctrl-0 = <&i2c1_pins>; 		// Pinmux
	    #address-cells = <1>;			// For i2c bus address
	    #size-cells = <0>; 				// Only reg address is needed
		rtk,i2c-reg-slave-num = <2>;	// I2C slaves
		status = "okay";
	    i2c-test1 {
	        compatible = "realtek,ameba-i2c-slave-test";
	        reg = <0x23>;
	    };
	    i2c-test2 {
	        compatible = "realtek,ameba-i2c-slave-test-append";
	        reg = <0x25>;
	    };
	};

	i2c1_pins: i2c1@0 {
		pins {
			pinmux = <REALTEK_PINMUX('B', 12, I2C)>, // I2C1_SDA
					<REALTEK_PINMUX('B', 13, I2C)>;  // I2C1_SCL
			bias-pull-up;
			slew-rate = <0>;
			drive-strength = <0>;
		};
	};

[Test Process]

/ # cd lib/modules/
/lib/modules # ls
5.4.63                   kmod-dmac-test.ko        kmod-hello-test.ko
kmod-dmac-multi-test.ko  kmod-gpio-test.ko        kmod-i2c-slave-test.ko

See kmod-i2c-slave-test.ko inside this directory. kmod-i2c-slave-test.ko has two input-parameters: addr10bit, verbose
Set addr10bit=1 to config a 10-bit address transfer mode for i2c. Set addr10bit=0 to config a 7-bit address transfer mode for i2c (default).
Set verbose=1 to open data dump (default). Set verbose=0 to close data dump.

Please refer to /sdk/sources/tests/i2c/readme.txt for the corresponding master settings.
ATTENTION!!! Make i2c slave online first, then use i2c master for transmitting.

[Result on two boards]
1
Slave: insmod kmod-i2c-slave-test.ko

Master: rtk_i2c_test -W -v
Start I2C-Master TX TEST on device /dev/i2c-0 to slave address 0x23.
I2C Master TX content:
TX | FF FF FF FF FF FF 40 00 00 00 00 95 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF F0 0D  |......@.........................|

Slave received master data:
FF FF FF FF FF FF 40 00 00 00 00 95 FF FF FF FF        | total: 16 bytes.
FF FF FF FF FF FF FF FF FF FF FF FF FF FF F0 0D        | total: 32 bytes.
Check the bytes and content of TRX-end manually. If the length and the content is the same, then PASS.


2
Slave: insmod kmod-i2c-slave-test.ko (If kmod-i2c-slave-test.ko already on, no need to do this)

Master: rtk_i2c_test -R -v
Start I2C-Master RX TEST on device /dev/i2c-0 to slave address 0x23.
I2C Master RX content:
RX | 23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  |#...............................|

Slave give data to master:
23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F        | total: 16 bytes.
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F        | total: 32 bytes.
Check the bytes and content of TRX-end manually. If the length and the content is the same, then PASS.


3
Slave: insmod kmod-i2c-slave-test.ko (If kmod-i2c-slave-test.ko already on, no need to do this)

Master: rtk_i2c_test -W -v -l 256
Start I2C-Master TX TEST on device /dev/i2c-0 to slave address 0x23.
I2C Master TX content:
RX | 23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  |#...............................|
RX | 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F  | !"#$%&'()*+,-./0123456789:;<=>?|
RX | 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F  |@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_|
RX | 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F  |`abcdefghijklmnopqrstuvwxyz{|}~.|
RX | 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F  |................................|
RX | A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF  |................................|
RX | C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF  |................................|
RX | E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF  |................................|

Slave received master data:
23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F        | total: 16 bytes.
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F        | total: 32 bytes.
20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F        | total: 48 bytes.
30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F        | total: 64 bytes.
40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F        | total: 80 bytes.
50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F        | total: 96 bytes.
60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F        | total: 112 bytes.
70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F        | total: 128 bytes.
80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F        | total: 144 bytes.
90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F        | total: 160 bytes.
A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF        | total: 176 bytes.
B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF        | total: 192 bytes.
C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF        | total: 208 bytes.
D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF        | total: 224 bytes.
E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF        | total: 240 bytes.
F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF        | total: 256 bytes.
Check the bytes and content of TRX-end manually. If the length and the content is the same, then PASS.


4
Slave: insmod kmod-i2c-slave-test.ko (If kmod-i2c-slave-test.ko already on, no need to do this)

Master: rtk_i2c_test -R -v -l 256
Start I2C-Master RX TEST on device /dev/i2c-0 to slave address 0x23.
I2C Master RX content:
RX | 23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  |#...............................|
RX | 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F  | !"#$%&'()*+,-./0123456789:;<=>?|
RX | 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F  |@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_|
RX | 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F  |`abcdefghijklmnopqrstuvwxyz{|}~.|
RX | 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F  |................................|
RX | A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF  |................................|
RX | C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF  |................................|
RX | E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF  |................................|

Slave give data to master:
23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F        | total: 16 bytes.
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F        | total: 32 bytes.
20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F        | total: 48 bytes.
30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F        | total: 64 bytes.
40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F        | total: 80 bytes.
50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F        | total: 96 bytes.
60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F        | total: 112 bytes.
70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F        | total: 128 bytes.
80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F        | total: 144 bytes.
90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F        | total: 160 bytes.
A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF        | total: 176 bytes.
B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF        | total: 192 bytes.
C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF        | total: 208 bytes.
D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF        | total: 224 bytes.
E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF        | total: 240 bytes.
F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF        | total: 256 bytes.


5
Slave: insmod kmod-i2c-slave-test.ko addr10bit=1 (If kmod-i2c-slave-test.ko already on, rmmod and insmod again by new parameter.)

Master: rtk_i2c_test -W -v -l 256 -b
I2C Master TX content:
TX | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  |................................|
TX | 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F  | !"#$%&'()*+,-./0123456789:;<=>?|
TX | 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F  |@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_|
TX | 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F  |`abcdefghijklmnopqrstuvwxyz{|}~.|
TX | 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F  |................................|
TX | A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF  |................................|
TX | C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF  |................................|
TX | E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF  |................................|

Slave received master data:
00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F        | total: 16 bytes.
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F        | total: 32 bytes.
20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F        | total: 48 bytes.
30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F        | total: 64 bytes.
40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F        | total: 80 bytes.
50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F        | total: 96 bytes.
60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F        | total: 112 bytes.
70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F        | total: 128 bytes.
80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F        | total: 144 bytes.
90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F        | total: 160 bytes.
A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF        | total: 176 bytes.
B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF        | total: 192 bytes.
C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF        | total: 208 bytes.
D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF        | total: 224 bytes.
E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF        | total: 240 bytes.
F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF        | total: 256 bytes.


6
Slave: insmod kmod-i2c-slave-test.ko addr10bit=1 (If kmod-i2c-slave-test.ko already on, rmmod and insmod again by new parameter.)

Master: rtk_i2c_test -R -v -l 256 -b
Start I2C-Master RX TEST on device /dev/i2c-0 to slave address 0x23.
I2C Master RX content:
RX | 23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F  |#...............................|
RX | 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F  | !"#$%&'()*+,-./0123456789:;<=>?|
RX | 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F  |@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_|
RX | 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F  |`abcdefghijklmnopqrstuvwxyz{|}~.|
RX | 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F  |................................|
RX | A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF  |................................|
RX | C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF  |................................|
RX | E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF  |................................|

Slave give data to master:
23 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F        | total: 16 bytes.
10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F        | total: 32 bytes.
20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F        | total: 48 bytes.
30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F        | total: 64 bytes.
40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F        | total: 80 bytes.
50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F        | total: 96 bytes.
60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F        | total: 112 bytes.
70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F        | total: 128 bytes.
80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F        | total: 144 bytes.
90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F        | total: 160 bytes.
A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF        | total: 176 bytes.
B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF        | total: 192 bytes.
C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF        | total: 208 bytes.
D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF        | total: 224 bytes.
E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF        | total: 240 bytes.
F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF        | total: 256 bytes.
