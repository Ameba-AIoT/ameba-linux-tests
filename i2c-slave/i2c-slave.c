// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek I2C Slave test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>
#include <linux/slab.h>

#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/of.h>

/* Can include directly in kernel space drivers. */
#include "../../linux/drivers/i2c/i2c-core.h"

#if IS_ENABLED(CONFIG_I2C_SLAVE)
#define TEST_READ_DONE			1
#define TEST_WRITE_DONE			2

static int recv_index = 0;
static int send_index = 0;

/* INPUT: insmod /lib/modules/kmod-i2c-slave-test.ko addr10bit=1 verbose=1 */
static int addr10bit = 0;
static int verbose = 1;
module_param(addr10bit, int, S_IRUSR);
module_param(verbose, int, S_IRUSR);

static int i2c_slave_callback_example(
	struct i2c_client *slave, enum i2c_slave_event slave_event, u8 *data)
{
	pr_debug("--- This is an example of i2c slave callback. ---");

	if (slave_event == I2C_SLAVE_READ_REQUESTED) {
		/* Data from slave to master. Requst. */
		send_index = 0;
		*data = 0; // the first byte.
		if (verbose) {
			pr_info(KERN_CONT "Slave give data to master:\n%02X ", *data);
		}
	} else if (slave_event == I2C_SLAVE_READ_PROCESSED) {
		/* Data from slave to master. Send END. */
		send_index++;
		*data = send_index; // generate some data.
		if (verbose) {
			if ((send_index % 16) == 15) {
				pr_info(KERN_CONT "%02X        | total: %d bytes.\n", *data, send_index + 1);
			} else {
				pr_info(KERN_CONT "%02X ", *data);
			}
		}
	} else if (slave_event == I2C_SLAVE_WRITE_REQUESTED) {
		/* Data from master to slave. Requst. */
		recv_index = 0;
		if (verbose) {
			pr_info("Slave received master data:\n");
		}
	} else if (slave_event == I2C_SLAVE_WRITE_RECEIVED) {
		/* Data from master to slave. Read. */
		if (verbose) {
			if ((recv_index % 16) == 15) {
				pr_info(KERN_CONT "%02X        | total: %d bytes.\n", *data, recv_index + 1);
			} else {
				pr_info(KERN_CONT "%02X ", *data);
			}
		}
		recv_index++;
	} else if (slave_event == I2C_SLAVE_STOP) {
		/* Current trasfer stop. */
		send_index = 0;
		recv_index = 0;
	}

	return 0;
}

int rtk_i2c_slave_test_probe(struct i2c_client *client)
{
	int ret = 0;

	struct i2c_client *slave_test = client;

	pr_info("--- rtk_i2c_slave_test_probe ---");

	pr_info("\n!!!!!!!!!!!!!! Start I2C Slave Test !!!!!!!!!!!!!!\n ");
	pr_info("I2C Slave Address Registered: 0x%02x %s\n", client->addr, addr10bit ? "10-bit mode" : "7-bit mode");

	/* Use extend flags to reg slave: read only.*/
	pr_info("test: i2c slave regist.");
	slave_test->flags = addr10bit ? (I2C_CLIENT_TEN | I2C_CLIENT_SLAVE) : I2C_CLIENT_SLAVE;
	slave_test->addr = client->addr;
	slave_test->dev = client->dev;
	slave_test->adapter = client->adapter;
	slave_test->slave_cb = i2c_slave_callback_example;
	i2c_slave_register(slave_test, slave_test->slave_cb);
	pr_info("i2c test slave register done.");
	return ret;
}

static void rtk_i2c_slave_test_remove(struct i2c_client *client)
{
	pr_info("--- rtk_i2c_slave_test_remove ---");
	i2c_slave_unregister(client);
}

#else // IS_ENABLED(CONFIG_I2C_SLAVE)
static int rtk_i2c_slave_test_probe(struct i2c_client *client)
{
	pr_info(" Please open the macro I2C_SLAVE_REALTEK first.");
	return -ENODEV;
}

static void rtk_i2c_slave_test_remove(struct i2c_client *client)
{
	pr_info(" Please open the macro I2C_SLAVE_REALTEK first.");
}
#endif // IS_ENABLED(CONFIG_I2C_SLAVE)

static const struct of_device_id rtk_i2c_slave_test_match[] = {
	{.compatible = "realtek,ameba-i2c-slave-test",},
	{},
};
MODULE_DEVICE_TABLE(of, rtk_i2c_slave_test_match);

static struct i2c_driver rtk_i2c_slave_test_driver = {
	.probe	= rtk_i2c_slave_test_probe,
	.remove	= rtk_i2c_slave_test_remove,
	.driver = {
		.name = "realtek,ameba-rtk-i2c",
		.owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(rtk_i2c_slave_test_match),
	},
};

module_i2c_driver(rtk_i2c_slave_test_driver);

MODULE_DESCRIPTION("Realtek Ameba I2C Slave test module");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
