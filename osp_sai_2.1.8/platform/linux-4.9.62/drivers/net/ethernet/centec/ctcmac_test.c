/*
 * Centec CpuMac Ethernet Driver For Test -- CpuMac controller implementation
 * Provides Bus interface for MIIM regs
 *
 * Author: liuht <liuht@centecnetworks.com>
 *
 * Copyright 2002-2018, Centec Networks (Suzhou) Co., Ltd.
 *
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/net_tstamp.h>

#include <asm/io.h>
#include <dt-bindings/pinctrl/pinctrl-ctc.h>
#include <soc/ctc/sysctl.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/iopoll.h>

#include "ctcmac.h"
#include "ctcmac_reg.h"

enum ctcmac_test_field {
	CTCMAC_RING_SIZE = 0,
	CTCMAC_PAYLOAD_SIZE = 1,
	CTCMAC_RXTS_EN = 2,
	CTCMAC_RXTS_DUMP = 3,
	CTCMAC_TXTS_EN = 4,
	CTCMAC_TXTS_DUMP = 5,
	CTCMAC_TXINFO_VAL1 = 6,
	CTCMAC_TXINFO_VAL2 = 7,
	CTCMAC_TXINFO_VAL3 = 8,
	CTCMAC_TXINFO_VAL4 = 9,
};

struct ctcmac_pkt_stats{
	u64 rx_good_ucast_bytes;
	u64 rx_good_ucast_pkt  ;
	u64 rx_good_mcast_bytes;
	u64 rx_good_mcast_pkt  ;
	u64 rx_good_bcast_bytes;
	u64 rx_good_bcast_pkt  ;
	u64 rx_good_pause_bytes;
	u64 rx_good_pause_pkt  ;
	u64 rx_good_pfc_bytes  ;
	u64 rx_good_pfc_pkt    ;
	u64 rx_good_control_bytes;
	u64 rx_good_control_pkt;
	u64 rx_fcs_error_bytes ;
	u64 rx_fcs_error_pkt   ;
	u64 rx_mac_overrun_bytes;
	u64 rx_mac_overrun_pkt ;
	u64 rx_good_63B_bytes  ;
	u64 rx_good_63B_pkt    ;
	u64 rx_bad_63B_bytes   ;
	u64 rx_bad_63B_pkt     ;
	u64 rx_good_mtu2B_bytes;
	u64 rx_good_mtu2B_pkt  ;
	u64 rx_bad_mtu2B_bytes ;
	u64 rx_bad_mtu2B_pkt   ;
	u64 rx_good_jumbo_bytes;
	u64 rx_good_jumbo_pkt  ;
	u64 rx_bad_jumbo_bytes ;
	u64 rx_bad_jumbo_pkt   ;
	u64 rx_64B_bytes       ;
	u64 rx_64B_pkt         ;
	u64 rx_127B_bytes      ;
	u64 rx_127B_pkt        ;
	u64 rx_255B_bytes      ;
	u64 rx_255B_pkt        ;
	u64 rx_511B_bytes      ;
	u64 rx_511B_pkt        ;
	u64 rx_1023B_bytes     ;
	u64 rx_1023B_pkt       ;
	u64 rx_mtu1B_bytes     ;
	u64 rx_mtu1B_pkt       ;
	u64 tx_ucast_bytes     ;
	u64 tx_ucast_pkt       ;
	u64 tx_mcast_bytes     ;
	u64 tx_mcast_pkt       ;
	u64 tx_bcast_bytes     ;
	u64 tx_bcast_pkt       ;
	u64 tx_pause_bytes     ;
	u64 tx_pause_pkt       ;
	u64 tx_control_bytes   ;
	u64 tx_control_pkt     ;
	u64 tx_fcs_error_bytes ;
	u64 tx_fcs_error_pkt   ;
	u64 tx_underrun_bytes  ;
	u64 tx_underrun_pkt    ;
	u64 tx_63B_bytes       ;
	u64 tx_63B_pkt         ;
	u64 tx_64B_bytes       ;
	u64 tx_64B_pkt         ;
	u64 tx_127B_bytes      ;
	u64 tx_127B_pkt        ;
	u64 tx_255B_bytes      ;
	u64 tx_255B_pkt        ;
	u64 tx_511B_bytes      ;
	u64 tx_511B_pkt        ;
	u64 tx_1023B_bytes     ;
	u64 tx_1023B_pkt       ;
	u64 tx_mtu1_bytes     ;
	u64 tx_mtu1_pkt       ;
	u64 tx_mtu2_bytes     ;
	u64 tx_mtu2_pkt       ;
	u64 tx_jumbo_bytes     ;
	u64 tx_jumbo_pkt       ;
	u32 mtu1;
	u32 mtu2;
};

struct ctcmac_ptp_info{
	u32 val1;
	u32 val2;
	u32 val3;
	u32 val4;
};


struct ctcmac_test_param {
	u32 ring_size;
	u32 payload_size;
	u32 rxts_en;
	u32 rxts_dump;
	u32 txts_en;
	u32 txts_dump;
	struct ctcmac_ptp_info ptp_info;
};

static int ctcmac_alloc_skb_resources(struct net_device *ndev);
static int ctcmac_free_skb_resources(struct ctcmac_private *priv);
static void cpumac_start(struct ctcmac_private *priv);
static void cpumac_halt(struct ctcmac_private *priv);
static void ctcmac_hw_init(struct ctcmac_private *priv) ;
static spinlock_t global_reglock __aligned(SMP_CACHE_BYTES);
static int g_reglock_init_done = 0;
static int g_mac_unit_init_done=0;
static struct ctcmac_test_param test_param[2];
static struct ctcmac_pkt_stats pkt_stats[2];
static int cpumac_unit_irq_installed = 0;
static struct regmap *regmap_base;
struct CpuMacUnit_regs *g_cpumacu_reg;
/* get cpumac register : just for test */
static int ctcmac_ethtool_get_eeprom(struct net_device *netdev,
				       struct ethtool_eeprom *ee, u8 *data)
{
	u8 *iobase;
	u32 val;
	unsigned long flags;
	struct ctcmac_private *priv =  netdev_priv(netdev);


	if(ee->offset >= 0x00010000){
		iobase = (u8 *)priv->cpumacu_reg;
		ee->offset -= (0x00010000 + CPUMACUNIT_REG_BASE);
	}else  if(ee->offset >= 0x00004000){
		iobase = (u8 *)priv->cpumac_mem;
		ee->offset -= 0x00004000;
	}else{
		iobase = (u8 *)priv->cpumac_reg;
	}

	spin_lock_irqsave(&priv->reglock, flags);
	val = readl((unsigned __iomem *)(iobase + ee->offset));
	printk(KERN_ERR "0x%llx : 0x%x\n", (u64)(iobase + ee->offset), val);
	memcpy(data, (u8 *)&val, 4);

	spin_unlock_irqrestore(&priv->reglock, flags);

	return 0;

}

static int ctcmac_fill_test_param(u32 index, u32 field, u32 val)
{

	if(field == CTCMAC_RING_SIZE){
		test_param[index].ring_size = val;
	}else if(field == CTCMAC_PAYLOAD_SIZE){
		test_param[index].payload_size = val;
	}else if(field == CTCMAC_RXTS_EN){
		test_param[index].rxts_en = val;
	}else if(field == CTCMAC_RXTS_DUMP){
		test_param[index].rxts_dump = val;
	}else if(field == CTCMAC_TXTS_EN){
		test_param[index].txts_en = val;
	}else if(field == CTCMAC_TXTS_DUMP){
		test_param[index].txts_dump = val;
	}else if(field == CTCMAC_TXINFO_VAL1){
		test_param[index].ptp_info.val1= val;
	}else if(field == CTCMAC_TXINFO_VAL2){
		test_param[index].ptp_info.val2= val;
	}else if(field == CTCMAC_TXINFO_VAL3){
		test_param[index].ptp_info.val3= val;
	}else if(field == CTCMAC_TXINFO_VAL4){
		test_param[index].ptp_info.val4= val;
	}

	printk(KERN_ERR "test param:\n");
	printk(KERN_ERR "ring size : %d\n", test_param[index].ring_size);
	printk(KERN_ERR "payload size : %d\n", test_param[index].payload_size);
	printk(KERN_ERR "rxts en : %d\n", test_param[index].rxts_en);
	printk(KERN_ERR "rxts dump : %d\n", test_param[index].rxts_dump);
	printk(KERN_ERR "txts en : %d\n", test_param[index].txts_en);
	printk(KERN_ERR "txts dump : %d\n", test_param[index].txts_dump);
	printk(KERN_ERR "ptp info : 0x%x 0x%x 0x%x 0x%x\n", 
	test_param[index].ptp_info.val1,
	test_param[index].ptp_info.val2,
	test_param[index].ptp_info.val3,
	test_param[index].ptp_info.val4);
    
	return 0;
}

/* set cpumac register : just for test */
static int ctcmac_ethtool_set_eeprom(struct net_device *netdev,
				       struct ethtool_eeprom *ee, u8 *data)
{
	u8 *iobase;
	u32 val=0;
	unsigned long flags;
	struct ctcmac_private *priv =  netdev_priv(netdev);


	if(ee->offset >= 0x00030000){
		memset(&pkt_stats[priv->index], 0, sizeof(struct ctcmac_pkt_stats ));
		return 0;
	}else if(ee->offset >= 0x00020000){
		val = data[0] | (data[1]<<8)|(data[2]<<16)|(data[3]<<24);
		ctcmac_fill_test_param(priv->index, ee->offset - 0x00020000, val);
		return 0;
	}else if(ee->offset >= 0x00010000){
		iobase = (u8 *)priv->cpumacu_reg;
		ee->offset -= (0x00010000 + CPUMACUNIT_REG_BASE);
	}else  if(ee->offset >= 0x00004000){
		iobase = (u8 *)priv->cpumac_mem;
		ee->offset -= 0x00004000;
	}else{
		iobase = (u8 *)priv->cpumac_reg;
	}

	val = data[0] | (data[1]<<8)|(data[2]<<16)|(data[3]<<24);
	printk(KERN_ERR "0x%llx : 0x%x\n", (u64)(iobase + ee->offset), val);
	spin_lock_irqsave(&priv->reglock, flags);
	writel(val, (unsigned __iomem *)(iobase + ee->offset));
	spin_unlock_irqrestore(&priv->reglock, flags);

	return 0;
}

