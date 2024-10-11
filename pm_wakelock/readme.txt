[Description]

User-space test case for Linux wakelock usage. It will create 2 wakelocks and enable autosleep, then release all 
wakelocks and system enter sleep mode.

[HW Setup]

[SW Setup]

[Parameters]

[Result]

	# rtk_pm_wakelock_test
	 Create first wake_lock: wakelock1
	 Create second wake_lock: wakelock2
	 Enable autosleep, sleep level: standby
	 Should not sleep now because of wakelock1 and wakelock2
	 Release wakelock1 here. Should not sleep because wakelock2 is not released
	 Release wakelock2 here. Should sleep because all wakelocks are released
	 PM: suspend entry (deep)
	 Filesystems sync: 0.003 seconds
	 root@rtl8730elh-va8:~# 
	 root@rtl8730elh-va8:~# Freezing user space processes ... (elapsed 0.003 seconds) done.
	 OOM killer disabled.
	 Freezing remaining freezable tasks ... (elapsed 0.001 seconds) done.
	 printk: Suspending console(s) (use no_console_suspend to debug)

