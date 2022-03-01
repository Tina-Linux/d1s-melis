/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_fileinfo.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/20 10:27  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __PHOTO_FILEINFO_H__31F749BD_D62E_4981_94D7_0CA35F0B2AB3__
#define __PHOTO_FILEINFO_H__31F749BD_D62E_4981_94D7_0CA35F0B2AB3__

#include "photo_uipara.h"

//文件信息资源
typedef struct
{
    //  load_bmp_res_t bmp_bg;
    //  load_bmp_res_t bmp_icon;
    char           str_path[20];
    char           str_time[20];
    char           str_filesize[20];
    char           str_size[20];
} PHOTO_FILE_INFO_RES_T;

__s32 photo_file_info_create_res(PHOTO_FILE_INFO_RES_T *res);
__s32 photo_file_info_destroy_res(PHOTO_FILE_INFO_RES_T *res);
void  draw_photo_file_info(H_WIN hwnd, PHOTO_FILE_INFO_RES_T *res, PHOTO_INFO_UI_T *ui);

#endif //__PHOTO_FILEINFO_H__31F749BD_D62E_4981_94D7_0CA35F0B2AB3__
//End of this file

