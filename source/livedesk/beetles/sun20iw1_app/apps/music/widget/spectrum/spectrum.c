/*
************************************************************************************************************************
*                                                         MUSIC
*                                                   the Audio Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : spectrum.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.02
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
* Gary.Wang      2009.11.02       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __spectrum_c
#define  __spectrum_c

#include "beetles_app.h"
#include "spectrum_i.h"
#include <log.h>

/*
 *#define A_POS                           0XFF
 *#define R_POS                           0X36
 *#define G_POS                           0X2E
 *#define B_POS                           0X2B
 *
 *#define DE_A_POS                        24
 *#define DE_R_POS                        16
 *#define DE_G_POS                         8
 *#define DE_B_POS                         0
 *
 *#define DE_GET_A( x )                   ( ( ( ( (__u32 )( x & ( 0xFFU << DE_A_POS ) ) ) >> DE_A_POS ) & 0xFF ) )
 *#define DE_GET_R( x )                   ( ( ( ( (__u32 )( x & ( 0xFFU << DE_R_POS ) ) ) >> DE_R_POS ) & 0xFF ) )
 *#define DE_GET_G( x )                   ( ( ( ( (__u32 )( x & ( 0xFFU << DE_G_POS ) ) ) >> DE_G_POS ) & 0xFF ) )
 *#define DE_GET_B( x )                   ( ( ( ( (__u32 )( x & ( 0xFFU << DE_B_POS ) ) ) >> DE_B_POS ) & 0xFF ) )
 *
 *#define DE2GUI( DE_color )              (   ( DE_GET_A( DE_color ) << A_POS ) \
 *                                          | ( DE_GET_R( DE_color ) << R_POS ) \
 *                                          | ( DE_GET_G( DE_color ) << G_POS ) \
 *                                          | ( DE_GET_B( DE_color ) << B_POS ) )
 */


/******************************************************************************/
/*                            configure information                           */
/******************************************************************************/
#define BUOY_Y_UPPER_LIMIT              0
#define BUOY_INVALID_Y                  10000
#define MIN_YSPACE_BTW_BUOY_PARTICLE    4


#define Max( x, y )           ( ( (x) > (y) ? (x) : (y) ) )
#define Min( x, y )           ( ( (x) < (y) ? (x) : (y) ) )


typedef struct _HTRUM_t
{
    H_LYR   h_lyr;
    RECT    disp_rect;
    __s32   min;
    __s32   max;
    __u32   bk_color;
    __s32   particle_nr;
    __s32   particle_width;
    __s32   particle_height;
    __u32  *particle_matrix;
    __s32  *particle_x_array;
    __s32   buoy_width;
    __s32   buoy_height;
    __u32  *buoy_matrix;
    __s32  *buoy_x_array;
    __s32  *value_array;
    __s32  *length_array;
    __s32  *buoy_y_array;
    __s32  *fall_times;
    __bool  is_internal_cmd;
    __bool  is_waking;
    __bool  is_showing;
    __s32  *old_length_array;
    __bool  old_is_waking;
    __bool  old_is_showing;
} HTRUM_t, *HTRUM;


#define spectrum_is_waking_i( htrum )  ( htrum->is_waking )

const static __u32  fall_curve[] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };


static __bool _is_valid_handle(HTRUM htrum);
static __s32  _ondraw(HTRUM htrum, H_WIN hwin);
static __s32  _spectrumCtrlProc(__gui_msg_t *msg);
static void   _save_locale(HTRUM htrum);
static void   _notify_parent(H_WIN hwin, __u32 notify_code, __u32 dwAddData);
static __bool _my_GUI_InvalidateRect(H_WIN hWnd, const RECT *prc, __bool bEraseBkgnd);
static __s32  _reset_environment(HTRUM htrum, H_WIN hwin);
static void   _reset_status(HTRUM htrum);
static void   _release_res(HTRUM htrum);
static __s32  _init_htrum(HTRUM htrum, H_WIN hwin);
static __s32  _init_bk(HTRUM htrum);
static __s32  _init_particle(HTRUM htrum, const void *particle_bmp_buf);
static __s32  _init_buoy(HTRUM htrum, const void *buoy_bmp_buf);
static __s32  _init_coordinate(HTRUM htrum);
static void   _draw_buoy(HTRUM htrum, __s32 i);
static void   _clear_buoy(HTRUM htrum, __s32 i);
static void   _redraw_particle(HTRUM htrum, __s32 i);
static void   _redraw_buoy(HTRUM htrum, __s32 i);
static __u32  _calculate_length(__s32 min, __s32 max, __s32 value, __u32 total_length);
static void   _draw_particle(HTRUM htrum, __s32 i);
static __s32  _requeset_other_res(HTRUM htrum);
static void   _draw_spectrum(HTRUM htrum);
static void   _redraw_spectrum(HTRUM htrum);

