#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>

char * if_name = NULL;
module_param(if_name, charp, 0644);

static int print_info(void)
{
	struct net_device * dev = dev_get_by_name(&init_net, if_name);
	struct in_ifaddr * addrs = NULL;
	struct wireless_dev * wdev = NULL;

	if (!dev)
		return -1;

	printk("-----net_device info-----\n");
	printk("name: %s\n", dev->name);
	printk("ifindex: %d\n", dev->ifindex);
	printk("dev_addr: %pM\n", dev->dev_addr);
	printk("broadcast: %pM\n", dev->broadcast);
	printk("flags: %u\n", dev->flags);
	printk("promiscuity: %u\n", dev->promiscuity);

	if (dev->ip_ptr)
		addrs = dev->ip_ptr->ifa_list;
	
	if (addrs)
	{
		printk("ifa_local: %pI4\n", &addrs->ifa_local);
		printk("ifa_address: %pI4\n", &addrs->ifa_address);
		printk("ifa_mask: %pI4\n", &addrs->ifa_mask);
		printk("ifa_broadcast: %pI4\n", &addrs->ifa_broadcast);
	}

	wdev = dev->ieee80211_ptr;
	if (!wdev)
		return -1;

	return -1;

}

int test_init(void)
{
	if (!if_name)
		return -1;

	return print_info();	
}

void test_exit(void)
{
	
}

module_init(test_init);
module_exit(test_exit);
MODULE_AUTHOR("test_kernel");
MODULE_DESCRIPTION("struct net_device example");
MODULE_LICENSE("GPL");
