#include <rtthread.h>

#ifdef RT_USING_FINSH
#include <finsh.h>

#include "lwip/tcpip.h"

extern void bcm_station_commands(rt_uint32_t argc, char** argv);
static int bcm_cmd_station_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_station_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_station_commands, bcm_sta, Broadcom WiFi STA commands);

extern void bcm_softap_commands(rt_uint32_t argc, char** argv);
static int bcm_cmd_softap_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_softap_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_softap_commands, bcm_ap, Broadcom WiFi AP commands);

#ifdef MHD_WLCMD
extern void bcm_wl_commands(rt_uint32_t argc, char** argv);
static int bcm_cmd_wl_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_wl_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_wl_commands, bcm_wl, Broadcom WiFi WL commands);
#endif

#if 1
extern void bcm_wifi_commands(int argc, char** argv);
static int bcm_cmd_wifi_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_wifi_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_wifi_commands, bcm_wifi, Broadcom WiFi DBG commands);
#endif

#if 1
extern void bcm_test_commands(uint32_t argc, uint8_t** argv);
static int bcm_cmd_wifi_tst(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_test_commands(argc, (uint8_t**)argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_wifi_tst, bcm_test, Broadcom WiFi TST commands);
#endif

//int host_sdio_controller_init( uint32_t index );
extern int mhd_config_polling( uint8_t enable );
extern int mhd_config_polling_interval( uint32_t interval_ms );
extern void bcm_wifi_start(void);
static int bcm_cmd_wifi_init(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }

    tcpip_init(NULL, NULL);

#ifdef USE_POLLING_MODE
	mhd_config_polling(1);
	mhd_config_polling_interval(50);
#endif
    bcm_wifi_start();
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_wifi_init, bcm_init, Broadcom WiFi Init commands);

#ifdef MHD_FWLOG
extern int mhd_bus_keep_awake(int awake);
extern int mhd_config_fwlog( uint8_t enable, uint32_t interval );
extern int mhd_read_fwlog(void);
static int bcm_cmd_fw_log(rt_uint32_t argc, char** argv)
{
    if (argc == 1)
    {
        mhd_read_fwlog();
    }
    else if (argc == 2)
    {
        rt_kprintf("argv=%s\n", argv[1]);
        mhd_bus_keep_awake(atoi(argv[1]));
    }
    else if (argc == 3)
    {
        rt_kprintf("argv[1]=%s argv[2]=%s\n", argv[1], argv[2]);
        mhd_config_fwlog(atoi(argv[1]), atoi(argv[2]));
    }
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_fw_log, bcm_fwlog, Broadcom WiFi Fwlog commands);
#endif


extern void bcm_tcpka_commands(rt_uint32_t argc, char** argv);
static int bcm_cmd_tcpka_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_tcpka_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_tcpka_commands, bcm_tcpka, Broadcom WiFi Tcpka commands);



extern void bcm_wowl_commands(rt_uint32_t argc, char** argv);
static int bcm_cmd_wowl_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_wowl_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_wowl_commands, bcm_wowl, Broadcom WiFi Wowl commands);





extern void bcm_sdio_commands(rt_uint32_t argc, char** argv);
static int bcm_cmd_sdio_commands(rt_uint32_t argc, char** argv)
{
    rt_kprintf("argc=%d\n", argc);
    if (argc > 1)
    {
        ++argv;
        --argc;
    }
    bcm_sdio_commands(argc, argv);
    return 0;
}
MSH_CMD_EXPORT_ALIAS(bcm_cmd_sdio_commands, bcm_sdio, SDIO for Broadcom control suspend/resume commands);





#endif /* RT_USING_FINSH */
