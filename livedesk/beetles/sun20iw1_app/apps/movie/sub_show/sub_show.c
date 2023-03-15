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
#include "sub_show.h"
#include "../common/common.h"
#include "beetles_app.h"

typedef struct
{
    __subtitle_profile_t *sub_list;
    __cedar_subtitle_item_t *sub_item;
} __sub_inf;

typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;
    __u32 sub_pos;
    __u32 sub_color;
    __u32 sub_size;

    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
    __u16 sub_update_timmer_id;
    __sub_inf sub_inf;
} movie_sub_show_scene_t;

#define min(x, y)          ( (x) < (y) ? (x) : (y) )
#define PGS_TRANS_VALUE     16

typedef enum
{
    SUB_UNKNOWN,
    SUB_TEXT,
    SUB_BITMAP,
    SUB_PGS,
    SUB_NUM
} SUB_TYPE;

static __u32  custom_palette[4] = { 0x00FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF0000FF };


/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR __sub_show_8bpp_layer_create(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        //0,                                                /* alpha_en  */
        //0,                                                /* alpha_val */
        1,                                              /* alpha_en  */
        0xa0,                                               /* alpha_val */
        1,                                              /* pipe      */
        11,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "sub menu layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = rect->width;
    fb.size.height      = rect->height;
    //fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe = pipe;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}

void __sub_show_init(movie_sub_show_scene_t *scene_para)
{
    scene_para->sub_inf.sub_list = (__subtitle_profile_t *)esMEMS_Malloc(0, sizeof(__subtitle_profile_t));
    scene_para->sub_inf.sub_item = (__cedar_subtitle_item_t *)esMEMS_Malloc(0, sizeof(__cedar_subtitle_item_t));

    if ((scene_para->sub_inf.sub_list == NULL) || (scene_para->sub_inf.sub_item == NULL))
    {
        __err("scene_para->sub_inf.sub_list or scene_para->sub_inf.sub_item esMEMS_Balloc fail\r");
    }

    eLIBs_memset(scene_para->sub_inf.sub_list, 0x00, sizeof(__subtitle_profile_t));
    eLIBs_memset(scene_para->sub_inf.sub_item, 0x00, sizeof(__cedar_subtitle_item_t));
    return ;
}

void __sub_show_uninit(movie_sub_show_scene_t *scene_para)
{
    if (scene_para->sub_inf.sub_list)
    {
        esMEMS_Mfree(0, scene_para->sub_inf.sub_list);
        scene_para->sub_inf.sub_list = NULL;
    }

    if (scene_para->sub_inf.sub_item)
    {
        esMEMS_Mfree(0, scene_para->sub_inf.sub_item);
        scene_para->sub_inf.sub_item = NULL;
    }
}

static __s32 __sub_show_install_sub_show_update_timmer(movie_sub_show_scene_t *scene_para)
{
    if (NULL == scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->sub_update_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->sub_update_timmer_id
                     , 100, NULL);//0.25秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __sub_show_uninstall_sub_show_update_timmer(movie_sub_show_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->sub_update_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->sub_update_timmer_id);
    }

    return EPDK_OK;
}


