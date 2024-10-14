// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek GPIO test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/io.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/pm_wakeirq.h>

#define RTK_DEV_NAME_MAX_LEN		50
static int test_case = 3;

module_param(test_case, int, S_IRUGO);

struct rtk_gpio_test_data {
	char dev_name[RTK_DEV_NAME_MAX_LEN];
	struct completion done;
	int index;
	int in_index;
	int out_index;

	const char *db_enable;
	int db_div_cnt;
};

struct rtk_gtest_case {
	const char *name;
	int (*func)(void);
	int trigger;
};

static struct rtk_gpio_test_data *rtk_gtest_data;

static irqreturn_t rtk_gpio_irq_handler(int irq, void *dev_id)
{
	pr_info("GPIO interrupt IRQ:%d\n", irq);
	disable_irq_nosync(irq);
	complete(&rtk_gtest_data->done);
	enable_irq(irq);

	return IRQ_HANDLED;
}

static int rtk_gpio_set_debounce(void)
{
	int gpio_index;
	int ret;

	gpio_index = rtk_gtest_data->index;
	if (rtk_gtest_data->db_enable && strcmp(rtk_gtest_data->db_enable, "enable") == 0) {
		pr_info("Set GPIO debounce db_div_cnt: %d\n", rtk_gtest_data->db_div_cnt);
		ret = gpiod_set_debounce(gpio_to_desc(gpio_index), rtk_gtest_data->db_div_cnt);
		if (ret) {
			pr_err("Error: Failed to set GPIO debounce: %d\n", ret);
			return -EINVAL;
		}
	}
	return 0;
}

static int rtk_gpio_irq_test(int int_idx);

static int rtk_gpio_request_high_irq(void)
{
	return rtk_gpio_irq_test(0);
}

static int rtk_gpio_request_low_irq(void)
{
	return rtk_gpio_irq_test(1);
}

static int rtk_gpio_request_fall_irq(void)
{
	return rtk_gpio_irq_test(2);
}

static int rtk_gpio_request_rise_irq(void)
{
	return rtk_gpio_irq_test(3);
}

static int rtk_gpio_request_both_irq(void)
{
	return rtk_gpio_irq_test(4);
}

static int rtk_gpio_re_request(void)
{
	int gpio_index;
	int req_status;

	gpio_index = rtk_gtest_data->index;

	pr_info("Request GPIO test start\n");
	req_status = gpio_request(gpio_index, NULL);
	if (req_status != 0) {
		pr_err("Error: The first time to request GPIO failed\n");
		return -EINVAL;
	}
	pr_info("First time GPIO request success\n");

	req_status = gpio_request(gpio_index, NULL);
	if (!req_status) {
		pr_info("Error: Repeat request is unpermitted\n");
		gpio_free(gpio_index);
		return -EINVAL;
	}
	pr_info("Test success: for repeat request is unpermitted\n");

	gpio_free(gpio_index);
	return 0;
}

static int rtk_gpio_output_test(void)
{
	int gpio_index;
	int req_status;
	int set_direct_status;
	int val;
	int i;

	gpio_index = rtk_gtest_data->index;

	pr_info("GPIO output test start\n");
	req_status = gpio_request(gpio_index, NULL);
	if (req_status != 0) {
		pr_err("Error: Failed to request GPIO %d\n", gpio_index);
		return -EINVAL;
	}

	set_direct_status = gpio_direction_output(gpio_index, 0);
	if (IS_ERR_VALUE(set_direct_status)) {
		pr_err("Error: Failed to set GPIO direction output\n");
		goto fail;
	}

	val = gpio_get_value(gpio_index);
	pr_info("GPIO get data value: %d\n", val);

	gpio_set_value(gpio_index, 1);
	val = gpio_get_value(gpio_index);
	pr_info("GPIO get data value: %d\n", val);

	if (val != 1) {
		pr_err("Error: Failed to set GPIO data value = 1\n");
		goto fail;
	}

	gpio_set_value(gpio_index, 0);
	val = gpio_get_value(gpio_index);
	pr_info("GPIO get data value: %d\n", val);
	if (val != 0) {
		pr_err("Error: Failed to set GPIO data value = 0\n");
		goto fail;
	}

	for (i = 0; i < 100; i++) {
		gpio_set_value(gpio_index, 1);
		udelay(200);
		gpio_set_value(gpio_index, 0);
		udelay(200);
	}

	gpio_free(gpio_index);
	pr_info("GPIO output test success\n");
	return 0;

fail:
	pr_err("Error: Failed to test GPIO output\n");
	gpio_free(gpio_index);
	return -EINVAL;
}

