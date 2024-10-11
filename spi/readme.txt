[Description]

User-space test case for Realtek Ameba SPI driver.

[HW Setup]

For master/slave test mode:
Wire the two SPI interfaces on Ameba board as specified in DTS.
By default, DTS & test application use SPI-0 ("/dev/spidev0.0") as master
and SPI-1 ("/dev/spidev1.0") as slave.
You can test using single board or two separate boards. For single board
test connect SPI-0 pins to SPI-1 pins of the same board. For two boards
test, connect SPI-0 pins of board-1 to SPI-1 pins of board-2.

Master		 	Slave
CS		<->		CS
CLK		<->		CLK
MOSI	<->		MOSI
MISO    <->     MISO

FULL DUPLEX spi test mode: wire spi0-MISO to spi0-MOSI.

[SW Setup]

1. Enable the pinmux group for SPI master and SPI slave in DTS for tests.
    a. Enable spi0 & spi1 DTS nodes in rtl8730elh-va7-tests-g2.dts and rtl8730elh-va8-tests-g2.dts.
    b. Disable i2c0, ledc & ir nodes. This is because the pins used by these modules conflict with spi0/spi1 pins.
2. Lunch with tests option.
NOTE: if slave mode included in test, please confirm SPI_SLAVE enabled.
bitbake linux-ameba -c menuconfig
 Symbol: SPI_SLAVE [=y]                                                                                                                                                                             │
  │ Type  : bool                                                                                                                                                                                       │
  │ Prompt: SPI slave protocol handlers                                                                                                                                                                │
  │   Location:                                                                                                                                                                                        │
  │     -> Device Drivers                                                                                                                                                                              │
  │ (1)   -> SPI support (SPI [=y])                                                                                                                                                                    │
  │   Defined at drivers/spi/Kconfig:925                                                                                                                                                               │
  │   Depends on: SPI [=y]

[Parameters]

Adjust the pinmux in DTS as required, avoid the pinmux conflicts:

	&spi0 {
		pinctrl-names = "default";
		pinctrl-0 = <&spi0_pins>;
		rtk,spi-slave-mode = <0>; // SPI master
		status = "disabled";
	};

	&spi1 {
		pinctrl-names = "default";
		pinctrl-0 = <&spi1_pins>;
		rtk,spi-slave-mode = <1>; // SPI slave
		status = "disabled";
	};

	spi0_pins: spi0@0 {
		pins {
			pinmux = <REALTEK_PINMUX('B', 17, SPI)>,  // SPI0_CS
					<REALTEK_PINMUX('B', 18, SPI)>,   // SPI0_CLK
					<REALTEK_PINMUX('B', 19, SPI)>,   // SPI0_MOSI
					<REALTEK_PINMUX('B', 20, SPI)>;   // SPI0_MISO
			bias-pull-up;
			slew-rate = <0>;
			drive-strength = <0>;
		};
	};

	spi1_pins: spi1@0 {
		pins {
			pinmux = <REALTEK_PINMUX('A', 2, SPI)>, // SPI1_CS
					<REALTEK_PINMUX('A', 3, SPI)>,  // SPI1_CLK
					<REALTEK_PINMUX('A', 4, SPI)>,  // SPI1_MISO
					<REALTEK_PINMUX('A', 5, SPI)>;  // SPI1_MOSI
			bias-pull-up;
			slew-rate = <0>;
			drive-strength = <0>;
		};
	};


[Test Process]
Test application is a standard spidev test application. But original
application only supports master mode & only supports full duplex transfers.
Standard test application is modified to support master mode & slave mode both
and added support for SlaveTX<-->MasterRX, SlaveRX<-->MasterTX & full duplex
transfers.

Below is the usage:

~ # rtk_spi_test --help
Usage: rtk_spi_test [-RMSTsbdHOvplI]
  -R --role(MASTER/SLAVE) SPI Master or Slave (default MASTER)
  -M --masterdevice       device to use for SPI Master (default /dev/spidev0.0)
  -S --slavedevice        device to use for SPI Slave (default /dev/spidev1.0)
  -T --transfertype       0 for fullduplex(default); 1 for tx-only; 2 for rx-only
  -s --speed              max speed (Hz) (default 1MHz)
  -d --delay              delay (usec) (default 100usec)
  -b --bpw                bits per word (default 8)
  -i --input              input data from a file (e.g. "test.bin")
  -o --output             output data to a file (e.g. "results.bin")
  -H --cpha               clock phase (default 0)
  -O --cpol               clock polarity (default 0)
  -v --verbose            Verbose (show tx buffer) (default OFF)
  -p                      Send data (e.g. "1234\xde\xad")
  -l --size               transfer size/length of buf to transfer

Examples:
Execute SLAVE full duplex TX/RX test with 64 bytes as transfer length 'rtk_spi_test -R SLAVE -T 0 -l 64'
Execute MASTER full duplex TX/RX test with 64 bytes as transfe length 'rtk_spi_test -R MASTER -T 0 -l 64'
For more examples, please refer to spi_test_script.sh included in spi tests src code.

~ #

Please find enclosed a helper bash script spi_test_script.sh which implements
common test cases for testing as well as for reference. It can execute below test
cases which are most common test cases (it internally calls rtk_spi_test with
appropriate arguments/options).

