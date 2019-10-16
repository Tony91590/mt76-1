// SPDX-License-Identifier: ISC
/* Copyright (C) 2019 MediaTek Inc.
 *
 * Author: Ryder Lee <ryder.lee@mediatek.com>
 *         Felix Fietkau <nbd@nbd.name>
 */

#include "mt7615.h"
#include "eeprom.h"

static int mt7615_efuse_read(struct mt7615_dev *dev, u32 base,
			     u16 addr, u8 *data)
{
	u32 val;
	int i;

	val = mt76_rr(dev, base + MT_EFUSE_CTRL);
	val &= ~(MT_EFUSE_CTRL_AIN | MT_EFUSE_CTRL_MODE);
	val |= FIELD_PREP(MT_EFUSE_CTRL_AIN, addr & ~0xf);
	val |= MT_EFUSE_CTRL_KICK;
	mt76_wr(dev, base + MT_EFUSE_CTRL, val);

	if (!mt76_poll(dev, base + MT_EFUSE_CTRL, MT_EFUSE_CTRL_KICK, 0, 1000))
		return -ETIMEDOUT;

	udelay(2);

	val = mt76_rr(dev, base + MT_EFUSE_CTRL);
	if ((val & MT_EFUSE_CTRL_AOUT) == MT_EFUSE_CTRL_AOUT ||
	    WARN_ON_ONCE(!(val & MT_EFUSE_CTRL_VALID))) {
		memset(data, 0x0, 16);
		return 0;
	}

	for (i = 0; i < 4; i++) {
		val = mt76_rr(dev, base + MT_EFUSE_RDATA(i));
		put_unaligned_le32(val, data + 4 * i);
	}

	return 0;
}

static int mt7615_efuse_init(struct mt7615_dev *dev)
{
	u32 val, base = mt7615_reg_map(dev, MT_EFUSE_BASE);
	int i, len = MT7615_EEPROM_SIZE;
	void *buf;

	val = mt76_rr(dev, base + MT_EFUSE_BASE_CTRL);
	if (val & MT_EFUSE_BASE_CTRL_EMPTY)
		return 0;

	dev->mt76.otp.data = devm_kzalloc(dev->mt76.dev, len, GFP_KERNEL);
	dev->mt76.otp.size = len;
	if (!dev->mt76.otp.data)
		return -ENOMEM;

	buf = dev->mt76.otp.data;
	for (i = 0; i + 16 <= len; i += 16) {
		int ret;

		ret = mt7615_efuse_read(dev, base, i, buf + i);
		if (ret)
			return ret;
	}

	return 0;
}

static int mt7615_eeprom_load(struct mt7615_dev *dev)
{
	int ret;

	ret = mt76_eeprom_init(&dev->mt76, MT7615_EEPROM_SIZE);
	if (ret < 0)
		return ret;

	return mt7615_efuse_init(dev);
}

static int mt7615_check_eeprom(struct mt76_dev *dev)
{
	u16 val = get_unaligned_le16(dev->eeprom.data);

	switch (val) {
	case 0x7615:
		return 0;
	default:
		return -EINVAL;
	}
}

static void mt7615_eeprom_parse_hw_cap(struct mt7615_dev *dev)
{
	u8 *eeprom = dev->mt76.eeprom.data;
	u8 tx_mask, rx_mask, max_nss;
	u32 val;

	val = FIELD_GET(MT_EE_NIC_WIFI_CONF_BAND_SEL,
			eeprom[MT_EE_WIFI_CONF]);
	switch (val) {
	case MT_EE_5GHZ:
		dev->mt76.cap.has_5ghz = true;
		break;
	case MT_EE_2GHZ:
		dev->mt76.cap.has_2ghz = true;
		break;
	default:
		dev->mt76.cap.has_2ghz = true;
		dev->mt76.cap.has_5ghz = true;
		break;
	}

	/* read tx-rx mask from eeprom */
	val = mt76_rr(dev, MT_TOP_STRAP_STA);
	max_nss = val & MT_TOP_3NSS ? 3 : 4;

	rx_mask =  FIELD_GET(MT_EE_NIC_CONF_RX_MASK,
			     eeprom[MT_EE_NIC_CONF_0]);
	if (!rx_mask || rx_mask > max_nss)
		rx_mask = max_nss;

	tx_mask =  FIELD_GET(MT_EE_NIC_CONF_TX_MASK,
			     eeprom[MT_EE_NIC_CONF_0]);
	if (!tx_mask || tx_mask > max_nss)
		tx_mask = max_nss;

	dev->chainmask = tx_mask << 8 | rx_mask;
	dev->mphy.antenna_mask = BIT(tx_mask) - 1;
}

