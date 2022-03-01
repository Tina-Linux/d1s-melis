#ifndef __BCM_WIFI_H__
#define __BCM_WIFI_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

// For Gotham board
#define WIFI_ERROR(args) \
  do {                   \
    MY_PRINTF args;      \
  } while (0 == 1)
#define WIFI_INFO(args) \
  do {                  \
    MY_PRINTF args;     \
  } while (0 == 1)
#define WIFI_DEBUG(args) \
  do {                   \
    MY_PRINTF args;      \
  } while (0 == 1)

// Don't change this macro, it's a fixed value
#define MHD_NUMCHANNELS    64

typedef enum {
  MHD_SECURE_OPEN = 0,  // OPEN
  MHD_WPA_PSK_AES,      // WPA-PSK AES
  MHD_WPA2_PSK_AES,     // WPA2-PSK AES
  MHD_WEP_OPEN,         // WEP+OPEN
  MHD_WEP_SHARED,       // WEP+SHARE
  MHD_WPA_PSK_TKIP,     // WPA-PSK TKIP
  MHD_WPA_PSK_MIXED,    // WPA-PSK AES & TKIP MIXED
  MHD_WPA2_PSK_TKIP,    // WPA2-PSK TKIP
  MHD_WPA2_PSK_MIXED,   // WPA2-PSK AES & TKIP MIXED
  MHD_WPA3_PSK_SAE,     // WPA3-PSK SAE
  MHD_WPS_OPEN,         // WPS OPEN, NOT supported
  MHD_WPS_AES,          // WPS AES, NOT supported
  MHD_IBSS_OPEN,        // ADHOC, NOT supported
  MHD_WPA_ENT_AES,      // WPA-ENT AES, NOT supported
  MHD_WPA_ENT_TKIP,     // WPA-ENT TKIP, NOT supported
  MHD_WPA_ENT_MIXED,    // WPA-ENT AES & TKIP MIXED, NOT supported
  MHD_WPA2_ENT_AES,     // WPA2-ENT AES, NOT supported
  MHD_WPA2_ENT_TKIP,    // WPA2-ENT TKIP, NOT supported
  MHD_WPA2_ENT_MIXED,   // WPA2-ENT AES & TKIP MIXED, NOT supported

  MHD_STA_SECURITY_MAX  // Enum count, used for sanity check.
} mhd_sta_security_t;

typedef enum {
  MHD_AP_OPEN = 0,        // 0 OPEN
  MHD_AP_WPA_AES_PSK,     // 1 WPA-PSK AES
  MHD_AP_WPA2_AES_PSK,    // 2 WPA2-PSK AES
  MHD_AP_WEP_OPEN,        // 3 WEP+OPEN
  MHD_AP_WEP_SHARED,      // 4 WEP+SHARE
  MHD_AP_WPA_TKIP_PSK,    // 5 WPA-PSK TKIP
  MHD_AP_WPA_MIXED_PSK,   // 6 WPA-PSK AES & TKIP MIXED
  MHD_AP_WPA2_TKIP_PSK,   // 7 WPA2-PSK TKIP
  MHD_AP_WPA2_MIXED_PSK,  // 8 WPA2-PSK AES & TKIP MIXED
  MHD_AP_WPS_OPEN,        // 9 WPS OPEN, NOT supported
  MHD_AP_WPS_AES,         // 10 WPS AES, NOT supported

  MHD_AP_SECURITY_MAX  // Enum count, used for sanity check.
} mhd_ap_security_t;

typedef enum {
  MHD_JOIN_STATE_SCAN_START = 0, // Start to scan before association
  MHD_JOIN_STATE_SCAN_FAIL,      // Return scan failure
  MHD_JOIN_STATE_SCAN_DONE,      // Return scan success
  MHD_JOIN_STATE_JOIN_START,     // Start to join remote AP
  MHD_JOIN_STATE_JOIN_FAIL,      // Return join failure
  MHD_JOIN_STATE_JOIN_DONE,      // Return join success
  MHD_JOIN_STATE_DHCP_START,     // Start to do DHCP
  MHD_JOIN_STATE_DHCP_FAIL,      // Return DHCP failure
  MHD_JOIN_STATE_DHCP_DONE,      // Return DHCP success

  MHD_JOIN_STATE_DHCP_MAX        // Enum count, used for sanity check.
} mhd_join_state_t;

