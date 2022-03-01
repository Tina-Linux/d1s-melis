#ifndef  __MOD_CODEC_CMD_
#define  __MOD_CODEC_CMD_
#include <typedef.h>

typedef struct JpegFrameInfo__
{
    void *jpegData;
    __u32 jpegData_len;

    __s32 pic_width;
    __s32 pic_height;

    unsigned long yuv_buf;

    unsigned long y_buf;
    unsigned long u_buf;
    unsigned long v_buf;

} stJpegFrameInfo;



typedef enum __MOD_CODEC_CMD_E
{
    MPEJ_CODEC_CMD_INIT,
    MPEJ_CODEC_CMD_ENCODER,
    MPEJ_CODEC_CMD_DECODER,

    MPEJ_CODEC_CMD_,
} _mod_codec_cmd_e;

#endif
