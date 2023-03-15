/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdbool.h>
#include <lwip/tcpip.h>
#include <lwip/netif.h>
//#include "wlan_if.h"
#include <string.h>

#include "tcpip_adapter.h"
//#include "console.h"

#define DEBUG(fmt,args...) do { printf("tcpip_adapter[%s,%d]"fmt,__func__,__LINE__,##args); }while(0)
#define INFO(fmt,args...) do { printf("tcpip_adapter[%s,%d]"fmt,__func__,__LINE__,##args); }while(0)
#define ERROR(fmt,args...) do { printf("tcpip_adapter[%s,%d]"fmt,__func__,__LINE__,##args); }while(0)
#if 0
static struct netif net_if;
#endif
struct netif *aw_netif[IF_MAX];

int netif_ifconfig(int argc, char **argv)
{
	int i;
	for(i=0; i<IF_MAX; i++) {
		if(aw_netif[i]) {
			printf("%c%c\t%s\n \
				\t%s%02"X16_F":%02"X16_F":%02"X16_F":%02"X16_F":%02"X16_F":%02"X16_F"\n \
				\t%s%"U16_F".%"U16_F".%"U16_F".%"U16_F"\n \
				\t%s%"U16_F".%"U16_F".%"U16_F".%"U16_F"\n \
				\t%s%"U16_F".%"U16_F".%"U16_F".%"U16_F"\n \
				\t%s %s %s %s%d\n",
				aw_netif[i]->name[0],aw_netif[i]->name[1],
				"Link encap:Ethernet",
				"HWaddr ",
				aw_netif[i]->hwaddr[0], aw_netif[i]->hwaddr[1], aw_netif[i]->hwaddr[2],
				aw_netif[i]->hwaddr[3], aw_netif[i]->hwaddr[4], aw_netif[i]->hwaddr[5],
				"inet addr ",
				ip4_addr1_16(netif_ip4_addr(aw_netif[i])),
				ip4_addr2_16(netif_ip4_addr(aw_netif[i])),
				ip4_addr3_16(netif_ip4_addr(aw_netif[i])),
				ip4_addr4_16(netif_ip4_addr(aw_netif[i])),
				"gw addr ",
				ip4_addr1_16(netif_ip4_gw(aw_netif[i])),
				ip4_addr2_16(netif_ip4_gw(aw_netif[i])),
				ip4_addr3_16(netif_ip4_gw(aw_netif[i])),
				ip4_addr4_16(netif_ip4_gw(aw_netif[i])),
				"netmask ",
				ip4_addr1_16(netif_ip4_netmask(aw_netif[i])),
				ip4_addr2_16(netif_ip4_netmask(aw_netif[i])),
				ip4_addr3_16(netif_ip4_netmask(aw_netif[i])),
				ip4_addr4_16(netif_ip4_netmask(aw_netif[i])),
				((aw_netif[i]->flags & NETIF_FLAG_UP) == NETIF_FLAG_UP) ? "UP" : "DOWN",
				((aw_netif[i]->flags & NETIF_FLAG_BROADCAST) == NETIF_FLAG_BROADCAST) ? "BROADCAST" : " ",
				((aw_netif[i]->flags & NETIF_FLAG_LINK_UP) == NETIF_FLAG_LINK_UP) ? "LINK_UP" : "LINK_DOWN",
				" mtu: ",aw_netif[i]->mtu);
			return 0;
		}
	}
	printf("net interface down.\n");
	return -1;
}

FINSH_FUNCTION_EXPORT(netif_ifconfig, ifconfig);
MSH_CMD_EXPORT(netif_ifconfig, ifconfig);

static int8_t ethernetif_init(if_type_t mode,if_info_t *info,uint8_t *mac)
{
	if(mode > IF_MAX || info == NULL) {
		return -1;
	}
	if(aw_netif[mode] == NULL ) {
#if 1
		aw_netif[mode] = calloc(1,sizeof(*aw_netif[mode])); //if free ?
		if(aw_netif[mode] == NULL) {
			ERROR("interface resource request failed\n");
			return -1;
		}
#else
		aw_netif[mode] = &net_if;
#endif
	}
	if(mac != NULL) {
		memcpy(aw_netif[mode]->hwaddr, mac, NETIF_MAX_HWADDR_LEN);
	}

	if(mode == MODE_STA) {
		netif_add(aw_netif[mode],&info->ip,&info->netmask,
					&info->gw,NULL,wlan_if_init_station,tcpip_input);
	}else if(mode == MODE_AP) {
		netif_add(aw_netif[mode],&info->ip,&info->netmask,
					&info->gw,NULL,wlan_if_init_ap,tcpip_input);
	}else {
		ERROR("net interface mode un supported.\n");
		goto failed;
	}

	netif_set_default(aw_netif[mode]);
	return 0;

failed:
	return -1;
}

struct netif* get_netif(if_type_t mode)
{
	if(aw_netif[mode] != NULL) {
		return aw_netif[mode];
	}
	ERROR("net interface not init.\n");
	return NULL;
}

void set_netif(if_type_t mode, struct netif *netif)
{
    if (mode >= IF_MAX) {
        printf("unsupported interface mode: %d\n", mode);
        return;
    }

    aw_netif[mode] = netif;
}

static bool tcpip_enable = false;
int8_t tcpip_stack_init(if_type_t mode,if_info_t *info,uint8_t *mac)
{

	if(tcpip_enable == false) {
		tcpip_init(NULL,NULL);
		tcpip_enable = true;
	}
	return ethernetif_init(mode,info,mac);
}

int cmd_tcpip_init(int argc, const char **argv)
{
	if_info_t net_info;
	IP4_ADDR(&net_info.ip,0,0,0,0);
	IP4_ADDR(&net_info.netmask,0,0,0,0);
	IP4_ADDR(&net_info.gw,0,0,0,0);
	tcpip_stack_init(MODE_STA,&net_info,NULL);
	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_tcpip_init, __cmd_tcpip_init, tcpip);