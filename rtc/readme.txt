[Description]

User-space test case for Realtek Ameba RTC driver.
    
[HW Setup]

None

[SW Setup]

Lunch with tests option.

[Parameters]

None

[Result]

	# rtk_rtc_test
	Current RTC date/time is 1-1-1970 0:0:6
	Set RTC date/time to 1-1-1970 0:10:6
	Set alarm time to 0:10:16
	Wait for 10 seconds for alarm...
	RTC alarm triggered
	RTC test PASS