/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_npl.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.06.14
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
* Gary.Wang      2010.06.14        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_NPL_C__
#define  __DSK_WKM_NPL_C__



#include "dsk_walkman_i.h"


/*
****************************************************************************************************
*Name        : dsk_wkm_npl_add_item
*Prototype   : __s32  dsk_wkm_npl_add_item( const char *file )
*Arguments   : file     input. full path.
*Return      : ==0       succeed
*              !=0       fail
*Description : add a file to now playlist
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_add_item(const char *file)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_npl_add_item(file);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_delete_item
*Prototype   : __s32  dsk_wkm_npl_delete_item( const char *file )
*Arguments   : file     input. full path.
*Return      : ==0       succeed
*              !=0       fail
*Description : delete a file from now playlist
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_delete_item(const char *file)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_npl_delete_item(file);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_clear
*Prototype   : __s32  dsk_wkm_npl_clear( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : clear the now playlist
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_clear(void)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);
    ret = __dsk_wkm_npl_clear();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_set_cur
*Prototype   : __s32  dsk_wkm_npl_set_cur( __u32 index )
*Arguments   : index     input. the index of the item that the playing ponter points to.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the index of the item that the playing ponter points to. Note : The function is
*              only valid when the walkman is NOT in playing status.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_set_cur(__u32 index)
{
    __u8    err;
    __s32   ret;

    if (__dsk_wkm_get_playing_state() == EPDK_TRUE)
    {
        return -1;
    }

    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_playing_seek(ORC_NPL_SEEK_SET, index);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_get_cur
*Prototype   : __s32  dsk_wkm_npl_get_cur( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. -1 means fail.
*Description : get the index of the item that the playing ponter points to.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_get_cur(void)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_get_cur();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_get_next
*Prototype   : __s32  dsk_wkm_npl_get_next( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. -1 means fail.
*Description : get the index of the item that the playing ponter points to.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_get_next(void)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_get_next();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}

/*
****************************************************************************************************
*Name        : dsk_wkm_npl_file2index
*Prototype   : __s32  dsk_wkm_npl_file2index( const char *file )
*Arguments   : file     input. full path.
*Return      : the index value of the file in the now play list. -1 means fail.
*Description : get the index value of the file in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_file2index(const char *file)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_file2index(file);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_index2file
*Prototype   : __s32  dsk_wkm_npl_index2file( __u32 index, char *file )
*Arguments   : index     input. the item index in the now play list.
*              file      output. the file corresponding to the index.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the file corresponding to its index in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_index2file(__u32 index, char *file)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_index2file(index, file);
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_get_total_count
*Prototype   : __s32  dsk_wkm_npl_get_total_count( void )
*Arguments   : void
*Return      : the total count of items in the now play list. -1 means fail.
*Description : get the total count of items in the now play list.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_get_total_count(void)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_get_total_count();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_save_list
*Prototype   : __s32  dsk_wkm_npl_save_list( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : save the now play list to data disk
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_save_list(void)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_save_list();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_npl_load_last_list
*Prototype   : __s32  dsk_wkm_npl_load_last_list( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : load the last play list to the npl.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_npl_load_last_list(void)
{
    __u8    err;
    __s32   ret;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    if (__dsk_wkm_npl_is_dirty())
    {
        __dsk_wkm_npl_clear_dirty_flag();
    }

    ret = __dsk_wkm_npl_load_last_list();
    g_post_mutex(dsk_wkm_res_mutex);
    return ret;
}



#endif     //  ifndef __DSK_WKM_NPL_C__

/* end of dsk_wkm_npl.c */
