#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>

#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/netlink.h>
#include <net/ip_fib.h>

#include <net/route.h>
#include <linux/inet.h>

//ip_route_output() -> struct rtable -> rt_gateway


extern struct net init_net;

struct nlmsghdr nlh = {
	.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)),
	.nlmsg_type = RTM_GETROUTE,
	.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST,
	.nlmsg_seq = 10,
};

char msg[512];
struct netlink_ext_ack extack = {
	._msg = msg,
};

const struct nla_policy rtm_ipv4_policy[RTA_MAX + 1] = {
	[RTA_DST]		= { .type = NLA_U32 },
	[RTA_SRC]		= { .type = NLA_U32 },
	[RTA_IIF]		= { .type = NLA_U32 },
	[RTA_OIF]		= { .type = NLA_U32 },
	[RTA_GATEWAY]		= { .type = NLA_U32 },
	[RTA_PRIORITY]		= { .type = NLA_U32 },
	[RTA_PREFSRC]		= { .type = NLA_U32 },
	[RTA_METRICS]		= { .type = NLA_NESTED },
	[RTA_MULTIPATH]		= { .len = sizeof(struct rtnexthop) },
	[RTA_FLOW]		= { .type = NLA_U32 },
	[RTA_ENCAP_TYPE]	= { .type = NLA_U16 },
	[RTA_ENCAP]		= { .type = NLA_NESTED },
	[RTA_UID]		= { .type = NLA_U32 },
	[RTA_MARK]		= { .type = NLA_U32 },
};

/*int gw_init(void)
{
	struct net * net = &init_net;
	struct rtmsg * rtm;
	struct nlattr * tb[RTA_MAX + 1];
	struct fib_result res;
	struct rtable * rt = NULL;
	struct flowi4 fl4;
	__be32 dst;
	__be32 src;
	u32 iif;
	struct sk_buff * skb;
	u32 table_id = RT_TABLE_MAIN;
	int err;
	int mark;
	kuid_t uid;
	struct net_device * dev;
	

	nlh.nlmsg_pid = current->pid;

	err = nlmsg_parse(&nlh, sizeof(*rtm), tb, RTA_MAX, rtm_ipv4_policy, &extack);
	if (err < 0)
		return -1;
	
	rtm = nlmsg_data(&nlh);
	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb)
		return -1;

	skb_reset_mac_header(skb);
	skb_reset_network_header(skb);
	
	src = tb[RTA_SRC] ? nla_get_in_addr(tb[RTA_SRC]) : 0;
	dst = tb[RTA_DST] ? nla_get_in_addr(tb[RTA_DST]) : 0;
	iif = tb[RTA_IIF] ? nla_get_u32(tb[RTA_IIF]) : 0;
	mark = tb[RTA_MARK] ? nla_get_in_addr(tb[RTA_MARK]) : 0;
	if (tb[RTA_UID])
		uid = make_kuid(current_user_ns(), nla_get_u32(tb[RTA_UID]));
	else
		uid = (iif ? INVALID_UID : current_uid());

	ip_hdr(skb)->protocol = IPPROTO_UDP;
	ip_hdr(skb)->saddr = in_aton("192.168.0.7");
	ip_hdr(skb)->daddr = in_aton("192.168.0.99");
	
	skb_reserve(skb, MAX_HEADER + sizeof(struct iphdr));
	
	memset(&fl4, 0, sizeof(fl4));
	fl4.daddr = dst;
	fl4.saddr = src;
	fl4.flowi4_tos = rtm->rtm_tos;
	fl4.flowi4_oif = tb[RTA_OIF] ? nla_get_u32(tb[RTA_OIF]) : 0;
	fl4.flowi4_mark = mark;
	fl4.flowi4_uid = uid;

	rcu_read_lock();

	rt = ip_route_output_key_hash_rcu(net, &fl4, &res, skb);
	err = 0;
	if (IS_ERR(rt))
		err = PTR_ERR(rt);
	else
		skb_dst_set(skb, &rt->dst);

	if (!err)
		printk("%pI4\n", &rt->dst);
	
	rcu_read_unlock();
	
	printk("mark : %d\n", mark);

	dev = dev_get_by_index_rcu(net, 2);
	if (!dev)
		return -1;
	printk("%s\n", dev->name);
	skb->protocol = htons(ETH_P_IP);
	skb->dev = dev;
	skb->mark = mark;

	dst = in_aton("192.168.0.99");
	src = in_aton("192.168.0.7");
	err = ip_route_input_noref(skb, dst, src, rtm->rtm_tos, dev);	
	rt = skb_rtable(skb);

	printk("%d\n", err);
	//if (!(rt->dst.error))
		printk("%pI4\n", &rt->rt_gateway);


	printk("success\n");
	kfree_skb(skb);
	return -1;
}*/

int gw_init(void)
{
	struct flowi4 fl4;
	struct fib_result res;
	struct net_device * dev;
	struct fib_table * ftable;
	int ret;

	dev = dev_get_by_index_rcu(&init_net, 2);
	
	fl4.flowi4_oif = 0;
	fl4.flowi4_iif = dev->ifindex;
	fl4.flowi4_mark = 0;
	fl4.flowi4_tos = 0;
	fl4.flowi4_scope = RT_SCOPE_UNIVERSE;
	fl4.flowi4_flags = 0;
	fl4.daddr = in_aton("192.168.0.99");
	fl4.saddr = in_aton("192.168.0.7");

#ifndef CONFIG_IP_MULTIPLE_TABLES
	ftable = fib_get_table(&init_net, RT_TABLE_MAIN);
#else
	ret = fib_lookup(&init_net, &fl4, &res, 0);

	if (ret != 0)
	{
		printk("fail\n");
		return -1;
	}
	ftable = res.table;
	printk("%u, %d, %u, %u\n", ftable->tb_id, ftable->tb_num_default, *(ftable->tb_data), ftable->__data[0]);

	printk("success\n");


#endif

	return -1;
}

void gw_exit(void)
{
	
}

module_init(gw_init);
module_exit(gw_exit);
MODULE_AUTHOR("testkernel.tistory.com");
MODULE_DESCRIPTION("struct net_device example");
MODULE_LICENSE("GPL");
