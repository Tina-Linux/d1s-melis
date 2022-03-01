#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bcm_wifi.h"
#include "my_print.h"
#include "error_codes.h"

#include "sys/sys_debug.h"
#include "os_semaphore.h"


#include "hal_sdhost.h"
#include "sdmmc.h"
#include "sdio.h"

#include "_sd_define.h"
#include "_sdhost.h"


#include "lwip/tcpip.h"

// Function to get latest timestamp.
extern host_time_t host_rtos_get_time(void);
// Input index to get string for security type.
// Make this function to avoid duplicating these strings and waste memory.
extern const char *mhd_get_security_string_by_index(uint8_t security);
static int bcm_sta_link_reg(void);

#ifndef USE_STATIC_IP
extern int mhd_sta_dhcpc_enable( uint8_t enable );
#endif
/////////////////////////////////

void bcm_wifi_start(void) {
  int ret;

  mhd_set_country_code(MHD_MK_CNTRY( 'U', 'S', 0 ));

  host_time_t start_time = host_rtos_get_time();
  ret = mhd_module_init();
  WIFI_DEBUG(("  <<Module Init>> [%d ms]\n",
              (int)(host_rtos_get_time() - start_time)));
  if (ret) {
    WIFI_ERROR(("Error mhd_module_init return = %d\n", ret));
  }
  // Clear handshake counters for STA
  mhd_sta_clear_handshake_counter();
  bcm_sta_link_reg();
}

/////////////////////////////////

// This function is to validate the channel.
// return 1: valid, 0: invalid, -1: fail to get channels list
int bcm_wifi_is_valid_channel(uint8_t channel)
{
  uint32_t chan_buf[MHD_NUMCHANNELS+1]; // Don't modify
  uint32_t chan_num = 0;
  int i;

  memset(chan_buf, 0, sizeof(chan_buf));
  if (mhd_get_channels(chan_buf) != 0) {
    return -1; // failed to get from fw
  }

  chan_num = chan_buf[0];
#if 0 // dump supported channels
  WIFI_INFO(("Supported %d channels: \n", chan_num));
  for (i=1; i<=chan_num; i++) {
    WIFI_INFO(("%d ", chan_buf[i]));//channels start from index 1 in buffer array
  }
  WIFI_INFO(("\n"));
#endif

  for (i=0; i<chan_num; i++) {
    if (channel == (uint8_t)chan_buf[i]) {
      return 1;
    }
  }
  return 0;
}

typedef void (*mhd_client_callback_t)(mhd_mac_t);
typedef void (*mhd_ap_dhcpd_lease_callback_t)(mhd_mac_t, uint32_t);
int mhd_softap_register_client_callback( mhd_client_callback_t client_assoc_cb,
                                         mhd_client_callback_t client_disassoc_cb );
int mhd_softap_register_dhcpd_callback( mhd_ap_dhcpd_lease_callback_t ap_dhcpd_lease_cb );

static void ap_client_assoc(mhd_mac_t mac) {
  WIFI_INFO(("client assoc: %02x:%02x:%02x:%02x:%02x:%02x\n",
    mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]));
}

static void ap_client_disassoc(mhd_mac_t mac) {
  WIFI_INFO(("client disassoc: %02x:%02x:%02x:%02x:%02x:%02x\n",
    mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]));
}

static void ap_dhcpd_lease(mhd_mac_t mac, uint32_t ip_addr) {
  WIFI_INFO(("dhcpd lease: %02x:%02x:%02x:%02x:%02x:%02x 0x%08x\n",
    mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],
    ip_addr));
}

extern int mhd_softap_start_dhcpd(uint32_t ip_address);
void bcm_ap_start(uint32_t argc, char** argv) {
  char ssid[32];
  char password[64];
  uint8_t security;
  uint8_t channel;
  char* security_name[] = {"open",
                           "wpa_psk_aes",
                           "wpa2_psk_aes",
                           "wep-open",
                           "wep-shared",
                           "wpa_psk_tkip",
                           "wpa2_psk_tkip",
                           "wpa2_psk_mix"};
  uint32_t ip_address = 0xc0a80101;
  int ret;

  if (argc < 4) {
    WIFI_ERROR(("Error argments number is less than 4\n"));
    return;
  }

  memset(ssid, 0, sizeof(ssid));
  strcpy(ssid, argv[1]);
  security = (uint8_t)atoi((const char*)argv[3]);
  memset(password, 0, sizeof(password));
  if (security != 0) {
    strcpy(password, argv[2]);
  }
  channel = (uint8_t)atoi((const char*)argv[4]);
  if (bcm_wifi_is_valid_channel(channel) != 1) {
    WIFI_ERROR(("Error input channel=%d NOT supported\n", channel));
    return;
  }
  WIFI_INFO(("Config Softap\n"));
  WIFI_INFO(("  SSID    : %s\n", ssid));
  WIFI_INFO(("  Password: %s\n", password));
  WIFI_INFO(("  Security: %s\n", security_name[security]));
  WIFI_INFO(("  Channel : %d\n", channel));
  ret = mhd_softap_start(ssid, password, security, channel);
  if (ret) {
    WIFI_ERROR(("Error mhd_softap_start return = %d\n", ret));
    return;
  }
  mhd_softap_start_dhcpd(ip_address);
  mhd_softap_register_client_callback(ap_client_assoc, ap_client_disassoc);
  mhd_softap_register_dhcpd_callback(ap_dhcpd_lease);
}

void bcm_ap_stop(void) {
  int ret;

  ret = mhd_softap_stop(1);
  if (ret) {
    WIFI_ERROR(("Error mhd_softap_stop ret = %d\n", ret));
  }
}

void bcm_ap_get_state(void) {
  int ret;
  ret = mhd_softap_get_state();
  if (ret) {
    WIFI_ERROR(("Error mhd_softap_get_state = %d\n", ret));
  }
}

#define MAC_LIST_COUNT    8
void bcm_ap_get_assoc_list(void) {
  int ret;
  mhd_mac_t mac_list[MAC_LIST_COUNT];
  uint32_t count = MAC_LIST_COUNT;
  int i;

  ret = mhd_softap_get_mac_list(mac_list, &count);
  if (ret) {
    WIFI_INFO(("mhd_softap_get_assoc_list = %d\n", ret));
    return;
  }
  for (i=0; i<count; i++) {
    WIFI_INFO(("MAC[%d]: %02X:%02X:%02X:%02X:%02X:%02X\n", i,
               mac_list[i].octet[0], mac_list[i].octet[1], mac_list[i].octet[2],
               mac_list[i].octet[3], mac_list[i].octet[4], mac_list[i].octet[5]));
  }
}

