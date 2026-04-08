// SPDX-License-Identifier: GPL-2.0
/*
 * Mediatek MT7628 AES Engine driver
 * Copyright (c) 2026 Richard van Schagen <richard@routerwrt.org>
 */

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#include "mtk_aes.h"

static int mtk_aes_alloc_desc(struct mtk_aes_priv *mtk)
{
	mtk->tx_ring.base = dmam_alloc_coherent(mtk->dev,
			sizeof(struct mtk_aes_tx_desc) *
			MTK_AES_RING_SIZE,
			&mtk->tx_ring.base_dma, GFP_KERNEL);

	if (!mtk->tx_ring.base)
		return -ENOMEM;

	mtk->rx_ring.base = dmam_alloc_coherent(mtk->dev,
			sizeof(struct mtk_aes_rx_desc) *
			MTK_AES_RING_SIZE,
			&mtk->rx_ring.base_dma, GFP_KERNEL);

	if (!mtk->rx_ring.base)
		return -ENOMEM;

	mtk->tx_ring.cpu_idx = 0;
	mtk->tx_ring.dma_idx = 0;
	mtk->rx_ring.cpu_idx = 0;
	mtk->rx_ring.dma_idx = 0;

	return 0;
}

static int mtk_aes_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mtk_aes_priv *mtk;
	struct resource *res;
	int err;

	mtk = devm_kzalloc(dev, sizeof(*mtk), GFP_KERNEL);
	if (!mtk)
		return -ENOMEM;

	mtk->dev = dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mtk->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(mtk->base))
		return PTR_ERR(mtk->base);

	mtk->irq = platform_get_irq(pdev, 0);
	if (mtk->irq < 0)
		return dev_err_probe(dev, mtk->irq, "missing IRQ\n");

	mtk->clk = devm_clk_get_optional(dev, "cryp");
	if (IS_ERR(mtk->clk))
		return dev_err_probe(dev, PTR_ERR(mtk->clk),
				     "failed to get aes clock\n");

	err = clk_prepare_enable(mtk->clk);
	if (err)
		return dev_err_probe(dev, err,
				     "failed to enable aes clock\n");

	mtk->rst = devm_reset_control_get_optional_exclusive(dev, "cryp");
	if (IS_ERR(mtk->rst)) {
		err = dev_err_probe(dev, PTR_ERR(mtk->rst),
				    "failed to get aes reset\n");
		goto err_clk;
	}

	if (mtk->rst) {
		err = reset_control_deassert(mtk->rst);
		if (err) {
			err = dev_err_probe(dev, err,
					    "failed to deassert aes reset\n");
			goto err_clk;
		}
	}

	platform_set_drvdata(pdev, mtk);

	err = mtk_aes_alloc_desc(mtk);
	if (err)
		goto err_clk;

	/* TODO: IRQ, engine */

	return 0;

err_clk:
	clk_disable_unprepare(mtk->clk);
	return err;
}

static void mtk_aes_remove(struct platform_device *pdev)
{
	struct mtk_aes_priv *mtk = platform_get_drvdata(pdev);

	if (mtk->rst)
		reset_control_assert(mtk->rst);

	clk_disable_unprepare(mtk->clk);
}

static const struct of_device_id of_crypto_id[] = {
	{ .compatible = "mediatek,mtk-aes" },
	{},
};

MODULE_DEVICE_TABLE(of, of_crypto_id);

static struct platform_driver mt7628_aes_driver = {
	.probe  = mtk_aes_probe,
	.remove = mtk_aes_remove,
	.driver = {
		.name           = "mtk-aes",
		.of_match_table = of_crypto_id,
	},
};

module_platform_driver(mt7628_aes_driver);

MODULE_AUTHOR("Richard van Schagen <richard@routerwrt.org>");
MODULE_DESCRIPTION("Mediatek MT7628 AES Crypto hardware driver");
MODULE_LICENSE("GPL");