int mt7615_eeprom_get_power_index(struct mt7615_dev *dev,
				  struct ieee80211_channel *chan,
				  u8 chain_idx)
{
	int index;

	if (chain_idx > 3)
		return -EINVAL;

	/* TSSI disabled */
	if (mt7615_ext_pa_enabled(dev, chan->band)) {
		if (chan->band == NL80211_BAND_2GHZ)
			return MT_EE_EXT_PA_2G_TARGET_POWER;
		else
			return MT_EE_EXT_PA_5G_TARGET_POWER;
	}

	/* TSSI enabled */
	if (chan->band == NL80211_BAND_2GHZ) {
		index = MT_EE_TX0_2G_TARGET_POWER + chain_idx * 6;
	} else {
		int group = mt7615_get_channel_group(chan->hw_value);

		switch (chain_idx) {
		case 1:
			index = MT_EE_TX1_5G_G0_TARGET_POWER;
			break;
		case 2:
			index = MT_EE_TX2_5G_G0_TARGET_POWER;
			break;
		case 3:
			index = MT_EE_TX3_5G_G0_TARGET_POWER;
			break;
		case 0:
		default:
			index = MT_EE_TX0_5G_G0_TARGET_POWER;
			break;
		}
		index += 5 * group;
	}

	return index;
}

static void mt7615_apply_cal_free_data(struct mt7615_dev *dev)
{
	static const u16 ical[] = {
		0x53, 0x54, 0x55, 0x56, 0x57, 0x5c, 0x5d, 0x62, 0x63, 0x68,
		0x69, 0x6e, 0x6f, 0x73, 0x74, 0x78, 0x79, 0x82, 0x83, 0x87,
		0x88, 0x8c, 0x8d, 0x91, 0x92, 0x96, 0x97, 0x9b, 0x9c, 0xa0,
		0xa1, 0xaa, 0xab, 0xaf, 0xb0, 0xb4, 0xb5, 0xb9, 0xba, 0xf4,
		0xf7, 0xff,
		0x140, 0x141, 0x145, 0x146, 0x14a, 0x14b, 0x154, 0x155, 0x159,
		0x15a, 0x15e, 0x15f, 0x163, 0x164, 0x168, 0x169, 0x16d, 0x16e,
		0x172, 0x173, 0x17c, 0x17d, 0x181, 0x182, 0x186, 0x187, 0x18b,
		0x18c
	};
	static const u16 ical_nocheck[] = {
		0x110, 0x111, 0x112, 0x113, 0x114, 0x115, 0x116, 0x117, 0x118,
		0x1b5, 0x1b6, 0x1b7, 0x3ac, 0x3ad, 0x3ae, 0x3af, 0x3b0, 0x3b1,
		0x3b2
	};
	u8 *eeprom = dev->mt76.eeprom.data;
	u8 *otp = dev->mt76.otp.data;
	int i;

	if (!otp)
		return;

	for (i = 0; i < ARRAY_SIZE(ical); i++)
		if (!otp[ical[i]])
			return;

	for (i = 0; i < ARRAY_SIZE(ical); i++)
		eeprom[ical[i]] = otp[ical[i]];

	for (i = 0; i < ARRAY_SIZE(ical_nocheck); i++)
		eeprom[ical_nocheck[i]] = otp[ical_nocheck[i]];
}

int mt7615_eeprom_init(struct mt7615_dev *dev)
{
	int ret;

	ret = mt7615_eeprom_load(dev);
	if (ret < 0)
		return ret;

	ret = mt7615_check_eeprom(&dev->mt76);
	if (ret && dev->mt76.otp.data)
		memcpy(dev->mt76.eeprom.data, dev->mt76.otp.data,
		       MT7615_EEPROM_SIZE);
	else
		mt7615_apply_cal_free_data(dev);

	mt7615_eeprom_parse_hw_cap(dev);
	memcpy(dev->mt76.macaddr, dev->mt76.eeprom.data + MT_EE_MAC_ADDR,
	       ETH_ALEN);

	mt76_eeprom_override(&dev->mt76);

	return 0;
}
