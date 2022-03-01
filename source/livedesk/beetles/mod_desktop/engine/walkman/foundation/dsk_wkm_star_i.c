/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_star_i.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.07
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.10.07        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_STAR_I_C__
#define  __DSK_WKM_STAR_I_C__


#include "foundation_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_set_star
*Prototype   : __s32  __dsk_wkm_set_star( const char *file, __u32 star )
*Arguments   : file      input. file name, full name.
*              star      input. star value and it rangs 0~~__DSK_WKM_STAR_MAX.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the audio file's star.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_set_star(const char *file, unsigned long star)
{
    signed long  UID;
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_ADD_FILE_TO_ORCHID, (signed long )file, &UID);

    if (ret != ORC_OK)
    {
        __wrn("Fail in setting star %d to file %s.\n", star, file);
        return -1;
    }

    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_SET_STAR, (signed long)UID, (void *)star);

    if (ret != ORC_OK)
    {
        __wrn("Fail in setting star %d to file %s.\n", star, file);
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_get_star
*Prototype   : __s32  __dsk_wkm_get_star( const char *file )
*Arguments   : file    input. full path.
*Return      : the star of the file. If return value < 0, that means some error occured.
*Description : get the star of the file.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_get_star(const char *file)
{
    __u32  UID;
    __s32  ret;
    media_info_t  media_info;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_ADD_FILE_TO_ORCHID, (signed long)file, &UID);

    if (ret != ORC_OK)
    {
        __wrn("Fail in getting star of file %s.\n", file);
        return -1;
    }

    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_GET_MEDIA_INFO, (__s32)UID, &media_info);

    if (ret != ORC_OK)
    {
        __wrn("Fail in getting of file %s.\n", file);
        return -1;
    }

    return media_info.audio_info.star;
}




#endif     //  ifndef __DSK_WKM_STAR_I_C__

/* end of dsk_wkm_star_i.c */
