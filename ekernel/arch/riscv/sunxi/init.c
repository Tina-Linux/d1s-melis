/*
 * ===========================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  bringup file.
 *
 *        Version:  Melis3.0
 *         Create:  2020-08-17 12:07:20
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-09-28 11:10:46
 *
 * ===========================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>
#include <log.h>
#include <init.h>
#include <sysinit.h>
#include <string.h>
#include <timex.h>
#include <debug.h>
#include <port.h>
#include <cmpxchg.h>
#include <sunxi_drv_gpio.h>
#include <hal_cfg.h>
#include <mod_mixture.h>

#define  STACK_CHECK_MAGICNUMBER    (0x5a5a5a5adeadbeef)
/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint8_t melis_kernel_running = 0;
/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint8_t *melis_ramdisk_base = 0;
/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint32_t melis_ramdisk_size = 0;

unsigned long __stack_chk_guard;

#if defined CONFIG_BOOT_LOGO_BMP
uint32_t melis_logo_bmp_size = 0;
uint8_t *melis_logo_bmp_base;
#endif

extern int          drv_clock_init(void);
extern int          multiple_console_init(void);
extern void         do_initcalls(void);
extern void         plic_init(void);
extern void         system_tick_init(void);
extern int          interrupt_init(void);
extern int          finsh_system_init(void);
extern void         decode_misa(unsigned long);
extern void         timekeeping_init(void);
extern void         trap_init(void);
extern int32_t      ksrv_init(void);
extern int32_t      input_init(void);
extern uint32_t     fsys_init(void);
extern int32_t      dmsdev_init(void);
extern int32_t      ramdiskdev_init(void);
extern int          dfs_init(void);

#ifdef CONFIG_CXX
int cplusplus_system_init(void);
#endif

static noinline void copy_config(void)
{
	u32  data_len = 0;

	data_len = *(u32 *)(0x43000000);
    Hal_Cfg_Init((void *)(0x43000000 + 512), data_len);
    return ;
}

static int32_t awos_bsp_init(void)
{
    system_tick_init();
    plic_init();
    interrupt_init();
	copy_config();
    return 0;
}

static void app_init(void)
{
#ifdef CONFIG_RT_USING_DFS
    int dfs_mount(
        const char *device_name,
        const char *path,
        const char *filesystemtype,
        unsigned long rwflag,
        const void *data);

#ifdef CONFIG_MELIS_ROOTFS
    dfs_mount(NULL, "/", "rootfs", 0, 0);
#endif
#ifdef CONFIG_RT_USING_DFS_LITTLEFS
    dfs_mount("ROOTFS", "/data", "lfs", 0, 0);
#endif
#endif

#if CONFIG_ROOTFS_MINFS
    int cnt = 0;
    void *pdir = NULL;
    do
    {
        pdir = esFSYS_opendir("d:");
        if(pdir)
        {
            esFSYS_closedir(pdir);
            pdir = NULL;
            break;
        }
        rt_thread_delay(1);
    }while((NULL == pdir) && (++cnt < 500));

    if(cnt >= 500)
    {
        rt_kprintf("d: partition open failed, please check board storage device!");
        while(1);
    }
#endif

    return;
}

asm(".option norvc");
static void awos_init_thread(void *para)
{
#ifdef CONFIG_PTHREAD
    int pthread_system_init(void);
    pthread_system_init();
#endif
    /*gpio & ccmu need to be initialized before other driver as the basic module*/
    drv_clock_init();
    drv_gpio_init();

#ifdef CONFIG_LEGACY_DMS_DEV
    dev_init();                 /*init device manage, create pDevSem semphore*/
#endif

#ifdef CONFIG_MELIS_VIRTUAL_FILESYSTEM
    fsys_init();                /*init filesystem manage and partition manage*/

    ksrv_init();                /*init kernel service message queue*/

    dfs_init();                 /*init dfs filesystem*/
#endif

#ifdef CONFIG_LEGACY_DMS_DEV
    dmsdev_init();              /*register DMS partition(alias B:\\ and /mnt/B) for device's esDEV_DEVReg */
#endif

#ifdef CONFIG_LEGACY_RAMDISK_DEV
    ramdiskdev_init();          /*register RAMDISK partition(alias C:\\ and /mnt/C)*/
#endif

    multiple_console_init();    /*init multi-console, from now on, debug uart api switch to multiple_console from sbi_uart */

#ifdef CONFIG_CXX
    cplusplus_system_init();
#endif

    do_initcalls();             /*init sdmmc/flash and all other driver device*/

    app_init();

#if CONFIG_ROOTFS_MINFS
    esMODS_MInstall("d:\\mod\\slib.mod", 0);
#endif

 int mid_disp = esMODS_MInstall("d:\\mod\\display.mod",0);
    esMODS_MOpen(mid_disp, 0);
    
#if CONFIG_MOD_MIXTURE_ENABLE   
   int mid = 0;
   void  *mp = 0;
   __u64 arg[3];
   mid = esMODS_MInstall("d:\\mod\\mixture.mod",0);
   mp = esMODS_MOpen(mid, 0);
   esKSRV_Save_Mixture_Hld(mid,mp);
   arg[0] = STARTUP;
   esMODS_MIoctrl((__mp *)mp, MOD_MIXTURE_START, 0, (void *)arg);

#endif

#ifdef CONFIG_MOD_CHARSET_ENABLE     
    eschs_init(0,0);
#endif

    svc_init();

    input_init();

    finsh_system_init();

#ifdef CONFIG_RT_USING_DFS_RAMFS_SIZE
    int dfs_ramfs_create_mount(int size);
    dfs_ramfs_create_mount(CONFIG_RT_USING_DFS_RAMFS_SIZE);
#endif

#ifdef CONFIG_LEGACY_DMS_DEV
    extern int32_t sunxi_driver_uart_dms_init(void);
    sunxi_driver_uart_dms_init();
#endif
}

asm(".option norvc");
void start_kernel(unsigned long spl_nanosec, unsigned long misa, unsigned long sbi_start, unsigned long sbi_end)
{
#if defined CONFIG_CC_STACKPROTECTOR_STRONG
    __stack_chk_guard = STACK_CHECK_MAGICNUMBER;
#endif
    awos_arch_lock_irq();
    trap_init();

    timekeeping_init();
    awos_init_bootstrap(awos_bsp_init, awos_init_thread, spl_nanosec);

    /* never come back */
    CODE_UNREACHABLE;
}
