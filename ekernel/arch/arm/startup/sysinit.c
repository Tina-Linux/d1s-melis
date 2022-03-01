/*
 * ===========================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  bringup platform routine.
 *
 *        Version:  Melis3.0
 *         Create:  2018-03-09 10:40:15
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-08 19:29:43
 *
 * ===========================================================================================
 */

#include "sysinit.h"
#include <string.h>
#include <debug.h>
#include <port.h>
#include <stdio.h>
#include <log.h>
#include <fcntl.h>
#include <libc.h>
#include <util.h>
#include <ktimer.h>

#define     STACK_CHECK_MAGICNUMBER    (0x5a5a5a5a)
/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint32_t    melis_arch_use_fpu = 0;

/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint8_t     melis_kernel_running = 0;
/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint8_t     *melis_ramdisk_base;
/* ----------------------------------------------------------------------------*/
/** @brief  <++> */
/* ----------------------------------------------------------------------------*/
uint32_t    melis_ramdisk_size;

#if defined CONFIG_BOOT_LOGO_BMP
uint32_t    melis_logo_bmp_size = 0;
uint8_t     *melis_logo_bmp_base;
#endif

void    hal_clock_init(void);
int     hal_gpio_init(void);
int32_t CNTR_Init(void);
void    sunxi_driver_uart_init(void);
void    set_console_device(void);
void    drv_dma_init(void);
int     multiple_console_init(void);
void    do_initcalls(void);

#ifdef CONFIG_RT_USING_FINSH
int     finsh_system_init(void);
int     list_thread(int argc, const char **argv);
#endif

#ifdef CONFIG_PTHREAD
int     pthread_system_init(void);
#endif


#ifdef CONFIG_RT_USING_DFS
int     mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data);
#endif

#ifdef CONFIG_RT_USING_DFS_RAMFS_SIZE
int     dfs_ramfs_create_mount(int size);
#endif

#ifdef CONFIG_CXX
int     cplusplus_system_init(void);
#endif

static noinline void copy_ramdisk(void)
{
    extern unsigned char blob_input_ramdisk_start[];
    extern unsigned char blob_input_ramdisk_end[];

    melis_ramdisk_size = (uint32_t)blob_input_ramdisk_end - (uint32_t)blob_input_ramdisk_start;
    melis_ramdisk_base = blob_input_ramdisk_start;

    __log("ramdisk: base %p, size %d.", melis_ramdisk_base, melis_ramdisk_size);
    return;
}

static noinline void clock_init(void)
{
    hal_clock_init();
}

static noinline void time_init(void)
{
    //initialise timer and conter manager
    //TMR_Init();
    CNTR_Init();
}

noinline void serial_init(void)
{
    sunxi_driver_uart_init();
    set_console_device();
}
static noinline void board_init(void)
{
    //TODO: Add board specific init.
    drv_dma_init();
}

#if defined (CONFIG_ZEPHYRKERNEL)
void zephyr_bsp_init(void)
{
    board_init();
}
#elif defined (CONFIG_RTTKERNEL)
static int32_t awos_bsp_init(void)
{
    hal_gpio_init();
    clock_init();
    time_init();
    serial_init();
    board_init();
    return 0;
}
#else
#error "os not supported!"
#endif

static void sys_insmod(void)
{
    esMODS_MInstall("c:\\mod\\slib.mod", 0);
    //esDEV_Plugin("\\drv\\spi.drv", 0, 0, 1);
    //esDEV_Plugin("\\drv\\spinor.drv", 0, 0, 1);
}

noinline static void app_init(void)
{
    //timestamp("APPINITBEGIN");
    //static char *argv[1];
    //argv[0] = "minimalapp";
    //int main_sample_virvi2vo(int argc, char *argv[]);
    //main_sample_virvi2vo(1, argv);

#ifdef CONFIG_RT_USING_FINSH
    finsh_system_init();
#endif

    //timestamp("APPINITEND");
}

__s32 ksrv_init(void);
static void awos_init_thread(void *para)
{
#ifdef CONFIG_PTHREAD
    pthread_system_init();
#endif

    multiple_console_init();

    timestamp("INITCALLSTA");
    do_initcalls();
    timestamp("INITCALLEND");

#ifdef CONFIG_RT_USING_DFS

#ifdef CONFIG_MELIS_ROOTFS
    mount(NULL, "/", "rootfs", 0, 0);
#endif

#ifdef CONFIG_RT_USING_DFS_LITTLEFS
    mount("ROOTFS", "/data", "lfs", 0, 0);
#endif

#ifdef CONFIG_RT_USING_DFS_RAMFS_SIZE
    dfs_ramfs_create_mount(CONFIG_RT_USING_DFS_RAMFS_SIZE);
#endif

#endif

#ifdef CONFIG_MELIS_VIRTUAL_FILESYSTEM
    fsys_init();
#endif
#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
    dev_init();
#endif

    ksrv_init();
#ifdef CONFIG_MELIS_MODULE
    sys_insmod();
#endif

    libc_stdio_set_console("uart0", O_RDWR);

#ifdef CONFIG_CXX
    cplusplus_system_init();
#endif

    app_init();
    __log("System bringup finished! ByeBye!");
}

#ifdef CONFIG_CC_STACKPROTECTOR_STRONG
void __stack_chk_fail(void)
{
    __err("fata error: stack corrupted!");
#ifdef CONFIG_RT_USING_FINSH
    list_thread(1, NULL);
#endif
    software_break();
}
/*
 * Symbol referenced by GCC compiler generated code for canary value.
 * The canary value gets initialized in start_kernel().
 */
unsigned long __stack_chk_guard;
#endif

void timekeeping_init(void);
void start_kernel(uint64_t ticks)
{
    awos_arch_fpu_init();
#if 0
    int usleep(unsigned int usecs);
    for(volatile int i = 3000; i > 0; i --) 
    {
        usleep(1000);
    }
#endif
#if defined CONFIG_CC_STACKPROTECTOR_STRONG
    {
        __stack_chk_guard = STACK_CHECK_MAGICNUMBER;
    }
#endif

#if defined (CONFIG_RTTKERNEL)
    awos_arch_lock_irq();
    timekeeping_init();
    awos_init_bootstrap(awos_bsp_init, awos_init_thread, ticks);
#elif defined (CONFIG_ZEPHYRKERNEL)
    void _PrepC(void);
    _PrepC();
#else
#error "only support zephyr and rt-thread os engine."
#endif

    /* never come back */
    CODE_UNREACHABLE;
}