#if 0
static void ctcmac_ethtool_dump_stats(struct ctcmac_pkt_stats *stats, int index)
{
	printk("eth%d RX packet stats:\n", index);
	printk("###############################################\n");
	printk("RX bytes good ucast %lld\n",   stats->rx_good_ucast_bytes            );
	printk("RX frame good ucast %lld\n",   stats->rx_good_ucast_pkt              );
	printk("RX bytes good mcast %lld\n",   stats->rx_good_mcast_bytes            );
	printk("RX frame good mcast %lld\n",   stats->rx_good_mcast_pkt              );
	printk("RX bytes good bcast %lld\n",   stats->rx_good_bcast_bytes            );
	printk("RX frame good bcast %lld\n",   stats->rx_good_bcast_pkt              );
	printk("RX bytes good pause %lld\n",   stats->rx_good_pause_bytes            );
	printk("RX frame good pause %lld\n",   stats->rx_good_pause_pkt              );
	printk("RX bytes pfc ucast %lld\n",    stats->rx_good_pfc_bytes             );
	printk("RX frame pfc ucast %lld\n",    stats->rx_good_pfc_pkt               );
	printk("RX bytes good control %lld\n", stats->rx_good_control_bytes              );
	printk("RX frame good control %lld\n", stats->rx_good_control_pkt              );
	printk("RX bytes fcs error %lld\n",    stats->rx_fcs_error_bytes            );
	printk("RX frame fcs error %lld\n",    stats->rx_fcs_error_pkt              );
	printk("RX bytes mac overrun %lld\n",  stats->rx_mac_overrun_bytes             );
	printk("RX frame mac overrun %lld\n",  stats->rx_mac_overrun_pkt              );
	printk("RX bytes good 63B %lld\n",     stats->rx_good_63B_bytes            );
	printk("RX frame good 63B %lld\n",     stats->rx_good_63B_pkt              );
	printk("RX bytes bad 63B %lld\n",      stats->rx_bad_63B_bytes            );
	printk("RX frame bad 63B %lld\n",      stats->rx_bad_63B_pkt              );
	printk("RX bytes good 9216B %lld\n",   stats->rx_good_9216B_bytes            );
	printk("RX frame good 9216B %lld\n",   stats->rx_good_9216B_pkt              );
	printk("RX bytes bad 9216B %lld\n",    stats->rx_bad_9216B_bytes            );
	printk("RX frame bad 9216B %lld\n",    stats->rx_bad_9216B_pkt              );
	printk("RX bytes good jumbo %lld\n",   stats->rx_good_jumbo_bytes            );
	printk("RX frame good jumbo %lld\n",   stats->rx_good_jumbo_pkt              );
	printk("RX bytes bad jumbo %lld\n",    stats->rx_bad_jumbo_bytes            );
	printk("RX frame bad jumbo %lld\n",    stats->rx_bad_jumbo_pkt              );
	printk("RX bytes 64B %lld\n",          stats->rx_64B_bytes            );
	printk("RX frame 64B %lld\n",          stats->rx_64B_pkt              );
	printk("RX bytes 127B %lld\n",         stats->rx_127B_bytes            );
	printk("RX frame 127B %lld\n",         stats->rx_127B_pkt              );
	printk("RX bytes 255B %lld\n",         stats->rx_255B_bytes            );
	printk("RX frame 255B %lld\n",         stats->rx_255B_pkt              );
	printk("RX bytes 511B %lld\n",         stats->rx_511B_bytes            );
	printk("RX frame 511B %lld\n",         stats->rx_511B_pkt              );
	printk("RX bytes 1023B %lld\n",        stats->rx_1023B_bytes            );
	printk("RX frame 1023B %lld\n",        stats->rx_1023B_pkt              );
	printk("RX bytes 1518B %lld\n",        stats->rx_1518B_bytes            );
	printk("RX frame 1518B %lld\n",        stats->rx_1518B_pkt              );
	printk("###############################################\n\n");

	printk("TX packet stats:\n");
	printk("###############################################\n");
	printk("TX bytes ucast %lld\n",    stats->tx_ucast_bytes               );
	printk("TX frame ucast %lld\n",    stats->tx_ucast_pkt                 );
	printk("TX bytes mcast %lld\n",    stats->tx_mcast_bytes               );
	printk("TX frame mcast %lld\n",    stats->tx_mcast_pkt                 );
	printk("TX bytes bcast %lld\n",    stats->tx_bcast_bytes               );
	printk("TX frame bcast %lld\n",    stats->tx_bcast_pkt                 );
	printk("TX bytes pause %lld\n",    stats->tx_pause_bytes               );
	printk("TX frame pause %lld\n",    stats->tx_pause_pkt                 );
	printk("TX bytes control %lld\n",  stats->tx_control_bytes               );
	printk("TX frame control %lld\n",  stats->tx_control_pkt                 );
	printk("TX bytes fcs error %lld\n",stats->tx_fcs_error_bytes               );
	printk("TX frame fcs error %lld\n",stats->tx_fcs_error_pkt                 );
	printk("TX bytes underrun %lld\n", stats->tx_underrun_bytes               );
	printk("TX frame underrun %lld\n", stats->tx_underrun_pkt                 );
	printk("TX bytes 63B %lld\n",      stats->tx_63B_bytes               );
	printk("TX frame 63B %lld\n",      stats->tx_63B_pkt                 );
	printk("TX bytes 64B %lld\n",      stats->tx_64B_bytes               );
	printk("TX frame 64B %lld\n",      stats->tx_64B_pkt                 );
	printk("TX bytes 127B %lld\n",     stats->tx_127B_bytes               );
	printk("TX frame 127B %lld\n",     stats->tx_127B_pkt                 );
	printk("TX bytes 255B %lld\n",     stats->tx_255B_bytes               );
	printk("TX frame 255B %lld\n",     stats->tx_255B_pkt                 );
	printk("TX bytes 511B %lld\n",     stats->tx_511B_bytes               );
	printk("TX frame 511B %lld\n",     stats->tx_511B_pkt                 );
	printk("TX bytes 1023B %lld\n",    stats->tx_1023B_bytes               );
	printk("TX frame 1023B %lld\n",    stats->tx_1023B_pkt                 );
	printk("TX bytes 1518B %lld\n",    stats->tx_1518B_bytes               );
	printk("TX frame 1518B %lld\n",    stats->tx_1518B_pkt                 );
	printk("TX bytes 1519B %lld\n",    stats->tx_1519B_bytes               );
	printk("TX frame 1519B %lld\n",    stats->tx_1519B_pkt                 );
	printk("TX bytes jumbo %lld\n",    stats->tx_jumbo_bytes               );
	printk("TX frame jumbo %lld\n",    stats->tx_jumbo_pkt                 );
	printk("###############################################\n\n");
    
}
#endif

static void ctcmac_ethtool_get_stats(struct net_device *netdev, struct ethtool_regs *regs, void *regbuf)
{
	u32 mtu;
	unsigned long flags;
	struct ctcmac_pkt_stats *stats;
	struct ctcmac_private *priv =  netdev_priv(netdev);

	spin_lock_irqsave(&priv->reglock, flags);
	stats = &pkt_stats[priv->index];
	stats->rx_good_ucast_bytes      +=   readq(&priv->cpumac_mem->CpuMacStatsRam0[0]);
	stats->rx_good_ucast_pkt        +=   readq(&priv->cpumac_mem->CpuMacStatsRam0[2]);
	stats->rx_good_mcast_bytes      +=   readq(&priv->cpumac_mem->CpuMacStatsRam1[0]);
	stats->rx_good_mcast_pkt        +=   readq(&priv->cpumac_mem->CpuMacStatsRam1[2]);
	stats->rx_good_bcast_bytes      +=   readq(&priv->cpumac_mem->CpuMacStatsRam2[0]);
	stats->rx_good_bcast_pkt        +=   readq(&priv->cpumac_mem->CpuMacStatsRam2[2]);
	stats->rx_good_pause_bytes      +=   readq(&priv->cpumac_mem->CpuMacStatsRam3[0]) ;
	stats->rx_good_pause_pkt        +=   readq(&priv->cpumac_mem->CpuMacStatsRam3[2]) ;
	stats->rx_good_pfc_bytes        +=   readq(&priv->cpumac_mem->CpuMacStatsRam4[0]) ;
	stats->rx_good_pfc_pkt          +=   readq(&priv->cpumac_mem->CpuMacStatsRam4[2]) ;
	stats->rx_good_control_bytes    +=   readq(&priv->cpumac_mem->CpuMacStatsRam5[0]) ;
	stats->rx_good_control_pkt      +=   readq(&priv->cpumac_mem->CpuMacStatsRam5[2]) ;
	stats->rx_fcs_error_bytes       +=   readq(&priv->cpumac_mem->CpuMacStatsRam6[0]) ;
	stats->rx_fcs_error_pkt         +=   readq(&priv->cpumac_mem->CpuMacStatsRam6[2]) ;
	stats->rx_mac_overrun_bytes     +=   readq(&priv->cpumac_mem->CpuMacStatsRam7[0]) ;
	stats->rx_mac_overrun_pkt       +=   readq(&priv->cpumac_mem->CpuMacStatsRam7[2]) ;
	stats->rx_good_63B_bytes        +=   readq(&priv->cpumac_mem->CpuMacStatsRam8[0]) ;
	stats->rx_good_63B_pkt          +=   readq(&priv->cpumac_mem->CpuMacStatsRam8[2]) ;
	stats->rx_bad_63B_bytes         +=   readq(&priv->cpumac_mem->CpuMacStatsRam9[0]) ;
	stats->rx_bad_63B_pkt           +=   readq(&priv->cpumac_mem->CpuMacStatsRam9[2]) ;
	stats->rx_good_mtu2B_bytes      +=   readq(&priv->cpumac_mem->CpuMacStatsRam10[0]);
	stats->rx_good_mtu2B_pkt        +=   readq(&priv->cpumac_mem->CpuMacStatsRam10[2]);
	stats->rx_bad_mtu2B_bytes       +=   readq(&priv->cpumac_mem->CpuMacStatsRam11[0]);
	stats->rx_bad_mtu2B_pkt         +=   readq(&priv->cpumac_mem->CpuMacStatsRam11[2]);
	stats->rx_good_jumbo_bytes      +=   readq(&priv->cpumac_mem->CpuMacStatsRam12[0]);
	stats->rx_good_jumbo_pkt        +=   readq(&priv->cpumac_mem->CpuMacStatsRam12[2]);
	stats->rx_bad_jumbo_bytes       +=   readq(&priv->cpumac_mem->CpuMacStatsRam13[0]);
	stats->rx_bad_jumbo_pkt         +=   readq(&priv->cpumac_mem->CpuMacStatsRam13[2]);
	stats->rx_64B_bytes             +=   readq(&priv->cpumac_mem->CpuMacStatsRam14[0]);
	stats->rx_64B_pkt               +=   readq(&priv->cpumac_mem->CpuMacStatsRam14[2]);
	stats->rx_127B_bytes            +=   readq(&priv->cpumac_mem->CpuMacStatsRam15[0]);
	stats->rx_127B_pkt              +=   readq(&priv->cpumac_mem->CpuMacStatsRam15[2]);
	stats->rx_255B_bytes            +=   readq(&priv->cpumac_mem->CpuMacStatsRam16[0]);
	stats->rx_255B_pkt              +=   readq(&priv->cpumac_mem->CpuMacStatsRam16[2]);
	stats->rx_511B_bytes            +=   readq(&priv->cpumac_mem->CpuMacStatsRam17[0]);
	stats->rx_511B_pkt              +=   readq(&priv->cpumac_mem->CpuMacStatsRam17[2]);
	stats->rx_1023B_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam18[0]);
	stats->rx_1023B_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam18[2]);
	stats->rx_mtu1B_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam19[0]);
	stats->rx_mtu1B_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam19[2]);
	stats->tx_ucast_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam20[0]);
	stats->tx_ucast_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam20[2]);
	stats->tx_mcast_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam21[0]);
	stats->tx_mcast_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam21[2]);
	stats->tx_bcast_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam22[0]);
	stats->tx_bcast_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam22[2]);
	stats->tx_pause_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam23[0]);
	stats->tx_pause_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam23[2]);
	stats->tx_control_bytes         +=   readq(&priv->cpumac_mem->CpuMacStatsRam24[0]);
	stats->tx_control_pkt           +=   readq(&priv->cpumac_mem->CpuMacStatsRam24[2]);
	stats->tx_fcs_error_bytes       +=   readq(&priv->cpumac_mem->CpuMacStatsRam25[0]);
	stats->tx_fcs_error_pkt         +=   readq(&priv->cpumac_mem->CpuMacStatsRam25[2]);
	stats->tx_underrun_bytes        +=   readq(&priv->cpumac_mem->CpuMacStatsRam26[0]);
	stats->tx_underrun_pkt          +=   readq(&priv->cpumac_mem->CpuMacStatsRam26[2]);
	stats->tx_63B_bytes             +=   readq(&priv->cpumac_mem->CpuMacStatsRam27[0]);
	stats->tx_63B_pkt               +=   readq(&priv->cpumac_mem->CpuMacStatsRam27[2]);
	stats->tx_64B_bytes             +=   readq(&priv->cpumac_mem->CpuMacStatsRam28[0]);
	stats->tx_64B_pkt               +=   readq(&priv->cpumac_mem->CpuMacStatsRam28[2]);
	stats->tx_127B_bytes            +=   readq(&priv->cpumac_mem->CpuMacStatsRam29[0]);
	stats->tx_127B_pkt              +=   readq(&priv->cpumac_mem->CpuMacStatsRam29[2]);
	stats->tx_255B_bytes            +=   readq(&priv->cpumac_mem->CpuMacStatsRam30[0]);
	stats->tx_255B_pkt              +=   readq(&priv->cpumac_mem->CpuMacStatsRam30[2]);
	stats->tx_511B_bytes            +=   readq(&priv->cpumac_mem->CpuMacStatsRam31[0]);
	stats->tx_511B_pkt              +=   readq(&priv->cpumac_mem->CpuMacStatsRam31[2]);
	stats->tx_1023B_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam32[0]);
	stats->tx_1023B_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam32[2]);
	stats->tx_mtu1_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam33[0]);
	stats->tx_mtu1_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam33[2]);
	stats->tx_mtu2_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam34[0]);
	stats->tx_mtu2_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam34[2]);
	stats->tx_jumbo_bytes           +=   readq(&priv->cpumac_mem->CpuMacStatsRam35[0]);
	stats->tx_jumbo_pkt             +=   readq(&priv->cpumac_mem->CpuMacStatsRam35[2]);
	mtu                                      =   readl(&priv->cpumac_reg->CpuMacStatsCfg[1]);
	stats->mtu1                         =  mtu & 0x3fff;
	stats->mtu2                         =  (mtu>>16) & 0x3fff;
	spin_unlock_irqrestore(&priv->reglock, flags);

	memcpy(regbuf, (void *)stats, sizeof(struct ctcmac_pkt_stats));

	//ctcmac_ethtool_dump_stats(stats, priv->index);
}

