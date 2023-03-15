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
#include "beetles_app.h"

#include "music.h"
#include "music_ui.h"
#include "music_list.h"
#include "music_player.h"

#include "music_lyrics.h"


/***************************************************************************************************
*Name        : __bool music_lyrics_exist(void)
*Prototype   :
*Arguments   : void
*Return      :  EPDK_TRUE      there are lyrics corresponding to the current playing file.
*               EPDK_FALSE     there is NO lyric corresponding to the current playing file.
*Description : check whether there are lyrics corresponding to the current playing file or not.
*Other       :
***************************************************************************************************/

__bool music_lyrics_exist(void)
{
    RECT rect;
    dsk_display_get_size(&rect.width, &rect.height);
    GUI_SetColor(0xFF2B2E36);

    if (rect.width == 800 && rect.height == 480)
    {
        GUI_FillRect(350, 400, 350 + 219 + 150, 400 + 125 - 51);
    }
    else
    {
        GUI_FillRect(350 - 80, 400, 350 + 219 + 150 - 80, 400 + 125 - 51);
    }

    return robin_lyric_exist();
}

static __epdk_charset_enm_e  _EncodingMap(__s32 cedar_encode)
{
    switch (cedar_encode)
    {
        case CEDAR_SUB_ENCODE_UTF8:
        case CEDAR_SUB_ENCODE_ANSI:
            return EPDK_CHARSET_ENM_UTF8;

        case CEDAR_SUB_ENCODE_GB2312:
            return EPDK_CHARSET_ENM_GBK;

        case CEDAR_SUB_ENCODE_UTF16LE:
            return EPDK_CHARSET_ENM_UTF16LE;

        case CEDAR_SUB_ENCODE_UTF16BE:
            return EPDK_CHARSET_ENM_UTF16BE;

        case CEDAR_SUB_ENCODE_BIG5:
            return EPDK_CHARSET_ENM_BIG5;

        case CEDAR_SUB_ENCODE_GBK:
            return EPDK_CHARSET_ENM_GBK;

        default:
            return EPDK_CHARSET_ENM_GBK;
            //return SPSC_GetSysDefaultCharset();//dsk_get_isn_charset();//112357接口返回错误
    }
}

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


/***************************************************************************************************
*Name       :  robin_get_lyric_item
*Prototype :    __cedar_subtitle_item_t  robin_get_lyric_item( __cedar_subtitle_item_t  *lyric_item_list, __u32 time )
*Arguments:
*time        :  input. the current time
*item_count: total number of item list
*Return     :   string of the correspond item
*
*Description:   get lyric item point
*Other       :
***************************************************************************************************/
#define  C_BACKWARD_ITEM_COUNT      5           //向后搜索时每次倒退5个条目

__cedar_subtitle_item_t  *robin_get_lyric_item(__cedar_subtitle_item_t  *lyric_item_list, __u32 item_count, __u32 time)
{
    __u32 i = 0;
    __u32 out_cnt = 0;

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    for (i = 0; i < item_count; i++)
    {
        if (time >= lyric_item_list[i].uPts)
        {
            if (time < lyric_item_list[i].uPts + lyric_item_list[i].uDuration)
            {
                return &(lyric_item_list[i]);
            }
        }
    }

    return NULL;
}

