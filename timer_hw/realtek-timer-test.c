// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Crypto test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/scatterlist.h>

#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timekeeping.h>

#include <linux/mfd/rtk-timer.h>

static int start_time = 0;
static int end_time = 0;
static int set_time = 0;
static int timer_idx = 0;

/* INPUT: insmod /lib/modules/kmod-timer-test.ko set_timer_ms=1 */
static int set_timer_ms = 0;
static int set_timer_us = 0;
module_param(set_timer_ms, int, S_IRUSR);
module_param(set_timer_us, int, S_IRUSR);

long get_system_time_nanosecond(void)
{
	struct timespec timestamp = {};

	getnstimeofday(&timestamp);
	return timestamp.tv_sec * 1000000000 + timestamp.tv_nsec;
}

static int rtk_timer_test_callback(void *id)
{
	int result = 0;

	rtk_gtimer_int_clear(timer_idx);
	rtk_gtimer_start(timer_idx, 0);
	end_time = get_system_time_nanosecond();

	result = end_time - start_time;
	pr_info("Test PASS. Sytem time passed %dns.\n", result);
	pr_info("\n");

	return 0;
}

static int rtk_timer_test_init(void)
{
	/* Unify timer unit. */
	if (set_timer_ms) {
		set_time = set_timer_ms * 1000000;
		pr_info("Hardware timer test. Set timer for %dms.\n", set_timer_ms);
	} else if (set_timer_us) {
		set_time = set_timer_us * 1000;
		if (set_time < 200000) {
			pr_info("Software cannot test timer less than 200us.");
			return -EINVAL;
		}
		pr_info("Hardware timer test. Set timer for %dus.\n", set_timer_us);
	} else {
		/* Software cannot test ns-level timer. */
		pr_info("Please set timer!\n");
		return -EINVAL;
	}

	timer_idx = rtk_gtimer_dynamic_init(set_time, rtk_timer_test_callback, NULL);

	if (timer_idx < 0) {
		pr_info("No enough timers to use. errno: %d.\n", timer_idx);
		return -EINVAL;
	}
	pr_info("Requested timer index is %d.\n", timer_idx);

	rtk_gtimer_change_period(timer_idx, set_time);
	rtk_gtimer_int_config(timer_idx, 1);

	rtk_gtimer_start(timer_idx, 1);
	start_time = get_system_time_nanosecond();

	return 0;
}

static void rtk_timer_test_exit(void)
{
	pr_info("Timer test exit: Deint gtimer idx %d.\n", timer_idx);
	set_timer_ms = 0;
	set_timer_us = 0;
	rtk_gtimer_int_clear(timer_idx);
	rtk_gtimer_deinit(timer_idx);
	timer_idx = 0;
}

late_initcall(rtk_timer_test_init);
module_exit(rtk_timer_test_exit);

MODULE_LICENSE("GPL");

