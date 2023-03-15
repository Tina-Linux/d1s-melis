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
#ifndef  __G_CLOSE_SCREEN_C__
#define  __G_CLOSE_SCREEN_C__

#include <log.h>
#include "../mod_init_i.h"
#include <kconfig.h>

/*
****************************************************************************************************
*Name        : dsk_enable_close_scn
*Prototype   : __s32 dsk_enable_close_scn( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EDPK_FAIL  fail
*Description : allow closing screen.
*Other       :
****************************************************************************************************
*/
__s32 g_enable_close_scn(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_CLOSE_SCN_GATE_ON;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = 0;
    msg.dwAddData2  = 0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_disable_close_scn
*Prototype   : __s32 dsk_disable_close_scn( void )
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EDPK_FAIL  fail
*Description : forbid closing screen.
*Other       :
****************************************************************************************************
*/
__s32 g_disable_close_scn(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_CLOSE_SCN_GATE_OFF;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = 0;
    msg.dwAddData2  = 0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_set_close_scn_time
*Prototype   : __s32 dsk_set_close_scn_time(__u32 time)
*Arguments   : time       input. time, unit:s.
*Return      : EPDK_OK    succeed
*              EDPK_FAIL  fail
*Description : set the time for closing screen.
*Other       :
****************************************************************************************************
*/
__s32 g_set_close_scn_time(__u32 time)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_SET_SCN_CLOSE;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)time * 1000;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendNotifyMessage(&msg);

    __msg("g_set_close_scn_time, time=%d\n", time);
    return ret;
}

__s32 g_get_close_scn_time(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_GET_SCN_CLOSE;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = 0;
    msg.dwAddData2  = 0;

    ret = GUI_SendMessage(&msg);

    __msg("g_get_close_scn_time, time=%d\n", ret);
    return ret;
}

__s32 g_close_screen(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_SCREEN_CLOSE;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendNotifyMessage(&msg);
    return ret;
}

void g_enable_standby(void)
{
    __gui_msg_t msg;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_ENABLE_STANDBY;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    GUI_SendMessage(&msg);
}

void g_disable_standby(void)
{
    __gui_msg_t msg;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_DISABLE_STANDBY;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    GUI_SendMessage(&msg);
}

__bool g_is_enable_standby(void)
{
    __gui_msg_t msg;
    __s32       ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_GET_STANDBY_FLAG;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    ret = GUI_SendMessage(&msg);
    return ret;
}

#endif     //  ifndef __DSK_CLOSE_SCREEN_C__

/* end of dsk_close_screen.c */
