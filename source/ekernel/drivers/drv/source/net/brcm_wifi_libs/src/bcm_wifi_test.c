#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bcm_wifi.h"
#include "bcm_wifi_test.h"
#include "my_print.h"

///////////////////////////////////

#ifdef MHD_PING_TEST
static void bcm_test_ping(uint32_t argc, uint8_t** argv) {
  const char* ip_addr;

  if (argc == 1) {
    ip_addr = "192.168.1.1";
    // ip_addr = "192.168.43.1";
    mhd_ping(ip_addr);
  } else if (argc == 2) {
    WIFI_INFO(("IP: %s\n", argv[1]));
    ip_addr = (const char*)argv[1];
    mhd_ping(ip_addr);
  }
}
#endif  // MHD_PING_TEST

extern int mhd_sta_network_down(void);
extern int mhd_sta_network_up(uint32_t ip, uint32_t gateway, uint32_t netmask);
void bcm_sta_change_ip(void) {
  mhd_sta_network_down();
  mhd_sta_network_up(0xc0a80162, 0xc0a80101, 0xffffff00);
}

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
static void bcm_versions(uint32_t argc, uint8_t** argv) {
  char version[250];
  uint32_t chip_id;
  uint32_t chip_rev;

  mhd_get_chip_info(&chip_id, &chip_rev);
  WIFI_INFO(("Chip: %x/%x\n", chip_id, chip_rev));

  WIFI_INFO(("MHD: %s\n", mhd_get_driver_version()));

  mhd_get_firmware_version(version, sizeof(version));
  WIFI_INFO(("%s\n", version));
}

#ifdef MHD_BUS_AWAKE
extern void host_rtos_delay_milliseconds(uint32_t num_ms);
extern int mhd_get_mpc(void);
// return 0: successful, others: failed
static void cmd_mpc_test(uint32_t argc, uint8_t** argv) {
  uint8_t state = 0;

  host_time_t start_time, stop_time;
  int count = 5;

  do {
    // Add timestamp to calculate ioctl time including cmd response.
    start_time = host_rtos_get_time();
    state = mhd_get_mpc();
    stop_time = host_rtos_get_time();
    WIFI_INFO(("GET MPC time diff: %d\n", stop_time - start_time));

    // Instert delay for each ioctl cmd test
    host_rtos_delay_milliseconds(50);
  } while (count-- > 0);
}
#endif  // MHD_BUS_AWAKE

////////////////////////////////////
#ifdef MHD_GGL_TCP_TEST
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/priv/tcp_priv.h"

static void tcp_error_callback(void* arg, err_t err) {
  WIFI_INFO(("TCP error %d\n", err));
}

#define PRINT_LEN 80
static err_t tcp_receive_callback(void* arg,
                                  struct tcp_pcb* pcb,
                                  struct pbuf* p,
                                  err_t err) {
  char buffer[PRINT_LEN + 1] = {0};
  int b;
  WIFI_INFO(("Data received, err = %d\n", err));
  for (int i = 0; (b = pbuf_copy_partial(p, buffer, PRINT_LEN, i)); i += b) {
    buffer[b + 1] = '\0';
    WIFI_INFO(("%s\n", buffer));
  }
  return ERR_OK;
}

static err_t tcp_send_callback(void* arg, struct tcp_pcb* pcb, u16_t len) {
  WIFI_INFO(("Data sent\n"));
  return ERR_OK;
}

static err_t tcp_connect_callback(void* arg, struct tcp_pcb* pcb, err_t err) {
  WIFI_INFO(("Connected\n"));
  return ERR_OK;
}

static struct tcp_pcb* pcb;

