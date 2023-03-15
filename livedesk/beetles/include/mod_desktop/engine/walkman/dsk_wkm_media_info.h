/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef  __DSK_WKM_MEDIA_INFO_H__
#define  __DSK_WKM_MEDIA_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    DSK_WKM_PIC_ZOOM_BUFFER_RATIO = 0,
    DSK_WKM_PIC_ZOOM_ORIGINAL_RATIO,

    DSK_WKM_PIC_ZOOM_
}
dsk_wkm_pic_zoom_e;


#define DSK_WKM_TITLE_SIZE               63
#define DSK_WKM_ARTIST_SIZE              63
#define DSK_WKM_ALBUM_SIZE               63
#define DSK_WKM_GENRE_SIZE               63
#define DSK_WKM_YEAR_SIZE                15


typedef struct _dsk_wkm_id3_pic_info_t
{
    __u32               length;      // 0 means no picture. The length of the picture info in the file.
    __u32               offset;      // The offset of the picture info in the file.
    __sw_pic_format_e   format;      // jpg, bmp, and so on
} dsk_wkm_id3_pic_info_t;


typedef struct _dsk_wkm_audio_info_t
{
    char                    title[DSK_WKM_TITLE_SIZE + 1];      // empty string : title is unknown.
    __epdk_charset_enm_e    title_charset;
    char                    artist[DSK_WKM_ARTIST_SIZE + 1];    // empty string : artist is unknown.
    __epdk_charset_enm_e    artist_charset;
    char                    album[DSK_WKM_ALBUM_SIZE + 1];      // empty string : album is unknown.
    __epdk_charset_enm_e    album_charset;
    char                    genre[DSK_WKM_GENRE_SIZE + 1];      // empty string : genre is unknown.
    __epdk_charset_enm_e    genre_charset;
    char                    year[DSK_WKM_YEAR_SIZE + 1];        // empty string : year is unknown.
    __epdk_charset_enm_e    year_charset;
    __u32                   duration;                           // 0 : the duration is unknown.
    __u32                   bps;
    __u32                   sample_rate;
    __sw_audio_format_e     format;
    dsk_wkm_id3_pic_info_t  album_cover_info;
} dsk_wkm_audio_info_t;


__s32  dsk_wkm_get_now_playing_file(char *filename);


/*
****************************************************************************************************
*Name        : dsk_wkm_get_tag
*Prototype   : __s32  dsk_wkm_get_tag( __cedar_tag_inf_t  *tag_info_p )
*Arguments   : tag_info_p   output. a buffer to store the instant tag information.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the instant tag information.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_get_tag(__cedar_tag_inf_t  *tag_info_p);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_cur_time
*Prototype   : __s32  dsk_wkm_get_cur_time( void )
*Arguments   : void
*Return      : the current playing time, unit : ms. -1 means error.
*Description : get the current playing time.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_get_cur_time(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_total_time
*Prototype   : __s32  dsk_wkm_get_total_time( void )
*Arguments   : void
*Return      : the total time of the current playing file, unit : ms. -1 means error.
*Description : get the total time of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_get_total_time(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_encoding
*Prototype   : __cedar_audio_fmt_t  dsk_wkm_get_audio_encoding( void )
*Arguments   : void
*Return      : the audio encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_audio_fmt_t.
*Description : get the audio encoding of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __cedar_audio_fmt_t  dsk_wkm_get_audio_encoding(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_bps
*Prototype   : __u32  dsk_wkm_get_audio_bps( void )
*Arguments   : void
*Return      : the audio instant bit rate of the current playing file.
*Description : get the audio instant bit rate of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __u32  dsk_wkm_get_audio_bps(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_sample_rate
*Prototype   : __u32  dsk_wkm_get_audio_sample_rate( void )
*Arguments   : void
*Return      : the sample rate of the current playing file.
*Description : get the sample rate of the current playing file.
*Other       :
****************************************************************************************************
*/
extern __u32  dsk_wkm_get_audio_sample_rate(void);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_audio_info
*Prototype   : __s32  dsk_wkm_get_audio_info( const char *file, dsk_wkm_audio_info_t *info_p )
*Arguments   : file      input.
*              info_p    output. a buffer to store the audio info.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the information of a audio file.
*Other       :
****************************************************************************************************
*/
extern __s32  dsk_wkm_get_audio_info(const char *file, dsk_wkm_audio_info_t *info_p);

/*
****************************************************************************************************
*Name        : dsk_wkm_get_album_cover
*Prototype   : __s32  dsk_wkm_get_album_cover( const char *file, void *buf, const SIZE *size_p,
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
extern __s32  dsk_wkm_get_album_cover(const char *file, void *buf, const SIZE *size_p, dsk_wkm_pic_zoom_e zoom);




#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_WKM_MEDIA_INFO_H__

/* end of dsk_wkm_media_info.h */