void bcm_ap_deauth_assoc_sta(uint32_t argc, char **argv) {
  int ret;
  mhd_mac_t addr;

  if (argc == 1) {
    addr.octet[0] = 0xFF;
    addr.octet[1] = 0xFF;
    addr.octet[2] = 0xFF;
    addr.octet[3] = 0xFF;
    addr.octet[4] = 0xFF;
    addr.octet[5] = 0xFF;
  } else if (argc == 2) {
    // hardcode the client mac address for test
    addr.octet[0] = 0x5C;
    addr.octet[1] = 0xA8;
    addr.octet[2] = 0x6A;
    addr.octet[3] = 0x36;
    addr.octet[4] = 0xD9;
    addr.octet[5] = 0xC8;
  } else {
    return;
  }
  WIFI_INFO(("Input mac %X:%X:%X:%X:%X:%X\n",
              addr.octet[0],addr.octet[1],addr.octet[2],
              addr.octet[3],addr.octet[4],addr.octet[5]));
  ret = mhd_softap_deauth_assoc_sta(&addr);
  if (ret) {
    WIFI_INFO(("mhd_softap_deauth_assoc_sta = %d\n", ret));
  }
}
////////////////////////////////////
void bcm_sta_scan(void) {
  int ret;

  ret = mhd_start_scan();
  if (ret) {
    WIFI_ERROR(("Error mhd_start_scan = %d\n", ret));
  }
}

extern int mhd_start_scan_ext(mhd_ssid_t* ssid,
                              mhd_mac_t* mac,
                              uint16_t* channel_list);
void bcm_sta_scan_ext(uint32_t argc, char** argv) {
  char* fixed_ssid = "mtest2";
  char fixed_mac[6] = {0x14, 0x75, 0x90, 0x39, 0x6D, 0x16};
  uint16_t fixed_chlist[] = {1, 6, 11, 0};

  mhd_ssid_t ssid;
  mhd_mac_t mac;
  int ret = -1;

  // input different param will do different test
  // 1: scan fixed ssid
  // 2: scan fixed mac address
  // 3: scan fixed channel list
  if (atoi((const char*)argv[1]) == 1) {
    // Init ssid
    memset(&ssid, 0, sizeof(ssid));
    ssid.length = strlen(fixed_ssid);
    memcpy(ssid.value, fixed_ssid, ssid.length);

    // scan fixed ssid
    ret = mhd_start_scan_ext(&ssid, NULL, NULL);
  } else if (atoi((const char*)argv[1]) == 2) {
    // Init mac
    memset(&mac, 0, sizeof(mac));
    memcpy(mac.octet, fixed_mac, sizeof(mac));

    // scan fixed mac address
    ret = mhd_start_scan_ext(NULL, &mac, NULL);
  } else if (atoi((const char*)argv[1]) == 3) {
    // scan fixed channel list
    ret = mhd_start_scan_ext(NULL, NULL, fixed_chlist);
  }

  if (ret) {
    WIFI_ERROR(("Error mhd_start_scan = %d\n", ret));
  }
}

static void my_print_results(mhd_ap_info_t* results, int num) {
  int k;
  mhd_ap_info_t* record;

  record = &results[0];
  for (k = 0; k < num; k++, record++) {
    /* Print SSID */
    WIFI_INFO(("\n[%03d]\n", k + 1));
    WIFI_INFO(("       SSID          : %s\n", record->ssid));

    /* Print other network characteristics */
    WIFI_INFO(("       BSSID         : %02X:%02X:%02X:%02X:%02X:%02X\n",
               record->bssid[0],
               record->bssid[1],
               record->bssid[2],
               record->bssid[3],
               record->bssid[4],
               record->bssid[5]));
    WIFI_INFO(("       RSSI          : %ddBm\n", (int)record->rssi));
    // clang-format off
    WIFI_INFO((
        "       Security      : %s\n",
        (record->security == MHD_SECURE_OPEN)
            ? "Open"
            : (record->security == MHD_WPA_PSK_AES) ? "WPA PSK AES"
            : (record->security == MHD_WPA2_PSK_AES) ? "WPA2 PSK AES"
            : (record->security == MHD_WEP_OPEN) ? "WEP OPEN"
            : (record->security == MHD_WEP_SHARED) ? "WEP SHARED"
            : (record->security == MHD_WPA_PSK_TKIP) ? "WPA PSK TKIP"
            : (record->security == MHD_WPA_PSK_MIXED) ? "WPA PSK MIXED"
            : (record->security == MHD_WPA2_PSK_TKIP) ? "WPA2 PSK TKIP"
            : (record->security == MHD_WPA2_PSK_MIXED) ? "WPA2 PSK MIXED"
            : (record->security == MHD_WPA3_PSK_SAE) ? "WPA3 PSK SAE"
            : (record->security == MHD_IBSS_OPEN) ? "ADHOC"
            : (record->security == MHD_WPS_OPEN) ? "WPS OPEN"
            : (record->security == MHD_WPS_AES) ? "WPS AES"
            : (record->security == MHD_WPA_ENT_TKIP) ? "WPA ENT TKIP"
            : (record->security == MHD_WPA_ENT_AES) ? "WPA ENT AES"
            : (record->security == MHD_WPA_ENT_MIXED) ? "WPA ENT Mixed"
            : (record->security == MHD_WPA2_ENT_AES) ? "WPA2 ENT AES"
            : (record->security == MHD_WPA2_ENT_TKIP) ? "WPA2 ENT TKIP"
            : (record->security == MHD_WPA2_ENT_MIXED) ? "WPA2 ENT Mixed"
            : "Unknown"));
    // clang-format on
    WIFI_INFO(("       Channel       : %d\n", (int)record->channel));
    if (record->ccode[0] != 0) {
      WIFI_INFO(("       Country       : %c%c\n", record->ccode[0],record->ccode[1]));
    }
  }
}

#ifdef MHD_WLCMD
#define BCM_MAX_SCANRESULTS 50
#else
#define BCM_MAX_SCANRESULTS 150
#endif
static mhd_ap_info_t my_scan_results[BCM_MAX_SCANRESULTS];
int bcm_scan_get_results(void) {
  int num = BCM_MAX_SCANRESULTS;

  memset((char*)my_scan_results, 0, sizeof(my_scan_results));
  num = sizeof(my_scan_results) / sizeof(mhd_ap_info_t);
  mhd_get_scan_results(my_scan_results, &num);
  my_print_results(my_scan_results, num);
  return 0;
}

