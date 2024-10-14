// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek DMAC test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <dt-bindings/realtek/dmac/realtek-ameba-dmac.h>
#include "rtk_dmac_test.h"

#define RTK_DMAC_TEST_TODO	0

#if RTK_DMAC_TEST_TODO
static u8 dma_memcpy_status = RTK_DMAC_TEST_SUCCESS;
#endif // RTK_DMAC_TEST_TODO

static u8 dma_cyclic_status = RTK_DMAC_TEST_SUCCESS;

void buf_print(u32 *buf, u32 buflen)
{
	u32 i;

	pr_info("\n");
	for (i = 0; i < buflen / 4; i++) {
		pr_info("offset [0x%x]: 0x%x ", i * 4, *(buf + i));
	}
	pr_info("\n\n");
}

bool buf_compare(u32 *bufa, u32 *bufb, u32 compare_len)
{
	u32 i;
	for (i = 0; i < compare_len / 4; i++){
		if (*(bufa + i) != *(bufb + i)){
			pr_info("buf_print: src\n");
			buf_print(bufa, compare_len);
			pr_info("buf_print: dst\n");
			buf_print(bufb, compare_len);
			return 0;
		}
	}
	return 1;
}

void rtk_dmac_test_end(
	struct device *test_dev,
	struct rtk_dma_test_addr *test_addr,
	struct dma_chan *chan)
{
	dma_release_channel(chan);

	dma_free_coherent(test_dev, test_addr->mem_size_src,\
		test_addr->buf_src, test_addr->dma_handle_src);

	dma_free_coherent(test_dev, test_addr->mem_size_dst,\
		test_addr->buf_dst, test_addr->dma_handle_dst);
}

void rtk_prepare_test_buf(
	struct device *test_dev,
	struct rtk_dma_test_addr *test_addr,
	u32 dma_test_times,
	u8 test_mode)
{
	int i;
	int mem_size = 4;

	if (test_mode == RTK_DMAC_TEST_MEMCPY) {
		mem_size = 4;
	} else if (test_mode == RTK_DMAC_TEST_CYCLIC) {
		mem_size = 1024;
	}

	pr_info("DMA test buffer size: %d\n", mem_size);

	test_addr->mem_size_src = mem_size;
	test_addr->mem_size_dst = mem_size;

	if (dma_test_times == 0) {
		dev_info(test_dev, "Coherent start src.\n");
		test_addr->buf_src = dma_alloc_coherent(test_dev, test_addr->mem_size_src, &test_addr->dma_handle_src, GFP_KERNEL);

		dev_info(test_dev, "Coherent start dst.\n");
		test_addr->buf_dst = dma_alloc_coherent(test_dev, test_addr->mem_size_dst, &test_addr->dma_handle_dst, GFP_KERNEL);

		pr_info("Map src addr [%p] to [%x]\n", test_addr->buf_src, test_addr->dma_handle_src);
		pr_info("Map dst addr [%p] to [%x]\n", test_addr->buf_dst, test_addr->dma_handle_dst);
	}

	for (i = 0; i < test_addr->mem_size_src / 4; i++) {
		*(test_addr->buf_src + i) = (i << 24 | i << 16 | i << 8 | i) + dma_test_times + 1;
	}

}

void rtk_dma_test_cyclic_callback(void *data)
{
	struct rtk_dma_test_addr *test_addr;
	test_addr = data;

	pr_info("\n\nDMA cyclic rx callback\n\n");
	pr_info("test_addr->buf_src = [%p]\n", test_addr->buf_src);
	pr_info("test_addr->buf_dst = [%p]\n", test_addr->buf_dst);
	pr_info("test_addr->mem_size_dst = %d\n", test_addr->mem_size_dst);

	if (buf_compare(test_addr->buf_src, test_addr->buf_dst, test_addr->mem_size_dst)) {
		pr_info("buf_dst and buf_src compare ok.\n\n");
		dma_cyclic_status = RTK_DMAC_TEST_SUCCESS;
	} else {
		pr_info("buf_dst and buf_src compare wrong.\n\n");
		dma_cyclic_status = RTK_DMAC_TEST_FAIL;
	}
}

