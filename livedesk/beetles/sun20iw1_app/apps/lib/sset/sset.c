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
#include "sset_i.h"

typedef struct  tag_SCENE_DATA
{
    H_WIN               sset_mwin;              /*SSET管理窗口句柄*/

    H_WIN               menu_lwin[MENU_CNT];    /*子菜单窗口layer窗口句柄*/
    H_WIN               menu_fwin[MENU_CNT];    /*子菜单窗口frame窗口句柄*/
    __sset_menu_type_e  menu_type[MENU_CNT];    /*子菜单窗口frame窗口句柄*/

    H_WIN               info_lwin;              /*信息栏窗口layer窗口句柄*/
    H_WIN               info_fwin;              /*信息栏窗口frame窗口句柄*/

    __sset_item_t       *phead;                 /*内部数据头指针*/
    __sset_item_t       *pmenu;                 /*主菜单节点指针*/
    __sset_uipara_t     *puipara;               /*SSET的layout数据指针*/
    __s32               menu_cnt;               /*子菜单当前所在级数*/

    __u8                time_sta;               /*GUI TIME 是否有效 1有效 0无效*/
    __u8                time_enable;            /*是否发送GUI_TIME消息 1发 0不发*/
    __u32               time_id;                /*GUI TIME ID*/

    __u32               sset_sta;               /*SSET场景是否退出 1退出 0未退出*/
    __u32               last_key;               /*保存上一次按键*/

    __bool              tbar_has_hide;          /*tbar是否是隐藏1为隐藏，0为显示*/

} sset_scene_data_t;

static  sset_scene_data_t   sset_data;
__sset_item_t *_sset_menu_get_pmenu(void)//..
{
    return sset_data.pmenu;
}

