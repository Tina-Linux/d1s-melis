/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Author: laumy <liumingyuan@allwinnertech.com>
* Date  : 2020-01-09
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <wifimanager.h>
#include "wifi_adapter.h"
#include "wifilog.h"
#include <net_ctrl/net_ctrl.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>
#include <unistd.h>
#define WIFI_CONFIG_NUM 3

typedef struct {
	char ssid[WIFI_SSID_MAX_LENGTH+1];
	char password[WIFI_PASSWORD_MAX_LENGTH+1];
}current_netinfo_t;

typedef struct {
	aw_wifi_network_info_t wifi_info[WIFI_CONFIG_NUM];
	aw_dhcp_cb_t dhcp_cb;
	current_netinfo_t current_network;
	aw_attr_t init_attr;
	aw_wifi_mode_t current_mode;
	bool wifi_is_on;
	bool enable;
}aw_wifi_handle_t;

aw_wifi_handle_t g_aw_wifi_worker = {
	.init_attr.msg_cb = aw_wifi_msg_callback,
	.init_attr.dhcp_cb = aw_dhcp_handle,
	.init_attr.dhcp_tries = 3,
	.init_attr.dhcp_timeout = 180000,
	.init_attr.connect_timeout = 180000,
	.current_mode = WIFI_MODE_STA,
	.wifi_is_on = false,
	.enable = false,
};

aw_wifi_handle_t *p_wifi_handle = &g_aw_wifi_worker;

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

void aw_wifi_msg_callback(aw_wifi_msg_data_t *p_msg)
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

int aw_dhcp_handle(int tries, int timeout_ms, void *arg)
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

void aw_wifi_status_handle(aw_wifi_state_t event)
{

	aw_wifi_msg_data_t msg_data = {
		.id = WIFI_MSG_ID_NETWORK_STATUS,
	};


	if(event == WIFI_DISCONNECTED ||
			event == DHCP_SUCCESS) {
		int len = 0;

		aw_wifi_status_change_t sta_chg;
		memset(&sta_chg,0,sizeof(aw_wifi_status_change_t));

		len = strlen(p_wifi_handle->current_network.ssid);
		strncpy(sta_chg.ssid,p_wifi_handle->current_network.ssid,len);
		if(len < WIFI_SSID_MAX_LENGTH)
			sta_chg.ssid[len] = '\0';

		len = strlen(p_wifi_handle->current_network.password);
		strncpy(sta_chg.password,p_wifi_handle->current_network.password,len);

		if(len < WIFI_PASSWORD_MAX_LENGTH)
			sta_chg.password[len] = '\0';

		if(event == DHCP_SUCCESS) {
			strncpy(sta_chg.status,"connect",sizeof("connect"));
        } else {
			strncpy(sta_chg.status,"disconnect",sizeof("disconnect"));
        }

		msg_data.data.network_status_change = (void*)&sta_chg;
		if(p_wifi_handle->init_attr.msg_cb)
			p_wifi_handle->init_attr.msg_cb(&msg_data);
	}

}

void aw_wifi_event(aw_wifi_state_t event)
{

	aw_wifi_msg_data_t msg_data = {
		.id = WIFI_MSG_ID_WIFI_STATE,
	};

	if(event > WIFI_DISCONNECTED &&
			event < WIFI_MAX) {

		msg_data.data.wlan_status = event;

		if(p_wifi_handle->init_attr.msg_cb)
			p_wifi_handle->init_attr.msg_cb(&msg_data);
	}

}

void aw_wifi_indicate_event_handle(aw_wifi_state_t event_cmd)
{
	static bool msg_cb_enable = true;
	int ret = -1;
	if(p_wifi_handle->enable == false)
		return ;

	if(p_wifi_handle->current_mode != WIFI_MODE_STA)
		//return ;

	if(event_cmd == WIFI_SCAN_STARTED)
		msg_cb_enable = true;

	if(event_cmd >= WIFI_DISCONNECTED && event_cmd <WIFI_MAX) {

		if(event_cmd == WIFI_HANDSHAKE_FAILED ||
				event_cmd == WIFI_NETWORK_NOT_FOUND) {

			aw_wifi_event(event_cmd);
			msg_cb_enable = false;
		}else {
			if(msg_cb_enable && event_cmd != WIFI_MAX)
				aw_wifi_event(event_cmd);
		}
		aw_wifi_status_handle(event_cmd);
	}

	if(event_cmd == WIFI_CONNECTED) {
		if (p_wifi_handle->current_mode == WIFI_MODE_STA) {
			if(p_wifi_handle->init_attr.dhcp_cb) {
				ret = p_wifi_handle->init_attr.dhcp_cb(p_wifi_handle->init_attr.dhcp_tries,
							p_wifi_handle->init_attr.dhcp_timeout,
							p_wifi_handle->init_attr.dhcp_arg);
				if(ret == 0) {
					aw_wifi_event(DHCP_SUCCESS);
					aw_wifi_status_handle(DHCP_SUCCESS);
				} else {
					WFLOGE("Got ip failed.");
				}
			}
		} else if (p_wifi_handle->current_mode == WIFI_MODE_HOSTAP) {
			net_config(g_wlan_netif, 1);
		}
	}
}

