/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_star.c
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
#ifndef  __DSK_WKM_STAR_C__
#define  __DSK_WKM_STAR_C__


#include "dsk_walkman_i.h"



/*
****************************************************************************************************
*Name        : dsk_wkm_set_star
*Prototype   : __s32  dsk_wkm_set_star( const char *file, __u32 star )
*Arguments   : file      input. file name, full name.
*              star      input. star value and it rangs 0~~DSK_WKM_STAR_MAX.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the audio file's star.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_set_star(const char *file, __u32 star)
{
    __u8   err;
    __s32  ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_set_star(file, star);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_star
*Prototype   : __s32  dsk_wkm_get_star( const char *file )
*Arguments   : file    input. full path.
*Return      : the star of the file. If return value < 0, that means some error occured.
*Description : get the star of the file.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_get_star(const char *file)
{
    __u8   err;
    __s32  ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_get_star(file);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



#endif     //  __ifndef DSK_WKM_STAR_C__

/* end of dsk_wkm_star.c */
