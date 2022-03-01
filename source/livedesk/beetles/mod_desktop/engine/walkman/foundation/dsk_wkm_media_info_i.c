/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_media_info_i.c
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
#ifndef  __DSK_WKM_MEDIA_INFO_I_C__
#define  __DSK_WKM_MEDIA_INFO_I_C__

#include "foundation_i.h"
#include <mediainfo/eLIBs_GetAudioFmt.h>
#include <mediainfo/mediafmt.h>
#include <libc/elibs_charset.h>

static __bool is_all_blank(const char *str, __u32 len);
static __epdk_charset_enm_e   charset_convert(const char *src, __u32 src_size, __s32 src_encoding, char *dst, __u32 dst_size, __u32 *len_p);
static __s32  __dsk_wkm_get_audio_info_from_orchid(const char *file, dsk_wkm_audio_info_t *info_p);
static __s32  __dsk_wkm_get_audio_info_from_file(const char *file, dsk_wkm_audio_info_t *info_p);
static __sw_pic_format_e      pic_format_map_a(__id3_img_format_t  format);
static __willow_img_format_t  pic_format_map_b(__sw_pic_format_e  format);


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
__s32  __dsk_wkm_get_tag(__cedar_tag_inf_t  *tag_info_p)
{
    if (tag_info_p == NULL)
    {
        return -1;
    }

    if (esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_TAG, 0, tag_info_p) != EPDK_OK)
    {
        return -1;
    }

    return 0;
}

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
__s32  __dsk_wkm_get_cur_time(void)
{
    return (__s32)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_CUR_TIME, 0, NULL);
}



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
__s32  __dsk_wkm_get_total_time(void)
{
    __s32  time;
    time = (__s32)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_TOTAL_TIME, 0, NULL);
    return time;
}



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
__cedar_audio_fmt_t  __dsk_wkm_get_audio_encoding(void)
{
    __cedar_audio_fmt_t  ret;
    ret = (__cedar_audio_fmt_t)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_ABSTYPE, 0, NULL);
    return ret;
}



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
__u32  __dsk_wkm_get_audio_bps(void)
{
    __u32  ret;
    ret = (__u32)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_AUDBPS, 0, NULL);
    return ret;
}



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
__u32  __dsk_wkm_get_audio_sample_rate(void)
{
    return (__u32)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_SAMPRATE, 0, NULL);
}



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
__s32  __dsk_wkm_get_audio_info(const char *file, dsk_wkm_audio_info_t *info_p)
{
    if (file == NULL || info_p == NULL)
    {
        return -1;
    }

    if (__dsk_wkm_get_audio_info_from_orchid(file, info_p) == 0)
    {
        return 0;
    }

    if (__dsk_wkm_get_audio_info_from_file(file, info_p) == 0)
    {
        return 0;
    }

    return -1;
}



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
__s32  __dsk_wkm_get_album_cover(const char *file, void *buf, const SIZE *size_p, dsk_wkm_pic_zoom_e zoom)
{
    __willow_get_albumart_param_t album_para;
    __willow_show_file_param_t check_para;
    dsk_wkm_audio_info_t  audio_inf;
    __u8 status;

    if (file == NULL || buf == NULL || size_p == NULL)
    {
        return -1;
    }

    if (__dsk_wkm_get_audio_info(file, &audio_inf))
    {
        return -1;
    }

    album_para.thumbs.buf = (__u8 *)buf;
    album_para.thumbs.filename = (char *)file;
    album_para.thumbs.format = PIXEL_COLOR_ARGB8888;
    album_para.thumbs.size.width = size_p->width;
    album_para.thumbs.size.height = size_p->height;

    if (audio_inf.album_cover_info.length != 0)
    {
        __mp     *dsk_wkm_hwlo = NULL;
        __u8      mid_wlo = 0;
        /* load willow */
        mid_wlo = esMODS_MInstall(WILLOW_FILE_PATH, 0);

        if (mid_wlo == 0)
        {
            __err("Error in loading willow module!\n");
            return -1;
        }

        dsk_wkm_hwlo = esMODS_MOpen(mid_wlo, 0);

        if (dsk_wkm_hwlo == NULL)
        {
            __err("Error in opening a handle of willow!\n");
            esMODS_MUninstall(mid_wlo);
            mid_wlo = 0;
            return -1;
        }

        album_para.album_art_info.enable = 1;
        album_para.album_art_info.img_type = pic_format_map_b(audio_inf.album_cover_info.format);
        album_para.album_art_info.length = (__u32)audio_inf.album_cover_info.length;
        album_para.album_art_info.offset = (__u32)audio_inf.album_cover_info.offset;

        while (1)
        {
            esKRNL_TimeDly(1);
            status = esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

            if (status != WILLOW_STATUS_BUSY)
            {
                break;
            }
        }

        esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_SET_ALBUM_ART, 0, &album_para);

        while (1)
        {
            esKRNL_TimeDly(1);
            status = esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

            if (status == WILLOW_STATUS_FINISH)
            {
                break;
            }
        }

        if (album_para.thumbs.filename)
        {
            check_para.filename = (char *)file;
            check_para.img_no = __dsk_wkm_npl_file2index(file);

            if (esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_CHECK_IMG, 0, &check_para) == EPDK_FAIL) //img not exist
            {
                esMODS_MClose(dsk_wkm_hwlo);
                esMODS_MUninstall(mid_wlo);
                return EPDK_FAIL;
            }

            while (1)
            {
                esKRNL_TimeDly(1);
                status = esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

                if (status != WILLOW_STATUS_BUSY)
                {
                    break;
                }
            }

            esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_START_DEC, 0, 0);
            esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_START_SHOW_EXT, 0, (void *)WILLOW_SCALE_RATIO_SCALE);

            while (1)
            {
                esKRNL_TimeDly(1);
                status = esMODS_MIoctrl(dsk_wkm_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

                if (status == WILLOW_STATUS_FINISH)
                {
                    break;
                }
            }

            esMODS_MClose(dsk_wkm_hwlo);
            esMODS_MUninstall(mid_wlo);
            return 0;
        }

        esMODS_MClose(dsk_wkm_hwlo);
        esMODS_MUninstall(mid_wlo);
        return -1;
    }

    return -1;
}



