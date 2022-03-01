/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_media_info.h
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
#ifndef  __robin_media_info_h
#define  __robin_media_info_h

#include <emodules/mod_charset.h>
#include <mediainfo/mediafmt.h>

typedef enum
{
    ROBIN_PIC_ZOOM_BUFFER_RATIO = 0,
    ROBIN_PIC_ZOOM_ORIGINAL_RATIO,

    ROBIN_PIC_ZOOM_
} robin_pic_zoom_e;


#define ROBIN_TITLE_SIZE               63
#define ROBIN_ARTIST_SIZE              63
#define ROBIN_ALBUM_SIZE               63
#define ROBIN_GENRE_SIZE               63
#define ROBIN_YEAR_SIZE                15


typedef struct _robin_id3_pic_info_t
{
    __u32               length;      // 0 means no picture. The length of the picture info in the file.
    __u32               offset;      // The offset of the picture info in the file.
    __sw_pic_format_e   format;      // jpg, bmp, and so on
} robin_id3_pic_info_t;


typedef struct _robin_audio_info_t
{
    char                  title[ROBIN_TITLE_SIZE + 1];      // empty string : title is unknown.
    __epdk_charset_enm_e  title_charset;
    char                  artist[ROBIN_ARTIST_SIZE + 1];    // empty string : artist is unknown.
    __epdk_charset_enm_e  artist_charset;
    char                  album[ROBIN_ALBUM_SIZE + 1];      // empty string : album is unknown.
    __epdk_charset_enm_e  album_charset;
    char                  genre[ROBIN_GENRE_SIZE + 1];      // empty string : genre is unknown.
    __epdk_charset_enm_e  genre_charset;
    char                  year[ROBIN_YEAR_SIZE + 1];        // empty string : year is unknown.
    __epdk_charset_enm_e  year_charset;
    __u32                 duration;                         // 0 : the duration is unknown.
    __u32                 bps;
    __u32                 sample_rate;
    __sw_audio_format_e   format;
    robin_id3_pic_info_t  album_cover_info;
} robin_audio_info_t;


/***************************************************************************************************
*Name        : robin_get_tag
*Prototype   : __s32  robin_get_tag( __cedar_tag_inf_t  *tag_info_p )
*Arguments   : tag_info_p   output. a buffer to store the instant tag information.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the instant tag information.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_tag(__cedar_tag_inf_t  *tag_info_p);

/***************************************************************************************************
*Name        : robin_get_cur_time
*Prototype   : __u32  robin_get_cur_time( void )
*Arguments   : void
*Return      : the current playing time, unit : ms.
*Description : get the current playing time.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_cur_time(void);

/***************************************************************************************************
*Name        : robin_get_total_time
*Prototype   : __u32  robin_get_total_time( void )
*Arguments   : void
*Return      : the total time of the current playing file, unit : ms. 0 means error.
*Description : get the total time of the current playing file.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_total_time(void);

/***************************************************************************************************
*Name        : robin_get_tag
*Prototype   : __s32  robin_get_tag( __cedar_tag_inf_t *tag_info_p )
*Arguments   : tag_info_p   output. a buf to store tag info.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the instant tag information.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_tag(__cedar_tag_inf_t *tag_info_p);

/***************************************************************************************************
*Name        : robin_get_audio_encoding
*Prototype   : __cedar_audio_fmt_t  robin_get_audio_encoding( void )
*Arguments   : void
*Return      : the audio encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_audio_fmt_t.
*Description : get the audio encoding of the current playing file.
*Other       :
***************************************************************************************************/
extern __cedar_audio_fmt_t  robin_get_audio_encoding(void);

/***************************************************************************************************
*Name        : robin_get_audio_bps
*Prototype   : __u32  robin_get_audio_bps( void )
*Arguments   : void
*Return      : the audio instant bit rate of the current playing file.
*Description : get the audio instant bit rate of the current playing file.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_audio_bps(void);

/***************************************************************************************************
*Name        : robin_get_audio_sample_rate
*Prototype   : __u32  robin_get_audio_sample_rate( void )
*Arguments   : void
*Return      : the sample rate of the current playing file.
*Description : get the sample rate of the current playing file.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_audio_sample_rate(void);

/***************************************************************************************************
*Name        : robin_get_video_encoding
*Prototype   : __cedar_video_fmt_t  robin_get_video_encoding( void )
*Arguments   : void
*Return      : the video encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_video_fmt_t.
*Description : get the video encoding of the current playing file.
*Other       :
***************************************************************************************************/
extern __cedar_video_fmt_t  robin_get_video_encoding(void);

/***************************************************************************************************
*Name        : robin_get_video_bps
*Prototype   : __u32  robin_get_video_bps( void )
*Arguments   : void
*Return      : the instant video bit rate of the current playing file.
*Description : get the instant video bit rate of the current playing file.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_video_bps(void);

/***************************************************************************************************
*Name        : robin_get_video_frame_rate
*Prototype   : __u32  robin_get_video_frame_rate( void )
*Arguments   : void
*Return      : the video frame rate of the current playing file.
*Description : get the video frame rate of the current playing file.
*Other       :
***************************************************************************************************/
extern __u32  robin_get_video_frame_rate(void);

/***************************************************************************************************
*Name        : robin_get_video_frame_size
*Prototype   : __s32  robin_get_video_frame_size( __u32 *width_p, __u32 *height_p )
*Arguments   : width_p        output. the width of the frame.
*              height_p       output. the height of the frame.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the video frame size of the current playing file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_video_frame_size(__u32 *width_p, __u32 *height_p);

/***************************************************************************************************
*Name        : robin_get_audio_info
*Prototype   : __s32  robin_get_audio_info( const char *file, robin_audio_info_t *info_p )
*Arguments   : file      input.
*              info_p    output. a buffer to store the audio info.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the information of a audio file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_audio_info(const char *file, robin_audio_info_t *info_p);

/***************************************************************************************************
*Name        : robin_get_album_cover
*Prototype   : __s32  robin_get_album_cover( const char *file, void *buf, const SIZE *size_p, robin_pic_zoom_e zoom )
*Arguments   : file      input.
*              buf       output. to store the album cover matrix. ARGB mode.
*              size_p    input. The width and height info of the buffer
*              zoom      input. zoom mode.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the album cover matrix of a audio file.
*Other       :
***************************************************************************************************/
extern __s32  robin_get_album_cover(const char *file, void *buf, const SIZE *size_p, robin_pic_zoom_e zoom);


#endif     //  ifndef __robin_media_info_h

/* end of robin_media_info.h */
