/*
 * =====================================================================================
 *
 *       Filename:  port.c
 *
 *    Description:  porting layer for custom OS implementation.
 *
 *        Version:  2.0
 *         Create:  2017-11-03 19:16:44
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-26 11:46:30
 *
 * =====================================================================================
 */

#include <port.h>
#include <rtthread.h>
#include <version.h>
#include <kconfig.h>
#include <preempt.h>
#include <_newlib_version.h>
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch.h>
#include <const.h>
#include <ktimer.h>
#include <log.h>

#define MELIS_VERSION_STRING(major, minor, patch) \
    #major"."#minor"."#patch

static const char *melis_banner_string(void)
{
    return ("V"MELIS_VERSION_STRING(3, 9, 0));
}

static int object_name_maxlen(struct rt_list_node *list)
{
    struct rt_list_node *node;
    struct rt_object *object = RT_NULL;
    int max_length = 0, length;

    rt_enter_critical();
    for (node = list->next; node != list; node = node->next)
    {
        object = rt_list_entry(node, struct rt_object, list);

        length = rt_strlen(object->name);
        if (length > max_length)
        {
            max_length = length;
        }
    }
    rt_exit_critical();

    if (max_length > RT_NAME_MAX || max_length == 0)
    {
        max_length = RT_NAME_MAX;
    }

    return max_length;
}

void object_split(int len)
{
    while (len--)
    {
        rt_kprintf("-");
    }
}