int aw_wifi_init(aw_attr_t *attr)
{
	if(NULL != attr) {
		memcpy(&p_wifi_handle->init_attr,attr,
				sizeof(aw_attr_t));
	}
}


int aw_wifi_on(aw_wifi_mode_t mode)
{
	int ret = 0;

	aw_wifi_msg_data_t msg_data = {
		.id = WIFI_MSG_ID_WIFI_HW_STATUS,
	};

	if(mode == WIFI_MODE_HOSTAP &&
			p_wifi_handle->current_mode != WIFI_MODE_HOSTAP) {
		ret = wifi_on(WIFI_MODE_HOSTAP);
		p_wifi_handle->current_mode = WIFI_MODE_HOSTAP;

	}else if(mode == WIFI_MODE_STA) {

		ret = wifi_on(WIFI_MODE_STA);
		p_wifi_handle->current_mode = WIFI_MODE_STA;
	}

	if(ret == 0) {
		msg_data.data.wlan_status = WLAN0_STATUS_UP;
		p_wifi_handle->wifi_is_on = true;
	}else {
		msg_data.data.wlan_status = WLAN0_STATUS_DOWN;
	}

	if(p_wifi_handle->init_attr.msg_cb)
		p_wifi_handle->init_attr.msg_cb(&msg_data);

	p_wifi_handle->enable = true;

	return ret;
}

aw_wifi_mode_t aw_wifi_get_current_mode(void)
{
	return p_wifi_handle->current_mode;
}

void aw_wifi_off(void)
{
	if (p_wifi_handle->wifi_is_on == true) {
		wifi_off();
		p_wifi_handle->wifi_is_on = false;
		p_wifi_handle->enable = false;
		p_wifi_handle->current_mode = WIFI_MODE_STA;
	} else {
		printf("wifi is already off, invalid cmd!\n");
	}
}

int aw_wifi_scan(aw_wifi_scan_results_t *scan_result,int max_num)
{
	if(p_wifi_handle->wifi_is_on == false ||
			p_wifi_handle->current_mode != WIFI_MODE_STA)
	{
		aw_wifi_on(WIFI_MODE_STA);
	}
	return wifi_scan(scan_result,max_num);
}

int aw_wifi_ap_start(char *ssid,char *pwd)
{
	if(p_wifi_handle->wifi_is_on == false ||
			p_wifi_handle->current_mode != WIFI_MODE_HOSTAP)
	{
		aw_wifi_on(WIFI_MODE_HOSTAP);
	}

	return wifi_ap_start(ssid,pwd);
}

int aw_wifi_ap_stop(void)
{
	p_wifi_handle->wifi_is_on = false;
	p_wifi_handle->enable = false;

	return wifi_ap_stop();
}

int aw_wifi_connect(const char *ssid,const char *password)
{
	int len;
	len = strlen(ssid);

	if(p_wifi_handle->wifi_is_on == false ||
			p_wifi_handle->current_mode != WIFI_MODE_STA)
	{
		aw_wifi_on(WIFI_MODE_STA);
	}

	strncpy(p_wifi_handle->current_network.ssid,ssid,len);
	p_wifi_handle->current_network.ssid[len] = '\0';

	len = strlen(password); //TODO NULL
	strncpy(p_wifi_handle->current_network.password,password,len);
	p_wifi_handle->current_network.password[len] = '\0';

	return wifi_connect(ssid,password);
}

int aw_wifi_disconnect(void)
{
	return wifi_disconnect();
}

int aw_wifi_get_network_info(aw_wifi_network_info_t *pinfo)
{
	//TODO;

	return 0;
}

int aw_wifi_network_info_save_to_file(aw_wifi_network_info_t *pinfo)
{
	//TODO;

	return 0;
}

int aw_wifi_get_rssi(int *rssi)
{
	//TODO;

	return 0;
}

int aw_wifi_get_mac(char *mac)
{
	//TODO;

	return 0;
}

int aw_wifi_get_ipaddr(char *ipaddr)
{
	//TODO;

	return 0;
}

int aw_wifi_get_bssid(char *bssid)
{
	//TODO;
	return 0;

}

int aw_wifi_get_freq(void)
{
	//TODO;
	return 0;
}

void aw_wifi_auto_reconnect(void)
{
	//TODO;

}
