/*
 * ARM GDB support
 * arch-specific portion of GDB stub
 *
 * File      : arm_stub.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-07-04     Wzyy2      first version
 * 2019-12-23     ZengZhijin refator to support hw breakpoint and watchpoint
 */
#include <gdb_stub.h>
#include <excep.h>
#include <arch.h>
#include <breakpoint.h>
#include <hardware_wbp.h>
#include <kgdb_util.h>

#define PS_N 0x80000000
#define PS_Z 0x40000000
#define PS_C 0x20000000
#define PS_V 0x10000000

#define IS_THUMB_ADDR(addr)     ((addr) & 1)
#define MAKE_THUMB_ADDR(addr)   ((addr) | 1)
#define UNMAKE_THUMB_ADDR(addr) ((addr) & ~1)

int compiled_break = 0;
static unsigned long step_addr = 0;
static int arm_ins_will_execute(unsigned long ins);
static int thumb_ins_will_execute(unsigned long ins);
static unsigned long arm_target_ins(unsigned long *pc, unsigned long ins);
static unsigned long thumb_target_ins(unsigned short *pc, unsigned short ins);
static int thumb_32bit_code(uint16_t ic);

extern unsigned long armv_swc_handler;
struct gdb_regs_t *regs = NULL;

int arm_kgdb_arch_init(void);
int arm_kgdb_arch_exit(void);

/**
 * gdb_breakpoint - generate a compiled_breadk
 * It is used to sync up with a debugger and stop progarm
 */
void arm_gdb_compiled_breakpoint(void)
{
    asm(".word 0xe7ffdeff");
}

void gdb_set_register(void *hw_regs)
{
    regs = (struct gdb_regs_t *)hw_regs;
}

void gdb_get_register(unsigned long *gdb_regs)
{
    int regno;
    for (regno = 0; regno < GDB_MAX_REGS; regno++)
    {
        gdb_regs[regno] = 0;
    }

    gdb_regs[GDB_R0]        = regs->regs_t.r0;
    gdb_regs[GDB_R1]        = regs->regs_t.r1;
    gdb_regs[GDB_R2]        = regs->regs_t.r2;
    gdb_regs[GDB_R3]        = regs->regs_t.r3;
    gdb_regs[GDB_R4]        = regs->regs_t.r4;
    gdb_regs[GDB_R5]        = regs->regs_t.r5;
    gdb_regs[GDB_R6]        = regs->regs_t.r6;
    gdb_regs[GDB_R7]        = regs->regs_t.r7;
    gdb_regs[GDB_R8]        = regs->regs_t.r8;
    gdb_regs[GDB_R9]        = regs->regs_t.r9;
    gdb_regs[GDB_R10]       = regs->regs_t.r10;
    gdb_regs[GDB_FP]        = regs->regs_t.r11;
    gdb_regs[GDB_IP]        = regs->regs_t.r12;
    gdb_regs[GDB_SP]       = regs->regs_t.sp;
    gdb_regs[GDB_LR]        = regs->regs_t.lr;
    gdb_regs[GDB_PC]        = regs->regs_t.abt_lr;
    gdb_regs[GDB_CPSR]      = regs->regs_t.orig_cpsr;

#if 0
    if (melis_arch_use_fpu)
    {
        int i;
        for (i = 0; i < GDB_DNUM; i++)
        {
            if (i < 16)
            {
                gdb_regs[GDB_D0 + i] = regs->regs_t.neon.DX[i + 16];
            }
            else
            {
                gdb_regs[GDB_D0 + i] = regs->regs_t.neon.DX[i - 16];
            }
        }
        gdb_regs[GDB_FPSCR] = (uint32_t)(regs->regs_t.neon.control_status & 0xFFFFFFFF);
        gdb_regs[GDB_FPEXC] = (uint32_t)(regs->regs_t.neon.control_status >> 32) & 0xFFFFFFFF;
    }
#endif
};

void gdb_put_register(unsigned long *gdb_regs)
{
    regs->regs_t.r0             = gdb_regs[GDB_R0];
    regs->regs_t.r1             = gdb_regs[GDB_R1];
    regs->regs_t.r2             = gdb_regs[GDB_R2];
    regs->regs_t.r3             = gdb_regs[GDB_R3];
    regs->regs_t.r4             = gdb_regs[GDB_R4];
    regs->regs_t.r5             = gdb_regs[GDB_R5];
    regs->regs_t.r6             = gdb_regs[GDB_R6];
    regs->regs_t.r7             = gdb_regs[GDB_R7];
    regs->regs_t.r8             = gdb_regs[GDB_R8];
    regs->regs_t.r9             = gdb_regs[GDB_R9];
    regs->regs_t.r10            = gdb_regs[GDB_R10];
    regs->regs_t.r11            = gdb_regs[GDB_FP];
    regs->regs_t.r12            = gdb_regs[GDB_IP];
    regs->regs_t.sp             = gdb_regs[GDB_SP];
    regs->regs_t.lr             = gdb_regs[GDB_LR];
    regs->regs_t.abt_lr         = gdb_regs[GDB_PC];
    regs->regs_t.orig_cpsr      = gdb_regs[GDB_CPSR];

#if 0
    if (melis_arch_use_fpu)
    {
        int i;
        for (i = 0; i < GDB_DNUM; i++)
        {
            if (i < 16)
            {
                regs->regs_t.neon.DX[i + 16] = gdb_regs[GDB_D0 + i];
            }
            else
            {
                regs->regs_t.neon.DX[i - 16] = gdb_regs[GDB_D0 + i];
            }
        }
        regs->regs_t.neon.control_status |= (gdb_regs[GDB_FPSCR] & 0xFFFFFFFF);
        regs->regs_t.neon.control_status |= ((gdb_regs[GDB_FPEXC] << 32) & 0xFFFFFFFF);
    }
#endif
}