/*
************************************************************************************************************************
*                                       calendar_main
*
*Description: 创建一个LYR WIN
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static H_WIN _sset_lyr_create_for_lstm(void)
{
    FB  fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,
        1,
        0xe0,

        1,
        9,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };
    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        0
    };
    __sset_uipara_layer_t *layer = &sset_data.puipara->lstm_lyr;
    __sset_item_t *user_para = sset_data.pmenu;
    int sub_item = 0;

    while (user_para)
    {
        sub_item++;
        user_para = user_para->next;
    }

    if (sub_item >= 7)
    {
        sub_item = 7;
    }

    __wrn("layer->fbh=%d\n", layer->fbh);
    fb.size.width  = layer->fbw;
    fb.size.height = layer->fbh;
    //////////////////////////////////////////////////////////////////////////
    //..{//add for 8bpp
    //..    fb.fmt.fmt.rgb.pixelfmt = layer->fmt;
    //..    if (layer->fmt <= PIXEL_MONO_8BPP)
    //..    {
    //..        para.mode = DISP_LAYER_WORK_MODE_PALETTE;
    //..    }
    //..}
    //////////////////////////////////////////////////////////////////////////
    para.pipe           = layer->pipe;
    para.scn_win.x      = layer->scnx;
    para.scn_win.y      = layer->scny;
    para.scn_win.width  = layer->scnw;
    para.scn_win.height = layer->scnh * sub_item;
    //eLIBs_printf("_sset_lyr_create:x=%d y=%d width=%d height=%d \n",layer->scnx,layer->scny,layer->scnw,layer->scnh);
    para.src_win.x      = layer->srcx;
    para.src_win.y      = layer->srcy;
    para.src_win.width  = layer->srcw;
    para.src_win.height = layer->srch * sub_item;
    //eLIBs_printf("_sset_lyr_create SRC :x=%d y=%d width=%d height=%d \n",layer->srcx,layer->srcy,layer->srcw,layer->srch);
    para.fb = &fb;
    create_info.lyrpara = &para;
    __wrn("fb.size.height=%d\n", fb.size.height);
    return (GUI_LyrWinCreate(&create_info));
}
static H_WIN _sset_lyr_create(__sset_uipara_layer_t *layer)
{
    FB  fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,
        1,
        0xe0,

        1,
        9,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };
    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        0
    };
    __wrn("layer->fbh=%d\n", layer->fbh);
    fb.size.width  = layer->fbw;
    fb.size.height = layer->fbh;
    //////////////////////////////////////////////////////////////////////////
    //..{//add for 8bpp
    //..    fb.fmt.fmt.rgb.pixelfmt = layer->fmt;
    //..    if (layer->fmt <= PIXEL_MONO_8BPP)
    //..    {
    //..        para.mode = DISP_LAYER_WORK_MODE_PALETTE;
    //..    }
    //..}
    //////////////////////////////////////////////////////////////////////////
    para.pipe           = layer->pipe;
    para.scn_win.x      = layer->scnx;
    para.scn_win.y      = layer->scny;
    para.scn_win.width  = layer->scnw;
    para.scn_win.height = layer->scnh;
    //eLIBs_printf("_sset_lyr_create:x=%d y=%d width=%d height=%d \n",layer->scnx,layer->scny,layer->scnw,layer->scnh);
    para.src_win.x      = layer->srcx;
    para.src_win.y      = layer->srcy;
    para.src_win.width  = layer->srcw;
    para.src_win.height = layer->srch;
    //eLIBs_printf("_sset_lyr_create SRC :x=%d y=%d width=%d height=%d \n",layer->srcx,layer->srcy,layer->srcw,layer->srch);
    para.fb = &fb;
    create_info.lyrpara = &para;
    __wrn("fb.size.height=%d\n", fb.size.height);
    return (GUI_LyrWinCreate(&create_info));
}




static H_WIN _sset_Tbarlyr_create(__sset_uipara_layer_t *layer)
{
    FB  fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,
        1,
        0xe0,

        1,
        10,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };
    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        0
    };
    __wrn("layer->fbh=%d\n", layer->fbh);
    fb.size.width  = layer->fbw;
    fb.size.height = layer->fbh;
    //////////////////////////////////////////////////////////////////////////
    //..{//add for 8bpp
    //..    fb.fmt.fmt.rgb.pixelfmt = layer->fmt;
    //..    if (layer->fmt <= PIXEL_MONO_8BPP)
    //..    {
    //..        para.mode = DISP_LAYER_WORK_MODE_PALETTE;
    //..    }
    //..}
    //////////////////////////////////////////////////////////////////////////
    para.pipe           = layer->pipe;
    para.scn_win.x      = layer->scnx;
    para.scn_win.y      = layer->scny;
    para.scn_win.width  = layer->scnw;
    para.scn_win.height = layer->scnh;
    para.src_win.x      = layer->srcx;
    para.src_win.y      = layer->srcy;
    para.src_win.width  = layer->srcw;
    para.src_win.height = layer->srch;
    para.fb = &fb;
    create_info.lyrpara = &para;
    __wrn("fb.size.height=%d\n", fb.size.height);
    return (GUI_LyrWinCreate(&create_info));
}

/*
************************************************************************************************************************
*                                       _show_win
*
*Description: 打开图层显示
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _show_win(__s32 menu_cnt)
{
    __u32       i;
    __u32       scn_height;
    __u32       scn_width;
    RECT        scnrect;
    RECT        srcrect;

    //eLIBs_printf("sset_data.menu_type[menu_cnt]=%d \n",sset_data.menu_type[menu_cnt]);
    if (sset_data.menu_type[menu_cnt] == MENU_TYPE_TBAR)
    {
#if 0   //慢慢淡出有问题，与DE2。0显示方式有关，待查
        GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
        eLIBs_printf("x=%d y=%d width=%d height=%d \n", scnrect.x, scnrect.y, scnrect.width, scnrect.height);
        scn_height = scnrect.height / 10 * 10;
        scnrect.y = scnrect.y + scn_height;
        scnrect.height = scnrect.height % 10;
#else
        GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
        eLIBs_printf("x=%d y=%d width=%d height=%d \n", scnrect.x, scnrect.y, scnrect.width, scnrect.height);
#endif
        //GUI_LyrWinSetScnWindow( sset_data.menu_lwin[menu_cnt], &scnrect );
        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(sset_data.menu_lwin[menu_cnt]);
        GUI_LyrWinCacheOff();
        //  for(i = 1 ; i <= 10; i++)
        //  {
        //  GUI_LyrWinCacheOn();
        //  scnrect.y = scnrect.y - scn_height/10;
        //  scnrect.height = scnrect.height + scn_height/10;
        //eLIBs_printf("x=%d .y=%d w=%d h=%d\n",scnrect.x,scnrect.y,scnrect.width,scnrect.height);
        //  GUI_LyrWinSetScnWindow( sset_data.menu_lwin[menu_cnt], &scnrect );
        //  GUI_LyrWinCacheOff();
        //esKRNL_TimeDly(100);
        //}
    }
    else if (sset_data.menu_type[menu_cnt] == MENU_TYPE_LSTM)
    {
        if (1) //if(set_scn_size == SCREEN_SIZE_800_480)
        {
            __wrn("GUI_LyrWinSetSta\n");
            //GUI_LyrWinSetSta( sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_ON );
            //GUI_LyrWinSetTop( sset_data.menu_lwin[menu_cnt] );
            //return ;
            GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
#if 0
            scn_width = scnrect.width / 10 * 10;
            scnrect.x = scnrect.x + scn_width;
#endif
            //scnrect.width = scnrect.width % 10;
            GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(sset_data.menu_lwin[menu_cnt]);
            GUI_LyrWinCacheOff();
            //for(i = 1 ; i <= 10; i++)
            {
                //GUI_LyrWinCacheOn();
                //scnrect.x = scnrect.x - scn_width/10;
                //scnrect.width = scnrect.width + scn_width/10;
                //eLIBs_printf("MENU_TYPE_LSTM x=%d .y=%d w=%d h=%d\n",scnrect.x,scnrect.y,scnrect.width,scnrect.height);
                //GUI_LyrWinSetScnWindow( sset_data.menu_lwin[menu_cnt], &scnrect );
                //GUI_LyrWinCacheOff();
            }
        }
        else
        {
            GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinGetSrcWindow(sset_data.menu_lwin[menu_cnt], &srcrect);
            scn_height = scnrect.height / 10 * 10;
            scnrect.height = scnrect.height % 10;
            srcrect.y = srcrect.y + scn_height;
            GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinSetSrcWindow(sset_data.menu_lwin[menu_cnt], &srcrect);
            GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(sset_data.menu_lwin[menu_cnt]);
            GUI_LyrWinCacheOff();

            for (i = 1 ; i <= 10; i++)
            {
                GUI_LyrWinCacheOn();
                scnrect.height = scnrect.height + scn_height / 10;
                srcrect.y = srcrect.y - scn_height / 10;
                GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
                GUI_LyrWinSetSrcWindow(sset_data.menu_lwin[menu_cnt], &srcrect);
                GUI_LyrWinCacheOff();
            }
        }
    }
}

/*
************************************************************************************************************************
*                                       _hide_win
*
*Description: 打开图层显示
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _hide_win(__s32 menu_cnt)
{
    __u32       i;
    __u32       scn_height;
    __u32       scn_width;
    RECT        scnrect;
    RECT        srcrect;

    if (sset_data.menu_type[menu_cnt] == MENU_TYPE_TBAR)
    {
        GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
        scn_height = scnrect.height / 10 * 10;

        for (i = 1 ; i < 10; i++)
        {
            GUI_LyrWinCacheOn();
            scnrect.y = scnrect.y + scn_height / 10;
            scnrect.height = scnrect.height - scn_height / 10;
            GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinCacheOff();
        }

        GUI_LyrWinCacheOn();
        GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_SLEEP);
        scnrect.y = scnrect.y - scn_height * 9 / 10;
        scnrect.height = scnrect.height + scn_height * 9 / 10;
        GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
        GUI_LyrWinCacheOff();
    }
    else if (sset_data.menu_type[menu_cnt] == MENU_TYPE_LSTM)
    {
        if (1) //if(set_scn_size == SCREEN_SIZE_800_480)
        {
            GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            scn_width = scnrect.width / 10 * 10;

            for (i = 1 ; i < 10; i++)
            {
                GUI_LyrWinCacheOn();
                scnrect.x = scnrect.x + scn_width / 10;
                scnrect.width = scnrect.width - scn_width / 10;
                GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
                GUI_LyrWinCacheOff();
            }

            GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_SLEEP);
            scnrect.x = scnrect.x - scn_width * 9 / 10;
            scnrect.width = scnrect.width + scn_width * 9 / 10;
            GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinCacheOff();
        }
        else
        {
            GUI_LyrWinGetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinGetSrcWindow(sset_data.menu_lwin[menu_cnt], &srcrect);
            scn_height = scnrect.height / 10 * 10;

            for (i = 1 ; i < 10; i++)
            {
                GUI_LyrWinCacheOn();
                scnrect.height = scnrect.height - scn_height / 10;
                srcrect.y = srcrect.y + scn_height / 10;
                GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
                GUI_LyrWinSetSrcWindow(sset_data.menu_lwin[menu_cnt], &srcrect);
                GUI_LyrWinCacheOff();
            }

            GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_SLEEP);
            scnrect.height = scnrect.height + scn_height * 9 / 10;
            srcrect.y = srcrect.y - scn_height * 9 / 10;
            GUI_LyrWinSetScnWindow(sset_data.menu_lwin[menu_cnt], &scnrect);
            GUI_LyrWinSetSrcWindow(sset_data.menu_lwin[menu_cnt], &srcrect);
            GUI_LyrWinCacheOff();
        }
    }
}

/*
************************************************************************************************************************
*                                       _sset_menu_setfcs
*
*Description: 设置tbar状态
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _sset_menu_setfcs(__u32 fcs_id)
{
    __u32               i;

    if (sset_data.menu_type[sset_data.menu_cnt] == MENU_TYPE_TBAR)
    {
        for (i = 0; i < fcs_id; i++)
        {
            sset_data.pmenu = sset_data.pmenu->next;
        }

        SSET_MenuTbar_SetFocus(sset_data.menu_fwin[sset_data.menu_cnt], fcs_id);
    }
    else if (sset_data.menu_type[sset_data.menu_cnt] == MENU_TYPE_LSTM)
    {
        for (i = 0; i < fcs_id; i++)
        {
            sset_data.pmenu = sset_data.pmenu->next;
        }

        SSET_MenuLstm_SetFocus(sset_data.menu_fwin[sset_data.menu_cnt], fcs_id);
    }
}

/*
************************************************************************************************************************
*                                       SSET_SubMenuDestroyAll
*
*Description: 销毁所有子菜单submenu
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _sset_menu_create(__u32 fcs_id, __sset_menu_type_e type)
{
    if (sset_data.menu_cnt == -1)
    {
        sset_data.menu_cnt = 0;
    }
    else
    {
        sset_data.menu_cnt = sset_data.menu_cnt + 1;
    }

    switch (type)
    {
        case MENU_TYPE_TBAR:
        {
            __u32            n_tmp = 0;
            __sset_item_t   *p_tmp;
            p_tmp = sset_data.phead->childlst;

            while (p_tmp)
            {
                n_tmp = n_tmp + 1;
                p_tmp = p_tmp->next;
            }

            if (sset_data.menu_lwin[sset_data.menu_cnt] == NULL)
            {
                sset_data.puipara->tbar_lyr.srcx = sset_data.puipara->menu_tbar_item.w * 2;
                sset_data.puipara->tbar_lyr.srcy = 0;

                if (set_scn_size == SCREEN_SIZE_480_272)
                {
                    if ((sset_data.puipara->menu_tbar_item.w * n_tmp) < sset_data.puipara->tbar_lyr.scnw)
                    {
                        sset_data.puipara->tbar_lyr.srcw = sset_data.puipara->tbar_lyr.scnw + sset_data.puipara->menu_tbar_item.w * 2;
                        sset_data.puipara->tbar_lyr.fbw  = sset_data.puipara->tbar_lyr.scnw + sset_data.puipara->menu_tbar_item.w * 4;
                    }
                    else
                    {
                        sset_data.puipara->tbar_lyr.srcw = sset_data.puipara->menu_tbar_item.w * (n_tmp + 2);
                        sset_data.puipara->tbar_lyr.fbw  = sset_data.puipara->menu_tbar_item.w * (n_tmp + 5);
                    }
                }
                else
                {
                    if ((sset_data.puipara->menu_tbar_item.w * n_tmp) < sset_data.puipara->tbar_lyr.scnw)
                    {
                        sset_data.puipara->tbar_lyr.srcw = sset_data.puipara->tbar_lyr.scnw;
                        sset_data.puipara->tbar_lyr.fbw  = sset_data.puipara->tbar_lyr.scnw + sset_data.puipara->menu_tbar_item.w * 3;
                    }
                    else
                    {
                        sset_data.puipara->tbar_lyr.srcw = sset_data.puipara->menu_tbar_item.w * n_tmp;
                        sset_data.puipara->tbar_lyr.fbw  = sset_data.puipara->menu_tbar_item.w * (n_tmp + 3);
                    }
                }

                sset_data.puipara->tbar_lyr.srch = sset_data.puipara->tbar_lyr.scnh;
                sset_data.puipara->tbar_lyr.fbh  = sset_data.puipara->tbar_lyr.srch;
                sset_data.pmenu = sset_data.pmenu->childlst;
                sset_data.menu_lwin[sset_data.menu_cnt] = _sset_lyr_create(&sset_data.puipara->tbar_lyr);
                sset_data.menu_fwin[sset_data.menu_cnt] = SSET_MenuTbar_Create(sset_data.sset_mwin, sset_data.menu_lwin[sset_data.menu_cnt], sset_data.pmenu);
                sset_data.menu_type[sset_data.menu_cnt] = type;
                _sset_menu_setfcs(fcs_id);
            }

            _show_win(sset_data.menu_cnt);
            sset_data.tbar_has_hide = EPDK_FALSE;
            GUI_WinSetFocusChild(sset_data.menu_fwin[sset_data.menu_cnt]);
        }
        break;

        case MENU_TYPE_LSTM:
        {
            if (sset_data.menu_lwin[sset_data.menu_cnt] == NULL)
            {
                //                  if(set_scn_size != SCREEN_SIZE_800_480)
                //                  {
                //                      sset_data.puipara->lstm_lyr.scnx = (sset_data.puipara->sset_lcd.w-sset_data.puipara->lstm_lyr.scnw) / 2;
                //                      sset_data.puipara->lstm_lyr.scny = (sset_data.puipara->sset_lcd.h-sset_data.puipara->lstm_lyr.scnh) / 2;
                //                  }
                /*
                sset_data.puipara->lstm_lyr.srcx = 0;
                sset_data.puipara->lstm_lyr.srcy = 0;
                sset_data.puipara->lstm_lyr.srcw = sset_data.puipara->lstm_lyr.scnw;
                sset_data.puipara->lstm_lyr.srch = sset_data.puipara->lstm_lyr.scnh * 2;
                sset_data.puipara->lstm_lyr.fbw  = sset_data.puipara->lstm_lyr.srcw;
                sset_data.puipara->lstm_lyr.fbh  = sset_data.puipara->lstm_lyr.srch;
                */
                sset_data.pmenu = sset_data.pmenu->childlst;
                // sset_data.menu_lwin[sset_data.menu_cnt] = _sset_lyr_create(&sset_data.puipara->lstm_lyr);
                sset_data.menu_lwin[sset_data.menu_cnt] = _sset_lyr_create_for_lstm();
                sset_data.menu_fwin[sset_data.menu_cnt] = SSET_MenuLstm_Create(sset_data.sset_mwin, sset_data.menu_lwin[sset_data.menu_cnt], sset_data.pmenu);
                sset_data.menu_type[sset_data.menu_cnt] = type;
                _sset_menu_setfcs(fcs_id);
            }

            _show_win(sset_data.menu_cnt);
            GUI_WinSetFocusChild(sset_data.menu_fwin[sset_data.menu_cnt]);
        }
        break;

        default:
            break;
    }
}


