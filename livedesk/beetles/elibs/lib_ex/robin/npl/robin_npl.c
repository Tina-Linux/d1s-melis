/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_npl.c
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.09.28
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
* Gary.Wang      2009.09.28       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_npl_c
#define  __robin_npl_c


#include "robin_i.h"
#include <kconfig.h>

/***************************************************************************************************
*Name        : robin_npl_get_next
*Prototype   : __s32  robin_npl_get_next( void )
*Arguments   : void
*Return      : the index of the next file to be played in the now play list. if return -1, that means
*              no item to play in current "play mode" or some error occured.
*Description : get the index of the next file to be played in the now play list according to the
*              current "play mode" and the current item. this operation will adjust the playing
*              pointer to the next item.
*Other       :
***************************************************************************************************/
__s32  robin_npl_get_next(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    return rat_npl_get_next(rat_hnpl);
}

/***************************************************************************************************
*Name        : robin_npl_get_next
*Prototype   : __s32  robin_npl_get_prev( void )
*Arguments   : void
*Return      : the index of the prev file to be played in the now play list. if return -1, that means
*              no item to play in current "play mode" or some error occured.
*Description : get the index of the prev file to be played in the now play list according to the
*              current "play mode" and the current item. this operation will adjust the playing
*              pointer to the next item.
*Other       :
***************************************************************************************************/
__s32  robin_npl_get_prev(void)
{
    return rat_npl_get_prev(rat_hnpl);
}



/***************************************************************************************************
*Name        : robin_npl_get_cur
*Prototype   : __s32  robin_npl_get_cur( void )
*Arguments   : void
*Return      : the index of the item that the playing ponter points to. if return -1, that means fail
*Description : get the index of the item that the playing ponter points to.
*Other       :
***************************************************************************************************/
__s32  robin_npl_get_cur(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    return rat_npl_get_cur(rat_hnpl);
}



/***************************************************************************************************
*Name        : robin_npl_file2index
*Prototype   : __s32  robin_npl_file2index( const char *file )
*Arguments   : file     input. full path.
*Return      : the index value of the file in the now play list. If return -1. that means fail.
*Description : get the index value of the file in the now play list.
*Other       :
***************************************************************************************************/
__s32  robin_npl_file2index(const char *file)
{
    return rat_npl_file2index(rat_hnpl, file);
}



/***************************************************************************************************
*Name        : robin_npl_index2file
*Prototype   : __s32  robin_npl_index2file( __u32 index, char *file )
*Arguments   : index     input. the item index in the now play list.
*              file      output. the file corresponding to the index.
*Return      : ==0       succeed
*              !=0       fail
*Description : get the file corresponding to its index in the now play list.
*Other       :
***************************************************************************************************/
__s32  robin_npl_index2file(__u32 index, char *file)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    return rat_npl_index2file(rat_hnpl, index, file);
}

/***************************************************************************************************
*Name        : robin_npl_get_total_count
*Prototype   :
*Arguments   :
*
*Return      : total count of video file
*
*Description :
*Other       :
***************************************************************************************************/
__s32  robin_npl_get_total_count(void)
{
    if (EPDK_FALSE == robin_is_open())
    {
        return 0;
    }

    return rat_npl_get_total_count(rat_hnpl);
}

/***************************************************************************************************
*Name        : robin_npl_set_cur
*Prototype   : __s32  robin_npl_get_cur( void )
*Arguments   : void
*Return      :
*Description : set the index of the item that the playing ponter points to.
*Other       :
***************************************************************************************************/
__s32  robin_npl_set_cur(__s32 index)
{

    if (EPDK_FALSE == robin_is_open())
    {
        return EPDK_FAIL;
    }

    return rat_npl_set_cur(rat_hnpl, index);
}

#endif     //  ifndef __robin_npl_c
