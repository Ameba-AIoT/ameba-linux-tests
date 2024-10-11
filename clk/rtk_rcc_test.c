// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek RCC test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of_device.h>

static struct clk *clk_gate;
static struct clk *clk_gate_fen;
static struct clk *clk_div;
static struct clk *clk_mux;
static struct clk *clk_mux_parent;

static int rtk_rcc_test_probe(struct platform_device *pdev)
{
	int ret = 0;
	unsigned long rate = 0;

	pr_info("[RCC TEST] Start...\n");
	
	pr_info("[RCC TEST] Gate clock test start...\n");
  
	pr_info("[RCC TEST] Get clock\n");
	
  	clk_gate = devm_clk_get(&pdev->dev, "cke_zgb");
	if (IS_ERR(clk_gate)) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to get clock\n");
		ret = -1;
		goto fail;
	}

	pr_info("[RCC TEST] Enable clock\n");

	ret = clk_prepare_enable(clk_gate);
	if (ret < 0) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to enable clock %d\n", ret);
		goto fail;
	}
	
	pr_info("[RCC TEST] Gate clock test done\n");
	
	pr_info("[RCC TEST] Gate clock with fen test start...\n");
  
	pr_info("[RCC TEST] Get clock\n");
	
  	clk_gate_fen = devm_clk_get(&pdev->dev, "cke_irda");
	if (IS_ERR(clk_gate_fen)) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to get clock\n");
		ret = -1;
		goto fail;
	}

	pr_info("[RCC TEST] Enable clock\n");

	ret = clk_prepare_enable(clk_gate_fen);
	if (ret < 0) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to enable clock %d\n", ret);
		goto fail;
	}
	
	pr_info("[RCC TEST] Gate clock with fen test done\n");
	
	pr_info("[RCC TEST] Mux clock test start...\n");
  
	pr_info("[RCC TEST] Get clock\n");
	
  	clk_mux = devm_clk_get(&pdev->dev, "cksl_ctc");
	if (IS_ERR(clk_mux)) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to get clock\n");
		ret = -1;
		goto fail;
	}
  
	pr_info("[RCC TEST] Get parent clock\n");
	
  	clk_mux_parent = devm_clk_get(&pdev->dev, "osc_131k");
	if (IS_ERR(clk_mux_parent)) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to get parent clock\n");
		ret = -1;
		goto fail;
	}

	pr_info("[RCC TEST] Set parent clock\n");
	
	clk_set_parent(clk_mux, clk_mux_parent);
	
	rate = clk_get_rate(clk_mux);
	
	pr_info("[RCC TEST] Get clock rate=%ld\n", rate);
	
	pr_info("[RCC TEST] Mux clock test done\n");

	pr_info("[RCC TEST] Divider clock test start...\n");
  
	pr_info("[RCC TEST] Get clock\n");
	
  	clk_div = devm_clk_get(&pdev->dev, "ckd_psram");
	if (IS_ERR(clk_div)) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to get clock\n");
		ret = -1;
		goto fail;
	}

	pr_info("[RCC TEST] Set clock rate to 220MHz\n");
	
	rate = clk_set_rate(clk_div, 220000000);
	if (ret < 0) {
		dev_err(&pdev->dev, "[RCC TEST] Fail to enable clock %d\n", ret);
		goto fail;
	}
	
	rate = clk_get_rate(clk_div);
	
	pr_info("[RCC TEST] Get clock rate=%ld\n", rate);

	pr_info("[RCC TEST] Divider clock test done\n");

	pr_info("[RCC TEST] Done\n");
	
	pr_info("[RCC TEST] PASS\n");
	
	return 0;

fail:
	pr_info("[RCC TEST] FAIL\n");
	return 0;
}

static int rtk_rcc_test_remove(struct platform_device *pdev)
{
	pr_info("[RCC TEST] Remove...\n");

	pr_info("[RCC TEST] Disable clocks\n");
	clk_disable_unprepare(clk_gate);
	clk_disable_unprepare(clk_gate_fen);

	pr_info("[RCC TEST] Put clocks\n");
	devm_clk_put(&pdev->dev, clk_gate);
	devm_clk_put(&pdev->dev, clk_gate_fen);
	devm_clk_put(&pdev->dev, clk_mux);
	devm_clk_put(&pdev->dev, clk_mux_parent);
	devm_clk_put(&pdev->dev, clk_div);
	
	pr_info("[RCC TEST] Exit\n");
	
	return 0;
}

static const struct of_device_id rtk_rcc_test_match[] = {
	{.compatible = "realtek,ameba-rcc-test",},
	{},
};
MODULE_DEVICE_TABLE(of, rtk_rcc_test_match);

static struct platform_driver rtk_rcc_test_driver = {
	.probe	= rtk_rcc_test_probe,
	.remove	= rtk_rcc_test_remove,
	.driver = {
		.name = "rtk-rcc-test",
		.of_match_table = of_match_ptr(rtk_rcc_test_match),
	},
};

static int rtk_rcc_test_init(void)
{
	platform_driver_register(&rtk_rcc_test_driver);
	
	return 0;
}

static void rtk_rcc_test_exit(void)
{
	platform_driver_unregister(&rtk_rcc_test_driver);
}

late_initcall(rtk_rcc_test_init);
module_exit(rtk_rcc_test_exit);

MODULE_LICENSE("GPL");

