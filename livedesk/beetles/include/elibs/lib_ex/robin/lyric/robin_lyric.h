/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_lyric.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.27
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
* Gary.Wang      2009.09.27       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_lyric_h
#define  __robin_lyric_h


/***************************************************************************************************
*Name        : robin_lyric_exist
*Prototype   : __bool robin_lyric_exist( void )
*Arguments   : void
*Return      : EPDK_TRUE      there are lyrics corresponding to the current playing file.
*              EPDK_FALSE     there is NO lyric corresponding to the current playing file.
*Description : check whether there are lyrics corresponding to the current playing file or not.
*Other       :
***************************************************************************************************/
extern __bool robin_lyric_exist(void);

/***************************************************************************************************
*Name        : robin_get_lyric_format
*Prototype   : __cedar_lyric_fmt_t  robin_get_lyric_format( void )
*Arguments   : void
*Return      : the lyric format of the current playing file. the formats are defined in mod_cedar.h,
*              see __cedar_lyric_fmt_t.
*Description : get the lyric format of the current playing file.
*Other       :
***************************************************************************************************/
extern __cedar_lyric_fmt_t  robin_get_lyric_format(void);

/***************************************************************************************************
*Name        : robin_get_lyric_item_count
*Prototype   : __s32  robin_get_lyric_item_count( void )
*Arguments   : void
*Return      : the item count of the lyric corresponding to the current playing file. if return -1,
*              that means no lyric or lyric information has NOT been parsed completely.
*Description : get the item count of the lyric corresponding to the current playing file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_lyric_item_count(void);

/***************************************************************************************************
*Name        : robin_get_lyric_item_list
*Prototype   : __u32  robin_get_lyric_item_list( __cedar_subtitle_item_t  *lyric_item_list, __s32 item_count )
*Arguments   : lyric_item_list    output. an array of lyric item.
*              item_count         input. the item count of the array.
*Return      : the real count of items in the array. if return -1, that means no lyric or lyric
*              information has NOT been parsed completely.
*Description : get lyric item list.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_lyric_item_list(__cedar_subtitle_item_t  *lyric_item_list, __u32 item_count);

#endif     //  ifndef __robin_lyric_h

/* end of robin_lyric.h */