typedef enum {
  MHD_PHY_TYPE_11A = 0, // 802.11A PHY type
  MHD_PHY_TYPE_11B,     // 802.11B PHY type
  MHD_PHY_TYPE_11G,     // 802.11G PHY type
  MHD_PHY_TYPE_11N,     // 802.11N PHY type
  MHD_PHY_TYPE_11AC,    // 802.11AC PHY type
  MHD_PHY_TYPE_ERR,     // Unknown PHY type

  MHD_PHY_TYPE_MAX  // Enum count, used for sanity check.
} mhd_phy_type_t;

#define MHD_MK_CNTRY(a, b, rev)                          \
   (((unsigned char)(a)) + (((unsigned char)(b)) << 8) + \
   (((unsigned short)(rev)) << 16))

typedef enum
{
    MHD_COUNTRY_UNITED_STATES = MHD_MK_CNTRY( 'U', 'S', 999 ), /* US United_States */
    MHD_COUNTRY_EUROPE        = MHD_MK_CNTRY( 'E', 'U', 999 ), /* EU Europe */
    MHD_COUNTRY_AUSTRALIA     = MHD_MK_CNTRY( 'A', 'U', 999 ), /* AU Australia */
    MHD_COUNTRY_WORLD_WIDE    = MHD_MK_CNTRY( 'W', 'W', 999 ), /* WW World_Wide */
} mhd_country_code_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct {
  uint8_t octet[6];
} mhd_ether_addr_t;

typedef struct {
  uint8_t length;    /* SSID length */
  uint8_t value[32]; /* SSID name (AP name)  */
} mhd_ssid_t;

typedef struct {
  uint8_t octet[6]; /* Unique 6-byte MAC address */
} mhd_mac_t;

typedef struct {
  uint8_t addr[4];
} mhd_ipv4_addr_t;

typedef struct {
  char ssid[32];
  char bssid[6];
  uint32_t channel;
  uint32_t security;
  uint32_t rssi;
  char ccode[4];
} mhd_ap_info_t;

typedef struct {
  mhd_ssid_t SSID;
  uint32_t security;
  uint8_t channel;
  uint8_t security_key_length;
  char security_key[64];
  mhd_mac_t BSSID;
} mhd_ap_connect_info_t;

typedef struct {
  mhd_ipv4_addr_t ip_address; /* IP address      */
  mhd_ipv4_addr_t gateway;    /* Gateway address */
  mhd_ipv4_addr_t netmask;    /* Netmask         */
  mhd_ipv4_addr_t dns1;       /* DNS1 address      */
  mhd_ipv4_addr_t dns2;       /* DNS2 address      */
} mhd_ipv4_setting_t;

typedef struct {
  uint32_t	txframe;	/* tx data frames (including failed and successful) */
  uint32_t	txbyte;		/* tx data bytes (including failed and successful) */
  uint32_t	txfail;		/* tx fail count */
  uint32_t	txretrans;	/* tx mac retransmits */
  uint32_t	rxframe;	/* rx data frames (successful only) */
  uint32_t	rxbyte;		/* rx data bytes (successful only) */
  uint32_t	rxbadfcs;	/* rx frames with bad FCS */
} mhd_cust_counter_t;

typedef struct {
    uint32_t success_count; /* Number of 4-way handshake success */
    uint32_t failure_count; /* Number of 4-way handshake failure */
} mhd_handshake_counter_t;

// The RSSI seen on rx frames since the last stat reset, expressed as a histogram.
// Histogram should have 8 bins as follows. Each bin is uint32_t.
typedef struct {
	uint32_t rssi_8x; //   (min, -80)
	uint32_t rssi_7x; //   (-79, -70)
	uint32_t rssi_6x; //   (-69, -60)
	uint32_t rssi_5x; //   (-59, -50)
	uint32_t rssi_4x; //   (-49, -40)
	uint32_t rssi_3x; //   (-39, -30)
	uint32_t rssi_2x; //   (-29, -20)
	uint32_t rssi_1x; //   (-19, -10)
} rx_rssi_hist_t;