__bool RegisterSpectrumControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_SPECTRUM;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = _spectrumCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == EPDK_OK;
}

__bool UnRegisterSpectrumControl(void)
{
    return (GUI_DeleteControlClass(CTRL_SPECTRUM));
}

__s32  SPECTRUM_SetValue(H_WIN hwin, __s32 *value_array)
{
    __u32  i;
    HTRUM  htrum = (HTRUM) GUI_CtrlWinGetAddData(hwin);

    if (!_is_valid_handle(htrum))
    {
        return -1;
    }

    if (!spectrum_is_waking_i(htrum))
    {
        return -1;
    }

    if (value_array == NULL)
    {
        for (i = 0; i < htrum->particle_nr; i++)
        {
            htrum->length_array[i] = htrum->old_length_array[i];    // old value is as new value
        }
    }
    else
    {
        for (i = 0; i < htrum->particle_nr; i++)
            htrum->length_array[i] = _calculate_length(htrum->min, htrum->max,
                                     value_array[i], htrum->particle_height);
    }

    _my_GUI_InvalidateRect(hwin, NULL, EPDK_TRUE);        // redraw
    return 0;
}



static __s32  _spectrumCtrlProc(__gui_msg_t *msg)
{
    __u32     dwStyle;
    HTRUM_t  *htrum = NULL;
    H_WIN     hwin;
    hwin = msg->h_deswin;
    dwStyle = GUI_WinGetStyle(hwin);
    htrum = (HTRUM)GUI_CtrlWinGetAddData(hwin);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            HTRUM_t  *htrum = NULL;
            htrum = (HTRUM_t *) g_malloc(sizeof(HTRUM_t));

            if (htrum == NULL)
            {
                return EPDK_FAIL;
            }

            g_memset(htrum, 0, sizeof(HTRUM_t));

            if (_init_htrum(htrum, hwin))
            {
                _release_res(htrum);
                g_free(htrum);
                htrum = NULL;
                return EPDK_FAIL;
            }

            GUI_CtrlWinSetAddData(hwin, (unsigned long)htrum);

            if (dwStyle & WS_VISIBLE)
            {
                /* draw spectrum */
                htrum->is_showing = EPDK_TRUE;
                _my_GUI_InvalidateRect(hwin, NULL, EPDK_TRUE);
            }

            return 0;
        }

        case GUI_MSG_DESTROY:
            if (htrum != NULL)
            {
                _release_res(htrum);
                g_free(htrum);
                htrum = NULL;
            }

            return 0;

        case GUI_MSG_ENABLE:
            return EPDK_OK;

        case GUI_MSG_SET_UNFOCUS:
            break;

        case GUI_MSG_SET_FOCUS:
            break;

        case GUI_MSG_KEY:
        {
            _notify_parent(hwin, STN_KEY, msg->dwAddData1);
            return 0;
        }

        case GUI_MSG_TOUCH:
        {
            break;
        }

        case GUI_MSG_PAINT:
            _ondraw(htrum, hwin);
            return EPDK_OK;

        case GUI_MSG_WIN_SLEEP:
            if (!spectrum_is_waking_i(htrum))                 // has slept
            {
                return EPDK_OK;
            }

            htrum->is_waking = EPDK_FALSE;
            _my_GUI_InvalidateRect(hwin, NULL, EPDK_TRUE);
            return EPDK_OK;

        case GUI_MSG_WIN_WAKEUP:
            if (spectrum_is_waking_i(htrum))                  // has waken up
            {
                return EPDK_OK;
            }

            htrum->is_waking = EPDK_TRUE;
            GUI_InvalidateRect(hwin, NULL, EPDK_TRUE);
            return EPDK_OK;

        default:
            break;
    }

    return GUI_CtrlWinDefaultProc(msg);
}