int rtk_dma_test_cyclic(struct device *test_dev, int loop)
{
	struct rtk_dma_test_addr test_addr;
	struct dma_slave_config *config;
	struct dma_chan *chan;
	struct dma_async_tx_descriptor *txdesc;
	u32 ret, dma_test_times = 0;
	const char *name = "dma_test_cyclic";
	struct dma_peripheral_config dma_peri;

	dev_info(test_dev, "rtk_dma_test_cyclic\n");

	while (dma_test_times < loop) {
		if(dma_cyclic_status == RTK_DMAC_TEST_FAIL) {
			pr_info("--- fail ---\n");
			rtk_dmac_test_end(test_dev, &test_addr, chan);
			return RTK_DMAC_TEST_FAIL;
		} else if (dma_cyclic_status == RTK_DMAC_TEST_WAIT) {
			pr_info("--- waiting ---\n");
			msleep(1000);
			continue;
		} else if (dma_cyclic_status == RTK_DMAC_TEST_SUCCESS) {
			pr_info("--- start ---\n");
			dma_cyclic_status = RTK_DMAC_TEST_WAIT;
			rtk_prepare_test_buf(test_dev, &test_addr, dma_test_times, RTK_DMAC_TEST_CYCLIC);
			dma_test_times ++;
		}

		dev_info(test_dev, "cyclic test round: %d\n", dma_test_times);

		if (dma_test_times == 1) {
			dev_info(test_dev, "----- request chan. -----\n");
			chan = dma_request_chan(test_dev, name);
			config = devm_kzalloc(test_dev, sizeof(*config), GFP_KERNEL);
		}

		config->device_fc = 1;
		config->direction = DMA_MEM_TO_MEM;
		config->dst_addr = test_addr.dma_handle_dst;
		config->dst_port_window_size = 0;
		config->dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		config->dst_maxburst = 1;
		config->src_addr = test_addr.dma_handle_src;
		config->src_port_window_size = 0;
		config->src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		config->src_maxburst = 1;

		/* For linux kernel 6.6 */
		dma_peri.slave_id = 0;
		config->peripheral_config = &dma_peri;
		config->peripheral_size = sizeof(struct dma_peripheral_config);

		dev_info(test_dev, "----- slave config. -----\n");
		ret = dmaengine_slave_config(chan, config);
		dev_info(test_dev, "----- dmaengine slave config, ret = %d -----\n", ret);

		if (ret != 0) {
			return RTK_DMAC_TEST_FAIL;
		}

		dev_info(test_dev, "----- prep dma cyclic -----\n");
		txdesc = dmaengine_prep_dma_cyclic(chan,\
			config->dst_addr, test_addr.mem_size_src,\
			test_addr.mem_size_src / 2,\
			DMA_MEM_TO_MEM, DMA_PREP_INTERRUPT);
		txdesc->callback = rtk_dma_test_cyclic_callback;
		txdesc->callback_param = &test_addr;

		dmaengine_submit(txdesc);
		dev_info(test_dev, "----- issue pending. -----\n");
		dma_async_issue_pending(chan);
	}

	while(dma_cyclic_status == RTK_DMAC_TEST_WAIT) {
		pr_info("wait for cyclic dma end.\n");
		msleep(1000);
	}

	rtk_dmac_test_end(test_dev, &test_addr, chan);

	return RTK_DMAC_TEST_SUCCESS;
}

static const struct of_device_id rtk_dma_test_match[] = {
	{.compatible = "realtek,ameba-rtk-dmac-test",},
	{},
};
MODULE_DEVICE_TABLE(of, rtk_dma_test_match);

static int rtk_dma_test_probe(struct platform_device *pdev)
{
	int ret = 0;
	int test_mode, loop = 0;

	const struct of_device_id *of_id = NULL;

	of_id = of_match_device(rtk_dma_test_match, &pdev->dev);
	if (!of_id || strcmp(of_id->compatible, rtk_dma_test_match->compatible)) {
		return 0;
	}

	pr_info("rtk_dma_test_probe\n");

	/* for future config. */
	test_mode = RTK_DMAC_TEST_AUTO;
	loop = 5;

	if (test_mode == RTK_DMAC_TEST_AUTO) {
		ret = rtk_dma_test_cyclic(&pdev->dev, loop);
		dev_info(&pdev->dev, "DMA test %s\n", (ret == RTK_DMAC_TEST_SUCCESS) ? "PASS" : "FAIL");
	}

	return 0;
}

static int rtk_dma_test_remove(struct platform_device *pdev)
{
	printk("----- rtk_dma_test_remove -----\n");
	return 0;
}

static struct platform_driver rtk_dma_test_driver = {
	.probe	= rtk_dma_test_probe,
	.remove	= rtk_dma_test_remove,
	.driver = {
		.name = "ameba-dma-test",
		.of_match_table = of_match_ptr(rtk_dma_test_match),
	},
};

static int rtk_dmac_test_init(void)
{
	pr_info("RTK_DMAC Test Start.\n");
	platform_driver_register(&rtk_dma_test_driver);
	return 0;
}
late_initcall(rtk_dmac_test_init);

static void rtk_dmac_test_exit(void)
{
	pr_info("RTK_DMAC Test Exit.\n");
	platform_driver_unregister(&rtk_dma_test_driver);
}
module_exit(rtk_dmac_test_exit);

MODULE_DESCRIPTION("Realtek Ameba DMAC test module");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