/*
************************************************************************************************************************
*                                       SSET_SubMenuDestroyAll
*
*Description: 销毁所有子菜单submenu
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _sset_menu_destroy(__s32 menu_cnt)
{
    if (menu_cnt == 0)
    {
        if (sset_data.menu_lwin[menu_cnt])
        {
            //_hide_win(menu_cnt);
            sset_data.tbar_has_hide = EPDK_TRUE;

            if (sset_data.menu_fwin[sset_data.menu_cnt])
            {
                SSET_MenuTbar_Destroy(sset_data.menu_fwin[menu_cnt]);
                sset_data.menu_fwin[menu_cnt] = 0;
            }

            GUI_LyrWinDelete(sset_data.menu_lwin[menu_cnt]);
            sset_data.menu_lwin[menu_cnt] = 0;
            sset_data.pmenu = sset_data.pmenu->father;
            sset_data.menu_cnt = -1;
        }
    }
    else
    {
        if (sset_data.menu_lwin[menu_cnt])
        {
            //_hide_win(menu_cnt);
            if (sset_data.menu_fwin[menu_cnt])
            {
                GUI_FrmWinDelete(sset_data.menu_fwin[menu_cnt]);
                sset_data.menu_fwin[menu_cnt] = 0;
            }

            GUI_LyrWinDelete(sset_data.menu_lwin[menu_cnt]);
            sset_data.menu_lwin[menu_cnt] = 0;
            sset_data.pmenu = sset_data.pmenu->father;
            sset_data.menu_cnt = menu_cnt - 1;
        }
    }
}

/*
************************************************************************************************************************
*                                       _sset_menu_sleep
*
*Description: 销毁所有子菜单submenu
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _sset_menu_sleep(__s32 menu_cnt)
{
    if (sset_data.menu_lwin[menu_cnt])
    {
        _hide_win(menu_cnt);

        if (sset_data.menu_cnt == 0)
        {
            sset_data.tbar_has_hide = EPDK_TRUE;
        }

        GUI_LyrWinSetSta(sset_data.menu_lwin[menu_cnt], GUI_LYRWIN_STA_SLEEP);
    }
}

/*
************************************************************************************************************************
*                                       SSET_SubMenuSleep
*
*Description: 进入一下级菜单
*
*Arguments  : hparent 父窗口
*
*Return     :
*
************************************************************************************************************************
*/
static void _sset_menu_next(__u32 fcs_id, __sset_menu_type_e type)
{
    _sset_menu_create(fcs_id, type);
}

