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
#ifndef __BDF_H__
#define __BDF_H__


/*
 * Based on bdf.h,v 1.16 2000/03/16 20:08:51 mleisher
 */

#include <ft2build.h>
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_STREAM_H


FT_BEGIN_HEADER


/* Imported from bdfP.h */

#define _bdf_glyph_modified( map, e )                 \
    ( (map)[(e) >> 5] & ( 1 << ( (e) & 31 ) ) )
#define _bdf_set_glyph_modified( map, e )              \
    ( (map)[(e) >> 5] |= ( 1 << ( (e) & 31 ) ) )
#define _bdf_clear_glyph_modified( map, e )             \
    ( (map)[(e) >> 5] &= ~( 1 << ( (e) & 31 ) ) )

/* end of bdfP.h */


/*************************************************************************/
/*                                                                       */
/* BDF font options macros and types.                                    */
/*                                                                       */
/*************************************************************************/


#define BDF_CORRECT_METRICS  0x01 /* Correct invalid metrics when loading. */
#define BDF_KEEP_COMMENTS    0x02 /* Preserve the font comments.           */
#define BDF_KEEP_UNENCODED   0x04 /* Keep the unencoded glyphs.            */
#define BDF_PROPORTIONAL     0x08 /* Font has proportional spacing.        */
#define BDF_MONOWIDTH        0x10 /* Font has mono width.                  */
#define BDF_CHARCELL         0x20 /* Font has charcell spacing.            */

#define BDF_ALL_SPACING  ( BDF_PROPORTIONAL | \
                           BDF_MONOWIDTH    | \
                           BDF_CHARCELL     )

#define BDF_DEFAULT_LOAD_OPTIONS  ( BDF_CORRECT_METRICS | \
                                    BDF_KEEP_COMMENTS   | \
                                    BDF_KEEP_UNENCODED  | \
                                    BDF_PROPORTIONAL    )


typedef struct  bdf_options_t_
{
    int            correct_metrics;
    int            keep_unencoded;
    int            keep_comments;
    int            font_spacing;

} bdf_options_t;


/* Callback function type for unknown configuration options. */
typedef int
(*bdf_options_callback_t)(bdf_options_t  *opts,
                          char          **params,
                          unsigned long   nparams,
                          void           *client_data);


/*************************************************************************/
/*                                                                       */
/* BDF font property macros and types.                                   */
/*                                                                       */
/*************************************************************************/


#define BDF_ATOM      1
#define BDF_INTEGER   2
#define BDF_CARDINAL  3


/* This structure represents a particular property of a font. */
/* There are a set of defaults and each font has their own.   */
typedef struct  bdf_property_t_
{
    char  *name;         /* Name of the property.   */
    int    format;       /* Format of the property. */
    int    builtin;      /* A builtin property.     */
    union
    {
        char          *atom;
        long           int32;
        unsigned long  card32;

    } value;             /* Value of the property.  */

} bdf_property_t;


/*************************************************************************/
/*                                                                       */
/* BDF font metric and glyph types.                                      */
/*                                                                       */
/*************************************************************************/


typedef struct  bdf_bbx_t_
{
    unsigned short  width;
    unsigned short  height;

    short           x_offset;
    short           y_offset;

    short           ascent;
    short           descent;

} bdf_bbx_t;


typedef struct  bdf_glyph_t_
{
    char           *name;        /* Glyph name.                          */
    long            encoding;    /* Glyph encoding.                      */
    unsigned short  swidth;      /* Scalable width.                      */
    unsigned short  dwidth;      /* Device width.                        */
    bdf_bbx_t       bbx;         /* Glyph bounding box.                  */
    unsigned char  *bitmap;      /* Glyph bitmap.                        */
    unsigned long   bpr;         /* Number of bytes used per row.        */
    unsigned short  bytes;       /* Number of bytes used for the bitmap. */

} bdf_glyph_t;


typedef struct  _hashnode_
{
    const char  *key;
    void        *data;

} _hashnode, *hashnode;


typedef struct  hashtable_
{
    int        limit;
    int        size;
    int        used;
    hashnode  *table;

} hashtable;


