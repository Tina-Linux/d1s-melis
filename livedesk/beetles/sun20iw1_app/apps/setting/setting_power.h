/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : setting_general.h
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __SETTING_POWER_H_
#define __SETTING_POWER_H_

#include "app_setting_i.h"
typedef struct tag_setting_power_para
{
    GUI_FONT *font;

    H_LYR layer;
    __s32 focus_id;
    __s32 old_focus;

} setting_power_para_t;

typedef enum
{
    BLACKLIGHT_ITEM_RES_ID = 0,
    SCREEN_OFF_ITEM_RES_ID,
    POWER_OFF_ITEM_RES_ID,

    POWER_ITEM_MAX_ID,
} power_res_item_id_e ;

typedef enum power_paint_index
{
    ID_PAINTING_INIT_POWER,
    ID_PAINTING_F_GENERAL,
} draw_power_index_e;


typedef enum
{
    //POWER_THEME_BG_BMP_ID  = 0 ,  // ±³¾°Í¼
    //POWER_THEME_SETTING_ICON_BMP_ID , //ÉèÖÃÍ¼±ê
    //POWER_THEME_RETURN_FCS_BMP_ID ,
    //POWER_THEME_RETURN_UNFCS_BMP_ID ,

    NEW_SET_FLAG_UF_BMP = 0,    // BYF
    NEW_SET_FLAG_F_BMP,     //
    NEW_POWER_FLAG_UF_BMP,
    NEW_POWER_FLAG_F_BMP,
    NEW_SCROLL_BG_BMP,
    NEW_SCROLL_BLOCK_BMP,
    NEW_ITEM_L_FCS_BMP,
    NEW_ITEM_L_UFCS_BMP,

    POWER_THEME_MAX_ID,
} power_res_theme_e ;

typedef struct tag_res_theme_s
{
    __u32   bmp_id ;        //Í¼Æ¬id
    HTHEME  bmp_handle ;    //Í¼Æ¬×ÊÔ´¾ä±ú
    void   *p_buf ;     //Í¼Æ¬×ÊÔ´buffer
} res_theme_t ;

H_WIN setting_power_win_create(H_WIN h_parent, setting_power_para_t *para);
__s32 setting_power_win_delete(H_WIN list_win);
#endif
