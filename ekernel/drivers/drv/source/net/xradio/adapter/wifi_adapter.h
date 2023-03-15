#ifndef WIFI_ADAPTER_H
#define WIFI_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

int wifi_init(void);

void wifi_deinit(void);

int wifi_connect(const char *ssid, const char *passwd);

int wifi_disconnect(void);


int wifi_get_mac_address(char *mac);


int wifi_get_rssi(int* rssi);


int wifi_set_channel(int chan);

int wifi_on(int mode);

int wifi_off(void);

int wifi_get_ip_address(char *ipaddr);

int wifi_get_ap_bssid(unsigned char *bssid);

void wifi_reconnect(void);

int wifi_ap_start(char *ssid,char *pwd);//(CONST WF_AP_CFG_IF_S *cfg)

int wifi_ap_stop(void);

int wifi_sta_recv_spec_frm(uint8_t enable, uint32_t channel, uint32_t frm_type);

int wifi_sta_send_raw_frm(uint8_t *frm, int frm_len, uint32_t channel, uint32_t frm_type);

int wifi_scan(aw_wifi_scan_results_t *scan_result,int max_num);
#ifdef __cplusplus
}
#endif

#endif
