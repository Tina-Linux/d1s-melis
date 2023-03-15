/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     :
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/11/4
 *  Last Modified:  2021-05-10 09:55:17
  Description   :
  Function List :
  History       :
******************************************************************************/

#define LOG_NDEBUG 0
#define LOG_TAG "sample_thumb"
#include <utils/plat_log.h>
#include <log.h>
//#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "media/mm_comm_vi.h"
#include "media/mpi_vi.h"
#include "media/mpi_isp.h"
#include "media/mpi_venc.h"
#include "media/mpi_sys.h"
#include "mm_common.h"
#include "mm_comm_venc.h"
#include "mm_comm_rc.h"
#include <mpi_videoformat_conversion.h>

#include <confparser.h>
#include "sample_virvi2venc.h"
#include "sample_virvi2venc_config.h"

#define VIDEV 0
#define SRC_WIDTH 1920
#define SRC_HEIGHT 1080
#define SRC_FPS 60
#define DST_FPS 20

#define PictureThumbWidth 240
#define PictureThumbHeight 120

int test_thumbnail(char **thumb, char **pic, int *len_thumb, int *len_pic)
{    
    ERRORTYPE ret;

    //return len's of jpg
    MPP_SYS_CONF_S mSysConf;
    memset(&mSysConf, 0, sizeof(MPP_SYS_CONF_S));
    mSysConf.nAlignWidth = 32;
    AW_MPI_SYS_SetConf(&mSysConf);
    ret = AW_MPI_SYS_Init();
    if (ret < 0)
    {
        aloge("sys Init failed!");
        return -1;
    }

    int len = 0;
	
    ret = AW_MPI_VI_CreateVipp(VIDEV);
    if (ret != SUCCESS)
    {
        __err("fatal error! AW_MPI_VI CreateVipp failed");
        return FAILURE;
    }

    VI_ATTR_S mViAttr;
    memset(&mViAttr, 0, sizeof(VI_ATTR_S));
    mViAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    mViAttr.memtype = V4L2_MEMORY_MMAP;
    mViAttr.format.pixelformat = V4L2_PIX_FMT_NV21M;
    mViAttr.format.field = V4L2_FIELD_NONE;
    mViAttr.format.colorspace = V4L2_COLORSPACE_JPEG;
    mViAttr.format.width = SRC_WIDTH;
    mViAttr.format.height = SRC_HEIGHT;
    mViAttr.nbufs = 5;
    mViAttr.drop_frame_num = 0;
    mViAttr.use_current_win = 0;
    mViAttr.nplanes = 2;
    mViAttr.fps = SRC_FPS;
    mViAttr.capturemode = V4L2_MODE_VIDEO;

    __log("%s use %d v4l2 buffers!!!", __func__, mViAttr.nbufs);

    ret = AW_MPI_VI_SetVippAttr(VIDEV, &mViAttr);
    if (ret != SUCCESS)
    {
        __err("fatal error! AW_MPI_VI SetVippAttr failed");
        return FAILURE;
    }

    ret = AW_MPI_VI_EnableVipp(VIDEV);
    if (ret != SUCCESS)
    {
        __err("fatal error! enableVipp fail!");
        return FAILURE;
    }

    AW_MPI_ISP_Run(VIDEV);

    VI_CHN inCh = 2;
    ret = AW_MPI_VI_CreateVirChn(VIDEV, inCh, NULL);
    if(ret < 0)
    {
        __err("Create VI Chn failed,VIDev = %d,VIChn = %d",VIDEV, inCh);
        return ret ;
    }
    ret = AW_MPI_VI_SetVirChnAttr(VIDEV, inCh, NULL);
    if(ret < 0)
    {
        __err("Set VI ChnAttr failed,VIDev = %d,VIChn = %d",VIDEV, inCh);
        return ret ;
    }
    ret = AW_MPI_VI_EnableVirChn(VIDEV, inCh);
    if(ret < 0){
        __err("VI Enable VirChn failed,VIDev = %d,VIChn = %d",VIDEV, inCh);
        return ret ;
    }

    VIDEO_FRAME_INFO_S pstFrameInfo;
    memset(&pstFrameInfo, 0x00, sizeof(VIDEO_FRAME_INFO_S));
    if ((ret = AW_MPI_VI_GetFrame(VIDEV, inCh, &pstFrameInfo, 5000)) != 0)
    {
        __err("VI Get Frame failed!");
        return ret;
    }else{
        __err("VI Get Frame success!");
        unsigned int nVbvBufSize =  ALIGN(pstFrameInfo.VFrame.mWidth*pstFrameInfo.VFrame.mHeight*3/2, 1024);
        // ALIGN((pstFrameInfo.VFrame.mWidth*pstFrameInfo.VFrame.mHeight*3/2 /2) + (pstFrameInfo.VFrame.mWidth*pstFrameInfo.VFrame.mHeight/3), 1024);
        VENC_CHN_ATTR_S mChnAttr;
        memset(&mChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
        mChnAttr.VeAttr.Type = PT_JPEG;
        mChnAttr.VeAttr.AttrJpeg.BufSize = nVbvBufSize;
        mChnAttr.VeAttr.AttrJpeg.bByFrame = TRUE;
        mChnAttr.VeAttr.AttrJpeg.PicWidth =  pstFrameInfo.VFrame.mWidth/4;
        mChnAttr.VeAttr.AttrJpeg.PicHeight = pstFrameInfo.VFrame.mHeight/4;
        mChnAttr.VeAttr.MaxKeyInterval = 1;
        mChnAttr.VeAttr.SrcPicWidth = pstFrameInfo.VFrame.mWidth;
        mChnAttr.VeAttr.SrcPicHeight = pstFrameInfo.VFrame.mHeight;
        mChnAttr.VeAttr.Field = VIDEO_FIELD_FRAME;
        mChnAttr.VeAttr.PixelFormat = pstFrameInfo.VFrame.mPixelFormat;
        mChnAttr.VeAttr.mColorSpace = V4L2_COLORSPACE_JPEG;
        __log("pixfmt:0x%x, colorSpace:0x%x", mChnAttr.VeAttr.PixelFormat, mChnAttr.VeAttr.mColorSpace);
        bool bSuccessFlag = false;
        VENC_CHN mChn = 0;
        while(mChn < VENC_MAX_CHN_NUM){
            ret = AW_MPI_VENC_CreateChn(mChn, &mChnAttr);
            if(SUCCESS == ret)
            {
                bSuccessFlag = true;
                __err("create venc channel[%d] success!", mChn);
                break;
            }
            else if(ERR_VENC_EXIST == ret)
            {
                __err("venc channel[%d] is exist, find next!", mChn);
                mChn++;
            }
            else{
                __err("fatal error! create venc channel[%d] ret[0x%x], find next!", mChn, ret);
                mChn++;
            }
        }
        if(false == bSuccessFlag)
        {
            mChn = MM_INVALID_CHN;
            __err("fatal error! create venc channel fail!");
            ret = FAILURE;
            goto _err0;
        }
        VENC_PARAM_JPEG_S mJpegParam;
        memset(&mJpegParam, 0, sizeof(VENC_PARAM_JPEG_S));
        mJpegParam.Qfactor = 60;
        AW_MPI_VENC_SetJpegParam(mChn, &mJpegParam);
        AW_MPI_VENC_ForbidDiscardingFrame(mChn, TRUE);
        VENC_JPEG_THUMB_BUFFER_S mJpegThumbBuf;
        VENC_EXIFINFO_S mExifInfo;
        memset(&mExifInfo, 0, sizeof(VENC_EXIFINFO_S));
        time_t t;
        struct tm *tm_t;
        time(&t);
        tm_t = localtime(&t);
        snprintf((char*)mExifInfo.DateTime, MM_DATA_TIME_LENGTH, "%4d:%02d:%02d %02d:%02d:%02d",
            tm_t->tm_year+1900, tm_t->tm_mon+1, tm_t->tm_mday,
            tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);
        mExifInfo.ThumbWidth = PictureThumbWidth;
        mExifInfo.ThumbHeight = PictureThumbHeight;
        mExifInfo.thumb_quality = 60;
        mExifInfo.Orientation = 0;
        mExifInfo.fr32FNumber = FRACTION32(10, 26);
        mExifInfo.MeteringMode = AVERAGE_AW_EXIF;
        mExifInfo.fr32FocalLength = FRACTION32(100, 228);
        mExifInfo.WhiteBalance = 0;
        mExifInfo.FocalLengthIn35mmFilm = 18;
        strcpy((char*)mExifInfo.ImageName, "aw-photo");
        AW_MPI_VENC_SetJpegExifInfo(mChn, &mExifInfo);
        AW_MPI_VENC_SendFrame(mChn, &pstFrameInfo, 0);
        ret = AW_MPI_VENC_StartRecvPic(mChn);
        if(SUCCESS != ret){
            __err("fatal error:%x jpegEnc AW_MPI_VENC_StartRecvPic", ret);
            ret = FAILURE;
        }
        VENC_STREAM_S mOutStream;
        VENC_PACK_S encPack;
        memset(&encPack, 0, sizeof(VENC_PACK_S));
        mOutStream.mpPack = &encPack;
        mOutStream.mPackCount = 1;
        
        ret = AW_MPI_VENC_GetStream(mChn, &mOutStream, 1000);
        if(ret != SUCCESS){
            __err("fatal error! why get stream fail?");
            ret = FAILURE;
            goto _err1;
        }
        else{
            ret = AW_MPI_VENC_GetJpegThumbBuffer(mChn, &mJpegThumbBuf);
            if(ret != SUCCESS){
                goto _err1;
            }
        }
        if(SUCCESS == ret)
        {
//#if 0
            len = mJpegThumbBuf.ThumbLen;
            *thumb = (char *)malloc(len);
            memcpy(*thumb, mJpegThumbBuf.ThumbAddrVir, len);
	    *len_thumb = len;
//#else
            len = mOutStream.mpPack[0].mLen0+mOutStream.mpPack[0].mLen1+mOutStream.mpPack[0].mLen2;
            *pic = (char *)malloc(len);

            if(mOutStream.mpPack[0].mLen0){
                 memcpy(*pic, mOutStream.mpPack[0].mpAddr0, mOutStream.mpPack[0].mLen0);
            }
            if(mOutStream.mpPack[0].mLen1){
                 memcpy(*pic + mOutStream.mpPack[0].mLen0, mOutStream.mpPack[0].mpAddr1, mOutStream.mpPack[0].mLen1);
            }
            if(mOutStream.mpPack[0].mLen2){
                 memcpy(*pic + mOutStream.mpPack[0].mLen0 +mOutStream.mpPack[0].mLen1, mOutStream.mpPack[0].mpAddr2, mOutStream.mpPack[0].mLen2);
            }
	    *len_pic = len;
//#endif
            __log("mJpegThumbBuf.ThumbLen:%d, pic len %d.", *len_thumb, *len_pic);
        }
        __log("AW_MPI_VENC_ReleaseStream mOutStream");
        ret = AW_MPI_VENC_ReleaseStream(mChn, &mOutStream);
        if(ret != SUCCESS)
        {
            __err("fatal error! why release stream fail(0x%x)?", ret);
            ret = FAILURE;
        }
        AW_MPI_VI_ReleaseFrame(VIDEV, inCh, &pstFrameInfo);
_err1:
        AW_MPI_VENC_StopRecvPic(mChn);
        AW_MPI_VENC_ResetChn(mChn);
        AW_MPI_VENC_DestroyChn(mChn);
	/*
         *while(1)
         *    sleep(1);
	 */
        AW_MPI_VI_DisableVirChn(VIDEV, inCh);
        AW_MPI_VI_DestroyVirChn(VIDEV, inCh);
        AW_MPI_VI_DisableVipp(VIDEV);
        AW_MPI_VI_DestoryVipp(VIDEV);
        AW_MPI_SYS_Exit();
        return 0;
    _err0:
        return -1;
    }
}

int main_sample_thumb(int argc, char *argv[])
{
    char *thumb = NULL;
    char *pic   = NULL;

    int len_thumb = 0;
    int len_pic   = 0;

    int ret = test_thumbnail(&thumb, &pic, &len_thumb, &len_pic);
    if(ret != 0)
    {
        __err("fatal error.");
        return -1;
    }

    void cpu_dcache_invalidate(unsigned long vaddr_start, unsigned long size);
    cpu_dcache_invalidate((unsigned long)thumb, len_thumb);
    __log("thumb = 0x%08x. thumb len %d. pic 0x%08x, pic len %d.\n",  (unsigned int)thumb, len_thumb, (unsigned int)pic, len_pic);
    while(1);
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(main_sample_thumb, __cmd_sample_thumb, test mpp virvi2venc);
