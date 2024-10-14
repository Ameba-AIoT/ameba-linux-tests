// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek DMAC test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <dt-bindings/realtek/dmac/realtek-ameba-dmac.h>
#include "rtk_dmac_test.h"

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
			pr_info("buf_print: src");
			buf_print(bufa, compare_len);
			pr_info("buf_print: dst");
			buf_print(bufb, compare_len);
			return 0;
		}
	}
	return 1;
}

void rtk_multi_dmac_test_end(
	struct device *test_dev,
	struct rtk_dma_test_addr *test_addr,
	struct dma_chan *chan)
{
	printk("dma client control: terminate manually!!");
	dmaengine_terminate_all(chan);
	dma_release_channel(chan);

	dma_free_coherent(test_dev, test_addr->mem_size_src,\
		test_addr->buf_src, test_addr->dma_handle_src);

	dma_free_coherent(test_dev, test_addr->mem_size_dst,\
		test_addr->buf_dst, test_addr->dma_handle_dst);
}

void rtk_prepare_test_buf(struct device *test_dev, struct rtk_dma_test_addr *test_addr)
{
	int i;
	int mem_size = 32;

	pr_info("buf size prepared for dma test is %d", mem_size);

	/* When doing memory to memory dma. src size = dst size. */
	/* If not the same, the result will be wrong. */
	test_addr->mem_size_src = mem_size;
	test_addr->mem_size_dst = mem_size;

	dev_info(test_dev, "coherent start src.\n");
	test_addr->buf_src = dma_alloc_coherent(test_dev, test_addr->mem_size_src, &test_addr->dma_handle_src, GFP_KERNEL);

	dev_info(test_dev, "coherent start dst.\n");
	test_addr->buf_dst = dma_alloc_coherent(test_dev, test_addr->mem_size_dst, &test_addr->dma_handle_dst, GFP_KERNEL);

	pr_info("map src addr [%p] to [%x]\n", test_addr->buf_src, test_addr->dma_handle_src);
	pr_info("map dst addr [%p] to [%x]\n", test_addr->buf_dst, test_addr->dma_handle_dst);

	for (i = 0; i < test_addr->mem_size_src / 4; i++) {
		*(test_addr->buf_src + i) = 0x66666666;
	}

	printk("src prepared buf contex:");
	buf_print(test_addr->buf_src, test_addr->mem_size_src);
	printk("\n");
}

void rtk_dma_test_cyclic_callback(void *data)
{
	struct rtk_dma_test_addr *test_addr;
	test_addr = data;

	pr_info("\n\ndma cyclic rx callback\n\n");
	pr_info("test_addr->buf_src = [%p]", test_addr->buf_src);
	pr_info("test_addr->buf_dst = [%p]", test_addr->buf_dst);
	pr_info("test_addr->mem_size_dst = %d", test_addr->mem_size_dst);

	if (buf_compare(test_addr->buf_src, test_addr->buf_dst, test_addr->mem_size_dst)) {
		pr_info("buf_dst and buf_src compare ok.\n\n");
		dma_cyclic_status = RTK_DMAC_TEST_SUCCESS;
	} else {
		pr_info("buf_dst and buf_src compare wrong.\n\n");
		dma_cyclic_status = RTK_DMAC_TEST_FAIL;
	}
}

