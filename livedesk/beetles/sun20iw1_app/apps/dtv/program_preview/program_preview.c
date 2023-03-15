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
#include "program_preview.h"
#include "program_preview_i.h"
#include "program_preview_uipara.h"

static __bool     bpreview_submenu_exist = EPDK_FALSE;
static __s32 last_key = -1;
extern dtv_ctr_t  dtv_ctr;
static __s32 create_flag = 0;


static __s32 _program_preview_Proc(__gui_msg_t *msg)
{
    program_preview_ctrl_t *program_preview_ctrl;
    program_preview_ctrl     = (program_preview_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!program_preview_ctrl)
    {
        __wrn("program_preview_ctrl is null ......\n");
        return EPDK_FAIL;
    }

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            program_preview_frmpara_t list_para;
            list_para.focus_id = 0;
            list_para.old_focus = 0;
            list_para.layer = program_preview_ctrl->layer;
            program_preview_ctrl->h_frm_list = program_preview_frmwin_create(msg->h_deswin, &list_para);
            GUI_WinSetFocusChild(program_preview_ctrl->h_frm_list);
            GUI_SetTimer(msg->h_deswin, SUBMENU_DVB_PREVIEW_TIMERID, 1000, NULL);
            last_key = -1;
            create_flag = 1;
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __gui_msg_t my_msg;
            __wrn("begin _program_preview_Proc destroy\n");
            create_flag--;

            if (0 != create_flag)
            {
                __wrn("-------------------------------------destroy err. create_flag:%d\n", create_flag);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_PREVIEW_TIMERID))
            {
                __wrn("SUBMENU_DVB_PREVIEW_TIMERID kill...\n");
                GUI_KillTimer(msg->h_deswin, SUBMENU_DVB_PREVIEW_TIMERID);
            }

            /*if(program_preview_ctrl->h_frm_list)
            {
                program_preview_frmwin_delete(program_preview_ctrl->h_frm_list);
                program_preview_ctrl->h_frm_list = 0;
            }*/
            if (dtv_ctr.h_lyr)
            {
                GUI_LyrWinDelete(program_preview_ctrl->layer);
                dtv_ctr.h_lyr = NULL ;
                program_preview_ctrl->layer = NULL;
                program_preview_ctrl->h_frm_list = NULL;
            }

            __wrn("...\n");
            My_Mfree(0, program_preview_ctrl);
            bpreview_submenu_exist = EPDK_FALSE;
            dtv_ctr.h_serverlist = NULL;
            //  esKSRV_SysInfo();
            eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.h_srcwin = NULL;
            my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            my_msg.dwAddData1 = DTV_CA_MSG_RESUME;
            my_msg.dwAddData2 = 0;
            GUI_SendNotifyMessage(&my_msg);
            check_dtv_or_radio();
            __wrn("end _program_preview_Proc destroy\n");
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            if (GUI_IsTimerInstalled(msg->h_deswin, SUBMENU_DVB_PREVIEW_TIMERID))
            {
                GUI_ResetTimer(msg->h_deswin, SUBMENU_DVB_PREVIEW_TIMERID, 1000, NULL);
            }

            __msg("---- GUI_MSG_KEY ----\n");

            if (msg->dwAddData2 == KEY_DOWN_ACTION)
            {
                last_key = msg->dwAddData1;
            }
        }
        break;

        case GUI_MSG_TIMER:
        {
            if (msg->dwAddData1 == SUBMENU_DVB_PREVIEW_TIMERID)
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

            break;
        }

        case GUI_MSG_COMMAND:
        {
        }
        break;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

__s32 dtv_preview_submenu_exist(void)
{
    return bpreview_submenu_exist;
}

H_WIN dtv_program_preview_create(H_WIN  h_parent, H_LYR layer)

{
    __gui_manwincreate_para_t create_info;
    program_preview_ctrl_t *program_preview_ctrl;
    __msg("****************************************************\n");
    __msg("********  enter program_preview menu  **************\n");
    __msg("****************************************************\n");
    bpreview_submenu_exist = EPDK_TRUE;
    //__wait__;
    program_preview_ctrl = (program_preview_ctrl_t *)My_Malloc(0, sizeof(program_preview_ctrl_t));

    if (!program_preview_ctrl)
    {
        __msg("memory alloc fail.........\n");
        bpreview_submenu_exist = EPDK_FALSE;
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset(program_preview_ctrl, 0, sizeof(program_preview_ctrl_t));
    program_preview_ctrl->program_preview_font = SWFFont;
    program_preview_ctrl->layer = layer;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = SUBMENU_DVB_PREVIEW;
    create_info.hParent         = h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_program_preview_Proc);
    create_info.attr            = (void *)program_preview_ctrl;
    create_info.id          = SUBMENU_DVB_PREVIEW_ID;
    create_info.hHosting        = NULL;
    return (GUI_ManWinCreate(&create_info));
}