static __s32 music_match_charset(char *filename, __epdk_charset_enm_e charset)
{
    __s32 file_size;
    char *pbuf;
    ES_FILE *fp;
    __s32 i;
    __s32 readnum;
    __u16 uni;
    __s32 len;
    __s32 ret;
    char *p;
    char *plrc;
    __s32 bok;
    __wrn("try music_match_charset: charset=%d\n", charset);

    if (EPDK_CHARSET_ENM_GBK == charset
        || EPDK_CHARSET_ENM_BIG5 == charset)
    {
    }
    else
    {
        __wrn("charset invalid...\n");
        return EPDK_FAIL;
    }

    bok = EPDK_OK;
    fp = NULL;
    pbuf = NULL;
    file_size = 0;
    fp = eLIBs_fopen(filename, "rb");

    if (NULL == fp)
    {
        __wrn("open file fail:%s\n", filename);
        bok = EPDK_FAIL;
        goto out;
    }

    eLIBs_fseek(fp, 0, SEEK_END);
    file_size = eLIBs_ftell(fp);
    eLIBs_fseek(fp, 0, SEEK_SET);
    pbuf = esMEMS_Balloc(file_size + 1);

    if (NULL == pbuf)
    {
        __wrn("mem not enough...\n");
        bok = EPDK_FAIL;
        goto out;
    }

    do
    {
        plrc = NULL;
        eLIBs_memset(pbuf, 0, file_size + 1);
        readnum = eLIBs_fread(pbuf, 1, file_size, fp);

        if (file_size != readnum)
        {
            bok = EPDK_FAIL;
            goto out;
        }

        p = pbuf;

        while (1)
        {
            p = eLIBs_strchr(p, '[');

            if (NULL == p || p >= pbuf + file_size - 1)
            {
                break;
            }

            if (*(p + 1) >= 0x30 && *(p + 1) <= 0x39)
            {
                plrc = p;
                readnum -= (plrc - pbuf);
                break;
            }
            else
            {
                p++;
            }
        }

        if (NULL == plrc)
        {
            bok = EPDK_FAIL;
            goto out;
        }

        for (i = 0 ; i < readnum ;)
        {
            if (EPDK_CHARSET_ENM_GBK == charset
                || EPDK_CHARSET_ENM_BIG5 == charset)
            {
                if ((__u8)(plrc[i]) >= 0x80)
                {
                    if (i >= readnum - 1)
                    {
                        goto out;
                    }

                    len = 2;
                }
                else
                {
                    len = 1;
                }
            }
            else
            {
                __wrn("cahrset invalid...\n");
                bok = EPDK_FAIL;
                goto out;
            }

            ret = esCHS_Char2Uni(charset, plrc + i, len, &uni);

            if (ret <= 0)
            {
                __wrn("str[i]=%x\n", plrc[i]);

                if (len >= 2)
                {
                    __wrn("str[i+1]=%x\n", plrc[i + 1]);
                }

                bok = EPDK_FAIL;
                goto out;
            }

            i += len;
        }
    } while (0);

out:
    ;

    if (fp)
    {
        eLIBs_fclose(fp);
        fp = NULL;
    }

    if (pbuf && file_size)
    {
        esMEMS_Bfree(pbuf, file_size + 1);
        pbuf = NULL;
        file_size = 0;
    }

    __wrn("music_match_charset ok...\n");
    return bok;
}

H_LYRIC new_music_lyrics(music_lyrics_para_t *para)
{
#if 1
    H_LYRIC hLyric = NULL;
    __s32 i = 0, total = 0;

    //total = robin_get_lyric_item_count();
    while ((total = robin_get_lyric_item_count()) == -1)
    {
        __inf("count = %d\n", total);
        esKRNL_TimeDly(10);
    }

    if (para == NULL)
    {
        return NULL;
    }

    //app_print_memory_usage("new music lyrics", NULL);
    hLyric = (H_LYRIC)esMEMS_Malloc(0, sizeof(music_lyrics_t));         //

    if (hLyric == NULL)
    {
        __msg("memory is not enough for displaying lyrics!\n");
        return NULL;
    }

    eLIBs_memset((void *)hLyric, 0, sizeof(music_lyrics_t));
    hLyric->total_item = total;
    hLyric->items = (__cedar_subtitle_item_t *)esMEMS_Balloc(sizeof(__cedar_subtitle_item_t) * total);
    eLIBs_memset((void *)(hLyric->items), 0, sizeof(__cedar_subtitle_item_t) * total);

    if (hLyric->items == NULL)
    {
        __msg("memory is not enough for displaying lyrics!\n");
        esMEMS_Mfree(0, hLyric);
        return NULL;
    }

    if (para != NULL)
    {
        eLIBs_memcpy((void *)(&(hLyric->UI)), para, sizeof(music_lyrics_para_t));   //UI 参数
    }
    else
    {
        __wrn("there is no parametters!\n");
    }

    robin_get_lyric_item_list(hLyric->items, total);            //获得所有的歌词列表
    hLyric->char_code = EPDK_CHARSET_ENM_UNKNOWN;

    do
    {
        __s32 ret;
        char name[1024] = {0};
        char *p;
        ret = robin_get_fsm_cur_file(name, sizeof(name));

        if (EPDK_FAIL == ret)
        {
            break;
        }

        p = eLIBs_strchrlast(name, '.');

        if (NULL == p)
        {
            break;
        }

        eLIBs_strcpy(p, ".lrc");
        ret = music_match_charset(name, EPDK_CHARSET_ENM_GBK);

        if (EPDK_OK == ret)
        {
            hLyric->char_code = EPDK_CHARSET_ENM_GBK;
            break;
        }

        ret = music_match_charset(name, EPDK_CHARSET_ENM_BIG5);

        if (EPDK_OK == ret)
        {
            hLyric->char_code = EPDK_CHARSET_ENM_BIG5;
            break;
        }
    } while (0);

    if (EPDK_CHARSET_ENM_UNKNOWN == hLyric->char_code)
    {
        hLyric->char_code = _EncodingMap(hLyric->items->uDataType); //获得字体编码
    }

    //GUI_ClearRect(hLyric->UI.area.x,hLyric->UI.area.y,hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);
    {
        __u32 font_height = 0, i = 0;
        __u32 X, Y;
        font_height = GUI_GetFontSizeY();
        hLyric->line_count = hLyric->UI.area.height / font_height;
        //__msg();
        X =  hLyric->UI.area.x;
        Y =  hLyric->UI.area.y;

        for (i = 0; i < hLyric->line_count; i++)
        {
            hLyric->line[i].area.x0 = X;
            hLyric->line[i].area.y0 = Y;
            hLyric->line[i].area.x1 = X + hLyric->UI.area.width;
            hLyric->line[i].area.y1 = Y + font_height;
            Y += font_height;
        }
    }
    return hLyric;
#endif
}

