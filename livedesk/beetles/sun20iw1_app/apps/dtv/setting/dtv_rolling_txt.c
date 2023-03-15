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
#include <emodules/mod_ca.h>
#include "dtv_setting_ca.h"
#if ((MAPLE_CA_STANDAR == MEDIAGATE_CAS) || (MAPLE_CA_STANDAR == DVN_JET_CAS))
#include "dtv_i.h"
#include "dtv_setting.h"
#include "dtv_setting_i.h"
#include "dtv_setting_uipara.h"
#define ROLLING_TXT_TIME        50

static __u8  bottom_dtvTxtBuf[512];
static __s32 bottom_txtPos;
static __u8  bottom_rollingTimes = 0;
static __u8  bottom_rollingTxt[512];
static __u32 bottom_rollingTxtLen;
static __u8  top_dtvTxtBuf[512];
static __s32 top_txtPos;
static __u8  top_rollingTimes = 0;
static __u8  top_rollingTxt[512];
static __u32 top_rollingTxtLen;

extern __u8 dtvRollingBarTimes;
extern __u8 dtvRollingTxt[];

H_LYR h_bottom_rt_lyr = NULL;
H_LYR h_top_rt_lyr = NULL;
extern H_WIN g_dtv_mainwin;

void dtv_show_rollingTxt_dialog(__u8 dtvRollingBarType)
{
    eLIBs_printf("dtv_show_rollingTxt_dialog : 0x%x\n", g_dtv_mainwin);

    if (g_dtv_mainwin)
    {
        __gui_msg_t my_msg;
        eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
        my_msg.id = GUI_MSG_COMMAND;
        my_msg.h_srcwin = NULL;
        my_msg.h_deswin = g_dtv_mainwin;
        my_msg.dwAddData1 = CMD_CREATE_ROLLING_TXT;
        my_msg.dwAddData2 = dtvRollingBarType;
        GUI_SendNotifyMessage(&my_msg);
    }
}
void dtv_rolling_txt_init(void)
{
    __u8  tmpTxt[256] = {0};
    extern __u8 dtvRollingBarType;
    char *rollingTxt = bottom_rollingTxt;
    __u32 *rollingTxtLen = &bottom_rollingTxtLen;
    int z = 0, k = 0, ret = 0;
    int dummyLen = (SCREEN_WIDTH + 7) / 8;
    top_txtPos = 0, bottom_txtPos = 0;
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

    if (dtvRollingBarType == 0x21) //top
    {
        eLIBs_memset(top_rollingTxt, 0x0, 512);
        rollingTxtLen = &top_rollingTxtLen;
        rollingTxt = top_rollingTxt;
        top_rollingTimes = dtvRollingBarTimes;
    }
    else
    {
        eLIBs_memset(bottom_rollingTxt, 0x0, 512);
        rollingTxtLen = &bottom_rollingTxtLen;
        rollingTxt = bottom_rollingTxt;
        bottom_rollingTimes = dtvRollingBarTimes;
    }

    eLIBs_printf("%s %d bottom_rollingTimes:%d\r\n", __FILE__, __LINE__, bottom_rollingTimes);
    eLIBs_printf("%s %d top_rollingTimes:%d\r\n", __FILE__, __LINE__, top_rollingTimes);
    ret = eLIBs_charset_convert(EPDK_CHARSET_ENM_GB2312, EPDK_CHARSET_ENM_UTF8, dtvRollingTxt, 128, tmpTxt, 256);
#endif
    tmpTxt[ret] = '\0';
    k = 0;

    for (z = 0; z < ret; z++)
    {
        //__wrn("%02x, \n", tmpTxt[z]);
        if (tmpTxt[z] == '\0')
        {
            break;
        }

        if (tmpTxt[z] == 0x0d && tmpTxt[z + 1] == 0x0a)
        {
            rollingTxt[k++] = 0x20;
            rollingTxt[k++] = 0x20;
            rollingTxt[k++] = 0x20;
            rollingTxt[k++] = 0x20;
            rollingTxt[k++] = 0x20;
            z++;
        }
        else
        {
            rollingTxt[k++] = tmpTxt[z];
        }
    }

    *rollingTxtLen = k;
    __wrn("before rollingTxtLen %d, %d, %d\n", rollingTxtLen, k, z);
    memset(rollingTxt + *rollingTxtLen, 0x20, dummyLen);
    (*rollingTxtLen) += dummyLen;
}
__s32 dtv_make_rollingTxt(void)
{
    __s32 char_size = 0;
    __u16 uni = 0;
    eLIBs_printf("%s %d h_bottom_rt_lyr:0x%x\r\n", __FILE__, __LINE__, h_bottom_rt_lyr);

    if (h_bottom_rt_lyr)
    {
        if (bottom_txtPos >= bottom_rollingTxtLen)
        {
            bottom_txtPos = 0;
            bottom_rollingTimes--;
            eLIBs_printf("WRN:L%d(%s):\r\n", __LINE__, __FILE__);
        }

        eLIBs_printf("WRN:L%d(%s): bottom_rollingTimes:%d\r\n", __LINE__, __FILE__, bottom_rollingTimes);
        char_size = esCHS_Char2Uni(EPDK_CHARSET_ENM_UTF8, (__u8 *)bottom_rollingTxt + bottom_txtPos,  bottom_rollingTxtLen - bottom_txtPos, &uni);

        if (char_size == 0)
        {
            char_size = 1;
        }

        bottom_txtPos += char_size;
        memcpy(bottom_dtvTxtBuf, bottom_rollingTxt, bottom_txtPos);
        bottom_dtvTxtBuf[bottom_txtPos] = '\0';
    }

    eLIBs_printf("%s %d h_top_rt_lyr:0x%x\r\n", __FILE__, __LINE__, h_top_rt_lyr);

    if (h_top_rt_lyr)
    {
        if (top_txtPos >= top_rollingTxtLen)
        {
            top_txtPos = 0;
            top_rollingTimes--;
            eLIBs_printf("WRN:L%d(%s):\r\n", __LINE__, __FILE__);
        }

        eLIBs_printf("WRN:L%d(%s): top_rollingTimes:%d\r\n", __LINE__, __FILE__, top_rollingTimes);
        char_size = esCHS_Char2Uni(EPDK_CHARSET_ENM_UTF8, (__u8 *)top_rollingTxt + top_txtPos, top_rollingTxtLen - top_txtPos, &uni);

        if (char_size == 0)
        {
            char_size = 1;
        }

        top_txtPos += char_size;
        memcpy(top_dtvTxtBuf, top_rollingTxt, top_txtPos);
        top_dtvTxtBuf[top_txtPos] = '\0';
    }

    return 0;
}
void dtv_hide_rollingTxt_dialog(__gui_msg_t *msg)
{
    if (h_bottom_rt_lyr != NULL)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        GUI_LyrWinSetSta(h_bottom_rt_lyr, GUI_LYRWIN_STA_SUSPEND);
        GUI_LyrWinDelete(h_bottom_rt_lyr);
        h_bottom_rt_lyr = NULL;
    }

    if (h_top_rt_lyr != NULL)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        GUI_LyrWinSetSta(h_top_rt_lyr, GUI_LYRWIN_STA_SUSPEND);
        GUI_LyrWinDelete(h_top_rt_lyr);
        h_top_rt_lyr = NULL;
    }

    if (EPDK_TRUE == GUI_IsTimerInstalled(msg->h_deswin, DVB_ROLLING_TXT_TIMER_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_ROLLING_TXT_TIMER_ID);
    }

    return ;
}