~ # sh spi_test_script.sh
Pinmux Workaround Warning: Forcing SPI1 CS/MISO/MOSI pins to SPI function
42008A08: 00023A03
42008A10: 00021103
42008A14: 00021103

---------------SINGLE BOARD TEST AVAILABLE TEST CASES --------------------------
1. 32 bytes Slave RX <--> Master TX interrupt mode transfer
2. 64 bytes Slave RX <--> Master TX interrupt mode transfer
3. 1K bytes Slave RX <--> Master TX DMA mode transfer
4. 32 bytes Slave TX <--> Master RX interrupt mode transfer
5. 64 bytes Slave TX <--> Master RX interrupt mode transfer
6. 1K bytes Slave TX <--> Master RX DMA mode transfer
7. 32 bytes FULL DUPLEX Slave TX/RX <--> Master TX/RX interrupt mode transfer
8. 64 bytes FULL DUPLEX Slave TX/RX <--> Master TX/RX interrupt mode transfer
9. 1K bytes FULL DUPLEX Slave TX/RX <--> Master TX/RX DMA mode transfer
----------------------------------------------------------------------------------

---------------TWO BOARDS TEST AVAILABLE TEST CASES ----------------------------
10. 32 bytes Slave RX interrupt mode transfer
11. 32 bytes Master TX interrupt mode transfer
12. 64 bytes Slave RX interrupt mode transfer
13. 64 bytes Master TX interrupt mode transfer
14. 1K bytes Slave RX DMA mode transfer
15. 1K bytes Master TX DMA mode transfer
16. 32 bytes Slave TX interrupt mode transfer
17. 32 bytes Master RX interrupt mode transfer
18. 64 bytes Slave TX interrupt mode transfer
19. 64 bytes Master RX interrupt mode transfer
20. 1K bytes Slave TX DMA mode transfer
21. 1K bytes Master RX DMA mode transfer
22. 32 bytes FULL DUPLEX Slave TX/RX interrupt mode transfer
23. 32 bytes FULL DUPLEX Master TX/RX interrupt mode transfer
24. 64 bytes FULL DUPLEX Slave TX/RX interrupt mode transfer
25. 64 bytes FULL DUPLEX Master TX/RX interrupt mode transfer
26. 1K bytes FULL DUPLEX Slave TX/RX DMA mode transfer
27. 1K bytes FULL DUPLEX Master TX/RX DMA mode transfer
----------------------------------------------------------------------------------

Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:



[Results on single board test]

Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
1
(Slave RX <--> Master TX) 32 bytes transfer - Interrupt
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: RX_ONLY
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: TX_ONLY
SPI-SLAVE: 32 bytes transfer success !
~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
2
(Slave RX <--> Master TX) 64 bytes transfer - Interrupt
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: RX_ONLY
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: TX_ONLY
total: tx 0.1KB, rx 0.1KB
SPI-SLAVE 64 bytes transfer success !
total: tx 0.1KB, rx 0.1KB

~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
3
(Slave RX <--> Master TX) 1024 bytes transfer - DMA
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: RX_ONLY
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: TX_ONLY
total: tx 1.0KB, rx 1.0KB
SPI-SLAVE 1024 bytes transfer success !
total: tx 1.0KB, rx 1.0KB

~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
4
(Slave TX <--> Master RX) 32 bytes transfer - Interrupt
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: TX_ONLY
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: RX_ONLY
SPI-MASTER: 32 bytes transfer success !


~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
5
(Slave TX <--> Master RX) 64 bytes transfer - Interrupt
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: TX_ONLY
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: RX_ONLY
SPI-MASTER 64 bytes transfer success !
total: tx 0.1KB, rx 0.1KB
total: tx 0.1KB, rx 0.1KB

~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
6
(Slave TX <--> Master RX) 1024 bytes transfer - DMA
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: TX_ONLY
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 1000000 Hz (1000 KHz)
transfer_type: RX_ONLY
SPI-MASTER 1024 bytes transfer success !
total: tx 1.0KB, rx 1.0KB
total: tx 1.0KB, rx 1.0KB


~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
7
(Full duplex) 32 bytes transfer - Interrupt
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: FULL DUPLEX
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: FULL DUPLEX
SPI-MASTER: 32 bytes transfer success !
SPI-SLAVE: 32 bytes transfer success !
~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
8
(Full duplex) 64 bytes transfer - Interrupt
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: FULL DUPLEX
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: FULL DUPLEX
SPI-MASTER 64 bytes transfer success !
total: tx 0.1KB, rx 0.1KB
SPI-SLAVE 64 bytes transfer success !
total: tx 0.1KB, rx 0.1KB
~ #
~ #
Please select the test case number 1~9 for single board test OR 10~27 for two board test from above test cases:
9
(Full duplex) 1024 bytes transfer - DMA
SPI Slave test with device /dev/spidev1.0
spi mode: 0x0
bits per word: 8
max speed: 7000000 Hz (7000 KHz)
transfer_type: FULL DUPLEX
SPI Master test with device /dev/spidev0.0
spi mode: 0x0
bits per word: 8
max speed: 1000000 Hz (1000 KHz)
transfer_type: FULL DUPLEX
SPI-MASTER 1024 bytes transfer success !
total: tx 1.0KB, rx 1.0KB
SPI-SLAVE 1024 bytes transfer success !
total: tx 1.0KB, rx 1.0KB

~ #
~ #