/*
************************************************************************************************************************
*                                       _sset_submenuprev
*
*Description: 返回上一级菜单
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _sset_menu_prev(void)
{
    if (sset_data.menu_cnt >= 0)
    {
        if (sset_data.menu_lwin[sset_data.menu_cnt])
        {
            _show_win(sset_data.menu_cnt);

            if (sset_data.menu_cnt == 0)
            {
                sset_data.tbar_has_hide = EPDK_FALSE;
            }

            GUI_WinSetFocusChild(sset_data.menu_fwin[sset_data.menu_cnt]);
        }
    }
}

/*
************************************************************************************************************************
*                                       _sset_keyproc
*
*Description: SSET 按键处理
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _sset_keyproc(__gui_msg_t *msg)
{
    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_RISE:
        {
            __s32                       ret;
            __sset_create_para_t        *psset;
            __wrn("aaaaa test menulist longdown 3\n");
            psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (sset_data.last_key == GUI_MSG_KEY_ESCAPE)
            {
                _sset_menu_destroy(sset_data.menu_cnt);
                {
                    __gui_msg_t         ntymsg;
                    ntymsg.id           =  GUI_MSG_COMMAND;
                    ntymsg.h_srcwin     =  msg->h_deswin;
                    ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                    ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_STA_CHG);
                    ntymsg.dwAddData2   =  sset_data.menu_cnt + 1;
                    ntymsg.p_arg        =  0;
                    ret = GUI_SendNotifyMessage(&ntymsg);

                    if (ret == -1)
                    {
                        return EPDK_FAIL;
                    }
                }

                if (sset_data.sset_sta == 1)
                {
                    return EPDK_FAIL;
                }

                _sset_menu_prev();
                sset_data.last_key = 0xffffffff;
                return EPDK_OK;
            }

            sset_data.last_key = 0xffffffff;
        }
        break;

        default:
        {
            __wrn("aaaaa test menulist longdown 4\n");
            sset_data.last_key = msg->dwAddData1;
        }
        break;
    }

    return GUI_ManWinDefaultProc(msg);
}

/*判断sset tbar 的图层是否是隐藏还是现实状态状态*/
__bool SSET_tbar_is_show(void)
{
    if (sset_data.tbar_has_hide == EPDK_FALSE)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

/*判断当前SSET 是否是TBAR  状态，1为是，0为不是*/
__bool SSET_court_is_tbar(void)
{
    if (sset_data.menu_cnt == 0)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}
/*把tbar show 出来*/
void SSET_show_tbar(void)
{
    _show_win(0);
    sset_data.tbar_has_hide = EPDK_FALSE;
    GUI_WinSetFocusChild(sset_data.menu_fwin[0]);
}

void SSET_set_curt_lyr_top(void)
{
    GUI_LyrWinSetTop(sset_data.menu_lwin[sset_data.menu_cnt]);
}

void SSET_item_update(__u32 icon_id, __u32 index, __u32 fcs_sta) //..
{
    SSET_MenuTbar_UpdateItem(sset_data.menu_fwin[0], icon_id, index, fcs_sta);
}

void SSET_hide_tbar(void)
{
    _hide_win(0);
    sset_data.tbar_has_hide = EPDK_TRUE;
    GUI_LyrWinSetSta(sset_data.menu_lwin[0], GUI_LYRWIN_STA_SLEEP);
}


static __s32 _sset_touchproc(__gui_msg_t *msg)
{
    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_OVERDOWN:
        case GUI_MSG_TOUCH_OVERMOVE:
            return EPDK_OK;

        case GUI_MSG_TOUCH_OVERUP:
        {
            __s32                       ret;
            __sset_create_para_t        *psset;
            psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (sset_data.menu_cnt == 0 && sset_data.tbar_has_hide == EPDK_FALSE)       /*当是tbar 并且他的状态是show下隐藏它*/
            {
                if (sset_data.menu_lwin[0])
                {
                    _hide_win(sset_data.menu_cnt);
                    sset_data.tbar_has_hide = EPDK_TRUE;
                    /*算便把图层给设成OFF*/
                    GUI_LyrWinSetSta(sset_data.menu_lwin[0], GUI_LYRWIN_STA_SLEEP);
                }
            }
            else
            {
                _sset_menu_destroy(sset_data.menu_cnt);
            }

            {
                __gui_msg_t         ntymsg;
                ntymsg.id           =  GUI_MSG_COMMAND;
                ntymsg.h_srcwin     =  msg->h_deswin;
                ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_STA_CHG);
                ntymsg.dwAddData2   =  sset_data.menu_cnt + 1;
                ntymsg.p_arg        =  0;
                ret = GUI_SendNotifyMessage(&ntymsg);

                if (ret == -1)
                {
                    return EPDK_FAIL;
                }
            }

            if (sset_data.sset_sta == 1)
            {
                return EPDK_FAIL;
            }

            if (sset_data.menu_cnt != 0) /*非tbar 的情况下*/
            {
                _sset_menu_prev();
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*                                       _sset_command
*
*Description: SSET COMMAND处理
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _sset_command(__gui_msg_t *msg)
{
    if (LOSWORD(msg->dwAddData1) == SSET_SUBMENU_ID)
    {
        __s32                   ret;
        __sset_item_t           *user_para;
        __sset_item_ncmsg_t     *usermsg;
        __sset_create_para_t    *psset;
        __wrn("aaaaa test menulist longdown 6\n");
        psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
        user_para = (__sset_item_t *)msg->dwReserved;
        sset_data.pmenu = user_para;                        /*将psubmenu指向焦点节点*/
        usermsg = &user_para->item_msg;
        {
            /*将被点击的菜单项所对应的通知码回传*/
            __gui_msg_t         ntymsg;
            ntymsg.id           =  GUI_MSG_COMMAND;
            ntymsg.h_srcwin     =  msg->h_deswin;
            ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
            ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, usermsg->msg_id);
            ntymsg.dwAddData2   =  usermsg->msg_value;
            ntymsg.dwReserved   =  0;
            ntymsg.p_arg        =  0;
            ret = GUI_SendNotifyMessage(&ntymsg);

            if (ret == -1)
            {
                return EPDK_FAIL;
            }
        }

        if (sset_data.sset_sta == 1)
        {
            return EPDK_FAIL;
        }

        if (user_para->childlst)
        {
            _sset_menu_sleep(sset_data.menu_cnt);
            {
                __gui_msg_t         ntymsg;
                ntymsg.id           =  GUI_MSG_COMMAND;
                ntymsg.h_srcwin     =  msg->h_deswin;
                ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_STA_NEXT);
                ntymsg.dwAddData2   =  sset_data.menu_cnt + 1;
                ntymsg.dwReserved   =  0;
                ntymsg.p_arg        =  0;
                ret = GUI_SendNotifyMessage(&ntymsg);

                if (ret == -1)
                {
                    return EPDK_FAIL;
                }
            }
        }

        if (sset_data.sset_sta == 1)
        {
            return EPDK_FAIL;
        }

        if (user_para->childlst)
        {
            __wrn("aaaaa test menulist longdown 7\n");
            /*子节点非空，进入下一级子菜单*/
            _sset_menu_next(0, MENU_TYPE_LSTM);
        }
    }
    else if (LOSWORD(msg->dwAddData1) == SSET_MENU_ID)
    {
        __wrn("aaaaa test menulist longdown 8\n");

        switch (HISWORD(msg->dwAddData1))
        {
            case TB_UNPUSHED:
            {
                __u32                       i;
                __s32                       ret;
                __sset_item_t               *user_para;
                __sset_item_ncmsg_t         *usermsg;
                __sset_create_para_t        *psset;
                psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
                user_para = sset_data.phead->childlst;

                for (i = 0; i < msg->dwAddData2; i++)
                {
                    user_para = user_para->next;
                }

                sset_data.pmenu = user_para;
                usermsg = &user_para->item_msg;

                if (user_para->item_sta == 1)
                {
                    {
                        __gui_msg_t         ntymsg;
                        __wrn("//////////message1\n");
                        __wrn("psset->sid=%d\n", psset->sid);
                        __wrn("usermsg->msg_id=%d\n", usermsg->msg_id);
                        __wrn("usermsg->msg_value=%d\n", usermsg->msg_value);
                        ntymsg.id           =  GUI_MSG_COMMAND;
                        ntymsg.h_srcwin     =  msg->h_deswin;
                        ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                        ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, usermsg->msg_id);
                        ntymsg.dwAddData2   =  usermsg->msg_value;
                        ntymsg.dwReserved   =  0;
                        ntymsg.p_arg        =  0;
                        ret = GUI_SendNotifyMessage(&ntymsg);

                        if (ret == -1)
                        {
                            return EPDK_FAIL;
                        }
                    }

                    if (sset_data.sset_sta == 1)
                    {
                        return EPDK_FAIL;
                    }

                    if (user_para->childlst)
                    {
                        _sset_menu_sleep(sset_data.menu_cnt);
                        __wrn("//////////message2\n");
                        __wrn("psset->sid=%d\n", psset->sid);
                        {
                            __gui_msg_t         ntymsg;
                            ntymsg.id           =  GUI_MSG_COMMAND;
                            ntymsg.h_srcwin     =  msg->h_deswin;
                            ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                            ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_STA_NEXT);
                            ntymsg.dwAddData2   =  sset_data.menu_cnt + 1;
                            ntymsg.dwReserved   =  0;
                            ntymsg.p_arg        =  0;
                            ret = GUI_SendNotifyMessage(&ntymsg);

                            if (ret == -1)
                            {
                                return EPDK_FAIL;
                            }
                        }
                    }

                    if (sset_data.sset_sta == 1)
                    {
                        return EPDK_FAIL;
                    }

                    if (user_para->childlst)
                    {
                        /*子节点非空，进入第一级子菜单*/
                        __wrn("aaaaa test menulist longdown 9\n");
                        _sset_menu_next(0, MENU_TYPE_LSTM);
                    }
                }
            }
            break;

            default:
                break;
        }
    }

    return EPDK_OK;
}

