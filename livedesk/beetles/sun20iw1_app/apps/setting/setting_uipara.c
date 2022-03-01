/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              LiveTouch Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : ebook
* File    : ebook_uipara.c
*
* By      : Bayden.chen
* Version : v1.0
* Date    : 2009-12-25 14:53:05
**********************************************************************************************************************
*/
#ifndef __SETTING_PARA_H__
#define __SETTING_PARA_H__
#include "setting_uipara.h"

enum
{
    SCN_TYPE_400_240,
    SCN_TYPE_480_272,
    SCN_TYPE_800_480,
    SCN_TYPE_480_640,
    SCN_TYPE_720_480,
    SCN_TYPE_720_576,
};

static  setting_uipara_t  setting_uipara;

/*
 setting_uipara_t* setting_get_uipara()
{
    SIZE screen_size;
    __s32 screen_type;

    dsk_display_get_size(&screen_size.width,&screen_size.height);
    if((screen_size.width == 800 )&&( screen_size.height == 480))
        screen_type = SCN_TYPE_800_480;
    else if((screen_size.width == 480 )&&( screen_size.height == 272))
        screen_type = SCN_TYPE_480_272;
    else if((screen_size.width == 400 )&&( screen_size.height == 240))
        screen_type = SCN_TYPE_400_240;
    else
    {
        screen_type = SCN_TYPE_400_240;
    }

    switch(screen_type)
    {
        case SCN_TYPE_800_480:
        case SCN_TYPE_480_272:
        case SCN_TYPE_400_240:
        {
            setting_uipara.lyr_win.x = 0;
            setting_uipara.lyr_win.y = 0;
            setting_uipara.lyr_win.w = 480;
            setting_uipara.lyr_win.h = 245;

            setting_uipara.lyr_scn.x = 0;
            setting_uipara.lyr_scn.y = (272 -setting_uipara.lyr_win.h);
            setting_uipara.lyr_scn.w = 480;
            setting_uipara.lyr_scn.h = 245;

            setting_uipara.item.w = 343;
            setting_uipara.item.h = 49;
            setting_uipara.item.x = 480-setting_uipara.item.w;
            setting_uipara.item.y = 0;

            setting_uipara.left_button.x = setting_uipara.item.x+200;
            setting_uipara.left_button.y = setting_uipara.item.y + 10;
            setting_uipara.left_button.w = 24;
            setting_uipara.left_button.h = 27;

            setting_uipara.right_button.x   = setting_uipara.item.x+301;
            setting_uipara.right_button.y   = setting_uipara.item.y + 10;
            setting_uipara.right_button.w   = 24;
            setting_uipara.left_button.h        = 27;

            setting_uipara.txt_pos.x = 15;
            setting_uipara.txt_pos.y = 105;

            setting_uipara.string_detail_pos_ex.x = 0;
            setting_uipara.string_detail_pos_ex.y = 130;
            setting_uipara.string_detail_pos_ex.w = 130;
            setting_uipara.string_detail_pos_ex.h = 20;

            setting_uipara.string_detail_pos_de.x = 0;
            setting_uipara.string_detail_pos_de.y = 150;
            setting_uipara.string_detail_pos_de.w = 130;
            setting_uipara.string_detail_pos_de.h = 20;

            setting_uipara.item_title_pos.x = setting_uipara.item.x+10;
            setting_uipara.item_title_pos.y = setting_uipara.item.y;
            setting_uipara.item_title_pos.w = 190;
            setting_uipara.item_title_pos.h = setting_uipara.item.h;

            setting_uipara.item_content_pos.x = setting_uipara.left_button.x+setting_uipara.left_button.w;
            setting_uipara.item_content_pos.y = setting_uipara.item.y;
            setting_uipara.item_content_pos.w = 70;
            setting_uipara.item_content_pos.h = setting_uipara.item.h;

            setting_uipara.return_bmp.x = 87;
            setting_uipara.return_bmp.y = 31;
            setting_uipara.return_bmp.x = 34;
            setting_uipara.return_bmp.x = 30;

            setting_uipara.setting_tips_uipara.tips_lyr.x = ( 480 - 320) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.y = ( 272 - 206) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.w = 320 ;
            setting_uipara.setting_tips_uipara.tips_lyr.h = 206 ;

            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.x = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.y = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.w = 320 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.h = 206 ;

            setting_uipara.setting_tips_uipara.ok_bmp_pos.x = ( 320 - 140) / 2;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.y = 156 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.w = 140 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.h = 38 ;

            setting_uipara.setting_tips_uipara.tip_tsize_pos.x = ( 320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.y = 30 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.h = 30 ;

            setting_uipara.setting_tips_uipara.tip_fsize_pos.x = ( 320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.y = 60 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.h = 30 ;

        }
            break;
        default :
            break;
    }
    return &setting_uipara;
}
 */

