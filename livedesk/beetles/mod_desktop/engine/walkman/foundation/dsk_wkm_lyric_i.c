/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_lyric_i.c
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
#ifndef  __DSK_WKM_LYRIC_I_C__
#define  __DSK_WKM_LYRIC_I_C__


#include "foundation_i.h"


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
__bool __dsk_wkm_lyric_exist(void)
{
    __s32  lyric_format;
    lyric_format = esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_LBSTYPE, 0, NULL);

    if (lyric_format == CEDAR_LRC_TYPE_NONE || lyric_format == CEDAR_LRC_TYPE_NOTSPT)
    {
        return EPDK_FALSE;
    }
    else
    {
        return EPDK_TRUE;
    }
}



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
__cedar_lyric_fmt_t  __dsk_wkm_get_lyric_format(void)
{
    __cedar_lyric_fmt_t  format;
    format = (__cedar_lyric_fmt_t)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_LBSTYPE, 0, NULL);
    return format;
}



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
__s32  __dsk_wkm_get_lyric_item_count(void)
{
    __cedar_buf_inf_t  *lyric_desc_p = NULL;
    lyric_desc_p = (__cedar_buf_inf_t *)(unsigned long)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_SUB_INFO, 0,
                   (void *)CEDAR_GET_SUB_INF_ALL);

    if (lyric_desc_p == NULL || lyric_desc_p->size <= 0)
    {
        __wrn("Fail in getting lyric info.\n");
        return -1;
    }

    return lyric_desc_p->size;
}



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
__s32  __dsk_wkm_get_lyric_item_list(__cedar_subtitle_item_t  *lyric_item_list, __u32 item_count)
{
    __cedar_buf_inf_t        *lyric_desc_p = NULL;
    __cedar_subtitle_item_t  *lyric_item_list_p = NULL;
    __u32                     count;

    if (lyric_item_list == NULL)
    {
        return 0;
    }

    lyric_desc_p = (__cedar_buf_inf_t *)(unsigned long)esMODS_MIoctrl(dsk_wkm_hced, CEDAR_CMD_GET_SUB_INFO, 0,
                   (void *)CEDAR_GET_SUB_INF_ALL);

    if (lyric_desc_p == NULL)
    {
        __wrn("Fail in getting lyric item list.\n");
        return 0;
    }

    lyric_item_list_p = (__cedar_subtitle_item_t *)(lyric_desc_p->buf);
    count = min(lyric_desc_p->size, item_count);
    g_memcpy(lyric_item_list, lyric_item_list_p, sizeof(__cedar_subtitle_item_t) * count);
    return count;
}



#endif     //  ifndef __DSK_WKM_LYRIC_I_C__

/* end of dsk_wkm_lyric_i.c */
