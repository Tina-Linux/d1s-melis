#include <rtthread.h>
#include "msh.h"
#include <finsh.h>
#include <finsh_api.h>
#include <shell.h>
#include <hal/sunxi_hal_rtc.h>
#include <drv/sunxi_drv_watchdog.h>
#include <drv/sunxi_drv_spinor.h>
#include <ktimer.h>
#include <hal/sdmmc/sd_test.h>

#include <backtrace.h>

void rt_perf_init(void);
void rt_perf_exit(void);

extern void     dump_memory(uint32_t *buf, int32_t len);
extern int32_t  check_virtual_address(unsigned long vaddr);
extern void     esKRNL_MemLeakChk(uint32_t en);

static int msh_backtrace(int argc, const char **argv)
{
    if (argc < 2)
    {
        backtrace(NULL, NULL, 0, 0, printf);
    }
    else
    {
        backtrace((char *)argv[1], NULL, 0, 0, printf);
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(msh_backtrace, __cmd_backtrace, backtrace command);

static int cmd_mmlk(int argc, const char **argv)
{
    static int i = 0;
    i = (i == 0) ? 1 : 0;
    esKRNL_MemLeakChk(i);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mmlk, __cmd_mmlk, memory leak check command);

#ifdef CONFIG_RT_USING_TASK_PERF_MONITOR
static int cmd_top(int argc, const char **argv)
{
    rt_perf_init();

    while (1)
    {
        char ch = getchar();
        if (ch == 'q' || ch == 3)
        {
            rt_perf_exit();

            return 0;
        }
        printf("    Please enter 'q' or Ctrl-C to quit top command.\n");
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_top, __cmd_top, top task);
#endif

static int cmd_reboot(int argc, const char **argv)
{
    int i = 0;
    if (argc == 2)
    {
        if (!strcmp(argv[1], "efex"))
        {
            printf("**** Jump to efex! *****\n");
#ifdef CONFIG_DRIVER_RTC
            hal_rtc_set_fel_flag();
#endif
        }
    }
#ifdef CONFIG_DRIVER_SPINOR
    sunxi_driver_spinor_deinit();
#endif
#ifdef CONFIG_DRIVER_WDT
    reset_cpu();
#endif
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_reboot, __cmd_reboot, reboot or jump to efex);

#if 0
static int cmd_dump_dram_para(int argc, const char **argv)
{
    extern boot_head_t kernel_param;
    unsigned int *port = (unsigned int *)(&kernel_param.boot_data.dram_para);
    int i = 0;
    for (; i < 32; i++, port++)
    {
        printf("dram para[%d] = 0x%x\n", i, *port);
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_dump_dram_para, __cmd_dump_dram_para, dump dram para);

#endif

static int cmd_$(int argc, const char **argv)
{
    *(unsigned long *)(0xf0000000) = 0;
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_$, __cmd_$, enter panic mode);

static int mmu_check_valid_address(void *start_addr, void *end_addr)
{
    if (start_addr > end_addr && end_addr != NULL)
    {
        return 0;
    }

    if (check_virtual_address((unsigned long)start_addr) != 1)
    {
        return 0;
    }

    if (end_addr != NULL)
    {
        if (check_virtual_address((unsigned long)end_addr) != 1)
        {
            return 0;
        }
    }

    return 1;
}

static int cmd_print_mem_value(int argc, const char **argv)
{
    char *err = NULL;
    unsigned long start_addr, end_addr;
    uint32_t len;

    if (argc < 2)
    {
        printf("Unsage: p addr [len]\n");
        return 0;
    }

    if (argc > 2)
    {
        start_addr = strtoul(argv[1], &err, 0);

        len = strtoul(argv[2], &err, 0);
        end_addr = start_addr + len;
        if (mmu_check_valid_address((void *)start_addr, (void *)end_addr) && end_addr != 0)
        {
            dump_memory((uint32_t *)start_addr, len / sizeof(uint32_t));
        }
        else
        {
            printf("Invalid address!\n");
        }
    }
    else
    {
        start_addr = strtoul(argv[1], &err, 0);
        if (mmu_check_valid_address((void *)start_addr, NULL))
        {
            dump_memory((uint32_t *)start_addr, sizeof(unsigned long) / sizeof(uint32_t));
        }
        else
        {
            printf("Invalid address!\n");
        }
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_print_mem_value, __cmd_p, print memory or register value);

static int cmd_modify_mem_value(int argc, const char **argv)
{
    unsigned long addr, value;
    char *err = NULL;

    if (argc < 3)
    {
        printf("Unsage: m addr value\n");
        return -1;
    }

    addr = strtoul(argv[1], &err, 0);
    value = strtoul(argv[2], &err, 0);

    if (mmu_check_valid_address((void *)addr, NULL))
    {
        *((volatile unsigned int *)addr) = value;
    }
    else
    {
        printf("Invalid address!\n");
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_modify_mem_value, __cmd_m, modify memory or register value);

#if 0
static int cmd_wtd(int argc, const char **argv)
{

    if (argc != 2)
    {
        printf("wtd reset/off/restart/info");
        return -1;
    }

    if (strcmp(argv[1], "reset") == 0)
    {
        drv_watchdog_reset(16);
        printf("after reset,now watchdog info is : \n");
        drv_watchdog_info();
    }
    else if (strcmp(argv[1], "off") == 0)
    {
        drv_watchdog_disable();
        printf("after disable,now watchdog info is : \n");
        drv_watchdog_info();
    }
    else if (strcmp(argv[1], "restart") == 0)
    {
        reset_cpu();
        printf("after restart,now watchdog info is : \n");
        drv_watchdog_info();
    }
    else if (strcmp(argv[1], "info") == 0)
    {
        drv_watchdog_info();
    }
    else
    {
        printf("not support %s \n", argv[1]);
    }

    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_wtd, __cmd_wtd, watchdog test);

#ifdef CONFIG_SAMPLE_GETTIMEOFDAY
int cmd_gettimeofday(int argc, const char **argv)
{
    struct timespec64 ts = {0LL, 0L};

    do_gettimeofday(&ts);

    printf("SEC: %lld, NSEC: %ld\n", ts.tv_sec, ts.tv_nsec);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_gettimeofday, __cmd_gettimeofday, getimeofday);
#endif
#endif

void show_irqs(void);

int cmd_listirq(int argc, const char **argv)
{
    show_irqs();
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_listirq, __cmd_listirq, list irq handler.);

void slab_info(void);
int cmd_slabinfo(int argc, const char **argv)
{
    slab_info();
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_slabinfo, __cmd_slabinfo, slab information.);

int cmd_uname(int argc, const char **argv)
{
	printf("Melis\n");
	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_uname, __cmd_uname, cat system.);

int cmd_chip_name(int argc, const char **argv)
{
#if defined(CONFIG_SOC_SUN3IW2) 
	printf("chip:1707\n");
#elif  defined (CONFIG_SOC_SUN20IW1)
	printf("chip:1859\n");
#elif  defined (CONFIG_SOC_SUN8IW19)
	printf("chip:1817\n");
#else
	printf("chip:unkonw\n");
#endif
	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_chip_name, __cmd_chip_name, chip name information.);


#if 0
static int cmd_mmc_save(int argc, const char **argv)
{

    FILE *file_reg;
    uint32_t i = 0;
    uint32_t mode = 0;
    uint32_t reg_addr, reg_value;
    struct mmc_host *host;
    SDC_InitTypeDef sdc_param = { 0 };
    uint32_t card_id = 1, debug_mask = 0;
    int ret;
    ret = 0;
    struct mmc_card_info *s_card_info;

    char *dst_file = "/data/sdio1_reg.bin";
    if (argc < 2)
    {
        printk("Usage:\n");
        printk("\tqigan_sdio <mode> \n");
        printk("\t<mode> 0:save, 1:resume\n");
        return -1;
    }
    mode = strtol(argv[1], NULL, 0);
    printf("mode = %d\n", mode);
    printf("save data:\n");
    file_reg = fopen(dst_file, "wb");
    if (file_reg == NULL)
    {
        printf("%s open failed\n", file_reg);
        goto qigan_sdio_exit;
    }
    s_card_info = mmc_card_save(card_id);
    if (!s_card_info)
    {
        goto close_file;
    }
    fwrite(s_card_info, sizeof(struct mmc_card_info), 1, file_reg);

    free(s_card_info);
close_file:
    fclose(file_reg);
qigan_sdio_exit:
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mmc_save, __cmd_mmc_save, mmc_save);

static int cmd_mmc_restore(int argc, const char **argv)
{
    FILE *file_reg;
    uint32_t i = 0;
    uint32_t mode = 0;
    uint32_t reg_addr, reg_value;
    struct mmc_host *host;
    SDC_InitTypeDef sdc_param = { 0 };
    struct mmc_card_info s_card_info;
    uint32_t card_id = 1, debug_mask = 0x3c;
    int ret;
    ret = 0;
    char *dst_file = "/data/sdio1_reg.bin";
    if (argc < 2)
    {
        printk("Usage:\n");
        printk("\tqigan_sdio <mode> \n");
        printk("\t<mode> 0:save, 1:resume\n");
        return -1;
    }
    mode = strtol(argv[1], NULL, 0);
    printf("mode = %d\n", mode);
    printf("resume data:\n");
    file_reg = fopen(dst_file, "rb");
    if (file_reg == NULL)
    {
        printf("%s open failed\n", file_reg);
        goto qigan_sdio_exit;
    }

    fread(&s_card_info, sizeof(struct mmc_card_info), 1, file_reg);
    sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
    sdc_param.debug_mask = debug_mask;
    host = hal_sdc_create(card_id, &sdc_param);
    //host->State = SDC_STATE_RESET;
    hal_sdc_init(host);
    mmc_card_restore(&s_card_info);
    fclose(file_reg);

qigan_sdio_exit:
    return ret;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mmc_restore, __cmd_mmc_restore, mmc_restore);
#endif

#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
static int cmd_set_log_level(int argc, char **argv)
{
    unsigned long level;
    char *end;

    if (argc != 2)
    {
        printf("Usage: set_log_level level\n");
		printf("       set_log_level 0 : disable __log/__err/__wrn/__inf/__msg and printk output\n");
        printf("       set_log_level 1 : disable __err/__wrn/__inf/__msg output\n");
        printf("       set_log_level 2 : disable __wrn/__inf/__msg output\n");
        printf("       set_log_level 3 : disable __inf/__msg output\n");
        printf("       set_log_level 4 : disable __msg output\n");
        printf("       set_log_level 5 : enable all log output\n");
        return -1;
    }

    level = strtoul(argv[1], &end, 0);
    set_log_level((int)level);
    printf("current level is %d\n", get_log_level());

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_set_log_level, __cmd_set_log_level, set log level);

static int cmd_get_log_level(int argc, char **argv)
{
    printf("current level is %d\n", get_log_level());

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_get_log_level, __cmd_get_log_level, get log level);
#endif

#if 0
static int cmd_rtc(int argc, const char **argv)
{

    if (argc != 2)
    {
        printf("rtc set_fel/probe_fel/clear_fel/get_boot_mode \n");
        return -1;
    }

    if (strcmp(argv[1], "set_fel") == 0)
    {
        rtc_get_bootmode_flag();
    }
    else if (strcmp(argv[1], "probe_fel") == 0)
    {
        rtc_probe_fel_flag();
    }
    else if (strcmp(argv[1], "clear_fel") == 0)
    {
        rtc_clear_fel_flag();
    }
    else if (strcmp(argv[1], "get_boot_mode") == 0)
    {
        rtc_get_bootmode_flag();

        else
        {
            printf("not support %s \n", argv[1]);
        }

    }
    FINSH_FUNCTION_EXPORT_ALIAS(cmd_rtc, __cmd_rtc, rtc test);
#endif
