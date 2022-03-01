/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dtv_settings.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
*
**************************************************************************************************************
*/
#include <log.h>
#include "dtv_setting.h"
#include "dtv_setting_i.h"
#include "dtv_setting_uipara.h"

#undef  KEY_NUM_DELAY
#define KEY_NUM_DELAY  300  /*3s*/
#define KEY_CA_DELAY   300  /*3s*/

extern dtv_ctr_t  dtv_ctr;

static __s32 _dtv_setting_Proc(__gui_msg_t *msg)
{
    dtv_setting_ctrl_t *dtv_setting_ctrl;
    dtv_setting_ctrl     = (dtv_setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!dtv_setting_ctrl)
    {
        __err("NULL\n");
        return EPDK_FAIL;
    }

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            dtv_setting_list_para_t list_para = {0};
            list_para.focus_id = 0;
            list_para.old_focus = 0;
            list_para.layer = dtv_setting_ctrl->layer;
            dtv_setting_ctrl->h_frm_list = dtv_setting_list_win_create(msg->h_deswin, &list_para);

            if (dtv_setting_ctrl->h_frm_list)
            {
                GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);
                GUI_SetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, 5000, NULL);
            }
            else
            {
                return EPDK_FAIL;
            }
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            //GUI_FrmWinDelete(dtv_setting_ctrl->h_frm_list);
            __gui_msg_t my_msg;
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.h_srcwin = NULL;
            my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            my_msg.dwAddData1 = DTV_BACK2PLAY_PROGRAM;
            my_msg.dwAddData2 = 0;
            GUI_SendNotifyMessage(&my_msg);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __gui_msg_t my_msg;
            /*if(dtv_setting_ctrl->h_frm_list)
            {
                dtv_setting_list_win_delete(dtv_setting_ctrl->h_frm_list);
                dtv_setting_ctrl->h_frm_list = 0;
            }*/
            GUI_KillTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID);

            if (dtv_ctr.h_lyr)
            {
                GUI_LyrWinSetSta(dtv_setting_ctrl->layer, GUI_LYRWIN_STA_SLEEP);
                GUI_LyrWinDelete(dtv_setting_ctrl->layer);
                dtv_setting_ctrl->layer = NULL;
                dtv_ctr.h_lyr = NULL ;
                dtv_setting_ctrl->h_frm_list = NULL;
            }

            __wrn("...\n");

            if (dtv_setting_ctrl)
            {
                My_Mfree(0, dtv_setting_ctrl);
                dtv_setting_ctrl = NULL;
            }

            eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.h_srcwin = NULL;
            my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            my_msg.dwAddData1 = DTV_CA_MSG_RESUME;
            my_msg.dwAddData2 = 0;
            GUI_SendNotifyMessage(&my_msg);
            //  esKSRV_SysInfo();
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            __msg("...GUI_MSG_KEY...\n");

            if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID))
            {
                if (dtv_setting_ctrl->focus_id == DTV_SETTING_CA_ID)
                {
                    GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, KEY_CA_DELAY, NULL);
                }
                else
                {
                    GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, KEY_NUM_DELAY, NULL);
                }
            }
        }
        break;

        case GUI_MSG_TIMER:
        {
            __msg("timer id = %d \n", msg->dwAddData1);

            //KEY_NUM_DELAY秒无操作时通知上层删除设置界面
            if (msg->dwAddData1 == SUBMENU_DVB_SETTING_TIMERID)
            {
                {
                    __gui_msg_t dmsg;
                    dmsg.id = GUI_MSG_COMMAND;
                    dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    dmsg.dwAddData1 = DTV_BACK2PLAY_PROGRAM ;
                    GUI_SendNotifyMessage(&dmsg);
                }
            }
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            GUI_LyrWinSel(dtv_setting_ctrl->layer);

            if (msg->dwAddData1 < DTV_SETTING_MAX)
            {
                dtv_setting_ctrl->focus_id = msg->dwAddData1;
            }

            GUI_FrmWinDelete(dtv_setting_ctrl->h_frm_list);
            dtv_setting_ctrl->h_frm_list = NULL;
            GUI_Clear();

            switch (msg->dwAddData1)
            {
                case DTV_SETTING_DISPLAY_ID:
                {
                    dtv_setting_disp_para_t list_para = {0};
                    list_para.focus_id = 0;
                    list_para.old_focus = 0;
                    list_para.layer = dtv_setting_ctrl->layer;
                    dtv_setting_ctrl->h_frm_list = dtv_setting_disp_win_create(msg->h_deswin, &list_para); //创建新的listbar
                    GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);

                    if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID))
                    {
                        GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, KEY_NUM_DELAY, NULL);
                    }

                    return EPDK_OK;
                }

                case DTV_SETTING_ALL_FREQ_SEARCH_ID:
                {
                    {
                        __gui_msg_t dmsg = {0};
                        GUI_Clear();
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = DTV_RESEARCH_SERVICE_LIST ;
                        GUI_SendNotifyMessage(&dmsg);
                    }
                    return EPDK_OK;
                }

