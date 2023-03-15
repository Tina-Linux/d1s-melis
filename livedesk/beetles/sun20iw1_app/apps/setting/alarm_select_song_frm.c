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
#include"alarm_select_song_frm.h"
#include"alarm_select_song_uipara.h"

#define BK_COLOR    0xB3000000
#define ALARM_LISTEN_MUSIC_MONITOR_INTERVAL     60

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

static  __s32   ALARM_LISTEN_MUSIC_TIMER = 0xff ;
typedef enum
{
    disk_name_tf = 0,
    disk_name_usb,
    disk_name_max
} disk_name_e ;

typedef struct disk_info_s
{
    disk_name_e disk_index ;
    char        disk_name[32] ;
} disk_info_t ;

static disk_info_t disk_info_array[disk_name_max] =
{
    { disk_name_tf, "SD" },
    { disk_name_usb, "USB" }
};

static __s32 alarm_sel_song_get_item_full_path(alarm_sel_song_t  *para, __s32 ItemIndex, char *FilePath);
void  alarm_sel_song_cmd2parent(H_WIN hwin, __s32 id, signed long data1, signed long data2)
{
    __gui_msg_t msg;
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_SendNotifyMessage(&msg);
}


void alarm_sel_song_playmusic(__gui_msg_t *msg)
{
    alarm_sel_song_t *alarm_sel_song_para   ;
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(msg->h_deswin);
    eLIBs_memset(alarm_sel_song_para->FullFileName, 0, sizeof(alarm_sel_song_para->FullFileName));
    alarm_sel_song_get_item_full_path(alarm_sel_song_para, alarm_sel_song_para->rat.index, alarm_sel_song_para->FullFileName);
    __msg("alarm_sel_song_para->FullFileName=%s\n", alarm_sel_song_para->FullFileName);
    robin_play_file(alarm_sel_song_para->FullFileName, NULL);

    if (GUI_IsTimerInstalled(msg->h_deswin, ALARM_LISTEN_MUSIC_TIMER))
    {
        GUI_ResetTimer(msg->h_deswin, ALARM_LISTEN_MUSIC_TIMER, ALARM_LISTEN_MUSIC_MONITOR_INTERVAL, NULL);
    }
    else
    {
        GUI_SetTimer(msg->h_deswin, ALARM_LISTEN_MUSIC_TIMER, ALARM_LISTEN_MUSIC_MONITOR_INTERVAL, NULL);
    }
}

rat_type_t  app_alarm_sel_song_get_root_type(__u32 *index)
{
    __s32 ret = 0 ;
    __s32 count = 0;
    __s32 len = 0 ;
    char disk_name[RAT_MAX_PARTITION][4];
    rat_type_t root_type ;
    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
    ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
    len = eLIBs_strlen(disk_name[0]);

    if ((EPDK_OK == ret) && (len != 0)) // 磁盘存在
    {
        root_type = RAT_USB ;
        count++;
    }

    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
    ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
    len = eLIBs_strlen(disk_name[0]);

    if ((EPDK_OK == ret) && (len != 0))
    {
        root_type = RAT_TF ;
        count++;
    }

    *index = count ;

    if (2 == count)  // 有两个磁盘
    {
        root_type = RAT_UNKNOWN ;
    }

    return root_type ;
}

__s32 alarm_sel_song_long_string_init(alarm_sel_song_t *alarm_sel_song_para)
{
    if (alarm_sel_song_para->long_str_handle == NULL)
    {
        __show_info_move_t show_info;
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = alarm_sel_song_para->layer;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = NULL;
        show_info.string = "";
        show_info.fontColor = GUI_WHITE;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = NULL ;
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;
        alarm_sel_song_para->long_str_handle = GUI_LongStringCreate(&show_info);

        if (alarm_sel_song_para->long_str_handle == NULL)
        {
            __wrn("create long string failed!\n");
        }
    }
    else
    {
        __msg("long string scroll has already started!\n");
    }

    return EPDK_OK;
}


__s32 alarm_sel_song_long_string_uninit(alarm_sel_song_t *alarm_sel_song_para)
{
    if (alarm_sel_song_para->long_str_handle != NULL)
    {
        GUI_LongStringDelete(alarm_sel_song_para->long_str_handle);
        alarm_sel_song_para->long_str_handle = NULL;
    }

    return EPDK_OK;
}

__s32 alarm_sel_song_long_string_roll(__lbar_draw_para_t *draw_param, char *string)
{
    __show_info_move_t show_info;
    __epdk_charset_enm_e encode = EPDK_CHARSET_ENM_UTF8;
    RECT roll_rect, text_rect;
    alarm_sel_song_uipara_t *uipara ;
    alarm_sel_song_t *alarm_sel_song_para ;
    __u32 font_size = 0;
    alarm_sel_song_para = (alarm_sel_song_t *)draw_param->attr;
    uipara = alarm_sel_song_get_uipara();

    if (alarm_sel_song_para->long_str_handle != NULL)
    {
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        text_rect.x = uipara->ListBarItemRect.x ;
        text_rect.y = uipara->ListBarItemRect.y ;
        text_rect.width = uipara->ListBarItemRect.w ;
        text_rect.height = uipara->ListBarItemRect.h ;
        roll_rect.x = draw_param->rect.x  + text_rect.x ;
        roll_rect.y = draw_param->rect.y  + text_rect.y ;
        roll_rect.width = text_rect.width ;
        roll_rect.height = text_rect.height ;
        font_size = GUI_GetFontDistY() ;
        show_info.hlayer = alarm_sel_song_para->layer;
        show_info.region.x = roll_rect.x;
        show_info.region.y = roll_rect.y;
        show_info.region.width = roll_rect.width - 1 ;
        show_info.region.height = roll_rect.height - 1 ;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = alarm_sel_song_para->font;
        show_info.string = string;
        show_info.encode_id = encode;
        show_info.fontColor = GUI_BLACK;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = theme_hdl2buf(alarm_sel_song_para->h_list_item_bg);
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0 ;
        GUI_LongStringStop(alarm_sel_song_para->long_str_handle);
        GUI_LongStringReset(alarm_sel_song_para->long_str_handle, &show_info);
        GUI_LongStringStart(alarm_sel_song_para->long_str_handle);
    }

    return EPDK_OK;
}

