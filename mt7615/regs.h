/* SPDX-License-Identifier: ISC */
/* Copyright (C) 2019 MediaTek Inc. */

#ifndef __MT7615_REGS_H
#define __MT7615_REGS_H

#define MT_HW_REV			0x1000
#define MT_HW_CHIPID			0x1008
#define MT_TOP_MISC2			0x1134
#define MT_TOP_MISC2_FW_STATE		GENMASK(2, 0)

#define MT_MCU_BASE			0x2000
#define MT_MCU(ofs)			(MT_MCU_BASE + (ofs))

#define MT_MCU_PCIE_REMAP_1		MT_MCU(0x500)
#define MT_MCU_PCIE_REMAP_1_OFFSET	GENMASK(17, 0)
#define MT_MCU_PCIE_REMAP_1_BASE	GENMASK(31, 18)
#define MT_PCIE_REMAP_BASE_1		0x40000

#define MT_MCU_PCIE_REMAP_2		MT_MCU(0x504)
#define MT_MCU_PCIE_REMAP_2_OFFSET	GENMASK(18, 0)
#define MT_MCU_PCIE_REMAP_2_BASE	GENMASK(31, 19)
#define MT_PCIE_REMAP_BASE_2		0x80000

#define MT_HIF_BASE			0x4000
#define MT_HIF(ofs)			(MT_HIF_BASE + (ofs))

#define MT_CFG_LPCR_HOST		MT_HIF(0x1f0)
#define MT_CFG_LPCR_HOST_FW_OWN		BIT(0)
#define MT_CFG_LPCR_HOST_DRV_OWN	BIT(1)

#define MT_INT_SOURCE_CSR		MT_HIF(0x200)
#define MT_INT_MASK_CSR			MT_HIF(0x204)
#define MT_DELAY_INT_CFG		MT_HIF(0x210)

#define MT_INT_RX_DONE(_n)		BIT(_n)
#define MT_INT_RX_DONE_ALL		GENMASK(1, 0)
#define MT_INT_TX_DONE_ALL		GENMASK(7, 4)
#define MT_INT_TX_DONE(_n)		BIT((_n) + 4)

#define MT_WPDMA_GLO_CFG		MT_HIF(0x208)
#define MT_WPDMA_GLO_CFG_TX_DMA_EN	BIT(0)
#define MT_WPDMA_GLO_CFG_TX_DMA_BUSY	BIT(1)
#define MT_WPDMA_GLO_CFG_RX_DMA_EN	BIT(2)
#define MT_WPDMA_GLO_CFG_RX_DMA_BUSY	BIT(3)
#define MT_WPDMA_GLO_CFG_DMA_BURST_SIZE	GENMASK(5, 4)
#define MT_WPDMA_GLO_CFG_TX_WRITEBACK_DONE	BIT(6)
#define MT_WPDMA_GLO_CFG_BIG_ENDIAN	BIT(7)
#define MT_WPDMA_GLO_CFG_TX_BT_SIZE_BIT0	BIT(9)
#define MT_WPDMA_GLO_CFG_MULTI_DMA_EN	GENMASK(11, 10)
#define MT_WPDMA_GLO_CFG_FIFO_LITTLE_ENDIAN	BIT(12)
#define MT_WPDMA_GLO_CFG_TX_BT_SIZE_BIT21	GENMASK(23, 22)
#define MT_WPDMA_GLO_CFG_SW_RESET	BIT(24)
#define MT_WPDMA_GLO_CFG_FIRST_TOKEN_ONLY	BIT(26)
#define MT_WPDMA_GLO_CFG_OMIT_TX_INFO	BIT(28)

#define MT_WPDMA_RST_IDX		MT_HIF(0x20c)

#define MT_TX_RING_BASE			MT_HIF(0x300)
#define MT_RX_RING_BASE			MT_HIF(0x400)

#define MT_WPDMA_GLO_CFG1		MT_HIF(0x500)
#define MT_WPDMA_TX_PRE_CFG		MT_HIF(0x510)
#define MT_WPDMA_RX_PRE_CFG		MT_HIF(0x520)
#define MT_WPDMA_ABT_CFG		MT_HIF(0x530)
#define MT_WPDMA_ABT_CFG1		MT_HIF(0x534)

#define MT_WF_PHY_BASE			0x10000
#define MT_WF_PHY(ofs)			(MT_WF_PHY_BASE + (ofs))