static int ctcmac_ethtool_get_eeprom_len(struct net_device *netdev)
{
	return 0x40000;
}

static int ctcmac_ethtool_get_regs_len(struct net_device *netdev)
{
	return sizeof(struct ctcmac_pkt_stats);
}

const struct ethtool_ops ctcmac_ethtool_test_ops = {
	.get_eeprom	       = ctcmac_ethtool_get_eeprom,
	.set_eeprom	       = ctcmac_ethtool_set_eeprom,
	.get_eeprom_len    = ctcmac_ethtool_get_eeprom_len,
	.get_regs		       = ctcmac_ethtool_get_stats,
	.get_regs_len		 = ctcmac_ethtool_get_regs_len,
};

static void clrsetbits(unsigned __iomem *addr, u32 clr, u32 set)
{
	writel((readl(addr) & ~(clr)) | (set) , addr);
}

static u32 ctcmac_regr(unsigned __iomem *addr)
{
	u32 val;
	val = readl(addr);
	return val;
}

static void ctcmac_regw(unsigned __iomem *addr, u32 val)
{
	writel(val, addr);
}

static inline int ctcmac_rxbd_unused(struct ctcmac_priv_rx_q *rxq)
{
	if (rxq->next_to_clean > rxq->next_to_use)
		return rxq->next_to_clean - rxq->next_to_use - 1;

	return rxq->rx_ring_size + rxq->next_to_clean - rxq->next_to_use - 1;
}

static int ctcmac_alloc_tx_queues(struct ctcmac_private *priv)
{
	int i;

	for (i = 0; i < priv->num_tx_queues; i++) {
		priv->tx_queue[i] = kzalloc(sizeof(struct ctcmac_priv_tx_q),
				    GFP_KERNEL);
		if (!priv->tx_queue[i])
			return -ENOMEM;

		priv->tx_queue[i]->tx_skbuff = NULL;
		priv->tx_queue[i]->qindex = i;
		priv->tx_queue[i]->dev = priv->ndev;
		spin_lock_init(&(priv->tx_queue[i]->txlock));
	}
	return 0;
}

static int ctcmac_alloc_rx_queues(struct ctcmac_private *priv)
{
	int i;

	for (i = 0; i < priv->num_rx_queues; i++) {
	    priv->rx_queue[i] = kzalloc(sizeof(struct ctcmac_priv_rx_q),
	        	    GFP_KERNEL);
	    if (!priv->rx_queue[i])
		return -ENOMEM;

	    priv->rx_queue[i]->qindex = i;
	    priv->rx_queue[i]->ndev = priv->ndev;
	}
	return 0;
}

static void ctcmac_unmap_io_space(struct ctcmac_private *priv)
{
	if(priv->iobase)
		iounmap(priv->iobase);
}

static void ctcmac_free_tx_queues(struct ctcmac_private *priv)
{
	int i;

	for (i = 0; i < priv->num_tx_queues; i++){
		if(priv->tx_queue[i])
			kfree(priv->tx_queue[i]);
	}   
}

static void ctcmac_free_rx_queues(struct ctcmac_private *priv)
{
	int i;

	for (i = 0; i < priv->num_rx_queues; i++){
		if(priv->rx_queue[i])
			kfree(priv->rx_queue[i]);
	}
}

static void ctcmac_free_dev(struct ctcmac_private *priv)
{
	if(priv->ndev)
		free_netdev(priv->ndev);
}
 
static int ctcmac_of_init(struct platform_device *ofdev, struct net_device **pdev)
{
	int err = 0, index;
	const char *ctype;
	struct net_device *dev = NULL;
	struct ctcmac_private *priv = NULL;
	unsigned int num_tx_qs, num_rx_qs;
	struct device_node *np = ofdev->dev.of_node;

	num_tx_qs = CTCMAC_TX_QUEUE_MAX;
	num_rx_qs = CTCMAC_RX_QUEUE_MAX;

	*pdev = alloc_etherdev_mq(sizeof(*priv), num_tx_qs);
	dev = *pdev;
	if (NULL == dev)
		return -ENOMEM;

	priv = netdev_priv(dev);
	priv->ndev = dev;
	priv->ofdev = ofdev;
	priv->dev = &ofdev->dev;
	priv->dev->coherent_dma_mask = DMA_BIT_MASK(64);
	priv->num_tx_queues = num_tx_qs;
	netif_set_real_num_rx_queues(dev, num_rx_qs);
	priv->num_rx_queues = num_rx_qs;

	priv->iobase = of_iomap(np, 0);
	priv->cpumac_reg = priv->iobase + CPUMAC_REG_BASE;
	priv->cpumac_mem = priv->iobase + CPUMAC_MEM_BASE;
	priv->cpumacu_reg = of_iomap(np, 1) + CPUMACUNIT_REG_BASE;
	g_cpumacu_reg = priv->cpumacu_reg;

	err = of_property_read_u32(np, "index", &index);
	if((err == 0))
		priv->index = index;
	else
		priv->index = 0;

	err = of_property_read_string(np, "phy-connection-type", &ctype);
	if((err == 0) && !strncmp(ctype, "sgmii", 5))
		priv->interface = PHY_INTERFACE_MODE_SGMII;
	else
		priv->interface = PHY_INTERFACE_MODE_MII;

	priv->supported = SUPPORTED_10baseT_Full | SUPPORTED_10baseT_Half;
	err = of_property_read_string(np, "capability-100M", &ctype);
	if((err == 0) && !strncmp(ctype, "support", 7))
		priv->supported |= SUPPORTED_100baseT_Full | SUPPORTED_100baseT_Half;

	err = of_property_read_string(np, "capability-1000M", &ctype);
	if((err == 0) && !strncmp(ctype, "support", 7))
		priv->supported |= SUPPORTED_1000baseT_Full;

	priv->phy_node = of_parse_phandle(np, "phy-handle", 0);

	priv->irqinfo[CTCMAC_NORMAL].irq = irq_of_parse_and_map(np, 0);
	priv->irqinfo[CTCMAC_FUNC].irq = irq_of_parse_and_map(np, 1);
	priv->irqinfo[CTCMAC_UNIT].irq = irq_of_parse_and_map(np, 2);

	return 0;

}

static int startup_ctcmac(struct net_device *ndev)
{
	int i;
	int err;
	struct ctcmac_private *priv = netdev_priv(ndev);

	if(ctcmac_alloc_tx_queues(priv))
		return -1;

	if(ctcmac_alloc_rx_queues(priv))
		return -1;

	/* Initializing some of the rx/tx queue level parameters */
	for (i = 0; i < priv->num_tx_queues; i++) {
		priv->tx_queue[i]->tx_ring_size = test_param[priv->index].ring_size;
		priv->tx_queue[i]->num_txbdfree = test_param[priv->index].ring_size;
	}

	for (i = 0; i < priv->num_rx_queues; i++) {
		priv->rx_queue[i]->rx_ring_size = test_param[priv->index].ring_size;
	}

	ctcmac_hw_init(priv);

	priv->total_fill_rx = 0;
	priv->total_free_rx = 0;

	err = ctcmac_alloc_skb_resources(ndev);
	if (err)
		return err;

	smp_mb__before_atomic();
	clear_bit(CTCMAC_DOWN, &priv->state);
	smp_mb__after_atomic();

	cpumac_start(priv);
	/* force link state update after mac reset */
	priv->oldlink = 0;
	priv->oldspeed = 0;
	priv->oldduplex = -1;

	phy_start(ndev->phydev);

	napi_enable(&priv->napi_rx);
	napi_enable(&priv->napi_tx);

	netif_tx_wake_all_queues(ndev);

	return 0;
}

static void stop_ctcmac(struct net_device *ndev)
{
	struct ctcmac_private *priv = netdev_priv(ndev);

	/* disable ints and gracefully shut down Rx/Tx DMA */
	cpumac_halt(priv);	

	netif_tx_stop_all_queues(ndev);

	smp_mb__before_atomic();
	set_bit(CTCMAC_DOWN, &priv->state);
	smp_mb__after_atomic();
	napi_disable(&priv->napi_rx);
	napi_disable(&priv->napi_tx);
	phy_stop(ndev->phydev);
	ctcmac_free_skb_resources(priv);
}