static void   _release_res(HTRUM htrum)
{
    if (htrum != NULL)
    {
        if (htrum->buoy_x_array != NULL)
        {
            g_free(htrum->buoy_x_array);
            htrum->buoy_x_array = NULL;
        }

        if (htrum->particle_x_array != NULL)
        {
            g_free(htrum->particle_x_array);
            htrum->particle_x_array = NULL;
        }

        if (htrum->fall_times != NULL)
        {
            g_free(htrum->fall_times);
            htrum->fall_times = NULL;
        }

        if (htrum->old_length_array != NULL)
        {
            g_free(htrum->old_length_array);
            htrum->old_length_array = NULL;
        }

        if (htrum->length_array != NULL)
        {
            g_free(htrum->length_array);
            htrum->length_array = NULL;
        }

        if (htrum->value_array != NULL)
        {
            g_free(htrum->value_array);
            htrum->value_array = NULL;
        }

        if (htrum->buoy_y_array != NULL)
        {
            g_free(htrum->buoy_y_array);
            htrum->buoy_y_array = NULL;
        }

        if (htrum->particle_matrix != NULL)
        {
            g_free(htrum->particle_matrix);
            htrum->particle_matrix = NULL;
        }

        if (htrum->buoy_matrix != NULL)
        {
            g_free(htrum->buoy_matrix);
            htrum->buoy_matrix = NULL;
        }
    }
}



static __bool _is_valid_handle(HTRUM htrum)
{
    if (htrum != NULL)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}



static __s32  _init_htrum(HTRUM htrum, H_WIN hwin)
{
    spectrum_para_t *para_p;
    RECT        rect;
    para_p = (spectrum_para_t *)GUI_WinGetAttr(hwin);

    /* check arg */
    if (para_p == NULL
        || para_p->particle_bmp_buf == NULL
        || para_p->buoy_bmp_buf == NULL
        || para_p->min >= para_p->max)
    {
        __wrn("spectrum input args are wrong.");
        goto error;
    }

    GUI_WinGetClientFBRect(hwin, &rect);
    /* init basic info */
    htrum->h_lyr            = (H_LYR)GUI_WinGetLyrWin(hwin);
    htrum->disp_rect.x      = rect.x     ;
    htrum->disp_rect.y      = rect.y     ;
    htrum->disp_rect.width  = rect.width ;
    htrum->disp_rect.height = rect.height;
    htrum->min              = para_p->min;
    htrum->max              = para_p->max;
    htrum->particle_nr      = para_p->particle_nr;

    /* init background info */
    if (_init_bk(htrum))
    {
        goto error;
    }

    /* init particle info */
    if (_init_particle(htrum, para_p->particle_bmp_buf))
    {
        goto error;
    }

    /* init buoy info */
    if (_init_buoy(htrum, para_p->buoy_bmp_buf))
    {
        goto error;
    }

    /* init coordinate */
    if (_init_coordinate(htrum))
    {
        goto error;
    }

    if (_requeset_other_res(htrum))
    {
        goto error;
    }

    _reset_status(htrum);
    return 0;
error:
    _release_res(htrum);
    return -1;
}



