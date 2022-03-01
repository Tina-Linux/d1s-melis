/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : sset_menu_lstm.c
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/
#include <log.h>
#include "sset_i.h"

//////////////////////////////////////////////////////////////////////////
#if 0 //(为了打印简单, 可把下面重定义的GUI宏临时拷到有需要的C文件中使用)
//re define GUI_BMP_Draw
#ifdef GUI_BMP_Draw
#define MY_GUI_BMP_DRAW                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw            )
#undef GUI_BMP_Draw
#define GUI_BMP_Draw(_p, _x, _y)             do \
    {\
        {__msg("    GUI_BMP_Draw(%d, %d)\n", _x, _y);} \
        MY_GUI_BMP_DRAW(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringAt
#ifdef GUI_DispStringAt
#define MY_GUI_DispStringAt                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt        )
#undef GUI_DispStringAt
#define GUI_DispStringAt(_p, _x, _y)         do\
    { \
        {__msg("    GUI_DispStringAt(%d, %d)->( %s )\n", _x, _y, _p);} \
        MY_GUI_DispStringAt(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringInRect
#ifdef GUI_DispStringInRect
#define MY_GUI_DispStringInRect              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect    )
#undef GUI_DispStringInRect
#define GUI_DispStringInRect(_p, _prt, _fmt)  do \
    { \
        {__msg("    GUI_DispStringInRect(%d, %d, %d, %d)->( %s )\n", (_prt)->x0, (_prt)->y0, (_prt)->x1, (_prt)->y1, _p);} \
        MY_GUI_DispStringInRect(_p, _prt, _fmt); \
    } while (0)
#endif

#endif

//////////////////////////////////////////////////////////////////////////

#define     ID_LISTITEM     1                   /* LISTITEM 条目上控件的ID 号       */

#define     COLOR_FOCUS     GUI_YELLOW//(APP_COLOR_YELLOW+0x0f)//0xB000A3FF         /* 焦点颜色                         */
#define     COLOR_INVALID   GUI_GREEN//(APP_COLOR_GREEN+0x0f)//0xB0A7A7A7           /* 无效颜色                         */
#define     COLOR_UFOCUS    GUI_GRAY//(APP_COLOR_GREEN+0x0f)//0xB0575757            /* 非焦点颜色                       */
#define     COLOR_RIM1      0x0//0xff575757         /* 边框颜色                         */
#define     COLOR_RIM2      0x0//0xffBDBDBD         /* 边框颜色                         */

/********** sset  menu_lstm  draw  ***********/
typedef struct taglstmitempara
{
    __u32                       item_id;
    void                        *user_item;
    //    void                      *focus_bmp;
    DECLARE_BMP_RES_Pt(focus_bmp);
    //    void                      *unfocus_bmp;
    DECLARE_BMP_RES_Pt(unfocus_bmp);
    //    void                      *opt_bmp;
    DECLARE_BMP_RES_Pt(opt_bmp);
    __pos_t                     bmp_pos;
    __pos_t                     optb_pos;
    char                        *text;
    __pos_t                     text_pos;
    __u32                       ftxt_color;
    __u32                       uftxt_color;
    GUI_FONT                   *draw_font;
    __epdk_charset_enm_e        draw_code;

} lstm_item_para_t;

typedef struct  taglistmenudata
{
    H_WIN                   h_lstm;
    __u32                   item_cnt;
    __sset_item_t           *user_opt;
    __u32                   touch_cnt;
} listmenu_data_t;

//////////////////////////////////////////////////////////////////////////
/***************************************************************************************************
*Name        : get_menu_text
*Prototype   : __s32  get_menu_text( __u16 text_id, void *buf, __u32 size )
*Arguments   : text_id        input. the ID of a menu text
*              buf            ouput. the buffer to store the menu text
*              size           input. the size of the buffer
*Return      : ==0     succeed
*              !=0     fail
*Description : get the content of a menu text according to its ID.
*Other       :
***************************************************************************************************/
// __s32  get_menu_text( __u16 text_id, void *buf, __u32 size )
// {
//  return Lang_GetString( hlang, lang_ID, text_id, buf, size );
// }
#ifndef get_menu_text
#define get_menu_text dsk_langres_get_menu_text
#endif
//////////////////////////////////////////////////////////////////////////

/*
************************************************************************************************************************
*                                       _draw_back
*
*Description: listmenu 的绘制函数
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _draw_back(__u32 width, __u32 height)
{
    __u32               i;
    RECT                item_rect;
    item_rect.x = 0;
    item_rect.y = 0;
    item_rect.width  = width - 1;
    item_rect.height = height - 1;
    __wrn("width=%d, height=%d\n", width, height);

    for (i = 0; i < APP_MAX_LIST_ITEM_COUNT; i++)
    {
        item_rect.y = i * height;
        GUI_SetBkColor(COLOR_UFOCUS);
        GUI_ClearRect(item_rect.x, item_rect.y,
                      item_rect.x + item_rect.width, item_rect.y + item_rect.height - 2);
        GUI_SetBkColor(COLOR_RIM1);
        GUI_ClearRect(item_rect.x, item_rect.y + item_rect.height - 2,
                      item_rect.x + item_rect.width, item_rect.y + item_rect.height - 1);
        GUI_SetBkColor(COLOR_RIM2);
        GUI_ClearRect(item_rect.x, item_rect.y + item_rect.height - 1,
                      item_rect.x + item_rect.width, item_rect.y + item_rect.height);
        //esKRNL_TimeDly(200);
    }

    return 0;
}

/*
************************************************************************************************************************
*                                       _draw_lstm
*
*Description: listmenu 的绘制函数
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _draw_lstm(__draw_para_t *item_para)
{
    RECT                item_rect;
    H_LYR               hlyrwin;
    H_WIN               lstmwin;
    GUI_MEMDEV_Handle   draw_mem;
    __sset_item_t       *user_para;
    __sset_item_attr_t  *pitem_attr;
    lstm_item_para_t    *iteminfo;

    if (!item_para->item)
    {
        __wrn("draw item error!\n");
        return EPDK_FAIL;
    }

    iteminfo = (lstm_item_para_t *)GUI_LstmGetItemAttr(item_para->item);
    user_para = (__sset_item_t *)iteminfo->user_item;
    pitem_attr = &user_para->item_attr;
    GUI_LstmGetItemRect(item_para->item, &item_rect);
    hlyrwin = GUI_LstmGetItemLyrWin(item_para->item);
    __wrn("=========item_para->item=%x\n", item_para->item);
    lstmwin = GUI_LstmGetHostWin(item_para->item);
    GUI_LyrWinSel(hlyrwin);
    draw_mem = GUI_MEMDEV_Create(item_rect.x, item_rect.y, item_rect.width, item_rect.height);
    GUI_MEMDEV_Select(draw_mem);
    __wrn("_draw_lstm:item_para->mode=%d\n", item_para->mode);

    switch (item_para->mode)
    {
        case GUI_PAIN_MODE_FOCUS:           /*条目由移动变停止，接到的绘制消息  */
        {
            __gui_msg_t         setmsg;
            setmsg.id           =  SSET_IN_UPDATE_INFO;
            setmsg.h_srcwin     =  lstmwin;
            setmsg.h_deswin     =  GUI_WinGetManWin(lstmwin);
            setmsg.dwAddData1   =  pitem_attr->icon[7];
            setmsg.dwAddData2   =  0;
            setmsg.p_arg        =  NULL;
            GUI_SendNotifyMessage(&setmsg);
        }
        break;

        case GUI_PAIN_MODE_FASTFOCUS:       /*条目移动时发送，未停止接到的绘制消息*/
        {
            GUI_SetBkColor(COLOR_FOCUS);
            GUI_ClearRect(item_rect.x, item_rect.y,
                          item_rect.x + item_rect.width,
                          item_rect.y + item_rect.height - 2);
            GUI_SetBkColor(COLOR_RIM1);
            GUI_ClearRect(item_rect.x, item_rect.y + item_rect.height - 2,
                          item_rect.x + item_rect.width,
                          item_rect.y + item_rect.height - 1);
            GUI_SetBkColor(COLOR_RIM2);
            GUI_ClearRect(item_rect.x, item_rect.y + item_rect.height - 1,
                          item_rect.x + item_rect.width,
                          item_rect.y + item_rect.height);

            if (iteminfo->focus_bmp)
            {
                GUI_BMP_Draw(iteminfo->focus_bmp, item_rect.x + iteminfo->bmp_pos.x,
                             item_rect.y + iteminfo->bmp_pos.y);
            }

            GUI_CharSetToEncode(iteminfo->draw_code);
            GUI_SetFont(iteminfo->draw_font);
            GUI_OpenAlphaBlend();
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

            if (user_para->item_sta)
            {
                GUI_SetColor(iteminfo->ftxt_color);
            }
            else
            {
                GUI_SetColor(GUI_WHITE);
            }

            if (iteminfo->text)
            {
                GUI_DispStringAt(iteminfo->text, item_rect.x + iteminfo->text_pos.x, item_rect.y + iteminfo->text_pos.y);
            }

            if (pitem_attr->text)
            {
                if (iteminfo->text)
                {
                    GUI_DispStringAt(pitem_attr->text, item_rect.x + iteminfo->text_pos.x + 100, item_rect.y + iteminfo->text_pos.y);
                }
                else
                {
                    GUI_DispStringAt(pitem_attr->text, item_rect.x + iteminfo->text_pos.x, item_rect.y + iteminfo->text_pos.y);
                }
            }

            GUI_LyrWinFlushFB(hlyrwin);
            GUI_CloseAlphaBlend();

            if (pitem_attr->opt == 1)
            {
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_BMP_Draw(iteminfo->opt_bmp, item_rect.x + iteminfo->optb_pos.x,
                             item_rect.y + iteminfo->optb_pos.y);
            }
        }
        break;

        case GUI_PAIN_MODE_FASTDRAW:        /*条目创建时发送，最先接到的绘制消息*/
        case GUI_PAIN_MODE_UNFOCUS:         /*条目由移动变停止，接到的绘制消息  */
        case GUI_PAIN_MODE_FASTUNFOCUS:     /*条目移动时发送，未停止接到的绘制消息*/
        {
            if (item_para->item == GUI_LstmGetCurItem(lstmwin))
            {
                __wrn("=========item_para->item=%x\n", item_para->item);
                GUI_SetBkColor(COLOR_FOCUS);
                GUI_ClearRect(item_rect.x, item_rect.y,
                              item_rect.x + item_rect.width,
                              item_rect.y + item_rect.height - 2);
            }
            else if (user_para->item_sta)
            {
                GUI_SetBkColor(COLOR_UFOCUS);
                GUI_ClearRect(item_rect.x, item_rect.y,
                              item_rect.x + item_rect.width,
                              item_rect.y + item_rect.height - 2);
            }
            else
            {
                GUI_SetBkColor(COLOR_INVALID);
                GUI_ClearRect(item_rect.x, item_rect.y,
                              item_rect.x + item_rect.width,
                              item_rect.y + item_rect.height - 2);
            }

            GUI_SetBkColor(COLOR_RIM1);
            GUI_ClearRect(item_rect.x, item_rect.y + item_rect.height - 2,
                          item_rect.x + item_rect.width,
                          item_rect.y + item_rect.height - 1);
            GUI_SetBkColor(COLOR_RIM2);
            GUI_ClearRect(item_rect.x, item_rect.y + item_rect.height - 1,
                          item_rect.x + item_rect.width,
                          item_rect.y + item_rect.height);

            if (iteminfo->unfocus_bmp)
            {
                GUI_BMP_Draw(iteminfo->unfocus_bmp, item_rect.x + iteminfo->bmp_pos.x,
                             item_rect.y + iteminfo->bmp_pos.y);
            }

            if (item_para->item == GUI_LstmGetCurItem(lstmwin)) //..
            {
                if (iteminfo->focus_bmp)
                {
                    GUI_BMP_Draw(iteminfo->focus_bmp, item_rect.x + iteminfo->bmp_pos.x,
                                 item_rect.y + iteminfo->bmp_pos.y);
                }
            }

            GUI_CharSetToEncode(iteminfo->draw_code);
            GUI_SetFont(iteminfo->draw_font);
            GUI_OpenAlphaBlend();
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

            if (user_para->item_sta)
            {
                GUI_SetColor(iteminfo->uftxt_color);
            }
            else
            {
                GUI_SetColor(GUI_WHITE);
            }

            if (iteminfo->text)
            {
                GUI_DispStringAt(iteminfo->text, item_rect.x + iteminfo->text_pos.x, item_rect.y + iteminfo->text_pos.y);
            }

            if (pitem_attr->text)
            {
                if (iteminfo->text)
                {
                    GUI_DispStringAt(pitem_attr->text, item_rect.x + iteminfo->text_pos.x + 100, item_rect.y + iteminfo->text_pos.y);
                }
                else
                {
                    GUI_DispStringAt(pitem_attr->text, item_rect.x + iteminfo->text_pos.x, item_rect.y + iteminfo->text_pos.y);
                }
            }

            GUI_LyrWinFlushFB(hlyrwin);
            GUI_CloseAlphaBlend();

            if (pitem_attr->opt == 1)
            {
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_BMP_Draw(iteminfo->opt_bmp, item_rect.x + iteminfo->optb_pos.x,
                             item_rect.y + iteminfo->optb_pos.y);
            }
        }
        break;

        default:
            break;
    }

    if (draw_mem)
    {
        GUI_MEMDEV_CopyToLCD(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       _spin_create
*
*Description: 条目中控件SPINBOX的创建
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _spin_create(__lstm_item_ctrl_t *pctrl, __sset_item_t  *item_para, H_WIN h_win, __u32  cnt)
{
    __lstm_item_ctrl_t              *plstm_ctrl;
    __sset_item_attr_t              *pitem_attr;
    spinbox_para_t                  *pspin;
    __sset_create_para_t            *psset;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    psset = (__sset_create_para_t *)GUI_WinGetAttr(GUI_WinGetParent(h_win));
    plstm_ctrl      = pctrl;
    pitem_attr = &item_para->item_attr;
    {
        pspin = (spinbox_para_t *)esMEMS_Balloc(sizeof(spinbox_para_t));
        eLIBs_memset(pspin, 0, sizeof(spinbox_para_t));

        if (pitem_attr->icon[3])
        {
            //          pspin->fdn_bmp = (void *)esMEMS_Balloc(puipara->menu_spin_dec_bmp.w*puipara->menu_spin_dec_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[3],
            //                 pspin->fdn_bmp,
            //                 puipara->menu_spin_dec_bmp.w*puipara->menu_spin_dec_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pspin->fdn_bmp, pitem_attr->icon[3]);
        }

        if (pitem_attr->icon[4])
        {
            //          pspin->fup_bmp = (void *)esMEMS_Balloc(puipara->menu_spin_inc_bmp.w*puipara->menu_spin_inc_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[4],
            //                 pspin->fup_bmp,
            //                 puipara->menu_spin_inc_bmp.w*puipara->menu_spin_inc_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pspin->fup_bmp, pitem_attr->icon[4]);
        }

        if (pitem_attr->icon[5])
        {
            //          pspin->ufdn_bmp = (void *)esMEMS_Balloc(puipara->menu_spin_dec_bmp.w*puipara->menu_spin_dec_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[5],
            //                 pspin->ufdn_bmp,
            //                 puipara->menu_spin_dec_bmp.w*puipara->menu_spin_dec_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pspin->ufdn_bmp, pitem_attr->icon[5]);
        }

        if (pitem_attr->icon[6])
        {
            //          pspin->ufup_bmp = (void *)esMEMS_Balloc(puipara->menu_spin_inc_bmp.w*puipara->menu_spin_inc_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[6],
            //                 pspin->ufup_bmp,
            //                 puipara->menu_spin_inc_bmp.w*puipara->menu_spin_inc_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pspin->ufup_bmp, pitem_attr->icon[6]);
        }

        pspin->cur_value = pitem_attr->cur_value,
               pspin->max_value = pitem_attr->max_value,
                      pspin->min_value = pitem_attr->min_value,
                             pspin->upbmp_pos.x = puipara->menu_spin_inc_bmp.x;
        pspin->upbmp_pos.y = puipara->menu_spin_inc_bmp.y;
        pspin->dnbmp_pos.x = puipara->menu_spin_dec_bmp.x;
        pspin->dnbmp_pos.y = puipara->menu_spin_dec_bmp.y;
        pspin->text_pos.x  = puipara->menu_spin_txt.x;
        pspin->text_pos.y  = puipara->menu_spin_txt.y;
        pspin->ftxt_color  = GUI_WHITE;///*GUI_RED*/APP_COLOR_WHITE;
        pspin->uftxt_color = GUI_YELLOW;///*GUI_YELLOW*/APP_COLOR_YELLOW;
        pspin->draw_font   = psset->sfont;
        pspin->draw_code   = psset->draw_code;
        pspin->bk_color    = COLOR_UFOCUS;
    }
    plstm_ctrl->name            = NULL;
    plstm_ctrl->spCaption       = NULL;
    plstm_ctrl->spClassName     = CTRL_SPINBOX;
    plstm_ctrl->id              = cnt;
    plstm_ctrl->dwExStyle       = WS_EX_NONE;
    plstm_ctrl->dwStyle         = WS_NONE;
    plstm_ctrl->ctl_rect.x      = puipara->spin_ctl.x;
    plstm_ctrl->ctl_rect.y      = puipara->spin_ctl.y;
    plstm_ctrl->ctl_rect.width  = puipara->spin_ctl.w;
    plstm_ctrl->ctl_rect.height = puipara->spin_ctl.h;
    plstm_ctrl->attr            = pspin;
}

/*
************************************************************************************************************************
*                                       _spin_destroy
*
*Description: 条目中控件SPINBOX的资源释放
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _spin_destroy(__lstm_item_ctrl_t *pctrl, H_WIN h_win)
{
    spinbox_para_t                  *pspin;
    __lstm_item_ctrl_t              *plstm_ctrl;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    plstm_ctrl = pctrl;

    if (plstm_ctrl->attr)
    {
        pspin = (spinbox_para_t *)plstm_ctrl->attr;

        if (pspin->fup_bmp)
        {
            //          esMEMS_Bfree(pspin->fup_bmp, puipara->menu_spin_inc_bmp.w*puipara->menu_spin_inc_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pspin->fup_bmp);
            pspin->fup_bmp = NULL;
        }

        if (pspin->fdn_bmp)
        {
            //          esMEMS_Bfree(pspin->fdn_bmp, puipara->menu_spin_dec_bmp.w*puipara->menu_spin_dec_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pspin->fdn_bmp);
            pspin->fdn_bmp = NULL;
        }

        if (pspin->ufup_bmp)
        {
            //          esMEMS_Bfree(pspin->ufup_bmp, puipara->menu_spin_inc_bmp.w*puipara->menu_spin_inc_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pspin->ufup_bmp);
            pspin->ufup_bmp = NULL;
        }

        if (pspin->ufdn_bmp)
        {
            //          esMEMS_Bfree(pspin->ufdn_bmp, puipara->menu_spin_dec_bmp.w*puipara->menu_spin_dec_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pspin->ufdn_bmp);
            pspin->ufdn_bmp = NULL;
        }

        esMEMS_Bfree(pspin, sizeof(spinbox_para_t));
        pspin = NULL;
    }
}

/*
************************************************************************************************************************
*                                       _slider_create
*
*Description: 条目中控件SLIDER的创建
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _slider_create(__lstm_item_ctrl_t *pctrl, __sset_item_t *item_para, H_WIN h_win, __u32  cnt)
{
    __lstm_item_ctrl_t              *plstm_ctrl;
    __sset_item_attr_t              *pitem_attr;
    slider_para_t                   *pslider;
    __sset_create_para_t            *psset;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    psset = (__sset_create_para_t *)GUI_WinGetAttr(GUI_WinGetParent(h_win));
    plstm_ctrl                      = pctrl;
    pitem_attr = &item_para->item_attr;
    {
        pslider = (slider_para_t *)esMEMS_Balloc(sizeof(slider_para_t));
        eLIBs_memset(pslider, 0, sizeof(slider_para_t));

        if (pitem_attr->icon[2])
        {
            //          pslider->head_bmp_buf = (void *)esMEMS_Balloc(puipara->menu_slid_head_bmp.w*puipara->menu_slid_head_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[2],
            //                 pslider->head_bmp_buf,
            //                 puipara->menu_slid_head_bmp.w*puipara->menu_slid_head_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pslider->head_bmp_buf, pitem_attr->icon[2]);
        }

        if (pitem_attr->icon[3])
        {
            //          pslider->tail_bmp_buf = (void *)esMEMS_Balloc(puipara->menu_slid_tail_bmp.w*puipara->menu_slid_tail_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[3],
            //                 pslider->tail_bmp_buf,
            //                 puipara->menu_slid_tail_bmp.w*puipara->menu_slid_tail_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pslider->tail_bmp_buf, pitem_attr->icon[3]);
        }

        if (pitem_attr->icon[4])
        {
            //          pslider->cursor_bmp_buf = (void *)esMEMS_Balloc(puipara->menu_slid_curr_bmp.w*puipara->menu_slid_curr_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[4],
            //                 pslider->cursor_bmp_buf,
            //                 puipara->menu_slid_curr_bmp.w*puipara->menu_slid_curr_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pslider->cursor_bmp_buf, pitem_attr->icon[4]);
        }
    }
    plstm_ctrl->name            = NULL;
    plstm_ctrl->spCaption       = NULL;
    plstm_ctrl->spClassName     = CTRL_SLIDER;
    plstm_ctrl->id              = cnt;
    plstm_ctrl->dwExStyle       = WS_EX_NONE;
    plstm_ctrl->dwStyle         = WS_NONE | SDS_RESPOND_OUTDEV;
    plstm_ctrl->ctl_rect.x      = puipara->slid_ctl.x;
    plstm_ctrl->ctl_rect.y      = puipara->slid_ctl.y;
    plstm_ctrl->ctl_rect.width  = puipara->slid_ctl.w;
    plstm_ctrl->ctl_rect.height = puipara->slid_ctl.h;
    plstm_ctrl->attr            = pslider;
}

/*
************************************************************************************************************************
*                                       _slider_destroy
*
*Description: 条目中控件SLIDER的资源释放
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _slider_destroy(__lstm_item_ctrl_t *pctrl, H_WIN h_win)
{
    slider_para_t                   *pslider;
    __lstm_item_ctrl_t              *plstm_ctrl;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    plstm_ctrl = pctrl;

    if (plstm_ctrl->attr)
    {
        pslider = (slider_para_t *)plstm_ctrl->attr;

        if (pslider->head_bmp_buf)
        {
            //          esMEMS_Bfree(pslider->head_bmp_buf, puipara->menu_slid_head_bmp.w*puipara->menu_slid_head_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pslider->head_bmp_buf);
            pslider->head_bmp_buf = NULL;
        }

        if (pslider->tail_bmp_buf)
        {
            //          esMEMS_Bfree(pslider->tail_bmp_buf, puipara->menu_slid_tail_bmp.w*puipara->menu_slid_tail_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pslider->tail_bmp_buf);
            pslider->tail_bmp_buf = NULL;
        }

        if (pslider->cursor_bmp_buf)
        {
            //          esMEMS_Bfree(pslider->cursor_bmp_buf, puipara->menu_slid_curr_bmp.w*puipara->menu_slid_curr_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pslider->cursor_bmp_buf);
            pslider->cursor_bmp_buf = NULL;
        }

        esMEMS_Bfree(pslider, sizeof(slider_para_t));
        pslider = NULL;
    }
}

/*
************************************************************************************************************************
*                                       _bttn_create
*
*Description: 条目中控件BUTTON的创建
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _bttn_create(__lstm_item_ctrl_t *pctrl, __sset_item_t  *item_para, H_WIN h_win, __u32  cnt)
{
    __lstm_item_ctrl_t              *plstm_ctrl;
    __sset_item_attr_t              *pitem_attr;
    button_para_t                   *pbttn;
    __sset_create_para_t            *psset;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    psset = (__sset_create_para_t *)GUI_WinGetAttr(GUI_WinGetParent(h_win));
    plstm_ctrl      = pctrl;
    pitem_attr = &item_para->item_attr;
    {
        pbttn = (button_para_t *)esMEMS_Balloc(sizeof(button_para_t));
        eLIBs_memset(pbttn, 0, sizeof(button_para_t));

        if (pitem_attr->icon[2])
        {
            //          pbttn->focus_bmp = (void *)esMEMS_Balloc(puipara->menu_stch_bmp.w*puipara->menu_stch_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[2],
            //                 pbttn->focus_bmp,
            //                 puipara->menu_stch_bmp.w*puipara->menu_stch_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pbttn->focus_bmp, pitem_attr->icon[2]);
        }

        if (pitem_attr->icon[3])
        {
            //          pbttn->unfocus_bmp = (void *)esMEMS_Balloc(puipara->menu_stch_bmp.w*puipara->menu_stch_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[3],
            //                 pbttn->unfocus_bmp,
            //                 puipara->menu_stch_bmp.w*puipara->menu_stch_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(pbttn->unfocus_bmp, pitem_attr->icon[3]);
        }

        if (pitem_attr->string[0])
        {
            pbttn->text = (char *)esMEMS_Balloc(0x30);
            get_menu_text(pitem_attr->string[0],
                          pbttn->text,
                          0x30);
        }

        pbttn->bmp_pos.x = 0;
        pbttn->bmp_pos.y = 0;
        pbttn->text_pos.x  = 0;
        pbttn->text_pos.y  = 0;
        pbttn->ftxt_color  = /*GUI_RED*/APP_COLOR_WHITE;
        pbttn->uftxt_color = /*GUI_YELLOW*/APP_COLOR_YELLOW;
        pbttn->draw_font   = psset->sfont;
        pbttn->draw_code   = psset->draw_code;
    }
    plstm_ctrl->name            = NULL;
    plstm_ctrl->spCaption       = NULL;
    plstm_ctrl->spClassName     = CTRL_BUTTON;
    plstm_ctrl->id              = cnt;
    plstm_ctrl->dwExStyle       = WS_EX_NONE;
    plstm_ctrl->dwStyle         = WS_NONE;
    plstm_ctrl->ctl_rect.x      = puipara->stch_ctl.x;
    plstm_ctrl->ctl_rect.y      = puipara->stch_ctl.y;
    plstm_ctrl->ctl_rect.width  = puipara->stch_ctl.w;
    plstm_ctrl->ctl_rect.height = puipara->stch_ctl.h;
    plstm_ctrl->attr            = pbttn;
}

/*
************************************************************************************************************************
*                                       _bttn_destroy
*
*Description: 条目中控件BUTTON的资源释放
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void _bttn_destroy(__lstm_item_ctrl_t *pctrl, H_WIN h_win)
{
    button_para_t                   *pbttn;
    __lstm_item_ctrl_t              *plstm_ctrl;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    plstm_ctrl = pctrl;

    if (plstm_ctrl->attr)
    {
        pbttn = (button_para_t *)plstm_ctrl->attr;

        if (pbttn->focus_bmp)
        {
            //          esMEMS_Bfree(pbttn->focus_bmp, puipara->menu_stch_bmp.w*puipara->menu_stch_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pbttn->focus_bmp);
            pbttn->focus_bmp = NULL;
        }

        if (pbttn->unfocus_bmp)
        {
            //          esMEMS_Bfree(pbttn->unfocus_bmp, puipara->menu_stch_bmp.w*puipara->menu_stch_bmp.h*4 + 0x100);
            Destroy_BMP_RES_Pt(pbttn->unfocus_bmp);
            pbttn->unfocus_bmp = NULL;
        }

        if (pbttn->text)
        {
            esMEMS_Bfree(pbttn->text, 0x30);
            pbttn->text = NULL;
        }

        esMEMS_Bfree(pbttn, sizeof(button_para_t));
        pbttn = NULL;
    }
}


/*
************************************************************************************************************************
*                                       _menu_lstm_create
*
*Description: _menu_lstm_create
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32 _menu_lstm_create(__gui_msg_t *msg)
{
    __u32                           opt = 1;
    __u32                           i = 0;
    H_WIN                           h_win;
    listmenu_data_t                 *plist_data;
    __lstm_para_t                   *plstm_para;
    __lstm_item_attr_t              *plstm_item;
    lstm_item_para_t                *plitem_para;
    __sset_uipara_t                 *puipara;
    __sset_create_para_t            *psset;
    __sset_item_t                   *user_para;
    __sset_item_attr_t              *pitem_attr;
    __lstm_item_ctrl_t              *plstm_ctrl;
    __gui_ctlwincreate_para_t       ctlcreate_info;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(msg->h_deswin));
    psset = (__sset_create_para_t *)GUI_WinGetAttr(GUI_WinGetParent(msg->h_deswin));
    plist_data  = (listmenu_data_t *)esMEMS_Balloc(sizeof(listmenu_data_t));

    if (!plist_data)
    {
        __err("malloc error \n");
        return EPDK_FAIL;
    }

    eLIBs_memset(plist_data, 0, sizeof(listmenu_data_t));
    user_para = (__sset_item_t *)GUI_WinGetAttr(msg->h_deswin);

    while (user_para)
    {
        i++;
        user_para = user_para->next;
    }

    plstm_para = (__lstm_para_t *)esMEMS_Balloc(sizeof(__lstm_para_t));
    eLIBs_memset(plstm_para, 0, sizeof(__lstm_para_t));
    plstm_para->nitem       = i;
    //plstm_para->bk_color    = APP_COLOR_GREEN+0x0f;
    plstm_para->bk_color    = GUI_GRAY;//GUI_GRAY;
    plist_data->item_cnt    = i;
    plstm_item = (__lstm_item_attr_t *)esMEMS_Balloc(sizeof(__lstm_item_attr_t) * i);
    eLIBs_memset(plstm_item, 0, sizeof(__lstm_item_attr_t)*i);
    user_para = (__sset_item_t *)GUI_WinGetAttr(msg->h_deswin);

    for (i = 0; i < plstm_para->nitem; i++)
    {
        pitem_attr = &user_para->item_attr;
        plitem_para = (lstm_item_para_t *)esMEMS_Balloc(sizeof(lstm_item_para_t));
        eLIBs_memset(plitem_para, 0, sizeof(lstm_item_para_t));

        if (pitem_attr->icon[0])
        {
            //          plitem_para->focus_bmp = (void *)esMEMS_Balloc(puipara->menu_lstm_bmp.w*puipara->menu_lstm_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[0],
            //                 plitem_para->focus_bmp,
            //                 puipara->menu_lstm_bmp.w*puipara->menu_lstm_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(plitem_para->focus_bmp, pitem_attr->icon[0]);
        }

        if (pitem_attr->icon[1])
        {
            //          plitem_para->unfocus_bmp = (void *)esMEMS_Balloc(puipara->menu_lstm_bmp.w*puipara->menu_lstm_bmp.h*4 + 0x100);
            //          GetRes(psset->icon_res,
            //                 psset->icon_style,
            //                 pitem_attr->icon[1],
            //                 plitem_para->unfocus_bmp,
            //                 puipara->menu_lstm_bmp.w*puipara->menu_lstm_bmp.h*4 + 0x100);
            Create_BMP_RES_Pt(plitem_para->unfocus_bmp, pitem_attr->icon[1]);
        }

        if (pitem_attr->string[0])
        {
            plitem_para->text = (char *)esMEMS_Balloc(0x30);
            get_menu_text(pitem_attr->string[0],
                          plitem_para->text,
                          0x30);
        }

        __wrn("=====plitem_para->item_id=%d\n", plitem_para->item_id);
        plitem_para->item_id        = i;
        plitem_para->user_item      = (void *)user_para;
        plitem_para->bmp_pos.x      = puipara->menu_lstm_bmp.x;
        plitem_para->bmp_pos.y      = puipara->menu_lstm_bmp.y;
        plitem_para->text_pos.x     = puipara->menu_lstm_txt.x - 7; //..
        plitem_para->text_pos.y     = puipara->menu_lstm_txt.y;//..
        plitem_para->ftxt_color     = GUI_BLACK;///*GUI_RED*/APP_COLOR_WHITE;
        plitem_para->uftxt_color    = GUI_BLACK;///*GUI_WHITE*/APP_COLOR_WHITE;
        plitem_para->draw_font      = psset->sfont;
        plitem_para->draw_code      = psset->draw_code;
        (plstm_item + i)->item_w_unfocs   = puipara->menu_lstm_fcs.w;
        (plstm_item + i)->item_w_focs     = puipara->menu_lstm_fcs.w;

        if (user_para->item_type == TYPE_OPT)
        {
            if (pitem_attr->opt)
            {
                plist_data->user_opt = user_para;
                opt = i;
            }

            if (pitem_attr->icon[2])
            {
                //              plitem_para->opt_bmp = (void *)esMEMS_Balloc(puipara->menu_opt_bmp.w*puipara->menu_opt_bmp.h*4 + 0x100);
                //              GetRes(psset->icon_res,
                //                     psset->icon_style,
                //                     pitem_attr->icon[2],
                //                     plitem_para->opt_bmp,
                //                     puipara->menu_opt_bmp.w*puipara->menu_opt_bmp.h*4 + 0x100);
                //                Create_BMP_RES_Pt(plitem_para->opt_bmp, pitem_attr->icon[2]);
            }

            plitem_para->optb_pos.x         = puipara->menu_opt_bmp.x;
            plitem_para->optb_pos.y         = puipara->menu_opt_bmp.y;
            (plstm_item + i)->nctrl           = 0;
            (plstm_item + i)->ctrls           = NULL;
            (plstm_item + i)->cb              = _draw_lstm;
        }
        else if (user_para->item_type == TYPE_SLIDE)
        {
            (plstm_item + i)->nctrl           = 1;
            plstm_ctrl = (__lstm_item_ctrl_t *)esMEMS_Balloc(sizeof(__lstm_item_ctrl_t) * (plstm_item + i)->nctrl);
            eLIBs_memset(plstm_ctrl, 0, sizeof(__lstm_item_ctrl_t) * (plstm_item + i)->nctrl);
            _slider_create(plstm_ctrl, user_para, msg->h_deswin, ID_LISTITEM + i + 1);
            (plstm_item + i)->ctrls           = plstm_ctrl;
            (plstm_item + i)->cb              = _draw_lstm;
        }
        else if (user_para->item_type == TYPE_SPIN)
        {
            (plstm_item + i)->nctrl           = 1;
            plstm_ctrl = (__lstm_item_ctrl_t *)esMEMS_Balloc(sizeof(__lstm_item_ctrl_t) * (plstm_item + i)->nctrl);
            eLIBs_memset(plstm_ctrl, 0, sizeof(__lstm_item_ctrl_t) * (plstm_item + i)->nctrl);
            _spin_create(plstm_ctrl, user_para, msg->h_deswin, ID_LISTITEM + i + 1);
            (plstm_item + i)->ctrls           = plstm_ctrl;
            (plstm_item + i)->cb              = _draw_lstm;
        }
        else if (user_para->item_type == TYPE_SWITCH)
        {
            (plstm_item + i)->nctrl           = 1;
            plstm_ctrl = (__lstm_item_ctrl_t *)esMEMS_Balloc(sizeof(__lstm_item_ctrl_t) * (plstm_item + i)->nctrl);
            eLIBs_memset(plstm_ctrl, 0, sizeof(__lstm_item_ctrl_t) * (plstm_item + i)->nctrl);
            _bttn_create(plstm_ctrl, user_para, msg->h_deswin, ID_LISTITEM + i + 1);
            (plstm_item + i)->ctrls           = plstm_ctrl;
            (plstm_item + i)->cb              = _draw_lstm;
        }
        else
        {
            (plstm_item + i)->nctrl       = 0;
            (plstm_item + i)->ctrls       = NULL;
            (plstm_item + i)->cb          = _draw_lstm;
        }

        (plstm_item + i)->p_arg           = plitem_para;
        user_para = user_para->next;
    }

    plstm_para->items       = plstm_item;
    plstm_para->steps       = 12;
    plstm_para->mode        = LINEAR_ROLL | FAST_SPEED | LISTBOX_MOD;
    plstm_para->curitemno   = opt;
    ctlcreate_info.name                 = "listmenu";
    ctlcreate_info.hParent              = msg->h_deswin;
    ctlcreate_info.dwStyle              = WS_VISIBLE;
    ctlcreate_info.dwExStyle            = WS_EX_CHILDPROC;
    ctlcreate_info.spClassName          = CTRL_LISTMENU;
    ctlcreate_info.ctl_rect.x           = puipara->lstm_ctl.x;
    ctlcreate_info.ctl_rect.y           = puipara->lstm_ctl.y;
    ctlcreate_info.ctl_rect.width       = puipara->lstm_ctl.w;
    ctlcreate_info.ctl_rect.height      = puipara->lstm_ctl.h;
    ctlcreate_info.id                   = SSET_SUBMENU_ID;
    ctlcreate_info.attr                 = plstm_para;
    h_win = GUI_CtrlWinCreate(&ctlcreate_info);

    if (!h_win)
    {
        return EPDK_FAIL;
    }

    plist_data->h_lstm = h_win;
    GUI_WinSetAddData(msg->h_deswin, (unsigned long)plist_data);
    GUI_WinSetFocusChild(h_win);
    //GUI_LyrWinSetSta(GUI_WinGetLyrWin(msg->h_deswin), GUI_LYRWIN_STA_ON);
    //GUI_LyrWinSetTop(GUI_WinGetLyrWin(msg->h_deswin));
    //GUI_LyrWinSel(GUI_WinGetLyrWin(msg->h_deswin));
    //_draw_back(puipara->lstm_ctl.w, puipara->menu_lstm_fcs.w);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       _menu_lstm_create
*
*Description: _menu_lstm_create
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void  _menu_lstm_destroy(H_WIN  h_win)
{
    __u32                           i;
    __u32                           lstm_nctrl;
    listmenu_data_t                 *plist_data;
    __lstm_para_t                   *plstm_para;
    __lstm_item_attr_t              *plstm_item;
    lstm_item_para_t                *plitem_para;
    __lstm_item_ctrl_t              *plstm_ctrls;
    __sset_uipara_t                 *puipara;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(GUI_WinGetParent(h_win));
    plist_data = (listmenu_data_t *)GUI_WinGetAddData(h_win);
    plstm_para = (__lstm_para_t *)GUI_WinGetAttr(plist_data->h_lstm);

    if (plstm_para)
    {
        plstm_item = plstm_para->items;

        if (plstm_item)
        {
            for (i = 0; i < plstm_para->nitem; i++)
            {
                plitem_para = (lstm_item_para_t *)(plstm_item + i)->p_arg;

                if (plitem_para)
                {
                    if (plitem_para->focus_bmp)
                    {
                        //                      esMEMS_Bfree(plitem_para->focus_bmp, puipara->menu_lstm_bmp.w*puipara->menu_lstm_bmp.h*4 + 0x100);
                        Destroy_BMP_RES_Pt(plitem_para->focus_bmp);
                        plitem_para->focus_bmp = NULL;
                    }

                    if (plitem_para->unfocus_bmp)
                    {
                        //                      esMEMS_Bfree(plitem_para->unfocus_bmp, puipara->menu_lstm_bmp.w*puipara->menu_lstm_bmp.h*4 + 0x100);
                        Destroy_BMP_RES_Pt(plitem_para->unfocus_bmp);
                        plitem_para->unfocus_bmp = NULL;
                    }

                    if (plitem_para->opt_bmp)
                    {
                        //                      esMEMS_Bfree(plitem_para->opt_bmp, puipara->menu_opt_bmp.w*puipara->menu_opt_bmp.h*4 + 0x100);
                        Destroy_BMP_RES_Pt(plitem_para->opt_bmp);
                        plitem_para->opt_bmp = NULL;
                    }

                    if (plitem_para->text)
                    {
                        esMEMS_Bfree(plitem_para->text, 0x30);
                        plitem_para->text = NULL;
                    }

                    esMEMS_Bfree(plitem_para, sizeof(lstm_item_para_t));
                    plitem_para = NULL;
                }

                lstm_nctrl = (plstm_item + i)->nctrl;

                if (lstm_nctrl > 0)
                {
                    plstm_ctrls = (plstm_item + i)->ctrls;

                    if (eLIBs_strcmp(plstm_ctrls->spClassName, CTRL_SLIDER) == 0)
                    {
                        _slider_destroy(plstm_ctrls, h_win);
                        esMEMS_Bfree(plstm_ctrls, sizeof(__lstm_item_ctrl_t));
                        plstm_ctrls = NULL;
                    }
                    else if (eLIBs_strcmp(plstm_ctrls->spClassName, CTRL_SPINBOX) == 0)
                    {
                        _spin_destroy(plstm_ctrls, h_win);
                        esMEMS_Bfree(plstm_ctrls, sizeof(__lstm_item_ctrl_t));
                        plstm_ctrls = NULL;
                    }
                    else if (eLIBs_strcmp(plstm_ctrls->spClassName, CTRL_BUTTON) == 0)
                    {
                        _bttn_destroy(plstm_ctrls, h_win);
                        esMEMS_Bfree(plstm_ctrls, sizeof(__lstm_item_ctrl_t));
                        plstm_ctrls = NULL;
                    }
                }
            }

            esMEMS_Bfree(plstm_item, sizeof(__lstm_item_attr_t)*plstm_para->nitem);
            plstm_item = NULL;
        }

        esMEMS_Bfree(plstm_para, sizeof(__lstm_para_t));
        plstm_para = NULL;
    }

    if (plist_data)
    {
        esMEMS_Bfree(plist_data, sizeof(listmenu_data_t));
        plist_data = NULL;
    }
}

/*
************************************************************************************************************************
*                                       _slider_command
*
*Description: _slider_command
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32  _slider_command(H_WIN h_win, __u32 wid, __sset_item_t *para)
{
    __sset_item_attr_t      *pitem_attr;
    __sset_item_t           *user_para;
    user_para = para;
    pitem_attr = &user_para->item_attr;

    switch (wid)
    {
        case SDN_STARTDRAG:
        {
        }
        break;

        case SDN_STOPDRAG:
        {
            pitem_attr->cur_value = SLIDER_GetValue(h_win);
            user_para->item_msg.msg_value = pitem_attr->cur_value;
        }

        return 0;

        case SDN_POS_CHANGED:
        {
            pitem_attr->cur_value = SLIDER_GetValue(h_win);
            user_para->item_msg.msg_value = pitem_attr->cur_value;
        }

        return 0;

        default:
            break;
    }

    return -1;
}

/*
************************************************************************************************************************
*                                       _spin_command
*
*Description: _spin_command
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32  _spin_command(__u32 value, __u32 wid, __sset_item_t *para)
{
    __sset_item_attr_t      *pitem_attr;
    __sset_item_t           *user_para;
    user_para = para;
    pitem_attr = &user_para->item_attr;

    switch (wid)
    {
        case SPN_CHG:
        {
            pitem_attr->cur_value =  value;
            user_para->item_msg.msg_value = value;
        }

        return 0;

        //      case SPN_STOPCHG:
        //          {
        //              pitem_attr->cur_value =  value;
        //              user_para->item_msg.msg_value = value;
        //          }
        //          return 0;
        default:
            break;
    }

    return -1;
}
/*
************************************************************************************************************************
*                                       _bttn_command
*
*Description: _bttn_command
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static __s32  _bttn_command(H_WIN h_win, __u32 wid, __sset_item_t *para)
{
    __sset_item_t           *user_para;
    user_para = para;

    switch (wid)
    {
        case BN_CLICKED:
        {
            user_para->item_msg.msg_value = 1;
        }

        return 0;

        case BN_PUSHED:
        {
        }
        break;

        default:
            break;
    }

    return -1;
}

/*
************************************************************************************************************************
*                                       menu_lstm_proc
*
*Description: menu_lstm win回调
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static void  _menu_lstm_command(__gui_msg_t *msg)
{
    listmenu_data_t         *plist_data;
    plist_data = (listmenu_data_t *)GUI_WinGetAddData(msg->h_deswin);

    if (LOSWORD(msg->dwAddData1) == SSET_SUBMENU_ID)
    {
        if (HISWORD(msg->dwAddData1) == LSTMN_UNPUSHED)
        {
            switch (LOSWORD(msg->dwAddData2))
            {
                case GUI_MSG_TOUCH_LONGDOWN:
                case GUI_MSG_TOUCH_DOWN:
                case GUI_MSG_KEY_LONGENTER:
                case GUI_MSG_KEY_ENTER:
                {
                    __s32                   ret = 0;
                    __sset_item_t           *user_para = 0;
                    H_ITEM                  hcuritem = 0;
                    lstm_item_para_t        *plitem_para = 0;
                    hcuritem = GUI_LstmGetCurItem(plist_data->h_lstm);

                    if (hcuritem == 0)
                    {
                        break;
                    }

                    plitem_para = (lstm_item_para_t *)GUI_LstmGetItemAttr(hcuritem);
                    user_para = (__sset_item_t *)plitem_para->user_item;

                    /*if((user_para->item_type == TYPE_OPT)&&(user_para->item_sta == 1))
                    {
                        plist_data->user_opt->item_attr.opt     = 0;
                        user_para->item_attr.opt                = 1;
                        plist_data->user_opt                    = user_para;
                    }*/
                    if (user_para->item_sta == 1)
                    {
                        __gui_msg_t     notifymsg;
                        notifymsg.h_srcwin      = msg->h_deswin;
                        notifymsg.h_deswin      = GUI_WinGetParent(msg->h_deswin);
                        notifymsg.id            = GUI_MSG_COMMAND;
                        notifymsg.dwAddData1    = (__u32)MAKELONG(SSET_SUBMENU_ID, HISWORD(msg->dwAddData1));
                        notifymsg.dwAddData2    = plitem_para->item_id;
                        notifymsg.dwReserved    = (unsigned long)user_para;
                        notifymsg.p_arg         = NULL;
                        ret = GUI_SendMessage(&notifymsg);
                    }

                    if (ret == EPDK_OK)
                    {
                        if ((user_para->item_type == TYPE_OPT) && (user_para->item_sta == 1))
                        {
                            plist_data->user_opt->item_attr.opt     = 0;
                            user_para->item_attr.opt                = 1;
                            plist_data->user_opt                    = user_para;
                        }

                        GUI_WinUpdate(plist_data->h_lstm, 1);
                    }
                }
                break;

                default:
                    break;
            }
        }
    }
    else if ((LOSWORD(msg->dwAddData1) > ID_LISTITEM) && (LOSWORD(msg->dwAddData1) < (ID_LISTITEM + plist_data->item_cnt + 1)))
    {
        __s32                   ret;
        H_WIN                   item_win;
        H_ITEM                  hcuritem;
        __sset_item_t           *user_para;
        lstm_item_para_t        *plitem_para;
        hcuritem = GUI_LstmGetCurItem(plist_data->h_lstm);

        if (hcuritem == 0)
        {
            return;
        }

        plitem_para = (lstm_item_para_t *)GUI_LstmGetItemAttr(hcuritem);
        item_win = GUI_WinGetDlgItem(plist_data->h_lstm, ID_LISTITEM + plitem_para->item_id + 1);
        user_para = (__sset_item_t *)plitem_para->user_item;

        if (user_para->item_type == TYPE_SLIDE)
        {
            ret = _slider_command(item_win, HISWORD(msg->dwAddData1), user_para);
        }
        else if (user_para->item_type == TYPE_SPIN)
        {
            ret = _spin_command(msg->dwAddData2, HISWORD(msg->dwAddData1), user_para);
        }
        else if (user_para->item_type == TYPE_SWITCH)
        {
            ret = _bttn_command(item_win, HISWORD(msg->dwAddData1), user_para);
        }
        else
        {
            ret = -1;
        }

        if ((ret == 0) && (user_para->item_sta == 1))
        {
            __gui_notify_msg_t      notify_msg;
            notify_msg.hWnd         = msg->h_deswin;
            notify_msg.id           = SSET_SUBMENU_ID;
            notify_msg.msgcode      = HISWORD(msg->dwAddData1);
            notify_msg.dwAddData    = plitem_para->item_id;
            notify_msg.dwReserved   = (unsigned long)user_para;
            GUI_NotifyParent(&notify_msg);
        }
    }
}

