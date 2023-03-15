/*
 * Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 * Email  : liumingyuan@allwinnertech.com
 * Create : 2019-09-12
 *
 */

#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__

#if __cplusplus
extern "C" {
#endif
typedef enum {
    WLAN0_STATUS_DOWN =0,
    WLAN0_STATUS_UP,
    WLAN0_STATUS_MAX
} aw_wifi_hw_status_t;

typedef enum {
    WIFI_MSG_ID_MIN = 0,
    WIFI_MSG_ID_WIFI_HW_STATUS = WIFI_MSG_ID_MIN,
    WIFI_MSG_ID_WIFI_STATE,
    WIFI_MSG_ID_NETWORK_STATUS,
    WIFI_MSG_ID_MAX
}aw_wifi_msg_id_t;

typedef enum {
    WIFI_DISCONNECTED = 1, // Connection start
    WIFI_SCAN_STARTED, // Scan start
    WIFI_SCAN_FAILED, // Scan failed
    WIFI_NETWORK_NOT_FOUND, // no AP found
    WIFI_AUTHENTICATION,//Authentication start
    WIFI_AUTH_REJECT, // Authentication rejected by AP
    WIFI_AUTH_TIMEOUT, // Authentication timeout with AP
    WIFI_ASSOCIATING, // Association starts
    WIFI_ASSOC_REJECT, // Association rejected by AP
    WIFI_ASSOCIATED, //Authentication succeed
    WIFI_4WAY_HANDSHAKE, //4way-handshark start
    WIFI_HANDSHAKE_FAILED, // 4way-handshake fails
    WIFI_4WAY_HANDSHAKE_DONE, //4way-handshark done
    WIFI_GROUP_HANDSHAKE, //group-handshark start
    WIFI_GROUP_HANDSHAKE_DONE, //group-handshark done = completed
    WIFI_CONNECTED, // Connection to AP done
    WIFI_CONN_TIMEOUT, // Connection timeout
    WIFI_DEAUTH, // Deauth received from AP
    DHCP_START_FAILED, // DHCP start fails
    DHCP_TIMEOUT, // DHCP timeout
    DHCP_SUCCESS, // DHCP success
	WIFI_MAX,
}aw_wifi_state_t;

typedef struct {
#define WIFI_STATUS_MAX_LENGTH      (32)
#define WIFI_SSID_MAX_LENGTH        (32)
#define WIFI_PASSWORD_MAX_LENGTH    (64)
    int  quantity;
    char status[WIFI_STATUS_MAX_LENGTH+1];
    char ssid[WIFI_SSID_MAX_LENGTH+1];
    char password[WIFI_PASSWORD_MAX_LENGTH+1];
}aw_wifi_status_change_t;

typedef struct {
    int id;
    union {
        int   wlan_status;
        void *network_status_change;
    } data;
}aw_wifi_msg_data_t;

#define WC_IP_ADDR_LEN    (4)
#define WC_BSSID_LEN      (6)
#define WC_ADDRESS_LEN    (6)
#define WC_WPA_STATEA_LEN (14)

typedef void (*aw_wifi_msg_cb_t)(aw_wifi_msg_data_t *p_msg);
/*
 *return: -1-dhcp start failed,0-dhcp success,1 dhcp timeout.
 */
typedef int (*aw_dhcp_cb_t)(int tries,int timeout_ms,void *arg);

typedef enum {
	WIFIMG_NONE = 0,
	WIFIMG_WEP,
	WIFIMG_WPA_PSK,
	WIFIMG_WPA2_PSK,
}aw_wifi_mgmt_t;

typedef enum {
	WIFI_MODE_STA = 0,
	WIFI_MODE_HOSTAP,
	WIFI_MODE_MONITOR,
	WIFI_MODE_INVALID,
}aw_wifi_mode_t;

typedef struct {
	bool enable;
	char ssid[WIFI_SSID_MAX_LENGTH+1];
	char pwd[WIFI_PASSWORD_MAX_LENGTH+1];
	uint8_t bssid[WC_BSSID_LEN];
	uint8_t ch;
	uint8_t ip_addr[WC_IP_ADDR_LEN];
	int security_mode; // don't save to file
}aw_wifi_network_info_t;

typedef struct {
	char ssid[WIFI_SSID_MAX_LENGTH+1];
	int rssi;
	int channel;
	aw_wifi_mgmt_t key;
	uint8_t bssid[WC_BSSID_LEN];
}aw_wifi_scan_results_t;

typedef struct {
	aw_dhcp_cb_t dhcp_cb;
	aw_wifi_msg_cb_t msg_cb;
	int dhcp_tries;
	int dhcp_timeout;
	void *dhcp_arg;
	int connect_timeout;
}aw_attr_t;

void aw_wifi_msg_callback(aw_wifi_msg_data_t *p_msg);

int aw_dhcp_handle(int tries, int timeout_ms, void *arg);

int aw_wifi_on(aw_wifi_mode_t mode);

void aw_wifi_off(void);

int aw_wifi_scan(aw_wifi_scan_results_t *scan_result,int max_num);

aw_wifi_mode_t aw_wifi_get_current_mode(void);

int aw_wifi_get_network_info(aw_wifi_network_info_t *pinfo);

int aw_wifi_network_info_save_to_file(aw_wifi_network_info_t *pinfo);

int aw_wifi_connect(const char *ssid,const char *password);

int aw_wifi_ap_start(char *ssid,char *pwd);

int aw_wifi_ap_stop(void);

int aw_wifi_disconnect(void);

void aw_wifi_auto_reconnect(void);

int aw_wifi_init(aw_attr_t *attr);

int aw_wifi_deinit(void);

void aw_wifi_event(aw_wifi_state_t event);

void aw_wifi_indicate_event_handle(aw_wifi_state_t event_cmd);

void aw_wifi_status_handle(aw_wifi_state_t event);
#if __cplusplus
};  // extern "C"
#endif
#endif
