/*
************************************************************************************************************************
*                                                         Progbar
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : progbar.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.10.09
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
* Gary.Wang      2009.10.09       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __progbar_c
#define  __progbar_c


#include "progbar_i.h"
#include <elibs_stdlib.h>



#define PROGBAR_DEFAULT_MIN             0
#define PROGBAR_DEFAULT_MAX             100
#define PROGBAR_DEFAULT_LINE_SIZE       4



typedef struct _HPGB_t
{
    H_LYR   h_lyr;
    RECT    disp_rect;
    RECT    valid_rect;
    __u32   byte_count;
    void   *bk_matrix;
    void   *ft_matrix;
    void   *frame_bmp_buf;
    __u32   frame_length;
    GUI_FONT *font;
    __u32   text_color;
    progbar_text_type_e  text_type;
    GUI_MEMDEV_Handle  hmem;
    __s32   min;
    __s32   max;
    __s32   value;
    __s32   length;
    __s32   line_size;
    __s32   last_key;
    __bool  is_internal_cmd;
    __bool  is_waking;
    __bool  is_showing;
    __s32   old_length;
    __bool  old_is_waking;
    __bool  old_is_showing;
} HPGB_t, *HPGB;


#define progbar_is_waking_i( hpgb )  ( hpgb->is_waking )


static void   _release_res(HPGB hpgb);
static __bool _is_valid_handle(HPGB hpgb);
static void   _progbar_reset_i(H_WIN hwin, HPGB hpgb);
static __s32  _init_bK_ft(HPGB hpgb, const void *head_bmp_buf, const void *tail_bmp_buf, __bool is_vertical);
static void   _draw_frame(const void *bmp_buf, __s32 x, __s32 y);
static void   _draw_progbar(HPGB hpgb, __u32 dwStyle);
static __u32  _calculate_length(__s32 min, __s32 max, __s32 value, __u32 total_length);
static __s32  _calculate_value(__s32 min, __s32 max, __u32 length, __u32 total_length);
static void   _redraw(HPGB hpgb, __u32 new_length, __u32 dwStyle);
static __s32  _init_hpgb(HPGB hpgb, H_WIN hwin);
static __s32  _ondraw(HPGB hpgb, H_WIN hwin);
static void   _redraw_vert(HPGB hpgb, __u32 new_length);
static void   _redraw_hori(HPGB hpgb, __u32 new_length);
static void   _draw_progbar_vert(HPGB hpgb);
static void   _draw_progbar_hori(HPGB hpgb);
static __s32  _ProgbarCtrlProc(__gui_msg_t *msg);
static void   _save_locale(HPGB hpgb);
static __bool _my_GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd);
static __s32  _reset_slid(HPGB hpgb, H_WIN hwin);
static void   _draw_text(HPGB hpgb);


__bool RegisterProgbarControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_PROGBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = _ProgbarCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == ORANGE_OK;
}



__s32  PROGBAR_SetRange(H_WIN hwin, __s32 min, __s32 max)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);
    __s32  total_length;

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    if (!progbar_is_waking_i(hpgb))
    {
        return -1;
    }

    if (min >= max)
    {
        return -1;
    }

    hpgb->min = min;
    hpgb->max = max;
    total_length = (GUI_WinGetStyle(hwin) & PGBS_VERTICAL) ?
                   hpgb->valid_rect.height : hpgb->valid_rect.width;

    if (GUI_WinGetCaptureWin() == hwin)           // is the capture window
    {
        /* calculate new value */
        hpgb->value = _calculate_value(min, max, hpgb->length, total_length);
        return 0;
    }
    else
    {
        /* calculate new length */
        if (hpgb->value < min  ||  hpgb->value > max)         // value is no longer valid
        {
            hpgb->value = 0;
            hpgb->length = 0;
        }
        else
        {
            hpgb->length = _calculate_length(min, max, hpgb->value, total_length);
        }

        if (hpgb->length != hpgb->old_length)
        {
            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);        // redraw
        }

        return 0;
    }
}



