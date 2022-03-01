/*
 * Filename: os_tcpip.h
 * description: tcp ip stack init
 * Created: 2019.07.22
 * Author:laumy
 */
#ifndef __TCPIP_ADAPTER_H__
#define __TCPIP_ADAPTER_H__

#include <lwip/netif.h>

typedef int net_err_t;

#define NET_OK 0
#define NET_ERR -1

typedef enum {
	MODE_STA = 0 , /*wlan station mode*/
	MODE_AP ,       /*wlan ap mode*/
	IF_MAX,
}if_type_t;

typedef struct {
	ip4_addr_t ip;
	ip4_addr_t netmask;
	ip4_addr_t gw;
}if_info_t;

struct netif* get_netif(if_type_t mode);
#endif
err_t wlan_if_init_ap(struct netif *netif);
err_t wlan_if_init_ap(struct netif *netif)
{
    return 0;
}

err_t wlan_if_init_station(struct netif *netif);
err_t wlan_if_init_station(struct netif *netif)
{
#ifdef CONFIG_RTL8723DS
	return rlt8723ds_wlan_init_statin(netif);
#endif
	return 0;
}
