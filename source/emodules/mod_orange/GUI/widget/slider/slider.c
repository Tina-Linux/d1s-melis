/*
************************************************************************************************************************
*                                                         Slider
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : slider.c
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
#ifndef  __slider_c
#define  __slider_c


#include "slider_i.h"
#include "ctrl_redef.h"
#include "rect_op.h"
#include "bmp_op.h"
#include <kconfig.h>

#define SLIDER_DEFAULT_MIN             0
#define SLIDER_DEFAULT_MAX             100
#define SLIDER_DEFAULT_LINE_SIZE       4

typedef struct _HSLID_t
{
    H_LYR   h_lyr;
    RECT    disp_rect;
    __u32   byte_count;
    void   *bk_matrix;
    void   *ft_matrix;
    void   *cursor_bmp_buf;
    __u32   cursor_length;
    __u32   bar_length;       // bar length : no direction, equal to head and tail bmp height if horizontal, equal to head and tail bmp width if vertical.
    GUI_MEMDEV_Handle  hmem;
    __s32   min;
    __s32   max;
    __s32   value;
    __s32   length;
    __s32   line_size;
    __s32   last_keyaction;  //last_key
    __bool  is_internal_cmd;
    __bool  is_waking;
    __bool  is_showing;
    __s32   old_length;
    __bool  old_is_waking;
    __bool  old_is_showing;
} HSLID_t, *HSLID;


#define slider_is_waking_i( hslid )  ( hslid->is_waking )


static void   _release_res(HSLID hslid);
static __bool _is_valid_handle(HSLID hslid);
static void   _slider_reset_i(HSLID hslid);
static __s32  _fill_bk_tail_matrix_hori(HSLID hslid, HBMP tail_hbmp, void *tail_matrix);
static __s32  _fill_ft_head_matrix_hori(HSLID hslid, HBMP head_hbmp, void *head_matrix);
static __s32  _fill_bk_mid_hori(HSLID hslid, HBMP tail_hbmp, void *tail_matrix);
static __s32  _fill_ft_mid_hori(HSLID hslid, HBMP head_hbmp, void *head_matrix);
static __s32  _fill_bk_tail_matrix_vert(HSLID hslid, HBMP tail_hbmp, void *tail_matrix);
static __s32  _fill_ft_head_matrix_vert(HSLID hslid, HBMP head_hbmp, void *head_matrix);
static __s32  _fill_bk_mid_vert(HSLID hslid, HBMP tail_hbmp, void *tail_matrix);
static __s32  _fill_ft_mid_vert(HSLID hslid, HBMP head_hbmp, void *head_matrix);
static __s32  _init_bK_ft(HSLID hslid, const void *head_bmp_buf, const void *tail_bmp_buf, __bool is_vertical);
static void   _draw_cursor(const void *bmp_buf, __s32 x, __s32 y);
static void   _draw_slider(HSLID hslid, __u32 dwStyle);
static __u32  _calculate_length(__s32 min, __s32 max, __s32 value, __u32 total_length);
static __s32  _calculate_value(__s32 min, __s32 max, __u32 length, __u32 total_length);
static void   _redraw(HSLID hslid, __u32 new_length, __u32 dwStyle);
static __s32  _init_hslid(HSLID hslid, H_WIN hwin);
static __bool _in_rect(HSLID hslid, __s32 x, __s32 y);
static __bool _in_cursor(HSLID hslid, __s32 x, __s32 y, __bool is_vertical);
static __s32  _ondraw(HSLID hslid, H_WIN hwin);
static void   _redraw_vert(HSLID hslid, __u32 new_length);
static void   _redraw_hori(HSLID hslid, __u32 new_length);
static void   _draw_slider_vert(HSLID hslid);
static void   _draw_slider_hori(HSLID hslid);
static __s32  _SliderCtrlProc(__gui_msg_t *msg);
static void   _save_locale(HSLID hslid);
static void   _notify_parent(H_WIN hwin, __u32 notify_code, __u32 dwAddData);
static __bool _my_GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd);
static void  *_get_layer_bk_matrix(HSLID hslid);
static __s32  _reset_matrix(HSLID hslid, H_WIN hwin, void *matrix);
static __s32  _reset_slid(HSLID hslid, H_WIN hwin);


__bool RegisterSliderControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_SLIDER;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = _SliderCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == ORANGE_OK;
}



__s32  SLIDER_SetRange(H_WIN hwin, __s32 min, __s32 max)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);
    __s32  total_length;

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    if (!slider_is_waking_i(hslid))
    {
        return -1;
    }

    if (min >= max)
    {
        return -1;
    }

    hslid->min = min;
    hslid->max = max;
    total_length = (GUI_WinGetStyle(hwin) & SDS_VERTICAL) ?
                   hslid->disp_rect.height - hslid->cursor_length : hslid->disp_rect.width - hslid->cursor_length;

    if (GUI_WinGetCaptureWin() == hwin)           // is the capture window
    {
        /* calculate new value */
        hslid->value = _calculate_value(min, max, hslid->length, total_length);
        return 0;
    }
    else
    {
        /* calculate new length */
        if (hslid->value < min  ||  hslid->value > max)         // value is no longer valid
        {
            hslid->value = 0;
            hslid->length = 0;
        }
        else
        {
            hslid->length = _calculate_length(min, max, hslid->value, total_length);
        }

        if (hslid->length != hslid->old_length)
        {
            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);        // redraw
            /* notify parent msg */
            _notify_parent(hwin, SDN_POS_CHANGED, 0);
        }

        return 0;
    }
}



__s32  SLIDER_SetValue(H_WIN hwin, __s32 value)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    if (!slider_is_waking_i(hslid))
    {
        return -1;
    }

    if (value < hslid->min || value > hslid->max)
    {
        return -1;
    }

    if (GUI_WinGetCaptureWin() == hwin)           // is the capture window
    {
        return 0;
    }

    hslid->value = value;
    /* calculate new length */
    {
        __s32  total_length;
        total_length = (GUI_WinGetStyle(hwin) & SDS_VERTICAL) ?
                       hslid->disp_rect.height - hslid->cursor_length : hslid->disp_rect.width - hslid->cursor_length;
        hslid->length = _calculate_length(hslid->min, hslid->max, hslid->value, total_length);
    }

    if (hslid->length != hslid->old_length)
    {
        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);        // redraw
        /* notify parent msg */
        _notify_parent(hwin, SDN_POS_CHANGED, 0);
    }

    return 0;
}



__s32  SLIDER_GetMin(H_WIN hwin)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    return hslid->min;
}



__s32  SLIDER_GetMax(H_WIN hwin)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    return hslid->max;
}



__s32  SLIDER_GetValue(H_WIN hwin)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    return hslid->value;
}



__s32  SLIDER_SetLineSize(H_WIN hwin, __u32 line_size)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    hslid->line_size = line_size;
    return 0;
}