static void my_link_up(void) { WIFI_INFO(("my link up handler\n")); }

static void my_link_down(void) { WIFI_INFO(("my link down handler\n")); }

static int bcm_sta_link_reg(void) {
  return mhd_sta_register_link_callback(my_link_up, my_link_down);
}

static int bcm_sta_link_dereg(void) {
  return mhd_sta_deregister_link_callback(my_link_up, my_link_down);
}

static void print_ap_info(mhd_ap_connect_info_t* ap) {
  WIFI_INFO(("Connected to -\n"));
  WIFI_INFO(("  SSID     : %s\n", ap->SSID.value));
  WIFI_INFO(("  BSSID    : %02x:%02x:%02x:%02x:%02x:%02x\n",
             ap->BSSID.octet[0],
             ap->BSSID.octet[1],
             ap->BSSID.octet[2],
             ap->BSSID.octet[3],
             ap->BSSID.octet[4],
             ap->BSSID.octet[5]));
  WIFI_INFO(("  Channel  : %d\n", ap->channel));
  if (ap->security_key_length < 64) {
    WIFI_INFO(("  Password : %s\n", &ap->security_key[0]));
  } else if (ap->security_key_length == 64) {
    // TODO: Use dump instead of print
  }
//  WIFI_INFO(("  Security : %s\n",
//             mhd_get_security_string_by_index((uint8_t)ap->security)));
}

#ifdef MHD_JOIN_STATE
static void bcm_join_state(mhd_join_state_t state) {
  char* join_state[] = {"SCAN_START",
                        "SCAN_FAIL ",
                        "SCAN_DONE ",
                        "JOIN_START",
                        "JOIN_FAIL ",
                        "JOIN_DONE ",
                        "DHCP_START",
                        "DHCP_FAIL ",
                        "DHCP_DONE "};
  WIFI_INFO(("  [%s] [%d]\n", join_state[state], (int)host_rtos_get_time()));

  if (state == MHD_JOIN_STATE_JOIN_DONE) {
    mhd_ap_connect_info_t ap;
    memset(&ap, 0, sizeof(ap));
    mhd_get_current_ap_info(&ap);
    print_ap_info(&ap);
  }
}
#endif  // MHD_JOIN_STATE

#ifdef MHD_LATENCY_OBSERVE
static void bcm_driver_latency_cb(uint32_t latency_ms) {
  //Driver Latency (ms)
  WIFI_INFO(("Driver Latency: %d ms\n", latency_ms));
}
static void bcm_firmware_latency_cb(uint32_t latency_us) {
  //Firmware Latency (us)
  WIFI_INFO(("Firmware Latency: %d us\n", latency_us));
}
#endif // MHD_LATENCY_OBSERVE

void bcm_sta_connect(uint32_t argc, char** argv) {
  char ssid[32];
  char security_key[64];
  char* password = NULL;
  uint8_t security = 0;
  char* security_name[] = {"OPEN",
                           "WPA-PSK AES",
                           "WPA2-PSK AES",
                           "WEP-OPEN",
                           "WEP-SHARED",
                           "WPA-PSK TKIP",
                           "WPA-PSK Mixed",
                           "WPA2-PSK TKIP",
                           "WPA2-PSK Mixed",
                           "WPA3-PSK SAE"};
  int ret;

  if (argc < 3) {
    WIFI_ERROR(("Error argments number is less than 3\n"));
    return;
  }

  // Init ssid
  memset(ssid, 0, sizeof(ssid));
  strcpy(ssid, argv[1]);
  // Init security type
  security = atoi(argv[2]);
  if (security > sizeof(security_name)) {
    WIFI_ERROR(("Error security beyond range, reset it to OPEN mode\n"));
    security = 0;
  }
  // Init security key
  if (security != 0 && argc > 2) {
    memset(security_key, 0, sizeof(security_key));
    strcpy(security_key, argv[3]);
    password = security_key;
  }
  // Print inputs
  WIFI_INFO(("Connect to -\n"));
  WIFI_INFO(("  SSID    : %s\n", ssid));
  WIFI_INFO(("  Password : %s\n", (security == 0) ? "NOT Needed" : password));
  WIFI_INFO(("  Security: %s\n", security_name[security]));

#ifdef MHD_JOIN_STATE
  ret = mhd_sta_register_join_state_callback(bcm_join_state);
  if (ret != 0) {
    WIFI_ERROR(("Error register join state callback\n"));
  }
#endif  // MHD_JOIN_STATE

#ifndef USE_STATIC_IP
  mhd_sta_dhcpc_enable(1);
#endif

  //bcm_sta_link_reg();
  uint32_t start_time = host_rtos_get_time();
  ret = mhd_sta_connect(ssid, NULL, security, password, 0);
  WIFI_DEBUG(("  <<Connect AP>> [%d ms]\n", host_rtos_get_time() - start_time));
  if (ret) {
    WIFI_ERROR(("Error mhd_sta_connect [%d %s]\n", ret, BcmErrorCodeString(ret)));
    return;
  }
  start_time = host_rtos_get_time();

#ifndef USE_STATIC_IP
  // set station ip from external dhcp server
  mhd_sta_network_up(0, 0, 0);
#else
  // set staion ip from static ip
  // eg: ip[192.168.43.98], gateway[192.168.43.1], netmask[255.255.255.0]
  mhd_sta_network_up(0xc0a82b62, 0xc0a82b01, 0xffffff00);
#endif
  WIFI_DEBUG(("  <<DHCP>> [%d ms]\n", host_rtos_get_time() - start_time));
  // mhd_sta_set_powersave(2, 200);

#ifdef MHD_LATENCY_OBSERVE
  ret = mhd_sta_register_latency_observe_callback(bcm_driver_latency_cb, bcm_firmware_latency_cb);
  if (ret != 0) {
    WIFI_ERROR(("Error register latency observe callback\n"));
  }
#endif  // MHD_LATENCY_OBSERVE
}