//accelerate use cpu special inst.
unsigned int __rt_ffs(rt_uint32_t value)
{
    if (value == 0)
    {
        return 0;
    }

    return 32UL - __builtin_clz(value & -value);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_interrupt_disable <disable interrupt>
 *
 * @return old cpsr statue.
 */
/* ----------------------------------------------------------------------------*/
rt_ubase_t rt_hw_interrupt_disable(void)
{
    return awos_arch_lock_irq();
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_interrupt_enable <enable interrupt>
 *
 * @param level, old cpsr status.
 */
/* ----------------------------------------------------------------------------*/
void rt_hw_interrupt_enable(rt_ubase_t level)
{
    return awos_arch_unlock_irq(level);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_context_switch_to <trigger thread context switch>
 *
 * @param to, stack pointer of the next thread.
 */
/* ----------------------------------------------------------------------------*/
void rt_hw_context_switch_to(rt_ubase_t to)
{
    return awos_arch_first_task_start(to);
}

/*
 * preempt schedule during exit irq environment.
 */
void schedule_preempt_inirq(void)
{
    // not allowd preemptation either scheduler lock or irqnest
    if (preempt_level() != 0 || hardirq_count() != 0)
    {
        return;
    }

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    //irq procedure cant change the status of current thread, if so, report bug.
    //RT_ASSERT((rt_thread_self()->stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY);
    /*
     * Notice: The arch_local_irq_enable/arch_lock_irq_disable should be work if
     * Each interrupt source perform the right acknowledge operations during the
     * irqhandler. but it seems did not ready now, so, each victim task(especially idle)
     * should prepare the irqstack by extend the task stack.
     * here, mask the enable/disable operations to avoid this.
     */
    do
    {
        //arch_local_irq_enable();
        rt_schedule();
        //arch_local_irq_disable();
    } while (thread_need_resched());

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    return;
}

static void finish_task_switch(rt_thread_t ARG_UNUSED(last))
{

}
/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_context_switch <swith thread from 'from' task to 'to' task.>
 *
 * @param from, sp of the thread need to sched out.
 * @param to, sp of the thread need to shced in.
 * The rt_hw_context_switch exception is the only execution context in the
 * system that can perform context switching. When an execution context finds
 * out it has to switch contexts, it call this.
 * When called, the decision that a context switch must happen has already been
 * taken. In other words, when it runs, we *know* we have to do context switch.
 */
/* ----------------------------------------------------------------------------*/
void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to)
{
    rt_ubase_t last;

    rt_thread_t prev = rt_list_entry(from, struct rt_thread, sp);
    rt_thread_t next = rt_list_entry(to, struct rt_thread, sp);

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    //#define TRACE_SWITCH
#ifdef TRACE_SWITCH
    __log("---------------------------------------------------------");
    __log("|high:[%8s][0x%08x]====>[%8s][0x%08x].|", \
          prev->name, prev->sched_o, next->name, next->sched_i);
#endif

    last = awos_arch_task_switch(from, to);

    if (!irqs_disabled())
    {
        __err("irqmask status error.");
        software_break();
    }

    RT_ASSERT(rt_thread_self() == prev);

    rt_thread_t task_last = rt_list_entry(last, struct rt_thread, sp);
    finish_task_switch(task_last);

#ifdef TRACE_SWITCH
    __log("|oldr:[%8s][0x%08x]====>[%8s][0x%08x].|", \
          prev->name, prev->sched_o, next->name, next->sched_i);
    __log("|down:[%8s][0x%08x]<====[%8s][0x%08x].|", \
          prev->name, prev->sched_i, task_last->name, task_last->sched_o);
    __log("---------------------------------------------------------");
#endif
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_hw_stack_init <prepare init context of specify thread, fill the
 *         initialize registers value on the stack.>
 *
 * @param entry. task entry point.
 * @param parameter, parameter for entry point.
 * @param stack_addr, stack base address.
 * @param exit, context if returned from entry.
 *
 * @return
 *    -sp, stack pointer that has been prepared.
 */
/* ----------------------------------------------------------------------------*/
rt_uint8_t *rt_hw_stack_init(void *entry, void *parameter, \
                             rt_uint8_t *stack_addr, void *exit)
{
    return awos_arch_stack_init(entry, parameter, stack_addr, exit);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_create <create task with specify parameter>
 *
 * @param name, thread name,
 * @param entry, entry function point of the task.
 * @param parameter, parameter for  'entry'
 * @param stack_size, stack size for stack.
 * @param priority, prority of the thread.
 * @param tick, time slice of the task, round-robin scheduler.
 *
 * @return
 *    -task handle if success.
 *    -NULL if faulre.
 */
/* ----------------------------------------------------------------------------*/
__hdle awos_task_create(const char *name, void (*entry)(void *parameter), \
                        void *parameter, uint32_t stack_size, uint8_t priority, \
                        uint32_t tick)
{
    rt_thread_t thr;

    thr = rt_thread_create(name, entry, parameter, stack_size, \
                           priority, tick);

    RT_ASSERT(thr != RT_NULL);
    rt_thread_startup(thr);

    return (__hdle)thr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_delete <delete a task from system scheduler structure.>
 *
 * @param thread, handle for specify thread.
 *
 * @return
 *    -0 if success.
 */
/* ----------------------------------------------------------------------------*/
int32_t awos_task_delete(__hdle thread)
{
    RT_ASSERT(thread != RT_NULL);
    if (thread == rt_thread_self())
    {
        void rt_thread_exit(void);
        rt_thread_exit();
        CODE_UNREACHABLE;
    }
    else
    {
        rt_thread_delete((rt_thread_t)thread);
    }

    return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_self <return thread hanlde of current context.>
 *
 * @return
 *    -[hdl], hdl of current context thread handle.
 *    -NULL, if faiulre.
 */
/* ----------------------------------------------------------------------------*/
__hdle awos_task_self(void)
{
    return (__hdle)rt_thread_self();
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_enter <enter interrupt log.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_enter(void)
{
    rt_interrupt_enter();
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_leave <leave interrupt log.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_leave(void)
{
    rt_interrupt_leave();
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_tick_increase <system tick increase, must be invokded in
 *          timier irq context.>
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_tick_increase(void)
{
    rt_tick_increase();
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_isin_irq <judge whether in irq context>
 *
 * @return
 *   -0: in task environment
 *   -1: in interrupt environment.
 */
/* ----------------------------------------------------------------------------*/
uint8_t awos_arch_isin_irq(void)
{
    return rt_interrupt_get_nest() ?  1 : 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_malloc_align <alloc memory meet specify alignment.>
 *
 * @param[in] size, memory size need to alloc.
 * @param[in] align, aligment needs.
 *
 * @return
 *    -NULL, if faulre.
 *    -[ptr], address if success.
 */
/* ----------------------------------------------------------------------------*/
void *k_malloc_align(uint32_t size, uint32_t align)
{
    /*return rt_page_alloc(size/RT_MM_PAGE_SIZE, align / RT_MM_PAGE_SIZE);*/
    return rt_malloc_align(size, align);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_free_align <free align>
 *
 * @param[in] ptr, the memory start address need to free.
 */
/* ----------------------------------------------------------------------------*/
void k_free_align(void *ptr, uint32_t size)
{
    /*return rt_page_free(ptr, size / RT_MM_PAGE_SIZE);*/
    return rt_free_align(ptr);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_malloc <system heap alloc interface>
 *
 * @param[in] sz, size you want to alloc.
 *
 * @return
 *    - NULL, if no heap left.
 *    - [ptr],pointer of the allocated memory if scuess.
 */
/* ----------------------------------------------------------------------------*/
/*void k_malloc(uint32_t sz) __attribute__((alias("rt_malloc")));*/

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_free <system heap free interface>
 *
 * @param[in] ptr, area you want to free.
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
/*void k_free(void* ptr) __attribute__((alias("rt_free")));*/

/* ----------------------------------------------------------------------------*/
/**
 * @brief  rt_application_init <application init entry.>
 *
 * @param app_init
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
int rt_application_init(pfun_krnl_init_t app_init)
{
    rt_thread_t app_init_thread;

    // highest priority to system bringup.
    app_init_thread = rt_thread_create("app_init", app_init, RT_NULL, 0x4000, 0, 10);
    RT_ASSERT(app_init_thread != RT_NULL);

    rt_thread_startup(app_init_thread);

    return 0;
}

static const char melis_banner [] =
{
    "|                  /'\\_/`\\       (_ )  _                       /'_  )    /'_ `\\   /' _`\\  \n\r"
    "|                  |     |   __   | | (_)  ___  ______  _   _ (_)_) |   ( (_) |   | ( ) |     \n\r"
    "|                  | (_) | /'__`\\ | | | |/',__)(______)( ) ( ) _(_ <     \\__, |   | | | |   \n\r"
    "|                  | | | |(  ___/ | | | |\\__, \\        | \\_/ |( )_) | _     | | _ | (_) |  \n\r"
    "|                  (_) (_)`\\____)(___)(_)(____/        `\\___/'`\\____)(_)    (_)(_)`\\___/' \n\r"
};
static void show_melis_version(void)
{
    //use newlibc printf for long strings.
    printk("\n\r");
    printk("===============================================================================================================\n\r");
    printk("%s", melis_banner);
    printk("|version : %s\n\r", melis_banner_string());
    printk("|commitid: %.*s\n\r", 100, SDK_GIT_VERSION);
    printk("|sunxiver: %x\n\r", MELIS_VERSION_CODE);
    printk("|timever : %.*s\n\r", 100, SDK_UTS_VERSION);
    printk("|compiler: %.*s\n\r", 100, GCC_VERSION);
#ifdef CONFIG_CC_OPTIMIZE_FOR_SIZE
    printk("|optimal : %.*s\n\r", 100, "-Os -g -gdwarf-2 -gstrict-dwarf");
#else
    printk("|optimal : %.*s\n\r", 100, "-O0 -g -gdwarf-2 -gstrict-dwarf");
#endif
    printk("|linker  : %.*s\n\r", 100, LNK_VERSION);
    printk("|newlibc : %.*s\n\r", 100, _NEWLIB_VERSION);
    printk("|author  : %.*s\n\r", 100, SDK_COMPILE_BY);
    printk("===============================================================================================================\n\r");
    printk("\n\r");
}

static void heap_clear_hook(void *ptr, rt_size_t size)
{
    if (ptr != RT_NULL)
    {
        rt_memset(ptr, 0x00, size);
    }
    return;
}

static const char *crbits[] = { "M", "A", "C", "", "", "", "", "", "", "",
                                "SW", "Z", "I", "V", "", "", "", "", "",
                                "WXN", "UWXN", "", "", "", "", "EE", "",
                                "", "TRE", "AFE", "TE", ""
                              };

static void decode_cache_cfg(uint32_t size)
{
    int32_t linesize = (1 << 2) << size;

    printf("linesize = %ld bytes\r\n", linesize);
}

static void print_cpu_info(void)
{
    uint32_t cr;
    uint32_t mainid;
    int32_t  i;
    int32_t  cpu_arch;
    char    *isa_arch, *vendor;

    void decode_cache_info(void);
    decode_cache_info();

    __asm__ __volatile__(
        "mrc p15, 0, %0, c1, c0, 0\n"
        : "=r"(cr) : : "memory");

    __asm__ __volatile__(
        "mrc p15, 0, %0, c0, c0, 0\n"
        : "=r"(mainid) : : "memory");

    printf("Control register: ");
    for (i = 0; i < sizeof(crbits) / sizeof(crbits[0]); i++)
    {
        if (cr & (1 << i))
        {
            printf("%s ", crbits[i]);
        }
    }

    printf("\n\r");
    switch (mainid >> 24)
    {
        case 0x41:
            vendor = "ARM";
            break;
        case 0x44:
            vendor = "Digital Equipment Corp.";
            break;
        case 0x40:
            vendor = "Motorola - Freescale Semiconductor Inc.";
            break;
        case 0x56:
            vendor = "Marvell Semiconductor Inc.";
            break;
        case 0x69:
            vendor = "Intel Corp.";
            break;
        default:
            vendor = "Unknown";
    }

    if ((mainid & 0x0008f000) == 0)
    {
        cpu_arch = CPU_ARCH_UNKNOWN;
    }
    else if ((mainid & 0x0008f000) == 0x00007000)
    {
        cpu_arch = (mainid & (1 << 23)) ? CPU_ARCH_ARMv4T : CPU_ARCH_ARMv3;
    }
    else if ((mainid & 0x00080000) == 0x00000000)
    {
        cpu_arch = (mainid >> 16) & 7;
        if (cpu_arch)
        {
            cpu_arch += CPU_ARCH_ARMv3;
        }
    }
    else if ((mainid & 0x000f0000) == 0x000f0000)
    {
        uint32_t mmfr0;

        /*
        ¦* Revised CPUID format. Read the Memory Model Feature
        ¦* Register 0 and check for VMSAv7 or PMSAv7
        ¦*/
        asm("mrc p15, 0, %0, c0, c1, 4" : "=r"(mmfr0));
        if ((mmfr0 & 0x0000000f) >= 0x00000003 || (mmfr0 & 0x000000f0) >= 0x00000030)
        {
            cpu_arch = CPU_ARCH_ARMv7;
        }
        else if ((mmfr0 & 0x0000000f) == 0x00000002 || (mmfr0 & 0x000000f0) == 0x00000020)
        {
            cpu_arch = CPU_ARCH_ARMv6;
        }
        else
        {
            cpu_arch = CPU_ARCH_UNKNOWN;
        }
    }
    else
    {
        cpu_arch = CPU_ARCH_UNKNOWN;
    }

    switch (cpu_arch)
    {
        case CPU_ARCH_ARMv3:
            isa_arch = "v3";
            break;
        case CPU_ARCH_ARMv4:
            isa_arch = "v4";
            break;
        case CPU_ARCH_ARMv4T:
            isa_arch = "v4T";
            break;
        case CPU_ARCH_ARMv5:
            isa_arch = "v5";
            break;
        case CPU_ARCH_ARMv5T:
            isa_arch = "v5T";
            break;
        case CPU_ARCH_ARMv5TE:
            isa_arch = "v5TE";
            break;
        case CPU_ARCH_ARMv5TEJ:
            isa_arch = "v5TEJ";
            break;
        case CPU_ARCH_ARMv6:
            isa_arch = "v6";
            break;
        case CPU_ARCH_ARMv7:
            isa_arch = "v7";
            break;
        case CPU_ARCH_UNKNOWN:
        default:
            isa_arch = "Unknown";
    }

    printf("vendor: %s\r\nISA-arch: %s\r\n", vendor, isa_arch);

    if (cpu_arch == CPU_ARCH_ARMv7)
    {
        int32_t major, minor;
        char *part;
        major = (mainid >> 20) & 0xf;
        minor = mainid & 0xf;
        switch (mainid & 0xfff0)
        {
            case ARM_CPU_PART_CORTEX_A5:
                part = "Cortex-A5";
                break;
            case ARM_CPU_PART_CORTEX_A7:
                part = "Cortex-A7";
                break;
            case ARM_CPU_PART_CORTEX_A8:
                part = "Cortex-A8";
                break;
            case ARM_CPU_PART_CORTEX_A9:
                part = "Cortex-A9";
                break;
            case ARM_CPU_PART_CORTEX_A15:
                part = "Cortex-A15";
                break;
            default:
                part = "unknown";
        }
        printf("core: %s r%ldp%ld\r\n", part, major, minor);
    }
    printf("\n\r");
}

void set_console_device(void)
{
    rt_console_set_device("uart0");
}

extern uint32_t melis_arch_use_fpu;
void kthread_enable_fpu(void)
{
    uint32_t fpscr = 0;
    uint32_t fpexc;

    /* A task is registering the fact that it needs an FPU context.  Set the
    FPU flag (which is saved as part of the task context). */
    melis_arch_use_fpu = 1;

    fpexc = fmrx(FPEXC) | FPEXC_EN;
    fmxr(FPEXC, fpexc);

    /* Initialise the floating point status register. */
    __asm__ __volatile__("fmxr  fpscr, %0" :: "r"(fpscr) : "memory");
}

void kthread_stop_fpu(void)
{
    /* A task is registering the fact that it needs an FPU context.  Set the
    FPU flag (which is saved as part of the task context). */
    melis_arch_use_fpu = 0;

    fmxr(FPEXC, fmrx(FPEXC) & ~FPEXC_EN);
}

int kthread_get_fpustat(void)
{
    if (rt_thread_self() == RT_NULL)
    {
        __err("fatal, scheduler not started!");
        return -1;
    }

    if (melis_arch_use_fpu == 1)
    {
        return 1;
    }
    else if (melis_arch_use_fpu == 0)
    {
        return 0;
    }
    else
    {
        __err("fatal, fpu status corrupted!");
        return -1;
    }
}

int msleep(unsigned int msecs)
{
    int ticks = msecs / (1000 / CONFIG_HZ);

    rt_thread_delay(ticks > 0 ? ticks : 1);
    return 0;
}

void sleep(int seconds)
{
    rt_thread_delay(seconds * CONFIG_HZ);
}

int smp_cpu_id(void)
{
    int cpuid;

    asm volatile("mrc p15, 0, %0, c0, c0, 5 \n"
                 : "=r"(cpuid)
                 :
                 : "cc");

    cpuid &= 0xf;

    return cpuid;
}


// CONFIG_COHERENT_SIZE for DMA hardware conherent.
#define COHERENT_SIZE         (CONFIG_COHERENT_SIZE)

//normal heap margin.
#define HEAP_TOP              (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE - COHERENT_SIZE)

// dma heap margin.
#define HEAP_BEGIN_COHERENT   (CONFIG_COHERENT_START_ADDR)
#define HEAP_END_COHERENT     (HEAP_BEGIN_COHERENT + COHERENT_SIZE)

extern uint8_t __bss_end[];
int awos_arch_specific_init(void);
#ifdef CONFIG_OF
void unflatten_device_tree(void);
#endif
struct rt_memheap coherent;
rt_err_t rt_memheap_init(struct rt_memheap *memheap,
                         const char        *name,
                         void              *start_addr,
                         rt_size_t         size);

unsigned long dma_coherent_alloc(unsigned long *virt_addr, unsigned long size)
{
    if (virt_addr == NULL)
    {
        __err("fatal error, inmput paramter invalid.");
        software_break();
        return 0;
    }

    *virt_addr = (unsigned long)rt_memheap_alloc(&coherent, size);
    if (*virt_addr == 0)
    {
        __err("alloc dma buffer failure.");
        return 0;
    }

    return *virt_addr - HEAP_BEGIN_COHERENT + (CONFIG_DRAM_PHYBASE + CONFIG_DRAM_SIZE - CONFIG_COHERENT_SIZE);
}

void dma_coherent_free(unsigned long virt_addr)
{
    rt_memheap_free((void *)virt_addr);
}

void awos_init_bootstrap(pfun_bsp_init_t rt_system_bsp_init, pfun_krnl_init_t app_init, uint64_t ticks)
{
    void *heap_start, *heap_end;
    rt_ubase_t ttbr;

    heap_start = (void *)&__bss_end[0];
    heap_end   = (void *)HEAP_TOP;

    rt_system_timer_init();
    rt_system_heap_init(heap_start, heap_end);
    rt_memheap_init(&coherent,
                    "dma-coherent",
                    (void *)HEAP_BEGIN_COHERENT,
                    COHERENT_SIZE);

#ifndef CONFIG_BOOTUP_TURBO
    rt_malloc_sethook(heap_clear_hook);
#endif
    rt_system_scheduler_init();
    rt_system_timer_thread_init();

    void task_inited_hook(rt_thread_t thread);
    rt_thread_inited_sethook(task_inited_hook);

    void task_delete_hook(rt_thread_t thread);
    rt_thread_delete_sethook(task_delete_hook);

#ifdef CONFIG_OF
    unflatten_device_tree();
#endif

    awos_arch_specific_init();

    if (rt_system_bsp_init)
    {
        rt_system_bsp_init();
    }
    else
    {
        while (1);
    }

    /*disable console log buffer.*/
    setbuf(stdout, NULL);

#ifndef CONFIG_DISABLE_ALL_DEBUGLOG
    show_melis_version();
#endif

#ifdef CONFIG_PRINT_CPU_INFO
    print_cpu_info();
#endif

    __asm__ volatile("mrc p15, 0, %0, c2, c0, 0" : "=r"(ttbr));
    __log("heap info: start = 0x%p, end = 0x%p, page_table at 0x%08x.", \
          heap_start, heap_end, ttbr);

    rt_application_init(app_init);

    /* initialize idle thread */
    rt_thread_idle_init();

    int do_gettimeofday(struct timespec64 * ts);
    struct timespec64 ts;
    do_gettimeofday(&ts);

    double ns_per_ticks = NS_PER_TICK;
    printf("[BOOT0]: ns from boot0: %f.\n", ticks * ns_per_ticks);
    timestamp("OSSTART");

    /* start scheduler */
    rt_system_scheduler_start();

    /* never come back, this marks the end of initialization.*/
    CODE_UNREACHABLE;
}

void local_irq_disable(void)
{
    asm volatile ("cpsid i");
}

void local_irq_enable(void)
{
    asm volatile ("cpsie i");
}
