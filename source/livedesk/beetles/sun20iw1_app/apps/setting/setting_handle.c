/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : setting_handle.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "apps.h"
#if (BEETLES_GAME_FLAG == 1)

#include "Setting_handle.h"
#include "setting_uipara.h"
#include "app_setting_i.h"

#define     HANDLE_LISTBAR_ITEM_NUM   16


typedef struct tag_handle_item_id
{
    __s32 id_string_detail_prompt;
    __s32 id_string_detail_prompt_ex;
    __s32 id_string_title;
} handle_item_id_t;

typedef struct tag_handle_item_res
{
    char string_title[128];
    OFFSET string_title_pos;

    char string_content[128];
    RECT  string_content_rect;

    H_THEME  h_bmp_focus;
    OFFSET   bmp_focus_pos;

    H_THEME  h_bmp_unfocus;
    OFFSET   bmp_unfocus_pos;

    __s32 content_num;
    __s32 content_nr;

    __s32 handle_adjust_flag;
} handle_item_res_t;

typedef struct tag_setting_focus_item_id
{
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} setting_focus_item_id_t;

typedef struct tag_setting_handle_attr
{
    char string_prompt[128];
    char string_detail_prompt[128];
    char string_detail_prompt_ex[128];
    H_THEME     h_bmp_big_setting, h_bmp_prompt, h_bmp_bottom, h_bg_bmp, h_left_bg_bmp, h_game_item;
    HTHEME      h_item_focus_bmp, h_item_unfocus_bmp;
    HTHEME      h_return_fcs_bmp, h_return_unfcs_bmp;
    handle_item_res_t res[HANDLE_LISTBAR_ITEM_NUM];
    /*  handle_item_res_t res_down;
        handle_item_res_t res_left;
        handle_item_res_t res_rignt;
        handle_item_res_t res_a;
        handle_item_res_t res_b;
        handle_item_res_t res_c;
        handle_item_res_t res_d;
        handle_item_res_t res_lshoot;
        handle_item_res_t res_rshoot;
        handle_item_res_t res_start;
        handle_item_res_t res_select;
    */
    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_LBAR   h_listbar;
    H_WIN   layer;
    GUI_FONT *font;
    H_WIN   parent;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;
    setting_handle_para_t *handle_para;

    __u32               handle_adjust_thread_id;
    __u8                handle_adjust_status;
    //set_handle_para* handle_uipara;
} setting_handle_attr_t;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, unsigned long data1, unsigned long data2);
extern __bool  bwait_for_handle_adjudt;
static __s32 setting_handle_content_paint(__gui_msg_t *msg);

static reg_system_para_t *setting_reg_para = NULL;

static setting_focus_item_id_t setting_focus_item_res_id =
{
    ID_SETTING_NEW_ITEM_R_FCS_BMP, ID_SETTING_NEW_ITEM_R_UFCS_BMP //ID_SETTING_SET_ITEM_F_BMP,ID_SETTING_SET_ITEM_UFCS_BMP
};

static handle_item_id_t setting_handle_res_id[] =
{
    {STRING_SET_GAME_HANDLE_UP, NULL, STRING_SET_GAME_HANDLE_UP},   // 0
    {STRING_SET_GAME_HANDLE_DOWN, NULL, STRING_SET_GAME_HANDLE_DOWN},   // 1
    {STRING_SET_GAME_HANDLE_LEFT, NULL, STRING_SET_GAME_HANDLE_LEFT},       // 2
    {STRING_SET_GAME_HANDLE_RIGHT, NULL, STRING_SET_GAME_HANDLE_RIGHT}, // 5
    {STRING_SET_GAME_HANDLE_A, NULL, STRING_SET_GAME_HANDLE_A}, // 输出
    {STRING_SET_GAME_HANDLE_B, NULL, STRING_SET_GAME_HANDLE_B}, // 按键音
    {STRING_SET_GAME_HANDLE_C, NULL, STRING_SET_GAME_HANDLE_C},
    {STRING_SET_GAME_HANDLE_D, NULL, STRING_SET_GAME_HANDLE_D},
    {STRING_SET_GAME_HANDLE_LSHOOT, NULL, STRING_SET_GAME_HANDLE_LSHOOT},
    {STRING_SET_GAME_HANDLE_RSHOOT, NULL, STRING_SET_GAME_HANDLE_RSHOOT},
    {STRING_SET_GAME_HANDLE_START, NULL, STRING_SET_GAME_HANDLE_START},
    {STRING_SET_GAME_HANDLE_ESC, NULL, STRING_SET_GAME_HANDLE_ESC},
    //..{STRING_SET_GAME_HANDLE_SELECT,NULL, STRING_SET_GAME_HANDLE_SELECT},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},


};
#define HANDLE_ADJUST_COMPLETE     1
#define HANDLE_ADJUST_WAIT     2


