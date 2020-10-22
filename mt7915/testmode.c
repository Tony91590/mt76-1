// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 MediaTek Inc. */

#include "mt7915.h"
#include "mac.h"
#include "mcu.h"
#include "testmode.h"

struct reg_band {
	u32 band[2];
};

#define REG_BAND(_reg) \
	{ .band[0] = MT_##_reg(0), .band[1] = MT_##_reg(1) }
#define REG_BAND_IDX(_reg, _idx) \
	{ .band[0] = MT_##_reg(0, _idx), .band[1] = MT_##_reg(1, _idx) }

static const struct reg_band reg_backup_list[] = {
	REG_BAND_IDX(AGG_PCR0, 0),
	REG_BAND_IDX(AGG_PCR0, 1),
	REG_BAND_IDX(AGG_AWSCR0, 0),
	REG_BAND_IDX(AGG_AWSCR0, 1),
	REG_BAND_IDX(AGG_AWSCR0, 2),
	REG_BAND_IDX(AGG_AWSCR0, 3),
	REG_BAND(AGG_MRCR),
	REG_BAND(TMAC_TFCR0),
	REG_BAND(TMAC_TCR0),
	REG_BAND(AGG_ATCR1),
	REG_BAND(AGG_ATCR3),
	REG_BAND(TMAC_TRCR0),
	REG_BAND(TMAC_ICR0),
	REG_BAND_IDX(ARB_DRNGR0, 0),
	REG_BAND_IDX(ARB_DRNGR0, 1),
};

static int
mt7915_tm_mode_ctrl(struct mt7915_dev *dev, bool enable)
{
	struct {
		u8 format_id;
		bool enable;
		u8 rsv[2];
	} __packed req = {
		.format_id = 0x6,
		.enable = enable,
	};

	return mt76_mcu_send_msg(&dev->mt76,
				 MCU_EXT_CMD_TX_POWER_FEATURE_CTRL,
				 &req, sizeof(req), false);
}

static int
mt7915_tm_set_trx(struct mt7915_dev *dev, struct mt7915_phy *phy,
		  int type, bool en)
{
	struct mt7915_tm_cmd req = {
		.testmode_en = 1,
		.param_idx = MCU_ATE_SET_TRX,
		.param.trx.type = type,
		.param.trx.enable = en,
		.param.trx.band = phy != &dev->phy,
	};

	return mt76_mcu_send_msg(&dev->mt76, MCU_EXT_CMD_ATE_CTRL, &req,
				 sizeof(req), false);
}

static void
mt7915_tm_reg_backup_restore(struct mt7915_dev *dev, struct mt7915_phy *phy)
{
	int n_regs = ARRAY_SIZE(reg_backup_list);
	bool ext_phy = phy != &dev->phy;
	u32 *b = dev->test.reg_backup;
	int i;

	if (dev->mt76.test.state == MT76_TM_STATE_OFF) {
		for (i = 0; i < n_regs; i++)
			mt76_wr(dev, reg_backup_list[i].band[ext_phy], b[i]);
		return;
	}

	if (b)
		return;

	b = devm_kzalloc(dev->mt76.dev, 4 * n_regs, GFP_KERNEL);
	if (!b)
		return;

	dev->test.reg_backup = b;
	for (i = 0; i < n_regs; i++)
		b[i] = mt76_rr(dev, reg_backup_list[i].band[ext_phy]);

	mt76_clear(dev, MT_AGG_PCR0(ext_phy, 0), MT_AGG_PCR0_MM_PROT |
		   MT_AGG_PCR0_GF_PROT | MT_AGG_PCR0_ERP_PROT |
		   MT_AGG_PCR0_VHT_PROT | MT_AGG_PCR0_BW20_PROT |
		   MT_AGG_PCR0_BW40_PROT | MT_AGG_PCR0_BW80_PROT);
	mt76_set(dev, MT_AGG_PCR0(ext_phy, 0), MT_AGG_PCR0_PTA_WIN_DIS);

	mt76_wr(dev, MT_AGG_PCR0(ext_phy, 1), MT_AGG_PCR1_RTS0_NUM_THRES |
		MT_AGG_PCR1_RTS0_LEN_THRES);

	mt76_clear(dev, MT_AGG_MRCR(ext_phy), MT_AGG_MRCR_BAR_CNT_LIMIT |
		   MT_AGG_MRCR_LAST_RTS_CTS_RN | MT_AGG_MRCR_RTS_FAIL_LIMIT |
		   MT_AGG_MRCR_TXCMD_RTS_FAIL_LIMIT);

	mt76_rmw(dev, MT_AGG_MRCR(ext_phy), MT_AGG_MRCR_RTS_FAIL_LIMIT |
		 MT_AGG_MRCR_TXCMD_RTS_FAIL_LIMIT,
		 FIELD_PREP(MT_AGG_MRCR_RTS_FAIL_LIMIT, 1) |
		 FIELD_PREP(MT_AGG_MRCR_TXCMD_RTS_FAIL_LIMIT, 1));

	mt76_wr(dev, MT_TMAC_TFCR0(ext_phy), 0);
	mt76_clear(dev, MT_TMAC_TCR0(ext_phy), MT_TMAC_TCR0_TBTT_STOP_CTRL);
}

static void
mt7915_tm_init(struct mt7915_dev *dev)
{
	bool en = !(dev->mt76.test.state == MT76_TM_STATE_OFF);

	if (!test_bit(MT76_STATE_RUNNING, &dev->phy.mt76->state))
		return;

	mt7915_tm_mode_ctrl(dev, en);
	mt7915_tm_reg_backup_restore(dev, &dev->phy);
	mt7915_tm_set_trx(dev, &dev->phy, TM_MAC_TXRX, !en);
}

static void
mt7915_tm_set_tx_frames(struct mt7915_dev *dev, bool en)
{
	static const u8 spe_idx_map[] = {0, 0, 1, 0, 3, 2, 4, 0,
					 9, 8, 6, 10, 16, 12, 18, 0};
	struct sk_buff *skb = dev->mt76.test.tx_skb;
	struct ieee80211_tx_info *info;

	mt7915_tm_set_trx(dev, &dev->phy, TM_MAC_RX_RXV, false);

	if (en) {
		u8 tx_ant = dev->mt76.test.tx_antenna_mask;

		mutex_unlock(&dev->mt76.mutex);
		mt7915_set_channel(&dev->phy);
		mutex_lock(&dev->mt76.mutex);

		mt7915_mcu_set_chan_info(&dev->phy, MCU_EXT_CMD_SET_RX_PATH);
		dev->test.spe_idx = spe_idx_map[tx_ant];
	}

	mt7915_tm_set_trx(dev, &dev->phy, TM_MAC_TX, en);

	if (!en || !skb)
		return;

	info = IEEE80211_SKB_CB(skb);
	info->control.vif = dev->phy.monitor_vif;
}

static int
mt7915_tm_set_state(struct mt76_dev *mdev, enum mt76_testmode_state state)
{
	struct mt7915_dev *dev = container_of(mdev, struct mt7915_dev, mt76);
	struct mt76_testmode_data *td = &mdev->test;
	enum mt76_testmode_state prev_state = td->state;

	mdev->test.state = state;

	if (prev_state == MT76_TM_STATE_TX_FRAMES)
		mt7915_tm_set_tx_frames(dev, false);
	else if (state == MT76_TM_STATE_TX_FRAMES)
		mt7915_tm_set_tx_frames(dev, true);
	else if (prev_state == MT76_TM_STATE_OFF || state == MT76_TM_STATE_OFF)
		mt7915_tm_init(dev);

	return 0;
}

const struct mt76_testmode_ops mt7915_testmode_ops = {
	.set_state = mt7915_tm_set_state,
};
