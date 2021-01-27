/* SPDX-License-Identifier: ISC */
/* Copyright (C) 2020 MediaTek Inc. */

#ifndef __MT7921_H
#define __MT7921_H

#include <linux/interrupt.h>
#include <linux/ktime.h>
#include "../mt76.h"
#include "regs.h"

#define MT7921_MAX_INTERFACES		4
#define MT7921_MAX_WMM_SETS		4
#define MT7921_WTBL_SIZE		20
#define MT7921_WTBL_RESERVED		(MT7921_WTBL_SIZE - 1)
#define MT7921_WTBL_STA			(MT7921_WTBL_RESERVED - \
					 MT7921_MAX_INTERFACES)

#define MT7921_HW_SCAN_TIMEOUT		(HZ / 10)
#define MT7921_WATCHDOG_TIME		(HZ / 10)
#define MT7921_RESET_TIMEOUT		(30 * HZ)

#define MT7921_TX_RING_SIZE		2048
#define MT7921_TX_MCU_RING_SIZE		256
#define MT7921_TX_FWDL_RING_SIZE	128

#define MT7921_RX_RING_SIZE		1536
#define MT7921_RX_MCU_RING_SIZE		512

#define MT7921_FIRMWARE_WM		"mediatek/WIFI_RAM_CODE_MT7961_1.bin"
#define MT7921_ROM_PATCH		"mediatek/WIFI_MT7961_patch_mcu_1_2_hdr.bin"

#define MT7921_EEPROM_SIZE		3584
#define MT7921_TOKEN_SIZE		8192
#define MT7921_TOKEN_FREE_THR		64

#define MT7921_CFEND_RATE_DEFAULT	0x49	/* OFDM 24M */
#define MT7921_CFEND_RATE_11B		0x03	/* 11B LP, 11M */
#define MT7921_5G_RATE_DEFAULT		0x4b	/* OFDM 6M */
#define MT7921_2G_RATE_DEFAULT		0x0	/* CCK 1M */

#define MT7921_SKU_RATE_NUM		161
#define MT7921_SKU_MAX_DELTA_IDX	MT7921_SKU_RATE_NUM
#define MT7921_SKU_TABLE_SIZE		(MT7921_SKU_RATE_NUM + 1)

#define MT7921_SCAN_IE_LEN		600
#define MT7921_MAX_SCHED_SCAN_INTERVAL	10
#define MT7921_MAX_SCHED_SCAN_SSID	10
#define MT7921_MAX_SCAN_MATCH		16

struct mt7921_vif;
struct mt7921_sta;

enum mt7921_txq_id {
	MT7921_TXQ_BAND0,
	MT7921_TXQ_BAND1,
	MT7921_TXQ_FWDL = 16,
	MT7921_TXQ_MCU_WM,
};

enum mt7921_rxq_id {
	MT7921_RXQ_BAND0 = 0,
	MT7921_RXQ_BAND1,
	MT7921_RXQ_MCU_WM = 0,
};

struct mt7921_sta_stats {
	struct rate_info prob_rate;
	struct rate_info tx_rate;

	unsigned long per;
	unsigned long changed;
	unsigned long jiffies;
};

struct mt7921_sta_key_conf {
	s8 keyidx;
	u8 key[16];
};

struct mt7921_sta {
	struct mt76_wcid wcid; /* must be first */

	struct mt7921_vif *vif;

	struct list_head stats_list;
	struct list_head poll_list;
	u32 airtime_ac[8];

	struct mt7921_sta_stats stats;

	unsigned long ampdu_state;

	struct mt7921_sta_key_conf bip;
};

struct mt7921_vif {
	struct mt76_vif mt76; /* must be first */

	struct mt7921_sta sta;
	struct mt7921_phy *phy;

	struct ieee80211_tx_queue_params queue_params[IEEE80211_NUM_ACS];
};

struct mib_stats {
	u16 ack_fail_cnt;
	u16 fcs_err_cnt;
	u16 rts_cnt;
	u16 rts_retries_cnt;
	u16 ba_miss_cnt;
};

struct mt7921_phy {
	struct mt76_phy *mt76;
	struct mt7921_dev *dev;

	struct ieee80211_sband_iftype_data iftype[2][NUM_NL80211_IFTYPES];

	struct ieee80211_vif *monitor_vif;

	u32 rxfilter;
	u64 omac_mask;

	u16 noise;

	s16 coverage_class;
	u8 slottime;

	__le32 rx_ampdu_ts;
	u32 ampdu_ref;

	struct mib_stats mib;
	struct list_head stats_list;

	u8 sta_work_count;

	struct sk_buff_head scan_event_list;
	struct delayed_work scan_work;
};

struct mt7921_dev {
	union { /* must be first */
		struct mt76_dev mt76;
		struct mt76_phy mphy;
	};

