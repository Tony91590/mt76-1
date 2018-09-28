/*
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 * Copyright (C) 2015 Jakub Kicinski <kubakici@wp.pl>
 * Copyright (C) 2018 Stanislaw Gruszka <stf_xl@wp.pl>
 * Copyright (C) 2018 Lorenzo Bianconi <lorenzo.bianconi83@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __MT76X0U_INITVALS_H
#define __MT76X0U_INITVALS_H

#include "phy.h"

static const struct mt76_reg_pair common_mac_reg_table[] = {
	{ MT_BCN_OFFSET(0),		0xf8f0e8e0 },
	{ MT_BCN_OFFSET(1),		0x6f77d0c8 },
	{ MT_LEGACY_BASIC_RATE,		0x0000013f },
	{ MT_HT_BASIC_RATE,		0x00008003 },
	{ MT_MAC_SYS_CTRL,		0x00000000 },
	{ MT_RX_FILTR_CFG,		0x00017f97 },
	{ MT_BKOFF_SLOT_CFG,		0x00000209 },
	{ MT_TX_SW_CFG0,		0x00000000 },
	{ MT_TX_SW_CFG1,		0x00080606 },
	{ MT_TX_LINK_CFG,		0x00001020 },
	{ MT_TX_TIMEOUT_CFG,		0x000a2090 },
	{ MT_MAX_LEN_CFG,		0xa0fff | 0x00001000 },
	{ MT_LED_CFG,			0x7f031e46 },
	{ MT_PBF_TX_MAX_PCNT,		0x1fbf1f1f },
	{ MT_PBF_RX_MAX_PCNT,		0x0000fe9f },
	{ MT_TX_RETRY_CFG,		0x47d01f0f },
	{ MT_AUTO_RSP_CFG,		0x00000013 },
	{ MT_CCK_PROT_CFG,		0x05740003 },
	{ MT_OFDM_PROT_CFG,		0x05740003 },
	{ MT_PBF_CFG,			0x00f40006 },
	{ MT_WPDMA_GLO_CFG,		0x00000030 },
	{ MT_GF20_PROT_CFG,		0x01744004 },
	{ MT_GF40_PROT_CFG,		0x03f44084 },
	{ MT_MM20_PROT_CFG,		0x01744004 },
	{ MT_MM40_PROT_CFG,		0x03f54084 },
	{ MT_TXOP_CTRL_CFG,		0x0000583f },
	{ MT_TX_RTS_CFG,		0x00092b20 },
	{ MT_EXP_ACK_TIME,		0x002400ca },
	{ MT_TXOP_HLDR_ET,		0x00000002 },
	{ MT_XIFS_TIME_CFG,		0x33a41010 },
	{ MT_PWR_PIN_CFG,		0x00000000 },
};

static const struct mt76_reg_pair mt76x0_mac_reg_table[] = {
	{ MT_IOCFG_6,			0xa0040080 },
	{ MT_PBF_SYS_CTRL,		0x00080c00 },
	{ MT_PBF_CFG,			0x77723c1f },
	{ MT_FCE_PSE_CTRL,		0x00000001 },
	{ MT_AMPDU_MAX_LEN_20M1S,	0xAAA99887 },
	{ MT_TX_SW_CFG0,		0x00000601 },
	{ MT_TX_SW_CFG1,		0x00040000 },
	{ MT_TX_SW_CFG2,		0x00000000 },
	{ 0xa44,			0x00000000 },
	{ MT_HEADER_TRANS_CTRL_REG,	0x00000000 },
	{ MT_TSO_CTRL,			0x00000000 },
	{ MT_BB_PA_MODE_CFG1,		0x00500055 },
	{ MT_RF_PA_MODE_CFG1,		0x00500055 },
	{ MT_TX_ALC_CFG_0,		0x2F2F000C },
	{ MT_TX0_BB_GAIN_ATTEN,		0x00000000 },
	{ MT_TX_PWR_CFG_0,		0x3A3A3A3A },
	{ MT_TX_PWR_CFG_1,		0x3A3A3A3A },
	{ MT_TX_PWR_CFG_2,		0x3A3A3A3A },
	{ MT_TX_PWR_CFG_3,		0x3A3A3A3A },
	{ MT_TX_PWR_CFG_4,		0x3A3A3A3A },
	{ MT_TX_PWR_CFG_7,		0x3A3A3A3A },
	{ MT_TX_PWR_CFG_8,		0x0000003A },
	{ MT_TX_PWR_CFG_9,		0x0000003A },
	{ 0x150C,			0x00000002 },
	{ 0x1238,			0x001700C8 },
	{ MT_LDO_CTRL_0,		0x00A647B6 },
	{ MT_LDO_CTRL_1,		0x6B006464 },
	{ MT_HT_BASIC_RATE,		0x00004003 },
	{ MT_HT_CTRL_CFG,		0x000001FF },
	{ MT_TXOP_HLDR_ET,		0x00000000 }
};

static const struct mt76_reg_pair mt76x0_bbp_init_tab[] = {
	{ MT_BBP(CORE, 1),	0x00000002 },
	{ MT_BBP(CORE, 4),	0x00000000 },
	{ MT_BBP(CORE, 24),	0x00000000 },
	{ MT_BBP(CORE, 32),	0x4003000a },
	{ MT_BBP(CORE, 42),	0x00000000 },
	{ MT_BBP(CORE, 44),	0x00000000 },
	{ MT_BBP(IBI, 11),	0x0FDE8081 },
	{ MT_BBP(AGC, 0),	0x00021400 },
	{ MT_BBP(AGC, 1),	0x00000003 },
	{ MT_BBP(AGC, 2),	0x003A6464 },
	{ MT_BBP(AGC, 15),	0x88A28CB8 },
	{ MT_BBP(AGC, 22),	0x00001E21 },
	{ MT_BBP(AGC, 23),	0x0000272C },
	{ MT_BBP(AGC, 24),	0x00002F3A },
	{ MT_BBP(AGC, 25),	0x8000005A },
	{ MT_BBP(AGC, 26),	0x007C2005 },
	{ MT_BBP(AGC, 33),	0x00003238 },
	{ MT_BBP(AGC, 34),	0x000A0C0C },
	{ MT_BBP(AGC, 37),	0x2121262C },
	{ MT_BBP(AGC, 41),	0x38383E45 },
	{ MT_BBP(AGC, 57),	0x00001010 },
	{ MT_BBP(AGC, 59),	0xBAA20E96 },
	{ MT_BBP(AGC, 63),	0x00000001 },
	{ MT_BBP(TXC, 0),	0x00280403 },
	{ MT_BBP(TXC, 1),	0x00000000 },
	{ MT_BBP(RXC, 1),	0x00000012 },
	{ MT_BBP(RXC, 2),	0x00000011 },
	{ MT_BBP(RXC, 3),	0x00000005 },
	{ MT_BBP(RXC, 4),	0x00000000 },
	{ MT_BBP(RXC, 5),	0xF977C4EC },
	{ MT_BBP(RXC, 7),	0x00000090 },
	{ MT_BBP(TXO, 8),	0x00000000 },
	{ MT_BBP(TXBE, 0),	0x00000000 },
	{ MT_BBP(TXBE, 4),	0x00000004 },
	{ MT_BBP(TXBE, 6),	0x00000000 },
	{ MT_BBP(TXBE, 8),	0x00000014 },
	{ MT_BBP(TXBE, 9),	0x20000000 },
	{ MT_BBP(TXBE, 10),	0x00000000 },
	{ MT_BBP(TXBE, 12),	0x00000000 },
	{ MT_BBP(TXBE, 13),	0x00000000 },
	{ MT_BBP(TXBE, 14),	0x00000000 },
	{ MT_BBP(TXBE, 15),	0x00000000 },
	{ MT_BBP(TXBE, 16),	0x00000000 },
	{ MT_BBP(TXBE, 17),	0x00000000 },
	{ MT_BBP(RXFE, 1),	0x00008800 },
	{ MT_BBP(RXFE, 3),	0x00000000 },
	{ MT_BBP(RXFE, 4),	0x00000000 },
	{ MT_BBP(RXO, 13),	0x00000192 },
	{ MT_BBP(RXO, 14),	0x00060612 },
	{ MT_BBP(RXO, 15),	0xC8321B18 },
	{ MT_BBP(RXO, 16),	0x0000001E },
	{ MT_BBP(RXO, 17),	0x00000000 },
	{ MT_BBP(RXO, 18),	0xCC00A993 },
	{ MT_BBP(RXO, 19),	0xB9CB9CB9 },
	{ MT_BBP(RXO, 20),	0x26c00057 },
	{ MT_BBP(RXO, 21),	0x00000001 },
	{ MT_BBP(RXO, 24),	0x00000006 },
	{ MT_BBP(RXO, 28),	0x0000003F },
};

static const struct mt76x0_bbp_switch_item mt76x0_bbp_switch_tab[] = {
	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 4),	0x1FEDA049 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 4),	0x1FECA054 } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 6),	0x00000045 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 6),	0x0000000A } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 8),	0x16344EF0 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 8),	0x122C54F2 } },

	{ RF_G_BAND | RF_BW_20,				{ MT_BBP(AGC, 12),	0x05052879 } },
	{ RF_G_BAND | RF_BW_40,				{ MT_BBP(AGC, 12),	0x050528F9 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 12),	0x050528F9 } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 13),	0x35050004 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 13),	0x2C3A0406 } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 14),	0x310F2E3C } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 14),	0x310F2A3F } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 26),	0x007C2005 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 26),	0x007C2005 } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 27),	0x000000E1 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 27),	0x000000EC } },

	{ RF_G_BAND | RF_BW_20,				{ MT_BBP(AGC, 28),	0x00060806 } },
	{ RF_G_BAND | RF_BW_40,				{ MT_BBP(AGC, 28),	0x00050806 } },
	{ RF_A_BAND | RF_BW_40,				{ MT_BBP(AGC, 28),	0x00060801 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_80,		{ MT_BBP(AGC, 28),	0x00060806 } },

	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(RXO, 28),	0x0000008A } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 31),	0x00000E23 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 31),	0x00000E13 } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 32),	0x00003218 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 32),	0x0000181C } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 33),	0x00003240 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 33),	0x00003218 } },

	{ RF_G_BAND | RF_BW_20,				{ MT_BBP(AGC, 35),	0x11111616 } },
	{ RF_G_BAND | RF_BW_40,				{ MT_BBP(AGC, 35),	0x11111516 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 35),	0x11111111 } },

	{ RF_G_BAND | RF_BW_20,				{ MT_BBP(AGC, 39),	0x2A2A3036 } },
	{ RF_G_BAND | RF_BW_40,				{ MT_BBP(AGC, 39),	0x2A2A2C36 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 39),	0x2A2A2A2A } },

	{ RF_G_BAND | RF_BW_20,				{ MT_BBP(AGC, 43),	0x27273438 } },
	{ RF_G_BAND | RF_BW_40,				{ MT_BBP(AGC, 43),	0x27272D38 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 43),	0x27271A1A } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 51),	0x17171C1C } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 51),	0xFFFFFFFF } },

	{ RF_G_BAND | RF_BW_20,				{ MT_BBP(AGC, 53),	0x26262A2F } },
	{ RF_G_BAND | RF_BW_40,				{ MT_BBP(AGC, 53),	0x2626322F } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 53),	0xFFFFFFFF } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 55),	0x40404040 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 55),	0xFFFFFFFF } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(AGC, 58),	0x00001010 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(AGC, 58),	0x00000000 } },

	{ RF_G_BAND | RF_BW_20 | RF_BW_40,		{ MT_BBP(RXFE, 0),	0x3D5000E0 } },
	{ RF_A_BAND | RF_BW_20 | RF_BW_40 | RF_BW_80,	{ MT_BBP(RXFE, 0),	0x895000E0 } },
};

static const struct mt76_reg_pair mt76x0_dcoc_tab[] = {
	{ MT_BBP(CAL, 47), 0x000010F0 },
	{ MT_BBP(CAL, 48), 0x00008080 },
	{ MT_BBP(CAL, 49), 0x00000F07 },
	{ MT_BBP(CAL, 50), 0x00000040 },
	{ MT_BBP(CAL, 51), 0x00000404 },
	{ MT_BBP(CAL, 52), 0x00080803 },
	{ MT_BBP(CAL, 53), 0x00000704 },
	{ MT_BBP(CAL, 54), 0x00002828 },
	{ MT_BBP(CAL, 55), 0x00005050 },
};

#endif
