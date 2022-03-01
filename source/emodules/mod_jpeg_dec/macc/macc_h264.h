/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               VE Driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : macc_h264.h
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-13 14:38
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-13 14:38    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#ifndef __MACC_H264_H__
#define __MACC_H264_H__


//macc io base
extern __u32    MACC_REGS_BASE;


//------------------------------------------------------
// define registers for h264 macc
//------------------------------------------------------
#define H264_REGS_BASE              (MACC_REGS_BASE + 0x200)

/* OFFSET */
#define H264_REG_o_SPS_RBSP                 0x00
#define H264_REG_o_PPS_RBSP                 0x04
#define H264_REG_o_SHS1                     0x08
#define H264_REG_o_SHS2                     0x0c
#define H264_REG_o_SHS_WP                   0x10
#define H264_REG_o_SHS_QP                   0x1c
#define H264_REG_o_FUNCTION_CTRL            0x20
#define H264_REG_o_TRIGGER_TYPE             0x24
#define H264_REG_o_FUNCTION_STATUS          0x28
#define H264_REG_o_CRTMB_NUM                0x2c
#define H264_REG_o_VLD_BIT_ADDR             0x30
#define H264_REG_o_VLD_OFFSET               0x34
#define H264_REG_o_VLD_BIT_LENGTH           0x38
#define H264_REG_o_VBV_END_ADDR             0x3c
#define H264_REG_o_SD_ROT_CTRL              0x40
#define H264_REG_o_ROT_LUMA_ADDR            0x44
#define H264_REG_o_ROT_CHROMA_ADDR          0x48
#define H264_REG_o_SHS_FRMBUF_IDX           0x4c
#define H264_REG_o_FIELD_INTRA_ADDR         0x50
#define H264_REG_o_NEIGHBOR_ADDR            0x54
#define H264_REG_o_MBA                      0x60
#define H264_REG_o_MB_NEIGHBOUR1            0x64
#define H264_REG_o_MB_NEIGHBOUR2            0x68
#define H264_REG_o_MB_NEIGHBOUR3            0x6c
#define H264_REG_o_MB_NEIGHBOUR4            0x70
#define H264_REG_o_MB_NEIGHBOUR5            0x74
#define H264_REG_o_MB_NEIGHBOUR6            0x78
#define H264_REG_o_MB_NEIGHBOUR7            0x7c
#define H264_REG_o_MB_MVBS_INFO             0x80
#define H264_REG_o_MB_CUR_INFO              0x84
#define H264_REG_o_MB_CBP_CUR               0x8c
#define H264_REG_o_MB_QP_CUR                0x90
#define H264_REG_o_MB_DISTSCALE_CUR1        0x94
#define H264_REG_o_MB_DISTSCALE_CUR2        0x98
#define H264_REG_o_CUR_MB_MV_L0             0x9c
#define H264_REG_o_CUR_MB_MV_L1             0xa0
#define H264_REG_o_CUR_MB_INTRA_MODE1       0xa4
#define H264_REG_o_CUR_MB_INTRA_MODE2       0xa8
#define H264_REG_o_ERROR_CASE               0xb8
#define H264_REG_o_SLICE_QUEUE_CMD_ADDR     0xc0
#define H264_REG_o_SLICE_QUEUE_LENGTH       0xc4
#define H264_REG_o_SLICE_QUEUE_START        0xc8
#define H264_REG_o_SLICE_QUEUE_STATUS       0xcc
#define H264_REG_o_INTRA_ABOVE_ADDR         0xd0
#define H264_REG_o_DBLK_ABOVE_ADDR          0xd4
#define H264_REG_o_RETURN_DATA              0xdc
#define H264_REG_o_RW_OFFSET                0xe0
#define H264_REG_o_RW_DATA                  0xe4


