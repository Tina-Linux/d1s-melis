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
#ifndef  __bmp_h
#define  __bmp_h
#include <log.h>


enum _origin_pos_e
{
    ORIGIN_POS_UPPER_LEFT,
    ORIGIN_POS_LOWER_LEFT
};


#define PALETTE_COLOR_BYTE_COUNT        4

#define spcl_size_align( x, y )         ( ( (x) + (y) - 1 ) & ~( (y) - 1 ) )
//#define abs(x) (x) >= 0 ? (x):-(x)



typedef  struct _full_color_t
{
    __u8  B;
    __u8  G;
    __u8  R;
    __u8  A;
} __attribute__((packed)) full_color_t;


typedef struct _bmp_file_head_t
{
    char    bfType[2];        // "BM"
    __u32   bfSize;           // total size of bmp file
    __u32   bfReserved;       // reserved field
    __u32   bfOffBits;        // pixel matrix offset from file head
} __attribute__((packed)) bmp_file_head_t;


typedef struct _bmp_info_head_t
{
    __u32   biSize;           // the size of bmp information head
    __u32   biWidth;          // the width of bmp, unit: pixel
    __s32   biHeight;         // the height of bmp, unit:pixel
    __u16   biPlanes;         // always 1
    __u16   biBitCount;       // bits per pixel. 1-mono, 4-16color, 8-256color, 24-truecolor
    __u32   biCompression;    // copression format. 0: no compression.
    __u32   biSizeImage;      // the size of pixel matirx.
    __u32   biXPelsPerMeter;  // horizontal dis, unit : pixel/meter
    __u32   biYPelsPerMeter;  // vertical dis, unit : pixel/meter
    __u32   biClrUsed;        // number of used colors, 0 means 2^biBitCount
    __u32   biClrImportant;   // number of important colors, 0 means that all colors are important.
} __attribute__((packed)) bmp_info_head_t;


typedef struct _HBMP_i_t
{
    __u32   byte_count;
    __u16   bitcount;
    __u32   width;
    __s32   height;
    __u32   row_size;
    __u32   matrix_off;
    __u32   real_row_size;//useful data row size
} HBMP_i_t, *HBMP_i;


typedef void*  HBMP;


extern HBMP    open_bmp(const char *bmp_file, HBMP_i_t *hbmp_hdl_buf);
HBMP open_ram_bmp(const void *base, const uint32_t size, HBMP_i_t *hbmp_hdl_buf);
extern __s32   close_bmp(HBMP hbmp);
extern __u32   get_bitcount(HBMP hbmp);
extern __u32   get_width(HBMP hbmp);
extern __s32   get_height(HBMP hbmp);
extern __u32   get_rowsize(HBMP hbmp);
extern __s32   get_matrix(HBMP hbmp, void *buf);
extern __s32   read_pixel(HBMP hbmp, __u32 x, __u32 y, full_color_t *color_p, __s32 origin_pos);
extern __s32   read_index(HBMP hbmp, __u32 x, __u32 y, __u8 *index_p, __s32 origin_pos);




#endif     //  ifndef __bmp_h

/* end of bmp.h  */
