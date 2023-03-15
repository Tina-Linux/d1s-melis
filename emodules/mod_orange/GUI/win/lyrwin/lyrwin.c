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
#include "lyrwin_i.h"
#include <mod_display.h>
#include <kconfig.h>

/*
**********************************************************************************************************************
*                                               InitZOrderInfo
*
* author:
*
* date:             2009-7-28
*
* Description:      initzorderinfo
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static void InitZOrderInfo(pzorderinfo pZOrderInfo, H_WIN hHost)
{
    pZOrderInfo->nNumber    = 0;
    pZOrderInfo->hWnd       = hHost;
    pZOrderInfo->pTopMost   = NULL;
}

/*
**********************************************************************************************************************
*                                               _LyrOn
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 _LyrOn(__gui_lyrwin_t   *p_lyr)
{
    __s32          ret;
    __bool         wakeup = ORANGE_FALSE;
    __u64          pdispbuffer[3];
    __disp_fb_t    temp_disp_fb;
#if GUI_CHECK_ARG_EN > 0

    if (!p_lyr)
    {
        __log("h_lyr empty!");
        return ORANGE_FAIL;
    }

#endif

    /* request hardware layer resource              */
    if (p_lyr->status == GUI_LYRWIN_STA_ON)
    {
        return EPDK_OK;
    }

    if (p_lyr->status == GUI_LYRWIN_STA_OFF)
    {
        goto exit;
    }

    /* resume fb, request software layer            */
    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        ret = GUI_FBResume(p_lyr->lyrpara.fb);

        if (ret != EPDK_OK)
        {
            ORANGE_WRN("framebuffer resume failed!");
            return ORANGE_FAIL;
        }

        p_lyr->h_swlyr = (__hdle)(unsigned long)GUI_CreateLayer(p_lyr->lyrpara.fb->addr[0],
                         p_lyr->lyrpara.fb->size.width,
                         p_lyr->lyrpara.fb->size.height,
                         p_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);

        if (p_lyr->h_swlyr == (__hdle)(-1))
        {
            GUI_FBSuspend(p_lyr->lyrpara.fb);
            ORANGE_WRN("no software layer fit!");
            return ORANGE_FAIL;
        }
    }

    /* request hw layer                             */
    if (1)
    {
		__mp * mod_disp = 0;

        /*open display driver handle*/
		mod_disp = esKSRV_Get_Display_Hld();
        if (!mod_disp)
        {
            ORANGE_WRN("open display device fail!");
            return ORANGE_FAIL;
        }

        p_lyr->devfp = mod_disp;

        if (p_lyr->type == GUI_LYRWIN_NORMAL)
        {
            /*request hardware layer*/
            p_lyr->h_hwlyr = (__hdle)(unsigned long)GUI_LogicLayerRequest(gui_scndir, mod_disp, 0, &(p_lyr->lyrpara));
			//log("herexx GUI_LogicLayerRequest xxxxxxxxxxx  p_lyr->h_hwlyr = %x", p_lyr->h_hwlyr);
            if (!p_lyr->h_hwlyr)
            {
                //log("close mod display");
                p_lyr->devfp = 0;

                if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
                {
                    GUI_ReleaseLayer((signed long)p_lyr->h_swlyr);
                    p_lyr->h_swlyr = 0;
                    GUI_FBSuspend(p_lyr->lyrpara.fb);
                }

                ORANGE_WRN("request hardware layer failed!");
                return ORANGE_FAIL;
            }
        }
        else
        {
#ifdef ORANGE_SUPPORT_SPRITE
xxxxxx
            __disp_sprite_block_para_t   spritepara;
            GUI_Fb2Dispfb(p_lyr->lyrpara.fb, &temp_disp_fb);
            spritepara.fb       = temp_disp_fb; //p_lyr->lyrpara.fb modified by Derek,2010-12-13 16:21:04
            spritepara.src_win.x = p_lyr->lyrpara.src_win.x;
            spritepara.src_win.y = p_lyr->lyrpara.src_win.y;
            spritepara.src_win.width = p_lyr->lyrpara.src_win.width;  //modified by Derek,2011-1-8 14:15:41
            spritepara.src_win.height = p_lyr->lyrpara.src_win.height;
            spritepara.scn_win.x = p_lyr->lyrpara.scn_win.x;
            spritepara.scn_win.y = p_lyr->lyrpara.scn_win.y;
            spritepara.scn_win.width = p_lyr->lyrpara.scn_win.width;
            spritepara.scn_win.height = p_lyr->lyrpara.scn_win.height;
            esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_OPEN, SEL_SCREEN, NULL); //modified by Derek,2011-1-7 14:56:07
            /*request hardware layer*/
            pdispbuffer[0] = spritepara.fb.format;
            pdispbuffer[1] = spritepara.fb.seq;
            pdispbuffer[2] = 0;
            esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_SET_FORMAT, SEL_SCREEN, (void *)pdispbuffer);
            pdispbuffer[0] = (__u32)&spritepara;
            pdispbuffer[1] = 0;
            pdispbuffer[2] = 0;
            p_lyr->h_hwlyr = eLIBs_fioctrl(mod_disp, DISP_CMD_SPRITE_BLOCK_REQUEST, SEL_SCREEN, (void *)pdispbuffer);
            pdispbuffer[0] = p_lyr->h_hwlyr;
            pdispbuffer[1] = 0;
            pdispbuffer[2] = 0;
            esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_BLOCK_OPEN, SEL_SCREEN, (void *)pdispbuffer);

            if (!p_lyr->h_hwlyr)
            {
                eLIBs_fclose(p_lyr->devfp);
                p_lyr->devfp = 0;

                if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
                {
                    GUI_ReleaseLayer(p_lyr->h_swlyr);
                    p_lyr->h_swlyr = 0;
                    GUI_FBSuspend(p_lyr->lyrpara.fb);
                }

                ORANGE_WRN("request hardware layer failed!");
            }

