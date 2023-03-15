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
/** WARNING: to use call trace facilities, enable
 *  compiler's frame pointer feature:
 *  -fno-omit-frame-pointer
 */

/* function call stack graph
 *
 *                                   low addr
 *                                  |___..___|
 *                               ---|___fp___|<--- previous fp
 * func_A stack start -->        |  |___lr___|<--- current fp
 *                               |  |        |
 *                               |  |   ..   |
 *                               |  |________|
 *                             --|--|___fp___|
 * func_B stack start -->      | -->|___lr___|
 *                             |    |        |
 *                             |    |        |
 *                             |    |___..___|
 *                           --|----|___fp___|
 * func_C stack start -->    |  --->|___lr___|
 *                           |      |        |
 *                           |       high addr
 *                           ------>
 *
 *                   func_C () {
 *                      ...
 *                      func_B();
 *                      ...
 *                   }
 *
 *                   func_B() {
 *                      ...
 *                      func_A();
 *                      ...
 *                   }
 *
 *static void backtrace_stack(int (*print_func)(const char *fmt, ...), unsigned long *fp)
 *{
 *    unsigned long *lr;
 *    unsigned long *pc;
 *
 *    while (1) {
 *    ¦   lr = (unsigned long *)(*fp);
 *    ¦   pc = lr - 1;
 *
 *    ¦   if (lr == (unsigned long *)rt_thread_exit) {
 *    ¦   ¦   printk("backtrace: reached task root!!!\r\n");
 *    ¦   ¦   break;
 *    ¦   }
 *
 *    ¦   printk("backtrace: %p\r\n", pc);
 *
 *    ¦   fp = (unsigned long *)*(fp - 1);
 *    }
 *}
 */

#include <log.h>

void rt_thread_exit(void);
void __backtrace(void);

void dump_backtrace_entry(unsigned long where, unsigned long from, unsigned long frame)
{
    printk("Function entered at [<0x%08x>] from [<0x%08x>]\r\n", where, from);
}

int dump_printk(const char *fmt, ...)
{
    return 0;
}


void dump_stack(void)
{
#ifdef CONFIG_USE_DUMP_STACK
    __log("dump_stack enter");
    __backtrace();
    __log("dump_stack exit");
#else
    __log("not support dump stack!");
#endif
}

static void dump_memory(unsigned int *buf, int len)
{
    int i;

    printk("tack memory:");
    for (i = 0; i < len; i ++)
    {
        if ((i % 4) == 0)
        {
            printk("\n\r0x%p: ", buf + i);
        }
        printk("0x%08x ", buf[i]);
    }
    printk("\n\r");
    return;
}


#define CONFIG_DEBUG_TRACE 0

#define ROR(x,y)        ((x >> y) | (x << (32 - y)))

unsigned int Read32(unsigned int *pos)
{
    return *pos;
}
/*
 * parse operation code
 * params:
 * @pc: pc register value pointer
 * @sp: sp register value pointer
 * return: get lr register or not
 */
