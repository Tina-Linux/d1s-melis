/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtlayer_process.c
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-18
*
*Description :      txtlayer process function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-18         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTLAYER_PROCESS_C__
#define __TXTLAYER_PROCESS_C__

#include "txtcfg.h"
#include "txtbox_i.h"
#include "txtlayer.h"
#include "txtlist.h"
#include "txtitem.h"
#include "txtlayer_process.h"

static __s32  charset_enm2gui(__s32 charset_enm)
{
    switch (charset_enm)
    {
        case EPDK_CHARSET_ENM_GBK         :
            GUI_UC_SetEncodeGBK();
            return 0;

        case EPDK_CHARSET_ENM_BIG5        :
            GUI_UC_SetEncodeBig5();
            return 0;

        case EPDK_CHARSET_ENM_UTF8        :
            GUI_UC_SetEncodeUTF8();
            return 0;

        case EPDK_CHARSET_ENM_UTF16BE     :
        case EPDK_CHARSET_ENM_UTF16LE     :
            return -1;

        case EPDK_CHARSET_ENM_SJIS        :
            GUI_UC_SetEncodeSJIS();
            return 0;

        case EPDK_CHARSET_ENM_EUC_JP      :
            GUI_UC_SetEncodeEUCJP();
            return 0;

        case EPDK_CHARSET_ENM_EUC_KR      :
            GUI_UC_SetEncodeEUCKR();
            return 0;

        case EPDK_CHARSET_ENM_KIO8_R      :
            GUI_UC_SetEncodeKIO8R();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_1  :
            GUI_UC_SetEncodeISO88591();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_2  :
            GUI_UC_SetEncodeISO88592();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_3  :
            GUI_UC_SetEncodeISO88593();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_4  :
            GUI_UC_SetEncodeISO88594();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_5  :
            GUI_UC_SetEncodeISO88595();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_6  :
            GUI_UC_SetEncodeISO88596();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_7  :
            GUI_UC_SetEncodeISO88597();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_8  :
            GUI_UC_SetEncodeISO88598();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_9  :
            GUI_UC_SetEncodeISO88599();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_10 :
            GUI_UC_SetEncodeISO885910();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_11 :
            GUI_UC_SetEncodeISO885911();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_12 :
            GUI_UC_SetEncodeISO885912();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_13 :
            GUI_UC_SetEncodeISO885913();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_14 :
            GUI_UC_SetEncodeISO885914();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_15 :
            GUI_UC_SetEncodeISO885915();
            return 0;

        case EPDK_CHARSET_ENM_ISO_8859_16 :
            GUI_UC_SetEncodeISO885916();
            return 0;

        case EPDK_CHARSET_ENM_CP874       :
            GUI_UC_SetEncodeCP874();
            return 0;

        case EPDK_CHARSET_ENM_CP1250      :
            GUI_UC_SetEncodeCP1250();
            return 0;

        case EPDK_CHARSET_ENM_CP1251      :
            GUI_UC_SetEncodeCP1251();
            return 0;

        case EPDK_CHARSET_ENM_CP1253      :
            GUI_UC_SetEncodeCP1253();
            return 0;

        case EPDK_CHARSET_ENM_CP1255      :
            GUI_UC_SetEncodeCP1255();
            return 0;

        case EPDK_CHARSET_ENM_CP1256      :
            GUI_UC_SetEncodeCP1256();
            return 0;

        case EPDK_CHARSET_ENM_CP1257      :
            GUI_UC_SetEncodeCP1257();
            return 0;

        case EPDK_CHARSET_ENM_CP1258      :
            GUI_UC_SetEncodeCP1258();
            return 0;

        default :
            GUI_UC_SetEncodeGBK();
            return -1;
    }
}


