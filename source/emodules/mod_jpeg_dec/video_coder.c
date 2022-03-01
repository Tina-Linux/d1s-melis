/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_herb sub-system
*                            video rec framework module
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : video_play.c
* Version: V1.0
* By     : xiechr
* Date   : 2011-3-2
* Description:
********************************************************************************
*/

#include "video_coder_i.h"
#include "mjpegDecode.h"

/*
*********************************************************************************************************
*                       VIDEO RECEIVE MODULE INIT
*
*Description: video play module init.
*
*Arguments  : None.
*
*Return     : Always return successful.
*********************************************************************************************************
*/
__s32 MInit(void)
{
    __msg("****************");
    __msg("==vplay==MInit!");
    __msg("****************");
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                       VIDEO RECEIVE MODULE EXIT
*
*Description: video receive module exit.
*
*Arguments  : None.
*
*Return     : Always return successful.
*********************************************************************************************************
*/
__s32 MExit(void)
{
    __msg("== vplay==MExit!");
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                       VIDEO PLAY MODULE OPEN
*
*Description: video play module open.
*
*Arguments  : mid   the module id of video play;
*             mode  the type of open.
*
*Return     : the handle of the video play driver.
*                   != 0    open video play module successful;
*                   == 0    open video play module fialed.
*********************************************************************************************************
*/
__mp *MOpen(__u32 mid, __u32 mode)
{
    __herb_vcoder_t  *pHerb_vcoder = NULL;
    __s32       result;
    pHerb_vcoder = (__herb_vcoder_t *)eLIBs_malloc(sizeof(__herb_vcoder_t));

    if (pHerb_vcoder == NULL)
    {
        __err("malloc failed!");
        return (__mp *)NULL;
    }

    pHerb_vcoder->mjpegDecCtx = (void *)MJPEG_init();
	__wrn("[YG]pHerb_vcoder->mjpegDecCtx:0x%x\n",pHerb_vcoder->mjpegDecCtx);
    pHerb_vcoder->mid = mid;
    return (__mp *)pHerb_vcoder;
}


/*
*********************************************************************************************************
*                       VIDEO PLAY MODULE CLOSE
*
*Description: video play module close.
*
*Arguments  : mp    the handle of the video play;
*
*Return     : Always return successful.
*********************************************************************************************************
*/
__s32 MClose(__mp *mp)
{
    __herb_vcoder_t  *pHerb_vcoder = (__herb_vcoder_t *)mp;
    MJPEG_exit(pHerb_vcoder->mjpegDecCtx);
    eLIBs_free((void *)pHerb_vcoder);
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                       VIDEO PLAY MODULE READ
*
*Description: video play module read, do nothing.
*
*Arguments  :
*
*Return     :
*********************************************************************************************************
*/
__u32 MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return n;
}


/*
*********************************************************************************************************
*                       VIDEO PLAY MODULE WRITE
*
*Description: video play module write, do nothing.
*
*Arguments  :
*
*Return     :
*********************************************************************************************************
*/
__u32 MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return n;
}



/*
*********************************************************************************************************
*                       VIDEO PLAY MODULE IO CONTROL
*
*Description: video play module IO control, process command from other module.
*
*Arguments  : mp        video play module pointer;
*             cmd       the command need send to video play module;
*             aux       user define;
*             pbuffer   user define.
*
*Return     :
*********************************************************************************************************
*/
long MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __s32 ret = EPDK_FAIL;

    switch (cmd)
    {
        case MPEJ_CODEC_CMD_DECODER:
        {
            __herb_vcoder_t  *pHerb_vcoder = (__herb_vcoder_t *)mp;
            stJpegFrameInfo *mjpegFrameInfo = (stJpegFrameInfo *)pbuffer;
            MJMACC_init(pHerb_vcoder->mjpegDecCtx, mjpegFrameInfo);
            ret = MJPEG_decode(pHerb_vcoder->mjpegDecCtx, mjpegFrameInfo);
            MJMACC_exit(pHerb_vcoder->mjpegDecCtx, mjpegFrameInfo);
            break;
        }

        case MPEJ_CODEC_CMD_INIT:
        {
            break;
        }

        case MPEJ_CODEC_CMD_ENCODER:
        {
            break;
        }

        default:
            __wrn("the cmd(%d) is invalid!", cmd);
            break;
    }

    return ret;
}


