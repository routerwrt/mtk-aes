/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __MTK_AES_H
#define __MTK_AES_H

#define MTK_AES_RING_SIZE		128

/* Power-of-2 masks */
#define MTK_AES_RING_MASK		(MTK_AES_RING_SIZE - 1)

/*
 * AES AES_RX Descriptor Format define
 */

#define RX2_DMA_SDL0_GET(_x)		(((_x) >> 16) & 0x3fff)
#define RX2_DMA_SDL0_SET(_x)		(((_x) & 0x3fff) << 16)
#define RX2_DMA_LS0			BIT(30)
#define RX2_DMA_DONE			BIT(31)

#define RX4_DMA_ENC			BIT(2)
#define RX4_DMA_UDV			BIT(3)
#define RX4_DMA_CBC			BIT(4)
#define RX4_DMA_IVR			BIT(5)
#define RX4_DMA_KIU			BIT(6)

struct mtk_aes_rx_desc {
	unsigned int			SDP0;
	unsigned int			rxd_info2;
	unsigned int			user_data;
	unsigned int			rxd_info4;
	unsigned int			IV[4];
}__aligned(32);

/*
 * AES AES_TX Descriptor Format define
 */

#define TX2_DMA_SDL1_SET(_x)		((_x) & 0x3fff)
#define TX2_DMA_LS1			BIT(14)
#define TX2_DMA_SDL0_SET(_x)		(((_x) & 0x3fff) << 16)
#define TX2_DMA_LS0			BIT(30)
#define TX2_DMA_DONE			BIT(31)

#define TX4_DMA_ENC			BIT(2)
#define TX4_DMA_UDV			BIT(3)
#define TX4_DMA_CBC			BIT(4)
#define TX4_DMA_IVR			BIT(5)
#define TX4_DMA_KIU			BIT(6)

#define TX4_DMA_AES_128			0
#define TX4_DMA_AES_192			1
#define TX4_DMA_AES_256			2

struct mtk_aes_tx_desc {
	unsigned int			SDP0;
	unsigned int			txd_info2;
	unsigned int			SDP1;
	unsigned int			txd_info4;
	unsigned int			IV[4];
}__aligned(32);


struct mtk_aes_ring {
	void				*base;
	dma_addr_t			base_dma;
	u16				cpu_idx;
	u16				dma_idx;
};

struct mtk_aes_priv {
	void __iomem			*base;
	struct device			*dev;

	struct clk			*clk;
	struct reset_control		*rst;
	int				irq;

	struct mtk_aes_ring		tx_ring;
	struct mtk_aes_ring		rx_ring;
};

#endif /* __MTK_AES_H */
