// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek Hello test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int hello_init(void)
{
  pr_info("Hello world\n");
  return 0;
}

static void hello_exit(void)
{
  pr_info("Bye world\n");
}

late_initcall(hello_init);
module_exit(hello_exit);

MODULE_DESCRIPTION("Realtek Ameba Hello test module");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