int gdb_arch_handle_exception(char *remcom_in_buffer, char *remcom_out_buffer)
{
    unsigned long addr, curins;
    unsigned long nextins;
    int ins_len = 4;
    char *ptr;

    /*clear single step*/
    if (step_addr)
    {
        gdb_remove_sw_break(step_addr, 0);
        gdb_remove_hw_break(step_addr);
        gdb_remove_hw_watch(step_addr);
        step_addr = 0;
    }

    switch (remcom_in_buffer[0])
    {
        case 'D':
        case 'k':
        case 'c':
            /*
             * If this was a compiled breakpoint, we need to move
             * to the next instruction or we will breakpoint
             * over and over again
             */
            ptr = &remcom_in_buffer[1];
            if (gdb_hex2long(&ptr, &addr))
            {
                regs->regs_t.abt_lr = addr;
            }
            else if (compiled_break == 1)
            {
                regs->regs_t.abt_lr += 4;
            }
            compiled_break = 0;
            return 0;

        case 's':
            ptr = &remcom_in_buffer[1];
            if (gdb_hex2long(&ptr, &addr))
            {
                regs->regs_t.abt_lr = addr;
            }

            if (regs->regs_t.orig_cpsr & 0x20)
            {
                uint16_t testins;
                testins = (*(unsigned short *)(regs->regs_t.abt_lr)) & 0xffff;
                if (thumb_32bit_code(testins) == 1)
                {
                    step_addr = regs->regs_t.abt_lr + 4;
                    if (thumb_32bit_code((*(unsigned short *)(step_addr)) & 0xffff) != 1)
                    {
                        ins_len = 2;
                        step_addr = MAKE_THUMB_ADDR(step_addr);
                    }
                    goto set_sw_break;
                }
            }

            if (regs->regs_t.orig_cpsr & 0x20)
            {
                curins = (*(unsigned short *)(regs->regs_t.abt_lr)) & 0xffff;
                /* Decode instruction to decide what the next pc will be */
                if (thumb_ins_will_execute(curins))
                {
                    step_addr = thumb_target_ins((unsigned short *)regs->regs_t.abt_lr, (unsigned short)curins);
                }
                else
                {
                    step_addr = MAKE_THUMB_ADDR(regs->regs_t.abt_lr + 2);
                }

                if (IS_THUMB_ADDR(step_addr))
                {
                    nextins = (*(unsigned short *)(UNMAKE_THUMB_ADDR(step_addr))) & 0xffff;
                }
                else
                {
                    nextins = (*(unsigned short *)(step_addr)) & 0xffff;
                }

                ins_len = 2;
                if (thumb_32bit_code(nextins) == 1)
                {
                    ins_len = 4;
                    step_addr = UNMAKE_THUMB_ADDR(step_addr);
                }
            }
            else
            {
                curins = *(unsigned long *)(regs->regs_t.abt_lr);
                /* Decode instruction to decide what the next pc will be */
                if (arm_ins_will_execute(curins))
                {
                    step_addr = arm_target_ins((unsigned long *)regs->regs_t.abt_lr, curins);
                }
                else
                {
                    step_addr = regs->regs_t.abt_lr + 4;
                }
                if (IS_THUMB_ADDR(step_addr))
                {
                    ins_len = 2;
                }
            }

#ifdef KGDB_DEBUG
            printf("\n next will be %x \n", step_addr);
#endif
set_sw_break:
            gdb_set_sw_break(step_addr, ins_len);

            if (compiled_break == 1)
            {
                regs->regs_t.abt_lr += 4;
            }
            compiled_break = 0;
            return 0;
    }

    return -1;
}

/* flush icache to let the sw breakpoint working */
void gdb_flush_icache_range(unsigned long start, unsigned long end)
{
    void awos_arch_flush_icache_all(void);
    awos_arch_flush_icache_all();
}