static void key_msg_map(__gui_msg_t *msg)
{
    if (sset_data.menu_cnt > 0)
    {
        __wrn("aaaaa test menulist longdown 2\n");

        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LEFT:
            {
                msg->dwAddData1 = GUI_MSG_KEY_UP;
                break;
            }

            case GUI_MSG_KEY_LONGLEFT:
            {
                msg->dwAddData1 = GUI_MSG_KEY_LONGUP;
                break;
            }

            case  GUI_MSG_KEY_RIGHT:
            {
                msg->dwAddData1 = GUI_MSG_KEY_DOWN;
                break;
            }

            case  GUI_MSG_KEY_LONGRIGHT:
            {
                msg->dwAddData1 = GUI_MSG_KEY_LONGDOWN;
                break;
            }

            default:
                break;
        }
    }
}

/*
************************************************************************************************************************
*                                       _sset_manage_proc
*
*Description: SSET 消息处理函数(回凋)
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static int _sset_manage_proc(__gui_msg_t *msg)
{
    __wrn("msg_>id=%d\n", msg->id);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return EPDK_OK;

        case GUI_MSG_KEY:
        {
            if (GUI_LyrWinGetSta(sset_data.menu_lwin[sset_data.menu_cnt]) != GUI_LYRWIN_STA_ON)
            {
                /*当图层是隐藏状态下 就不接受KEY 消息*/
                return EPDK_OK;
            }

            if (sset_data.time_sta == 1)
            {
                __sset_create_para_t    *psset;
                psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
                GUI_ResetTimer(msg->h_deswin, sset_data.time_id, psset->time_speed, NULL);
            }

            __wrn("------ test menulist 1\n");
            key_msg_map(msg);
            __wrn("------ test menulist 2\n");
            _sset_keyproc(msg);
        }

        return EPDK_OK;

        case GUI_MSG_TOUCH:
        {
            if (sset_data.time_sta == 1)
            {
                __sset_create_para_t    *psset;
                psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
                GUI_ResetTimer(msg->h_deswin, sset_data.time_id, psset->time_speed, NULL);
            }

            _sset_touchproc(msg);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            __s32           ret;
            __wrn("------ test menulist 3\n");
            ret = _sset_command(msg);
            return ret;
        }

        case GUI_MSG_TIMER:
        {
            __sset_create_para_t    *psset;
            psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (sset_data.time_enable == 1)
            {
                __gui_msg_t         ntymsg;
                ntymsg.id           =  GUI_MSG_COMMAND;
                ntymsg.h_srcwin     =  msg->h_deswin;
                ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_TIMER);
                ntymsg.dwAddData2   =  sset_data.menu_cnt;
                ntymsg.dwReserved   =  0;
                ntymsg.p_arg        =  0;
                GUI_SendNotifyMessage(&ntymsg);
            }
        }

        return EPDK_OK;

        case SSET_IN_UPDATE_INFO:
        {
            __gui_msg_t     setmsg;
            setmsg.id           =  SSET_IN_INFO_UPDATE;
            setmsg.h_srcwin     =  msg->h_deswin;
            setmsg.h_deswin     =  sset_data.info_fwin;
            setmsg.dwAddData1   =  msg->dwAddData1;
            setmsg.dwAddData2   =  0;
            setmsg.p_arg        =  NULL;
            GUI_SendNotifyMessage(&setmsg);
        }

        return EPDK_OK;

        case SSET_STRING_UPDATE://..
        {
            __gui_msg_t     setmsg;
            __msg("+++++++SSET_STRING_UPDATE+++++++\n");
            setmsg.id           =  SSET_STRING_UPDATE;
            setmsg.h_srcwin     =  msg->h_deswin;
            setmsg.h_deswin     =  sset_data.menu_fwin[sset_data.menu_cnt];
            setmsg.dwAddData1   =  msg->dwAddData1;
            setmsg.dwAddData2   =  msg->dwAddData2;
            setmsg.dwReserved   =  msg->dwReserved;
            setmsg.p_arg        =  NULL;
            GUI_SendNotifyMessage(&setmsg);
        }

        return EPDK_OK;

        case SSET_ITEM_UPDATE://..
        {
            __gui_msg_t     setmsg;
            __msg("+++++++++SSET_ITEM_UPDATE++++++++\n");
            setmsg.id           =  SSET_ITEM_UPDATE;
            setmsg.h_srcwin     =  msg->h_deswin;
            setmsg.h_deswin     =  sset_data.menu_fwin[sset_data.menu_cnt];
            setmsg.dwAddData1   =  msg->dwAddData1;
            setmsg.dwAddData2   =  msg->dwAddData2;
            setmsg.dwReserved   =  msg->dwReserved;
            setmsg.p_arg        =  NULL;
            GUI_SendMessage(&setmsg);
        }

        return EPDK_OK;

        case SSET_APP2SSET_PREV:
        {
            __s32                       ret;
            __sset_create_para_t        *psset;
            psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);
            GUI_LyrWinSetSta(sset_data.menu_lwin[sset_data.menu_cnt], GUI_LYRWIN_STA_OFF);
            _sset_menu_destroy(sset_data.menu_cnt);

            if (sset_data.sset_sta == 1)
            {
                return EPDK_FAIL;
            }

            if (sset_data.menu_cnt != 0) /*非tbar 的情况下*/
            {
                _sset_menu_prev();
            }

            if (msg->h_srcwin != NULL)
            {
                __gui_msg_t         ntymsg;
                ntymsg.id           =  GUI_MSG_COMMAND;
                ntymsg.h_srcwin     =  msg->h_deswin;
                ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_STA_CHG);
                ntymsg.dwAddData2   =  sset_data.menu_cnt + 1;
                ntymsg.p_arg        =  0;
                ret = GUI_SendNotifyMessage(&ntymsg);

                if (ret == -1)
                {
                    return EPDK_FAIL;
                }
            }
        }

        return EPDK_OK;

        case SSET_APP2SSET_TO_TBAR: /*应用发过来让SSET 退出到tbar级*/
        {
            __s32                       ret;
            __sset_create_para_t        *psset;
            psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            while (sset_data.menu_cnt > 0)
            {
                GUI_LyrWinSetSta(sset_data.menu_lwin[sset_data.menu_cnt], GUI_LYRWIN_STA_OFF);
                _sset_menu_destroy(sset_data.menu_cnt);

                if (sset_data.sset_sta == 1)
                {
                    return EPDK_FAIL;
                }

                if (sset_data.menu_cnt != 0) /*非tbar 的情况下*/
                {
                    _sset_menu_prev();
                }
            }

            if (msg->h_srcwin != NULL)
            {
                __gui_msg_t         ntymsg;
                ntymsg.id           =  GUI_MSG_COMMAND;
                ntymsg.h_srcwin     =  msg->h_deswin;
                ntymsg.h_deswin     =  GUI_WinGetParent(msg->h_deswin);
                ntymsg.dwAddData1   = (__u32)MAKELONG(psset->sid, SSET_NC_STA_CHG);
                ntymsg.dwAddData2   =  sset_data.menu_cnt + 1;
                ntymsg.p_arg        =  0;
                ret = GUI_SendNotifyMessage(&ntymsg);

                if (ret == -1)
                {
                    return EPDK_FAIL;
                }
            }
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __s32                   i = 0;
            __sset_create_para_t    *psset;

            if (sset_data.menu_cnt >= 0)
            {
                for (i = MENU_CNT - 1; i >= 0; i--)
                {
                    _sset_menu_destroy(i);
                }
            }

            psset = (__sset_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (sset_data.time_sta == 1)
            {
                GUI_KillTimer(msg->h_deswin, sset_data.time_id);
            }

            SSET_ItemAttr_Destroy(sset_data.phead);

            if (psset)
            {
                esMEMS_Bfree(psset, sizeof(__sset_create_para_t));
                psset = NULL;
            }

            sset_data.sset_sta = 1;
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*                                       SSET_Create
*
*Description: SSET 场景创建函数
*
*Arguments  : para      场景创建参数指针
*
*Return     :
*
************************************************************************************************************************
*/
H_WIN SSET_Create(__sset_create_para_t  *para)
{
    H_WIN                       h_win;
    __sset_create_para_t        *psset;
    __gui_manwincreate_para_t   sset_create_info;
    eLIBs_memset(&sset_data, 0, sizeof(sset_scene_data_t));
    sset_data.menu_cnt = -1;
    sset_data.puipara  = SSET_Get_Uipara();

    if (para->time_speed != 0)
    {
        sset_data.time_sta = 1;
        sset_data.time_enable = 1;
        sset_data.time_id  = 0x5000;
    }

    psset = (__sset_create_para_t *)esMEMS_Balloc(sizeof(__sset_create_para_t));
    eLIBs_memset(psset, 0, sizeof(__sset_create_para_t));
    psset->name = para->name;
    psset->sid = para->sid;
    psset->pwin = para->pwin;
    psset->powner = para->powner;
    psset->attr = para->attr;
    psset->bfont = para->bfont;
    psset->sfont = para->sfont;
    psset->icon_res = para->icon_res;
    psset->lang_res = para->lang_res;
    psset->icon_style = para->icon_style;
    psset->lang_style = para->lang_style;
    psset->draw_code = para->draw_code;
    psset->time_speed = para->time_speed;
    sset_data.phead = SSET_ItemAttr_Create(psset->attr);
    sset_data.pmenu = sset_data.phead;
    eLIBs_memset(&sset_create_info, 0, sizeof(__gui_manwincreate_para_t));
    sset_create_info.name                   = psset->name;
    sset_create_info.hParent                = psset->pwin;
    sset_create_info.hHosting               = psset->powner;
    sset_create_info.ManWindowProc          = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_sset_manage_proc);
    sset_create_info.attr                   = psset;
    sset_create_info.id                     = para->sid;
    h_win = GUI_ManWinCreate(&sset_create_info);
    GUI_WinSetAddData(h_win, (unsigned long)sset_data.puipara);

    if (!h_win)
    {
        __wrn("create mainwin fail\n");
        return NULL;
    }

    sset_data.sset_mwin = h_win;
    GUI_WinSetFocusChild(h_win);

    /*create timer for hiding sset*/
    if (sset_data.time_sta == 1)
    {
        GUI_SetTimer(h_win, sset_data.time_id, psset->time_speed, NULL);
    }

    return h_win;
}

/*
************************************************************************************************************************
*                                       SSET_Destroy
*
*Description: SSET 场景销毁函数
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_Destroy(H_WIN h_win)
{
    if (h_win)
    {
        GUI_ManWinDelete(h_win);
        h_win = 0;
    }

    sset_data.sset_mwin = 0;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Search
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_EnnableNode(char *name)
{
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    SSET_ItemAttr_Ennable(p_item);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Search
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_DisableNode(char *name)
{
    __sset_item_t   *p_item = NULL;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    SSET_ItemAttr_Disable(p_item);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Search
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_SetAttr(char *name, __sset_item_attr_t *attr)
{
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    SSET_Item_SetAttr(p_item, attr);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Search
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_GetAttr(char *name, __sset_item_attr_t *attr)
{
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    SSET_Item_GetAttr(p_item, attr);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_Get_Nodelist
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_Get_Nodelist(char *name, __sset_nodelist_sta_e *sta)
{
    __u32            menu_cnt = 0;
    __sset_item_t   *p_tmp;
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    p_tmp = p_item->childlst;

    if (p_tmp == NULL)
    {
        *sta = NODELIST_DEL;
        return EPDK_OK;
    }

    while (p_tmp->father != sset_data.phead)
    {
        menu_cnt++;
        p_tmp = p_tmp->father;
    }

    if (sset_data.menu_lwin[menu_cnt] == NULL)
    {
        *sta = NODELIST_DEL;
        return EPDK_OK;
    }

    switch (GUI_LyrWinGetSta(sset_data.menu_lwin[menu_cnt]))
    {
        case GUI_LYRWIN_STA_ON:
        {
            *sta = NODELIST_ON;
        }
        break;

        case GUI_LYRWIN_STA_OFF:
        case GUI_LYRWIN_STA_SLEEP:
        case GUI_LYRWIN_STA_SUSPEND:
        {
            *sta = NODELIST_OFF;
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_Get_Nodelist
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_Set_Nodelist(char *name, __sset_nodelist_sta_e sta)
{
    __s32            menu_cnt = -1;
    __sset_item_t   *p_tmp;
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    if (p_item->childlst == NULL)
    {
        return EPDK_FAIL;
    }

    p_tmp = p_item;

    while (p_tmp != sset_data.phead)
    {
        menu_cnt = menu_cnt + 1;
        p_tmp = p_tmp->father;
    }

    sset_data.pmenu = p_item;
    sset_data.menu_cnt = menu_cnt;

    if (sta == NODELIST_ON)
    {
        if (sset_data.menu_cnt == -1)
        {
            _sset_menu_create(0, MENU_TYPE_TBAR);
            sset_data.time_enable = 1;
        }
        else
        {
            _sset_menu_create(0, MENU_TYPE_LSTM);
            sset_data.time_enable = 1;
        }
    }
    else if (sta == NODELIST_OFF)
    {
        _sset_menu_sleep(sset_data.menu_cnt + 1);
        sset_data.time_enable = 0;
    }
    else if (sta == NODELIST_DEL)
    {
        sset_data.menu_cnt = sset_data.menu_cnt + 1;
        _sset_menu_destroy(sset_data.menu_cnt);
        sset_data.time_enable = 0;
    }

    return EPDK_OK;
}

/*
*************************************************************************************
*                                       SSET_UserPara_Insert
*
*Description: 插入一组条目
*
*Arguments  : pitem 要插入条目的插入节点指针
*             para  插入条目的头指针
*             num   插入的条目数
*
*Return     : NULL
*
************************************************************************************
*/
__s32 SSET_Insert(char *name, __sset_item_para_t *para, __u32 num, __sset_item_insert_mode_e mode)
{
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    //__here__
    SSET_ItemAttr_Insert(p_item, para, num, mode);
    //__here__
    return EPDK_OK;
}
/*
************************************************************************************
*                                       SSET_UserPara_Delete
*
*Description: 删除一组条目
*
*Arguments  : pitem 删除条目所挂载的节点指针
*             mode  删除方式
*
*Return     : NULL
*
************************************************************************************
*/
__s32 SSET_Delete(char *name, __sset_item_delete_mode_e mode)
{
    __sset_item_t   *p_item;
    p_item = SSET_ItemAttr_Search(name, sset_data.phead);

    if (p_item == NULL)
    {
        return EPDK_FAIL;
    }

    SSET_ItemAttr_Delete(p_item, mode);
    return EPDK_OK;
}
