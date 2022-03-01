/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : movie_sset.h
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/07/21 14:25  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#ifndef __MOVIE_SSET_H__
#define __MOVIE_SSET_H__
//////////////////////////////////////////////////////////////////////////
#include "beetles_app.h"

/*notify message of sset*/
typedef enum
{
    MOVIE_SSET_NC_RETURN = 1,
    MOVIE_SSET_NC_PRE,
    MOVIE_SSET_NC_PLAY,
    MOVIE_SSET_NC_NEXT,
    MOVIE_SSET_NC_ZOOM,
    MOVIE_SSET_NC_SUBTITLE,
    MOVIE_SSET_NC_PLAYMODE,
    MOVIE_SSET_NC_CHANNEL,
    MOVIE_SSET_NC_TRACK,
    //MOVIE_SSET_NC_TVOUT,
    MOVIE_SSET_NC_AB,
    MOVIE_SSET_NC_LIST//二级菜单发过来的消息
} __movie_sset_nc_t;

typedef struct
{
    __s32 sub_pos;
    __s32 track;
    __s32 channel;
    __s32 playmode;
    __s32 zoom;
} __movie_sset_list_para_t;

#define MOVIE_SSET_NC_ID_UNKNOW    0xff

__sset_create_para_t *Movie_SSET_Build_CreatePara(H_WIN hManwin, __u32 sub_id
        , __movie_sset_list_para_t *sset_list_para);


__s32 Movie_SSET_Destroy_CreatePara(__sset_create_para_t *create_para);

#endif //__MOVIE_SSET_H__


