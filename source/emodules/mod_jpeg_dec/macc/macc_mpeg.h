/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               VE Driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : macc_mpeg.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-13 14:38
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-13 14:38    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __MACC_MPEG_H__
#define __MACC_MPEG_H__


//macc io base
extern __u32    MACC_REGS_BASE;


//------------------------------------------------------
// define registers for mpeg1/2/4 macc
//------------------------------------------------------
#define MPEG_REGS_BASE              (MACC_REGS_BASE + 0x100)

/* offset */
#define MPEG_REG_o_PIC_HDR          0x00
#define MPEG_REG_o_VOP_HDR          0x04
#define MPEG_REG_o_FRM_SIZE         0x08
#define MPEG_REG_o_PIC_BOUND        0x0C
#define MPEG_REG_o_MB_ADDR          0x10
#define MPEG_REG_o_VE_CTL           0x14
#define MPEG_REG_o_VE_TRIGR         0x18
#define MPEG_REG_o_VE_STAT          0x1C
#define MPEG_REG_o_TRB_TRD          0x20
#define MPEG_REG_o_DIR_MODE         0x24
#define MPEG_REG_o_VLD_START        0x28
#define MPEG_REG_o_VLD_OFFSET       0x2C
#define MPEG_REG_o_VLD_LEN          0x30
#define MPEG_REG_o_VLD_END          0x34
#define MPEG_REG_o_MB_HDR_INF       0x38
#define MPEG_REG_o_FDC_QAC          0x3C
#define MPEG_REG_o_DBLK_ADDR        0x40
#define MPEG_REG_o_NOT_DEC          0x44
#define MPEG_REG_o_RECON_LUMA       0x48
#define MPEG_REG_o_RECON_CHROMA     0x4C
#define MPEG_REG_o_FREF_LUMA        0x50
#define MPEG_REG_o_FREF_CHROMA      0x54
#define MPEG_REG_o_BREF_LUMA        0x58
#define MPEG_REG_o_BREF_CHROMA      0x5C
#define MPEG_REG_o_SOC_X            0x60
#define MPEG_REG_o_SOC_Y            0x64
#define MPEG_REG_o_SOL              0x68
#define MPEG_REG_o_SDLX             0x6C
#define MPEG_REG_o_SDLY             0x70
#define MPEG_REG_o_SSR              0x74
#define MPEG_REG_o_SDCX             0x78
#define MPEG_REG_o_SDCY             0x7C
#define MPEG_REG_o_QMINPUT          0x80
#define MPEG_REG_o_QPINPUT          0x84
#define MPEG_REG_o_RESERVED0        0x88
#define MPEG_REG_o_RESERVED1        0x8C
#define MPEG_REG_o_IQ_IDCT          0x90
#define MPEG_REG_o_MB_HDR           0x94
#define MPEG_REG_o_1ST_MV           0x98
#define MPEG_REG_o_2ND_MV           0x9A
#define MPEG_REG_o_3RD_MV           0xA0
#define MPEG_REG_o_4TH_MV           0xA4
#define MPEG_REG_o_5TH_MV           0xA8
#define MPEG_REG_o_6TH_MV           0xAC
#define MPEG_REG_o_7TH_MV           0xB0
#define MPEG_REG_o_8TH_MV           0xB4
#define MPEG_REG_o_JPG_MCU0         0xB8
#define MPEG_REG_o_JPG_MCU1         0xBC
#define MPEG_REG_o_JPG_RESINT       0xC0
#define MPEG_REG_o_DEC_ERR          0xC4
#define MPEG_REG_o_MB_NUM           0xC8
#define MPEG_REG_o_JPG_YADDR        0xCC
#define MPEG_REG_o_JPG_CADDR        0xD0
#define MPEG_REG_o_ROTAT_CTL        0xD4
#define MPEG_REG_o_JPEG_START_MCU   0xD8
#define MPEG_REG_o_JPEG_END_MCU     0xDC
#define MPEG_REG_o_SRAM_RW_OFFSET   0xE0
#define MPEG_REG_o_SRAM_RW_DATA     0xE4