__s32  PROGBAR_SetValue(H_WIN hwin, __s32 value)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    if (!progbar_is_waking_i(hpgb))
    {
        return -1;
    }

    if (value < hpgb->min || value > hpgb->max)
    {
        return -1;
    }

    if (GUI_WinGetCaptureWin() == hwin)           // is the capture window
    {
        return 0;
    }

    hpgb->value = value;
    /* calculate new length */
    {
        __s32  total_length;
        total_length = (GUI_WinGetStyle(hwin) & PGBS_VERTICAL) ?
                       hpgb->valid_rect.height - hpgb->frame_length : hpgb->valid_rect.width - hpgb->frame_length;
        hpgb->length = _calculate_length(hpgb->min, hpgb->max, hpgb->value, total_length);
    }

    if (hpgb->length != hpgb->old_length)
    {
        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);        // redraw
    }

    return 0;
}



__s32  PROGBAR_GetMin(H_WIN hwin)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    return hpgb->min;
}



__s32  PROGBAR_GetMax(H_WIN hwin)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    return hpgb->max;
}



__s32  PROGBAR_GetValue(H_WIN hwin)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    return hpgb->value;
}



__s32  PROGBAR_SetLineSize(H_WIN hwin, __u32 line_size)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    hpgb->line_size = line_size;
    return 0;
}



__s32  PROGBAR_GetLineSize(H_WIN hwin)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hpgb))
    {
        return -1;
    }

    return hpgb->line_size;
}



static __u32  _calculate_length(__s32 min, __s32 max, __s32 value, __u32 total_length)
{
    __u32  length;
    __u32  cur;
    __u32  total;
    cur   = value - min;
    total = max - min;

    if (cur == 0)
    {
        length = 0;
    }
    else if (cur == total)
    {
        length = total_length;
    }
    else
    {
        if (total_length < (__u32) - 1 / cur)               // a * b < 0xffffffff
        {
            length = total_length * cur / total;
        }
        else                                                // a * b >= 0xffffffff
        {
            length = cur / (total / total_length);
        }
    }

    return length;
}



static __s32  _calculate_value(__s32 min, __s32 max, __u32 length, __u32 total_length)
{
    __u32  cur;
    __u32  total;
    total = max - min;

    if (length == 0)
    {
        cur = 0;
    }
    else if (length == total_length)
    {
        cur = total;
    }
    else
    {
        if (total < (__u32) - 1 / length)                   // a * b < 0xffffffff
        {
            cur = total * length / total_length;
        }
        else                                                // a * b >= 0xffffffff
        {
            cur =  total / (total_length / length);
        }
    }

    return ((__s32)cur + min);
}



static void   _redraw_vert(HPGB hpgb, __u32 new_length)
{
    GUI_RECT  dst_pos;
    __u32     cur_length;
    cur_length = hpgb->length;
    /* draw frontground */
    dst_pos.x0 = hpgb->valid_rect.x;
    dst_pos.y0 = hpgb->valid_rect.y + hpgb->valid_rect.height - cur_length;
    dst_pos.x1 = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->ft_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height,
                       0, hpgb->valid_rect.height - cur_length);
    }

    /* draw background */
    dst_pos.x0 = hpgb->valid_rect.x;
    dst_pos.y0 = hpgb->valid_rect.y;
    dst_pos.x1 = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - cur_length - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->bk_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height, 0, 0);
    }

    /* draw text */
    _draw_text(hpgb);
}



static void   _redraw_hori(HPGB hpgb, __u32 new_length)
{
    GUI_RECT  dst_pos;
    __u32     cur_length;
    cur_length = hpgb->length;
    /* draw frontground */
    dst_pos.x0 = hpgb->valid_rect.x;
    dst_pos.y0 = hpgb->valid_rect.y;
    dst_pos.x1 = hpgb->valid_rect.x + cur_length - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->ft_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height, 0, 0);
    }

    /* draw background */
    dst_pos.x0 = hpgb->valid_rect.x + cur_length;
    dst_pos.y0 = hpgb->valid_rect.y;
    dst_pos.x1 = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->bk_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height, cur_length, 0);
    }

    /* draw text */
    _draw_text(hpgb);
}