// The MCS rate and channel bandwidth used for the tx frames successfully
// transmitted since the last stat reset, expressed as a histogram.
// The histogram should have 30 bins as follows. Each bin is uint32_t.
typedef struct {
	uint32_t mcs0_bw20; // mcs index 0, bandwidth 20 Mbps
	uint32_t mcs0_bw40; // mcs index 0, bandwidth 40 Mbps
	uint32_t mcs0_bw80; // mcs index 0, bandwidth 80 Mbps
	uint32_t mcs1_bw20; // mcs index 1, bandwidth 20 Mbps
	uint32_t mcs1_bw40; // mcs index 1, bandwidth 40 Mbps
	uint32_t mcs1_bw80; // mcs index 1, bandwidth 80 Mbps
	uint32_t mcs2_bw20; // mcs index 2, bandwidth 20 Mbps
	uint32_t mcs2_bw40; // mcs index 2, bandwidth 40 Mbps
	uint32_t mcs2_bw80; // mcs index 2, bandwidth 80 Mbps
	uint32_t mcs3_bw20; // mcs index 3, bandwidth 20 Mbps
	uint32_t mcs3_bw40; // mcs index 3, bandwidth 40 Mbps
	uint32_t mcs3_bw80; // mcs index 3, bandwidth 80 Mbps
	uint32_t mcs4_bw20; // mcs index 4, bandwidth 20 Mbps
	uint32_t mcs4_bw40; // mcs index 4, bandwidth 40 Mbps
	uint32_t mcs4_bw80; // mcs index 4, bandwidth 80 Mbps
	uint32_t mcs5_bw20; // mcs index 5, bandwidth 20 Mbps
	uint32_t mcs5_bw40; // mcs index 5, bandwidth 40 Mbps
	uint32_t mcs5_bw80; // mcs index 5, bandwidth 80 Mbps
	uint32_t mcs6_bw20; // mcs index 6, bandwidth 20 Mbps
	uint32_t mcs6_bw40; // mcs index 6, bandwidth 40 Mbps
	uint32_t mcs6_bw80; // mcs index 6, bandwidth 80 Mbps
	uint32_t mcs7_bw20; // mcs index 7, bandwidth 20 Mbps
	uint32_t mcs7_bw40; // mcs index 7, bandwidth 40 Mbps
	uint32_t mcs7_bw80; // mcs index 7, bandwidth 80 Mbps
	uint32_t mcs8_bw20; // mcs index 8, bandwidth 20 Mbps
	uint32_t mcs8_bw40; // mcs index 8, bandwidth 40 Mbps
	uint32_t mcs8_bw80; // mcs index 8, bandwidth 80 Mbps
	uint32_t mcs9_bw20; // mcs index 9, bandwidth 20 Mbps
	uint32_t mcs9_bw40; // mcs index 9, bandwidth 40 Mbps
	uint32_t mcs9_bw80; // mcs index 9, bandwidth 80 Mbps
} tx_mcs_hist_t;

// The average of the number of MPDUs in each AMPDU since the last stat reset.
// This is a running average.
typedef struct {
	uint32_t avg; // average number of MPDUs
	uint32_t sum; // total number of MPDUs
	uint32_t cnt; // transmit times
} tx_aggr_avg_t;

// The number of transmit retries for a given frame, expressed as a histogram.
// The histogram should have 5 bins as follows. Each bin is uint32_t.
typedef struct {
	uint32_t retries_0; //   0 retries
	uint32_t retries_1; //   1 retries
	uint32_t retries_2; //   2 retries
	uint32_t retries_3; //   3 retries
	uint32_t retries_4; //   4 or more retries
} tx_retry_hist_t;

// Histogram for rx rssi, tx rate/bandwidth, tx retry times, tx aggr average.
typedef struct {
	rx_rssi_hist_t  rx_rssi_hist;
	tx_mcs_hist_t   tx_mcs_hist;
	tx_retry_hist_t tx_retry_hist;
	tx_aggr_avg_t   tx_aggr_avg;
} mhd_cust_stats_t;

typedef uint32_t host_time_t;
extern host_time_t host_rtos_get_time(void);

///////////////////////////////////////////////////////////////////////

// API for WiFi init
////////////////////////////////////////

// download fw/nvram image, driver init
// return 0:success, 1:failed
extern int mhd_module_init(void);

