#include <log.h>
#include "app_multi_screen_home.h"
#include "desktop_scene.h"

static uint32_t home_root_check_disk(uint32_t root_type[2]) //chengf
{
    int32_t     i;
    int32_t     ret;
    char        disk_name[RAT_MAX_PARTITION][4];

    root_type[0]    = RAT_UNKNOWN;  //USB
    root_type[1]    = RAT_UNKNOWN;  //SD
    ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);

    if (EPDK_OK == ret)
    {
        //while (i < RAT_MAX_PARTITION)
        i = 0;

        if (disk_name[i][0])
        {
            __wrn("=========== USB : diskname[%d]=%s", i, disk_name[i]);
            root_type[0] = RAT_USB;
        }

        i++;
    }

    ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);

    if (EPDK_OK == ret)
    {
        //while (i < RAT_MAX_PARTITION)
        i = 0;

        if (disk_name[i][0])
        {
            __wrn("=========== SD : diskname[%d]=%s", i, disk_name[i]);
            root_type[1] = RAT_TF;
        }

        i++;
    }

    return EPDK_OK; //root_type;
}

pmulti_screen_t get_multi_screen(void)
{
    reg_system_para_t *sys = NULL;

    sys = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    return &sys->multi_screen_para;
}

int32_t app_home_cmd2parent(H_WIN hwin, __u16 id, uint32_t data2, uint32_t reserved)
{
    __gui_msg_t     msg;

    msg.h_deswin    = GUI_WinGetParent(hwin);
    msg.h_srcwin    = NULL;
    msg.id          = GUI_MSG_COMMAND;
    msg.dwAddData1  = id;
    msg.dwAddData2  = data2;
    msg.dwReserved  = reserved;
    return GUI_SendNotifyMessage(&msg);
}
int32_t app_home_cmd2win(H_WIN hwin, __u16 id, uint32_t data2, uint32_t reserved)
{
    __gui_msg_t     msg;

    msg.h_deswin    = hwin;
    msg.h_srcwin    = NULL;
    msg.id          = GUI_MSG_COMMAND;
    msg.dwAddData1  = id;
    msg.dwAddData2  = data2;
    msg.dwReserved  = reserved;
    return GUI_SendNotifyMessage(&msg);
}

static int32_t app_home_cmd2root(H_WIN hwin, int32_t id, int32_t data2, int32_t reserved)
{
    H_WIN           hparent;
    __gui_msg_t     msg;

    hparent = GUI_WinGetParent(hwin);

    if (!hparent)
    {
        __err("hparent is null...");
        return EPDK_FAIL;
    }

    msg.h_deswin = hparent;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;
    GUI_SendNotifyMessage(&msg);
    return EPDK_OK;
}
static __bool _home_has_external_disk(void)
{
    int32_t     ret;
    char        diskname[RAT_MAX_PARTITION][4] = {{0},};

    ret     = rat_get_partition_name(RAT_USB_DISK, diskname, 0);

    if (EPDK_OK == ret)
    {
        if (diskname[0][0])
        {
            __wrn("======diskname[0][0] = %c =====", diskname[0][0]);
            return RAT_USB;
        }
        else
        {
            __wrn("diskname null");
        }
    }

    ret = rat_get_partition_name(RAT_SD_CARD, diskname, 0);

    if (EPDK_OK == ret)
    {
        if (diskname[0][0])
        {
            __wrn("======diskname[0][0] = %c =====", diskname[0][0]);
            return RAT_TF;
        }
        else
        {
            __wrn("diskname null");
        }
    }

    return EPDK_FALSE;
}

static __bool _home_has_encrypt_card(void)
{
    int32_t ret;
    char    diskname[RAT_MAX_PARTITION][4];
    
#if 0
    {
        __target_para_t target;
        __err("err:esKSRV_GetTargetPara the API fail..the bug is waiting to fix");
       // esKSRV_GetTargetPara(&target);

        if (target.debugenable == EPDK_TRUE)
        {
            return EPDK_TRUE;  //112350 如果是调试，则默认为有外设
        }
    }
    ret = rat_get_partition_name(RAT_USB_DISK, diskname, 0);

    if (EPDK_OK == ret)
    {
        if (diskname[0][0])
        {
            return EPDK_TRUE;
        }
    }

#endif
    ret     = rat_get_partition_name(RAT_SD_CARD, diskname, 0);

    if (EPDK_OK == ret)
    {
        if (diskname[0][0])
        {
            return EPDK_TRUE;
        }
    }

    return EPDK_FALSE;
}