// If you fill full parameters, it'll igonre scanning before association to save
// time. Once failed, it'll scan and join again.
void bcm_sta_connect_ext(uint32_t argc, char** argv) {
  char ssid[32];
  char security_key[64];
  char* password = NULL;
  uint8_t security = 0;
  uint8_t channel = 0;
  char* security_name[] = {"OPEN",
                           "WPA-PSK AES",
                           "WPA2-PSK AES",
                           "WEP-OPEN",
                           "WEP-SHARED",
                           "WPA-PSK TKIP",
                           "WPA-PSK Mixed",
                           "WPA2-PSK TKIP",
                           "WPA2-PSK Mixed",
                           "WPA3-PSK SAE"};
  // didn't get input from argv, we hard code mac address here just for easier
  // to test
  char bssid_addr[6] = {0x5c, 0xa8, 0x6a, 0x36, 0xd9, 0xc8};
  char* bssid = NULL;
  int ret;

  if (argc < 3) {
    WIFI_ERROR(("Error argments number is less than 3\n"));
    return;
  }

  // Init ssid
  memset(ssid, 0, sizeof(ssid));
  strcpy(ssid, argv[1]);
  // Init security type
  security = atoi((const char*)argv[2]);
  if (security > sizeof(security_name)) {
    WIFI_ERROR(("Error security beyond range, reset it to OPEN mode\n"));
    security = 0;
  }
  // In this example code, once you inputed channel, we auto filled fixed mac
  // bssid for you.
  if (security != 0 && argc > 3) {
    // Init security key
    memset(security_key, 0, sizeof(security_key));
    strcpy(security_key, argv[3]);
    password = security_key;
    // Init channel
    channel = (uint8_t)atoi((const char*)argv[4]);
    // Init bssid
    bssid = &bssid_addr[0];
  } else if (security == 0 && argc > 2) {
    // Init security key
    password = NULL;
    // Init channel
    channel = (uint8_t)atoi((const char*)argv[3]);
    // Init bssid
    bssid = &bssid_addr[0];
  }
  if (bcm_wifi_is_valid_channel(channel) != 1) {
    WIFI_ERROR(("Error input channel=%d NOT supported\n", channel));
    return;
  }

  // Print inputs
  WIFI_INFO(("connect to -\n"));
  WIFI_INFO(("ssid    : %s\n", ssid));
  WIFI_INFO(("security: %s\n", security_name[security]));
  if (security == 0) {
    WIFI_INFO(("password: Not needed\n"));
  } else {
    WIFI_INFO(("password: %s\n", password));
  }
  WIFI_INFO(("channel : %d\n", channel));
  if (bssid != NULL) {
    WIFI_INFO(("bssid   : %02x:%02x:%02x:%02x:%02x:%02x\n",
               bssid[0],
               bssid[1],
               bssid[2],
               bssid[3],
               bssid[4],
               bssid[5]));
  }

#ifdef MHD_JOIN_STATE
  ret = mhd_sta_register_join_state_callback(bcm_join_state);
  if (ret != 0) {
    WIFI_ERROR(("Error register join state callback\n"));
  }
#endif  // MHD_JOIN_STATE
  //bcm_sta_link_reg();
  uint32_t start_time = host_rtos_get_time();
  ret = mhd_sta_connect(ssid, bssid, security, password, channel);
  WIFI_DEBUG(("  <<Connect AP>> [%d ms]\n", host_rtos_get_time() - start_time));
  if (ret) {
    WIFI_ERROR(("Error mhd_sta_connect [%d %s]\n", ret, BcmErrorCodeString(ret)));
    return;
  }
  start_time = host_rtos_get_time();
  mhd_sta_network_up(0, 0, 0);
  WIFI_DEBUG(("  <<DHCP>> [%d ms]\n", host_rtos_get_time() - start_time));
  // mhd_sta_set_powersave(2, 200);

#ifdef MHD_LATENCY_OBSERVE
  ret = mhd_sta_register_latency_observe_callback(bcm_driver_latency_cb, bcm_firmware_latency_cb);
  if (ret != 0) {
    WIFI_ERROR(("Error register latency observe callback\n"));
  }
#endif  // MHD_LATENCY_OBSERVE
}

void bcm_sta_disconnect(void) {
  int ret;

  ret = mhd_sta_disconnect(1);
  if (ret) {
    WIFI_ERROR(("Error mhd_sta_disconnect ret = %d\n", ret));
  }
  //bcm_sta_link_dereg();
  mhd_sta_network_down();
}

void bcm_sta_get_state(void) {
  int ret;
  ret = mhd_sta_get_state();
  WIFI_INFO(("mhd_sta_get_state = %d\n", ret));
}

void bcm_sta_get_ap_info(void) {
  mhd_ap_connect_info_t ap;
  memset(&ap, 0, sizeof(ap));
  mhd_get_current_ap_info(&ap);
  print_ap_info(&ap);
}

#ifdef MHD_CUST_STATISTICS
void bcm_sta_get_rssi(void) {
  int ret;
  ret = mhd_sta_get_rssi();
  WIFI_INFO(("mhd_sta_get_rssi = %d\n", ret));
}

void bcm_sta_get_rate(void) {
  int ret;
  ret = mhd_sta_get_rate();
  WIFI_INFO(("mhd_sta_get_rate = %d\n", ret));
}

void bcm_sta_get_phy_type(void) {
  mhd_phy_type_t phy_type;
  char *phy_str[] = {"802.11A", "802.11B", "802.11G", "802.11N", "802.11AC", "Unkown"};
  phy_type = mhd_sta_get_phy_type();
  WIFI_INFO(("PHY type[%d]: %s\n", phy_type, phy_str[phy_type]));
}
#endif // MHD_CUST_STATISTICS

#ifdef MHD_CUST_COUNTERS
void bcm_sta_clear_cust_counters(void) {
  // Clear counters for RX and TX
  mhd_sta_clear_cust_counters();

  // Clear handshake counters for STA
  mhd_sta_clear_handshake_counter();
}

void bcm_sta_get_cust_counters(void) {
  int ret;
  mhd_cust_counter_t custom_counters;
  mhd_handshake_counter_t handshake_counter;

  ret = mhd_sta_get_cust_counters(&custom_counters);

  if (ret == 0) {
    WIFI_INFO(("tx data frames: %d\n", custom_counters.txframe));
    WIFI_INFO(("tx data bytes: %d\n", custom_counters.txbyte));
    WIFI_INFO(("tx fail count: %d\n", custom_counters.txfail));
    WIFI_INFO(("tx mac retransmits: %d\n", custom_counters.txretrans));
    WIFI_INFO(("rx data frames: %d\n", custom_counters.rxframe));
    WIFI_INFO(("rx data bytes: %d\n", custom_counters.rxbyte));
    WIFI_INFO(("rx bad FCS count: %d\n", custom_counters.rxbadfcs));
  } else {
    WIFI_INFO(("mhd_sta_get_cust_counters failed. ret = %d\n", ret));
  }

  mhd_sta_get_handshake_counter(&handshake_counter);
  WIFI_INFO(("4-way handshake success: %d\n", handshake_counter.success_count));
  WIFI_INFO(("4-way handshake failure: %d\n", handshake_counter.failure_count));
}
#endif // MHD_CUST_COUNTERS