__s32  SLIDER_GetLineSize(H_WIN hwin)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(hslid))
    {
        return -1;
    }

    return hslid->line_size;
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
        //      if( total_length < (__u32)-1 / cur )                // a * b < 0xffffffff
        //      {
        //          length = total_length * cur / total;
        //      }
        //      else                                                // a * b >= 0xffffffff
        //      {
        //          length = cur / ( total / total_length );
        //      }
        length = (__u32)((__u64)total_length * (__u64)cur / (__u64)total);
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
        //      if( total < (__u32)-1 / length )                    // a * b < 0xffffffff
        //      {
        //          cur = total * length / total_length;
        //      }
        //      else                                                // a * b >= 0xffffffff
        //      {
        //          cur =  total / ( total_length / length );
        //      }
        cur = (__u32)((__u64)total * (__u64)length / (__u64)total_length);
    }

    return ((__s32)cur + min);
}



static void   _redraw_vert(HSLID hslid, __u32 new_length)
{
    GUI_RECT  dst_pos;
    __u32     cursor_half_length;
    cursor_half_length = hslid->cursor_length >> 1;

    if (new_length == hslid->old_length)                    // the same as old
    {
        return;
    }

    if (new_length > hslid->old_length)                     // increase
    {
        /* draw frontground */
        dst_pos.x0 = hslid->disp_rect.x;
        dst_pos.y0 = hslid->disp_rect.y + hslid->disp_rect.height - (new_length + cursor_half_length);
        dst_pos.x1 = hslid->disp_rect.x + hslid->disp_rect.width - 1;
        dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - hslid->old_length - 1;
        GUI_SetRectMem(&dst_pos, hslid->ft_matrix, hslid->disp_rect.width, hslid->disp_rect.height,
                       0, hslid->disp_rect.height - (new_length + cursor_half_length));
    }
    else                                                    // decrease
    {
        /* draw background */
        dst_pos.x0 = hslid->disp_rect.x;
        dst_pos.y0 = hslid->disp_rect.y + hslid->disp_rect.height - (hslid->old_length + hslid->cursor_length);
        dst_pos.x1 = hslid->disp_rect.x + hslid->disp_rect.width - 1;
        dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - (new_length + cursor_half_length) - 1;
        GUI_SetRectMem(&dst_pos, hslid->bk_matrix, hslid->disp_rect.width, hslid->disp_rect.height,
                       0, hslid->disp_rect.height - (hslid->old_length + hslid->cursor_length));
    }

    /* draw cursor */
    _draw_cursor(hslid->cursor_bmp_buf, hslid->disp_rect.x,
                 hslid->disp_rect.y + hslid->disp_rect.height - (new_length + hslid->cursor_length));
}



static void   _redraw_hori(HSLID hslid, __u32 new_length)
{
    GUI_RECT  dst_pos;
    __u32     cursor_half_length;
    cursor_half_length = hslid->cursor_length >> 1;

    if (new_length == hslid->old_length)                    // the same as old
    {
        return;
    }

    if (new_length > hslid->old_length)                     // increase
    {
        /* draw frontground */
        dst_pos.x0 = hslid->disp_rect.x + hslid->old_length;
        dst_pos.y0 = hslid->disp_rect.y;
        dst_pos.x1 = hslid->disp_rect.x + new_length + cursor_half_length - 1;
        dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - 1;
        GUI_SetRectMem(&dst_pos, hslid->ft_matrix, hslid->disp_rect.width, hslid->disp_rect.height, hslid->old_length, 0);
    }
    else                                                    // decrease
    {
        /* draw background */
        dst_pos.x0 = hslid->disp_rect.x + new_length + cursor_half_length;
        dst_pos.y0 = hslid->disp_rect.y;
        dst_pos.x1 = hslid->disp_rect.x + hslid->old_length + hslid->cursor_length - 1;
        dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - 1;
        GUI_SetRectMem(&dst_pos, hslid->bk_matrix, hslid->disp_rect.width, hslid->disp_rect.height, new_length + cursor_half_length, 0);
    }

    /* draw cursor */
    _draw_cursor(hslid->cursor_bmp_buf, hslid->disp_rect.x + new_length, hslid->disp_rect.y);
}



static void   _redraw(HSLID hslid, __u32 new_length, __u32 dwStyle)
{
    __bool  is_vertical;
    __bool  use_memdev;
    is_vertical = (dwStyle & SDS_VERTICAL) ? EPDK_TRUE : EPDK_FALSE;
    use_memdev  = (dwStyle & WS_MEMDEV) ? EPDK_TRUE : EPDK_FALSE;
    GUI_LyrWinSel(hslid->h_lyr);

    if (use_memdev)                                         // use memory device
    {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        GUI_MEMDEV_Select(hslid->hmem);                     // start memory device
#endif
    }

    if (is_vertical)
    {
        _redraw_vert(hslid, new_length);
    }
    else
    {
        _redraw_hori(hslid, new_length);
    }

    if (use_memdev)                                         // use memory device
    {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        GUI_MEMDEV_CopyToLCD(hslid->hmem);                  // copy from memory device to fb
        GUI_MEMDEV_Select(0);                            // stop memory device
#endif
    }
}



static void   _release_res(HSLID hslid)
{
    if (hslid != NULL)
    {
        if (hslid->hmem != 0)
        {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
            GUI_MEMDEV_Delete(hslid->hmem);
#endif
            hslid->hmem = 0;
        }

        if (hslid->cursor_bmp_buf != NULL)
        {
            g_free(hslid->cursor_bmp_buf);
            hslid->cursor_bmp_buf = NULL;
        }

        if (hslid->ft_matrix != NULL)
        {
            g_free(hslid->ft_matrix);
            hslid->ft_matrix = NULL;
        }

        if (hslid->bk_matrix != NULL)
        {
            g_free(hslid->bk_matrix);
            hslid->bk_matrix = NULL;
        }
    }
}



static __bool _is_valid_handle(HSLID hslid)
{
    if (hslid != NULL)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}



static void   _slider_reset_i(HSLID hslid)
{
    hslid->min             = SLIDER_DEFAULT_MIN;
    hslid->max             = SLIDER_DEFAULT_MAX;
    hslid->value           = hslid->min;
    hslid->length          = 0;
    hslid->last_keyaction  = KEY_UP_ACTION;  //hslid->last_key  = GUI_MSG_KEY_RISE
    hslid->line_size       = SLIDER_DEFAULT_LINE_SIZE;
    hslid->is_waking       = EPDK_TRUE;
    hslid->is_showing      = EPDK_FALSE;
    hslid->old_length      = 0;
    hslid->old_is_waking   = EPDK_TRUE;
    hslid->old_is_showing  = EPDK_FALSE;
}



static void   _draw_slider_vert(HSLID hslid)
{
    GUI_RECT  dst_pos;
    __u32     cursor_half_length;
    __u32     cur_length;
    cursor_half_length = hslid->cursor_length >> 1;
    cur_length = hslid->length;
    /* draw frontground */
    dst_pos.x0 = hslid->disp_rect.x;
    dst_pos.y0 = hslid->disp_rect.y + hslid->disp_rect.height - (cur_length + cursor_half_length);
    dst_pos.x1 = hslid->disp_rect.x + hslid->disp_rect.width - 1;
    dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - 1;
    GUI_SetRectMem(&dst_pos, hslid->ft_matrix, hslid->disp_rect.width, hslid->disp_rect.height,
                   0, hslid->disp_rect.height - (cur_length + cursor_half_length));
    /* draw background */
    dst_pos.x0 = hslid->disp_rect.x;
    dst_pos.y0 = hslid->disp_rect.y;
    dst_pos.x1 = hslid->disp_rect.x + hslid->disp_rect.width - 1;
    dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - (cur_length + cursor_half_length);
    GUI_SetRectMem(&dst_pos, hslid->bk_matrix, hslid->disp_rect.width, hslid->disp_rect.height, 0, 0);
    /* draw cursor */
    _draw_cursor(hslid->cursor_bmp_buf, hslid->disp_rect.x,
                 hslid->disp_rect.y + hslid->disp_rect.height - cur_length - hslid->cursor_length);
}