static void LIST_ItemBitmapConvert(__cedar_subtitle_item_t *item, __u8 *buffer, SIZE *size, SIZE *valid_size, __bool valid_palatte, SUB_TYPE sub_type, __bool bTop)
{
    __u32  i;
    __u32  j;
    __u8  *p;
    __u8  *q;
    __u32  valid_width;
    __u32  valid_height;
    __u8  alpha_array[4];
    valid_width  = min(size->width, item->nPicWidth);
    valid_height = min(size->height, item->nPicHeight);

    if (1 == bTop)
    {
        q = (__u8 *)buffer + (size->width - valid_width) / 2;
    }
    else
    {
        q = (__u8 *)buffer + (size->height - valid_height) * size->width + (size->width - valid_width) / 2;
    }

    p = item->pSubtitle;

    if (SUB_BITMAP == sub_type)
    {
        alpha_array[0] = item->contrast.backpixelContrast;
        alpha_array[1] = item->contrast.patpixelContrast;
        alpha_array[2] = item->contrast.pixel1Contrast;
        alpha_array[3] = item->contrast.pixel2Contrast;

        if (valid_palatte)
        {
            __u8  index_array[4];
            index_array[0] = item->colorCode.backpixelColorCode;
            index_array[1] = item->colorCode.patpixelColorCode;
            index_array[2] = item->colorCode.pixel1ColorCode;
            index_array[3] = item->colorCode.pixel2ColorCode;

            for (i = 0;  i < valid_height;  i++)
            {
                for (j = 0;  j < valid_width;  j++)
                {
                    if (alpha_array[*p] == 0)
                    {
                        *q++ = 0;
                    }
                    else
                    {
                        *q++ = index_array[*p] + 0xD0;
                    }

                    p++;
                }

                q += (size->width - valid_width);
                p += (item->nPicWidth - valid_width);
            }
        }
        else
        {
            for (i = 0;  i < valid_height;  i++)
            {
                for (j = 0;  j < valid_width;  j++)
                {
                    if (alpha_array[*p] == 0)
                    {
                        *p = 0;
                    }

                    *q++ = *p++ + 0xD0;
                }

                q += (size->width - valid_width);
                p += (item->nPicWidth - valid_width);
            }
        }
    }
    else if (SUB_PGS == sub_type)
    {
        for (i = 0;  i < valid_height;  i++)
        {
            for (j = 0;  j < valid_width;  j++)
            {
                if (*p == PGS_TRANS_VALUE)
                {
                    *q++ = 0;
                }
                else
                {
                    *q++ = *p + 0xD0;
                }

                p++;
            }

            q += (size->width - valid_width);
            p += (item->nPicWidth - valid_width);
        }
    }

    valid_size->width  = valid_width ;
    valid_size->height = valid_height;
}

static __epdk_charset_enm_e GetSysDefaultCharset(void)
{
    __epdk_language_enm_e e_lang;
    __epdk_charset_enm_e e_charset;
    reg_system_para_t *para;
    para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    if (!para)
    {
        return EPDK_CHARSET_ENM_GBK;
    }

    e_lang = para->language;

    if (EPDK_LANGUAGE_ENM_CHINESES == e_lang)
    {
        e_charset = EPDK_CHARSET_ENM_GBK;
    }
    else if (EPDK_LANGUAGE_ENM_CHINESET == e_lang)
    {
        e_charset = EPDK_CHARSET_ENM_BIG5;
    }
    else
    {
        e_charset = EPDK_CHARSET_ENM_GBK;
    }

    return e_charset;
}


static __epdk_charset_enm_e LIST_ItemEncodeConvert(__s32 src_encode)
{
    __epdk_charset_enm_e des_encode;

    //  __inf("src_encode = %d", src_encode);

    /*user add more according to your need*/
    switch ((__cedar_subtitle_encode_t)src_encode)
    {
        case CEDAR_SUB_ENCODE_UTF8:
            des_encode = EPDK_CHARSET_ENM_UTF8;
            break;

        case CEDAR_SUB_ENCODE_GB2312:
            des_encode = EPDK_CHARSET_ENM_GB2312;
            break;

        case CEDAR_SUB_ENCODE_GBK:
        case CEDAR_SUB_ENCODE_ANSI:
            des_encode = EPDK_CHARSET_ENM_GBK;
            break;

        case CEDAR_SUB_ENCODE_UTF16BE:
            des_encode = EPDK_CHARSET_ENM_UTF16BE;
            break;

        case CEDAR_SUB_ENCODE_UTF16LE:
            des_encode = EPDK_CHARSET_ENM_UTF16LE;
            break;

        case CEDAR_SUB_ENCODE_BIG5:
            des_encode = EPDK_CHARSET_ENM_BIG5;
            break;

        default:
            des_encode = GetSysDefaultCharset();
            break;
    }

    //  __inf("des_encode = %d", des_encode);
    return des_encode;
}

static __s32 __sub_show_set_text_color(__u32 index, __u32 color)
{
#if 0
    ES_FILE *hdis;
    __u32  palette[256];
    __u32  arg[3];
    __s32  ret;
    hdis = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

    if (hdis == NULL)
    {
        __err("Error in opening disp!");
        return -1;
    }

    /* get palette */
    arg[0] = (__u32)palette;
    arg[1] = 0;
    arg[2] = 256 * 4;
    ret = eLIBs_fioctrl(hdis, DISP_CMD_GET_PALETTE_TBL, 0, (void *)arg);

    if (ret != 0)
    {
        __wrn("Fail in setting palette.");
        eLIBs_fclose(hdis);
        return -1;
    }

    //set user define text color
    com_set_palette_text_color(palette, index, color);
    /* set palette */
    arg[0] = (__u32)palette;
    arg[1] = 0;
    arg[2] = 256 * 4;
    ret = esMODS_MIoctrl(hdis, DISP_CMD_SET_PALETTE_TBL, 0, (void *)arg);

    if (ret != 0)
    {
        __wrn("Fail in setting palette.");
        eLIBs_fclose(hdis);
        return -1;
    }

    eLIBs_fclose(hdis);
#else
    __wrn("not support anymore after d100");
#endif
    return EPDK_OK;
}

