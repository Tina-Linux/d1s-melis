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
#include "schedule_uipara.h"
#include "schedule_list.h"

//#define   LISTBAR_ITEM_NUM   8



typedef struct tag_schedule_list_focus_item_id
{
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} schedule_list_focus_item_id_t;

typedef struct tag_schedule_list_attr
{

    //HTHEME        h_bg_bmp;
    HTHEME      h_list_top_bmp;
    HTHEME      h_item_focus_bmp, h_item_unfocus_bmp;

    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_LBAR   h_listbar;
    H_WIN   layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;
    schedule_list_para_t *para;

    __s32 timer_scan_schedule_id;
    __s32 total;
    //set_para* uipara;
} schedule_list_attr_t;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, __u32 data1, __u32 data2);


static __s32 schedule_list_item_paint_ex(__lbar_draw_para_t *draw_param);
static void _schedule_list_res_init(schedule_list_attr_t *attr);
static void _schedule_list_res_uninit(schedule_list_attr_t *attr) ;


static void schedule_update_top_view(schedule_list_attr_t *gattr)
{
    char tmp[128];
    GUI_RECT gui_rect;
    GUI_LyrWinSel(gattr->layer);
    GUI_BMP_Draw(theme_hdl2buf(gattr->h_list_top_bmp), 0, 0);
    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH;
    gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
    dsk_langres_get_menu_text(STRING_DTV_SCHEDULE, tmp, sizeof(tmp));
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

static __s32 schedule_list_item_paint_ex(__lbar_draw_para_t *draw_param)
{
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    schedule_list_attr_t *gattr;
    //GUI_MEMDEV_Handle draw_mem;
    maple_schdl_event_t *shdl_event = NULL;
    __s32 total = 0;
    char time[128] = {0};
    char tmp[128] = {0};
    char shdl_name[MAPLE_MAX_NAME_LEN];
    gattr = (schedule_list_attr_t *)draw_param->attr;

    //__here__;
    if (gattr == NULL)
    {
        return EPDK_FAIL;
    }

    //__here__;
    if (GUI_LyrWinGetSta(gattr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    //__here__;
    //GUI_LyrWinSel(gattr->layer);
    GUI_SetFont(gattr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinCacheOn();
    //GUI_MEMDEV_Select(draw_mem);
    //gattr->new_focus = draw_param->index;
    //获取节目安排信息
    shdl_event = maple_epg_get_schdl_event(0, draw_param->index);

    //03/11  12:00
    if (shdl_event->startTime.month > 9)
    {
        sprintf(time, "%d", shdl_event->startTime.month);
    }
    else
    {
        sprintf(time, "%d%d", 0, shdl_event->startTime.month);
    }

    eLIBs_strcat(time, "/");

    if (shdl_event->startTime.day > 9)
    {
        sprintf(tmp, "%d  ", shdl_event->startTime.day);
    }
    else
    {
        sprintf(tmp, "%d%d  ", 0, shdl_event->startTime.day);
    }

    eLIBs_strcat(time, tmp);

    if (shdl_event->startTime.hour > 9)
    {
        sprintf(tmp, "%d:", shdl_event->startTime.hour);
    }
    else
    {
        sprintf(tmp, "%d%d:", 0, shdl_event->startTime.hour);
    }

    eLIBs_strcat(time, tmp);

    if (shdl_event->startTime.min > 9)
    {
        sprintf(tmp, "%d", shdl_event->startTime.min);
    }
    else
    {
        sprintf(tmp, "%d%d", 0, shdl_event->startTime.min);
    }

    eLIBs_strcat(time, tmp);
    //__wrn("%s draw_param->index = %d\n",time,draw_param->index);
    //__wrn("x = %d  y=%d\n",draw_param->rect.x,draw_param->rect.y);
    eLIBs_strcpy(shdl_name, shdl_event->name);
    __msg("shdl_name=%s \n", shdl_name);
    __msg("time=%s \n", tmp);
    total = maple_epg_get_eventnum(0);

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            __msg("LBAR_MODE_NORMAL \n");
            GUI_BMP_Draw(theme_hdl2buf(gattr->h_item_unfocus_bmp), draw_param->rect.x, draw_param->rect.y);

            if (total > 0)
            {
                gui_rect.x0 = draw_param->rect.x + 10;
                gui_rect.y0 = draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + 110;
                gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
                GUI_SetColor(gattr->unfocus_txt_color);
                GUI_DispStringInRect(time, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
                gui_rect.x0 = 110 + draw_param->rect.x;
                gui_rect.y0 = draw_param->rect.y;
                gui_rect.x1 = LISTBAR_WIDTH + draw_param->rect.x;
                gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
                GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            }
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            __msg("LBAR_MODE_FOCUS \n");
            GUI_BMP_Draw(theme_hdl2buf(gattr->h_item_focus_bmp), draw_param->rect.x, draw_param->rect.y);

            if (total > 0)
            {
                gui_rect.x0 = draw_param->rect.x + 10;
                gui_rect.y0 = draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + 110;
                gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
                GUI_SetColor(gattr->focus_txt_color);
                GUI_DispStringInRect(time, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
                gui_rect.x0 = 110 + draw_param->rect.x;
                gui_rect.y0 =  draw_param->rect.y;
                gui_rect.x1 = LISTBAR_WIDTH + draw_param->rect.x;
                gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
                GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            }
        }
        break;

        default:
            break;
    }

    //GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    //GUI_MEMDEV_CopyToLCD(draw_mem);
    //GUI_MEMDEV_Select( NULL );
    //GUI_MEMDEV_Delete(draw_mem);
    //draw_mem = NULL;
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}



static void schedule_list_listbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config;
    //RECT rect;
    schedule_list_attr_t *attr;
    __s32 index = 0;
    __s32 start_id, focus_id;
    attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    //dsk_display_get_size((__s32 *)&rect.width,(__s32 *)&rect.height);
#if 0
    index = maple_get_cur_schedule_index(0);

    if ((attr->total - index) < 6)
    {
        start_id = 0;
        focus_id = 0;
    }
    else
#endif
    {
        start_id = 0;//index;
        focus_id = 0;//index;
    }

    __msg("attr->total = %d\n", attr->total);
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.start_id = start_id;
    config.focus_id = focus_id;
    config.item_cnt = attr->total;
    config.item_height = LISTBAR_HIGH;
    config.item_width = LISTBAR_WIDTH;
    config.list_rect.x = 0;
    config.list_rect.y = LISTBAR_HIGH;
    config.list_rect.width = LISTBAR_WIDTH;
    config.list_rect.height = LISTBAR_HIGH * ((attr->total > 10) ? 10 : attr->total); //rect.height - SETTING_TOP_HIGH;
    config.list_attr = (void *)attr;
    attr->h_listbar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(attr->h_listbar, schedule_list_item_paint_ex, &config);
    LISTBAR_ShowPage(attr->h_listbar);
}




/*
    申请资源
*/
static void _schedule_list_res_init(schedule_list_attr_t *attr)
{
    __u32   i;
    __bool  restore_flag;
    attr->h_item_focus_bmp = theme_open(ID_DTV_PRELIST_F_BMP);
    attr->h_item_unfocus_bmp = theme_open(ID_DTV_PRELIST_UF_BMP);
    attr->h_list_top_bmp = theme_open(ID_DTV_SETTING_TOP_BMP);
    //attr->h_bg_bmp = theme_open(ID_DTV_LIST_BG_BMP);
}

static void _schedule_list_res_uninit(schedule_list_attr_t *attr)
{
    __u32   i;
    //theme_close(attr->h_bg_bmp);
    //attr->h_bg_bmp = 0;
    theme_close(attr->h_list_top_bmp);
    attr->h_list_top_bmp = 0;
    theme_close(attr->h_item_focus_bmp);
    attr->h_item_focus_bmp = 0;
    theme_close(attr->h_item_unfocus_bmp);
    attr->h_item_unfocus_bmp = 0;
}

void schedule_list_enter_action(__gui_msg_t *msg)
{
    schedule_list_attr_t *attr;
    attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
}


static __s32 _schedule_list_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    schedule_list_attr_t *attr;
    attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                attr->old_focus = attr->new_focus;
                LISTBAR_PrevItem(attr->h_listbar);
            }

            break;
        }

        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                attr->old_focus = attr->new_focus;
                LISTBAR_NextItem(attr->h_listbar);
            }

            break;
        }

