/*
 * Filename: ifconfig.c
 * description: get mac/ip address.
 * Created: 2020.05.06
 * Author:flyranchao
 */

#include <stdio.h>
#include <string.h>
#include <finsh.h>
extern void mhd_sta_dhcpc_get_ip_address(char *);
extern void mhd_get_mac_address(char *);
extern void mhd_sta_dhcpc_get_ip_dns(char *);
extern void mhd_sta_dhcpc_get_ip_server(char *);
extern void mhd_sta_dhcpc_get_ip_gateway(char *);
int cmd_ifconfig(int argc, char **argv)
{
	char mac_buf[6];
	char pbuf[4];

	memset(pbuf, 0, sizeof(pbuf));
	memset(mac_buf, 0, sizeof(mac_buf));
	mhd_get_mac_address(mac_buf);
	printf("\nmac address: %02x-%02x-%02x-%02x-%02x-%02x\n", \
			mac_buf[0], mac_buf[1], mac_buf[2], mac_buf[3], mac_buf[4], mac_buf[5]);
	mhd_sta_dhcpc_get_ip_address(pbuf);
	printf("ip address: %d.%d.%d.%d\n", \
			pbuf[0], pbuf[1], pbuf[2], pbuf[3]);

	mhd_sta_dhcpc_get_ip_dns(pbuf);
	printf("ip dns: %d.%d.%d.%d\n", \
			pbuf[0], pbuf[1], pbuf[2], pbuf[3]);

	mhd_sta_dhcpc_get_ip_server(pbuf);
	printf("ip server: %d.%d.%d.%d\n", \
			pbuf[0], pbuf[1], pbuf[2], pbuf[3]);

	mhd_sta_dhcpc_get_ip_gateway(pbuf);
	printf("ip gateway: %d.%d.%d.%d\n", \
			pbuf[0], pbuf[1], pbuf[2], pbuf[3]);

	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_ifconfig, __cmd_ifconfig, ifconfig);