static int32_t _home_destory_all_scene(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    pmulti_screen_home_attr     = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_OK;
    }

    if (pmulti_screen_home_attr->alarmfrm)
    {
        app_alarm_frm_destory(pmulti_screen_home_attr->alarmfrm);
        pmulti_screen_home_attr->alarmfrm = NULL;
    }
    if (pmulti_screen_home_attr->desktopfrm)
    {
        desktop_scene_destory(pmulti_screen_home_attr->desktopfrm);      
        pmulti_screen_home_attr->desktopfrm = NULL;
    }

    if (pmulti_screen_home_attr->headbarfrm)
    {
        //headbar_scene_destory(pmulti_screen_home_attr->headbarfrm);
        //pmulti_screen_home_attr->headbarfrm = NULL;
    }

    if (pmulti_screen_home_attr->unitfrm)
    {
        //headbar_scene_destory(pmulti_screen_home_attr->unitfrm);
        //pmulti_screen_home_attr->unitfrm = NULL;
    }

    uninit_multi_screen_res(&pmulti_screen_home_attr->ui);

    pmulti_screen_home_attr->multi_screen_is_exist = EPDK_FALSE;

    return EPDK_OK;
}

static int32_t _home_on_create(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    pmulti_screen_home_attr     = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        __msg("get pmulti_screen_home_attr is failed");
        return EPDK_FAIL;
    }

    home_root_check_disk(pmulti_screen_home_attr->root_type);

    __wrn("home check disk:root_type[0]=%d,root_type[1]=%d", pmulti_screen_home_attr->root_type[0], pmulti_screen_home_attr->root_type[1]);
    __msg("init res is begin");

    pmulti_screen_home_attr->cur_desk_app_index = dsktop_scene_get_cur_app();

    init_multi_screen_res(&pmulti_screen_home_attr->ui, pmulti_screen_home_attr->cur_desk_app_index);
    GUI_Memdev_FrameBuffer_Release(1);
    pmulti_screen_home_attr->desktopfrm = (H_WIN)desktop_scene_create(msg->h_deswin);

    GUI_WinSetFocusChild(pmulti_screen_home_attr->desktopfrm);	

    pmulti_screen_home_attr->headbarfrm             = NULL;
    pmulti_screen_home_attr->unitfrm                = NULL;
    pmulti_screen_home_attr->alarmfrm               = NULL;
    pmulti_screen_home_attr->multi_screen_is_exist  = EPDK_TRUE;

    gscene_hbar_set_state(HBAR_ST_SHOW);

    {
        char title[256] = {0};

        eLIBs_memset(title, 0, sizeof(title));

        dsk_langres_get_menu_text(STRING_HOME_TITLE, title, sizeof(title));

        gscene_hbar_set_title(title, sizeof(title));
    }

    return EPDK_OK;
}

static int32_t _home_on_close(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    pmulti_screen_home_attr     = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_OK;
    }

    GUI_ManWinDelete(msg->h_deswin);

    GUI_WinSetFocusChild(msg->h_deswin);

    return EPDK_OK;
}
static int32_t _home_on_destory(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_OK;
    }
    
    if(pmulti_screen_home_attr->dlg)
    {
        app_dialog_destroy(pmulti_screen_home_attr->dlg);
    }

    _home_destory_all_scene(msg);

    esMEMS_Mfree(0, pmulti_screen_home_attr);

    GUI_WinSetAttr(msg->h_deswin, NULL);

    gscene_hbar_set_state(HBAR_ST_HIDE);        // hbar

    return EPDK_OK;
}

