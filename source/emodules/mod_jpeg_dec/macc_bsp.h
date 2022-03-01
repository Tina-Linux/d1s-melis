/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               VE Driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : macc.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-13 14:38
* Descript: vdec_lib的对外头文件.因为macc的一些操作涉及系统其它模块，所以不放在vdec_lib内部，由各系统自己去实现定义的
                函数接口.
* Update  : date                auther      ver     notes
*           2010-12-13 14:38    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __MACC_BSP_H__
#define __MACC_BSP_H__

#include <typedef.h>
#include <ktype.h>
#include <const.h>

#if(EPDK_ARCH == EPDK_ARCH_SUNIII || EPDK_ARCH == EPDK_ARCH_SUNIV)

//#include "epdk.h"
//#include "mod_cedar_i.h"
//#include "drv_vdec_lib_cfg.h"


//MACC io base address
extern __u32    MACC_REGS_BASE;
#include "macc_core.h"
#include "macc_h264.h"
#include "macc_mpeg.h"
#include "macc_rmvb.h"
#include "macc_vc1.h"

#define VE_IRQ_NUM      82 //F133
//define macc hardware information
#define MACC_PHY_IO_BASE        (0x01c0e000)
#define INTC_IRQNO_MACC         (VE_IRQ_NUM)

//define fucntion for register access
#define ReadReg(uAddr)          (*(volatile __u32 *)(uAddr))
#define WriteReg(uAddr, dwVal)  *(volatile __u32 *)(uAddr) = (dwVal);

//#define ReadReg(uAddr)          (*((volatile unsigned long  *)(uAddr)))

//#define WriteReg(uAddr, dwVal)        (*((volatile unsigned long  *)(uAddr)) = (unsigned long)(dwVal))


#define MACC_INT_VE_FINISH          (0)
#define MACC_INT_VE_ERROR           (1)
#define MACC_INT_VE_MEMREQ          (2)
#define MACC_INT_VE_DBLK            (3)

//define macc reset status
typedef enum __MACC_RESET_STAT
{
    MACC_RESET_DISABLE = 0,
    MACC_RESET_ENABLE  = 1,
} __macc_reset_stat_e;

//define macc access dram mode
typedef enum __MACC_DRAM_ACS_MODE
{
    MACC_DRAM_ACS_DISABLE   = 0,
    MACC_DRAM_ACS_ENABLE    = 1,
    MACC_DRAM_ACS_ERR       = 2,
} __macc_dram_acs_mode_e;


//define sub-module of video engine
typedef enum __MACC_MOD_TYPE
{
    MACC_MODULE_MPEG_DEC = 0,
    MACC_MODULE_H264_DEC = 1,
    MACC_MODULE_VC1_DEC  = 2,
    MACC_MODULE_RMVB_DEC = 3,
    MACC_MODULE_MPEG_ENC = 8,
    MACC_MODULE_ISP_CORE = 10,
    MACC_MODULE_AVC_ENC  = 11,
    MACC_MODULE_AVC_ERR  = 12,
} __macc_mod_type_e;

//------------------------------------------------------
// macc bsp interrface
//------------------------------------------------------
extern __s32 MACC_Init(__macc_mod_type_e nModType);
extern __s32 MACC_Exit(__macc_mod_type_e nModType);
extern __s32 MACC_SetClk(__u32 nFreq);
extern __s32 MACC_QueryINT(__macc_mod_type_e nModType, __u32 nIntType);
extern __s32 X8MACC_QueryINT(__macc_mod_type_e nModType, __u32 nIntType);
extern __s32 MACC_X8QueryINT(__macc_mod_type_e nModType, __u32 nIntType);
extern __s32 MACC_EnableINT(__macc_mod_type_e nModType, __u32 nIntType);
extern __s32 MACC_DisableINT(__macc_mod_type_e nModType, __u32 nIntType);
extern __s32 MACC_InstIsr(__macc_mod_type_e nModType, __pISR_t pIsr, void *arg);
extern __s32 MACC_UninstIsr(__macc_mod_type_e nModType, __pISR_t pIsr);
extern __s32 MACC_ResetVeCore(__macc_mod_type_e nModType, __s32 nRstMode);
extern __s32 MACC_ModuleSel(__macc_mod_type_e nModType);
extern __s32 MACC_EnableCore(__macc_mod_type_e nModType);
extern __s32 MACC_DisableCore(__macc_mod_type_e nModType);
extern __s32 MACC_Activate(__macc_mod_type_e nModType);
extern __s32 MACC_Deactivate(void);
extern __s32 MACC_DramAcsCfg(__macc_dram_acs_mode_e mode);
extern __u64 MACC_GetMACCVersion(void);

#endif  //(EPDK_ARCH == EPDK_ARCH_SUNIII || EPDK_ARCH == EPDK_ARCH_SUNIV)
#endif  //__MACC_BSP_H__