static unsigned long gdb_arch_regs[GDB_MAX_REGS];
static int arm_ins_will_execute(unsigned long ins)
{
    int res = 0;
    unsigned long psr = regs->regs_t.orig_cpsr;
    /* condition codes */
    switch ((ins & 0xF0000000) >> 28)
    {
        case 0x0: /* EQ */
            res = (psr & PS_Z) != 0;
            break;
        case 0x1: /* NE */
            res = (psr & PS_Z) == 0;
            break;
        case 0x2: /* CS */
            res = (psr & PS_C) != 0;
            break;
        case 0x3: /* CC */
            res = (psr & PS_C) == 0;
            break;
        case 0x4: /* MI */
            res = (psr & PS_N) != 0;
            break;
        case 0x5: /* PL */
            res = (psr & PS_N) == 0;
            break;
        case 0x6: /* VS */
            res = (psr & PS_V) != 0;
            break;
        case 0x7: /* VC */
            res = (psr & PS_V) == 0;
            break;
        case 0x8: /* HI */
            res = ((psr & PS_C) != 0) && ((psr & PS_Z) == 0);
            break;
        case 0x9: /* LS */
            res = ((psr & PS_C) == 0) || ((psr & PS_Z) != 0);
            break;
        case 0xA: /* GE */
            res = ((psr & (PS_N | PS_V)) == (PS_N | PS_V)) ||
                  ((psr & (PS_N | PS_V)) == 0);
            break;
        case 0xB: /* LT */
            res = ((psr & (PS_N | PS_V)) == PS_N) ||
                  ((psr & (PS_N | PS_V)) == PS_V);
            break;
        case 0xC: /* GT */
            res = ((psr & (PS_N | PS_V)) == (PS_N | PS_V)) ||
                  ((psr & (PS_N | PS_V)) == 0);
            res = ((psr & PS_Z) == 0) && res;
            break;
        case 0xD: /* LE */
            res = ((psr & (PS_N | PS_V)) == PS_N) ||
                  ((psr & (PS_N | PS_V)) == PS_V);
            res = ((psr & PS_Z) == PS_Z) || res;
            break;
        case 0xE: /* AL */
            res = 1;
            break;
        case 0xF: /* NV */
            if ((((ins & 0x0E000000) >> 24) == 0xA) || (((ins & 0x0E000000) >> 24) == 0xB))
            {
                res = 1;
            }
            else
            {
                res = 0;
            }
            break;
    }
    return res;
}

static int thumb_ins_will_execute(unsigned long ins)
{
    int res = 0;
    unsigned long psr = regs->regs_t.orig_cpsr;

    if (((ins & 0xF000) >> 12) != 0xD)
    {
        return 1;
    }
    /* condition codes */
    switch ((ins & 0x0F00) >> 8)
    {
        case 0x0: /* EQ */
            res = (psr & PS_Z) != 0;
            break;
        case 0x1: /* NE */
            res = (psr & PS_Z) == 0;
            break;
        case 0x2: /* CS */
            res = (psr & PS_C) != 0;
            break;
        case 0x3: /* CC */
            res = (psr & PS_C) == 0;
            break;
        case 0x4: /* MI */
            res = (psr & PS_N) != 0;
            break;
        case 0x5: /* PL */
            res = (psr & PS_N) == 0;
            break;
        case 0x6: /* VS */
            res = (psr & PS_V) != 0;
            break;
        case 0x7: /* VC */
            res = (psr & PS_V) == 0;
            break;
        case 0x8: /* HI */
            res = ((psr & PS_C) != 0) && ((psr & PS_Z) == 0);
            break;
        case 0x9: /* LS */
            res = ((psr & PS_C) == 0) || ((psr & PS_Z) != 0);
            break;
        case 0xA: /* GE */
            res = ((psr & (PS_N | PS_V)) == (PS_N | PS_V)) ||
                  ((psr & (PS_N | PS_V)) == 0);
            break;
        case 0xB: /* LT */
            res = ((psr & (PS_N | PS_V)) == PS_N) ||
                  ((psr & (PS_N | PS_V)) == PS_V);
            break;
        case 0xC: /* GT */
            res = ((psr & (PS_N | PS_V)) == (PS_N | PS_V)) ||
                  ((psr & (PS_N | PS_V)) == 0);
            res = ((psr & PS_Z) == 0) && res;
            break;
        case 0xD: /* LE */
            res = ((psr & (PS_N | PS_V)) == PS_N) ||
                  ((psr & (PS_N | PS_V)) == PS_V);
            res = ((psr & PS_Z) == PS_Z) || res;
            break;
        case 0xE: /* AL */
            res = 1;
            break;
        case 0xF: /* NV */
            /* TODO what is NV condition for thumb state. */
            res = 1;
            break;
    }
    return res;
}

static unsigned long RmShifted(int shift)
{
    unsigned long Rm = gdb_arch_regs[shift & 0x00F];
    int shift_count;
    if ((shift & 0x010) == 0)
    {
        shift_count = (shift & 0xF80) >> 7;
    }
    else
    {
        shift_count = gdb_arch_regs[(shift & 0xF00) >> 8];
    }
    switch ((shift & 0x060) >> 5)
    {
        case 0x0: /* Logical left */
            Rm <<= shift_count;
            break;
        case 0x1: /* Logical right */
            Rm >>= shift_count;
            break;
        case 0x2: /* Arithmetic right */
            Rm = (unsigned long)((long)Rm >> shift_count);
            break;
        case 0x3: /* Rotate right */
            if (shift_count == 0)
            {
                /* Special case, RORx */
                Rm >>= 1;
                if (gdb_arch_regs[GDB_CPSR] & PS_C)
                {
                    Rm |= 0x80000000;
                }
            }
            else
            {
                Rm = (Rm >> shift_count) | (Rm << (32 - shift_count));
            }
            break;
    }
    return Rm;
}
/*
 * @param pc: the current program counter pointer
 * @param ins: the instruction value
 * Decide the next instruction to be executed for a given instruction
 */
