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
#include <log.h>
#include "desktop_scene.h"
#include "app_multi_screen_home.h"

/*
 *static desktop_scene_ui_t* desktop_scene_get_ui_param(void)
 *{
 *    return &(desktop_scene_ui_800_480);
 *}
 */

/*
 *#define KARAOKE_LYRIC_USE_MALLOC 1
 *
 *#if KARAOKE_LYRIC_USE_MALLOC
 *#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
 *#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
 *#endif
 */

int32_t NUM_PER_LINE    = 4;

static int32_t  home_timeid = 0xfe;

static uint8_t  g_last_desk_app_index   = 0;

static H_LYR _desktop_scene_32bpp_layer_create(RECT *LayerRect)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                                         /* size      */
        {0, 0, 0},                                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0, {NULL, 0}}}, 0},     /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        8,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "desktop scene",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width           = LayerRect->width;
    fb.size.height          = LayerRect->height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = LayerRect->width;
    lstlyr.src_win.height   = LayerRect->height;
    lstlyr.scn_win.x        = LayerRect->x;
    lstlyr.scn_win.y        = LayerRect->y;
    lstlyr.scn_win.width    = LayerRect->width;
    lstlyr.scn_win.height   = LayerRect->height;
    lstlyr.pipe             = 1;
    lstlyr.fb               = &fb;
    lyrcreate_info.lyrpara  = &lstlyr;

    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __wrn("desktop scene layer create error !");
    }

    return layer;
}
static void desktop_scene_view_change_focus(pdesktop_scene_attr_t self)
{
    int16_t     i = 0;
    int32_t     x0 = 0, x1 = 0;
    int32_t     y0 = 0, y1 = 0;
    GUI_RECT    rect = {0};

    GUI_LyrWinSel(self->layer);

    com_memdev_create(self->layer);

    GUI_SetFont(self->pfont);

    GUI_UC_SetEncodeUTF8();

    GUI_SetDrawMode(GUI_DRAWMODE_SHADOW);

    GUI_SetBkColor(0);

    /*清空重画上次聚焦的图标*/    
    draw_multi_screen_icon_coor_rect(self->pui, HOME_UNFOCUS_ICON_START + self->last_focus_item, EPDK_FALSE, &uf_rect_800_480[self->last_focus_item]);

    draw_multi_screen_string_center(self->pui, HOME_STRING_START + self->last_focus_item, self->unfocus_txt_color, &string_rect_800_480[self->last_focus_item]);

    /*清空重画当前聚焦的图标*/
    draw_multi_screen_icon_coor_rect(self->pui, HOME_FOCUS_ICON_START + self->cur_focus_item, EPDK_TRUE, &uf_rect_800_480[self->cur_focus_item]);

    draw_multi_screen_string_center(self->pui, HOME_STRING_START + self->cur_focus_item, self->focus_txt_color, &string_rect_800_480[self->cur_focus_item]);

    __msg("self->last_focus_item=%d  self->cur_focus_item==%d", self->last_focus_item, self->cur_focus_item);

    com_memdev_delete();

    GUI_LyrWinFlushFB(self->layer);
    
}
static void desktop_scene_view_blink_focus(pdesktop_scene_attr_t self)
{
    int16_t     i = 0;
    int32_t     x0 = 0, x1 = 0;
    int32_t     y0 = 0, y1 = 0;
    GUI_RECT    rect = {0};
    static __bool   already_focus = EPDK_TRUE;

    GUI_LyrWinSel(self->layer);

    com_memdev_create(self->layer);

    GUI_SetFont(self->pfont);

    GUI_UC_SetEncodeUTF8();

    GUI_SetDrawMode(GUI_DRAWMODE_SHADOW);

    GUI_SetBkColor(0);

    x0  = uf_rect_800_480[self->cur_focus_item].x0, x1 = uf_rect_800_480[self->cur_focus_item].x1;
    y0  = uf_rect_800_480[self->cur_focus_item].y0, y1 = uf_rect_800_480[self->cur_focus_item].y1;

    GUI_ClearRect(x0, y0, x1, y1);

    if (already_focus)
    {
        draw_multi_screen_icon_coor_rect(self->pui, HOME_UNFOCUS_ICON_START + self->cur_focus_item, EPDK_FALSE, &uf_rect_800_480[self->cur_focus_item]);
        draw_multi_screen_string_center(self->pui, HOME_STRING_START + self->cur_focus_item, self->unfocus_txt_color, &string_rect_800_480[self->cur_focus_item]);
        already_focus = EPDK_FALSE;
    }
    else
    {
        draw_multi_screen_icon_coor_rect(self->pui, HOME_FOCUS_ICON_START + self->cur_focus_item, EPDK_TRUE, &uf_rect_800_480[self->cur_focus_item]);
        draw_multi_screen_string_center(self->pui, HOME_STRING_START + self->cur_focus_item, self->focus_txt_color, &string_rect_800_480[self->cur_focus_item]);
        already_focus = EPDK_TRUE;
    }

    com_memdev_delete();
}

