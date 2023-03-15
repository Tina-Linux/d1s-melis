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
#ifndef  __robin_media_info_c
#define  __robin_media_info_c
#include "robin_i.h"
#include <string.h>
#include <mediainfo/eLIBs_GetAudioFmt.h>
#include <mediainfo/mediafmt.h>
#include "libc/elibs_charset.h"
#include <kconfig.h>

static __bool is_all_blank(const char *str, __u32 len);
static __epdk_charset_enm_e   charset_convert(const char *src, __u32 src_size, __s32 src_encoding, char *dst, __u32 dst_size, __u32 *len_p);
static __s32  robin_get_audio_info_from_orchid(const char *file, robin_audio_info_t *info_p);
static __s32  robin_get_audio_info_from_file(const char *file, robin_audio_info_t *info_p);
static __sw_pic_format_e      pic_format_map_a(__id3_img_format_t  format);
static __willow_img_format_t  pic_format_map_b(__sw_pic_format_e  format);


/***************************************************************************************************
*Name        : robin_get_tag
*Prototype   : __s32  robin_get_tag( __cedar_tag_inf_t  *tag_info_p )
*Arguments   : tag_info_p   output. a buffer to store the instant tag information.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the instant tag information.
*Other       :
***************************************************************************************************/
__s32  robin_get_tag(__cedar_tag_inf_t  *tag_info_p)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (tag_info_p == NULL)
    {
        return -1;
    }

    if (esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_TAG, 0, tag_info_p) != EPDK_OK)
    {
        return -1;
    }

    return 0;
}



/***************************************************************************************************
*Name        : robin_get_cur_time
*Prototype   : __u32  robin_get_cur_time( void )
*Arguments   : void
*Return      : the current playing time, unit : ms.
*Description : get the current playing time.
*Other       :
***************************************************************************************************/
__u32  robin_get_cur_time(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    return (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_CUR_TIME, 0, NULL);
}



/***************************************************************************************************
*Name        : robin_get_total_time
*Prototype   : __u32  robin_get_total_time( void )
*Arguments   : void
*Return      : the total time of the current playing file, unit : ms. 0 means error.
*Description : get the total time of the current playing file.
*Other       :
***************************************************************************************************/
__u32  robin_get_total_time(void)
{
    __u32  time;

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    if (get_msg_nr_of_q(robin_cmdQ) != 0)
    {
        return 0;
    }

    time = (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_TOTAL_TIME, 0, NULL);
    return time;
}



