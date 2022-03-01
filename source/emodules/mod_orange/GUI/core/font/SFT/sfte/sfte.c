/*
************************************************************************************************************************
*                                                       SFTE 2.0
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : sfte.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.23
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.11.23       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __sfte_c
#define  __sfte_c


#include "cfg.h"
#include "lib/lib.h"
#include "sfte.h"
#include "SFT.h"
#include "sft_decode.h"
#include <log.h>
#include <kapi.h>
#include <elibs_string.h>

#ifdef SIM_PC_WIN
#pragma pack(1)
#endif

#define SFTE_LABEL            _4_char_to_int32( 'S', 'F', 'T', 'E' )
#define INVALID_LABEL         _4_char_to_int32( 'X', 'X', 'X', 'X' )

#define INVALID_PIXEL_SIZE    0
#define INVALID_UNICODE       ((__u16)0xFFFF)


typedef struct SFTE_Bitmap_Size_
{
    size_info_t                  size_info;
    struct  SFTE_Bitmap_Size_   *next;
} SFTE_Bitmap_Size;

typedef struct _glyph_index_t /*added by Derek*/
{
    __u32  unicode;
    __u32  offset;
} glyph_index_t;


typedef struct _sfte_prvt_data_t
{
    __s32         label;                          // handle valid label
    G_FILE       *fp;                             // file pointer to the sft file
    size_info_t  *size_info_list;                 // list of all available pixel sizes
    size_info_t   cur_pixel_size;                 // current pixel size
    __u32         bitmap_buf_size;                // the size of the bitmap buffer
    __u32         unicode;                        // a cache for the current char
#ifdef SFT_USE_MUTEX
    g_mutex_t     mutex;                          // a mutex
#endif // #ifdef SFT_USE_MUTEX
} sfte_prvt_data_t;


static __s32  init_face(SFTE_Face face, G_FILE *fp);
static __Bool is_valid_face(SFTE_Face face);
static void   release_hdl_res(SFTE_Face face);
static __s32  SFTE_Get_Glyph_i(SFTE_Face face, __u16 unicode);
static __u32  SFTE_Get_XAdvance_i(SFTE_Face face, __u16 unicode);
static __s32  SFTE_Set_Pixel_Sizes_i(SFTE_Face face, __u32 pixel_size);
static __s32  SFTE_Get_Index_Table_i(SFTE_Face face);  //added by Derek


