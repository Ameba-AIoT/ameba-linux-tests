[Description]

Test case for Realtek Ameba Watchdog driver.

[HW Setup]

None

[SW Setup]

Adjust the configuration in top DTS as required:

	&watchdog {
		rtk,wdg-interrupt-mode = <0>;
		status = "okay";
	};

[NOTE]: If rtk,wdg-interrupt-mode = 0, watchdog driver will not feed dog by irq-early. Watchdog can only be fed by userspace.
		If rtk,wdg-interrupt-mode = 1, watchdog can be fed by driver self, according to irq-early. System will not reboot event if userspace stop feeding dog.

[Parameters]

None

[Result]

# rtk_watchdog_test
Watchdog test started
Watchdog timeout: 30s
# Feed watchdog. 27s passed.
Feed watchdog. 27s passed.
Stop feeding watchdog, expected to reboot in 30s
To reboot, please ensure rtk,wdg-interrupt-mode = <0>
0s passed.
1s passed.
2s passed.
3s passed.
4s passed.
5s passed.
6s passed.
7s passed.
8s passed.
9s passed.
10s passed.
11s passed.
12s passed.
13s passed.
14s passed.
15s passed.
16s passed.
17s passed.
18s passed.
19s passed.
20s passed.
21s passed.
22s passed.
23s passed.
24s passed.
25s passed.
26s passed.
27s passed.
28s passed.
29s passed.
ROM:[V1.1]
FLASHRATE:1
BOOT FROM NAND
IMG1(OTA1) VALID, ret: 0
<reboot happened>