	const struct mt76_bus_ops *bus_ops;
	struct mt7921_phy phy;
	struct tasklet_struct irq_tasklet;

	u16 chainmask;

	struct work_struct init_work;
	struct work_struct reset_work;
	wait_queue_head_t reset_wait;
	u32 reset_state;

	struct list_head sta_poll_list;
	spinlock_t sta_poll_lock;

	spinlock_t token_lock;
	int token_count;
	struct idr token;

	u8 fw_debug;
};

enum {
	MT_LMAC_AC00,
	MT_LMAC_AC01,
	MT_LMAC_AC02,
	MT_LMAC_AC03,
	MT_LMAC_ALTX0 = 0x10,
	MT_LMAC_BMC0,
	MT_LMAC_BCN0,
};

static inline struct mt7921_phy *
mt7921_hw_phy(struct ieee80211_hw *hw)
{
	struct mt76_phy *phy = hw->priv;

	return phy->priv;
}

static inline struct mt7921_dev *
mt7921_hw_dev(struct ieee80211_hw *hw)
{
	struct mt76_phy *phy = hw->priv;

	return container_of(phy->dev, struct mt7921_dev, mt76);
}

static inline u8 mt7921_lmac_mapping(struct mt7921_dev *dev, u8 ac)
{
	/* LMAC uses the reverse order of mac80211 AC indexes */
	return 3 - ac;
}

extern const struct ieee80211_ops mt7921_ops;
extern struct pci_driver mt7921_pci_driver;

u32 mt7921_reg_map(struct mt7921_dev *dev, u32 addr);

int mt7921_register_device(struct mt7921_dev *dev);
void mt7921_unregister_device(struct mt7921_dev *dev);
int mt7921_eeprom_init(struct mt7921_dev *dev);
void mt7921_eeprom_parse_band_config(struct mt7921_phy *phy);
int mt7921_eeprom_get_target_power(struct mt7921_dev *dev,
				   struct ieee80211_channel *chan,
				   u8 chain_idx);
void mt7921_eeprom_init_sku(struct mt7921_dev *dev);
int mt7921_dma_init(struct mt7921_dev *dev);
void mt7921_dma_prefetch(struct mt7921_dev *dev);
void mt7921_dma_cleanup(struct mt7921_dev *dev);
int mt7921_mcu_init(struct mt7921_dev *dev);
int mt7921_mcu_add_bss_info(struct mt7921_phy *phy,
			    struct ieee80211_vif *vif, int enable);
int mt7921_mcu_add_key(struct mt7921_dev *dev, struct ieee80211_vif *vif,
		       struct mt7921_sta *msta, struct ieee80211_key_conf *key,
		       enum set_key_cmd cmd);
int mt7921_set_channel(struct mt7921_phy *phy);
int mt7921_mcu_set_chan_info(struct mt7921_phy *phy, int cmd);
int mt7921_mcu_set_tx(struct mt7921_dev *dev, struct ieee80211_vif *vif);
int mt7921_mcu_set_eeprom(struct mt7921_dev *dev);
int mt7921_mcu_get_eeprom(struct mt7921_dev *dev, u32 offset);
int mt7921_mcu_get_rx_rate(struct mt7921_phy *phy, struct ieee80211_vif *vif,
			   struct ieee80211_sta *sta, struct rate_info *rate);
int mt7921_mcu_fw_log_2_host(struct mt7921_dev *dev, u8 ctrl);
void mt7921_mcu_rx_event(struct mt7921_dev *dev, struct sk_buff *skb);
void mt7921_mcu_exit(struct mt7921_dev *dev);

static inline void mt7921_irq_enable(struct mt7921_dev *dev, u32 mask)
{
	mt76_set_irq_mask(&dev->mt76, 0, 0, mask);

	tasklet_schedule(&dev->irq_tasklet);
}

static inline u32
mt7921_reg_map_l1(struct mt7921_dev *dev, u32 addr)
{
	u32 offset = FIELD_GET(MT_HIF_REMAP_L1_OFFSET, addr);
	u32 base = FIELD_GET(MT_HIF_REMAP_L1_BASE, addr);

	mt76_rmw_field(dev, MT_HIF_REMAP_L1, MT_HIF_REMAP_L1_MASK, base);
	/* use read to push write */
	mt76_rr(dev, MT_HIF_REMAP_L1);

	return MT_HIF_REMAP_BASE_L1 + offset;
}

static inline u32
mt7921_l1_rr(struct mt7921_dev *dev, u32 addr)
{
	return mt76_rr(dev, mt7921_reg_map_l1(dev, addr));
}

static inline void
mt7921_l1_wr(struct mt7921_dev *dev, u32 addr, u32 val)
{
	mt76_wr(dev, mt7921_reg_map_l1(dev, addr), val);
}

