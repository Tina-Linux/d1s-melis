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

static  tbar_para_t     *pmenu_para = 0;
#define ID_PHOTO_SCENE_SSET                2
#define ID_FM_SCENE_SSET               10
#define EBOOK_SSET_SCENE_ID     0x8003
#define     MOVIE_SSET_ID                       0x80




/*
************************************************************************************************************************
*                                       SSET_MenuTbar_Create
*
*Description: 创建dsk_tbar
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
H_WIN SSET_MenuTbar_Create(H_WIN hparent, H_WIN hlayer, __sset_item_t *pitem)
{
    __u32                       i = 0;
    __u32                       tbar_width;
    H_WIN                       h_win;
    tbar_item_t                 *pmenu_item;
    __sset_item_t               *user_para;
    __sset_item_attr_t          *pitem_attr;
    __sset_uipara_t             *puipara;
    __sset_create_para_t        *psset;
    //__gui_manwincreate_para_t    manwinpara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(hparent);
    psset = (__sset_create_para_t *)GUI_WinGetAttr(hparent);
    pmenu_para = (tbar_para_t *)esMEMS_Balloc(sizeof(tbar_para_t));

    if (!pmenu_para)
    {
        __err("malloc error \n");
        return 0;
    }

    eLIBs_memset(pmenu_para, 0, sizeof(tbar_para_t));
    user_para = pitem;

    while (user_para)
    {
        i++;
        user_para = user_para->next;
    }

    pmenu_item = (tbar_item_t *)esMEMS_Balloc(sizeof(tbar_item_t) * i);

    if (!pmenu_item)
    {
        __err("malloc error \n");
        return 0;
    }

    eLIBs_memset(pmenu_item, 0, sizeof(tbar_item_t)*i);
    pmenu_para->item            = pmenu_item;
    pmenu_para->item_nr         = i;
    pmenu_para->frame_size.width    = puipara->menu_tbar_item.w;
    pmenu_para->frame_size.height   = puipara->menu_tbar_item.h;
    pmenu_para->focus_color     = GUI_YELLOW;///*GUI_RED*/APP_COLOR_YELLOW;
    pmenu_para->unfocus_color   = GUI_GRAY;//*GUI_YELLOW*/APP_COLOR_WHITE;
    pmenu_para->draw_font       = SWFFont;
    user_para = pitem;
    tbar_width = pmenu_para->item_nr * pmenu_para->frame_size.width;

    if (tbar_width < puipara->tbar_lyr.scnw)
    {
        pmenu_para->scn_width = tbar_width ;
    }
    else
    {
        pmenu_para->scn_width = puipara->tbar_lyr.scnw;
    }

    pmenu_para->src_start = puipara->menu_tbar_item.x;//..

    if (ID_FM_SCENE_SSET == psset->sid)
    {
        pmenu_para->bmp_pos.x           = puipara->menu_tbar_bmp.x;
        pmenu_para->bmp_pos.y           = puipara->menu_tbar_bmp.y;
        pmenu_para->text_pos.x          = puipara->menu_tbar_txt.x;
        pmenu_para->text_pos.y          = puipara->menu_tbar_txt.y;//..
        pmenu_para->h_bgbmp = dsk_theme_open(ID_FM_SSET_MENU_BG_BMP);
        pmenu_para->bg_bmp  = dsk_theme_hdl2buf(pmenu_para->h_bgbmp);
        pmenu_para->h_ssetid            = ID_FM_SCENE_SSET;
    }
    else if (ID_PHOTO_SCENE_SSET == psset->sid)
    {
        pmenu_para->bmp_pos.x           = puipara->menu_tbar_bmp.x;
        pmenu_para->bmp_pos.y           = puipara->menu_tbar_bmp.y;
        pmenu_para->text_pos.x          = puipara->menu_tbar_txt.x;
        pmenu_para->text_pos.y          = puipara->menu_tbar_txt.y;//..
        pmenu_para->h_bgbmp = dsk_theme_open(ID_PHOTO_MENU_BACK1_BMP);
        pmenu_para->bg_bmp  = dsk_theme_hdl2buf(pmenu_para->h_bgbmp);
        pmenu_para->h_ssetid            = ID_PHOTO_SCENE_SSET;
    }
    else if (EBOOK_SSET_SCENE_ID == psset->sid)
    {
        pmenu_para->bmp_pos.x           = puipara->menu_tbar_bmp.x;
        pmenu_para->bmp_pos.y           = puipara->menu_tbar_bmp.y;
        pmenu_para->text_pos.x          = puipara->menu_tbar_txt.x;
        pmenu_para->text_pos.y          = puipara->menu_tbar_txt.y;//..
        pmenu_para->h_bgbmp = dsk_theme_open(ID_EBOOK_MENU_BOTTOM_BMP);
        pmenu_para->bg_bmp  = dsk_theme_hdl2buf(pmenu_para->h_bgbmp);
        pmenu_para->h_ssetid            = EBOOK_SSET_SCENE_ID;
    }
    else if (MOVIE_SSET_ID == psset->sid)
    {
        pmenu_para->bmp_pos.x           = puipara->menu_tbar_bmp.x;
        pmenu_para->bmp_pos.y           = puipara->menu_tbar_bmp.y;
        pmenu_para->text_pos.x          = puipara->menu_tbar_txt.x;
        pmenu_para->text_pos.y          = puipara->menu_tbar_txt.y;//..
        pmenu_para->h_bgbmp = dsk_theme_open(ID_EBOOK_MENU_BOTTOM_BMP);
        pmenu_para->bg_bmp  = dsk_theme_hdl2buf(pmenu_para->h_bgbmp);
        pmenu_para->h_ssetid            = MOVIE_SSET_ID;
    }
    else
    {
        pmenu_para->bmp_pos.x           = puipara->menu_tbar_bmp.x;
        pmenu_para->bmp_pos.y           = puipara->menu_tbar_bmp.y;
        pmenu_para->text_pos.x          = puipara->menu_tbar_txt.x;
        pmenu_para->text_pos.y          = puipara->menu_tbar_txt.y;
        pmenu_para->h_bgbmp = dsk_theme_open(ID_EBOOK_MENU_BOTTOM_BMP);
        pmenu_para->bg_bmp  = dsk_theme_hdl2buf(pmenu_para->h_bgbmp);
        pmenu_para->h_ssetid            = 0;
    }

    for (i = 0; i < pmenu_para->item_nr; i++)
    {
        if (user_para->item_type != TYPE_TBAR)
        {
            return 0;
        }

        pitem_attr = &user_para->item_attr;

        if (user_para->item_sta == 1)
        {
            if (pitem_attr->icon[0])
            {
                //              (pmenu_item+i)->focus_bmp = (void *)esMEMS_Balloc(puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                //              GetRes(psset->icon_res,
                //                     psset->icon_style,
                //                     pitem_attr->icon[0],
                //                     (pmenu_item+i)->focus_bmp,
                //                     puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                Create_BMP_RES_Pt((pmenu_item + i)->focus_bmp, pitem_attr->icon[0]);
            }

            if (pitem_attr->icon[1])
            {
                //              (pmenu_item+i)->unfocus_bmp = (void *)esMEMS_Balloc(puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                //              GetRes(psset->icon_res,
                //                     psset->icon_style,
                //                     pitem_attr->icon[1],
                //                     (pmenu_item+i)->unfocus_bmp,
                //                     puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                Create_BMP_RES_Pt((pmenu_item + i)->unfocus_bmp, pitem_attr->icon[1]);
            }

            if (pitem_attr->string[0])
            {
                get_menu_text(pitem_attr->string[0], (pmenu_item + i)->text, sizeof((pmenu_item + i)->text));
            }
        }
        else
        {
            if (pitem_attr->icon[2])
            {
                //              (pmenu_item+i)->focus_bmp = (void *)esMEMS_Balloc(puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                //              GetRes(psset->icon_res,
                //                     psset->icon_style,
                //                     pitem_attr->icon[2],
                //                     (pmenu_item+i)->focus_bmp,
                //                     puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                Create_BMP_RES_Pt((pmenu_item + i)->focus_bmp, pitem_attr->icon[2]);
            }

            if (pitem_attr->icon[3])
            {
                //              (pmenu_item+i)->unfocus_bmp = (void *)esMEMS_Balloc(puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                //              GetRes(psset->icon_res,
                //                     psset->icon_style,
                //                     pitem_attr->icon[3],
                //                     (pmenu_item+i)->unfocus_bmp,
                //                     puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                Create_BMP_RES_Pt((pmenu_item + i)->unfocus_bmp, pitem_attr->icon[3]);
            }

            if (pitem_attr->string[1])
            {
                get_menu_text(pitem_attr->string[1], (pmenu_item + i)->text, sizeof((pmenu_item + i)->text));
            }
        }

        user_para = user_para->next;
    }

    h_win = tbar_frm_create(hparent, hlayer, pmenu_para);
    return h_win;
}
/*
************************************************************************************************************************
*                                       SSET_MenuTbar_DestroyItem
*
*Description:   SSET_MenuTbar_DestroyItem
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/

//..
__s32 SSET_MenuTbar_DestroyItem(H_WIN h_win, __u32 index, __u32 fcs_sta)
{
    //__u32                     i = 0;
    tbar_item_t                 *pmenu_item = 0;
    //__msg("+++++++++++SSET_MenuTbar_DestroyItem+++++++++\n");
    __msg("index = %d\n", index);
    __msg("fcs_sta = %d\n", fcs_sta);

    if (pmenu_para)
    {
        pmenu_item = pmenu_para->item;

        if (pmenu_item)
        {
            __msg("((pmenu_item+index-1)->focus_bmp) = %x\n", ((pmenu_item + index - 1)->focus_bmp));
            __msg("((pmenu_item+index-1)->unfocus_bmp) = %x\n", ((pmenu_item + index - 1)->unfocus_bmp));

            if (fcs_sta == SSET_FOCUSE_NORMAL)
            {
                if ((pmenu_item + index - 1)->unfocus_bmp)
                {
                    ////__here__;
                    Destroy_BMP_RES_Pt((pmenu_item + index - 1)->unfocus_bmp);
                    (pmenu_item + index - 1)->unfocus_bmp = NULL;
                    ////__here__;
                }
            }
            else if (fcs_sta == SSET_FOCUSE_PRESS)
            {
                ////__here__;
                if ((pmenu_item + index - 1)->focus_bmp)
                {
                    //  //__here__;
                    Destroy_BMP_RES_Pt((pmenu_item + index - 1)->focus_bmp);
                    (pmenu_item + index - 1)->focus_bmp = NULL;
                    //  //__here__;
                }
            }
            else
            {
                //__msg("++++++++++Create_BMP_RES failed++++++++++++++\n");
            }

            //esMEMS_Bfree( pmenu_item, sizeof(tbar_item_t)*pmenu_para->item_nr );
            //pmenu_item = NULL;
        }

        //esMEMS_Bfree( pmenu_para, sizeof(tbar_para_t) );
        //pmenu_para = NULL;
    }

    return EPDK_OK;
}


/*
************************************************************************************************************************
*                                       SSET_MenuTbar_Destroy
*
*Description: 销毁dsk_tbar
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuTbar_Destroy(H_WIN h_win)
{
    __u32                       i = 0;
    tbar_item_t                 *pmenu_item = 0;
    __sset_uipara_t             *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));

    if (pmenu_para)
    {
        if (pmenu_para->h_bgbmp)
        {
            dsk_theme_close(pmenu_para->h_bgbmp);
            pmenu_para->h_bgbmp = NULL;
        }

        if (pmenu_para->bg_bmp)
        {
            //dsk_theme_close (pmenu_para->bg_bmp);
            pmenu_para->bg_bmp = NULL;
        }

        pmenu_item = pmenu_para->item;

        if (pmenu_item)
        {
            for (i = 0; i < pmenu_para->item_nr; i++)
            {
                if ((pmenu_item + i)->focus_bmp)
                {
                    //                  esMEMS_Bfree((pmenu_item+i)->focus_bmp, puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                    Destroy_BMP_RES_Pt((pmenu_item + i)->focus_bmp);
                    (pmenu_item + i)->focus_bmp = NULL;
                }

                if ((pmenu_item + i)->unfocus_bmp)
                {
                    //                  esMEMS_Bfree((pmenu_item+i)->unfocus_bmp, puipara->menu_tbar_bmp.w*puipara->menu_tbar_bmp.h*4 + 0x100);
                    Destroy_BMP_RES_Pt((pmenu_item + i)->unfocus_bmp);
                    (pmenu_item + i)->unfocus_bmp = NULL;
                }
            }

            esMEMS_Bfree(pmenu_item, sizeof(tbar_item_t)*pmenu_para->item_nr);
            pmenu_item = NULL;
        }

        esMEMS_Bfree(pmenu_para, sizeof(tbar_para_t));
        pmenu_para = NULL;
    }

    tbar_frm_delete(h_win);
    h_win = NULL;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_MenuTbar_SetFocus
*
*Description: 设置MENU的焦点
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuTbar_SetFocus(H_WIN h_win, __u32 fcs_id)
{
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_MenuTbar_UpdateItem
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuTbar_UpdateItem(H_WIN h_win, __u32 item_no, __u32 index, __u32 fcs_sta)
{
    tbar_item_t                 *pmenu_item = 0;
    __sset_item_t   *pmenu;
    __msg("item_no = %d\n", item_no);

    if (pmenu_para)
    {
        pmenu_item = pmenu_para->item;
    }

    pmenu = (__sset_item_t *)_sset_menu_get_pmenu();

    if (pmenu->item_sta == 1)
    {
        ////__here__;
        SSET_MenuTbar_DestroyItem(h_win, index, fcs_sta);
    }

    //__msg("pmenu->item_attr.icon[0] = %d\n ", pmenu->item_attr.icon[0]);
    //__msg("pmenu->item_attr.icon[1] = %d\n ", pmenu->item_attr.icon[1]);
    //__msg("pmenu->item_attr.icon[2] = %d\n ", pmenu->item_attr.icon[2]);
    //__msg("pmenu->item_attr.icon[3] = %d\n ", pmenu->item_attr.icon[3]);
    //__msg("pmenu->item_attr.icon[4] = %d\n ", pmenu->item_attr.icon[4]);

    if (pmenu->item_sta == 1)
    {
        if (item_no)
        {
            ////__here__;
            if (fcs_sta == SSET_FOCUSE_PRESS)
            {
                //__here__;
                Create_BMP_RES_Pt((pmenu_item + index - 1)->focus_bmp, item_no);
            }
            else if (fcs_sta == SSET_FOCUSE_NORMAL)
            {
                //__here__;
                Create_BMP_RES_Pt((pmenu_item + index - 1)->unfocus_bmp, item_no);
            }
            else
            {
                __msg("++++++++++Create_BMP_RES failed++++++++++++++\n");
            }
        }

        __msg("((pmenu_item+item_no-1)->focus_bmp) = %x\n", ((pmenu_item + index - 1)->focus_bmp));
        __msg("((pmenu_item+item_no-1)->unfocus_bmp) = %x\n", ((pmenu_item + index - 1)->unfocus_bmp));
    }

    return EPDK_OK;
}