static __s32 alarm_sel_song_long_string_stop_roll(alarm_sel_song_t *alarm_sel_song_para)
{
    if (alarm_sel_song_para->long_str_handle != NULL)
    {
        return GUI_LongStringStop(alarm_sel_song_para->long_str_handle);
    }

    return EPDK_FAIL;
}

__s32 alarm_sel_song_paint_next_item(alarm_sel_song_t *alarm_sel_song_para)
{
    __listbar_scene_t ListBarScene;
    LISTBAR_NextItem(alarm_sel_song_para->listbar_handle);
    LISTBAR_GetScene(alarm_sel_song_para->listbar_handle, &ListBarScene);

    if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
    {
        alarm_sel_song_para->current_sel_disk_index = ListBarScene.focus_id;
    }
    else
    {
        alarm_sel_song_para->rat.index = ListBarScene.focus_id;
    }

    return EPDK_OK;
}

__s32 alarm_sel_song_paint_prev_item(alarm_sel_song_t *alarm_sel_song_para)
{
    __listbar_scene_t ListBarScene;
    LISTBAR_PrevItem(alarm_sel_song_para->listbar_handle);
    LISTBAR_GetScene(alarm_sel_song_para->listbar_handle, &ListBarScene);

    if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
    {
        alarm_sel_song_para->current_sel_disk_index = ListBarScene.focus_id;
    }
    else
    {
        alarm_sel_song_para->rat.index = ListBarScene.focus_id;
    }

    return EPDK_OK;
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

void  alarm_sel_song_get_search_path(alarm_sel_song_t  *para)
{
    __s32 ret = 0;
    char root_type[50];
    char disk_name[RAT_MAX_PARTITION][4];
    eLIBs_memset(root_type, 0, sizeof(root_type));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    eLIBs_memset(para->search_path, 0, sizeof(para->search_path));

    switch (para->root_type)
    {
        case RAT_LOCAL:
            eLIBs_strcpy(root_type, RAT_LOCAL_DISK);
            break;

        case RAT_TF:
            eLIBs_strcpy(root_type, RAT_SD_CARD);
            break;

        case RAT_USB:
            eLIBs_strcpy(root_type, RAT_USB_DISK);
            break;

        default:
            break;
    }

    __msg("root_type=%s\n", root_type);
    ret = rat_get_partition_name(root_type, disk_name, 0);
    eLIBs_strcpy(para->search_path, disk_name[0]);
    __msg("para->search_path=%s\n", para->search_path);
    return ;
}

static __s32 alarm_sel_song_extract_name_from_full_path(char *FullFilePath, char **FileName)
{
    *FileName = eLIBs_strchrlast(FullFilePath, '\\') + 1 ;
    return EPDK_OK;
}

static __s32 alarm_sel_song_get_item_full_path(alarm_sel_song_t  *para, __s32 ItemIndex, char *FilePath)
{
    int ret;
    rat_entry_t ItemInfo;
    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
    ret = rat_get_item_info_by_index(para->rat.handle, ItemIndex, &ItemInfo);

    if (ret == EPDK_FAIL)
    {
        __wrn("get file information form rat fail!!");
        return EPDK_FAIL;
    }

    eLIBs_strcpy(FilePath, ItemInfo.Path);
    return EPDK_OK;
}

static __s32 alarm_sel_song_draw_item_text(__lbar_draw_para_t *draw_param)
{
    int ret;
    alarm_sel_song_t  *alarm_sel_song_para ;
    char FileName[RAT_MAX_FULL_PATH_LEN] = {0};
    GUI_RECT GUI_TextRect;
    RECT TextRect;
    alarm_sel_song_uipara_t *uipara ;
    GUI_MEMDEV_Handle   draw_mem;
    char disk_name[32] = { 0 } ;
    char *p_file_name = NULL ;
    alarm_sel_song_para = (alarm_sel_song_t *)draw_param->attr;
    GUI_SetFont(alarm_sel_song_para->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_WHITE);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    uipara = alarm_sel_song_get_uipara();
    TextRect.x = uipara->ListBarItemRect.x ;
    TextRect.y = uipara->ListBarItemRect.y ;
    TextRect.width = uipara->ListBarItemRect.w ;
    TextRect.height = uipara->ListBarItemRect.h  ;
    GUI_TextRect.x0 = draw_param->rect.x  + TextRect.x;
    GUI_TextRect.y0 = draw_param->rect.y  + TextRect.y;
    GUI_TextRect.x1 = GUI_TextRect.x0 + TextRect.width - 1 ;
    GUI_TextRect.y1 = GUI_TextRect.y0 + TextRect.height - 1 ;
    draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y, uipara->ListBarItemRect.w, uipara->ListBarItemRect.h);
    GUI_MEMDEV_Select(draw_mem);

    if (0 == alarm_sel_song_para->is_in_sel_disk_scene)
    {
        ret = alarm_sel_song_get_item_full_path(alarm_sel_song_para, draw_param->index, FileName);

        if (ret == EPDK_FAIL)
        {
            __wrn("get file information from rat fail!!");
            return EPDK_FAIL;
        }

        p_file_name = eLIBs_strchrlast(FileName, '\\') + 1 ;

        if (draw_param->mode == LBAR_MODE_FOCUS)
        {
            GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_list_item_bg), GUI_TextRect.x0 - 1, GUI_TextRect.y0);
        }

        //__here__ ;
        __msg("%s\n", p_file_name);
        GUI_DispStringInRect(p_file_name, &GUI_TextRect, GUI_TA_LEFT | GUI_TA_VCENTER);
        //__here__ ;
    }
    else
    {
        eLIBs_strcpy(disk_name, disk_info_array[draw_param->index].disk_name);

        if (draw_param->mode == LBAR_MODE_FOCUS)
        {
            GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_list_item_bg), GUI_TextRect.x0 - 1, GUI_TextRect.y0);
        }

        GUI_DispStringInRect(disk_name, &GUI_TextRect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    if ((draw_param->mode == LBAR_MODE_FOCUS) && (0 == alarm_sel_song_para->is_in_sel_disk_scene))
    {
        alarm_sel_song_long_string_roll(draw_param, p_file_name);
    }

    return EPDK_OK;
}