static void   _draw_slider_hori(HSLID hslid)
{
    GUI_RECT  dst_pos;
    __u32     cursor_half_length;
    __u32     cur_length;
    cursor_half_length = hslid->cursor_length >> 1;
    cur_length = hslid->length;
    /* draw frontground */
    dst_pos.x0 = hslid->disp_rect.x;
    dst_pos.y0 = hslid->disp_rect.y;
    dst_pos.x1 = hslid->disp_rect.x + cur_length + cursor_half_length - 1;
    dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - 1;
    GUI_SetRectMem(&dst_pos, hslid->ft_matrix, hslid->disp_rect.width, hslid->disp_rect.height, 0, 0);
    /* draw background */
    dst_pos.x0 = hslid->disp_rect.x + cur_length + cursor_half_length;
    dst_pos.y0 = hslid->disp_rect.y;
    dst_pos.x1 = hslid->disp_rect.x + hslid->disp_rect.width - 1;
    dst_pos.y1 = hslid->disp_rect.y + hslid->disp_rect.height - 1;
    GUI_SetRectMem(&dst_pos, hslid->bk_matrix, hslid->disp_rect.width, hslid->disp_rect.height, cur_length + cursor_half_length, 0);
    /* draw cursor */
    _draw_cursor(hslid->cursor_bmp_buf, hslid->disp_rect.x + cur_length, hslid->disp_rect.y);
}



static void   _draw_slider(HSLID hslid, __u32 dwStyle)
{
    __bool  is_vertical;
    __bool  use_memdev;
    is_vertical = (dwStyle & SDS_VERTICAL) ? EPDK_TRUE : EPDK_FALSE;
    use_memdev  = (dwStyle & WS_MEMDEV) ? EPDK_TRUE : EPDK_FALSE;
    GUI_LyrWinSel(hslid->h_lyr);

    if (use_memdev)                                         // use memory device
    {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        GUI_MEMDEV_Select(hslid->hmem);                     // start memory device
#endif
    }

    if (is_vertical)
    {
        _draw_slider_vert(hslid);
    }
    else
    {
        _draw_slider_hori(hslid);
    }

    if (use_memdev)                                         // use memory device
    {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        GUI_MEMDEV_CopyToLCD(hslid->hmem);                  // copy from memory device to fb
        GUI_MEMDEV_Select(0);                            // stop memory device
#endif
    }
}



