[Description]

User-space test case for Realtek Ameba PWM driver.

[HW Setup]

Connect the PWM pin(s) under test to LA:

[SW Setup]

1. Enable the pinmux group for PWM in DTS for tests.
2. Lunch with tests option.

[Parameters]

Adjust the PWM pins in DTS, avoid the pinmux conflicts.
	pwm_pins: pwm@0 {
		pins1 {
			pinmux = <REALTEK_PINMUX('B', 14, PWM)>, // HS_PWM0
				 <REALTEK_PINMUX('B', 15, PWM)>,     // HS_PWM1
				 <REALTEK_PINMUX('B', 16, PWM)>,     // HS_PWM2
				 <REALTEK_PINMUX('A', 11, PWM)>,     // HS_PWM3
				 <REALTEK_PINMUX('A', 12, PWM)>;     // HS_PWM4
			bias-pull-up;
			slew-rate = <0>;
			drive-strength = <0>;
		};
		pins2 {
			pinmux = <REALTEK_PINMUX('A', 13, PWM)>;  // HS_PWM5
			bias-pull-up;
			swd-disable;
			slew-rate = <0>;
			drive-strength = <0>;
		};
	};

[Result]

	# rtk_pwm_test 3 20000000 10000000
	Open PWM sysfs node
	Create PWM3 sysfs nodes
	Set period to 20000000ns
	Set duty cycle to 8000000ns/20000000ns
	Enable PWM3
	Please check the PWM3 output via LA
	Stop PWM3 output
	PWM test done

For above example, PWM channel 3 shall output waveform with 20ms perido and 10/20=50% duty cycle,
check the output waveform with LA.