static void desktop_scene_view_show_all(pdesktop_scene_attr_t self)
{
    int16_t     i = 0;
    int32_t     x0 = 0, x1 = 0;
    int32_t     y0 = 0, y1 = 0;

    GUI_LyrWinSel(self->layer);

    com_memdev_create(self->layer);

    GUI_SetFont(self->pfont);

    GUI_UC_SetEncodeUTF8();

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    GUI_SetBkColor(0);

    /*first paint all the icon*/
    for (i = 0; i < self->item_total ; i++)
    {
        if (i != self->cur_focus_item)
        {
            draw_multi_screen_icon_coor_rect(self->pui, HOME_UNFOCUS_ICON_START + i, EPDK_FALSE, &uf_rect_800_480[i]);
            draw_multi_screen_string_center(self->pui, HOME_STRING_START + i, self->unfocus_txt_color, &string_rect_800_480[i]);
        }
        else
        {
            draw_multi_screen_icon_coor_rect(self->pui, HOME_FOCUS_ICON_START + self->cur_focus_item, EPDK_TRUE, &uf_rect_800_480[self->cur_focus_item]);
            draw_multi_screen_string_center(self->pui, HOME_STRING_START + self->cur_focus_item, self->focus_txt_color, &string_rect_800_480[self->cur_focus_item]);
        }
        //__err("icon:%d  x:%d  y:%d  w:%d  h:%d",i,uf_rect_800_480[i].x0,uf_rect_800_480[i].x1,uf_rect_800_480[i].y0,uf_rect_800_480[i].y1);
    }

    com_memdev_delete();
}

int32_t desktop_scene_print_rect_pos(GUI_RECT *gui_rect)
{
    __log("gui_rect->x0=%d, gui_rect->y0=%d, gui_rect->x1=%d, gui_rect->y1=%d",gui_rect->x0, gui_rect->y0, gui_rect->x1, gui_rect->y1);
    return EPDK_OK;
}