/***************************************************************************************************
*Name        : robin_get_audio_encoding
*Prototype   : __cedar_audio_fmt_t  robin_get_audio_encoding( void )
*Arguments   : void
*Return      : the audio encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_audio_fmt_t.
*Description : get the audio encoding of the current playing file.
*Other       :
***************************************************************************************************/
__cedar_audio_fmt_t  robin_get_audio_encoding(void)
{
    __cedar_audio_fmt_t  ret;
    robin_wait_no_file();
    ret = (__cedar_audio_fmt_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_ABSTYPE, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



/***************************************************************************************************
*Name        : robin_get_audio_bps
*Prototype   : __u32  robin_get_audio_bps( void )
*Arguments   : void
*Return      : the audio instant bit rate of the current playing file.
*Description : get the audio instant bit rate of the current playing file.
*Other       :
***************************************************************************************************/
__u32  robin_get_audio_bps(void)
{
    __u32  ret;
    __u8   err;
    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_AUDBPS, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



/***************************************************************************************************
*Name        : robin_get_audio_sample_rate
*Prototype   : __u32  robin_get_audio_sample_rate( void )
*Arguments   : void
*Return      : the sample rate of the current playing file.
*Description : get the sample rate of the current playing file.
*Other       :
***************************************************************************************************/
__u32  robin_get_audio_sample_rate(void)
{
    return (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_SAMPRATE, 0, NULL);
}



/***************************************************************************************************
*Name        : robin_get_video_encoding
*Prototype   : __cedar_video_fmt_t  robin_get_video_encoding( void )
*Arguments   : void
*Return      : the video encoding of the current playing file. The encoding value are defined in
*              mod_cedar.h, see __cedar_video_fmt_t.
*Description : get the video encoding of the current playing file.
*Other       :
***************************************************************************************************/
__cedar_video_fmt_t  robin_get_video_encoding(void)
{
    __cedar_video_fmt_t  ret;
    __u8   err;
    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = (__cedar_video_fmt_t)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_VBSTYPE, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



/***************************************************************************************************
*Name        : robin_get_video_bps
*Prototype   : __u32  robin_get_video_bps( void )
*Arguments   : void
*Return      : the instant video bit rate of the current playing file.
*Description : get the instant video bit rate of the current playing file.
*Other       :
***************************************************************************************************/
__u32  robin_get_video_bps(void)
{
    __u32  ret;
    __u8   err;

    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    __msg("**********before CEDAR_CMD_GET_VIDBITRATE********\n");
    ret = (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_VIDBITRATE, 0, NULL);
    __msg("**********after CEDAR_CMD_GET_VIDBITRATE********\n");
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



/***************************************************************************************************
*Name        : robin_get_video_frame_rate
*Prototype   : __u32  robin_get_video_frame_rate( void )
*Arguments   : void
*Return      : the video frame rate of the current playing file.
*Description : get the video frame rate of the current playing file.
*Other       :
***************************************************************************************************/
__u32  robin_get_video_frame_rate(void)
{
    __u32  ret;
    __u8   err;
    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    ret = (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_VIDFPS, 0, NULL);
    g_post_mutex(robin_cedar_mutex);
    return ret;
}



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
__s32  robin_get_video_frame_size(__u32 *width_p, __u32 *height_p)
{
    __u32  size;
    __u8   err;

    if (width_p == NULL || height_p == NULL)
    {
        return -1;
    }

    g_pend_mutex(robin_cedar_mutex, &err);
    robin_wait_no_file();
    size = (__u32)esMODS_MIoctrl(robin_hced, CEDAR_CMD_GET_FRAMESIZE, 0, NULL);
    *width_p  = (size & (0xFFFFU << 16)) >> 16;
    *height_p = size & 0xFFFF;
    g_post_mutex(robin_cedar_mutex);
    return 0;
}



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
__s32  robin_get_audio_info(const char *file, robin_audio_info_t *info_p)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (file == NULL || info_p == NULL)
    {
        return -1;
    }

    if (robin_get_audio_info_from_orchid(file, info_p) == 0)
    {
        return 0;
    }

    if (robin_get_audio_info_from_file(file, info_p) == 0)
    {
        return 0;
    }

    return -1;
}



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
__s32  robin_get_album_cover(const char *file, void *buf, const SIZE *size_p, robin_pic_zoom_e zoom)
{
    __willow_get_albumart_param_t album_para;
    __willow_show_file_param_t check_para;
    robin_audio_info_t  audio_inf;
    __u8 status;

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    if (file == NULL || buf == NULL || size_p == NULL)
    {
        return -1;
    }

    if (robin_get_audio_info(file, &audio_inf))
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
        album_para.album_art_info.enable = 1;
        album_para.album_art_info.img_type = pic_format_map_b(audio_inf.album_cover_info.format);
        album_para.album_art_info.length = (__u32)audio_inf.album_cover_info.length;
        album_para.album_art_info.offset = (__u32)audio_inf.album_cover_info.offset;

        while (1)
        {
            esKRNL_TimeDly(1);
            status = esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

            if (status != WILLOW_STATUS_BUSY)
            {
                break;
            }
        }

        esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_SET_ALBUM_ART, 0, &album_para);

        while (1)
        {
            esKRNL_TimeDly(1);
            status = esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

            if (status == WILLOW_STATUS_FINISH)
            {
                break;
            }
        }

        if (album_para.thumbs.filename)
        {
            check_para.filename = (char *)file;
            check_para.img_no = robin_npl_file2index(file);

            if (esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_CHECK_IMG, 0, &check_para) == EPDK_FAIL) //img not exist
            {
                return EPDK_FAIL;
            }

            while (1)
            {
                esKRNL_TimeDly(1);
                status = esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

                if (status != WILLOW_STATUS_BUSY)
                {
                    break;
                }
            }

            esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_START_DEC, 0, 0);
            esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_START_SHOW_EXT, 0, (void *)WILLOW_SCALE_RATIO_SCALE);

            while (1)
            {
                esKRNL_TimeDly(1);
                status = esMODS_MIoctrl(robin_hwlo, WILLOW_CMD_QUERY_STATUS, 0, 0);

                if (status == WILLOW_STATUS_FINISH)
                {
                    break;
                }
            }

            return 0;
        }

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

static __attribute__((optimize("O0"))) __s32 robin_get_audio_info_from_file(const char *file, robin_audio_info_t *info_p)
{
    __audio_file_info_t aif;
    __u32               len;
    char                info_buf[SZ_1K];

    if (file == NULL || info_p == NULL)
    {
        return -1;
    }

    memset(info_buf, 0x00, SZ_1K);
    aif.InforBufLeftLength = sizeof(info_buf);
    aif.InforBuf           = info_buf;

    if (!eLIBs_GetAudioInfo(file, &aif))          // fail in getting information
    {
        __wrn("Fail in extracting audio info of file %s.\n", file);
        return -1;
    }

    info_p->title_charset = charset_convert(aif.ultitle, aif.ultitle_sz,
                                            aif.ultitleCharEncode, info_p->title, ROBIN_TITLE_SIZE, &len);

    if (is_all_blank(info_p->title, len))
    {
        info_p->artist[0] = '\0';
    }

    info_p->artist_charset = charset_convert(aif.ulauthor, aif.ulauthor_sz,
                             aif.ulauthorCharEncode, info_p->artist, ROBIN_ARTIST_SIZE, &len);

    if (is_all_blank(info_p->artist, len))
    {
        info_p->artist[0] = '\0';
    }

    info_p->genre_charset = charset_convert(aif.ulGenre, aif.ulGenre_sz,
                                            aif.ulGenreCharEncode, info_p->genre, ROBIN_GENRE_SIZE, &len);

    if (is_all_blank(info_p->genre, len))
    {
        info_p->genre[0] = '\0';
    }

    info_p->album_charset = charset_convert(aif.ulAlbum, aif.ulAlbum_sz,
                                            aif.ulAlbumCharEncode, info_p->album, ROBIN_ALBUM_SIZE, &len);

    if (is_all_blank(info_p->album, len))
    {
        info_p->album[0] = '\0';
    }

    info_p->year_charset = charset_convert(aif.ulYear, aif.ulYear_sz,
                                           aif.ulYearCharEncode, info_p->year, ROBIN_YEAR_SIZE, &len);

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



static __s32  robin_get_audio_info_from_orchid(const char *file, robin_audio_info_t *info_p)
{
    media_info_t    media_info;
    __u32           UID;
    return -1;
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



#endif     //  ifndef __robin_media_info_c

/* end of robin_media_info.c */