static __s32 setting_handle_item_paint_ex(__lbar_draw_para_t *draw_param);
static void _setting_handle_res_init(setting_handle_attr_t *handle_attr);
static void _setting_handle_res_uninit(setting_handle_attr_t *handle_attr) ;
extern  __u32                   arg;
extern __u32 handle_key_map[];
extern USBHJoystick_CallBack    joystickRead;

static void gbxgame_handle_adjust_thread_cb(void *parg)
{
    __bool loopFlag = EPDK_TRUE;
    __s32  cur_key;
    __s32  realse = EPDK_TRUE;
    H_WIN  h_handle;
    int i = 0;
    __s32 oldbuttom, oldstickX, oldstickY;
    __s32 buttom = 0, stickX = 0, stickY = 0x7f, reserve = 0;
    reg_game_para_t *para;
    setting_handle_attr_t *handle_attr;
    handle_item_res_t  *p_item_res;
    handle_attr = (setting_handle_attr_t *)parg;

    if (joystickRead == NULL)
    {
        return;
    }

    para = (reg_game_para_t *)dsk_reg_get_para_by_app(REG_APP_GAME);

    if (para && (EPDK_FALSE == para->adjust_flag))
    {
        for (i = 0; i < HANDLE_LISTBAR_ITEM_NUM; i++)
        {
            para->handle[i][0] = handle_key_map[i];
        }

        for (i = 1; i < 10; i++) //初始化buttom等前多次校验
        {
            joystickRead((void *)arg, &buttom, &stickX, &stickY, &reserve);

            if (reserve == -1)
            {
                joystickRead = NULL;
                break;
            }

            if (i != 1)
            {
                if ((buttom == oldbuttom) && (stickX == oldstickX) && (stickY == oldstickY))
                {
                    continue;
                }
                else
                {
                    oldbuttom = buttom;
                    oldstickX = stickX;
                    oldstickY = stickY;
                    i = 0;//没开始校验就有按键按下
                    esKRNL_TimeDly(10);
                }
            }
            else
            {
                oldbuttom = buttom;
                oldstickX = stickX;
                oldstickY = stickY;
            }
        }

        if (i == 10)
        {
            para->buttom = buttom;
            para->stickx = stickX;
            para->sticky = stickY;
        }
        else
        {
            loopFlag = EPDK_FALSE;
        }

        i = 0;
    }

    //..__wrn("++++para->buttom=%d,para->stickx=%d,para->sticky = %d\n",para->buttom,para->stickx,para->sticky = stickY);
    while (loopFlag)
    {
        if (HANDLE_ADJUST_WAIT == handle_attr->handle_adjust_status)
        {
            //校验完成提示
            H_WIN  h_dialog;
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_SET_GAME_HANDLE_ADJUST_DONE };
            __wrn("1.......................\n");
            default_dialog(h_dialog, GUI_WinGetParent(handle_attr->parent), HANDLE_ADJUST_DIALOG_ID, ADLG_OK, lang_id);
            esKRNL_TimeDly(200);

            if (h_dialog)
            {
                app_dialog_destroy(h_dialog);
                h_dialog = NULL;
            }

            handle_attr->handle_adjust_status = HANDLE_ADJUST_COMPLETE;
        }
        else if (HANDLE_ADJUST_COMPLETE == handle_attr->handle_adjust_status)
        {
            //__wrn("2.......................\n");
            loopFlag = EPDK_FALSE;
        }
        else
        {
            joystickRead((void *)arg, &buttom, &stickX, &stickY, &reserve);

            //__wrn("!!!!  %08x     %08x    %08x\n",para->buttom,para->stickx,para->sticky);
            //__wrn("@@@@  %08x     %08x    %08x\n",buttom,stickX,stickY);
            if (reserve == -1)
            {
                joystickRead = NULL;
                loopFlag = EPDK_FALSE;
            }

            //.. __wrn("---para->buttom=%d,para->stickx=%d,para->sticky = %d\n",para->buttom,para->stickx,para->sticky = stickY);

            if ((buttom == para->buttom)
                && (stickX == para->stickx)
                && (stickY == para->sticky))
            {
                __wrn("-----realse=%d\n", realse);
                __wrn("-----++handle_attr->new_focus=%d\n", handle_attr->new_focus);

                if (EPDK_FALSE == realse)
                {
                    __gui_msg_t  msg;
                    msg.id = GUI_MSG_COMMAND;
                    msg.h_deswin = handle_attr->parent;
                    GUI_SendMessage(&msg);
                    __wrn("-------handle_attr->new_focus=%d\n", handle_attr->new_focus);

                    if (handle_attr->new_focus >= 12) //..
                    {
                        __wrn("addust complete....\n");
                        handle_attr->handle_adjust_status = HANDLE_ADJUST_WAIT;
                        para->adjust_flag = EPDK_TRUE;
                    }
                }

                realse = EPDK_TRUE;
                esKRNL_TimeDly(10);
            }
            else if (EPDK_TRUE == realse)
            {
                realse = EPDK_FALSE;

                //__wrn("@@@@   %08x        %08x    %08x\n",buttom,stickX,stickY);
                if (buttom != para->buttom)
                {
                    i++;
                }

                if (stickX != para->stickx)
                {
                    i++;
                }

                if (stickY != para->sticky)
                {
                    i++;
                }

                if (i >= 2)
                {
                    //组合键,提示用户
                    __wrn(".....................7 handle_attr->new_focus = %d\n", handle_attr->new_focus);
                }
                else if (EPDK_FALSE == handle_attr->res[handle_attr->new_focus].handle_adjust_flag)
                {
                    if (buttom != para->buttom)
                    {
                        int bit;

                        for (i = 0; i < (sizeof(buttom) * 8 - 1); i++)
                        {
                            bit = ((__u32)1 << i);

                            if ((buttom & bit) != (para->buttom & bit))
                            {
                                para->handle[handle_attr->new_focus][1] = 0;
                                para->handle[handle_attr->new_focus][2] = bit;
                            }
                        }
                    }
                    else if (stickX != para->stickx)
                    {
                        para->handle[handle_attr->new_focus][1] = 1;
                        para->handle[handle_attr->new_focus][2] = stickX;
                    }
                    else if (stickY != para->sticky)
                    {
                        para->handle[handle_attr->new_focus][1] = 2;
                        para->handle[handle_attr->new_focus][2] = stickY;
                    }

                    handle_attr->res[handle_attr->new_focus].handle_adjust_flag = EPDK_TRUE;
                    p_item_res = &handle_attr->res[handle_attr->new_focus];
                    eLIBs_strcpy(p_item_res->string_content, "OK");
                    //__wrn("@@@@  %08x        %08x    %08x\n",buttom,stickX,stickY);
                    //__wrn("!!!!  %08x        %08x    %08x\n",para->buttom,para->stickx,para->sticky);
                    LISTBAR_UpdatePage(handle_attr->h_listbar);
                }

                i = 0;
            }
        }

        esKRNL_TimeDly(10);

        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            loopFlag = EPDK_FALSE;
        }
    }