int rtk_dma_test_cyclic(struct device *test_dev, struct rtk_dma_test_addr *test_addr)
{
	struct dma_slave_config *config;
	struct dma_chan *chan;
	struct dma_async_tx_descriptor *txdesc;
	dma_cookie_t cookie;
	u32 ret;
	const char *name = "dma_test_cyclic";
	int i, j;
	struct dma_tx_state state;
	struct dma_peripheral_config dma_peri;

	dev_info(test_dev, "rtk_dma_test_cyclic");
	rtk_prepare_test_buf(test_dev, test_addr);

	dev_info(test_dev, "----- request chan. -----");
	chan = dma_request_chan(test_dev, name);
	config = devm_kzalloc(test_dev, sizeof(*config), GFP_KERNEL);

	config->device_fc = 1;
	config->direction = DMA_MEM_TO_MEM;
	config->dst_addr = test_addr->dma_handle_dst;
	config->dst_port_window_size = 0;
	config->dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	config->dst_maxburst = 1;

	/* For linux kernel 6.6 */
	dma_peri.slave_id = 0;
	config->peripheral_config = &dma_peri;
	config->peripheral_size = sizeof(struct dma_peripheral_config);

	config->src_addr = test_addr->dma_handle_src;
	config->src_port_window_size = 0;
	config->src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	config->src_maxburst = 1;

	dev_info(test_dev, "----- slave config. -----\n");
	ret = dmaengine_slave_config(chan, config);
	dev_info(test_dev, "----- dmaengine slave config, ret = %d -----\n", ret);

	if (ret != 0) {
		return RTK_DMAC_TEST_FAIL;
	}

	dev_info(test_dev, "----- prep dma cyclic -----");
	txdesc = dmaengine_prep_dma_cyclic(chan, config->dst_addr, test_addr->mem_size_dst, test_addr->mem_size_dst, DMA_MEM_TO_MEM, 0);
	txdesc->callback = rtk_dma_test_cyclic_callback;
	txdesc->callback_param = test_addr;

	cookie = dmaengine_submit(txdesc);
	dev_info(test_dev, "----- issue pending. -----\n");
	dma_async_issue_pending(chan);

	i = 0;
	while (i <= 5) {
		printk("round: %d", i);
		msleep(500);
		printk("----- dynamically show dst buf -----\n");
		buf_print(test_addr->buf_dst, test_addr->mem_size_dst);
		printk("----- Use 0xFF to revert buf dynamically. -----\n");
		for (j = 0; j < test_addr->mem_size_dst / 4; j++) {
			*(test_addr->buf_dst + j) = 0xFFFFFFFF;
			printk("[0x%x] 0x%x", (unsigned int)(test_addr->buf_dst + j), *(test_addr->buf_dst + j));
		}
		dmaengine_tx_status(chan, cookie, &state);
		printk("Already tx bytes = %x", state.residue);
		if (i == 2) {
			printk("\n\n\nTest pause dma channel. \n\n");
			printk("----- dynamically show dst buf before pasue -----\n");
			buf_print(test_addr->buf_dst, test_addr->mem_size_dst);
			dmaengine_pause(chan);
			printk("----- Use 0xFF to revert buf after pause. -----\n");
			for (j = 0; j < test_addr->mem_size_dst / 4; j++) {
				*(test_addr->buf_dst + j) = 0xFFFFFFFF;
				printk("[0x%x] 0x%x", (unsigned int)(test_addr->buf_dst + j), *(test_addr->buf_dst + j));
			}
			msleep(500);
			printk("----- dynamically show dst buf after pasue -----\n");
			buf_print(test_addr->buf_dst, test_addr->mem_size_dst);
			printk("\n\n\nTest resume dma channel. \n\n");
			dmaengine_resume(chan);
			msleep(500);
			printk("----- dynamically show dst buf after resume -----\n");
			buf_print(test_addr->buf_dst, test_addr->mem_size_dst);
			printk("Pause and resume test end.\n\n");
		}
		i++;
	}

	rtk_multi_dmac_test_end(test_dev, test_addr, chan);

	return RTK_DMAC_TEST_SUCCESS;
}

static int rtk_multi_dma_test_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct rtk_dma_test_addr test_addr;

	pr_info("rtk_multi_dma_test_probe");

	rtk_dma_test_cyclic(&pdev->dev, &test_addr);

	return 0;
}

static int rtk_multi_dma_test_remove(struct platform_device *pdev)
{
	printk("----- rtk_multi_dma_test_remove -----");
	return 0;
}

static const struct of_device_id rtk_multi_dma_test_match[] = {
	{.compatible = "realtek,ameba-rtk-dmac-multi-test",},
	{},
};
MODULE_DEVICE_TABLE(of, rtk_multi_dma_test_match);

static struct platform_driver rtk_multi_dma_test_driver = {
	.probe	= rtk_multi_dma_test_probe,
	.remove	= rtk_multi_dma_test_remove,
	.driver = {
		.name = "ameba-dma-multi-test",
		.of_match_table = of_match_ptr(rtk_multi_dma_test_match),
	},
};

static int rtk_multi_dmac_test_init(void)
{
	pr_info("RTK_DMAC Multi block Test Start. \n");
	platform_driver_register(&rtk_multi_dma_test_driver);
	return 0;
}
late_initcall(rtk_multi_dmac_test_init);

static void rtk_multi_dmac_test_exit(void)
{
	pr_info("RTK_DMAC Multi block Test Exit.\n");
	platform_driver_unregister(&rtk_multi_dma_test_driver);
}
module_exit(rtk_multi_dmac_test_exit);

MODULE_DESCRIPTION("Realtek Ameba DMAC multiblock test module");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Realtek Corporation");
