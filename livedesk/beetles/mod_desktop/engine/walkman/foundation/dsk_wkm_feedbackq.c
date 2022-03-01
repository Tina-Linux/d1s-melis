/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_wkm_feedbackq.c
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.10.06
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
* Gary.Wang      2010.10.06        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __DSK_WKM_FEEDBACKQ_C__
#define  __DSK_WKM_FEEDBACKQ_C__


#include "foundation_i.h"


#define DSK_WKM_FEEDBACKQ_LEN                  1


g_Queue  volatile  dsk_wkm_feedbackQ = NULL;


/***************************************************************************************************
*Name        : __dsk_wkm_feedbackQ_create
*Prototype   : __s32 __dsk_wkm_feedbackQ_create( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : create feedback message queue
*Other       :
***************************************************************************************************/
__s32 __dsk_wkm_feedbackQ_create(void)
{
    if (dsk_wkm_feedbackQ == NULL)
    {
        dsk_wkm_feedbackQ = g_QCreate(DSK_WKM_FEEDBACKQ_LEN);

        if (dsk_wkm_feedbackQ == NULL)
        {
            __err("Error in creating dsk_wkm_feedbackQ!\n");
            return -1;
        }
    }

    return 0;
}



/***************************************************************************************************
*Name        : __dsk_wkm_feedbackQ_delete
*Prototype   : void  __dsk_wkm_feedbackQ_delete( void )
*Arguments   : void
*Return      : void
*Description : delete feedback message queue
*Other       :
***************************************************************************************************/
void  __dsk_wkm_feedbackQ_delete(void)
{
    __u8  err;

    if (dsk_wkm_feedbackQ != NULL)
    {
        g_QDel(dsk_wkm_feedbackQ, OS_DEL_ALWAYS, &err);
        dsk_wkm_feedbackQ = NULL;
    }
}






#endif     //  ifndef __DSK_WKM_FEEDBACKQ_C__

/* end of dsk_wkm_feedbackq.c */
