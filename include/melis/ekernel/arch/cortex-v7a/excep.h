/*
 * =====================================================================================
 *
 *       Filename:  excep.h
 *
 *    Description:  Exceptions context definition.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:16:16
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-26 11:15:01
 *
 * =====================================================================================
 */

#ifndef  __EXP926_INC__
#define  __EXP926_INC__

#include <stdint.h>
#include <list.h>

struct neon_vfp_regs
{
    // record cpuid.
    uint32_t cpu_id;

    /*
     * VFP implementation specific state
     */
    //uint64_t fpinst_fpinst2;
    uint64_t control_status;
    uint64_t DX[32];
} __attribute__((packed));
/* ----------------------------------------------------------------------------*/
/* @brief  <except_regs_t> the spot registers of exception happend. */
/* ----------------------------------------------------------------------------*/
typedef struct
{
    uint32_t fcsid;                     // 0x00
    uint32_t abt_dFAR;                  // 0x04
    uint32_t abt_iFSR;                  // 0x08
    uint32_t abt_dFSR;                  // 0x0c
    uint32_t abt_lr;                    // 0x10
    uint32_t orig_cpsr;                 // 0x14
    uint32_t lr;                        // 0x18
    uint32_t sp;                        // 0x1c
    uint32_t r0;                        // 0x20
    uint32_t r1;                        // 0x24
    uint32_t r2;                        // 0x28
    uint32_t r3;                        // 0x2c
    uint32_t r4;                        // 0x30
    uint32_t r5;                        // 0x34
    uint32_t r6;                        // 0x38
    uint32_t r7;                        // 0x3c
    uint32_t r8;                        // 0x40
    uint32_t r9;                        // 0x44
    uint32_t r10;                       // 0x48
    uint32_t r11;                       // 0x4c
    uint32_t r12;                       // 0x50
} __attribute__((packed)) excep_regs_t;

/* ----------------------------------------------------------------------------*/
/* @brief  <irq_regs_t> the spot registers of interrupt happend. */
/* ----------------------------------------------------------------------------*/
#ifdef CONFIG_NEST_INTERRUPT
typedef struct
{
    struct neon_vfp_regs neon_context;  //neon
    uint32_t spsr_irq;                  //0x00,
    uint32_t r00;                       //0x04,
    uint32_t r01;                       //0x08,
    uint32_t r02;                       //0x0c,
    uint32_t r03;                       //0x10,
    uint32_t r04;                       //0x14,
    uint32_t r05;                       //0x18,
    uint32_t r06;                       //0x1c,
    uint32_t r07;                       //0x20,
    uint32_t r08;                       //0x24,
    uint32_t r09;                       //0x28,
    uint32_t r10;                       //0x2c,
    uint32_t r11;                       //0x30,
    uint32_t r12;                       //0x34,
    uint32_t r13;                       //0x38
    uint32_t lr_svc;                    //0x3c,
    uint32_t lr_irq;                    //0x40, means pc
} __attribute__((packed)) irq_regs_neon_t;

typedef struct
{
    uint32_t spsr_irq;                  //0x00,
    uint32_t r00;                       //0x04,
    uint32_t r01;                       //0x08,
    uint32_t r02;                       //0x0c,
    uint32_t r03;                       //0x10,
    uint32_t r04;                       //0x14,
    uint32_t r05;                       //0x18,
    uint32_t r06;                       //0x1c,
    uint32_t r07;                       //0x20,
    uint32_t r08;                       //0x24,
    uint32_t r09;                       //0x28,
    uint32_t r10;                       //0x2c,
    uint32_t r11;                       //0x30,
    uint32_t r12;                       //0x34,
    uint32_t r13;                       //0x38
    uint32_t lr_svc;                    //0x3c,
    uint32_t lr_irq;                    //0x40, means pc
} __attribute__((packed)) irq_regs_no_neon_t;
#else
typedef struct
{
    struct neon_vfp_regs neon_context;  //neon
    uint32_t prev_sp;                   //0x00, interrupted thread sp. (r13)
    uint32_t prev_lr;                   //0x04, interrupted thread lr. (r14)
    uint32_t prev_spsr;                 //0x08, interrupted thread spsr.
    uint32_t prev_cpsr;                 //0x0c, interrupted thread cpsr.
    uint32_t r0;                        //0x10, ......
    uint32_t r1;                        //0x14, ......
    uint32_t r2;                        //0x18, ......
    uint32_t r3;                        //0x1c, ......
    uint32_t r4;                        //0x20, ......
    uint32_t r5;                        //0x24, ......
    uint32_t r6;                        //0x28, ......
    uint32_t r7;                        //0x2c, ......
    uint32_t r8;                        //0x30, ......
    uint32_t r9;                        //0x34, ......
    uint32_t r10;                       //0x38, ......
    uint32_t r11;                       //0x3c, ......
    uint32_t r12;                       //0x40, ......
    uint32_t retn_lr;                   //0x44, interrupted thread pc. (r15)
} __attribute__((packed)) irq_regs_neon_t;