#define MT_WF_PHY_WF2_RFCTRL0		MT_WF_PHY(0x1900)
#define MT_WF_PHY_WF2_RFCTRL0_LPBCN_EN	BIT(9)

#define MT_WF_CFG_BASE			0x20200
#define MT_WF_CFG(ofs)			(MT_WF_CFG_BASE + (ofs))

#define MT_CFG_CCR			MT_WF_CFG(0x000)
#define MT_CFG_CCR_MAC_D1_1X_GC_EN	BIT(24)
#define MT_CFG_CCR_MAC_D0_1X_GC_EN	BIT(25)
#define MT_CFG_CCR_MAC_D1_2X_GC_EN	BIT(30)
#define MT_CFG_CCR_MAC_D0_2X_GC_EN	BIT(31)

#define MT_WF_AGG_BASE			0x20a00
#define MT_WF_AGG(ofs)			(MT_WF_AGG_BASE + (ofs))

#define MT_AGG_ARCR			MT_WF_AGG(0x010)
#define MT_AGG_ARCR_INIT_RATE1		BIT(0)
#define MT_AGG_ARCR_RTS_RATE_THR	GENMASK(12, 8)
#define MT_AGG_ARCR_RATE_DOWN_RATIO	GENMASK(17, 16)
#define MT_AGG_ARCR_RATE_DOWN_RATIO_EN	BIT(19)
#define MT_AGG_ARCR_RATE_UP_EXTRA_TH	GENMASK(22, 20)

#define MT_AGG_ARUCR			MT_WF_AGG(0x018)
#define MT_AGG_ARDCR			MT_WF_AGG(0x01c)
#define MT_AGG_ARxCR_LIMIT_SHIFT(_n)	(4 * (_n))
#define MT_AGG_ARxCR_LIMIT(_n)		GENMASK(2 + \
					MT_AGG_ARxCR_LIMIT_SHIFT(_n), \
					MT_AGG_ARxCR_LIMIT_SHIFT(_n))

#define MT_AGG_SCR			MT_WF_AGG(0x0fc)
#define MT_AGG_SCR_NLNAV_MID_PTEC_DIS	BIT(3)

#define MT_WF_TMAC_BASE			0x21000
#define MT_WF_TMAC(ofs)			(MT_WF_TMAC_BASE + (ofs))

#define MT_TMAC_CTCR0			MT_WF_TMAC(0x0f4)
#define MT_TMAC_CTCR0_INS_DDLMT_REFTIME	GENMASK(5, 0)
#define MT_TMAC_CTCR0_INS_DDLMT_DENSITY	GENMASK(15, 12)
#define MT_TMAC_CTCR0_INS_DDLMT_EN	BIT(17)
#define MT_TMAC_CTCR0_INS_DDLMT_VHT_SMPDU_EN	BIT(18)

#define MT_WF_RMAC_BASE			0x21200
#define MT_WF_RMAC(ofs)			(MT_WF_RMAC_BASE + (ofs))

#define MT_WF_RFCR			MT_WF_RMAC(0x000)
#define MT_WF_RFCR_DROP_STBC_MULTI	BIT(0)
#define MT_WF_RFCR_DROP_FCSFAIL		BIT(1)
#define MT_WF_RFCR_DROP_VERSION		BIT(3)
#define MT_WF_RFCR_DROP_PROBEREQ	BIT(4)
#define MT_WF_RFCR_DROP_MCAST		BIT(5)
#define MT_WF_RFCR_DROP_BCAST		BIT(6)
#define MT_WF_RFCR_DROP_MCAST_FILTERED	BIT(7)
#define MT_WF_RFCR_DROP_A3_MAC		BIT(8)
#define MT_WF_RFCR_DROP_A3_BSSID	BIT(9)
#define MT_WF_RFCR_DROP_A2_BSSID	BIT(10)
#define MT_WF_RFCR_DROP_OTHER_BEACON	BIT(11)
#define MT_WF_RFCR_DROP_FRAME_REPORT	BIT(12)
#define MT_WF_RFCR_DROP_CTL_RSV		BIT(13)
#define MT_WF_RFCR_DROP_CTS		BIT(14)
#define MT_WF_RFCR_DROP_RTS		BIT(15)
#define MT_WF_RFCR_DROP_DUPLICATE	BIT(16)
#define MT_WF_RFCR_DROP_OTHER_BSS	BIT(17)
#define MT_WF_RFCR_DROP_OTHER_UC	BIT(18)
#define MT_WF_RFCR_DROP_OTHER_TIM	BIT(19)
#define MT_WF_RFCR_DROP_NDPA		BIT(20)
#define MT_WF_RFCR_DROP_UNWANTED_CTL	BIT(21)