static __s32 alarm_sels_song_listbar_item_draw(__lbar_draw_para_t *draw_param)
{
    alarm_sel_song_t  *alarm_sel_song_para ;
    alarm_sel_song_para = (alarm_sel_song_t *)draw_param->attr;

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
            alarm_sel_song_draw_item_text(draw_param);
            break ;

        case LBAR_MODE_FOCUS:
            alarm_sel_song_draw_item_text(draw_param);

            if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
            {
                alarm_sel_song_para->current_sel_disk_index = draw_param->index ;
            }
            else
            {
                alarm_sel_song_para->rat.index = draw_param->index ;
            }

            break ;

        case LBAR_MODE_INVALID:
            break ;

        case LBAR_MODE_VACANT:
            break ;

        case LBAR_MODE_DIFFUSE:
            break;

        default:
            break ;
    }

    return EPDK_OK;
}

static __s32  alarm_sel_song_listbar_init(H_WIN  list_win)
{
    alarm_sel_song_t *alarm_sel_song_para ;
    __listbar_config_t config;
    alarm_sel_song_uipara_t *uipara ;
    uipara = alarm_sel_song_get_uipara();
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(list_win);
    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    config.item_width = uipara->ListBarItemRect.w ;
    config.item_height = uipara->ListBarItemRect.h ;
    config.list_rect.x = uipara->ListBarAreaRect.x ;
    config.list_rect.y = uipara->ListBarAreaRect.y ;
    config.list_rect.width = uipara->ListBarAreaRect.w ;
    config.list_rect.height = uipara->ListBarAreaRect.h ;
    config.start_id = 0;
    config.focus_id = 0;

    if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
    {
        config.item_cnt = disk_name_max ;
    }
    else
    {
        config.item_cnt = alarm_sel_song_para->rat.total;   //Rat.total必须已经获得
    }

    config.bk_color = BK_COLOR;
    config.alpha_status = 0 ;
    config.list_attr = (void *)alarm_sel_song_para;
    alarm_sel_song_para->listbar_handle = LISTBAR_Create(list_win);
    LISTBAR_Config(alarm_sel_song_para->listbar_handle, alarm_sels_song_listbar_item_draw, &config);
    return EPDK_OK ;
}

static __s32 alarm_sel_song_listbar_uninit(H_WIN  list_win)
{
    alarm_sel_song_t *alarm_sel_song_para ;
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(list_win);

    if (alarm_sel_song_para->listbar_handle != NULL)
    {
        LISTBAR_SuspendPage(alarm_sel_song_para->listbar_handle);
        LISTBAR_Delete(alarm_sel_song_para->listbar_handle);
        alarm_sel_song_para->listbar_handle = NULL;
    }

    return EPDK_OK ;
}

static __s32 alarm_sel_song_rat_init(alarm_sel_song_t *alarm_sel_song_para)
{
    if (alarm_sel_song_para->rat.handle == NULL)
    {
        //alarm_sel_song_para->rat.handle = rat_open(alarm_sel_song_para->search_path, alarm_sel_song_para->media_type, 0);
        alarm_sel_song_para->rat.handle = rat_open_ex(alarm_sel_song_para->root_type, alarm_sel_song_para->media_type, 0);

        if (alarm_sel_song_para->rat.handle == NULL)
        {
            __wrn("open rat failed\n");
        }
        else
        {
            __msg("open rat successed\n");
        }

        alarm_sel_song_para->rat.index = 0;
        alarm_sel_song_para->rat.total = rat_get_cur_scan_cnt(alarm_sel_song_para->rat.handle);;
    }

    return EPDK_OK;
}

static __s32 alarm_sel_song_rat_uninit(alarm_sel_song_t *alarm_sel_song_para)
{
    if (alarm_sel_song_para->rat.handle != NULL)
    {
        alarm_sel_song_para->rat.handle = NULL ;
    }

    return EPDK_OK;
}

