#include "record_list_view.h"
#include "record_comm.h"
#include <log.h>

static __s32 record_list_view_musicList_draw(__lbar_draw_para_t *draw_param);
static __s32 record_list_view_normal_item_on_draw(__lbar_draw_para_t *draw_param, precord_list_view_t prec_list_view);
static __s32 record_list_view_focus_item_on_draw(__lbar_draw_para_t *draw_param, precord_list_view_t prec_list_view);


/*void recordlist_menu_init(recordlist_menu_ctrl_t   rlmenu_ctrl)
{
     eLIBs_memset( (void *)&rlmenu_ctrl->recordlist_nmenu ,  0 , sizeof(rlmenu_ctrl->recordlist_nmenu) );
     eLIBs_memset( (void *)&rlmenu_ctrl->recordlist_fmenu ,  0 , sizeof(rlmenu_ctrl->recordlist_fmenu) );
     rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id = ID_RECORD_BACK_0_BMP ;
     rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_nmenu[NMENU_PRE].res_id = ID_RECORD_PRE_0_BMP ;
     rlmenu_ctrl->recordlist_nmenu[NMENU_PRE].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_nmenu[NMENU_PRE].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_nmenu[NMENU_PLAY].res_id = ID_RECORD_PLAY_PAUSE_BMP ;
     rlmenu_ctrl->recordlist_nmenu[NMENU_PLAY].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_nmenu[NMENU_PLAY].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_nmenu[NMENU_PAUSE].res_id = ID_RECORD_PLAY_START_BMP ;
     rlmenu_ctrl->recordlist_nmenu[NMENU_PAUSE].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_nmenu[NMENU_PAUSE].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_nmenu[NMENU_NEXT].res_id = ID_RECORD_NEXT_0_BMP ;
     rlmenu_ctrl->recordlist_nmenu[NMENU_NEXT].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_nmenu[NMENU_NEXT].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_nmenu[NMENU_DEL].res_id = ID_RECORD_DEL_0_BMP ;
     rlmenu_ctrl->recordlist_nmenu[NMENU_DEL].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_nmenu[NMENU_DEL].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle  );




     rlmenu_ctrl->recordlist_fmenu[FMENU_BACK].res_id = ID_RECORD_BACK_1_BMP ;
     rlmenu_ctrl->recordlist_fmenu[FMENU_BACK].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_fmenu[FMENU_BACK].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_fmenu[FMENU_PRE].res_id = ID_RECORD_PRE_1_BMP ;
     rlmenu_ctrl->recordlist_fmenu[FMENU_PRE].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_fmenu[FMENU_PRE].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_fmenu[FMENU_PLAY].res_id = ID_RECORD_PLAY_PAUSE1_BMP ;
     rlmenu_ctrl->recordlist_fmenu[FMENU_PLAY].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_fmenu[FMENU_PLAY].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_fmenu[FMENU_PAUSE].res_id = ID_RECORD_PLAY_START1_BMP ;
     rlmenu_ctrl->recordlist_fmenu[FMENU_PAUSE].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_fmenu[FMENU_PAUSE].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_fmenu[FMENU_NEXT].res_id = ID_RECORD_NEXT_1_BMP ;
     rlmenu_ctrl->recordlist_fmenu[FMENU_NEXT].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_fmenu[FMENU_NEXT].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].h_bmp_handle  );

     rlmenu_ctrl->recordlist_fmenu[NMENU_DEL].res_id = ID_RECORD_DEL_1_BMP ;
     rlmenu_ctrl->recordlist_fmenu[NMENU_DEL].h_bmp_handle = theme_open( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].res_id );
     rlmenu_ctrl->recordlist_fmenu[NMENU_DEL].res_buffer = theme_hdl2buf( rlmenu_ctrl->recordlist_fmenu[NMENU_BACK].h_bmp_handle  );

}
void recordlist_menu_uinit(recordlist_menu_ctrl_t   rlmenu_ctrl)
{
  dsk_theme_close( rlmenu_ctrl->recordlist_nmenu[NMENU_BACK].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_nmenu[NMENU_PRE].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_nmenu[NMENU_PLAY].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_nmenu[NMENU_PAUSE].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_nmenu[NMENU_NEXT].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_nmenu[NMENU_NEXT].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_fmenu[FMENU_BACK].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_fmenu[FMENU_PRE].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_fmenu[FMENU_PLAY].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_fmenu[FMENU_PAUSE].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_fmenu[FMENU_NEXT].h_bmp_handle );
  dsk_theme_close( rlmenu_ctrl->recordlist_fmenu[FMENU_NEXT].h_bmp_handle );
}*/

