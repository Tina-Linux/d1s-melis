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
#include <port.h>
#include <rtthread.h>
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
#include <compiler.h>

#include <hal_mem.h>
#include <hal_thread.h>

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

rt_ubase_t rt_hw_interrupt_disable(void)
{
    return awos_arch_lock_irq();
}

void rt_hw_interrupt_enable(rt_ubase_t level)
{
    return awos_arch_unlock_irq(level);
}

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

int aw_application_init(pfun_krnl_init_t app_init)
{
    rt_thread_t app_init_thread;

    // highest priority to system bringup.
    app_init_thread = rt_thread_create("app_init", app_init, RT_NULL, 0x4000, 0, 10);
    RT_ASSERT(app_init_thread != RT_NULL);

    rt_thread_startup(app_init_thread);

    return 0;
}

static void heap_clear_hook(void *ptr, rt_size_t size)
{
    if (ptr != RT_NULL)
    {
        memset(ptr, 0x00, size);
    }
    return;
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
    if (kthread_self() == RT_NULL)
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

    kthread_sleep(ticks > 0 ? ticks : 1);
    return 0;
}

void sleep(int seconds)
{
    kthread_sleep(seconds * CONFIG_HZ);
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

void awos_init_bootstrap(pfun_bsp_init_t aw_system_bsp_init, pfun_krnl_init_t app_init, uint64_t ticks)
{
    void *heap_start, *heap_end;
    unsigned long ttbr;

    heap_start = (void *)&__bss_end[0];
    heap_end   = (void *)HEAP_TOP;

    rt_system_timer_init();
    rt_system_heap_init(heap_start, heap_end);

#ifdef CONFIG_KASAN
    void kasan_init(void);
    void rt_page_alloc_func_hook(void *ptr, rt_size_t pages);
    void rt_page_free_func_hook(void *ptr, rt_size_t pages);
    void rt_malloc_large_func_hook(void *ptr, rt_size_t size);
    void rt_free_large_func_hook(void *ptr, rt_size_t size);
    void rt_malloc_small_func_hook(void *ptr, rt_size_t size);
    void rt_free_small_func_hook(void *ptr, rt_size_t size);
    void rt_realloc_small_func_hook(void *ptr, rt_size_t size);
    void rt_free_func_hook(void *ptr);

    kasan_init();
    rt_page_alloc_sethook(rt_page_alloc_func_hook);
    rt_page_free_sethook(rt_page_alloc_func_hook);
    rt_malloc_large_sethook(rt_malloc_large_func_hook);
    rt_free_large_sethook(rt_free_large_func_hook);
    rt_malloc_small_sethook(rt_malloc_small_func_hook);
    rt_free_small_sethook(rt_free_small_func_hook);
    rt_realloc_small_sethook(rt_realloc_small_func_hook);
    rt_free_sethook(rt_free_func_hook);
#endif

    rt_memheap_init(&coherent,
                    "dma-coherent",
                    (void *)HEAP_BEGIN_COHERENT,
                    COHERENT_SIZE);

#ifndef CONFIG_BOOTUP_TURBO
    rt_malloc_sethook(heap_clear_hook);
#endif
    extern void common_init(void);
    common_init();

    rt_system_scheduler_init();
    rt_system_timer_thread_init();

    void system_time_init(void);
    system_time_init();

    void task_inited_hook(rt_thread_t thread);
    rt_thread_inited_sethook(task_inited_hook);

    void task_delete_hook(rt_thread_t thread);
    rt_thread_delete_sethook(task_delete_hook);

#ifdef CONFIG_OF
    unflatten_device_tree();
#endif

    awos_arch_specific_init();

    if (aw_system_bsp_init)
    {
        aw_system_bsp_init();
    }
    else
    {
        while (1);
    }

    /*disable console log buffer.*/
    setbuf(stdout, NULL);
    setbuf(stdin, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

#ifndef CONFIG_DISABLE_ALL_DEBUGLOG
    void show_melis_version(void);
    show_melis_version();
#endif

#ifdef CONFIG_PRINT_CPU_INFO
    void print_cpu_info(void);
    print_cpu_info();
#endif

    __asm__ volatile("mrc p15, 0, %0, c2, c0, 0" : "=r"(ttbr));
    __log("heap info: start = 0x%p, end = 0x%p, page_table at 0x%08x.", \
          heap_start, heap_end, ttbr);

    aw_application_init(app_init);

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

unsigned long arch_irq_is_disable(void)
{
    unsigned long result;
    __asm__ volatile ("mrs %0, cpsr" : "=r" (result) );
    if (result & 0x02)
        return 1;
    return 0;
}
