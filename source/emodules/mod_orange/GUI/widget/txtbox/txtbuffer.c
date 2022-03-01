/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：        txtbuffer.c
*
*Author：           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-18
*
*Description :      txtbox function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>                  <version>     <description>
*
* William Wang     2008-12-18         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTBUFFER_C__
#define __TXTBUFFER_C__

#include "txtbuffer.h"

//------------------------------------------------------------------------------------------------------------
//创建ListBuffer并分配空间

//==================================================================
//function name:    TxtBuffer_Create
//author:
//date:             2008-12-18
//description:      txtbuffer create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
txtbuffer_man_t    *TxtBuffer_Create(void)
{
    txtbuffer_man_t  *ptxtbuffer;
    ptxtbuffer = (txtbuffer_man_t *)TXTBOX_MALLOC(sizeof(txtbuffer_man_t));

    if (!ptxtbuffer)
    {
        return NULL;
    }

    return ptxtbuffer;
}

//------------------------------------------------------------------------------------------------------------
//销毁ListBuffer并回收空间

//==================================================================
//function name:    TxtBuffer_Destroy
//author:
//date:             2008-12-18
//description:      txtbuffer destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32  TxtBuffer_Destroy(txtbuffer_man_t *txtbuffer)
{
    if (txtbuffer)
    {
        TXTBOX_MEMFREE(txtbuffer);
    }
    else
    {
        return 0;
    }

    return 0;
}

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------


#endif
