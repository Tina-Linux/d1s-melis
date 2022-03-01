/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_star.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.28
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
* Gary.Wang      2009.09.28       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_star_h
#define  __robin_star_h


#define ROBIN_STAR_MAX           5


/***************************************************************************************************
*Name        : robin_set_star
*Prototype   : __s32  robin_set_star( const char *file, __u32 star )
*Arguments   : file      input. file name, full name.
*              star      input. star value and it rangs 0~~ROBIN_STAR_MAX.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the audio file's star.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_star(const char *file, __u32 star);

/***************************************************************************************************
*Name        : robin_get_star
*Prototype   : __s32  robin_get_star( const char *file )
*Arguments   : file    input. full path.
*Return      : the star of the file. If return value < 0, that means some error occured.
*Description : get the star of the file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_star(const char *file);



#endif     //  ifndef __robin_star_h

/* end of robin_star.h */