unsigned long thumb_decoder(unsigned short *pc, unsigned short ins);
unsigned long thumb_get_next_pc(unsigned short *pc, unsigned short ins);
static unsigned long thumb_target_ins(unsigned short *pc, unsigned short ins)
{
    return thumb_get_next_pc(pc, ins);
}

/*
 * @param pc: the current program counter pointer
 * @param ins: the instruction value
 * Decide the next instruction to be executed for a given instruction
 */
static unsigned long arm_target_ins(unsigned long *pc, unsigned long ins)
{
    unsigned long new_pc, offset, op2;
    unsigned long Rn;
    int i, reg_count, c;

    gdb_get_register(gdb_arch_regs);

    switch ((ins & 0x0C000000) >> 26)
    {
        case 0x0:
            /* BX or BLX */
            if ((ins & 0x0FFFFFD0) == 0x012FFF10)
            {
                new_pc = (unsigned long)gdb_arch_regs[ins & 0x0000000F];
                return new_pc;
            }
            /* Data processing */
            new_pc = (unsigned long)(pc + 1);
            if ((ins & 0x0000F000) == 0x0000F000)
            {
                /* Destination register is PC */
                if ((ins & 0x0FBF0000) != 0x010F0000)
                {
                    Rn = (unsigned long)gdb_arch_regs[(ins & 0x000F0000) >> 16];
                    if ((ins & 0x000F0000) == 0x000F0000)
                    {
                        Rn += 8;    /* PC prefetch! */
                    }
                    if ((ins & 0x02000000) == 0)
                    {
                        op2 = RmShifted(ins & 0x00000FFF);
                    }
                    else
                    {
                        op2 = ins & 0x000000FF;
                        i = (ins & 0x00000F00) >> 8;  /* Rotate count */
                        op2 = (op2 >> (i * 2)) | (op2 << (32 - (i * 2)));
                    }
                    switch ((ins & 0x01E00000) >> 21)
                    {
                        case 0x0: /* AND */
                            new_pc = Rn & op2;
                            break;
                        case 0x1: /* EOR */
                            new_pc = Rn ^ op2;
                            break;
                        case 0x2: /* SUB */
                            new_pc = Rn - op2;
                            break;
                        case 0x3: /* RSB */
                            new_pc = op2 - Rn;
                            break;
                        case 0x4: /* ADD */
                            new_pc = Rn + op2;
                            break;
                        case 0x5: /* ADC */
                            c = (gdb_arch_regs[GDB_CPSR] & PS_C) != 0;
                            new_pc = Rn + op2 + c;
                            break;
                        case 0x6: /* SBC */
                            c = (gdb_arch_regs[GDB_CPSR] & PS_C) != 0;
                            new_pc = Rn - op2 + c - 1;
                            break;
                        case 0x7: // RSC
                            c = (gdb_arch_regs[GDB_CPSR] & PS_C) != 0;
                            new_pc = op2 - Rn + c - 1;
                            break;
                        case 0x8: /* TST */
                        case 0x9: /* TEQ */
                        case 0xA: /* CMP */
                        case 0xB: /* CMN */
                            break; /* PC doesn't change */
                        case 0xC: /* ORR */
                            new_pc = Rn | op2;
                            break;
                        case 0xD: /* MOV */
                            new_pc = op2;
                            break;
                        case 0xE: /* BIC */
                            new_pc = Rn & ~op2;
                            break;
                        case 0xF: /* MVN */
                            new_pc = ~op2;
                            break;
                    }
                }
            }
            return new_pc;
        case 0x1:
            if ((ins & 0x02000010) == 0x02000010)
            {
                /* Undefined! */
                return (unsigned long)(pc + 1);
            }
            else
            {
                if ((ins & 0x00100000) == 0)
                {
                    /* STR */
                    return (unsigned long)(pc + 1);
                }
                else
                {
                    /* LDR */
                    if ((ins & 0x0000F000) != 0x0000F000)
                    {
                        /* Rd not PC */
                        return (unsigned long)(pc + 1);
                    }
                    else
                    {
                        Rn = (unsigned long)gdb_arch_regs[(ins & 0x000F0000) >> 16];
                        if ((ins & 0x000F0000) == 0x000F0000)
                        {
                            Rn += 8;    /* PC prefetch! */
                        }
                        if (ins & 0x01000000)
                        {
                            /* Add/subtract offset before */
                            if ((ins & 0x02000000) == 0)
                            {
                                /* Immediate offset */
                                if (ins & 0x00800000)
                                {
                                    /* Add offset */
                                    Rn += (ins & 0x00000FFF);
                                }
                                else
                                {
                                    /* Subtract offset */
                                    Rn -= (ins & 0x00000FFF);
                                }
                            }
                            else
                            {
                                /* Offset is in a register */
                                if (ins & 0x00800000)
                                {
                                    /* Add offset */
                                    Rn += RmShifted(ins & 0x00000FFF);
                                }
                                else
                                {
                                    /* Subtract offset */
                                    Rn -= RmShifted(ins & 0x00000FFF);
                                }
                            }
                        }
                        return *(unsigned long *)Rn;
                    }
                }
            }
            return (unsigned long)(pc + 1);
        case 0x2:  /* Branch, LDM/STM */
            if ((ins & 0x02000000) == 0)
            {
                /* LDM/STM */
                if ((ins & 0x00100000) == 0)
                {
                    /* STM */
                    return (unsigned long)(pc + 1);
                }
                else
                {
                    /* LDM */
                    if ((ins & 0x00008000) == 0)
                    {
                        /* PC not in list */
                        return (unsigned long)(pc + 1);
                    }
                    else
                    {
                        Rn = (unsigned long)gdb_arch_regs[(ins & 0x000F0000) >> 16];
                        if ((ins & 0x000F0000) == 0x000F0000)
                        {
                            Rn += 8;    /* PC prefetch! */
                        }
                        offset = ins & 0x0000FFFF;
                        reg_count = 0;
                        for (i = 0;  i < 15;  i++)
                        {
                            if (offset & (1 << i))
                            {
                                reg_count++;
                            }
                        }
                        if (ins & 0x00800000)
                        {
                            /* Add offset */
                            Rn += reg_count * 4;
                        }
                        else
                        {
                            /* Subtract offset */
                            Rn -= 4;
                        }
                        return *(unsigned long *)Rn;
                    }
                }
            }
            else
            {
                /* Branch */
                if (arm_ins_will_execute(ins))
                {
                    offset = (ins & 0x00FFFFFF) << 2;
                    if (ins & 0x00800000)
                    {
                        offset |= 0xFC000000;    /* sign extend */
                    }
                    new_pc = (unsigned long)(pc + 2) + offset;
                    /* If its BLX, make new_pc a thumb address. */
                    if ((ins & 0xFE000000) == 0xFA000000)
                    {
                        if ((ins & 0x01000000) == 0x01000000)
                        {
                            /* new_pc |= 2; */
                        }
                        {
                            unsigned short nextins = (*(unsigned short *)new_pc) & 0xffff;
                            if (!thumb_32bit_code(nextins))
                            {
                                new_pc = MAKE_THUMB_ADDR(new_pc);
                            }
                        }
                    }
                    return new_pc;
                }
                else
                {
                    /* Falls through */
                    return (unsigned long)(pc + 1);
                }
            }
        case 0x3:  /* Coprocessor & SWI */
            if (((ins & 0x03000000) == 0x03000000) && arm_ins_will_execute(ins))
            {
                /* SWI */
                /* TODO(wzyy2) some problems.*/
                extern unsigned long armv_swc_handler;
                return armv_swc_handler;
            }
            else
            {
                return (unsigned long)(pc + 1);
            }
        default:
            /* Never reached - but fixes compiler warning.*/
            return 0;
    }
}