static __s32  _init_bK_ft(HSLID hslid, const void *head_bmp_buf, const void *tail_bmp_buf, __bool is_vertical)
{
    void  *src_matrix = NULL;
    __u32  buf_size;
    HBMP   head_hbmp = 0;
    void  *head_matrix = NULL;
    HBMP   tail_hbmp = 0;
    void  *tail_matrix = NULL;
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
    byte_count  = hslid->byte_count;

    /* check arg */
    if (is_vertical)
    {
        if (head_width != tail_width)
        {
            __err("Error. The width of head and tail is NOT equal.\n");
            goto error;
        }

        hslid->bar_length = head_width;
    }
    else
    {
        if (head_height != tail_height)
        {
            __err("Error. The height of head and tail is NOT equal.\n");
            goto error;
        }

        hslid->bar_length = head_height;
    }

    /* get bmp matrix */
    head_matrix = g_malloc(head_width * head_height * byte_count);

    if (head_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    bmp_get_matrix(head_bmp_buf, head_matrix);
    tail_matrix = g_malloc(tail_width * tail_height * byte_count);

    if (tail_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    bmp_get_matrix(tail_bmp_buf, tail_matrix);
    buf_size = hslid->disp_rect.width * hslid->disp_rect.height * byte_count;
    /* get layer background matrix */
    src_matrix = _get_layer_bk_matrix(hslid);

    if (src_matrix == NULL)
    {
        goto error;
    }

    /* compose background matrix */
    hslid->bk_matrix = g_malloc(buf_size);

    if (hslid->bk_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    g_memcpy(hslid->bk_matrix, src_matrix, buf_size);

    if (is_vertical)
    {
        /* fill tail */
        _fill_bk_tail_matrix_vert(hslid, tail_hbmp, tail_matrix);
        /* fill mid */
        _fill_bk_mid_vert(hslid, tail_hbmp, tail_matrix);
    }
    else
    {
        /* fill tail */
        _fill_bk_tail_matrix_hori(hslid, tail_hbmp, tail_matrix);
        /* fill mid */
        _fill_bk_mid_hori(hslid, tail_hbmp, tail_matrix);
    }

    /* compose frontground matrix */
    hslid->ft_matrix = g_malloc(buf_size);

    if (hslid->ft_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    g_memcpy(hslid->ft_matrix, src_matrix, buf_size);

    if (is_vertical)
    {
        /* fill tail */
        _fill_ft_head_matrix_vert(hslid, head_hbmp, head_matrix);
        /* fill mid */
        _fill_ft_mid_vert(hslid, head_hbmp, head_matrix);
    }
    else
    {
        /* fill tail */
        _fill_ft_head_matrix_hori(hslid, head_hbmp, head_matrix);
        /* fill mid */
        _fill_ft_mid_hori(hslid, head_hbmp, head_matrix);
    }

    if (src_matrix != NULL)
    {
        g_free(src_matrix);
        src_matrix = NULL;
    }

    if (head_matrix != NULL)
    {
        g_free(head_matrix);
        src_matrix = NULL;
    }

    if (head_hbmp != 0)
    {
        bmp_close(head_hbmp);
        head_hbmp = 0;
    }

    if (tail_matrix != NULL)
    {
        g_free(tail_matrix);
        tail_matrix = NULL;
    }

    if (tail_hbmp != 0)
    {
        bmp_close(tail_hbmp);
        tail_hbmp = 0;
    }

    return 0;
error:

    if (hslid->bk_matrix != NULL)
    {
        g_free(hslid->bk_matrix);
        hslid->bk_matrix = NULL;
    }

    if (hslid->ft_matrix != NULL)
    {
        g_free(hslid->ft_matrix);
        hslid->ft_matrix = NULL;
    }

    if (src_matrix != NULL)
    {
        g_free(src_matrix);
        src_matrix = NULL;
    }

    if (head_matrix != NULL)
    {
        g_free(head_matrix);
        src_matrix = NULL;
    }

    if (head_hbmp != 0)
    {
        bmp_close(head_hbmp);
        head_hbmp = 0;
    }

    if (tail_matrix != NULL)
    {
        g_free(tail_matrix);
        tail_matrix = NULL;
    }

    if (tail_hbmp != 0)
    {
        bmp_close(tail_hbmp);
        tail_hbmp = 0;
    }

    if (src_matrix != NULL)
    {
        g_free(src_matrix);
        src_matrix = NULL;
    }

    return -1;
}



static __s32  _fill_bk_tail_matrix_hori(HSLID hslid, HBMP tail_hbmp, void *tail_matrix)
{
    RECT   dst_rect;
    RECT   src_rect;
    SIZE   rect_size;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(tail_hbmp);
    height = bmp_get_height(tail_hbmp);
    byte_count = bmp_get_bitcount(tail_hbmp) >> 3;
    src_rect.x      = 0;
    src_rect.y      = 0;
    src_rect.width  = width;
    src_rect.height = height;
    dst_rect.x      = hslid->disp_rect.width - width;
    dst_rect.y      = (hslid->disp_rect.height - height) >> 1;
    dst_rect.width  = hslid->disp_rect.width;
    dst_rect.height = hslid->disp_rect.height;
    rect_size.width  = width;
    rect_size.height = height;
    rect_copy_alpha(&dst_rect, hslid->bk_matrix, &src_rect, tail_matrix, &rect_size, byte_count);
    return 0;
}



static __s32  _fill_bk_mid_hori(HSLID hslid, HBMP tail_hbmp, void *tail_matrix)
{
    void  *column_buf = NULL;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(tail_hbmp);
    height = bmp_get_height(tail_hbmp);
    byte_count = bmp_get_bitcount(tail_hbmp) >> 3;
    column_buf = (__u32 *)g_malloc(height * byte_count);

    if (column_buf == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    /* get column */
    rect_get_column(tail_matrix, width, height, 0, column_buf, byte_count);
    /* fill */
    {
        SIZE  size;
        RECT  rect;
        size.width  = hslid->disp_rect.width;
        size.height = hslid->disp_rect.height;
        rect.x      = 0;
        rect.y      = (hslid->disp_rect.height - height) >> 1;
        rect.width  = hslid->disp_rect.width - bmp_get_width(tail_hbmp);
        rect.height = height;
        fill_rect_with_column(hslid->bk_matrix, &size, &rect, column_buf, hslid->byte_count);
    }

    if (column_buf != NULL)
    {
        g_free(column_buf);
        column_buf = NULL;
    }

    return 0;
error:

    if (column_buf != NULL)
    {
        g_free(column_buf);
        column_buf = NULL;
    }

    return -1;
}



static __s32  _fill_ft_head_matrix_hori(HSLID hslid, HBMP head_hbmp, void *head_matrix)
{
    RECT   dst_rect;
    RECT   src_rect;
    SIZE   rect_size;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(head_hbmp);
    height = bmp_get_height(head_hbmp);
    byte_count = bmp_get_bitcount(head_hbmp) >> 3;
    src_rect.x      = 0;
    src_rect.y      = 0;
    src_rect.width  = width;
    src_rect.height = height;
    dst_rect.x      = 0;
    dst_rect.y      = (hslid->disp_rect.height - height) >> 1;
    dst_rect.width  = hslid->disp_rect.width;
    dst_rect.height = hslid->disp_rect.height;
    rect_size.width  = width;
    rect_size.height = height;
    rect_copy_alpha(&dst_rect, hslid->ft_matrix, &src_rect, head_matrix, &rect_size, byte_count);
    return 0;
}



static __s32  _fill_ft_mid_hori(HSLID hslid, HBMP head_hbmp, void *head_matrix)
{
    __u32 *column_buf = NULL;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(head_hbmp);
    height = bmp_get_height(head_hbmp);
    byte_count = bmp_get_bitcount(head_hbmp) >> 3;
    column_buf = (__u32 *)g_malloc(height * byte_count);

    if (column_buf == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    /* get column */
    rect_get_column(head_matrix, width, height, width - 1, column_buf, byte_count);
    /* fill */
    {
        SIZE  size;
        RECT  rect;
        size.width  = hslid->disp_rect.width;
        size.height = hslid->disp_rect.height;
        rect.x      = width;
        rect.y      = (hslid->disp_rect.height - height) >> 1;
        rect.width  = hslid->disp_rect.width - width;
        rect.height = height;
        fill_rect_with_column(hslid->ft_matrix, &size, &rect, column_buf, hslid->byte_count);
    }

    if (column_buf != NULL)
    {
        g_free(column_buf);
        column_buf = NULL;
    }

    return 0;
error:

    if (column_buf != NULL)
    {
        g_free(column_buf);
        column_buf = NULL;
    }

    return -1;
}



static __s32  _fill_bk_tail_matrix_vert(HSLID hslid, HBMP tail_hbmp, void *tail_matrix)
{
    RECT   dst_rect;
    RECT   src_rect;
    SIZE   rect_size;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(tail_hbmp);
    height = bmp_get_height(tail_hbmp);
    byte_count = bmp_get_bitcount(tail_hbmp) >> 3;
    src_rect.x      = 0;
    src_rect.y      = 0;
    src_rect.width  = width;
    src_rect.height = height;
    dst_rect.x      = (hslid->disp_rect.width - width) >> 1;
    dst_rect.y      = 0;
    dst_rect.width  = hslid->disp_rect.width;
    dst_rect.height = hslid->disp_rect.height;
    rect_size.width  = width;
    rect_size.height = height;
    rect_copy_alpha(&dst_rect, hslid->bk_matrix, &src_rect, tail_matrix, &rect_size, byte_count);
    return 0;
}



static __s32  _fill_bk_mid_vert(HSLID hslid, HBMP tail_hbmp, void *tail_matrix)
{
    void  *row_buf = NULL;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(tail_hbmp);
    height = bmp_get_height(tail_hbmp);
    byte_count = bmp_get_bitcount(tail_hbmp) >> 3;
    row_buf = (__u32 *)g_malloc(width * byte_count);

    if (row_buf == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    /* get row */
    rect_get_row(tail_matrix, width, height, height - 1, row_buf, byte_count);
    /* fill */
    {
        SIZE  size;
        RECT  rect;
        size.width  = hslid->disp_rect.width;
        size.height = hslid->disp_rect.height;
        rect.x      = (hslid->disp_rect.width - width) >> 1;
        rect.y      = height;
        rect.width  = width;
        rect.height = hslid->disp_rect.height - height;
        fill_rect_with_row(hslid->bk_matrix, &size, &rect, row_buf, hslid->byte_count);
    }

    if (row_buf != NULL)
    {
        g_free(row_buf);
        row_buf = NULL;
    }

    return 0;
error:

    if (row_buf != NULL)
    {
        g_free(row_buf);
        row_buf = NULL;
    }

    return -1;
}



static __s32  _fill_ft_head_matrix_vert(HSLID hslid, HBMP head_hbmp, void *head_matrix)
{
    RECT   dst_rect;
    RECT   src_rect;
    SIZE   rect_size;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(head_hbmp);
    height = bmp_get_height(head_hbmp);
    byte_count = bmp_get_bitcount(head_hbmp) >> 3;
    src_rect.x      = 0;
    src_rect.y      = 0;
    src_rect.width  = width;
    src_rect.height = height;
    dst_rect.x      = (hslid->disp_rect.width - width) >> 1;
    dst_rect.y      = hslid->disp_rect.height - height;
    dst_rect.width  = hslid->disp_rect.width;
    dst_rect.height = hslid->disp_rect.height;
    rect_size.width  = width;
    rect_size.height = height;
    rect_copy_alpha(&dst_rect, hslid->ft_matrix, &src_rect, head_matrix, &rect_size, byte_count);
    return 0;
}



static __s32  _fill_ft_mid_vert(HSLID hslid, HBMP head_hbmp, void *head_matrix)
{
    __u32 *row_buf = NULL;
    __u32  width;
    __u32  height;
    __u32  byte_count;
    width  = bmp_get_width(head_hbmp);
    height = bmp_get_height(head_hbmp);
    byte_count = bmp_get_bitcount(head_hbmp) >> 3;
    row_buf = (__u32 *)g_malloc(width * byte_count);

    if (row_buf == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    /* get row */
    rect_get_row(head_matrix, width, height, 0, row_buf, byte_count);
    /* fill */
    {
        SIZE  size;
        RECT  rect;
        size.width  = hslid->disp_rect.width;
        size.height = hslid->disp_rect.height;
        rect.x      = (hslid->disp_rect.width - width) >> 1;
        rect.y      = 0;
        rect.width  = width;
        rect.height = hslid->disp_rect.height - height;
        fill_rect_with_row(hslid->ft_matrix, &size, &rect, row_buf, hslid->byte_count);
    }

    if (row_buf != NULL)
    {
        g_free(row_buf);
        row_buf = NULL;
    }

    return 0;
error:

    if (row_buf != NULL)
    {
        g_free(row_buf);
        row_buf = NULL;
    }

    return -1;
}



static void   _draw_cursor(const void *bmp_buf, __s32 x, __s32 y)
{
    __s32  old_mode;
    old_mode = GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_BMP_Draw(bmp_buf, x, y);
    GUI_SetDrawMode(old_mode);
}



static __s32  _SliderCtrlProc(__gui_msg_t *msg)
{
    __u32     dwStyle;
    HSLID_t  *hslid = NULL;
    H_WIN     hwin;
    __bool    is_vertical;
    __s32     new_length;
    __s32     total_length = 0;
    __bool    respond_outdev = EPDK_FALSE;
    __s32     old_length = 0;
    hwin = msg->h_deswin;
    dwStyle = GUI_WinGetStyle(hwin);
    is_vertical = (dwStyle & SDS_VERTICAL) ? EPDK_TRUE : EPDK_FALSE;
    hslid = (HSLID)GUI_CtrlWinGetAddData(hwin);

    if (hslid != NULL)
    {
        total_length = is_vertical ? hslid->disp_rect.height - hslid->cursor_length :
                       hslid->disp_rect.width  - hslid->cursor_length;
        respond_outdev = (dwStyle & SDS_RESPOND_OUTDEV) ? EPDK_TRUE : EPDK_FALSE;
        old_length = hslid->old_length;
    }

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            HSLID_t  *hslid = NULL;
            hslid = (HSLID_t *) g_malloc(sizeof(HSLID_t));

            if (hslid == NULL)
            {
                return ORANGE_FAIL;
            }

            g_memset(hslid, 0, sizeof(HSLID_t));

            if (_init_hslid(hslid, hwin))
            {
                _release_res(hslid);
                g_free(hslid);
                hslid = NULL;
                return ORANGE_FAIL;
            }

            GUI_CtrlWinSetAddData(hwin, (unsigned long)hslid);
            //if( dwStyle & WS_VISIBLE )
            //{
            //  /* draw slider */
            //  hslid->is_showing = EPDK_TRUE;
            //  _my_GUI_InvalidateRect( hwin, NULL, ORANGE_TRUE );          // redraw
            //}
            return 0;
        }

        case GUI_MSG_DESTROY:
            if (hslid != NULL)
            {
                _release_res(hslid);
                g_free(hslid);
                hslid = NULL;
            }

            return 0;

        case GUI_MSG_ENABLE:
            return ORANGE_OK;

        case GUI_MSG_SET_UNFOCUS:
            break;

        case GUI_MSG_SET_FOCUS:
            break;

        case GUI_MSG_KEY:
        {
            if (!respond_outdev)
            {
                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                return 0;
            }

            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_UP:
                {
                    switch (msg->dwAddData2)
                    {
                        case KEY_UP_ACTION   :
                            if (hslid->last_keyaction == KEY_DOWN_ACTION)
                            {
                                if (GUI_WinGetCaptureWin() != hwin)
                                {
                                    break;
                                }

                                GUI_WinReleaseCapture();
                                hslid->last_keyaction = msg->dwAddData2;
                                _notify_parent(hwin, SDN_STOPDRAG, 0);
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        case KEY_DOWN_ACTION   :
                            if (hslid->last_keyaction == KEY_UP_ACTION)
                            {
                                if (is_vertical)
                                {
                                    if (GUI_WinGetCaptureWin() == hwin)
                                    {
                                        break;
                                    }

                                    GUI_WinSetCaptureWin(hwin);
                                    new_length = hslid->old_length + hslid->line_size;

                                    if (new_length > total_length)
                                    {
                                        new_length = total_length;
                                    }

                                    if (new_length != hslid->old_length)
                                    {
                                        hslid->length = new_length;
                                        hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                                        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                                    }

                                    hslid->last_keyaction = msg->dwAddData2;
                                    /* notify parent msg */
                                    _notify_parent(hwin, SDN_STARTDRAG, 0);

                                    if (new_length != old_length)
                                    {
                                        _notify_parent(hwin, SDN_POS_CHANGED, 0);
                                    }
                                }
                                else
                                {
                                    _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                                }
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        default :
                            _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            return 0;
                    }

                    return 0;
                }

                case GUI_MSG_KEY_DOWN:
                {
                    switch (msg->dwAddData2)
                    {
                        case KEY_UP_ACTION   :
                            if (hslid->last_keyaction == KEY_DOWN_ACTION)
                            {
                                if (GUI_WinGetCaptureWin() != hwin)
                                {
                                    break;
                                }

                                GUI_WinReleaseCapture();
                                hslid->last_keyaction = msg->dwAddData2;
                                _notify_parent(hwin, SDN_STOPDRAG, 0);
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        case KEY_DOWN_ACTION   :
                            if (hslid->last_keyaction == KEY_UP_ACTION)
                            {
                                if (is_vertical)
                                {
                                    if (GUI_WinGetCaptureWin() == hwin)
                                    {
                                        break;
                                    }

                                    GUI_WinSetCaptureWin(hwin);
                                    new_length = hslid->old_length - hslid->line_size;

                                    if (new_length < 0)
                                    {
                                        new_length = 0;
                                    }

                                    if (new_length != hslid->old_length)
                                    {
                                        hslid->length = new_length;
                                        hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                                        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                                    }

                                    hslid->last_keyaction = msg->dwAddData2;
                                    /* notify parent msg */
                                    _notify_parent(hwin, SDN_STARTDRAG, 0);

                                    if (new_length != old_length)
                                    {
                                        _notify_parent(hwin, SDN_POS_CHANGED, 0);
                                    }
                                }
                                else
                                {
                                    _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                                }
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        default :
                            _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            return 0;
                    }

                    return 0;
                }

                case GUI_MSG_KEY_RIGHT:
                {
                    switch (msg->dwAddData2)
                    {
                        case KEY_UP_ACTION   :
                            if (hslid->last_keyaction == KEY_DOWN_ACTION)
                            {
                                if (GUI_WinGetCaptureWin() != hwin)
                                {
                                    break;
                                }

                                GUI_WinReleaseCapture();
                                hslid->last_keyaction = msg->dwAddData2;
                                _notify_parent(hwin, SDN_STOPDRAG, 0);
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        case KEY_DOWN_ACTION   :
                            if (hslid->last_keyaction == KEY_UP_ACTION)
                            {
                                if (!is_vertical)
                                {
                                    if (GUI_WinGetCaptureWin() == hwin)
                                    {
                                        break;
                                    }

                                    GUI_WinSetCaptureWin(hwin);
                                    new_length = hslid->old_length + hslid->line_size;

                                    if (new_length > total_length)
                                    {
                                        new_length = total_length;
                                    }

                                    if (new_length != hslid->old_length)
                                    {
                                        hslid->length = new_length;
                                        hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                                        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                                    }

                                    hslid->last_keyaction = msg->dwAddData2;
                                    /* notify parent msg */
                                    _notify_parent(hwin, SDN_STARTDRAG, 0);

                                    if (new_length != old_length)
                                    {
                                        _notify_parent(hwin, SDN_POS_CHANGED, 0);
                                    }
                                }
                                else
                                {
                                    _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                                }
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        default :
                            _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            return 0;
                    }

                    return 0;
                }

                case GUI_MSG_KEY_LEFT:
                {
                    switch (msg->dwAddData2)
                    {
                        case KEY_UP_ACTION   :
                            if (hslid->last_keyaction == KEY_DOWN_ACTION)
                            {
                                if (GUI_WinGetCaptureWin() != hwin)
                                {
                                    break;
                                }

                                GUI_WinReleaseCapture();
                                hslid->last_keyaction = msg->dwAddData2;
                                _notify_parent(hwin, SDN_STOPDRAG, 0);
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        case KEY_DOWN_ACTION   :
                            if (hslid->last_keyaction == KEY_UP_ACTION)
                            {
                                if (!is_vertical)
                                {
                                    if (GUI_WinGetCaptureWin() == hwin)
                                    {
                                        break;
                                    }

                                    GUI_WinSetCaptureWin(hwin);
                                    new_length = hslid->old_length - hslid->line_size;

                                    if (new_length < 0)
                                    {
                                        new_length = 0;
                                    }

                                    if (new_length != hslid->old_length)
                                    {
                                        hslid->length = new_length;
                                        hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                                        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                                    }

                                    hslid->last_keyaction = msg->dwAddData2;
                                    /* notify parent msg */
                                    _notify_parent(hwin, SDN_STARTDRAG, 0);

                                    if (new_length != old_length)
                                    {
                                        _notify_parent(hwin, SDN_POS_CHANGED, 0);
                                    }
                                }
                                else
                                {
                                    _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                                }
                            }
                            else
                            {
                                _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            }

                            return 0;

                        default :
                            _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                            return 0;
                    }

                    return 0;
                }

                case GUI_MSG_KEY_LONGUP:
                {
                    if (is_vertical)
                    {
                        if (GUI_WinGetCaptureWin() != hwin)
                        {
                            break;
                        }

                        new_length = hslid->old_length + hslid->line_size;

                        if (new_length > total_length)
                        {
                            new_length = total_length;
                        }

                        if (new_length != hslid->old_length)
                        {
                            hslid->length = new_length;
                            hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                        }

                        //hslid->last_key = msg->dwAddData1;
                        /* notify parent msg */
                        if (new_length != old_length)
                        {
                            _notify_parent(hwin, SDN_POS_CHANGED, 0);
                        }
                    }
                    else
                    {
                        _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                    }

                    return 0;
                }

                case GUI_MSG_KEY_LONGDOWN:
                {
                    if (is_vertical)
                    {
                        if (GUI_WinGetCaptureWin() != hwin)
                        {
                            break;
                        }

                        new_length = hslid->old_length - hslid->line_size;

                        if (new_length < 0)
                        {
                            new_length = 0;
                        }

                        if (new_length != hslid->old_length)
                        {
                            hslid->length = new_length;
                            hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                        }

                        //hslid->last_key = msg->dwAddData1;
                        /* notify parent msg */
                        if (new_length != old_length)
                        {
                            _notify_parent(hwin, SDN_POS_CHANGED, 0);
                        }
                    }
                    else
                    {
                        _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                    }

                    return 0;
                }

                case GUI_MSG_KEY_LONGRIGHT:
                {
                    if (!is_vertical)
                    {
                        if (GUI_WinGetCaptureWin() != hwin)
                        {
                            break;
                        }

                        new_length = hslid->old_length + hslid->line_size;

                        if (new_length > total_length)
                        {
                            new_length = total_length;
                        }

                        if (new_length != hslid->old_length)
                        {
                            hslid->length = new_length;
                            hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                        }

                        //hslid->last_key = msg->dwAddData1;
                        /* notify parent msg */
                        if (new_length != old_length)
                        {
                            _notify_parent(hwin, SDN_POS_CHANGED, 0);
                        }
                    }
                    else
                    {
                        _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                    }

                    return 0;
                }

                case GUI_MSG_KEY_LONGLEFT:
                {
                    if (!is_vertical)
                    {
                        if (GUI_WinGetCaptureWin() != hwin)
                        {
                            break;
                        }

                        new_length = hslid->old_length - hslid->line_size;

                        if (new_length < 0)
                        {
                            new_length = 0;
                        }

                        if (new_length != hslid->old_length)
                        {
                            hslid->length = new_length;
                            hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                        }

                        //hslid->last_key = msg->dwAddData1;
                        /* notify parent msg */
                        if (new_length != old_length)
                        {
                            _notify_parent(hwin, SDN_POS_CHANGED, 0);
                        }
                    }
                    else
                    {
                        _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                    }

                    return 0;
                }

                default:
                    _notify_parent(hwin, SDN_KEY, msg->dwAddData1);
                    return 0;
            }
        }
            break;
        case GUI_MSG_TOUCH:
        {
            __s32  x;
            __s32  y;

            if (!respond_outdev)                    // not respond outside device input
            {
                break;
            }

            x = (__s32) GUI_PARSEX(msg->dwAddData2);
            y = (__s32) GUI_PARSEY(msg->dwAddData2);

            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_DOWN:
                {
                    if (GUI_WinGetCaptureWin() == hwin)
                    {
                        break;
                    }

                    if (dwStyle & SDS_RESPOND_WITHIN_WIN)   // respond the touch point out of the window rect.
                    {
                        if (!_in_rect(hslid, x, y))             // point NOT in valid rect
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (is_vertical)
                        {
                            if (y > hslid->disp_rect.height)
                            {
                                y = hslid->disp_rect.height;
                            }
                            else if (y < 0)
                            {
                                y = 0;
                            }
                        }
                        else
                        {
                            if (x > hslid->disp_rect.width)
                            {
                                x = hslid->disp_rect.width;
                            }
                            else if (x < 0)
                            {
                                x = 0;
                            }
                        }
                    }

                    GUI_WinSetCaptureWin(hwin);

                    if (_in_cursor(hslid, x, y, is_vertical))                    // point in cursor
                    {
                        new_length = hslid->old_length;
                    }
                    else
                    {
                        new_length = is_vertical ? hslid->disp_rect.height - y : x;

                        if (new_length > total_length)
                        {
                            new_length = total_length;
                        }
                    }

                    if (new_length != hslid->old_length)
                    {
                        hslid->length = new_length;
                        hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                    }

                    /* notify parent msg */
                    _notify_parent(hwin, SDN_STARTDRAG, 0);

                    if (new_length != old_length)
                    {
                        _notify_parent(hwin, SDN_POS_CHANGED, 0);
                    }

                    break;
                }

                case GUI_MSG_TOUCH_MOVE:
                {
                    if (GUI_WinGetCaptureWin() != hwin)
                    {
                        break;
                    }

                    if (is_vertical)
                    {
                        if (y < hslid->cursor_length)
                        {
                            new_length = total_length;
                        }
                        else if (y > total_length + hslid->cursor_length)
                        {
                            new_length = 0;
                        }
                        else
                        {
                            new_length = total_length + hslid->cursor_length - y;
                        }
                    }
                    else
                    {
                        if (x < 0)
                        {
                            new_length = 0;
                        }
                        else if (x > total_length)
                        {
                            new_length = total_length;
                        }
                        else
                        {
                            new_length = x;
                        }
                    }

                    if (new_length != hslid->old_length)                        // pos changed
                    {
                        hslid->length = new_length;
                        hslid->value  = _calculate_value(hslid->min, hslid->max, hslid->length, total_length);
                        _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);            // redraw
                        /* notify parent msg */
                        _notify_parent(hwin, SDN_POS_CHANGED, 0);
                        return 0;
                    }

                    break;
                }

                case GUI_MSG_TOUCH_UP:
                {
                    if (GUI_WinGetCaptureWin() != hwin)
                    {
                        break;
                    }

                    GUI_WinReleaseCapture();
                    /* notify parent msg */
                    _notify_parent(hwin, SDN_STOPDRAG, 0);
                    return 0;
                }

                default :
                    break;
            }

            return 0;
        }

        case GUI_MSG_PAINT:
            _ondraw(hslid, hwin);
            return ORANGE_OK;

        case GUI_MSG_WIN_SLEEP:
            if (!slider_is_waking_i(hslid))                 // has slept
            {
                return ORANGE_OK;
            }

            hslid->is_waking = EPDK_FALSE;
            _my_GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);

            /* release memory device */
            if (hslid->hmem != 0)
            {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
                GUI_MEMDEV_Delete(hslid->hmem);
#endif
                hslid->hmem = 0;
            }

            return ORANGE_OK;

        case GUI_MSG_WIN_WAKEUP:
            if (slider_is_waking_i(hslid))                  // has waken up
            {
                return ORANGE_OK;
            }

            hslid->is_waking = EPDK_TRUE;
            GUI_InvalidateRect(hwin, NULL, ORANGE_TRUE);
            return ORANGE_OK;

        default:
            break;
    }

    return GUI_CtrlWinDefaultProc(msg);
}



static __s32  _init_hslid(HSLID hslid, H_WIN hwin)
{
    __u32       cursor_width;
    __u32       cursor_height;
    const void *head_bmp_buf;
    const void *tail_bmp_buf;
    const void *cursor_bmp_buf;
    HBMP        cursor_hbmp = 0;
    slider_para_t *para_p;
    __u32       dwStyle;
    __bool      is_vertical;
    RECT        rect;
    para_p = (slider_para_t *)GUI_WinGetAttr(hwin);
    dwStyle = GUI_WinGetStyle(hwin);

    /* check the input arguments */
    if (para_p == NULL
        || para_p->head_bmp_buf   == NULL
        || para_p->tail_bmp_buf   == NULL
        || para_p->cursor_bmp_buf == NULL)
    {
        return -1;
    }

    if (dwStyle & SDS_VERTICAL)
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
    cursor_bmp_buf = para_p->cursor_bmp_buf;
    cursor_hbmp = bmp_open(cursor_bmp_buf);

    if (cursor_hbmp == 0)
    {
        goto error;
    }

    cursor_width  = bmp_get_width(cursor_hbmp);
    cursor_height = bmp_get_height(cursor_hbmp);

    if (is_vertical)
    {
        if (cursor_width > rect.width)
        {
            __wrn("slider input arg is wrong.\n");
            goto error;
        }
    }
    else
    {
        if (cursor_height > rect.height)
        {
            __wrn("slider input arg is wrong.\n");
            goto error;
        }
    }

    /* init basic info */
    hslid->h_lyr        = (H_LYR)GUI_WinGetLyrWin(hwin);
    hslid->byte_count   = bmp_get_bitcount(cursor_hbmp) >> 3;
    hslid->disp_rect.x  = rect.x     ;
    hslid->disp_rect.y  = rect.y     ;

    if (is_vertical)
    {
        hslid->disp_rect.width  = cursor_width;
        hslid->disp_rect.height = rect.height;
        hslid->cursor_length    = cursor_height;
    }
    else
    {
        hslid->disp_rect.width  = rect.width ;
        hslid->disp_rect.height = cursor_height;
        hslid->cursor_length    = cursor_width;
    }

    /* init cursor info */
    hslid->cursor_bmp_buf = g_malloc(bmp_get_size(cursor_hbmp));

    if (hslid->cursor_bmp_buf == NULL)
    {
        goto error;
    }

    g_memcpy(hslid->cursor_bmp_buf, cursor_bmp_buf, bmp_get_size(cursor_hbmp));

    /* init background and frontground info */
    if (_init_bK_ft(hslid, head_bmp_buf, tail_bmp_buf, is_vertical))
    {
        goto error;
    }

    /* create the memory device */
    if (dwStyle & WS_MEMDEV)                                // use memory device
    {
        GUI_LyrWinSel(hslid->h_lyr);
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        hslid->hmem = GUI_MEMDEV_Create(hslid->disp_rect.x,
                                        hslid->disp_rect.y,
                                        hslid->disp_rect.x + hslid->disp_rect.width  - 1,
                                        hslid->disp_rect.y + hslid->disp_rect.height - 1);
#endif

        if (hslid->hmem == 0)
        {
            __wrn("Fail in creating memory device.\n");
            goto error;
        }
    }

    /* reset status */
    _slider_reset_i(hslid);
    __msg("is_showing is %d and old_is_showing is %d.\n", hslid->is_showing, hslid->old_is_showing);

    if (cursor_hbmp != 0)
    {
        bmp_close(cursor_hbmp);
        cursor_hbmp = 0;
    }

    return 0;
error:

    if (cursor_hbmp != 0)
    {
        bmp_close(cursor_hbmp);
        cursor_hbmp = 0;
    }

    _release_res(hslid);
    hslid = NULL;
    return -1;
}



static __s32  _ondraw(HSLID hslid, H_WIN hwin)
{
    __u32  dwStyle;
    dwStyle = GUI_WinGetStyle(hwin);

    if (hslid->is_internal_cmd)                                 // internal cmd to draw
    {
        if (!hslid->is_waking && !hslid->old_is_waking)         // sleeping
        {
            _save_locale(hslid);
            hslid->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (!hslid->is_waking && hslid->old_is_waking)          // to sleep
        {
            _save_locale(hslid);
            hslid->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (hslid->is_waking && !hslid->old_is_waking)          // to wake up
        {
            _draw_slider(hslid, dwStyle);
            hslid->is_showing = EPDK_TRUE;
            _save_locale(hslid);
            hslid->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (hslid->is_showing && !hslid->old_is_showing)        // to show
        {
            _draw_slider(hslid, dwStyle);
            _save_locale(hslid);
            hslid->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (hslid->length != hslid->old_length)                // pos changed
        {
            _redraw(hslid, hslid->length, dwStyle);
            _save_locale(hslid);
            hslid->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        hslid->is_internal_cmd = EPDK_FALSE;
    }
    else
    {
        _reset_slid(hslid, hwin);
        _draw_slider(hslid, dwStyle);
        hslid->is_showing = EPDK_TRUE;
        hslid->is_waking  = EPDK_TRUE;
        _save_locale(hslid);
    }

    return 0;
}



static void   _save_locale(HSLID hslid)
{
    hslid->old_length     = hslid->length    ;
    hslid->old_is_waking  = hslid->is_waking ;
    hslid->old_is_showing = hslid->is_showing;
}



static __bool _in_rect(HSLID hslid, __s32 x, __s32 y)
{
    if (x < 0 || x > hslid->disp_rect.width)
    {
        return EPDK_FALSE;
    }

    if (y < 0 || y > hslid->disp_rect.height)
    {
        return EPDK_FALSE;
    }

    return EPDK_TRUE;
}



static __bool _in_cursor(HSLID hslid, __s32 x, __s32 y, __bool is_vertical)
{
    if (is_vertical)
    {
        if (y < hslid->disp_rect.height - (hslid->old_length + hslid->cursor_length)
            || y > hslid->disp_rect.height - hslid->old_length)
        {
            return EPDK_FALSE;
        }

        if (x < 0 || x > hslid->disp_rect.width)
        {
            return EPDK_FALSE;
        }

        return EPDK_TRUE;
    }
    else
    {
        if (x < hslid->old_length || x > hslid->old_length + hslid->cursor_length)
        {
            return EPDK_FALSE;
        }

        if (y < 0 || y > hslid->disp_rect.height)
        {
            return EPDK_FALSE;
        }

        return EPDK_TRUE;
    }
}



static void   _notify_parent(H_WIN hwin, __u32 notify_code, __u32 dwAddData)
{
    __gui_notify_msg_t   notify_msg;
    notify_msg.hWnd     = hwin;
    notify_msg.id       = GUI_WinGetItemId(hwin);
    notify_msg.msgcode  = notify_code;
    notify_msg.dwAddData = dwAddData;
    GUI_NotifyParent(&notify_msg);
}


static __bool _my_GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd)
{
    HSLID  hslid = (HSLID) GUI_CtrlWinGetAddData(hWnd);
    hslid->is_internal_cmd = EPDK_TRUE;
    return  GUI_InvalidateRect(hWnd, prc, bEraseBkgnd);
}



static void *_get_layer_bk_matrix(HSLID hslid)
{
    void  *src_matrix = NULL;
    __u32  buf_size;
    GUI_RECT  src_pos;
    buf_size = hslid->disp_rect.width * hslid->disp_rect.height * hslid->byte_count;
    src_matrix = g_malloc(buf_size);

    if (src_matrix == NULL)
    {
        __err("Error in allocating memory.\n");
        goto error;
    }

    src_pos.x0 = hslid->disp_rect.x;
    src_pos.y0 = hslid->disp_rect.y;
    src_pos.x1 = src_pos.x0 + hslid->disp_rect.width - 1;
    src_pos.y1 = src_pos.y0 + hslid->disp_rect.height - 1;
    GUI_LyrWinSel(hslid->h_lyr);
    GUI_GetRectMem(&src_pos, src_matrix, hslid->disp_rect.width, hslid->disp_rect.height, 0, 0);
    return src_matrix;
error:

    if (src_matrix != NULL)
    {
        g_free(src_matrix);
        src_matrix = NULL;
    }

    return NULL;
}



static __s32  _reset_matrix(HSLID hslid, H_WIN hwin, void *matrix)
{
    void  *src_matrix = NULL;
    /* get layer background matrix */
    src_matrix = _get_layer_bk_matrix(hslid);

    if (src_matrix == NULL)
    {
        goto error;
    }

    /* copy bar from old matrix to new layer bk matrix */
    {
        RECT   dst_rect;
        RECT   src_rect;
        SIZE   rect_size;

        if (GUI_WinGetStyle(hwin)  & SDS_VERTICAL)
        {
            src_rect.x      = (hslid->disp_rect.width - hslid->bar_length) >> 1;
            src_rect.y      = 0;
            src_rect.width  = hslid->disp_rect.width;
            src_rect.height = hslid->disp_rect.height;
            dst_rect.x      = src_rect.x     ;
            dst_rect.y      = src_rect.y     ;
            dst_rect.width  = src_rect.width ;
            dst_rect.height = src_rect.height;
            rect_size.width  = hslid->bar_length;
            rect_size.height = hslid->disp_rect.height;
        }
        else
        {
            src_rect.x      = 0;
            src_rect.y      = (hslid->disp_rect.height - hslid->bar_length) >> 1;
            src_rect.width  = hslid->disp_rect.width;
            src_rect.height = hslid->disp_rect.height;
            dst_rect.x      = src_rect.x     ;
            dst_rect.y      = src_rect.y     ;
            dst_rect.width  = src_rect.width ;
            dst_rect.height = src_rect.height;
            rect_size.width  = hslid->disp_rect.width;
            rect_size.height = hslid->bar_length;
        }

        rect_copy_alpha(&dst_rect, src_matrix, &src_rect, matrix, &rect_size, hslid->byte_count);
    }
    /* overlap old matrix */
    g_memcpy(matrix, src_matrix, hslid->disp_rect.height * hslid->disp_rect.width * hslid->byte_count);
    g_free(src_matrix);
    src_matrix = NULL;
    return 0;
error:

    if (src_matrix != NULL)
    {
        g_free(src_matrix);
        src_matrix = NULL;
    }

    return -1;
}



static __s32  _reset_slid(HSLID hslid, H_WIN hwin)
{
    RECT            rect;
    __u32           dwStyle;
    dwStyle = GUI_WinGetStyle(hwin);
    hslid->h_lyr = GUI_LyrP2H(GUI_WinGetLyrWin(hwin));
    /* reset display rect */
    GUI_WinGetClientFBRect(hwin, &rect);
    hslid->disp_rect.x  = rect.x;
    hslid->disp_rect.y  = rect.y;

    /* reset memory device */
    if (dwStyle & WS_MEMDEV)                                // use memory device
    {
        /* destroy old memory device */
        if (hslid->hmem != 0)
        {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
            GUI_MEMDEV_Delete(hslid->hmem);
#endif
            hslid->hmem = 0;
        }

        /* create new memory device */
        GUI_LyrWinSel(hslid->h_lyr);
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        hslid->hmem = GUI_MEMDEV_Create(hslid->disp_rect.x,
                                        hslid->disp_rect.y,
                                        hslid->disp_rect.x + hslid->disp_rect.width  - 1,
                                        hslid->disp_rect.y + hslid->disp_rect.height - 1);
#endif

        if (hslid->hmem == 0)
        {
            __wrn("Fail in creating memory device.\n");
            return -1;
        }
    }

    /* reset bk matrix */
    _reset_matrix(hslid, hwin, hslid->bk_matrix);
    /* reset ft matrix */
    _reset_matrix(hslid, hwin, hslid->ft_matrix);
    return 0;
}



#endif     //  ifndef __slider_c

/* end of slider.c */