end:
    {
        __gui_msg_t  msg;

        if (!handle_attr->handle_para->call_status)
        {
            msg.id = MSG_OP_EXIT;
            msg.h_deswin = handle_attr->parent;
            __wrn("MSG_OP_EXIT'''''''\n");
        }
        else
        {
            msg.id = GUI_MSG_COMMAND;
            msg.h_deswin = GUI_WinGetParent(handle_attr->parent);
            __wrn("GUI_MSG_COMMAND'''''''\n");
        }

        GUI_SendNotifyMessage(&msg);
    }
    esKRNL_TDel(OS_PRIO_SELF);
    __wrn("delete thread ok'''''''\n");
}



static void setting_listbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config;
    setting_uipara_t *uipara = NULL;
    setting_handle_attr_t *handle_attr;
    handle_attr = (setting_handle_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    uipara = (setting_uipara_t *)setting_get_uipara();
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.focus_id = 0;
    config.item_cnt = HANDLE_LISTBAR_ITEM_NUM ;
    config.item_height = uipara->item.h;
    config.item_width = uipara->item.w;
    config.list_rect.x = uipara->item.x;
    config.list_rect.y = uipara->item.y;
    config.list_rect.width = uipara->item.w;
    config.list_rect.height = uipara->lyr_scn.h - 56;
    config.start_id = 0;
    config.list_attr = (void *)handle_attr;
    handle_attr->h_listbar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(handle_attr->h_listbar, setting_handle_item_paint_ex, &config);
    LISTBAR_ShowPage(handle_attr->h_listbar);
}