setting_uipara_t *setting_get_uipara(void)
{
    SIZE screen_size;
    __s32 screen_type;
    dsk_display_get_size(&screen_size.width, &screen_size.height);

    if ((screen_size.width == 800) && (screen_size.height == 480))
    {
        screen_type = SCN_TYPE_800_480;
    }
    if ((screen_size.width == 480) && (screen_size.height == 640))
    {
        screen_type = SCN_TYPE_480_640;
    }    
    else if ((screen_size.width == 480) && (screen_size.height == 272))
    {
        screen_type = SCN_TYPE_480_272;
    }
    else if ((screen_size.width == 400) && (screen_size.height == 240))
    {
        screen_type = SCN_TYPE_400_240;
    }
    else if ((screen_size.width == 720) && (screen_size.height == 480))
    {
        screen_type = SCN_TYPE_720_480;
    }
    else if ((screen_size.width == 720) && (screen_size.height == 576))
    {
        screen_type = SCN_TYPE_720_576;
    }
    else
    {
        screen_type = SCN_TYPE_400_240;
    }

    switch (screen_type)
    {
        case SCN_TYPE_800_480:
        case SCN_TYPE_480_272:
        case SCN_TYPE_400_240:
        case SCN_TYPE_480_640:            
        {
            setting_uipara.lyr_win.x = 0;
            setting_uipara.lyr_win.y = 0;
            setting_uipara.lyr_win.w = 800;//480;
            setting_uipara.lyr_win.h = 480;//245;
            setting_uipara.lyr_scn.x = 0;
            setting_uipara.lyr_scn.y = 46;//(272 -setting_uipara.lyr_win.h);
            setting_uipara.lyr_scn.w = 800;//480;
            setting_uipara.lyr_scn.h = 480 - 46;//245;
            /*right item location*/
            setting_uipara.item.w = 490;
            setting_uipara.item.h = 54;
            setting_uipara.item.x = 800 - setting_uipara.item.w;
            setting_uipara.item.y = 0;
#if 1
            /*left item location*/
            setting_uipara.item_general_setting.w = 300;
            setting_uipara.item_general_setting.h = 432;
            setting_uipara.item_general_setting.x = 0;
            setting_uipara.item_general_setting.y = 0;
            setting_uipara.item_power_manager.w = 300;
            setting_uipara.item_power_manager.h = 106;
            setting_uipara.item_power_manager.x = 0;
            setting_uipara.item_power_manager.y = 106;
            setting_uipara.item_alarm1_setting.w = 300;
            setting_uipara.item_alarm1_setting.h = 106;
            setting_uipara.item_alarm1_setting.x = 0;
            setting_uipara.item_alarm1_setting.y = 0;
            setting_uipara.item_alarm2_setting.w = 300;
            setting_uipara.item_alarm2_setting.h = 106;
            setting_uipara.item_alarm2_setting.x = 0;
            setting_uipara.item_alarm2_setting.y = 106;
            setting_uipara.item_alarm3_setting.w = 300;
            setting_uipara.item_alarm3_setting.h = 106;
            setting_uipara.item_alarm3_setting.x = 0;
            setting_uipara.item_alarm3_setting.y = 106 * 2;
#endif
            setting_uipara.left_button.x = 490 - (800 - 509) + 30;
            setting_uipara.left_button.y = setting_uipara.item.y + 4;
            setting_uipara.left_button.w = 46;
            setting_uipara.left_button.h = 46;
            setting_uipara.right_button.x   = 490 - (800 - 724);
            setting_uipara.right_button.y   = setting_uipara.item.y + 4;
            setting_uipara.right_button.w   = 46;
            setting_uipara.right_button.h   = 46;
            setting_uipara.txt_pos.x = 15;
            setting_uipara.txt_pos.y = 105;
            //setting_uipara.string_detail_pos_ex.x = 0;
            //setting_uipara.string_detail_pos_ex.y = 130;
            //setting_uipara.string_detail_pos_ex.w = 130;
            //setting_uipara.string_detail_pos_ex.h = 20;
            setting_uipara.string_detail_pos_ex.x = 0;
            setting_uipara.string_detail_pos_ex.y = 260;
            setting_uipara.string_detail_pos_ex.w = 300;
            setting_uipara.string_detail_pos_ex.h = 50;
            setting_uipara.string_detail_pos_de.x = 0;
            setting_uipara.string_detail_pos_de.y = 150;
            setting_uipara.string_detail_pos_de.w = 130;
            setting_uipara.string_detail_pos_de.h = 40;
            //如语言
            setting_uipara.item_title_pos.x = 20;
            setting_uipara.item_title_pos.y = setting_uipara.item.y;
            setting_uipara.item_title_pos.w = 180;
            setting_uipara.item_title_pos.h = setting_uipara.item.h;
            //如简体中文
            setting_uipara.item_content_pos.x = setting_uipara.left_button.x + setting_uipara.left_button.w;
            setting_uipara.item_content_pos.y = setting_uipara.item.y;
            setting_uipara.item_content_pos.w = (setting_uipara.right_button.x - setting_uipara.item_content_pos.x);
            setting_uipara.item_content_pos.h = setting_uipara.item.h;
            setting_uipara.return_bmp.x = 87;
            setting_uipara.return_bmp.y = 31;
            setting_uipara.return_bmp.w = 34;
            setting_uipara.return_bmp.h = 30;
            setting_uipara.long_string_area.x = 154 ;
            setting_uipara.long_string_area.y = setting_uipara.item_title_pos.y + 8 ;
            setting_uipara.long_string_area.w = 144 ;
            setting_uipara.long_string_area.h = 32 ;
            setting_uipara.setting_tips_uipara.tips_lyr.x = (800 - 320) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.y = (480 - 206) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.w = 320 ;
            setting_uipara.setting_tips_uipara.tips_lyr.h = 206 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.x = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.y = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.w = 320 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.h = 206 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.x = (320 - 140) / 2;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.y = 156 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.w = 140 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.h = 38 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.x = (320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.y = 30 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.h = 30 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.x = (320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.y = 60 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.h = 30 ;
            /*scroll bar start*/
            setting_uipara.scroll_bg_rect.x = 300;
            setting_uipara.scroll_bg_rect.y = 0;
            setting_uipara.scroll_bg_rect.width = 10;
            setting_uipara.scroll_bg_rect.height = 10;
            setting_uipara.scroll_rect.x = 300;
            setting_uipara.scroll_rect.y = 0;
            setting_uipara.scroll_rect.width = 10;
            setting_uipara.scroll_rect.height = 10;
            //scroll head/body/tail height
            setting_uipara.scroll_head_height = 0;
            setting_uipara.scroll_body_height = 30;
            setting_uipara.scroll_tail_height = 0;
            /*scroll bar end*/
        }
        break;
#if 0                    
        case SCN_TYPE_480_640:
        {
            setting_uipara.lyr_win.x = 0;
            setting_uipara.lyr_win.y = 0;
            setting_uipara.lyr_win.w = 640;//480;
            setting_uipara.lyr_win.h = 480;//245;
            setting_uipara.lyr_scn.x = 0;
            setting_uipara.lyr_scn.y = 46 + 60;//(272 -setting_uipara.lyr_win.h);
            setting_uipara.lyr_scn.w = 640;//480;
            setting_uipara.lyr_scn.h = 480 - 46 - 60;//245;
            /*right item location*/
            setting_uipara.item.w = 330;
            setting_uipara.item.h = 50;
            setting_uipara.item.x = 640 - setting_uipara.item.w;
            setting_uipara.item.y = 0;
#if 1
            /*left item location*/
            setting_uipara.item_general_setting.w = 300;
            setting_uipara.item_general_setting.h = 432;
            setting_uipara.item_general_setting.x = 0;
            setting_uipara.item_general_setting.y = 0;
            setting_uipara.item_power_manager.w = 300/2;
            setting_uipara.item_power_manager.h = 106/2;
            setting_uipara.item_power_manager.x = 0;
            setting_uipara.item_power_manager.y = 106;
            setting_uipara.item_alarm1_setting.w = 300/2;
            setting_uipara.item_alarm1_setting.h = 106/2;
            setting_uipara.item_alarm1_setting.x = 0;
            setting_uipara.item_alarm1_setting.y = 0;
            setting_uipara.item_alarm2_setting.w = 300/2;
            setting_uipara.item_alarm2_setting.h = 106/2;
            setting_uipara.item_alarm2_setting.x = 0;
            setting_uipara.item_alarm2_setting.y = 106/2;
            setting_uipara.item_alarm3_setting.w = 300/2;
            setting_uipara.item_alarm3_setting.h = 106/2;
            setting_uipara.item_alarm3_setting.x = 0;
            setting_uipara.item_alarm3_setting.y = 106 * 2/2;
#endif
            setting_uipara.left_button.x = (400 - (640 - 200) + 70);
            setting_uipara.left_button.y = (setting_uipara.item.y + 4);
            setting_uipara.left_button.w = 40;
            setting_uipara.left_button.h = 40;
            setting_uipara.right_button.x   = (400 - (640 - 300) + 70);
            setting_uipara.right_button.y   = (setting_uipara.item.y + 4);
            setting_uipara.right_button.w   = 40;
            setting_uipara.right_button.h   = 40;
            setting_uipara.txt_pos.x = 15;
            setting_uipara.txt_pos.y = 105 + 60;
            //setting_uipara.string_detail_pos_ex.x = 0;
            //setting_uipara.string_detail_pos_ex.y = 130;
            //setting_uipara.string_detail_pos_ex.w = 130;
            //setting_uipara.string_detail_pos_ex.h = 20;
            setting_uipara.string_detail_pos_ex.x = 0;
            setting_uipara.string_detail_pos_ex.y = 160;
            setting_uipara.string_detail_pos_ex.w = 300;
            setting_uipara.string_detail_pos_ex.h = 50;
            setting_uipara.string_detail_pos_de.x = 0;
            setting_uipara.string_detail_pos_de.y = 150;
            setting_uipara.string_detail_pos_de.w = 130;
            setting_uipara.string_detail_pos_de.h = 40;
            //如语言
            setting_uipara.item_title_pos.x = 220;
            setting_uipara.item_title_pos.y = setting_uipara.item.y;
            setting_uipara.item_title_pos.w = 180;
            setting_uipara.item_title_pos.h = setting_uipara.item.h;
            //如简体中文
            setting_uipara.item_content_pos.x = setting_uipara.left_button.x + setting_uipara.left_button.w;
            setting_uipara.item_content_pos.y = setting_uipara.item.y;
            setting_uipara.item_content_pos.w = (setting_uipara.right_button.x - setting_uipara.item_content_pos.x);
            setting_uipara.item_content_pos.h = setting_uipara.item.h;
            setting_uipara.return_bmp.x = 87;
            setting_uipara.return_bmp.y = 31;
            setting_uipara.return_bmp.w = 34;
            setting_uipara.return_bmp.h = 30;
            setting_uipara.long_string_area.x = 154 ;
            setting_uipara.long_string_area.y = setting_uipara.item_title_pos.y + 8 ;
            setting_uipara.long_string_area.w = 144 ;
            setting_uipara.long_string_area.h = 32 ;
            setting_uipara.setting_tips_uipara.tips_lyr.x = (640 - 320) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.y = (480 - 206) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.w = 320 ;
            setting_uipara.setting_tips_uipara.tips_lyr.h = 206 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.x = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.y = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.w = 320 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.h = 206 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.x = (320 - 140) / 2;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.y = 156 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.w = 140 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.h = 38 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.x = (320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.y = 30 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.h = 30 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.x = (320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.y = 60 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.h = 30 ;
            /*scroll bar start*/
            setting_uipara.scroll_bg_rect.x = 400;
            setting_uipara.scroll_bg_rect.y = 0;
            setting_uipara.scroll_bg_rect.width = 10;
            setting_uipara.scroll_bg_rect.height = 10;
            setting_uipara.scroll_rect.x = 400;
            setting_uipara.scroll_rect.y = 0;
            setting_uipara.scroll_rect.width = 10;
            setting_uipara.scroll_rect.height = 10;
            //scroll head/body/tail height
            setting_uipara.scroll_head_height = 0;
            setting_uipara.scroll_body_height = 30;
            setting_uipara.scroll_tail_height = 0;
            /*scroll bar end*/
        }
#endif
        break;
        
        case SCN_TYPE_720_480:
        case SCN_TYPE_720_576:
        {
            setting_uipara.lyr_win.x = 0;
            setting_uipara.lyr_win.y = 0;
            setting_uipara.lyr_win.w = 720;//480;
            setting_uipara.lyr_win.h = 480;//245;
            setting_uipara.lyr_scn.x = 0;
            setting_uipara.lyr_scn.y = 46;//(272 -setting_uipara.lyr_win.h);
            setting_uipara.lyr_scn.w = 720;//480;
            setting_uipara.lyr_scn.h = 480;//245;
            setting_uipara.item.w = 410;
            setting_uipara.item.h = 106;
            setting_uipara.item.x = 720 - setting_uipara.item.w;
            setting_uipara.item.y = 0;
#if 1
            setting_uipara.item_general_setting.w = 300;
            setting_uipara.item_general_setting.h = 106;
            setting_uipara.item_general_setting.x = 0;
            setting_uipara.item_general_setting.y = 0;
            setting_uipara.item_power_manager.w = 300;
            setting_uipara.item_power_manager.h = 106;
            setting_uipara.item_power_manager.x = 0;
            setting_uipara.item_power_manager.y = 106;
            setting_uipara.item_alarm1_setting.w = 300;
            setting_uipara.item_alarm1_setting.h = 106;
            setting_uipara.item_alarm1_setting.x = 0;
            setting_uipara.item_alarm1_setting.y = 0;
            setting_uipara.item_alarm2_setting.w = 300;
            setting_uipara.item_alarm2_setting.h = 106;
            setting_uipara.item_alarm2_setting.x = 0;
            setting_uipara.item_alarm2_setting.y = 106;
            setting_uipara.item_alarm3_setting.w = 300;
            setting_uipara.item_alarm3_setting.h = 106;
            setting_uipara.item_alarm3_setting.x = 0;
            setting_uipara.item_alarm3_setting.y = 106 * 2;
#endif
            setting_uipara.left_button.x = 490 - (800 - 509) - 80;
            setting_uipara.left_button.y = setting_uipara.item.y + 30;
            setting_uipara.left_button.w = 46;
            setting_uipara.left_button.h = 46;
            setting_uipara.right_button.x   = 490 - (800 - 724) - 80;
            setting_uipara.right_button.y   = setting_uipara.item.y + 30;
            setting_uipara.right_button.w   = 46;
            setting_uipara.right_button.h   = 46;
            setting_uipara.txt_pos.x = 15;
            setting_uipara.txt_pos.y = 105;
            setting_uipara.string_detail_pos_ex.x = 0;
            setting_uipara.string_detail_pos_ex.y = 130;
            setting_uipara.string_detail_pos_ex.w = 130;
            setting_uipara.string_detail_pos_ex.h = 20;
            setting_uipara.string_detail_pos_de.x = 0;
            setting_uipara.string_detail_pos_de.y = 150;
            setting_uipara.string_detail_pos_de.w = 130;
            setting_uipara.string_detail_pos_de.h = 40;
            //如语言
            setting_uipara.item_title_pos.x = 20;
            setting_uipara.item_title_pos.y = setting_uipara.item.y;
            setting_uipara.item_title_pos.w = 180;
            setting_uipara.item_title_pos.h = setting_uipara.item.h;
            //如简体中文
            setting_uipara.item_content_pos.x = setting_uipara.left_button.x + setting_uipara.left_button.w;
            setting_uipara.item_content_pos.y = setting_uipara.item.y;
            setting_uipara.item_content_pos.w = (setting_uipara.right_button.x - setting_uipara.item_content_pos.x);
            setting_uipara.item_content_pos.h = setting_uipara.item.h;
            setting_uipara.return_bmp.x = 87;
            setting_uipara.return_bmp.y = 31;
            setting_uipara.return_bmp.w = 34;
            setting_uipara.return_bmp.h = 30;
            setting_uipara.long_string_area.x = 154 ;
            setting_uipara.long_string_area.y = setting_uipara.item_title_pos.y + 8 ;
            setting_uipara.long_string_area.w = 144 ;
            setting_uipara.long_string_area.h = 32 ;
            setting_uipara.setting_tips_uipara.tips_lyr.x = (720 - 320) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.y = (480 - 206) / 2;
            setting_uipara.setting_tips_uipara.tips_lyr.w = 320 ;
            setting_uipara.setting_tips_uipara.tips_lyr.h = 206 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.x = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.y = 0 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.w = 320 ;
            setting_uipara.setting_tips_uipara.sys_info_bmp_pos.h = 206 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.x = (320 - 140) / 2;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.y = 156 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.w = 140 ;
            setting_uipara.setting_tips_uipara.ok_bmp_pos.h = 38 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.x = (320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.y = 30 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_tsize_pos.h = 30 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.x = (320 - 140) / 2 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.y = 60 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.w = 200 ;
            setting_uipara.setting_tips_uipara.tip_fsize_pos.h = 30 ;
        }
        break;

        default :
            break;
    }

    return &setting_uipara;
}

#endif


