[Description]

User-space test case for Realtek Ameba I2C master driver.

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

1. Enable the pinmux group for I2C master in DTS for tests.
2. Lunch with tests option.

[Parameters]

Adjust the I2C master pins under test in DTS, avoid the pinmux conflicts:
	&i2c0 {
		pinctrl-names = "default";
		pinctrl-0 = <&i2c0_pins>;	 // Pinmux
		rtk,i2c-reg-slave-num = <0>; // I2C master
		status = "okay";
	};

	i2c0_pins: i2c0@0 {
		pins {
			pinmux = <REALTEK_PINMUX('A', 0, I2C)>, // I2C0_SDA
					<REALTEK_PINMUX('A', 1, I2C)>;  // I2C0_SCL
			bias-pull-up;
			slew-rate = <0>;
			drive-strength = <0>;
		};
	};

[Test Process]
Test application is a i2c-dev test application. Support for i2c master tx or i2c master rx.

Below is the usage:

~ # rtk_i2c_test --help
Usage: rtk_i2c_test [-RMSTsbdHOvplI]
  -M --masterdevice       device to use for I2C Master (default /dev/i2c-0)
  -a --address            send data to slave address (default 0x23)
  -b --addr10bit          slave address is in ten-bit mode
  -R --read               master read data form slave device
  -W --write              master write data to slave device
  -i --input              input data from a file (e.g. "test.bin")
  -o --output             output data to a file (e.g. "results.bin")
  -v --verbose            Verbose (show tx buffer) (default OFF)
  -p                      Send data (e.g. "1234\xde\xad")
  -l --size               transfer size/length of buf to transfer

Examples:
	Execute I2C MASTER TX by default 'rtk_i2c_test -W -v'
	Execute I2C MASTER RX by default 'rtk_i2c_test -R -v'
	For more examples, please refer to i2c_test_script.sh included in i2c tests src code.

~ #

Please find enclosed a helper bash script i2c_test_script.sh which implements
common test cases for testing as well as for reference. It can execute below test
cases which are most common test cases (it internally calls rtk_i2c_test with
appropriate arguments/options).

~ # sh i2c_test_script.sh

--------------- TWO BOARDS TEST AVAILABLE TEST CASES --------------------------
1. I2C MASTER TX TEST to slave 0x23(7-bit mode) by default data, using /dev/i2c-0.
2. I2C MASTER RX TEST to slave 0x23(7-bit mode) by default data, using /dev/i2c-0."
3. I2C MASTER RX TEST to slave 0x23(7-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0.
4. I2C MASTER RX TEST to slave 0x23(7-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0.
5. I2C MASTER TX TEST to slave 0x23(10-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0.
6. I2C MASTER RX TEST to slave 0x23(10-bit mode) 256 bytes INCREMENTAL data, using /dev/i2c-0.
----------------------------------------------------------------------------------

Please refer to /sdk/sources/tests/i2c-slave/readme.txt for the corresponding slave settings.
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
