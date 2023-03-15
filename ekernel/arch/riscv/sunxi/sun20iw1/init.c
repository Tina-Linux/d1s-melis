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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <log.h>
#include <init.h>
#include <sysinit.h>
#include <timex.h>
#include <port.h>

#ifdef CONFIG_DRIVERS_GPIO
#include <sunxi_drv_gpio.h>
#endif
#include <hal_cfg.h>
#include <hal_time.h>
#include <mod_mixture.h>

#include <io.h>

#define  STACK_CHECK_MAGICNUMBER    (0x5a5a5a5adeadbeef)
uint8_t melis_kernel_running = 0;
uint8_t *melis_ramdisk_base = 0;
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
extern void         timekeeping_init(void);
extern void         trap_init(void);
extern int32_t      ksrv_init(void);
extern int32_t      input_init(void);
extern uint32_t     fsys_init(void);
extern int32_t      dmsdev_init(void);
extern int32_t      ramdiskdev_init(void);
extern int          dfs_init(void);
#ifdef CONFIG_COMMAND_PQD
extern int          init_pq(void);
#endif

#ifdef CONFIG_CXX
int cplusplus_system_init(void);
#endif

static noinline void copy_ramdisk(void)
{
    return;
}

#ifdef CONFIG_DRIVER_OSAL_CFG
static noinline void copy_config(void)
{
	u32  data_len = 0;

	data_len = *(u32 *)(0x43000000);
    Hal_Cfg_Init((void *)(0x43000000 + 512), 64 * 1024);
    return ;
}
#endif

static int32_t awos_bsp_init(void)
{
    system_tick_init();
#ifdef CONFIG_RV_PLIC
    plic_init();
#endif
    interrupt_init();
    copy_ramdisk();
#ifdef CONFIG_DRIVER_OSAL_CFG
	copy_config();
#endif

    /*gpio & ccmu need to be initialized before other driver as the basic module*/
#ifdef CONFIG_DRIVER_CCMU
    drv_clock_init();
#endif

#ifdef CONFIG_DRIVER_GPIO
    drv_gpio_init();
#endif

#ifdef CONFIG_DRIVER_SERIAL
	char uartname[6];
	void sunxi_driver_uart_init(void);
	sunxi_driver_uart_init();
	sprintf(uartname, "uart%d", CONFIG_CLI_UART_PORT);
	rt_console_set_device(uartname);
#endif

#ifdef CONFIG_SUBSYS_MULTI_CONSOLE
    multiple_console_init();
#endif

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

#ifdef CONFIG_ROOTFS_MINFS
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
        hal_msleep(1);
    } while((NULL == pdir) && (++cnt < 500));

    if(cnt >= 500)
    {
        printf("d: partition open failed, please check board storage device!");
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

#ifdef CONFIG_CXX
    cplusplus_system_init();
#endif

    do_initcalls();             /*init sdmmc/flash and all other driver device*/

    app_init();

#ifdef CONFIG_ROOTFS_MINFS
    esMODS_MInstall("d:\\mod\\slib.mod", 0);
#endif

	int mid_disp = esMODS_MInstall("d:\\mod\\display.mod",0);
    esMODS_MOpen(mid_disp, 0);

#ifdef CONFIG_COMMAND_PQD
    init_pq();
#endif

#ifdef CONFIG_MOD_MIXTURE_ENABLE

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

#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
    svc_init();

    input_init();
#endif

    finsh_system_init();

#ifdef CONFIG_RT_USING_DFS_RAMFS_SIZE
    int dfs_ramfs_create_mount(int size);
    dfs_ramfs_create_mount(CONFIG_RT_USING_DFS_RAMFS_SIZE);
#endif

#ifdef CONFIG_LEGACY_DMS_DEV
    extern int32_t sunxi_driver_uart_dms_init(void);
    sunxi_driver_uart_dms_init();
#endif
    app_entry(NULL);
}

asm(".option norvc");
void start_kernel(unsigned long spl_nanosec, unsigned long misa, unsigned long sbi_start, unsigned long sbi_end)
{
#if defined CONFIG_CC_STACKPROTECTOR_STRONG
    __stack_chk_guard = STACK_CHECK_MAGICNUMBER;
#endif
    trap_init();

    timekeeping_init();

    awos_init_bootstrap(awos_bsp_init, awos_init_thread, spl_nanosec);

    /* never come back */
    CODE_UNREACHABLE;
}
