#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netfilter.h>
#include <linux/netfilter_arp.h>
#include <linux/skbuff.h> 
#include <linux/ip.h>
#include <linux/if_ether.h>

struct nf_hook_ops hook;

unsigned int hook_func(void * priv, struct sk_buff * skb, const struct nf_hook_state * state)
{
	struct ethhdr * eth = eth_hdr(skb);
	struct iphdr * ip = ip_hdr(skb);

	printk("caller: %s\n", current->comm);
	printk("mac: %pM\n", eth->h_source);
	printk("ip protocol: %d\n", ip->protocol);
	printk("%pI4 -> %pI4\n", &ip->saddr, &ip->daddr);
	return NF_ACCEPT;
}

int start(void)
{
	hook.hook = hook_func;
	hook.hooknum = NF_INET_POST_ROUTING;
	hook.pf = NFPROTO_INET;

	if (!nf_register_net_hook(&init_net, &hook))
		printk("netfilter register success\n");
	else
		printk("netfilter register fail\n");

	return 0;
}

void end(void)
{
	nf_unregister_net_hook(&init_net, &hook);
}

module_init(start);
module_exit(end);
MODULE_AUTHOR("testkernel.tistory.com");
MODULE_DESCRIPTION("netfilter example");
MODULE_LICENSE("GPL");