static __bool is_all_blank(const char *str, __u32 len)
{
    __u32   i;

    for (i = 0;  i < len;  i++)
    {
        if (str[i] != ' ')
        {
            return EPDK_FALSE;
        }
    }

    return EPDK_TRUE;
}



static __epdk_charset_enm_e   charset_convert(const char *src, __u32 src_size, __s32 src_encoding, char *dst, __u32 dst_size, __u32 *len_p)
{
    __u32  len;

    if (src == NULL || src_size == 0 || dst == NULL || dst_size == 0)
    {
        *len_p = 0;
        return EPDK_CHARSET_ENM_UNKNOWN;
    }

    len = min(src_size, dst_size - 1);

    switch (src_encoding)
    {
        case A_AUDIO_FONTTYPE_ISOIEC8859_1 :
            g_strncpy((char *)dst, (char *)src, len);
            dst[len] = '\0';
            *len_p = len;
            return EPDK_CHARSET_ENM_UNKNOWN;

        case A_AUDIO_FONTTYPE_UTF_16LE     :
            len = eLIBs_charset_convert(EPDK_CHARSET_ENM_UTF16LE, EPDK_CHARSET_ENM_UTF8,
                                        (__u8 *)src, src_size, (__u8 *)dst, dst_size - 1);
            dst[len] = '\0';
            *len_p = len;
            return EPDK_CHARSET_ENM_UTF8;

        case A_AUDIO_FONTTYPE_UTF_16BE     :
            len = eLIBs_charset_convert(EPDK_CHARSET_ENM_UTF16BE, EPDK_CHARSET_ENM_UTF8,
                                        (__u8 *)src, src_size, (__u8 *)dst, dst_size - 1);
            dst[len] = '\0';
            *len_p = len;
            return EPDK_CHARSET_ENM_UTF8;

        case A_AUDIO_FONTTYPE_UTF_8        :
            g_strncpy((char *)dst, (char *)src, len);
            dst[len] = '\0';
            *len_p = len;
            return EPDK_CHARSET_ENM_UTF8;

        default  :
            g_strncpy((char *)dst, (char *)src, len);
            dst[len] = '\0';
            *len_p = len;
            return EPDK_CHARSET_ENM_UNKNOWN;
    }
}



