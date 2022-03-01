#ifndef __JPEG_Decoder_H__
#define __JPEG_Decoder_H__

//#include "epdk.h"
#include <typedef.h>
#include <misc/fb.h>
#include <ekernel/ktype.h>
#include <kconfig.h>
#include "mod_codec_cmd.h"


#define TRUE    1
#define FALSE   0

#define MAX_FRM_NUMBER              6
#define MAX_VBV_BUFFER_SIZE         8*1024*1024    //4*512k, 512k is standard for mjpeg?
#define MAX_PIC_NUM_IN_VBV          256

#undef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#undef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))

//define the result of frame decode
typedef enum __PIC_DEC_RESULT
{
    PIC_DEC_ERR_VE_BUSY         = -1,   // video engine is busy
    PIC_DEC_ERR_VFMT_ERR        = -2,   // the format of the video is not supported, fatal error
    PIC_DEC_ERR_COMMON          = -3,   //common decode error

    PIC_DEC_ERR_NONE            = 0,    // decode one picture successed
    PIC_DEC_ERR_FRMNOTDEC       = 1,    // decode one picture successed
    PIC_DEC_ERR_VBS_UNDERFLOW   = 2,    // video bitstream underflow
    PIC_DEC_ERR_NO_FRAMEBUFFER  = 3,    // there is no free frame buffer for decoder
    PIC_DEC_ERR_DEC_VBSEND      = 4,    // decode all video bitstream end

    PIC_DEC_ERR_

} __pic_dec_result;


typedef enum
{
    JPEG420 = 0,
    JPEG411,
    JPEG422,
    JPEG444,
    JPEG422T,
    JPEG400,
    JPEGERR
} JpegPaserFormat;


typedef enum
{
    JPEG_MARKER_ERROR   = 0,
    JPEG_FORMAT_UNSUPPORT,
    JPEG_PARSER_OK
} JpegParserState;

#define MAX_COMPONENTS    3
#define BITS_IN_JSAMPLE   8
#define DCTSIZE2          64
#define NUM_QUANT_TBLS    4
#define NUM_HUFF_TBLS     2
#define MAX_COMPS_IN_SCAN 4


//-----------------------------------------------------------------------------------
#define STC_ID_VALID_CTRL           (1<<0)
#define PTS_VALID_CTRL              (1<<1)
#define FRM_BORDER_CTRL             (1<<2)
#define FIRST_PART_CTRL             (1<<3)
#define LAST_PART_CTRL              (1<<4)
#define SLICE_STRUCTURE_CTRL        (1<<5)
#define MULTIPLE_FRAME_CTRL         (1<<6)
#define FRM_DEC_MODE_VALID_CTRL     (1<<7)
#define FRM_RATE_CTLR               (1<<8)
#define BROKEN_SLICE_FLAG_CTRL      (1<<9)

#ifndef VDRV_TRUE
#define VDRV_TRUE                   (1)
#endif

#ifndef VDRV_FALSE
#define VDRV_FALSE                  (0)
#endif

#ifndef VDRV_SUCCESS
#define VDRV_SUCCESS                (0)
#endif

#ifndef VDRV_FAIL
#define VDRV_FAIL                   (-1)
#endif

#ifndef VDRV_NULL
#define VDRV_NULL                   (0)
#endif

typedef struct
{
    __u16 start_code[16];
    __u8  offset[16];
    __u8  symbol[256];
} JHUFF_TBL;

typedef struct
{
    __s32   component_id;
    __s32   component_index;
    __s16   h_samp_factor;
    __s16   v_samp_factor;
    __s32   quant_tbl_no;

    __s32   dc_tbl_no;
    __s32   ac_tbl_no;
} jpeg_component_info;

typedef struct vdec_vbv_info
{
    __u8    *vbs_addr;
    __u32   vbs_len;
    __u8    *vbv_buf_start;
    __u8    *vbv_buf_end;
    __u32   vbv_buf_start_phy;//physical address.
    __u32   vbv_buf_end_phy;
    __u8    *read_addr;
    __u8    *write_addr;
    __bool  hasPicBorder;
    __bool  hasSliceStructure;

    __u16   PicReadIdx;
    __u16   PicWriteIdx;
    __u16   PicIdxSize;//chunk of vbv buffer.
    __s32   valid_size;//byte of vbv buffer

    __u8    frame_mode;
    __u8    data_num;

    __u8    *cur_read_addr;
    __s32   frame_size;

} vdec_vbv_info;

typedef struct JpegDecoderContex
{
    __s32 width;
    __s32 height;
    __s32 num_components;
    __s32 comps_in_scan;
    __s32 next_restart_num;
    __s32 restart_interval;
    jpeg_component_info cur_comp_info[MAX_COMPONENTS];

    __u8  sos_nb_blocks[MAX_COMPS_IN_SCAN];
    __u8  sos_h_count[MAX_COMPS_IN_SCAN];
    __u8  sos_v_count[MAX_COMPS_IN_SCAN];
    __u8  seq_comp_id[MAX_COMPS_IN_SCAN];

    __s32 max_h_samp_factor;
    __s32 max_v_samp_factor;
    __u16 HorFactor;
    __u16 VerFactor;

    __u16 eoi_reached;
    __s32 JPEG_Format;
    __u16 mcu_width;
    __u16 mcu_height;


    __u16 QTab[4][DCTSIZE2];    /* quantization step for each coefficient */
    JHUFF_TBL dc_huff_tbl[NUM_HUFF_TBLS];
    JHUFF_TBL ac_huff_tbl[NUM_HUFF_TBLS];

    __u8  unread_marker;
    __u8  saw_SOI;
    __u8  saw_SOF;

    __s32 BufferFillOver;
    __s16 BufferFillFirst;

    vdec_vbv_info vbvInfo;
    __u32  JpegLumaOutStartAddr;
    __u32  JpegChroOutStartAddr;


    __u16 HasDht;
    __u16 HasConfigDht;


    __u32 nDispYLineStride;
    __u32 nDispCLineStride;
    __u32 nDispCBufferSize;

    __u32 yuv_buf;
    __u32 y_buf;
    __u32 u_buf;
    __u32 v_buf;


} JpegDecoderContex;

typedef struct JpegDecoderContex *JpegDecoderCtx;


//------------------------------------------------------------------

__s32 JpegDecoderMain(JpegDecoderCtx jpgctx, stJpegFrameInfo *jpginfo);

void *MJPEG_init(void);
__s32 MJPEG_exit(void *mjpegDecCtx);
__s32 MJPEG_decode(void *mjpegDecCtx, void *mjpegFrameInfo);
__s32 MJMACC_init(void *mjpegDecCtx, void *mjpegFrameInfo);
__s32 MJMACC_exit(void *mjpegDecCtx, void *mjpegFrameInfo);
__s32 MJPEG_WaitVeReady(void);



#endif
