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
#include "photo_fileinfo.h"
#include "app_anole.h"

#define FILE_INFO_TXT_FORMAT        "%s : %s"
//#define FILE_INFO_COLON                " : "
#define FILE_INFO_TIME_STR_FORMAT   "%d/%d/%d %d%d:%d%d"
#define FILE_INFO_TXT_SIZE_FORMAT   "%s : %d x %d"


/************************************************************************/
//加载资源
/************************************************************************/
int32_t photo_file_info_create_res(PHOTO_FILE_INFO_RES_T *res)
{
    //  create_bmp_res(ID_PHOTO_INFO_ICON_BMP, res->bmp_icon);
    get_lang_res(STRING_PHOTO_TAKEN_TIME,   res->str_time);
    get_lang_res(STRING_PHOTO_FILE_SIZE,    res->str_filesize);
    get_lang_res(STRING_PHOTO_PLACE,        res->str_path);
    get_lang_res(STRING_PHOTO_SIZE,         res->str_size);
    //  get_lang_res(STRING_PHOTO_RESOLUTION, );
    //  get_lang_res(STRING_PHOTO_CAMERA_MODEL, );
    //  get_lang_res(STRING_PHOTO_EXPOSUREBAIS, );
    //  get_lang_res(STRING_PHOTO_FLASH, );
    //  get_lang_res(STRING_PHOTO_FOCUS_LENGTH, );
    //  get_lang_res(STRING_PHOTO_COLOR_SPACE, );
    return EPDK_OK;
}

/************************************************************************/
//释放资源
/************************************************************************/
int32_t photo_file_info_destroy_res(PHOTO_FILE_INFO_RES_T *res)
{
    //  destroy_bmp_res(res->bmp_icon);
    return EPDK_OK;
}

/************************************************************************/
//画文件信息
/************************************************************************/
void draw_photo_file_info(H_WIN hwnd, PHOTO_FILE_INFO_RES_T *res, PHOTO_INFO_UI_T *ui)
{
    int32_t     x, y;
    APP_ANOLE_FILE_INFO_T   info;
    char        buf[MAX_FILE_NAME_LEN] = {0};
    char        buf1[64] = {0};
    H_LYR       layer;
    HTHEME      h_bmp;
    void        *bmp_buf;

    layer   = GUI_WinGetLyrWin(hwnd);

    GUI_LyrWinSel(layer);

    __wrn("GUI_LyrWinGetSta(layer) is %d %x\n", GUI_LyrWinGetSta(layer), layer);

    //GUI_LyrWinSetSta(layer,GUI_LYRWIN_STA_ON);
    //GUI_UC_SetEncodeUTF8();
    //GUI_SetFont(SWFFont);

    ZeroMemory(&info, sizeof(info));

    APP_ANOLE_GetCurPicInfo(&info);

    //GUI_BMP_RES_Draw(res->bmp_bg, ui->rt.x, ui->rt.y);
    //GUI_BMP_RES_Draw(res->bmp_icon, ui->icon.x, ui->icon.y);

    __wrn("测试  ui->rt.x=%d  ui->rt.y=%d\n", ui->rt.x, ui->rt.y);

    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);

    GUI_SetFont(SWFFont);

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    GUI_SetColor(GUI_BLACK);

    APP_Temp_ResBmpDraw(ID_PHOTO_FILEINFO_BG_BMP, ui->rt.x, ui->rt.y);

    x   = ui->txt.x + ui->rt.x; //相对于rt
    y   = ui->txt.y + ui->rt.y;

    //文件名
    {
        GUI_RECT    rt;
        char        *psrc = info.pic_name;
        int32_t     w = ui->txt_line_w;

        GUI_GetTextExtend(&rt, psrc, eLIBs_strlen(psrc));

        if (rt.x1 - rt.x0 > w)
        {
            char    buf_temp[MAX_FILE_NAME_LEN] = {0};

            rt.x0   = x;
            rt.x1   = rt.x0 + w * 2 - 20;//(w * (2 * 10 - 1) / 10);
            rt.y0   = y + 5;
            rt.y1   = y + ui->filename_sp + 25;

            GetClippedString(&rt, psrc, buf_temp, "...");

            rt.x1   = rt.x0 + w;

            __wrn("buf_temp=%s\n", buf_temp);
            GUI_DispStringInRectWrap(buf_temp, &rt, GUI_TA_LEFT | GUI_TA_TOP, GUI_WRAPMODE_CHAR);

            y += ui->filename_sp;
        }
        else
        {
            __wrn("psrc=%s\n", psrc);
            __wrn("x=%d, y=%d\n", x, y);

            GUI_DispStringAt(psrc, x, y + 5);

            y   += ui->filename_sp / 2;
        }
    }
    //GUI_DispStringAt(buf, x, y);
    //y += ui->txt_sp;
    //时间
    app_sprintf(buf1, FILE_INFO_TIME_STR_FORMAT, info.time.year, info.time.month, info.time.day,
                info.time.hour / 10, info.time.hour % 10, info.time.minute / 10, info.time.minute % 10);

    app_sprintf(buf, FILE_INFO_TXT_FORMAT, res->str_time, buf1);

    __wrn("buf=%s\n", buf);
    __wrn("x=%d, y=%d\n", x, y);

    GUI_DispStringAt(buf, x, y + 30);

    y   += ui->txt_sp;

    //文件大小
    filesize2str(info.pic.file_len, buf1);

    app_sprintf(buf, FILE_INFO_TXT_FORMAT, res->str_filesize, buf1);

    GUI_DispStringAt(buf, x, y + 37);

    __wrn("buf=%s\n", buf);
    __wrn("x=%d, y=%d\n", x, y);

    y   += ui->txt_sp;

    //分辨率
    app_sprintf(buf, FILE_INFO_TXT_SIZE_FORMAT, res->str_size, info.pic.size.width, info.pic.size.height);

    GUI_DispStringAt(buf, x, y + 44);

    __wrn("x=%d, y=%d\n", x, y);
    __wrn("buf=%s\n", buf);

    y   += ui->txt_sp;

    //文件位置
    app_sprintf(buf, FILE_INFO_TXT_FORMAT, res->str_path, info.path_name);

    {
        GUI_RECT    rt;
        char        *psrc = buf;
        int32_t     w = ui->txt_line_w;

        GUI_GetTextExtend(&rt, psrc, eLIBs_strlen(psrc));

        if (rt.x1 - rt.x0 > w)
        {
            char    buf_temp[MAX_FILE_NAME_LEN] = {0};

            rt.x0   = x;
            rt.x1   = rt.x0 + w * 2 - 20;//(w * (2 * 10 - 1) / 10);
            rt.y0   = y + 51;
            rt.y1   = y + ui->filename_sp;

            GetClippedString(&rt, psrc, buf_temp, "...");

            rt.x1 = rt.x0 + w;

            __wrn("buf_temp=%s\n", buf_temp);
            GUI_DispStringInRectWrap(buf_temp, &rt, GUI_TA_LEFT | GUI_TA_TOP, GUI_WRAPMODE_CHAR);

            y += ui->txt_sp * 2;
        }
        else
        {
            __wrn("psrc=%s\n", psrc);
            __wrn("x=%d, y=%d\n", x, y);

            GUI_DispStringAt(psrc, x, y + 51);

            y += ui->txt_sp;
        }
    }
}