void bcm_clear_cust_stats(void) {
  mhd_clear_cust_stats();
}

#define	STATS_PRVAL(name)	\
	do { \
		if(stats->tx_mcs_hist.name) \
			WIFI_INFO(("%s[%u] ", #name, stats->tx_mcs_hist.name)); \
	} while(0)

static void stats_dump_rate_bw(mhd_cust_stats_t *stats)
{
    WIFI_INFO(("Stats: TX_RATE "));
    STATS_PRVAL(mcs0_bw20);
    STATS_PRVAL(mcs0_bw40);
    STATS_PRVAL(mcs0_bw80);
    STATS_PRVAL(mcs1_bw20);
    STATS_PRVAL(mcs1_bw40);
    STATS_PRVAL(mcs1_bw80);
    STATS_PRVAL(mcs2_bw20);
    STATS_PRVAL(mcs2_bw40);
    STATS_PRVAL(mcs2_bw80);
    STATS_PRVAL(mcs3_bw20);
    STATS_PRVAL(mcs3_bw40);
    STATS_PRVAL(mcs3_bw80);
    STATS_PRVAL(mcs4_bw20);
    STATS_PRVAL(mcs4_bw40);
    STATS_PRVAL(mcs4_bw80);
    STATS_PRVAL(mcs5_bw20);
    STATS_PRVAL(mcs5_bw40);
    STATS_PRVAL(mcs5_bw80);
    STATS_PRVAL(mcs6_bw20);
    STATS_PRVAL(mcs6_bw40);
    STATS_PRVAL(mcs6_bw80);
    STATS_PRVAL(mcs7_bw20);
    STATS_PRVAL(mcs7_bw40);
    STATS_PRVAL(mcs7_bw80);
    STATS_PRVAL(mcs8_bw20);
    STATS_PRVAL(mcs8_bw40);
    STATS_PRVAL(mcs8_bw80);
    STATS_PRVAL(mcs9_bw20);
    STATS_PRVAL(mcs9_bw40);
    STATS_PRVAL(mcs9_bw80);
    WIFI_INFO(("\n"));
}

void cust_stats_print(mhd_cust_stats_t *stats)
{
	WIFI_INFO(("Stats: RX_RSSI 8x[%d] 7x[%d] 6x[%d] 5x[%d] 4x[%d] 3x[%d] 2x[%d] 1x[%d]\n",
		stats->rx_rssi_hist.rssi_8x,
		stats->rx_rssi_hist.rssi_7x,
		stats->rx_rssi_hist.rssi_6x,
		stats->rx_rssi_hist.rssi_5x,
		stats->rx_rssi_hist.rssi_4x,
		stats->rx_rssi_hist.rssi_3x,
		stats->rx_rssi_hist.rssi_2x,
		stats->rx_rssi_hist.rssi_1x));
    stats_dump_rate_bw(stats);
	WIFI_INFO(("Stats: TX_AGGR sum[%d] cnt[%d] avg[%d]\n",
		stats->tx_aggr_avg.sum,
		stats->tx_aggr_avg.cnt,
		stats->tx_aggr_avg.avg));
	WIFI_INFO(("Stats: TX_RETRY R0[%d] R1[%d] R2[%d] R3[%d] R4[%d]\n",
		stats->tx_retry_hist.retries_0,
		stats->tx_retry_hist.retries_1,
		stats->tx_retry_hist.retries_2,
		stats->tx_retry_hist.retries_3,
		stats->tx_retry_hist.retries_4));
}

void bcm_get_cust_stats(void) {
  int ret;
  mhd_cust_stats_t custom_stats;

  ret = mhd_get_cust_stats(&custom_stats);

  if (ret == 0) {
    cust_stats_print(&custom_stats);
  } else {
    WIFI_INFO(("bcm_sta_get_cust_statistics failed. ret = %d\n", ret));
  }
}

void bcm_sta_get_connection(void) {
  int ret;
  ret = mhd_sta_get_connection();
  WIFI_INFO(("mhd_sta_get_connection = %d\n", ret));
}

// Here is just an example for how to set mac address from host.
// Function mhd_host_set_mac_address() just stores the mac address in buffer.
// When wifi bring up, it will get the mac from the buffer and set into chip.
void bcm_preinit_sta_mac(void) {
  mhd_mac_t mac_addr;
  mac_addr.octet[0] = 0x00;
  mac_addr.octet[1] = 0x11;
  mac_addr.octet[2] = 0x11;
  mac_addr.octet[3] = 0x11;
  mac_addr.octet[4] = 0x11;
  mac_addr.octet[5] = 0x11;

  mhd_host_set_mac_address(mac_addr);
}

void bcm_preinit_ap_bssid(void) {
  mhd_mac_t mac_addr;
  mac_addr.octet[0] = 0x00;
  mac_addr.octet[1] = 0x22;
  mac_addr.octet[2] = 0x22;
  mac_addr.octet[3] = 0x22;
  mac_addr.octet[4] = 0x22;
  mac_addr.octet[5] = 0x22;

  mhd_host_set_ap_bssid(mac_addr);
}

////////////////////////////////////
void bcm_softap_commands(uint32_t argc, char** argv) {
  if (strcmp((const char*)argv[0], "start") == 0) {
    bcm_ap_start(argc, argv);
  } else if (strcmp((const char*)argv[0], "stop") == 0) {
    bcm_ap_stop();
  } else if (strcmp((const char*)argv[0], "state") == 0) {
    bcm_ap_get_state();
  } else if (strcmp((const char *)argv[0], "list") == 0) {
	bcm_ap_get_assoc_list();
  } else if (strcmp((const char *)argv[0], "deauth") == 0) {
    bcm_ap_deauth_assoc_sta(argc, argv);
  } else {
    WIFI_INFO(("ap start ssid password security channel\n"));
    WIFI_INFO(("ap stop\n"));
    WIFI_INFO(("ap state\n"));
    WIFI_INFO(("ap list\n"));
    WIFI_INFO(("ap deauth [mac_addr]\n"));
  }
}

void bcm_station_commands(uint32_t argc, char** argv) {
  if (strcmp((const char*)argv[0], "scan") == 0) {
    bcm_sta_scan();
  } else if (strcmp((const char*)argv[0], "scan_ext") == 0) {
    bcm_sta_scan_ext(argc, argv);
  } else if (strcmp((const char*)argv[0], "scanresults") == 0) {
    bcm_scan_get_results();

  } else if (strcmp((const char*)argv[0], "connect") == 0) {
    bcm_sta_connect(argc, argv);
  } else if (strcmp((const char*)argv[0], "connect_ext") == 0) {
    bcm_sta_connect_ext(argc, argv);
  } else if (strcmp((const char*)argv[0], "reconnect") == 0) {
    mhd_sta_reassociate();
  } else if (strcmp((const char*)argv[0], "disconnect") == 0) {
    bcm_sta_disconnect();
  } else if (strcmp((const char*)argv[0], "connection") == 0) {
    bcm_sta_get_connection();
  } else if (strcmp((const char*)argv[0], "state") == 0) {
    bcm_sta_get_state();

#ifdef MHD_CUST_STATISTICS
  } else if (strcmp((const char*)argv[0], "rssi") == 0) {
    bcm_sta_get_rssi();
  } else if (strcmp((const char*)argv[0], "rate") == 0) {
    bcm_sta_get_rate();
  } else if (strcmp((const char*)argv[0], "phy_type") == 0) {
    bcm_sta_get_phy_type();
#endif // MHD_CUST_STATISTICS
#ifdef MHD_CUST_COUNTERS
  } else if (strcmp((const char*)argv[0], "clear_cust_cnt") == 0) {
    bcm_sta_clear_cust_counters();
  } else if (strcmp((const char*)argv[0], "cust_counters") == 0) {
    bcm_sta_get_cust_counters();
#endif // MHD_CUST_COUNTERS


  } else if (strcmp((const char*)argv[0], "clear_cust_stats") == 0) {
    bcm_clear_cust_stats();
  } else if (strcmp((const char*)argv[0], "cust_stats") == 0) {
    bcm_get_cust_stats();

  } else {
    WIFI_INFO(("sta scan\n"));
    WIFI_INFO(("sta scan_ext <1,2,3>\n"));
    WIFI_INFO(("sta scanresults\n"));
    WIFI_INFO(("sta connect ssid security [password]\n"));
    WIFI_INFO(("sta connect_ext ssid security [password] channel bssid\n"));
    WIFI_INFO(("sta disconnect\n"));
    WIFI_INFO(("sta state\n"));
  #ifdef MHD_CUST_STATISTICS
    WIFI_INFO(("sta rssi\n"));
    WIFI_INFO(("sta rate\n"));
    WIFI_INFO(("sta phy_type\n"));
#endif // MHD_CUST_STATISTICS
#ifdef MHD_CUST_COUNTERS
    WIFI_INFO(("sta clear_cust_cnt\n"));
    WIFI_INFO(("sta cust_counters\n"));
#endif // MHD_CUST_COUNTERS
    WIFI_INFO(("sta connection\n"));
    WIFI_INFO(("sta reconnect\n"));
    WIFI_INFO(("sta mac\n"));
  }
}

#ifdef MHD_WLCMD
void bcm_wl_commands(uint32_t argc, char** argv) {
  int ret = 0;

  ret = mhd_wl_cmd(argc, (char**)argv);
  if (ret) {
    WIFI_ERROR(("Error Command Invalid: ret=%d\n", ret));
  }
}
#else
void bcm_wl_commands(uint32_t argc, char** argv) {
  WIFI_ERROR(("Error WLCMD Mode Disabled\n"));
}
#endif  // MHD_WLCMD


// GOOGLE: Added country code conversion function to go from string to
// power table country entry.
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) \
  ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))