static void   _redraw(HPGB hpgb, __u32 new_length, __u32 dwStyle)
{
    __bool  is_vertical;
    __bool  use_memdev;
    is_vertical = (dwStyle & PGBS_VERTICAL) ? EPDK_TRUE : EPDK_FALSE;
    use_memdev  = (dwStyle & WS_MEMDEV) ? EPDK_TRUE : EPDK_FALSE;
    GUI_LyrWinSel(hpgb->h_lyr);

    if (use_memdev)                                         // use memory device
    {
        GUI_MEMDEV_Select(hpgb->hmem);                     // start memory device
    }

    if (is_vertical)
    {
        _redraw_vert(hpgb, new_length);
    }
    else
    {
        _redraw_hori(hpgb, new_length);
    }

    GUI_LyrWinFlushFB(hpgb->h_lyr);

    if (use_memdev)                                         // use memory device
    {
        GUI_MEMDEV_CopyToLCD(hpgb->hmem);                  // copy from memory device to fb
        GUI_MEMDEV_Select(0);                            // stop memory device
    }
}



static void   _release_res(HPGB hpgb)
{
    if (hpgb != NULL)
    {
        if (hpgb->hmem != 0)
        {
            GUI_MEMDEV_Delete(hpgb->hmem);
            hpgb->hmem = 0;
        }

        if (hpgb->frame_bmp_buf != NULL)
        {
            g_free(hpgb->frame_bmp_buf);
            hpgb->frame_bmp_buf = NULL;
        }

        if (hpgb->ft_matrix != NULL)
        {
            g_free(hpgb->ft_matrix);
            hpgb->ft_matrix = NULL;
        }

        if (hpgb->bk_matrix != NULL)
        {
            g_free(hpgb->bk_matrix);
            hpgb->bk_matrix = NULL;
        }
    }
}



static __bool _is_valid_handle(HPGB hpgb)
{
    if (hpgb != NULL)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}



static void   _progbar_reset_i(H_WIN hwin, HPGB hpgb)
{
    __u32  total_length;
    total_length = (GUI_WinGetStyle(hwin) & PGBS_VERTICAL) ?
                   hpgb->valid_rect.height : hpgb->valid_rect.width;
    hpgb->length = _calculate_length(hpgb->min, hpgb->max, hpgb->value, total_length);
    hpgb->last_key        = GUI_MSG_KEY_RISE;
    hpgb->line_size       = PROGBAR_DEFAULT_LINE_SIZE;
    hpgb->is_waking       = EPDK_TRUE;
    hpgb->is_showing      = EPDK_FALSE;
    hpgb->old_length      = 0;
    hpgb->old_is_waking   = EPDK_TRUE;
    hpgb->old_is_showing  = EPDK_FALSE;
}



static void   _draw_progbar_vert(HPGB hpgb)
{
    GUI_RECT  dst_pos;
    __u32     cur_length;
    cur_length = hpgb->length;
    /* draw frame */
    _draw_frame(hpgb->frame_bmp_buf, hpgb->disp_rect.x, hpgb->disp_rect.y);
    /* draw frontground */
    dst_pos.x0 = hpgb->valid_rect.x;
    dst_pos.y0 = hpgb->valid_rect.y + hpgb->valid_rect.height - cur_length;
    dst_pos.x1 = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->ft_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height,
                       0, hpgb->valid_rect.height - cur_length);
    }

    /* draw background */
    dst_pos.x0 = hpgb->valid_rect.x;
    dst_pos.y0 = hpgb->valid_rect.y;
    dst_pos.x1 = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - cur_length - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->bk_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height, 0, 0);
    }

    /* draw text */
    _draw_text(hpgb);
}



