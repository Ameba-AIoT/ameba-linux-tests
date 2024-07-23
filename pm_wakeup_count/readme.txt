[Description]

User-space test case for Linux wakeup_count usage. Wakeup_count is synchronization mechanism with kernel PM subsystem. 
User should check it before enter sleep mode. 

[HW Setup]

[SW Setup]

[Parameters]

[Result]

	 # rtk_pm_wakeup_count_test
	 Wakeup_count is 1
     Write it back now
	 Write success. Ready to enter sleep mode
	 PM: suspend entry (deep)
	 Filesystems sync: 0.003 seconds
	 Freezing user space processes ... (elapsed 0.001 seconds) done.
     OOM killer disabled.
     Freezing remaining freezable tasks ... (elapsed 0.002 seconds) done.
     printk: Suspending console(s) (use no_console_suspend to debug)