#endif

typedef struct {
  const char* short_code;
  mhd_country_code_t mapping;
} country_code_entry_t;

static const country_code_entry_t country_mapping_table[] = {
    {"WW", MHD_COUNTRY_WORLD_WIDE},

    {"CA", MHD_COUNTRY_UNITED_STATES},
    {"US", MHD_COUNTRY_UNITED_STATES},

    {"EU", MHD_COUNTRY_EUROPE},
    {"UK", MHD_COUNTRY_EUROPE},
    {"GB", MHD_COUNTRY_EUROPE},
    {"FR", MHD_COUNTRY_EUROPE},
    {"DE", MHD_COUNTRY_EUROPE},
    {"ES", MHD_COUNTRY_EUROPE},
    {"IT", MHD_COUNTRY_EUROPE},
    {"NL", MHD_COUNTRY_EUROPE},
    {"DK", MHD_COUNTRY_EUROPE},
    {"NO", MHD_COUNTRY_EUROPE},
    {"FI", MHD_COUNTRY_EUROPE},
    {"SE", MHD_COUNTRY_EUROPE},
    {"BE", MHD_COUNTRY_EUROPE},
    {"IE", MHD_COUNTRY_EUROPE},

    {"AU", MHD_COUNTRY_AUSTRALIA},
};

void bcm_wifi_country_code_get_letters(mhd_country_code_t country_code, char country_letters[3]) {
  uint32_t country_code_val = (uint32_t)(country_code);
  country_letters[0] = (char)(country_code_val & 0x7F);
  country_letters[1] = (char)((country_code_val >> 8) & 0x7F);
  country_letters[2] = '\0';
}