static void   _draw_progbar_hori(HPGB hpgb)
{
    GUI_RECT  dst_pos;
    __u32     cur_length;
    cur_length = hpgb->length;
    /* draw frame */
    _draw_frame(hpgb->frame_bmp_buf, hpgb->disp_rect.x, hpgb->disp_rect.y);
    /* draw frontground */
    dst_pos.x0 = hpgb->valid_rect.x;
    dst_pos.y0 = hpgb->valid_rect.y;
    dst_pos.x1 = hpgb->valid_rect.x + cur_length - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->ft_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height, 0, 0);
    }

    /* draw background */
    dst_pos.x0 = hpgb->valid_rect.x + cur_length;
    dst_pos.y0 = hpgb->valid_rect.y;
    dst_pos.x1 = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
    dst_pos.y1 = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;

    if (dst_pos.x1 >= dst_pos.x0 && dst_pos.y1 >= dst_pos.y0)
    {
        GUI_SetRectMem(&dst_pos, hpgb->bk_matrix, hpgb->valid_rect.width, hpgb->valid_rect.height, cur_length, 0);
    }

    /* draw text */
    _draw_text(hpgb);
}



static void   _draw_text(HPGB hpgb)
{
    if (hpgb->font != NULL && hpgb->text_type != PROGBAR_TEXT_TYPE_NONE)
    {
        __s32  old_mode;
        __s32  old_font_mode;
        char   src_text[16];

        /* get text string */
        if (hpgb->text_type == PROGBAR_TEXT_TYPE_VALUE)               // show value
        {
            eLIBs_int2str_dec(hpgb->value, src_text);
        }
        else if (hpgb->text_type == PROGBAR_TEXT_TYPE_PERCENT)        // show percent
        {
            __s32  percent;
            percent = (hpgb->value - hpgb->min) * 100 / (hpgb->max - hpgb->min);
            eLIBs_int2str_dec(percent, src_text);
            eLIBs_strcat(src_text, "%");
        }

        /* draw text */
        old_mode = GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

        if (hpgb->byte_count == 4)
        {
            GUI_OpenAlphaBlend();
        }

        if (hpgb->byte_count == 1)
        {
            old_font_mode = GUI_SetFontMode(GUI_FONTMODE_8BPP32);
        }

        GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
        GUI_SetFont(hpgb->font);
        GUI_SetColor(hpgb->text_color);
        {
            GUI_RECT  rect;
            rect.x0  = hpgb->valid_rect.x;
            rect.y0  = hpgb->valid_rect.y;
            rect.x1  = hpgb->valid_rect.x + hpgb->valid_rect.width - 1;
            rect.y1  = hpgb->valid_rect.y + hpgb->valid_rect.height - 1;
            GUI_DispStringInRect(src_text, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
        }

        if (hpgb->byte_count == 1)
        {
            GUI_SetFontMode(old_font_mode);
        }

        if (hpgb->byte_count == 4)
        {
            GUI_CloseAlphaBlend();
        }

        GUI_SetDrawMode(old_mode);
    }
}



static void   _draw_progbar(HPGB hpgb, __u32 dwStyle)
{
    __bool  is_vertical;
    __bool  use_memdev;
    is_vertical = (dwStyle & PGBS_VERTICAL) ? EPDK_TRUE : EPDK_FALSE;
    use_memdev  = (dwStyle & WS_MEMDEV) ? EPDK_TRUE : EPDK_FALSE;
    GUI_LyrWinSel(hpgb->h_lyr);

    if (use_memdev)                                         // use memory device
    {
        GUI_MEMDEV_Select(hpgb->hmem);                     // start memory device
    }

    if (is_vertical)
    {
        _draw_progbar_vert(hpgb);
    }
    else
    {
        _draw_progbar_hori(hpgb);
    }

    GUI_LyrWinFlushFB(hpgb->h_lyr);

    if (use_memdev)                                         // use memory device
    {
        GUI_MEMDEV_CopyToLCD(hpgb->hmem);                  // copy from memory device to fb
        GUI_MEMDEV_Select(0);                            // stop memory device
    }
}



static __s32  _init_bK_ft(HPGB hpgb, const void *head_bmp_buf, const void *tail_bmp_buf, __bool is_vertical)
{
    HBMP   head_hbmp = 0;
    HBMP   tail_hbmp = 0;
    __u32  head_height;
    __u32  head_width;
    __u32  tail_height;
    __u32  tail_width;
    __u32  byte_count;
    /* open bmp */
    head_hbmp = bmp_open(head_bmp_buf);

    if (head_hbmp == 0)
    {
        __wrn("Fail in opening bmp buf.\n");
        goto error;
    }

    tail_hbmp = bmp_open(tail_bmp_buf);

    if (tail_hbmp == 0)
    {
        __wrn("Fail in opening bmp buf.\n");
        goto error;
    }

    head_width  = bmp_get_width(head_hbmp);
    head_height = bmp_get_height(head_hbmp);
    tail_width  = bmp_get_width(tail_hbmp);
    tail_height = bmp_get_height(tail_hbmp);
    byte_count  = hpgb->byte_count;

    /* check arg */
    if (head_width != tail_width || head_height != tail_height)
    {
        __err("Error. The width of head and tail is NOT equal.\n");
        goto error;
    }

    if (hpgb->disp_rect.width < head_width)
    {
        hpgb->valid_rect.x = hpgb->disp_rect.x;
        hpgb->valid_rect.width = hpgb->disp_rect.width;
    }
    else
    {
        hpgb->valid_rect.x = hpgb->disp_rect.x + (hpgb->disp_rect.width - head_width) / 2;
        hpgb->valid_rect.width = head_width;
    }

    if (hpgb->disp_rect.height < head_height)
    {
        hpgb->valid_rect.y = hpgb->disp_rect.y;
        hpgb->valid_rect.height = hpgb->disp_rect.height;
    }
    else
    {
        hpgb->valid_rect.y = hpgb->disp_rect.y + (hpgb->disp_rect.height - head_height) / 2;
        hpgb->valid_rect.height = head_height;
    }

    /* get bmp matrix */
    hpgb->ft_matrix = g_malloc(head_width * head_height * byte_count);

    if (hpgb->ft_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    bmp_get_matrix(head_bmp_buf, hpgb->ft_matrix);
    hpgb->bk_matrix = g_malloc(tail_width * tail_height * byte_count);

    if (hpgb->bk_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    bmp_get_matrix(tail_bmp_buf, hpgb->bk_matrix);

    if (head_hbmp != 0)
    {
        bmp_close(head_hbmp);
        head_hbmp = 0;
    }

    if (tail_hbmp != 0)
    {
        bmp_close(tail_hbmp);
        tail_hbmp = 0;
    }

    return 0;
error:

    if (hpgb->bk_matrix != NULL)
    {
        g_free(hpgb->bk_matrix);
        hpgb->bk_matrix = NULL;
    }

    if (hpgb->ft_matrix != NULL)
    {
        g_free(hpgb->ft_matrix);
        hpgb->ft_matrix = NULL;
    }

    if (head_hbmp != 0)
    {
        bmp_close(head_hbmp);
        head_hbmp = 0;
    }

    if (tail_hbmp != 0)
    {
        bmp_close(tail_hbmp);
        tail_hbmp = 0;
    }

    return -1;
}



static void   _draw_frame(const void *bmp_buf, __s32 x, __s32 y)
{
    __s32  old_mode;
    old_mode = GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_BMP_Draw(bmp_buf, x, y);
    GUI_SetDrawMode(old_mode);
}



static __s32  _ProgbarCtrlProc(__gui_msg_t *msg)
{
    __u32     dwStyle;
    HPGB_t  *hpgb = NULL;
    H_WIN     hwin;
    __bool    is_vertical;
    hwin = msg->h_deswin;
    dwStyle = GUI_WinGetStyle(hwin);
    is_vertical = (dwStyle & PGBS_VERTICAL) ? EPDK_TRUE : EPDK_FALSE;
    hpgb = (HPGB)GUI_CtrlWinGetAddData(hwin);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            HPGB_t  *hpgb = NULL;
            hpgb = (HPGB_t *) g_malloc(sizeof(HPGB_t));

            if (hpgb == NULL)
            {
                return ORANGE_FAIL;
            }

            g_memset(hpgb, 0, sizeof(HPGB_t));

            if (_init_hpgb(hpgb, hwin))
            {
                _release_res(hpgb);
                g_free(hpgb);
                hpgb = NULL;
                return ORANGE_FAIL;
            }

            GUI_CtrlWinSetAddData(hwin, (__u32)hpgb);
            return 0;
        }

        case GUI_MSG_DESTROY:
            if (hpgb != NULL)
            {
                _release_res(hpgb);
                g_free(hpgb);
                hpgb = NULL;
            }

            return 0;

        case GUI_MSG_ENABLE:
            return ORANGE_OK;

        case GUI_MSG_SET_UNFOCUS:
            break;

        case GUI_MSG_SET_FOCUS:
            break;

        case GUI_MSG_KEY:
            return 0;

        case GUI_MSG_TOUCH:
            return 0;

        case GUI_MSG_PAINT:
            _ondraw(hpgb, hwin);
            return ORANGE_OK;

        case GUI_MSG_WIN_SLEEP:
            if (!progbar_is_waking_i(hpgb))                 // has slept
            {
                return ORANGE_OK;
            }

            hpgb->is_waking = EPDK_FALSE;
            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);

            /* release memory device */
            if (hpgb->hmem != 0)
            {
                GUI_MEMDEV_Delete(hpgb->hmem);
                hpgb->hmem = 0;
            }

            return ORANGE_OK;

        case GUI_MSG_WIN_WAKEUP:
            if (progbar_is_waking_i(hpgb))                  // has waken up
            {
                return ORANGE_OK;
            }

            hpgb->is_waking = EPDK_TRUE;
            GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);
            return ORANGE_OK;

        default:
            break;
    }

    return GUI_CtrlWinDefaultProc(msg);
}