static int rtk_gpio_input_test(void)
{
	int gpio_index;
	int req_status;
	int set_direct_status;
	int val;
	int i;

	gpio_index = rtk_gtest_data->index;

	pr_info("GPIO input test start\n");
	req_status = gpio_request(gpio_index, NULL);
	if (req_status != 0) {
		pr_err("Error: Failed to request GPIO %d\n", gpio_index);
		return -EINVAL;
	}

	set_direct_status = gpio_direction_input(gpio_index);
	if (IS_ERR_VALUE(set_direct_status)) {
		pr_err("Error: Failed to set GPIO direction input\n");
		goto fail;
	}

	for (i = 0; i < 10; i++) {
		val = gpio_get_value(gpio_index);
		pr_info("Get GPIO data value: %d\n", val);
		udelay(500);
	}

	gpio_free(gpio_index);
	pr_info("GPIO input test success\n");
	return 0;

fail:
	pr_err("Error: Failed to test GPIO input\n");
	gpio_free(gpio_index);
	return -EINVAL;
}

static int rtk_gpio_inout_test(void)
{
	int in_index;
	int out_index;
	int ret = 0;
	int set_value = 0;
	int value;
	int i;

	in_index = rtk_gtest_data->in_index;
	out_index = rtk_gtest_data->out_index;

	pr_info("GPIO in-out test start\n");

	ret = gpio_request(in_index, NULL);
	if (ret != 0) {
		pr_err("Error: Failed to request input GPIO %d\n", in_index);
		goto fail_exit;
	}

	ret = gpio_direction_input(in_index);
	if (IS_ERR_VALUE(ret)) {
		pr_err("Error: Failed to set input direction\n");
		goto fail_in;
	}

	ret = gpio_request(out_index, NULL);
	if (ret != 0) {
		pr_err("Error: Failed to request output GPIO %d\n", out_index);
		goto fail_in;
	}

	ret = gpio_direction_output(out_index, set_value);
	if (IS_ERR_VALUE(ret)) {
		pr_err("Error: Failed to set output direction\n");
		goto fail_out;
	}

	for (i = 0; i < 10; i++) {
		set_value = !set_value;
		gpio_set_value(out_index, set_value);
		udelay(200);
		value = gpio_get_value(in_index);
		if (value != set_value) {
			ret = -EINVAL;
			pr_err("Error: GPIO check fail: out = %d, in = %d\n", set_value, value);
			goto fail_out;
		}
	}

fail_out:
	gpio_free(out_index);
fail_in:
	gpio_free(in_index);
fail_exit:
	return ret;
}

static struct rtk_gtest_case rtk_test_case[] = {
	{"rtk_gpio_high_irq", rtk_gpio_request_high_irq, IRQF_TRIGGER_HIGH},
	{"rtk_gpio_low_irq", rtk_gpio_request_low_irq, IRQF_TRIGGER_LOW},
	{"rtk_gpio_fall_irq", rtk_gpio_request_fall_irq, IRQF_TRIGGER_FALLING},
	{"rtk_gpio_rise_irq", rtk_gpio_request_rise_irq, IRQF_TRIGGER_RISING},
	{"rtk_gpio_both_irq", rtk_gpio_request_both_irq, (IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING)},
	{"rtk_gpio_re_request", rtk_gpio_re_request},
	{"rtk_gpio_output_test", rtk_gpio_output_test},
	{"rtk_gpio_input_test", rtk_gpio_input_test},
	{"rtk_gpio_inout_test", rtk_gpio_inout_test},
};

static int rtk_gpio_irq_test(int int_idx)
{
	int ret;
	int virq = gpio_to_irq(rtk_gtest_data->index);
	int i = 0;

	reinit_completion(&rtk_gtest_data->done);

	ret = gpio_request(rtk_gtest_data->index, NULL);
	if (ret != 0) {
		pr_err("Error: Failed to request GPIO %d\n", rtk_gtest_data->index);
		return -EINVAL;
	}

	ret = request_irq(virq, rtk_gpio_irq_handler, rtk_test_case[int_idx].trigger, "gpio_irq", NULL);
	if (IS_ERR_VALUE(ret)) {
		pr_err("Error: Failed to request IRQ\n");
		goto fail_gpio;
	}

	ret = rtk_gpio_set_debounce();
	if (ret != 0) {
		pr_err("Error: Failed to set GPIO debounce\n");
		goto faill_irq;
	}

	for (i; i < 5; i++) {
		ret = wait_for_completion_timeout(&rtk_gtest_data->done, 60 * HZ);
		if (ret == 0) {
			pr_err("Error: Wait for IRQ timeout\n");
			goto faill_irq;
		}
		pr_info("%s[%d] success, ret=%d\n", rtk_test_case[int_idx].name, i, ret);
	}

	free_irq(virq, NULL);
	gpio_free(rtk_gtest_data->index);

	pr_info("%s success\n", rtk_test_case[int_idx].name);

	return 0;

faill_irq:
	free_irq(virq, NULL);
fail_gpio:
	pr_err("Error: %s test fail\n", rtk_test_case[int_idx].name);
	gpio_free(rtk_gtest_data->index);
	return -EINVAL;
}

