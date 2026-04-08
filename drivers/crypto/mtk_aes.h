/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __MTK_AES_H
#define __MTK_AES_H

struct mtk_aes {
	void __iomem			*base;
	struct device			*dev;

	struct clk			*clk;
	struct reset_control		*rst;
	int				irq;
};
#endif /* __RALINK_FE_H */