int arm_arch_set_hw_watchpoint(enum gdb_bptype type, int i, unsigned long addr)
{
    return arm_install_hw_watchpoint(type, i, addr);
}

int arm_arch_remove_hw_watchpoint(enum gdb_bptype type, int i, unsigned long addr)
{
    arm_uninstall_hw_watchpoint(i);
    return 0;
}

int arm_arch_set_hw_breakpoint(int i, unsigned long addr)
{
    return arm_install_hw_breakpoint(i, addr);
}

int arm_arch_remove_hw_breakpoint(int i, unsigned long addr)
{
    arm_uninstall_hw_breakpoint(i);
    return 0;
}

int arm_arch_set_sw_breakpoint(unsigned long addr, int ins_len, char *saved_instr)
{
    unsigned long fixed_addr;
    int err;
    int break_instr_size = BREAK_INSTR_SIZE;

    if (IS_THUMB_ADDR(addr))
    {
        fixed_addr = UNMAKE_THUMB_ADDR(addr);
        break_instr_size = 2;
    }
    else
    {
        fixed_addr = addr;
    }

    if (ins_len != 0)
    {
        break_instr_size = ins_len;
    }

    err = probe_kernel_write((void *)saved_instr, (void *)fixed_addr, break_instr_size);
    if (err)
    {
        return err;
    }

    return probe_kernel_write((void *)fixed_addr,
                              (void *)arch_gdb_ops.gdb_bpt_instr,
                              break_instr_size);
}

int arm_arch_remove_sw_breakpoint(unsigned long addr, char *bundle)
{
    unsigned long fixed_addr;
    int err;
    int break_instr_size = BREAK_INSTR_SIZE;

    if (IS_THUMB_ADDR(addr))
    {
        fixed_addr = UNMAKE_THUMB_ADDR(addr);
        break_instr_size = 2;
    }
    else
    {
        fixed_addr = addr;
    }

    return probe_kernel_write((void *)fixed_addr, (void *)bundle, break_instr_size);
}

int arm_check_data_access(unsigned long addr)
{
    int32_t check_virtual_address(uint32_t vaddr);
    return check_virtual_address(addr);
}

struct gdb_arch arch_gdb_ops =
{
    .gdb_bpt_instr      =  {0xfe, 0xde, 0xff, 0xe7},  //Little-Endian, defined as GDB_BREAKINST 0xe7ffdefe
    .kgdb_init = arm_kgdb_arch_init,
    .kgdb_exit = arm_kgdb_arch_exit,
    .set_compiled_breakpoint = arm_gdb_compiled_breakpoint,
    .check_data_access = arm_check_data_access,

