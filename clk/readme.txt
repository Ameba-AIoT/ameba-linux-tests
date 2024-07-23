[Description]

Test case for Realtek Ameba RCC driver, implemented as kernel module.
    
[HW Setup]

None

[SW Setup]

Lunch with tests option.

[Parameters]

Adjust the clocks under test as required:
	rcc-test {
		compatible = "realtek,ameba-rcc-test";
		clocks = <&rcc RTK_CKE_ZGB>,
			<&rcc RTK_CKE_IRDA>,
			<&rcc RTK_CKD_PSRAM>,
			<&rcc RTK_CKSL_CTC>,
			<&clk_osc_131khz>;
		clock-names = "cke_zgb","cke_irda","ckd_psram","cksl_ctc","osc_131k";
	};

[Result]

$ insmod /lib/modules/5.4.63/kmod-rcc-test.ko
...
[RCC TEST] Pass