static int rtk_gpio_test_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int gpio_in;
	int gpio_out;
	int ret;

	if (np == NULL) {
		pr_err("Error: Invalid gpio-test device node, please check the DTS file\n");
		return -ENODEV;
	}

	gpio_in = of_get_named_gpio(np, "rtk,test-gpios", 0);
	if (!gpio_is_valid(gpio_in)) {
		pr_err("Error: Failed to get test-gpios <0> property\n");
		return -ENODEV;
	}

	pr_info("GPIO in: %d\n", gpio_in);

	gpio_out = of_get_named_gpio(np, "rtk,test-gpios", 1);
	if (!gpio_is_valid(gpio_out)) {
		pr_err("Error: Failed to Fail to get test-gpios <1> property\n");
		return -ENODEV;
	}

	pr_info("GPIO out: %d\n", gpio_out);

	rtk_gtest_data = devm_kzalloc(&pdev->dev, sizeof(struct rtk_gpio_test_data), GFP_KERNEL);
	if (!rtk_gtest_data) {
		return -ENOMEM;
	}

	rtk_gtest_data->index = gpio_in;
	rtk_gtest_data->in_index = gpio_in;
	rtk_gtest_data->out_index = gpio_out;
	of_property_read_string(np, "rtk,db_enable", &rtk_gtest_data->db_enable);
	of_property_read_u32(np, "rtk,db_div_cnt", &rtk_gtest_data->db_div_cnt);

	strlcpy(rtk_gtest_data->dev_name, "gpio-dev", RTK_DEV_NAME_MAX_LEN);

	init_completion(&rtk_gtest_data->done);

	dev_set_name(&pdev->dev, "gpio-dev");

	platform_set_drvdata(pdev, rtk_gtest_data);

	pr_info("GPIO test: %s\n", rtk_test_case[test_case].name);

	if ((test_case <= 4) && (of_property_read_bool(pdev->dev.of_node, "wakeup-source"))) {
		device_init_wakeup(&pdev->dev, true);
		dev_pm_set_wake_irq(&pdev->dev, gpio_to_irq(rtk_gtest_data->index));
	}
	ret = rtk_test_case[test_case].func();
	pr_info("GPIO test %s\n", (ret == 0) ? "pass" : "fail");

	return 0;
}

static int rtk_gpio_test_remove(struct platform_device *pdev)
{
	if (test_case <= 4) {
		if (of_property_read_bool(pdev->dev.of_node, "wakeup-source")) {
			dev_pm_clear_wake_irq(&pdev->dev);
			device_init_wakeup(&pdev->dev, false);
		}
	}

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static const struct of_device_id rtk_gpio_test_match[] = {
	{ .compatible = "realtek,ameba-gpio-test"},
	{}
};

static struct platform_driver rtk_gpio_test_driver = {
	.probe = rtk_gpio_test_probe,
	.remove	= rtk_gpio_test_remove,
	.driver = {
		.name = "gpio-dev",
		.owner = THIS_MODULE,
		.of_match_table = rtk_gpio_test_match,
	},
};

static int __init rtk_gpio_test_init(void)
{
	int ret;

	ret = platform_driver_register(&rtk_gpio_test_driver);
	if (IS_ERR_VALUE(ret)) {
		pr_err("Error: Failed to register GPIO test driver\n");
		return -EINVAL;
	}

	return 0;
}

static void __exit rtk_gpio_test_exit(void)
{
	platform_driver_unregister(&rtk_gpio_test_driver);
}

late_initcall(rtk_gpio_test_init);
module_exit(rtk_gpio_test_exit);

MODULE_DESCRIPTION("Realtek Ameba GPIO test module");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
