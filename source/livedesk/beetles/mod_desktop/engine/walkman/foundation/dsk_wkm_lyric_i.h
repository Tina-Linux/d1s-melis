/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_lyric_i.h
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
#ifndef  __DSK_WKM_LYRIC_I_H__
#define  __DSK_WKM_LYRIC_I_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : __dsk_wkm_lyric_exist
*Prototype   : __bool __dsk_wkm_lyric_exist( void )
*Arguments   : void
*Return      : EPDK_TRUE      there are lyrics corresponding to the current playing file.
*              EPDK_FALSE     there is NO lyric corresponding to the current playing file.
*Description : check whether there are lyrics corresponding to the current playing file or not.
*Other       :
****************************************************************************************************
*/
extern __bool __dsk_wkm_lyric_exist(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_lyric_format
*Prototype   : __cedar_lyric_fmt_t  __dsk_wkm_get_lyric_format( void )
*Arguments   : void
*Return      : the lyric format of the current playing file. the formats are defined in mod_cedar.h,
*              see __cedar_lyric_fmt_t.
*Description : get the lyric format of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __cedar_lyric_fmt_t  __dsk_wkm_get_lyric_format(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_lyric_item_count
*Prototype   : __s32  __dsk_wkm_get_lyric_item_count( void )
*Arguments   : void
*Return      : the item count of the lyric corresponding to the current playing file. if return -1,
*              that means no lyric or lyric information has NOT been parsed completely.
*Description : get the item count of the lyric corresponding to the current playing file.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_lyric_item_count(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_lyric_item_list
*Prototype   : __s32  __dsk_wkm_get_lyric_item_list( __cedar_subtitle_item_t  *lyric_item_list, __u32 item_count )
*Arguments   : lyric_item_list    output. an array of lyric item.
*              item_count         input. the item count of the array.
*Return      : the real count of items in the array. if return -1, that means no lyric or lyric
*              information has NOT been parsed completely.
*Description : get lyric item list.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_lyric_item_list(__cedar_subtitle_item_t  *lyric_item_list, __u32 item_count);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_LYRIC_I_H__

/* end of dsk_wkm_lyric_i.h */
