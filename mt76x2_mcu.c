/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/kernel.h>
#include <linux/firmware.h>
#include <linux/delay.h>

#include "mt76x2.h"
#include "mt76x2_mcu.h"
#include "mt76x2_eeprom.h"
#include "mt76x02_dma.h"

static int
mt76pci_load_rom_patch(struct mt76x2_dev *dev)
{
	const struct firmware *fw = NULL;
	struct mt76x02_patch_header *hdr;
	bool rom_protect = !is_mt7612(dev);
	int len, ret = 0;
	__le32 *cur;
	u32 patch_mask, patch_reg;

	if (rom_protect && !mt76_poll(dev, MT_MCU_SEMAPHORE_03, 1, 1, 600)) {
		dev_err(dev->mt76.dev,
			"Could not get hardware semaphore for ROM PATCH\n");
		return -ETIMEDOUT;
	}

	if (mt76xx_rev(dev) >= MT76XX_REV_E3) {
		patch_mask = BIT(0);
		patch_reg = MT_MCU_CLOCK_CTL;
	} else {
		patch_mask = BIT(1);
		patch_reg = MT_MCU_COM_REG0;
	}

	if (rom_protect && (mt76_rr(dev, patch_reg) & patch_mask)) {
		dev_info(dev->mt76.dev, "ROM patch already applied\n");
		goto out;
	}

	ret = request_firmware(&fw, MT7662_ROM_PATCH, dev->mt76.dev);
	if (ret)
		goto out;

	if (!fw || !fw->data || fw->size <= sizeof(*hdr)) {
		ret = -EIO;
		dev_err(dev->mt76.dev, "Failed to load firmware\n");
		goto out;
	}

	hdr = (struct mt76x02_patch_header *)fw->data;
	dev_info(dev->mt76.dev, "ROM patch build: %.15s\n", hdr->build_time);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, MT_MCU_ROM_PATCH_OFFSET);

	cur = (__le32 *) (fw->data + sizeof(*hdr));
	len = fw->size - sizeof(*hdr);
	mt76_wr_copy(dev, MT_MCU_ROM_PATCH_ADDR, cur, len);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, 0);

	/* Trigger ROM */
	mt76_wr(dev, MT_MCU_INT_LEVEL, 4);

	if (!mt76_poll_msec(dev, patch_reg, patch_mask, patch_mask, 2000)) {
		dev_err(dev->mt76.dev, "Failed to load ROM patch\n");
		ret = -ETIMEDOUT;
	}

out:
	/* release semaphore */
	if (rom_protect)
		mt76_wr(dev, MT_MCU_SEMAPHORE_03, 1);
	release_firmware(fw);
	return ret;
}

static int
mt76pci_load_firmware(struct mt76x2_dev *dev)
{
	const struct firmware *fw;
	const struct mt76x02_fw_header *hdr;
	int len, ret;
	__le32 *cur;
	u32 offset, val;

	ret = request_firmware(&fw, MT7662_FIRMWARE, dev->mt76.dev);
	if (ret)
		return ret;

	if (!fw || !fw->data || fw->size < sizeof(*hdr))
		goto error;

	hdr = (const struct mt76x02_fw_header *)fw->data;

	len = sizeof(*hdr);
	len += le32_to_cpu(hdr->ilm_len);
	len += le32_to_cpu(hdr->dlm_len);

	if (fw->size != len)
		goto error;

	val = le16_to_cpu(hdr->fw_ver);
	dev_info(dev->mt76.dev, "Firmware Version: %d.%d.%02d\n",
		 (val >> 12) & 0xf, (val >> 8) & 0xf, val & 0xf);

	val = le16_to_cpu(hdr->build_ver);
	dev_info(dev->mt76.dev, "Build: %x\n", val);
	dev_info(dev->mt76.dev, "Build Time: %.16s\n", hdr->build_time);

	cur = (__le32 *) (fw->data + sizeof(*hdr));
	len = le32_to_cpu(hdr->ilm_len);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, MT_MCU_ILM_OFFSET);
	mt76_wr_copy(dev, MT_MCU_ILM_ADDR, cur, len);

	cur += len / sizeof(*cur);
	len = le32_to_cpu(hdr->dlm_len);

	if (mt76xx_rev(dev) >= MT76XX_REV_E3)
		offset = MT_MCU_DLM_ADDR_E3;
	else
		offset = MT_MCU_DLM_ADDR;

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, MT_MCU_DLM_OFFSET);
	mt76_wr_copy(dev, offset, cur, len);

	mt76_wr(dev, MT_MCU_PCIE_REMAP_BASE4, 0);

	val = mt76x2_eeprom_get(dev, MT_EE_NIC_CONF_2);
	if (FIELD_GET(MT_EE_NIC_CONF_2_XTAL_OPTION, val) == 1)
		mt76_set(dev, MT_MCU_COM_REG0, BIT(30));

	/* trigger firmware */
	mt76_wr(dev, MT_MCU_INT_LEVEL, 2);
	if (!mt76_poll_msec(dev, MT_MCU_COM_REG0, 1, 1, 200)) {
		dev_err(dev->mt76.dev, "Firmware failed to start\n");
		release_firmware(fw);
		return -ETIMEDOUT;
	}

	dev_info(dev->mt76.dev, "Firmware running!\n");

	release_firmware(fw);

	return ret;

