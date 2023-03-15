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
#include "program_preview_uipara.h"
#include "program_preview_list.h"

#define     LISTBAR_ITEM_NUM   10 //用户在当前界面需要 画的listbar个数
#define     LISTBAR_ITEM_ICON_HEIGHT    21
#define     LISTBAR_ITEM_ICON_WIDTH 25
typedef struct tag_program_preview_frmfocus_item_id
{
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} program_preview_frmfocus_item_id_t;

typedef struct tag_program_preview_frmattr
{
    HTHEME      h_title_bmp;
    HTHEME      h_locked_bmp, h_locked_un_bmp;
    HTHEME      h_radio_bmp, h_radio_un_bmp;
    HTHEME      h_item_focus_bmp, h_item_unfocus_bmp;

    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_LBAR   h_listbar;
    H_LYR   layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    program_preview_frmpara_t *para;

    char curprogramname[1024];

    __s32 total;
    //set_para* uipara;
} program_preview_frmattr_t;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, __u32 data1, __u32 data2);


static __s32 program_preview_frmitem_paint_ex(__lbar_draw_para_t *draw_param);
static void _program_preview_frmres_init(program_preview_frmattr_t *attr);
static void _program_preview_frmres_uninit(program_preview_frmattr_t *attr) ;

static void __draw_entpy_listbar(program_preview_frmattr_t *gattr);


static __s32 preview_play_program(__s32 index)
{
    //现在播放全部由线程播放
    //所以缩略图的坐标请在dtv_uipara.c中修改
#if 0
    RECT  rect;
    program_preview_uipara_t *uipara = NULL;
    uipara = program_preview_get_uipara();
    rect.x = uipara->program_pre.x;
    rect.y = uipara->program_pre.y;
    rect.width = uipara->program_pre.width;
    rect.height = uipara->program_pre.height;
    //maple_stop_program();
    maple_play_program_ex(&rect, index); //现在播放全部由线程播放
    //maple_epg_search();
#endif
    return 0;
}
static void program_preview_update_topview(program_preview_frmattr_t *gattr)
{
    char tmp[128];
    GUI_RECT gui_rect;
    program_preview_uipara_t *uipara = NULL;
    uipara = program_preview_get_uipara();
    //顶部更新
    GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_title_bmp), 0, 0);
#if 1
    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH * 2 / 3;
    gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
    dsk_langres_get_menu_text(STRING_DTV_PROGRAM_PREVIEW, tmp, sizeof(tmp));
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //此处还有节目数
#endif
    //program_preview_update_bottom_view(gattr);
}

