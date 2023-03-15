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
#ifndef  __DSK_WKM_NPL_I_C__
#define  __DSK_WKM_NPL_I_C__



#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_add_item
*Prototype   : __s32  __dsk_wkm_npl_add_item( const char *file )
*Arguments   : file     input. full path.
*Return      : ==0       succeed
*              !=0       fail
*Description : add a file to now playlist
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_add_item(const char *file)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_ADD_ITEM_TO_NPL, (__s32)dsk_wkm_hnpl, (void *)file);

    if (ret != ORC_OK)
    {
        __wrn("Fail in add file %s to npl.", file);
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_delete_item
*Prototype   : __s32  __dsk_wkm_npl_delete_item( const char *file )
*Arguments   : file     input. full path.
*Return      : ==0       succeed
*              !=0       fail
*Description : delete a file from now playlist
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_delete_item(const char *file)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_DELETE_ITEM_FROM_NPL, (__s32)dsk_wkm_hnpl, (void *)file);

    if (ret != ORC_OK)
    {
        __wrn("Fail in deleting file %s from npl.", file);
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_clear
*Prototype   : __s32  __dsk_wkm_npl_clear( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : clear the now playlist
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_clear(void)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_CLEAR_NPL, 0, (void *)dsk_wkm_hnpl);

    if (ret != ORC_OK)
    {
        __wrn("Fail in clearing the npl.");
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_playing_seek
*Prototype   : __s32  __dsk_wkm_npl_playing_seek( __s32 origin, __s32 offset )
*Arguments   : origin    input. origin position, as follows : ORC_NPL_SEEK_SET, ORC_NPL_SEEK_CUR ,
*                        and ORC_NPL_SEEK_END.
*              offset    input. offset.
*Return      : ==0       succeed
*              !=0       fail
*Description : seek the playing pointer. There is a playing pointer in the now play list, and it
*              should be adjusted to always point to the current playing item.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_playing_seek(__s32 origin, __s32 offset)
{
    __s32           ret;
    orc_arg_seek_t  seek_arg;
    seek_arg.hnpl = dsk_wkm_hnpl;
    seek_arg.offset = offset;
    seek_arg.origin = origin;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_PLAYING_SEEK, 0, &seek_arg);

    if (ret != ORC_OK)
    {
        __wrn("Fail in seeking playing pointer.");
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_get_next
*Prototype   : __s32  __dsk_wkm_npl_get_next( void )
*Arguments   : void
*Return      : the index of the next file to be played in the now play list. if return -1, that means
*              no item to play in current "play mode" or some error occured.
*Description : get the index of the next file to be played in the now play list according to the
*              current "play mode" and the current item. this operation will adjust the playing
*              pointer to the next item.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_get_next(void)
{
    __s32  ret;
    __s32  index;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_PLAYING_AUTO_GET_NEXT, (__s32)dsk_wkm_hnpl, &index);

    if (ret != ORC_OK)
    {
        __wrn("Fail in getting next item.");
        return -1;
    }

    return index;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_get_cur
*Prototype   : __s32  __dsk_wkm_npl_get_cur( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. if return -1, that means fail
*Description : get the index of the item that the playing ponter points to.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_get_cur(void)
{
    __s32  ret;
    __s32  index;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_PLAYING_GET_CUR, (__s32)dsk_wkm_hnpl, &index);

    if (ret != ORC_OK)
    {
        __wrn("Fail in getting cur item.");
        return -1;
    }

    return index;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_file2index
*Prototype   : __s32  __dsk_wkm_npl_file2index( const char *file )
*Arguments   : file     input. full path.
*Return      : the index value of the file in the now play list. If return -1. that means fail.
*Description : get the index value of the file in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_file2index(const char *file)
{
    __u32  index;
    index = g_fioctrl(dsk_wkm_horc, ORC_CMD_FILE_TO_INDEX_IN_NPL, (__s32)dsk_wkm_hnpl, (void *)file);

    if (index == (__u32) - 1)
    {
        return -1;
    }
    else
    {
        return index;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_index2file
*Prototype   : __s32  __dsk_wkm_npl_index2file( __u32 index, char *file )
*Arguments   : index     input. the item index in the now play list.
*              file      output. the file corresponding to the index.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the file corresponding to its index in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_index2file(__u32 index, char *file)
{
    __s32  ret;
    orc_arg_index_to_file_t  arg;
    arg.hnpl  = dsk_wkm_hnpl;
    arg.index = index;
    arg.file  = file;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_INDEX_TO_FILE_IN_NPL, 0, (void *)&arg);

    if (ret != ORC_OK)
    {
        __wrn("Fail in converting index to file.");
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_invalidate_item
*Prototype   : __s32  __dsk_wkm_npl_invalidate_item( __u32 index )
*Arguments   : index     input. the item index in the now play list.
*Return      : ==0       succeed
*              !=0       fail
*Description : set a item as invalid status.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_invalidate_item(unsigned long index)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_INVALIDATE_ITEM_IN_NPL, (signed long)dsk_wkm_hnpl, (void *)index);

    if (ret != ORC_OK)
    {
        __wrn("Fail in invalidate item %d.", index);
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_item_is_valid
*Prototype   : __bool __dsk_wkm_npl_item_is_valid( __u32 index )
*Arguments   : index     input. the item index in the now play list.
*Return      : If the item is valid, return "true"; else return "false".
*Description : check whether an item in now play list is valid or not.
*Other       :
****************************************************************************************************
*/
__bool __dsk_wkm_npl_item_is_valid(unsigned long index)
{
    return g_fioctrl(dsk_wkm_horc, ORC_CMD_IS_VALID_ITEM_IN_NPL, (signed long)dsk_wkm_hnpl, (void *)index);
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_get_total_count
*Prototype   : __s32  __dsk_wkm_npl_get_total_count( void )
*Arguments   : void
*Return      : the total count of items in the now play list. If return -1. that means fail.
*Description : get the total count of items in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_get_total_count(void)
{
    __s32  nr;
    nr = g_fioctrl(dsk_wkm_horc, ORC_CMD_GET_TOTAL_ITEM_COUNT_OF_NPL, 0, (void *)dsk_wkm_hnpl);

    if (nr == -1)
    {
        return -1;
    }
    else
    {
        return nr;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_get_valid_count
*Prototype   : __s32  __dsk_wkm_npl_get_valid_count( void )
*Arguments   : void
*Return      : the count of the valid items in the now play list. If return -1. that means fail.
*Description : get the count of the valid items in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_get_valid_count(void)
{
    __s32  nr;
    nr = g_fioctrl(dsk_wkm_horc, ORC_CMD_GET_VALID_ITEM_COUNT_OF_NPL, 0, (void *)dsk_wkm_hnpl);

    if (nr == -1)
    {
        return -1;
    }
    else
    {
        return nr;
    }
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_save_list
*Prototype   : __s32  __dsk_wkm_npl_save_list( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : save the now play list to data disk
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_save_list(void)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_SAVE_NPL, 0, (void *)dsk_wkm_hnpl);

    if (ret != ORC_OK)
    {
        __wrn("Fail in saving npl.");
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_load_last_list
*Prototype   : __s32  __dsk_wkm_npl_load_last_list( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : load the last play list to the npl.
*Other       :
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_load_last_list(void)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_LOAD_LAST_NPL, 0, (void *)dsk_wkm_hnpl);

    if (ret != ORC_OK)
    {
        __wrn("Fail in loading last npl.");
        return -1;
    }

    return 0;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_is_dirty
*Prototype   : __bool  __dsk_wkm_npl_is_dirty( void )
*Arguments   : void
*Return      : if npl is dirty, return EPDK_TRUE; else return EPDK_FALSE.
*Description : check wherther the npl is dirty or not.
*Other       : if the npl is dirty, all operations on npl will be invalid and then you should call
*              __dsk_wkm_npl_clear_dirty_flag() to clear the dirty flag.
****************************************************************************************************
*/
__bool  __dsk_wkm_npl_is_dirty(void)
{
    __bool  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_NPL_IS_DIRTY, 0, (void *)dsk_wkm_hnpl);
    return ret;
}



/*
****************************************************************************************************
*Name        : __dsk_wkm_npl_clear_dirty_flag
*Prototype   : __s32  __dsk_wkm_npl_clear_dirty_flag( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : clear dirty flag of npl.
*Other       : if the npl is dirty, all operations on npl will be invalid and then you should call
*              __dsk_wkm_npl_clear_dirty_flag() to clear the dirty flag.
****************************************************************************************************
*/
__s32  __dsk_wkm_npl_clear_dirty_flag(void)
{
    __s32  ret;
    ret = g_fioctrl(dsk_wkm_horc, ORC_CMD_NPL_CLEAR_DIRTY_FLAG, 0, (void *)dsk_wkm_hnpl);

    if (ret != ORC_OK)
    {
        __wrn("Fail in clearing dirty flag.");
        return -1;
    }

    return 0;
}



#endif     //  ifndef __DSK_WKM_NPL_I_C__

/* end of dsk_wkm_npl_i.c */
