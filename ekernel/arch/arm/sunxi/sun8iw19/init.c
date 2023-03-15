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
#include <script.h>
#include <hal_cfg.h>
#include <init.h>
#include <compiler.h>

#define     STACK_CHECK_MAGICNUMBER    (0x5a5a5a5a)

uint32_t    melis_arch_use_fpu = 0;
uint8_t     melis_kernel_running = 0;

void    hal_clock_init(void);
int     hal_gpio_init(void);
int32_t CNTR_Init(void);
void    sunxi_driver_uart_init(void);
void    set_console_device(void);
void    drv_dma_init(void);
int     multiple_console_init(void);
void    do_initcalls(void);
__s32 ksrv_init(void);
void timekeeping_init(void);

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

#ifdef CONFIG_CC_STACKPROTECTOR_STRONG
/*
 * Symbol referenced by GCC compiler generated code for canary value.
 * The canary value gets initialized in start_kernel().
 */
unsigned long __stack_chk_guard;

void __stack_chk_fail(void)
{
    __err("fata error: stack corrupted!");
#ifdef CONFIG_RT_USING_FINSH
    list_thread(1, NULL);
#endif
    software_break();
}
#endif

static noinline void clock_init(void)
{
    hal_clock_init();
}

static noinline void time_init(void)
{
    CNTR_Init();
}

noinline void serial_init(void)
{
    sunxi_driver_uart_init();
    set_console_device();
}

static int32_t awos_bsp_init(void)
{
    extern unsigned char blob_input_fexconfig_start[];
    extern unsigned char blob_input_fexconfig_end[];

#ifdef CONFIG_FEXCONFIG
    Hal_Cfg_Init(blob_input_fexconfig_start, blob_input_fexconfig_end - blob_input_fexconfig_start);
#endif
    hal_gpio_init();
    clock_init();
    time_init();
    serial_init();
    drv_dma_init();

#ifdef CONFIG_SUBSYS_MULTI_CONSOLE
    multiple_console_init();
#endif

    return 0;
}

noinline static void app_init(void)
{
#ifdef CONFIG_RT_USING_FINSH
    finsh_system_init();
#endif
    app_entry(NULL);
}

static void awos_init_thread(void *para)
{
#ifdef CONFIG_PTHREAD
    pthread_system_init();
#endif

    multiple_console_init();

#ifdef CONFIG_MELIS_VIRTUAL_FILESYSTEM
    fsys_init();
#endif

#ifdef CONFIG_MELIS_LEGACY_DRIVER_MAN
    dev_init();
#endif

#ifdef CONFIG_MELIS_KSERVICE
    ksrv_init();
#endif

    do_initcalls();

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

#ifdef CONFIG_RAMFS_DEVICE
    void mkfs(const char *fs_name, const char *device_name);
    mkfs(CONFIG_RAMFS_TYPE, "ram0");
    mount("ram0", CONFIG_RAMFS_MOUNT_PATH, CONFIG_RAMFS_TYPE, 0, 0);
#endif
#endif

    libc_stdio_set_console("uart0", O_RDWR);

#ifdef CONFIG_CXX
    cplusplus_system_init();
#endif

#ifdef CONFIG_ADBD_AUTOLOAD
    int adbd_main(void);
    adbd_main();
#endif

    app_init();
}

void start_kernel(uint64_t ticks)
{
    awos_arch_fpu_init();

#if defined CONFIG_CC_STACKPROTECTOR_STRONG
    __stack_chk_guard = STACK_CHECK_MAGICNUMBER;
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