static __s32 program_preview_update_bottom_view(program_preview_frmattr_t *gattr)
{
#if 0
    char *cur, *next;
    GUI_RECT gui_rect;
    program_preview_uipara_t *uipara = NULL;
    uipara = program_preview_get_uipara();
    maple_sepg_search();
    //底部电视节目
    cur = maple_get_channel_of_curprogramname();
    next = maple_get_channel_of_nextprogramname();
    __msg("gattr->curprogramname = %s, cur = %s\n", gattr->curprogramname, cur);

    if (NULL == gattr->curprogramname[0] && NULL == cur[0])
    {
        return EPDK_OK;
    }

    if (NULL != gattr->curprogramname[0] && NULL == cur[0])
    {
        return EPDK_OK;
    }

    if (gattr->curprogramname[0] && cur[0] && eLIBs_strcmp(gattr->curprogramname, cur) == 0)
    {
        return EPDK_OK;
    }

    eLIBs_strcpy(gattr->curprogramname, cur);
    GUI_SetColor(GUI_DARKGREEN);
    gui_rect.x0 = uipara->cur_next_prog[0].x;
    gui_rect.y0 = uipara->cur_next_prog[0].y;
    gui_rect.x1 = gui_rect.x0 + uipara->cur_next_prog[0].width;
    gui_rect.y1 = gui_rect.y0 + uipara->cur_next_prog[0].height;
    GUI_FillRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
    GUI_SetColor(GUI_DARKGREEN);
    gui_rect.x0 = uipara->cur_next_prog[1].x;
    gui_rect.y0 = uipara->cur_next_prog[1].y;
    gui_rect.x1 = gui_rect.x0 + uipara->cur_next_prog[1].width;
    gui_rect.y1 = gui_rect.y0 + uipara->cur_next_prog[1].height;
    GUI_FillRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
    GUI_SetColor(GUI_BLACK);
    gui_rect.x0 = uipara->cur_next_prog[0].x;
    gui_rect.y0 = uipara->cur_next_prog[0].y;
    gui_rect.x1 = gui_rect.x0 + uipara->cur_next_prog[0].width;
    gui_rect.y1 = gui_rect.y0 + uipara->cur_next_prog[0].height;
    GUI_DispStringInRect(cur, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = uipara->cur_next_prog[1].x;
    gui_rect.y0 = uipara->cur_next_prog[1].y;
    gui_rect.x1 = gui_rect.x0 + uipara->cur_next_prog[1].width;
    gui_rect.y1 = gui_rect.y0 + uipara->cur_next_prog[1].height;
    GUI_DispStringInRect(next, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
#endif
    return 0;
}

static __s32 program_preview_frmitem_paint_ex(__lbar_draw_para_t *draw_param)
{
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    __u8 cafree = EPDK_TRUE;
    program_preview_frmattr_t *gattr;
    //GUI_MEMDEV_Handle draw_mem;
    maple_schdl_event_t *shdl_event = NULL;
    maple_serv_type_e   servType;
    //program_preview_uipara_t *uipara = NULL;
    char server_name[MAPLE_MAX_NAME_LEN] = {0};
    char *tmp = NULL;
    static __s32 cnt = 0;
    char shdl_name[MAPLE_MAX_NAME_LEN];
    gattr = (program_preview_frmattr_t *)draw_param->attr;

    if (gattr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(gattr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    //GUI_LyrWinSel(gattr->layer);
    //uipara = program_preview_get_uipara();
    GUI_SetFont(gattr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinCacheOn();
    //draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y,draw_param->rect.width,draw_param->rect.height);
    //GUI_MEMDEV_Select(draw_mem);
    //获取节目名称
    tmp = maple_get_name_of_channel(draw_param->index);
    servType = maple_get_server_type_of_channel(draw_param->index);
    cafree = maple_get_cafree_status_of_channel(draw_param->index);

    //节目名称对齐显示
    if (draw_param->index < (10 - 1)) //index from 0
    {
        eLIBs_sprintf(server_name, "   %d    %s", draw_param->index + 1, tmp);
    }
    else
    {
        eLIBs_sprintf(server_name, "   %d   %s", draw_param->index + 1, tmp);
    }

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            __msg("LBAR_MODE_NORMAL \n");
            GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_unfocus_bmp), draw_param->rect.x, draw_param->rect.y);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + draw_param->rect.width;
            gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
            GUI_SetColor(gattr->unfocus_txt_color);
            GUI_DispStringInRect(server_name, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);

            if (servType == MAPLE_SERV_TYPE_RADIO)
            {
                gui_rect.x0 = draw_param->rect.x + draw_param->rect.width - 2 * LISTBAR_ITEM_ICON_WIDTH;
                gui_rect.y0 = draw_param->rect.y + (LISTBAR_HIGH - LISTBAR_ITEM_ICON_HEIGHT) / 2;
                GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_radio_bmp), gui_rect.x0, gui_rect.y0);
            }

            if (cafree == EPDK_FALSE)
            {
                gui_rect.x0 = draw_param->rect.x + draw_param->rect.width - LISTBAR_ITEM_ICON_WIDTH;
                gui_rect.y0 = draw_param->rect.y + (LISTBAR_HIGH - LISTBAR_ITEM_ICON_HEIGHT) / 2;
                GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_locked_un_bmp), gui_rect.x0, gui_rect.y0);
            }
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            __msg("LBAR_MODE_FOCUS \n");
            GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_focus_bmp), draw_param->rect.x, draw_param->rect.y);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + draw_param->rect.width;
            gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
            GUI_SetColor(gattr->focus_txt_color);
            GUI_DispStringInRect(server_name, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);  /* GUI_TA_VCENTER */

            if (servType == MAPLE_SERV_TYPE_RADIO)
            {
                gui_rect.x0 = draw_param->rect.x + draw_param->rect.width - 2 * LISTBAR_ITEM_ICON_WIDTH;
                gui_rect.y0 = draw_param->rect.y + (LISTBAR_HIGH - LISTBAR_ITEM_ICON_HEIGHT) / 2;
                GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_radio_bmp), gui_rect.x0, gui_rect.y0);
            }

            if (cafree == EPDK_FALSE)
            {
                gui_rect.x0 = draw_param->rect.x + draw_param->rect.width - LISTBAR_ITEM_ICON_WIDTH;
                gui_rect.y0 = draw_param->rect.y + (LISTBAR_HIGH - LISTBAR_ITEM_ICON_HEIGHT) / 2;
                GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_locked_bmp), gui_rect.x0, gui_rect.y0);
            }
        }
        break;

        default:
        {
        }
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