    .set_sw_breakpoint = arm_arch_set_sw_breakpoint,
    .remove_sw_breakpoint = arm_arch_remove_sw_breakpoint,
    .set_hw_breakpoint = arm_arch_set_hw_breakpoint,
    .remove_hw_breakpoint = arm_arch_remove_hw_breakpoint,
    .set_hw_watchpoint = arm_arch_set_hw_watchpoint,
    .remove_hw_watchpoint = arm_arch_remove_hw_watchpoint,
};

#define BITS_SHIFT(x, high, low) (((x) >> (low)) & ((1<<((high)-(low)+1))-1))
#define BITS(x, high, low) ((x) & (((1<<((high)-(low)+1))-1) << (low)))
#define BITS_SHIFT(x, high, low) (((x) >> (low)) & ((1<<((high)-(low)+1))-1))
#define SIGN_EXTEND(val, topbit) (BIT(val, topbit) ? ((val) | (0xffffffff << (topbit))) : (val))
/* #define SIGN_EXTEND(val, topbit) (ASR_SIMPLE(LSL(val, 32-(topbit)), 32-(topbit))) */
#define BIT(x, bit) ((x) & (1 << (bit)))
#define BIT(x, bit) ((x) & (1 << (bit)))
#define submask(x) ((1L << ((x) + 1)) - 1)
#define SBITS(obj,st,fn) \
    ((long) (BITS(obj,st,fn) | ((long) BIT(obj,st) * ~ submask (st - fn))))


#define LSL(val, shift) (((shift) >= 32) ? 0 : ((val) << (shift)))

#define ASR_SIMPLE(val, shift) (((int)(val)) >> (shift))

static int thumb_32bit_code(uint16_t ic)
{
    uint16_t op = (ic >> 11) & 0xFFFF;
    if (op == 0x1D || op == 0x1E || op == 0x1F)
    {
        return 1;
    }
    return 0;
}

static int bitcount(unsigned long val)
{
    int nbits;
    for (nbits = 0; val != 0; nbits++)
    {
        val &= val - 1;    /* Delete rightmost 1-bit in val.  */
    }
    return nbits;
}

unsigned long read_mem_uint(unsigned long memaddr, int len)
{
    unsigned long *addr = (unsigned long *)(memaddr);
    return *addr;
}

#define ARM_INT_REGISTER_SIZE 4

unsigned long thumb_get_next_pc(unsigned short *pc, unsigned short ins)
{
    int Rm;
    int immed;
    unsigned long nextpc;
    gdb_get_register(gdb_arch_regs);
    unsigned short inst1 = ins;

    unsigned long pc_val = ((unsigned long) pc) + 4;  /* PC after prefetch */
    pc_val = MAKE_THUMB_ADDR(pc_val);

    if ((ins & 0xff00) == 0xbd00)   /* pop {rlist, pc} */
    {
        unsigned long sp;

        /* Fetch the saved PC from the stack.  It's stored above
           all of the other registers.  */
        unsigned long offset = bitcount(BITS(inst1, 7, 0)) * ARM_INT_REGISTER_SIZE;
        sp = gdb_arch_regs[GDB_SP];
        nextpc = read_mem_uint(sp + offset, 4);
        nextpc = MAKE_THUMB_ADDR(nextpc);
        return nextpc;
    }
    else if ((inst1 & 0xff87) == 0x4687)  /* mov pc, REG */
    {
        if (BITS(inst1, 6, 3) == 0x0f)
        {
            nextpc = pc_val;
        }
        else
        {
            nextpc = gdb_arch_regs[BITS(inst1, 6, 3)];
        }
        nextpc = MAKE_THUMB_ADDR(nextpc);
        return nextpc;
    }
    else if ((inst1 & 0xf500) == 0xb100)
    {
        /* CBNZ or CBZ.  */
        int imm = (BIT(inst1, 9) << 6) + (BITS(inst1, 7, 3) << 1);
        unsigned long reg = gdb_arch_regs[BITS(inst1, 3, 0)];

        if (BIT(inst1, 11) && reg != 0)
        {
            nextpc = pc_val + imm;
        }
        else if (!BIT(inst1, 11) && reg == 0)
        {
            nextpc = pc_val + imm;
        }
    }

    switch (BITS_SHIFT(ins, 15, 11))
    {
        case 0x00:
        case 0x01:
        case 0x02:
            // shift by immediate
            return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
        case 0x03:
            // add/subtract 3 register
            return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            // add/subtract/compare/move immediate
            return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
        case 0x08:
            if (BIT(ins, 10))
            {
                if (BITS_SHIFT(ins, 9, 8) == 3)
                {
                    Rm = BITS_SHIFT(ins, 6, 3);
                    return (unsigned long)gdb_arch_regs[Rm];
                }
                else
                {
                    /*TODO */
                }
            }
            else
            {
                /*TODO */
            }
            break;
        case 0x09:
            break;
        case 0x0a:
        case 0x0b:
            break;
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
            break;
        case 0x10:
        case 0x11:
            break;
        case 0x12:
        case 0x13:
            break;
        case 0x14:
        case 0x15:
            break;
        case 0x16:
        case 0x17:
            break;
        case 0x18:
        case 0x19:
            break;
        case 0x1a:
        case 0x1b:
            switch (BITS_SHIFT(ins, 11, 8))
            {
                default:
                    immed = BITS(ins, 7, 0);
                    immed <<= 1;
                    immed = SIGN_EXTEND(immed, 8);
                    immed += 4;
                    return MAKE_THUMB_ADDR((unsigned long)gdb_arch_regs[GDB_PC] + immed);
                case 0xe:
                    break;
                case 0xf:
                    nextpc = armv_swc_handler;
                    break;
            }
            break;
        case 0x1c:
            // unconditional branch
            immed = BITS(ins, 10, 0);
            immed <<= 1;
            immed = SIGN_EXTEND(immed, 11);
            immed += 4;
            return MAKE_THUMB_ADDR((unsigned long)gdb_arch_regs[GDB_PC] + immed);
        case 0x1d:
            if (ins & 1)
            {
                // undefined instruction
            }
            else
            {
                // blx suffix
            }
            break;
        case 0x1e:
        case 0x1f:
            // bl/blx prefix
            break;
    }
    return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
}