static __bool position_in_rect(RECT *rect, __s32 x, __s32 y)
{
    if ((x > rect->x) && (y > rect->y) && (x < (rect->x + rect->width)) && (y < (rect->y + rect->height)))
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

/*
    申请资源
*/
static void _setting_handle_res_init(setting_handle_attr_t *handle_attr)
{
    __u32   i;
    __bool  restore_flag;
    handle_item_res_t  *p_item_res;
    handle_attr->h_bmp_big_setting = theme_open(ID_SETTING_SET_BSYSSE_BMP);
    handle_attr->h_bg_bmp = theme_open(ID_SETTING_WINDOW_BG_BMP);
    handle_attr->h_item_focus_bmp = theme_open(setting_focus_item_res_id.id_bmp_focus_item);
    handle_attr->h_item_unfocus_bmp = theme_open(setting_focus_item_res_id.id_bmp_unfocus_item);
    handle_attr->h_return_fcs_bmp = theme_open(ID_SETTING_RETURN_FCS_BMP);
    handle_attr->h_return_unfcs_bmp = theme_open(ID_SETTING_RETURN_UFCS_BMP);
    handle_attr->h_left_bg_bmp = theme_open(ID_SETTING_LEFT_BG_BMP);
    handle_attr->item_nr = sizeof(setting_handle_res_id) / sizeof(setting_handle_res_id[0]);
    handle_attr->h_game_item = theme_open(ID_SETTING_GAME_ITEM_BMP);
    __wrn("handle_attr->item_nr = %d \n", handle_attr->item_nr);

    for (i = 0; i < 12; i++)
    {
        p_item_res = &handle_attr->res[i];
        eLIBs_strcpy(p_item_res->string_content, "-");
    }
}

static void _setting_handle_res_uninit(setting_handle_attr_t *handle_attr)
{
    __u32   i;
    handle_item_res_t  *p_item_res;
    theme_close(handle_attr->h_bmp_big_setting);
    handle_attr->h_bmp_big_setting = 0;
    theme_close(handle_attr->h_bg_bmp);
    handle_attr->h_bg_bmp = 0;
    theme_close(handle_attr->h_item_focus_bmp);
    handle_attr->h_item_focus_bmp = 0;
    theme_close(handle_attr->h_item_unfocus_bmp);
    handle_attr->h_item_unfocus_bmp = 0;
    theme_close(handle_attr->h_return_fcs_bmp);
    handle_attr->h_return_fcs_bmp = 0;
    theme_close(handle_attr->h_return_unfcs_bmp);
    handle_attr->h_return_unfcs_bmp = 0;
    theme_close(handle_attr->h_left_bg_bmp);
    handle_attr->h_left_bg_bmp = 0;
    theme_close(handle_attr->h_game_item);
    handle_attr->h_game_item = 0;
}
/*
void setting_handle_on_set_content_num(setting_handle_attr_t *handle_attr)
{
    if( setting_reg_para == NULL )
        return;
    //语言
    if( setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESES )
        handle_attr->res_up.content_num = 0;
    else if( setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESET )
        handle_attr->res_up.content_num = 1;
    else if( setting_reg_para->language == EPDK_LANGUAGE_ENM_ENGLISH )
        handle_attr->res_up.content_num = 2;
    else
        handle_attr->res_up.content_num = 0;

}*/
static __s32  _setting_handle_content_res_reset(setting_handle_attr_t *handle_attr)
{
    int i;
    __u32    *content_id = NULL ;
    handle_item_res_t  *p_item_res = NULL ;
    //__msg( " handle_attr->new_focus = %d \n " , handle_attr->new_focus );
    dsk_langres_get_menu_text(setting_handle_res_id[handle_attr->new_focus].id_string_detail_prompt, handle_attr->string_detail_prompt, sizeof(handle_attr->string_detail_prompt));
    dsk_langres_get_menu_text(setting_handle_res_id[handle_attr->new_focus].id_string_detail_prompt_ex, handle_attr->string_detail_prompt_ex, sizeof(handle_attr->string_detail_prompt_ex));
    //__here__ ;
    p_item_res = &handle_attr->res[handle_attr->new_focus];
    dsk_langres_get_menu_text(setting_handle_res_id[handle_attr->new_focus].id_string_title, p_item_res->string_title, sizeof(p_item_res->string_title));
    return EPDK_OK ;
}

static __s32 setting_handle_paint_item_detail_ex(setting_handle_attr_t *handle_attr)
{
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    handle_item_res_t  *p_item_res;
    setting_uipara_t *uipara = NULL;
    GUI_MEMDEV_Handle   draw_mem;
    _setting_handle_content_res_reset(handle_attr);
    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_LyrWinSel(handle_attr->layer);
    draw_mem = GUI_MEMDEV_Create(uipara->string_detail_pos_ex.x, uipara->string_detail_pos_ex.y, uipara->string_detail_pos_de.w,
                                 uipara->string_detail_pos_ex.h + uipara->string_detail_pos_de.h);
    GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(handle_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    //GUI_LyrWinCacheOn();
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(handle_attr->focus_txt_color);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(handle_attr->string_detail_prompt, &gui_rect, GUI_TA_LEFT);
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(handle_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}

static __s32 setting_handle_item_paint_ex(__lbar_draw_para_t *draw_param)
{
    setting_handle_attr_t *handle_attr = NULL ;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    handle_item_res_t  *p_item_res_old_fcs = NULL, *p_item_res_new_fcs = NULL ;
    setting_uipara_t *uipara = NULL;
    //GUI_MEMDEV_Handle draw_mem;
    handle_attr = (setting_handle_attr_t *)draw_param->attr;

    //__msg( " draw_param->mode = %d \n " , draw_param->mode );
    //__wrn("draw_param->index=%d\n",draw_param->index );
    if (handle_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(handle_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_SetFont(handle_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinCacheOn();
    //draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y,draw_param->rect.width,draw_param->rect.height);
    //GUI_MEMDEV_Select(draw_mem);
    __wrn("------draw_param->index=%d\n", draw_param->index);
    handle_attr->new_focus = draw_param->index;
    /* if(handle_attr->new_focus >= HANDLE_LISTBAR_ITEM_NUM)//..
     {
         handle_attr->new_focus = HANDLE_LISTBAR_ITEM_NUM-1;

     }*/
    _setting_handle_content_res_reset(handle_attr);
    GUI_SetColor(handle_attr->focus_txt_color);
    p_item_res_new_fcs = &handle_attr->res[handle_attr->new_focus];

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            //__wrn( "LBAR_MODE_NORMAL \n" );
            GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_item_unfocus_bmp), draw_param->rect.x, draw_param->rect.y);
            //GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_left),draw_param->rect.x+uipara->left_button.x,draw_param->rect.y+uipara->left_button.y);
            //GUI_BMP_Draw(theme_hdl2buf(p_item_res->h_bmp_right),draw_param->rect.x+uipara->right_button.x,draw_param->rect.y+uipara->right_button.y);
            gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(handle_attr->unfocus_txt_color);
            GUI_DispStringInRect(p_item_res_new_fcs->string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            __wrn("---p_item_res_new_fcs->string_content===%s\n", p_item_res_new_fcs->string_content);
            GUI_DispStringInRect(p_item_res_new_fcs->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            //__wrn( "LBAR_MODE_FOCUS \n" );
            //..setting_handle_paint_item_detail_ex( handle_attr ) ;
            GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_item_focus_bmp), draw_param->rect.x, draw_param->rect.y);
            //..GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_item_focus_bmp),draw_param->rect.x,draw_param->rect.y+45);
            gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(handle_attr->focus_txt_color);
            GUI_DispStringInRect(p_item_res_new_fcs->string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            __wrn("---p_item_res_new_fcs->string_content=%s\n", p_item_res_new_fcs->string_content);
            GUI_DispStringInRect(p_item_res_new_fcs->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        default:
            break;
    }

    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32 setting_handle_paint_item_detail(__gui_msg_t *msg)
{
    setting_handle_attr_t *handle_attr;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    handle_item_res_t  *p_item_res;
    setting_uipara_t *uipara = NULL;
    GUI_MEMDEV_Handle   draw_mem;
    handle_attr = (setting_handle_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (handle_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(handle_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    _setting_handle_content_res_reset(handle_attr);
    uipara = (setting_uipara_t *)setting_get_uipara();
    //__msg( "enter setting_handle_paint_item_detail \n " );
    GUI_LyrWinSel(handle_attr->layer);
    draw_mem = GUI_MEMDEV_Create(uipara->string_detail_pos_ex.x, uipara->string_detail_pos_ex.y, uipara->string_detail_pos_de.w,
                                 uipara->string_detail_pos_ex.h + uipara->string_detail_pos_de.h);
    GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(handle_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    //GUI_LyrWinCacheOn();
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(handle_attr->focus_txt_color);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(handle_attr->string_detail_prompt, &gui_rect, GUI_TA_LEFT);
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(handle_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}

static __s32 setting_handle_paint(__gui_msg_t *msg)
{
    setting_handle_attr_t *handle_attr;
    handle_item_res_t  *p_item_res;
    __s32 i;
    GUI_RECT gui_rect;
    RECT rect;
    setting_uipara_t *uipara = NULL;
    __wrn("-----game handle adjust1 paint\n");
    handle_attr = (setting_handle_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (handle_attr == NULL)
    {
        __wrn("-----game handle adjust2 paint\n");
        return EPDK_FAIL;
    }

    //..if (GUI_LyrWinGetSta(handle_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    //..    return EPDK_FAIL;
    __wrn("-----game handle adjust3 paint\n");
    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_LyrWinSel(handle_attr->layer);
    GUI_SetFont(handle_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff); //80%的透明度
    GUI_SetColor(handle_attr->unfocus_txt_color);
    //GUI_LyrWinCacheOn();
    get_menu_text(STRING_SET_GAME_HANDLE_ADJUST_SET, handle_attr->string_prompt, 128);//set_common(通用设置)
    get_menu_text(setting_handle_res_id[handle_attr->new_focus].id_string_detail_prompt, handle_attr->string_detail_prompt, 128);
    get_menu_text(setting_handle_res_id[handle_attr->new_focus].id_string_detail_prompt_ex, handle_attr->string_detail_prompt_ex, 128);
    //GUI_DispStringAt(handle_attr->string_prompt, handle_attr->handle_uipara->string_prompt_pos.x, handle_attr->handle_uipara->string_prompt_pos.y);
    GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_bg_bmp), 0, 0);
    GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_bmp_big_setting), 1, 0);
    GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_return_unfcs_bmp), uipara->return_bmp.x, uipara->return_bmp.y);
    GUI_DispStringAt(handle_attr->string_prompt, uipara->txt_pos.x, uipara->txt_pos.y);
    return EPDK_OK;
}

static __s32 _setting_handle_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    __s32 last_key = -1;
    setting_handle_attr_t *handle_attr;
    handle_attr = (setting_handle_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    __wrn("msg->dwAddData1 = %d  msg->dwAddData2 = %d\n", msg->dwAddData1, msg->dwAddData2);
#if 0

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                handle_attr->old_focus = handle_attr->new_focus;
                LISTBAR_PrevItem(handle_attr->h_listbar);
                setting_handle_paint_item_detail(msg);
            }

            break;
        }

        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                handle_attr->old_focus = handle_attr->new_focus;
                LISTBAR_NextItem(handle_attr->h_listbar);
                setting_handle_paint_item_detail(msg);
            }

            break;
        }

        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                __wrn("handle_attr->new_focus = %d \n", handle_attr->new_focus);
                handle_attr->old_focus = handle_attr->new_focus;
                LISTBAR_NextItem(handle_attr->h_listbar);
                setting_handle_paint_item_detail(msg);
            }

            break;
        }

        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                handle_attr->old_focus = handle_attr->new_focus;
                LISTBAR_PrevItem(handle_attr->h_listbar);
                setting_handle_paint_item_detail(msg);
            }

            break;
        }

        case GUI_MSG_KEY_ENTER:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                handle_attr->new_focus = LISTBAR_GetFocusItem(handle_attr->h_listbar);
                //setting_handle_enter_action(msg);
                //setting_handle_on_right_button(msg);
            }

            break;
        }

        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_LONGESCAPE:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                if (!handle_attr->handle_para->call_status)
                {
                    __wrn("------game escape\n");
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
                    GUI_FrmWinDelete(msg->h_deswin);
                }
                else
                {
                    handle_attr->handle_adjust_status = HANDLE_ADJUST_COMPLETE;
                }
            }

            break;
        }

        default:
        {
            break;
        }
    }