static void program_preview_frmlistbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config;
    RECT rect;
    program_preview_frmattr_t *attr;
    __s32 index;
    __s32 start_id, focus_id;
    program_preview_uipara_t *uipara = NULL;
    attr = (program_preview_frmattr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == attr)
    {
        return ;
    }

    uipara = program_preview_get_uipara();
    index = maple_get_cur_program_preview_index();

    if (index < 10)
    {
        start_id = 0;
        focus_id = index;
    }
    else
    {
        start_id = index - 9;
        focus_id = index;
    }

    __msg("start_id:%d, focus_id:%d\n", start_id, focus_id);
    __msg("attr->total = %d, index = %d\n", attr->total, index);
    __msg("x = %d, y = %d, w=%d, h = %d\n", uipara->listbar.x, uipara->listbar.y, uipara->listbar.width, uipara->listbar.height);
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.start_id = start_id;
    config.focus_id = focus_id;
    config.item_cnt = attr->total;
    config.item_height = LISTBAR_HIGH;
    config.item_width = LISTBAR_WIDTH * 2 / 3;
    config.list_rect.x = 0;
    config.list_rect.y = LISTBAR_HIGH;
    config.list_rect.width = LISTBAR_WIDTH * 2 / 3;
    config.list_rect.height = (attr->total > 10 ? 10 : attr->total) * LISTBAR_HIGH;
    config.list_attr = (void *)attr;
    attr->h_listbar = LISTBAR_Create(msg->h_deswin);

    if (NULL == attr->h_listbar)
    {
        __wrn("listbar create fail\n");
        return ;
    }

    LISTBAR_Config(attr->h_listbar, program_preview_frmitem_paint_ex, &config);
    LISTBAR_ShowPage(attr->h_listbar);
}




/*
    申请资源
*/
static void _program_preview_frmres_init(program_preview_frmattr_t *attr)
{
    __u32   i;
    __bool  restore_flag;
    attr->h_item_focus_bmp = dsk_theme_open(ID_DTV_PRELIST_F_BMP);
    attr->h_item_unfocus_bmp = dsk_theme_open(ID_DTV_PRELIST_UF_BMP);
    attr->h_title_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);
    attr->h_locked_bmp = dsk_theme_open(ID_DTV_PRELIST_LOCK_BMP);
    attr->h_locked_un_bmp = dsk_theme_open(ID_DTV_PRELIST_LOCK_UN_BMP);
    attr->h_radio_bmp = dsk_theme_open(ID_DTV_PRELIST_RADIO_BMP);
    attr->h_radio_un_bmp = dsk_theme_open(ID_DTV_PRELIST_RADIO_UN_BMP);
}

static void _program_preview_frmres_uninit(program_preview_frmattr_t *attr)
{
    __u32   i;
    dsk_theme_close(attr->h_title_bmp);
    attr->h_title_bmp = 0;
    dsk_theme_close(attr->h_item_focus_bmp);
    attr->h_item_focus_bmp = 0;
    dsk_theme_close(attr->h_item_unfocus_bmp);
    attr->h_item_unfocus_bmp = 0;
    dsk_theme_close(attr->h_locked_bmp);
    attr->h_locked_bmp = 0;
    dsk_theme_close(attr->h_locked_un_bmp);
    attr->h_locked_un_bmp = 0;
    dsk_theme_close(attr->h_radio_bmp);
    attr->h_radio_bmp = 0;
    dsk_theme_close(attr->h_radio_un_bmp);
    attr->h_radio_un_bmp = 0;
}