#endif
        }
    }

exit:



    if (p_lyr->status != GUI_LYRWIN_STA_ON)
    {
        Layer_DeattachFromInVisList(GUI_LyrP2H(p_lyr));
        Layer_AttachToVisableList(GUI_LyrP2H(p_lyr));
    }

    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        wakeup = ORANGE_TRUE;
    }

    p_lyr->status = GUI_LYRWIN_STA_ON;

    /* tell all the wins */
    if (wakeup)
    {
        __gui_win_t *p_win;
        __gui_msg_t msg;

        if (p_lyr->ManWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->ManWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_WAKEUP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }

        if (p_lyr->TopMostWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->TopMostWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_WAKEUP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }
    }

	if (p_lyr->type == GUI_LYRWIN_NORMAL)
	{
		/* open layer  modified by Derek,2010.12.07.16:05								  */
		pdispbuffer[0] = (unsigned long)(p_lyr->h_hwlyr);
		pdispbuffer[1] = 0;
		pdispbuffer[2] = 0;
		ret = esMODS_MIoctrl(p_lyr->devfp, MOD_DISP_CMD_LAYER_OPEN, SEL_SCREEN, (void *)pdispbuffer);

		if (ret == ORANGE_FAIL)
		{
			ORANGE_WRN("lyr cannot be opened!");
			return ORANGE_FAIL;
		}
	}
	else
	{
#ifdef ORANGE_SUPPORT_SPRITE
XX
		esMODS_MIoctrl(p_lyr->devfp, DISP_CMD_SPRITE_OPEN, SEL_SCREEN, NULL);
		/* open layer									*/
		pdispbuffer[0] = p_lyr->h_hwlyr;
		pdispbuffer[1] = 0;
		pdispbuffer[2] = 0;
		ret = esMODS_MIoctrl(p_lyr->devfp, DISP_CMD_SPRITE_BLOCK_OPEN, SEL_SCREEN, (void *)pdispbuffer);

		if (ret == ORANGE_FAIL)
		{
			ORANGE_WRN("lyr cannot be opened!");
			return ORANGE_FAIL;
		}
#endif
	}

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _LyrOff
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 _LyrOff(__gui_lyrwin_t   *p_lyr)
{
    __s32          ret;
    __bool         wakeup = ORANGE_FALSE;
    __u64          pdispbuffer[3];
    __disp_fb_t    temp_disp_fb;
#if GUI_CHECK_ARG_EN > 0

    if (!p_lyr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

#endif

    /* request hardware layer resource */
    if (p_lyr->status == GUI_LYRWIN_STA_OFF)
    {
        return EPDK_OK;
    }

    if (p_lyr->status == GUI_LYRWIN_STA_ON)
    {
        if (p_lyr->type == GUI_LYRWIN_NORMAL)
        {
            pdispbuffer[0] = (unsigned long)p_lyr->h_hwlyr;
            pdispbuffer[1] = 0;
            pdispbuffer[2] = 0;
            ret = esMODS_MIoctrl(p_lyr->devfp, MOD_DISP_CMD_LAYER_CLOSE, SEL_SCREEN, (void *)pdispbuffer);

            if (ret == ORANGE_FAIL)
            {
                ORANGE_WRN("lyr cannot be opened!");
                return ORANGE_FAIL;
            }
        }
        else
        {
#ifdef ORANGE_SUPPORT_SPRITE
            pdispbuffer[0] = p_lyr->h_hwlyr;
            pdispbuffer[1] = 0;
            pdispbuffer[2] = 0;
            ret = esMODS_MIoctrl(p_lyr->devfp, DISP_CMD_SPRITE_BLOCK_CLOSE, SEL_SCREEN, (void *)pdispbuffer);

            if (ret == ORANGE_FAIL)
            {
                ORANGE_WRN("lyr cannot be opened!");
                return ORANGE_FAIL;
            }

#endif
        }

        goto exit;
    }

    /* resume fb, request software layer            */
    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        ret = GUI_FBResume(p_lyr->lyrpara.fb);

        if (ret != EPDK_OK)
        {
            ORANGE_WRN("framebuffer resume failed!");
            return ORANGE_FAIL;
        }

        p_lyr->h_swlyr = (__hdle)(unsigned long)GUI_CreateLayer(p_lyr->lyrpara.fb->addr[0],
                         p_lyr->lyrpara.fb->size.width,
                         p_lyr->lyrpara.fb->size.height,
                         p_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);

        if (p_lyr->h_swlyr == (__hdle)(-1))
        {
            GUI_FBSuspend(p_lyr->lyrpara.fb);
            ORANGE_WRN("no software layer fit!");
            return ORANGE_FAIL;
        }
    }

    /* request hw layer                             */
    if (1)
    {
        __mp *mod_disp = 0;
        /*open display driver handle*/
        // = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
		mod_disp = esKSRV_Get_Display_Hld();
        if (!mod_disp)
        {
            ORANGE_WRN("open display device fail!");
            return ORANGE_FAIL;
        }
         p_lyr->devfp = mod_disp;

        if (p_lyr->type == GUI_LYRWIN_NORMAL)
        {
            /*request hardware layer*/
            p_lyr->h_hwlyr = (__hdle)(unsigned long)GUI_LogicLayerRequest(gui_scndir, mod_disp, 0, &(p_lyr->lyrpara));

            if (!p_lyr->h_hwlyr)
            {
                p_lyr->devfp = 0;

                if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
                {
                    GUI_ReleaseLayer((signed long)(p_lyr->h_swlyr));
                    p_lyr->h_swlyr = 0;
                    GUI_FBSuspend(p_lyr->lyrpara.fb);
                }

                ORANGE_WRN("request hardware layer failed!");
                return ORANGE_FAIL;
            }
        }
        else
        {
#ifdef ORANGE_SUPPORT_SPRITE
            __disp_sprite_block_para_t   spritepara;
            GUI_Fb2Dispfb(p_lyr->lyrpara.fb, &temp_disp_fb);
            spritepara.fb       = temp_disp_fb; //p_lyr->lyrpara.fb; modified by Derek,2010-12-13 16:23:26
            spritepara.src_win.x = p_lyr->lyrpara.src_win.x;
            spritepara.src_win.y = p_lyr->lyrpara.src_win.y;
            spritepara.src_win.width = p_lyr->lyrpara.src_win.width;  //modified by Derek,2011-1-8 14:15:41
            spritepara.src_win.height = p_lyr->lyrpara.src_win.height;
            spritepara.scn_win.x = p_lyr->lyrpara.scn_win.x;
            spritepara.scn_win.y = p_lyr->lyrpara.scn_win.y;
            spritepara.scn_win.width = p_lyr->lyrpara.scn_win.width;
            spritepara.scn_win.height = p_lyr->lyrpara.scn_win.height;
            esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_OPEN, SEL_SCREEN, NULL);
            /*request hardware layer*/
            pdispbuffer[0] = spritepara.fb.format;
            pdispbuffer[1] = spritepara.fb.seq;
            pdispbuffer[2] = 0;
            esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_SET_FORMAT, SEL_SCREEN, (void *)pdispbuffer);
            pdispbuffer[0] = (__u32)&spritepara;
            pdispbuffer[1] = 0;
            pdispbuffer[2] = 0;
            p_lyr->h_hwlyr = esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_BLOCK_REQUEST, SEL_SCREEN, (void *)pdispbuffer);
            pdispbuffer[0] = p_lyr->h_hwlyr;
            pdispbuffer[1] = 0;
            pdispbuffer[2] = 0;
            esMODS_MIoctrl(mod_disp, DISP_CMD_SPRITE_BLOCK_CLOSE, SEL_SCREEN, (void *)pdispbuffer);

            if (!p_lyr->h_hwlyr)
            {
                p_lyr->devfp = 0;

                if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
                {
                    GUI_ReleaseLayer(p_lyr->h_swlyr);
                    p_lyr->h_swlyr = 0;
                    GUI_FBSuspend(p_lyr->lyrpara.fb);
                }

                ORANGE_WRN("request hardware layer failed!");
                return ORANGE_FAIL;
            }