/*
**********************************************************************************************************************
*                                               TxtLayer_BackUpItem
*
* author:
*
* date:             2009-2-22
*
* Description:      txtlayer backupitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  TxtLayer_BackUpItem(txtlayer_man_t    *txtlayer)
{
    // __u32                size;
    void                *src_addr;
    __u32                bpp;
    __u32                itemsize;
    // __u32                backcount;
    bpp             = txtlayer->txtlist->bpp;
    itemsize        = txtlayer->txtlist->ItemSize;

    if (txtlayer->backup == EPDK_FALSE)
    {
        //size        = itemsize;
        src_addr    = (void *)((unsigned long)txtlayer->txtbuffer->buffer +
                               ((txtlayer->txtbuffer->rectsz.width * txtlayer->txtlist->CurItem->rect.y * bpp + 7) / 8) +
                               ((txtlayer->txtlist->CurItem->rect.x * bpp + 7) / 8));   /*get source address */
        txtlayer->back_count    = 1;
        txtlayer->res_addr      = src_addr;
        txtlayer->res_x         = txtlayer->txtlist->CurItem->rect.x;
        txtlayer->res_y         = txtlayer->txtlist->CurItem->rect.y;
        txtlayer->back_size         = itemsize;
        txtlayer->back_pos.x    = txtlayer->txtlist->CurItem->validpos.x;
        txtlayer->back_pos.y    = txtlayer->txtlist->CurItem->rect.y;

        if (GUI_GetScnDir() == GUI_SCNDIR_NORMAL)
        {
            eLIBs_memcpy(txtlayer->chg_fb, txtlayer->res_addr, txtlayer->back_size);
        }
        else
        {
            GUI_RECT  destrect;
            destrect.x0     = txtlayer->txtlist->CurItem->rect.x;
            destrect.y0     = txtlayer->txtlist->CurItem->rect.y;
            destrect.x1     = txtlayer->txtlist->CurItem->rect.x + txtlayer->txtlist->head->rect.width - 1;
            destrect.y1     = txtlayer->txtlist->CurItem->rect.y + txtlayer->txtlist->head->rect.height - 1;
            GUI_LyrWinSel(txtlayer->layer);
            GUI_GetRectMem(&destrect, txtlayer->chg_fb, txtlayer->txtlist->head->rect.width,
                           txtlayer->txtlist->head->rect.height,
                           0, 0);
        }

        txtlayer->backup = EPDK_TRUE;
        return OK;
    }

    if (txtlayer->txtlist->PrevItem == txtlayer->txtlist->CurItem)
    {
        return OK;
    }

    src_addr    = (void *)((unsigned long)txtlayer->txtbuffer->buffer +
                           ((txtlayer->txtbuffer->rectsz.width * txtlayer->txtlist->CurItem->rect.y * bpp + 7) / 8) +
                           ((txtlayer->txtlist->CurItem->rect.x * bpp + 7) / 8));   /*get source address */

    if (GUI_GetScnDir() == GUI_SCNDIR_NORMAL)
    {
        eLIBs_memcpy(txtlayer->res_addr, txtlayer->chg_fb, txtlayer->back_size);
    }
    else
    {
        GUI_RECT  destrect;
        destrect.x0     = txtlayer->res_x;
        destrect.y0     = txtlayer->res_y;
        destrect.x1     = txtlayer->res_x + txtlayer->txtlist->head->rect.width - 1;
        destrect.y1     = txtlayer->res_y + txtlayer->txtlist->head->rect.height - 1;
        GUI_LyrWinSel(txtlayer->layer);
        GUI_SetRectMem(&destrect, txtlayer->chg_fb, txtlayer->txtlist->head->rect.width,
                       txtlayer->txtlist->head->rect.height, 0, 0);
    }

    txtlayer->back_count    = 1;
    txtlayer->res_addr      = src_addr;
    txtlayer->res_x         = txtlayer->txtlist->CurItem->rect.x;
    txtlayer->res_y         = txtlayer->txtlist->CurItem->rect.y;
    txtlayer->back_size     = itemsize;
    txtlayer->back_pos.x    = txtlayer->txtlist->CurItem->validpos.x;
    txtlayer->back_pos.y    = txtlayer->txtlist->CurItem->rect.y;

    if (GUI_GetScnDir() == GUI_SCNDIR_NORMAL)
    {
        eLIBs_memcpy(txtlayer->chg_fb, src_addr, itemsize);
    }
    else
    {
        GUI_RECT  destrect;
        destrect.x0     = txtlayer->txtlist->CurItem->rect.x;
        destrect.y0     = txtlayer->txtlist->CurItem->rect.y;
        destrect.x1     = txtlayer->txtlist->CurItem->rect.x + txtlayer->txtlist->head->rect.width - 1;
        destrect.y1     = txtlayer->txtlist->CurItem->rect.y + txtlayer->txtlist->head->rect.height - 1;
        GUI_LyrWinSel(txtlayer->layer);
        GUI_GetRectMem(&destrect, txtlayer->chg_fb, txtlayer->txtlist->head->rect.width,
                       txtlayer->txtlist->head->rect.height, 0, 0);
    }

    return OK;
}
/*
**********************************************************************************************************************
*                                               TxtLayer_StampToPos
*
* author:
*
* date:             2009-2-22
*
* Description:      txtlayer stamptopos
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
__u32 TxtLayer_StampToPos(txtlayer_man_t *txtlayer, __u32 Stamp, __pos_t *pos)
{
    __u32           i;
    __u32           dist;
    txtitem_t       *item;
    txtitem_t       *next;
    item  = txtlayer->txtlist->head;
    next  = txtlayer->txtlist->head->Next;

    for (i = 0; i < txtlayer->txtlist->ItemCount; i++)
    {
        if (Stamp >= item->stamp)
        {
            if (next == txtlayer->txtlist->head)
            {
                txtlayer->txtlist->PrevItem     = txtlayer->txtlist->CurItem;
                txtlayer->txtlist->CurItem      = item;

                if (Stamp > item->estamp)
                {
                    pos->x = 0;
                    pos->y = txtlayer->txtlist->CurItem->rect.y + txtlayer->txtlist->CurItem->rect.height
                             - txtlayer->start_pos.y;
                    return OK;
                }
                else
                {
                    dist = (Stamp - item->stamp) * txtlayer->txtlist->head->rect.height / (item->estamp - item->stamp);
                    pos->x = 0;
                    pos->y = item->rect.y + dist - txtlayer->start_pos.y;
                    return OK;
                }
            }
            else if (Stamp < next->stamp)
            {
                txtlayer->txtlist->PrevItem     = txtlayer->txtlist->CurItem;
                txtlayer->txtlist->CurItem      = item;

                if (Stamp > item->estamp)
                {
                    pos->x = 0;
                    pos->y = next->rect.y - txtlayer->start_pos.y;
                }
                else
                {
                    dist = (Stamp - item->stamp) * txtlayer->txtlist->head->rect.height / (item->estamp - item->stamp);
                    pos->x = 0;
                    pos->y = item->rect.y + dist - txtlayer->start_pos.y;
                }

                return OK;
            }
        }
        else if (Stamp < txtlayer->txtlist->head->stamp)
        {
            txtlayer->txtlist->PrevItem     = txtlayer->txtlist->CurItem;
            txtlayer->txtlist->CurItem      = item;
            //dist = Stamp * txtlayer->txtlist->head->rect.height/item->stamp;
            pos->x = 0;
            pos->y = 0;
            return OK;
        }

        item = next;
        next = item->Next;
    }

    return __LINE__;
}
/*
**********************************************************************************************************************
*                                               TxtLayer_ItemCopy
*
* author:
*
* date:             2009-2-22
*
* Description:      txtlayer itemcopy
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __u32 TxtLayer_ItemCopy(txtlayer_man_t *txtlayer, __pos_t *srcstart, __pos_t *srcend, __pos_t *dststart, __pos_t *dstend)
{
    //__u32  count     = 0;
    //__u32  csize     = 0;
    __u32  osize     = 0;
    //__u32  itemsize  = 0;
    __u32  bpp;
    __u32  stride   = 0;
    //void  *csrcaddr;
    //void  *cdstaddr;
    void  *osrcaddr;
    void  *odstaddr;
    __u32  i;
    bpp = txtlayer->txtlist->bpp;

    if (GUI_GetScnDir() == GUI_SCNDIR_NORMAL)
    {
        //count = (dstend->y - dststart->y)/txtlayer->txtlist->head->rect.height;
        odstaddr = (void *)((unsigned long)txtlayer->txtbuffer->buffer +
                            ((txtlayer->txtbuffer->rectsz.width * dststart->y * bpp + 7) / 8) +
                            ((dststart->x * bpp + 7) / 8));   /*get source address */
        osize = ((dstend->x - dststart->x) * bpp + 7) >> 3;
        osrcaddr = (void *)((unsigned long)txtlayer->foucs_FB +
                            ((txtlayer->txtbuffer->rectsz.width * srcstart->y * bpp + 7) / 8) +
                            ((srcstart->x * bpp + 7) / 8));   /*get source address */
        stride = (txtlayer->txtbuffer->rectsz.width * bpp + 7) >> 3;

        for (i = 0; i < txtlayer->txtlist->head->rect.height; i++)
        {
            eLIBs_memcpy(odstaddr, osrcaddr, osize);
            osrcaddr = (void *)((unsigned long)osrcaddr + stride);
            odstaddr = (void *)((unsigned long)odstaddr + stride);
        }
    }
    else
    {
        GUI_RECT  destrect;
        destrect.x0     = srcstart->x;
        destrect.y0     = srcstart->y;
        destrect.x1     = srcstart->x + dstend->x - dststart->x;
        destrect.y1     = srcstart->y + txtlayer->txtlist->head->rect.height - 1;
        GUI_LyrWinSel(txtlayer->focus_lyr);
        GUI_GetRectMemEx(&destrect, txtlayer->txtbuffer->buffer, txtlayer->txtbuffer->rectsz.width,
                         txtlayer->txtbuffer->rectsz.height, dststart->x, dststart->y);
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               TxtLayer_StampProcess
*
* author:
*
* date:             2009-2-22
*
* Description:      txtlayer stampprocess
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 TxtLayer_StampProcess(txtlayer_man_t *txtlayer, __u32 Stamp)
{
    __pos_t         srcend;
    __pos_t         srcstart;
    __pos_t         dstend;
    __pos_t         dststart;
    __u32           stamp_len;

    if (!(txtlayer->status & TXTBOX_DRAWFINISH))
    {
        return __LINE__;
    }

    TxtLayer_BackUpItem(txtlayer);

    if (txtlayer->mode == OK_MODE)
    {
        if (txtlayer->txtlist->PrevItem != txtlayer->txtlist->CurItem)
        {
            TxtLayer_DrawMultiItem(txtlayer);
        }

        if (txtlayer->lastmode != OK_MODE)
        {
            if (GUI_GetScnDir() == GUI_SCNDIR_NORMAL)
            {
                eLIBs_memcpy(txtlayer->res_addr, txtlayer->chg_fb, txtlayer->back_size);
            }
            else
            {
                GUI_RECT  destrect;
                destrect.x0     = txtlayer->res_x;
                destrect.y0     = txtlayer->res_y;
                destrect.x1     = txtlayer->res_x + txtlayer->txtlist->head->rect.width - 1;
                destrect.y1     = txtlayer->res_y + txtlayer->txtlist->head->rect.height - 1;
                GUI_LyrWinSel(txtlayer->layer);
                GUI_SetRectMem(&destrect, txtlayer->chg_fb, txtlayer->txtlist->head->rect.width,
                               txtlayer->txtlist->head->rect.height, 0, 0);
            }

            txtlayer->lastmode = OK_MODE;
        }

        stamp_len = txtlayer->txtlist->CurItem->estamp - txtlayer->txtlist->CurItem->stamp;
        dstend.y = txtlayer->txtlist->CurItem->rect.y;

        if (Stamp > txtlayer->txtlist->CurItem->estamp)
        {
            dstend.x = txtlayer->txtlist->CurItem->font_len
                       + txtlayer->txtlist->CurItem->validpos.x;
        }
        else if (Stamp > txtlayer->txtlist->CurItem->stamp)
        {
            dstend.x = ((Stamp - txtlayer->txtlist->CurItem->stamp) * txtlayer->txtlist->CurItem->font_len / stamp_len)
                       + txtlayer->txtlist->CurItem->validpos.x;
        }
        else
        {
            dstend.x = txtlayer->txtlist->CurItem->validpos.x;
        }

        dststart.y = txtlayer->back_pos.y;
        dststart.x = txtlayer->back_pos.x;
        srcstart.y = 0;
        srcstart.x = txtlayer->txtlist->CurItem->validpos.x;
        srcend.y = dstend.y - dststart.y;
        srcend.x = txtlayer->txtlist->CurItem->validpos.x + dstend.x - dststart.x;
        TxtLayer_ItemCopy(txtlayer, &srcstart, &srcend, &dststart, &dstend);
        //TxtLayer_AlphaProcess(txtlayer);
        GUI_LyrWinFlushFB(txtlayer->layer);
    }
    else
    {
        if (txtlayer->txtlist->PrevItem != txtlayer->txtlist->CurItem || txtlayer->lastmode == OK_MODE)
        {
            TxtLayer_DrawMultiItem(txtlayer);
            dstend.y = txtlayer->txtlist->CurItem->rect.y;
            dstend.x = txtlayer->txtlist->CurItem->font_len
                       + txtlayer->txtlist->CurItem->validpos.x;
            dststart.y = txtlayer->back_pos.y;
            dststart.x = txtlayer->back_pos.x;
            srcstart.y = 0;
            srcstart.x = txtlayer->txtlist->CurItem->validpos.x;
            srcend.y = dstend.y - dststart.y;
            srcend.x = txtlayer->txtlist->CurItem->validpos.x + dstend.x - dststart.x;
            TxtLayer_ItemCopy(txtlayer, &srcstart, &srcend, &dststart, &dstend);
            //TxtLayer_AlphaProcess(txtlayer);
            GUI_LyrWinFlushFB(txtlayer->layer);
            txtlayer->lastmode = LINE_MODE;
        }
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               TxtLayer_DrawItem
*
* author:
*
* date:             2009-2-22
*
* Description:      txtlayer drawitem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 TxtLayer_DrawItem(txtlayer_man_t *txtlayer, txtitem_t   *txtitem)
{
    //__u32 font_len;
    __u16 uni, CharWidth;
    __s32 char_size;
    __u32 i = 0;
    txtitem->font_len = 0;
    charset_enm2gui(txtlayer->draw_code);
    GUI_SetFont(txtlayer->draw_font);

    while (i < txtitem->length)
    {
        char_size = esCHS_Char2Uni(txtlayer->draw_code,
                                   (__u8 *)txtitem->string + i,
                                   txtitem->length - i,
                                   &uni);

        if (char_size <= 0)
        {
            break;
        }

        CharWidth = GUI_GetCharDistX(uni);
        i += char_size;
        txtitem->font_len += CharWidth;
    }

    /*draw first layer*/
    GUI_LyrWinSel(txtlayer->layer);
    charset_enm2gui(txtlayer->draw_code);
    GUI_SetBkColor(txtlayer->bk_color);
    GUI_ClearRect(txtitem->rect.x,
                  txtitem->rect.y,
                  (txtitem->rect.x + txtitem->rect.width - 1),
                  (txtitem->rect.y + txtitem->rect.height - 1));

    /*draw foucs string*/
    if (txtitem->font_len < txtitem->rect.width)
    {
        txtitem->validpos.x = (txtitem->rect.width - txtitem->font_len) >> 1;
    }
    else
    {
        txtitem->validpos.x = 0;
    }

    //txtitem->validpos.y = (txtitem->rect.height - txtlayer->font_size)>>1;
    txtitem->validpos.y = 0;
    GUI_SetFont(txtlayer->draw_font);
    GUI_SetFontMode(GUI_FONTMODE_8BPP128_1);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_DispStringAt(txtitem->string,
                     txtitem->validpos.x,
                     txtitem->rect.y + txtitem->validpos.y);
    return OK;
}

__u32 TxtLayer_FoucsDrawItem(txtlayer_man_t *txtlayer, txtitem_t   *txtitem, __u32 x, __u32 y)
{
    //   __u32 font_len;
    /*draw the second layer*/
    GUI_LyrWinSel(txtlayer->focus_lyr);
    charset_enm2gui(txtlayer->draw_code);
    GUI_SetBkColor(txtlayer->bk_color);
    GUI_ClearRect(x, y,
                  (x + txtitem->rect.width - 1),
                  (y + txtitem->rect.height - 1));
    GUI_SetFont(txtlayer->draw_font);
    GUI_SetFontMode(GUI_FONTMODE_8BPP128_2);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    //GUI_SetColor(GUI_WHITE);
    GUI_DispStringAt(txtitem->string,
                     txtitem->validpos.x,
                     y + txtitem->validpos.y);
    return OK;
}

__u32 TxtLayer_DrawMultiItem(txtlayer_man_t *txtlayer)
{
    __u32               x = 0;
    __u32               y = 0;
    __u32               count;
    txtitem_t          *Fitem;
    Fitem = txtlayer->txtlist->CurItem;

    for (count = 0; count < 1; count++)
    {
        TxtLayer_FoucsDrawItem(txtlayer, Fitem, x, y);
        y       = y + Fitem->rect.height;
        Fitem   = Fitem->Next;
    }

    return OK;
}
//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------
#endif