void program_preview_frmenter_action(__gui_msg_t *msg)
{
    program_preview_frmattr_t *attr;
    attr = (program_preview_frmattr_t *)GUI_WinGetAddData(msg->h_deswin);
}


static __s32 _program_preview_frmlistbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    program_preview_frmattr_t *attr;
    __wrn("_program_preview_frmlistbar_key_proc() : msg->dwAddData1 = %d, msg->dwAddData2 = %d\n", msg->dwAddData1, msg->dwAddData2);
    attr = (program_preview_frmattr_t *)GUI_WinGetAddData(msg->h_deswin);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_PREV:
        case GUI_MSG_KEY_LONGPREV:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                __s32 index;
                maple_sepg_clean();
                //__wrn("maple_stop_program 123.\n");
                //maple_stop_program();
                g_is_programme_list_selected = 1;
                //com_memdev_create(attr->layer);
                attr->old_focus = attr->new_focus;
                LISTBAR_PrevItem(attr->h_listbar);
                index = LISTBAR_GetFocusItem(attr->h_listbar);
                attr->new_focus = index;
                //dtv_set_cur_user_index(index);
                //preview_play_program(dtv_get_cur_user_index());
                //__draw_entpy_listbar(attr);
                //com_memdev_delete();
                eLIBs_memclr(attr->curprogramname, sizeof(attr->curprogramname));
            }

            break;
        }

        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_NEXT:
        case GUI_MSG_KEY_LONGNEXT:
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                __s32 index;
                maple_sepg_clean();
                //__wrn("maple_stop_program 123.\n");
                //maple_stop_program();
                g_is_programme_list_selected = 1;
                //com_memdev_create(attr->layer);
                attr->old_focus = attr->new_focus;
                LISTBAR_NextItem(attr->h_listbar);
                index = LISTBAR_GetFocusItem(attr->h_listbar);
                attr->new_focus = index;
                //__draw_entpy_listbar(attr);
                //com_memdev_delete();
                eLIBs_memclr(attr->curprogramname, sizeof(attr->curprogramname));
            }

            break;
        }

        case GUI_MSG_KEY_ENTER:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);

                if (attr->new_focus != dtv_get_cur_user_index())
                {
                    dtv_set_cur_user_index(attr->new_focus);
                    dtv_ca_msg_clear();
                    maple_stop_program();
                    maple_play_program();
                    check_dtv_or_radio();

                    if (dtv_ctr.h_no_auth != NULL || dtv_ctr.h_radio_tip)
                    {
                        GUI_WinSetFocusChild(msg->h_deswin);
                    }
                }
                else
                {
                    __gui_msg_t my_msg;
                    my_msg.id = GUI_MSG_CLOSE;
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

        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_MENU:
        case GUI_MSG_KEY_LONGMENU:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                {
                    __gui_msg_t my_msg;
                    my_msg.id = GUI_MSG_CLOSE;
                    my_msg.h_srcwin = NULL;
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.dwAddData1 = 0;
                    my_msg.dwAddData2 = 0;
                    GUI_SendNotifyMessage(&my_msg);
                }
            }

            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

static __s32 _program_preview_frmlistbar_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 ret = EPDK_OK;
    static __s32 is_touch_down = 0;
    static __s32 is_touch_moving = 0;
    program_preview_frmattr_t *attr;
    attr = (program_preview_frmattr_t *)GUI_WinGetAddData(msg->h_deswin);
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

static __s32  program_preview_frmtouch_proc(__gui_msg_t *msg)
{
    return EPDK_OK;
}


//listbar不足LISTBAR_ITEM_NUM时，在其后画unfocus图片
static void __draw_entpy_listbar(program_preview_frmattr_t *gattr)
{
    __s32 i;
    RECT  rect;
    __s32 index;
    program_preview_uipara_t *uipara;
    uipara = program_preview_get_uipara();
    __msg("__draw_entpy_listbar()\n");

    if (gattr->total >= (uipara->listbar.height / LISTBAR_HIGH))    /* LISTBAR_ITEM_NUM */
    {
        return ;
    }

    //    index = LISTBAR_ITEM_NUM - gattr->total;
    for (i = gattr->total; i < (uipara->listbar.height / LISTBAR_HIGH); i++) /* LISTBAR_ITEM_NUM */
    {
        rect.x = 0;//uipara->listbar.x;
        rect.y = uipara->listbar.y + LISTBAR_HIGH * i;
        //rect.width = uipara->listbar.width;
        //rect.height = uipara->listbar.height;
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_unfocus_bmp), rect.x, rect.y);
    }
}

