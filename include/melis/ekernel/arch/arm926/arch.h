/*
 * =====================================================================================
 *
 *       Filename:  asm.h
 *
 *    Description:  arch abstract layer of melis platform.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:13:58
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-11-26 12:11:47
 *
 * =====================================================================================
 */

#ifndef  __ARCH_ARM926__
#define  __ARCH_ARM926__

#include <stdint.h>
#include <cpu.h>

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_read_sp read stack pointer register.
 *
 * @return  the value of stack pointer register.
 */
/* ----------------------------------------------------------------------------*/
static inline uint32_t awos_arch_read_sp(void)
{
    uint32_t sp;
    asm volatile("mov %0, r13\n":"=r"(sp));
    return sp;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_cp15_ctl_rd read coprocesser control register.
 *
 * @return  the value of cp15 control register.
 */
/* ----------------------------------------------------------------------------*/
static inline uint32_t awos_arch_cp15_ctl_rd(void)
{
    uint32_t ctl;
    asm volatile("mrc p15, 0, %0, c1, c0, 0":"=r"(ctl));
    return ctl;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_cache_enable  d/i cache enable function.
 *
 * @param[in] bit, indicatior of D/I Cache.
 */
/* ----------------------------------------------------------------------------*/
static inline void awos_arch_cache_enable(uint32_t bit)
{
    asm volatile(\
                 "mrc  p15,0,r0,c1,c0,0\n\t"    \
                 "orr  r0,r0,%0\n\t"            \
                 "mcr  p15,0,r0,c1,c0,0"        \
                 :                              \
                 :"r"(bit)                     \
                 :"memory");
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_cache_disable d/i cache disable function.
 *
 * @param[in] bit indicator of D/I cache.
 */
/* ----------------------------------------------------------------------------*/
static inline void awos_arch_cache_disable(uint32_t bit)
{
    asm volatile(\
                 "mrc  p15,0,r0,c1,c0,0\n\t"    \
                 "bic  r0,r0,%0\n\t"            \
                 "mcr  p15,0,r0,c1,c0,0"        \
                 :                              \
                 :"r"(bit)                     \
                 :"memory");
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_icache_status  get icache status.
 *
 * @return
 *   -0: disable icache
 *   -1: enabled icache
 */
/* ----------------------------------------------------------------------------*/
static inline uint32_t awos_arch_icache_status(void)
{
    return (awos_arch_cp15_ctl_rd() & ICACHE_MASK);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief awos_arch_dcache_status get dcache status.
 *
 * @return
 *   -0: disable
 *   -1: enabled
 */
/* ----------------------------------------------------------------------------*/
static inline uint32_t awos_arch_dcache_status(void)
{
    return (awos_arch_cp15_ctl_rd() & DCACHE_MASK);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  __get_cpsr  Get cpsr register.
 *
 * @return  value of cpsr register.
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long __get_cpsr(void)
{
    unsigned long retval;
    asm volatile(
        " mrs  %0, cpsr":"=r"(retval): /* no inputs */);
    return retval;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  __set_cpsr  Set val to cpsr register.
 *
 * @param[in] val, new value of cpsr.
 */
/* ----------------------------------------------------------------------------*/
static inline void __set_cpsr(unsigned long val)
{
    asm volatile(
        " msr  cpsr, %0": /* no outputs */ :"r"(val));
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_disable_interrupt  disable the IRQ&FIQ response capablity
 * of processor.
 *
 * @return old cpsr value.
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long awos_arch_disable_interrupt(void)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | INT_MASK);
    return _cpsr;
}
/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_restore_interrupt restore the cpsr interrupt control status.
 *
 * @param old_cpsr  the backed cpsr status.
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long awos_arch_restore_interrupt(unsigned long old_cpsr)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr((_cpsr & ~INT_MASK) | (old_cpsr & INT_MASK));
    return _cpsr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  disable_irq disable irq interrupt.
 *
 * @return  old cpsr value.
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long disable_irq(void)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | IRQ_MASK);
    return _cpsr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  restore_irq  restore old irq control flag of cpsr.
 *
 * @param[in] old_cpsr old cpsr value.
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long restore_irq(unsigned long old_cpsr)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr((_cpsr & ~IRQ_MASK) | (old_cpsr & IRQ_MASK));
    return _cpsr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_enable_irq enable irq interrupt.
 *
 * @return  old cpsr status.
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long awos_arch_enable_irq(void)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr & ~IRQ_MASK);
    return _cpsr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_disable_fiq disable fiq interrupt.
 *
 * @return  old value of cpsr.
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long awos_arch_disable_fiq(void)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr(_cpsr | FIQ_MASK);
    return _cpsr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  restore_fiq  restor fiq status from old cpsr flag.
 *
 * @param[in] old_cpsr, backed value of environment.
 *
 * @return  old cpsr value.
 */
/* ----------------------------------------------------------------------------*/
static inline unsigned long restore_fiq(unsigned long old_cpsr)
{
    unsigned long _cpsr;
    _cpsr = __get_cpsr();
    __set_cpsr((_cpsr & ~FIQ_MASK) | (old_cpsr & FIQ_MASK));
    return _cpsr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_lock_irq disable the interrupt response capacity of processor.
 *
 * @return
 *   - old value of processor status register.
 */
/* ----------------------------------------------------------------------------*/
uint32_t awos_arch_lock_irq(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_unlock_irq restore the former value of status cpu register.
 *
 * @param[in] cpu_sr, former status register value needed to restore.
 */
/* ----------------------------------------------------------------------------*/

void awos_arch_unlock_irq(uint32_t cpu_sr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_set_vector_base <set new vector address>
 *
 * @param[in] newvector: new vector table address.
 * @attention: only ARM11 and cortex-a can modify the vector address.
 *             ARM7,ARM9,ARM10 only fixed at 0x0 or 0xffff0000.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_set_vector_base(uint32_t newvector);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_first_task_start start the operation system scheduler, and
 *         switch to the first prepare ready task, usually, the bringup thread.
 *
 * @param[in] to, the stack pointer of the next task switching into.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_first_task_start(uint32_t to);


/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_task_switch  switch thread from thread from(sp) to to(sp).
 *         thread "to" context would be restored and to deploy and run.
 *
 * @param[in] from, the stack pointer of the task going to be swtch out.
 * @param[in] to, the stack pointer of the task going to be switch into.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_task_switch(uint32_t from, uint32_t to);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_task_switch_inirq try to switch thread on interrupt context.
 *
 * @param[in] from, the stack pointer of the task going to be swtch out.
 * @param[in] to, the stack pointer of the task going to be switch into.
 *
 * @attention This function did not do the actual switch operation on irq context
 *            just setup flags, and swith when going out of the irq context.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_task_switch_inirq(uint32_t from, uint32_t to);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_dbg_task_switch_to  for debug only, used to investigate speci-
 *         fy task status.
 *
 * @param[in] to, the stack pointer of task you want to switch into.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_dbg_task_switch_to(uint32_t to);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_test_clean_flush_cache, test, clean, and flush cache op.
 *  @attention  refer arm926ej-s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_test_clean_flush_cache(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_clean_dcache <clean dcache op.>
 *  @attention refer arm926ej=s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_clean_dcache(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_clean_flush_dcache <flush dcache op.>
 *  @attention refer arm926ej=s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_clean_flush_dcache(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_clean_flush_cache <clean flush cache>
 *  @attention refer arm926ej=s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_clean_flush_cache(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_flush_icache, flush i cache op.
 *  @attention refer arm926ej-s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_flush_icache(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_flush_dcache <flush_dcache op>
 *  @attention refer arm926ej-s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_flush_dcache(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_flush_cache <flush cache region>
 *  @attention refer arm926ej-s processor arch spec for detail.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_flush_cache(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_mems_flush_icache_region  flush icache data to physical
 * memory.
 *
 * @param[in] addr, start address of memory block.
 * @param[in] bytes, size of memory block need to be flushed, by BYTE.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_mems_flush_icache_region(void *addr, uint32_t bytes);

/* ----------------------------------------------------------------------------*/
/**
 * @brief awos_arch_mems_flush_dcache_region flush dcache data to physical memory.
 *
 * @param[in] addr, start address of memory block.
 * @param[in] bytes size of memory block need to be flushed, by BYTE.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_mems_flush_dcache_region(void *addr, uint32_t bytes);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_mems_flush_cache_region flush cache data to physical memory.
 *
 * @param[in] addr, start address of memory block.
 * @param[in] bytes, sz of memory block, by BYTE.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_mems_flush_cache_region(void *addr, uint32_t bytes);

/* ----------------------------------------------------------------------------*/
/**
 * @brief awos_arch_mems_clean_dcache_region clean dcache region. refer arm926ej-s
 *         arch spec.
 *
 * @param[in] addr, start address of memory block.
 * @param[in] bytes, size of memory block, by BYTE.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_mems_clean_dcache_region(void *addr, uint32_t bytes);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_mems_clean_flush_dcache_region clean, flush dcache region.
 *
 * @param[in] addr, start address of memory block need to be operated.
 * @param[in] bytes, size of memory block, by BYTE.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_mems_clean_flush_dcache_region(void *addr, uint32_t bytes);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_mems_clean_flush_cache_region  clean, flush cache region.
 *
 * @param[in] addr, start address of memory block need to be operated.
 * @param[in] bytes, size of memory block.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_mems_clean_flush_cache_region(void *addr, uint32_t bytes);
void cpu_arm926_cache_clean_invalidate_all(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_lock_tlb_zone lock the page table items in tle, not swap out.
 *
 * @param[in] addr, virtual address which you want to lock its page table entry.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_lock_tlb_zone(uint32_t addr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_chksched_start, get the status of operation system scheduler.
 *
 * @return
 *   -0: the scheduler has not been started.
 *   -1: the scheduler has been started.
 */
/* ----------------------------------------------------------------------------*/
uint8_t awos_arch_chksched_start(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_bus_to_virt  ioremap operation, Map io Registers to Virtual.
 *
 * @param[in] phyaddr, IO register address want mapped.
 *
 * @return the virtual address that can be accessed by software of IO registers.
 */
/* ----------------------------------------------------------------------------*/
uint32_t awos_arch_bus_to_virt(uint32_t phyaddr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_phys_to_virt map physical memory address to virtual address.
 *
 * @param[in] phyaddr, physcal memory address.
 *
 * @return  the virtual address of specify physical address.
 */
/* ----------------------------------------------------------------------------*/
uint32_t awos_arch_phys_to_virt(uint32_t phyaddr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_vmem_create  create virtual memory area through MMU.
 *
 * @param[in] virtaddr, the area start virtual address want create.
 * @param[in] npage, page number of the area, 4K.
 * @param[in] domain, previlege level of the area.
 *
 * @return
 *   -0: success.
 *   -1: failure.
 */
/* ----------------------------------------------------------------------------*/
int32_t awos_arch_vmem_create(uint8_t *virtaddr, uint32_t npage, uint8_t domain);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_vmem_delete destory the virtual area created.
 *
 * @param[in] virtaddr, start address of the virtual zone.
 * @param[in] npage, page number want to delete successive.
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_vmem_delete(uint8_t *virtaddr, uint32_t npage);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_vaddr_map2_phys  virtual to physical address convert, especi-
 *         ally for the DMA Controller and VE, GE etc.
 *
 * @param[in] vaddr, virtual address you want to convert.
 *
 * @return  the physical address of specify virtual address after mmu transition.
 */
/* ----------------------------------------------------------------------------*/
uint32_t awos_arch_vaddr_map2_phys(uint32_t vaddr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief awos_arch_set_fsce_id set the fast context extension id. refer arm926ej-s
 *         arch spec for detail.
 *
 * @param[in] fsceid id you want to set to CP15 id you want to set to CP15
 *
 * @return  the fsceid has been set.
 */
/* ----------------------------------------------------------------------------*/
uint8_t awos_arch_set_fsce_id(uint8_t fsceid);

/* ----------------------------------------------------------------------------*/
/** @brief  aw_hal_enable_vfp enable vfp(vector floating point, fpu) function.
 *  @attention not support SUN3IW2 based on arm926ej-s processor.
 */
/* ----------------------------------------------------------------------------*/
void aw_hal_enable_vfp(void);

/* ----------------------------------------------------------------------------*/
/** @brief  jtag_enable_op enable jtag pin into jtag mode.
 */
/* ----------------------------------------------------------------------------*/
void jtag_enable_op(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_stack_init This function will initialize thread stack.
 *
 * @param[in] tentry  the entry of thread
 * @param[in] parameter the parameter of entry
 * @param[in] stack_addr the beginning of the thread stack.
 * @param[in] texit the function will be called when thread entry return.
 *
 * @return  the stack pointer with the full context.
 */
/* ----------------------------------------------------------------------------*/
uint8_t *awos_arch_stack_init(void *tentry, void *parameter, uint8_t *stack_addr, \
                              void *texit);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_machine_shutdown shutdown processor. */
/* ----------------------------------------------------------------------------*/
static void awos_arch_machine_shutdown(void)
{
    awos_arch_disable_interrupt();

    while (1);
}

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_machine_reset reset processor */
/* ----------------------------------------------------------------------------*/
static void awos_arch_machine_reset(void)
{
    awos_arch_disable_interrupt();

    while (1);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_ffs This function finds the first bit set(beginning with the least
 *         significant bit) in value and reeturn the index of the bit.
 *
 * @param[in] value the value you want to operat __ffs on .
 *
 * @return
 *   -0: the argument was zero.
 *   -x: the first bit position starting from 1.
 */
/* ----------------------------------------------------------------------------*/
int32_t awos_arch_ffs(int32_t value);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_entry, IRQ routine. */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_entry(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_swi_func_error, wrong syscall report.
 *
 * @param[in] spsr
 * @param[in] cpsr
 * @param[in] swino
 * @param[in] swihander
 * @param[in] caller_lr
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_swi_func_error(uint32_t spsr, uint32_t cpsr, uint32_t swino, \
                              uint32_t swihander, uint32_t caller_lr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_swi_mode_error <wrong cpu mode happed>
 *
 * @param spsr
 * @param cpsr
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_swi_mode_error(uint32_t spsr, uint32_t cpsr);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_fiq_trap_entry <FIQ not support.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_fiq_trap_entry(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_rst_trap_entry <reset mode> */
/* ----------------------------------------------------------------------------*/
void awos_arch_rst_trap_entry(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_rsv_trap_entry <reserved exception, should never happend> */
/* ----------------------------------------------------------------------------*/
void awos_arch_rsv_trap_entry(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_enter < implementation by OS vendor.> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_enter(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_leave <implementation by OS vendor> */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_leave(void);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_tick_increase <++> */
/* ----------------------------------------------------------------------------*/
void awos_arch_tick_increase(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_isin_irq <++>
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
uint8_t awos_arch_isin_irq(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_arch_hal_uart_send <++>
 *
 * @param str
 */
/* ----------------------------------------------------------------------------*/
void awos_arch_hal_uart_send(const char *str);

/* ----------------------------------------------------------------------------*/
/**
 * @brief cpu_arm926_reset
 * Perform a soft reset of the system.  Put the CPU into the
 * same state as it would be if it had been reset, and branch
 * to what would be the reset vector.
 * @param loc reset location of startup.
 */
/* ----------------------------------------------------------------------------*/
void cpu_arm926_reset(uint32_t loc);
void cpu_arm926_tlb_invalidate_all(void);
void cpu_arm926_set_pgd(uint32_t pgd);
void cpu_arm926_proc_fin(void);
void cpu_arm926_dma_map_area(uint32_t start, uint32_t size);
void cpu_do_idle(void);

#define cpu_tlb_invalidate_all  cpu_arm926_tlb_invalidate_all

#define cpu_cache_clean_inv     cpu_arm926_cache_clean_invalidate_all

#define CPSR_ALLOC() uint32_t __cpsr
#define MELIS_CPU_CRITICAL_ENTER() { __cpsr = awos_arch_lock_irq(); }
#define MELIS_CPU_CRITICAL_LEAVE() { awos_arch_unlock_irq(__cpsr);  }

#endif /* __ARCH_ARM926__ */