#endif

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (last_key == GUI_MSG_KEY_ESCAPE)
        {
            if (!handle_attr->handle_para->call_status)
            {
                __wrn("------game escape\n");
                main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
                GUI_FrmWinDelete(msg->h_deswin);
            }
            else
            {
                handle_attr->handle_adjust_status = HANDLE_ADJUST_COMPLETE;
            }
        }

        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return ret;
}

/*
    回调
*/
static __s32 _setting_handle_Proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            setting_handle_attr_t *handle_attr;
            setting_handle_para_t *handle_para;
            setting_uipara_t *uipara = NULL; //..
            GUI_RECT   gui_rect;//..
            char       handle_adjust_string[128];//..
            __wrn("-----setting handle create\n");
            //setting_reg_para = (reg_system_para_t*)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
            handle_para = (setting_handle_para_t *)GUI_WinGetAttr(msg->h_deswin);
            handle_attr = (setting_handle_attr_t *)My_Malloc(0, sizeof(setting_handle_attr_t));

            if (!handle_attr)
            {
                __wrn("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(handle_attr, 0, sizeof(setting_handle_attr_t));
            __wrn("---handle_attr=%08x\n", handle_attr);
            handle_attr->new_focus = handle_para->focus_id;
            handle_attr->old_focus = handle_para->focus_id;
            handle_attr->handle_para = handle_para;
            handle_attr->parent = msg->h_deswin;
            //setting_handle_on_set_content_num(handle_attr);
            _setting_handle_res_init(handle_attr);
            //_setting_handle_content_res_reset(handle_attr);
            handle_attr->focus_txt_color = SETTING_FOCUS_COLOR;
            handle_attr->unfocus_txt_color = SETTING_UNFOCUS_COLOR;
            handle_attr->font = handle_para->font;
            handle_attr->layer = handle_para->layer;
            handle_attr->handle_adjust_thread_id = esKRNL_TCreate(gbxgame_handle_adjust_thread_cb, (void *)handle_attr, 0x10000, KRNL_priolevel3);

            if (handle_attr->handle_adjust_thread_id == NULL)
            {
                __wrn("esKRNL_TCreate  gbxgame_thread_cb fail! \n");
            }

            __wrn("---msg->h_deswin=%08x,handle_attr=%08x\n", msg->h_deswin, handle_attr);
            GUI_WinSetAddData(msg->h_deswin, (__u32)handle_attr);
            __wrn("+++++msg->h_deswin=%08x,handle_attr=%08x\n", msg->h_deswin, handle_attr);
            GUI_LyrWinSetSta(handle_attr->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(handle_attr->layer);
            GUI_LyrWinSel(handle_attr->layer);
            //..GUI_Clear();//..
            {
                //..画左侧列表
                uipara = (setting_uipara_t *)setting_get_uipara();
                GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_left_bg_bmp), 0, 0);
                GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_left_bg_bmp), 0, 1 * uipara->item.h);
                GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_left_bg_bmp), 0, 2 * uipara->item.h);
                GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_left_bg_bmp), 0, 3 * uipara->item.h);
                GUI_BMP_Draw(theme_hdl2buf(handle_attr->h_game_item), 1, 20);
                get_menu_text(STRING_SET_GAME_HANDLE_ADJUST, handle_adjust_string, 128);//set_common(通用设置)
                gui_rect.x0 = 0;
                gui_rect.y0 = 0;
                gui_rect.x1 = 300;
                gui_rect.y1 = 106;
                GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
                GUI_SetFont(SWFFont);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetColor(GUI_BLACK);
                GUI_DispStringAt(handle_adjust_string, 21 + 46 + 10, 30 + 10);
                //..esKRNL_TimeDly(100);//112350
            }
            setting_listbar_init(msg);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            __wrn("-----setting handle close\n");
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            setting_handle_attr_t *handle_attr;
            setting_handle_para_t *handle_para;
            __s32 ret;
            __wrn("GUI_MSG_DESTROY ..............\n");
            handle_attr = (setting_handle_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!handle_attr)
            {
                __err("handle_attr is null ......\n");
                return EPDK_FAIL;
            }

            if (handle_attr->handle_adjust_thread_id != 0)
            {
                while (esKRNL_TDelReq(handle_attr->handle_adjust_thread_id) != OS_TASK_NOT_EXIST)
                {
                    esKRNL_TimeDly(5);
                }

                handle_attr->handle_adjust_thread_id = 0;
                __msg("\n thread has been deleted.\n");
            }

            ret = LISTBAR_SuspendPage(handle_attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar suspend fail\n");
            }

            ret = LISTBAR_Delete(handle_attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar delete fail\n");
            }

            handle_attr->h_listbar = NULL;
            _setting_handle_res_uninit(handle_attr);
            //dsk_reg_flush();  //写进文件里面
            handle_para = handle_attr->handle_para;

            if (handle_para && (!handle_para->call_status))
            {
                USBJoystickPlugOut(NULL);
            }

            if (handle_para)
            {
                My_Mfree(0, handle_para);
            }

            if (handle_attr)
            {
                My_Mfree(0, handle_attr);
            }

            bwait_for_handle_adjudt = EPDK_FALSE;
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            __wrn("-----setting handle paint\n");
            setting_handle_paint(msg);
            setting_handle_paint_item_detail(msg) ;
            return EPDK_OK;

        case GUI_MSG_KEY:
            __wrn("-----setting handle key\n");
            _setting_handle_listbar_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            //setting_handle_touch_proc(msg);////
            //_setting_handle_listbar_touch_proc(msg);
            return EPDK_OK;

        //case MSG_RESTORE_FACTORY:
        //setting_restore_default(msg);
        //return EPDK_OK;

        case MSG_OP_EXIT:
        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __wrn("-----setting handle create OP_EXIT  PART_PLUGOUT\n");
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            setting_handle_attr_t *handle_attr;
            setting_handle_para_t *handle_para;
            __wrn("-----setting handle create command\n");
            handle_attr = (setting_handle_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            handle_attr->old_focus = handle_attr->new_focus;
            LISTBAR_NextItem(handle_attr->h_listbar);
            //..setting_handle_paint_item_detail(msg);
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_handle_win_create(H_WIN h_parent, setting_handle_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_handle_para_t  *handle_para;
    FB fb;
    __s32 ret;
    __inf("*********enter  handle setting win proc *****************\n");
    ret = USBJoystickPlugIn(NULL);
    __wrn("------ret=%d\n", ret);

    if (EPDK_FAIL == ret)
    {
        //无手柄提示
        H_WIN  h_dialog;
        __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_SET_GAME_NO_HANDLE };
        //__wrn("1.......................\n");
        default_dialog(h_dialog, h_parent, HANDLE_ADJUST_DIALOG_ID, ADLG_OK, lang_id);
        __msg("delay 2s .....................\n");
        esKRNL_TimeDly(200);
        __msg("delay end .....................\n");

        if (h_dialog)
        {
            __msg("app_dialog_destroy 2s .....................\n");
            app_dialog_destroy(h_dialog);
            h_dialog = NULL;
        }

        __msg("return .....................\n");
        return NULL;
    }

    __msg(" USBJoystickPlugIn ok.....................\n");
    GUI_LyrWinGetFB(para->layer, &fb);
    handle_para = (setting_handle_para_t *)My_Malloc(0, sizeof(setting_handle_para_t));

    if (!handle_para)
    {
        __msg("memory alloc fail.........\n");
        return EPDK_FAIL;
    }

    eLIBs_memset((void *)handle_para, 0, sizeof(setting_handle_para_t));
    handle_para->font = para->font;
    handle_para->layer = para->layer;
    handle_para->focus_id = para->focus_id;
    handle_para->call_status = para->call_status;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "handle setting win",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = HANDLE_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_handle_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)handle_para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 setting_handle_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}

#endif