SFTE_Face SFTE_New_Face(const char *font_file)
{
    SFTE_Face   face = NULL;
    G_FILE     *fp   = NULL;
    /* open sft file */
    fp = g_fopen(font_file, "rb");

    if (fp == NULL)
    {
        __wrn("Error in opening font file %s. At line %u of file %s.\n", font_file, __LINE__, __FILE__);
        return NULL;
    }

    face = (struct SFTE_FaceRec_ *) g_malloc(sizeof(struct SFTE_FaceRec_));

    if (face == NULL)
    {
        __err("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    g_memset(face, 0, sizeof(struct SFTE_FaceRec_));

    if (init_face(face, fp) != 0)
    {
        __wrn("Error in initialising face. At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    return face;
error:

    if (fp != NULL)
    {
        g_fclose(fp);
    }

    if (face != NULL)
    {
        g_free(face);
    }

    return NULL;
}



__s32  SFTE_Done_Face(SFTE_Face face)
{
    if (!is_valid_face(face))
    {
        __wrn("Fail in destroying face. Because face is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    release_hdl_res(face);
    g_free(face);
    return 0;
}




__s32  SFTE_Set_Pixel_Sizes(SFTE_Face face, __u32 pixel_size)
{
    //sfte_prvt_data_t  *pd;
    //__u8               err;
    __s32              ret;

    if (!is_valid_face(face))
    {
        __log("Fail in setting pixel size %x. Because face is invalid. At line %u of file %s.\n", pixel_size, __LINE__, __FILE__);
        return 0;
    }

    //pd = (sfte_prvt_data_t *)face->hidden;
#ifdef SFT_USE_MUTEX
    g_pend_mutex(pd->mutex, &err);
#endif // #ifdef SFT_USE_MUTEX
//__log("ssssss pixel_size = %d",face->size);
    ret = SFTE_Set_Pixel_Sizes_i(face, pixel_size);
#ifdef SFT_USE_MUTEX
    g_post_mutex(pd->mutex);
#endif // #ifdef SFT_USE_MUTEX
    return ret;
}

__s32 SFTE_Get_Index_Table(SFTE_Face face) //added by Derek
{
    sfte_prvt_data_t  *pd;
    __u8               err;
    __s32              ret;

    if (!is_valid_face(face))
    {
        __log("Fail in getting index table. Because face is invalid. At line %u of file %s.\n", __LINE__, __FILE__);
        return 0;
    }

    pd = (sfte_prvt_data_t *)face->hidden;
#ifdef SFT_USE_MUTEX
    g_pend_mutex(pd->mutex, &err);
#endif // #ifdef SFT_USE_MUTEX
    ret = SFTE_Get_Index_Table_i(face);
#ifdef SFT_USE_MUTEX
    g_post_mutex(pd->mutex);
#endif // #ifdef SFT_USE_MUTEX
    return ret;
}

__s32  SFTE_Get_Glyph(SFTE_Face face, __u16 unicode)
{
    //sfte_prvt_data_t  *pd;
    //__u8               err;
    __u32              ret;

    if (!is_valid_face(face))
    {
        __wrn("Fail in getting glyph of unicode %x. Because face is invalid. At line %u of file %s.\n", unicode, __LINE__, __FILE__);
        return 0;
    }

    //pd = (sfte_prvt_data_t *)face->hidden;
#ifdef SFT_USE_MUTEX
    g_pend_mutex(pd->mutex, &err);
#endif // #ifdef SFT_USE_MUTEX
    ret = SFTE_Get_Glyph_i(face, unicode);
#ifdef SFT_USE_MUTEX
    g_post_mutex(pd->mutex);
#endif // #ifdef SFT_USE_MUTEX
    return ret;
}



__u32  SFTE_Get_XAdvance(SFTE_Face face, __u16 unicode)
{
    //sfte_prvt_data_t  *pd;
    //__u8               err;
    __u32              ret;

    if (!is_valid_face(face))
    {
        __wrn("Fail in getting x advance of unicode %x. Because face is invalid. At line %u of file %s.\n", unicode, __LINE__, __FILE__);
        return 0;
    }

    //pd = (sfte_prvt_data_t *)face->hidden;
#ifdef SFT_USE_MUTEX
    g_pend_mutex(pd->mutex, &err);
#endif // #ifdef SFT_USE_MUTEX
    ret = SFTE_Get_XAdvance_i(face, unicode);
#ifdef SFT_USE_MUTEX
    g_post_mutex(pd->mutex);
#endif // #ifdef SFT_USE_MUTEX
    return ret;
}

static __u32 SFTE_Get_Offset(glyph_index_t *index_list, __u32 unicode, __u32 char_nr)
{
    __s32 mid;
    __s32 low;
    __s32 high;

    if ((index_list[0].unicode > unicode) || (index_list[char_nr - 1].unicode < unicode))
    {
        __wrn("unicode is invalid!\n");
        unicode = 0x20;
    }

    for (low = 0, high = char_nr - 1;   low <= high;)              //find by binary search
    {
        mid = (low + high) >> 1 ;

        if (unicode == index_list[mid].unicode)                         // find destination
        {
            return index_list[mid].offset;
        }
        else if (unicode < index_list[mid].unicode)
        {
            high = mid - 1;    // search in the front half
        }
        else
        {
            low = mid + 1;    // search in the back half
        }

        if (low > high)
        {
            __wrn("start value is big than end!\n");
        }
    }

    return 0;                                         // not find destination
}

static __s32  SFTE_Get_Glyph_i(SFTE_Face face, __u16 unicode)
{
    G_FILE            *fp;
    glyph_t            glyph;
    __u32              pitch_abs;
    __u32              bitmap_size;
    sfte_prvt_data_t  *pd;
    size_info_t       *p;
    __u32              offset;
    pd = (sfte_prvt_data_t *)face->hidden;

    if (pd->cur_pixel_size.pixel_size == INVALID_PIXEL_SIZE)
    {
        __wrn("Fail in getting glyph of unicode %x. Because pixel size is NOT valid.\n", unicode);
        return -1;
    }

    /* check whether the input unicode is the current unicode */
    if (pd->unicode != INVALID_UNICODE && pd->unicode == unicode)
    {
        return 0;
    }

    fp = pd->fp;
    p  = &(pd->cur_pixel_size);
    offset = SFTE_Get_Offset((glyph_index_t *)(face->index_table), unicode, face->num_glyphs);

    if (offset == 0)                    // the char doesn't been contained.
    {
        __inf("offset is invalid!\n");
        return -1;
    }

    /* get glyph's information */
    g_fseek(fp, offset, SEEK_SET);
    g_fread(&glyph, 1, sizeof(glyph), fp);
    face->glyph->advance.x    = glyph.advance.x  ;
    face->glyph->advance.y    = glyph.advance.y  ;
    face->glyph->bitmap_left  = glyph.bitmap_left;
    face->glyph->bitmap_top   = glyph.bitmap_top ;
    face->glyph->bitmap.rows  = glyph.rows;
    face->glyph->bitmap.width = glyph.width;
    face->glyph->bitmap.pitch = glyph.pitch;

    switch (glyph.pixel_mode)
    {
        case SFT_PIXEL_MODE_MONO:
            face->glyph->bitmap.pixel_mode = SFTE_PIXEL_MODE_MONO;
            break;

        case SFT_PIXEL_MODE_GRAY:
            face->glyph->bitmap.pixel_mode = SFTE_PIXEL_MODE_GRAY;
            break;

        default :
            __wrn("Error. Inlegal pixel mode %u. At line %u of file %s.\n", glyph.pixel_mode, __LINE__, __FILE__);
            return -1;
    }

    /* get bitmap matrix */
    if (glyph.pitch > 0)
    {
        pitch_abs = glyph.pitch;
    }
    else
    {
        pitch_abs = (__u32) - glyph.pitch;
    }

    bitmap_size = glyph.rows * pitch_abs;

    if (bitmap_size != 0)
    {
        if (face->glyph->bitmap.buffer != NULL
            && pd->bitmap_buf_size < bitmap_size)
        {
            __msg("current bitmap buffer size is %u and new bitmap size is %u.\n"
                  "pitch abs is %u and glyph rows is %u.\n", pd->bitmap_buf_size, bitmap_size, pitch_abs, glyph.rows);
            g_free(face->glyph->bitmap.buffer);
            face->glyph->bitmap.buffer = NULL;
            pd->bitmap_buf_size        = 0;
        }

        if (face->glyph->bitmap.buffer == NULL)
        {
            face->glyph->bitmap.buffer = (__u8 *) g_malloc(bitmap_size);

            if (face->glyph->bitmap.buffer == NULL)
            {
                __err("Error in getting glyph of unicode %x. can't allocate memory. At line %u of file %s.\n", unicode, __LINE__, __FILE__);
                return -1;
            }

            pd->bitmap_buf_size = bitmap_size;
        }

        g_fread(face->glyph->bitmap.buffer, 1, bitmap_size, fp);
    }

    pd->unicode = unicode;
    return 0;
}



static __u32  SFTE_Get_XAdvance_i(SFTE_Face face, __u16 unicode)
{
    G_FILE            *fp;
    sfte_prvt_data_t  *pd;
    size_info_t       *p;
    __u8               xadvance;
    pd = (sfte_prvt_data_t *)face->hidden;

    if (pd->cur_pixel_size.pixel_size == INVALID_PIXEL_SIZE)
    {
        __wrn("Fail in getting glyph of unicode %x. Because pixel size is NOT valid.\n", unicode);
        return 0;
    }

    /* check whether the input unicode is the current unicode */
    if (pd->unicode != INVALID_UNICODE && pd->unicode == unicode)
    {
        return face->glyph->advance.x >> 6;
    }

    fp = pd->fp;
    p  = &(pd->cur_pixel_size);
    /* get glyph's xadvance */
    g_fseek(fp, p->glyph_xadvance_table_offset + unicode * sizeof(__u8), SEEK_SET);
    g_fread(&xadvance, 1, sizeof(__u8), fp);

    if (xadvance == 0)                    // the char doesn't been contained.
    {
        return 0;
    }

    return xadvance;
}



static __s32  SFTE_Set_Pixel_Sizes_i(SFTE_Face face, __u32 pixel_size)
{
    size_info_t        *p;
    __u32               border_size;
    __u32               buf_size;
    sfte_prvt_data_t   *pd;
    __u32               i;
    pd = (sfte_prvt_data_t *)face->hidden;
    /* check whether the pixel size is supported by the sft file */
    for (i = 0;  i < face->num_fixed_sizes;  i++)
    {
        p = pd->size_info_list + i;

        if (p->pixel_size == pixel_size)
        {
            if (pd->cur_pixel_size.pixel_size == pixel_size)     // is the current size
            {
            	__inf("ccccccccc is the current size");
                return 0;
            }

            /* allocate bitmap buffer */
            border_size = pixel_size + (pixel_size >> 1);
            buf_size    = border_size * border_size;

            if (face->glyph->bitmap.buffer != NULL
                && pd->bitmap_buf_size < buf_size)
            {
                g_free(face->glyph->bitmap.buffer);
                face->glyph->bitmap.buffer = NULL;
                pd->bitmap_buf_size        = 0;
            }

            if (face->glyph->bitmap.buffer == NULL)
            {
                face->glyph->bitmap.buffer = (__u8 *) g_malloc(buf_size);

                if (face->glyph->bitmap.buffer == NULL)
                {
                    __err("Error in setting pixel size. can't allocate memory. At line %u of file %s.\n", __LINE__, __FILE__);
                    return -1;
                }

                pd->bitmap_buf_size = buf_size;
            }

            /* fill size information */
            face->size->metrics.ascender      = p->ascender;
            face->size->metrics.descender     = p->descender;
            face->glyph->metrics.height       = p->height       ;
            face->glyph->metrics.horiBearingY = p->horiBearingY ;
            g_memcpy(&(pd->cur_pixel_size), p, sizeof(size_info_t));
            pd->unicode = INVALID_UNICODE;
            return 0;
        }
    }

    __wrn("Fail in setting pixel size. the pixel size %u is not supported by the sft file.\n", pixel_size);
    return -1;
}

static __s32  SFTE_Get_Index_Table_i(SFTE_Face face) //added by Derek
{
    sfte_prvt_data_t           *pd = NULL;
    pd = (sfte_prvt_data_t *)face->hidden;
    face->index_table = (void *) g_malloc(sizeof(glyph_index_t) * face->num_glyphs);

    if (NULL == face->index_table)
    {
        __err("Error in getting index table. can't allocate memory. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    g_memset(face->index_table, 0, sizeof(glyph_index_t) * face->num_glyphs);
    g_fseek(pd->fp, pd->cur_pixel_size.glyph_index_table_offset, SEEK_SET);
    g_fread(face->index_table, 1, sizeof(glyph_index_t) * face->num_glyphs, pd->fp);
    return 0;
}


static __Bool is_valid_face(SFTE_Face face)
{
    sfte_prvt_data_t  *pd = NULL;

    if (face == NULL)
    {
        return __FALSE;
    }

    pd = (sfte_prvt_data_t *)face->hidden;

    if (pd->label != SFTE_LABEL)
    {
        return __FALSE;
    }

    return __TRUE;
}



static __s32 init_face(SFTE_Face face, G_FILE *fp)
{
    //size_info_t                *size_info_list = NULL;
    sfte_prvt_data_t           *pd = NULL;
    sft_file_head_t             h;
    g_fread(&h, 1, sizeof(h), fp);
    face->num_fixed_sizes = h.pixel_size_nr;
    face->num_glyphs      = h.char_nr;
    face->units_per_EM    = h.units_per_EM;
    face->ascender        = h.ascender;
    face->descender       = h.descender;
    face->height          = h.height;
    face->index_table     = NULL;  //added by Derek
    face->size = (SFTE_SizeRec *) g_malloc(sizeof(SFTE_SizeRec));

    if (face->size == NULL)
    {
        __err("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    /* allcate a buffer to store glyph */
    face->glyph = (struct SFTE_GlyphSlotRec_ *) g_malloc(sizeof(struct SFTE_GlyphSlotRec_));

    if (face->glyph == NULL)
    {
        __err("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    g_memset(face->glyph, 0, sizeof(struct SFTE_GlyphSlotRec_));
    face->hidden = (void *) g_malloc(sizeof(sfte_prvt_data_t));

    if (face->hidden == NULL)
    {
        __err("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
        return -1;
    }

    g_memset(face->hidden, 0, sizeof(sfte_prvt_data_t));
    pd = (sfte_prvt_data_t *)face->hidden;
    /* get the list of all the pixel sizes */
    pd->size_info_list = (size_info_t *) g_malloc(sizeof(size_info_t) * face->num_fixed_sizes);

    if (pd->size_info_list == NULL)
    {
        __err("Error in allocating memory. At line %u of file %s.\n", __LINE__, __FILE__);
        goto error;
    }

    g_fseek(fp, h.pixel_size_tbl_offset, SEEK_SET);
    g_fread(pd->size_info_list, 1, sizeof(size_info_t) * face->num_fixed_sizes, fp);
    pd->label            = SFTE_LABEL;
    pd->fp               = fp;
    pd->cur_pixel_size.pixel_size = INVALID_PIXEL_SIZE;
    pd->bitmap_buf_size  = 0;
    pd->unicode          = INVALID_UNICODE;
#ifdef SFT_USE_MUTEX
    pd->mutex = g_create_mutex();

    if (pd->mutex == NULL)
    {
        __err("Error in creating a mutex.\n");
        goto error;
    }

#endif // #ifdef SFT_USE_MUTEX
    return 0;
error:
    release_hdl_res(face);
    return -1;
}



static void   release_hdl_res(SFTE_Face face)
{
    if (face->hidden != NULL)
    {
        sfte_prvt_data_t  *pd = NULL;
        pd = (sfte_prvt_data_t *)face->hidden;

        if (pd->size_info_list != NULL)
        {
            g_free(pd->size_info_list);
            pd->size_info_list = NULL;
        }

        if (pd->fp != NULL)
        {
            g_fclose(pd->fp);
            pd->fp = NULL;
        }

#ifdef SFT_USE_MUTEX

        if (pd->mutex != NULL)
        {
            __u8  err;
            g_delete_mutex(pd->mutex, &err);
            pd->mutex = NULL;
        }

#endif // #ifdef SFT_USE_MUTEX
        pd->label = INVALID_LABEL;
        g_free(face->hidden);
        face->hidden = NULL;
    }

    if (face->glyph != NULL)
    {
        if (face->glyph->bitmap.buffer != NULL)
        {
            g_free(face->glyph->bitmap.buffer);
            face->glyph->bitmap.buffer = NULL;
        }

        g_free(face->glyph);
        face->glyph = NULL;
    }

    if (face->size != NULL)
    {
        g_free(face->size);
        face->size = NULL;
    }

    if (face->index_table != NULL)  //added by Derek
    {
        g_free(face->index_table);
        face->index_table = NULL;
    }
}



#endif     //  ifndef __sfte_c

/* end of sfte.c */