error:
	dev_err(dev->mt76.dev, "Invalid firmware\n");
	release_firmware(fw);
	return -ENOENT;
}

int mt76x2_mcu_load_cr(struct mt76x2_dev *dev, u8 type, u8 temp_level,
		       u8 channel)
{
	struct sk_buff *skb;
	struct {
		u8 cr_mode;
		u8 temp;
		u8 ch;
		u8 _pad0;

		__le32 cfg;
	} __packed __aligned(4) msg = {
		.cr_mode = type,
		.temp = temp_level,
		.ch = channel,
	};
	u32 val;

	val = BIT(31);
	val |= (mt76x2_eeprom_get(dev, MT_EE_NIC_CONF_0) >> 8) & 0x00ff;
	val |= (mt76x2_eeprom_get(dev, MT_EE_NIC_CONF_1) << 8) & 0xff00;
	msg.cfg = cpu_to_le32(val);

	/* first set the channel without the extension channel info */
	skb = mt76_mcu_msg_alloc(dev, &msg, sizeof(msg));
	return mt76_mcu_send_msg(dev, skb, CMD_LOAD_CR, true);
}

int mt76x2_mcu_set_channel(struct mt76x2_dev *dev, u8 channel, u8 bw,
			   u8 bw_index, bool scan)
{
	struct sk_buff *skb;
	struct {
		u8 idx;
		u8 scan;
		u8 bw;
		u8 _pad0;

		__le16 chainmask;
		u8 ext_chan;
		u8 _pad1;

	} __packed __aligned(4) msg = {
		.idx = channel,
		.scan = scan,
		.bw = bw,
		.chainmask = cpu_to_le16(dev->chainmask),
	};

	/* first set the channel without the extension channel info */
	skb = mt76_mcu_msg_alloc(dev, &msg, sizeof(msg));
	mt76_mcu_send_msg(dev, skb, CMD_SWITCH_CHANNEL_OP, true);

	usleep_range(5000, 10000);

	msg.ext_chan = 0xe0 + bw_index;
	skb = mt76_mcu_msg_alloc(dev, &msg, sizeof(msg));
	return mt76_mcu_send_msg(dev, skb, CMD_SWITCH_CHANNEL_OP, true);
}

int mt76x2_mcu_calibrate(struct mt76x2_dev *dev, enum mcu_calibration type,
			 u32 param)
{
	struct sk_buff *skb;
	struct {
		__le32 id;
		__le32 value;
	} __packed __aligned(4) msg = {
		.id = cpu_to_le32(type),
		.value = cpu_to_le32(param),
	};
	int ret;

	mt76_clear(dev, MT_MCU_COM_REG0, BIT(31));

	skb = mt76_mcu_msg_alloc(dev, &msg, sizeof(msg));
	ret = mt76_mcu_send_msg(dev, skb, CMD_CALIBRATION_OP, true);
	if (ret)
		return ret;

	if (WARN_ON(!mt76_poll_msec(dev, MT_MCU_COM_REG0,
				    BIT(31), BIT(31), 100)))
		return -ETIMEDOUT;

	return 0;
}

int mt76x2_mcu_tssi_comp(struct mt76x2_dev *dev,
			 struct mt76x2_tssi_comp *tssi_data)
{
	struct sk_buff *skb;
	struct {
		__le32 id;
		struct mt76x2_tssi_comp data;
	} __packed __aligned(4) msg = {
		.id = cpu_to_le32(MCU_CAL_TSSI_COMP),
		.data = *tssi_data,
	};

	skb = mt76_mcu_msg_alloc(dev, &msg, sizeof(msg));
	return mt76_mcu_send_msg(dev, skb, CMD_CALIBRATION_OP, true);
}

int mt76x2_mcu_init_gain(struct mt76x2_dev *dev, u8 channel, u32 gain,
			 bool force)
{
	struct sk_buff *skb;
	struct {
		__le32 channel;
		__le32 gain_val;
	} __packed __aligned(4) msg = {
		.channel = cpu_to_le32(channel),
		.gain_val = cpu_to_le32(gain),
	};

	if (force)
		msg.channel |= cpu_to_le32(BIT(31));

	skb = mt76_mcu_msg_alloc(dev, &msg, sizeof(msg));
	return mt76_mcu_send_msg(dev, skb, CMD_INIT_GAIN_OP, true);
}

int mt76x2_mcu_init(struct mt76x2_dev *dev)
{
	static const struct mt76_mcu_ops mt76x2_mcu_ops = {
		.mcu_msg_alloc = mt76x02_mcu_msg_alloc,
		.mcu_send_msg = mt76x02_mcu_msg_send,
	};
	int ret;

	dev->mt76.mcu_ops = &mt76x2_mcu_ops;

	ret = mt76pci_load_rom_patch(dev);
	if (ret)
		return ret;

	ret = mt76pci_load_firmware(dev);
	if (ret)
		return ret;

	mt76x02_mcu_function_select(&dev->mt76, Q_SELECT, 1, true);
	return 0;
}