__s32 record_list_view_musicList_init(precord_list_view_t prec_list_view, H_WIN framewin)
{
    __listbar_config_t  config;
    __scroll_bar_t      scroll_param;
    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    eLIBs_memset(&scroll_param, 0, sizeof(__scroll_bar_t));
    eLIBs_memset(&prec_list_view->show_info, 0, sizeof(__show_info_move_t));
    prec_list_view->show_info.hlayer = prec_list_view->layer;
    prec_list_view->show_info.alpha_en = EPDK_TRUE;
    prec_list_view->show_info.pFont = prec_list_view->sys_font;
    prec_list_view->show_info.encode_id = EPDK_CHARSET_ENM_UTF8;
    prec_list_view->show_info.fontColor = GUI_WHITE;
    prec_list_view->show_info.bkColor = 0x80000000;
    prec_list_view->show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
    prec_list_view->show_info.region.width = 324;
    prec_list_view->show_info.region.height = 70;
    prec_list_view->show_info.bmp = get_record_bmp_data(prec_list_view->pui, RECORD_MUSIC_PROGRESS_FOCUS_BMP);
    prec_list_view->show_info.bmp_pos.x = 0;
    prec_list_view->show_info.bmp_pos.y = 29;
    prec_list_view->long_str_handle = GUI_LongStringCreate(&prec_list_view->show_info);

    if (prec_list_view->long_str_handle == NULL)
    {
        __wrn("create long string failed!\n");
    }

    config.list_rect.x = 0;
    config.list_rect.y = 46;
    config.list_rect.width = 324;
    config.list_rect.height = 432;
    config.item_width = 324;
    config.item_height = 54;                                    //these are numbers only for test
    config.start_id = prec_list_view->precord_list_doc->topIndex;
    config.focus_id = prec_list_view->precord_list_doc->playIndex;
    config.bk_color = 0xFF2B2E36;
    config.alpha_status = 1;                                    //打开Alpha开关
    config.list_attr = (void *)prec_list_view;
    config.item_cnt = prec_list_view->precord_list_doc->total;   //Rat.total必须已经获得
#if 0
    __wrn("config.item_cnt:%d\r\n", config.item_cnt);
    __wrn("config.start_id:%d\r\n", config.start_id);
    __wrn("config.focus_id:%d\r\n", config.focus_id);
    scroll_param.show_rect.x = 325;
    scroll_param.show_rect.y = 46;
    scroll_param.show_rect.width = 10;
    scroll_param.show_rect.height = 434;
    //滚动条区域(除去head,body,tail)
    scroll_param.scroll_rect.x = 325;
    scroll_param.scroll_rect.y = 46;
    scroll_param.scroll_rect.width = 10;
    scroll_param.scroll_rect.height = 404 - 2;      //滚动条区域(除去head,body,tail)
    scroll_param.head_height = 3;
    scroll_param.tail_height = 3;               //head, tail
    scroll_param.back_bmp = get_record_bmp_data(prec_list_view->pui, RECORD_MUSIC_PROGRESS_BG_BMP);
    scroll_param.head_bmp = NULL;//get_record_bmp_data(prec_list_view->pui, RECORD_MUSIC_PROGRESS_UP_BMP);
    scroll_param.body_bmp = get_record_bmp_data(prec_list_view->pui, RECORD_MUSIC_PROGRESS_FLAG_BMP);
    scroll_param.tail_bmp = NULL;//get_record_bmp_data(prec_list_view->pui, RECORD_MUSIC_PROGRESS_DOWN_BMP);
#endif
    prec_list_view->listbar_handle = LISTBAR_Create(framewin);                  //new listbar

    if (prec_list_view->listbar_handle == NULL)
    {
        __msg("\n\n\nLIST BAR FAIL!\n\n\n\n");
        return EPDK_FAIL;
    }

    LISTBAR_Config(prec_list_view->listbar_handle, record_list_view_musicList_draw, &config);    //config parametters for listbar
#if 0
    LISTBAR_ScrollConfig(prec_list_view->listbar_handle, &scroll_param);
#endif
    record_list_view_start_show(prec_list_view, prec_list_view->layer);
    return EPDK_OK;
}

