/*
************************************************************************************************************************
*                                                        walkman
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : dsk_walkman.c
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
#ifndef  __DSK_WALKMAN_C__
#define  __DSK_WALKMAN_C__



#include "dsk_walkman_i.h"


#define DSK_WKM_MONITOR_PRIO              KRNL_priolevel3


static __s32 _request_open_res(void);
static void  _release_open_res(void);
static void  _release_init_res(void);


/*
****************************************************************************************************
*Name        : dsk_wkm_init
*Prototype   : __s32 dsk_wkm_init( dsk_wkm_mode_e work_mode )
*Arguments   : work_mode  input. work mode
*Return      : ==0     succeed
*              !=0     fail
*Description : init walkman
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_init(dsk_wkm_mode_e dsk_wkm_mode)
{
    switch (dsk_wkm_mode)
    {
        case DSK_WKM_MODE_AUDIO_MIN :
        {
            if (__dsk_wkm_res_mutex_create())
            {
                goto error;
            }

            //if( __dsk_wkm_orchid_open( ) )
            //  goto error;
            //if( __dsk_wkm_npl_create( ORC_MEDIA_TYPE_AUDIO, ORC_NPL_WORK_MODE_MANAGE ) )
            //if( __dsk_wkm_npl_create( ORC_MEDIA_TYPE_AUDIO, ORC_NPL_WORK_MODE_USE ) )
            //  goto error;
            break;
        }

        default :
        {
            goto error;
        }
    }

    __dsk_wkm_set_open_state(EPDK_FALSE);
    __dsk_wkm_set_playing_state(EPDK_FALSE);
    __dsk_wkm_set_pause_state(EPDK_FALSE);
    return 0;
error:
    _release_init_res();
    __dsk_wkm_set_open_state(EPDK_FALSE);
    __dsk_wkm_set_playing_state(EPDK_FALSE);
    __dsk_wkm_set_pause_state(EPDK_FALSE);
    return -1;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_exit
*Prototype   : void dsk_wkm_exit( void )
*Arguments   : void
*Return      : void
*Description : exit walkman
*Other       :
****************************************************************************************************
*/
void  dsk_wkm_exit(void)
{
    to_quit_dsk_wkm  = EPDK_TRUE;
    /* close walkman */
    dsk_wkm_close();
    _release_init_res();
}



/*
****************************************************************************************************
*Name        : dsk_wkm_open
*Prototype   : __s32  dsk_wkm_open( void )
*Arguments   : void
*Return      : ==0    succeed
*              !=0    fail
*Description : open walkman and then you can play file.
*Other       :
****************************************************************************************************
*/
__s32  dsk_wkm_open(void)
{
    if (__dsk_wkm_get_open_state() == EPDK_TRUE)
    {
        return 0;
    }

    to_quit_dsk_wkm  = EPDK_FALSE;

    if (_request_open_res())
    {
        goto error;
    }

    return 0;
error:
    _release_open_res();
    to_quit_dsk_wkm  = EPDK_FALSE;
    return -1;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_close
*Prototype   : void  dsk_wkm_close( void )
*Arguments   : void
*Return      : void
*Description : close walkman.
*Other       :
****************************************************************************************************
*/
void  dsk_wkm_close(void)
{
    to_quit_dsk_wkm  = EPDK_TRUE;
    dsk_wkm_stop();
    _release_open_res();
}



/*
****************************************************************************************************
*Name        : dsk_wkm_is_open
*Prototype   : __bool  dsk_wkm_is_open( void )
*Arguments   : void
*Return      : if walkman is open, return EPDK_TRUE; else return EPDK_FALSE.
*Description : check whether walkman is open or not.
*Other       :
****************************************************************************************************
*/
__bool  dsk_wkm_is_open(void)
{
    if (__dsk_wkm_get_open_state() == EPDK_TRUE)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}



/*
****************************************************************************************************
*Name        : dsk_wkm_set_cb
*Prototype   : __pCBK_t dsk_wkm_set_cb( dsk_wkm_event_e event, __pCBK_t cb )
*Arguments   : event   input. event type.
*              cb      input. the callback function. NULL means unregister the callback function. 回
*                      调函数里面不能调用“dsk_wkm_..."相关的接口，以免死锁。
*Return      : the old callback function.
*Description : set callback functions for events.
*Other       :
****************************************************************************************************
*/
__pCBK_t dsk_wkm_set_cb(dsk_wkm_event_e event, __pCBK_t cb, void *ctx)
{
    __u8    err;
    __pCBK_t  cb_old;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    switch (event)
    {
        case DSK_WKM_EVENT_AUTO_PLAY_NEXT:
        {
            cb_old = cb_auto_play_next;
            cb_auto_play_next = cb;
            cb_ctx_auto_play_next = ctx;
            break;
        }

        case DSK_WKM_EVENT_NO_ITEM_TO_PLAY:
        {
            cb_old = cb_no_item_to_play;
            cb_no_item_to_play = cb;
            cb_ctx_no_item_to_play = ctx;
            break;
        }

        default :
        {
            cb_old = NULL;
        }
    }

    g_post_mutex(dsk_wkm_res_mutex);
    return cb_old;
}



/*
****************************************************************************************************
*Name        : dsk_wkm_get_cb
*Prototype   : __pCBK_t dsk_wkm_get_cb( dsk_wkm_event_e event )
*Arguments   : event   input. event type.
*Return      : the callback function of a event.
*Description : get the callback function of a event.
*Other       :
****************************************************************************************************
*/
__pCBK_t dsk_wkm_get_cb(dsk_wkm_event_e event)
{
    __u8    err;
    __pCBK_t   cb;
    g_pend_mutex(dsk_wkm_res_mutex, &err);

    switch (event)
    {
        case DSK_WKM_EVENT_AUTO_PLAY_NEXT:
        {
            cb = cb_auto_play_next;
            break;
        }

        case DSK_WKM_EVENT_NO_ITEM_TO_PLAY:
        {
            cb = cb_no_item_to_play;
            break;
        }

        default :
        {
            cb = NULL;
        }
    }

    g_post_mutex(dsk_wkm_res_mutex);
    return cb;
}

__s32 dsk_wkm_set_attr(void *attr)
{
    return __dsk_wkm_set_attr(attr);
}

void *dsk_wkm_get_attr(void)
{
    return __dsk_wkm_get_attr();
}

static void  _release_init_res(void)
{
    __dsk_wkm_npl_delete();
    __dsk_wkm_orchid_close();
    __dsk_wkm_res_mutex_delete();
}



static __s32  _request_open_res(void)
{
    if (dsk_wkm_monitor_create(DSK_WKM_MONITOR_PRIO))
    {
        goto error;
    }

    return 0;
error:
    _release_open_res();
    return -1;
}



static void  _release_open_res(void)
{
    dsk_wkm_monitor_delete();
}



#endif     //  ifndef __DSK_WALKMAN_C__

/* end of dsk_walkman.c */