static __s32 alarm_sel_song_win_on_create(H_WIN  list_win)
{
    alarm_sel_song_t *alarm_sel_song_para   ;
    rat_type_t  root_type ;
    __u32 disk_num ;
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(list_win);

    if (!alarm_sel_song_para)
    {
        __msg("alarm select song win get attribute fail \n");
        return EPDK_FAIL ;
    }

    alarm_sel_song_para->h_button_fc_bmp = theme_open(ID_SETTING_BUTTON_FC_BMP);
    alarm_sel_song_para->h_button_uf_bmp = theme_open(ID_SETTING_BUTTON_UF_BMP);
    alarm_sel_song_para->h_list_item_bg = theme_open(ID_SETTING_LIST_ITEM_FOCUS_BMP);
    alarm_sel_song_para->button_fcs_id = BUTTON_FCS_OK ;
    root_type = app_alarm_sel_song_get_root_type(&disk_num);
    __msg("root_type = %d\n", root_type);
    __msg("disk_num = %d\n", disk_num);
    alarm_sel_song_para->media_type = RAT_MEDIA_TYPE_AUDIO ;

    if (2 == disk_num)   // sd ,udisk 同时存在
    {
        alarm_sel_song_para->is_in_sel_disk_scene = 1 ;
    }
    else
    {
        alarm_sel_song_para->is_in_sel_disk_scene = 0 ;
        alarm_sel_song_para->root_type = root_type ;
        alarm_sel_song_get_search_path(alarm_sel_song_para);
        alarm_sel_song_rat_init(alarm_sel_song_para);
    }

    //__here__ ;
    alarm_sel_song_long_string_init(alarm_sel_song_para);
    //__here__ ;
    alarm_sel_song_listbar_init(list_win);
    //__here__ ;

    if (EPDK_TRUE == robin_is_open())     //背景音存在时，robin已经打开了
    {
        //__here__ ;
        alarm_sel_song_para->bg_music_play_mode = robin_get_play_mode();
        alarm_sel_song_para->bg_music_is_exist = 1 ;
        alarm_sel_song_para->current_npl_index = robin_npl_get_cur();
    }
    else
    {
        //__here__ ;
        robin_open(ROBIN_MODE_AUDIO_MIN, NULL);
        alarm_sel_song_para->bg_music_is_exist = 0 ;
    }

    robin_set_play_mode(RAT_PLAY_MODE_ROTATE_ONE);
    GUI_LyrWinSetSta(alarm_sel_song_para->layer, GUI_LYR_STA_ON);
    GUI_LyrWinSetTop(alarm_sel_song_para->layer);
    //__here__ ;
    LISTBAR_ShowPage(alarm_sel_song_para->listbar_handle);
    //__here__ ;
    GUI_WinSetCaptureWin(list_win);
    __msg("alarm_sel_song_win_on_create end \n");
    return EPDK_OK ;
}