static int32_t  desktop_scene_frm_on_key(__gui_msg_t *msg)
{
    static int32_t          last_key = -1;
    pdesktop_scene_attr_t   pdesktopFrm_attr = NULL;

    pdesktopFrm_attr        = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdesktopFrm_attr == NULL)
    {
        __msg("get attr is failed");
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        __inf("last key = %d ", last_key);

        if ((GUI_MSG_KEY_ENTER == last_key) || (GUI_MSG_KEY_PLAY_PAUSE == last_key))
        {
            __inf("self->cur_focus_item= %d ", pdesktopFrm_attr->cur_focus_item);
            app_home_cmd2parent(msg->h_deswin, EXIT_MULTI_SCREEB_HOME_TO_AP_MSG, pdesktopFrm_attr->cur_focus_item, 0);
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
#if 0

            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            {
                __log("GUI_MSG_KEY_PREV or GUI_MSG_KEY_NEXT");
                pdesktopFrm_attr->last_focus_item = pdesktopFrm_attr->cur_focus_item;
                pdesktopFrm_attr->cur_focus_item += NUM_PER_LINE;
                pdesktopFrm_attr->cur_focus_item %=  pdesktopFrm_attr->item_total;
                __log("KEY_RIGHT after:self->cur_focus_item= %d ", pdesktopFrm_attr->cur_focus_item);
                desktop_scene_view_change_focus(pdesktopFrm_attr);
            }
            break;
#endif

            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                pdesktopFrm_attr->last_focus_item   = pdesktopFrm_attr->cur_focus_item;
                pdesktopFrm_attr->cur_focus_item++;

                if (pdesktopFrm_attr->cur_focus_item >= pdesktopFrm_attr->item_total)
                {
                    pdesktopFrm_attr->cur_focus_item = 0;
                }

                __inf("KEY_RIGHT after:self->cur_focus_item= %d ", pdesktopFrm_attr->cur_focus_item);

                desktop_scene_view_change_focus(pdesktopFrm_attr);
            }
            break;

            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                pdesktopFrm_attr->last_focus_item   = pdesktopFrm_attr->cur_focus_item;
                pdesktopFrm_attr->cur_focus_item--;

                if (pdesktopFrm_attr->cur_focus_item < 0)
                {
                    pdesktopFrm_attr->cur_focus_item    = pdesktopFrm_attr->item_total - 1;
                }

                __inf("KEY_LEFT after:self->cur_focus_item= %d ", pdesktopFrm_attr->cur_focus_item);
                     
                desktop_scene_view_change_focus(pdesktopFrm_attr);
            }
            break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key    = -1;
    }
    else
    {
        last_key    = msg->dwAddData1;
    }

    return EPDK_OK;
}
#if 0
static void get_touch_mmenu_index_by_rect(__s32 x,__s32 y,  pdesktop_scene_attr_t self, H_WIN cur_win)
{
	__s32 i;
	for(i = 0;i<8;i++)
	{
		if((x>uf_rect_800_480[i].x0 && x<uf_rect_800_480[i].x1) && (y>uf_rect_800_480[i].y0 && y<uf_rect_800_480[i].y1))
		{
            self->last_focus_item = self->cur_focus_item;
			self->cur_focus_item = i;
            desktop_scene_view_change_focus(self);
            //app_home_cmd2parent(cur_win, EXIT_MULTI_SCREEB_HOME_TO_AP_MSG, self->cur_focus_item, 0);
			break;
		}
	}
}

