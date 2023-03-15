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
#ifndef  __SFT_h
#define  __SFT_h


#ifdef SFT_OS_WIN32
#define __PACKED__
#else
#define __PACKED__        __packed
#endif

#ifdef MAGIC_SIZE
#undef MAGIC_SIZE
#endif
#define MAGIC_SIZE            4
#define VERSION_SIZE          4

#define SFT_MAGIC             "SFTT"
#define VERSION               "2.OO"


/*************************************************************************/
/*                                                                       */
/* <Enum>                                                                */
/*    sft_pixel_mode_e                                                   */
/*                                                                       */
/* <Description>                                                         */
/*    An enumeration type used to describe the format of pixels in a     */
/*    given bitmap.                                                      */
/*                                                                       */
/* <Values>                                                              */
/*    SFT_PIXEL_MODE_MONO ::                                             */
/*      A monochrome bitmap, using 1 bit per pixel.  Note that pixels    */
/*      are stored in most-significant order (MSB), which means that     */
/*      the left-most pixel in a byte has value 128.                     */
/*                                                                       */
/*    SFT_PIXEL_MODE_GRAY ::                                             */
/*      An 8-bit bitmap, generally used to represent anti-aliased glyph  */
/*      images.  Each pixel is stored in one byte.  Note that the number */
/*      of value `gray' levels is stored in the `num_bytes' field of     */
/*      the @FT_Bitmap structure (it generally is 256).                  */
/*************************************************************************/
typedef enum _sft_pixel_mode_e
{
    SFT_PIXEL_MODE_MONO = 1,
    SFT_PIXEL_MODE_GRAY = 2,

    SFT_PIXEL_MODE_
} sft_pixel_mode_e;


#ifdef SFT_OS_WIN32
#pragma pack(push, 1)
#endif // #ifdef SFT_OS_WIN32
#ifdef SIM_PC_WIN
#pragma pack(push, 1)
#endif
typedef struct _SFT_Vector
{
    __u16  x;
    __u16  y;
} __attribute__((__packed__)) SFT_Vector;
#ifdef SIM_PC_WIN
#pragma pack(pop)
#endif
#ifdef SFT_OS_WIN32
#pragma pack(pop)
#endif // #ifdef SFT_OS_WIN32


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    glyph_t                                                            */
/*                                                                       */
/* <Fields>                                                              */
/*                                                                       */
/*    advance      :: This is the transformed advance width for the      */
/*                    glyph.                                             */
/*                                                                       */
/*    rows         :: The number of bitmap rows.                         */
/*                                                                       */
/*    width        :: The number of pixels in bitmap row.                */
/*                                                                       */
/*    pitch        :: The pitch's absolute value is the number of bytes  */
/*                    taken by one bitmap row, including padding.        */
/*                    However, the pitch is positive when the bitmap has */
/*                    a `down' flow, and negative when it has an `up'    */
/*                    flow.  In all cases, the pitch is an offset to add */
/*                    to a bitmap pointer in order to go down one row.   */
/*                                                                       */
/*    pixel_mode   :: The pixel mode, i.e., how pixel bits are stored.   */
/*                    See @FT_Pixel_Mode for possible values.            */
/*                                                                       */
/*    bitmap_left  :: This is the bitmap's left bearing expressed        */
/*                    in integer pixels.  Of course, this is only        */
/*                    valid if the format is                             */
/*                    @FT_GLYPH_FORMAT_BITMAP.                           */
/*                                                                       */
/*    bitmap_top   :: This is the bitmap's top bearing expressed in      */
/*                    integer pixels.  Remember that this is the         */
/*                    distance from the baseline to the top-most         */
/*                    glyph scanline, upwards y-coordinates being        */
/*                    *positive*.                                        */
/*                                                                       */
/*************************************************************************/
#ifdef SFT_OS_WIN32
#pragma pack(push, 1)
#endif // #ifdef SFT_OS_WIN32
#ifdef SIM_PC_WIN
#pragma pack(push, 1)
#endif
typedef struct _glyph_t
{
    SFT_Vector  advance;
    __s16       bitmap_left;
    __s16       bitmap_top;
    __u16       rows;
    __u16       width;
    __s16       pitch;
    __s16       pixel_mode;
} __attribute__((__packed__)) glyph_t;
#ifdef SIM_PC_WIN
#pragma pack(pop)
#endif

#ifdef SFT_OS_WIN32
#pragma pack(pop)
#endif // #ifdef SFT_OS_WIN32



typedef struct _size_info_t
{
    __u32  pixel_size;
    __u32  ascender;    /* ascender in 26.6 frac. pixels               */
    __u32  descender;   /* descender in 26.6 frac. pixels              */
    __u32  height;
    __u32  horiBearingY;
    __u32  glyph_index_table_offset;
    __u32  glyph_xadvance_table_offset;
} size_info_t;


typedef struct _sft_file_head_t
{
    char   magic[MAGIC_SIZE];          // magic
    char   version[VERSION_SIZE];      // version
    __u32  file_head_size;             // the size of the SFT file head
    __u32  char_nr;                    // the number of all characters contained in the file
    __u32  pixel_size_nr;              // the number of all pixel sizes supported by the SFT file
    __u32  pixel_size_tbl_offset;      // the offset of pixel size table off the start of the file.
    __u32  units_per_EM;
    __s32  ascender;
    __s32  descender;
    __u32  height;
    __u32  reserved[4];                // reserved
} sft_file_head_t;



#endif     //  ifndef __SFT_h

/* end of SFT.h */
