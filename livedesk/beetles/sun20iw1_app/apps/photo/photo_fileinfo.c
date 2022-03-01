/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_fileinfo.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/20 10:28  create this file, implements the fundamental interface;
**************************************************************************************************************
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
