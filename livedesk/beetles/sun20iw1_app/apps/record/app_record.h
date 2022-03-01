/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
* File      :   music.h
* By        :   Kingvan
* Func  :
* Version   :   v1.0
* ============================================================================================================
* 2010-5-3 15:24:52  Kingvan  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __APP_RECORD_H__
#define __APP_RECORD_H__


#include "record_view.h"
#include "record_list_view.h"
#include "record_list_view_doc.h"
#include "record_view_doc.h"


//#define APP_RECORD                    "app_record"
//#define APP_RECORD_ID             APP_FM_ID//0x9001
#define APP_RECORD_VIEW_ID          (APP_RECORD_ID+1)
#define APP_RECORD_LIST_VIEW_ID     (APP_RECORD_ID+2)
#define APP_RECORD_DISK_VIEW_ID     (APP_RECORD_ID+3)



#define RECORD_TO_HOME                  (GUI_MSG_USER_DEF+0+10)
#define RECORD_TO_LIST_VIEW_FRAME       (GUI_MSG_USER_DEF+1+10)
#define RECORD_TO_VIEW_FRAME            (GUI_MSG_USER_DEF+2+10)
#define RECORD_VIEW_FRAME_STARTRECORD   (GUI_MSG_USER_DEF+3+10)
#define RECORD_VIEW_FRAME_PAUSERECORD   (GUI_MSG_USER_DEF+4+10)
#define RECORD_VIEW_FRAME_STOPRECORD    (GUI_MSG_USER_DEF+5+10)

#define RECORD_LIST_VIEW_FRAME_BACK     (GUI_MSG_USER_DEF+6+10)
#define RECORD_LIST_VIEW_FRAME_PRE      (GUI_MSG_USER_DEF+7+10)
#define RECORD_LIST_VIEW_FRAME_NEXT     (GUI_MSG_USER_DEF+8+10)
#define RECORD_LIST_VIEW_FRAME_PLAY     (GUI_MSG_USER_DEF+9+10)
#define RECORD_LIST_VIEW_FRAME_PAUSE    (GUI_MSG_USER_DEF+10+10)
#define RECORD_LIST_VIEW_FRAME_DELETE   (GUI_MSG_USER_DEF+11+10)
#define RECORD_LIST_VIEW_FRAME_CONTINUE (GUI_MSG_USER_DEF+12+10)

#define RECORD_ROOT_TYPE                RAT_TF

typedef enum RECORD_FRM_TYPE
{
    RECORD_NONE,
    TO_FM,
    MAIN_HOME,
    RECORD_FRM,
    RECORD_LIST_FRM
} RECORD_FRM_TYPE;

typedef struct tag_record_ctrl
{
    RECORD_FRM_TYPE     curOpenFrmType;
    H_WIN               frm;
    H_WIN               alarmfrm;

    record_ui_t         rec_ui;
    record_view_t       rec_view;
    record_list_view_t  rec_listView;
    record_list_doc_t   rec_listDoc;
    record_doc_t        rec_Doc;

    __u32               disks;
    __u32               root_type;
    __u32               root_type_ch;
    __bool              bIsFMRecord;
    H_LYR               layer;
    H_LYR               record_list_layer;
    GUI_FONT            *pfont;
} record_ctrl_t, *precord_ctrl_t;



extern H_WIN app_record_create(root_para_t  *para);

extern __s32 app_record_notify_delete_sub_scene(H_WIN hmanwin);










#include "record_view_frame.h"
#include "record_list_view_frame.h"
#include "record_disk_view_frame.h"

#endif//__APP_RECORD_H__