static __s32  _init_hpgb(HPGB hpgb, H_WIN hwin)
{
    __u32       frame_width;
    __u32       frame_height;
    const void *head_bmp_buf;
    const void *tail_bmp_buf;
    const void *frame_bmp_buf;
    HBMP        frame_hbmp = 0;
    progbar_para_t *para_p;
    __u32       dwStyle;
    __bool      is_vertical;
    RECT        rect;
    para_p = (progbar_para_t *)GUI_WinGetAttr(hwin);
    dwStyle = GUI_WinGetStyle(hwin);

    /* check the input arguments */
    if (para_p == NULL
        || para_p->head_bmp_buf   == NULL
        || para_p->tail_bmp_buf   == NULL
        || para_p->frame_bmp_buf == NULL)
    {
        return -1;
    }

    if (dwStyle & PGBS_VERTICAL)
    {
        is_vertical = EPDK_TRUE;
    }
    else
    {
        is_vertical = EPDK_FALSE;
    }

    GUI_WinGetClientFBRect(hwin, &rect);
    head_bmp_buf   = para_p->head_bmp_buf  ;
    tail_bmp_buf   = para_p->tail_bmp_buf  ;
    frame_bmp_buf  = para_p->frame_bmp_buf;
    frame_hbmp = bmp_open(frame_bmp_buf);

    if (frame_hbmp == 0)
    {
        goto error;
    }

    frame_width  = bmp_get_width(frame_hbmp);
    frame_height = bmp_get_height(frame_hbmp);

    if (frame_width > rect.width || frame_height > rect.height)
    {
        __wrn("progbar input arg is wrong.\n");
        goto error;
    }

    /* init basic info */
    hpgb->h_lyr            = (H_LYR)GUI_WinGetLyrWin(hwin);
    hpgb->byte_count       = bmp_get_bitcount(frame_hbmp) >> 3;
    hpgb->disp_rect.x      = rect.x     ;
    hpgb->disp_rect.y      = rect.y     ;
    hpgb->disp_rect.width  = frame_width;
    hpgb->disp_rect.height = frame_height;
    hpgb->font             = para_p->font;
    hpgb->text_type        = para_p->text_type;
    hpgb->min              = para_p->min;
    hpgb->max              = para_p->max;
    hpgb->value            = para_p->value;
    hpgb->text_color       = para_p->text_color;
    /* init frame info */
    hpgb->frame_bmp_buf = g_malloc(bmp_get_size(frame_hbmp));

    if (hpgb->frame_bmp_buf == NULL)
    {
        goto error;
    }

    g_memcpy(hpgb->frame_bmp_buf, frame_bmp_buf, bmp_get_size(frame_hbmp));

    /* init background and frontground info */
    if (_init_bK_ft(hpgb, head_bmp_buf, tail_bmp_buf, is_vertical))
    {
        goto error;
    }

    /* create the memory device */
    if (dwStyle & WS_MEMDEV)                                // use memory device
    {
        GUI_LyrWinSel(hpgb->h_lyr);
        hpgb->hmem = GUI_MEMDEV_Create(hpgb->disp_rect.x,
                                       hpgb->disp_rect.y,
                                       hpgb->disp_rect.width  - 1,
                                       hpgb->disp_rect.height - 1);

        if (hpgb->hmem == 0)
        {
            __wrn("Fail in creating memory device.\n");
            goto error;
        }
    }

    /* reset status */
    _progbar_reset_i(hwin, hpgb);
    __msg("is_showing is %d and old_is_showing is %d.\n", hpgb->is_showing, hpgb->old_is_showing);

    if (frame_hbmp != 0)
    {
        bmp_close(frame_hbmp);
        frame_hbmp = 0;
    }

    return 0;
error:

    if (frame_hbmp != 0)
    {
        bmp_close(frame_hbmp);
        frame_hbmp = 0;
    }

    _release_res(hpgb);
    hpgb = NULL;
    return -1;
}