static __s32 alarm_sel_song_win_paint_button(H_WIN  list_win)
{
    alarm_sel_song_t *alarm_sel_song_para   ;
    alarm_sel_song_uipara_t *uipara ;
    GUI_RECT gui_rect ;
    char ok_txt[128] = {0}, listen_txt[128] = {0}, cancle_txt[128] = {0} ;

    if ((__s32)GUI_LYR_STA_ON != (__s32)GUI_LyrWinGetSta(GUI_WinGetLyrWin(list_win)))
    {
        __msg(" alarm select song layer not on GUI_LYR_STA_ON state \n");
        return EPDK_FAIL
               ;
    }

    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(list_win);

    if (!alarm_sel_song_para)
    {
        __msg("alarm select song win get attribute fail \n");
        return EPDK_FAIL ;
    }

    GUI_SetFont(alarm_sel_song_para->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(BK_COLOR);
    GUI_SetColor(GUI_WHITE);
    GUI_LyrWinSel(alarm_sel_song_para->layer);
    uipara = alarm_sel_song_get_uipara();

    if (BUTTON_FCS_OK == alarm_sel_song_para->button_fcs_id)
    {
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_fc_bmp), uipara->OkBmpRect.x, uipara->OkBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_OK, ok_txt, sizeof(ok_txt));
        gui_rect.x0 = uipara->OkBmpRect.x ;
        gui_rect.y0 = uipara->OkBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->OkBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->OkBmpRect.h - 1 ;
        GUI_DispStringInRect(ok_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_uf_bmp), uipara->ListenTestBmpRect.x, uipara->ListenTestBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_EXPLORER_LISTEN, listen_txt, sizeof(listen_txt));
        gui_rect.x0 = uipara->ListenTestBmpRect.x ;
        gui_rect.y0 = uipara->ListenTestBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->ListenTestBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->ListenTestBmpRect.h - 1 ;
        GUI_DispStringInRect(listen_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_uf_bmp), uipara->CancleBmpRect.x, uipara->CancleBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_CANCLE, cancle_txt, sizeof(cancle_txt));
        gui_rect.x0 = uipara->CancleBmpRect.x ;
        gui_rect.y0 = uipara->CancleBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->CancleBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->CancleBmpRect.h - 1 ;
        GUI_DispStringInRect(cancle_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else if (BUTTON_FCS_LISTEN == alarm_sel_song_para->button_fcs_id)
    {
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_uf_bmp), uipara->OkBmpRect.x, uipara->OkBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_OK, ok_txt, sizeof(ok_txt));
        gui_rect.x0 = uipara->OkBmpRect.x ;
        gui_rect.y0 = uipara->OkBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->OkBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->OkBmpRect.h - 1 ;
        GUI_DispStringInRect(ok_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_fc_bmp), uipara->ListenTestBmpRect.x, uipara->ListenTestBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_EXPLORER_LISTEN, listen_txt, sizeof(listen_txt));
        gui_rect.x0 = uipara->ListenTestBmpRect.x ;
        gui_rect.y0 = uipara->ListenTestBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->ListenTestBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->ListenTestBmpRect.h - 1 ;
        GUI_DispStringInRect(listen_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_uf_bmp), uipara->CancleBmpRect.x, uipara->CancleBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_CANCLE, cancle_txt, sizeof(cancle_txt));
        gui_rect.x0 = uipara->CancleBmpRect.x ;
        gui_rect.y0 = uipara->CancleBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->CancleBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->CancleBmpRect.h - 1 ;
        GUI_DispStringInRect(cancle_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else if (BUTTON_FCS_CANCLE == alarm_sel_song_para->button_fcs_id)
    {
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_uf_bmp), uipara->OkBmpRect.x, uipara->OkBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_OK, ok_txt, sizeof(ok_txt));
        gui_rect.x0 = uipara->OkBmpRect.x ;
        gui_rect.y0 = uipara->OkBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->OkBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->OkBmpRect.h - 1 ;
        GUI_DispStringInRect(ok_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_uf_bmp), uipara->ListenTestBmpRect.x, uipara->ListenTestBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_EXPLORER_LISTEN, listen_txt, sizeof(listen_txt));
        gui_rect.x0 = uipara->ListenTestBmpRect.x ;
        gui_rect.y0 = uipara->ListenTestBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->ListenTestBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->ListenTestBmpRect.h - 1 ;
        GUI_DispStringInRect(listen_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_BMP_Draw(theme_hdl2buf(alarm_sel_song_para->h_button_fc_bmp), uipara->CancleBmpRect.x, uipara->CancleBmpRect.y);
        dsk_langres_get_menu_text(STRING_SET_CANCLE, cancle_txt, sizeof(cancle_txt));
        gui_rect.x0 = uipara->CancleBmpRect.x ;
        gui_rect.y0 = uipara->CancleBmpRect.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->CancleBmpRect.w - 1 ;
        gui_rect.y1 = gui_rect.y0 + uipara->CancleBmpRect.h - 1 ;
        GUI_DispStringInRect(cancle_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    return EPDK_OK ;
}

static __s32 alarm_sel_song_win_on_paint(H_WIN  list_win)
{
    alarm_sel_song_t *alarm_sel_song_para   ;
    GUI_MEMDEV_Handle   draw_mem ;
    alarm_sel_song_uipara_t *uipara ;
    GUI_RECT gui_rect ;
    char ok_txt[128] = {0}, cancle_txt[128] = {0} ;

    if ((__s32)GUI_LYR_STA_ON != (__s32)GUI_LyrWinGetSta(GUI_WinGetLyrWin(list_win)))
    {
        __msg(" alarm select song layer not on GUI_LYR_STA_ON state ,return  \n");
        return EPDK_FAIL
               ;
    }

    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(list_win);

    if (!alarm_sel_song_para)
    {
        __msg("alarm select song win get attribute fail \n");
        return EPDK_FAIL ;
    }

    GUI_SetFont(alarm_sel_song_para->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(BK_COLOR);
    GUI_SetColor(GUI_WHITE);
    GUI_LyrWinSel(alarm_sel_song_para->layer);
    uipara = alarm_sel_song_get_uipara();
    GUI_Clear();
    draw_mem = GUI_MEMDEV_Create(uipara->FbRect.x, uipara->FbRect.y, uipara->FbRect.w, uipara->FbRect.h);

    if (!draw_mem)
    {
        __wrn("alarm select song win create memory device fail\n");
    }

    GUI_MEMDEV_Select(draw_mem);
    alarm_sel_song_win_paint_button(list_win);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK ;
}

static __s32 alarm_sel_song_on_listbar_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 ret = EPDK_FAIL ;
    static __s32 is_touch_down = EPDK_FALSE ;
    static __s32 is_touch_moving = EPDK_FALSE ;
    alarm_sel_song_t *alarm_sel_song_para   ;
    RECT    rect ;
    alarm_sel_song_uipara_t *uipara ;
    uipara = alarm_sel_song_get_uipara();
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(msg->h_deswin);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);
    rect.x = uipara->ListBarAreaRect.x ;
    rect.y = uipara->ListBarAreaRect.y ;
    rect.width = uipara->ListBarAreaRect.w ;
    rect.height = uipara->ListBarAreaRect.h ;
    __msg("rect.x = %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        {
            ret = position_in_rect(&rect, x, y);

            if (EPDK_TRUE == ret)
            {
                __msg("listbar get touch down message \n");

                if (LBAR_STATE_MOVING == LISTBAR_GetState(alarm_sel_song_para->listbar_handle))
                {
                    __msg(" receive GUI_MSG_TOUCH_DOWN message , listbar is moving , break \n");
                    break ;
                }

                alarm_sel_song_long_string_stop_roll(alarm_sel_song_para);
                ret = LISTBAR_TouchDown(alarm_sel_song_para->listbar_handle, x, y, direct, speed);

                if (EPDK_OK == ret)
                {
                    __msg(" LISTBAR_TouchDown succeed \n ");
                    is_touch_down = EPDK_TRUE ;
                    is_touch_moving = EPDK_FALSE ;
                }
            }

            break;
        }

        case GUI_MSG_TOUCH_MOVE:
        {
            __msg("listbar get touch move message \n");

            if (is_touch_down == EPDK_TRUE)
            {
                //__here__ ;
                alarm_sel_song_long_string_stop_roll(alarm_sel_song_para);
                ret = LISTBAR_TouchMove(alarm_sel_song_para->listbar_handle, x, y, direct, speed);

                if (EPDK_OK == ret)
                {
                    is_touch_moving = EPDK_TRUE ;
                }
            }

            break;
        }

        case GUI_MSG_TOUCH_UP:
        {
            __msg("listbar get touch up message \n");

            if (is_touch_down == EPDK_FALSE)
            {
                __msg("is_touch_down == EPDK_FALSE \n");
                break;
            }
            else
            {
                is_touch_down = EPDK_FALSE ;
            }

            LISTBAR_TouchUp(alarm_sel_song_para->listbar_handle, x, y, direct, speed);
        }

        default:
        {
            break;
        }
    }

    return ret;
}