unsigned long thumbv2_get_next_pc(unsigned short *pc_p, unsigned short ins)
{
    int Rm;
    int immed;
    unsigned long nextpc;
    unsigned short inst1 = ins;
    unsigned short inst2;
    unsigned long pc = (unsigned long)pc_p;

    gdb_get_register(gdb_arch_regs);

    unsigned long pc_val = ((unsigned long) pc) + 4;  /* PC after prefetch */
    pc_val = MAKE_THUMB_ADDR(pc_val);

    inst2 = read_mem_uint(((unsigned long) pc) + 2, 2) & 0xffff;

    /* Default to the next instruction.  */
    nextpc = pc + 4;
    nextpc = MAKE_THUMB_ADDR(nextpc);

    if ((inst1 & 0xf800) == 0xf000 && (inst2 & 0x8000) == 0x8000)
    {
        /* Branches and miscellaneous control instructions.  */

        if ((inst2 & 0x1000) != 0 || (inst2 & 0xd001) == 0xc000)
        {
            /* B, BL, BLX.  */
            int j1, j2, imm1, imm2;

            imm1 = SBITS(inst1, 10, 0);
            imm2 = BITS(inst2, 10, 0);
            j1 = BIT(inst2, 13);
            j2 = BIT(inst2, 11);

            unsigned long offset = ((imm1 << 12) + (imm2 << 1));
            offset ^= ((!j2) << 22) | ((!j1) << 23);

            nextpc = pc_val + offset;
            /* For BLX make sure to clear the low bits.  */
            if (BIT(inst2, 12) == 0)
            {
                nextpc = nextpc & 0xfffffffc;
            }
        }
        else if (inst1 == 0xf3de && (inst2 & 0xff00) == 0x3f00)
        {
            /* SUBS PC, LR, #imm8.  */
            nextpc = gdb_arch_regs[GDB_LR];
            nextpc -= inst2 & 0x00ff;
        }
        else if ((inst2 & 0xd000) == 0x8000 && (inst1 & 0x0380) != 0x0380)
        {
            /* Conditional branch.  */
            if (thumb_ins_will_execute(inst1))
            {
                int sign, j1, j2, imm1, imm2;

                sign = SBITS(inst1, 10, 10);
                imm1 = BITS(inst1, 5, 0);
                imm2 = BITS(inst2, 10, 0);
                j1 = BIT(inst2, 13);
                j2 = BIT(inst2, 11);

                unsigned long offset
                    = (sign << 20) + (j2 << 19) + (j1 << 18);
                offset += (imm1 << 12) + (imm2 << 1);

                nextpc = pc_val + offset;
            }
        }
    }
    else if ((inst1 & 0xfe50) == 0xe810)
    {
        /* Load multiple or RFE.  */
        int rn, offset, load_pc = 1;

        rn = BITS(inst1, 3, 0);
        if (BIT(inst1, 7) && !BIT(inst1, 8))
        {
            /* LDMIA or POP */
            if (!BIT(inst2, 15))
            {
                load_pc = 0;
            }
            offset = bitcount(inst2) * 4 - 4;
        }
        else if (!BIT(inst1, 7) && BIT(inst1, 8))
        {
            /* LDMDB */
            if (!BIT(inst2, 15))
            {
                load_pc = 0;
            }
            offset = -4;
        }
        else if (BIT(inst1, 7) && BIT(inst1, 8))
        {
            /* RFEIA */
            offset = 0;
        }
        else if (!BIT(inst1, 7) && !BIT(inst1, 8))
        {
            /* RFEDB */
            offset = -8;
        }
        else
        {
            load_pc = 0;
        }

        if (load_pc)
        {
            unsigned long addr = gdb_arch_regs[rn];
            nextpc = read_mem_uint(addr + offset, 4);
        }
    }
    else if ((inst1 & 0xffef) == 0xea4f && (inst2 & 0xfff0) == 0x0f00)
    {
        /* MOV PC or MOVS PC.  */
        nextpc = gdb_arch_regs[BITS(inst2, 3, 0)];
        nextpc = MAKE_THUMB_ADDR(nextpc);
    }
    else if ((inst1 & 0xff70) == 0xf850 && (inst2 & 0xf000) == 0xf000)
    {
        /* LDR PC.  */
        unsigned long base;
        int rn, load_pc = 1;

        rn = BITS(inst1, 3, 0);
        base = gdb_arch_regs[rn];
        if (rn == GDB_PC)
        {
            base = (base + 4) & ~(unsigned long) 0x3;
            if (BIT(inst1, 7))
            {
                base += BITS(inst2, 11, 0);
            }
            else
            {
                base -= BITS(inst2, 11, 0);
            }
        }
        else if (BIT(inst1, 7))
        {
            base += BITS(inst2, 11, 0);
        }
        else if (BIT(inst2, 11))
        {
            if (BIT(inst2, 10))
            {
                if (BIT(inst2, 9))
                {
                    base += BITS(inst2, 7, 0);
                }
                else
                {
                    base -= BITS(inst2, 7, 0);
                }
            }
        }
        else if ((inst2 & 0x0fc0) == 0x0000)
        {
            int shift = BITS(inst2, 5, 4), rm = BITS(inst2, 3, 0);
            base += gdb_arch_regs[rm] << shift;
        }
        else
            /* Reserved.  */
        {
            load_pc = 0;
        }

        if (load_pc)
        {
            nextpc = read_mem_uint(base, 4);
        }
    }
    else if ((inst1 & 0xfff0) == 0xe8d0 && (inst2 & 0xfff0) == 0xf000)
    {
        /* TBB.  */
        unsigned long tbl_reg, table, offset, length;

        tbl_reg = BITS(inst1, 3, 0);
        if (tbl_reg == 0x0f)
        {
            table = pc + 4;    /* Regcache copy of PC isn't right yet.  */
        }
        else
        {
            table = gdb_arch_regs[tbl_reg];
        }

        offset = gdb_arch_regs[BITS(inst2, 3, 0)];
        length = read_mem_uint(table + offset, 1) & 0xf;
        nextpc = pc_val + length;
    }
    else if ((inst1 & 0xfff0) == 0xe8d0 && (inst2 & 0xfff0) == 0xf010)
    {
        /* TBH.  */
        unsigned long tbl_reg, table, offset, length;

        tbl_reg = BITS(inst1, 3, 0);
        if (tbl_reg == 0x0f)
        {
            table = pc + 4;    /* Regcache copy of PC isn't right yet.  */
        }
        else
        {
            table = gdb_arch_regs[tbl_reg];
        }

        offset = 2 * gdb_arch_regs[BITS(inst2, 3, 0)];
        length = (2 * read_mem_uint(table + offset, 2)) & 0xff;
        nextpc = pc_val + length;
    }
    return nextpc;
}

