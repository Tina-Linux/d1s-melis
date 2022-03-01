/*
************************************************************************************************************************
*                                                       walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_media_info_i.h
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
#ifndef  __DSK_WKM_MEDIA_INFO_I_H__
#define  __DSK_WKM_MEDIA_INFO_I_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : __dsk_wkm_get_tag
*Prototype   : __s32  __dsk_wkm_get_tag( __cedar_tag_inf_t  *tag_info_p )
*Arguments   : tag_info_p   output. a buffer to store the instant tag information.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the instant tag information.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_tag(__cedar_tag_inf_t  *tag_info_p);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_cur_time
*Prototype   : __s32  __dsk_wkm_get_cur_time( void )
*Arguments   : void
*Return      : the current playing time, unit : ms. -1 means error.
*Description : get the current playing time.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_cur_time(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_total_time
*Prototype   : __s32  __dsk_wkm_get_total_time( void )
*Arguments   : void
*Return      : the total time of the current playing file, unit : ms. -1 means error.
*Description : get the total time of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_total_time(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_audio_encoding
*Prototype   : __cedar_audio_fmt_t  __dsk_wkm_get_audio_encoding( void )
*Arguments   : void
*Return      : the audio encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_audio_fmt_t.
*Description : get the audio encoding of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __cedar_audio_fmt_t  __dsk_wkm_get_audio_encoding(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_audio_bps
*Prototype   : __u32  __dsk_wkm_get_audio_bps( void )
*Arguments   : void
*Return      : the audio instant bit rate of the current playing file.
*Description : get the audio instant bit rate of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __u32  __dsk_wkm_get_audio_bps(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_audio_sample_rate
*Prototype   : __u32  __dsk_wkm_get_audio_sample_rate( void )
*Arguments   : void
*Return      : the sample rate of the current playing file.
*Description : get the sample rate of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __u32  __dsk_wkm_get_audio_sample_rate(void);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_audio_info
*Prototype   : __s32  __dsk_wkm_get_audio_info( const char *file, dsk_wkm_audio_info_t *info_p )
*Arguments   : file      input.
*              info_p    output. a buffer to store the audio info.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the information of a audio file.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_audio_info(const char *file, dsk_wkm_audio_info_t *info_p);

/*
****************************************************************************************************
*Name        : __dsk_wkm_get_album_cover
*Prototype   : __s32  __dsk_wkm_get_album_cover( const char *file, void *buf, const SIZE *size_p,
*                                                dsk_wkm_pic_zoom_e zoom )
*Arguments   : file      input.
*              buf       output. to store the album cover matrix. ARGB mode.
*              size_p    input. The width and height info of the buffer
*              zoom      input. zoom mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the album cover matrix of a audio file.
*Other       :
****************************************************************************************************
*/
extern __s32  __dsk_wkm_get_album_cover(const char *file, void *buf, const SIZE *size_p, dsk_wkm_pic_zoom_e zoom);



#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_MEDIA_INFO_I_H__

/* end of dsk_wkm_media_info_i.h */