__s32 delete_music_lyrics(H_LYRIC hLyric)
{
#if 1

    if (hLyric == NULL)
    {
        return EPDK_FAIL;
    }

    if (hLyric->total_item != 0)
    {
        esMEMS_Bfree((void *)(hLyric->items), sizeof(__cedar_subtitle_item_t) * hLyric->total_item);
    }

    esMEMS_Mfree(0, hLyric);
    GUI_UC_SetEncodeUTF8();
#endif
    return EPDK_OK;
}

__s32 clear_music_lyrics_area(H_LYRIC hLyric)
{
#if 1

    if (hLyric == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_ClearRect(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);
#endif
    return EPDK_OK;
}

#if 0
//更新显示歌词
__s32 update_music_lyrics(H_LYRIC hLyric)
{
    __u32 time = 0, width = 0;
    __cedar_subtitle_item_t *item = NULL;

    if (hLyric == NULL)
    {
        return EPDK_FAIL;
    }

    time = robin_get_cur_time();

    if (time < 500)
    {
        return EPDK_OK;
    }

    item = robin_get_lyric_item(hLyric->items, hLyric->total_item, time);

    if (item != NULL)
    {
        if (item == hLyric->last_item)
        {
            return EPDK_OK;
        }

        if (item < hLyric->last_item)
        {
            hLyric->hl_count = 0;
        }

        hLyric->last_item = item;

        //char text[128];
        if (hLyric->UI.layer != NULL)               //选择用户图层
        {
            //GUI_LyrWinSel(hLyric->UI.layer);
        }

        //GUI_SetBkColor(0x00000000);
        GUI_SetBkColor(0xB3000000);
        //GUI_SetColor(hLyric->UI.font_color);
        //GUI_ClearRect(area.x0,area.y0, area.x1, area.y1);
        charset_enm2gui(hLyric->char_code);
        //eLIBs_strcpy(text, (char *)(item->pSubtitle));
        //GUI_DispStringInRect(text, &area, GUI_TA_HCENTER|GUI_TA_VCENTER);
        //if(hLyric->line_count >= 5)                   //only for test
        {
            __u32 len = 0, i  = 0;
            GUI_MEMDEV_Handle   draw_mem;
            draw_mem = GUI_MEMDEV_Create(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.width, hLyric->UI.area.height);
            GUI_MEMDEV_Select(draw_mem);

            if (hLyric->hl_count > 2)
            {
                hLyric->hl_count = 0;
            }

            GUI_ClearRect(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);
            width  = hLyric->UI.area.width;

            if (width > 10)
            {
                width -= 10;
            }

            if (hLyric->hl_count == 0)          //从新开始显示歌词
            {
                hLyric->hl_count = 1;
                i = 0;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);

                if (len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                i = 2;
                item ++;//获得下一个条目,要判断是否结束
                hLyric->last_normal_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.font_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);

                if (len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }
            }
            else if (hLyric->hl_count == 1)     //normal 与 focus互换
            {
                i = 0;
                //focus 变成normal
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.font_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle), width);

                if (len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                i = 2;
                hLyric->last_normal_item = hLyric->last_focus_item;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);

                if (len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                hLyric->hl_count = 2;
            }
            else if (hLyric->hl_count == 2)
            {
                hLyric->hl_count = 1;
                i = 2;
                //上一次的焦点变为普通
                //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.font_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle), width);

                if (len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(hLyric->last_focus_item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                hLyric->last_normal_item = hLyric->last_focus_item;
                i = 0;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);

                if (len != 0)
                {
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    //GUI_ClearRect(hLyric->line[i].area.x0,hLyric->line[i].area.y0,hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }
            }

            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select(NULL);
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;
        }
    }

    return EPDK_OK;
}
#endif

//在一个矩形区域内自动滚动显示所有歌词
__s32 update_music_lyrics(H_LYRIC hLyric)
{
#if 1
    __u32 time = 0, width = 0;
    __cedar_subtitle_item_t *item = NULL;

    // __here__;
    if (NULL == hLyric
        || NULL == hLyric->UI.layer)
    {
        return EPDK_FAIL;
    }

    if (hLyric->UI.layer)
    {
        if (GUI_LYRWIN_STA_ON !=  GUI_LyrWinGetSta(hLyric->UI.layer))
        {
            return EPDK_OK;
        }
    }

    time = robin_get_cur_time();

    if (time < 500)
    {
        return EPDK_OK;                         //前500ms获得不到歌词
    }

    item = robin_get_lyric_item(hLyric->items, hLyric->total_item, time);//获得高亮显示的item

    if (item != NULL)
    {
#if 0

        if (item == hLyric->last_item)
        {
            return EPDK_OK;                     //不需要更新歌词
        }

#endif

        if (item < hLyric->last_item)
        {
            hLyric->hl_count = 0;               //从零开始显示
        }

        hLyric->last_item = item;

        //char text[128];
        if (hLyric->UI.layer != NULL)               //选择用户图层
        {
            GUI_LyrWinSel(hLyric->UI.layer);
        }

        GUI_SetBkColor(0xFF2B2E36);
        //GUI_SetColor(hLyric->UI.font_color);
        //GUI_ClearRect(area.x0,area.y0, area.x1, area.y1);
        charset_enm2gui(hLyric->char_code);
        //eLIBs_strcpy(text, (char *)(item->pSubtitle));
        //GUI_DispStringInRect(text, &area, GUI_TA_HCENTER|GUI_TA_VCENTER);
        //if(hLyric->line_count >= 5)                   //only for test
        {
            __u32 len = 0, i  = 0;
            GUI_MEMDEV_Handle   draw_mem;
            draw_mem = GUI_MEMDEV_Create(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.width, hLyric->UI.area.height);
            GUI_MEMDEV_Select(draw_mem);

            if (hLyric->hl_count > 2)
            {
                hLyric->hl_count = 0;
            }

            GUI_ClearRect(hLyric->UI.area.x, hLyric->UI.area.y, hLyric->UI.area.x + hLyric->UI.area.width, hLyric->UI.area.y + hLyric->UI.area.height);
            width  = hLyric->UI.area.width;

            if (width > 16)
            {
                width -= 16;
            }

            {
                //hLyric->hl_count= 1;
                i = 0;
                hLyric->last_focus_item = item;
                //width  = hLyric->UI.area.width;
                GUI_SetColor(hLyric->UI.hight_light_color);
                len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);

                if (len != 0)
                {
                    //__msg("first line test = %s\n", hLyric->line[i].text);
                    GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    i++;
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);

                    if (len != 0)
                    {
                        //__msg("second line test = %s\n", hLyric->line[i].text);
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                    }
                }

                i = 2;

                if (item != (hLyric->items + (hLyric->total_item - 1)))
                {
                    item ++;//获得下一个条目,要判断是否结束
                    hLyric->last_normal_item = item;
                    GUI_SetColor(hLyric->UI.font_color);
                    len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle), width);

                    if (len != 0)
                    {
                        //__msg("third line test = %s\n", hLyric->line[i].text);
                        GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                        i++;
                        //                      __msg("first line text len = %d\n", len);
                        len = GUI_GetStringByWidth(hLyric->line[i].text, (char *)(item->pSubtitle + len), width);

                        //                  __msg("second line text len = %d, width = %d\n", len, width);
                        if (len != 0)
                        {
                            __msg("second line test = %s\n", hLyric->line[i].text);
                            __msg("second line area x0 = %d, y0 = %d, x1 = %d, y1 = %d\n", hLyric->line[i].area.x0, hLyric->line[i].area.y0, hLyric->line[i].area.x1, hLyric->line[i].area.y1);
                            GUI_DispStringInRect(hLyric->line[i].text, &(hLyric->line[i].area), GUI_TA_HCENTER);
                        }
                    }
                }
            }

            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select(NULL);
            GUI_MEMDEV_Delete(draw_mem);
            draw_mem = NULL;
        }
    }

#endif
    return EPDK_OK;
}