// scan in station mode
// return 0:success, 1:failed
extern int mhd_start_scan(void);

// Utils
////////////////////////////////////////
extern void mhd_ping(const char* ip_addr);
extern void host_rtos_delay_milliseconds(uint32_t num_ms);
extern int mhd_get_mpc(void);
extern int mhd_bus_keep_awake(int awake);

// API for softap mode
////////////////////////////////////////

// start softap
// ssid:  no more than 32 bytes
// password: no more than 32 bytes
// security: refer to mhd_ap_security_t
// channel: any channel supported by the current power table
//          use mhd_get_channels() to get the supported channels
extern int mhd_softap_start(const char* ssid,
                            const char* password,
                            uint8_t security,
                            uint8_t channel);

// stop softap.
// input 0:not force, 1:force to stop
// return 0:success, 1:failed
extern int mhd_softap_stop(uint8_t force);

// get softap state.
// return 0:disabled, 1:enabled
extern int mhd_softap_get_state(void);

// get associated client list.
// return 0:success, -1:failed
extern int mhd_softap_get_mac_list(mhd_mac_t *mac_list, uint32_t *count);

// deauthenticate associated client
// return 0:success, -1:failed
extern int mhd_softap_deauth_assoc_sta(const mhd_mac_t* mac);

// start dhcp deamon with gateway ip address
// return 0:success, -1:failed
extern int mhd_softap_start_dhcpd(uint32_t ip_address);

// API for station mode
////////////////////////////////////////

// get scan result in station mode
// return 0:success, 1:failed
extern int mhd_get_scan_results(mhd_ap_info_t* results, int* num);

// station connects to ap.
// security: refer to enum mhd_sta_security_t
// return 0:success, 1:failed
extern int mhd_sta_connect(const char* ssid,
                           const char* bssid,
                           uint8_t security,
                           char* password,
                           uint8_t channel);

// station reassociate with ap. 0:success, 1:failed
extern int mhd_sta_reassociate(void);

// station leaves ap and network down
// input 0:not force, 1:force to disconnect
// return 0:success, 1:failed
extern int mhd_sta_disconnect(uint8_t force);

// get station state.
// return 0:disabled, 1:enabled
extern int mhd_sta_get_state(void);

// get rssi in sta mode
// return -1:error, others:rate*2 (return value is not real rate)
// note: should be used AFTER connected to remote AP
//         real_rate = return_value / 2
extern int mhd_sta_get_rate(void);

// get rate in sta mode
// return 0xffff:error, others:rssi
// note: should be used AFTER connected to remote AP
extern int mhd_sta_get_rssi(void);

// get phy type in sta mode
// return phy types as defined in mhd_phy_type_t
// note: should be used AFTER connected to remote AP
extern mhd_phy_type_t mhd_sta_get_phy_type(void);

// station get connection state.
// return 0:disconnected, 1:connected
extern int mhd_sta_get_connection(void);

// set station interface up, and start DHCP or config static ip if necessary
// eg1: for DHCP
//      mhd_sta_network_up(0, 0, 0);
// eg2: for static IP ip[192.168.1.98], gateway[192.168.1.1],
// netmask[255.255.255.0]
//      mhd_sta_network_up(0xc0a80162, 0xc0a80101, 0xffffff00);
extern int mhd_sta_network_up(uint32_t ip, uint32_t gateway, uint32_t netmask);

// set station interface down
extern int mhd_sta_network_down(void);

// set power save mode
extern int mhd_sta_set_powersave(uint8_t mode, uint8_t time_ms);

// function type for network link up/down callback.
typedef void (*mhd_link_callback_t)(void);

// register callback function for network link up and link down.
// return 1: failed, 0: successful
extern int mhd_sta_register_link_callback(mhd_link_callback_t link_up_cb,
                                          mhd_link_callback_t link_down_cb);

// deregister callback function for network link up and link down.
// return 1: failed, 0: successful
extern int mhd_sta_deregister_link_callback(mhd_link_callback_t link_up_cb,
                                            mhd_link_callback_t link_down_cb);

// preinit mac address to buffer, when wifi bring up, station mode will read
// from the buffer and use this as mac address return 1: failed, 0: successful
extern void mhd_host_set_mac_address(mhd_mac_t mac);

