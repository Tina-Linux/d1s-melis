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
#ifndef __PCF_H__
#define __PCF_H__


#include <ft2build.h>
#include FT_INTERNAL_DRIVER_H
#include FT_INTERNAL_STREAM_H


FT_BEGIN_HEADER

typedef struct  PCF_TableRec_
{
    FT_ULong  type;
    FT_ULong  format;
    FT_ULong  size;
    FT_ULong  offset;

} PCF_TableRec, *PCF_Table;


typedef struct  PCF_TocRec_
{
    FT_ULong   version;
    FT_ULong   count;
    PCF_Table  tables;

} PCF_TocRec, *PCF_Toc;


typedef struct  PCF_ParsePropertyRec_
{
    FT_Long  name;
    FT_Byte  isString;
    FT_Long  value;

} PCF_ParsePropertyRec, *PCF_ParseProperty;


typedef struct  PCF_PropertyRec_
{
    FT_String  *name;
    FT_Byte     isString;

    union
    {
        FT_String  *atom;
        FT_Long     integer;
        FT_ULong    cardinal;

    } value;

} PCF_PropertyRec, *PCF_Property;


typedef struct  PCF_Compressed_MetricRec_
{
    FT_Byte  leftSideBearing;
    FT_Byte  rightSideBearing;
    FT_Byte  characterWidth;
    FT_Byte  ascent;
    FT_Byte  descent;

} PCF_Compressed_MetricRec, *PCF_Compressed_Metric;


typedef struct  PCF_MetricRec_
{
    FT_Short  leftSideBearing;
    FT_Short  rightSideBearing;
    FT_Short  characterWidth;
    FT_Short  ascent;
    FT_Short  descent;
    FT_Short  attributes;
    FT_ULong  bits;

} PCF_MetricRec, *PCF_Metric;


typedef struct  PCF_AccelRec_
{
    FT_Byte        noOverlap;
    FT_Byte        constantMetrics;
    FT_Byte        terminalFont;
    FT_Byte        constantWidth;
    FT_Byte        inkInside;
    FT_Byte        inkMetrics;
    FT_Byte        drawDirection;
    FT_Long        fontAscent;
    FT_Long        fontDescent;
    FT_Long        maxOverlap;
    PCF_MetricRec  minbounds;
    PCF_MetricRec  maxbounds;
    PCF_MetricRec  ink_minbounds;
    PCF_MetricRec  ink_maxbounds;

} PCF_AccelRec, *PCF_Accel;


typedef struct  PCF_EncodingRec_
{
    FT_Long    enc;
    FT_UShort  glyph;

} PCF_EncodingRec, *PCF_Encoding;


typedef struct  PCF_FaceRec_
{
    FT_FaceRec     root;

    FT_StreamRec   gzip_stream;
    FT_Stream      gzip_source;

    char          *charset_encoding;
    char          *charset_registry;

    PCF_TocRec     toc;
    PCF_AccelRec   accel;

    int            nprops;
    PCF_Property   properties;

    FT_Long        nmetrics;
    PCF_Metric     metrics;
    FT_Long        nencodings;
    PCF_Encoding   encodings;

    FT_Short       defaultChar;

    FT_ULong       bitmapsFormat;

    FT_CharMap     charmap_handle;
    FT_CharMapRec  charmap;  /* a single charmap per face */

} PCF_FaceRec, *PCF_Face;


/* macros for pcf font format */

#define LSBFirst  0
#define MSBFirst  1

#define PCF_FILE_VERSION        ( ( 'p' << 24 ) | \
                                  ( 'c' << 16 ) | \
                                  ( 'f' <<  8 ) | 1 )
#define PCF_FORMAT_MASK         0xFFFFFF00UL

#define PCF_DEFAULT_FORMAT      0x00000000UL
#define PCF_INKBOUNDS           0x00000200UL
#define PCF_ACCEL_W_INKBOUNDS   0x00000100UL
#define PCF_COMPRESSED_METRICS  0x00000100UL

#define PCF_FORMAT_MATCH( a, b ) \
    ( ( (a) & PCF_FORMAT_MASK ) == ( (b) & PCF_FORMAT_MASK ) )

#define PCF_GLYPH_PAD_MASK  ( 3 << 0 )
#define PCF_BYTE_MASK       ( 1 << 2 )
#define PCF_BIT_MASK        ( 1 << 3 )
#define PCF_SCAN_UNIT_MASK  ( 3 << 4 )

#define PCF_BYTE_ORDER( f ) \
    ( ( (f) & PCF_BYTE_MASK ) ? MSBFirst : LSBFirst )
#define PCF_BIT_ORDER( f ) \
    ( ( (f) & PCF_BIT_MASK ) ? MSBFirst : LSBFirst )
#define PCF_GLYPH_PAD_INDEX( f ) \
    ( (f) & PCF_GLYPH_PAD_MASK )
#define PCF_GLYPH_PAD( f ) \
    ( 1 << PCF_GLYPH_PAD_INDEX( f ) )
#define PCF_SCAN_UNIT_INDEX( f ) \
    ( ( (f) & PCF_SCAN_UNIT_MASK ) >> 4 )
#define PCF_SCAN_UNIT( f ) \
    ( 1 << PCF_SCAN_UNIT_INDEX( f ) )
#define PCF_FORMAT_BITS( f )             \
    ( (f) & ( PCF_GLYPH_PAD_MASK | \
              PCF_BYTE_MASK      | \
              PCF_BIT_MASK       | \
              PCF_SCAN_UNIT_MASK ) )

#define PCF_SIZE_TO_INDEX( s )  ( (s) == 4 ? 2 : (s) == 2 ? 1 : 0 )
#define PCF_INDEX_TO_SIZE( b )  ( 1 << b )

#define PCF_FORMAT( bit, byte, glyph, scan )          \
    ( ( PCF_SIZE_TO_INDEX( scan )      << 4 ) | \
      ( ( (bit)  == MSBFirst ? 1 : 0 ) << 3 ) | \
      ( ( (byte) == MSBFirst ? 1 : 0 ) << 2 ) | \
      ( PCF_SIZE_TO_INDEX( glyph )     << 0 ) )

#define PCF_PROPERTIES        ( 1 << 0 )
#define PCF_ACCELERATORS      ( 1 << 1 )
#define PCF_METRICS           ( 1 << 2 )
#define PCF_BITMAPS           ( 1 << 3 )
#define PCF_INK_METRICS       ( 1 << 4 )
#define PCF_BDF_ENCODINGS     ( 1 << 5 )
#define PCF_SWIDTHS           ( 1 << 6 )
#define PCF_GLYPH_NAMES       ( 1 << 7 )
#define PCF_BDF_ACCELERATORS  ( 1 << 8 )

#define GLYPHPADOPTIONS  4 /* I'm not sure about this */

FT_LOCAL(FT_Error)
pcf_load_font(FT_Stream,
              PCF_Face);

FT_END_HEADER

#endif /* __PCF_H__ */


/* END */
