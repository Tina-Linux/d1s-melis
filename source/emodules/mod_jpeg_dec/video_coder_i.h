#ifndef __VIDEO_CODER_I
#define __VIDEO_CODER_I
#include "mod_defs.h"
#include "macc_bsp.h"
#include "mod_codec_cmd.h"
#include "elibs_stdio.h"
#include "elibs_stdlib.h"
#include "elibs_string.h"
#include "log.h"
#include <kapi.h>

typedef struct __HERB_VREC
{
    __u32                   mid;
    void                    *mjpegDecCtx;
} __herb_vcoder_t;

typedef struct
{
    __u8  *vbv_buf_start;
    __u8  *vbv_buf_end;
    __u8  *read_addr;
    __u8  *write_addr;
    __s32 valid_size;
    __s32 lastmb_offset;
    __u32 vbv_buf_start_phy;
    __u32 vbv_buf_end_phy;
} jpeg_vbv_info;

#endif
