#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <net/if.h> //if_nameindex(), if_freenameindex()
#include <netinet/ether.h> 
#include <sys/ioctl.h>
#include <unistd.h>

int main(void)
{
	struct if_nameindex * if_arr, * itf;
	struct ifreq ifr;	
	int sock;

	sock = socket(PF_INET, SOCK_STREAM, 0); //dont care, TCP, UDP
	if (sock < 0)
	{
		perror("socket error");
		return -1;
	}

	if_arr = if_nameindex();
	if (!if_arr)
	{
		perror("if_nameindex error");
		return -1;
	}

	for (itf = if_arr; itf->if_index != 0 || itf->if_name != NULL; itf++)
	{	
		memcpy(ifr.ifr_name, itf->if_name, sizeof(ifr.ifr_name)); //necessary "ref dev_get_by_name()"
		printf("%d, %s\n", itf->if_index, itf->if_name);

		ioctl(sock, SIOCGIFADDR, &ifr);
		printf("%s\n", inet_ntoa(*(struct in_addr *)&ifr.ifr_addr.sa_data[2]));

		ioctl(sock, SIOCGIFHWADDR, &ifr);	
		printf("%s\n", ether_ntoa((struct ether_addr *)ifr.ifr_hwaddr.sa_data));
	}

	if_freenameindex(if_arr);
	close(sock);
	return 0;
}