__s32 record_list_view_musicList_UnInit(precord_list_view_t prec_list_view)
{
    if (prec_list_view->listbar_handle != NULL)
    {
        LISTBAR_SuspendPage(prec_list_view->listbar_handle);
        LISTBAR_Delete(prec_list_view->listbar_handle);
        prec_list_view->listbar_handle = NULL;
    }

    if (prec_list_view->long_str_handle != NULL)
    {
        GUI_LongStringDelete(prec_list_view->long_str_handle);
        prec_list_view->long_str_handle = NULL;
    }

    //..recordlist_menu_uinit(prec_list_view->rlmenu_ctrl);//..
    return EPDK_OK;
}
static __s32 record_list_view_vacant_item_on_draw(__lbar_draw_para_t *draw_param, precord_list_view_t prec_list_view)
{
    GUI_RECT    item_rect = {0};
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    item_rect.x0 = draw_param->rect.x;
    item_rect.y0 = draw_param->rect.y;
    item_rect.x1 = draw_param->rect.x + draw_param->rect.width;
    item_rect.y1 = draw_param->rect.y + draw_param->rect.height;
    draw_record_bmp_coor(prec_list_view->pui, RECORD_MUSICLIST_BG, draw_param->rect.x, draw_param->rect.y);
    return 0;
}
static __s32 record_list_view_musicList_draw(__lbar_draw_para_t *draw_param)
{
    precord_list_view_t prec_list_view;
    prec_list_view = (precord_list_view_t)draw_param->attr;     //for draw the picture  in different media type

    if (prec_list_view == NULL)
    {
        return EPDK_FAIL;
    }

    if (prec_list_view->precord_list_doc->total <= 0)
    {
        return EPDK_FAIL;
    }

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
            __wrn("normal item index:%d: x=%d, y=%d\n", draw_param->index, draw_param->rect.x, draw_param->rect.y);
            record_list_view_normal_item_on_draw(draw_param, prec_list_view);
            break;

        case LBAR_MODE_FOCUS:
            __wrn("focus item index:%d: x=%d, y=%d\n", draw_param->index, draw_param->rect.x, draw_param->rect.y);
            record_list_view_focus_item_on_draw(draw_param, prec_list_view);
            break;

        case LBAR_MODE_INVALID:
            break;

        case LBAR_MODE_VACANT:
            __wrn("focus item index:%d: x=%d, y=%d\n", draw_param->index, draw_param->rect.x, draw_param->rect.y);
            record_list_view_vacant_item_on_draw(draw_param, prec_list_view);
            break;

        case LBAR_MODE_DIFFUSE:
            break;

        default:
            break;
    }

    return EPDK_OK;
}

