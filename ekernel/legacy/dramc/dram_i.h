/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               DRAM CSP Module
*
*                                    (c) Copyright 2006-2010, kevin.z.m China
*                                             All Rights Reserved
*
* File    : dram_i.h
* By      : kevin.z.m
* Version : v1.0
* Date    : 2011-1-25 14:47
* Descript: dram csp for aw1619;
* Update  : date                auther      ver     notes
*           2011-1-25 14:47     kevin.z.m   1.0     build the file;
*           2011-1-26 14:00     cpl         1.1     modify for aw1619 system
*********************************************************************************************************
*/
#ifndef __DRAM_I_H__
#define __DRAM_I_H__
//#include "bsp_i.h"

#define DRAMC_PHY_BASE      0x01C01000
#define DRAMC_MEM_SIZE      0x400
#define DRAMC_Common        0xf1c62000
#define DRAM_Controller     0xf1c63000
#define System_Protected    0xf1c63000

extern uint32_t gDramcVirtAddr;

#ifndef __REG
#define __REG_XXX
#define __REG(x)    (*(volatile unsigned int   *)(x))
#endif

#define     C_WORK_MODE         __REG(DRAMC_Common + 0x00)
#define     MC_R1_WORK_MODE     __REG(DRAMC_Common + 0x04
#define     MC_DBGCR            __REG(DRAMC_Common + 0x08)
#define     MC_TMR              __REG(DRAMC_Common + 0x0c)
#define     MC_MnCR0(n)         __REG(DRAMC_Common + 0x10 + n*0x08)
#define     MC_MnCR1(n)         __REG(DRAMC_Common + 0x14 + n*0x08)
#define     MC_BWCR             __REG(DRAMC_Common + 0x90)
#define     MC_MAER             __REG(DRAMC_Common + 0x94)
#define     MC_MCFADR           __REG(DRAMC_Common + 0x98)
#define     MC_GCGCR            __REG(DRAMC_Common + 0x9c)
#define     MC_CPU_BWCR         __REG(DRAMC_Common + 0xa0)
#define     MC_DTMB_BWCR        __REG(DRAMC_Common + 0xa4)
#define     MC_VE_BWCR          __REG(DRAMC_Common + 0xa8)
#define     MC_DISP_BWCR        __REG(DRAMC_Common + 0xac)
#define     MC_OTHER_BWCR       __REG(DRAMC_Common + 0xb0)
#define     MC_TOTAL_BWCR       __REG(DRAMC_Common + 0xb4)

#define DRAM_REG_SCONR          __REG(gDramcVirtAddr + 0x00)
#define DRAM_REG_STMG0R         __REG(gDramcVirtAddr + 0x04)
#define DRAM_REG_STMG1R         __REG(gDramcVirtAddr + 0x08)
#define DRAM_REG_SCTLR          __REG(gDramcVirtAddr + 0x0c)
#define DRAM_REG_SREFR          __REG(gDramcVirtAddr + 0x10)
#define DRAM_REG_SEXTMR         __REG(gDramcVirtAddr + 0x14)
#define DRAM_REG_SMCR0          __REG(gDramcVirtAddr + 0x18)
#define DRAM_REG_SMCR1          __REG(gDramcVirtAddr + 0x1c)
#define DRAM_REG_SMCR2          __REG(gDramcVirtAddr + 0x20)
#define DRAM_REG_DDLYR          __REG(gDramcVirtAddr + 0x24)
#define DRAM_REG_DADRR          __REG(gDramcVirtAddr + 0x28)
#define DRAM_REG_DVALR          __REG(gDramcVirtAddr + 0x2c)
#define DRAM_REG_DRPTR0         __REG(gDramcVirtAddr + 0x30)
#define DRAM_REG_DRPTR1         __REG(gDramcVirtAddr + 0x34)
#define DRAM_REG_DRPTR2         __REG(gDramcVirtAddr + 0x38)
#define DRAM_REG_DRPTR3         __REG(gDramcVirtAddr + 0x3c)
#define DRAM_REG_SEFR           __REG(gDramcVirtAddr + 0x40)
#define DRAM_REG_MAE            __REG(gDramcVirtAddr + 0x44)
#define DRAM_REG_ASPR           __REG(gDramcVirtAddr + 0x48)
#define DRAM_REG_SDLY0          __REG(gDramcVirtAddr + 0x4C)
#define DRAM_REG_SDLY1          __REG(gDramcVirtAddr + 0x50)
#define DRAM_REG_SDLY2          __REG(gDramcVirtAddr + 0x54)
#define DRAM_REG_MCR0           __REG(gDramcVirtAddr + 0x100 + 4*0)
#define DRAM_REG_MCR1           __REG(gDramcVirtAddr + 0x100 + 4*1)
#define DRAM_REG_MCR2           __REG(gDramcVirtAddr + 0x100 + 4*2)
#define DRAM_REG_MCR3           __REG(gDramcVirtAddr + 0x100 + 4*3)
#define DRAM_REG_MCR4           __REG(gDramcVirtAddr + 0x100 + 4*4)
#define DRAM_REG_MCR5           __REG(gDramcVirtAddr + 0x100 + 4*5)
#define DRAM_REG_MCR6           __REG(gDramcVirtAddr + 0x100 + 4*6)
#define DRAM_REG_MCR7           __REG(gDramcVirtAddr + 0x100 + 4*7)
#define DRAM_REG_MCR8           __REG(gDramcVirtAddr + 0x100 + 4*8)
#define DRAM_REG_MCR9           __REG(gDramcVirtAddr + 0x100 + 4*9)
#define DRAM_REG_MCR10          __REG(gDramcVirtAddr + 0x100 + 4*10)
#define DRAM_REG_MCR11          __REG(gDramcVirtAddr + 0x100 + 4*11)
#define DRAM_REG_BWCR           __REG(gDramcVirtAddr + 0x140)

//PIO register for dram
#define DRAM_PIO_BASE           0x01c20800
#define DRAM_PIO_MEMSIZE        0x400
extern __u32 gDramPioBase;
#define SDR_PAD_DRV_REG         __REG(gDramPioBase + 0x220)
#define SDR_PAD_PUL_REG         __REG(gDramPioBase + 0x224)

//CCM register for dram
#define DRAM_CCM_BASE           0x01c20000
#define DRAM_CCM_MEMSIZE        0x400
extern __u32 gDramCcmBase;
#define DRAM_CCM_AHB_GATE_REG   __REG(gDramCcmBase + 0x0c)
#define DRAM_CCM_APB_GATE_REG   __REG(gDramCcmBase + 0x10)
#define DRAM_CCM_SDRAM_PLL_REG  __REG(gDramCcmBase + 0x20)


#define SDR_T_CAS               0x2
#define SDR_T_RAS               0x7
#define SDR_T_RCD               0x2
#define SDR_T_RP                0x2
#define SDR_T_WR                0x2
#define SDR_T_RFC               0xd
#define SDR_T_XSR               0xf9
#define SDR_T_RC                0xa
#define SDR_T_INIT              0x8
#define SDR_T_INIT_REF          0x7
#define SDR_T_WTR               0x1
#define SDR_T_RRD               0x1
#define SDR_T_XP                0x0


#endif  //__DRAM_I_H__