static __s32 create_flag = 0;

/*
    回调
*/
static __s32 _program_preview_frmProc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            program_preview_frmattr_t *attr;
            program_preview_frmpara_t *para;
            HTHEME  bg_bmp;
            para = (program_preview_frmpara_t *)GUI_WinGetAttr(msg->h_deswin);
            attr = (program_preview_frmattr_t *)My_Malloc(0, sizeof(program_preview_frmattr_t));
            create_flag = 1;

            if (!attr)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(attr, 0, sizeof(program_preview_frmattr_t));
            attr->new_focus = para->focus_id;
            attr->old_focus = para->focus_id;
            attr->para = para;
            _program_preview_frmres_init(attr);
            attr->focus_txt_color = PROGRAM_PREVIEW_FOCUS_COLOR;
            attr->unfocus_txt_color = PROGRAM_PREVIEW_UNFOCUS_COLOR;
            attr->font = para->font;
            attr->layer = para->layer;
            attr->total = maple_get_cur_service_list_num();

            if (attr->total <= 0)
            {
                attr->total = 10;
            }

            __msg("attr->total = %d\n", attr->total);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)attr);
            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSetTop(attr->layer);
            //if(!GUI_IsTimerInstalled(msg->h_deswin,SUBMENU_DVB_PREVIEW_UPDATEID))
            //{
            // //   GUI_SetTimer(msg->h_deswin, SUBMENU_DVB_PREVIEW_UPDATEID, 1000, NULL);
            //}
            program_preview_update_topview(attr);
            program_preview_frmlistbar_init(msg);
            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_ON);
        }

        return EPDK_OK;

        case GUI_MSG_TIMER:
            return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            program_preview_frmattr_t *attr;
            program_preview_frmpara_t *para;
            __s32 ret;
            //if(GUI_IsTimerInstalled(msg->h_deswin,SUBMENU_DVB_PREVIEW_UPDATEID))
            //{
            //  GUI_KillTimer(msg->h_deswin, SUBMENU_DVB_PREVIEW_UPDATEID);
            //}
            create_flag--;

            if (0 != create_flag)
            {
                __wrn("-------------------------------------destroy err. create_flag:%d\n", create_flag);
            }

            attr = (program_preview_frmattr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!attr)
            {
                __wrn("attr is null ......\n");
                return EPDK_FAIL;
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
            _program_preview_frmres_uninit(attr);
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

        case GUI_MSG_PAINT:
            return EPDK_OK;

        case GUI_MSG_KEY:
            return _program_preview_frmlistbar_key_proc(msg);

        case GUI_MSG_TOUCH:
            return program_preview_frmtouch_proc(msg);////

        case GUI_MSG_COMMAND:
        {
            /*switch(LOWORD(msg->dwAddData1))
            {
                case DVBEPG_ID_CREATE_EPG_LIST:
                {
                    program_preview_frmlistbar_init(msg);
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

H_WIN program_preview_frmwin_create(H_WIN h_parent, program_preview_frmpara_t *list_para)
{
    __gui_framewincreate_para_t framewin_para;
    program_preview_frmpara_t  *para;
    FB fb;
    __msg("*********enter  general program_preview_list win proc *****************\n");
    GUI_LyrWinGetFB(list_para->layer, &fb);
    para = (program_preview_frmpara_t *)My_Malloc(0, sizeof(program_preview_frmpara_t));

    if (!para)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset((void *)para, 0, sizeof(program_preview_frmpara_t));
    para->font = SWFFont;
    para->layer = list_para->layer;
    para->focus_id = list_para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    SUBMENU_DVB_PREVIEW_LST;
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = SUBMENU_DVB_PREVIEW_LIST_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_program_preview_frmProc);
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


__s32 program_preview_frmwin_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}