typedef struct
{
    uint32_t prev_sp;                   //0x00, interrupted thread sp. (r13)
    uint32_t prev_lr;                   //0x04, interrupted thread lr. (r14)
    uint32_t prev_spsr;                 //0x08, interrupted thread spsr.
    uint32_t prev_cpsr;                 //0x0c, interrupted thread cpsr.
    uint32_t r0;                        //0x10, ......
    uint32_t r1;                        //0x14, ......
    uint32_t r2;                        //0x18, ......
    uint32_t r3;                        //0x1c, ......
    uint32_t r4;                        //0x20, ......
    uint32_t r5;                        //0x24, ......
    uint32_t r6;                        //0x28, ......
    uint32_t r7;                        //0x2c, ......
    uint32_t r8;                        //0x30, ......
    uint32_t r9;                        //0x34, ......
    uint32_t r10;                       //0x38, ......
    uint32_t r11;                       //0x3c, ......
    uint32_t r12;                       //0x40, ......
    uint32_t retn_lr;                   //0x44, interrupted thread pc. (r15)
} __attribute__((packed)) irq_regs_no_neon_t;
#endif

typedef struct
{
    uint32_t use_fpu;
    union
    {
       irq_regs_no_neon_t  no_neon;
       irq_regs_neon_t     have_neon;
    } context;
} __attribute__((packed)) irq_regs_t;


/* ----------------------------------------------------------------------------*/
/* @brief  <switch_ctx_regs_t> the spot registers of switch happend. */
/* ----------------------------------------------------------------------------*/

//uint8_t *awos_arch_stack_init(void *tentry, void *parameter, uint8_t *stack_addr, void *texit);
//must be identical match with awos_arch_stack_init(...) stack layout.
#ifndef CONFIG_CONSISTENT_CONTEXT_SWITCH
typedef struct
{
    uint32_t use_fpu;
    uint32_t fcse;
    uint32_t cpsr;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
} switch_ctx_regs_t;
#else
typedef struct
{
    struct neon_vfp_regs neon;
    uint32_t fcse;
    uint32_t cpsr;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
} __attribute__((packed)) switch_ctx_regs_with_neon_t;

typedef struct
{
    uint32_t fcse;
    uint32_t cpsr;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
} __attribute__((packed)) switch_ctx_regs_no_neon_t;

typedef struct
{
    uint32_t use_fpu;
    union 
    {
       switch_ctx_regs_no_neon_t   no_neon; 
       switch_ctx_regs_with_neon_t have_neon; 
    } context;
} __attribute__((packed)) switch_ctx_regs_t;

#endif

/* ----------------------------------------------------------------------------*/
/**
 * @brief  cpu_recover_from_exception <resume from the exception context.>
 *
 * @param excep_regs_t, cpu state to be recoverd.
 */
/* ----------------------------------------------------------------------------*/
void cpu_recover_from_exception(excep_regs_t *);

/*
 * All callee preserved registers:
 * v1 - v7, fp, ip, sp, lr, f4, f5, f6, f7
 */
#define _JBLEN 23

typedef struct _jmp_buf
{
    int _jb[_JBLEN + 1];
} jmp_buf[1];
//typedef int jmp_buf[_JBLEN];
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

/**
 * When ARM7TDMI comes across an instruction which it cannot handle,
 * it takes the undefined instruction trap.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void awos_arch_und_trap_entry(excep_regs_t *regs);

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during a data access.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void awos_arch_dabt_trap_entry(excep_regs_t *regs);

struct undef_hook
{
    struct list_head node;
    uint32_t instr_mask;
    uint32_t instr_val;
    uint32_t cpsr_mask;
    uint32_t cpsr_val;
    int (*fn)(excep_regs_t *regs);
};

struct data_abort_hook
{
    struct list_head node;
    uint32_t dfsr_mask;
    uint32_t dfsr_val;
    uint32_t dfar_mask;
    uint32_t dfar_val;
    uint32_t cpsr_mask;
    uint32_t cpsr_val;
    int (*fn)(excep_regs_t *regs);
};

struct instr_prefetch_abort_hook
{
    struct list_head node;
    uint32_t ifsr_mask;
    uint32_t ifsr_val;
    uint32_t ifar_mask;
    uint32_t ifar_val;
    uint32_t cpsr_mask;
    uint32_t cpsr_val;
    int (*fn)(excep_regs_t *regs);
};


void register_undef_hook(struct undef_hook *hook);
void unregister_undef_hook(struct undef_hook *hook);

void register_data_abort_hook(struct data_abort_hook *hook);
void unregister_data_abort_hook(struct data_abort_hook *hook);

void register_instr_prefetch_abort_hook(struct instr_prefetch_abort_hook *hook);
void unregister_instr_prefetch_abort_hook(struct instr_prefetch_abort_hook *hook);

/* ----------------------------------------------------------------------------*/
/** @brief  awos_arch_irq_trap_entry, IRQ routine. */
/* ----------------------------------------------------------------------------*/
void awos_arch_irq_trap_entry(irq_regs_t *regs);
/* ----------------------------------------------------------------------------*/

#endif