static __s32 __sub_show_update_subtitle(movie_sub_show_scene_t *scene_para)
{
    __s32   ret;
    __s32   time;
    __s32   sub_index;
    static __s32    start_time = -1;
    __subtitle_stream_info_t    *p_item_inf = NULL;  /*current item pointer*/
    SUB_TYPE    sub_type;
    __bool  valid_pallete = 0;
    SIZE    buffer_size;
    SIZE    valid_size;
    __u8    *sub_buffer = NULL;
    RECT lyr_rect;
    //    __here__;
    GUI_LyrWinGetScnWindow(scene_para->hlyr, &lyr_rect);
    ret = robin_subtitle_exist();

    if (-1 == ret) //无字幕
    {
        __wrn("robin_subtitle_exist ret fail...");
        GUI_LyrWinSel(scene_para->hlyr);
        GUI_SetBkColor(0x0);
        GUI_Clear();
        return EPDK_FAIL;
    }

    //    __wrn("robin_subtitle_exist ret ok...");
    time = robin_get_cur_time();
    //  __here__;
    ret = robin_get_subtitle_item(time, scene_para->sub_inf.sub_item);

    //    __here__;
    if (-1 == ret) //无字幕
    {
        GUI_LyrWinSel(scene_para->hlyr);
        GUI_SetBkColor(0x0);
        GUI_Clear();
        return EPDK_FAIL;
    }

    __wrn("********robin_get_subtitle_item ok*******");

    if (start_time == scene_para->sub_inf.sub_item->uPts) //字幕和原来相同
    {
        __wrn("start_time == scene_para->sub_inf.sub_item->uPts");
        return EPDK_FAIL;
    }

    //__here__;
    start_time = scene_para->sub_inf.sub_item->uPts;
    GUI_LyrWinSel(scene_para->hlyr);
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    sub_index = robin_get_subtitle_index();

    if (sub_index == -1)
    {
        sub_index = 0;
    }

    //__here__;
    robin_get_subtitle_list(scene_para->sub_inf.sub_list);
    //__here__;
    p_item_inf = &scene_para->sub_inf.sub_list->SubtitleStreamList[sub_index];
    __wrn("p_item_inf->subtitle_type=%d", p_item_inf->subtitle_type);

    switch (p_item_inf->subtitle_type)
    {
        case CEDAR_LRC_TYPE_NONE    :
        case CEDAR_LRC_TYPE_NOTSPT  :
            sub_type = SUB_UNKNOWN;
            break;

        case CEDAR_LRC_TYPE_SUB    :
        case CEDAR_LRC_TYPE_ISUB   :
        case CEDAR_LRC_TYPE_IDXSUB :
        {
            sub_type = SUB_BITMAP;
            break;
        }

        case CEDAR_LRC_TYPE_PGS    :
        {
            sub_type = SUB_PGS;
            break;
        }

        case CEDAR_LRC_TYPE_LRC    :
        case CEDAR_LRC_TYPE_SRT    :
        case CEDAR_LRC_TYPE_KSC    :
        case CEDAR_LRC_TYPE_SMI    :
        case CEDAR_LRC_TYPE_SSA    :
        case CEDAR_LRC_TYPE_TXT    :
        case CEDAR_LRC_TYPE_ITXT   :
        default :
        {
            sub_type = SUB_TEXT;
            break;
        }
    }

    __wrn("sub_type=%d", sub_type);
    buffer_size.width = 0;
    buffer_size.height = 0;

    if (sub_type == SUB_BITMAP || sub_type == SUB_PGS)
    {
        buffer_size.width = lyr_rect.width;
        buffer_size.height = lyr_rect.height;
        sub_buffer = (__u8 *)esMEMS_Palloc((buffer_size.width * buffer_size.height + 1023) / 1024, 0);

        if (NULL == sub_buffer)
        {
            __msg("alloc fail...");
            return EPDK_FAIL;
        }

        eLIBs_memset(sub_buffer, 0x0, buffer_size.width * buffer_size.height);

        if (p_item_inf->palette_info.valid_flg)
        {
            __u32  i;

            for (i = 0;  i < 16;  i++)
            {
                p_item_inf->palette_info.palette_ary[i] |= 0xFF000000;
            }

            com_set_palette_color_by_index(0xD0,
                                           (__u32 *)p_item_inf->palette_info.palette_ary, 16);
            valid_pallete = 1;
        }
        else
        {
            com_set_palette_color_by_index(0xD0, custom_palette, 4);
            valid_pallete = 0;
        }
    }

    /*now we adjust the type fo subtitle*/
    if (scene_para->sub_inf.sub_item->uDataType == CEDAR_SUB_ENCODE_BITMAP || sub_type == SUB_PGS)
    {
        GUI_RECT gui_rect;

        if (NULL == sub_buffer)
        {
            return EPDK_TRUE;
        }

        //如果图层置顶，则设置top属性
        LIST_ItemBitmapConvert(scene_para->sub_inf.sub_item, sub_buffer, &buffer_size, &valid_size, valid_pallete
                               , sub_type, 2 == scene_para->sub_pos);
        gui_rect.x0 = 0;
        gui_rect.x1 = lyr_rect.width - 1;
        gui_rect.y0 = 0;
        gui_rect.y1 = lyr_rect.height - 1;
        //__inf("x0 =%d, y0 = %d, x1 = %d,y1 = %d", gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        GUI_SetRectMem(&gui_rect, sub_buffer, buffer_size.width, buffer_size.height, 0, 0);
    }
    else
    {
        __epdk_charset_enm_e coding;
        GUI_RECT gui_rect;
        __s32      old_mode;
        __u8 err;
        __wrn("scene_para->sub_inf.sub_item->uDataType=%d", scene_para->sub_inf.sub_item->uDataType);
        coding = LIST_ItemEncodeConvert(scene_para->sub_inf.sub_item->uDataType);
        //  old_mode = GUI_SetFontMode( GUI_FONTMODE_8BPP32_FRAME );
        GUI_SetFrameColor8bpp32(0xF2);
        __wrn("coding=%d", coding);
        GUI_CharSetToEncode(coding);
        GUI_SetFont(SWFFont);
        __sub_show_set_text_color(0xD0, scene_para->sub_color);
        GUI_SetColor(GUI_YELLOW);
        gui_rect.x0 = 0;
        gui_rect.x1 = lyr_rect.width - 1;
        gui_rect.y0 = 0;
        gui_rect.y1 = lyr_rect.height - 1;
        __wrn("(char*)scene_para->sub_inf.sub_item->pSubtitle=%s", (char *)scene_para->sub_inf.sub_item->pSubtitle);

        /*如果启用了独立字幕图层，并且在上面显示，则设置TOP的属性*/
        if (2 == scene_para->sub_pos)
        {
            GUI_DispStringInRectWrap((char *)scene_para->sub_inf.sub_item->pSubtitle, &gui_rect, GUI_TA_TOP | GUI_TA_HCENTER, GUI_WRAPMODE_WORD);
        }
        else
        {
            GUI_DispStringInRectWrap((char *)scene_para->sub_inf.sub_item->pSubtitle, &gui_rect, GUI_TA_BOTTOM | GUI_TA_HCENTER, GUI_WRAPMODE_WORD);
        }

        __msg("scene_para->sub_inf.sub_item->pSubtitle=%s", scene_para->sub_inf.sub_item->pSubtitle);
        //  GUI_SetFontMode( old_mode );
    }

    //__here__;
    if (sub_type == SUB_BITMAP || sub_type == SUB_PGS)
    {
        esMEMS_Pfree(sub_buffer, (buffer_size.width * buffer_size.height + 1023) / 1024);
    }

    //__here__;
    return EPDK_OK;
}