static void ctcmac_reset(struct net_device *ndev)
{    
	struct ctcmac_private *priv = netdev_priv(ndev);
	while (test_and_set_bit_lock(CTCMAC_RESETTING, &priv->state))
		cpu_relax();

	stop_ctcmac(ndev);    
	startup_ctcmac(ndev);    
	clear_bit_unlock(CTCMAC_RESETTING, &priv->state);
}

/* ctcmac_reset_task gets scheduled when a packet has not been
 * transmitted after a set amount of time.
 * For now, assume that clearing out all the structures, and
 * starting over will fix the problem.
 */
static void ctcmac_reset_task(struct work_struct *work)
{
	struct ctcmac_private *priv = container_of(work, struct ctcmac_private,
						 reset_task);
	ctcmac_reset(priv->ndev);
}

static int ctcmac_rxbd_used_untreated(struct ctcmac_private *priv, int qidx)
{
	u32 count;

	if(qidx){
		count = readl(&priv->cpumac_reg->CpuMacDescMon[2]);
		return count & 0xffff;
	}

	count = readl(&priv->cpumac_reg->CpuMacDescMon[1]);

	return (count >> 16 ) & 0xffff;
}

static int ctcmac_txbd_used_untreated(struct ctcmac_private *priv)
{
	u32 count;

	count = readl(&priv->cpumac_reg->CpuMacDescMon[2]);

	return (count >> 16 ) & 0xffff;
}

static bool ctcmac_add_rx_frag(struct ctcmac_rx_buff *rxb, u32 lstatus,
			     struct sk_buff *skb, bool first)
{
	struct page *page = rxb->page;
	unsigned int size = (lstatus & CPU_MAC_DESC_INTF_W1_DESC_SIZE_MASK)>>8;

	if (likely(first)) {
		skb_put(skb, size);
	} else {
		skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, page,
				rxb->page_offset, size, CTCMAC_RXB_TRUESIZE);
	}

	/* try reuse page */
	if (unlikely(page_count(page) != 1))
		return false;

	/* change offset to the other half */
	rxb->page_offset ^= CTCMAC_RXB_TRUESIZE;

	page_ref_inc(page);

	return true;
}

static void ctcmac_reuse_rx_page(struct ctcmac_priv_rx_q *rxq,
			       struct ctcmac_rx_buff *old_rxb)
{
	struct ctcmac_rx_buff *new_rxb;
	u16 nta = rxq->next_to_alloc;

	new_rxb = &rxq->rx_buff[nta];

	/* find next buf that can reuse a page */
	nta++;
	rxq->next_to_alloc = (nta < rxq->rx_ring_size) ? nta : 0;

	/* copy page reference */
	*new_rxb = *old_rxb;

	/* sync for use by the device */
	dma_sync_single_range_for_device(rxq->dev, old_rxb->dma,
					 old_rxb->page_offset,
					 CTCMAC_RXB_TRUESIZE, DMA_FROM_DEVICE);
}

static struct sk_buff *ctcmac_get_next_rxbuff(struct ctcmac_priv_rx_q *rx_queue,
					    u32 lstatus, struct sk_buff *skb)
{
	struct ctcmac_rx_buff *rxb = &rx_queue->rx_buff[rx_queue->next_to_clean];
	struct page *page = rxb->page;
	bool first = false;

	if (likely(!skb)) {
		void *buff_addr = page_address(page) + rxb->page_offset;
		skb = build_skb(buff_addr, CTCMAC_SKBFRAG_SIZE);
		if (unlikely(!skb)) {
			return NULL;
		}
		first = true;
	}

	dma_sync_single_range_for_cpu(rx_queue->dev, rxb->dma, rxb->page_offset,
				      CTCMAC_RXB_TRUESIZE, DMA_FROM_DEVICE);

	if (ctcmac_add_rx_frag(rxb, lstatus, skb, first)) {
		/* reuse the free half of the page */
		ctcmac_reuse_rx_page(rx_queue, rxb);
	} else {
		/* page cannot be reused, unmap it */
		dma_unmap_page(rx_queue->dev, rxb->dma,
		            PAGE_SIZE, DMA_FROM_DEVICE);
	}

	/* clear rxb content */
	rxb->page = NULL;

    return skb;
}

static void ctcmac_process_frame(struct net_device *ndev, struct sk_buff *skb)
{
	struct ctcmac_private *priv = netdev_priv(ndev);

	if (test_param[priv->index].rxts_en){
		struct skb_shared_hwtstamps *shhwtstamps = skb_hwtstamps(skb);
		u64 *ns = (u64 *) (skb->data + skb->len -8);

		if (test_param[priv->index].rxts_dump){
			printk(KERN_ERR "receive frame time stamp 0x%llx\n", *ns);
		}

		memset(shhwtstamps, 0, sizeof(*shhwtstamps));
		shhwtstamps->hwtstamp = ns_to_ktime(be64_to_cpu(*ns));
		skb_pull(skb, 8);
		if (test_param[priv->index].rxts_dump){
			printk(KERN_ERR "receive frame time stamp %lld\n", shhwtstamps->hwtstamp.tv64);
		}
	}

	skb->protocol = eth_type_trans(skb, ndev);
}

static int ctc_mac_serdes_init(struct ctcmac_private *priv)
{
	int ret=0;
	u32 status;
	int delay_ms = 10;
	
	/* reset serdes */
	writel(0x83806000, &priv->cpumacu_reg->CpuMacUnitHssCfg[0]);
	writel(0x28061800, &priv->cpumacu_reg->CpuMacUnitHssCfg[2]);
	writel(0x0026c03a, &priv->cpumacu_reg->CpuMacUnitHssCfg[6]);
	writel(0x28061810, &priv->cpumacu_reg->CpuMacUnitHssCfg[8]);
	writel(0x0026c03a, &priv->cpumacu_reg->CpuMacUnitHssCfg[12]);

	/* offset0 bit1 BlkRstN */
	writel(0x83806002, &priv->cpumacu_reg->CpuMacUnitHssCfg[0]);
	mdelay(delay_ms);

	writel(0x80002309, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x80000842, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8000ea45, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);

	/* serdes 0 init */
	writel(0x83000a05, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83002008, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300640f, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83000214, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83008015, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83000116, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83001817, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83003018, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83000e24, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83008226, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83001f27, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83002028, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83002829, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300302a, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83002038, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300223a, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300523b, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x83002040, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300f141, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300014a, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8300e693, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);

	/* serdes 1 init */
	writel(0x84000a05, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84002008, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400640f, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84000214, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84008015, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84000116, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84001817, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84003018, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84000e24, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84008226, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84001f27, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84002028, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84002829, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400302a, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84002038, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400223a, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400523b, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x84002040, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400f141, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400014a, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);
	mdelay(delay_ms);
	writel(0x8400e693, &priv->cpumacu_reg->CpuMacUnitHssRegAccCtl);

	/* serdes post release */
	writel(0x83806003, &priv->cpumacu_reg->CpuMacUnitHssCfg[0]);
	writel(0x83826003, &priv->cpumacu_reg->CpuMacUnitHssCfg[0]);
	
	writel(0x28061801, &priv->cpumacu_reg->CpuMacUnitHssCfg[2]);
	writel(0x28061c01, &priv->cpumacu_reg->CpuMacUnitHssCfg[2]);
	writel(0x28071c01, &priv->cpumacu_reg->CpuMacUnitHssCfg[2]);
	
	writel(0x28061811, &priv->cpumacu_reg->CpuMacUnitHssCfg[8]);
	writel(0x28061c11, &priv->cpumacu_reg->CpuMacUnitHssCfg[8]);
	writel(0x28071c11, &priv->cpumacu_reg->CpuMacUnitHssCfg[8]);

	ret = readl_poll_timeout(&priv->cpumacu_reg->CpuMacUnitHssMon[1], status, 
		status & CPU_MAC_UNIT_HSS_MON_W1_MON_HSS_L0_DFE_RST_DONE, 1000, 2000000);
	if(ret){
		netdev_dbg(priv->ndev,"%s:wait for hss reset done fail with CpuMacUnitHssMon[1]:0x%x\n", 
			priv->ndev->name, readl(&priv->cpumacu_reg->CpuMacUnitHssMon[1]));
	}
	mdelay(delay_ms);

	return 0;
}

