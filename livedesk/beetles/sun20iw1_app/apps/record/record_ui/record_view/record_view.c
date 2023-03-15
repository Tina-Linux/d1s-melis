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
#include "record_view.h"
#include "record_comm.h"
static void record_view_updateBottom(precord_view_t prec_view)
{
#if !RECORD_CHANGE_0815
    FOCUSE_STATE menuList[MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_PRESS, FOCUSE_NORMAL, FOCUSE_NORMAL};
#else
    FOCUSE_STATE menuList[MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_PRESS, FOCUSE_NORMAL};
#endif
    GUI_RECT rect = {380, 442, 424, 471};
    GUI_COLOR txt_color;
    menuList[prec_view->curMenu] = prec_view->curMenuSt;
    draw_record_bmp(prec_view->pui, RECORD_BAR_BOTTOM_BMP);
#if !RECORD_CHANGE_0815
    draw_record_mapping(prec_view->pui, MENU_BACK, menuList[MENU_BACK], 0);
    draw_record_mapping(prec_view->pui, MENU_QUALITY,    menuList[MENU_QUALITY], 0);
#endif

    if (/* prec_view->curMenuSt == FOCUSE_PRESS && */prec_view->curMenu == MENU_RECORD)
    {
        txt_color = GUI_RED;
    }
    else
    {
        txt_color = GUI_GRAY;
    }

    /*if( prec_view->precord_doc->recState == RECORD_STOP )
    {
        draw_record_bmp_ext( prec_view->pui, RECORD_MENU_RECORD_STOP, 0, 0 );
        draw_record_lang_ext( prec_view->pui, STRING_MENU_STOP, txt_color, &rect );
    }
    else */
    if (prec_view->precord_doc->recState == RECORD_START)
    {
        draw_record_bmp_ext(prec_view->pui, RECORD_MENU_RECORD_START, 0, 0);
        draw_record_lang_ext(prec_view->pui, STRING_MENU_PAUSE, txt_color, &rect);
    }
    else
    {
        draw_record_bmp_ext(prec_view->pui, RECORD_MENU_RECORD_PAUSE, 0, 0);
        draw_record_lang_ext(prec_view->pui, STRING_MENU_RECORD, txt_color, &rect);
    }

    draw_record_mapping(prec_view->pui, MENU_SAVE,       menuList[MENU_SAVE], 0);
    draw_record_mapping(prec_view->pui, MENU_MUSIC_LIST, menuList[MENU_MUSIC_LIST], 0);
    //..draw_record_mapping( prec_view->pui, MENU_FORMAT, menuList[MENU_FORMAT], 0 );
}

static void record_view_Bottom(precord_view_t prec_view)
{
#if !RECORD_CHANGE_0815
    FOCUSE_STATE menuList[MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_NORMAL, FOCUSE_PRESS, FOCUSE_NORMAL, FOCUSE_NORMAL};
#else
    FOCUSE_STATE menuList[MENU_MAX] = {FOCUSE_NORMAL, FOCUSE_PRESS, FOCUSE_NORMAL};
#endif
    GUI_RECT rect = {380, 442, 424, 471};
    GUI_COLOR txt_color;
    menuList[prec_view->curMenu] = prec_view->curMenuSt;
    //  draw_record_bmp(prec_view->pui, RECORD_BAR_BOTTOM_BMP);
#if !RECORD_CHANGE_0815
    draw_record_mapping(prec_view->pui, MENU_BACK, menuList[MENU_BACK], 0);
    draw_record_mapping(prec_view->pui, MENU_QUALITY,    menuList[MENU_QUALITY], 0);
#endif

    if (/* prec_view->curMenuSt == FOCUSE_PRESS && */prec_view->curMenu == MENU_RECORD)
    {
        txt_color = GUI_RED;
    }
    else
    {
        txt_color = GUI_GRAY;
    }

    /*if( prec_view->precord_doc->recState == RECORD_STOP )
    {
        draw_record_bmp_ext( prec_view->pui, RECORD_MENU_RECORD_STOP, 0, 0 );
        draw_record_lang_ext( prec_view->pui, STRING_MENU_STOP, txt_color, &rect );
    }

    else */if (prec_view->precord_doc->recState == RECORD_START)
    {
        draw_record_bmp_ext(prec_view->pui, RECORD_MENU_RECORD_START, 0, 0);
        draw_record_lang_ext(prec_view->pui, STRING_MENU_PAUSE, txt_color, &rect);
    }
    else
    {
        draw_record_bmp_ext(prec_view->pui, RECORD_MENU_RECORD_PAUSE, 0, 0);
        draw_record_lang_ext(prec_view->pui, STRING_MENU_RECORD, txt_color, &rect);
    }

    draw_record_mapping(prec_view->pui, MENU_SAVE,       menuList[MENU_SAVE], 0);
    draw_record_mapping(prec_view->pui, MENU_MUSIC_LIST, menuList[MENU_MUSIC_LIST], 0);
    //..draw_record_mapping( prec_view->pui, MENU_FORMAT, menuList[MENU_FORMAT], 0 );
}

