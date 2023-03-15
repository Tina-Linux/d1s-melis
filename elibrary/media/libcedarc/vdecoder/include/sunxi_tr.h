/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __SUNXI_TR_H__
#define __SUNXI_TR_H__

#define TRANSFORM_DRIVE_NAME "/dev/transform"

typedef enum TR_PIXEL_FORMAT
{
    TR_FORMAT_ARGB_8888                    = 0x00,//MSB  A-R-G-B  LSB
    TR_FORMAT_ABGR_8888                    = 0x01,
    TR_FORMAT_RGBA_8888                    = 0x02,
    TR_FORMAT_BGRA_8888                    = 0x03,
    TR_FORMAT_XRGB_8888                    = 0x04,
    TR_FORMAT_XBGR_8888                    = 0x05,
    TR_FORMAT_RGBX_8888                    = 0x06,
    TR_FORMAT_BGRX_8888                    = 0x07,
    TR_FORMAT_RGB_888                      = 0x08,
    TR_FORMAT_BGR_888                      = 0x09,
    TR_FORMAT_RGB_565                      = 0x0a,
    TR_FORMAT_BGR_565                      = 0x0b,
    TR_FORMAT_ARGB_4444                    = 0x0c,
    TR_FORMAT_ABGR_4444                    = 0x0d,
    TR_FORMAT_RGBA_4444                    = 0x0e,
    TR_FORMAT_BGRA_4444                    = 0x0f,
    TR_FORMAT_ARGB_1555                    = 0x10,
    TR_FORMAT_ABGR_1555                    = 0x11,
    TR_FORMAT_RGBA_5551                    = 0x12,
    TR_FORMAT_BGRA_5551                    = 0x13,

    /* SP: semi-planar, P:planar, I:interleaved
     * UVUV: U in the LSBs;     VUVU: V in the LSBs */
    TR_FORMAT_YUV444_I_AYUV                = 0x40,//MSB  A-Y-U-V  LSB, reserved
    TR_FORMAT_YUV444_I_VUYA                = 0x41,//MSB  V-U-Y-A  LSB
    TR_FORMAT_YUV422_I_YVYU                = 0x42,//MSB  Y-V-Y-U  LSB
    TR_FORMAT_YUV422_I_YUYV                = 0x43,//MSB  Y-U-Y-V  LSB
    TR_FORMAT_YUV422_I_UYVY                = 0x44,//MSB  U-Y-V-Y  LSB
    TR_FORMAT_YUV422_I_VYUY                = 0x45,//MSB  V-Y-U-Y  LSB
    TR_FORMAT_YUV444_P                     = 0x46,//MSB  P3-2-1-0 LSB,  YYYY UUUU VVVV, reserved
    TR_FORMAT_YUV422_P                     = 0x47,//MSB  P3-2-1-0 LSB   YYYY UU   VV
    TR_FORMAT_YUV420_P                     = 0x48,//MSB  P3-2-1-0 LSB   YYYY U    V  //**yv12
    TR_FORMAT_YUV411_P                     = 0x49,//MSB  P3-2-1-0 LSB   YYYY U    V
    TR_FORMAT_YUV422_SP_UVUV               = 0x4a,//MSB  V-U-V-U  LSB
    TR_FORMAT_YUV422_SP_VUVU               = 0x4b,//MSB  U-V-U-V  LSB
    TR_FORMAT_YUV420_SP_UVUV               = 0x4c,//**NV21
    TR_FORMAT_YUV420_SP_VUVU               = 0x4d,//**NV12
    TR_FORMAT_YUV411_SP_UVUV               = 0x4e,
    TR_FORMAT_YUV411_SP_VUVU               = 0x4f,
}tr_pixel_format;

typedef enum TR_MODE
{
    TR_ROT_0        = 0x0,//rotate clockwise 0 ROTgree
    TR_ROT_90       = 0x1,//rotate clockwise 90 ROTgree
    TR_ROT_180      = 0x2,//rotate clockwise 180 ROTgree
    TR_ROT_270      = 0x3,//rotate clockwise 270 ROTgree
    TR_HFLIP        = 0x4,//horizontal flip
    TR_HFLIP_ROT_90 = 0x5,//first rotate clockwise 90 ROTgree then horizontal flip
    TR_VFLIP        = 0x6,//vertical flip
    TR_VFLIP_ROT_90 = 0x7,//first rotate clockwise 90 ROTgree then vertical flip
}tr_mode;

typedef struct TR_RECT
{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
}tr_rect;

typedef struct TR_FRAME
{
    unsigned char fmt;
    unsigned char haddr[3];
    unsigned int  laddr[3];
    unsigned int  pitch[3]; /* line stride of fb */
    unsigned int  height[3];
}tr_frame;

typedef struct TR_INFO
{
    tr_mode mode;

    tr_frame   src_frame;
    tr_rect    src_rect;

    tr_frame   dst_frame;
    tr_rect    dst_rect;

    int fd;
}tr_info;

typedef enum tag_TR_CMD
{
    TR_REQUEST     = 0x03,
    TR_RELEASE     = 0x04,
    TR_COMMIT      = 0x05,
    TR_QUERY       = 0x06,
    TR_SET_TIMEOUT = 0x07,
}tr_cmd_t;

#endif