unsigned long thumb_decoder(unsigned short *pc, unsigned short ins)
{
    int Rm;
    int immed;
    gdb_get_register(gdb_arch_regs);
    switch (BITS_SHIFT(ins, 15, 11))
    {
        case 0x00:
        case 0x01:
        case 0x02:
            // shift by immediate
            return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
        case 0x03:
            // add/subtract 3 register
            return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            // add/subtract/compare/move immediate
            return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
        case 0x08:
            if (BIT(ins, 10))
            {
                if (BITS_SHIFT(ins, 9, 8) == 3)
                {
                    Rm = BITS_SHIFT(ins, 6, 3);
                    return (unsigned long)gdb_arch_regs[Rm];
                }
                else
                {
                    /*TODO */
                }
            }
            else
            {
                /*TODO */
            }
            break;
        case 0x09:
            break;
        case 0x0a:
        case 0x0b:
            break;
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
            break;
        case 0x10:
        case 0x11:
            break;
        case 0x12:
        case 0x13:
            break;
        case 0x14:
        case 0x15:
            break;
        case 0x16:
        case 0x17:
            break;
        case 0x18:
        case 0x19:
            break;
        case 0x1a:
        case 0x1b:
            switch (BITS_SHIFT(ins, 11, 8))
            {
                default:
                    immed = BITS(ins, 7, 0);
                    immed <<= 1;
                    immed = SIGN_EXTEND(immed, 8);
                    immed += 4;
                    return MAKE_THUMB_ADDR((unsigned long)gdb_arch_regs[GDB_PC] + immed);
                case 0xe:
                    break;
                case 0xf:
                    break;
            }
            break;
        case 0x1c:
            // unconditional branch
            immed = BITS(ins, 10, 0);
            immed <<= 1;
            immed = SIGN_EXTEND(immed, 11);
            immed += 4;
            return MAKE_THUMB_ADDR((unsigned long)gdb_arch_regs[GDB_PC] + immed);
        case 0x1d:
            if (ins & 1)
            {
                // undefined instruction
            }
            else
            {
                // blx suffix
            }
            break;
        case 0x1e:
        case 0x1f:
            // bl/blx prefix
            break;
    }
    return MAKE_THUMB_ADDR((unsigned long)(pc + 1));
}
