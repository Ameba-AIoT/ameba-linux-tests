// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek DMAC test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>
#include <linux/slab.h>

struct rtk_dma_test {
	struct device	dev;
	void __iomem	*base;
	spinlock_t	lock;
	int		irq;
};

struct rtk_dma_test_addr {
	u32		*buf_src;
	dma_addr_t	dma_handle_src;
	u32		mem_size_src;

	u32		*buf_dst;
	dma_addr_t	dma_handle_dst;
	u32		mem_size_dst;
};

enum test_mode {
	RTK_DMAC_TEST_AUTO	= 0,
	RTK_DMAC_TEST_MEMCPY	= 1,
	RTK_DMAC_TEST_CYCLIC	= 2,
	RTK_DMAC_TEST_SG	= 3,
};

enum test_result {
	RTK_DMAC_TEST_FAIL	= 0,
	RTK_DMAC_TEST_SUCCESS	= 1,
	RTK_DMAC_TEST_WAIT	= 2,
};


