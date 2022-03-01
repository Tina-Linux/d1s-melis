


#ifndef __EXPLORER_H__
#define __EXPLORER_H__

#include "beetles_app.h"

/*
typedef enum explorer_cmd_s
{
    EXPLORER_CMD_HOME = 0,
    EXPLORER_CMD_COPY,
    EXPLORER_CMD_PASTE,
    EXPLORER_CMD_CANCEL_PASTE,
    EXPLORER_CMD_DELETE,
    EXPLORER_CMD_CANCEL_DELETE,
    EXPLORER_CMD_SORT,
    EXPLORER_CMD_UP_PAGE,
    EXPLORER_CMD_DOWN_PAGE,
    EXPLORER_CMD_RETURN,
    EXPLORER_CMD_LIST_MENU,
    EXPLORER_CMD_SQUARE_MENU,
    EXPLORER_CMD_ENTER_DIR,
    EXPLORER_CMD_RETURN_DIR,
    EXPLORER_CMD_PLAY_FILE,
    EXPLORER_CMD_ENTER_MEDIALIB,
    EXPLORER_CMD_UPDATE_TOOLBAR,
    EXPLORER_CMD_SEARCH,
    EXPLORER_CMD_CANCEL_SEARCH,
    EXPLORER_CMD_SET_BROWSER_FOCUS,
    EXPLORER_CMD_SET_TOOLBAR_FOCUS,
    EXPLORER_CMD_PLUG_IN,
    EXPLORER_CMD_PLUG_OUT,
    EXPLORER_CMD_NULL,
}explorer_cmd_e;
*/
#define EXPLORER_CMD_HOME           1
#define EXPLORER_CMD_RETURN         2
#define EXPLORER_CMD_PLAY_FILE      3


typedef enum explorer_view_mode_s
{
    EXPLR_LIST_MODE = 10000,
    EXPLR_SQUARE_MODE = 10001,
} explorer_view_mode_e;


typedef struct explorer_param_s
{
    Activity *activity;
    H_WIN manager_win;
    void *browser_scene;
    explorer_view_mode_e view_mode;
    __u32 is_medialib_mode;

} explorer_param_t;

#define C_EXPLR_WIDTH       800
#define C_EXPLR_HEIGHT      480



//#define APP_EXPLORER_ID       0x2001
#define EXPLR_LIST_WIN_ID           (APP_EXPLORER_ID+1)
#define EXPLR_SQUARE_WIN_ID         (APP_EXPLORER_ID+2)
#define DELETE_FILE_DIALOG_ID       (APP_EXPLORER_ID+3)
#define FW_UPDATE_DIALOG_ID         (APP_EXPLORER_ID+4)
#define ENTER_SELECT_ITEM_ID        (APP_EXPLORER_ID+5)
#define SELECT_UP_ITEM_ID           (APP_EXPLORER_ID+6)
#define SELECT_DOWN_ITEM_ID         (APP_EXPLORER_ID+7)
#define EXPLR_DIALOG_WIN_ID         (APP_EXPLORER_ID+8)


typedef enum explr_dialog_cmd_s
{

    EXPLORER_DIALOG_COPY = EXPLR_DIALOG_WIN_ID + 1,
    EXPLORER_DIALOG_PARSE,
    EXPLORER_DIALOG_CUT,
    EXPLORER_DIALOG_DEL,
    EXPLORER_DIALOG_CANCEL,

    EXPLORER_DIALOG_CREATE,

    EXPLORER_DIALOG_,

} explr_dialog_cmd_t;



signed long app_explorer_create(root_para_t  *para);
__s32 explorer_dialog_create(__gui_msg_t *msg);


#endif






















