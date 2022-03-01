/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               VE Driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : macc_vc1.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-13 15:59
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-13 15:59    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __MACC_VC1_H__
#define __MACC_VC1_H__

//#include "epdk.h"
//#include "drv_vdec_lib_cfg.h"


//macc io base
extern __u32    MACC_REGS_BASE;


//------------------------------------------------------
// define registers for vc1 macc
//------------------------------------------------------
#define VC1_REGS_BASE               (MACC_REGS_BASE + 0x300)

#define VC1_o_EPHS_REG              0x00
#define VC1_o_PICCTRL_REG           0x04
#define VC1_o_PICQP_REG             0x08
#define VC1_o_PICMV_REG             0x0c
#define VC1_o_PICINTENCOMP_REG      0x10
#define VC1_o_PICINTERLACE_REG      0x14
#define VC1_o_PICHDRLEN_REG         0x18
#define VC1_o_FSIZE_REG             0x1c
#define VC1_o_PICSIZE_REG           0x20
#define VC1_o_DEC_CTRL_REG          0x24
#define VC1_o_STARTTYPE_REG         0x28
#define VC1_o_STATUS_REG            0x2C
#define VC1_o_VLDBADDR_REG          0x30
#define VC1_o_VLDOFFSET_REG         0x34
#define VC1_o_VLDLEN_REG            0x38
#define VC1_o_VBVENDADDR_REG        0x3C
#define VC1_o_RECYADDR_REG          0x40
#define VC1_o_RECCADDR_REG          0x44
#define VC1_o_FORYADDR_REG          0x48
#define VC1_o_FORCADDR_REG          0x4c
#define VC1_o_BACKYADDR_REG         0x50
#define VC1_o_BACKCADDR_REG         0x54
#define VC1_o_MBHADDR_REG           0x58
#define VC1_o_DCACADDR_REG          0x5c
#define VC1_o_BITPLANEINFOADDR_REG  0x60
#define VC1_o_COLMVINFOADDR_REG     0x64
#define VC1_o_MBA_REG               0x70
#define VC1_o_MBHDR_REG             0x74
#define VC1_o_TRANSFORM_REG         0x78
#define VC1_o_MBCBF_REG             0x7C
#define VC1_o_MV1_REG               0x80
#define VC1_o_MV2_REG               0x84
#define VC1_o_MV3_REG               0x88
#define VC1_o_MV4_REG               0x8C
#define VC1_o_ERROR_CASE_REG        0xb8
#define VC1_o_CRTMBNUM_REG          0xbc
#define VC1_o_EXTRA_CTRL_REG        0xc0
#define VC1_o_EXTRA_YBUFADDR_REG    0xc4
#define VC1_o_EXTRA_CBUFADDR_REG    0xc8
#define VC1_o_OVERLAP_UP_ADDR_REG   0xd0
#define VC1_o_DBLK_ABOVE_ADDR_REG   0xd4
#define VC1_o_BITS_RETDATA_REG      0xdc
#define VC1_o_DEBUG_BUF_ADDR_REG    0xfc