#ifdef DTV_AUTO_SEARCH_ENABLE

                case DTV_SETTING_AUTO_SEARCH_ID:
                {
                    {
                        __gui_msg_t dmsg = {0};
                        GUI_Clear();
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = DTV_AUTO_RESEARCH_SERVICE_LIST ;
                        GUI_SendNotifyMessage(&dmsg);
                    }
                    return EPDK_OK;
                }

#endif

                case DTV_SETTING_MANU_SEARCH_ID:
                {
#if 0           //手动输入      
                    {
                        __gui_msg_t dmsg = {0};

                        GUI_Clear();
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = DTV_MENU_RESEARCH ;

                        GUI_SendNotifyMessage(&dmsg);
                    }
#else          //手动选择频点
                    {
                        dtv_setting_search_para_t list_para = {0};

                        list_para.focus_id = 0;
                        list_para.old_focus = 0;
                        list_para.layer = dtv_setting_ctrl->layer;
                        dtv_setting_ctrl->h_frm_list = dtv_setting_search_win_create(msg->h_deswin, &list_para); //创建新的listbar

                        GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);

                        if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID))
                        {
                            GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, KEY_NUM_DELAY, NULL);
                        }

                    }
#endif
                    return EPDK_OK;
                }

                case DTV_SETTING_CA_ID: //CA卡序列号
                {
                    static __s32 focus_id = 0;

                    if (msg->dwAddData2 == 0)
                    {
                        setting_ca_para_t ca_para = {0};
                        ca_para.focus_id = focus_id;
                        ca_para.layer = dtv_setting_ctrl->layer;
                        dtv_setting_ctrl->h_frm_list = dtv_setting_ca_win_create(msg->h_deswin, &ca_para);//创建新的frm win

                        if (dtv_setting_ctrl->h_frm_list != NULL)
                        {
                            __wrn("dtv_setting_ctrl->h_frm_list:0x%x\r\n", dtv_setting_ctrl->h_frm_list);
                            GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);
                        }
                        else
                        {
                            __wrn("create ca frmwin fail...\n");
                        }
                    }
                    else if (msg->dwAddData2 == 0 + 0x2008)//DTV_SETTING_CA_USER_INFO
                    {
                        setting_ca_sub_para_t ca_sub_para;
                        __wrn("create ca DTV_SETTING_CA_USER_INFO\n");
                        focus_id = 0;
                        eLIBs_memset(&ca_sub_para, 0x00, sizeof(ca_sub_para));
                        ca_sub_para.Current_item = 0;
                        ca_sub_para.layer = dtv_setting_ctrl->layer;
                        dtv_setting_ctrl->h_frm_list = dtv_setting_ca_sub_win_create(msg->h_deswin, &ca_sub_para);

                        if (dtv_setting_ctrl->h_frm_list != NULL)
                        {
                            __wrn("sub dtv_setting_ctrl->h_frm_list:0x%x\r\n", dtv_setting_ctrl->h_frm_list);
                            GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);
                        }
                        else
                        {
                            __wrn("create ca frmwin fail...\n");
                        }
                    }
                    else if (msg->dwAddData2 == 1 + 0x2008)//DTV_SETTING_CA_AUTHORIZATION
                    {
                        setting_ca_sub_para_t ca_sub_para;
                        __wrn("create ca DTV_SETTING_CA_AUTHORIZATION\n");
                        focus_id = 1;
                        eLIBs_memset(&ca_sub_para, 0x00, sizeof(ca_sub_para));
                        ca_sub_para.Current_item = 1;
                        ca_sub_para.layer = dtv_setting_ctrl->layer;
                        dtv_setting_ctrl->h_frm_list = dtv_setting_ca_sub_win_create(msg->h_deswin, &ca_sub_para);

                        if (dtv_setting_ctrl->h_frm_list != NULL)
                        {
                            __wrn("sub dtv_setting_ctrl->h_frm_list:0x%x\r\n", dtv_setting_ctrl->h_frm_list);
                            GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);
                        }
                        else
                        {
                            __wrn("create ca frmwin fail...\n");
                        }
                    }
                    else if (msg->dwAddData2 == 2 + 0x2008) //DTV_SETTING_CA_EIGEN
                    {
                        setting_ca_sub_para_t ca_sub_para;
                        __wrn("create ca DTV_SETTING_CA_EIGEN\n");
                        focus_id = 2;
                        eLIBs_memset(&ca_sub_para, 0x00, sizeof(ca_sub_para));
                        ca_sub_para.Current_item = 2;
                        ca_sub_para.layer = dtv_setting_ctrl->layer;
                        dtv_setting_ctrl->h_frm_list = dtv_setting_ca_sub_win_create(msg->h_deswin, &ca_sub_para);

                        if (dtv_setting_ctrl->h_frm_list != NULL)
                        {
                            __wrn("sub dtv_setting_ctrl->h_frm_list:0x%x\r\n", dtv_setting_ctrl->h_frm_list);
                            GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);
                        }
                        else
                        {
                            __wrn("create ca frmwin fail...\n");
                        }
                    }

                    if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID))
                    {
                        GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, KEY_CA_DELAY, NULL);
                    }

                    return EPDK_OK;
                }

                case DTV_SETTING_EPG:
                {
                    {
                        __s32 total = 0;

                        if (maple_get_cur_search_freq_tatol() > 0)
                        {
                            maple_epg_getdata();
                            total = maple_epg_get_eventnum(0);

                            if (0 >= total)
                            {
                                esKRNL_TimeDly(100);
                                total = maple_epg_get_eventnum(0);
                            }
                        }

                        if (total <= 0)
                        {
                            dtv_setting_disp_para_t list_para = {0};
                            {
                                H_WIN dlg;
                                __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_NO_EPG};
                                default_dialog(dlg, msg->h_deswin, SUBMENU_DVB_SETTING_DLG_ID, ADLG_OK, lang_id);
                                esKRNL_TimeDly(150);
                                app_dialog_destroy(dlg);
                            }
                            {
                                __gui_msg_t dmsg = {0};
                                dmsg.h_deswin = msg->h_deswin;
                                dmsg.id       = GUI_MSG_COMMAND;
                                dmsg.dwAddData1 = DTV_SETTING_BACK2LIST; //DTV_SETTING_BACK2LIST;
                                GUI_SendNotifyMessage(&dmsg);
                            }
                        }
                        else
                        {
                            //dtv_setting_ctrl->h_frm_list = dtv_schedule_create(msg->h_deswin,dtv_setting_ctrl->layer);
                            {
                                __gui_msg_t dmsg = {0};
                                dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                                dmsg.id       = GUI_MSG_COMMAND;
                                dmsg.dwAddData1 = DTV_2_EPG; //DTV_SETTING_BACK2LIST;
                                GUI_SendNotifyMessage(&dmsg);
                                eLIBs_printf("SEND 2 DTV_2_EPG\n");
                            }
                        }

                        return EPDK_OK;
                    }
                }

                case DTV_SETTING_WIRELESS_ID:
                {
                    dtv_setting_antenna_para_t list_para = {0};
                    list_para.focus_id = maple_get_antenna_mode();
                    list_para.old_focus = maple_get_antenna_mode();
                    list_para.layer = dtv_setting_ctrl->layer;
                    dtv_setting_ctrl->h_frm_list = dtv_setting_antenna_win_create(msg->h_deswin, &list_para);

                    if (dtv_setting_ctrl->h_frm_list)
                    {
                        GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);
                        GUI_SetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, 5000, NULL);
                    }
                    else
                    {
                        return EPDK_FAIL;
                    }

                    return EPDK_OK;
                }

                case DTV_SETTING_TIME_ZONE_ID:
                {
                    return EPDK_OK;
                }

                case DTV_SETTING_BACK2LIST:
                {
                    dtv_setting_list_para_t list_para = {0};
                    __msg("dtv_setting_ctrl->focus_id=%d \n", dtv_setting_ctrl->focus_id);
                    list_para.focus_id = dtv_setting_ctrl->focus_id;
                    list_para.old_focus = dtv_setting_ctrl->focus_id;
                    list_para.layer = dtv_setting_ctrl->layer;
                    //__here__;
                    dtv_setting_ctrl->h_frm_list = dtv_setting_list_win_create(msg->h_deswin, &list_para);
                    GUI_WinSetFocusChild(dtv_setting_ctrl->h_frm_list);

                    if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID))
                    {
                        GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_SETTING_TIMERID, KEY_NUM_DELAY, NULL);
                    }

                    return EPDK_OK;
                }

                case DTV_SETTING_REPLAY:
                {
                    {
                        __gui_msg_t dmsg = {0};
                        __wrn("dtv_ctr.h_setting    : 0x%08x\n", dtv_ctr.h_setting);
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = DTV_BACK2PLAY_PROGRAM ;
                        GUI_SendNotifyMessage(&dmsg);
                    }
                    return EPDK_OK;
                }

                case DTV_SETTING_2_MANU_SEARCH:
                {
                    {
                        __gui_msg_t dmsg = {0};
                        __wrn("dtv_ctr.h_setting	: 0x%08x\n", dtv_ctr.h_setting);
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = DTV_SETTING_2_MANU_SEARCH ;
                        dmsg.dwAddData2 = msg->dwAddData2;
                        GUI_SendNotifyMessage(&dmsg);
                    }
                    return EPDK_OK;
                }
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}