static __s32 alarm_sel_song_on_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 ret = EPDK_FAIL ;
    alarm_sel_song_t *alarm_sel_song_para   ;
    RECT    rect ;
    alarm_sel_song_uipara_t *uipara ;
    uipara = alarm_sel_song_get_uipara();
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(msg->h_deswin);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        {
            rect.x = uipara->OkBmpRect.x ;
            rect.y = uipara->OkBmpRect.y ;
            rect.width = uipara->OkBmpRect.w ;
            rect.height = uipara->OkBmpRect.h ;
            ret = position_in_rect(&rect, x, y);     //判断是否在确定按钮区域

            if (EPDK_TRUE == ret)
            {
                alarm_sel_song_para->button_fcs_id = BUTTON_FCS_OK ;
                alarm_sel_song_win_paint_button(msg->h_deswin);

                if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
                {
                    alarm_sel_song_para->is_in_sel_disk_scene = 0 ;
                    __msg("alarm_sel_song_para->current_sel_disk_index=%d\n", alarm_sel_song_para->current_sel_disk_index);

                    if (disk_name_tf == alarm_sel_song_para->current_sel_disk_index)
                    {
                        //__here__ ;
                        alarm_sel_song_para->root_type = RAT_TF ;
                        alarm_sel_song_get_search_path(alarm_sel_song_para);
                        alarm_sel_song_rat_init(alarm_sel_song_para);
                        alarm_sel_song_listbar_uninit(msg->h_deswin);
                        alarm_sel_song_listbar_init(msg->h_deswin);
                        LISTBAR_ShowPage(alarm_sel_song_para->listbar_handle) ;
                    }
                    else if (disk_name_usb == alarm_sel_song_para->current_sel_disk_index)
                    {
                        //__here__ ;
                        alarm_sel_song_para->root_type = RAT_USB ;
                        alarm_sel_song_get_search_path(alarm_sel_song_para);
                        alarm_sel_song_rat_init(alarm_sel_song_para);
                        alarm_sel_song_listbar_uninit(msg->h_deswin);
                        alarm_sel_song_listbar_init(msg->h_deswin);
                        LISTBAR_ShowPage(alarm_sel_song_para->listbar_handle) ;
                    }
                    else
                    {
                        //__here__ ;
                        alarm_sel_song_para->root_type = RAT_UNKNOWN ;
                    }
                }
                else
                {
                    alarm_sel_song_get_item_full_path(alarm_sel_song_para, alarm_sel_song_para->rat.index, alarm_sel_song_para->FullFileName);
                    alarm_sel_song_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_VIEW, 0, (signed long)(alarm_sel_song_para->FullFileName));
                }

                break ;
            }

            rect.x = uipara->ListenTestBmpRect.x ;
            rect.y = uipara->ListenTestBmpRect.y ;
            rect.width = uipara->ListenTestBmpRect.w ;
            rect.height = uipara->ListenTestBmpRect.h ;
            ret = position_in_rect(&rect, x, y);     // 判断是否在试听按钮区域内

            if (EPDK_TRUE == ret)
            {
                alarm_sel_song_para->button_fcs_id = BUTTON_FCS_LISTEN;
                alarm_sel_song_win_paint_button(msg->h_deswin);

                if (0 == alarm_sel_song_para->is_in_sel_disk_scene)
                {
                    alarm_sel_song_para->is_listening_music = 1 ;
                    alarm_sel_song_playmusic(msg);
                }
            }

            rect.x = uipara->CancleBmpRect.x ;
            rect.y = uipara->CancleBmpRect.y ;
            rect.width = uipara->CancleBmpRect.w ;
            rect.height = uipara->CancleBmpRect.h ;
            ret = position_in_rect(&rect, x, y);     // 判断是否在取消按钮区域

            if (EPDK_TRUE == ret)
            {
                if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
                {
                    //__here__ ;
                    alarm_sel_song_para->button_fcs_id = BUTTON_FCS_CANCLE ;
                    alarm_sel_song_win_paint_button(msg->h_deswin);
                    alarm_sel_song_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_VIEW, 0, 0);
                }
                else
                {
                    //__here__ ;
                    alarm_sel_song_para->button_fcs_id = BUTTON_FCS_CANCLE ;
                    alarm_sel_song_win_paint_button(msg->h_deswin);
                    alarm_sel_song_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_VIEW, 0, 0);
                }

                break ;
            }

            alarm_sel_song_on_listbar_touch_proc(msg);
            break;
        }

        case GUI_MSG_TOUCH_MOVE:
            alarm_sel_song_on_listbar_touch_proc(msg);
            break ;

        case GUI_MSG_TOUCH_UP:
            alarm_sel_song_on_listbar_touch_proc(msg);
            break;

        default:
            break;
    }

    return EPDK_OK ;
}