#define VC1_EPHS_REG                (VC1_REGS_BASE + VC1_o_EPHS_REG           )
#define VC1_PICCTRL_REG             (VC1_REGS_BASE + VC1_o_PICCTRL_REG        )
#define VC1_PICQP_REG               (VC1_REGS_BASE + VC1_o_PICQP_REG          )
#define VC1_PICMV_REG               (VC1_REGS_BASE + VC1_o_PICMV_REG          )
#define VC1_PICINTENCOMP_REG        (VC1_REGS_BASE + VC1_o_PICINTENCOMP_REG   )
#define VC1_PICINTERLACE_REG        (VC1_REGS_BASE + VC1_o_PICINTERLACE_REG   )
#define VC1_PICHDRLEN_REG           (VC1_REGS_BASE + VC1_o_PICHDRLEN_REG      )
#define VC1_FSIZE_REG               (VC1_REGS_BASE + VC1_o_FSIZE_REG          )
#define VC1_PICSIZE_REG             (VC1_REGS_BASE + VC1_o_PICSIZE_REG        )
#define VC1_DEC_CTRL_REG            (VC1_REGS_BASE + VC1_o_DEC_CTRL_REG       )
#define VC1_STARTTYPE_REG           (VC1_REGS_BASE + VC1_o_STARTTYPE_REG      )
#define VC1_STATUS_REG              (VC1_REGS_BASE + VC1_o_STATUS_REG         )
#define VC1_VLDBADDR_REG            (VC1_REGS_BASE + VC1_o_VLDBADDR_REG       )
#define VC1_VLDOFFSET_REG           (VC1_REGS_BASE + VC1_o_VLDOFFSET_REG      )
#define VC1_VLDLEN_REG              (VC1_REGS_BASE + VC1_o_VLDLEN_REG         )
#define VC1_VBVENDADDR_REG          (VC1_REGS_BASE + VC1_o_VBVENDADDR_REG     )
#define VC1_RECYADDR_REG            (VC1_REGS_BASE + VC1_o_RECYADDR_REG       )
#define VC1_RECCADDR_REG            (VC1_REGS_BASE + VC1_o_RECCADDR_REG       )
#define VC1_FORYADDR_REG            (VC1_REGS_BASE + VC1_o_FORYADDR_REG       )
#define VC1_FORCADDR_REG            (VC1_REGS_BASE + VC1_o_FORCADDR_REG       )
#define VC1_BACKYADDR_REG           (VC1_REGS_BASE + VC1_o_BACKYADDR_REG      )
#define VC1_BACKCADDR_REG           (VC1_REGS_BASE + VC1_o_BACKCADDR_REG      )
#define VC1_MBHADDR_REG             (VC1_REGS_BASE + VC1_o_MBHADDR_REG        )
#define VC1_DCACADDR_REG            (VC1_REGS_BASE + VC1_o_DCACADDR_REG       )
#define VC1_BITPLANEINFOADDR_REG    (VC1_REGS_BASE + VC1_o_BITPLANEINFOADDR_REG)
#define VC1_COLMVINFOADDR_REG       (VC1_REGS_BASE + VC1_o_COLMVINFOADDR_REG  )
#define VC1_MBA_REG                 (VC1_REGS_BASE + VC1_o_MBA_REG            )
#define VC1_MBHDR_REG               (VC1_REGS_BASE + VC1_o_MBHDR_REG          )
#define VC1_TRANSFORM_REG           (VC1_REGS_BASE + VC1_o_TRANSFORM_REG      )
#define VC1_MBCBF_REG               (VC1_REGS_BASE + VC1_o_MBCBF_REG          )
#define VC1_MV1_REG                 (VC1_REGS_BASE + VC1_o_MV1_REG            )
#define VC1_MV2_REG                 (VC1_REGS_BASE + VC1_o_MV2_REG            )
#define VC1_MV3_REG                 (VC1_REGS_BASE + VC1_o_MV3_REG            )
#define VC1_MV4_REG                 (VC1_REGS_BASE + VC1_o_MV4_REG            )
#define VC1_ERROR_CASE_REG          (VC1_REGS_BASE + VC1_o_ERROR_CASE_REG     )
#define VC1_CRTMBNUM_REG            (VC1_REGS_BASE + VC1_o_CRTMBNUM_REG       )
#define VC1_EXTRA_CTRL_REG          (VC1_REGS_BASE + VC1_o_EXTRA_CTRL_REG     )
#define VC1_EXTRA_YBUFADDR_REG      (VC1_REGS_BASE + VC1_o_EXTRA_YBUFADDR_REG )
#define VC1_EXTRA_CBUFADDR_REG      (VC1_REGS_BASE + VC1_o_EXTRA_CBUFADDR_REG )
#define VC1_OVERLAP_UP_ADDR_REG     (VC1_REGS_BASE + VC1_o_OVERLAP_UP_ADDR_REG)
#define VC1_DBLK_ABOVE_ADDR_REG     (VC1_REGS_BASE + VC1_o_DBLK_ABOVE_ADDR_REG)
#define VC1_BITS_RETDATA_REG        (VC1_REGS_BASE + VC1_o_BITS_RETDATA_REG   )
#define VC1_DEBUG_BUF_ADDR_REG      (VC1_REGS_BASE + VC1_o_DEBUG_BUF_ADDR_REG )


//define macc interrput type for vc1
#define MACC_INT_VC1_FINISH         (0)
#define MACC_INT_VC1_ERROR          (1)
#define MACC_INT_VC1_MEMREQ         (2)
#define MACC_INT_VC1_DBLK           (3)


#endif  //__MACC_VC1_H__