static void record_view_recInfo(precord_view_t prec_view)
{
    char curRecTimes[10] = {0};
    GUI_RECT rectName = {124, 121, 344, 153};
    GUI_RECT rectSize = {435, 250, 565, 282};
    GUI_RECT rectQuty = {246, 250, 348, 282};
    __s16 offsetX = 0, offsetY = 0;
    //可录时间
    //draw_record_lang( prec_view->pui, STRING_RECORD_TIME, APP_COLOR_WHITE);
    GUI_SetColor(GUI_WHITE);
    /*if( prec_view->precord_doc->diskFreeTime >= 100*3600 )
    {
        GUI_DispStringAt( ">99:59:59", 110, 29 );
    }
    else
    {
        record_s_to_char(curRecTimes, prec_view->precord_doc->diskFreeTime, ':');
        GUI_DispStringAt( curRecTimes, 110, 29 );
    }*///..
    //已录时间00:00:01
    __wrn("prec_view->precord_doc->recordCurTimes=%d\n", prec_view->precord_doc->recordCurTimes);
    record_ms_to_char(curRecTimes, prec_view->precord_doc->recordCurTimes, ':');
    draw_record_bmp_ext(prec_view->pui, curRecTimes[0] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 44;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[1] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 44;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[2] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 16;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[3] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 44;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[4] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 44;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[5] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 16;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[6] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 44;
    draw_record_bmp_ext(prec_view->pui, curRecTimes[7] - '0' + RECORD_0_BMP, offsetX, offsetY);
    offsetX += 44;
    //录制文件信息
    draw_record_lang_ext(prec_view->pui, prec_view->precord_doc->recQuality + STRING_POOR_QUALITY, GUI_WHITE, &rectQuty);
    //..draw_record_str( prec_view->pui, prec_view->precord_doc->recordName, APP_COLOR_WHITE, &rectName );
    eLIBs_int2str_dec(prec_view->precord_doc->fileSize, curRecTimes);
    eLIBs_strcat(curRecTimes, " KB");
    draw_record_str(prec_view->pui, curRecTimes, GUI_WHITE, &rectSize);
}

/*static */void record_view_menuBox(precord_view_t prec_view)
{
    if (prec_view->curMenuSt == FOCUSE_PRESS)
    {
        switch (prec_view->curMenu)
        {
#if !RECORD_CHANGE_0815

            case MENU_QUALITY:
                draw_record_bmp(prec_view->pui, RECORD_QUALITY_BK_BMP);
                draw_record_bmp_ext(prec_view->pui, RECORD_QUALITY_FLAG_BMP, 0, prec_view->precord_doc->recQualityTemp * -39);

                if (prec_view->precord_doc->recQualityTemp == 0)
                {
                    draw_record_lang(prec_view->pui, STRING_POOR_QUALITY,  GUI_RED);
                    draw_record_lang(prec_view->pui, STRING_NORMAL_QUALITY, GUI_BLACK);
                    draw_record_lang(prec_view->pui, STRING_GOOD_QUALITY, GUI_BLACK);
                }
                else if (prec_view->precord_doc->recQualityTemp == 1)
                {
                    draw_record_lang(prec_view->pui, STRING_POOR_QUALITY, GUI_BLACK);
                    draw_record_lang(prec_view->pui, STRING_NORMAL_QUALITY, GUI_RED);
                    draw_record_lang(prec_view->pui, STRING_GOOD_QUALITY, GUI_BLACK);
                }
                else
                {
                    draw_record_lang(prec_view->pui, STRING_POOR_QUALITY, GUI_BLACK);
                    draw_record_lang(prec_view->pui, STRING_NORMAL_QUALITY, GUI_BLACK);
                    draw_record_lang(prec_view->pui, STRING_GOOD_QUALITY, GUI_RED);
                }

                break;
#endif

            case MENU_RECORD:
                break;

            case MENU_SAVE:
                break;

            case MENU_MUSIC_LIST:
                break;
#if 0

            case MENU_FORMAT:
                draw_record_bmp_ext(prec_view->pui, RECORD_FORMAT_BK_BMP/*RECORD_QUALITY_BK_BMP*/, 0, 0);
                draw_record_bmp_ext(prec_view->pui, RECORD_QUALITY_FLAG_BMP, 314, prec_view->precord_doc->audioTypeTemp * -27);

                if (prec_view->precord_doc->audioTypeTemp == 0)
                {
                    GUI_SetColor(APP_COLOR_WHITE);
                    //GUI_DispStringAt( "WMA", 410, 150 );
                    GUI_DispStringAt("WAV", 410, 175);
                    GUI_SetColor(APP_COLOR_YELLOW);
                    GUI_DispStringAt("MP3", 410, 200);
                }
                else if (prec_view->precord_doc->audioTypeTemp == 1)
                {
                    GUI_SetColor(APP_COLOR_WHITE);
                    //GUI_DispStringAt( "WMA", 410, 150 );
                    GUI_DispStringAt("MP3", 410, 200);
                    GUI_SetColor(APP_COLOR_YELLOW);
                    GUI_DispStringAt("WAV", 410, 175);
                }
                else
                {
                    GUI_SetColor(APP_COLOR_WHITE);
                    GUI_DispStringAt("WAV", 410, 175);
                    GUI_DispStringAt("MP3", 410, 200);
                    GUI_SetColor(APP_COLOR_YELLOW);
                    //GUI_DispStringAt( "WMA", 328, 150 );
                }

                break;
#endif

            default:
                break;
        }

        /*clean the menubox*/
#if !RECORD_CHANGE_0815

        if (prec_view->curMenu != MENU_QUALITY)
        {
            draw_record_bmp(prec_view->pui, RECORD_QUALITY_BK1_BMP);
        }

#endif
    }
}

static void record_view_rec_ani(precord_view_t prec_view)
{
    //__u16 fp = 0, i, aniCnt = RECORD_ANI_4_BMP;
    //__s16 aniX = -16,aniY = 156;
    draw_record_bmp(prec_view->pui, RECORD_BG);
    draw_record_bmp(prec_view->pui, RECORD_ANI_BG_BMP);  //..
    //fp = prec_view->precord_doc->spctrmLevel[0]/100;
    //if( fp > 25 )
    //  fp = 25;
    //aniX += 19*fp;
    //for( i = fp; i >0 && i <= 25; i-- )
    //{
    //  draw_record_bmp_coor( prec_view->pui, aniCnt, aniX, aniY );
    //  if( aniCnt > RECORD_ANI_0_BMP )
    //      aniCnt -= 1;
    //  aniX -= 19;
    //}
}
static void record_view1_rec_ani(precord_view_t prec_view)
{
    __u16 fp = 0, i, aniCnt = RECORD_ANI_7_BMP;
    __s16 aniX = 212, aniY = 287;
    draw_record_bmp(prec_view->pui, RECORD_BG0);
    draw_record_bmp(prec_view->pui, RECORD_ANI_BG_BMP);
    fp = prec_view->precord_doc->spctrmLevel[0] / 100;

    if (fp > 20)
    {
        fp = 20;
    }

    aniX += 19 * fp;

    for (i = fp; i > 0 && i <= 20; i--)
    {
        draw_record_bmp_coor(prec_view->pui, aniCnt, aniX, aniY);

        if (aniCnt > RECORD_ANI_0_BMP)
        {
            aniCnt -= 1;
        }

        aniX -= 19;
    }
}


static void record_view_rec_nofiles(precord_view_t prec_view)
{
    if (prec_view->precord_doc->timeCnt < NO_FILES_TIMES)
    {
        draw_record_lang(prec_view->pui, STRING_MSG3, APP_COLOR_WHITE);
    }
}

void record_view_show(precord_view_t prec_view, H_WIN layer)
{
    __s16 x1, y1;
    __s16 x0 = 0, y0 = 0, w = 800, h = 480;

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    GUI_SetBkColor(0x00);
    GUI_Clear();
    record_view_rec_ani(prec_view);
    record_view_recInfo(prec_view);
    record_view_Bottom(prec_view);
    record_view_menuBox(prec_view);
    record_view_rec_nofiles(prec_view);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
}
void record_updateview_show(precord_view_t prec_view, H_WIN layer)
{
    GUI_MEMDEV_Handle   draw_mem;
    __s16 x1, y1;
    __s16 x0 = 0, y0 = 362, w = 800, h = 118;

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

    //__here__;
    if (draw_mem == 0)
    {
        __log("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
    }

    if (draw_mem)
    {
        GUI_MEMDEV_Select(draw_mem);
    }
    else
    {
        //GUI_LyrWinSetSta(layer,GUI_LYRWIN_STA_OFF);
    }

    GUI_SetBkColor(0x00);
    //GUI_ClearRect(x0, y0, x1,  y1);
    //..GUI_Clear();
    //record_view_menuBox(prec_view);
    record_view_updateBottom(prec_view);
    record_view_rec_nofiles(prec_view);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    if (draw_mem)
    {
        GUI_MEMDEV_CopyToLCD(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
    }
    else
    {
        //GUI_LyrWinSetSta(layer,GUI_LYRWIN_STA_ON);
    }

    __wrn("pviewFrm_ctl->pview->curMenu====%d\n", prec_view->curMenu);
    __wrn("prec_view->curMenuSt ======%d\n", prec_view->curMenuSt);
    record_view_menuBox(prec_view);
}
void record_timer_view_show(precord_view_t prec_view, H_WIN layer)
{
    GUI_MEMDEV_Handle   draw_mem;
    __s16 x1, y1;
    //__s16 x0 = 248, y0 =170, w = 323, h = 116;
    __s16 x0 = 230, y0 = 170, w = 360, h = 170;

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    x1 = w + x0 - 1;
    y1 = h + y0 - 1;
    GUI_LyrWinSel(layer);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w,  h);

    //__here__;
    if (draw_mem == 0)
    {
        __log("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& \n");
    }

    if (draw_mem)
    {
        GUI_MEMDEV_Select(draw_mem);
    }
    else
    {
        //GUI_LyrWinSetSta(layer,GUI_LYRWIN_STA_OFF);
    }

    GUI_SetBkColor(0x00);
    //GUI_ClearRect(x0, y0, x1,  y1);
    //..GUI_Clear();
    //..record_view_rec_ani(prec_view);
    record_view1_rec_ani(prec_view);
    record_view_recInfo(prec_view);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    if (draw_mem)
    {
        GUI_MEMDEV_CopyToLCD(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
    }
    else
    {
        //GUI_LyrWinSetSta(layer,GUI_LYRWIN_STA_ON);
    }
}