static __s32  alarm_sel_song_on_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    __s32   ret = EPDK_FALSE ;
    alarm_sel_song_t *alarm_sel_song_para   ;
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
                __msg("----alarm select song window receive down key----\n");
                alarm_sel_song_long_string_stop_roll(alarm_sel_song_para);
                alarm_sel_song_paint_next_item(alarm_sel_song_para);
                break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
                __msg("----alarm select song window receive up key----\n");
                alarm_sel_song_long_string_stop_roll(alarm_sel_song_para);
                alarm_sel_song_paint_prev_item(alarm_sel_song_para);
                break;

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
                if (BUTTON_FCS_MAX == alarm_sel_song_para->button_fcs_id)
                {
                    alarm_sel_song_para->button_fcs_id = BUTTON_FCS_OK ;
                }
                else
                {
                    alarm_sel_song_para->button_fcs_id++ ;
                }

                alarm_sel_song_win_paint_button(msg->h_deswin);
                break ;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
                if (BUTTON_FCS_OK == alarm_sel_song_para->button_fcs_id)
                {
                    alarm_sel_song_para->button_fcs_id = BUTTON_FCS_MAX ;
                }
                else
                {
                    alarm_sel_song_para->button_fcs_id-- ;
                }

                alarm_sel_song_win_paint_button(msg->h_deswin);
                break ;

            case GUI_MSG_KEY_ENTER:
                if (BUTTON_FCS_OK == alarm_sel_song_para->button_fcs_id)
                {
                    alarm_sel_song_para->button_fcs_id = BUTTON_FCS_OK ;
                    alarm_sel_song_win_paint_button(msg->h_deswin);

                    if (1 == alarm_sel_song_para->is_in_sel_disk_scene)
                    {
                        alarm_sel_song_para->is_in_sel_disk_scene = 0 ;

                        if (disk_name_tf == alarm_sel_song_para->current_sel_disk_index)
                        {
                            alarm_sel_song_para->root_type = RAT_TF ;
                            alarm_sel_song_get_search_path(alarm_sel_song_para);
                            alarm_sel_song_rat_init(alarm_sel_song_para);
                            alarm_sel_song_listbar_uninit(msg->h_deswin);
                            alarm_sel_song_listbar_init(msg->h_deswin);
                            LISTBAR_ShowPage(alarm_sel_song_para->listbar_handle) ;
                        }
                        else if (disk_name_usb == alarm_sel_song_para->current_sel_disk_index)
                        {
                            alarm_sel_song_para->root_type = RAT_USB ;
                            alarm_sel_song_get_search_path(alarm_sel_song_para);
                            alarm_sel_song_rat_init(alarm_sel_song_para);
                            alarm_sel_song_listbar_uninit(msg->h_deswin);
                            alarm_sel_song_listbar_init(msg->h_deswin);
                            LISTBAR_ShowPage(alarm_sel_song_para->listbar_handle) ;
                        }
                        else
                        {
                            alarm_sel_song_para->root_type = RAT_UNKNOWN ;
                        }
                    }
                    else
                    {
                        alarm_sel_song_get_item_full_path(alarm_sel_song_para, alarm_sel_song_para->rat.index, alarm_sel_song_para->FullFileName);
                        alarm_sel_song_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_VIEW, 0, (signed long)(alarm_sel_song_para->FullFileName));
                    }
                }
                else if (BUTTON_FCS_LISTEN == alarm_sel_song_para->button_fcs_id)
                {
                    if (0 == alarm_sel_song_para->is_in_sel_disk_scene)
                    {
                        alarm_sel_song_para->is_listening_music = 1 ;
                        alarm_sel_song_playmusic(msg);
                    }
                }
                else
                {
                    alarm_sel_song_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_VIEW, 0, 0);
                }

                break;

            case GUI_MSG_KEY_VADD:
                break;

            case GUI_MSG_KEY_VDEC:
                break;

            case GUI_MSG_KEY_MENU:
                break;

            case GUI_MSG_KEY_RISE:
                break;

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1 ;
    }
    else
    {
        last_key = msg->dwAddData1 ;
    }

    return EPDK_OK;
}


static __s32  alarm_sel_song_win_on_destroy(__gui_msg_t *msg)
{
    alarm_sel_song_t *alarm_sel_song_para ;
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!alarm_sel_song_para)
    {
        __msg("alarm select song win get win attribute fail \n");
        return EPDK_FAIL ;
    }

    if (GUI_WinGetCaptureWin() == msg->h_deswin)
    {
        GUI_WinReleaseCapture();
    }

    //__here__ ;
    if (alarm_sel_song_para->h_button_fc_bmp)
    {
        theme_close(alarm_sel_song_para->h_button_fc_bmp);
        alarm_sel_song_para->h_button_fc_bmp = NULL ;
    }

    if (alarm_sel_song_para->h_button_uf_bmp)
    {
        theme_close(alarm_sel_song_para->h_button_uf_bmp);
        alarm_sel_song_para->h_button_uf_bmp = NULL ;
    }

    if (alarm_sel_song_para->h_list_item_bg)
    {
        theme_close(alarm_sel_song_para->h_list_item_bg);
        alarm_sel_song_para->h_list_item_bg = NULL ;
    }

    //__here__ ;
    if (GUI_IsTimerInstalled(msg->h_deswin, ALARM_LISTEN_MUSIC_TIMER))
    {
        GUI_KillTimer(msg->h_deswin, ALARM_LISTEN_MUSIC_TIMER);
    }

    //__here__ ;
    if (1 == alarm_sel_song_para->bg_music_is_exist)     // 背景音乐存在的情况下，不能调用robin_close()
    {
        char FileName[RAT_MAX_FULL_PATH_LEN] = {0} ;
        robin_set_play_mode(alarm_sel_song_para->bg_music_play_mode) ;
        alarm_sel_song_para->bg_music_is_exist = 0 ;
        robin_set_cmd_stop();
        robin_npl_index2file(alarm_sel_song_para->current_npl_index, FileName);
        robin_play_file(FileName, NULL);
    }
    else
    {
        robin_set_cmd_stop();
        robin_close();
    }

    //__here__ ;
    alarm_sel_song_long_string_uninit(alarm_sel_song_para);
    //__here__ ;
    alarm_sel_song_listbar_uninit(msg->h_deswin);
    //__here__ ;
    alarm_sel_song_rat_uninit(alarm_sel_song_para);
    //__here__ ;

    if (alarm_sel_song_para)
    {
        esMEMS_Mfree(0, alarm_sel_song_para);
        alarm_sel_song_para = NULL ;
    }

    //__here__ ;
    return EPDK_OK;
}