static __s32 __sub_show_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            movie_sub_show_scene_t *scene_para = NULL;
            __msg("__sub_show_proc GUI_MSG_CREATE begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (NULL == scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
            __sub_show_init(scene_para);
            __sub_show_install_sub_show_update_timmer(scene_para);
            __msg("__sub_show_proc GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            movie_sub_show_scene_t *scene_para = NULL;
            __msg("__sub_show_proc GUI_MSG_DESTROY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (NULL == scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            __sub_show_uninstall_sub_show_update_timmer(scene_para);
            __sub_show_uninit(scene_para);
            __msg("__sub_show_proc GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            __msg("__sub_show_proc GUI_MSG_KEY begin");
            __msg("__sub_show_proc GUI_MSG_KEY end");
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            __msg("__sub_show_proc GUI_MSG_TOUCH begin");
            __msg("__sub_show_proc GUI_MSG_TOUCH end");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            movie_sub_show_scene_t *scene_para = NULL;
            __msg("__sub_show_proc GUI_MSG_PAINT begin");
            __msg("__sub_show_proc GUI_MSG_PAINT begin");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __s32 ret;
            movie_sub_show_scene_t *scene_para = NULL;
            //__msg("__sub_show_proc GUI_MSG_TIMER begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);
            //            __here__;
            ret = -1;

            if (scene_para != NULL)
            {
                //__here__;
                if (LOWORD(msg->dwAddData1) == scene_para->sub_update_timmer_id)
                {
                    __s32 sem_nr;
                    //   __here__;
                    sem_nr = MOVIE_ROBIN_ACCEPT;

                    if (sem_nr > 0)
                    {
                        __sub_show_update_subtitle(scene_para);
                        MOVIE_ROBIN_POST;
                    }

                    ret = EPDK_OK;
                }
            }
            else
            {
                __err("scene_para is null...");
            }

            //            __msg("__sub_show_proc GUI_MSG_TIMER end");

            if (-1 == ret)//未处理，交给子场景处理
            {
                //   __here__;
                break;
            }
            else//已处理
            {
                //   __here__;
                return EPDK_OK;
            }
        }

        case GUI_MSG_CLOSE:
        {
            __msg("__sub_show_proc GUI_MSG_CLOSE begin");
            __msg("__sub_show_proc GUI_MSG_CLOSE end");
            break;
        }

        default:
        {
            break;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_sub_show_scene_create(movie_sub_show_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_sub_show_scene_t *sence_para;
    movie_sub_show_uipara_t *ui_para;
    RECT lyr_rect;
    sence_para = (movie_sub_show_scene_t *)esMEMS_Malloc(0, sizeof(movie_sub_show_scene_t));

    if (!sence_para)
    {
        __msg("mem not enough...");
        return NULL;
    }

    //eLIBs_printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
    eLIBs_memset(sence_para, 0, sizeof(movie_sub_show_scene_t));
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->sub_pos = create_para->sub_pos;
    sence_para->sub_color = create_para->sub_color;
    sence_para->sub_size = create_para->sub_size;
    sence_para->sub_update_timmer_id = 0x17;
    ui_para = movie_sub_show_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("movie_spsc_get_uipara fail...");
        return NULL;
    }

    if (sence_para->sub_pos >= movie_sub_show_pos_num)
    {
        sence_para->sub_pos = movie_sub_show_pos_num - 1;
    }

    lyr_rect.x      = ui_para->uipara_lyr[sence_para->sub_pos].x;
    lyr_rect.y      = ui_para->uipara_lyr[sence_para->sub_pos].y;
    lyr_rect.width  = ui_para->uipara_lyr[sence_para->sub_pos].w;
    lyr_rect.height = ui_para->uipara_lyr[sence_para->sub_pos].h;
    sence_para->hlyr = __sub_show_8bpp_layer_create(&lyr_rect, ui_para->pipe);

    if (NULL == sence_para->hlyr)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("sence_para->hlyr is null...");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          =   "movie_sub_show_win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE | WS_DISABLED;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = sence_para->scene_id;
    framewin_para.hHosting      =  sence_para->hparent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__sub_show_proc);
    framewin_para.rect.x        = ui_para->uipara_frm.x;
    framewin_para.rect.y        = ui_para->uipara_frm.y;
    framewin_para.rect.width    = ui_para->uipara_frm.w;
    framewin_para.rect.height   = ui_para->uipara_frm.h;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)sence_para;
    framewin_para.hLayer        = sence_para->hlyr;
    sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);

    if (NULL == sence_para->hfrm)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("GUI_FrmWinCreate fail...");
        return NULL;
    }

    if (sence_para->hlyr)
    {
        GUI_LyrWinSetSta(sence_para->hlyr, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(sence_para->hlyr);
    }
    else
    {
        __err("invalid para...");
    }

    return sence_para;
}

__s32 movie_sub_show_scene_delete(void *handle)
{
    movie_sub_show_scene_t *scene_para;
    scene_para = (movie_sub_show_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_FrmWinDelete(scene_para->hfrm);

    if (!scene_para->hlyr)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_LyrWinDelete(scene_para->hlyr);
    eLIBs_memset(scene_para, 0, sizeof(movie_sub_show_scene_t));//防止重复释放
    esMEMS_Mfree(0, scene_para);
    return EPDK_OK;
}