static void tcp_test(void) {
  struct netif* netif = netif_find("wi0");
  ip_addr_t ip;
  int port = 80;
  char write_buf[256] = "GET /index.html HTTP/1.1\nConnection: Keep-Alive\n\n";

  // TODO: Fill in the ip address of a webserver.
#if LWIP_IPV4 && LWIP_IPV6
  IP4_ADDR(&ip.u_addr.ip4, 192, 168, 1, 1);
#elif LWIP_IPV4
  IP4_ADDR(&ip, 192, 168, 1, 1);
#elif LWIP_IPV6
  IP6_ADDR(&ip, 192, 168, 1, 1);
#endif

  pcb = tcp_new();
  WIFI_INFO(("pcb created\n"));
  err_t error = tcp_bind(pcb, &netif->ip_addr, 0);
  if (error != ERR_OK) {
    WIFI_INFO(("tcp_bind error: %d\n", error));
    return;
  }
  tcp_err(pcb, tcp_error_callback);
  tcp_recv(pcb, tcp_receive_callback);
  tcp_sent(pcb, tcp_send_callback);
  error = tcp_connect(pcb, &ip, port, tcp_connect_callback);
  if (error != ERR_OK) {
    WIFI_INFO(("tcp_connect error: %d\n", error));
    return;
  }

  error = tcp_write(pcb, write_buf, strlen(write_buf), 0x00);
  if (error != ERR_OK) {
    WIFI_INFO(("tcp_write error: %d\n", error));
  }
}

static void tcp_cleanup(void) {
  tcp_close(pcb);
  pcb = NULL;
}
#endif  // MHD_GGL_TCP_TEST

////////////////////////////////////
#ifdef MHD_LWIPERF_TEST
//#if LWIP_IPV4 && LWIP_TCP
#include "lwip/apps/lwiperf.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"

/* Report state => string */
const char* report_type_str[] = {
    "TCP_DONE_SERVER",             /* LWIPERF_TCP_DONE_SERVER,*/
    "TCP_DONE_CLIENT",             /* LWIPERF_TCP_DONE_CLIENT,*/
    "TCP_ABORTED_LOCAL",           /* LWIPERF_TCP_ABORTED_LOCAL, */
    "TCP_ABORTED_LOCAL_DATAERROR", /* LWIPERF_TCP_ABORTED_LOCAL_DATAERROR, */
    "TCP_ABORTED_LOCAL_TXERROR",   /* LWIPERF_TCP_ABORTED_LOCAL_TXERROR, */
    "TCP_ABORTED_REMOTE",          /* LWIPERF_TCP_ABORTED_REMOTE, */
    "UDP_STARTED",                 /* LWIPERF_UDP_STARTED, */
    "UDP_DONE",                    /* LWIPERF_UDP_DONE, */
    "UDP_ABORTED_LOCAL",           /* LWIPERF_UDP_ABORTED_LOCAL, */
    "UDP_ABORTED_REMOTE"           /* LWIPERF_UDP_ABORTED_REMOTE */
};

/** Prototype of a report function that is called when a session is finished.
    This report function shows the test results. */
static void lwiperf_report(void* arg,
                           enum lwiperf_report_type report_type,
                           const ip_addr_t* local_addr,
                           u16_t local_port,
                           const ip_addr_t* remote_addr,
                           u16_t remote_port,
                           u32_t bytes_transferred,
                           u32_t ms_duration,
                           u32_t bandwidth_kbitpsec) {
  WIFI_INFO(("-------------------------------------------------\r\n"));
  if ((report_type < (sizeof(report_type_str) / sizeof(report_type_str[0]))) &&
      local_addr && remote_addr) {
    WIFI_INFO((" %s \r\n", report_type_str[report_type]));
    WIFI_INFO((" Local address : %u.%u.%u.%u ",
               ((u8_t*)local_addr)[0],
               ((u8_t*)local_addr)[1],
               ((u8_t*)local_addr)[2],
               ((u8_t*)local_addr)[3]));
    WIFI_INFO((" Port %d \r\n", local_port));
    WIFI_INFO((" Remote address : %u.%u.%u.%u ",
               ((u8_t*)remote_addr)[0],
               ((u8_t*)remote_addr)[1],
               ((u8_t*)remote_addr)[2],
               ((u8_t*)remote_addr)[3]));
    WIFI_INFO((" Port %d \r\n", remote_port));
    WIFI_INFO((" Bytes Transferred %d \r\n", bytes_transferred));
    WIFI_INFO((" Duration (ms) %d \r\n", ms_duration));
    WIFI_INFO((" Bandwidth (kbitpsec) %d \r\n", bandwidth_kbitpsec));
  } else {
    WIFI_INFO((" IPERF Report error\r\n"));
  }
}

