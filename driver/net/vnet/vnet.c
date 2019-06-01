#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>


// ref
// drivers/net/cs89x0.c


static struct net_device *vnet_dev;


// ref: LDD3
static void emulator_rx_packet(struct sk_buff *skb, struct net_device *dev)
{
	unsigned char *type;
	struct iphdr *ih;
	__be32 *saddr, *daddr, tmp;
	unsigned char tmp_dev_addr[ETH_ALEN];
	struct ethhdr *ethhdr;
	
	struct sk_buff *rx_skb;
		
    // swap mac address of src/dst
	ethhdr = (struct ethhdr *)skb->data;
	memcpy(tmp_dev_addr, ethhdr->h_dest, ETH_ALEN);
	memcpy(ethhdr->h_dest, ethhdr->h_source, ETH_ALEN);
	memcpy(ethhdr->h_source, tmp_dev_addr, ETH_ALEN);

    // swap ip address of src/dst
	ih = (struct iphdr *)(skb->data + sizeof(struct ethhdr));
	saddr = &ih->saddr;
	daddr = &ih->daddr;

	tmp = *saddr;
	*saddr = *daddr;
	*daddr = tmp;
	
	//((u8 *)saddr)[2] ^= 1; /* change the third octet (class C) */
	//((u8 *)daddr)[2] ^= 1;
	type = skb->data + sizeof(struct ethhdr) + sizeof(struct iphdr);
	//printk("tx package type = %02x\n", *type);
	*type = 0; // 0x8 - ping; 0x0 - reply
	
	ih->check = 0;		   /* and rebuild the checksum (ip needs it) */
	ih->check = ip_fast_csum((unsigned char *)ih, ih->ihl);
	
	// allocate sk_buff
	rx_skb = dev_alloc_skb(skb->len + 2);
	skb_reserve(rx_skb, 2); /* align IP on 16B boundary */	
	memcpy(skb_put(rx_skb, skb->len), skb->data, skb->len);

	/* write metadata, and then pass to the receive level */
	rx_skb->dev = dev;
	rx_skb->protocol = eth_type_trans(rx_skb, dev);
	rx_skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
	dev->stats.rx_packets++;
	dev->stats.rx_bytes += skb->len;

	// submit sk_buff
	netif_rx(rx_skb);
}


static int vnet_send_packet(struct sk_buff *skb, struct net_device *dev)
{
	static int cnt = 0;
	printk("vnet_send_packet cnt = %d\n", ++cnt);


    // stop the interface queue
    netif_stop_queue(dev);      

    // send packet in skb to interface
    // emulate sk_buff, and report it
    emulator_rx_packet(skb, dev);

    // release sk_buff
    dev_kfree_skb(skb);
    // wake up queue
    netif_wake_queue(dev);
    

    // update stats info
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;
    
	return 0;
}

static int __init vnet_init(void)
{
    // 1. allocate net_device struct
    vnet_dev = alloc_netdev(0, "vnet%d", ether_setup);  // default function: ether_setup

    // 2. config net_device
    vnet_dev->hard_start_xmit = vnet_send_packet;

    // setup MAC address
    vnet_dev->dev_addr[0] = 0x08;
    vnet_dev->dev_addr[1] = 0x89;
    vnet_dev->dev_addr[2] = 0x89;
    vnet_dev->dev_addr[3] = 0x89;
    vnet_dev->dev_addr[4] = 0x89;
    vnet_dev->dev_addr[5] = 0x11;

    // setup ping flags
    vnet_dev->flags |= IFF_NOARP;
    vnet_dev->features |= NETIF_F_NO_CSUM;
    
    // 3. register net device
    register_netdev(vnet_dev);

    return 0;
}

static void __exit vnet_exit(void)
{
    unregister_netdev(vnet_dev);
    free_netdev(vnet_dev);
}

module_init(vnet_init);
module_exit(vnet_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 virtual net interface driver");
MODULE_LICENSE("GPL");