// preinit ap bssid to buffer, when wifi bring up, ap mode will read from the
// buffer and use this as bssid return 1: failed, 0: successful
extern void mhd_host_set_ap_bssid(mhd_mac_t mac);

// Extended Scan with specific SSID
extern int mhd_start_scan_ext(mhd_ssid_t* ssid,
                              mhd_mac_t* mac,
                              uint16_t* channel_list);

// get channels supported in power table.
// return 0:success, 1:failed
// Note: The number for channels_buffer should be a fixed value: MHD_NUMCHANNELS + 1
//       MHD_NUMCHANNELS is fixed to be 64
extern int mhd_get_channels(uint32_t channels_buffer[]);

// This function is to validate the channel.
// return 1: valid, 0: invalid, -1: fail to get channels list
extern int bcm_wifi_is_valid_channel(uint8_t channel);

// WL Command
extern int mhd_wl_cmd(uint32_t argc, char** argv);

// Get current associated AP's info.
// This function only available AFTER STA success to join AP.
extern void mhd_get_current_ap_info(mhd_ap_connect_info_t* ap);

// Callback function type for join state.
typedef void (*mhd_join_state_callback_t)(mhd_join_state_t);
// Function to register callback for getting join state.
extern int mhd_sta_register_join_state_callback(
    mhd_join_state_callback_t join_state_cb);

// get wifi chip information.
// input pointer of chip id and revision.
// return 0: successful, others: failed
extern int mhd_get_chip_info(uint32_t* chip_id, uint32_t* chip_rev);
// get mhd drvier version.
// return version string.
extern const char* mhd_get_driver_version(void);
// get wifi firmware information.
// input buffer pointer and length.
// return 0: successful, others: failed
extern int mhd_get_firmware_version(char* version, int length);

// Callback function type for latency observe
typedef void (*mhd_latency_observe_callback_t)( uint32_t latency_ms );
// Function to register callback for getting both driver latency and firmware latency.
extern int mhd_sta_register_latency_observe_callback(
    mhd_latency_observe_callback_t driver_latency_cb,
    mhd_latency_observe_callback_t firmware_latency_cb);

// reset custom counters in sta mode
// return 0:success, -1: error
// Note: this function will NOT clear 4-way handshake counters
extern int mhd_sta_clear_cust_counters(void);
// get custom counters in sta mode
// return 0:success, -1:error
extern int mhd_sta_get_cust_counters(mhd_cust_counter_t *cust_counters);

// reset custom statistics
// return 0:success, -1: error
extern int mhd_clear_cust_stats(void);
// get custom statistics
// return 0:success, -1:error
extern int mhd_get_cust_stats(mhd_cust_stats_t *cust_stats);

// Clear 4-way handshake counters
extern void mhd_sta_clear_handshake_counter(void);
// Get 4-way handshake counters
extern void mhd_sta_get_handshake_counter(mhd_handshake_counter_t * counter);

// set country code
// Note: this function should be called before mhd_module_init() if you want to set country code.
//          Setting incorrect country code will cause wifi firmware fail to bring up.
//          e.g. mhd_set_country_code(MHD_COUNTRY_WORLD_WIDE);
//               mhd_module_init();
extern void mhd_set_country_code(mhd_country_code_t country);


// App shell commands.
void bcm_station_commands(uint32_t argc, char** argv);
void bcm_softap_commands(uint32_t argc, char** argv);

// GOOGLE: Get full list of valid Wifi channel numbers for current power table.
// return >0: number of channels in list, 0: error
extern size_t bcm_wifi_get_channel_list(uint8_t* chan_out, size_t chan_out_size);

// GOOGLE: Get actual country code letters used by the underlying Wifi system.
void bcm_wifi_country_code_get_letters(
    mhd_country_code_t country_code,
    char country_letters[3]);

// GOOGLE: Added country code conversion function to go from string to
// power table country entry.
// Return a Country Code enum value from a string representation such as
// "US/001", "WW/999", etc.
mhd_country_code_t bcm_wifi_convert_country_code(const char* country_code);

// To use resume mode for driver init without load wifi firmware
void mhd_use_wifi_resume( int enable );

// For backup driver data
void* mhd_wifi_resume_backup(int *size);

// For restore driver data
void mhd_wifi_resume_restore(uint8_t *data, int size);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
#endif
