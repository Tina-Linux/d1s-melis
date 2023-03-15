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
#include "mod_init_i.h"
#include <kconfig.h>

#define  BEETLES_ROOTFS_CARD_UPDATE_ROOT "d:\\"
#define  BEETLES_CARD_UPDATE_ROOT "f:\\"

#define LANG_DIR            BEETLES_ROOTFS_CARD_UPDATE_ROOT"apps\\lang.bin"
#define THEME_DIR           BEETLES_ROOTFS_CARD_UPDATE_ROOT"apps\\theme.bin"

#define UPDATE_CONFIG_PATH  BEETLES_CARD_UPDATE_ROOT"update_config.txt"

GUI_FONT    *SWFFont = NULL;

static H_WIN init_mainwin;


static int      ModDBMSID;
static __mp    *ModDBMS;
extern GUI_FONT    *SWFFont;

#define INIT_FW_UPDATE_MAX_ICON_RES_NUM 2
#if defined(LCD_1024X600)
#define D_INIT_SERVER_PROG_BG_W (680)
#define D_INIT_SERVER_PROG_BG_H (26)

#define D_INIT_SERVER_PROG_LEFT_W (12)
#define D_INIT_SERVER_PROG_LEFT_H (26)

#define D_INIT_SERVER_PROG_MID_W (12)
#define D_INIT_SERVER_PROG_MID_H (26)

#define D_INIT_SERVER_PROG_CURSOR_W (26)
#define D_INIT_SERVER_PROG_CURSOR_H (26)
#elif defined(LCD_1280X720)
#define D_INIT_SERVER_PROG_BG_W (1088)
#define D_INIT_SERVER_PROG_BG_H (39)

#define D_INIT_SERVER_PROG_LEFT_W (19)
#define D_INIT_SERVER_PROG_LEFT_H (39)

#define D_INIT_SERVER_PROG_MID_W (19)
#define D_INIT_SERVER_PROG_MID_H (39)

#define D_INIT_SERVER_PROG_CURSOR_W (39)
#define D_INIT_SERVER_PROG_CURSOR_H (39)
#elif defined(LCD_1280X800)
#define D_INIT_SERVER_PROG_BG_W (1088 - DISPLAY_OFFSET_WIDTH_TOTAL)
#define D_INIT_SERVER_PROG_BG_H (39)

#define D_INIT_SERVER_PROG_LEFT_W (19)
#define D_INIT_SERVER_PROG_LEFT_H (39)

#define D_INIT_SERVER_PROG_MID_W (19)
#define D_INIT_SERVER_PROG_MID_H (39)

#define D_INIT_SERVER_PROG_CURSOR_W (39)
#define D_INIT_SERVER_PROG_CURSOR_H (39)
#elif defined(LCD_1366X768)
#define D_INIT_SERVER_PROG_BG_W (1088 - DISPLAY_OFFSET_WIDTH_TOTAL)
#define D_INIT_SERVER_PROG_BG_H (39)

#define D_INIT_SERVER_PROG_LEFT_W (19)
#define D_INIT_SERVER_PROG_LEFT_H (39)

#define D_INIT_SERVER_PROG_MID_W (19)
#define D_INIT_SERVER_PROG_MID_H (39)

#define D_INIT_SERVER_PROG_CURSOR_W (39)
#define D_INIT_SERVER_PROG_CURSOR_H (39)
#elif defined(LCD_1920X1200)
#define D_INIT_SERVER_PROG_BG_W (1088)
#define D_INIT_SERVER_PROG_BG_H (39)

#define D_INIT_SERVER_PROG_LEFT_W (19)
#define D_INIT_SERVER_PROG_LEFT_H (39)

#define D_INIT_SERVER_PROG_MID_W (19)
#define D_INIT_SERVER_PROG_MID_H (39)

#define D_INIT_SERVER_PROG_CURSOR_W (39)
#define D_INIT_SERVER_PROG_CURSOR_H (39)
#else
#define D_INIT_SERVER_PROG_BG_W (680)
#define D_INIT_SERVER_PROG_BG_H (26)

#define D_INIT_SERVER_PROG_LEFT_W (12)
#define D_INIT_SERVER_PROG_LEFT_H (26)

#define D_INIT_SERVER_PROG_MID_W (12)
#define D_INIT_SERVER_PROG_MID_H (26)

#define D_INIT_SERVER_PROG_CURSOR_W (26)
#define D_INIT_SERVER_PROG_CURSOR_H (26)
#endif

typedef enum
{
    init_fw_update_prog_bg = 0,
    init_fw_update_prog_left,
    init_fw_update_prog_mid,
    init_fw_update_prog_cursor,
    init_fw_update_prog_icon_num
} init_fw_update_icon_t;

typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[INIT_FW_UPDATE_MAX_ICON_RES_NUM];
    HTHEME res_hdl[INIT_FW_UPDATE_MAX_ICON_RES_NUM];
} init_fw_update_rect_t;

static __s32 __init_prog_draw_progress(init_fw_update_rect_t *prog_bg, init_fw_update_rect_t *prog_left,
                                       init_fw_update_rect_t *prog_mid, init_fw_update_rect_t *prog_cursor,
                                       __s32 min, __s32 max, __s32 cur)
{
    char str_text[128];
    //画进度条背景
    {
        void *pbmp;

        if (!prog_bg->res_hdl[0])
        {
            __msg("prog_bg->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_bg->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_bg->x, prog_bg->y);
    }
    //画进度条左边的图标
    {
        void *pbmp;
        __s32 focus;
        focus = 0;

        if (!prog_left->res_hdl[focus])
        {
            __msg("prog_left->res_hdl[focus] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_left->res_hdl[focus]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_left->x, prog_left->y);
    }

    //画进度条和cursor
    if (cur > max) //保护一下
    {
        cur = max;
    }

    if (cur < min) //保护一下
    {
        cur = min;
    }

    if (cur >= min
        && cur <= max)
    {
        void *pbmp;

        if (!prog_mid->res_hdl[0])
        {
            __msg("prog_mid->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_mid->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        {
            __s32 i;
            __s32 n;
            __s32 mid_w;
            __s32 max_mid_w;
            __s32 cursor_w;
            __s32 cursor_pos;
            __s32 bg_pos;
            __s32 bg_w;

            if (cur == min)
            {
                n = 0;
            }
            else
            {
                n = (prog_bg->w - prog_cursor->w) * (cur - min)
                    / prog_mid->w / (max - min);
            }

            mid_w = prog_mid->w;
            max_mid_w = prog_bg->w - prog_cursor->w;
            cursor_w = prog_cursor->w;
            bg_pos = prog_bg->x;
            bg_w = prog_bg->w;

            //防止非整数倍时画不满
            if (max_mid_w - n * mid_w < mid_w)
            {
                n++;
                cursor_pos = bg_pos + bg_w - cursor_w;
            }
            else
            {
                cursor_pos = bg_pos + n * mid_w;
            }

            for (i = 1 ; i < n ; i++)
            {
                GUI_BMP_Draw(pbmp, prog_mid->x + i * prog_mid->w
                             , prog_mid->y);
            }

            //画cursor
            if (!prog_cursor->res_hdl[0])
            {
                __msg("prog_cursor->res_hdl[0] is null...");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(prog_cursor->res_hdl[0]);

            if (!pbmp)
            {
                __msg("pbmp is null...");
                return EPDK_FAIL;
            }

            GUI_BMP_Draw(pbmp, cursor_pos, prog_cursor->y);
        }
    }

    return EPDK_OK;
}

static H_LYR __prog_32bpp_layer_create(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb = {
		{0, 0},                              /* size      */
        {NULL, NULL, NULL},                                    /* buffer    */
        {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)NULL, {NULL, 0}}}   /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
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

static __u32 read_config_file(ES_FILE *pfile, __s32 *ca_enable, char *update_path)
{
    __u32 file_len = 0, count = 0;
    char *file_buf = NULL, *head = NULL, *ptr = NULL;

    if (pfile == NULL || ca_enable == NULL || update_path == NULL)
    {
        __log("read_config para null ");
        return EPDK_FAIL;
    }

    eLIBs_fseek(pfile, 0, SEEK_END);
    file_len = eLIBs_ftell(pfile);
    eLIBs_fseek(pfile, 0, SEEK_SET);
    file_buf = (unsigned char *)eLIBs_malloc(sizeof(unsigned char) * file_len);

    if (file_buf == NULL)
    {
        __log("file_buf malloc null ");
        return EPDK_FAIL;
    }

    head = file_buf;
    count = eLIBs_fread(file_buf, sizeof(unsigned char), file_len, pfile);

    if (count != file_len)
    {
        eLIBs_free(file_buf);
        __log("count != file_len");
        return EPDK_FAIL;
    }

    //__log("file_buf:%s", file_buf);
    ptr = eLIBs_strstr(head, "ca_enable:<");
    head = ptr + eLIBs_strlen("ca_enable:<");
    __log("*head:%c", *head);
    *ca_enable = *head - '0';
    ptr = eLIBs_strstr(head, "update_path:<");
    head = ptr + eLIBs_strlen("update_path:<");
    ptr = eLIBs_strchr(head, '>');
    eLIBs_memcpy(update_path, BEETLES_CARD_UPDATE_ROOT, eLIBs_strlen(BEETLES_CARD_UPDATE_ROOT));
    eLIBs_memcpy(update_path + eLIBs_strlen(BEETLES_CARD_UPDATE_ROOT), head, ptr - head);
    __log("update_path:%s", update_path);	
	return EPDK_OK;
}
static void fw_update(void)
{
    __s32 len;
    __u8    mid_update;
    __mp   *mod_update;
    __s32 progress = -1, old_progress = -1;
    RECT rect = {0};
    char str_text[256] = {0};
    GUI_RECT gui_rect;
    H_LYR hlyr;
    __s32 width, height;
    __u32 status = 0;
    user_gpio_set_t  gpio_set[1];
    __s32 ret;
    __hdle  card_pin_hd[2] = {NULL};
    __s32 ca_enable = 0;
    char update_file[128] = {0};
    init_fw_update_rect_t prog_ui[init_fw_update_prog_icon_num];

    dsk_display_get_size(&width, &height);
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    hlyr = __prog_32bpp_layer_create(&rect, 1);

    if (NULL == hlyr)
    {
        __err("create layer fail...");
        return ;
    }

    GUI_LyrWinSetSta(hlyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(hlyr);
    GUI_LyrWinSel(hlyr);
    dsk_langres_get_menu_text(STRING_FW_UPDATE, str_text, sizeof(str_text));
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    gui_rect.x0 = 0;
    gui_rect.y0 = height / 2 - 64;
    gui_rect.x1 = rect.width - 1;
    gui_rect.y1 = gui_rect.y0 + 32;
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    __log("str_text=%s ", str_text);
    prog_ui[init_fw_update_prog_bg].x = (width - D_INIT_SERVER_PROG_BG_W) / 2;
    prog_ui[init_fw_update_prog_bg].y = height / 2;
    prog_ui[init_fw_update_prog_bg].w = D_INIT_SERVER_PROG_BG_W;
    prog_ui[init_fw_update_prog_bg].h = D_INIT_SERVER_PROG_BG_H;
    prog_ui[init_fw_update_prog_bg].res_id[0] = ID_INIT_PROG_BG_BMP;
    prog_ui[init_fw_update_prog_bg].res_id[1] = 0;
    prog_ui[init_fw_update_prog_left].x = (width - D_INIT_SERVER_PROG_BG_W) / 2;
    prog_ui[init_fw_update_prog_left].y = height / 2;
    prog_ui[init_fw_update_prog_left].w = D_INIT_SERVER_PROG_LEFT_W;
    prog_ui[init_fw_update_prog_left].h = D_INIT_SERVER_PROG_LEFT_H;
    prog_ui[init_fw_update_prog_left].res_id[0] = ID_INIT_PROG_LEFT_BMP;
    prog_ui[init_fw_update_prog_left].res_id[1] = 0;
    prog_ui[init_fw_update_prog_mid].x = (width - D_INIT_SERVER_PROG_BG_W) / 2;
    prog_ui[init_fw_update_prog_mid].y = height / 2;
    prog_ui[init_fw_update_prog_mid].w = D_INIT_SERVER_PROG_MID_W;
    prog_ui[init_fw_update_prog_mid].h = D_INIT_SERVER_PROG_MID_H;
    prog_ui[init_fw_update_prog_mid].res_id[0] = ID_INIT_PROG_MID_BMP;
    prog_ui[init_fw_update_prog_mid].res_id[1] = 0;
    prog_ui[init_fw_update_prog_cursor].x = (width - D_INIT_SERVER_PROG_BG_W) / 2;
    prog_ui[init_fw_update_prog_cursor].y = height / 2;
    prog_ui[init_fw_update_prog_cursor].w = D_INIT_SERVER_PROG_CURSOR_W;
    prog_ui[init_fw_update_prog_cursor].h = D_INIT_SERVER_PROG_CURSOR_H;
    prog_ui[init_fw_update_prog_cursor].res_id[0] = ID_INIT_PROG_CURSOR_BMP;
    prog_ui[init_fw_update_prog_cursor].res_id[1] = 0;
    {
        ES_FILE *pfile = NULL;

        if ((pfile = eLIBs_fopen(UPDATE_CONFIG_PATH, "r+")) == NULL)
        {
            gui_rect.x0 = 0;
            gui_rect.y0 = height / 2 - 90;
            gui_rect.x1 = rect.width;
            gui_rect.y1 = height / 2 - 64;
            GUI_SetFont(SWFFont);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_SetColor(GUI_WHITE);
            GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
            eLIBs_memset(str_text, 0x00, sizeof(str_text));
            eLIBs_sprintf(str_text, "Not Find Update Config File:%s", UPDATE_CONFIG_PATH);
            GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_LyrWinFlushFB(hlyr);
            eLIBs_memset(str_text, 0x00, sizeof(str_text));
            __log("请在TF卡放置升级配置文件:[%s]", UPDATE_CONFIG_PATH);

            while (1);
        }
        else
        {
            read_config_file(pfile, &ca_enable, update_file);
            eLIBs_fclose(pfile);
            pfile = NULL;

            if ((pfile = eLIBs_fopen(update_file, "r+")) == NULL)
            {
                gui_rect.x0 = 0;
                gui_rect.y0 = height / 2 - 90;
                gui_rect.x1 = rect.width;
                gui_rect.y1 = height / 2 - 64;
                GUI_SetFont(SWFFont);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_SetColor(GUI_WHITE);
                GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
                eLIBs_memset(str_text, 0x00, sizeof(str_text));
                eLIBs_sprintf(str_text, "Not Find Update firmware File:%s", update_file);
                GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
				GUI_LyrWinFlushFB(hlyr);
                eLIBs_memset(str_text, 0x00, sizeof(str_text));
                __log("请在TF卡放置固件:[%s]", update_file);

                while (1);
            }
            else
            {
                gui_rect.x0 = 0;
                gui_rect.y0 = height / 2 - 90;
                gui_rect.x1 = rect.width;
                gui_rect.y1 = height / 2 - 64;
                GUI_SetFont(SWFFont);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_SetColor(GUI_WHITE);
                GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
                eLIBs_memset(str_text, 0x00, sizeof(str_text));
                eLIBs_sprintf(str_text, "Update firmware File:%s", update_file);
                GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
				GUI_LyrWinFlushFB(hlyr);
            }

            eLIBs_fclose(pfile);
            pfile = NULL;
        }
    }
    {
        int i;
        int j;

        for (i = 0 ; i < init_fw_update_prog_icon_num ; i++)
        {
            for (j = 0 ; j < INIT_FW_UPDATE_MAX_ICON_RES_NUM ; j++)
            {
                if (prog_ui[i].res_id[j])
                {
                    prog_ui[i].res_hdl[j] = dsk_theme_open(prog_ui[i].res_id[j]);

                    if (!prog_ui[i].res_hdl[j])
                    {
                        __wrn("dsk_theme_open fail...");
                    }
                    else//先读取nor数据占住到内存
                    {
                        dsk_theme_hdl2buf(prog_ui[i].res_hdl[j]);
                    }
                }
                else
                {
                    prog_ui[i].res_hdl[j] = NULL;
                }
            }
        }
    }
    __init_prog_draw_progress(&prog_ui[init_fw_update_prog_bg],
                              &prog_ui[init_fw_update_prog_left],
                              &prog_ui[init_fw_update_prog_mid],
                              &prog_ui[init_fw_update_prog_cursor],
                              0, 100, 0);

    if (ca_enable == 0)
    {
        mid_update = esMODS_MInstall(BEETLES_ROOTFS_CARD_UPDATE_ROOT"mod\\update.mod", 0);
    }
    else
    {
        mid_update = esMODS_MInstall(BEETLES_ROOTFS_CARD_UPDATE_ROOT"mod\\update_ca.mod", 0);
    }

    if (mid_update == 0)
    {
        __err("update mod install error ");
        goto err1;
    }

    mod_update = esMODS_MOpen(mid_update, 0);

    if (mod_update == NULL)
    {
        __err("open update mode error ");
        esMODS_MUninstall(mid_update);
        goto err1;
    }

    esMODS_MIoctrl(mod_update, UPDATE_CMD_START, 0, (void *)update_file);
    while (1)
    {
        esKRNL_TimeDly(10);
        progress = esMODS_MIoctrl(mod_update, UPDATE_CMD_CHECK_PROG, 0, 0);

		if (old_progress == progress)
		{
			continue;
		}

		eLIBs_printf("%s %d progress=%d\n", __FILE__, __LINE__, progress);
        GUI_LyrWinSel(hlyr);
        GUI_SetBkColor(GUI_BLUE);
        GUI_Clear();
        __init_prog_draw_progress(&prog_ui[init_fw_update_prog_bg],
                                  &prog_ui[init_fw_update_prog_left],
                                  &prog_ui[init_fw_update_prog_mid],
                                  &prog_ui[init_fw_update_prog_cursor],
                                  0, 100, progress);
        gui_rect.x0 = 0;
        gui_rect.y0 = height / 2 - 64;
        gui_rect.x1 = rect.width - 1;
        gui_rect.y1 = gui_rect.y0 + 32;
        GUI_SetFont(SWFFont);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_SetColor(GUI_WHITE);
        GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
        eLIBs_memset(str_text, 0, sizeof(str_text));
        dsk_langres_get_menu_text(STRING_FW_UPDATE, str_text, sizeof(str_text));
        eLIBs_sprintf(str_text, "%s%d\%", str_text, progress);
        GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        gui_rect.x0 = 0;
        gui_rect.y0 = height / 2 - 90;
        gui_rect.x1 = rect.width;
        gui_rect.y1 = height / 2 - 64;
        eLIBs_memset(str_text, 0x00, sizeof(str_text));
        eLIBs_sprintf(str_text, "Update firmware File:%s", update_file);
        GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
		GUI_LyrWinFlushFB(hlyr);
		old_progress = progress;
		
        if (100 == progress)
        {
            __wrn("update complete");
            esKRNL_TimeDly(100);
            break;
        }
    }

    status = esMODS_MIoctrl(mod_update, UPDATE_CMD_CHECK_PROG, 0, 0);
    gui_rect.x0 = 0;
    gui_rect.y0 = height / 2 - 64;
    gui_rect.x1 = rect.width - 1;
    gui_rect.y1 = gui_rect.y0 + 32;
    GUI_Clear();
    GUI_LyrWinSel(hlyr);
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    __wrn("status===========%d", status);

    if (status != -1)
    {
        dsk_langres_get_menu_text(STRING_UPDATE_FINISH, str_text, sizeof(str_text));
    }
    else
    {
        dsk_langres_get_menu_text(STRING_UPDATE_FAIL, str_text, sizeof(str_text));
    }

    GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    gui_rect.x0 = 0;
    gui_rect.y0 = height / 2 - 90;
    gui_rect.x1 = rect.width;
    gui_rect.y1 = height / 2 - 64;
    eLIBs_memset(str_text, 0x00, sizeof(str_text));
    eLIBs_sprintf(str_text, "Update firmware File:%s", update_file);
    GUI_DispStringInRect(str_text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_LyrWinFlushFB(hlyr);
    ret = esCFG_GetKeyValue("sddet_para", "detect_pin", (int *)gpio_set, sizeof(user_gpio_set_t) / 4);
    if (!ret)
    {
        __wrn("sd dectect pin port = %d, pin_num = %d", gpio_set->port, gpio_set->port_num);
        card_pin_hd[0] = esPINS_PinGrpReq(gpio_set, 1);

        if (!card_pin_hd[0])
        {
            __wrn("SD Detect : request detect pin fail");
        }

        __wrn("sd dectect pin port = %d, pin_num = %d", gpio_set->port, gpio_set->port_num);
    }

    while (1)
    {
        esKRNL_TimeDly(100);

        if (card_pin_hd[0])
        {
            __s32 data = 0;// 0:in；1:out。
            data = esPINS_ReadPinData(card_pin_hd[0], "detect_pin");  //获取电平

            if (1 == data)
            {
                __err("sdcard plugout , reboot now");
				esKSRV_Reset();
            }
        }
    }
err1:
    return ;
}

__s32 live_init(void)
{
    __log("~~~~~~~~~~~~~~file system char set~~~~~~~~~~~");
    dsk_set_fs_charset(EPDK_CHARSET_ENM_UTF8);
    dsk_set_isn_charset(EPDK_CHARSET_ENM_GBK);
    //create font
    SWFFont     = GUI_SFT_CreateFont(16, BEETLES_ROOTFS_CARD_UPDATE_ROOT"res\\fonts\\font16.sft");

    if (!SWFFont)
    {
        __msg("GUI_SFT_CreateFont fail, set default font....");
        SWFFont = GUI_GetDefaultFont();
    }


    dsk_langres_init(LANG_DIR);
    dsk_theme_init(THEME_DIR);
	dsk_display_set_lcd_brightness(10);//打开背光，启动默认是没有背光的
    return EPDK_OK;
}

void application_init_process(void *arg)
{
    __log("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    __log("&           application_init_process enter!                         &");
    __log("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
#if SCREEN_ROTATE_TYPE
	GUI_SetScnDir(SCREEN_ROTATE_TYPE);
#endif
    live_init();
    __log("start to run firmware update .........");
    {
        GUI_LyrWinCacheOn();
        GUI_LyrWinCacheOff();
        fw_update();
    }
}