void dtv_rolling_txt_frame_draw(__gui_msg_t *msg)
{
    dtv_make_rollingTxt();
    eLIBs_printf("%s %d bottom_rollingTimes:%d\r\n", __FILE__, __LINE__, bottom_rollingTimes);
    eLIBs_printf("%s %d top_rollingTimes:%d\r\n", __FILE__, __LINE__, top_rollingTimes);

    if (h_bottom_rt_lyr != NULL && bottom_rollingTimes == 0)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        GUI_LyrWinSetSta(h_bottom_rt_lyr, GUI_LYRWIN_STA_SUSPEND);
        GUI_LyrWinDelete(h_bottom_rt_lyr);
        h_bottom_rt_lyr = NULL;
    }

    if (h_top_rt_lyr != NULL && top_rollingTimes == 0)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        GUI_LyrWinSetSta(h_top_rt_lyr, GUI_LYRWIN_STA_SUSPEND);
        GUI_LyrWinDelete(h_top_rt_lyr);
        h_top_rt_lyr = NULL;
    }

    eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);

    if (h_top_rt_lyr == NULL && h_bottom_rt_lyr == NULL)
    {
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);

        if (EPDK_TRUE == GUI_IsTimerInstalled(msg->h_deswin, DVB_ROLLING_TXT_TIMER_ID))
        {
            eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
            GUI_KillTimer(msg->h_deswin, DVB_ROLLING_TXT_TIMER_ID);
        }

        return ;
    }

    if (h_top_rt_lyr != NULL)
    {
        FB fb;
        GUI_RECT guiRect = {0};
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        eLIBs_memset(&fb, 0x00, sizeof(fb));
        GUI_LyrWinSel(h_top_rt_lyr);
        GUI_LyrWinSetSta(h_top_rt_lyr, GUI_LYRWIN_STA_ON);
        GUI_LyrWinGetFB(h_top_rt_lyr, &fb);
        GUI_LyrWinSetTop(h_top_rt_lyr);
        GUI_SetFont(SWFFont);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetBkColor(GUI_BLACK);
        GUI_SetColor(GUI_BLACK);
        guiRect.x0 = 0;
        guiRect.y0 = 0;
        guiRect.x1 = fb.size.width - 1;
        guiRect.y1 = fb.size.height - 1;
        GUI_FillRect(guiRect.x0, guiRect.y0, guiRect.x1, guiRect.y1);
        GUI_SetColor(GUI_WHITE);
        GUI_DispStringInRect(top_dtvTxtBuf, &guiRect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    }

    if (h_bottom_rt_lyr != NULL)
    {
        FB fb;
        GUI_RECT guiRect = {0};
        eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
        eLIBs_memset(&fb, 0x00, sizeof(fb));
        GUI_LyrWinSel(h_bottom_rt_lyr);
        GUI_LyrWinSetSta(h_bottom_rt_lyr, GUI_LYRWIN_STA_ON);
        GUI_LyrWinGetFB(h_bottom_rt_lyr, &fb);
        GUI_LyrWinSetTop(h_bottom_rt_lyr);
        GUI_SetFont(SWFFont);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetBkColor(GUI_BLACK);
        GUI_SetColor(GUI_BLACK);
        guiRect.x0 = 0;
        guiRect.y0 = 0;
        guiRect.x1 = fb.size.width - 1;
        guiRect.y1 = fb.size.height - 1;
        GUI_FillRect(guiRect.x0, guiRect.y0, guiRect.x1, guiRect.y1);
        GUI_SetColor(GUI_WHITE);
        GUI_DispStringInRect(bottom_dtvTxtBuf, &guiRect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    }
}
void dtv_rolling_txt_create(__gui_msg_t *msg)
{
    {
        RECT rect;
        rect.x = 0;
        rect.width = SCREEN_WIDTH;
        rect.height = 30;
        eLIBs_printf("WRN:L%d(%s): dtvRollingBarType:0x%x\r\n", __LINE__, __FILE__, dtvRollingBarType);
        eLIBs_printf("h_bottom_rt_lyr:0x%x h_top_rt_lyr:0x%x\r\n", h_bottom_rt_lyr, h_top_rt_lyr);

        if (h_bottom_rt_lyr == NULL && dtvRollingBarType == 0x22)
        {
            eLIBs_printf("WRN:L%d(%s):\r\n", __LINE__, __FILE__);
            rect.y = SCREEN_HEIGHT - 30;
            h_bottom_rt_lyr = dtv_com_layer_create(&rect, "rollingTxt bottom");
            eLIBs_printf("CMD_CREATE_ROLLING_TXT dtv_com_layer_create : 0x%x\n", h_bottom_rt_lyr);
        }
        else if (h_top_rt_lyr == NULL && dtvRollingBarType == 0x21)
        {
            eLIBs_printf("WRN:L%d(%s):\r\n", __LINE__, __FILE__);
            rect.y = 0;
            h_top_rt_lyr = dtv_com_layer_create(&rect, "rollingTxt top");
            eLIBs_printf("CMD_CREATE_ROLLING_TXT dtv_com_layer_create : 0x%x\n", h_top_rt_lyr);
        }
    }
    dtv_rolling_txt_init();

    if (EPDK_FALSE == GUI_IsTimerInstalled(msg->h_deswin, DVB_ROLLING_TXT_TIMER_ID))
    {
        GUI_SetTimer(msg->h_deswin, DVB_ROLLING_TXT_TIMER_ID, ROLLING_TXT_TIME, NULL);
    }
}
#endif