static __s32  alarm_sel_song_win_on_timer(__gui_msg_t *msg)
{
    alarm_sel_song_t *alarm_sel_song_para ;
    alarm_sel_song_para = (alarm_sel_song_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!alarm_sel_song_para)
    {
        __msg("alarm select song win get win attribute fail \n");
        return EPDK_FAIL ;
    }

    if (ALARM_LISTEN_MUSIC_TIMER == msg->dwAddData1)
    {
        if (CEDAR_STAT_STOP == robin_get_fsm_status())
        {
            robin_play_file(alarm_sel_song_para->FullFileName, NULL);
        }

        GUI_ResetTimer(msg->h_deswin, ALARM_LISTEN_MUSIC_TIMER, ALARM_LISTEN_MUSIC_MONITOR_INTERVAL, 0);
    }

    return EPDK_OK;
}

static __s32 alarm_sel_song_win_cb(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            alarm_sel_song_win_on_create(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_PAINT:
            alarm_sel_song_win_on_paint(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_CLOSE:
            GUI_FrmWinDelete(msg->h_deswin);
            return EPDK_OK ;

        case DSK_MSG_HOME:
            alarm_sel_song_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_VIEW, 0, 0);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
            alarm_sel_song_win_on_destroy(msg);
            return EPDK_OK;

        case GUI_MSG_COMMAND:
            return EPDK_OK;

        case GUI_MSG_KEY:
            alarm_sel_song_on_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            alarm_sel_song_on_touch_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TIMER:
            alarm_sel_song_win_on_timer(msg);
            return EPDK_OK ;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN alarm_sel_song_frm_create(H_WIN h_parent, alarm_sel_song_t *para, __u16 id)
{
    __gui_framewincreate_para_t framewin_para;
    alarm_sel_song_t *alarm_sel_song_para ;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    alarm_sel_song_para = (alarm_sel_song_t *)esMEMS_Malloc(0, sizeof(alarm_sel_song_t));

    if (!alarm_sel_song_para)
    {
        __msg("memory alloc fail , return \n");
        return (H_WIN)EPDK_FAIL ;
    }

    eLIBs_memset((void *)alarm_sel_song_para, 0, sizeof(alarm_sel_song_t));
    alarm_sel_song_para->font = para->font ;
    alarm_sel_song_para->layer = para->layer ;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "alarm sel song window",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id        = id ;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)alarm_sel_song_win_cb);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 255;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)alarm_sel_song_para;
    framewin_para.hLayer = alarm_sel_song_para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

static H_LYR alarm_sel_song_layer_create(void)
{
    H_LYR layer = NULL;
    alarm_sel_song_uipara_t *uipara ;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        10,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "alarm select song layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    uipara = alarm_sel_song_get_uipara();
    fb.size.width       = uipara->FbRect.w ;
    fb.size.height      = uipara->FbRect.h;
    lstlyr.src_win.x        = uipara->SrcWinRect.x ;
    lstlyr.src_win.y        = uipara->SrcWinRect.y ;
    lstlyr.src_win.width    = uipara->SrcWinRect.w ;
    lstlyr.src_win.height   = uipara->SrcWinRect.h ;
    lstlyr.scn_win.x        = uipara->ScnWinRect.x ;
    lstlyr.scn_win.y        = uipara->ScnWinRect.y ;
    lstlyr.scn_win.width    = uipara->ScnWinRect.w ;
    lstlyr.scn_win.height   = uipara->ScnWinRect.h ;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !\n");
    }

    return layer;
}

H_WIN app_alarm_sel_song_create(H_WIN hparent, __u16 win_id)
{
    H_WIN win ;
    alarm_sel_song_t para ;
    para.font = SWFFont ;
    para.layer = alarm_sel_song_layer_create();
    win = alarm_sel_song_frm_create(hparent, &para, win_id);
    __msg("win = %x\n", win);
    return win ;
}

__s32  app_alarm_sel_song_delete(H_WIN win)
{
    H_LYR layer ;

    if (!win)
    {
        __msg("window handle is null return \n");
        return EPDK_FAIL ;
    }

    layer = GUI_WinGetLyrWin(win);

    if (!layer)
    {
        __msg("app alarm select song get layer win fail\n");
        return EPDK_FAIL ;
    }

    GUI_LyrWinDelete(layer);
    return EPDK_OK ;
}