static void ctcmac_hw_init(struct ctcmac_private *priv)
{
	int i;
	u32 val;
	int use_extram = 0;
	
	/* two cpumac access the same cpumac unit register */
	spin_lock_irq(&global_reglock);   
	if(priv->index == 0){
		clrsetbits(&priv->cpumacu_reg->CpuMacUnitResetCtl, 
		    CPU_MAC_UNIT_RESET_CTL_W0_RESET_CORE_BASE, CPU_MAC_UNIT_RESET_CTL_W0_RESET_CORE_CPU_MAC0);
		clrsetbits(&priv->cpumacu_reg->CpuMacUnitResetCtl, 
		    CPU_MAC_UNIT_RESET_CTL_W0_RESET_CORE_CPU_MAC0, 0);
	}else{
		clrsetbits(&priv->cpumacu_reg->CpuMacUnitResetCtl, 
		    CPU_MAC_UNIT_RESET_CTL_W0_RESET_CORE_BASE, CPU_MAC_UNIT_RESET_CTL_W0_RESET_CORE_CPU_MAC1);
		clrsetbits(&priv->cpumacu_reg->CpuMacUnitResetCtl, 
		    CPU_MAC_UNIT_RESET_CTL_W0_RESET_CORE_CPU_MAC1, 0);
	}

	clrsetbits(&priv->cpumacu_reg->CpuMacUnitTsCfg, 
	            0, CPU_MAC_UNIT_TS_CFG_W0_CFG_FORCE_S_AND_NS_EN);

	spin_unlock_irq(&global_reglock);
	mdelay(10);
	
	clrsetbits(&priv->cpumac_reg->CpuMacInit, 0, CPU_MAC_INIT_DONE_W0_INIT_DONE);
	udelay(1);
	
	if (priv->interface == PHY_INTERFACE_MODE_SGMII){
		/* switch to sgmii and enable auto nego */
		val = readl(&priv->cpumac_reg->CpuMacSgmiiAutoNegCfg);
		val &= ~(CPU_MAC_SGMII_AUTO_NEG_CFG_W0_CFG_AN_ENABLE_MASK
		              | CPU_MAC_SGMII_AUTO_NEG_CFG_W0_CFG_AN_MODE_MASK);
		val |= (CSA_SGMII_MD_MASK | CSA_EN);
		writel(val, &priv->cpumac_reg->CpuMacSgmiiAutoNegCfg);
	}
	
	clrsetbits(&priv->cpumac_reg->CpuMacSgmiiCfg[0], 
	    CPU_MAC_SGMII_CFG_W0_CFG_MII_RX_LINK_FILTER_EN, 0);
	clrsetbits(&priv->cpumac_reg->CpuMacSgmiiCfg[0], 
	    0, CPU_MAC_SGMII_CFG_W0_CFG_TX_EVEN_IGNORE);

	clrsetbits(&priv->cpumac_reg->CpuMacAxiCfg, 
	    0, CPU_MAC_AXI_CFG_W0_CFG_AXI_RD_D_WORD_SWAP_EN);
	clrsetbits(&priv->cpumac_reg->CpuMacAxiCfg, 
	    0, CPU_MAC_AXI_CFG_W0_CFG_AXI_WR_D_WORD_SWAP_EN);

	clrsetbits(&priv->cpumac_reg->CpuMacGmacCfg[0], 
	    0, CPU_MAC_GMAC_CFG_W0_CFG_RX_OVERRUN_DROP_EN
	        |CPU_MAC_GMAC_CFG_W0_CFG_RX_OVERSIZE_DROP_EN);

	clrsetbits(&priv->cpumac_reg->CpuMacGmacCfg[2], 
	    CPU_MAC_GMAC_CFG_W2_CFG_TX_STRIP_CRC_EN, 0);

	clrsetbits(&priv->cpumac_reg->CpuMacGmacCfg[2], 
	    0, CPU_MAC_GMAC_CFG_W2_CFG_TX_CUT_THROUGH_EN);

	for (i = 0; i < priv->num_tx_queues; i++) {
		if(priv->tx_queue[i]->tx_ring_size > CTCMAC_INTERNAL_RING_SIZE){
			use_extram = 1;
			break;
		}
	}

	for (i = 0; i < priv->num_rx_queues; i++) {
		if(priv->rx_queue[i]->rx_ring_size > CTCMAC_INTERNAL_RING_SIZE){
			use_extram = 1;
			break;
		}
	}
	    
	if(use_extram){
		spin_lock_irq(&global_reglock); 
		//printk(KERN_ERR "ctcmac_hw_init enable extram %d\n", priv->index);
		regmap_read(regmap_base, offsetof(struct SysCtl_regs, SysMemCtl), &val);
		val |= SYS_MEM_CTL_W0_CFG_RAM_MUX_EN;
		regmap_write(regmap_base, offsetof(struct SysCtl_regs, SysMemCtl), val);
		spin_unlock_irq(&global_reglock); 
		if(priv->index == 0){
			ctcmac_regw(&priv->cpumac_reg->CpuMacExtRamCfg[1], CTCMAC0_EXSRAM_BASE);
		}else{
			ctcmac_regw(&priv->cpumac_reg->CpuMacExtRamCfg[1], CTCMAC1_EXSRAM_BASE);
		}
		ctcmac_regw(&priv->cpumac_reg->CpuMacExtRamCfg[0], test_param[priv->index].ring_size);
		clrsetbits(&priv->cpumac_reg->CpuMacExtRamCfg[0], 
		    0, CPU_MAC_EXT_RAM_CFG_W0_CFG_EXT_RAM_EN);
		
	}else{
		//printk(KERN_ERR "ctcmac_hw_init disable extram %d\n", priv->index);
		clrsetbits(&priv->cpumac_reg->CpuMacExtRamCfg[0], 
		    CPU_MAC_EXT_RAM_CFG_W0_CFG_EXT_RAM_EN, 0);
		spin_lock_irq(&global_reglock); 
		regmap_read(regmap_base, offsetof(struct SysCtl_regs, SysMemCtl), &val);
		val &= ~SYS_MEM_CTL_W0_CFG_RAM_MUX_EN;
		regmap_write(regmap_base, offsetof(struct SysCtl_regs, SysMemCtl), val);
		spin_unlock_irq(&global_reglock); 
	}

	if (test_param[priv->index].rxts_en){
		clrsetbits(&priv->cpumac_reg->CpuMacGmacCfg[0], 
		    0, CPU_MAC_GMAC_CFG_W0_CFG_RX_TS_EN);
	}

	if (test_param[priv->index].txts_en){
		clrsetbits(&priv->cpumac_reg->CpuMacGmacCfg[2], 
		    CPU_MAC_GMAC_CFG_W2_CFG_TX_WAIT_CAPTURE_TS,  
		    CPU_MAC_GMAC_CFG_W2_CFG_TX_HDR_INFO_EN);
	}

	/* clear all interrupt */
	ctcmac_regw(&priv->cpumac_reg->CpuMacInterruptFunc[1], 0xffffffff);
	ctcmac_regw(&priv->cpumac_reg->CpuMacInterruptNormal[1], 0xffffffff);
	/* mask all interrupt */
	ctcmac_regw(&priv->cpumac_reg->CpuMacInterruptFunc[2], 0xffffffff);
	ctcmac_regw(&priv->cpumac_reg->CpuMacInterruptNormal[2], 0xffffffff);

	printk("ctcmac_hw_init 0x%x\n", priv->ndev->flags);
    
}

/* update cpumac speed when phy linkup speed changed */
static noinline void ctcmac_update_link_state(struct ctcmac_private *priv, struct phy_device *phydev)
{
	int timeout=2000;
	u32 mon, cfg_rep, cfg_smp;
	int speed = phydev->speed;

	if (priv->interface != PHY_INTERFACE_MODE_SGMII)
		return ;

	if(netif_msg_link(priv)){
		netdev_dbg(priv->ndev,"link up speed is %d\n", speed);
	}

	if (phydev->link) {
		cfg_rep = readl(&priv->cpumac_reg->CpuMacSgmiiCfg[0]);
		cfg_smp = readl(&priv->cpumac_reg->CpuMacSgmiiCfg[1]);
		cfg_rep &= ~CSC_REP_MASK;
		cfg_smp &= ~CSC_SMP_MASK;
		if(speed == 1000){        
			cfg_rep |= CSC_1000M;
			cfg_smp |= CSC_1000M;
		}
		else if(speed == 100){
			cfg_rep |= CSC_100M;
			cfg_smp |= CSC_100M;
		}
		else if(speed == 10){
			cfg_rep |= CSC_10M;
			cfg_smp |= CSC_10M;
		}
		else{
			return;
		}
		writel(cfg_rep, &priv->cpumac_reg->CpuMacSgmiiCfg[0]);
		writel(cfg_smp, &priv->cpumac_reg->CpuMacSgmiiCfg[1]);

		while(timeout--){
			mon = readl(&priv->cpumac_reg->CpuMacSgmiiMon[0]);
			if((mon & CSM_ANST_MASK) == 6){
				break;
			}

			mdelay(1);
		}

		if((mon & CSM_ANST_MASK) != 6)
		{
			printk("Error! when phy link up, auto-neg status %d is not right.\n", mon);
		}
		if (!priv->oldlink)
			priv->oldlink = 1;
		
	}else{
		priv->oldlink = 0;
		priv->oldspeed = 0;
		priv->oldduplex = -1;
	}

	
	return;
}

static void adjust_link(struct net_device *dev)
{
	struct ctcmac_private *priv = netdev_priv(dev);
	struct phy_device *phydev = dev->phydev;

	if (unlikely(phydev->link != priv->oldlink ||
		     (phydev->link && (phydev->duplex != priv->oldduplex ||
				       phydev->speed != priv->oldspeed))))
		ctcmac_update_link_state(priv, phydev);
}

/* Initializes driver's PHY state, and attaches to the PHY.
 * Returns 0 on success.
 */
static int ctcmac_init_phy(struct net_device *dev)
{
	struct ctcmac_private *priv = netdev_priv(dev);
	phy_interface_t interface;
	struct phy_device *phydev;

	priv->oldlink = 0;
	priv->oldspeed = 0;
	priv->oldduplex = -1;

	interface = priv->interface;

	phydev = of_phy_connect(dev, priv->phy_node, &adjust_link, 0,
				interface);
	if (!phydev) {
		dev_err(&dev->dev, "could not attach to PHY\n");
		return -ENODEV;
	}

	/* Remove any features not supported by the controller */
	phydev->supported = priv->supported;
	phydev->advertising = phydev->supported;

	return 0;
}

static irqreturn_t ctcmac_receive(int irq, struct ctcmac_private *priv)
{
	unsigned long flags;

	if (likely(napi_schedule_prep(&priv->napi_rx))) {
		/* disable interrupt */
		spin_lock_irqsave(&priv->reglock, flags);
		writel(CTCMAC_NOR_RX0_D|CTCMAC_NOR_RX1_D, &priv->cpumac_reg->CpuMacInterruptFunc[2]);
		spin_unlock_irqrestore(&priv->reglock, flags);
		__napi_schedule(&priv->napi_rx);
	} else {
		/* clear interrupt */
		writel(CTCMAC_NOR_RX0_D|CTCMAC_NOR_RX1_D, &priv->cpumac_reg->CpuMacInterruptFunc[1]);
	}

	return IRQ_HANDLED;
}

static irqreturn_t ctcmac_transmit(int irq, struct ctcmac_private *priv)
{
	unsigned long flags;

	if (likely(napi_schedule_prep(&priv->napi_tx))) {
		/* disable interrupt */
		spin_lock_irqsave(&priv->reglock, flags);
		writel(CTCMAC_NOR_TX_D, &priv->cpumac_reg->CpuMacInterruptFunc[2]);
		spin_unlock_irqrestore(&priv->reglock, flags);
		__napi_schedule(&priv->napi_tx);

	} else {
		/* clear interrupt */
		writel(CTCMAC_NOR_TX_D, &priv->cpumac_reg->CpuMacInterruptFunc[1]);
	}

	return IRQ_HANDLED;
}

static irqreturn_t ctcmac_func(int irq, void *data)
{
	u32 event, stat, mask;
	struct ctcmac_private *priv = (struct ctcmac_private *)data;

	stat = ctcmac_regr(&priv->cpumac_reg->CpuMacInterruptFunc[0]);
	mask = ctcmac_regr(&priv->cpumac_reg->CpuMacInterruptFunc[2]);
	event = stat & ~mask;

	if((event & CTCMAC_NOR_RX0_D) || (event & CTCMAC_NOR_RX1_D)){
		ctcmac_receive(irq, priv);
	}

	if(event & CTCMAC_NOR_TX_D){
		ctcmac_transmit(irq, priv);
	}

	return IRQ_HANDLED;
}

static irqreturn_t ctcmac_unit(int irq, void *data)
{
	struct CpuMacUnit_mems *mems = (struct CpuMacUnit_mems *)(g_cpumacu_reg+0x400-CPUMACUNIT_REG_BASE);

	ctcmac_regw(&g_cpumacu_reg->CpuMacUnitInterruptFunc[2], 1);
	ctcmac_regw(&g_cpumacu_reg->CpuMacUnitInterruptFunc[1], 1);

	while(ctcmac_regr(&g_cpumacu_reg->CpuMacUnitFifoStatus)){
		printk(KERN_ERR "Tx Capture time stamp in fifo 0x%x 0x%x 0x%x\n",
		ctcmac_regr(&mems->CpuMacUnitTxCaptureTs0[0]),
		ctcmac_regr(&mems->CpuMacUnitTxCaptureTs0[1]),
		ctcmac_regr(&mems->CpuMacUnitTxCaptureTs0[2]));
	}
	ctcmac_regw(&g_cpumacu_reg->CpuMacUnitInterruptFunc[3], 1);

	return IRQ_HANDLED;
}

