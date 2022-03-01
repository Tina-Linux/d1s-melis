/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_sset.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/07/21 14:25  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __PHOTO_SSET_H__3E755E4D_EFFD_4F85_AFDC_86CE8D4989C8__
#define __PHOTO_SSET_H__3E755E4D_EFFD_4F85_AFDC_86CE8D4989C8__
//////////////////////////////////////////////////////////////////////////
#include "beetles_app.h"
#include "photo_uipara.h"
#include "app_anole.h"

#define ID_PHOTO_SCENE_SSET                2
//////////////////////////////////////////////////////////////////////////

/*notify message of sset*/
typedef enum
{
    PHOTO_SSET_NC_EXIT = 1,
    PHOTO_SSET_NC_BGMUSIC,
    PHOTO_SSET_NC_ZOOM,
    PHOTO_SSET_NC_SLIDESHOW,
    PHOTO_SSET_NC_FILEINFO,
    PHOTO_SSET_NC_AUTOPLAY,
    PHOTO_SSET_NC_INTERVAL,
    PHOTO_SSET_NC_SAVE_BG,
    PHOTO_SSET_NC_INTERVAL_USER
} __sset_nc_t;
#define PHOTO_SSET_NC_ID_UNKNOW    0xFF

extern  __sset_item_para_t  photo_sset_arry[];

__sset_create_para_t *Photo_SSET_Build_CreatePara(H_WIN  h_win);
//__s32 Photo_SSET_Destroy_CreatePara(__sset_create_para_t* create_para);
#define Photo_SSET_Destroy_CreatePara(_p) BFreeType(_p, __sset_create_para_t)

// __s32 Track_ListCreate(void);
// __s32 Subtitle_ListCreate(void);

//////////////////////////////////////////////////////////////////////////
#endif //__PHOTO_SSET_H__3E755E4D_EFFD_4F85_AFDC_86CE8D4989C8__
//End of this file