int parse(unsigned int *pc, unsigned int *sp)
{
    unsigned int i;
    unsigned int lr_flag = 0;
    unsigned int opcode = 0;
    int num = 0;

    /* Read opcode */
    opcode = *pc;

    /* Registers */
    unsigned int Rn    = ((opcode >> 16) & 0xF);
    unsigned int Rd    = ((opcode >> 12) & 0xF);
    unsigned int Rm    = ((opcode >> 0) & 0xF);
    unsigned int Rs    = ((opcode >> 8) & 0xF);
    unsigned int Imm   = ((opcode >> 0) & 0xFF);
    unsigned int amt   = Rs << 1;

    /* Flags */
    int I = (opcode >> 25) & 1;
    int P = (opcode >> 24) & 1;
    int U = (opcode >> 23) & 1;
    int B = (opcode >> 22) & 1;
    int W = (opcode >> 21) & 1;
    int S = (opcode >> 20) & 1;
    int L = (opcode >> 20) & 1;


    switch ((opcode >> 25) & 7)
    {
        case 4:
        {
            // LDM/STM
            int pf    = 0;
#if CONFIG_DEBUG_TRACE
            printk("0x%x: [0x%0x]    #", pc, *pc);
            if (L)
            {
                printk("ldm");
                if (Rn == 13)
                {
                    printk("%c%c", (P) ? 'e' : 'f', (U) ? 'd' : 'a');
                }
                else
                {
                    printk("%c%c", (U) ? 'i' : 'd', (P) ? 'b' : 'a');
                }
            }
            else
            {
                printk("stm");
                if (Rn == 13)
                {
                    printk("%c%c", (P) ? 'f' : 'e', (U) ? 'a' : 'd');
                }
                else
                {
                    printk("%c%c", (U) ? 'i' : 'd', (P) ? 'b' : 'a');
                }
            }

            if (Rn == 13)
            {
                printk(" sp");
            }
            else
            {
                printk(" r%d", Rn);
            }

            if (W)
            {
                printk("!");
            }

            printk(", {");
#endif
            num = 0;
            for (i = 0; i < 16; i++)
            {
                if ((opcode >> i) & 1)
                {
#if CONFIG_DEBUG_TRACE
                    if (pf)
                    {
                        printk(", ");
                    }
                    printk("r%d", i);
#endif
                    if (i == 14)
                    {
                        if (!L)
                        {
                            lr_flag = 1;
                        }
                        else
                        {
                        }
                    }
                    if (L)
                    {
                        if (((opcode >> 14) & 1) || ((opcode >> 15) & 1))
                        {
                            // ignore instruction like : ldmfd   sp!, {lr}
                        }
                        else
                        {
                            num++;
                        }
                    }
                    else
                    {
                        num++;
                    }
                    pf = 1;
                }
            }
            if (L)
            {
                if (Rn == 13)
                {
                    if (P && !U) //ldmea
                    {
                        ;//*sp = *sp - num * 4;
                    }
                    if (P && U) // ldmed
                    {
                        ;//*sp = *sp - num * 4;
                    }
                    if (!P && U) //ldmfd
                    {
                        *sp = *sp - num * 4;
                    }
                    if (!P && !U) //ldmfa
                    {
                        ;//*sp = *sp - num * 4;
                    }
                }
                else
                {
                    if (P && !U) //ldmdb
                    {
                        ;//*sp = *sp - num * 4;
                    }
                    if (P && U) //ldmib
                    {
                        ;//*sp = *sp - num * 4;
                    }
                    if (!P && U) //ldmia
                    {
                        ;//*sp = *sp - num * 4;
                    }
                    if (!P && !U) //ldmda
                    {
                        ;//*sp = *sp - num * 4;
                    }
                }
            }
            else
            {
                if (Rn == 13)
                {
                    if (P && !U) //stmfd
                    {
                        *sp = *sp + num * 4;
                    }
                    if (P && U) //stmfa
                    {
                        //*sp = *sp + num * 4;
                    }
                    if (!P && U) //stmea
                    {
                        ;//*sp = *sp + num * 4;
                    }
                    if (!P && !U) //stmed
                    {
                        ;//*sp = *sp + num * 4;
                    }
                }
                else
                {
                    if (P && !U) //stmdb
                    {
                        ;//*sp = *sp + num * 4;
                    }
                    if (P && U) //stmib
                    {
                        ;//*sp = *sp + num * 4;
                    }
                    if (!P && U) //stmia
                    {
                        ;//*sp = *sp + num * 4;
                    }
                    if (!P && !U) //stmda
                    {
                        ;//*sp = *sp + num * 4;
                    }
                }
            }

#if CONFIG_DEBUG_TRACE
            printk("}");
            if (B)
            {
                printk("^");
            }
            printk("\n");
            printk("The sp is 0x%x:[0x%x]\n", *sp, *(unsigned int *)(*sp));
#endif
            return lr_flag;
        }
        default:
            break;
    }

    // arm instruction : sub, add, str, ldr, ignore add/ldr now
    switch ((opcode >> 26) & 0x3)
    {
        case 0:
        {
            switch ((opcode >> 21) & 0xF)
            {
                case 2:         // SUB
                {
                    if (!I)
                    {
                        if (Rd == Rn && Rd == 13)
                        {
#if CONFIG_DEBUG_TRACE
                            printk("sub");
                            printk(" r%d, r%d, r%d", Rd, Rn, Rm);
                            printk("\n");
                            printk("can not decode\n");
#endif
                        }
                    }
                    else
                    {
                        if (Rd == Rn && Rd == 13)
                        {
#if CONFIG_DEBUG_TRACE
                            printk("0x%x: [0x%0x]    #", pc, *pc);
                            printk("sub");
                            printk(" r%d, r%d, #0x%X", Rd, Rn, ROR(Imm, amt));
                            printk("\n");
#endif
                            *sp = *sp + ROR(Imm, amt);
                        }
                    }
                    return lr_flag;
                }
                case 4:   // ADD
                {
                    if (!I)
                    {
                        if (Rd == Rn && Rd == 13)
                        {
#if CONFIG_DEBUG_TRACE
                            printk("add");
                            printk(" r%d, r%d, r%d", Rd, Rn, Rm);
                            printk("\n");
                            printk("can not decode\n");
#endif
                        }
                    }
                    else
                    {
                        if (Rd == Rn && Rd == 13)
                        {
#if CONFIG_DEBUG_TRACE
                            printk("0x%x: [0x%0x]    #", pc, *pc);
                            printk("add");
                            printk(" r%d, r%d, #0x%X", Rd, Rn, ROR(Imm, amt));
                            printk("\n");
#endif
                            //*sp = *sp - ROR(Imm, amt);
                        }
                    }
                    return lr_flag;
                }
                default:
                    break;
            }
        }
        break;
        case 1:         // LDR/STR
        {
            unsigned int  addr, value;
            if (Rn != 13)
            {
                return lr_flag;
            }
            if (I)
            {
                return lr_flag;
            }
#if CONFIG_DEBUG_TRACE
            printk("0x%x: [0x%0x]    #", pc, *pc);
            printk("%s%s", (L) ? "ldr" : "str", (B) ? "b" : "");
            printk(" r%d,", Rd);
            printk(" [r%d", Rn);
#endif
            Imm = opcode & 0xFFF;

            if (I)
            {
#if CONFIG_DEBUG_TRACE
                printk(", %sr%d", (U) ? "" : "-", Rm);
#endif
            }
            else
            {
                value = Imm;
#if CONFIG_DEBUG_TRACE
                printk(", #%s0x%X", (U) ? "" : "-", value);
#endif
            }
#if CONFIG_DEBUG_TRACE
            printk("]%s\n", (W) ? "!" : "");
#endif
            if (L)
            {
                //*sp = *sp - value;
            }
            else if (W)
            {
                *sp = *sp + value;
            }
            return lr_flag;
        }
        default:
            break;
    }

    // thumb instruction : push and pop
    if ((opcode >> 12) == 11)
    {
        switch ((opcode >> 9) & 7)
        {
            case 2:         // PUSH
            {
                int lrf = opcode & 0x100;
                int pf  = 0;
#if CONFIG_DEBUG_TRACE
                printk("0x%x: [0x%0x]    #", pc, *pc);
                printk("push {");
#endif

                for (i = 0; i < 8; i++)
                {
                    if ((opcode >> i) & 1)
                    {
#if CONFIG_DEBUG_TRACE
                        if (pf)
                        {
                            printk(",");
                        }
                        printk("r%d", i);
#endif
                        *sp = *sp + 4;
                        pf = 1;
                    }
                }

                if (lrf)
                {
#if CONFIG_DEBUG_TRACE
                    if (pf)
                    {
                        printk(",");
                    }
                    printk("lr");
#endif
                    lr_flag = 1;
                }
#if CONFIG_DEBUG_TRACE
                printk("}\n");
#endif
                return lr_flag;
            }

            case 6:         // POP
            {
                int pcf = opcode & 0x100;
                int pf  = 0;

#if CONFIG_DEBUG_TRACE
                printk("0x%x: [0x%0x]    #", pc, *pc);
                printk("pop {");
#endif
                for (i = 0; i < 8; i++)
                {
                    if ((opcode >> i) & 1)
                    {
#if CONFIG_DEBUG_TRACE
                        if (pf)
                        {
                            printk(",");
                        }
                        printk("r%d", i);
#endif
                        *sp = *sp - 4;
                        pf = 1;
                    }
                }
#if CONFIG_DEBUG_TRACE
                if (pcf)
                {
                    if (pf)
                    {
                        printk(",");
                    }
                    printk("pc");
                }
                printk("}\n");
#endif
                return lr_flag;
            }
            default:
                return lr_flag;
        }
    }
    return lr_flag;
}
/*
 *  dump_stack_nofp
 *  @pc : the pc register value
 *  @sp : the sp register value
 *  @level : function stack level
 *  @last_pointer : a pointer to save the last function call
 *  @flag : flag to control printk, 0 or 1, 0 is for esKRNL_DumpStack, 1 is for memory leak check
 * */
