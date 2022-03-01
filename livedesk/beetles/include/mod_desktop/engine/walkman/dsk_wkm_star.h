/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_star.h
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
#ifndef  __DSK_WKM_STAR_H__
#define  __DSK_WKM_STAR_H__

#ifdef __cplusplus
extern "C" {
#endif


#define DSK_WKM_STAR_MAX           5


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
extern __s32  dsk_wkm_set_star(const char *file, __u32 star);

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
extern __s32  dsk_wkm_get_star(const char *file);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_STAR_H__

/* end of dsk_wkm_star.h */