mhd_country_code_t bcm_wifi_convert_country_code(const char* country_code) {
  if (country_code == NULL) {
    LOG_ERROR("Invalid country_code pointer, using WORLD_WIDE!");
    return MHD_COUNTRY_WORLD_WIDE;
  }

  const int kDefaultCodeRev = 999;
  const size_t kMaxCountryCodeLen = sizeof("US/999");
  char code_upper[kMaxCountryCodeLen + 1];
  memset(code_upper, 0, sizeof(code_upper));
  strncpy(code_upper, country_code, kMaxCountryCodeLen);
  size_t code_len = strnlen(code_upper, kMaxCountryCodeLen);

  size_t idx;
  for (idx = 0; idx < code_len; idx++) {
    code_upper[idx] = toupper(code_upper[idx]);
  }

  char country_short_code[3] = {'\0', '\0', '\0'};
  int code_rev = kDefaultCodeRev;
  bool failure = false;  // Only failure of 2-letter country code.

  // Country code format is XX/YYY or XX, where XX is an uppercase ISO country
  // code or other meta-identifier (e.g. EU, WW) and YYY is a "revision" number
  // in decimal.

  if (code_len < 2) {
    LOG_ERROR("WiFi: Invalid country code: '%s', using WORLD_WIDE!",
              code_upper);
    failure = true;
  } else if ((code_len == strlen("US/999")) && (code_upper[2] == '/')) {
    code_rev = (int)strtol(&code_upper[3], NULL, 10);
    if ((code_rev <= 0) || (code_rev > 999)) {
      LOG_ERROR(
          "WiFi: Invalid country code revision in '%s', using WORLD_WIDE!",
          code_upper);
      failure = true;
    }
  } else if (code_len == 2) {
    // Use default code revision for 2-letter codes.
    code_rev = kDefaultCodeRev;
  } else {
    LOG_ERROR("WiFi: Invalid country code: '%s', using WORLD_WIDE!",
              code_upper);
    failure = true;
  }

  // Start with a value of WORLD_WIDE as a sane error case.
  mhd_country_code_t country_code_val = MHD_COUNTRY_WORLD_WIDE;

  if (!failure) {
    country_short_code[0] = code_upper[0];
    country_short_code[1] = code_upper[1];
  } else {
    country_short_code[0] = 'W';
    country_short_code[1] = 'W';
    code_rev = kDefaultCodeRev;
  }

  for (idx = 0; idx < ARRAY_SIZE(country_mapping_table); idx++) {
    const char* candidate_code = country_mapping_table[idx].short_code;
    mhd_country_code_t candidate_country = country_mapping_table[idx].mapping;

    // 1- Match country code.
    if (strcmp(country_short_code, candidate_code) == 0) {
      // 2- Try to match code to existing country along with code revision by
      // forwarding the revision to the synthesized country code. This is to
      // ensure we can disambiguate virtual country codes (2 letters) with a
      // specific destination revision code.
      unsigned char country_a = candidate_country & 0xFF;
      unsigned char country_b = (candidate_country >> 8) & 0xFF;
      mhd_country_code_t synthesized_country =
          MHD_MK_CNTRY(country_a, country_b, code_rev);
      if (candidate_country == synthesized_country) {
        // Full match!
        country_code_val = candidate_country;
        break;
      }
    }
  }

  if (idx == ARRAY_SIZE(country_mapping_table)) {
    LOG_WARN("WiFi: Did not find mapping for country '%s', using WORLD_WIDE",
             country_short_code);
  }

  char country_letters[3] = {};
  bcm_wifi_country_code_get_letters(country_code_val, country_letters);

  LOG_INFO(
      "WiFi: Setting country code to %s/%03d", country_letters, code_rev);
  return country_code_val;
}

// Get full list of valid Wifi channel numbers for current power table.
// return >0: number of channels in list, 0: error
size_t bcm_wifi_get_channel_list(uint8_t* chan_out, size_t chan_out_size)
{
  uint32_t chan_buf[MHD_NUMCHANNELS+1]; // Don't modify
  uint32_t channel_count = 0;
  int i;

  memset(chan_buf, 0, sizeof(chan_buf));
  if (mhd_get_channels(chan_buf) != 0) {
    LOG_ERROR("WiFi: Failed to get channel list from FW!");
    return 0;
  }

  // First entry of value returned from mhd_get_channels() is number of valid
  // entries in reset of array.
  channel_count = (size_t)chan_buf[0];
  if (channel_count > chan_out_size) {
    return 0;
  }

  for (i = 1; i <= channel_count; i++) {
    *chan_out++ = (uint8_t)chan_buf[i];
  }

  return channel_count;
}

///////////////////////////////////////
static unsigned char bcm_cmd_argc = 1;
static unsigned char bcm_index_of_arg = 1;

#define KA_STA_SERVER_IP   "192.168.1.110"
#define KA_STA_SERVER_PORT 8080
#define KA_STA_INTERVAL    8

static char ka_payload[] = "alive"; //{0x88};
static int ka_payload_len = sizeof(ka_payload);

int mhd_tcpka_config( char *server_ip, uint16_t server_port, uint16_t interval, char *ka_payload, int ka_payload_len );
int mhd_tcpka_clear_all( void );

void bcm_tcpka_commands( int argc, char **argv )
{
    int ret;
    ret = 0;

	if (strcmp((const char*)argv[0], "add") == 0) {
	    if (argc == bcm_cmd_argc)
	    {
	        ret = mhd_tcpka_config(KA_STA_SERVER_IP, KA_STA_SERVER_PORT, KA_STA_INTERVAL, ka_payload, ka_payload_len);
	    }
	    else if (argc == (bcm_cmd_argc+1))
	    {
	        char *server_ip = argv[bcm_index_of_arg];
	        ret = mhd_tcpka_config(server_ip, KA_STA_SERVER_PORT, KA_STA_INTERVAL, ka_payload, ka_payload_len);
	    }
	    else if (argc == (bcm_cmd_argc+2))
	    {
	        char *server_ip = argv[bcm_index_of_arg];
	        int server_port = atoi(argv[bcm_index_of_arg+1]);
	        ret = mhd_tcpka_config(server_ip, server_port, KA_STA_INTERVAL, ka_payload, ka_payload_len);
	    }
	    else if (argc == (bcm_cmd_argc+3))
	    {
	        char *server_ip = argv[bcm_index_of_arg];
	        int server_port = atoi(argv[bcm_index_of_arg+1]);
	        uint16_t interval = atoi(argv[bcm_index_of_arg+2]);
	        ret = mhd_tcpka_config(server_ip, server_port, interval, ka_payload, ka_payload_len);
	    }

        if ( ret != 0)
    		LOG_ERROR("tcpka config failed, ret=%d\n", ret);
	}
	else if (strcmp((const char*)argv[0], "del") == 0) {
		mhd_tcpka_clear_all();
	}
}

int mhd_sta_wowl_init( const char *pattern_string, int offset );
int mhd_sta_wowl_status( void );
int mhd_sta_wowl_enable( void );
int mhd_sta_wowl_disable( void );

void bcm_wowl_commands( int argc, char **argv )
{
	if (strcmp((const char*)argv[0], "status") == 0) {
		mhd_sta_wowl_status();
	} else if (strcmp((const char*)argv[0], "enable") == 0) {
		mhd_sta_wowl_enable();
	} else if (strcmp((const char*)argv[0], "init") == 0) {
		static int wowl_added = 0;
		if (wowl_added == 0) // make sure only add one wowl entry
		{
			mhd_sta_wowl_init("0x983B16F8F39C983B16F8F39C", 54); // tcp:54, udp:42
			wowl_added = 1;
		}
	} else if (strcmp((const char*)argv[0], "disable") == 0) {
		mhd_sta_wowl_disable();
	}
}

