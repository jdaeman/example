#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/inet.h>

#include <net/ip_fib.h> //fib_xxx
#include <net/route.h> //ip_route_output()

char * ifname = NULL;
module_param(ifname, charp, 0644);

int use_rtable(void)
{
	struct rtable * rtable = NULL;
	struct net_device * dev = NULL;
	unsigned int src;

	if (!ifname)
		return -1;

	dev = dev_get_by_name(&init_net, ifname);
	if (!dev)
		return -1;

	src = dev->ip_ptr->ifa_list->ifa_local;
	rtable = ip_route_output(&init_net, ~src, src, 0, 0);
	if (!rtable)
		return -1;

	printk("rtable: %pI4\n", &rtable->rt_gateway);
	return 0;
}

int use_fib(void)
{
	struct flowi4 fl4;
	struct fib_result res;
	struct net_device * dev = NULL;
	struct fib_table * ftable = NULL;
	struct fib_nh * nh = NULL;
	int ret;

	if (!ifname)
		return -1;

	dev = dev_get_by_name(&init_net, ifname);
	if (!dev)
		return -1;	

#ifndef CONFIG_IP_MULTIPLE_TABLES
	ftable = fib_get_table(&init_net, RT_TABLE_MAIN);

#else
	memset(&fl4, 0, sizeof(fl4));
	fl4.flowi4_iif = dev->ifindex;

	ret = fib_lookup(&init_net, &fl4, &res, 0);
	if (ret < 0)
		return -1;

	if (res.fi)
		nh = (res.fi)->fib_nh;
	if (nh)
		printk("fib: %pI4\n", &nh->nh_gw);

	ftable = res.table;
#endif

	if (!ftable)
		return -1;

	//printk("tb_id: %u\n", ftable->tb_id);
	//printk("tb_num_default: %d\n", ftable->tb_num_default);
	
	return 0;
}

int route_init(void)
{
	if (!ifname)
		return -1;

	use_fib();
	use_rtable();

	return -1;
}

void route_exit(void)
{
	
}

module_init(route_init);
module_exit(route_exit);
MODULE_AUTHOR("testkernel.tistory.com");
MODULE_DESCRIPTION("kernel routing table");
MODULE_LICENSE("GPL");