/*************************************************
设置菜单:
参数:
h_parent:父窗口
layer:     菜单要往哪个图层上画

返回:设置菜单句柄
*************************************************/
H_WIN dtv_setting_create(H_WIN  h_parent, H_LYR layer)

{
    __gui_manwincreate_para_t create_info;
    dtv_setting_ctrl_t *dtv_setting_ctrl;
    __inf("************************************************\n");
    __inf("********  enter dtv setting   **************\n");
    __inf("************************************************\n");
    dtv_setting_ctrl = (dtv_setting_ctrl_t *)My_Malloc(0, sizeof(dtv_setting_ctrl_t));

    if (!dtv_setting_ctrl)
    {
        __msg("memory alloc fail.........\n");
        return NULL;
    }

    eLIBs_memset(dtv_setting_ctrl, 0, sizeof(dtv_setting_ctrl_t));
    dtv_setting_ctrl->dtv_setting_font = SWFFont;
    dtv_setting_ctrl->layer = layer;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = SUBMENU_DVB_SETTING;
    create_info.hParent         = h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dtv_setting_Proc);
    create_info.attr            = (void *)dtv_setting_ctrl;
    create_info.id          = SUBMENU_DVB_SETTING_ID;
    create_info.hHosting        = NULL;
    return (GUI_ManWinCreate(&create_info));
}