static irqreturn_t ctcmac_normal(int irq, void *grp_id) //TODO by liuht
{
	return IRQ_HANDLED;
}

static int ctcmac_request_irq(struct ctcmac_private *priv)
{
	int err = 0;

	err = request_irq(priv->irqinfo[CTCMAC_NORMAL].irq, ctcmac_normal, 0,
			  priv->irqinfo[CTCMAC_NORMAL].name, priv);
	if (err < 0) {
		free_irq(priv->irqinfo[CTCMAC_NORMAL].irq, priv);
	}
	enable_irq_wake(priv->irqinfo[CTCMAC_NORMAL].irq);

	err = request_irq(priv->irqinfo[CTCMAC_FUNC].irq, ctcmac_func, 0,
			  priv->irqinfo[CTCMAC_FUNC].name, priv);
	if (err < 0) {
		free_irq(priv->irqinfo[CTCMAC_FUNC].irq, priv);
	}
	enable_irq_wake(priv->irqinfo[CTCMAC_FUNC].irq);

	spin_lock_irq(&global_reglock);   
	if(priv->irqinfo[CTCMAC_UNIT].irq && !cpumac_unit_irq_installed){
		err = request_irq(priv->irqinfo[CTCMAC_UNIT].irq, ctcmac_unit, 0,
				  "cpumac_unit", NULL);
		if (err < 0) {
			free_irq(priv->irqinfo[CTCMAC_UNIT].irq, NULL);
			return err;
		}
		enable_irq_wake(priv->irqinfo[CTCMAC_UNIT].irq);
		cpumac_unit_irq_installed =1;
	}
	spin_unlock_irq(&global_reglock);   

	return err;
}

static void ctcmac_free_irq(struct ctcmac_private *priv)
{   
	free_irq(priv->irqinfo[CTCMAC_NORMAL].irq, priv);
	free_irq(priv->irqinfo[CTCMAC_FUNC].irq, priv);

	spin_lock_irq(&global_reglock);   
	if(cpumac_unit_irq_installed == 1){
		free_irq(priv->irqinfo[CTCMAC_UNIT].irq, NULL);
		cpumac_unit_irq_installed = 0;
	}
	spin_unlock_irq(&global_reglock);   
}

static bool ctcmac_new_page(struct ctcmac_priv_rx_q *rxq, struct ctcmac_rx_buff *rxb)
{
	struct page *page;
	dma_addr_t addr;

	page = dev_alloc_page();
	if (unlikely(!page))
		return false;

	addr = dma_map_page(rxq->dev, page, 0, PAGE_SIZE, DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(rxq->dev, addr))) {
		__free_page(page);

		return false;
	}

	rxb->dma = addr;
	rxb->page = page;
	rxb->page_offset = 0;

	return true;
}

static void ctcmac_fill_rxbd(struct ctcmac_private *priv, struct ctcmac_rx_buff *rxb, int qidx)
{
	u32 desc_cfg_low, desc_cfg_high;
	dma_addr_t bufaddr = rxb->dma + rxb->page_offset;

	desc_cfg_low = (bufaddr - CTC_DDR_BASE)& CPU_MAC_DESC_INTF_W0_DESC_ADDR_31_0_MASK;
	/* CPU_MAC_DESC_INTF_W1_DESC_SIZE:bit(8) */
	desc_cfg_high = (test_param[priv->index].payload_size<<8) | 
	    (((bufaddr- CTC_DDR_BASE) >> 32) & CPU_MAC_DESC_INTF_W1_DESC_ADDR_39_32_MASK);

	spin_lock_irq(&priv->reglock);
	if(qidx){
		ctcmac_regw(&priv->cpumac_mem->CpuMacDescIntf1[0], desc_cfg_low);
		smp_mb__before_atomic();
		ctcmac_regw(&priv->cpumac_mem->CpuMacDescIntf1[1], desc_cfg_high);
		priv->total_fill_rx++;

	}else{
		ctcmac_regw(&priv->cpumac_mem->CpuMacDescIntf0[0], desc_cfg_low);
		smp_mb__before_atomic();
		ctcmac_regw(&priv->cpumac_mem->CpuMacDescIntf0[1], desc_cfg_high);
		priv->total_fill_rx++;
	}

	spin_unlock_irq(&priv->reglock);
}

static void ctcmac_fill_txbd(struct ctcmac_private *priv, struct ctcmac_desc_cfg *txdesc)
{
	u32 desc_cfg_low, desc_cfg_high;

	desc_cfg_low = txdesc->addr_low;
	/* CPU_MAC_DESC_INTF_W1_DESC_SIZE:bit(8) */
	/* CPU_MAC_DESC_INTF_W1_DESC_SOP:bit(22) */
	/* CPU_MAC_DESC_INTF_W1_DESC_EOP:bit(23) */
	desc_cfg_high = txdesc->addr_high | 
	    (txdesc->size << 8) |
	    (txdesc->sop << 22) | 
	    (txdesc->eop << 23);

	spin_lock_irq(&priv->reglock);
	ctcmac_regw(&priv->cpumac_mem->CpuMacDescIntf2[0], desc_cfg_low);
	smp_mb__before_atomic();
	ctcmac_regw(&priv->cpumac_mem->CpuMacDescIntf2[1], desc_cfg_high);
	spin_unlock_irq(&priv->reglock);
}

static void ctcmac_get_txbd(struct ctcmac_private *priv)
{
	u32 lstatus;

	spin_lock_irq(&priv->reglock);
	lstatus = ctcmac_regr(&priv->cpumac_mem->CpuMacDescIntf2[0]);
	smp_mb__before_atomic();
	lstatus = ctcmac_regr(&priv->cpumac_mem->CpuMacDescIntf2[1]);
	
	spin_unlock_irq(&priv->reglock);
}

static void ctcmac_get_rxbd(struct ctcmac_private *priv, u32 *lstatus, int qidx)
{    
	spin_lock_irq(&priv->reglock);
	if(qidx){
		ctcmac_regr(&priv->cpumac_mem->CpuMacDescIntf1[0]);
		*lstatus = ctcmac_regr(&priv->cpumac_mem->CpuMacDescIntf1[1]);
	}else{
		ctcmac_regr(&priv->cpumac_mem->CpuMacDescIntf0[0]);
		*lstatus = ctcmac_regr(&priv->cpumac_mem->CpuMacDescIntf0[1]);
	}
	smp_mb__before_atomic();

	priv->total_free_rx++;
	spin_unlock_irq(&priv->reglock);
}

static void ctcmac_alloc_rx_buffs(struct ctcmac_priv_rx_q *rx_queue,
				int alloc_cnt)
{
	int i;
	int qidx = rx_queue->qindex;
	struct ctcmac_rx_buff *rxb;
	struct net_device *ndev = rx_queue->ndev;
	struct ctcmac_private *priv = netdev_priv(ndev);

	i = rx_queue->next_to_use;
	rxb = &rx_queue->rx_buff[i];

	while (alloc_cnt--) {
		/* try reuse page */
		if (unlikely(!rxb->page)) {
			if (unlikely(!ctcmac_new_page(rx_queue, rxb))) {
				break;
			}
		}

		ctcmac_fill_rxbd(priv, rxb, qidx);
		rxb++;

		if (unlikely(++i == rx_queue->rx_ring_size)) {
			i = 0;
			rxb = rx_queue->rx_buff;
		}
	}

	rx_queue->next_to_use = i;
	rx_queue->next_to_alloc = i;
}

static int ctcmac_clean_rx_ring(struct ctcmac_priv_rx_q *rx_queue, int rx_work_limit)
{
	struct net_device *ndev = rx_queue->ndev;
	struct ctcmac_private *priv = netdev_priv(ndev);
	int i, howmany = 0;
	struct sk_buff *skb = rx_queue->skb;
	int cleaned_cnt = ctcmac_rxbd_unused(rx_queue);
	unsigned int total_bytes = 0, total_pkts = 0;
	int qidx = rx_queue->qindex;

	/* Get the first full descriptor */
	i = rx_queue->next_to_clean;

	while (rx_work_limit--) {
		u32 lstatus;

		if (cleaned_cnt >= test_param[priv->index].ring_size/2) {
			ctcmac_alloc_rx_buffs(rx_queue, cleaned_cnt);
			cleaned_cnt = 0;
		}

		if(ctcmac_rxbd_used_untreated(priv, qidx)<=0)
			break;

		if(qidx != 0){
			printk(KERN_ERR "rx queue %d rx packet!\n", qidx);
		}
		ctcmac_get_rxbd(priv, &lstatus, qidx);

		/* fetch next to clean buffer from the ring */
		skb = ctcmac_get_next_rxbuff(rx_queue, lstatus, skb);       
		if (unlikely(!skb))
			break;

		cleaned_cnt++;
		howmany++;

		if (unlikely(++i == rx_queue->rx_ring_size))
			i = 0;

		rx_queue->next_to_clean = i;

		/* fetch next buffer if not the last in frame */
		if (!(lstatus & CPU_MAC_DESC_INTF_W1_DESC_EOP))
			continue;

		if (unlikely(lstatus & CPU_MAC_DESC_INTF_W1_DESC_ERR)) {
			if (!test_param[priv->index].rxts_en){
				/* discard faulty buffer */
				dev_kfree_skb(skb);
				skb = NULL;
				rx_queue->stats.rx_dropped++;
				pr_err("%s: Error with rx desc status 0x%x\n", ndev->name, lstatus);
				continue;
			}else{
				pr_err("%s: Error with rx desc status 0x%x\n", ndev->name, lstatus);
			}
		}

		/* Increment the number of packets */
		total_pkts++;
		total_bytes += skb->len;

		skb_record_rx_queue(skb, rx_queue->qindex);    
		ctcmac_process_frame(ndev, skb);
		/* Send the packet up the stack */
		napi_gro_receive(&rx_queue->napi_rx, skb);

		skb = NULL;
	}

	/* Store incomplete frames for completion */
	rx_queue->skb = skb;

	rx_queue->stats.rx_packets += total_pkts;
	rx_queue->stats.rx_bytes += total_bytes;

	if (cleaned_cnt)
		ctcmac_alloc_rx_buffs(rx_queue, cleaned_cnt);

	return howmany;
}

