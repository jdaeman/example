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
	printk("name: %s\n", dev->name); //interface name
	printk("ifindex: %d\n", dev->ifindex); //interface index
	printk("dev_addr: %pM\n", dev->dev_addr); //hw address
	printk("broadcast: %pM\n", dev->broadcast); //hw broadcast address
	printk("flags: %u\n", dev->flags); //interface state "ref, ifconfig"
	printk("promiscuity: %u\n", dev->promiscuity); //promiscious mode flag

	if (dev->ip_ptr)
		addrs = dev->ip_ptr->ifa_list;
	
	if (addrs)
	{
		printk("ifa_local: %pI4\n", &addrs->ifa_local); //ip address
		printk("ifa_address: %pI4\n", &addrs->ifa_address); //;;
		printk("ifa_mask: %pI4\n", &addrs->ifa_mask); //subnet mask
		printk("ifa_broadcast: %pI4\n", &addrs->ifa_broadcast); //ip broadcast address
	}

	wdev = dev->ieee80211_ptr; //wireless device
	if (!wdev)
		return -1;

	return -1;
}

int test_init(void)
{
	if (!if_name)
		return -1;

	//always return -1, so dont care about rmmod
	return print_info();	
}

void test_exit(void)
{
	
}

module_init(test_init);
module_exit(test_exit);
MODULE_AUTHOR("testkernel.tistory.com");
MODULE_DESCRIPTION("struct net_device example");
MODULE_LICENSE("GPL");