/*
************************************************************************************************************************
*                                       menu_lstm_proc
*
*Description: menu_lstm win回调
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
static int menu_lstm_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            _menu_lstm_create(msg);
        }

        return EPDK_OK;

        case GUI_MSG_NOTIFY_CHILD_DELETED:
            break;

        case GUI_MSG_PAINT:
            return EPDK_OK;

        case GUI_MSG_KEY:
        {
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_LONGUP:
                case GUI_MSG_KEY_LONGDOWN:
                    break;

                default:
                    break;
            }
        }
        break;

        case GUI_MSG_TOUCH:
        {
            listmenu_data_t         *plist_data;
            plist_data = (listmenu_data_t *)GUI_WinGetAddData(msg->h_deswin);

            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_DOWN:
                {
                    plist_data->touch_cnt = 50;
                }
                break;

                case GUI_MSG_TOUCH_LONGDOWN:
                {
                    if (plist_data->touch_cnt > 0)
                    {
                        plist_data->touch_cnt--;
                        return EPDK_OK;
                    }
                }
                break;

                default:
                    break;
            }
        }
        break;

        case GUI_MSG_COMMAND:
        {
            _menu_lstm_command(msg);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            _menu_lstm_destroy(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*                                       SSET_MenuLstm_Create
*
*Description:
*
*Arguments  : hparent 父窗口
*             pitem   本级菜单所对应的参数节点
*
*Return     :
*
************************************************************************************************************************
*/
H_WIN SSET_MenuLstm_Create(H_WIN hparent, H_WIN hlayer, __sset_item_t *pitem)
{
    H_WIN                           h_win = 0 ;
    __sset_uipara_t                 *puipara;
    __gui_framewincreate_para_t     frmcreate_info;
    puipara = (__sset_uipara_t *)GUI_WinGetAddData(hparent);
    eLIBs_memset(&frmcreate_info, 0, sizeof(__gui_framewincreate_para_t));
    frmcreate_info.attr             = (void *)pitem;
    frmcreate_info.BkColor.alpha    = 0;
    frmcreate_info.BkColor.red      = 0;
    frmcreate_info.BkColor.green    = 0;
    frmcreate_info.BkColor.blue     = 0;
    frmcreate_info.dwStyle          = WS_VISIBLE;
    frmcreate_info.dwExStyle        = 0;
    frmcreate_info.hLayer           = hlayer;
    frmcreate_info.hOwner           = 0;
    frmcreate_info.hHosting         = hparent;
    frmcreate_info.FrameWinProc     = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)menu_lstm_proc);
    frmcreate_info.rect.x           = puipara->lstm_frm.x;
    frmcreate_info.rect.y           = puipara->lstm_frm.y;
    frmcreate_info.rect.width       = puipara->lstm_frm.w;
    frmcreate_info.rect.height      = puipara->lstm_frm.h;
    h_win = GUI_FrmWinCreate(&frmcreate_info);

    if (!h_win)
    {
        __wrn("frame create create failed!\n");
        return NULL;
    }

    return h_win;
}

/*
************************************************************************************************************************
*                                       SSET_MenuLstm_Destroy
*
*Description: 销毁一级子菜单menu_lstm
*
*Arguments  : cnt 菜单级数
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuLstm_Destroy(H_WIN h_win)
{
    if (h_win)
    {
        GUI_FrmWinDelete(h_win);
        h_win = 0;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_MenuLstm_SetFocus
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuLstm_SetFocus(H_WIN h_win, __u32 fcs_id)
{
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       SSET_SubMenuUpdateItem
*
*Description: 更新子菜单submenu的一个条目
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuLstm_UpdateItem(H_WIN h_win, __u32 item_no)
{
    listmenu_data_t         *plist_data;
    plist_data = (listmenu_data_t *)GUI_WinGetAddData(h_win);
    GUI_LstmUpdateItem(plist_data->h_lstm, item_no);
    return EPDK_OK;
}