static void ctcmac_clean_tx_ring(struct ctcmac_priv_tx_q *tx_queue)
{
	u16 next_to_clean;
	int tqi = tx_queue->qindex;
	struct sk_buff *skb;
	struct netdev_queue *txq;
	struct	ctcmac_tx_buff *tx_buff;
	struct net_device *dev = tx_queue->dev;
	struct ctcmac_private *priv = netdev_priv(dev);

	txq = netdev_get_tx_queue(dev, tqi);
	next_to_clean = tx_queue->next_to_clean;
	while (ctcmac_txbd_used_untreated(priv)) {
	    
		ctcmac_get_txbd(priv);

		tx_buff = &tx_queue->tx_buff[next_to_clean];
		//printk(KERN_ERR "ctcmac_clean_tx_ring eth%d 0x%x 0x%x\n", priv->index, next_to_clean, tx_buff->alloc);
		skb = tx_queue->tx_skbuff[next_to_clean];
		dev_kfree_skb_any(skb);
		tx_queue->tx_skbuff[next_to_clean] = NULL;

		        
		dma_unmap_single(priv->dev, tx_buff->dma,
		    				 tx_buff->len, DMA_TO_DEVICE);
		if(tx_buff->alloc){
		    kfree(tx_buff->vaddr);
		}

		if((next_to_clean + 1) >=tx_queue->tx_ring_size){
		    next_to_clean = 0;
		}else{
		    next_to_clean++; 
		}

		spin_lock(&tx_queue->txlock);
		tx_queue->num_txbdfree++;
		spin_unlock(&tx_queue->txlock);
	}

	/* If we freed a buffer, we can restart transmission, if necessary */
	if (tx_queue->num_txbdfree &&
		netif_tx_queue_stopped(txq) &&
		!(test_bit(CTCMAC_DOWN, &priv->state))){
		netif_wake_subqueue(priv->ndev, tqi);
	}

	tx_queue->next_to_clean = next_to_clean;
}

static int ctcmac_poll_rx_sq(struct napi_struct *napi, int budget)
{
	int qidx;
	int work_done = 0;
	int rx_work_limit;
	struct ctcmac_private *priv =
		container_of(napi, struct ctcmac_private, napi_rx);
	struct ctcmac_priv_rx_q *rx_queue = NULL;

	/* clear interrupt */
	writel(CTCMAC_NOR_RX0_D|CTCMAC_NOR_RX1_D, 
	    &priv->cpumac_reg->CpuMacInterruptFunc[1]);

	rx_work_limit = budget;
	for(qidx=priv->num_rx_queues-1; qidx>=0; qidx--){
		rx_queue = priv->rx_queue[qidx];
		work_done += ctcmac_clean_rx_ring(rx_queue, rx_work_limit);
		rx_work_limit -= work_done;
	}

	if (work_done < budget) {
		napi_complete(napi);
		/* enable interrupt */
		spin_lock_irq(&priv->reglock);
		writel(CTCMAC_NOR_RX0_D|CTCMAC_NOR_RX1_D, 
		&priv->cpumac_reg->CpuMacInterruptFunc[3]);
		spin_unlock_irq(&priv->reglock);
	}

	return work_done;
}

static int ctcmac_poll_tx_sq(struct napi_struct *napi, int budget) //TODO by liuht
{
	struct ctcmac_private *priv =
		container_of(napi, struct ctcmac_private, napi_tx);
	struct ctcmac_priv_tx_q *tx_queue = priv->tx_queue[0];

	/* clear interrupt */
	writel(CTCMAC_NOR_TX_D, &priv->cpumac_reg->CpuMacInterruptFunc[1]);

	ctcmac_clean_tx_ring(tx_queue);

	napi_complete(napi);
	/* enable interrupt */
	spin_lock_irq(&priv->reglock);
	writel(CTCMAC_NOR_TX_D, 
	    &priv->cpumac_reg->CpuMacInterruptFunc[3]);
	spin_unlock_irq(&priv->reglock);

	return 0;
}

static void ctcmac_free_rx_resources(struct ctcmac_private *priv)
{
	int i, j;
	struct ctcmac_priv_rx_q *rx_queue = NULL;

	for (i = 0; i < priv->num_rx_queues; i++) {
		rx_queue = priv->rx_queue[i]; 
		if (rx_queue->skb)
			dev_kfree_skb(rx_queue->skb);

		for (j = 0; j < rx_queue->rx_ring_size; j++){
			struct	ctcmac_rx_buff *rxb = &rx_queue->rx_buff[j];
			if(!rxb->page)
				continue;
			dma_unmap_single(rx_queue->dev, rxb->dma,
					 PAGE_SIZE, DMA_TO_DEVICE);
			__free_page(rxb->page);
		}
		if(rx_queue->rx_buff){
			kfree(rx_queue->rx_buff);
			rx_queue->rx_buff = NULL;
		}
	}
}

static int ctcmac_init_rx_resources(struct net_device *ndev)
{
	int i;
	struct ctcmac_private *priv = netdev_priv(ndev);
	struct device *dev = priv->dev;
	struct ctcmac_priv_rx_q *rx_queue = NULL;

	for (i = 0; i < priv->num_rx_queues; i++) {
		rx_queue = priv->rx_queue[i]; 
		rx_queue->ndev = ndev;
		rx_queue->dev = dev;
		rx_queue->next_to_clean = 0;
		rx_queue->next_to_use = 0;
		rx_queue->next_to_alloc = 0;
		rx_queue->skb = NULL;
		rx_queue->rx_buff = kcalloc(rx_queue->rx_ring_size,
					    sizeof(*rx_queue->rx_buff),
					    GFP_KERNEL);
		if (!rx_queue->rx_buff)
			goto cleanup;

		ctcmac_alloc_rx_buffs(rx_queue, ctcmac_rxbd_unused(rx_queue));
	}

	return 0;

cleanup:
	ctcmac_free_rx_resources(priv);

	return -1;
}

static void ctcmac_free_tx_resources(struct ctcmac_private *priv)
{
	int i;
	struct ctcmac_priv_tx_q *tx_queue = NULL;

	for (i = 0; i < priv->num_tx_queues; i++) {
		struct netdev_queue *txq;

		tx_queue = priv->tx_queue[i];
		txq = netdev_get_tx_queue(tx_queue->dev, tx_queue->qindex);

		if(tx_queue->tx_skbuff){
			kfree(tx_queue->tx_skbuff);
			tx_queue->tx_skbuff = NULL;
		}        
	}
}

static int ctcmac_init_tx_resources(struct net_device *ndev)
{
	int i;
	struct ctcmac_private *priv = netdev_priv(ndev);
	struct ctcmac_priv_tx_q *tx_queue = NULL;

	for (i = 0; i < priv->num_tx_queues; i++) {
		tx_queue = priv->tx_queue[i];
		tx_queue->num_txbdfree = tx_queue->tx_ring_size;
		tx_queue->next_to_clean = 0;
		tx_queue->next_to_alloc = 0;
		tx_queue->dev = ndev;
		tx_queue->tx_skbuff =
		kmalloc_array(tx_queue->tx_ring_size,
		      sizeof(*tx_queue->tx_skbuff),
		      GFP_KERNEL);

		if (!tx_queue->tx_skbuff)
			goto cleanup;
	}

	return 0;

	cleanup:
	ctcmac_free_tx_resources(priv);

	return -1;
}

static int ctcmac_alloc_skb_resources(struct net_device *ndev)
{    
	if(ctcmac_init_rx_resources(ndev))
		return -1;
	if(ctcmac_init_tx_resources(ndev))
		return -1;

	return 0;
}

static int ctcmac_free_skb_resources(struct ctcmac_private *priv)
{
	ctcmac_free_rx_resources(priv);
	ctcmac_free_tx_resources(priv);
	ctcmac_free_tx_queues(priv);
	ctcmac_free_rx_queues(priv);

	return 0;
}

static void cpumac_start(struct ctcmac_private *priv)
{    
	/* 1. enable rx/tx interrupt */
	writel(CTCMAC_NOR_TX_D|CTCMAC_NOR_RX0_D|CTCMAC_NOR_RX1_D, 
	    &priv->cpumac_reg->CpuMacInterruptFunc[3]);
	/* 2. enable rx/tx */
	clrsetbits(&priv->cpumac_reg->CpuMacReset, CPU_MAC_RESET_W0_SOFT_RST_TX, 0);
	clrsetbits(&priv->cpumac_reg->CpuMacReset, CPU_MAC_RESET_W0_SOFT_RST_RX, 0);

	netif_trans_update(priv->ndev); /* prevent tx timeout */
}

static void cpumac_halt(struct ctcmac_private *priv)
{    
	/* 1. disable rx/tx interrupt */
	ctcmac_regw(&priv->cpumac_reg->CpuMacInterruptFunc[2], 0xffffffff);
	ctcmac_regw(&priv->cpumac_reg->CpuMacInterruptNormal[2], 0xffffffff);
	/* 2. disable rx/tx */
	clrsetbits(&priv->cpumac_reg->CpuMacReset, 0, CPU_MAC_RESET_W0_SOFT_RST_TX);
	clrsetbits(&priv->cpumac_reg->CpuMacReset, 0, CPU_MAC_RESET_W0_SOFT_RST_RX);
}

static int ctcmac_enet_open(struct net_device *dev)
{
	struct ctcmac_private *priv = netdev_priv(dev);
	int err;

	err = ctcmac_init_phy(dev);
	if(err){
		return err;
	}
	err = ctcmac_request_irq(priv);
	if (err){
		return err;
	}
	err = startup_ctcmac(dev);
	if (err){
		return err;
	}
	return 0;
}

static struct	ctcmac_tx_buff * skb_to_txbuff(struct ctcmac_private *priv, struct sk_buff *skb)
{
	u64 addr, offset;
	int frag_index, nr_frags, rq;
	skb_frag_t *frag;
	struct	ctcmac_tx_buff *tx_buff;
	struct ctcmac_priv_tx_q *tx_queue = NULL;

	nr_frags = skb_shinfo(skb)->nr_frags;
	rq = skb->queue_mapping;
	tx_queue = priv->tx_queue[rq];

	tx_buff = &tx_queue->tx_buff[tx_queue->next_to_alloc];
	addr = (u64)skb->data;
	if((!test_param[priv->index].txts_en) && (!nr_frags) && 
	    ((addr & PAGE_MASK) == ((addr+skb_headlen(skb)) & PAGE_MASK))){
		tx_buff->alloc = 0;
		tx_buff->vaddr = skb->data;
		tx_buff->len = skb_headlen(skb);
		tx_buff->dma = dma_map_single(priv->dev, skb->data, skb_headlen(skb),
				 DMA_TO_DEVICE);
		tx_buff->offset = 0;
	    
	}else{
		int alloc_size;

		if (test_param[priv->index].txts_en){
			alloc_size = ALIGN(skb->len+16, BUF_ALIGNMENT);     
			tx_buff->len = skb->len + 16;
		}else{
			alloc_size = ALIGN(skb->len, BUF_ALIGNMENT);
			tx_buff->len = skb->len;

		}
		tx_buff->alloc = 1;
		tx_buff->vaddr = kmalloc(alloc_size, GFP_KERNEL);        
		offset = (BUF_ALIGNMENT - (((u64)tx_buff->vaddr)&(BUF_ALIGNMENT - 1)));
		if(offset == BUF_ALIGNMENT){
			offset = 0;
		}

		tx_buff->offset = offset;
		if (test_param[priv->index].txts_en){
			memcpy(tx_buff->vaddr + offset, &test_param[priv->index].ptp_info, 16);
		}

		offset += 16;
		memcpy(tx_buff->vaddr + offset, skb->data, skb_headlen(skb));
		offset += skb_headlen(skb);
		for(frag_index = 0; frag_index <  nr_frags; frag_index++){
			frag = &skb_shinfo(skb)->frags[frag_index];
			memcpy(tx_buff->vaddr + offset, frag, skb_frag_size(frag));
			offset += skb_frag_size(frag);
		}

		tx_buff->dma = dma_map_single(priv->dev, tx_buff->vaddr, tx_buff->len,
				 DMA_TO_DEVICE);
	}
	return tx_buff;
}

