/*
 * Copyright (C) 2017 ALLWINNERTECH TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of ALLWINNERTECH TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <lwip/tcpip.h>
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/inet.h>
#include <net_ctrl/net_ctrl.h>
#include <wifimanager.h>
#include <unistd.h>

static const char* wifi_state_to_str(int sta)
{
	switch(sta) {
		case WIFI_DISCONNECTED:
			return "WIFI_DISCONNECTED";
		case WIFI_SCAN_STARTED:
			return "WIFI_SCAN_STARTED";
		case WIFI_SCAN_FAILED:
			return "WIFI_SCAN_FAILED";
		case WIFI_NETWORK_NOT_FOUND:
			return "WIFI_NETWORK_NOT_FOUND";
		case WIFI_ASSOCIATING:
			return "WIFI_ASSOCIATING";
		case WIFI_AUTH_REJECT:
			return "WIFI_AUTH_REJECT";
		case WIFI_AUTH_TIMEOUT:
			return "WIFI_AUTH_TIMEOUT";
		case WIFI_HANDSHAKE_FAILED:
			return "WIFI_HANDSHAKE_FAILED";
		case WIFI_CONNECTED:
			return "WIFI_CONNECTED";
		case WIFI_CONN_TIMEOUT:
			return "WIFI_CONN_TIMEOUT";
		case DHCP_START_FAILED:
			return "DHCP_START_FAILED";
		case DHCP_TIMEOUT:
			return "DHCP_TIMEOUT";
		case DHCP_SUCCESS:
			return "DHCP_SUCCESS";
		default:
			return "UNKNOWN";
	}
}

static void wifi_status_change(aw_wifi_status_change_t *wifi_status)
{
	if(strncmp(wifi_status->status,"connect",7) == 0)
		printf("wifi connect successful:%s\n",wifi_status->ssid);
	else
		printf("wifi connect failed.\n");
}

static void wifi_state_change(aw_wifi_state_t state)
{
	printf("wifi state:%s\n",wifi_state_to_str(state));
}

static void wifi_hw_status(aw_wifi_hw_status_t hw_status)
{
	printf("wifi %s\n",hw_status?"up":"down");
}

void wifi_msg_callback(aw_wifi_msg_data_t *p_msg)
{
	switch(p_msg->id) {
		case WIFI_MSG_ID_WIFI_HW_STATUS:
			wifi_hw_status(p_msg->data.wlan_status);
			break;
		case WIFI_MSG_ID_WIFI_STATE:
			wifi_state_change(p_msg->data.wlan_status);
			break;
		case WIFI_MSG_ID_NETWORK_STATUS:
			wifi_status_change(p_msg->data.network_status_change);
			break;
		default:
			printf("Not support.!\n");
			break;
	}
}

static int dhcp_handle(int tries,int timeout_ms,void *arg)
{
	struct netif *net_if = NULL;
	struct dhcp *dhcp_data = NULL;
	uint32_t addr_val = 0;

	net_if = g_wlan_netif;

	if(net_if == NULL) {
		printf("get net interface failed\n");
		return -1;
	}

	if(dhcp_start(net_if) != 0) {
		printf("star dhcp failed.");
		return -1;
	}

    dhcp_data = netif_dhcp_data(net_if);
    if (NULL == dhcp_data) {
		printf("LwIP_DHCP: dhcp data failed.");
		return -1;
	}

    while (1) {
		/* If DHCP stopped by wifi_disconn_hdl*/
		if(dhcp_data->state == 0) {
		    printf("LwIP_DHCP: dhcp stop.");
			return -1;
		}

		/* Read the new IP address */
		addr_val = net_if->ip_addr.addr;

	    // Assigned
		if (addr_val != 0) {
		    printf("Dhcp got ip %hu.%hu.%hu.%hu",
					ip4_addr1_16(netif_ip4_addr(net_if)),
					ip4_addr2_16(netif_ip4_addr(net_if)),
					ip4_addr3_16(netif_ip4_addr(net_if)),
					ip4_addr4_16(netif_ip4_addr(net_if)));
			return 0;
		} else {
			/* DHCP timeout */
			printf("dhcp tries time: %d\n",dhcp_data->tries);
			if (dhcp_data->tries > tries) {
				/* Stop DHCP */
				dhcp_stop(net_if);
				printf("dhcp timeout, stop dhcp");
				return -1;
			} else {
				sleep(1);
			}
		}
    }

	return 0;
}

#define MAX_SCAN_RESULTS_NUM 32

#define DHCPC_TRYIES          3
#define DHCPC_TIMEOUT         180000
#define DHCPC_CONNECT_TIMEOUT 180000