static __s32  __dsk_wkm_get_audio_info_from_file(const char *file, dsk_wkm_audio_info_t *info_p)
{
    __audio_file_info_t aif;
    __u32               len;
    char                info_buf[SZ_1K];

    if (file == NULL || info_p == NULL)
    {
        return -1;
    }

    aif.InforBufLeftLength = sizeof(info_buf);
    aif.InforBuf           = info_buf;

    if (!eLIBs_GetAudioInfo(file, &aif))          // fail in getting information
    {
        __wrn("Fail in extracting audio info of file %s.\n", file);
        return -1;
    }

    info_p->title_charset = charset_convert(aif.ultitle, aif.ultitle_sz,
                                            aif.ultitleCharEncode, info_p->title, DSK_WKM_TITLE_SIZE, &len);

    if (is_all_blank(info_p->title, len))
    {
        info_p->artist[0] = '\0';
    }

    info_p->artist_charset = charset_convert(aif.ulauthor, aif.ulauthor_sz,
                             aif.ulauthorCharEncode, info_p->artist, DSK_WKM_ARTIST_SIZE, &len);

    if (is_all_blank(info_p->artist, len))
    {
        info_p->artist[0] = '\0';
    }

    info_p->genre_charset = charset_convert(aif.ulGenre, aif.ulGenre_sz,
                                            aif.ulGenreCharEncode, info_p->genre, DSK_WKM_GENRE_SIZE, &len);

    if (is_all_blank(info_p->genre, len))
    {
        info_p->genre[0] = '\0';
    }

    info_p->album_charset = charset_convert(aif.ulAlbum, aif.ulAlbum_sz,
                                            aif.ulAlbumCharEncode, info_p->album, DSK_WKM_ALBUM_SIZE, &len);

    if (is_all_blank(info_p->album, len))
    {
        info_p->album[0] = '\0';
    }

    info_p->year_charset = charset_convert(aif.ulYear, aif.ulYear_sz,
                                           aif.ulYearCharEncode, info_p->year, DSK_WKM_YEAR_SIZE, &len);

    if (is_all_blank(info_p->year, len))
    {
        info_p->year[0] = '\0';
    }

    info_p->format      = (__sw_audio_format_e)aif.ulFormat;
    info_p->duration    = aif.ulDuration;
    info_p->bps         = aif.ulBitRate;
    info_p->sample_rate = aif.ulSampleRate;

    /* extract album cover pic */
    if (aif.ulAPic_sz == 0)
    {
        info_p->album_cover_info.length = 0;
    }
    else
    {
        info_p->album_cover_info.length = aif.ulAPic->length;
        info_p->album_cover_info.offset = aif.ulAPic->FileLocation;
        info_p->album_cover_info.format = pic_format_map_a(aif.ulAPic->img_format);
    }

    return 0;
}