#define sdio_cardinfo_file "/data/sdio_reg.bin"
void sdio_cardinfo_save(uint32_t card_id)
{
	FILE* fp;
	struct mmc_card_info *s_card_info;

	fp = fopen(sdio_cardinfo_file, "wb");
	if (fp == NULL) {
		printf("%s open failed\n", sdio_cardinfo_file);
		return;
	}

	s_card_info = mmc_card_save(card_id);
	if (!s_card_info)
		goto fail;
	fwrite(s_card_info, sizeof(struct mmc_card_info), 1, fp);

	free(s_card_info);
fail:
	fclose(fp);
}

void sdio_cardinfo_load(uint32_t card_id)
{
	FILE* fp;
	SDC_InitTypeDef sdc_param = { 0 };
	struct mmc_host *host;
	struct mmc_card_info s_card_info;
	uint32_t debug_mask = 0x3c;

	fp = fopen(sdio_cardinfo_file, "rb");
	if (fp == NULL){
		printf("%s open failed\n", sdio_cardinfo_file);
		return;
	}

	fread(&s_card_info, sizeof(struct mmc_card_info), 1, fp);
	fclose(fp);
	sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
	sdc_param.debug_mask = debug_mask;
	host = hal_sdc_create(card_id, &sdc_param);
	hal_sdc_init(host);
	mmc_card_restore(&s_card_info);
}

struct sdio_info {
	uint16_t card_id;
	SDC_InitTypeDef sdc_param;
	SDCard_InitTypeDef card_param;
	struct mmc_card *card;
};

static int sdio_card_detect(struct sdio_info *sdio)
{
    int ret = mmc_card_create(sdio->card_id, &sdio->card_param);
	if (ret != 0) {
		printf( "SDIO failed to init. ret=%d\n", ret );
		return ret;
	}

	sdio->card = mmc_card_open(sdio->card_id);
	if (sdio->card == NULL) {
		printf("card open fail\n");
		return ret;
	}
	/* scan card for detect card is exist? */
	if (!mmc_card_present(sdio->card)) {
		if (mmc_rescan(sdio->card, sdio->card->id)) {
			printf("Initial card failed!!\n");
			mmc_card_close(sdio->card_id);
			return ret;
		} else {
			printf("Initial card success\n");
			mmc_card_close(sdio->card_id);
		}
	} else {
		printf("%s not eixst\n", __func__);
		mmc_card_close(sdio->card_id);
		return ret;
	}

	return ret;
}

static struct sdio_info sdio_wifi;
static void sdio_controller_init( uint32_t index )
{
	SDC_InitTypeDef sdc_param;
	struct mmc_host *host;

	memset(&sdio_wifi, 0, sizeof(struct sdio_info));
	sdio_wifi.sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
	//sdio_wifi.sdc_param.cd_cb = &sdio_card_detect;
	sdio_wifi.sdc_param.debug_mask = (ROM_INF_MASK | ROM_WRN_MASK | ROM_ERR_MASK | ROM_ANY_MASK);
	sdio_wifi.sdc_param.dma_use = 1;

	sdio_wifi.card_id = index;
	sdio_wifi.card_param.debug_mask = sdio_wifi.sdc_param.debug_mask;
	sdio_wifi.card_param.type = MMC_TYPE_SDIO;

	host = hal_sdc_create(index, &sdio_wifi.sdc_param);
	host->param.pwr_mode = POWER_MODE_180;
	hal_sdc_init(host);

	if (sdio_wifi.sdc_param.cd_mode == CARD_ALWAYS_PRESENT) {
		sdio_card_detect(&sdio_wifi);
	}
}

#define wifi_info_file "/data/wifi_data.bin"

void bcm_driver_resume_save(void)
{
	FILE* fp;
	int size;
	char* data;

	fp = fopen(wifi_info_file, "wb");
	if (fp == NULL) {
		printf("%s open failed\n", wifi_info_file);
		return;
	}

	data = mhd_wifi_resume_backup(&size);
	printf("%s: size %d\r\n", __func__, size);
	fwrite(&size, sizeof(size), 1, fp);
	fwrite(data, size, 1, fp);
	fclose(fp);
}

void bcm_driver_resume_load(void)
{
	FILE* fp;
	int size;
	char *data;
	struct wifi_network_data *wifi_data;

	fp = fopen(wifi_info_file, "rb");
	if (fp == NULL){
		printf("%s open failed\n", wifi_info_file);
		return;
	}

	fread(&size, sizeof(size), 1, fp);
	printf("%s: size %d\r\n", __func__, size);

	data = malloc(size);
	if (data) {
		fread(data, size, 1, fp);
		mhd_wifi_resume_restore(data, size);
		free(data);
	}
	fclose(fp);
}

void bcm_sdio_commands( int argc, char **argv )
{
	if (strcmp((const char*)argv[0], "save") == 0) {
		sdio_cardinfo_save(1);
		bcm_driver_resume_save();
	} else if (strcmp((const char*)argv[0], "load") == 0) {
		sdio_cardinfo_load(1);
		bcm_driver_resume_load();
	} else if (strcmp((const char*)argv[0], "resume") == 0) {
		mhd_use_wifi_resume(1);
	} else if (strcmp((const char*)argv[0], "init") == 0) {
		sdio_controller_init(1);
	} else if (strcmp((const char*)argv[0], "go") == 0) {
		char *argv[4] = {"connect", "Cloud4G", "2", "12345678" };
		sdio_controller_init(1);
		tcpip_init(NULL, NULL);
		bcm_wifi_start();
		bcm_sta_connect(4, argv);
		//do keep alive and enable wowl, then backup data
		sdio_cardinfo_save(1);
		bcm_driver_resume_save();
		//you can power off SoC
	} else if (strcmp((const char*)argv[0], "reinit") == 0) {
		int ret;
		tcpip_init(NULL, NULL);
		sdio_cardinfo_load(1);
		//You need to know, this is a first power on or not.
		bcm_driver_resume_load();

		mhd_use_wifi_resume(1); //enable resume mode
		bcm_wifi_start();
		mhd_sta_wowl_status(); //check wake state

#ifdef MHD_JOIN_STATE
		ret = mhd_sta_register_join_state_callback(bcm_join_state);
		if (ret != 0) {
		WIFI_ERROR(("Error register join state callback\n"));
		}
#endif  // MHD_JOIN_STATE
	}
}
//////////////////////////////////////