#define MT_WF_DMA_BASE			0x21800
#define MT_WF_DMA(ofs)			(MT_WF_DMA_BASE + (ofs))

#define MT_DMA_DCR0			MT_WF_DMA(0x000)
#define MT_DMA_DCR0_MAX_RX_LEN		GENMASK(15, 2)
#define MT_DMA_DCR0_RX_VEC_DROP		BIT(17)

#define MT_WTBL_BASE			0x30000
#define MT_WTBL_ENTRY_SIZE		256

#define MT_WTBL_OFF_BASE		0x23400
#define MT_WTBL_OFF(n)			(MT_WTBL_OFF_BASE + (n))

#define MT_WTBL_UPDATE			MT_WTBL_OFF(0x030)
#define MT_WTBL_UPDATE_WLAN_IDX		GENMASK(7, 0)
#define MT_WTBL_UPDATE_RATE_UPDATE	BIT(13)
#define MT_WTBL_UPDATE_TX_COUNT_CLEAR	BIT(14)
#define MT_WTBL_UPDATE_BUSY		BIT(31)

#define MT_WTBL_ON_BASE			0x23000
#define MT_WTBL_ON(_n)			(MT_WTBL_ON_BASE + (_n))

#define MT_WTBL_RIUCR0			MT_WTBL_ON(0x020)

#define MT_WTBL_RIUCR1			MT_WTBL_ON(0x024)
#define MT_WTBL_RIUCR1_RATE0		GENMASK(11, 0)
#define MT_WTBL_RIUCR1_RATE1		GENMASK(23, 12)
#define MT_WTBL_RIUCR1_RATE2_LO		GENMASK(31, 24)

#define MT_WTBL_RIUCR2			MT_WTBL_ON(0x028)
#define MT_WTBL_RIUCR2_RATE2_HI		GENMASK(3, 0)
#define MT_WTBL_RIUCR2_RATE3		GENMASK(15, 4)
#define MT_WTBL_RIUCR2_RATE4		GENMASK(27, 16)
#define MT_WTBL_RIUCR2_RATE5_LO		GENMASK(31, 28)

#define MT_WTBL_RIUCR3			MT_WTBL_ON(0x02c)
#define MT_WTBL_RIUCR3_RATE5_HI		GENMASK(7, 0)
#define MT_WTBL_RIUCR3_RATE6		GENMASK(19, 8)
#define MT_WTBL_RIUCR3_RATE7		GENMASK(31, 20)

#define MT_WTBL_W5_CHANGE_BW_RATE	GENMASK(7, 5)
#define MT_WTBL_W5_SHORT_GI_20		BIT(8)
#define MT_WTBL_W5_SHORT_GI_40		BIT(9)
#define MT_WTBL_W5_SHORT_GI_80		BIT(10)
#define MT_WTBL_W5_SHORT_GI_160		BIT(11)
#define MT_WTBL_W5_BW_CAP		GENMASK(13, 12)
#define MT_WTBL_W5_MPDU_FAIL_COUNT	GENMASK(25, 23)
#define MT_WTBL_W5_MPDU_OK_COUNT	GENMASK(28, 26)
#define MT_WTBL_W5_RATE_IDX		GENMASK(31, 29)

#define MT_WTBL_W27_CC_BW_SEL		GENMASK(6, 5)

#define MT_EFUSE_BASE			0x81070000
#define MT_EFUSE_BASE_CTRL		0x000
#define MT_EFUSE_BASE_CTRL_EMPTY	BIT(30)

#define MT_EFUSE_CTRL			0x008
#define MT_EFUSE_CTRL_AOUT		GENMASK(5, 0)
#define MT_EFUSE_CTRL_MODE		GENMASK(7, 6)
#define MT_EFUSE_CTRL_LDO_OFF_TIME	GENMASK(13, 8)
#define MT_EFUSE_CTRL_LDO_ON_TIME	GENMASK(15, 14)
#define MT_EFUSE_CTRL_AIN		GENMASK(25, 16)
#define MT_EFUSE_CTRL_VALID		BIT(29)
#define MT_EFUSE_CTRL_KICK		BIT(30)
#define MT_EFUSE_CTRL_SEL		BIT(31)

#define MT_EFUSE_WDATA(_i)		(0x010 + ((_i) * 4))
#define MT_EFUSE_RDATA(_i)		(0x030 + ((_i) * 4))

#endif