static __s32 mmenu_touch_proc(__gui_msg_t *msg)
{
	__s32 x = 0, y = 0;

    pdesktop_scene_attr_t   pdesktopFrm_attr = NULL;
    pdesktopFrm_attr        = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

	x = LOSWORD(msg->dwAddData2);
	y = HISWORD(msg->dwAddData2);
	switch(msg->dwAddData1)
	{
		case GUI_MSG_TOUCH_DOWN:
		{
            __log("GUI_MSG_TOUCH_DOWN");
			if (GUI_WinGetCaptureWin() != msg->h_deswin)				// 设置capture
			{
				GUI_WinSetCaptureWin(msg->h_deswin);
			}
			get_touch_mmenu_index_by_rect(x,y,pdesktopFrm_attr,msg->h_deswin);
			break;
		}
		case GUI_MSG_TOUCH_UP:
		{
            __log("GUI_MSG_TOUCH_UP");
            /*
			if(GUI_WinGetCaptureWin() == msg->h_deswin)
			{
				GUI_WinReleaseCapture();
			}
            */
			//main_cmd2parent(msg->h_deswin, ID_OP_SUB_ENTER, pdesktopFrm_attr->focus_item, 0);
			break;
		}
		default:
		{
			break;
		}
	}
	return EPDK_OK;
}
#endif
static __s32 desktop_scene_frm_manager_proc(__gui_msg_t *msg)
{
    //__log("%s, msg->id:%d ", __FUNCTION__, msg->id);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            pdesktop_scene_attr_t pdesktopFrm_attr = NULL;

            pdesktopFrm_attr    = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

            if (pdesktopFrm_attr == NULL)
            {
                __msg("get attr is failed");
                return EPDK_FAIL;
            }

            GUI_LyrWinCacheOn();

            GUI_LyrWinSetSta(pdesktopFrm_attr->layer, GUI_LYRWIN_STA_ON);

            GUI_LyrWinSetTop(pdesktopFrm_attr->layer);

            GUI_LyrWinFlushFB(pdesktopFrm_attr->layer);

            GUI_LyrWinCacheOff();
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            pdesktop_scene_attr_t pdesktopFrm_attr = NULL;

            __msg("++++++GUI_MSG_DESTROY++++++");
            pdesktopFrm_attr    = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

            if (pdesktopFrm_attr == NULL)
            {
                return EPDK_FAIL;
            }

            if (pdesktopFrm_attr != NULL)
            {
                esMEMS_Mfree(0, pdesktopFrm_attr);
                GUI_WinSetAttr(msg->h_deswin, NULL);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, home_timeid) == EPDK_TRUE)
            {
                GUI_KillTimer(msg->h_deswin, home_timeid);
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            pdesktop_scene_attr_t   pdesktopFrm_attr = NULL;

            pdesktopFrm_attr    = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

            if (pdesktopFrm_attr == NULL)
            {
                __msg("pdesktopFrm_attr failed++++++");
                return EPDK_FAIL;
            }

            if (GUI_LyrWinGetSta(pdesktopFrm_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
            {
                return EPDK_OK;
            }

            desktop_scene_view_show_all(pdesktopFrm_attr);

            if (!GUI_IsTimerInstalled(msg->h_deswin, home_timeid))
            {
                //GUI_SetTimer(msg->h_deswin, home_timeid, 100 , NULL);
            }

            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            
			//if((msg->dwAddData1 == 5)&&(msg->dwAddData2 == 1))
            
            if (GUI_IsTimerInstalled(msg->h_deswin, home_timeid))
            {
                GUI_ResetTimer(msg->h_deswin, home_timeid, 100, NULL);
            }

            desktop_scene_frm_on_key(msg);
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
			__inf("touchmsg:%d   x = %d  y = %d",msg->dwAddData1,LOSWORD(msg->dwAddData2),HISWORD(msg->dwAddData2));
            //mmenu_touch_proc(msg);            
            return EPDK_OK;
		}
        case GUI_MSG_WIN_WAKEUP:
        {
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            pdesktop_scene_attr_t pdesktopFrm_attr = NULL;

            pdesktopFrm_attr    = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

            if (pdesktopFrm_attr == NULL)
            {
                __msg("pdesktopFrm_attr failed++++++");
                return EPDK_FAIL;
            }

            if (msg->dwAddData1 == home_timeid)
            {
                desktop_scene_view_blink_focus(pdesktopFrm_attr);
            }
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            pdesktop_scene_attr_t   pdesktopFrm_attr = NULL;

            pdesktopFrm_attr = (pdesktop_scene_attr_t)GUI_WinGetAttr(msg->h_deswin);

            if (pdesktopFrm_attr == NULL)
            {
                __msg("pdesktopFrm_attr failed++++++");
                return EPDK_FAIL;
            }

            if (msg->dwAddData1 == DESKTOP_REFRESH_MSG)
            {
                if (pdesktopFrm_attr->cur_focus_item == msg->dwAddData2)
                {
                    return EPDK_OK;
                }

                pdesktopFrm_attr->last_focus_item   = pdesktopFrm_attr->cur_focus_item;
                pdesktopFrm_attr->cur_focus_item    = msg->dwAddData2;
                desktop_scene_view_change_focus(pdesktopFrm_attr);
            }
        }

        return EPDK_OK;

        default:
            return EPDK_OK;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN desktop_scene_create(H_WIN h_parent)
{
    __gui_framewincreate_para_t framewin_para;
    pdesktop_scene_attr_t       pdesktopFrm_attr = NULL;
    RECT                        layerRect = {0};
    int32_t                     width = 0, height = 0,temp = 0;;
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;
    FB                          fb;

    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(h_parent);

    if (pmulti_screen_home_attr == NULL)
    {
        __wrn("get pmulti_screen_home_attr is null");
        return NULL;
    }

    //分配空间
    pdesktopFrm_attr = (pdesktop_scene_attr_t)esMEMS_Malloc(0, sizeof(desktop_scene_attr_t));

    if (pdesktopFrm_attr == NULL)
    {
        __err("esMEMS_Malloc fail");
        return NULL;
    }

    eLIBs_memset(pdesktopFrm_attr, 0, sizeof(desktop_scene_attr_t));

    /*Create layer*/
    dsk_display_get_size(&width, &height);
	
    layerRect.x         = 0;
    layerRect.y         = 0;
    layerRect.width     = width;//height;
    layerRect.height    = height;//width;
    __log("width = %d, height= %d", layerRect.width, layerRect.height);

    pdesktopFrm_attr->layer = _desktop_scene_32bpp_layer_create(&layerRect);

    /*初始化home 参数*/
    __log("cur_desk_app_index:%d", pmulti_screen_home_attr->cur_desk_app_index);

    //pdesktopFrm_attr->last_focus_item = pdesktopFrm_attr->cur_focus_item = pmulti_screen_home_attr->cur_desk_app_index;
    pdesktopFrm_attr->cur_focus_item    = pmulti_screen_home_attr->cur_desk_app_index;
    pdesktopFrm_attr->last_focus_item   = pmulti_screen_home_attr->cur_desk_app_index;
    pdesktopFrm_attr->focus_txt_color   = GUI_YELLOW;
    pdesktopFrm_attr->unfocus_txt_color = GUI_BLACK;
    pdesktopFrm_attr->item_total        = HOME_UNFOCUS_ICON_NUM;
    pdesktopFrm_attr->pfont             = pmulti_screen_home_attr->pfont;
    pdesktopFrm_attr->pui               = &pmulti_screen_home_attr->ui;

    if (pdesktopFrm_attr->layer == NULL)
    {
        __wrn("desktop_scene_create fail");
        esMEMS_Mfree(0, pdesktopFrm_attr);
        return NULL;
    }

    eLIBs_memset(&fb, 0x00, sizeof(fb));

    GUI_LyrWinGetFB(pdesktopFrm_attr->layer, &fb);

    __inf("fb.width = %d, fb.height=%d", fb.size.width, fb.size.height);

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));

    framewin_para.name          = "desktop_scene_frame";
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = DESKTOP_SCENE_FRM_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)desktop_scene_frm_manager_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)pdesktopFrm_attr;
    framewin_para.hLayer        = pdesktopFrm_attr->layer;

    return ((__hdle)GUI_FrmWinCreate(&framewin_para));
}

int32_t desktop_scene_destory(H_WIN mm_win)
{
    pdesktop_scene_attr_t pdesktopFrm_attr = NULL;

    pdesktopFrm_attr    = (pdesktop_scene_attr_t)GUI_WinGetAttr(mm_win);

    if (pdesktopFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    g_last_desk_app_index = pdesktopFrm_attr->cur_focus_item ;

    if (GUI_WinGetCaptureWin() == mm_win)
    {
        GUI_WinReleaseCapture();
    }

    GUI_LyrWinDelete(pdesktopFrm_attr->layer);
    return EPDK_TRUE;
}

void dsktop_scene_set_cur_app(__u8 index)
{
    g_last_desk_app_index = index;
}

uint8_t dsktop_scene_get_cur_app(void)
{
    __log("g_last_desk_app_index:%d", g_last_desk_app_index);
    return g_last_desk_app_index;
}