#endif
        }
    }

exit:

    if (p_lyr->status == GUI_LYRWIN_STA_ON)
    {
        Layer_DeattachFromVisList(GUI_LyrP2H(p_lyr));
        Layer_AttachToInVisList(GUI_LyrP2H(p_lyr));
    }

    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        wakeup = ORANGE_TRUE;
    }

    p_lyr->status = GUI_LYRWIN_STA_OFF;

    /* tell all the wins */
    if (wakeup)
    {
        __gui_win_t *p_win;
        __gui_msg_t msg;

        if (p_lyr->ManWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->ManWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_WAKEUP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }

        if (p_lyr->TopMostWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->TopMostWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_WAKEUP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _LyrSleep
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 _LyrSleep(__gui_lyrwin_t   *p_lyr)
{
    __s32          ret;
    __bool         wakeup = ORANGE_FALSE;
    __u64          pbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!p_lyr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

#endif
 
    /* check if layer already in sleep status */
    if (p_lyr->status == GUI_LYRWIN_STA_SLEEP)
    {
         return EPDK_OK;
    }

    /* if suspend status, resume fb */
    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        ret = GUI_FBResume(p_lyr->lyrpara.fb);

        if (ret != EPDK_OK)
        {
            ORANGE_WRN("framebuffer resume failed!");
            return ORANGE_FAIL;
        }

        p_lyr->h_swlyr = (__hdle)(signed long)GUI_CreateLayer(p_lyr->lyrpara.fb->addr[0],
                         p_lyr->lyrpara.fb->size.width,
                         p_lyr->lyrpara.fb->size.height,
                         p_lyr->lyrpara.fb->fmt.fmt.rgb.pixelfmt);

        if (p_lyr->h_swlyr == (__hdle)(-1))
        {
            GUI_FBSuspend(p_lyr->lyrpara.fb);
            ORANGE_WRN("no software layer fit!");
            return ORANGE_FAIL;
        }
    }
    else    /* if on/off status */
    {
         if (p_lyr->type == GUI_LYRWIN_NORMAL)
        {
            /*release hardware layer*/
            pbuffer[0] = (unsigned long)p_lyr->h_hwlyr; //modified by Derek,2010.12.07.16:00
            pbuffer[1] = 0;
            pbuffer[2] = 0;
             ret = esMODS_MIoctrl(p_lyr->devfp, MOD_DISP_CMD_LAYER_CLOSE, SEL_SCREEN, (void *)pbuffer);

            if (ret == ORANGE_FAIL)
            {
                ORANGE_WRN("lyr cannot be closed!");
                return ORANGE_FAIL;
            }
            ret = esMODS_MIoctrl(p_lyr->devfp, MOD_DISP_LAYER_RELEASE, SEL_SCREEN, (void *)pbuffer);

            if (ret != EPDK_OK)
            {
                ORANGE_WRN("release hardware layer failed!");
                return ORANGE_FAIL;
            }

            p_lyr->devfp = 0;
            p_lyr->h_hwlyr = 0;
#ifdef CONFIG_SOC_SUN20IW1P1
            esKRNL_TimeDly(2);//add by bayden, 延时30ms，等待显示驱动V中断到达 Make sure the parameters are set
#endif
        }
        else
        {
#ifdef ORANGE_SUPPORT_SPRITE
            /*release hardware layer*/
            pbuffer[0] = p_lyr->h_hwlyr; //modified by Derek,2011-1-7 15:07:23
            pbuffer[1] = 0;
            pbuffer[2] = 0;
            ret = esMODS_MIoctrl(p_lyr->devfp, DISP_CMD_SPRITE_BLOCK_RELEASE, SEL_SCREEN, (void *)pbuffer);

            if (ret != EPDK_OK)
            {
                ORANGE_WRN("release hardware layer failed!");
                return ORANGE_FAIL;
            }

            p_lyr->devfp = 0;
            p_lyr->h_hwlyr = 0;
#endif
        }
    }

    if (p_lyr->status == GUI_LYRWIN_STA_ON)
    {
        Layer_DeattachFromVisList(GUI_LyrP2H(p_lyr));
        Layer_AttachToInVisList(GUI_LyrP2H(p_lyr));
    }

    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        wakeup = ORANGE_TRUE;
    }

    /* set status */
    p_lyr->status = GUI_LYRWIN_STA_SLEEP;

    /* tell all the wins */
    if (wakeup)
    {
        __gui_win_t *p_win;
        __gui_msg_t msg;

        if (p_lyr->ManWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->ManWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_WAKEUP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }

        if (p_lyr->TopMostWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->TopMostWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_WAKEUP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               _LyrSuspend
*
* Description: sleep layer, release framebuffer memory, sleep all the windows in the layer.
*
* Arguments  : layer handle
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 _LyrSuspend(__gui_lyrwin_t   *p_lyr)
{
    __s32          ret;
    __bool         wakeup = ORANGE_FALSE;
    __u64          pbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!p_lyr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

#endif

    /* check if layer already in suspend status */
    if (p_lyr->status == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("layer already in suspend status!");
        return EPDK_OK;
    }

    /*release hardware layer */
    if (p_lyr->h_hwlyr)
    {
        if (p_lyr->type == GUI_LYRWIN_NORMAL)
        {
            pbuffer[0] = (unsigned long)p_lyr->h_hwlyr;
            pbuffer[1] = 0;
            pbuffer[2] = 0;
            ret = esMODS_MIoctrl(p_lyr->devfp, MOD_DISP_CMD_LAYER_CLOSE, SEL_SCREEN, (void *)pbuffer);

            if (ret == ORANGE_FAIL)
            {
                ORANGE_WRN("lyr cannot be closed!");
                return ORANGE_FAIL;
            }

            ret = esMODS_MIoctrl(p_lyr->devfp, MOD_DISP_LAYER_RELEASE, SEL_SCREEN, (void *)pbuffer);

            if (ret != EPDK_OK)
            {
                __err("release hardware layer failed!");
            }

            p_lyr->devfp = 0;
            p_lyr->h_hwlyr = 0;
#ifdef CONFIG_SOC_SUN20IW1P1
            esKRNL_TimeDly(2);//add by bayden, 延时30ms，等待显示驱动V中断到达 Make sure the parameters are set
#endif
        }
        else
        {
#ifdef ORANGE_SUPPORT_SPRITE
XXXXXX
            /*release hardware layer*/
            pbuffer[0] = p_lyr->h_hwlyr;
            pbuffer[1] = 0;
            pbuffer[2] = 0;
            ret = esMODS_MIoctrl(p_lyr->devfp, DISP_CMD_SPRITE_BLOCK_RELEASE, SEL_SCREEN, (void *)pbuffer);

            if (ret != EPDK_OK)
            {
                ORANGE_WRN("release hardware layer failed!");
                return ORANGE_FAIL;
            }

            p_lyr->devfp = 0;
            p_lyr->h_hwlyr = 0;
#endif
        }
    }

    /* release framebuffer */
    if (GUI_FBGetStatus(p_lyr->lyrpara.fb) == GUI_FB_STA_SLEEP)
    {
        ORANGE_WRN("fp already sleep!");
        return ORANGE_FAIL;
    }

    ret = GUI_FBSuspend(p_lyr->lyrpara.fb);

    if (ret != EPDK_OK)
    {
        ORANGE_WRN("framebuffer suspend failed!");
        return ORANGE_FAIL;
    }

    /* release software layer */
    if (p_lyr->h_swlyr)
    {
        GUI_ReleaseLayer((signed long)(p_lyr->h_swlyr));
        p_lyr->h_swlyr = 0;
    }

    if (p_lyr->status == GUI_LYRWIN_STA_ON)
    {
        Layer_DeattachFromVisList(GUI_LyrP2H(p_lyr));
        Layer_AttachToInVisList(GUI_LyrP2H(p_lyr));
    }

    if (p_lyr->status != GUI_LYRWIN_STA_SUSPEND)
    {
        wakeup = ORANGE_TRUE;
    }

    /* set status to suspend */
    p_lyr->status = GUI_LYRWIN_STA_SUSPEND;

    /* tell all the wins */
    if (wakeup)
    {
        __gui_win_t *p_win;
        __gui_msg_t msg;

        if (p_lyr->ManWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->ManWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_SLEEP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }

        if (p_lyr->TopMostWinZOrder.pTopMost)
        {
            p_win        = GUI_WinH2P(p_lyr->TopMostWinZOrder.pTopMost->hWnd);
            msg.id       = GUI_MSG_WIN_SLEEP;
            msg.h_srcwin = GUI_WinP2H(0);
            msg.p_arg    = 0;

            while (p_win)
            {
                msg.h_deswin = GUI_WinP2H(p_win);
                GUI_SendMessage(&msg);

                if (p_win->pZOrderNode->pNext)
                {
                    p_win = GUI_WinH2P(p_win->pZOrderNode->pNext->hWnd);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LyrWinSetSrcWindow
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinSetSrcWindow(H_LYR h_lyr, const RECT *rect)
{
    __s32                   ret;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __wrn("GUI_LyrWinSetSrcWindow ............");
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

    if (!rect)
    {
        ORANGE_WRN("rect empty!");
        return ORANGE_FAIL;
    }

#endif
    GUI_LyrLock();
    lyr_attr->lyrpara.src_win = *rect;
    __wrn("lyr_attr->h_hwlyr ............%08x", lyr_attr->h_hwlyr);

    /* free hardware layer resource                 */
    if (lyr_attr->h_hwlyr)
    {
        ret = GUI_LogicLayerSetSrcWin(gui_scndir, lyr_attr->devfp, lyr_attr->h_hwlyr, (__rect_t *)rect);

        if (ret != EPDK_OK)
        {
            ORANGE_WRN("layer set source windows failed!");
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(0);
#endif
    }

    GUI_LyrUnlock();
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinSetScnWindow
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinSetScnWindow(H_WIN h_lyr, const RECT *rect)
{
    __u32                   ret;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __disp_rect_t               disp_rect;
    __u64          pdispbuffer[3];
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

    if (!rect)
    {
        ORANGE_WRN("rect empty!");
        return ORANGE_FAIL;
    }

#endif
    GUI_LyrLock();
    lyr_attr->lyrpara.scn_win = *rect;

    if (lyr_attr->h_hwlyr)
    {
        if (lyr_attr->type == GUI_LYRWIN_NORMAL)
        {
            ret = GUI_LogicLayerSetScnWin(gui_scndir, lyr_attr->devfp, lyr_attr->h_hwlyr, (__rect_t *)rect);

            if (ret != EPDK_OK)
            {
                ORANGE_WRN("layer set source windows failed!");
                GUI_LyrUnlock();
                return ORANGE_FAIL;
            }
        }
        else
        {
#ifdef ORANGE_SUPPORT_SPRITE
XXXXXXX
            /*release hardware layer*/
            disp_rect.x         = rect->x;
            disp_rect.y         = rect->y;
            disp_rect.width     = rect->width;
            disp_rect.height    = rect->height;
            pdispbuffer[0] = lyr_attr->h_hwlyr;
            pdispbuffer[1] = (__u32)&disp_rect;
            pdispbuffer[2] = 0;
            ret = esMODS_MIoctrl(lyr_attr->devfp, DISP_CMD_SPRITE_BLOCK_SET_SCREEN_WINDOW, SEL_SCREEN, (void *)pdispbuffer);

            if (ret != EPDK_OK)
            {
                ORANGE_WRN("release hardware layer failed!");
                GUI_LyrUnlock();
                return ORANGE_FAIL;
            }

#endif
        }

#ifdef SIM_PC_WIN
        RefreshDeContent(0);
#endif
    }

    GUI_LyrUnlock();
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinGetSrcWindow
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinGetSrcWindow(H_WIN h_lyr, RECT *rect)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

    if (!rect)
    {
        ORANGE_WRN("rect empty!");
        return ORANGE_FAIL;
    }

#endif
    GUI_LyrLock();
    /* free hardware layer resource                 */
    *rect = lyr_attr->lyrpara.src_win;
    GUI_LyrUnlock();
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinGetScnWindow
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinGetScnWindow(H_WIN h_lyr, RECT *rect)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

    if (!rect)
    {
        ORANGE_WRN("rect empty!");
        return ORANGE_FAIL;
    }

#endif
    GUI_LyrLock();
    *rect = lyr_attr->lyrpara.scn_win;
    GUI_LyrUnlock();
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LyrWinGetFB
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinGetFB(H_LYR h_lyr, FB *fb)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
#if GUI_CHECK_ARG_EN > 0

    if (!h_lyr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

    if (!fb)
    {
        ORANGE_WRN("fb empty!");
        return ORANGE_FAIL;
    }

#endif
    GUI_LyrLock();
    *fb = *(lyr_attr->lyrpara.fb);
    GUI_LyrUnlock();
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LyrWinSetFB
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinSetFB(H_LYR h_lyr, const FB *fb)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

    if (!fb)
    {
        ORANGE_WRN("fb empty!");
        return ORANGE_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->status != GUI_LYRWIN_STA_SUSPEND)
    {
        GUI_LyrUnlock();
        return ORANGE_FAIL;
    }

    if (fb->addr[0] != NULL)
    {
        GUI_LyrUnlock();
        return ORANGE_FAIL;
    }

    *(lyr_attr->lyrpara.fb) = *fb;
    GUI_LyrUnlock();
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LyrWinGetSta
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__gui_lyrwin_sta_t GUI_LyrWinGetSta(H_LYR h_lyr)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __gui_lyrwin_sta_t        ret;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return GUI_LYRWIN_STA_ERR;
    }

#endif
    GUI_LyrLock();
    ret = (__gui_lyrwin_sta_t)lyr_attr->status;
    GUI_LyrUnlock();
    return ret;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinSetSta
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_LyrWinSetSta(H_LYR h_lyr, __gui_lyrwin_sta_t status)
{
    __s32                   ret_val;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        return ORANGE_FAIL;
    }

#endif
    ret_val  = EPDK_OK;
    GUI_LyrLock();
 
    switch (status)
    {
        case GUI_LYRWIN_STA_ON:
             _LyrOn(lyr_attr);
#ifdef SIM_PC_WIN
            RefreshDeContent(1);
#endif
            break;

        case GUI_LYRWIN_STA_OFF:
 			
            _LyrOff(lyr_attr);
#ifdef SIM_PC_WIN
            RefreshDeContent(1);
#endif
            break;

        case GUI_LYRWIN_STA_SLEEP:
 			
            _LyrSleep(lyr_attr);
#ifdef SIM_PC_WIN
            RefreshDeContent(1);
#endif
            break;

        case GUI_LYRWIN_STA_SUSPEND:
 			
            _LyrSuspend(lyr_attr);
#ifdef SIM_PC_WIN
            RefreshDeContent(1);
#endif
            break;

        default:
            ORANGE_WRN("sta not support! sta = %x", status);
            ret_val = ORANGE_FAIL;
            break;
    }

    GUI_LyrUnlock();
    return ret_val;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinSel
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32  GUI_LyrWinSel(H_LYR h_lyr)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;

    if (lyr_attr == NULL)
    {
        return EPDK_OK;
    }

    GUI_Lock();
    GUI_SelLayer((unsigned long)(lyr_attr->h_swlyr));
    GUI_Unlock();
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinFlushFB
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_LyrWinFlushFB(H_LYR h_lyr)
{
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        ORANGE_WRN("h_lyr empty!");
        __log("h_lyr empty!");
        return ORANGE_FAIL;
    }

#endif
    return GUI_FBFlushCache(lyr_attr->lyrpara.fb);
}

/*
**********************************************************************************************************************
*                                               GUI_LyrWinCreate
*
* author:
*
* date:             2009-7-25
*
* Description:      gui layerwincreate
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
H_LYR  GUI_LyrWinCreate(__layerwincreate_para_t *create_info)
{
    __gui_lyrwin_t   *pLayer   = NULL;
    /* alloc win struct memory                      */
    pLayer = (__gui_lyrwin_t *)orange_malloc(sizeof(__gui_lyrwin_t));
    if (!pLayer)
    {
        __log("alloc win memory failed!");
        return NULL;
    }

    /* init win struct contents                     */
    /* name                                         */
    eLIBs_memclr(pLayer->name, GUI_NAME_MAX);
    eLIBs_strncpy(pLayer->name, create_info->name, GUI_NAME_MAX - 1);
    /* copy lyrpara into struct                     */
    pLayer->lyrpara      = *create_info->lyrpara;
    pLayer->lyrpara.fb   = (FB *)orange_malloc(sizeof(FB));
    *pLayer->lyrpara.fb  = *create_info->lyrpara->fb;
    pLayer->status       = GUI_LYRWIN_STA_SUSPEND;
    pLayer->h_swlyr      = GUI_LyrP2H(0);
    pLayer->h_hwlyr      = GUI_LyrP2H(0);
    pLayer->h_uselyr     = GUI_LyrP2H(0);
    pLayer->h_exlyr      = GUI_LyrP2H(0);
    pLayer->type         = create_info->type;
    pLayer->Next         = NULL;
    /*init z order*/
    InitZOrderInfo(&pLayer->ManWinZOrder, NULL);
    InitZOrderInfo(&pLayer->TopMostWinZOrder, NULL);

    //Layer_AttachToInVisList(GUI_LyrP2H(pLayer));

    /* init status                                  */
    if (GUI_LyrWinSetSta(GUI_LyrP2H(pLayer), create_info->initsta) != EPDK_OK)
    {
        __log("lyr win cannot be set status!");
        goto err;
    }

    /*设置图层的内部使用状态*/
    pLayer->use_sts = LAYER_DRAW;
    return GUI_LyrP2H(pLayer);
err:

    if (pLayer->lyrpara.fb)
    {
        orange_mfree(pLayer->lyrpara.fb);
    }

    if (pLayer)
    {
        orange_mfree(pLayer);
    }

    return HWND_INVALID;
}
/*
**********************************************************************************************************************
*                                               GUI_LyrWinDelete
*
* author:
*
* date:             2009-7-25
*
* Description:      gui layerwindelete
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_LyrWinDelete(H_LYR  h_lyr)
{
    __gui_win_t             *next;    // hosted window list.
    __gui_win_t             *p_firstchild;
    __gui_lyrwin_t          *pLayer   = (__gui_lyrwin_t *)h_lyr;

    if (!h_lyr)
    {
        ORANGE_WRN("layer win handle error!");
        return ORANGE_FAIL;
    }

    if (!pLayer)
    {
        ORANGE_WRN("layer attr handle error!");
        return ORANGE_FAIL;
    }

    GUI_WinLock();

    if (pLayer->ManWinZOrder.pTopMost)
    {
        p_firstchild = (__gui_win_t *)pLayer->ManWinZOrder.pTopMost->hWnd;

        /* delete child win                             */
        while (1)
        {
            /* if child list is empty, no win need be delete                                                            */
            if (!p_firstchild)
            {
                break;
            }

            if(!p_firstchild->pZOrderNode)
            {
                __err("p_firstchild->pZOrderNode = NULL");
                break;
            }
            if (p_firstchild->pZOrderNode->pNext)
            {
                next = GUI_WinH2P(p_firstchild->pZOrderNode->pNext->hWnd);
            }
            else
            {
                next = NULL;
            }

            if (p_firstchild->WinType == TYPE_FRAMEWIN)
            {
#ifdef CONFIG_GUI_WIN_FRMWIN_EN

                if (GUI_FrmWinDelete((H_WIN)p_firstchild) != ORANGE_OK)
                {
                    ORANGE_WRN("Not all child window can be deleted!");
                    GUI_WinUnlock();
                    return EPDK_FAIL;
                }

#endif
            }
            else if (p_firstchild->WinType == TYPE_CONTROL)
            {
#ifdef CONFIG_GUI_WIN_CTRLWIN_EN

                if (GUI_CtrlWinDelete((H_WIN)p_firstchild) != ORANGE_OK)
                {
                    ORANGE_WRN("Not all child window can be deleted!");
                    GUI_WinUnlock();
                    return EPDK_FAIL;
                }

#endif
            }

            if (NULL == p_firstchild->pZOrderNode->pNext)
            {
                p_firstchild = NULL;
            }
            else
            {
                p_firstchild = next;
            }

            //p_firstchild = next;
        }
    }

    GUI_WinUnlock();

    /* release hw layer                             */
    if (GUI_LyrWinSetSta(h_lyr, GUI_LYRWIN_STA_SUSPEND) != EPDK_OK)
    {
        ORANGE_WRN("layer cannot be suspended!");
        return ORANGE_FAIL;
    };

    Layer_DeattachFromInVisList(h_lyr);

    /*free layer parameter framebuffer*/
    if (pLayer->lyrpara.fb)
    {
        orange_mfree((void *)pLayer->lyrpara.fb);
    }

    orange_mfree((void *)pLayer);
    pLayer   = 0;
    return EPDK_OK;
}

H_LYR GUI_LyrWinGetHWLyr(H_LYR h_lyr)
{
    __s32 ret_val;
    __gui_lyrwin_t *lyr_attr = (__gui_lyrwin_t *)h_lyr;

    if (lyr_attr == NULL)
    {
        return NULL;
    }

    return lyr_attr->h_hwlyr;
}
