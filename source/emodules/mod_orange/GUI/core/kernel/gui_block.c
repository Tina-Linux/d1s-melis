/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
* Moudle:         orange
* File Name£º     gui_mp.c
*
* Author£º        Derek Lai
* Version :       1.0
* Date:           2021-1-12 10:16:23
* Description :   provide mp interface
* Others :        None at present.
* History :
* <Author>        <time>      <version>     <description>
* Derek Lai       16:15:34      1.1.0          Create File
*******************************************************************************************************************
*/
#ifndef __GUI_MP_MIX_C__
#define __GUI_MP_MIX_C__

#include "epdk.h"
#include "mod_orange.h"
#include "gui_core_kernel_private.h"

#define     BLITMASK        0x00000FFF
#define     BLITALPHAMASK   0x0000000F
#define     BLITRFMASK      0x000001F0
#define     BLITBITMASK     0x00000E00

/*
**********************************************************************************************************************
*                                               GUI_FBSuspend
*
* Description: sleep fb, if a fb is free, its frame buffer will be free.
*
* Arguments  :  fb       frame buffer struct
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
static __u8  GUI_MpBlockGetBPP(__u32 block_fmt)
{
    __u8  bpp;

    switch (block_fmt)
    {
        case  PIXEL_MONO_1BPP:
            bpp = 1;
            break;

        case PIXEL_MONO_2BPP:
            bpp = 2;
            break;

        case PIXEL_MONO_4BPP:
            bpp = 4;
            break;

        case PIXEL_MONO_8BPP:
            bpp = 8;
            break;

        case PIXEL_COLOR_RGB655:
        case PIXEL_COLOR_RGB565:
        case PIXEL_COLOR_RGB556:
        case PIXEL_COLOR_ARGB1555:
            bpp = 16;
            break;

        case PIXEL_COLOR_ARGB8888:
            bpp = 32;
            break;

        default:
            bpp = 0;
            break;
    }

    return bpp;
}
/*
**********************************************************************************************************************
*                                               GUI_FBSuspend
*
* Description: sleep fb, if a fb is free, its frame buffer will be free.
*
* Arguments  :  fb       frame buffer struct
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
static __u32 GUI_Pixelfmt2MpPixelfmt(__u32 fmt)
{
    __u32  mp_fmt;

    if (PIXEL_COLOR_ARGB8888 == fmt)
    {
        mp_fmt = fmt - 1;
    }
    else
    {
        mp_fmt = fmt;
    }

    return mp_fmt;
}
/*
**********************************************************************************************************************
*                                               GUI_FBSuspend
*
* Description: sleep fb, if a fb is free, its frame buffer will be free.
*
* Arguments  :  fb       frame buffer struct
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
static __mp_color_t GUI_Color2MpColor(GUI_COLOR ck_value)
{
    __mp_color_t mp_color;
    mp_color.alpha = (__u8)((ck_value & 0xFF000000) >> 24);
    mp_color.red   = (__u8)((ck_value & 0x00FF0000) >> 16);
    mp_color.green = (__u8)((ck_value & 0x0000FF00) >> 8);
    mp_color.blue  = (__u8)(ck_value & 0x000000FF);
    return mp_color;
}


/*
**********************************************************************************************************************
*                                               GUI_BlockCreate
*
* author:
* date:             2011-1-12 10:20:07
* Description:      request mp can to multip sem
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
HBLOCK GUI_BlockCreate(__u32 width, __u32 height, __u32 format, __u8 byte_seq)
{
    __u8            bpp;
    __gui_block_t  *block   = NULL;
    __u32           size    = 0;
    void            *addr   = NULL;
    //RECT clip_rect;
    /* alloc mp block memory*/
    block = (__gui_block_t *)orange_malloc(sizeof(__gui_block_t));

    if (!block)
    {
        ORANGE_WRN("alloc mp block memory failed!\n");
        return NULL;
    }

    bpp = GUI_MpBlockGetBPP(format);

    if (bpp)
    {
        /*get the size of framebuffer*/
        size = (width * height * bpp + 7) >> 3;
        addr = orange_pmalloc(size);

        if (!addr)
        {
            ORANGE_WRN("pmalloc framebuffer failed!\n");
            goto error;
        }
    }

    block->fb_addr[0]           = (__u32)addr;
    block->fb_width             = width;
    block->fb_height            = height;
    block->fb_fmt               = (__pixel_rgbfmt_t)format;
    block->fb_byte_seq          = byte_seq;
    block->block_rect.x         = 0;
    block->block_rect.y         = 0;
    block->block_rect.width     = width;
    block->block_rect.height    = height;
    block->new_fb               = ORANGE_TRUE;
    block->block_swlyr      = GUI_CreateLayer((void *)block->fb_addr[0], block->block_rect.width,
                              block->block_rect.height, block->fb_fmt);

    if (block->block_swlyr == (__hdle)(-1))
    {
        ORANGE_WRN("no software layer fit!\n");
        goto error;
    }

    //block->clip_rect = &clip_rect;
    block->clip_rect.x      = 0;
    block->clip_rect.y      = 0;
    block->clip_rect.width  = block->block_rect.width;
    block->clip_rect.height = block->block_rect.height;
    pthread_mutex_init(&(block->lock), (void *)1);
    return GUI_P2Handle(block);
error:

    if (addr)
    {
        orange_pfree(addr, size);
        addr = NULL;
    }

    if (block)
    {
        orange_mfree(block);
        block = NULL;
    }

    return NULL;
}
/*
**********************************************************************************************************************
*                                               GUI_BlockSel
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
HBLOCK GUI_BlockCreateFrom(RECT *block_rect, __u32  format, __u32 fb_width, __u32 fb_height, void *addr, __u8 byte_seq)
{
    __gui_block_t  *block = NULL;
    //RECT clip_rect;
    /* alloc mp block memory*/
    block = (__gui_block_t *)orange_malloc(sizeof(__gui_block_t));

    if (!block)
    {
        ORANGE_WRN("alloc mp block memory failed!\n");
        return NULL;
    }

    block->fb_addr[0]       = (__u32)addr;
    block->fb_width         = fb_width;
    block->fb_height        = fb_height;
    block->fb_fmt           = (__pixel_rgbfmt_t)format;
    block->fb_byte_seq      = byte_seq;

    if (block_rect == NULL)
    {
        block->block_rect.x         = 0;
        block->block_rect.y         = 0;
        block->block_rect.width     = block->fb_width;
        block->block_rect.height    = block->fb_height;
    }
    else
    {
        block->block_rect       = *block_rect;
    }

    block->new_fb           = ORANGE_FALSE;
    block->block_swlyr      = GUI_CreateLayer((void *)block->fb_addr[0], block->block_rect.width,
                              block->block_rect.height, block->fb_fmt);

    if (block->block_swlyr == (__hdle)(-1))
    {
        ORANGE_WRN("no software layer fit!\n");
        goto error;
    }

    block->clip_rect.x      = 0;
    block->clip_rect.y      = 0;
    block->clip_rect.width  = block->fb_width;
    block->clip_rect.height = block->fb_height;
    pthread_mutex_init(&(block->lock), (void *)1);
    return GUI_P2Handle(block);