static __s32 record_list_view_normal_item_on_draw(__lbar_draw_para_t *draw_param, precord_list_view_t prec_list_view)
{
    char        str[1024] = {0};
    GUI_RECT    item_rect = {0};
    record_list_getFileName(prec_list_view->precord_list_doc, draw_param->index, str);
    GUI_SetFont(prec_list_view->sys_font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetColor(GUI_BLACK);
    item_rect.x0 = draw_param->rect.x;
    item_rect.y0 = draw_param->rect.y;
    item_rect.x1 = draw_param->rect.x + draw_param->rect.width;
    item_rect.y1 = draw_param->rect.y + draw_param->rect.height;
    // ..GUI_DispStringInRect(str, &item_rect, GUI_TA_LEFT|GUI_TA_VCENTER );
    draw_record_bmp_coor(prec_list_view->pui, RECORD_MUSICLIST_BG, draw_param->rect.x, draw_param->rect.y);
    GUI_DispStringInRect(str, &item_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    return EPDK_OK;
}

static __s32 record_list_view_focus_item_on_draw(__lbar_draw_para_t *draw_param, precord_list_view_t prec_list_view)
{
    char str[1024] = {0};
    GUI_RECT    item_rect = {0};
    prec_list_view->precord_list_doc->curIndex = draw_param->index;
    record_list_getFileName(prec_list_view->precord_list_doc, draw_param->index, str);
    item_rect.x0 = draw_param->rect.x;
    item_rect.y0 = draw_param->rect.y;
    item_rect.x1 = draw_param->rect.x + draw_param->rect.width;
    item_rect.y1 = draw_param->rect.y + draw_param->rect.height;
    // ..GUI_DispStringInRect(str, &item_rect, GUI_TA_LEFT|GUI_TA_VCENTER );
    draw_record_bmp_coor(prec_list_view->pui, RECORD_MUSIC_PROGRESS_FOCUS_BMP, draw_param->rect.x, draw_param->rect.y);
    GUI_DispStringInRect(str, &item_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    return EPDK_OK;
}

static void record_list_view_playbar(precord_list_view_t prec_list_view)
{
    FOCUSE_STATE musicList[MUSIC_MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL};
    RECORD_LIST_FOCUSE focuse;
    char times[10] = {0};
    __u32 cnt = 0, len = 0;
    __s16 offsetX = 0, offsetY = 0;
    draw_record_bmp(prec_list_view->pui, RECORD_66_BMP);
    musicList[prec_list_view->curMenu] = prec_list_view->curMenuSt;
    //..draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_BACK,  musicList[MUSIC_MENU_BACK] );
    //..draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_BACKWARD,   musicList[MUSIC_MENU_BACKWARD] );
    //..draw_record_music_mapping( prec_list_view->pui, prec_list_view->playMenu,   musicList[prec_list_view->playMenu] );
    //..draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_FORWARD, musicList[MUSIC_MENU_FORWARD] );
    //..draw_record_music_mapping( prec_list_view->pui, MUSIC_MENU_DEL,     musicList[MUSIC_MENU_DEL] );
    GUI_SetColor(GUI_WHITE);
    record_ms_to_char(times, prec_list_view->precord_list_doc->musicCurTimes, ':');
    GUI_DispStringAt(times, 367, 200);
    record_ms_to_char(times, prec_list_view->precord_list_doc->musicMaxTimes, ':');
    GUI_DispStringAt(times, 680, 200);

    //200 = 400/2
    if ((prec_list_view->precord_list_doc->musicMaxTimes / 1000) == 0)
    {
        len = 0;
    }
    else
    {
        len = ((prec_list_view->precord_list_doc->musicCurTimes / 1000) * 200) / (prec_list_view->precord_list_doc->musicMaxTimes / 1000);
    }

    for (cnt = 0; cnt < len; cnt++)
    {
        draw_record_bmp_ext(prec_list_view->pui, RECORD_66_MID_BMP,  offsetX, offsetY);
        offsetX += 2;
    }

    if (cnt == 200)
    {
        offsetX -= 1;
        draw_record_bmp_ext(prec_list_view->pui, RECORD_66_MID_BMP,  offsetX, offsetY);
        offsetX += 2;
    }

    offsetX -= 2;

    if ((prec_list_view->precord_list_doc->musicCurTimes / 1000) == 0)
    {
        draw_record_bmp_ext(prec_list_view->pui, RECORD_66_FLAG_BMP,  0, 0);
    }
    else if ((prec_list_view->precord_list_doc->musicMaxTimes / 1000) == (prec_list_view->precord_list_doc->musicCurTimes / 1000))
    {
        offsetX -= 4;
        draw_record_bmp_ext(prec_list_view->pui, RECORD_66_MID_BMP, offsetX, offsetY);
    }
    else
    {
        if (offsetX > 400 - 10)
        {
            offsetX = 400 - 10;
        }

        draw_record_bmp_ext(prec_list_view->pui, RECORD_66_FLAG_BMP, offsetX, offsetY);
    }
}

static void record_list_view_delBox(precord_list_view_t prec_list_view)
{
    __s16 offsetX = 0, offsetY = 0;

    if (prec_list_view->delState == RECORD_DEL_NONE)
    {
        return;
    }

    draw_record_bmp(prec_list_view->pui, RECORD_DEL_BOX_BMP);
    draw_record_bmp_ext(prec_list_view->pui, RECORD_DEL_SEL0_0_BMP + (prec_list_view->delState == RECORD_DEL_OK), offsetX, offsetY);
    offsetX += 71;
    draw_record_bmp_ext(prec_list_view->pui, RECORD_DEL_SEL0_0_BMP + (prec_list_view->delState == RECORD_DEL_CAL), offsetX, offsetY);
    draw_record_lang(prec_list_view->pui, STRING_YES, APP_COLOR_WHITE);
    draw_record_lang(prec_list_view->pui, STRING_NO, APP_COLOR_WHITE);
    draw_record_lang(prec_list_view->pui, STRING_DELETE, APP_COLOR_WHITE);
    draw_record_lang(prec_list_view->pui, STRING_MSG4, APP_COLOR_WHITE);
}

void record_list_view_show(precord_list_view_t prec_list_view, H_WIN layer)
{
    GUI_MEMDEV_Handle   draw_mem;
    __s16 x1, y1;
    __s16 x0 = 367, y0 = 150, w = 400, h = 75;

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

    if (draw_mem == 0)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_SetBkColor(0x00);
    GUI_ClearRect(x0, y0, x1,  y1);
    draw_record_bmp(prec_list_view->pui, RECORD_MUSICPLAY_BG);
    record_list_view_playbar(prec_list_view);
    record_list_view_delBox(prec_list_view);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
}

void record_list_icon_view_show(precord_list_view_t prec_list_view, H_WIN layer)
{
    FOCUSE_STATE musicList[MUSIC_MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL};
    /* GUI_MEMDEV_Handle draw_mem;
    __s16 x1, y1;
    __s16 x0 = 367, y0 = 250, w = 420, h = 50;*/

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        //return;
        GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_SLEEP);
    }

    //x1 = w+x0-1;
    //y1 = h+y0-1;
    GUI_LyrWinSel(layer);
    /*draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);
    if( draw_mem == 0 )
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }
    GUI_MEMDEV_Select(draw_mem);

    GUI_SetBkColor(0x00);
    GUI_ClearRect(x0, y0, x1,  y1);*/
    musicList[prec_list_view->curMenu] = prec_list_view->curMenuSt;
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_BACK,  musicList[MUSIC_MENU_BACK]);
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_BACKWARD,   musicList[MUSIC_MENU_BACKWARD]);
    draw_record_music_mapping(prec_list_view->pui, prec_list_view->playMenu,   musicList[prec_list_view->playMenu]);
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_FORWARD, musicList[MUSIC_MENU_FORWARD]);
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_DEL,     musicList[MUSIC_MENU_DEL]);
    //GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_ON);
    /* GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select( NULL );
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;*/
}