/* registers */
#define MPEG_REG_PIC_HDR            ( MPEG_REGS_BASE + MPEG_REG_o_PIC_HDR        )
#define MPEG_REG_VOP_HDR            ( MPEG_REGS_BASE + MPEG_REG_o_VOP_HDR        )
#define MPEG_REG_FRM_SIZE           ( MPEG_REGS_BASE + MPEG_REG_o_FRM_SIZE       )
#define MPEG_REG_PIC_BOUND          ( MPEG_REGS_BASE + MPEG_REG_o_PIC_BOUND      )
#define MPEG_REG_MB_ADDR            ( MPEG_REGS_BASE + MPEG_REG_o_MB_ADDR        )
#define MPEG_REG_VE_CTL             ( MPEG_REGS_BASE + MPEG_REG_o_VE_CTL         )
#define MPEG_REG_VE_TRIGR           ( MPEG_REGS_BASE + MPEG_REG_o_VE_TRIGR       )
#define MPEG_REG_VE_STAT            ( MPEG_REGS_BASE + MPEG_REG_o_VE_STAT        )
#define MPEG_REG_TRB_TRD            ( MPEG_REGS_BASE + MPEG_REG_o_TRB_TRD        )
#define MPEG_REG_DIR_MODE           ( MPEG_REGS_BASE + MPEG_REG_o_DIR_MODE       )
#define MPEG_REG_VLD_START          ( MPEG_REGS_BASE + MPEG_REG_o_VLD_START      )
#define MPEG_REG_VLD_OFFSET         ( MPEG_REGS_BASE + MPEG_REG_o_VLD_OFFSET     )
#define MPEG_REG_VLD_LEN            ( MPEG_REGS_BASE + MPEG_REG_o_VLD_LEN        )
#define MPEG_REG_VLD_END            ( MPEG_REGS_BASE + MPEG_REG_o_VLD_END        )
#define MPEG_REG_MB_HDR_INF         ( MPEG_REGS_BASE + MPEG_REG_o_MB_HDR_INF     )
#define MPEG_REG_FDC_QAC            ( MPEG_REGS_BASE + MPEG_REG_o_FDC_QAC        )
#define MPEG_REG_DBLK_ADDR          ( MPEG_REGS_BASE + MPEG_REG_o_DBLK_ADDR      )
#define MPEG_REG_NOT_DEC            ( MPEG_REGS_BASE + MPEG_REG_o_NOT_DEC        )
#define MPEG_REG_RECON_LUMA         ( MPEG_REGS_BASE + MPEG_REG_o_RECON_LUMA     )
#define MPEG_REG_RECON_CHROMA       ( MPEG_REGS_BASE + MPEG_REG_o_RECON_CHROMA   )
#define MPEG_REG_FREF_LUMA          ( MPEG_REGS_BASE + MPEG_REG_o_FREF_LUMA      )
#define MPEG_REG_FREF_CHROMA        ( MPEG_REGS_BASE + MPEG_REG_o_FREF_CHROMA    )
#define MPEG_REG_BREF_LUMA          ( MPEG_REGS_BASE + MPEG_REG_o_BREF_LUMA      )
#define MPEG_REG_BREF_CHROMA        ( MPEG_REGS_BASE + MPEG_REG_o_BREF_CHROMA    )
#define MPEG_REG_SOC_X              ( MPEG_REGS_BASE + MPEG_REG_o_SOC_X          )
#define MPEG_REG_SOC_Y              ( MPEG_REGS_BASE + MPEG_REG_o_SOC_Y          )
#define MPEG_REG_SOL                ( MPEG_REGS_BASE + MPEG_REG_o_SOL            )
#define MPEG_REG_SDLX               ( MPEG_REGS_BASE + MPEG_REG_o_SDLX           )
#define MPEG_REG_SDLY               ( MPEG_REGS_BASE + MPEG_REG_o_SDLY           )
#define MPEG_REG_SSR                ( MPEG_REGS_BASE + MPEG_REG_o_SSR            )
#define MPEG_REG_SDCX               ( MPEG_REGS_BASE + MPEG_REG_o_SDCX           )
#define MPEG_REG_SDCY               ( MPEG_REGS_BASE + MPEG_REG_o_SDCY           )
#define MPEG_REG_QMINPUT            ( MPEG_REGS_BASE + MPEG_REG_o_QMINPUT        )
#define MPEG_REG_QPINPUT            ( MPEG_REGS_BASE + MPEG_REG_o_QPINPUT        )
#define MPEG_REG_RESERVED0          ( MPEG_REGS_BASE + MPEG_REG_o_RESERVED0      )
#define MPEG_REG_RESERVED1          ( MPEG_REGS_BASE + MPEG_REG_o_RESERVED1      )
#define MPEG_REG_IQ_IDCT            ( MPEG_REGS_BASE + MPEG_REG_o_IQ_IDCT        )
#define MPEG_REG_MB_HDR             ( MPEG_REGS_BASE + MPEG_REG_o_MB_HDR         )
#define MPEG_REG_1ST_MV             ( MPEG_REGS_BASE + MPEG_REG_o_1ST_MV         )
#define MPEG_REG_2ND_MV             ( MPEG_REGS_BASE + MPEG_REG_o_2ND_MV         )
#define MPEG_REG_3RD_MV             ( MPEG_REGS_BASE + MPEG_REG_o_3RD_MV         )
#define MPEG_REG_4TH_MV             ( MPEG_REGS_BASE + MPEG_REG_o_4TH_MV         )
#define MPEG_REG_5TH_MV             ( MPEG_REGS_BASE + MPEG_REG_o_5TH_MV         )
#define MPEG_REG_6TH_MV             ( MPEG_REGS_BASE + MPEG_REG_o_6TH_MV         )
#define MPEG_REG_7TH_MV             ( MPEG_REGS_BASE + MPEG_REG_o_7TH_MV         )
#define MPEG_REG_8TH_MV             ( MPEG_REGS_BASE + MPEG_REG_o_8TH_MV         )
#define MPEG_REG_JPG_MCU0           ( MPEG_REGS_BASE + MPEG_REG_o_JPG_MCU0       )
#define MPEG_REG_JPG_MCU1           ( MPEG_REGS_BASE + MPEG_REG_o_JPG_MCU1       )
#define MPEG_REG_JPG_RESINT         ( MPEG_REGS_BASE + MPEG_REG_o_JPG_RESINT     )
#define MPEG_REG_DEC_ERR            ( MPEG_REGS_BASE + MPEG_REG_o_DEC_ERR        )
#define MPEG_REG_MB_NUM             ( MPEG_REGS_BASE + MPEG_REG_o_MB_NUM         )
#define MPEG_REG_JPG_YADDR          ( MPEG_REGS_BASE + MPEG_REG_o_JPG_YADDR      )
#define MPEG_REG_JPG_CADDR          ( MPEG_REGS_BASE + MPEG_REG_o_JPG_CADDR      )
#define MPEG_REG_ROTAT_CTL          ( MPEG_REGS_BASE + MPEG_REG_o_ROTAT_CTL      )
#define MPEG_REG_JPEGSTART_MCU      ( MPEG_REGS_BASE + MPEG_REG_o_JPEG_START_MCU )
#define MPEG_REG_JPEGSEND_MCU       ( MPEG_REGS_BASE + MPEG_REG_o_JPEG_END_MCU   )
#define MPEG_REG_SRAM_RW_OFFSET     ( MPEG_REGS_BASE + MPEG_REG_o_SRAM_RW_OFFSET )
#define MPEG_REG_SRAM_RW_DATA       ( MPEG_REGS_BASE + MPEG_REG_o_SRAM_RW_DATA   )


//define macc interrput type for mpeg
#define MACC_INT_MPEG_FINISH        (0)
#define MACC_INT_MPEG_ERROR         (1)
#define MACC_INT_MPEG_MEMREQ        (2)
#define MACC_INT_MPEG_DBLK          (3)

#endif  //__MACC_MPEG_H__

