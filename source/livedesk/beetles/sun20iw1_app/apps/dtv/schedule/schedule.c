/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : schedules.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
*
**************************************************************************************************************
*/
#include <log.h>
#include "schedule.h"
#include "schedule_i.h"
#include "schedule_uipara.h"
extern dtv_ctr_t  dtv_ctr;
static __s32 create_flag = 0;


static __s32 _schedule_Proc(__gui_msg_t *msg)
{
    schedule_ctrl_t *schedule_ctrl;
    schedule_ctrl    = (schedule_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!schedule_ctrl)
    {
        __wrn("schedule_ctrl is null ......\n");
        return EPDK_FAIL;
    }

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            //com_set_palette_by_id(ID_SCHEDULE_WKM_BMP);
            //schedule_win_create(msg);
            schedule_list_para_t list_para;
            create_flag = 1;
            list_para.focus_id = 0;
            list_para.old_focus = 0;
            list_para.layer = schedule_ctrl->layer;
            schedule_ctrl->h_frm_list = schedule_list_win_create(msg->h_deswin, &list_para);

            if (NULL == schedule_ctrl->h_frm_list)
            {
                __msg("schedule_ctrl->h_frm_list=NULL\n");
                return EPDK_FAIL;
            }

            GUI_WinSetFocusChild(schedule_ctrl->h_frm_list);
            schedule_ctrl->destroy_timerid = SUBMENU_DVB_SCHUDULE_DESTROY_TIMERID;

            if (!GUI_IsTimerInstalled(msg->h_deswin, schedule_ctrl->destroy_timerid))
            {
                GUI_SetTimer(msg->h_deswin, schedule_ctrl->destroy_timerid, 1000, NULL);
            }
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            GUI_FrmWinDelete(schedule_ctrl->h_frm_list);
            //GUI_LyrWinDelete(schedule_ctrl->layer);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            /*if(schedule_ctrl->h_frm_list)
            {
                schedule_list_win_delete(schedule_ctrl->h_frm_list);
                schedule_ctrl->h_frm_list = 0;
            }*/
            if (GUI_IsTimerInstalled(msg->h_deswin, schedule_ctrl->destroy_timerid))
            {
                GUI_KillTimer(msg->h_deswin, schedule_ctrl->destroy_timerid);
                schedule_ctrl->destroy_timerid = 0;
            }

            if (dtv_ctr.h_lyr)
            {
                GUI_LyrWinDelete(schedule_ctrl->layer);
                schedule_ctrl->layer = NULL;
                dtv_ctr.h_lyr = NULL ;
                schedule_ctrl->h_frm_list = 0;
            }

            __wrn("...\n");
            My_Mfree(0, schedule_ctrl);
            //  esKSRV_SysInfo();
            create_flag--;

            if (0 != create_flag)
            {
                __wrn("-------------------------------------destroy err. create_flag:%d\n", create_flag);
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            if (GUI_IsTimerInstalled(msg->h_deswin, schedule_ctrl->destroy_timerid))
            {
                GUI_ResetTimer(msg->h_deswin, schedule_ctrl->destroy_timerid, 1000, NULL);
            }

            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                //eLIBs_printf("msg->dwAddData1========%d \n",msg->dwAddData1);
                switch (msg->dwAddData1)
                {
                    case GUI_MSG_KEY_ESCAPE:
                    case GUI_MSG_KEY_MENU:
                    case GUI_MSG_KEY_ENTER:
                    {
                        __gui_msg_t my_msg;
                        my_msg.id = GUI_MSG_COMMAND;
                        my_msg.h_srcwin = NULL;
                        my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        my_msg.dwAddData1 = DTV_BACK2PLAY_PROGRAM;
                        my_msg.dwAddData2 = 0;
                        GUI_SendNotifyMessage(&my_msg);
                        return EPDK_OK;
                    }

                    default:
                        break;
                }
            }
        }
        break;

        case GUI_MSG_TIMER:
        {
            if (msg->dwAddData1 == schedule_ctrl->destroy_timerid)
            {
                GUI_KillTimer(msg->h_deswin, schedule_ctrl->destroy_timerid);
                schedule_ctrl->destroy_timerid = 0;
                {
                    __gui_msg_t my_msg;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.h_srcwin = NULL;
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.dwAddData1 = DTV_BACK2PLAY_PROGRAM;
                    my_msg.dwAddData2 = 0;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }
            }
        }
        break;

        case GUI_MSG_COMMAND:
        {
        }
        break;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN dtv_schedule_create(H_WIN h_parent, H_LYR layer)

{
    __gui_manwincreate_para_t create_info;
    schedule_ctrl_t *schedule_ctrl;
    __inf("****************************************************************************************\n");
    __inf("********  enter schedule menu  **************\n");
    __inf("****************************************************************************************\n");
    //__wait__;
    schedule_ctrl = (schedule_ctrl_t *)My_Malloc(0, sizeof(schedule_ctrl_t));

    if (!schedule_ctrl)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset(schedule_ctrl, 0, sizeof(schedule_ctrl_t));
    schedule_ctrl->schedule_font = SWFFont;
    schedule_ctrl->layer = layer;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = SUBMENU_DVB_SCHUDULE;
    create_info.hParent         = h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_schedule_Proc);
    create_info.attr            = (void *)schedule_ctrl;
    create_info.id          = SUBMENU_DVB_SCHUDULE_ID;
    create_info.hHosting        = NULL;
    return (GUI_ManWinCreate(&create_info));
}