static __s32  _ondraw(HTRUM htrum, H_WIN hwin)
{
    if (htrum->is_internal_cmd)                                 // internal cmd to draw
    {
        if (!htrum->is_waking && !htrum->old_is_waking)         // sleeping
        {
            _save_locale(htrum);
            htrum->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (!htrum->is_waking && htrum->old_is_waking)          // to sleep
        {
            _save_locale(htrum);
            htrum->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (htrum->is_waking && !htrum->old_is_waking)          // to wake up
        {
            __wrn("NOT support wake up in intertal cmd.");
            _save_locale(htrum);
            htrum->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        if (htrum->is_showing && !htrum->old_is_showing)        // to show
        {
            _draw_spectrum(htrum);
            //GUI_LyrWinFlushFB( htrum->h_lyr );            //removed by libaiao,2011.5.31
            _save_locale(htrum);
            htrum->is_internal_cmd = EPDK_FALSE;
            return 0;
        }

        _redraw_spectrum(htrum);
        //GUI_LyrWinFlushFB( htrum->h_lyr );                //removed by libaiao,2011.5.31
        _save_locale(htrum);
        htrum->is_internal_cmd = EPDK_FALSE;
        return 0;
    }
    else
    {
        _reset_environment(htrum, hwin);
        _draw_spectrum(htrum);
        //GUI_LyrWinFlushFB( htrum->h_lyr );                //removed by libaiao,2011.5.31
        htrum->is_showing = EPDK_TRUE;
        htrum->is_waking  = EPDK_TRUE;
        _save_locale(htrum);
    }

    return 0;
}



static void   _save_locale(HTRUM htrum)
{
    g_memcpy(htrum->old_length_array, htrum->length_array, sizeof(__s32) * htrum->particle_nr);
    htrum->old_is_waking  = htrum->is_waking ;
    htrum->old_is_showing = htrum->is_showing;
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
    HTRUM  htrum = (HTRUM) GUI_CtrlWinGetAddData(hWnd);
    htrum->is_internal_cmd = EPDK_TRUE;
    return  GUI_InvalidateRect(hWnd, prc, bEraseBkgnd);
}



static void   _reset_status(HTRUM htrum)
{
    __u32  i;

    /* init buoy and value information */
    for (i = 0;  i < htrum->particle_nr;  i++)
    {
        htrum->buoy_y_array[i] = BUOY_INVALID_Y;
        htrum->value_array[i]  = htrum->min;
        htrum->length_array[i]     = 0;
        htrum->old_length_array[i] = 0;
        htrum->fall_times[i]  = 0;
    }

    htrum->is_waking      = EPDK_TRUE;
    htrum->is_showing     = EPDK_FALSE;
    htrum->old_is_waking  = EPDK_TRUE;
    htrum->old_is_showing = EPDK_FALSE;
}

static __s32  _reset_environment(HTRUM htrum, H_WIN hwin)
{
    RECT  rect;
    htrum->h_lyr = GUI_WinGetLyrWin(hwin);
    /* reset display rect */
    GUI_WinGetClientFBRect(hwin, &rect);
    htrum->disp_rect.x      = rect.x     ;
    htrum->disp_rect.y      = rect.y     ;
    htrum->disp_rect.width  = rect.width ;
    htrum->disp_rect.height = rect.height;
    /* reset bk */
    _init_bk(htrum);
    return 0;
}



static __u32  _calculate_length(__s32 min, __s32 max, __s32 value, __u32 total_length)
{
    __s32  length;
    __s32  cur;
    __s32  total;

    if (value < min)
    {
        return 0;
    }
    else if (value > max)
    {
        return total_length;
    }

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



static __s32  _init_bk(HTRUM htrum)
{
    GUI_RECT  src_pos;
    __u32     de_color = 0;             //initialed by libaiao
    //__u32     gui_color = 0;
    GUI_LyrWinSel(htrum->h_lyr);
    /* get bk color */
    src_pos.x0 = htrum->disp_rect.x;
    src_pos.y0 = htrum->disp_rect.y;
    src_pos.x1 = src_pos.x0 + 1 - 1;
    src_pos.y1 = src_pos.y0 + 1 - 1;
    GUI_GetRectMem(&src_pos, &de_color, 1, 1, 0, 0);
    //gui_color = DE2GUI( de_color );
    htrum->bk_color = 0xFF2B2E36;
    return 0;
}

static __s32  _init_particle(HTRUM htrum, const void *particle_bmp_buf)
{
    HBMP   hbmp = NULL;
    __u32  width;
    __u32  height;
    hbmp = bmp_open(particle_bmp_buf);

    if (hbmp == NULL)
    {
        __wrn("Fail in opening bmp buf.");
        goto error;
    }

    if (bmp_get_bitcount(hbmp) != 32)
    {
        __wrn("spectrum only support 32bit bmp.");
        goto error;
    }

    width  = bmp_get_width(hbmp);
    height = bmp_get_height(hbmp);
    htrum->particle_width  = width ;
    htrum->particle_height = height;

    /* check input arg */
    if (height > htrum->disp_rect.height || width > htrum->disp_rect.width)
    {
        __wrn("spectrum input arg is wrong.");
        goto error;
    }

    /* get particle matrix */
    htrum->particle_matrix = (__u32 *) g_malloc(width * height * (bmp_get_bitcount(hbmp) >> 3));

    if (htrum->particle_matrix == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    bmp_get_matrix(particle_bmp_buf, htrum->particle_matrix);

    if (hbmp != NULL)
    {
        bmp_close(hbmp);
        hbmp = NULL;
    }

    return 0;
error:

    if (htrum->particle_matrix != NULL)
    {
        g_free(htrum->particle_matrix);
        htrum->particle_matrix = NULL;
    }

    if (hbmp != NULL)
    {
        bmp_close(hbmp);
        hbmp = NULL;
    }

    return -1;
}



static __s32  _init_buoy(HTRUM htrum, const void *buoy_bmp_buf)
{
    HBMP   hbmp = NULL;
    __u32  width;
    __u32  height;
    hbmp = bmp_open(buoy_bmp_buf);

    if (hbmp == NULL)
    {
        __wrn("Fail in opening bmp buf.");
        goto error;
    }

    if (bmp_get_bitcount(hbmp) != 32)
    {
        __wrn("spectrum only support 32bit bmp.");
        goto error;
    }

    width  = bmp_get_width(hbmp);
    height = bmp_get_height(hbmp);
    htrum->buoy_width  = width ;
    htrum->buoy_height = height;
    /* get buoy matrix */
    htrum->buoy_matrix = (__u32 *) g_malloc(width * height * (bmp_get_bitcount(hbmp) >> 3));

    if (htrum->buoy_matrix == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    bmp_get_matrix(buoy_bmp_buf, htrum->buoy_matrix);

    if (hbmp != NULL)
    {
        bmp_close(hbmp);
        hbmp = NULL;
    }

    return 0;
error:

    if (htrum->buoy_matrix != NULL)
    {
        g_free(htrum->buoy_matrix);
        htrum->buoy_matrix = NULL;
    }

    if (hbmp != NULL)
    {
        bmp_close(hbmp);
        hbmp = NULL;
    }

    return -1;
}



static __s32  _init_coordinate(HTRUM htrum)
{
    __s32  particle_xspace;
    __s32  x_offset;
    __s32  particle_nr;
    __s32  max_width;
    __s32  xspace_buoy_offset_particle;
    __s32  particle_origin_x;
    __s32  i;
    particle_nr = htrum->particle_nr;
    max_width = Max(htrum->particle_width, htrum->buoy_width);

    /* get coordinate info */
    if (particle_nr != 1)
    {
        particle_xspace = (htrum->disp_rect.width - particle_nr * max_width) / (particle_nr - 1);
    }
    else
    {
        __msg("para err...particle_nr == 1");
        goto error;
    }

    x_offset = (htrum->disp_rect.width - particle_nr * max_width - (particle_nr - 1) * particle_xspace) >> 1;
    htrum->particle_x_array = (__s32 *)g_malloc(particle_nr * sizeof(__s32));

    if (htrum->particle_x_array == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    htrum->buoy_x_array = (__s32 *)g_malloc(particle_nr * sizeof(__s32));

    if (htrum->buoy_x_array == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    /* init x array */
    xspace_buoy_offset_particle = (htrum->buoy_width - htrum->particle_width) >> 1;

    if (htrum->particle_width >= htrum->buoy_width)
    {
        particle_origin_x = x_offset;
    }
    else
    {
        particle_origin_x = x_offset - xspace_buoy_offset_particle ;
    }

    for (i = 0;  i < particle_nr;  i++)
    {
        htrum->particle_x_array[i] = particle_origin_x + i * (max_width + particle_xspace);
        htrum->buoy_x_array[i] = htrum->particle_x_array[i] + xspace_buoy_offset_particle;
    }

    return 0;
error:

    if (htrum->buoy_x_array != NULL)
    {
        g_free(htrum->buoy_x_array);
        htrum->buoy_x_array = NULL;
    }

    if (htrum->particle_x_array != NULL)
    {
        g_free(htrum->particle_x_array);
        htrum->particle_x_array = NULL;
    }

    return -1;
}



static __s32  _requeset_other_res(HTRUM htrum)
{
    __s32  particle_nr;
    particle_nr = htrum->particle_nr;
    htrum->buoy_y_array = (__s32 *)g_malloc(sizeof(__s32) * particle_nr);

    if (htrum->buoy_y_array == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    htrum->value_array = (__s32 *)g_malloc(sizeof(__s32) * particle_nr);

    if (htrum->value_array == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    htrum->old_length_array = (__s32 *)g_malloc(sizeof(__s32) * particle_nr);

    if (htrum->old_length_array == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    htrum->length_array = (__s32 *)g_malloc(sizeof(__s32) * particle_nr);

    if (htrum->length_array == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    htrum->fall_times = (__s32 *)g_malloc(sizeof(__s32) * particle_nr);

    if (htrum->fall_times == NULL)
    {
        __err("Error in allocating memory.");
        goto error;
    }

    return 0;
error:

    if (htrum->fall_times != NULL)
    {
        g_free(htrum->fall_times);
        htrum->fall_times = NULL;
    }

    if (htrum->old_length_array != NULL)
    {
        g_free(htrum->old_length_array);
        htrum->old_length_array = NULL;
    }

    if (htrum->length_array != NULL)
    {
        g_free(htrum->length_array);
        htrum->length_array = NULL;
    }

    if (htrum->value_array != NULL)
    {
        g_free(htrum->value_array);
        htrum->value_array = NULL;
    }

    if (htrum->buoy_y_array != NULL)
    {
        g_free(htrum->buoy_y_array);
        htrum->buoy_y_array = NULL;
    }

    return -1;
}



static void   _clear_buoy(HTRUM htrum, __s32 i)
{
    if (htrum->buoy_y_array[i] != BUOY_INVALID_Y)
    {
        __s32  x;
        __s32  y;
        x = htrum->buoy_x_array[i];
        y = htrum->buoy_y_array[i];
        GUI_SetColor(htrum->bk_color);
        GUI_FillRect(htrum->disp_rect.x + x,
                     htrum->disp_rect.y + y,
                     htrum->disp_rect.x + x + htrum->buoy_width  - 1,
                     htrum->disp_rect.y + y + htrum->buoy_height - 1);
    }
}



static void   _draw_buoy(HTRUM htrum, __s32 i)
{
    GUI_RECT  dst_pos;
    __s32     x;
    __s32     y;
    x = htrum->buoy_x_array[i];
    y = htrum->buoy_y_array[i];

    if (y != BUOY_INVALID_Y)
    {
        dst_pos.x0 = htrum->disp_rect.x + x;
        dst_pos.y0 = htrum->disp_rect.y + y;
        dst_pos.x1 = dst_pos.x0 + htrum->buoy_width - 1;
        dst_pos.y1 = dst_pos.y0 + htrum->buoy_height - 1;
        GUI_SetRectMem(&dst_pos, htrum->buoy_matrix, htrum->buoy_width, htrum->buoy_height, 0, 0);
    }
}



static void   _draw_particle(HTRUM htrum, __s32 i)
{
    __s32  length;
    __s32  x;
    length = htrum->length_array[i];
    x = htrum->particle_x_array[i];

    if (length != 0)
    {
        GUI_RECT  dst_pos;
        dst_pos.x0 = htrum->disp_rect.x + x;
        dst_pos.y0 = htrum->disp_rect.y + htrum->disp_rect.height - length;
        dst_pos.x1 = dst_pos.x0 + htrum->particle_width - 1;
        dst_pos.y1 = htrum->disp_rect.y + htrum->disp_rect.height - 1;
        GUI_SetRectMem(&dst_pos, htrum->particle_matrix, htrum->particle_width, htrum->particle_height,
                       0, htrum->particle_height - length);
    }
}



static void   _redraw_particle(HTRUM htrum, __s32 i)
{
    __s32  new_length;
    __s32  old_length;
    __s32  x;
    new_length = htrum->length_array[i];
    old_length = htrum->old_length_array[i];
    x = htrum->particle_x_array[i];

    if (new_length > old_length)
    {
        GUI_RECT  dst_pos;
        dst_pos.x0 = htrum->disp_rect.x + x;
        dst_pos.y0 = htrum->disp_rect.y + htrum->disp_rect.height - new_length;
        dst_pos.x1 = dst_pos.x0 + htrum->particle_width - 1;
        dst_pos.y1 = htrum->disp_rect.y + htrum->disp_rect.height - old_length - 1;
        GUI_SetRectMem(&dst_pos, htrum->particle_matrix, htrum->particle_width, htrum->particle_height,
                       0, htrum->particle_height - new_length);
    }
    else if (new_length < old_length)
    {
        GUI_SetColor(htrum->bk_color);
        GUI_FillRect(htrum->disp_rect.x + x,
                     htrum->disp_rect.y + htrum->disp_rect.height - old_length,
                     htrum->disp_rect.x + x + htrum->particle_width - 1,
                     htrum->disp_rect.y + htrum->disp_rect.height - new_length - 1);
    }
}

static void   _redraw_buoy(HTRUM htrum, __s32 i)
{
    __s32  buoy_y;
    __bool is_draw_new_buoy;
    __s32  new_length;
    __s32  rect_height;
    __s32  particle_y;
    is_draw_new_buoy = EPDK_FALSE;
    buoy_y = htrum->buoy_y_array[i];
    new_length = htrum->length_array[i];
    rect_height = htrum->disp_rect.height;
    particle_y = rect_height - new_length;

    if (buoy_y == BUOY_INVALID_Y)
    {
        if (new_length != 0)
        {
            is_draw_new_buoy = EPDK_TRUE;
            buoy_y  = rect_height - 2 * new_length;
            buoy_y = Min(buoy_y, particle_y - htrum->buoy_height);
            htrum->buoy_y_array[i] = Max(buoy_y, BUOY_Y_UPPER_LIMIT);
            htrum->fall_times[i] = 0;
        }
        else
        {
            is_draw_new_buoy = EPDK_FALSE;
            htrum->buoy_y_array[i] = BUOY_INVALID_Y;
            htrum->fall_times[i] = 0;
        }
    }
    else
    {
        __s32  expect_y;
        expect_y   = buoy_y + fall_curve[htrum->fall_times[i]];

        if (expect_y + htrum->buoy_height + MIN_YSPACE_BTW_BUOY_PARTICLE < particle_y)
        {
            is_draw_new_buoy = EPDK_TRUE;
            htrum->buoy_y_array[i] = expect_y;
            htrum->fall_times[i]++;
        }
        else
        {
            if (new_length == 0)
            {
                is_draw_new_buoy = EPDK_FALSE;
                htrum->buoy_y_array[i] = BUOY_INVALID_Y;
                htrum->fall_times[i] = 0;
            }
            else
            {
                is_draw_new_buoy = EPDK_TRUE;

                if (expect_y < particle_y)
                {
                    buoy_y = Min(expect_y - htrum->buoy_height, expect_y - (particle_y - expect_y));
                }
                else
                    buoy_y = Min(expect_y - htrum->buoy_height,
                                 particle_y - (expect_y + htrum->buoy_height  - particle_y));

                buoy_y = Min(buoy_y, particle_y - htrum->buoy_height);
                htrum->buoy_y_array[i] = Max(buoy_y, BUOY_Y_UPPER_LIMIT);
                htrum->fall_times[i] = 0;
            }
        }
    }

    if (is_draw_new_buoy)
    {
        _draw_buoy(htrum, i);
    }
}

static void  _draw_spectrum(HTRUM htrum)
{
    __s32  i;
    GUI_LyrWinSel(htrum->h_lyr);

    for (i = 0; i < htrum->particle_nr; i++)
    {
        /* draw the buoy */
        _draw_buoy(htrum, i);
        /* draw particle */
        _draw_particle(htrum, i);
    }
}

static void  _redraw_spectrum(HTRUM htrum)
{
    __s32  i;
    GUI_LyrWinSel(htrum->h_lyr);

    for (i = 0; i < htrum->particle_nr; i++)
    {
        /* clear the old buoy */
        _clear_buoy(htrum, i);
        /* draw particles */
        _redraw_particle(htrum, i);
        /* draw the buoy */
        _redraw_buoy(htrum, i);
    }
}

#endif     //  ifndef __spectrum_c

/* end of spectrum.c */
