/*
 * =====================================================================================
 *
 *       Filename:  cpu.h
 *
 *    Description:  Processor register  Status and Control bits definition.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:15:35
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-06-08 17:41:14
 *
 * =====================================================================================
 */
#ifndef __CPUV7A_INC__
#define __CPUV7A_INC__

/* ARM CPU Work Mode */
#define ARMV7A_USR_MODE        0x10
#define ARMV7A_FIQ_MODE        0x11
#define ARMV7A_IRQ_MODE        0x12
#define ARMV7A_SVC_MODE        0x13
#define ARMV7A_ABT_MODE        0x17
#define ARMV7A_UND_MODE        0x1b
#define ARMV7A_SYS_MODE        0x1f
#define ARMV7A_MODE_MON        0x16

#define ARMV7A_MODE_MASK       0x1f
#define ARMV7A_THB_MASK        0x20
#define ARMV7A_FIQ_MASK        0x40
#define ARMV7A_IRQ_MASK        0x80
#define ARMV7A_CC_V_BIT        (1 << 28)
#define ARMV7A_CC_C_BIT        (1 << 29)
#define ARMV7A_CC_Z_BIT        (1 << 30)
#define ARMV7A_CC_N_BIT        (1 << 31)

/* CP15, C1 Byte */
#define C1_M_BIT             (1 << 0)
#define C1_A_BIT             (1 << 1)
#define C1_C_BIT             (1 << 2)
#define C1_W_BIT             (1 << 3)
#define C1_P_BIT             (1 << 4)
#define C1_D_BIT             (1 << 5)
#define C1_L_BIT             (1 << 6)
#define C1_B_BIT             (1 << 7)
#define C1_S_BIT             (1 << 8)
#define C1_R_BIT             (1 << 9)
#define C1_F_BIT             (1 << 10)
#define C1_Z_BIT             (1 << 11)
#define C1_I_BIT             (1 << 12)
#define C1_V_BIT             (1 << 13)
#define C1_RR_BIT            (1 << 14)
#define C1_L4_BIT            (1 << 15)
#define C1_AFE_BIT           (1 << 29)

#define IRQ_MASK             0x00000080
#define FIQ_MASK             0x00000040
#define INT_MASK             (IRQ_MASK | FIQ_MASK)
#define ICACHE_MASK          (uint32_t)(1 << 12)
#define DCACHE_MASK          (uint32_t)(1 << 2)


/* CR1 bits (CP#15 CR1) */

#define CR_M                 0x00000001 /* MMU enable                          */
#define CR_A                 0x00000002 /* Alignment abort enable              */
#define CR_C                 0x00000004 /* Dcache enable                       */
#define CR_W                 0x00000008 /* Write buffer enable                 */
#define CR_P                 0x00000010 /* 32-bit exception handler            */
#define CR_D                 0x00000020 /* 32-bit data address range           */
#define CR_L                 0x00000040 /* Implementation defined              */
#define CR_B                 0x00000080 /* Big endian                          */
#define CR_S                 0x00000100 /* System MMU protection               */
#define CR_R                 0x00000200 /* ROM MMU protection                  */
#define CR_F                 0x00000400 /* Implementation defined              */
#define CR_Z                 0x00000800 /* Implementation defined              */
#define CR_I                 0x00001000 /* Icache enable                       */
#define CR_V                 0x00002000 /* Vectors relocated to 0xffff0000     */
#define CR_RR                0x00004000 /* Round Robin cache replacement       */
#define CR_L4                0x00008000 /* LDR pc can set T bit                */
#define CR_DT                0x00010000
#define CR_IT                0x00040000
#define CR_ST                0x00080000
#define CR_FI                0x00200000 /* Fast interrupt (lower latency mode) */
#define CR_U                 0x00400000 /* Unaligned access operation          */
#define CR_XP                0x00800000 /* Extended page tables                */
#define CR_VE                0x01000000 /* Vectored interrupts                 */

/* CP15 register c2 contains a pointer to the base address of a paged table in
 * physical memory.  Only bits 14-31 of the page table address is retained there;
 * The full 30-bit address is formed by ORing in bits 2-13 or the virtual address
 * (MVA).  As a consequence, the page table must be aligned to a 16Kb address in
 * physical memory and could require up to 16Kb of memory.
 */
#define PGTABLE_SIZE          0x00004000


/*
 * system irq stack map. be carful not overlap with irqs_gnu.S code area. *
 * SVC: only used during bringup stage, would be taken by each task stack.*
 * FIQ: ARCH not support FIQ preset, fake.                                *
 * UNDEF, ABT: should be enough preset.                                   *
 * IRQ: should satisfy the worst case of user irq procedure.set to 2k .   *
 */
#ifdef CONFIG_ARMCPU_HIGH_VECTOR_ADDRESS
#define EPOS_INTV_BASEv      0xffff0000
#else
#define EPOS_INTV_BASEv      0x00000000
#endif

//0xffff0000 ~ 0xffff1000 ro
//0xffff1000 ~ 0xffffafff rw
//[0xffff0000, 0xffff6fff] => [0x40004000, 0x4000afff]

#define EPOS_STACK_FIQ       (EPOS_INTV_BASEv + 0x1400)
#define EPOS_STACK_SYS       (EPOS_INTV_BASEv + 0x1600)
#define EPOS_STACK_SVC       (EPOS_INTV_BASEv + 0x2000)
#define EPOS_STACK_UNDEF     (EPOS_INTV_BASEv + 0x3000)
#define EPOS_STACK_ABT       (EPOS_INTV_BASEv + 0x4000)
#define EPOS_STACK_IRQ       (EPOS_INTV_BASEv + 0x5000)
#define EPOS_STACK_MON       (EPOS_INTV_BASEv + 0x6000)

/* the context of irq is [r0-r12, lr], 14 registers total.*/
#define EPOS_STACK_IRQ_SAFE_CHK     (EPOS_STACK_IRQ - 4*14)

/**********************************************************
 * support arm Fast Context Switch Extension              *
 * if (va[31:25] == 0b0000000)                            *
 *     mva = va | fcseid << 25;                           *
 * else                                                   *
 *     mva =  va;                                         *
 **********************************************************/
#define NOFCSE_AREA          0xc0000000
#define FCSEID_SHIFT         25
#define GCC_SWINO            0xffffff
#define RVDS_SWINO           0x0

#define DMA_BIDIRECTIONAL    (0)     /* Memory<----->Device */
#define DMA_TO_DEVICE        (1)     /* Memory------>Device */
#define DMA_FROM_DEVICE      (2)     /* Memory<------Device */

#endif