#if 0

        case GUI_MSG_KEY_ENTER:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
            }

            break;
        }

        case GUI_MSG_KEY_MENU:
        case GUI_MSG_KEY_LONGMENU:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                {
                    __gui_msg_t my_msg = {0};
                    my_msg.id = DTV_SETTING_BACK2LIST;
                    my_msg.h_srcwin = NULL;
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.dwAddData1 = 0;
                    my_msg.dwAddData2 = 0;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }
            }

            break;
        }

#endif

        default:
        {
            break;
        }
    }

    return ret;
}

static __s32 _schedule_list_listbar_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 ret = EPDK_OK;
    static __s32 is_touch_down = 0;
    static __s32 is_touch_moving = 0;
    schedule_list_attr_t *attr;
    attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        case GUI_MSG_TOUCH_LONGDOWN:
        {
            GUI_WinSetCaptureWin(msg->h_deswin);
            //  LISTBAR_UpdatePage( attr->h_listbar );   /////////
            ret = LISTBAR_TouchDown(attr->h_listbar, x, y, direct, speed);
            is_touch_down = 1;
            break;
        }

        case GUI_MSG_TOUCH_MOVE:
        {
            if (is_touch_down == 1)
            {
                LISTBAR_TouchMove(attr->h_listbar, x, y, direct, speed);
                is_touch_moving = 1;
            }

            break;
        }

        case GUI_MSG_TOUCH_UP:
        {
            __gui_msg_t send;
            RECT rect;
            __bool chg = EPDK_FALSE;

            if (is_touch_down == 0)
            {
                break;
            }
            else
            {
                is_touch_down = 0;
            }

            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }

            LISTBAR_TouchUp(attr->h_listbar, x, y, direct, speed);
            __msg("----LISTBAR_TouchUp--x: %d, y: %d, direct: %d, speed: %d\n", x, y, direct, speed);
            {
                index = LISTBAR_GetFocusItem(attr->h_listbar);
                //__msg("index = %d\n" , index);
            }
            LISTBAR_GetItemRect(attr->h_listbar, index, &rect);
            __msg("----rect -- x: %d, y: %d, width: %d, height: %d\n", rect.x, rect.y, rect.width, rect.height);

            if (attr->old_focus == attr->new_focus)
            {
                chg = EPDK_TRUE;
            }

            attr->old_focus = attr->new_focus ;

            if ((x < rect.x) || (x > rect.x + rect.width) || (y < rect.y) || (y > rect.y + rect.height))
            {
                //__here__
                break;
            }
            else if (index != attr->new_focus)
            {
                attr->new_focus = index;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

static __s32  schedule_list_touch_proc(__gui_msg_t *msg)
{
    return EPDK_OK;
}

static __s32 schedule_get_cur_index(void)
{
    return 0;
}

static __s32 create_flag = 0;


/*
    回调
*/
static __s32 _schedule_list_Proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            schedule_list_attr_t *attr;
            schedule_list_para_t *para;
            HTHEME  bg_bmp;
            __s32 i;
            create_flag = 1;
            para = (schedule_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
            attr = (schedule_list_attr_t *)My_Malloc(0, sizeof(schedule_list_attr_t));

            if (!attr)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(attr, 0, sizeof(schedule_list_attr_t));
            attr->new_focus = para->focus_id;
            attr->old_focus = para->focus_id;
            attr->para = para;
            _schedule_list_res_init(attr);
            attr->focus_txt_color = SCHEDULE_FOCUS_COLOR;
            attr->unfocus_txt_color = SCHEDULE_UNFOCUS_COLOR;
            attr->font = para->font;
            attr->layer = para->layer;
            attr->timer_scan_schedule_id = SUBMENU_DVB_SCHUDULE_TIMER_ID;
            //GUI_SetTimer(msg->h_deswin, attr->timer_scan_schedule_id, 50, NULL);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)attr);
            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSel(attr->layer);
            attr->total = maple_epg_get_eventnum(0);//只获取当天的

            if (attr->total <= 0)
            {
                attr->total = 10;
            }

            __msg("attr->total=====%d \n", attr->total);
            schedule_update_top_view(attr);

            if (attr->total > 0) //无节目列表时不创建listbar
            {
                schedule_list_listbar_init(msg);
            }

            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_ON);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            schedule_list_attr_t *attr;
            __s32 ret;
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!attr)
            {
                __wrn("attr is null ......\n");
                return EPDK_FAIL;
            }

            //GUI_FrmWinDelete(msg->h_deswin);
            GUI_LyrWinDelete(attr->layer);
            attr->layer = NULL;
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            schedule_list_attr_t *attr;
            schedule_list_para_t *para;
            __s32 ret;
            create_flag--;

            if (0 != create_flag)
            {
                __wrn("-------------------------------------destroy err. create_flag:%d\n", create_flag);
            }

            attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!attr)
            {
                __wrn("attr is null ......\n");
                return EPDK_FAIL;
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, attr->timer_scan_schedule_id))
            {
                GUI_KillTimer(msg->h_deswin, attr->timer_scan_schedule_id);
            }

            if (attr->layer)
            {
                GUI_LyrWinSel(attr->layer);

                if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(attr->layer)
                    && GUI_LYRWIN_STA_SLEEP != GUI_LyrWinGetSta(attr->layer))
                {
                    __wrn("------------------------Err Frame Buf fail.\n");
                }
                else
                {
                    GUI_Clear();
                }
            }

            ret = LISTBAR_SuspendPage(attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar suspend fail\n");
            }

            ret = LISTBAR_Delete(attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar delete fail\n");
            }

            attr->h_listbar = NULL;
            _schedule_list_res_uninit(attr);
            //dsk_reg_flush();  //写进文件里面
            para = attr->para;

            if (para)
            {
                __wrn("...\n");
                My_Mfree(0, para);
            }

            if (attr)
            {
                __wrn("...\n");
                My_Mfree(0, attr);
            }
        }

        return EPDK_OK;

        case GUI_MSG_TIMER:
        {
            schedule_list_attr_t *attr;
            __s32 schudule_num;
            __s32 i;
            __msg("timer coming...msg->dwAddData1 = %d\n", msg->dwAddData1);
            attr = (schedule_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!attr)
            {
                __wrn("attr is null ......\n");
                return EPDK_FAIL;
            }

            if (attr->timer_scan_schedule_id == msg->dwAddData1)
            {
                GUI_ResetTimer(msg->h_deswin, attr->timer_scan_schedule_id, 50, NULL);
                //maple_epg_search();
                esKRNL_TimeDly(10);
                schudule_num = maple_epg_get_eventnum(0);
                __msg("schudule_num = %d attr->total = %d\n", schudule_num, attr->total);

                if (attr->total != schudule_num || 0 == attr->total)
                {
                    maple_epg_getdata();
                    //maple_epg_search_stop();
                    //maple_epg_search();
                    GUI_LyrWinSel(attr->layer);

                    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(attr->layer)
                        && GUI_LYRWIN_STA_SLEEP != GUI_LyrWinGetSta(attr->layer))
                    {
                        __wrn("------------------------Err Frame Buf fail.\n");
                    }

                    attr->total = schudule_num;
                    LISTBAR_Delete(attr->h_listbar);
                    attr->h_listbar = NULL;

                    if (attr->total > 0) //无节目列表时不创建listbar
                    {
                        schedule_update_top_view(attr);
                        schedule_list_listbar_init(msg);
                    }
                    else
                    {
                        //GUI_BMP_Draw(theme_hdl2buf(attr->h_bg_bmp),0,0);
                        for (i = 0; i < 6; i++)
                        {
                            GUI_BMP_Draw(theme_hdl2buf(attr->h_item_focus_bmp), 0, SETTING_TOP_HIGH + LISTBAR_HIGH + LISTBAR_HIGH * (i));
                        }
                    }
                }
            }
        }
        return EPDK_OK;
        case GUI_MSG_KEY:
            return _schedule_list_listbar_key_proc(msg);

        case GUI_MSG_TOUCH:
            return schedule_list_touch_proc(msg);////

        case GUI_MSG_COMMAND:
        {
            /*switch(LOWORD(msg->dwAddData1))
            {
                case DVBEPG_ID_CREATE_EPG_LIST:
                {
                    schedule_list_listbar_init(msg);
                }
                break;
                default:
                    break;
            }*/
        }
        break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN schedule_list_win_create(H_WIN h_parent, schedule_list_para_t *list_para)
{
    __gui_framewincreate_para_t framewin_para;
    schedule_list_para_t  *para;
    FB fb;
    __msg("*********enter  general schedule_list win proc *****************\n");
    GUI_LyrWinGetFB(list_para->layer, &fb);
    para = (schedule_list_para_t *)My_Malloc(0, sizeof(schedule_list_para_t));

    if (!para)
    {
        __msg("memory alloc fail.........\n");
        return NULL;
    }

    eLIBs_memset((void *)para, 0, sizeof(schedule_list_para_t));
    para->font = SWFFont;
    para->layer = list_para->layer;
    para->focus_id = list_para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    SUBMENU_DVB_SCHUDULE_LST;
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = SUBMENU_DVB_SCHUDULE_LIST_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_schedule_list_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 schedule_list_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}