error:

    if (block)
    {
        orange_mfree(block);
        block = NULL;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockSel
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockSel(HBLOCK hblock)
{
    __gui_block_t  *block;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    GUI_Lock();
    GUI_SelLayer(block->block_swlyr);
    GUI_Unlock();
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockDelete
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockDelete(HBLOCK hblock)
{
    __u8            bpp = 0;
    __u32           size = 0;
    __gui_block_t   *block;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    if (ORANGE_TRUE == block->new_fb)
    {
        bpp = GUI_MpBlockGetBPP(block->fb_fmt);

        if (bpp)
        {
            if (block->fb_addr[0] != NULL)
            {
                size = (block->block_rect.width * block->block_rect.height * bpp + 7) >> 3;
                orange_pfree((void *)block->fb_addr[0], size);
                block->fb_addr[0] = NULL;
            }
        }
        else
        {
            ORANGE_WRN("frame buffer format error!\n");
            return ORANGE_FAIL;
        }
    }

    if (block->block_swlyr)
    {
        GUI_ReleaseLayer(block->block_swlyr);
        block->block_swlyr = NULL;
    }

    pthread_mutex_destroy(block->lock);
    orange_mfree(block);
    block = NULL;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockRotate
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
HBLOCK GUI_BlockRotate(HBLOCK hblock, __u32 angle)
{
    __gui_block_t           *block      = NULL;
    __gui_block_t           *dst_block  = NULL;
    void                    *addr       = NULL;
    __mp_rotate_mirror_t    rotate;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    __u8                    bpp         = 0;
    __u32                   size        = 0;
    //RECT                  clip_rect;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return NULL;
    }

    dst_block = (__gui_block_t *)orange_malloc(sizeof(__gui_block_t));

    if (!dst_block)
    {
        ORANGE_WRN("alloc mp dst block memory failed!\n");
        return NULL;
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return NULL;
    }

    hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

    if (hdle == 0)
    {
        ORANGE_WRN("request scaler fail\n");
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
        return NULL;
    }

    dst_block->fb_fmt           = block->fb_fmt;
    dst_block->fb_byte_seq  = block->fb_byte_seq;
    dst_block->block_rect.x         = 0;
    dst_block->block_rect.y         = 0;

    if ((ROTATE_ANGLE_NONE == angle) || (ROTATE_ANGLE_180 == angle))
    {
        dst_block->block_rect.width         = block->block_rect.width;
        dst_block->block_rect.height        = block->block_rect.height;
    }
    else if ((ROTATE_ANGLE_90 == angle) || (ROTATE_ANGLE_270 == angle))
    {
        dst_block->block_rect.width         = block->block_rect.height;
        dst_block->block_rect.height        = block->block_rect.width;
    }
    else
    {
        ORANGE_WRN("block size error!\n");
        goto error;
    }

    bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

    if (bpp)
    {
        /*get the size of framebuffer*/
        size = (dst_block->block_rect.width * dst_block->block_rect.height * bpp + 7) >> 3;
        addr = orange_pmalloc(size);

        if (!addr)
        {
            ORANGE_WRN("pmalloc framebuffer failed!\n");
            goto error;
        }
    }

    dst_block->fb_addr[0]       = (__u32)addr;
    dst_block->fb_width         = dst_block->block_rect.width;
    dst_block->fb_height        = dst_block->block_rect.height;
    dst_block->new_fb           = ORANGE_TRUE;
    dst_block->block_swlyr      = GUI_CreateLayer((void *)dst_block->fb_addr[0],
                                  dst_block->block_rect.width, dst_block->block_rect.height, dst_block->fb_fmt);

    if (dst_block->block_swlyr == (__hdle)(-1))
    {
        ORANGE_WRN("no software layer fit!\n");
        goto error;
    }

    dst_block->clip_rect.x      = 0;
    dst_block->clip_rect.y      = 0;
    dst_block->clip_rect.width  = dst_block->block_rect.width;
    dst_block->clip_rect.height = dst_block->block_rect.height;
    pthread_mutex_init(&(dst_block->lock), (void *)1);
    rotate.layer_addr[0]            = block->fb_addr[0];
    rotate.layer_line_width         = block->fb_width;
    rotate.layer_pixelfmt           = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(block->fb_fmt);
    rotate.layer_byte_seq           = block->fb_byte_seq;
    rotate.layer_src_rect.x         = block->block_rect.x;
    rotate.layer_src_rect.y         = block->block_rect.y;
    rotate.layer_src_rect.width     = block->block_rect.width;
    rotate.layer_src_rect.height    = block->block_rect.height;
    rotate.layer_dst_rect.x         = 0;
    rotate.layer_dst_rect.y         = 0;
    rotate.layer_dst_rect.width     = dst_block->block_rect.width;
    rotate.layer_dst_rect.height    = dst_block->block_rect.height;
    rotate.dst_addr[0]              = dst_block->fb_addr[0];
    rotate.dst_line_width           = dst_block->fb_width;
    rotate.dst_window_rect.x        = 0;
    rotate.dst_window_rect.y        = 0;
    rotate.dst_window_rect.width    = dst_block->block_rect.width;
    rotate.dst_window_rect.height   = dst_block->block_rect.height;

    switch (angle)
    {
        case ROTATE_ANGLE_NONE:
            rotate.win_op = MP_WIN_OP_NOP;
            break;

        case ROTATE_ANGLE_90:
            rotate.win_op = MP_WIN_OP_ROTATE_90;
            break;

        case ROTATE_ANGLE_180:
            rotate.win_op = MP_WIN_OP_ROTATE_180;
            break;

        case ROTATE_ANGLE_270:
            rotate.win_op = MP_WIN_OP_ROTATE_270;
            break;

        default:
            ORANGE_WRN("rotate angle is invalid!\n");
            goto error;
    }

    eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate);
    eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return GUI_P2Handle(dst_block);
error:

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    if (addr)
    {
        orange_pfree(addr, size);
        addr = NULL;
    }

    if (dst_block->block_swlyr)
    {
        GUI_ReleaseLayer(dst_block->block_swlyr);
        dst_block->block_swlyr = NULL;
    }

    if (dst_block)
    {
        orange_mfree(dst_block);
        dst_block = NULL;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockHFlip
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
HBLOCK GUI_BlockHFlip(HBLOCK hblock)
{
    __gui_block_t           *block      = NULL;
    __gui_block_t           *dst_block  = NULL;
    void                    *addr       = NULL;
    __mp_rotate_mirror_t    rotate;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    __u8                    bpp         = 0;
    __u32                   size        = 0;
    //RECT                  clip_rect;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return NULL;
    }

    dst_block = (__gui_block_t *)orange_malloc(sizeof(__gui_block_t));

    if (!dst_block)
    {
        ORANGE_WRN("alloc mp dst block memory failed!\n");
        return NULL;
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return NULL;
    }

    hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

    if (hdle == 0)
    {
        ORANGE_WRN("request scaler fail\n");
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
        return NULL;
    }

    dst_block->fb_fmt               = block->fb_fmt;
    dst_block->fb_byte_seq          = block->fb_byte_seq;
    dst_block->block_rect.x         = 0;
    dst_block->block_rect.y         = 0;
    dst_block->block_rect.width     = block->block_rect.width;
    dst_block->block_rect.height    = block->block_rect.height;
    bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

    if (bpp)
    {
        /*get the size of framebuffer*/
        size = (dst_block->block_rect.width * dst_block->block_rect.height * bpp + 7) >> 3;
        addr = orange_pmalloc(size);

        if (!addr)
        {
            ORANGE_WRN("pmalloc framebuffer failed!\n");
            goto error;
        }
    }

    dst_block->fb_addr[0]       = (__u32)addr;
    dst_block->fb_width         = dst_block->block_rect.width;
    dst_block->fb_height        = dst_block->block_rect.height;
    dst_block->new_fb           = ORANGE_TRUE;
    dst_block->block_swlyr      = GUI_CreateLayer((void *)dst_block->fb_addr[0],
                                  dst_block->block_rect.width, dst_block->block_rect.height, dst_block->fb_fmt);

    if (dst_block->block_swlyr == (__hdle)(-1))
    {
        ORANGE_WRN("no software layer fit!\n");
        goto error;
    }

    dst_block->clip_rect.x      = 0;
    dst_block->clip_rect.y      = 0;
    dst_block->clip_rect.width  = dst_block->block_rect.width;
    dst_block->clip_rect.height = dst_block->block_rect.height;
    pthread_mutex_init(&(dst_block->lock), (void *)1);
    rotate.layer_addr[0]            = block->fb_addr[0];
    rotate.layer_line_width         = block->fb_width;
    rotate.layer_pixelfmt           = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(block->fb_fmt);
    rotate.layer_byte_seq           = block->fb_byte_seq;
    rotate.layer_src_rect.x         = block->block_rect.x;
    rotate.layer_src_rect.y         = block->block_rect.y;
    rotate.layer_src_rect.width     = block->block_rect.width;
    rotate.layer_src_rect.height    = block->block_rect.height;
    rotate.layer_dst_rect.x         = 0;
    rotate.layer_dst_rect.y         = 0;
    rotate.layer_dst_rect.width     = dst_block->block_rect.width;
    rotate.layer_dst_rect.height    = dst_block->block_rect.height;
    rotate.dst_addr[0]              = dst_block->fb_addr[0];
    rotate.dst_line_width           = dst_block->fb_width;
    rotate.dst_window_rect.x        = 0;
    rotate.dst_window_rect.y        = 0;
    rotate.dst_window_rect.width    = dst_block->block_rect.width;
    rotate.dst_window_rect.height   = dst_block->block_rect.height;
    rotate.win_op                   = MP_WIN_OP_MIRROR_LR;
    eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate);
    eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return GUI_P2Handle(dst_block);
error:

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    if (addr)
    {
        orange_pfree(addr, size);
        addr = NULL;
    }

    if (dst_block->block_swlyr)
    {
        GUI_ReleaseLayer(dst_block->block_swlyr);
        dst_block->block_swlyr = NULL;
    }

    if (dst_block)
    {
        orange_mfree(dst_block);
        dst_block = NULL;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockHFlip
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
HBLOCK GUI_BlockVFlip(HBLOCK hblock)
{
    __gui_block_t           *block      = NULL;
    __gui_block_t           *dst_block  = NULL;
    void                    *addr       = NULL;
    __mp_rotate_mirror_t    rotate;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    __u8                    bpp         = 0;
    __u32                   size        = 0;
    //RECT                  clip_rect;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return NULL;
    }

    dst_block = (__gui_block_t *)orange_malloc(sizeof(__gui_block_t));

    if (!dst_block)
    {
        ORANGE_WRN("alloc mp dst block memory failed!\n");
        return NULL;
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return NULL;
    }

    hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

    if (hdle == 0)
    {
        ORANGE_WRN("request scaler fail\n");
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
        return NULL;
    }

    dst_block->fb_fmt               = block->fb_fmt;
    dst_block->fb_byte_seq          = block->fb_byte_seq;
    dst_block->block_rect.x         = 0;
    dst_block->block_rect.y         = 0;
    dst_block->block_rect.width     = block->block_rect.width;
    dst_block->block_rect.height    = block->block_rect.height;
    bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

    if (bpp)
    {
        /*get the size of framebuffer*/
        size = (dst_block->block_rect.width * dst_block->block_rect.height * bpp + 7) >> 3;
        addr = orange_pmalloc(size);

        if (!addr)
        {
            ORANGE_WRN("pmalloc framebuffer failed!\n");
            goto error;
        }
    }

    dst_block->fb_addr[0]       = (__u32)addr;
    dst_block->fb_width         = dst_block->block_rect.width;
    dst_block->fb_height        = dst_block->block_rect.height;
    dst_block->new_fb           = ORANGE_TRUE;
    dst_block->block_swlyr      = GUI_CreateLayer((void *)dst_block->fb_addr[0],
                                  dst_block->block_rect.width, dst_block->block_rect.height, dst_block->fb_fmt);

    if (dst_block->block_swlyr == (__hdle)(-1))
    {
        ORANGE_WRN("no software layer fit!\n");
        goto error;
    }

    dst_block->clip_rect.x      = 0;
    dst_block->clip_rect.y      = 0;
    dst_block->clip_rect.width  = dst_block->block_rect.width;
    dst_block->clip_rect.height = dst_block->block_rect.height;
    pthread_mutex_init(&(dst_block->lock), (void *)1);
    rotate.layer_addr[0]            = block->fb_addr[0];
    rotate.layer_line_width         = block->fb_width;
    rotate.layer_pixelfmt           = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(block->fb_fmt);
    rotate.layer_byte_seq           = block->fb_byte_seq;
    rotate.layer_src_rect.x         = block->block_rect.x;
    rotate.layer_src_rect.y         = block->block_rect.y;
    rotate.layer_src_rect.width     = block->block_rect.width;
    rotate.layer_src_rect.height    = block->block_rect.height;
    rotate.layer_dst_rect.x         = 0;
    rotate.layer_dst_rect.y         = 0;
    rotate.layer_dst_rect.width     = dst_block->block_rect.width;
    rotate.layer_dst_rect.height    = dst_block->block_rect.height;
    rotate.dst_addr[0]              = dst_block->fb_addr[0];
    rotate.dst_line_width           = dst_block->fb_width;
    rotate.dst_window_rect.x        = 0;
    rotate.dst_window_rect.y        = 0;
    rotate.dst_window_rect.width    = dst_block->block_rect.width;
    rotate.dst_window_rect.height   = dst_block->block_rect.height;
    rotate.win_op                   = MP_WIN_OP_MIRROR_UD;
    eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate);
    eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return GUI_P2Handle(dst_block);
error:

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    if (addr)
    {
        orange_pfree(addr, size);
        addr = NULL;
    }

    if (dst_block->block_swlyr)
    {
        GUI_ReleaseLayer(dst_block->block_swlyr);
        dst_block->block_swlyr = NULL;
    }

    if (dst_block)
    {
        orange_mfree(dst_block);
        dst_block = NULL;
    }

    return NULL;
}
/*
**********************************************************************************************************************
*                                               GUI_SetClip
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_SetBlockClip(HBLOCK hblock, RECT *clip_rect)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    if (((clip_rect->x + clip_rect->width) <= block->fb_width) && ((clip_rect->y + clip_rect->height) <= block->fb_height))
    {
        block->clip_rect.x      = clip_rect->x;
        block->clip_rect.y      = clip_rect->y;
        block->clip_rect.width  = clip_rect->width;
        block->clip_rect.height = clip_rect->height;
    }
    else
    {
        ORANGE_WRN("set rect size is too big!\n");
        return ORANGE_FAIL;
    }

    return ORANGE_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_SetClip
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
RECT GUI_GetBlockClip(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    RECT                clip_rect;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
    }

    clip_rect = block->clip_rect;
    return clip_rect;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockBitBlit(HBLOCK hdstblock, __s32 dx, __s32 dy, HBLOCK hsrcblock, RECT *srcrect, __u32 flags, void *value)
{
    __gui_block_t           *src_block  = NULL;
    __gui_block_t           *dst_block  = NULL;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    __gui_block_value_t     *block_value = NULL;
    __mp_rect_t             lyr_src_rect;
    src_block = (__gui_block_t *)hsrcblock;

    if (!src_block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    dst_block = (__gui_block_t *)hdstblock;

    if (!dst_block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    if (NULL == srcrect)
    {
        lyr_src_rect.x      = src_block->block_rect.x;
        lyr_src_rect.y      = src_block->block_rect.y;
        lyr_src_rect.width  = src_block->block_rect.width;
        lyr_src_rect.height = src_block->block_rect.height;
    }
    else
    {
        if ((src_block->block_rect.x + srcrect->x) <= src_block->fb_width)
        {
            lyr_src_rect.x      = src_block->block_rect.x + srcrect->x;
        }
        else
        {
            ORANGE_WRN("the source rect point is wrong!\n");
            return ORANGE_FAIL;
        }

        if ((src_block->block_rect.y + srcrect->y) <= src_block->fb_height)
        {
            lyr_src_rect.y      = src_block->block_rect.y + srcrect->y;
        }
        else
        {
            ORANGE_WRN("the source rect point is wrong!\n");
            return ORANGE_FAIL;
        }

        lyr_src_rect.width  = srcrect->width;
        lyr_src_rect.height = srcrect->height;
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return ORANGE_FAIL;
    }

    block_value = (__gui_block_value_t *)value;

    /*************************************************************************************************************/
    //copy//
    if ((flags & BLITMASK) == BLITFLAG_COLORSET)
    {
        __mp_copy_t copy_para;
        hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

        if (hdle == 0)
        {
            ORANGE_WRN("request scaler fail\n");
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
            return ORANGE_FAIL;
        }

        copy_para.layer_addr[0]         = src_block->fb_addr[0];
        copy_para.layer_line_width      = src_block->fb_width;
        copy_para.layer_pixelfmt        = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
        copy_para.layer_byte_seq        = src_block->fb_byte_seq;
        copy_para.layer_src_rect        = lyr_src_rect;
        copy_para.layer_dst_rect.x      = dx;
        copy_para.layer_dst_rect.y      = dy;
        copy_para.dst_addr[0]           = dst_block->fb_addr[0];
        copy_para.dst_line_width        = dst_block->fb_width;
        copy_para.dst_window_rect.x     = dst_block->clip_rect.x;
        copy_para.dst_window_rect.y     = dst_block->clip_rect.y;
        copy_para.dst_window_rect.width = dst_block->clip_rect.width;
        copy_para.dst_window_rect.height = dst_block->clip_rect.height;
        eLIBs_fioctrl(gmpfile, MP_CMD_COPY, SEL_SCREEN, &copy_para);
        eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //rotate and alpha//

    if ((0 != (flags & BLITALPHAMASK)) && (0 != (flags & BLITRFMASK)))
    {
        void            *addr   = NULL;
        __u8            bpp;
        __u32           size    = 0;
        __u32           mid_src_width, mid_src_height;

        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

                if (bpp)
                {
                    /*get the size of framebuffer*/
                    size = (dst_block->fb_width * dst_block->fb_height * bpp + 7) >> 3;
                    addr = orange_pmalloc(size);

                    if (!addr)
                    {
                        ORANGE_WRN("pmalloc framebuffer failed!\n");
                        return ORANGE_FAIL;
                    }
                }

                //orange_memcpy(addr, (void *)dst_block->fb_addr[0], size);
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = src_block->fb_addr[0];
                rotate_para.layer_line_width        = src_block->fb_width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                rotate_para.layer_byte_seq          = src_block->fb_byte_seq;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = 0;
                rotate_para.layer_dst_rect.y        = 0;
                rotate_para.dst_addr[0]             = (__u32)addr;
                rotate_para.dst_line_width          = dst_block->fb_width;
                rotate_para.dst_window_rect.x       = 0;
                rotate_para.dst_window_rect.y       = 0;
                rotate_para.dst_window_rect.width   = dst_block->fb_width;
                rotate_para.dst_window_rect.height  = dst_block->fb_height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_90;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_180;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_270;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_LR;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_UD;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                //orange_pfree(addr, size);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }

        switch (flags & BLITALPHAMASK)
        {
            case BLITFLAG_COLORALPHA:
            case BLITFLAG_ALPHACHANNEL:
            case BLITFLAG_BOTHALPHA:
            {
                __mp_alpha_blend_t alpha_para;
                //block_value = (__gui_block_value_t *)value;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_block->fb_width)
                {
                    mid_src_width = dst_block->fb_width;
                }

                if (mid_src_height >= dst_block->fb_height)
                {
                    mid_src_height = dst_block->fb_height;
                }

                alpha_para.layer_addr[0]                = (__u32)addr;
                alpha_para.layer_line_width             = dst_block->fb_width;
                alpha_para.layer_pixelfmt               = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_block->fb_fmt);
                alpha_para.layer_byte_seq               = dst_block->fb_byte_seq;
                alpha_para.layer_src_rect.x             = 0;
                alpha_para.layer_src_rect.y             = 0;
                alpha_para.layer_src_rect.width         = mid_src_width;
                alpha_para.layer_src_rect.height        = mid_src_height;
                alpha_para.layer_dst_rect.x             = dx;
                alpha_para.layer_dst_rect.y             = dy;
                alpha_para.dst_addr[0]                  = dst_block->fb_addr[0];
                alpha_para.dst_line_width               = dst_block->fb_width;
                alpha_para.dst_window_rect.x            = dst_block->clip_rect.x;
                alpha_para.dst_window_rect.y            = dst_block->clip_rect.y;
                alpha_para.dst_window_rect.width        = dst_block->clip_rect.width;
                alpha_para.dst_window_rect.height       = dst_block->clip_rect.height;

                if (!value)
                {
                    alpha_para.layer_src_alpha              = 1;
                }
                else
                {
                    alpha_para.layer_src_alpha              = block_value->src_globle_value;
                }

                alpha_para.layer_dst_alpha              = 1;
                alpha_para.dst_in_alpha_mode            = MP_IN_ALPHA_PIXEL_ALPHA;

                if (BLITFLAG_COLORALPHA == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_ALPHA;
                }
                else if (BLITFLAG_ALPHACHANNEL == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_GLOBAL_ALPHA;
                }
                else
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_MULTIPLY_GLOBAL_ALPHA;
                }

                alpha_para.img_area_out_alpha_ctl       = 0;
                alpha_para.A0_out_alpha_mode            = MP_OUT_ALPHA_LAYER0_ALPHA;
                alpha_para.A1_out_alpha_mode            = MP_OUT_ALPHA_LAYER1_ALPHA;
                alpha_para.A2_out_alpha_mode            = MP_OUT_ALPHA_FORMULA;
                alpha_para.img_area_out_alpha_value     = 0xFF;
                alpha_para.non_img_area_out_alpha_value = 0xFF;
                eLIBs_fioctrl(gmpfile, MP_CMD_ALPHA_BLEND, SEL_SCREEN, &alpha_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            case BLITFLAG_COLORKEY:
            {
                __mp_colorkey_t colorkey_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_block->fb_width)
                {
                    mid_src_width = dst_block->fb_width;
                }

                if (mid_src_height >= dst_block->fb_height)
                {
                    mid_src_height = dst_block->fb_height;
                }

                colorkey_para.layer_addr[0]             = (__u32)addr;
                colorkey_para.layer_line_width          = dst_block->fb_width;
                colorkey_para.layer_pixelfmt            = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_block->fb_fmt);
                colorkey_para.layer_byte_seq            = dst_block->fb_byte_seq;
                colorkey_para.layer_src_rect.x          = 0;
                colorkey_para.layer_src_rect.y          = 0;
                colorkey_para.layer_src_rect.width      = mid_src_width;
                colorkey_para.layer_src_rect.height     = mid_src_height;
                colorkey_para.layer_dst_rect.x          = dx;
                colorkey_para.layer_dst_rect.y          = dy;
                colorkey_para.dst_addr[0]                   = dst_block->fb_addr[0];
                colorkey_para.dst_line_width                = dst_block->fb_width;
                colorkey_para.dst_window_rect.x             = dst_block->clip_rect.x;
                colorkey_para.dst_window_rect.y             = dst_block->clip_rect.y;
                colorkey_para.dst_window_rect.width         = dst_block->clip_rect.width;
                colorkey_para.dst_window_rect.height        = dst_block->clip_rect.height;
                colorkey_para.ck_ctrl.red_ctrl              = 0;
                colorkey_para.ck_ctrl.green_ctrl            = 0;
                colorkey_para.ck_ctrl.blue_ctrl             = 0;
                colorkey_para.ck_ctrl.max_color             = GUI_Color2MpColor(block_value->ck_value);
                colorkey_para.ck_ctrl.min_color             = GUI_Color2MpColor(block_value->ck_value);
                //colorkey_para.ck_ctrl.ky_type                 = 0;
                //colorkey_para.ck_ctrl                         = ck_ctrl;
                eLIBs_fioctrl(gmpfile, MP_CMD_COLORKEY, SEL_SCREEN, &colorkey_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("alpha mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;

                if (addr)
                {
                    orange_pfree(addr, size);
                    addr = NULL;
                }

                return ORANGE_FAIL;
        }

        if (addr)
        {
            orange_pfree(addr, size);
            addr = NULL;
        }

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //rotate and bitblt//
    if ((0 != (flags & BLITBITMASK)) && (0 != (flags & BLITRFMASK)))
    {
        void            *addr   = NULL;
        __u8            bpp;
        __u32           size    = 0;
        __u32           mid_src_width, mid_src_height;

        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

                if (bpp)
                {
                    /*get the size of framebuffer*/
                    size = (dst_block->fb_width * dst_block->fb_height * bpp + 7) >> 3;
                    addr = orange_pmalloc(size);

                    if (!addr)
                    {
                        ORANGE_WRN("pmalloc framebuffer failed!\n");
                        return ORANGE_FAIL;
                    }
                }

                //orange_memcpy(addr, (void *)dst_block->fb_addr[0], size);
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = src_block->fb_addr[0];
                rotate_para.layer_line_width        = src_block->fb_width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                rotate_para.layer_byte_seq          = src_block->fb_byte_seq;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = 0;
                rotate_para.layer_dst_rect.y        = 0;
                rotate_para.dst_addr[0]             = (__u32)addr;
                rotate_para.dst_line_width          = dst_block->fb_width;
                rotate_para.dst_window_rect.x       = 0;
                rotate_para.dst_window_rect.y       = 0;
                rotate_para.dst_window_rect.width   = dst_block->fb_width;
                rotate_para.dst_window_rect.height  = dst_block->fb_height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_90;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_180;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_270;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_LR;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_UD;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                //orange_pfree(addr, size);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }

        switch (flags & BLITBITMASK)
        {
            case BLITFLAG_AND:
            case BLITFLAG_OR:
            case BLITFLAG_XOR:
            {
                __mp_bitblt_t bitblt_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_block->fb_width)
                {
                    mid_src_width = dst_block->fb_width;
                }

                if (mid_src_height >= dst_block->fb_height)
                {
                    mid_src_height = dst_block->fb_height;
                }

                bitblt_para.layer_addr[0]               = (__u32)addr;
                bitblt_para.layer_line_width            = dst_block->fb_width;
                bitblt_para.layer_pixelfmt              = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_block->fb_fmt);
                bitblt_para.layer_byte_seq              = dst_block->fb_byte_seq;
                bitblt_para.layer_src_rect.x            = 0;
                bitblt_para.layer_src_rect.y            = 0;
                bitblt_para.layer_src_rect.width        = mid_src_width;
                bitblt_para.layer_src_rect.height       = mid_src_height;
                bitblt_para.layer_dst_rect.x            = dx;
                bitblt_para.layer_dst_rect.y            = dy;
                bitblt_para.dst_addr[0]                 = dst_block->fb_addr[0];
                bitblt_para.dst_line_width              = dst_block->fb_width;
                bitblt_para.dst_window_rect.x           = dst_block->clip_rect.x;
                bitblt_para.dst_window_rect.y           = dst_block->clip_rect.y;
                bitblt_para.dst_window_rect.width       = dst_block->clip_rect.width;
                bitblt_para.dst_window_rect.height      = dst_block->clip_rect.height;

                if (BLITFLAG_AND == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_AND;
                }
                else if (BLITFLAG_OR == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_OR;
                }
                else
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_XOR;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_BITBLT, SEL_SCREEN, &bitblt_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("blitbit mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;

                if (addr)
                {
                    orange_pfree(addr, size);
                    addr = NULL;
                }

                return ORANGE_FAIL;
        }

        if (addr)
        {
            orange_pfree(addr, size);
            addr = NULL;
        }

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //alpha//
    if (0 != (flags & BLITALPHAMASK))
    {
        switch (flags & BLITALPHAMASK)
        {
            case BLITFLAG_COLORALPHA:
            case BLITFLAG_ALPHACHANNEL:
            case BLITFLAG_BOTHALPHA:
            {
                __mp_alpha_blend_t alpha_para;
                //block_value = (__gui_block_value_t *)value;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                alpha_para.layer_addr[0]                = src_block->fb_addr[0];
                alpha_para.layer_line_width             = src_block->fb_width;
                alpha_para.layer_pixelfmt               = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                alpha_para.layer_byte_seq               = src_block->fb_byte_seq;
                alpha_para.layer_src_rect               = lyr_src_rect;
                alpha_para.layer_dst_rect.x             = dx;
                alpha_para.layer_dst_rect.y             = dy;
                alpha_para.dst_addr[0]                  = dst_block->fb_addr[0];
                alpha_para.dst_line_width               = dst_block->fb_width;
                alpha_para.dst_window_rect.x            = dst_block->clip_rect.x;
                alpha_para.dst_window_rect.y            = dst_block->clip_rect.y;
                alpha_para.dst_window_rect.width        = dst_block->clip_rect.width;
                alpha_para.dst_window_rect.height       = dst_block->clip_rect.height;

                if (!value)
                {
                    alpha_para.layer_src_alpha              = 1;
                }
                else
                {
                    alpha_para.layer_src_alpha              = block_value->src_globle_value;
                }

                alpha_para.layer_dst_alpha              = 1;
                alpha_para.dst_in_alpha_mode            = MP_IN_ALPHA_PIXEL_ALPHA;

                if (BLITFLAG_COLORALPHA == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_ALPHA;
                }
                else if (BLITFLAG_ALPHACHANNEL == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_GLOBAL_ALPHA;
                }
                else
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_MULTIPLY_GLOBAL_ALPHA;
                }

                alpha_para.img_area_out_alpha_ctl       = 0;
                alpha_para.A0_out_alpha_mode            = MP_OUT_ALPHA_LAYER0_ALPHA;
                alpha_para.A1_out_alpha_mode            = MP_OUT_ALPHA_LAYER1_ALPHA;
                alpha_para.A2_out_alpha_mode            = MP_OUT_ALPHA_FORMULA;
                alpha_para.img_area_out_alpha_value     = 0xFF;
                alpha_para.non_img_area_out_alpha_value = 0xFF;
                eLIBs_fioctrl(gmpfile, MP_CMD_ALPHA_BLEND, SEL_SCREEN, &alpha_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            case BLITFLAG_COLORKEY:
            {
                __mp_colorkey_t colorkey_para;
                //__mp_ck_ctrl_t  ck_ctrl;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                colorkey_para.layer_addr[0]                 = src_block->fb_addr[0];
                colorkey_para.layer_line_width              = src_block->fb_width;
                colorkey_para.layer_pixelfmt                = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                colorkey_para.layer_byte_seq                = src_block->fb_byte_seq;
                colorkey_para.layer_src_rect                = lyr_src_rect;
                colorkey_para.layer_dst_rect.x              = dx;
                colorkey_para.layer_dst_rect.y              = dy;
                colorkey_para.dst_addr[0]                   = dst_block->fb_addr[0];
                colorkey_para.dst_line_width                = dst_block->fb_width;
                colorkey_para.dst_window_rect.x             = dst_block->clip_rect.x;
                colorkey_para.dst_window_rect.y             = dst_block->clip_rect.y;
                colorkey_para.dst_window_rect.width         = dst_block->clip_rect.width;
                colorkey_para.dst_window_rect.height        = dst_block->clip_rect.height;
                colorkey_para.ck_ctrl.red_ctrl              = 0;
                colorkey_para.ck_ctrl.green_ctrl            = 0;
                colorkey_para.ck_ctrl.blue_ctrl             = 0;
                colorkey_para.ck_ctrl.max_color             = GUI_Color2MpColor(block_value->ck_value);
                colorkey_para.ck_ctrl.min_color             = GUI_Color2MpColor(block_value->ck_value);
                //colorkey_para.ck_ctrl.ky_type                 = 0;
                //colorkey_para.ck_ctrl                         = ck_ctrl;
                eLIBs_fioctrl(gmpfile, MP_CMD_COLORKEY, SEL_SCREEN, &colorkey_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("alpha mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    /*************************************************************************************************************/
    //rotate//
    if (0 != (flags & BLITRFMASK))
    {
        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = src_block->fb_addr[0];
                rotate_para.layer_line_width        = src_block->fb_width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                rotate_para.layer_byte_seq          = src_block->fb_byte_seq;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = dx;
                rotate_para.layer_dst_rect.y        = dy;
                rotate_para.dst_addr[0]             = dst_block->fb_addr[0];
                rotate_para.dst_line_width          = dst_block->fb_width;
                rotate_para.dst_window_rect.x       = dst_block->clip_rect.x;
                rotate_para.dst_window_rect.y       = dst_block->clip_rect.y;
                rotate_para.dst_window_rect.width   = dst_block->clip_rect.width;
                rotate_para.dst_window_rect.height  = dst_block->clip_rect.height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_90;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_180;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_270;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_MIRROR_LR;
                }
                else
                {
                    rotate_para.win_op = MP_WIN_OP_MIRROR_UD;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    /*************************************************************************************************************/
    //bitblt//

    if (0 != (flags & BLITBITMASK))
    {
        switch (flags & BLITBITMASK)
        {
            case BLITFLAG_AND:
            case BLITFLAG_OR:
            case BLITFLAG_XOR:
            {
                __mp_bitblt_t bitblt_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                bitblt_para.layer_addr[0]               = src_block->fb_addr[0];
                bitblt_para.layer_line_width            = src_block->fb_width;
                bitblt_para.layer_pixelfmt              = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                bitblt_para.layer_byte_seq              = src_block->fb_byte_seq;
                bitblt_para.layer_src_rect              = lyr_src_rect;
                bitblt_para.layer_dst_rect.x            = dx;
                bitblt_para.layer_dst_rect.y            = dy;
                bitblt_para.dst_addr[0]                 = dst_block->fb_addr[0];
                bitblt_para.dst_line_width              = dst_block->fb_width;
                bitblt_para.dst_window_rect.x           = dst_block->clip_rect.x;
                bitblt_para.dst_window_rect.y           = dst_block->clip_rect.y;
                bitblt_para.dst_window_rect.width       = dst_block->clip_rect.width;
                bitblt_para.dst_window_rect.height      = dst_block->clip_rect.height;

                if (BLITFLAG_AND == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_AND;
                }
                else if (BLITFLAG_OR == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_OR;
                }
                else
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_XOR;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_BITBLT, SEL_SCREEN, &bitblt_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default:
                ORANGE_WRN("Bit flags is wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockLayerBlit(H_LYR  hdstlayer, RECT *clip_rect, __s32 dx, __s32 dy, HBLOCK hsrcblock,
                         RECT *srcrect, __u32 flags, void *value)
{
    __gui_block_t       *src_block      = NULL;
    __gui_lyrwin_t          *dst_lyr    = NULL;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    __gui_block_value_t     *block_value = NULL;
    FB                      dst_fb;
    __mp_rect_t             lyr_src_rect;
    __mp_rect_t             dst_window_rect;
    src_block = (__gui_block_t *)hsrcblock;

    if (!src_block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    dst_lyr = GUI_LyrH2P(hdstlayer);

    if (!dst_lyr)
    {
        ORANGE_WRN("dst layer is null!\n");
        return ORANGE_FAIL;
    }

    /*if(NULL == srcrect)
    {
        lyr_src_rect.x      = 0;
        lyr_src_rect.y      = 0;
        lyr_src_rect.width  = src_block->fb_width;
        lyr_src_rect.height = src_block->fb_height;
    }
    else
    {
        lyr_src_rect.x      = srcrect->x;
        lyr_src_rect.y      = srcrect->y;
        lyr_src_rect.width  = srcrect->width;
        lyr_src_rect.height = srcrect->height;
    }*/

    if (NULL == srcrect)
    {
        lyr_src_rect.x      = src_block->block_rect.x;
        lyr_src_rect.y      = src_block->block_rect.y;
        lyr_src_rect.width  = src_block->block_rect.width;
        lyr_src_rect.height = src_block->block_rect.height;
    }
    else
    {
        if ((src_block->block_rect.x + srcrect->x) <= src_block->fb_width)
        {
            lyr_src_rect.x      = src_block->block_rect.x + srcrect->x;
        }
        else
        {
            ORANGE_WRN("the source rect point is wrong!\n");
            return ORANGE_FAIL;
        }

        if ((src_block->block_rect.y + srcrect->y) <= src_block->fb_height)
        {
            lyr_src_rect.y      = src_block->block_rect.y + srcrect->y;
        }
        else
        {
            ORANGE_WRN("the source rect point is wrong!\n");
            return ORANGE_FAIL;
        }

        lyr_src_rect.width  = srcrect->width;
        lyr_src_rect.height = srcrect->height;
    }

    if (NULL == clip_rect)
    {
        dst_window_rect.x       = 0;
        dst_window_rect.y       = 0;
        dst_window_rect.width   = dst_lyr->lyrpara.fb->size.width;
        dst_window_rect.height  = dst_lyr->lyrpara.fb->size.height;
    }
    else
    {
        dst_window_rect.x       = clip_rect->x;
        dst_window_rect.y       = clip_rect->y;
        dst_window_rect.width   = clip_rect->width;
        dst_window_rect.height  = clip_rect->height;
    }

    if (EPDK_OK != GUI_LyrWinGetFB(hdstlayer, &dst_fb))
    {
        ORANGE_WRN("lyr_bt_fb empty!\n");
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return ORANGE_FAIL;
    }

    block_value = (__gui_block_value_t *)value;

    /*************************************************************************************************************/
    //copy//

    if (BLITFLAG_COLORSET == (flags & BLITMASK))
    {
        __mp_copy_t copy_para;
        hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

        if (hdle == 0)
        {
            ORANGE_WRN("request scaler fail\n");
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
            return ORANGE_FAIL;
        }

        copy_para.layer_addr[0]         = src_block->fb_addr[0];
        copy_para.layer_line_width      = src_block->fb_width;
        copy_para.layer_pixelfmt        = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
        copy_para.layer_byte_seq        = src_block->fb_byte_seq;
        copy_para.layer_src_rect        = lyr_src_rect;
        copy_para.layer_dst_rect.x      = dx;
        copy_para.layer_dst_rect.y      = dy;
        copy_para.dst_addr[0]               = (__u32)dst_fb.addr[0];
        copy_para.dst_line_width            = dst_fb.size.width;
        copy_para.dst_window_rect           = dst_window_rect;
        eLIBs_fioctrl(gmpfile, MP_CMD_COPY, SEL_SCREEN, &copy_para);
        eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //rotate and alpha//
    if ((0 != (flags & BLITALPHAMASK)) && (0 != (flags & BLITRFMASK)))
    {
        void            *addr   = NULL;
        __u8            bpp;
        __u32           size    = 0;
        __u32           mid_src_width, mid_src_height;

        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                bpp = GUI_MpBlockGetBPP(dst_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);

                if (bpp)
                {
                    /*get the size of framebuffer*/
                    size = (dst_lyr->lyrpara.fb->size.width * dst_lyr->lyrpara.fb->size.height * bpp + 7) >> 3;
                    addr = orange_pmalloc(size);

                    if (!addr)
                    {
                        ORANGE_WRN("pmalloc framebuffer failed!\n");
                        return ORANGE_FAIL;
                    }
                }

                //orange_memcpy(addr, (void *)dst_block->fb_addr[0], size);
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = src_block->fb_addr[0];
                rotate_para.layer_line_width        = src_block->fb_width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                rotate_para.layer_byte_seq          = src_block->fb_byte_seq;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = 0;
                rotate_para.layer_dst_rect.y        = 0;
                rotate_para.dst_addr[0]             = (__u32)addr;
                rotate_para.dst_line_width          = dst_lyr->lyrpara.fb->size.width;
                rotate_para.dst_window_rect.x       = 0;
                rotate_para.dst_window_rect.y       = 0;
                rotate_para.dst_window_rect.width   = dst_lyr->lyrpara.fb->size.width;
                rotate_para.dst_window_rect.height  = dst_lyr->lyrpara.fb->size.height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_90;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_180;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_270;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_LR;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_UD;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                //orange_pfree(addr, size);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }

        switch (flags & BLITALPHAMASK)
        {
            case BLITFLAG_COLORALPHA:
            case BLITFLAG_ALPHACHANNEL:
            case BLITFLAG_BOTHALPHA:
            {
                __mp_alpha_blend_t alpha_para;
                //block_value = (__gui_block_value_t *)value;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_lyr->lyrpara.fb->size.width)
                {
                    mid_src_width = dst_lyr->lyrpara.fb->size.width;
                }

                if (mid_src_height >= dst_lyr->lyrpara.fb->size.height)
                {
                    mid_src_height = dst_lyr->lyrpara.fb->size.height;
                }

                alpha_para.layer_addr[0]                = (__u32)addr;
                alpha_para.layer_line_width             = dst_lyr->lyrpara.fb->size.width;
                alpha_para.layer_pixelfmt               = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                alpha_para.layer_byte_seq               = dst_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                alpha_para.layer_src_rect.x             = 0;
                alpha_para.layer_src_rect.y             = 0;
                alpha_para.layer_src_rect.width         = mid_src_width;
                alpha_para.layer_src_rect.height        = mid_src_height;
                alpha_para.layer_dst_rect.x             = dx;
                alpha_para.layer_dst_rect.y             = dy;
                alpha_para.dst_addr[0]              = (__u32)dst_lyr->lyrpara.fb->addr[0];
                alpha_para.dst_line_width           = dst_lyr->lyrpara.fb->size.width;
                alpha_para.dst_window_rect          = dst_window_rect;

                if (!value)
                {
                    alpha_para.layer_src_alpha              = 1;
                }
                else
                {
                    alpha_para.layer_src_alpha              = block_value->src_globle_value;
                }

                alpha_para.layer_dst_alpha              = 1;
                alpha_para.dst_in_alpha_mode            = MP_IN_ALPHA_PIXEL_ALPHA;

                if (BLITFLAG_COLORALPHA == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_ALPHA;
                }
                else if (BLITFLAG_ALPHACHANNEL == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_GLOBAL_ALPHA;
                }
                else
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_MULTIPLY_GLOBAL_ALPHA;
                }

                alpha_para.img_area_out_alpha_ctl       = 0;
                alpha_para.A0_out_alpha_mode            = MP_OUT_ALPHA_LAYER0_ALPHA;
                alpha_para.A1_out_alpha_mode            = MP_OUT_ALPHA_LAYER1_ALPHA;
                alpha_para.A2_out_alpha_mode            = MP_OUT_ALPHA_FORMULA;
                alpha_para.img_area_out_alpha_value     = 0xFF;
                alpha_para.non_img_area_out_alpha_value = 0xFF;
                eLIBs_fioctrl(gmpfile, MP_CMD_ALPHA_BLEND, SEL_SCREEN, &alpha_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            case BLITFLAG_COLORKEY:
            {
                __mp_colorkey_t colorkey_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_lyr->lyrpara.fb->size.width)
                {
                    mid_src_width = dst_lyr->lyrpara.fb->size.width;
                }

                if (mid_src_height >= dst_lyr->lyrpara.fb->size.height)
                {
                    mid_src_height = dst_lyr->lyrpara.fb->size.height;
                }

                colorkey_para.layer_addr[0]             = (__u32)addr;
                colorkey_para.layer_line_width          = dst_lyr->lyrpara.fb->size.width;
                colorkey_para.layer_pixelfmt            = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                colorkey_para.layer_byte_seq            = dst_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                colorkey_para.layer_src_rect.x          = 0;
                colorkey_para.layer_src_rect.y          = 0;
                colorkey_para.layer_src_rect.width      = mid_src_width;
                colorkey_para.layer_src_rect.height     = mid_src_height;
                colorkey_para.layer_dst_rect.x          = dx;
                colorkey_para.layer_dst_rect.y          = dy;
                colorkey_para.dst_addr[0]                   = (__u32)dst_lyr->lyrpara.fb->addr[0];
                colorkey_para.dst_line_width                = dst_lyr->lyrpara.fb->size.width;
                colorkey_para.dst_window_rect               = dst_window_rect;
                colorkey_para.ck_ctrl.red_ctrl              = 0;
                colorkey_para.ck_ctrl.green_ctrl            = 0;
                colorkey_para.ck_ctrl.blue_ctrl             = 0;
                colorkey_para.ck_ctrl.max_color             = GUI_Color2MpColor(block_value->ck_value);
                colorkey_para.ck_ctrl.min_color             = GUI_Color2MpColor(block_value->ck_value);
                //colorkey_para.ck_ctrl.ky_type                 = 0;
                //colorkey_para.ck_ctrl                         = ck_ctrl;
                eLIBs_fioctrl(gmpfile, MP_CMD_COLORKEY, SEL_SCREEN, &colorkey_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("alpha mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;

                if (addr)
                {
                    orange_pfree(addr, size);
                    addr = NULL;
                }

                return ORANGE_FAIL;
        }

        if (addr)
        {
            orange_pfree(addr, size);
            addr = NULL;
        }

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //rotate and bitblt//
    if ((0 != (flags & BLITBITMASK)) && (0 != (flags & BLITRFMASK)))
    {
        void            *addr   = NULL;
        __u8            bpp;
        __u32           size    = 0;
        __u32           mid_src_width, mid_src_height;

        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                bpp = GUI_MpBlockGetBPP(dst_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);

                if (bpp)
                {
                    /*get the size of framebuffer*/
                    size = (dst_lyr->lyrpara.fb->size.width * dst_lyr->lyrpara.fb->size.height * bpp + 7) >> 3;
                    addr = orange_pmalloc(size);

                    if (!addr)
                    {
                        ORANGE_WRN("pmalloc framebuffer failed!\n");
                        return ORANGE_FAIL;
                    }
                }

                //orange_memcpy(addr, (void *)dst_block->fb_addr[0], size);
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = src_block->fb_addr[0];
                rotate_para.layer_line_width        = src_block->fb_width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                rotate_para.layer_byte_seq          = src_block->fb_byte_seq;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = 0;
                rotate_para.layer_dst_rect.y        = 0;
                rotate_para.dst_addr[0]             = (__u32)addr;
                rotate_para.dst_line_width          = dst_lyr->lyrpara.fb->size.width;
                rotate_para.dst_window_rect.x       = 0;
                rotate_para.dst_window_rect.y       = 0;
                rotate_para.dst_window_rect.width   = dst_lyr->lyrpara.fb->size.width;
                rotate_para.dst_window_rect.height  = dst_lyr->lyrpara.fb->size.height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_90;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_180;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_270;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_LR;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_UD;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                //orange_pfree(addr, size);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }

        switch (flags & BLITBITMASK)
        {
            case BLITFLAG_AND:
            case BLITFLAG_OR:
            case BLITFLAG_XOR:
            {
                __mp_bitblt_t bitblt_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_lyr->lyrpara.fb->size.width)
                {
                    mid_src_width = dst_lyr->lyrpara.fb->size.width;
                }

                if (mid_src_height >= dst_lyr->lyrpara.fb->size.height)
                {
                    mid_src_height = dst_lyr->lyrpara.fb->size.height;
                }

                bitblt_para.layer_addr[0]               = (__u32)addr;
                bitblt_para.layer_line_width            = dst_lyr->lyrpara.fb->size.width;
                bitblt_para.layer_pixelfmt              = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                bitblt_para.layer_byte_seq              = dst_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                bitblt_para.layer_src_rect.x            = 0;
                bitblt_para.layer_src_rect.y            = 0;
                bitblt_para.layer_src_rect.width        = mid_src_width;
                bitblt_para.layer_src_rect.height       = mid_src_height;
                bitblt_para.layer_dst_rect.x            = dx;
                bitblt_para.layer_dst_rect.y            = dy;
                bitblt_para.dst_addr[0]                 = (__u32)dst_lyr->lyrpara.fb->addr[0];
                bitblt_para.dst_line_width              = dst_lyr->lyrpara.fb->size.width;
                bitblt_para.dst_window_rect             = dst_window_rect;;

                if (BLITFLAG_AND == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_AND;
                }
                else if (BLITFLAG_OR == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_OR;
                }
                else
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_XOR;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_BITBLT, SEL_SCREEN, &bitblt_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("blitbit mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;

                if (addr)
                {
                    orange_pfree(addr, size);
                    addr = NULL;
                }

                return ORANGE_FAIL;
        }

        if (addr)
        {
            orange_pfree(addr, size);
            addr = NULL;
        }

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //alpha//
    if (0 != (flags & BLITALPHAMASK))
    {
        switch (flags & BLITALPHAMASK)
        {
            case BLITFLAG_COLORALPHA:
            case BLITFLAG_ALPHACHANNEL:
            case BLITFLAG_BOTHALPHA:
            {
                __mp_alpha_blend_t alpha_para;
                //block_value = (__gui_block_value_t *)value;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                alpha_para.layer_addr[0]            = src_block->fb_addr[0];
                alpha_para.layer_line_width         = src_block->fb_width;
                alpha_para.layer_pixelfmt           = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                alpha_para.layer_byte_seq           = src_block->fb_byte_seq;
                alpha_para.layer_src_rect           = lyr_src_rect;
                alpha_para.layer_dst_rect.x         = dx;
                alpha_para.layer_dst_rect.y         = dy;
                alpha_para.dst_addr[0]              = (__u32)dst_lyr->lyrpara.fb->addr[0];
                alpha_para.dst_line_width           = dst_lyr->lyrpara.fb->size.width;
                alpha_para.dst_window_rect          = dst_window_rect;

                if (!value)
                {
                    alpha_para.layer_src_alpha              = 1;
                }
                else
                {
                    alpha_para.layer_src_alpha              = block_value->src_globle_value;
                }

                alpha_para.layer_dst_alpha              = 1;
                alpha_para.dst_in_alpha_mode            = MP_IN_ALPHA_PIXEL_ALPHA;

                if (BLITFLAG_COLORALPHA == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_ALPHA;
                }
                else if (BLITFLAG_ALPHACHANNEL == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_GLOBAL_ALPHA;
                }
                else
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_MULTIPLY_GLOBAL_ALPHA;
                }

                alpha_para.img_area_out_alpha_ctl       = 0;
                alpha_para.A0_out_alpha_mode            = MP_OUT_ALPHA_LAYER0_ALPHA;
                alpha_para.A1_out_alpha_mode            = MP_OUT_ALPHA_LAYER1_ALPHA;
                alpha_para.A2_out_alpha_mode            = MP_OUT_ALPHA_FORMULA;
                alpha_para.img_area_out_alpha_value     = 0xFF;
                alpha_para.non_img_area_out_alpha_value = 0xFF;
                eLIBs_fioctrl(gmpfile, MP_CMD_ALPHA_BLEND, SEL_SCREEN, &alpha_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            case BLITFLAG_COLORKEY:
            {
                __mp_colorkey_t colorkey_para;
                //__mp_ck_ctrl_t  ck_ctrl;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                colorkey_para.layer_addr[0]                 = src_block->fb_addr[0];
                colorkey_para.layer_line_width              = src_block->fb_width;
                colorkey_para.layer_pixelfmt                = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                colorkey_para.layer_byte_seq                = src_block->fb_byte_seq;
                colorkey_para.layer_src_rect                = lyr_src_rect;
                colorkey_para.layer_dst_rect.x              = dx;
                colorkey_para.layer_dst_rect.y              = dy;
                colorkey_para.dst_addr[0]                   = (__u32)dst_lyr->lyrpara.fb->addr[0];
                colorkey_para.dst_line_width                = dst_lyr->lyrpara.fb->size.width;
                colorkey_para.dst_window_rect               = dst_window_rect;;
                colorkey_para.ck_ctrl.red_ctrl              = 0;
                colorkey_para.ck_ctrl.green_ctrl            = 0;
                colorkey_para.ck_ctrl.blue_ctrl             = 0;
                colorkey_para.ck_ctrl.max_color             = GUI_Color2MpColor(block_value->ck_value);
                colorkey_para.ck_ctrl.min_color             = GUI_Color2MpColor(block_value->ck_value);
                //colorkey_para.ck_ctrl.ky_type                 = 0;
                //colorkey_para.ck_ctrl                         = ck_ctrl;
                eLIBs_fioctrl(gmpfile, MP_CMD_COLORKEY, SEL_SCREEN, &colorkey_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("alpha mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    /*************************************************************************************************************/
    //rotate//
    if (0 != (flags & BLITRFMASK))
    {
        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = src_block->fb_addr[0];
                rotate_para.layer_line_width        = src_block->fb_width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                rotate_para.layer_byte_seq          = src_block->fb_byte_seq;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = dx;
                rotate_para.layer_dst_rect.y        = dy;
                rotate_para.dst_addr[0]             = (__u32)dst_lyr->lyrpara.fb->addr[0];
                rotate_para.dst_line_width          = dst_lyr->lyrpara.fb->size.width;
                rotate_para.dst_window_rect         = dst_window_rect;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_90;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_180;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_270;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_MIRROR_LR;
                }
                else
                {
                    rotate_para.win_op = MP_WIN_OP_MIRROR_UD;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    /*************************************************************************************************************/
    //bitblt//
    if (0 != (flags & BLITBITMASK))
    {
        switch (flags & BLITBITMASK)
        {
            case BLITFLAG_AND:
            case BLITFLAG_OR:
            case BLITFLAG_XOR:
            {
                __mp_bitblt_t bitblt_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                bitblt_para.layer_addr[0]               = src_block->fb_addr[0];
                bitblt_para.layer_line_width            = src_block->fb_width;
                bitblt_para.layer_pixelfmt              = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_block->fb_fmt);
                bitblt_para.layer_byte_seq              = src_block->fb_byte_seq;
                bitblt_para.layer_src_rect              = lyr_src_rect;
                bitblt_para.layer_dst_rect.x            = dx;
                bitblt_para.layer_dst_rect.y            = dy;
                bitblt_para.dst_addr[0]                 = (__u32)dst_lyr->lyrpara.fb->addr[0];
                bitblt_para.dst_line_width              = dst_lyr->lyrpara.fb->size.width;
                bitblt_para.dst_window_rect             = dst_window_rect;

                if (BLITFLAG_AND == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_AND;
                }
                else if (BLITFLAG_OR == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_OR;
                }
                else
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_XOR;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_BITBLT, SEL_SCREEN, &bitblt_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default:
                ORANGE_WRN("Bit flags is wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockGetLayerMem(H_LYR  hsrclayer, RECT *srcrect, HBLOCK hdstblock, __s32 dx, __s32 dy, __u32 flags, void *value)
{
    __gui_lyrwin_t          *src_lyr    = NULL;
    __gui_block_t           *dst_block  = NULL;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    __gui_block_value_t     *block_value = NULL;
    __mp_rect_t             lyr_src_rect;
    src_lyr = GUI_LyrH2P(hsrclayer);

    if (!src_lyr)
    {
        ORANGE_WRN("source layer is null!\n");
        return ORANGE_FAIL;
    }

    dst_block = (__gui_block_t *)hdstblock;

    if (!dst_block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    if (NULL == srcrect)
    {
        lyr_src_rect.x      = 0;
        lyr_src_rect.y      = 0;
        lyr_src_rect.width  = src_lyr->lyrpara.fb->size.width;
        lyr_src_rect.height = src_lyr->lyrpara.fb->size.height;
    }
    else
    {
        lyr_src_rect.x      = srcrect->x;
        lyr_src_rect.y      = srcrect->y;
        lyr_src_rect.width  = srcrect->width;
        lyr_src_rect.height = srcrect->height;
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return ORANGE_FAIL;
    }

    block_value = (__gui_block_value_t *)value;

    /*************************************************************************************************************/
    //copy//
    if (BLITFLAG_COLORSET == (flags & BLITMASK))
    {
        __mp_copy_t copy_para;
        hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

        if (hdle == 0)
        {
            ORANGE_WRN("request scaler fail\n");
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
            return ORANGE_FAIL;
        }

        copy_para.layer_addr[0]         = (__u32)src_lyr->lyrpara.fb->addr[0];
        copy_para.layer_line_width      = src_lyr->lyrpara.fb->size.width;
        copy_para.layer_pixelfmt        = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
        copy_para.layer_byte_seq        = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
        copy_para.layer_src_rect        = lyr_src_rect;
        copy_para.layer_dst_rect.x      = dx;
        copy_para.layer_dst_rect.y      = dy;
        copy_para.dst_addr[0]           = dst_block->fb_addr[0];
        copy_para.dst_line_width        = dst_block->fb_width;
        copy_para.dst_window_rect.x     = dst_block->clip_rect.x;
        copy_para.dst_window_rect.y     = dst_block->clip_rect.y;
        copy_para.dst_window_rect.width = dst_block->clip_rect.width;
        copy_para.dst_window_rect.height = dst_block->clip_rect.height;
        eLIBs_fioctrl(gmpfile, MP_CMD_COPY, SEL_SCREEN, &copy_para);
        eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //rotate and alpha//

    if ((0 != (flags & BLITALPHAMASK)) && (0 != (flags & BLITRFMASK)))
    {
        void            *addr   = NULL;
        __u8            bpp;
        __u32           size    = 0;
        __u32           mid_src_width, mid_src_height;

        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

                if (bpp)
                {
                    /*get the size of framebuffer*/
                    size = (dst_block->fb_width * dst_block->fb_height * bpp + 7) >> 3;
                    addr = orange_pmalloc(size);

                    if (!addr)
                    {
                        ORANGE_WRN("pmalloc framebuffer failed!\n");
                        return ORANGE_FAIL;
                    }
                }

                //orange_memcpy(addr, (void *)dst_block->fb_addr[0], size);
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = (__u32)src_lyr->lyrpara.fb->addr[0];
                rotate_para.layer_line_width        = src_lyr->lyrpara.fb->size.width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                rotate_para.layer_byte_seq          = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = 0;
                rotate_para.layer_dst_rect.y        = 0;
                rotate_para.dst_addr[0]             = (__u32)addr;
                rotate_para.dst_line_width          = dst_block->fb_width;
                rotate_para.dst_window_rect.x       = 0;
                rotate_para.dst_window_rect.y       = 0;
                rotate_para.dst_window_rect.width   = dst_block->fb_width;
                rotate_para.dst_window_rect.height  = dst_block->fb_height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_90;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_180;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_270;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_LR;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_UD;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                //orange_pfree(addr, size);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }

        switch (flags & BLITALPHAMASK)
        {
            case BLITFLAG_COLORALPHA:
            case BLITFLAG_ALPHACHANNEL:
            case BLITFLAG_BOTHALPHA:
            {
                __mp_alpha_blend_t alpha_para;
                //block_value = (__gui_block_value_t *)value;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_block->fb_width)
                {
                    mid_src_width = dst_block->fb_width;
                }

                if (mid_src_height >= dst_block->fb_height)
                {
                    mid_src_height = dst_block->fb_height;
                }

                alpha_para.layer_addr[0]                = (__u32)addr;
                alpha_para.layer_line_width             = dst_block->fb_width;
                alpha_para.layer_pixelfmt               = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_block->fb_fmt);
                alpha_para.layer_byte_seq               = dst_block->fb_byte_seq;
                alpha_para.layer_src_rect.x             = 0;
                alpha_para.layer_src_rect.y             = 0;
                alpha_para.layer_src_rect.width         = mid_src_width;
                alpha_para.layer_src_rect.height        = mid_src_height;
                alpha_para.layer_dst_rect.x             = dx;
                alpha_para.layer_dst_rect.y             = dy;
                alpha_para.dst_addr[0]                  = dst_block->fb_addr[0];
                alpha_para.dst_line_width               = dst_block->fb_width;
                alpha_para.dst_window_rect.x            = dst_block->clip_rect.x;
                alpha_para.dst_window_rect.y            = dst_block->clip_rect.y;
                alpha_para.dst_window_rect.width        = dst_block->clip_rect.width;
                alpha_para.dst_window_rect.height       = dst_block->clip_rect.height;

                if (!value)
                {
                    alpha_para.layer_src_alpha              = 1;
                }
                else
                {
                    alpha_para.layer_src_alpha              = block_value->src_globle_value;
                }

                alpha_para.layer_dst_alpha              = 1;
                alpha_para.dst_in_alpha_mode            = MP_IN_ALPHA_PIXEL_ALPHA;

                if (BLITFLAG_COLORALPHA == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_ALPHA;
                }
                else if (BLITFLAG_ALPHACHANNEL == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_GLOBAL_ALPHA;
                }
                else
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_MULTIPLY_GLOBAL_ALPHA;
                }

                alpha_para.img_area_out_alpha_ctl       = 0;
                alpha_para.A0_out_alpha_mode            = MP_OUT_ALPHA_LAYER0_ALPHA;
                alpha_para.A1_out_alpha_mode            = MP_OUT_ALPHA_LAYER1_ALPHA;
                alpha_para.A2_out_alpha_mode            = MP_OUT_ALPHA_FORMULA;
                alpha_para.img_area_out_alpha_value     = 0xFF;
                alpha_para.non_img_area_out_alpha_value = 0xFF;
                eLIBs_fioctrl(gmpfile, MP_CMD_ALPHA_BLEND, SEL_SCREEN, &alpha_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            case BLITFLAG_COLORKEY:
            {
                __mp_colorkey_t colorkey_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_block->fb_width)
                {
                    mid_src_width = dst_block->fb_width;
                }

                if (mid_src_height >= dst_block->fb_height)
                {
                    mid_src_height = dst_block->fb_height;
                }

                colorkey_para.layer_addr[0]             = (__u32)addr;
                colorkey_para.layer_line_width          = dst_block->fb_width;
                colorkey_para.layer_pixelfmt            = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_block->fb_fmt);
                colorkey_para.layer_byte_seq            = dst_block->fb_byte_seq;
                colorkey_para.layer_src_rect.x          = 0;
                colorkey_para.layer_src_rect.y          = 0;
                colorkey_para.layer_src_rect.width      = mid_src_width;
                colorkey_para.layer_src_rect.height     = mid_src_height;
                colorkey_para.layer_dst_rect.x          = dx;
                colorkey_para.layer_dst_rect.y          = dy;
                colorkey_para.dst_addr[0]                   = dst_block->fb_addr[0];
                colorkey_para.dst_line_width                = dst_block->fb_width;
                colorkey_para.dst_window_rect.x             = dst_block->clip_rect.x;
                colorkey_para.dst_window_rect.y             = dst_block->clip_rect.y;
                colorkey_para.dst_window_rect.width         = dst_block->clip_rect.width;
                colorkey_para.dst_window_rect.height        = dst_block->clip_rect.height;
                colorkey_para.ck_ctrl.red_ctrl              = 0;
                colorkey_para.ck_ctrl.green_ctrl            = 0;
                colorkey_para.ck_ctrl.blue_ctrl             = 0;
                colorkey_para.ck_ctrl.max_color             = GUI_Color2MpColor(block_value->ck_value);
                colorkey_para.ck_ctrl.min_color             = GUI_Color2MpColor(block_value->ck_value);
                //colorkey_para.ck_ctrl.ky_type                 = 0;
                //colorkey_para.ck_ctrl                         = ck_ctrl;
                eLIBs_fioctrl(gmpfile, MP_CMD_COLORKEY, SEL_SCREEN, &colorkey_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
            {
                ORANGE_WRN("alpha mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;

                if (addr)
                {
                    orange_pfree(addr, size);
                    addr = NULL;
                }

                return ORANGE_FAIL;
            }
        }

        if (addr)
        {
            orange_pfree(addr, size);
            addr = NULL;
        }

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //rotate and bitblt//
    if ((0 != (flags & BLITBITMASK)) && (0 != (flags & BLITRFMASK)))
    {
        void            *addr   = NULL;
        __u8            bpp;
        __u32           size    = 0;
        __u32           mid_src_width, mid_src_height;

        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                bpp = GUI_MpBlockGetBPP(dst_block->fb_fmt);

                if (bpp)
                {
                    /*get the size of framebuffer*/
                    size = (dst_block->fb_width * dst_block->fb_height * bpp + 7) >> 3;
                    addr = orange_pmalloc(size);

                    if (!addr)
                    {
                        ORANGE_WRN("pmalloc framebuffer failed!\n");
                        return ORANGE_FAIL;
                    }
                }

                //orange_memcpy(addr, (void *)dst_block->fb_addr[0], size);
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = (__u32)src_lyr->lyrpara.fb->addr[0];
                rotate_para.layer_line_width        = src_lyr->lyrpara.fb->size.width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                rotate_para.layer_byte_seq          = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = 0;
                rotate_para.layer_dst_rect.y        = 0;
                rotate_para.dst_addr[0]             = (__u32)addr;
                rotate_para.dst_line_width          = dst_block->fb_width;
                rotate_para.dst_window_rect.x       = 0;
                rotate_para.dst_window_rect.y       = 0;
                rotate_para.dst_window_rect.width   = dst_block->fb_width;
                rotate_para.dst_window_rect.height  = dst_block->fb_height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_90;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_180;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_ROTATE_270;
                    mid_src_width           = srcrect->height;
                    mid_src_height          = srcrect->width;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_LR;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }
                else
                {
                    rotate_para.win_op  = MP_WIN_OP_MIRROR_UD;
                    mid_src_width           = srcrect->width;
                    mid_src_height          = srcrect->height;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                //orange_pfree(addr, size);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }

        switch (flags & BLITBITMASK)
        {
            case BLITFLAG_AND:
            case BLITFLAG_OR:
            case BLITFLAG_XOR:
            {
                __mp_bitblt_t bitblt_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;

                    if (addr)
                    {
                        orange_pfree(addr, size);
                        addr = NULL;
                    }

                    return ORANGE_FAIL;
                }

                if (mid_src_width >= dst_block->fb_width)
                {
                    mid_src_width = dst_block->fb_width;
                }

                if (mid_src_height >= dst_block->fb_height)
                {
                    mid_src_height = dst_block->fb_height;
                }

                bitblt_para.layer_addr[0]               = (__u32)addr;
                bitblt_para.layer_line_width            = dst_block->fb_width;
                bitblt_para.layer_pixelfmt              = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(dst_block->fb_fmt);
                bitblt_para.layer_byte_seq              = dst_block->fb_byte_seq;
                bitblt_para.layer_src_rect.x            = 0;
                bitblt_para.layer_src_rect.y            = 0;
                bitblt_para.layer_src_rect.width        = mid_src_width;
                bitblt_para.layer_src_rect.height       = mid_src_height;
                bitblt_para.layer_dst_rect.x            = dx;
                bitblt_para.layer_dst_rect.y            = dy;
                bitblt_para.dst_addr[0]                 = dst_block->fb_addr[0];
                bitblt_para.dst_line_width              = dst_block->fb_width;
                bitblt_para.dst_window_rect.x           = dst_block->clip_rect.x;
                bitblt_para.dst_window_rect.y           = dst_block->clip_rect.y;
                bitblt_para.dst_window_rect.width       = dst_block->clip_rect.width;
                bitblt_para.dst_window_rect.height      = dst_block->clip_rect.height;

                if (BLITFLAG_AND == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_AND;
                }
                else if (BLITFLAG_OR == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_OR;
                }
                else
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_XOR;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_BITBLT, SEL_SCREEN, &bitblt_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("blitbit mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;

                if (addr)
                {
                    orange_pfree(addr, size);
                    addr = NULL;
                }

                return ORANGE_FAIL;
        }

        if (addr)
        {
            orange_pfree(addr, size);
            addr = NULL;
        }

        if (gmpfile)
        {
            eLIBs_fclose(gmpfile);
            gmpfile = NULL;
        }

        return ORANGE_OK;
    }

    /*************************************************************************************************************/
    //alpha//
    if (0 != (flags & BLITALPHAMASK))
    {
        switch (flags & BLITALPHAMASK)
        {
            case BLITFLAG_COLORALPHA:
            case BLITFLAG_ALPHACHANNEL:
            case BLITFLAG_BOTHALPHA:
            {
                __mp_alpha_blend_t alpha_para;
                //block_value = (__gui_block_value_t *)value;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                alpha_para.layer_addr[0]            = (__u32)src_lyr->lyrpara.fb->addr[0];
                alpha_para.layer_line_width         = src_lyr->lyrpara.fb->size.width;
                alpha_para.layer_pixelfmt           = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                alpha_para.layer_byte_seq           = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                alpha_para.layer_src_rect           = lyr_src_rect;
                alpha_para.layer_dst_rect.x         = dx;
                alpha_para.layer_dst_rect.y         = dy;
                alpha_para.dst_addr[0]              = dst_block->fb_addr[0];
                alpha_para.dst_line_width           = dst_block->fb_width;
                alpha_para.dst_window_rect.x        = dst_block->clip_rect.x;
                alpha_para.dst_window_rect.y        = dst_block->clip_rect.y;
                alpha_para.dst_window_rect.width    = dst_block->clip_rect.width;
                alpha_para.dst_window_rect.height   = dst_block->clip_rect.height;

                if (!value)
                {
                    alpha_para.layer_src_alpha          = 1;
                }
                else
                {
                    alpha_para.layer_src_alpha          = block_value->src_globle_value;
                }

                alpha_para.layer_dst_alpha              = 1;
                alpha_para.dst_in_alpha_mode            = MP_IN_ALPHA_PIXEL_ALPHA;

                if (BLITFLAG_COLORALPHA == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_ALPHA;
                }
                else if (BLITFLAG_ALPHACHANNEL == (flags & BLITALPHAMASK))
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_GLOBAL_ALPHA;
                }
                else
                {
                    alpha_para.src_in_alpha_mode        = MP_IN_ALPHA_PIXEL_MULTIPLY_GLOBAL_ALPHA;
                }

                alpha_para.img_area_out_alpha_ctl       = 0;
                alpha_para.A0_out_alpha_mode            = MP_OUT_ALPHA_LAYER0_ALPHA;
                alpha_para.A1_out_alpha_mode            = MP_OUT_ALPHA_LAYER1_ALPHA;
                alpha_para.A2_out_alpha_mode            = MP_OUT_ALPHA_FORMULA;
                alpha_para.img_area_out_alpha_value     = 0xFF;
                alpha_para.non_img_area_out_alpha_value = 0xFF;
                eLIBs_fioctrl(gmpfile, MP_CMD_ALPHA_BLEND, SEL_SCREEN, &alpha_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            case BLITFLAG_COLORKEY:
            {
                __mp_colorkey_t colorkey_para;
                //__mp_ck_ctrl_t  ck_ctrl;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                colorkey_para.layer_addr[0]                 = (__u32)src_lyr->lyrpara.fb->addr[0];
                colorkey_para.layer_line_width              = src_lyr->lyrpara.fb->size.width;
                colorkey_para.layer_pixelfmt                = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                colorkey_para.layer_byte_seq                = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                colorkey_para.layer_src_rect                = lyr_src_rect;
                colorkey_para.layer_dst_rect.x              = dx;
                colorkey_para.layer_dst_rect.y              = dy;
                colorkey_para.dst_addr[0]                   = dst_block->fb_addr[0];
                colorkey_para.dst_line_width                = dst_block->fb_width;
                colorkey_para.dst_window_rect.x             = dst_block->clip_rect.x;
                colorkey_para.dst_window_rect.y             = dst_block->clip_rect.y;
                colorkey_para.dst_window_rect.width         = dst_block->clip_rect.width;
                colorkey_para.dst_window_rect.height        = dst_block->clip_rect.height;
                colorkey_para.ck_ctrl.red_ctrl              = 0;
                colorkey_para.ck_ctrl.green_ctrl            = 0;
                colorkey_para.ck_ctrl.blue_ctrl             = 0;
                colorkey_para.ck_ctrl.max_color             = GUI_Color2MpColor(block_value->ck_value);
                colorkey_para.ck_ctrl.min_color             = GUI_Color2MpColor(block_value->ck_value);
                //colorkey_para.ck_ctrl.ky_type                 = 0;
                //colorkey_para.ck_ctrl                         = ck_ctrl;
                eLIBs_fioctrl(gmpfile, MP_CMD_COLORKEY, SEL_SCREEN, &colorkey_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default :
                ORANGE_WRN("alpha mode selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    /*************************************************************************************************************/
    //rotate//
    if (0 != (flags & BLITRFMASK))
    {
        switch (flags & BLITRFMASK)
        {
            case BLITFLAG_ROTATE90:
            case BLITFLAG_ROTATE180:
            case BLITFLAG_ROTATE270:
            case BLITFLAG_HFLIP:
            case BLITFLAG_VFLIP:
            {
                __mp_rotate_mirror_t rotate_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                rotate_para.layer_addr[0]           = (__u32)src_lyr->lyrpara.fb->addr[0];;
                rotate_para.layer_line_width        = src_lyr->lyrpara.fb->size.width;
                rotate_para.layer_pixelfmt          = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                rotate_para.layer_byte_seq          = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                rotate_para.layer_src_rect          = lyr_src_rect;
                rotate_para.layer_dst_rect.x        = dx;
                rotate_para.layer_dst_rect.y        = dy;
                rotate_para.dst_addr[0]             = dst_block->fb_addr[0];
                rotate_para.dst_line_width          = dst_block->fb_width;
                rotate_para.dst_window_rect.x       = dst_block->clip_rect.x;
                rotate_para.dst_window_rect.y       = dst_block->clip_rect.y;
                rotate_para.dst_window_rect.width   = dst_block->clip_rect.width;
                rotate_para.dst_window_rect.height  = dst_block->clip_rect.height;

                if (BLITFLAG_ROTATE90 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_90;
                }
                else if (BLITFLAG_ROTATE180 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_180;
                }
                else if (BLITFLAG_ROTATE270 == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_ROTATE_270;
                }
                else if (BLITFLAG_HFLIP == (flags & BLITRFMASK))
                {
                    rotate_para.win_op = MP_WIN_OP_MIRROR_LR;
                }
                else
                {
                    rotate_para.win_op = MP_WIN_OP_MIRROR_UD;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_ROTATE_MIRROR, SEL_SCREEN, &rotate_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break; //or return
            }

            default:
                ORANGE_WRN("rotate or mirror flags selected wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    /*************************************************************************************************************/
    //bitblt//
    if (0 != (flags & BLITBITMASK))
    {
        switch (flags & BLITBITMASK)
        {
            case BLITFLAG_AND:
            case BLITFLAG_OR:
            case BLITFLAG_XOR:
            {
                __mp_bitblt_t bitblt_para;
                hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

                if (hdle == 0)
                {
                    ORANGE_WRN("request scaler fail\n");
                    eLIBs_fclose(gmpfile);
                    gmpfile = NULL;
                    return ORANGE_FAIL;
                }

                bitblt_para.layer_addr[0]               = (__u32)src_lyr->lyrpara.fb->addr[0];
                bitblt_para.layer_line_width            = src_lyr->lyrpara.fb->size.width;
                bitblt_para.layer_pixelfmt              = (__MP_pixel_rgbfmt_t)GUI_Pixelfmt2MpPixelfmt(src_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);
                bitblt_para.layer_byte_seq              = src_lyr->lyrpara.fb->fmt.fmt.rgb.br_swap;
                bitblt_para.layer_src_rect              = lyr_src_rect;
                bitblt_para.layer_dst_rect.x            = dx;
                bitblt_para.layer_dst_rect.y            = dy;
                bitblt_para.dst_addr[0]                 = dst_block->fb_addr[0];
                bitblt_para.dst_line_width              = dst_block->fb_width;
                bitblt_para.dst_window_rect.x           = dst_block->clip_rect.x;
                bitblt_para.dst_window_rect.y           = dst_block->clip_rect.y;
                bitblt_para.dst_window_rect.width       = dst_block->clip_rect.width;
                bitblt_para.dst_window_rect.height      = dst_block->clip_rect.height;

                if (BLITFLAG_AND == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_AND;
                }
                else if (BLITFLAG_OR == (flags & BLITBITMASK))
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_OR;
                }
                else
                {
                    bitblt_para.layers_bitblt_op = MP_BITBLT_XOR;
                }

                eLIBs_fioctrl(gmpfile, MP_CMD_BITBLT, SEL_SCREEN, &bitblt_para);
                eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);
                break;
            }

            default:
                ORANGE_WRN("Bit flags is wrong!\n");
                eLIBs_fclose(gmpfile);
                gmpfile = NULL;
                return ORANGE_FAIL;
        }
    }

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_GetBlockWidth(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    return block->block_rect.width;//fb_width;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_GetBlockHeight(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    return block->block_rect.height;//fb_height;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_GetBlockFormat(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    return block->fb_fmt;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_GetBlockAddr(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    return block->fb_addr[0];
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_GetBlockFBWidth(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    return block->fb_width;
}


/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_GetBlockFBHeight(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    return block->fb_height;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockLock(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    pthread_mutex_lock(block->lock);
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockUnLock(HBLOCK hblock)
{
    __gui_block_t       *block      = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    pthread_mutex_unlock(block->lock);
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_BlockBitBlit
*
* author:
* date:             2011-1-12 10:20:07
* Description:      Destroy mp block
* parameters:
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__s32 GUI_BlockFill(HBLOCK hblock, GUI_COLOR colorvalue)
{
    __gui_block_t       *block      = NULL;
    __mp_color_t        mp_color;
    __mp_fill_t         fill_para;
    ES_FILE                 *gmpfile    = NULL;
    __s32                   hdle        = NULL;
    block = (__gui_block_t *)hblock;

    if (!block)
    {
        ORANGE_WRN("mp block is null!\n");
        return ORANGE_FAIL;
    }

    gmpfile = eLIBs_fopen("b:\\MP\\MIXER", "r+");

    if (gmpfile == ((ES_FILE *)0))
    {
        ORANGE_WRN("Don't open the mp_drv!\n");
        return ORANGE_FAIL;
    }

    hdle = eLIBs_fioctrl(gmpfile, MP_CMD_REQUEST, SEL_SCREEN, NULL);

    if (hdle == 0)
    {
        ORANGE_WRN("request scaler fail\n");
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
        return ORANGE_FAIL;
    }

    /*mp_color.alpha = (__u8)((colorvalue & 0xFF000000)>>24);
    mp_color.red   = (__u8)((colorvalue & 0x00FF0000)>>16);
    mp_color.green = (__u8)((colorvalue & 0x0000FF00)>>8);
    mp_color.blue  = (__u8)(colorvalue & 0x000000FF);*/
    //mp_color  = GUI_Color2MpColor(colorvalue);
    fill_para.dst_addr[0]               = block->fb_addr[0];
    fill_para.dst_line_width            = block->fb_width;
    fill_para.dst_window_rect.x         = block->clip_rect.x;
    fill_para.dst_window_rect.y         = block->clip_rect.y;
    fill_para.dst_window_rect.width     = block->clip_rect.width;
    fill_para.dst_window_rect.height    = block->clip_rect.height;
    fill_para.fill_color = GUI_Color2MpColor(colorvalue); //mp_color;
    eLIBs_fioctrl(gmpfile, MP_CMD_FILL, SEL_SCREEN, &fill_para);
    eLIBs_fioctrl(gmpfile, MP_CMD_RELEASE, SEL_SCREEN, NULL);

    if (gmpfile)
    {
        eLIBs_fclose(gmpfile);
        gmpfile = NULL;
    }

    return ORANGE_OK;
}






#endif
