/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : gui.c
*
* By      : Steven
* Version : v1.0
* Date    : 2009-1-6 11:59:31
**********************************************************************************************************************
*/

#include "GUI_Private.h"
#include <elibs_stdio.h>
#include <mod_display.h>
#include <elibs_string.h>
#include <mod_touchpanel.h>
#include "gui_x.h"
#include "core_private.h"
#include <kapi.h>
#include <mod_defs.h>


__gui_rect_t        g_rcScr;
__u8                gui_scndir = GUI_SCNDIR_NORMAL;
__gui_rect_zoom gui_zoom_rect = {1,1,1,1};

/*
**********************************************************************************************************************
*                                               GUI_Init
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
int GUI_Init(void)
{
    int         r;
    __u32       width;
    __u32       height;
	__u64 args[3];
	__u32 mid_display;
	__mp *mp_display = NULL;

	mp_display = esKSRV_Get_Display_Hld();
	if(!mp_display)
	{
		__log("disp open fail!\n");
	}
	args[0] = 0;
	args[1] = 0;
	args[2] = 0;
	width 	= esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, SEL_SCREEN,(void *)args);
	height 	= esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, SEL_SCREEN,(void *)args);
	__wrn("GUI_Init gui get scn    w = %d    h = %d",width,height);
//    GUI_SetZoomWin(800,480,600,360);
    g_rcScr.left    = 0;
    g_rcScr.top     = 0;
    g_rcScr.right   = width - 1;
    g_rcScr.bottom  = height - 1;
    GUI_X_Init();           //  KeySem = esKRNL_SemCreate(0);   mcl
    GUI_WinInit();
    r = GUI_Core_Init();
    return r;
}
/*
**********************************************************************************************************************
*                                               GUI_Exit
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
int GUI_Exit(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_GetScnDir
*
* author:
*
* date:             2010-3-20
*
* Description:      gui getscndir
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

int GUI_GetScnDir(void)
{
    return  gui_scndir;
}

int GUI_SetScnDir(__u8  scn_dir)
{
    gui_scndir = scn_dir;
    return ORANGE_OK;
}

int GUI_SetZoomWin(int src_w,int src_h,int dst_w,int dst_h)
{
    gui_zoom_rect.src_w = src_w;
    gui_zoom_rect.src_h = src_h;
    gui_zoom_rect.dst_w = dst_w;
    gui_zoom_rect.dst_h = dst_h;

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_CharSetToEncode
*
* author:
*
* date:             2009-11-17
*
* Description:      gui charsettoencode
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_CharSetToEncode(__s32 charset_enm)
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
*                                               GUI_DirMemcpy
*
* author:
*
* date:             2010-3-22
*
* Description:      gui dirmemcpy
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_DirMemcpy(__gui_blitinfo_t *blitinfo)
{
    __u32       osize     = 0;
    __u32       bpp;
    __u32       srcstride   = 0;
    __u32       dststride   = 0;
    void        *osrcaddr;
    void        *odstaddr;
    __u32       i;
    __pos_t     src_start;
    //__pos_t     src_end;
    __pos_t     dst_start;
    __pos_t     dst_end;
    __u32       fb_width;
    __u32       fb_height;
    __u32       cpy_height;
    __u32       srcfb_width;
    __u32       srcfb_height;
    bpp = blitinfo->bpp;

    if (gui_scndir == GUI_SCNDIR_NORMAL)
    {
        fb_width        = blitinfo->dst_width;
        fb_height       = blitinfo->dst_height;
        srcfb_width     = blitinfo->src_width;
        srcfb_height    = blitinfo->src_height;
        /*Ó³ÉäÔ´ÆðÊ¼×ø±ê*/
        src_start.x     = blitinfo->srcstart.x;
        src_start.y     = blitinfo->srcstart.y;
        /*Ó³ÉäÔ´½áÊø×ø±ê*/
        //src_end.x       = blitinfo->srcend.x;
        //src_end.y       = blitinfo->srcend.y;
        /*Ó³ÉäÄ¿±êÆðÊ¼×ø±ê*/
        dst_start.x     = blitinfo->dststart.x;
        dst_start.y     = blitinfo->dststart.y;
        /*Ó³ÉäÄ¿±ê½áÊø×ø±ê*/
        dst_end.x       = blitinfo->dstend.x;
        dst_end.y       = blitinfo->dstend.x;
        cpy_height      = blitinfo->dstend.y - blitinfo->dststart.y;
    }
    else if (gui_scndir == GUI_SCNDIR_ROTATE90)
    {
        fb_width        = blitinfo->dst_height;
        fb_height       = blitinfo->dst_width;
        srcfb_width     = blitinfo->src_height;
        srcfb_height    = blitinfo->src_width;
        /*Ó³ÉäÔ´ÆðÊ¼×ø±ê*/
        src_start.x     = blitinfo->src_width - blitinfo->srcend.y;
        src_start.y     = blitinfo->srcstart.x;
        /*Ó³ÉäÔ´½áÊø×ø±ê*/
        //src_end.x       = blitinfo->src_width - blitinfo->srcstart.y;
        //src_end.y       = blitinfo->srcend.x;
        /*Ó³ÉäÄ¿±êÆðÊ¼×ø±ê*/
        dst_start.x     = fb_height - blitinfo->dststart.y;
        dst_start.y     = blitinfo->dstend.x;
        /*Ó³ÉäÄ¿±ê½áÊø×ø±ê*/
        dst_end.x       = fb_height - blitinfo->dststart.y;
        dst_end.y       = blitinfo->dstend.x;
        cpy_height      = blitinfo->dstend.x - blitinfo->dststart.x;
    }
    else if (gui_scndir == GUI_SCNDIR_ROTATE180)
    {
        fb_width        = blitinfo->dst_width;
        fb_height       = blitinfo->dst_height;
        srcfb_width     = blitinfo->src_width;
        srcfb_height    = blitinfo->src_height;
        /*Ó³ÉäÔ´ÆðÊ¼×ø±ê*/
        src_start.x     = srcfb_width  - blitinfo->srcend.x;
        src_start.y     = srcfb_height - blitinfo->srcend.y;
        /*Ó³ÉäÔ´½áÊø×ø±ê*/
        //src_end.x       = srcfb_width  - blitinfo->srcstart.x;
        //src_end.y       = srcfb_height - blitinfo->srcstart.y;
        /*Ó³ÉäÔ´ÆðÊ¼×ø±ê*/
        dst_start.x     = fb_width  - blitinfo->dstend.x;
        dst_start.y     = fb_height - blitinfo->dstend.y;
        /*Ó³ÉäÔ´½áÊø×ø±ê*/
        dst_end.x       = fb_width  - blitinfo->dststart.x;
        dst_end.y       = fb_height - blitinfo->dststart.y;
        cpy_height      = blitinfo->dstend.y - blitinfo->dststart.y;
    }
    else
    {
        fb_width        = blitinfo->dst_height;
        fb_height       = blitinfo->dst_width;
        srcfb_width     = blitinfo->src_height;
        srcfb_height    = blitinfo->src_width;
        /*Ó³ÉäÔ´ÆðÊ¼×ø±ê*/
        src_start.x     = blitinfo->srcstart.y;
        src_start.y     = blitinfo->src_height - blitinfo->srcend.x;
        /*Ó³ÉäÔ´½áÊø×ø±ê*/
        //src_end.x       = blitinfo->srcend.y;
        //src_end.y       = blitinfo->src_height - blitinfo->srcstart.x;
        /*Ó³ÉäÄ¿±êÆðÊ¼×ø±ê*/
        dst_start.x     = blitinfo->dststart.y;
        dst_start.y     = fb_width - blitinfo->dstend.x;
        /*Ó³ÉäÄ¿±ê½áÊø×ø±ê*/
        dst_end.x       = blitinfo->dstend.y;
        dst_end.y       = fb_width - blitinfo->dststart.x;
        cpy_height      = blitinfo->dstend.x - blitinfo->dststart.x;
    }

    //count = (dstend->y - dststart->y)/txtlayer->txtlist->head->rect.height;
    odstaddr = (void *)((unsigned long)blitinfo->dst_addr +
                        ((fb_width * dst_start.y * bpp + 7) / 8) +
                        ((dst_start.x * bpp + 7) / 8));   /*get source address */
    osrcaddr = (void *)((unsigned long)blitinfo->src_addr +
                        ((fb_width * src_start.y * bpp + 7) / 8) +
                        ((src_start.x * bpp + 7) / 8));   /*get source address */
    osize       = ((dst_end.x - dst_start.x) * bpp + 7) >> 3;
    dststride   = (fb_width * bpp + 7) >> 3;
    srcstride   = (srcfb_width * bpp + 7) >> 3;

    for (i = 0; i < cpy_height; i++)
    {
        eLIBs_memcpy(odstaddr, osrcaddr, osize);
        osrcaddr = (void *)((unsigned long)osrcaddr + srcstride);
        odstaddr = (void *)((unsigned long)odstaddr + dststride);
    }
}