static __s32  _ondraw(HPGB hpgb, H_WIN hwin)
{
    __u32  dwStyle;
    dwStyle = GUI_WinGetStyle(hwin);

    if (hpgb->is_internal_cmd)                                 // internal cmd to draw
    {
        if (!hpgb->is_waking && !hpgb->old_is_waking)         // sleeping
        {
            _save_locale(hpgb);
            hpgb->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (!hpgb->is_waking && hpgb->old_is_waking)          // to sleep
        {
            _save_locale(hpgb);
            hpgb->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (hpgb->is_waking && !hpgb->old_is_waking)          // to wake up
        {
            _draw_progbar(hpgb, dwStyle);
            hpgb->is_showing = EPDK_TRUE;
            _save_locale(hpgb);
            hpgb->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (hpgb->is_showing && !hpgb->old_is_showing)        // to show
        {
            _draw_progbar(hpgb, dwStyle);
            _save_locale(hpgb);
            hpgb->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (hpgb->length != hpgb->old_length)                // pos changed
        {
            _redraw(hpgb, hpgb->length, dwStyle);
            _save_locale(hpgb);
            hpgb->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        hpgb->is_internal_cmd = EPDK_FALSE;
    }
    else
    {
        _reset_slid(hpgb, hwin);
        _draw_progbar(hpgb, dwStyle);
        hpgb->is_showing = EPDK_TRUE;
        hpgb->is_waking  = EPDK_TRUE;
        _save_locale(hpgb);
    }

    return 0;
}



static void   _save_locale(HPGB hpgb)
{
    hpgb->old_length     = hpgb->length    ;
    hpgb->old_is_waking  = hpgb->is_waking ;
    hpgb->old_is_showing = hpgb->is_showing;
}



static __bool _my_GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd)
{
    HPGB  hpgb = (HPGB) GUI_CtrlWinGetAddData(hWnd);
    hpgb->is_internal_cmd = EPDK_TRUE;
    return  GUI_InvalidateRect(hWnd, prc, bEraseBkgnd);
}



static __s32  _reset_slid(HPGB hpgb, H_WIN hwin)
{
    return 0;
}



#endif     //  ifndef __progbar_c

/* end of progbar.c */
