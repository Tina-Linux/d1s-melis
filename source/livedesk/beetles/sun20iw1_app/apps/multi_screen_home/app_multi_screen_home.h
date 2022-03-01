

#ifndef __APP_MULTI_SCREEN_HOME_H__
#define __APP_MULTI_SCREEN_HOME_H__

#include "app_multi_screen_home_i.h"
#include "multi_screen_ui.h"
//#include "desktop_scene.h"

#define MULTI_SCREEN_ENABLE_MOVE_TO_HEADBAR 0   //headbar  ø…“‘“∆∂Ø


#define APP_MULTI_SCREEN_HOME_ID                APP_HOME_ID//(APP_ALARM_ID+100)
#define APP_MULTI_SCREEN_HOME                   APP_HOME
#define DESKTOP_SCENE_FRM_ID                    (APP_MULTI_SCREEN_HOME_ID+1)
#define HEADBAR_SCENE_FRM_ID                    (APP_MULTI_SCREEN_HOME_ID+2)
#define UNIT_SCENE_FRM_ID                       (APP_MULTI_SCREEN_HOME_ID+3)
#define DLG_SCENE_FRM_ID                        (APP_MULTI_SCREEN_HOME_ID+4)

#define DIGITAL_FRM_ID                          (APP_MULTI_SCREEN_HOME_ID+5)



#define MULTI_SCREEN_FILTER_TP                  0

#define HEADBAR_HEIGHT                          25
#define HEADBAR_SCENE_HEIGHT                    70
#define HEADBAR_WIN_HEIGHT                      (HEADBAR_HEIGHT+HEADBAR_SCENE_HEIGHT)

#if MULTI_SCREEN_FILTER_TP
#define DESKTOP_SCENE_SWITCH_SCREEN_CNT         7
#else
#define DESKTOP_SCENE_SWITCH_SCREEN_CNT         16
#endif
#define DESKTOP_SCENE_SWITCH_SCREEN_X_SIZE      45

//msg/
#define LET_HEADBAR_SCENE_CHANGE_MSG            0
#define LET_HEADBAR_SCENE_ROLL_UP_MSG           1
#define LET_HEADBAR_ADD_UNIT_MSG                2
#define LET_UNIT_SCENE_CREATE_MSG               3
#define LET_UNIT_SCENE_DESTORY_MSG              4
#define LET_UNIT_SCENE_MOVING_MSG               5
#define LET_DESKTOP_SWITCH_SCREEB_MSG           6
#define DESKTOP_ADD_HEADBAR_UNIT_MSG            7
#define DESKTOP_ADD_HEADBAR_UNIT_OK_MSG         8
#define DESKTOP_ADD_HEADBAR_UNIT_FAIL_MSG       9
#define EXIT_MULTI_SCREEB_HOME_TO_AP_MSG        10
#define EXIT_MULTI_SCREEB_HOME_TO_SHORTCUT_MSG  11
#define LET_HEADBAR_READY_ADD_UNIT_MSG          12
#define LET_DESKTOP_REFRESH_VIEW_MSG            13
#define DESKTOP_REFRESH_MSG                     (14)


typedef enum
{
    MULTI_SCREEN_HOME_MODE_BACKGROUND = 0,
    MULTI_SCREEN_HOME_MODE_NORMAL,
    MULTI_SCREEN_HOME_MODE_INVALID,
} multi_screen_home_mode_e;

typedef struct tag_multi_screen_home_attr
{
    H_WIN               desktopfrm;
    H_WIN               headbarfrm;
    H_WIN               unitfrm;
    H_WIN               alarmfrm;
    H_WIN               dlg;

    __bool              multi_screen_is_exist;

    GUI_FONT            *pfont;

    multi_screen_ui_t   ui;

    __s16               cur_desk_app_index;

    __u32 root_type[2]; //root_type[0]:USB , root_type[1]:SD
} multi_screen_home_attr_t, *pmulti_screen_home_attr_t;



extern __s32  app_home_cmd2parent(H_WIN hwin, __u16 id, __u32 data2, __u32 reserved);
extern __s32  app_home_cmd2win(H_WIN hwin, __u16 id, __u32 data2, __u32 reserved);

extern H_WIN app_home_create(root_para_t *para);

extern multi_screen_home_mode_e app_home_get_mode(H_WIN hwin);
#endif//__APP_MULTI_SCREEN_HOME_H__