void wifi_main(int argc,char  *argv[])
{
	int c;
	aw_wifi_scan_results_t scan_result[MAX_SCAN_RESULTS_NUM];
	int ret;
	int i = 0;
	int mode = 0;

	aw_attr_t attr = {
		.dhcp_cb = dhcp_handle,
		.msg_cb  = wifi_msg_callback,
		.dhcp_tries = DHCPC_TRYIES,
		.dhcp_timeout = DHCPC_TIMEOUT,
		.connect_timeout = DHCPC_CONNECT_TIMEOUT,
	};

	if(argc < 2) {
		goto help;
	}

	aw_wifi_init(&attr);

	while((c = getopt(argc,argv,"o:c:sdfh")) != -1) {
		switch(c) {
			case 'o':
				if(argc != 3) {
					goto help;
				}
				mode = atoi(argv[2]);
				if(mode == 0) {
					aw_wifi_on(WIFI_MODE_STA);
				}else if(mode == 1) {
					aw_wifi_on(WIFI_MODE_HOSTAP);
				} else
					goto help;
				break;
			case 'c':
				if(argc < 4 || argc > 6){
					goto help;
				}
				if(aw_wifi_get_current_mode() == WIFI_MODE_STA) {
					aw_wifi_connect(argv[2],argv[3]);
				}else if(aw_wifi_get_current_mode() == WIFI_MODE_HOSTAP)
					aw_wifi_ap_start(argv[2],argv[3]);
				else {
					printf("Not init.\n");
				}
				break;
			case 's':
				ret = aw_wifi_scan(scan_result,MAX_SCAN_RESULTS_NUM);
				for(i=0;i>=0 && i<ret;i++) {
					printf("%2d bssid: %02X:%02X:%02X:%02X:%02X:%02X   ssid:%-24.24s "
							"ch: %-2d   rssi: %2d   key_mgmt: %2d\n",i,
							scan_result[i].bssid[0],scan_result[i].bssid[1],scan_result[i].bssid[2],
							scan_result[i].bssid[3],scan_result[i].bssid[4],scan_result[i].bssid[5],
							scan_result[i].ssid,scan_result[i].channel,scan_result[i].rssi,
							scan_result[i].key);
				}
				break;
			case 'd':
				if (aw_wifi_get_current_mode() == WIFI_MODE_STA) {
					ret = aw_wifi_disconnect();
					if (ret == 0)
						printf("disconnect from ap success\n");
					else
						printf("disconnect from ap fail\n");
				} else if (aw_wifi_get_current_mode() == WIFI_MODE_HOSTAP) {
					ret = aw_wifi_ap_stop();
					if (ret == 0)
						printf("bring down ap success\n");
					else
						printf("bring down ap fail\n");
				} else {
					printf("unknown wifi mode, execute cmd fail\n");
				}
				break;
			case 'f':
				aw_wifi_off();
				break;
			case 'h':
				goto help;
			default:
				printf("invaild.\n");
		}
	}
	return;
help:
	printf("commands:\n\n");
	printf("wifi -o <mode> = wifi on, STA: mode=0, AP: mode=1\n\n");
	printf("wifi -s = scan AP and show AP list\n\n");
	printf("wifi -c <ssid> <passwd> = STA: connect AP, AP: config and re-enable AP\n\n");
	printf("wifi -d = STA: disconnect from AP, AP: bring down AP\n\n");
	printf("wifi -f = wifi off\n\n");

	return ;
}
FINSH_FUNCTION_EXPORT_CMD(wifi_main,wifi, wifi testcmd);

void aw_dhcpc_restart(void)
{
	int ret;
	struct netif *net_if = NULL;
	net_if = g_wlan_netif;

	if(net_if == NULL) {
		printf("%s:%d get net interface failed\n", __func__, __LINE__);
		return;
	}

	/* clean netif info */
	memset(&net_if->ip_addr, 0, sizeof(ip_addr_t));
	memset(&net_if->netmask, 0, sizeof(ip_addr_t));
	memset(&net_if->gw, 0, sizeof(ip_addr_t));

	/* stop dhcp client */
	dhcp_stop(net_if);
	dhcp_cleanup(net_if);
	/* start dhcp client */
	ret = dhcp_handle(DHCPC_TRYIES, DHCPC_TIMEOUT, NULL);
	if(ret == 0) {
		aw_wifi_event(DHCP_SUCCESS);
		aw_wifi_status_handle(DHCP_SUCCESS);
		printf("address: %s\n", inet_ntoa(net_if->ip_addr));
		printf("gateway: %s\n", inet_ntoa(net_if->gw));
		printf("netmask: %s\n", inet_ntoa(net_if->netmask));
	} else {
		printf("Got ip failed.");
	}
}
FINSH_FUNCTION_EXPORT_CMD(aw_dhcpc_restart,dhcpc_restart,DHCP client restart command);