/* REGISTERS */
#define H264_REG_SPS_RBSP                   (H264_REGS_BASE+H264_REG_o_SPS_RBSP            )
#define H264_REG_PPS_RBSP                   (H264_REGS_BASE+H264_REG_o_PPS_RBSP            )
#define H264_REG_SHS1                       (H264_REGS_BASE+H264_REG_o_SHS1                )
#define H264_REG_SHS2                       (H264_REGS_BASE+H264_REG_o_SHS2                )
#define H264_REG_SHS_WP                     (H264_REGS_BASE+H264_REG_o_SHS_WP              )
#define H264_REG_SHS_QP                     (H264_REGS_BASE+H264_REG_o_SHS_QP              )
#define H264_REG_FUNCTION_CTRL              (H264_REGS_BASE+H264_REG_o_FUNCTION_CTRL       )
#define H264_REG_TRIGGER_TYPE               (H264_REGS_BASE+H264_REG_o_TRIGGER_TYPE        )
#define H264_REG_FUNCTION_STATUS            (H264_REGS_BASE+H264_REG_o_FUNCTION_STATUS     )
#define H264_REG_CRTMB_NUM                  (H264_REGS_BASE+H264_REG_o_CRTMB_NUM           )
#define H264_REG_VLD_BIT_ADDR               (H264_REGS_BASE+H264_REG_o_VLD_BIT_ADDR        )
#define H264_REG_VLD_OFFSET                 (H264_REGS_BASE+H264_REG_o_VLD_OFFSET          )
#define H264_REG_VLD_BIT_LENGTH             (H264_REGS_BASE+H264_REG_o_VLD_BIT_LENGTH      )
#define H264_REG_VBV_END_ADDR               (H264_REGS_BASE+H264_REG_o_VBV_END_ADDR        )
#define H264_REG_SD_ROT_CTRL                (H264_REGS_BASE+H264_REG_o_SD_ROT_CTRL         )
#define H264_REG_ROT_LUMA_ADDR              (H264_REGS_BASE+H264_REG_o_ROT_LUMA_ADDR       )
#define H264_REG_ROT_CHROMA_ADDR            (H264_REGS_BASE+H264_REG_o_ROT_CHROMA_ADDR     )
#define H264_REG_SHS_FRMBUF_IDX             (H264_REGS_BASE+H264_REG_o_SHS_FRMBUF_IDX      )
#define H264_REG_FIELD_INTRA_ADDR           (H264_REGS_BASE+H264_REG_o_FIELD_INTRA_ADDR    )
#define H264_REG_NEIGHBOR_ADDR              (H264_REGS_BASE+H264_REG_o_NEIGHBOR_ADDR       )
#define H264_REG_MBA                        (H264_REGS_BASE+H264_REG_o_MBA                 )
#define H264_REG_MB_NEIGHBOUR1              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR1       )
#define H264_REG_MB_NEIGHBOUR2              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR2       )
#define H264_REG_MB_NEIGHBOUR3              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR3       )
#define H264_REG_MB_NEIGHBOUR4              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR4       )
#define H264_REG_MB_NEIGHBOUR5              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR5       )
#define H264_REG_MB_NEIGHBOUR6              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR6       )
#define H264_REG_MB_NEIGHBOUR7              (H264_REGS_BASE+H264_REG_o_MB_NEIGHBOUR7       )
#define H264_REG_MB_MVBS_INFO               (H264_REGS_BASE+H264_REG_o_MB_MVBS_INFO        )
#define H264_REG_MB_CUR_INFO                (H264_REGS_BASE+H264_REG_o_MB_CUR_INFO         )
#define H264_REG_MB_CBP_CUR                 (H264_REGS_BASE+H264_REG_o_MB_CBP_CUR          )
#define H264_REG_MB_QP_CUR                  (H264_REGS_BASE+H264_REG_o_MB_QP_CUR           )
#define H264_REG_MB_DISTSCALE_CUR1          (H264_REGS_BASE+H264_REG_o_MB_DISTSCALE_CUR1   )
#define H264_REG_MB_DISTSCALE_CUR2          (H264_REGS_BASE+H264_REG_o_MB_DISTSCALE_CUR2   )
#define H264_REG_CUR_MB_MV_L0               (H264_REGS_BASE+H264_REG_o_CUR_MB_MV_L0        )
#define H264_REG_CUR_MB_MV_L1               (H264_REGS_BASE+H264_REG_o_CUR_MB_MV_L1        )
#define H264_REG_CUR_MB_INTRA_MODE1         (H264_REGS_BASE+H264_REG_o_CUR_MB_INTRA_MODE1  )
#define H264_REG_CUR_MB_INTRA_MODE2         (H264_REGS_BASE+H264_REG_o_CUR_MB_INTRA_MODE2  )
#define H264_REG_ERROR_CASE                 (H264_REGS_BASE+H264_REG_o_ERROR_CASE          )
#define H264_REG_SLICE_QUEUE_CMD_ADDR       (H264_REGS_BASE+H264_REG_o_SLICE_QUEUE_CMD_ADDR)
#define H264_REG_SLICE_QUEUE_LENGTH         (H264_REGS_BASE+H264_REG_o_SLICE_QUEUE_LENGTH  )
#define H264_REG_SLICE_QUEUE_START          (H264_REGS_BASE+H264_REG_o_SLICE_QUEUE_START   )
#define H264_REG_SLICE_QUEUE_STATUS         (H264_REGS_BASE+H264_REG_o_SLICE_QUEUE_STATUS  )
#define H264_REG_INTRA_ABOVE_ADDR           (H264_REGS_BASE+H264_REG_o_INTRA_ABOVE_ADDR    )
#define H264_REG_DBLK_ABOVE_ADDR            (H264_REGS_BASE+H264_REG_o_DBLK_ABOVE_ADDR     )
#define H264_REG_RETURN_DATA                (H264_REGS_BASE+H264_REG_o_RETURN_DATA         )
#define H264_REG_RW_OFFSET                  (H264_REGS_BASE+H264_REG_o_RW_OFFSET           )
#define H264_REG_RW_DATA                    (H264_REGS_BASE+H264_REG_o_RW_DATA             )


//define macc interrput type for h264
#define MACC_INT_H264_SLICEEND              (0)
#define MACC_INT_H264_ERROR                 (1)
#define MACC_INT_H264_VDATAREQ              (2)
#define MACC_INT_H264_OVTIME                (3)
#define MACC_INT_H264_SQDEC                 (4)


#endif  //__MACC_H264_H__