static inline u32
mt7921_l1_rmw(struct mt7921_dev *dev, u32 addr, u32 mask, u32 val)
{
	val |= mt7921_l1_rr(dev, addr) & ~mask;
	mt7921_l1_wr(dev, addr, val);

	return val;
}

#define mt7921_l1_set(dev, addr, val)	mt7921_l1_rmw(dev, addr, 0, val)
#define mt7921_l1_clear(dev, addr, val)	mt7921_l1_rmw(dev, addr, val, 0)

bool mt7921_mac_wtbl_update(struct mt7921_dev *dev, int idx, u32 mask);
void mt7921_mac_reset_counters(struct mt7921_phy *phy);
void mt7921_mac_write_txwi(struct mt7921_dev *dev, __le32 *txwi,
			   struct sk_buff *skb, struct mt76_wcid *wcid,
			   struct ieee80211_key_conf *key, bool beacon);
void mt7921_mac_set_timing(struct mt7921_phy *phy);
int mt7921_mac_fill_rx(struct mt7921_dev *dev, struct sk_buff *skb);
void mt7921_mac_fill_rx_vector(struct mt7921_dev *dev, struct sk_buff *skb);
void mt7921_mac_tx_free(struct mt7921_dev *dev, struct sk_buff *skb);
int mt7921_mac_sta_add(struct mt76_dev *mdev, struct ieee80211_vif *vif,
		       struct ieee80211_sta *sta);
void mt7921_mac_sta_remove(struct mt76_dev *mdev, struct ieee80211_vif *vif,
			   struct ieee80211_sta *sta);
void mt7921_mac_work(struct work_struct *work);
void mt7921_mac_reset_work(struct work_struct *work);
int mt7921_tx_prepare_skb(struct mt76_dev *mdev, void *txwi_ptr,
			  enum mt76_txq_id qid, struct mt76_wcid *wcid,
			  struct ieee80211_sta *sta,
			  struct mt76_tx_info *tx_info);
void mt7921_tx_complete_skb(struct mt76_dev *mdev, struct mt76_queue_entry *e);
int mt7921_init_tx_queues(struct mt7921_phy *phy, int idx, int n_desc);
void mt7921_tx_token_put(struct mt7921_dev *dev);
void mt7921_queue_rx_skb(struct mt76_dev *mdev, enum mt76_rxq_id q,
			 struct sk_buff *skb);
void mt7921_sta_ps(struct mt76_dev *mdev, struct ieee80211_sta *sta, bool ps);
void mt7921_stats_work(struct work_struct *work);
void mt7921_txp_skb_unmap(struct mt76_dev *dev,
			  struct mt76_txwi_cache *txwi);
void mt7921_set_stream_he_caps(struct mt7921_phy *phy);
void mt7921_update_channel(struct mt76_dev *mdev);
int mt7921_init_debugfs(struct mt7921_dev *dev);

int mt7921_mcu_uni_tx_ba(struct mt7921_dev *dev,
			 struct ieee80211_ampdu_params *params,
			 bool enable);
int mt7921_mcu_uni_rx_ba(struct mt7921_dev *dev,
			 struct ieee80211_ampdu_params *params,
			 bool enable);
void mt7921_scan_work(struct work_struct *work);
int mt7921_mcu_hw_scan(struct mt7921_phy *phy, struct ieee80211_vif *vif,
		       struct ieee80211_scan_request *scan_req);
int mt7921_mcu_sched_scan_req(struct mt7921_phy *phy,
			      struct ieee80211_vif *vif,
			      struct cfg80211_sched_scan_request *sreq);
int mt7921_mcu_sched_scan_enable(struct mt7921_phy *phy,
				 struct ieee80211_vif *vif,
				 bool enable);
int mt7921_mcu_cancel_hw_scan(struct mt7921_phy *phy,
			      struct ieee80211_vif *vif);
u32 mt7921_get_wtbl_info(struct mt7921_dev *dev, u16 wlan_idx);
int mt7921_mcu_uni_bss_ps(struct mt7921_dev *dev, struct ieee80211_vif *vif);
int mt7921_mcu_uni_bss_bcnft(struct mt7921_dev *dev, struct ieee80211_vif *vif,
			     bool enable);
int mt7921_mcu_set_bss_pm(struct mt7921_dev *dev, struct ieee80211_vif *vif,
			  bool enable);
int mt7921_mcu_set_hif_suspend(struct mt7921_dev *dev, bool suspend);
void mt7921_mcu_set_suspend_iter(void *priv, u8 *mac,
				 struct ieee80211_vif *vif);
int mt7921_mcu_update_gtk_rekey(struct ieee80211_hw *hw,
				struct ieee80211_vif *vif,
				struct cfg80211_gtk_rekey_data *key);
int mt7921_mcu_update_arp_filter(struct ieee80211_hw *hw,
				 struct ieee80211_vif *vif,
				 struct ieee80211_bss_conf *info);
#endif