void dump_stack_nofp(unsigned int pc, unsigned int sp, int level, unsigned int *last_pointer, int flag)
{
    unsigned int lr = 0;
    unsigned int temp = 0;
    unsigned int *pos = 0;
    unsigned int function = 0;
    int level_num = 0;
    printk("enter the %s\n", __FUNCTION__);
    if (pc == 0 || sp == 0)
    {
        asm volatile("mov %0, pc\n":"=r"(pc));
        asm volatile("mov %0, r13\n":"=r"(sp));
    }
    pc = pc - 8;
    pos = (unsigned int *)pc;
    if (flag == 0)
    {
        printk("The pc is 0x%x:[0x%x]\n", pos, *pos);
        printk("The sp is 0x%x:[0x%x]\n", sp, *(unsigned int *)sp);
        printk("\n");
        printk("The call chain:\n");
    }
    while (*pos != 0xdeadbeef && pos != (void *)rt_thread_exit)
    {
        while (*pos != 0xdeadbeef && pos != (void *)rt_thread_exit)
        {
            temp = parse(pos, &sp);
            if (temp != 0)
            {

#if CONFIG_DEBUG_TRACE
                printk("The sp is 0x%x:[0x%x]\n", sp - 4, *(unsigned int *)(sp - 4));
#endif
                function = (unsigned int)pos;
                lr = *(unsigned int *)(sp - 4);
                pos = (unsigned int *)(lr - 4);
                if (lr == (unsigned int)rt_thread_exit)
                {
                    pos = (unsigned int *)(lr);
                }
                else
                {
                    pos = (unsigned int *)(lr - 4);
                }
                //printk("The function from [0x%08x] to [0x%08x]\n", pos, function);
                if (flag == 0)
                {
                    printk("[0x%08x]\n", function);
                    printk("    |\n");
                    printk("[0x%08x]\n", pos);
                }
                break;
            }
            pos = pos - 1;
        }
        if (level > 0)
        {
            level_num++;
            if (level_num >= level)
            {
                break;
            }
        }
    }
    if (last_pointer)
    {
        *last_pointer = (unsigned int)pos;
    }
    printk("exit the %s\n", __FUNCTION__);
}
