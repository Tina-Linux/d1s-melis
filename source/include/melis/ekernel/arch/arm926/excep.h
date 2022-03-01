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
 *  Last Modified:  2019-01-02 14:19:29
 *
 * =====================================================================================
 */

#ifndef  __EXP926_INC__
#define  __EXP926_INC__

#include <stdint.h>

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
} excep_regs_t;

/* ----------------------------------------------------------------------------*/
/* @brief  <switch_ctx_regs_t> the spot registers of switch happend. */
/* ----------------------------------------------------------------------------*/
typedef struct
{
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

#endif