static __s32  __dsk_wkm_get_audio_info_from_orchid(const char *file, dsk_wkm_audio_info_t *info_p)
{
    media_info_t    media_info;
    __u32           UID;

    if (dsk_wkm_horc == NULL)
    {
        return -1;
    }

    UID = g_fioctrl(dsk_wkm_horc, ORC_CMD_FILE_TO_UID, 0, (void *)file);

    if (UID == (__u32) - 1)
    {
        return -1;
    }

    if (g_fioctrl(dsk_wkm_horc, ORC_CMD_GET_MEDIA_INFO, (__s32)UID, &media_info) != ORC_OK)
    {
        return -1;
    }

    g_strncpy(info_p->title, media_info.audio_info.title,
              min(DSK_WKM_TITLE_SIZE, ORC_TITLE_SIZE));
    info_p->title[min(DSK_WKM_TITLE_SIZE, ORC_TITLE_SIZE)] = '\0';
    info_p->title_charset = (__epdk_charset_enm_e)media_info.audio_info.title_encoding;
    g_strncpy(info_p->artist, media_info.audio_info.artist,
              min(DSK_WKM_ARTIST_SIZE, ORC_ARTIST_SIZE));
    info_p->artist[min(DSK_WKM_ARTIST_SIZE, ORC_ARTIST_SIZE)] = '\0';
    info_p->artist_charset = (__epdk_charset_enm_e)media_info.audio_info.artist_encoding;
    g_strncpy(info_p->album, media_info.audio_info.album,
              min(DSK_WKM_ALBUM_SIZE, ORC_ALBUM_SIZE));
    info_p->album[min(DSK_WKM_ALBUM_SIZE, ORC_ALBUM_SIZE)] = '\0';
    info_p->album_charset = (__epdk_charset_enm_e)media_info.audio_info.album_encoding;
    g_strncpy(info_p->genre, media_info.audio_info.genre,
              min(DSK_WKM_GENRE_SIZE, ORC_GENRE_SIZE));
    info_p->genre[min(DSK_WKM_GENRE_SIZE, ORC_GENRE_SIZE)] = '\0';
    info_p->genre_charset = (__epdk_charset_enm_e)media_info.audio_info.genre_encoding;

    if (media_info.audio_info.year == 0  || media_info.audio_info.year > 3000)
    {
        info_p->year[0] = '\0';
        info_p->year_charset = EPDK_CHARSET_ENM_UNKNOWN;
    }
    else
    {
        eLIBs_uint2str_dec(media_info.audio_info.year, info_p->year);
        info_p->year_charset = EPDK_CHARSET_ENM_UTF8;
    }

    info_p->format      = (__sw_audio_format_e)media_info.audio_info.file_format;
    info_p->duration    = media_info.audio_info.duration;
    info_p->bps         = media_info.audio_info.rate;
    info_p->sample_rate = media_info.audio_info.sample_rate;
    info_p->album_cover_info.length = media_info.audio_info.album_cover_info.length;
    info_p->album_cover_info.offset = media_info.audio_info.album_cover_info.offset;
    info_p->album_cover_info.format = media_info.audio_info.album_cover_info.format;
    return 0;
}



static __sw_pic_format_e  pic_format_map_a(__id3_img_format_t  format)
{
    switch (format)
    {
        case ID3_IMG_FORMAT_BMP :
            return SW_PIC_FORMAT_BMP;

        case ID3_IMG_FORMAT_JPG :
            return SW_PIC_FORMAT_JPG;

        case ID3_IMG_FORMAT_GIF :
            return SW_PIC_FORMAT_GIF;

        case ID3_IMG_FORMAT_PNG :
            return SW_PIC_FORMAT_PNG;

        default :
            return SW_PIC_FORMAT_UNKNOWN;
    }
}



static __willow_img_format_t  pic_format_map_b(__sw_pic_format_e  format)
{
    switch (format)
    {
        case SW_PIC_FORMAT_BMP :
            return IMG_FORMAT_BMP;

        case SW_PIC_FORMAT_JPG :
            return IMG_FORMAT_JPG;

        case SW_PIC_FORMAT_GIF :
            return IMG_FORMAT_GIF;

        case SW_PIC_FORMAT_PNG :
            return IMG_FORMAT_PNG;

        default :
            return IMG_FORMAT_UNSUPPORT;
    }
}





#endif     //  ifndef __DSK_WKM_MEDIA_INFO_I_C__

/* end of dsk_wkm_media_info_i.c */