static void* lwiperf_session;
static void bcm_lwiperf_start(void) {
  lwiperf_session = lwiperf_start_tcp_server_default(lwiperf_report, 0);
  if (lwiperf_session != NULL) {
    WIFI_INFO((" IPERF TCP Server start\r\n"));
  } else {
    WIFI_INFO(("IPERF TCP Server initialization failed!\r\n"));
  }
}

static void bcm_lwiperf_stop(void) { lwiperf_abort(lwiperf_session); }

static void bcm_iperf_tx(const char* ip_addr, uint32_t count, uint16_t port) {
  lwiperf_session = lwiperf_start_tcp_client_default(
      lwiperf_report, (char*)ip_addr, count, port, NULL);
  if (lwiperf_session != NULL) {
    WIFI_INFO(("IPERF Session started\n"));
  } else {
    WIFI_INFO(("IPERF Session not started\n"));
  }
}
//#endif // LWIP_IPV4 && LWIP_TCP
#endif  // MHD_LWIPERF_TEST

////////////////////////////////////

void bcm_test_commands(uint32_t argc, uint8_t** argv) {

  if (0) {
#ifdef MHD_PING_TEST
  } else if (strcmp((const char*)argv[0], "ping") == 0) {
    WIFI_INFO(("ping\n"));
    bcm_test_ping(argc, argv);
#endif

#ifdef MHD_GGL_TCP_TEST
  } else if (strcmp((const char*)argv[0], "tcp_test") == 0) {
    tcp_test();
  } else if (strcmp((const char*)argv[0], "tcp_cleanup") == 0) {
    tcp_cleanup();
#endif

  } else if (strcmp((const char*)argv[0], "change_ip") == 0) {
    bcm_sta_change_ip();

  } else if (strcmp((const char*)argv[0], "versions") == 0) {
    bcm_versions(argc, argv);

#ifdef MHD_BUS_AWAKE
  } else if (strcmp((const char*)argv[0], "bus_awake") == 0) {
    mhd_bus_keep_awake(atoi((const char*)argv[1]));

  } else if (strcmp((const char*)argv[0], "mpc_test") == 0) {
    cmd_mpc_test(argc, argv);
#endif  // MHD_BUS_AWAKE

#ifdef MHD_LWIPERF_TEST
  } else if (strcmp((const char*)argv[0], "lwiperf_start") == 0) {
    bcm_lwiperf_start();
  } else if (strcmp((const char*)argv[0], "lwiperf_stop") == 0) {
    bcm_lwiperf_stop();
  } else if (strcmp((const char*)argv[0], "lwiperf_tx") == 0) {
    const char* ip_addr;
    uint32_t count = 10;
    uint16_t port = 5001;
    if (argc < 2) {
      WIFI_INFO(("Input \"iperf tx ip_address time_s\"\n"));
      return;
    } else if (argc == 2) {
      count = 10;
    } else if (argc == 3) {
      count = (uint32_t)atoi((const char*)argv[2]);
    } else if (argc == 4) {
      port = (uint16_t)atoi((const char*)argv[3]);
    }
    WIFI_INFO(("count %d\n", count));
    ip_addr = (const char*)argv[1];
    bcm_iperf_tx(ip_addr, count, port);
#endif

  } else {
#ifdef MHD_PING_TEST
    WIFI_INFO(("test ping [ip_addr]\n"));
#endif
#ifdef MHD_GGL_TCP_TEST
    WIFI_INFO(("test tcp_test\n"));
    WIFI_INFO(("test tcp_cleanup\n"));
#endif
    WIFI_INFO(("test change_ip\n"));
    WIFI_INFO(("test versions\n"));
#ifdef MHD_LWIPERF_TEST
    WIFI_INFO(("test lwiperf_start\n"));
    WIFI_INFO(("test lwiperf_stop\n"));
    WIFI_INFO(("test lwiperf_tx ip_addr time_s\n"));
#endif
  }
}
