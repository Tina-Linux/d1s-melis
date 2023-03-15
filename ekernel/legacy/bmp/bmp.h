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
#include <typedef.h>
#include <rtthread.h>

#define PALETTE_COLOR_BYTE_COUNT        4

enum _origin_pos_e
{
    ORIGIN_POS_UPPER_LEFT,
    ORIGIN_POS_LOWER_LEFT
};

typedef struct _full_color_t
{
    uint8_t     B;
    uint8_t     G;
    uint8_t     R;
    uint8_t     A;
} __attribute__((packed)) full_color_t;

typedef struct _bmp_file_head_t
{
    char        bfType[2];        // "BM"
    uint32_t    bfSize;           // total size of bmp file
    uint32_t    bfReserved;       // reserved field
    uint32_t    bfOffBits;        // pixel matrix offset from file head
} __attribute__((packed)) bmp_file_head_t;

typedef struct _bmp_info_head_t
{
    uint32_t    biSize;           // the size of bmp information head
    uint32_t    biWidth;          // the width of bmp, unit: pixel
    int32_t     biHeight;         // the height of bmp, unit:pixel
    uint16_t    biPlanes;         // always 1
    uint16_t    biBitCount;       // bits per pixel. 1-mono, 4-16color, 8-256color, 24-truecolor
    uint32_t    biCompression;    // copression format. 0: no compression.
    uint32_t    biSizeImage;      // the size of pixel matirx.
    uint32_t    biXPelsPerMeter;  // horizontal dis, unit : pixel/meter
    uint32_t    biYPelsPerMeter;  // vertical dis, unit : pixel/meter
    uint32_t    biClrUsed;        // number of used colors, 0 means 2^biBitCount
    uint32_t    biClrImportant;   // number of important colors, 0 means that all colors are important.
} __attribute__((packed)) bmp_info_head_t;

typedef struct _HBMP_i_t
{
    uint32_t    byte_count;
    uint16_t    bitcount;
    uint32_t    width;
    int32_t     height;
    uint32_t    row_size;
    uint32_t    matrix_off;
} HBMP_i_t, *HBMP_i;


typedef void  *HBMP;
extern HBMP    open_bmp(const char *bmp_file, HBMP_i_t *hbmp_hdl_buf);
extern HBMP    open_ram_bmp(const unsigned long base, const __u32 size, HBMP_i_t *hbmp_hdl_buf);
extern int32_t   close_bmp(HBMP hbmp);
extern uint32_t   get_bitcount(HBMP hbmp);
extern uint32_t   get_width(HBMP hbmp);
extern int32_t   get_height(HBMP hbmp);
extern uint32_t   get_rowsize(HBMP hbmp);
extern int32_t   get_matrix(HBMP hbmp, void *buf);
extern int32_t   read_pixel(HBMP hbmp, uint32_t x, uint32_t y, full_color_t *color_p, int32_t origin_pos);
extern int32_t   read_index(HBMP hbmp, uint32_t x, uint32_t y, uint8_t *index_p, int32_t origin_pos);

#endif     //  ifndef __bmp_h