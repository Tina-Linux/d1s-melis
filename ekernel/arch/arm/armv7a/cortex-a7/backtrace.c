/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
/*
 * The file is from Alios-Things, which is licensed under Apache License 2.0
 *
 * Modified:
 *     2020-06-01   Zeng Zhijin
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>

#include <rtdef.h>
#include <rtthread.h>
#include <excep.h>

#include <backtrace.h>

#include <hal_interrupt.h>
#include <hal_thread.h>

#if defined(__GNUC__)
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#error "Can not support big-endian!"
#endif
#endif

/* #define BACKTRACE_DEBUG 1 */

#ifndef BACKTRACE_DEBUG
#define backtrace_debug(fmt, ...)
#else
#define backtrace_debug(fmt, ...) \
    printf("[%s:%d]:"fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

#define BT_SCAN_MAX_LIMIT   0x2000
#define BT_LEVEL_LIMIT    64

#define ARM_STATE   0
#define THUMB_STATE 1

#define PC2ADDR(pc)          ((char *)(((uintptr_t)(pc)) & 0xfffffffe))

#define IS_VALID_TEXT_ADDRESS(pc) backtrace_check_address((unsigned long)pc)

#define IS_THUMB_ADDR(pc)     ((uintptr_t)(pc) & 0x1)
#define MAKE_THUMB_ADDR(pc)    do{(pc) = (void *)((uintptr_t)(pc) | 0x1);}while(0)

#define PRINT_CALL(print_func, fmt, ...) \
    if (print_func) { \
        print_func(fmt, ##__VA_ARGS__); \
    } \

typedef struct
{
    rt_thread_t task;
} tcb_shadow_t;

typedef struct
{
    switch_ctx_regs_t regs_ctx;
} switch_context_t;

enum task_states
{
    TASK_SUSPEND = 0,
    TASK_READY,
    TASK_INTERRUPTED,
    TASK_RUNNING,
};

extern char *_mmu_text_start;
extern char *_mmu_text_end;

extern void _interrupt_return_address(void);
extern void ret_from_create_c(void);

extern int32_t check_virtual_address(uint32_t vaddr);

static int32_t backtrace_check_address(unsigned long pc)
{
    return check_virtual_address((uint32_t)pc);
}

static int thumb_thumb32bit_code(uint16_t ic)
{
    uint16_t op = (ic >> 11) & 0xFFFF;
    if (op == 0x1D || op == 0x1E || op == 0x1F)
    {
        return 1;
    }
    return 0;
}

static void *get_task_stack_bottom(void)
{
    return &ret_from_create_c;
}

static void *find_task(char *name)
{
    return rt_thread_find(name);
}

static int check_task_is_running(void *task)
{
    rt_thread_t thread = (rt_thread_t)task;

    if (thread == NULL)
    {
        return TASK_SUSPEND;
    }

    if (thread == kthread_self() && hal_interrupt_get_nest() == 0)
    {
        return TASK_RUNNING;
    }
    else if (thread == kthread_self())
    {
        return TASK_INTERRUPTED;
    }
    else if (thread->stat == RT_THREAD_READY)
    {
        return TASK_READY;
    }
    else
    {
        return TASK_SUSPEND;
    }
}

/*
 * convert long to string
 */
static char *long2str(long num, char *str)
{
    char         index[] = "0123456789ABCDEF";
    unsigned long usnum   = (unsigned long)num;

    str[7] = index[usnum % 16];
    usnum /= 16;
    str[6] = index[usnum % 16];
    usnum /= 16;
    str[5] = index[usnum % 16];
    usnum /= 16;
    str[4] = index[usnum % 16];
    usnum /= 16;
    str[3] = index[usnum % 16];
    usnum /= 16;
    str[2] = index[usnum % 16];
    usnum /= 16;
    str[1] = index[usnum % 16];
    usnum /= 16;
    str[0] = index[usnum % 16];
    usnum /= 16;

    return str;
}

static void get_register_from_task_stack(void *context, char **PC, char **LR, int **SP)
{
    switch_context_t *task_ctx;

    task_ctx = (switch_context_t *)context;

    if (task_ctx->regs_ctx.use_fpu)
    {
        *PC = (char *)task_ctx->regs_ctx.context.have_neon.pc;
        *LR = (char *)task_ctx->regs_ctx.context.have_neon.lr;
        *SP = (int *)(task_ctx + 1);
        if (task_ctx->regs_ctx.context.have_neon.cpsr & 0x20)
        {
            MAKE_THUMB_ADDR(*PC);
        }
    }
    else
    {
        *PC = (char *)task_ctx->regs_ctx.context.no_neon.pc;
        *LR = (char *)task_ctx->regs_ctx.context.no_neon.lr;
        *SP = (int *)(task_ctx + 1);
        if (task_ctx->regs_ctx.context.no_neon.cpsr & 0x20)
        {
            MAKE_THUMB_ADDR(*PC);
        }
    }
}

static int find_lr_offset(char *LR, print_function print_func, bool *state)
{
    char *LR_fixed;
    unsigned short thumb_ins;
    char backtrace_output_buf[] = "backtrace : 0X         \r\n";
    int offset;

    if (*state == ARM_STATE)
    {
        offset = 4;
    }
    else if (*state == THUMB_STATE)
    {
        offset = 2;
    }

    LR_fixed = PC2ADDR(LR);

#ifndef CONFIG_ARCH_ARM_ARMV8M
    /* meet _interrupt_return_address, it is irq handler exit address. */
    if (LR_fixed == PC2ADDR(&_interrupt_return_address))
    {
        long2str((long)&_interrupt_return_address, &backtrace_output_buf[14]);
        PRINT_CALL(print_func, backtrace_output_buf);
        return 0;
    }
#endif
    /* meet rt_thread_exit, it is task exit function */
    if (LR_fixed == PC2ADDR(get_task_stack_bottom()))
    {
        long2str((long)get_task_stack_bottom(), &backtrace_output_buf[14]);
        PRINT_CALL(print_func, backtrace_output_buf);
        return 0;
    }

    if (IS_VALID_TEXT_ADDRESS(LR_fixed))
    {
        uint16_t ins16 = *(uint16_t *)(LR_fixed - 2);
        uint32_t ins32 = *(uint32_t *)(LR_fixed - 4);
        if ((ins16 & 0xFF80) == 0x4700)
        {
            /* bx <register>  */
            *state = !(*state);
            offset = 2;
        }
        else if ((ins16 & 0xFF80) == 0x4780)
        {
            /* blx <register>  */
            *state = !(*state);
            offset = 2;
        }
        else if ((ins32 & 0x0FFFFFF0) == 0x012FFF10)
        {
            /* bx <register>  */
            *state = !(*state);
            offset = 4;
        }
        else if ((ins32 & 0xFE000000) == 0xFA000000)
        {
            /* blx immed  */
            *state = !(*state);
            offset = 4;
        }
        else if ((ins32 & 0xF800D000) == 0xF000C000)
        {
            /* blx immed  */
            *state = !(*state);
            offset = 4;
        }
        else if (*state == THUMB_STATE)
        {
            ins16 = *(uint16_t *)(LR_fixed - 4);
            offset = thumb_thumb32bit_code(ins16) == 1 ? 4 : 2;
        }
    }

    long2str((long)LR_fixed - offset, &backtrace_output_buf[14]);
    PRINT_CALL(print_func, backtrace_output_buf);

    return offset;
}

static int arm_get_push_lr_ins_framesize(unsigned int inst, int *offset)
{
    int framesize = -1;
    if ((inst & 0xFFFF4000) == 0xE92D4000)
    {
        /* push {..., lr, ... }*/
        /* check whether push {...lr, pc}, if push pc, and offset should be 2. */
        *offset = (inst & 0x8000) == 0x8000 ? 2 : 1;
        framesize = __builtin_popcount(inst & 0xFFFF);
    }
    else if (inst == 0xE52DE004)
    {
        /* str lr [sp, #-4]! */
        *offset = 1;
        framesize = 1;
    }
#if 0
    else if ((inst & 0xFFFF0000) == 0xE92D0000)
    {
        /* push {.....}*/
        *offset = 0;
        framesize = __builtin_popcount(inst & 0xFFFF);
    }
#endif
    backtrace_debug("inst = 0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int arm_backtrace_return_pop(unsigned int inst)
{
    int framesize = -1;
    if ((inst & 0x0FFF8000) == 0x08BD8000)
    {
        /* pop {..., pc}  */
        framesize = __builtin_popcount(inst & 0xFFFF);
    }
    else if ((inst & 0x0FFFFFFF) == 0x012FFF1E)
    {
        /* bx lr */
        framesize = 0;
    }
    else if ((inst & 0x0FFFFFFF) == 0x049DF004)
    {
        /* ldr pc, [sp], #4 */
        framesize = 1;
    }
    backtrace_debug("inst = 0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int arm_backtrace_stack_push(unsigned int inst)
{
    unsigned int sub;
    unsigned int shift;
    int framesize = -1;

    if ((inst & 0x0FFFF000) == 0x024DD000)
    {
        /* sub sp, sp, #imm*/
        sub = inst & 0xFF;
        shift = (inst >> 8) & 0xF;
        if (shift != 0)
        {
            shift = 32 - 2 * shift;
            sub = sub << shift;
        }
        framesize = sub / 4;
    }
    else if ((inst & 0x0FFF0000) == 0x092D0000)
    {
        /* push {...} */
        framesize = __builtin_popcount(inst & 0xFFFF);
    }
    else if ((inst & 0x0FBF0F00) == 0x0D2D0B00)
    {
        /* vpush {...} */
        framesize = inst & 0xFF;
    }
    else if ((inst & 0xFFFF0FFF) == 0xE52D0004)
    {
        /* str xxx, [sp, #-4]! */
        framesize = 1;
    }
    backtrace_debug("inst = 0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int thumb_get_push_lr_ins_framesize(unsigned int inst, int *offset, int thumb32bit)
{
    int framesize = -1;
    if (thumb32bit)
    {
        if ((inst & 0xFFFFF000) == 0xe92d4000)
        {
            /* stmdb sp!, {...,lr} */
            framesize = __builtin_popcount(inst & 0xFFF);
            framesize++;
            *offset = 1;
        }
    }
    else
    {
        if ((inst & 0xFF00) == 0xB500)
        {
            /* push {..., lr, ... }*/
            framesize = __builtin_popcount(inst & 0xFF) + 1;
            *offset = 1;
        }
        else if ((inst & 0xFF00) == 0xB400)
        {
            /* push {... }*/
            framesize = __builtin_popcount(inst & 0xFF);
            *offset = 0;
        }

    }
    backtrace_debug("inst:0x%x, framesize = %d\n", inst, framesize);

    return framesize;
}

static int thumb_get_push_ins_framesize(unsigned int inst, unsigned char *jump, int thumb32bit)
{
    int framesize = -1;
    if (thumb32bit)
    {
        /* push {...} */
        if ((inst & 0xff00) == 0xb400)
        {
            framesize = __builtin_popcount((unsigned char)inst);
            *jump = 1;
        }
        /* sub sp, #immed  */
        else if ((inst & 0xff80) == 0xb080)
        {
            framesize = (inst & 0x7f);
        }

    }

    backtrace_debug("inst:0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int thumb_backtrace_stack_push(unsigned int inst, int thumb32bit)
{
    unsigned int sub;
    unsigned int shift;
    int framesize = -1;

    if (thumb32bit)
    {
        if ((inst & 0xFFFFF000) == 0xe92d4000)
        {
            /* stmdb sp!, {...} */
            framesize = __builtin_popcount(inst & 0xfff);
            framesize++;
        }
        else if ((inst & 0xFBFF8F00) == 0xF1AD0D00)
        {
            /* sub.w  sp, sp, #imm" */
            sub = 128 + (inst & 0x7f);
            shift  = (inst >> 7) & 0x1;
            shift += ((inst >> 12) & 0x7) << 1;
            shift += ((inst >> 26) & 0x1) << 4;
            framesize = sub << (30 - shift);
        }
        else if ((inst & 0xffbf0f00) == 0xed2d0b00)
        {
            /* vpush {...} 64-bit registers*/
            framesize = (inst & 0xff);
        }
        else if ((inst & 0xffbf0f00) == 0xed2d0a00)
        {
            /* vpush {...} 32-bit registers*/
            framesize = (inst & 0xff);
        }
    }
    else
    {
        if ((inst & 0xff00) == 0xb500)
        {
            /* push   {..., lr} */
            framesize = __builtin_popcount(inst & 0xff);
            framesize++;
        }
        else if ((inst & 0xff80) == 0xb080)
        {
            /* sub sp, sp, #imm */
            framesize = (inst & 0x7f);
        }
    }

    backtrace_debug("inst:0x%x, framesize = %d\n", inst, framesize);

    return framesize;
}

static int arm_backtrace_stack_pop(unsigned int inst)
{
    unsigned int add;
    unsigned int shift;
    int framesize = -1;

    if ((inst & 0x0FFFF000) == 0x028DD000)
    {
        /* add sp, sp, #imm */
        add = inst & 0xFF;
        shift = (inst >> 8) & 0xF;
        if (shift != 0)
        {
            shift = 32 - 2 * shift;
            add = add << shift;
        }
        framesize = add / 4;
    }
    else if ((inst & 0x0FFF0000) == 0x08BD0000)
    {
        /* pop {...} */
        framesize = __builtin_popcount(inst & 0xFFFF);
    }
    else if ((inst & 0x0FBF0F00) == 0x0CBD0B00)
    {
        /* vpop {...} */
        framesize = inst & 0xFF;
    }
    else if ((inst & 0x0FFF0FFF) == 0x049D0004)
    {
        /* ldr xxx, [sp], #4 */
        framesize = 1;
    }
    backtrace_debug("inst = 0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int arm_bakctrace_from_stack(int **pSP, char **pPC, char **pLR,
                                    print_function print_func)
{
    char *parse_addr = NULL;
    int  *SP = *pSP;
    char *PC = *pPC;
    char *LR = NULL;
    int i;
    int temp;
    int framesize;
    int offset = 0;
    unsigned int ins32;
    char backtrace_output_buf[] = "backtrace : 0X         \r\n";

    bool state = ARM_STATE;

    if (SP == get_task_stack_bottom())
    {
        long2str((long)get_task_stack_bottom(), &backtrace_output_buf[14]);
        PRINT_CALL(print_func, backtrace_output_buf);
        return 1;
    }

    for (i = 4; i < BT_SCAN_MAX_LIMIT; i += 4)
    {
        parse_addr = PC - i;
        if (IS_VALID_TEXT_ADDRESS(parse_addr) == 0)
        {
            PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
            return -1;
        }
        ins32 = *(long *)parse_addr;
        framesize = arm_get_push_lr_ins_framesize(ins32, &offset);
        if (framesize >= 0)
        {
            break;
        }
    }

    backtrace_debug("i = %d, parse_addr = %p, PC = %p, offset = %d, framesize = %d\n", i, parse_addr, PC, offset, framesize);

    if (i == BT_SCAN_MAX_LIMIT)
    {
        PRINT_CALL(print_func, "backtrace fail!\r\n");
        return -1;
    }

    for (i = 4; parse_addr + i < PC; i += 4)
    {
        if (IS_VALID_TEXT_ADDRESS(parse_addr + i) == 0)
        {
            PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
            return -1;
        }
        ins32 = *(unsigned long *)(parse_addr + i);
        temp = arm_backtrace_stack_push(ins32);
        if (temp >= 0)
        {
            framesize += temp;
        }
    }

    if (IS_VALID_TEXT_ADDRESS(parse_addr - 4))
    {
        ins32 = *(unsigned long *)(parse_addr - 4);
    }
    else
    {
        PRINT_CALL(print_func, "backtrace fail!\r\n");
        return -1;
    }

    temp = arm_backtrace_stack_push(ins32);
    if (temp >= 0)
    {
        framesize += temp;
        offset += temp;
    }

    if (offset == 0)
    {
        LR = *pLR;
    }

    if (LR == NULL)
    {
        if (IS_VALID_TEXT_ADDRESS(SP + framesize - offset))
        {
            LR  = (char *) * (SP + framesize - offset);
        }
        else
        {
            PRINT_CALL(print_func, "backtrace fail!\r\n");
            return -1;
        }
        return -1;
    }

    if (IS_VALID_TEXT_ADDRESS(LR) == 0)
    {
        PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
        return -1;
    }
    *pSP   = SP + framesize;
    offset = find_lr_offset(LR, print_func, &state);
    *pPC   = LR - offset;

    if (state == THUMB_STATE)
    {
        MAKE_THUMB_ADDR(*pPC);
    }

    backtrace_debug("*pSP = %p, offset = %d, *pPC = %p, framesize = %d, state = %d\n", *pSP, offset, *pPC, framesize, state);

    return offset == 0 ? 1 : 0;
}

static int arm_backtrace_from_lr(int **pSP, char **pPC, char *LR,
                                 print_function print_func)
{
    int *SP = *pSP;
    char *PC = *pPC;
    char *parse_addr = NULL;
    int i;
    int temp;
    int framesize = 0;
    int offset;
    unsigned int ins32;
    bool state = ARM_STATE;

    if (IS_VALID_TEXT_ADDRESS(PC) == 0)
    {
        if (IS_VALID_TEXT_ADDRESS(LR) == 0)
        {
            PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
            return -1;
        }
        offset = find_lr_offset(LR, print_func, &state);
        PC     = LR - offset;
        *pPC   = PC;
        return offset == 0 ? 1 : 0;
    }

    for (i = 0; i < BT_SCAN_MAX_LIMIT; i += 4)
    {
        parse_addr = PC + i;
        if (IS_VALID_TEXT_ADDRESS(parse_addr))
        {
            ins32 = *(unsigned long *)parse_addr;
        }
        else
        {
            PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
            return -1;
        }
        framesize = arm_backtrace_return_pop(ins32);
        if (framesize >= 0)
        {
            break;
        }

        temp = arm_get_push_lr_ins_framesize(ins32, &offset);
        if (temp >= 0)
        {
            framesize = 0;
            break;
        }
    }

    if (i == BT_SCAN_MAX_LIMIT)
    {
        PRINT_CALL(print_func, "backtrace fail!\r\n");
        return -1;
    }

    for (i = 4; parse_addr - i >= PC; i += 4)
    {
        if (IS_VALID_TEXT_ADDRESS(parse_addr - i))
        {
            ins32 = *(unsigned long *)(parse_addr - i);
        }
        else
        {
            PRINT_CALL(print_func, "backtrace fail!\r\n");
            return -1;
        }
        temp = arm_backtrace_stack_pop(ins32);
        if (temp >= 0)
        {
            framesize += temp;
        }
    }

    if (IS_VALID_TEXT_ADDRESS(LR) == 0)
    {
        PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
        return -1;
    }
    *pSP   = SP + framesize;
    offset = find_lr_offset(LR, print_func, &state);
    *pPC   = LR - offset;

    if (state == THUMB_STATE)
    {
        MAKE_THUMB_ADDR(*pPC);
    }

    backtrace_debug("*pSP = %p, offset = %d, *pPC = %p, framesize = %d, state = %d\n", *pSP, offset, *pPC, framesize, state);

    return offset == 0 ? 1 : 0;
}

int thumb_get_next_inst(print_function print_func, int *error, int *offset, char *ins16_h_addr, char *ins16_l_addr, int lsb, int *thumb32bit)
{
    unsigned int ins32 = 0;
    unsigned short ins16_l = 0;
    unsigned short ins16_h = 0;

    if (IS_VALID_TEXT_ADDRESS(ins16_l_addr) == 0)
    {
        PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
        *error = -1;
        return -1;
    }
    ins16_l = *(unsigned short *)(ins16_l_addr);

    if (IS_VALID_TEXT_ADDRESS(ins16_h_addr) == 0)
    {
        PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
        *error = -1;
        return -1;
    }
    ins16_h = *(unsigned short *)(ins16_h_addr);

    if (thumb_thumb32bit_code(ins16_l))
    {
        /* thumb-2 thumb32bit code: [low address code : high address code]  */
        ins32 = (ins16_l << 16) | ins16_h;
        *offset += 2;
        *thumb32bit = 1;
    }
    else
    {
        *thumb32bit = 0;
        if (lsb)
        {
            ins32 = ins16_l;
        }
        else
        {
            ins32 = ins16_h;
        }
    }
    return ins32;
}

static int thumb_backtrace_from_stack(int **pSP, char **pPC, char **pLR,
                                      print_function print_func)
{
    char *parse_addr = NULL;
    int *SP = *pSP;
    char *PC = PC2ADDR(*pPC);
    char *LR = NULL;
    int i;
    int temp;
    unsigned int ins32 = 0;
    unsigned int shift = 0;
    unsigned int sub = 0;
    unsigned int offset    = 1;
    unsigned short ins16   = 0;
    unsigned short ins16_h = 0;
    unsigned short ins16_l = 0;

    int framesize = 0;
    bool state = THUMB_STATE;
    char backtrace_output_buf[] = "backtrace : 0X         \r\n";

    if (SP == get_task_stack_bottom())
    {
        long2str((long)get_task_stack_bottom(), &backtrace_output_buf[14]);
        PRINT_CALL(print_func, backtrace_output_buf);
        return 1;
    }

    for (i = 2; i < BT_SCAN_MAX_LIMIT; i += 2)
    {
        parse_addr = PC - i;
        int error = 0;
        int thumb32bit = 0;
        ins32 = thumb_get_next_inst(print_func, &error, &i, parse_addr, parse_addr - 2, 0, &thumb32bit);
        if (error)
        {
            return -1;
        }
        framesize = thumb_get_push_lr_ins_framesize(ins32, &offset, thumb32bit);
        if (framesize >= 0)
        {
            break;
        }
    }

    backtrace_debug("i = %d, parse_addr = %p, PC = %p, offset = %d, framesize = %d\n", i, parse_addr, PC, offset, framesize);

    if (i == BT_SCAN_MAX_LIMIT)
    {
        PRINT_CALL(print_func, "backtrace fail!\r\n");
        return -1;
    }

    for (i = 2; parse_addr + i <= PC; i += 2)
    {
        int error = 0;
        int thumb32bit = 0;
        ins32 = thumb_get_next_inst(print_func, &error, &i, parse_addr + i + 2, parse_addr + i, 1, &thumb32bit);
        if (error)
        {
            return -1;
        }
        temp = thumb_backtrace_stack_push(ins32, thumb32bit);
        if (temp >= 0)
        {
            framesize += temp;
        }
    }

    backtrace_debug("i = %d, framesize = %d, SP = %p, offset = %d\n", i, framesize, SP, offset);
    if (offset == 0)
    {
        LR = *pLR;
    }

    if (!LR)
    {
        if (IS_VALID_TEXT_ADDRESS(SP + framesize - offset) == 0)
        {
            PRINT_CALL(print_func, "backtrace fail!\r\n");
            return -1;
        }
        LR = (char *) * (SP + framesize - offset);
    }

    if (IS_VALID_TEXT_ADDRESS(LR) == 0)
    {
        PRINT_CALL(print_func, "backtrace fail!\r\n");
        return -1;
    }
    *pSP   = SP + framesize;
    offset = find_lr_offset(LR, print_func, &state);
    *pPC   = LR - offset;

    if (state == THUMB_STATE)
    {
        MAKE_THUMB_ADDR(*pPC);
    }

    backtrace_debug("*pSP = %p, offset = %d, *pPC = %p, state=%d\n", *pSP, offset, *pPC, state);

    return offset == 0 ? 1 : 0;
}

static int thumb_backtrace_return_pop(unsigned int inst, int thumb32bit)
{
    int framesize = -1;
#if 0
    if ((inst & 0xff00) == 0xb500)
    {
        framesize = 0;
    }
    if ((inst & 0xff00) == 0xb400)
    {
        framesize = __builtin_popcount((unsigned char)inst);
    }
    if ((inst & 0xff80) == 0xb080)
    {
        framesize = (inst & 0x7f);
        if (((inst >> 16) & 0xff00) == 0xb400)
        {
            framesize += __builtin_popcount((unsigned char)(inst >> 16));
        }
    }
#else
    if (!thumb32bit)
    {
        /* bx lr */
        if ((inst & 0xFFFF) == 0x4770)
        {
            return 0;
        }
    }
#endif
    backtrace_debug("inst = 0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int thumb_backtrace_stack_pop(unsigned int inst, unsigned char *sp_change, int thumb32bit)
{
    int framesize = -1;
    if (!thumb32bit)
    {
        if ((inst & 0xff00) == 0xbc00)
        {
            /* find "pop   {...}" */
            framesize = __builtin_popcount((unsigned char)inst);
            *sp_change = 1;
        }
    }
    else
    {
        if ((inst & 0xffbf0f00) == 0xecbd0b00)
        {
            /* vpop {...} */
            framesize = inst & 0xFF;
            *sp_change = 1;
        }
    }
    backtrace_debug("inst = 0x%x, framesize = %d\n", inst, framesize);
    return framesize;
}

static int thumb_backtrace_from_lr(int **pSP, char **pPC, char *LR,
                                   print_function print_func)
{
    int *SP = *pSP;
    char *PC = PC2ADDR(*pPC);
    char *parse_addr = NULL;
    int i;
    unsigned int   ins32 = 0;
    unsigned short ins16 = 0;
    unsigned short ins16_h = 0;
    unsigned short ins16_l = 0;

    unsigned int framesize = 0;
    unsigned int offset = 0;
    int temp = 0;
    unsigned char sp_change = 0;
    unsigned char jump = 0;
    bool state = THUMB_STATE;

    if (IS_VALID_TEXT_ADDRESS(PC) == 0)
    {
        if (IS_VALID_TEXT_ADDRESS(LR) == 0)
        {
            PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
            return -1;
        }
        offset = find_lr_offset(LR, print_func, &state);
        PC     = LR - offset;
        *pPC   = PC;
        return offset == 0 ? 1 : 0;
    }

    for (i = 2; i < BT_SCAN_MAX_LIMIT; i += 2)
    {
        parse_addr = PC + i;
        int error = 0;
        int thumb32bit = 0;
        ins32 = thumb_get_next_inst(print_func, &error, &i, PC + i + 2, PC + i, 1, &thumb32bit);
        if (error)
        {
            return -1;
        }
        backtrace_debug("parse_addr = 0x%x, i = %d\n", parse_addr, i);
        temp = thumb_backtrace_stack_pop(ins32, &sp_change, thumb32bit);
        if (temp >= 0)
        {
            //framesize += temp;
            continue;
        }
        temp = thumb_backtrace_return_pop(ins32, thumb32bit);
        if (temp >= 0)
        {
            framesize += temp;
            break;
        }
    }

    if (i == BT_SCAN_MAX_LIMIT)
    {
        PRINT_CALL(print_func, "backtrace fail!\r\n");
        return -1;
    }

    backtrace_debug("parse_addr = 0x%08x, framesize = %d, sp_change = %d\n", parse_addr, framesize, sp_change);
    if (sp_change)
    {
        for (i = 2; i < BT_SCAN_MAX_LIMIT; i += 2)
        {
            parse_addr = PC - i;
            int error = 0;
            int thumb32bit = 0;
            ins32 = thumb_get_next_inst(print_func, &error, &i, parse_addr, parse_addr - 2, 0, &thumb32bit);
            if (error)
            {
                return -1;
            }
            temp = thumb_get_push_ins_framesize(ins32, &jump, thumb32bit);
            if (temp >= 0)
            {
                framesize += temp;
            }
            if (jump > 0)
            {
                break;
            }
        }
    }

    if (IS_VALID_TEXT_ADDRESS(LR) == 0)
    {
        PRINT_CALL(print_func, "backtrace : invalid lr\r\n");
        return -1;
    }
    *pSP   = SP + framesize;
    offset = find_lr_offset(LR, print_func, &state);
    *pPC   = LR - offset;

    if (state == THUMB_STATE)
    {
        MAKE_THUMB_ADDR(*pPC);
    }

    backtrace_debug("*pSP = %p, offset = %d, *pPC = %p, framesize = %d, state=%d\n", *pSP, offset, *pPC, framesize, state);

    return offset == 0 ? 1 : 0;
}

static int backtrace_from_stack(int **pSP, char **pPC, char **pLR,
                                print_function print_func)
{
    if (IS_VALID_TEXT_ADDRESS(*pPC) == 0)
    {
        return -1;
    }

    if (IS_THUMB_ADDR(*pPC))
    {
        return thumb_backtrace_from_stack(pSP, pPC, pLR, print_func);
    }
    else
    {
        return arm_bakctrace_from_stack(pSP, pPC, pLR, print_func);
    }
}

static int backtrace_from_lr(int **pSP, char **pPC, char *LR,
                             print_function print_func)
{
    if (IS_THUMB_ADDR(*pPC))
    {
        return thumb_backtrace_from_lr(pSP, pPC, LR, print_func);
    }
    else
    {
        return arm_backtrace_from_lr(pSP, pPC, LR, print_func);
    }
}

/*
 * _backtrace function
 * @taskname: the task need to get call stack
 * @output:the call stack output buffer
 * @size:  the size of output
 * @offset:
 * @print_func: print function
 * @arg_cpsr: cpsr register value when use in exception mode
 * @arg_sp: sp register value when use in exception mode
 * @arg_pc: pc register value when use in exception mode
 * @arg_lr: lr register value when use in exception mode
 * @exception_mode: 0/1; 0:normal mode, 1:exception mode
 */
static int _backtrace(char *taskname, void *output[], int size, int offset, print_function print_func,
                      unsigned long arg_cpsr,
                      unsigned long arg_sp,
                      unsigned long arg_pc,
                      unsigned long arg_lr,
                      unsigned long exception_mode)
{
    char *PC = NULL;
    int  *SP = NULL;
    char *saved_pc;
    int  *saved_sp;
    int   CPSR = 0;
    int   level;
    int   ret;
    tcb_shadow_t *task = NULL;
    char    *LR = NULL;
    int check_self = 0;
    char backtrace_output_buf[] = "backtrace : 0X         \r\n";

    if (output && size > 0)
    {
        memset(output, 0, size * sizeof(void *));
    }

    if (taskname)
    {
        task = find_task(taskname);
        if (task == NULL)
        {
            print_func("Task not found : %s\n", taskname);
            return 0;
        }
        get_register_from_task_stack(((rt_thread_t)task)->sp, &PC, &LR, &SP);
        if (check_task_is_running(task) == TASK_RUNNING ||
            check_task_is_running(task) == TASK_INTERRUPTED)
        {
            check_self = 1;
        }
    }

    if (taskname == NULL || check_self == 1)
    {
        __asm__ volatile("mov %0, sp\n" : "=r"(SP));
        __asm__ volatile("mov %0, pc\n" : "=r"(PC));
        __asm__ volatile("mov %0, lr\n" : "=r"(LR));
        __asm__ volatile("mrs %0, CPSR\n" : "=r"(CPSR));

        /* CPSR Thumb state bit. bit[5] */
        if (CPSR & 0x20)
        {
            MAKE_THUMB_ADDR(PC);
        }
        check_self = 1;
    }

    if (SP == NULL)
    {
        return 0;
    }

    if (exception_mode == 1)
    {
        LR = (char *)arg_lr;
        SP = (int *)arg_sp;
        PC = (char *)arg_pc;
#ifdef CONFIG_ARCH_ARM_ARMV8M
        CPSR = (int)arg_cpsr;
        MAKE_THUMB_ADDR(PC);
#else
        CPSR = (int)arg_cpsr;
        /* CPSR Thumb state bit. bit[5] */
        if (CPSR & 0x20)
        {
            MAKE_THUMB_ADDR(PC);
        }
#endif
    }

    ret = -1;
    level = 0;
    if (output)
    {
        if (level >= offset && level - offset < size)
        {
            output[level - offset] = PC;
        }
        if (level - offset >= size)
        {
            goto out;
        }
    }

    long2str((long)PC, &backtrace_output_buf[14]);
    PRINT_CALL(print_func, backtrace_output_buf);
    char *save_pc = PC;
    int *save_sp = SP;
    char *save_lr = LR;

    for (level = 1; level < BT_LEVEL_LIMIT; level++)
    {
        ret = backtrace_from_stack(&SP, &PC, &LR, print_func);
        if (ret != 0)
        {
            break;
        }
        if (output)
        {
            if (level >= offset && level - offset < size)
            {
                output[level - offset] = PC;
            }
            if (level - offset >= size)
            {
                break;
            }
        }
    }

    if ((check_self == 0 || exception_mode == 1) && level == 1)
    {
        PC = save_pc;
        SP = save_sp;
        LR = save_lr;
        ret = backtrace_from_lr(&SP, &PC, LR, print_func);
        if (ret == 0)
        {
            for (; level < BT_LEVEL_LIMIT; level++)
            {
                ret = backtrace_from_stack(&SP, &PC, &LR, print_func);
                if (ret != 0)
                {
                    break;
                }
                if (output)
                {
                    if (level >= offset && level - offset < size)
                    {
                        output[level - offset] = PC;
                    }
                    if (level - offset >= size)
                    {
                        break;
                    }
                }
            }
        }
    }
out:
    return level - offset < 0 ? 0 : level - offset;
}

int arch_backtrace(char *taskname, void *trace[], int size, int offset, print_function print_func)
{
    return _backtrace(taskname, trace, size, offset, print_func, 0, 0, 0, 0, 0);
}

int arch_backtrace_exception(print_function print_func,
                             unsigned long arg_cpsr,
                             unsigned long arg_sp,
                             unsigned long arg_pc,
                             unsigned long arg_lr)
{
    return _backtrace(NULL, NULL, 0, 0, print_func, arg_cpsr, arg_sp, arg_pc, arg_lr, 1);
}