void record_list_allview_show(precord_list_view_t prec_list_view, H_WIN layer)
{
    GUI_MEMDEV_Handle   draw_mem;
    __s16 x1, y1;
    __s16 x0 = 0, y0 = 0, w = 800, h = 480;
    FOCUSE_STATE musicList[MUSIC_MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_NORMAL}; //..

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

    if (draw_mem == 0)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
        //return;
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_SetBkColor(0x00);
    GUI_ClearRect(x0, y0, x1,  y1);
    draw_record_bmp(prec_list_view->pui, RECORD_LIST_WIN_BG);  //..
    // draw_record_bmp( prec_list_view->pui, RECORD_LIST_BG );//..
    record_list_view_playbar(prec_list_view);
    // record_list_icon_view_show(prec_list_view,layer);//..
    musicList[prec_list_view->curMenu] = prec_list_view->curMenuSt;
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_BACK,  musicList[MUSIC_MENU_BACK]);
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_BACKWARD,   musicList[MUSIC_MENU_BACKWARD]);
    draw_record_music_mapping(prec_list_view->pui, prec_list_view->playMenu,   musicList[prec_list_view->playMenu]);
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_FORWARD, musicList[MUSIC_MENU_FORWARD]);
    draw_record_music_mapping(prec_list_view->pui, MUSIC_MENU_DEL,     musicList[MUSIC_MENU_DEL]);
    record_list_view_delBox(prec_list_view);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
}


void record_list_view_start_show(precord_list_view_t prec_list_view, H_WIN layer)
{
#if 0
    GUI_MEMDEV_Handle   draw_mem;
    __s16 x1, y1;
    __s16 x0 = 0, y0 = 0, w = 480, h = 272;

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

    if (draw_mem == 0)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_SetBkColor(0x00);
    GUI_ClearRect(x0, y0, x1,  y1);
#endif
    GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_SLEEP);
    GUI_LyrWinSel(layer);
    //draw_record_bmp( prec_list_view->pui, RECORD_MUSICLIST_BG );
    record_list_allview_show(prec_list_view, layer);
    LISTBAR_ShowPage(prec_list_view->listbar_handle);
    GUI_LyrWinSetSta(layer, GUI_LYRWIN_STA_ON);
#if 0
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
}