static uint8_t _home_check_media_exist(__gui_msg_t *msg, uint8_t external_disk_type)
{
    HRAT        handle;
    int32_t     total;
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;
    rat_media_type_t    media_type;
    int32_t file_emtpy[] = { 0,
                           STRING_EXPLR_PHOTO_EMPTY,
                           STRING_EXPLR_MUSIC_EMPTY,
                           STRING_EXPLR_VEDIO_EMPTY,
                           STRING_EXPLR_EBOOK_EMPTY
                         };

    pmulti_screen_home_attr     = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    switch (msg->dwAddData2)
    {
        case ID_HOME_PICTURE:
            media_type = RAT_MEDIA_TYPE_PIC;
            break;

        case ID_HOME_MUSIC:
            media_type = RAT_MEDIA_TYPE_AUDIO;
            break;

        case ID_HOME_EBOOK:
            media_type = RAT_MEDIA_TYPE_EBOOK;
            break;

        case ID_HOME_MOVIE:
            media_type = RAT_MEDIA_TYPE_VIDEO;
            break;

        default:
            return EPDK_OK;
    }

    handle  = rat_open_ex(external_disk_type, media_type, 0);

    if (handle == NULL)
    {
        return EPDK_FAIL;
    }

    total   = rat_get_cur_scan_cnt(handle);

    if (total <= 0)
    {
        H_WIN       h_dialog = NULL;
        int32_t     lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY};

        lang_id[1]  = file_emtpy[media_type];

        default_dialog_timeout(pmulti_screen_home_attr->dlg, msg->h_deswin, DLG_SCENE_FRM_ID,  ADLG_OK, lang_id, 200);
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
static int32_t _home_on_restore(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    __log("enter %s", __FUNCTION__);

    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (EPDK_TRUE == pmulti_screen_home_attr->multi_screen_is_exist)
    {
        __wrn("home alreadly forground...");
        return EPDK_FAIL;
    }

    gscene_bgd_set_state(BGD_STATUS_SHOW);

    pmulti_screen_home_attr->cur_desk_app_index = dsktop_scene_get_cur_app();

    init_multi_screen_res(&pmulti_screen_home_attr->ui, pmulti_screen_home_attr->cur_desk_app_index);

    pmulti_screen_home_attr->desktopfrm     = (H_WIN)desktop_scene_create(msg->h_deswin);

    pmulti_screen_home_attr->headbarfrm     = NULL;
    pmulti_screen_home_attr->unitfrm        = NULL;
    pmulti_screen_home_attr->alarmfrm       = NULL;
    pmulti_screen_home_attr->multi_screen_is_exist = EPDK_TRUE;

    GUI_WinSetFocusChild(pmulti_screen_home_attr->desktopfrm);

    gscene_hbar_set_state(HBAR_ST_SHOW);
    {
        char    title[256] = {0};

        eLIBs_memset(title, 0, sizeof(title));

        dsk_langres_get_menu_text(STRING_HOME_TITLE, title, sizeof(title));

        gscene_hbar_set_title(title, sizeof(title));
    }
    return EPDK_OK;
}

static int32_t _home_on_command(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_FAIL;
    }

    switch (msg->dwAddData1)
    {
        case DESKTOP_REFRESH_MSG:
        {
            __gui_msg_t my_msg;

            eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));

            my_msg.h_deswin     = pmulti_screen_home_attr->desktopfrm;
            my_msg.h_srcwin     = NULL;
            my_msg.id           = GUI_MSG_COMMAND;
            my_msg.dwAddData1   = msg->dwAddData1;
            my_msg.dwAddData2   = msg->dwAddData2;
            my_msg.dwReserved   = 0;

            __log("%s %d msg->dwAddData2:%d", __FILE__, __LINE__, msg->dwAddData2);
            GUI_SendMessage(&my_msg);
        }

        return EPDK_OK;

        case EXIT_MULTI_SCREEB_HOME_TO_AP_MSG:
        {
            __log("++++EXIT_MULTI_SCREEB_HOME_TO_AP_MSG+++");
            __log("&&&&&&&&&&&&&&msg->dwAddData1=%d, msg->dwAddData2=%d", msg->dwAddData1, msg->dwAddData2);

            pmulti_screen_home_attr->cur_desk_app_index = msg->dwAddData2;

            if (ID_HOME_PICTURE == msg->dwAddData2 || ID_HOME_RECORD 	== msg->dwAddData2 ||
                ID_HOME_MUSIC 	== msg->dwAddData2 || ID_HOME_MOVIE 	== msg->dwAddData2 || 
                ID_HOME_EBOOK 	== msg->dwAddData2 || ID_HOME_EXPLORER  == msg->dwAddData2 )
            {
                uint8_t external_disk_type  = _home_has_external_disk();

                if (EPDK_FALSE == external_disk_type) //no disk
                {
                    HBarState hbar_sta = HBAR_ST_HIDE;
                    int32_t lang_id[] = {STRING_HOME_CUE, STRING_HOME_NULL_DISK};

                    __log("_multi_screen_home_no_external_disk");
                    default_dialog_timeout(pmulti_screen_home_attr->dlg, msg->h_deswin, DLG_SCENE_FRM_ID, ADLG_OK, lang_id, 200);
                }
                else
                {
                    //__log("%s %d external_disk_type:%d", __FILE__, __LINE__, external_disk_type);
                    if (_home_check_media_exist(msg, external_disk_type) != EPDK_OK)
                    {
                        return EPDK_FAIL;
                    }
__log("sarbafe....EXIT_MULTI_SCREEB_HOME_TO_AP_MSG");
                    _home_destory_all_scene(msg);
                    home_root_check_disk(pmulti_screen_home_attr->root_type);

                    if ((RAT_USB == pmulti_screen_home_attr->root_type[0]) && (RAT_TF == pmulti_screen_home_attr->root_type[1]))
                    {
                        __wrn("usb and sd all plug in,GetCurrentDevTypeEx = %d", GetCurrentDevTypeEx());

                        if (RAT_TF == GetCurrentDevTypeEx())
                        {
                            app_home_cmd2root(msg->h_deswin, SWITCH_TO_OTHER_APP, msg->dwAddData2, RAT_TF);
                        }
                        else if (RAT_USB == GetCurrentDevTypeEx())
                        {
                            app_home_cmd2root(msg->h_deswin, SWITCH_TO_OTHER_APP, msg->dwAddData2, RAT_USB);
                        }
                        else
                        {
                            __wrn("send SWITCH_TO_OTHER_APP:GetCurrentDevTypeEx = null ");
                            app_home_cmd2root(msg->h_deswin, SWITCH_TO_OTHER_APP, msg->dwAddData2, RAT_TF);
                        }
                    }
                    else if ((RAT_USB == pmulti_screen_home_attr->root_type[0]) && (RAT_TF != pmulti_screen_home_attr->root_type[1]))
                    {
                        app_home_cmd2root(msg->h_deswin, SWITCH_TO_OTHER_APP, msg->dwAddData2, RAT_USB);
                    }
                    else if ((RAT_USB != pmulti_screen_home_attr->root_type[0]) && (RAT_TF == pmulti_screen_home_attr->root_type[1]))
                    {
                        app_home_cmd2root(msg->h_deswin, SWITCH_TO_OTHER_APP, msg->dwAddData2, RAT_TF);
                    }
                    else
                    {
                        __wrn("send SWITCH_TO_OTHER_APP:no disk ");
                    }
                }
            }
            else if (ID_HOME_FM == msg->dwAddData2 || ID_HOME_SETTING == msg->dwAddData2 || ID_HOME_DTV == msg->dwAddData2)
            {
                __log(" %d home SWITCH_TO_OTHER_APP",msg->dwAddData2);
                
                _home_destory_all_scene(msg);  

                app_home_cmd2root(msg->h_deswin, SWITCH_TO_OTHER_APP, msg->dwAddData2, RAT_TF);                
            }
        }

        return EPDK_OK;

        default:
        {
            __log("MSG:L%d(%s) 0x%x: 0x%x", __LINE__, __FILE__, LOSWORD(msg->dwAddData1), HISWORD(msg->dwAddData1));

            if (LOSWORD(msg->dwAddData1) == DLG_SCENE_FRM_ID && HISWORD(msg->dwAddData1) == ADLG_CMD_EXIT)
            {
                app_dialog_destroy(pmulti_screen_home_attr->dlg);
                pmulti_screen_home_attr->dlg = NULL;
            }
        }
        break;
    }

    return EPDK_OK;
}