typedef struct  bdf_glyphlist_t_
{
    unsigned short  pad;          /* Pad to 4-byte boundary.              */
    unsigned short  bpp;          /* Bits per pixel.                      */
    long            start;        /* Beginning encoding value of glyphs.  */
    long            end;          /* Ending encoding value of glyphs.     */
    bdf_glyph_t    *glyphs;       /* Glyphs themselves.                   */
    unsigned long   glyphs_size;  /* Glyph structures allocated.          */
    unsigned long   glyphs_used;  /* Glyph structures used.               */
    bdf_bbx_t       bbx;          /* Overall bounding box of glyphs.      */

} bdf_glyphlist_t;


typedef struct  bdf_font_t_
{
    char            *name;           /* Name of the font.                   */
    bdf_bbx_t        bbx;            /* Font bounding box.                  */

    long             point_size;     /* Point size of the font.             */
    unsigned long    resolution_x;   /* Font horizontal resolution.         */
    unsigned long    resolution_y;   /* Font vertical resolution.           */

    int              spacing;        /* Font spacing value.                 */

    unsigned short   monowidth;      /* Logical width for monowidth font.   */

    long             default_char;   /* Encoding of the default glyph.      */

    long             font_ascent;    /* Font ascent.                        */
    long             font_descent;   /* Font descent.                       */

    unsigned long    glyphs_size;    /* Glyph structures allocated.         */
    unsigned long    glyphs_used;    /* Glyph structures used.              */
    bdf_glyph_t     *glyphs;         /* Glyphs themselves.                  */

    unsigned long    unencoded_size; /* Unencoded glyph struct. allocated.  */
    unsigned long    unencoded_used; /* Unencoded glyph struct. used.       */
    bdf_glyph_t     *unencoded;      /* Unencoded glyphs themselves.        */

    unsigned long    props_size;     /* Font properties allocated.          */
    unsigned long    props_used;     /* Font properties used.               */
    bdf_property_t  *props;          /* Font properties themselves.         */

    char            *comments;       /* Font comments.                      */
    unsigned long    comments_len;   /* Length of comment string.           */

    bdf_glyphlist_t  overflow;       /* Storage used for glyph insertion.   */

    void            *internal;       /* Internal data for the font.         */

    unsigned long    nmod[2048];     /* Bitmap indicating modified glyphs.  */
    unsigned long    umod[2048];     /* Bitmap indicating modified          */
    /* unencoded glyphs.                   */
    unsigned short   modified;       /* Boolean indicating font modified.   */
    unsigned short   bpp;            /* Bits per pixel.                     */

    FT_Memory        memory;

    bdf_property_t  *user_props;
    unsigned long    nuser_props;
    hashtable        proptbl;

} bdf_font_t;


/*************************************************************************/
/*                                                                       */
/* Types for load/save callbacks.                                        */
/*                                                                       */
/*************************************************************************/


/* Error codes. */
#define BDF_MISSING_START       -1
#define BDF_MISSING_FONTNAME    -2
#define BDF_MISSING_SIZE        -3
#define BDF_MISSING_CHARS       -4
#define BDF_MISSING_STARTCHAR   -5
#define BDF_MISSING_ENCODING    -6
#define BDF_MISSING_BBX         -7

#define BDF_OUT_OF_MEMORY      -20

#define BDF_INVALID_LINE      -100


/*************************************************************************/
/*                                                                       */
/* BDF font API.                                                         */
/*                                                                       */
/*************************************************************************/

FT_LOCAL(FT_Error)
bdf_load_font(FT_Stream       stream,
              FT_Memory       memory,
              bdf_options_t  *opts,
              bdf_font_t **font);

FT_LOCAL(void)
bdf_free_font(bdf_font_t  *font);

FT_LOCAL(bdf_property_t *)
bdf_get_property(char        *name,
                 bdf_font_t  *font);

FT_LOCAL(bdf_property_t *)
bdf_get_font_property(bdf_font_t  *font,
                      const char  *name);


FT_END_HEADER


#endif /* __BDF_H__ */


/* END */