static int ctcmac_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int rq = 0;
	unsigned int  bytes_sent;
	struct netdev_queue *txq;
	struct ctcmac_desc_cfg tx_desc;
	struct	ctcmac_tx_buff *tx_buff;
	struct ctcmac_priv_tx_q *tx_queue = NULL;
	struct ctcmac_private *priv = netdev_priv(dev);

	rq = skb->queue_mapping;
	tx_queue = priv->tx_queue[rq];
	txq = netdev_get_tx_queue(dev, rq);

	/* check if there is space to queue this packet */
	if (tx_queue->num_txbdfree <= 0) {
		pr_err("%s: no space left before send pkt!\n", priv->ndev->name);
		/* no space, stop the queue */
		netif_tx_stop_queue(txq);
		dev->stats.tx_fifo_errors++;
		return NETDEV_TX_BUSY;
	}

	/* Update transmit stats */
	bytes_sent = skb->len;
	tx_queue->stats.tx_bytes += bytes_sent;
	tx_queue->stats.tx_packets++;

	tx_buff = skb_to_txbuff(priv, skb);
	tx_desc.sop = 1;
	tx_desc.eop = 1;
	tx_desc.size = tx_buff->len;
	tx_desc.addr_low = (tx_buff->dma+tx_buff->offset - CTC_DDR_BASE) 
		& CPU_MAC_DESC_INTF_W0_DESC_ADDR_31_0_MASK;
	tx_desc.addr_high = ((tx_buff->dma+tx_buff->offset - CTC_DDR_BASE) >> 32) 
		& CPU_MAC_DESC_INTF_W1_DESC_ADDR_39_32_MASK;
	ctcmac_fill_txbd(priv, &tx_desc);
	tx_queue->tx_skbuff[tx_queue->next_to_alloc] = skb;

	//printk(KERN_ERR "ctcmac_start_xmit next_to_alloc eth%d 0x%x 0x%x\n", priv->index, tx_queue->next_to_alloc, tx_buff->alloc);
	
	if(tx_queue->next_to_alloc >= tx_queue->tx_ring_size - 1){
		tx_queue->next_to_alloc = 0;
	}else{
		tx_queue->next_to_alloc++;
	}

	/* We can work in parallel with 872(), except
	 * when modifying num_txbdfree. Note that we didn't grab the lock
	 * when we were reading the num_txbdfree and checking for available
	 * space, that's because outside of this function it can only grow.
	 */
	spin_lock_bh(&tx_queue->txlock);
	/* reduce TxBD free count */
	tx_queue->num_txbdfree--;
	spin_unlock_bh(&tx_queue->txlock);

	/* If the next BD still needs to be cleaned up, then the bds
	 * are full.  We need to tell the kernel to stop sending us stuff.
	 */
	if (!tx_queue->num_txbdfree) {
		netif_tx_stop_queue(txq);
		pr_err("%s: no space left after send pkt!\n", priv->ndev->name);
		dev->stats.tx_fifo_errors++;
	}

	return NETDEV_TX_OK;
}

static int ctcmac_change_mtu(struct net_device *dev, int new_mtu)
{
	struct ctcmac_private *priv = netdev_priv(dev);
	int frame_size = new_mtu + ETH_HLEN;

	if ((frame_size < 64) || (frame_size > CTCMAC_JUMBO_FRAME_SIZE)) {
		return -EINVAL;
	}

	while (test_and_set_bit_lock(CTCMAC_RESETTING, &priv->state))
		cpu_relax();

	if (dev->flags & IFF_UP)
		stop_ctcmac(dev);

	dev->mtu = new_mtu;

	if (dev->flags & IFF_UP)
		startup_ctcmac(dev);

	clear_bit_unlock(CTCMAC_RESETTING, &priv->state);

	return 0;
}

static int ctcmac_set_features(struct net_device *dev, netdev_features_t features)
{
	return 0;
}

/* Stops the kernel queue, and halts the controller */
static int ctcmac_close(struct net_device *dev)
{
	struct ctcmac_private *priv = netdev_priv(dev);

	cancel_work_sync(&priv->reset_task);
	stop_ctcmac(dev);

	/* Disconnect from the PHY */
	phy_disconnect(dev->phydev);
	ctcmac_free_irq(priv);
	return 0;
}

static void ctcmac_set_multi(struct net_device *dev)
{
}

static void ctcmac_timeout(struct net_device *dev)
{
	struct ctcmac_private *priv = netdev_priv(dev);

	dev->stats.tx_errors++;
	schedule_work(&priv->reset_task);
}

static int ctcmac_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct phy_device *phydev = dev->phydev;

	if (!netif_running(dev))
		return -EINVAL;

	if (!phydev)
		return -ENODEV;

	return phy_mii_ioctl(phydev, rq, cmd);
}

static struct net_device_stats *ctcmac_get_stats(struct net_device *dev)
{
	int qidx;
	unsigned long rx_packets = 0, rx_bytes = 0, rx_dropped = 0;
	unsigned long tx_packets = 0, tx_bytes = 0;
	struct ctcmac_private *priv = netdev_priv(dev);

	for(qidx = 0; qidx < priv->num_rx_queues; qidx++){
		if(!priv->rx_queue[qidx]){
			return &dev->stats; 
		}
		rx_packets += priv->rx_queue[qidx]->stats.rx_packets;
		rx_bytes += priv->rx_queue[qidx]->stats.rx_bytes;
		rx_dropped += priv->rx_queue[qidx]->stats.rx_dropped;
	}

	if(!priv->tx_queue[0]){
		return &dev->stats; 
	}

	tx_packets = priv->tx_queue[0]->stats.tx_packets;
	tx_bytes = priv->tx_queue[0]->stats.tx_bytes;

	dev->stats.rx_packets = rx_packets;
	dev->stats.rx_bytes   = rx_bytes;
	dev->stats.rx_dropped = rx_dropped;
	dev->stats.tx_bytes   = tx_bytes;
	dev->stats.tx_packets = tx_packets;

	return &dev->stats;
}

static int ctcmac_set_mac_addr(struct net_device *dev, void *p)
{
	eth_mac_addr(dev, p);

	return 0;
}

static const struct net_device_ops ctcmac_netdev_ops = {
	.ndo_open = ctcmac_enet_open,
	.ndo_start_xmit = ctcmac_start_xmit,
	.ndo_stop = ctcmac_close,
	.ndo_change_mtu = ctcmac_change_mtu,
	.ndo_set_features = ctcmac_set_features,
	.ndo_set_rx_mode = ctcmac_set_multi,
	.ndo_tx_timeout = ctcmac_timeout,
	.ndo_do_ioctl = ctcmac_ioctl,
	.ndo_get_stats = ctcmac_get_stats,
	.ndo_set_mac_address = ctcmac_set_mac_addr,
	.ndo_validate_addr = eth_validate_addr,
};

static int ctcmac_probe(struct platform_device *ofdev)
{
	struct net_device *dev = NULL;
	struct ctcmac_private *priv = NULL;
	int err = 0;

	regmap_base = syscon_regmap_lookup_by_phandle(ofdev->dev.of_node,"ctc,sysctrl");
	if (IS_ERR(regmap_base))
		return PTR_ERR(regmap_base);
	
	err = ctcmac_of_init(ofdev, &dev);
	if(err){
		return err;
	}

	priv = netdev_priv(dev);
	SET_NETDEV_DEV(dev, &ofdev->dev);
	INIT_WORK(&priv->reset_task, ctcmac_reset_task);
	platform_set_drvdata(ofdev, priv);

	dev->base_addr = (unsigned long)priv->iobase;
	dev->watchdog_timeo = TX_TIMEOUT;
	dev->mtu = CTCMAC_DEFAULT_MTU;
	dev->netdev_ops = &ctcmac_netdev_ops;
	dev->ethtool_ops = &ctcmac_ethtool_test_ops;

	netif_napi_add(dev, &priv->napi_rx, ctcmac_poll_rx_sq, CTCMAC_NAIP_RX_WEIGHT);
	netif_napi_add(dev, &priv->napi_tx, ctcmac_poll_tx_sq, CTCMAC_NAIP_TX_WEIGHT);

	set_bit(CTCMAC_DOWN, &priv->state);

	if(!g_reglock_init_done)
		spin_lock_init(&global_reglock);

	g_reglock_init_done = 1;

	spin_lock_init(&priv->reglock);
	/* Carrier starts down, phylib will bring it up */
	netif_carrier_off(dev);
	err = register_netdev(dev);
	if (err) {
		goto register_fail;
	}

	if(!g_mac_unit_init_done){
		writel(0x07, &priv->cpumacu_reg->CpuMacUnitResetCtl);
		writel(0x00, &priv->cpumacu_reg->CpuMacUnitResetCtl);
		
		clrsetbits(&priv->cpumacu_reg->CpuMacUnitTsCfg, 
		            0, CPU_MAC_UNIT_TS_CFG_W0_CFG_FORCE_S_AND_NS_EN);
		if (priv->interface == PHY_INTERFACE_MODE_SGMII){
			clrsetbits(&priv->cpumacu_reg->CpuMacUnitRefPulseCfg[1], 
			                        CPU_MAC_UNIT_REF_PULSE_CFG_W1_REF_LINK_PULSE_RST,0);

			ctc_mac_serdes_init(priv);
		}
		g_mac_unit_init_done = 1;
	}
	
	mdelay(10);
	
	sprintf(priv->irqinfo[CTCMAC_NORMAL].name, "%s%s",
				dev->name, "_normal");
	sprintf(priv->irqinfo[CTCMAC_FUNC].name, "%s%s",
				dev->name, "_func");
	sprintf(priv->irqinfo[CTCMAC_UNIT].name, "%s%s",
				dev->name, "_unit");
	test_param[priv->index].ring_size = 64;
	test_param[priv->index].payload_size = 256;

	return 0;

	register_fail:
	ctcmac_unmap_io_space(priv);
	ctcmac_free_rx_queues(priv);
	ctcmac_free_tx_queues(priv);
	of_node_put(priv->phy_node);
	ctcmac_free_dev(priv);

	return err;
}

static int ctcmac_remove(struct platform_device *ofdev)
{
	struct ctcmac_private *priv = platform_get_drvdata(ofdev);

	of_node_put(priv->phy_node);

	unregister_netdev(priv->ndev);

	ctcmac_unmap_io_space(priv);
	ctcmac_free_rx_queues(priv);
	ctcmac_free_tx_queues(priv);
	ctcmac_free_dev(priv);

	return 0;
}

static const struct of_device_id ctcmac_match[] =
{
	{
		.type = "network",
		.compatible = "ctc,mac-test",
	},
	{},
};
MODULE_DEVICE_TABLE(of, ctcmac_match);

/* Structure for a device driver */
static struct platform_driver ctcmac_driver = {
	.driver = {
		.name = "ctc-cpumac-test",
		.of_match_table = ctcmac_match,
	},
	.probe = ctcmac_probe,
	.remove = ctcmac_remove,
};

module_platform_driver(ctcmac_driver);
