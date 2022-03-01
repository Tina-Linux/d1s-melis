/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               VE Driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : macc_core.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-13 14:38
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-13 14:38    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __MACC_CORE_H__
#define __MACC_CORE_H__


//------------------------------------------------------
// define registers for macc core
//------------------------------------------------------
#define MACC_REG_o_MODESEL_REG              0x00
#define MACC_REG_o_RESET_REG                0x04
#define MACC_REG_o_CYCLES_COUNTER_REG       0x08
#define MACC_REG_o_OVERTIMETH_REG           0x0c
#define MACC_REG_o_MMCREQ_WNUM_REG          0x10
#define MACC_REG_o_CACHEREG_WNUM_REG        0x14
#define MACC_REG_o_STATUS_REG               0x1c
#define MACC_REG_o_RDDATA_COUNTER_REG       0x20
#define MACC_REG_o_WRDATA_COUNTER_REG       0x24

#define MACC_REG_o_PRI_CHROMA_BUF_LEN_REG       0xc4
#define MACC_REG_o_PRI_BUF_LINE_STRIDE_REG       0xc8
#define MACC_REG_o_SEC_BUF_LINE_STRIDE_REG       0xcc

#define MACC_REG_o_OUTPUT_FORMAT_REG        0xec
#define MACC_REG_o_CHROMA_BUF_LEN0_REG      0xe8
#define MACC_REG_o_VERSION                  0xf0

#define MACC_REG_MODESEL_REG                (MACC_REGS_BASE + MACC_REG_o_MODESEL_REG       )
#define MACC_REG_RESET_REG                  (MACC_REGS_BASE + MACC_REG_o_RESET_REG         )
#define MACC_REG_CYCLES_COUNTER_REG         (MACC_REGS_BASE + MACC_REG_o_CYCLES_COUNTER_REG)
#define MACC_REG_OVERTIMETH_REG             (MACC_REGS_BASE + MACC_REG_o_OVERTIMETH_REG    )
#define MACC_REG_MMCREQ_WNUM_REG            (MACC_REGS_BASE + MACC_REG_o_MMCREQ_WNUM_REG   )
#define MACC_REG_CACHEREG_WNUM_REG          (MACC_REGS_BASE + MACC_REG_o_CACHEREG_WNUM_REG )
#define MACC_REG_STATUS_REG                 (MACC_REGS_BASE + MACC_REG_o_STATUS_REG        )
#define MACC_REG_RDDATA_COUNTER_REG         (MACC_REGS_BASE + MACC_REG_o_RDDATA_COUNTER_REG)
#define MACC_REG_WRDATA_COUNTER_REG         (MACC_REGS_BASE + MACC_REG_o_WRDATA_COUNTER_REG)
#define MACC_REG_OUTPUT_FORMAT_REG          (MACC_REGS_BASE + MACC_REG_o_OUTPUT_FORMAT_REG)
#define MACC_REG_CHROMA_BUF_LEN0_REG        (MACC_REGS_BASE + MACC_REG_o_CHROMA_BUF_LEN0_REG)

#define MACC_REG_PRI_CHROMA_BUF_LEN_REG        (MACC_REGS_BASE + MACC_REG_o_PRI_CHROMA_BUF_LEN_REG)
#define MACC_REG_PRI_BUF_LINE_STRIDE_REG        (MACC_REGS_BASE + MACC_REG_o_PRI_BUF_LINE_STRIDE_REG)
#define MACC_REG_SEC_BUF_LINE_STRIDE_REG        (MACC_REGS_BASE + MACC_REG_o_SEC_BUF_LINE_STRIDE_REG)

#define MACC_REG_VERSION_REG                (MACC_REGS_BASE + MACC_REG_o_VERSION)

#endif  //__MACC_CORE_H__