/*
**********************************************************************************************************************
*                                               GUI_PhyPt2LogPt
*
* author:
*
* date:             2010-3-20
*
* Description:      gui phypt2logpt
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_PhyPt2LogPt(__u8 direct, GUI_POINT *pPhyPoint, GUI_POINT *pLogPoint)
{
    GUI_LOCK();

    switch (direct)
    {
        case GUI_SCNDIR_NORMAL:
            pLogPoint->x = pPhyPoint->x;
            pLogPoint->y = pPhyPoint->y;
            break;

        case GUI_SCNDIR_ROTATE90:
            pLogPoint->x = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - pPhyPoint->y;
            pLogPoint->y = pPhyPoint->x;
            break;

        case GUI_SCNDIR_ROTATE180:
            pLogPoint->x = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - pPhyPoint->x;
            pLogPoint->y = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - pPhyPoint->y;
            break;

        case GUI_SCNDIR_ROTATE270:
            pLogPoint->x = pPhyPoint->y;
            pLogPoint->y = Screen_Context.LCDYSize[GUI_Context.SelLayer] - pPhyPoint->x - 1;
            break;
    }

    GUI_UNLOCK();
}


/*
**********************************************************************************************************************
*                                               GUI_LogPt2PhyPt
*
* author:
*
* date:             2010-3-20
*
* Description:      gui logpt2phypt
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_LogPt2PhyPt(__u8 direct, GUI_POINT *pLogPoint, GUI_POINT *pPhyPoint)
{
    GUI_LOCK();

    switch (direct)
    {
        case GUI_SCNDIR_NORMAL:
            pPhyPoint->x = pLogPoint->x;
            pPhyPoint->y = pLogPoint->y;
            break;

        case GUI_SCNDIR_ROTATE90:
            pPhyPoint->y = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - pLogPoint->x;
            pPhyPoint->x = pLogPoint->y;
            break;

        case GUI_SCNDIR_ROTATE180:
            pPhyPoint->x = Screen_Context.LCDXSize[GUI_Context.SelLayer] - 1 - pLogPoint->x;
            pPhyPoint->y = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - pLogPoint->y;
            break;

        case GUI_SCNDIR_ROTATE270:
            pPhyPoint->y = pLogPoint->x;
            pPhyPoint->x = Screen_Context.LCDYSize[GUI_Context.SelLayer] - 1 - pLogPoint->y;
            break;
    }

    GUI_UNLOCK();
}

/*
**********************************************************************************************************************
*                                               GUI_PhyPt2LogPt
*
* author:
*
* date:             2010-3-20
*
* Description:      gui phypt2logpt
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ScnPhyPt2LogPt(__u8 direct, GUI_POINT *pPhyPoint, GUI_POINT *pLogPoint)
{
    GUI_LOCK();

    switch (direct)
    {
        case GUI_SCNDIR_NORMAL:
            pLogPoint->x = pPhyPoint->x;
            pLogPoint->y = pPhyPoint->y;
            break;

        case GUI_SCNDIR_ROTATE90:
            pLogPoint->x = g_rcScr.bottom - pPhyPoint->y;
            pLogPoint->y = pPhyPoint->x;
            break;

        case GUI_SCNDIR_ROTATE180:
            pLogPoint->x = g_rcScr.right - pPhyPoint->x;
            pLogPoint->y = g_rcScr.bottom - pPhyPoint->y;
            break;

        case GUI_SCNDIR_ROTATE270:
            pLogPoint->x = pPhyPoint->y;
            pLogPoint->y = g_rcScr.right - pPhyPoint->x;
            break;
    }

    GUI_UNLOCK();
}


/*
**********************************************************************************************************************
*                                               GUI_LogPt2PhyPt
*
* author:
*
* date:             2010-3-20
*
* Description:      gui logpt2phypt
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_ScnLogPt2PhyPt(__u8 direct, GUI_POINT *pLogPoint, GUI_POINT *pPhyPoint)
{
    GUI_LOCK();

    switch (direct)
    {
        case GUI_SCNDIR_NORMAL:
            pPhyPoint->x = pLogPoint->x;
            pPhyPoint->y = pLogPoint->y;
            break;

        case GUI_SCNDIR_ROTATE90:
            pPhyPoint->y = g_rcScr.bottom - pLogPoint->x;
            pPhyPoint->x = pLogPoint->y;
            break;

        case GUI_SCNDIR_ROTATE180:
            pPhyPoint->x = g_rcScr.right - pLogPoint->x;
            pPhyPoint->y = g_rcScr.bottom - pLogPoint->y;
            break;

        case GUI_SCNDIR_ROTATE270:
            pPhyPoint->y = pLogPoint->x;
            pPhyPoint->x = g_rcScr.right - pLogPoint->y;
            break;
    }

    GUI_UNLOCK();
}


/*
**********************************************************************************************************************
*                                               GUI_TouchLogDirToPhyDir
*
* author:
*
* date:             2010-3-25
*
* Description:      gui touchlogdirtophydir
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
/*
__s16  GUI_TouchLogDirToPhyDir(__u8 direct, __s16 LogDir)
{
    __s16        PhyDir;

    switch (direct)
    {
        case GUI_SCNDIR_NORMAL:
            PhyDir = LogDir;
            break;

        case GUI_SCNDIR_ROTATE90:
            switch (LogDir)
            {
                case EV_TP_ACTION_DOWN:                 //´¥ÃþÆÁÏÂ»¬
                    PhyDir = EV_TP_ACTION_RIGHT;
                    break;

                case EV_TP_ACTION_UP:                   //´¥ÃþÆÁÉÏ»¬
                    PhyDir = EV_TP_ACTION_LEFT;
                    break;

                case EV_TP_ACTION_LEFT:                 //´¥ÃþÆÁ×ó»¬
                    PhyDir = EV_TP_ACTION_DOWN;
                    break;

                case EV_TP_ACTION_RIGHT:                //´¥ÃþÆÁÓÒ»¬
                    PhyDir = EV_TP_ACTION_UP;
                    break;

                case EV_TP_ACTION_ANTICLOCKWISE:        //´¥ÃþÆÁÄæÊ±Õë»¬¶¯
                    PhyDir = EV_TP_ACTION_ANTICLOCKWISE;
                    break;

                case EV_TP_ACTION_CLOCKWISE:            //´¥ÃþÆÁË³Ê±Õë»¬¶¯
                    PhyDir = EV_TP_ACTION_CLOCKWISE;
                    break;

                case EV_TP_ACTION_LD2RU:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,×óÏÂµ½ÓÒÉÏ
                    PhyDir = EV_TP_ACTION_RD2LU;
                    break;

                case EV_TP_ACTION_RU2LD:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,ÓÒÉÏµ½×óÏÂ
                    PhyDir = EV_TP_ACTION_LU2RD;
                    break;

                case EV_TP_ACTION_LU2RD:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,×óÉÏµ½ÓÒÏÂ
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;

                case EV_TP_ACTION_RD2LU:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,ÓÒÏÂµ½×óÉÏ
                    PhyDir = EV_TP_ACTION_RU2LD;
                    break;

                default:
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;
            }

            break;

        case GUI_SCNDIR_ROTATE180:
            switch (LogDir)
            {
                case EV_TP_ACTION_DOWN:                 //´¥ÃþÆÁÏÂ»¬
                    PhyDir = EV_TP_ACTION_UP;
                    break;

                case EV_TP_ACTION_UP:                   //´¥ÃþÆÁÉÏ»¬
                    PhyDir = EV_TP_ACTION_DOWN;
                    break;

                case EV_TP_ACTION_LEFT:                 //´¥ÃþÆÁ×ó»¬
                    PhyDir = EV_TP_ACTION_RIGHT;
                    break;

                case EV_TP_ACTION_RIGHT:                //´¥ÃþÆÁÓÒ»¬
                    PhyDir = EV_TP_ACTION_LEFT;
                    break;

                case EV_TP_ACTION_ANTICLOCKWISE:        //´¥ÃþÆÁÄæÊ±Õë»¬¶¯
                    PhyDir = EV_TP_ACTION_ANTICLOCKWISE;
                    break;

                case EV_TP_ACTION_CLOCKWISE:            //´¥ÃþÆÁË³Ê±Õë»¬¶¯
                    PhyDir = EV_TP_ACTION_CLOCKWISE;
                    break;

                case EV_TP_ACTION_LD2RU:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,×óÏÂµ½ÓÒÉÏ
                    PhyDir = EV_TP_ACTION_RU2LD;
                    break;

                case EV_TP_ACTION_RU2LD:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,ÓÒÉÏµ½×óÏÂ
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;

                case EV_TP_ACTION_LU2RD:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,×óÉÏµ½ÓÒÏÂ
                    PhyDir = EV_TP_ACTION_RD2LU;
                    break;

                case EV_TP_ACTION_RD2LU:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,ÓÒÏÂµ½×óÉÏ
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;

                default:
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;
            }

            break;

        case GUI_SCNDIR_ROTATE270:
            switch (LogDir)
            {
                case EV_TP_ACTION_DOWN:                 //´¥ÃþÆÁÏÂ»¬
                    PhyDir = EV_TP_ACTION_LEFT;
                    break;

                case EV_TP_ACTION_UP:                   //´¥ÃþÆÁÉÏ»¬
                    PhyDir = EV_TP_ACTION_RIGHT;
                    break;

                case EV_TP_ACTION_LEFT:                 //´¥ÃþÆÁ×ó»¬
                    PhyDir = EV_TP_ACTION_UP;
                    break;

                case EV_TP_ACTION_RIGHT:                //´¥ÃþÆÁÓÒ»¬
                    PhyDir = EV_TP_ACTION_DOWN;
                    break;

                case EV_TP_ACTION_ANTICLOCKWISE:        //´¥ÃþÆÁÄæÊ±Õë»¬¶¯
                    PhyDir = EV_TP_ACTION_ANTICLOCKWISE;
                    break;

                case EV_TP_ACTION_CLOCKWISE:            //´¥ÃþÆÁË³Ê±Õë»¬¶¯
                    PhyDir = EV_TP_ACTION_CLOCKWISE;
                    break;

                case EV_TP_ACTION_LD2RU:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,×óÏÂµ½ÓÒÉÏ
                    PhyDir = EV_TP_ACTION_LU2RD;
                    break;

                case EV_TP_ACTION_RU2LD:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,ÓÒÉÏµ½×óÏÂ
                    PhyDir = EV_TP_ACTION_RD2LU;
                    break;

                case EV_TP_ACTION_LU2RD:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,×óÉÏµ½ÓÒÏÂ
                    PhyDir = EV_TP_ACTION_RU2LD;
                    break;

                case EV_TP_ACTION_RD2LU:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,ÓÒÏÂµ½×óÉÏ
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;

                default:
                    PhyDir = EV_TP_ACTION_LD2RU;
                    break;
            }

            break;

        default:
            PhyDir = LogDir;
            break;
    }

    return   PhyDir;
}
*/
/*
**********************************************************************************************************************
*                                               GUI_TouchPhyDirToLogDir
*
* author:
*
* date:             2010-3-25
*
* Description:      gui touchphydirtologdir
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s16  GUI_TouchPhyDirToLogDir(__u8 direct, __s16 PhyDir)
{
    __s16        LogDir;

    switch (direct)
    {
        case GUI_SCNDIR_NORMAL:
            LogDir = PhyDir;
            break;

        case GUI_SCNDIR_ROTATE90:
            switch (PhyDir)
            {
                case EV_TP_ACTION_DOWN:                 //´¥ÃþÆÁÏÂ»¬
                    LogDir = EV_TP_ACTION_LEFT;
                    break;

                case EV_TP_ACTION_UP:                   //´¥ÃþÆÁÉÏ»¬
                    LogDir = EV_TP_ACTION_RIGHT;
                    break;

                case EV_TP_ACTION_LEFT:                 //´¥ÃþÆÁ×ó»¬
                    LogDir = EV_TP_ACTION_UP;
                    break;

                case EV_TP_ACTION_RIGHT:                //´¥ÃþÆÁÓÒ»¬
                    LogDir = EV_TP_ACTION_DOWN;
                    break;

                case EV_TP_ACTION_ANTICLOCKWISE:        //´¥ÃþÆÁÄæÊ±Õë»¬¶¯
                    LogDir = EV_TP_ACTION_ANTICLOCKWISE;
                    break;

                case EV_TP_ACTION_CLOCKWISE:            //´¥ÃþÆÁË³Ê±Õë»¬¶¯
                    LogDir = EV_TP_ACTION_CLOCKWISE;
                    break;

                case EV_TP_ACTION_LD2RU:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,×óÏÂµ½ÓÒÉÏ
                    LogDir = EV_TP_ACTION_LU2RD;
                    break;

                case EV_TP_ACTION_RU2LD:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,ÓÒÉÏµ½×óÏÂ
                    LogDir = EV_TP_ACTION_RD2LU;
                    break;

                case EV_TP_ACTION_LU2RD:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,×óÉÏµ½ÓÒÏÂ
                    LogDir = EV_TP_ACTION_RU2LD;
                    break;

                case EV_TP_ACTION_RD2LU:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,ÓÒÏÂµ½×óÉÏ
                    LogDir = EV_TP_ACTION_LD2RU;
                    break;

                default:
                    LogDir = EV_TP_ACTION_RU2LD;
                    break;
            }

            break;

        case GUI_SCNDIR_ROTATE180:
            switch (PhyDir)
            {
                case EV_TP_ACTION_DOWN:                 //´¥ÃþÆÁÏÂ»¬
                    LogDir = EV_TP_ACTION_UP;
                    break;

                case EV_TP_ACTION_UP:                   //´¥ÃþÆÁÉÏ»¬
                    LogDir = EV_TP_ACTION_DOWN;
                    break;

                case EV_TP_ACTION_LEFT:                 //´¥ÃþÆÁ×ó»¬
                    LogDir = EV_TP_ACTION_RIGHT;
                    break;

                case EV_TP_ACTION_RIGHT:                //´¥ÃþÆÁÓÒ»¬
                    LogDir = EV_TP_ACTION_LEFT;
                    break;

                case EV_TP_ACTION_ANTICLOCKWISE:        //´¥ÃþÆÁÄæÊ±Õë»¬¶¯
                    LogDir = EV_TP_ACTION_ANTICLOCKWISE;
                    break;

                case EV_TP_ACTION_CLOCKWISE:            //´¥ÃþÆÁË³Ê±Õë»¬¶¯
                    LogDir = EV_TP_ACTION_CLOCKWISE;
                    break;

                case EV_TP_ACTION_LD2RU:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,×óÏÂµ½ÓÒÉÏ
                    LogDir = EV_TP_ACTION_RU2LD;
                    break;

                case EV_TP_ACTION_RU2LD:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,ÓÒÉÏµ½×óÏÂ
                    LogDir = EV_TP_ACTION_LD2RU;
                    break;

                case EV_TP_ACTION_LU2RD:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,×óÉÏµ½ÓÒÏÂ
                    LogDir = EV_TP_ACTION_RD2LU;
                    break;

                case EV_TP_ACTION_RD2LU:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,ÓÒÏÂµ½×óÉÏ
                    LogDir = EV_TP_ACTION_LD2RU;
                    break;

                default:
                    LogDir = EV_TP_ACTION_RU2LD;
                    break;
            }

            break;

        case GUI_SCNDIR_ROTATE270:
            switch (PhyDir)
            {
                case EV_TP_ACTION_DOWN:                 //´¥ÃþÆÁÏÂ»¬
                    LogDir = EV_TP_ACTION_RIGHT;
                    break;

                case EV_TP_ACTION_UP:                   //´¥ÃþÆÁÉÏ»¬
                    LogDir = EV_TP_ACTION_LEFT;
                    break;

                case EV_TP_ACTION_LEFT:                 //´¥ÃþÆÁ×ó»¬
                    LogDir = EV_TP_ACTION_DOWN;
                    break;

                case EV_TP_ACTION_RIGHT:                //´¥ÃþÆÁÓÒ»¬
                    LogDir = EV_TP_ACTION_UP;
                    break;

                case EV_TP_ACTION_ANTICLOCKWISE:        //´¥ÃþÆÁÄæÊ±Õë»¬¶¯
                    LogDir = EV_TP_ACTION_ANTICLOCKWISE;
                    break;

                case EV_TP_ACTION_CLOCKWISE:            //´¥ÃþÆÁË³Ê±Õë»¬¶¯
                    LogDir = EV_TP_ACTION_CLOCKWISE;
                    break;

                case EV_TP_ACTION_LD2RU:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,×óÏÂµ½ÓÒÉÏ
                    LogDir = EV_TP_ACTION_RD2LU;
                    break;

                case EV_TP_ACTION_RU2LD:                //´¥ÃþÆÁÐ±Ïß»¬¶¯,ÓÒÉÏµ½×óÏÂ
                    LogDir = EV_TP_ACTION_LU2RD;
                    break;

                case EV_TP_ACTION_LU2RD:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,×óÉÏµ½ÓÒÏÂ
                    LogDir = EV_TP_ACTION_LD2RU;
                    break;

                case EV_TP_ACTION_RD2LU:                //´¥ÃþÆÁ·´Ð±Ïß»¬¶¯,ÓÒÏÂµ½×óÉÏ
                    LogDir = EV_TP_ACTION_RU2LD;
                    break;

                default:
                    LogDir = EV_TP_ACTION_RU2LD;
                    break;
            }

            break;

        default:
            LogDir = PhyDir;
            break;
    }

    return   LogDir;
}

/*
**********************************************************************************************************************
*                                               GUI_GetScreenSize
*
* author:
*
* date:             2010-3-29
*
* Description:      gui getscreensize
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_GetScreenSize(__s32 *p_width, __s32 *p_height)
{
__mp *mp_display = NULL;
#if GUI_CHECK_ARG_EN > 0

    if ((!p_width) || (!p_height))
    {
        ORANGE_WRN("parameter empty!\n");
        return ORANGE_FAIL;
    }

#endif
	mp_display = esKSRV_Get_Display_Hld();
    *p_width  = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_WIDTH, SEL_SCREEN, 0); //modified by Derek,2010.12.07.15:05
    *p_height = esMODS_MIoctrl(mp_display, MOD_DISP_GET_SCN_HEIGHT, SEL_SCREEN, 0); //modified by Derek,2010.12.07.15:05

    if ((gui_scndir == GUI_SCNDIR_ROTATE90) ||
        (gui_scndir == GUI_SCNDIR_ROTATE270))
    {
        *p_width = *p_height;
        *p_height = *p_width;
    }
__log("get size w: %d    h: %d",*p_width,*p_height);

    return EPDK_OK;
}
