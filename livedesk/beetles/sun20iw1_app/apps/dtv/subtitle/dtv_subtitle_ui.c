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
#include "dtv_subtitle_ui.h"

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))


typedef struct subtitle_ctrl_s
{
    H_LYR        layer;
    H_WIN        frmwin;
} subtitle_ctrl_t;

typedef struct subtitle_uipara_s
{
    RECT    subtitle;
} __subtitle_uipara_t;

static __subtitle_uipara_t subtitle_480_272 =
{
    {0, 272 - SUBTITLE_HIGHT, 480, SUBTITLE_HIGHT},
};

static H_LYR  g_layer = NULL;
static H_WIN g_h_subtitle = NULL;


signed long dtv_subtitle_create(H_WIN parent);


__subtitle_uipara_t *subtitle_get_uipara(void)
{
    ES_FILE         *disp;
    __s32           width;
    __s32           height;
    /* get lcd size*/
    dsk_display_get_size((__s32 *)&width, (__s32 *)&height);

    if ((width == 480) && (height == 272))
    {
        return &subtitle_480_272;
    }
    else
    {
        return &subtitle_480_272;
    }
}

static __s32 _subtitle_paint(CSubtitleDrawItem *pSub)
{
    __s32 i = 0;
    CSubtitleDrawModeISDB *sub;
    __subtitle_uipara_t *uipara;
    GUI_RECT rect;
    char text[128];
    sub = (CSubtitleDrawModeISDB *)&pSub->unionDrawMode.isdb;
    uipara = subtitle_get_uipara();

    if (NULL == g_layer)
    {
        __wrn("NULL == g_layer\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSetSta(g_layer, GUI_LYRWIN_STA_ON);
    //print_layer_info();
    GUI_LyrWinSel(g_layer);

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(g_layer)
        && GUI_LYRWIN_STA_SLEEP != GUI_LyrWinGetSta(g_layer))
    {
        __wrn("------------------------Err Frame Buf fail.\n");
        return EPDK_FAIL;
    }

    //com_memdev_create(g_layer);
    GUI_LyrWinSetTop(g_layer);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    //GUI_UC_SetEncodeISO885915();
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(0xDB000000);//
    //__msg("sub->position_flags = %d\n",sub->position_flags);
    //__msg("sub->startX = %d\n",sub->startX);
    //__msg("sub->startY = %d\n",sub->startY);
    //__msg("sub->subWidth = %d\n",sub->subWidth);
    //__msg("sub->subHeight = %d\n",sub->subHeight);
    rect.x0 = uipara->subtitle.x;
    rect.y0 = uipara->subtitle.y - uipara->subtitle.y;
    rect.x1 = rect.x0 + uipara->subtitle.width;
    rect.y1 = rect.y0 + uipara->subtitle.height;
    GUI_ClearRectEx(&rect);
    __msg("sub->subNum = %d\n", sub->subNum);

    for (i = 0; i < sub->subNum; i++)
    {
        //此处y坐标变成了相对图层的坐标，暂时不明原因
        rect.x0 = uipara->subtitle.x;
        rect.y0 = uipara->subtitle.y + i * uipara->subtitle.height / sub->subNum - uipara->subtitle.y;
        rect.x1 = rect.x0 + uipara->subtitle.width;
        rect.y1 = rect.y0 + uipara->subtitle.height / sub->subNum - 1;
        __msg("x0=%d y0=%d x1=%d y1=%d\n", rect.x0, rect.y0, rect.x1, rect.y1);
        eLIBs_strcpy(text, pSub->unionDrawMode.isdb.subData[i]);
        //GUI_ClearRectEx(&rect);
        //dsk_langres_get_menu_text(STRING_MOVIE_NEXT,text,sizeof(text));
        GUI_DispStringInRect(text, &rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
        //GUI_DispStringAt(text,rect.x0,rect.y0);
        __msg("text = %s \n", text);
    }

    //com_memdev_delete();
    return EPDK_OK;
}

static __s32 _subtitle_clean(CSubtitleDrawItem *pSub)
{
    //unsigned int i;
    GUI_RECT rect;
    __subtitle_uipara_t *uipara;

    //CSubtitleDrawModeISDB *sub = (CSubtitleDrawModeISDB *)&pSub->unionDrawMode.isdb;
    if (NULL == g_layer)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(g_layer);

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(g_layer)
        && GUI_LYRWIN_STA_SLEEP != GUI_LyrWinGetSta(g_layer))
    {
        __wrn("------------------------Err Frame Buf fail.\n");
        return EPDK_FAIL;
    }

#if 1
    //GUI_Clear();
    GUI_SetBkColor(0xDB000000);//
    uipara = subtitle_get_uipara();
    rect.x0 = uipara->subtitle.x;
    rect.y0 = uipara->subtitle.y - uipara->subtitle.y;
    rect.x1 = rect.x0 + uipara->subtitle.width;
    rect.y1 = rect.y0 + uipara->subtitle.height;
    GUI_ClearRectEx(&rect);
#endif
    GUI_LyrWinSetBottom(g_layer);
    return EPDK_OK;
}

static __bool g_show_subtitle = EPDK_FALSE;//字幕显示开关

//字幕切换,on ->off  或off->on
__s32 dtv_subtitle_switch_onoff(void)
{
    g_show_subtitle = !g_show_subtitle;

    if (EPDK_FALSE == g_show_subtitle) //off
    {
        if (g_h_subtitle)
        {
            dtv_subtitle_delete();
            g_h_subtitle = NULL;
        }
    }
    else
    {
        if (!g_h_subtitle)
        {
            dtv_subtitle_create(GUI_WinGetHandFromName(APP_DTV));
        }
    }

    return 0;
}

__s32 dtv_subtitle_paint(void *arg)
{
    CSubtitleDrawItem *pSub = NULL;
    CSubtitleDrawModeISDB *sub = NULL;
    __msg("g_show_subtitle = %d\n", g_show_subtitle);

    if (g_show_subtitle == FALSE)
    {
        return EPDK_OK;
    }

    pSub = (CSubtitleDrawItem *)arg;
    sub = (CSubtitleDrawModeISDB *)&pSub->unionDrawMode.isdb;
    __msg("sub->Clear_screen_flags = %d\n", sub->Clear_screen_flags);

    if (sub->Clear_screen_flags == 0)
    {
        return _subtitle_paint(pSub);
    }
    else
    {
        return _subtitle_clean(pSub);
    }
}

extern __mp *maple;
static __s32 _dtv_subtitle_frmwin_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            //return maple_set_subtitle_cb(dtv_subtitle_paint);
            /*{
                __pCBK_t            callback;


                callback = esKRNL_GetCallBack(dtv_subtitle_paint);

                return esMODS_MIoctrl( maple, MAPLE_CMD_SET_SPLY_DRAW_CB, NULL, (void*)callback );
            }*/
            break;

        case GUI_MSG_CLOSE:
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            return EPDK_OK;

        case GUI_MSG_DESTROY:
            return EPDK_OK;

        case GUI_MSG_PAINT:
            return EPDK_OK;

        case GUI_MSG_KEY:
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
    建立窗口
*/
static H_WIN dtv_subtitle_frmwin_create(H_WIN h_parent, H_LYR layer)
{
    __gui_framewincreate_para_t framewin_para;
    FB fb;

    if (layer == NULL)
    {
        return NULL;
    }

    GUI_LyrWinGetFB(layer, &fb);
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = SUBTITLE_FRMWIN;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.hHosting      = h_parent;
    framewin_para.id            = SUBTITLE_FRMWIN_ID;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dtv_subtitle_frmwin_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          =  NULL;
    framewin_para.hLayer        = layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

static __s32 subtitle_main_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            subtitle_ctrl_t *sub_ctrl = NULL;
            RECT rect;
            __subtitle_uipara_t *uipara;
            uipara = subtitle_get_uipara();
            sub_ctrl = esMEMS_Malloc(0, sizeof(subtitle_ctrl_t));

            if (NULL == sub_ctrl)
            {
                __wrn("mem not enough...\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(sub_ctrl, 0, sizeof(subtitle_ctrl_t));
            sub_ctrl->layer  = dtv_com_layer_create(&uipara->subtitle, "subtitle layer");

            if (NULL == sub_ctrl->layer)
            {
                return EPDK_FAIL;
            }

            __msg("dvb_subtitle_layer_create ok\n");
            g_layer = sub_ctrl->layer;
            sub_ctrl->frmwin = dtv_subtitle_frmwin_create(msg->h_deswin, sub_ctrl->layer);

            if (sub_ctrl->frmwin)
            {
                GUI_WinSetFocusChild(sub_ctrl->frmwin);
            }
            else
            {
                GUI_LyrWinDelete(sub_ctrl->layer);
                sub_ctrl->layer = NULL;

                if (NULL != sub_ctrl)
                {
                    esMEMS_Mfree(0, sub_ctrl);
                    sub_ctrl = NULL;
                }
            }

            __msg("sub_ctrl->frmwin = %08x\n", sub_ctrl->frmwin);
            GUI_LyrWinSel(sub_ctrl->layer);
            GUI_LyrWinSetBottom(sub_ctrl->layer);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)sub_ctrl);
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            subtitle_ctrl_t *sub_ctrl = NULL;
            sub_ctrl = (subtitle_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (NULL == sub_ctrl)
            {
                return EPDK_FAIL;
            }

            if (sub_ctrl->layer)
            {
                GUI_LyrWinSetTop(sub_ctrl->layer);
                GUI_LyrWinDelete(sub_ctrl->layer);
                g_layer = NULL;
            }

            if (NULL != sub_ctrl)
            {
                esMEMS_Mfree(0, sub_ctrl);
                sub_ctrl = NULL;
            }

            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}


//字幕要做的主要两件事
//1、创建字幕绘制图层
//2、调用maple_set_subtitle_cb()设置绘制字幕回调函数给中间件
signed long dtv_subtitle_create(H_WIN parent)
{
    __gui_manwincreate_para_t create_info;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    __msg("dtv_subtitle_create \n");
    create_info.name = DTV_SUBTITLE;
    create_info.id = DTV_SUBTITLE_ID;
    create_info.hParent = parent;//GUI_WinGetRootWin();
    create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)subtitle_main_proc);
    create_info.attr = NULL;
    create_info.hHosting = NULL;
    g_h_subtitle = (GUI_ManWinCreate(&create_info));
    return (signed long)g_h_subtitle;
}

__s32 dtv_subtitle_delete(void)
{
    if (g_layer)
    {
        GUI_LyrWinSetBottom(g_layer);
        //GUI_LyrWinDelete(g_layer);
        GUI_ManWinDelete(GUI_WinGetHandFromName(DTV_SUBTITLE));
        g_layer = NULL;
    }

    g_show_subtitle = EPDK_FALSE;
    return EPDK_OK;
}

__bool dtv_has_subtitle(void)
{
    return g_show_subtitle;
}

#endif