static int32_t _home_on_alarm_coming(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t pmulti_screen_home_attr;
    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_OK;
    }

    if (pmulti_screen_home_attr->unitfrm != NULL)
    {
        //  unit_scene_destory(pmulti_screen_home_attr->unitfrm);
        //  pmulti_screen_home_attr->unitfrm = NULL;
    }

    if (EPDK_TRUE == pmulti_screen_home_attr->multi_screen_is_exist)
    {
        gscene_hbar_set_state(HBAR_ST_HIDE);
    }

    if (pmulti_screen_home_attr->alarmfrm == NULL && pmulti_screen_home_attr->multi_screen_is_exist == EPDK_TRUE)
    {
        pmulti_screen_home_attr->alarmfrm = app_alarm_frm_create(msg->h_deswin);
    }

    return EPDK_OK;
}

static int32_t _home_on_alarm_closed(__gui_msg_t *msg)
{
    pmulti_screen_home_attr_t pmulti_screen_home_attr;
    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

    if (pmulti_screen_home_attr == NULL)
    {
        return EPDK_OK;
    }

    pmulti_screen_home_attr->alarmfrm = NULL;
    gscene_hbar_set_state(HBAR_ST_SHOW);
    return EPDK_OK;
}

static int32_t _home_manager_win_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __inf("%s %d _home_on_create", __FILE__, __LINE__);
            return _home_on_create(msg);
        }

        case ALARM_MSG_CLOSE:
            return _home_on_alarm_closed(msg);

        case DSK_MSG_ALARM:
            return _home_on_alarm_coming(msg);

        case GUI_MSG_CLOSE:
            __err("app GUI_MSG_CLOSE");                        
            return _home_on_close(msg);

        case GUI_MSG_DESTROY:
            __err("app GUI_MSG_DESTROY");            
            return _home_on_destory(msg);

        case DSK_APP_RESTORE:
            __err("app DSK_APP_RESTORE");            
            return _home_on_restore(msg);

        case GUI_MSG_COMMAND:
            return _home_on_command(msg);

        case GUI_MSG_KEY:
            break;

        case GUI_MSG_TOUCH:
            break;

        case DSK_MSG_FS_PART_PLUGIN:
        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __gui_msg_t                 msgex;
            pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

            pmulti_screen_home_attr     = (pmulti_screen_home_attr_t)GUI_WinGetAttr(msg->h_deswin);

            if (pmulti_screen_home_attr == NULL)
            {
                return EPDK_OK;
            }

            if (pmulti_screen_home_attr->desktopfrm)
            {
                eLIBs_memset(&msgex, 0x00, sizeof(__gui_msg_t));
                eLIBs_memcpy(&msgex, msg, sizeof(__gui_msg_t));
                msgex.h_deswin = pmulti_screen_home_attr->desktopfrm;
                GUI_SendNotifyMessage(&msgex);
            }

            return EPDK_OK;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN app_home_create(root_para_t  *para)
{
    reg_system_para_t           *system_para = NULL;
    __gui_manwincreate_para_t   create_info;
    pmulti_screen_home_attr_t   pmulti_screen_home_attr = NULL;

    __inf("****************************************************************************************");
    __log("********  enter multi screen home application  **************");
    __inf("****************************************************************************************");

    gscene_hbar_set_state(HBAR_ST_HIDE);    //先hide ，创建完主界面后再show

    gscene_bgd_set_state(BGD_STATUS_SHOW);

    system_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    dsk_display_set_lcd_bright(system_para->lcd_bright * 10);

    dsk_display_set_lcd_constract(system_para->lcd_constract * 10);

    dsk_display_set_lcd_saturation(system_para->lcd_saturation * 10);

    dsk_display_set_lcd_hue(system_para->lcd_hue * 10);

    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)esMEMS_Malloc(0, sizeof(multi_screen_home_attr_t));

    if (pmulti_screen_home_attr == NULL)
    {
        __msg("esMEMS_Malloc fail");
        return NULL;
    }

    eLIBs_memset(pmulti_screen_home_attr, 0, sizeof(multi_screen_home_attr_t));

    pmulti_screen_home_attr->pfont  = para->font;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.name            = APP_MULTI_SCREEN_HOME;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_home_manager_win_proc);
    create_info.attr            = (void *)pmulti_screen_home_attr;
    create_info.id              = APP_MULTI_SCREEN_HOME_ID;
    create_info.hHosting        = NULL;

    return (GUI_ManWinCreate(&create_info));
}

multi_screen_home_mode_e app_home_get_mode(H_WIN hwin)
{
#if 0
    pmulti_screen_home_attr_t pmulti_screen_home_attr = NULL;
    pmulti_screen_home_attr = (pmulti_screen_home_attr_t)GUI_WinGetAttr(hwin);

    if (pmulti_screen_home_attr == NULL)
    {
        return MULTI_SCREEN_HOME_MODE_INVALID;
    }

    if (EPDK_TRUE == pmulti_screen_home_attr->multi_screen_is_exist)
    {
        return MULTI_SCREEN_HOME_MODE_NORMAL;
    }
    else
    {
        return MULTI_SCREEN_HOME_MODE_BACKGROUND;
    }

#endif
    return MULTI_SCREEN_HOME_MODE_NORMAL;
}
