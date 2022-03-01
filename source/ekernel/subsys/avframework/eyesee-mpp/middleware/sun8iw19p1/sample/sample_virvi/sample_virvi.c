/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : sample_virvi.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/1/5
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#include <machine/endian.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils/plat_log.h>
#include "media/mpi_sys.h"
#include "media/mm_comm_vi.h"
#include "media/mpi_vi.h"
#include "media/mpi_isp.h"
#include <mpi_venc.h>
#include <utils/VIDEO_FRAME_INFO_S.h>
#include <confparser.h>
#include "sample_virvi.h"
#include "sample_virvi_config.h"

//int AutoTestCount = 0,GetFrameCount = 0;
//VirVi_Cap_S privCap[MAX_VIPP_DEV_NUM][MAX_VIR_CHN_NUM];

int hal_virvi_start(VI_DEV ViDev, VI_CHN ViCh, void *pAttr)
{
    int ret = -1;

    ret = AW_MPI_VI_CreateVirChn(ViDev, ViCh, pAttr);
    if(ret < 0)
    {
        aloge("Create VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    ret = AW_MPI_VI_SetVirChnAttr(ViDev, ViCh, pAttr);
    if(ret < 0)
    {
        aloge("Set VI ChnAttr failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    ret = AW_MPI_VI_EnableVirChn(ViDev, ViCh);
    if(ret < 0)
    {
        aloge("VI Enable VirChn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }

    return 0;
}

int hal_virvi_end(VI_DEV ViDev, VI_CHN ViCh)
{
    int ret = -1;
    ret = AW_MPI_VI_DisableVirChn(ViDev, ViCh);
    if(ret < 0)
    {
        aloge("Disable VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    ret = AW_MPI_VI_DestroyVirChn(ViDev, ViCh);
    if(ret < 0)
    {
        aloge("Destory VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    return 0;
}

static int ParseCmdLine(int argc, char **argv, SampleVirViCmdLineParam *pCmdLinePara)
{
    alogd("sample virvi path:[%s], arg number is [%d]", argv[0], argc);
    int ret = 0;
    int i=1;
    memset(pCmdLinePara, 0, sizeof(SampleVirViCmdLineParam));
    while(i < argc)
    {
        if(!strcmp(argv[i], "-path"))
        {
            if(++i >= argc)
            {
                aloge("fatal error! use -h to learn how to set parameter!!!");
                ret = -1;
                break;
            }
            if(strlen(argv[i]) >= MAX_FILE_PATH_SIZE)
            {
                aloge("fatal error! file path[%s] too long: [%d]>=[%d]!", argv[i], strlen(argv[i]), MAX_FILE_PATH_SIZE);
            }
            strncpy(pCmdLinePara->mConfigFilePath, argv[i], MAX_FILE_PATH_SIZE-1);
            pCmdLinePara->mConfigFilePath[MAX_FILE_PATH_SIZE-1] = '\0';
        }
        else if(!strcmp(argv[i], "-h"))
        {
            alogd("CmdLine param:\n"
                "\t-path /home/sample_virvi.conf\n");
            ret = 1;
            break;
        }
        else
        {
            alogd("ignore invalid CmdLine param:[%s], type -h to get how to set parameter!", argv[i]);
        }
        i++;
    }
    return ret;
}

static ERRORTYPE loadSampleVirViConfig(SampleVirViConfig *pConfig, const char *conf_path)
{
    int ret;
    memset(pConfig, 0, sizeof(SampleVirViConfig));
    pConfig->AutoTestCount = 1;
    pConfig->GetFrameCount = 600;
    pConfig->DevNum = 0;
    pConfig->FrameRate = 60;
    pConfig->PicWidth = 1920;
    pConfig->PicHeight = 1080;
    pConfig->PicFormat = V4L2_PIX_FMT_NV21M;
    pConfig->mColorSpace = V4L2_COLORSPACE_JPEG;
    pConfig->mRawStoreCount = 0;
    pConfig->mRawStoreInterval = 30;
    pConfig->mJpegStoreCount = 2;
    pConfig->mJpegStoreInterval = 60;
    strcpy(pConfig->mStoreDirectory, "/mnt/E");

    if(conf_path != NULL)
    {
        CONFPARSER_S stConfParser;
        ret = createConfParser(conf_path, &stConfParser);
        if(ret < 0)
        {
            aloge("load conf fail");
            return FAILURE;
        }
        pConfig->AutoTestCount = GetConfParaInt(&stConfParser, SAMPLE_VirVi_Auto_Test_Count, 0);
        pConfig->GetFrameCount = GetConfParaInt(&stConfParser, SAMPLE_VirVi_Get_Frame_Count, 0);
        pConfig->DevNum = GetConfParaInt(&stConfParser, SAMPLE_VirVi_Dev_Num, 0);
        pConfig->FrameRate = GetConfParaInt(&stConfParser, SAMPLE_VirVi_Frame_Rate, 0);
        pConfig->PicWidth = GetConfParaInt(&stConfParser, SAMPLE_VirVi_Pic_Width, 0);
        pConfig->PicHeight = GetConfParaInt(&stConfParser, SAMPLE_VirVi_Pic_Height, 0);
        char *pStrPixelFormat = (char*)GetConfParaString(&stConfParser, SAMPLE_VirVi_Pic_Format, NULL);
        if(!strcmp(pStrPixelFormat, "nv21"))
        {
            alogd("nv21!!!");
            pConfig->PicFormat = V4L2_PIX_FMT_NV21M;
        }
        else
        {
            aloge("fatal error! conf file pic_format must be yuv420sp");
            pConfig->PicFormat = V4L2_PIX_FMT_NV21M;
        }
        pConfig->mRawStoreCount = GetConfParaInt(&stConfParser, SAMPLE_VirVi_RawStoreCount, 0);
        pConfig->mRawStoreInterval = GetConfParaInt(&stConfParser, SAMPLE_VirVi_RawStoreInterval, 0);
        pConfig->mJpegStoreCount = GetConfParaInt(&stConfParser, SAMPLE_VirVi_JpegStoreCount, 0);
        pConfig->mJpegStoreInterval = GetConfParaInt(&stConfParser, SAMPLE_VirVi_JpegStoreInterval, 0);
        char *pDir = (char*)GetConfParaString(&stConfParser, SAMPLE_VirVi_StoreDir, NULL);
        strcpy(pConfig->mStoreDirectory, pDir);
        alogd("dev_num=%d, pic_width=%d, pic_height=%d, pic_frame_rate=%d, storDir=[%s]",
           pConfig->DevNum,pConfig->PicWidth,pConfig->PicHeight,pConfig->FrameRate, pConfig->mStoreDirectory);
        destroyConfParser(&stConfParser);
    }
    return SUCCESS;
}

int saveRawPicture(VirVi_Cap_S *pCap, VIDEO_FRAME_INFO_S *pFrameInfo)
{
    SampleVirViContext *pContext = (SampleVirViContext*)pCap->mpContext;
    //make file name, e.g., /mnt/extsd/pic[0].NV21M
    int i;
    char strPixFmt[16] = {0};
    switch(pContext->mConfigPara.PicFormat)
    {
        case V4L2_PIX_FMT_NV12M:
        {
            strcpy(strPixFmt,"nv12m");
            break;
        }
        case V4L2_PIX_FMT_NV12:
        {
            strcpy(strPixFmt,"nv12");
            break;
        }
        case V4L2_PIX_FMT_NV21M:
        {
            strcpy(strPixFmt,"nv21m");
            break;
        }
        case V4L2_PIX_FMT_NV21:
        {
            strcpy(strPixFmt,"nv21");
            break;
        }
        default:
        {
            strcpy(strPixFmt,"unknown");
            break;
        }
    }
    char strFilePath[MAX_FILE_PATH_SIZE*2];
    snprintf(strFilePath, MAX_FILE_PATH_SIZE*2, "%s/pic[%d].%s", pContext->mConfigPara.mStoreDirectory, pCap->mRawStoreNum, strPixFmt);

    int nLen;
    FILE *fpPic = fopen(strFilePath, "wb");
    if(fpPic != NULL)
    {
        VideoFrameBufferSizeInfo FrameSizeInfo;
        getVideoFrameBufferSizeInfo(pFrameInfo, &FrameSizeInfo);
        int yuvSize[3] = {FrameSizeInfo.mYSize, FrameSizeInfo.mUSize, FrameSizeInfo.mVSize};
        for(i=0; i<3; i++)
        {
            if(pFrameInfo->VFrame.mpVirAddr[i] != NULL)
            {
                nLen = fwrite(pFrameInfo->VFrame.mpVirAddr[i], 1, yuvSize[i], fpPic);
                if(nLen != yuvSize[i])
                {
                    aloge("fatal error! fwrite fail,[%d]!=[%d], virAddr[%d]=[%p]", nLen, yuvSize[i], i, pFrameInfo->VFrame.mpVirAddr[i]);
                }
                alogd("virAddr[%d]=[%p], length=[%d]", i, pFrameInfo->VFrame.mpVirAddr[i], yuvSize[i]);
            }
        }
        fclose(fpPic);
        alogd("store raw frame in file[%s]", strFilePath);
    }
    else
    {
        aloge("fatal error! open file[%s] fail!", strFilePath);
    }
    return 0;
}

static ERRORTYPE sampleVirvi_VEncCallback(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    ERRORTYPE ret = SUCCESS;
    VirVi_Cap_S *pThiz = (VirVi_Cap_S*)cookie;
    if(MOD_ID_VENC == pChn->mModId)
    {
        if(pChn->mChnId != pThiz->mChn)
        {
            aloge("fatal error! VO chnId[%d]!=[%d]", pChn->mChnId, pThiz->mChn);
        }
        switch(event)
        {
            case MPP_EVENT_RELEASE_VIDEO_BUFFER:
            {
                VIDEO_FRAME_INFO_S *pVideoFrameInfo = (VIDEO_FRAME_INFO_S*)pEventData;
                if(pThiz->mCurFrameId != pVideoFrameInfo->mId)
                {
                    aloge("fatal error! frameId is not match[%d]!=[%d]!", pThiz->mCurFrameId, pVideoFrameInfo->mId);
                }
                cdx_sem_up(&pThiz->mSemFrameBack);
                break;
            }
            case MPP_EVENT_VENC_BUFFER_FULL:
            {
                alogd("jpeg encoder chn[%d] vbvBuffer full", pChn->mChnId);
                break;
            }
            default:
            {
                aloge("fatal error! unknown event[0x%x] from channel[0x%x][0x%x][0x%x]!", event, pChn->mModId, pChn->mDevId, pChn->mChnId);
                ret = ERR_VENC_ILLEGAL_PARAM;
                break;
            }
        }
    }
    else
    {
        aloge("fatal error! why modId[0x%x]?", pChn->mModId);
        ret = FAILURE;
    }
    return ret;
}

int saveJpegPicture(VirVi_Cap_S *pCap, VIDEO_FRAME_INFO_S *pFrameInfo)
{
    SampleVirViContext *pContext = (SampleVirViContext*)pCap->mpContext;

    //call mpi_venc to encode jpeg.
    int result = SUCCESS;
    ERRORTYPE ret;
    ERRORTYPE venc_ret = SUCCESS;
    unsigned int nVbvBufSize = ALIGN((pFrameInfo->VFrame.mWidth*pFrameInfo->VFrame.mHeight*3/2 /2) + (pFrameInfo->VFrame.mWidth*pFrameInfo->VFrame.mHeight/3), 1024);
    memset(&pCap->mChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
    pCap->mChnAttr.VeAttr.Type = PT_JPEG;
    pCap->mChnAttr.VeAttr.AttrJpeg.MaxPicWidth = 0;
    pCap->mChnAttr.VeAttr.AttrJpeg.MaxPicHeight = 0;
    pCap->mChnAttr.VeAttr.AttrJpeg.BufSize = nVbvBufSize;
    pCap->mChnAttr.VeAttr.AttrJpeg.bByFrame = TRUE;
    pCap->mChnAttr.VeAttr.AttrJpeg.PicWidth = pFrameInfo->VFrame.mWidth;
    pCap->mChnAttr.VeAttr.AttrJpeg.PicHeight = pFrameInfo->VFrame.mHeight;
    pCap->mChnAttr.VeAttr.AttrJpeg.bSupportDCF = FALSE;
    pCap->mChnAttr.VeAttr.MaxKeyInterval = 1;
    pCap->mChnAttr.VeAttr.SrcPicWidth = pFrameInfo->VFrame.mWidth;
    pCap->mChnAttr.VeAttr.SrcPicHeight = pFrameInfo->VFrame.mHeight;
    pCap->mChnAttr.VeAttr.Field = VIDEO_FIELD_FRAME;
    pCap->mChnAttr.VeAttr.PixelFormat = pFrameInfo->VFrame.mPixelFormat;
    pCap->mChnAttr.VeAttr.mColorSpace = pContext->mConfigPara.mColorSpace;
    alogd("pixfmt:0x%x, colorSpace:0x%x", pCap->mChnAttr.VeAttr.PixelFormat, pCap->mChnAttr.VeAttr.mColorSpace);
    bool bSuccessFlag = false;
    pCap->mChn = 0;
    while(pCap->mChn < VENC_MAX_CHN_NUM)
    {
        ret = AW_MPI_VENC_CreateChn(pCap->mChn, &pCap->mChnAttr);
        if(SUCCESS == ret)
        {
            bSuccessFlag = true;
            alogd("create venc channel[%d] success!", pCap->mChn);
            break;
        }
        else if(ERR_VENC_EXIST == ret)
        {
            alogd("venc channel[%d] is exist, find next!", pCap->mChn);
            pCap->mChn++;
        }
        else
        {
            aloge("fatal error! create venc channel[%d] ret[0x%x], find next!", pCap->mChn, ret);
            pCap->mChn++;
        }
    }
    if(false == bSuccessFlag)
    {
        pCap->mChn = MM_INVALID_CHN;
        aloge("fatal error! create venc channel fail!");
        result = FAILURE;
        goto _err0;
    }

    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void*)pCap;
    cbInfo.callback = (MPPCallbackFuncType)&sampleVirvi_VEncCallback;
    AW_MPI_VENC_RegisterCallback(pCap->mChn, &cbInfo);

    memset(&pCap->mJpegParam, 0, sizeof(VENC_PARAM_JPEG_S));
    pCap->mJpegParam.Qfactor = 90;
    AW_MPI_VENC_SetJpegParam(pCap->mChn, &pCap->mJpegParam);
    AW_MPI_VENC_ForbidDiscardingFrame(pCap->mChn, TRUE);

    venc_ret = AW_MPI_VENC_StartRecvPic(pCap->mChn);
    if(SUCCESS != venc_ret)
    {
        aloge("fatal error:%x jpegEnc AW_MPI_VENC_StartRecvPic",venc_ret);
        result = FAILURE;
    }

    pCap->mCurFrameId = pFrameInfo->mId;
    memset(&pCap->mExifInfo, 0, sizeof(VENC_EXIFINFO_S));
    time_t t;
    struct tm *tm_t;
    time(&t);
    tm_t = localtime(&t);
    snprintf((char*)pCap->mExifInfo.DateTime, MM_DATA_TIME_LENGTH, "%4d:%02d:%02d %02d:%02d:%02d",
        tm_t->tm_year+1900, tm_t->tm_mon+1, tm_t->tm_mday,
        tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);
    pCap->mExifInfo.ThumbWidth = 320;
    pCap->mExifInfo.ThumbHeight = 240;
    pCap->mExifInfo.thumb_quality = 60;
    pCap->mExifInfo.Orientation = 0;
    pCap->mExifInfo.fr32FNumber = FRACTION32(10, 26);
    pCap->mExifInfo.MeteringMode = AVERAGE_AW_EXIF;
    pCap->mExifInfo.fr32FocalLength = FRACTION32(100, 228);
    pCap->mExifInfo.WhiteBalance = 0;
    pCap->mExifInfo.FocalLengthIn35mmFilm = 18;
    strcpy((char*)pCap->mExifInfo.ImageName, "aw-photo");
    AW_MPI_VENC_SetJpegExifInfo(pCap->mChn, &pCap->mExifInfo);
    AW_MPI_VENC_SendFrame(pCap->mChn, pFrameInfo, 0);
    //cdx_sem_down(&mSemFrameBack);
    ret = cdx_sem_down_timedwait(&pCap->mSemFrameBack, 10*1000);
    if(ret != 0)
    {
        aloge("fatal error! jpeg encode error[0x%x]", ret);
        result = FAILURE;
    }
    ret = AW_MPI_VENC_GetStream(pCap->mChn, &pCap->mOutStream, 1000);
    if(ret != SUCCESS)
    {
        aloge("fatal error! why get stream fail?");
        result = FAILURE;
    }
    else
    {
        AW_MPI_VENC_GetJpegThumbBuffer(pCap->mChn, &pCap->mJpegThumbBuf);
    }
    if(SUCCESS == ret)
    {
        //make file name, e.g., /mnt/extsd/pic[0].jpg
        char strFilePath[MAX_FILE_PATH_SIZE*2];
        snprintf(strFilePath, MAX_FILE_PATH_SIZE*2, "%s/pic[%d].jpg", pContext->mConfigPara.mStoreDirectory, pCap->mJpegStoreNum);

        FILE *fpPic = fopen(strFilePath, "wb");
        if(fpPic != NULL)
        {
            if(pCap->mOutStream.mpPack[0].mpAddr0 != NULL && pCap->mOutStream.mpPack[0].mLen0 > 0)
            {
                fwrite(pCap->mOutStream.mpPack[0].mpAddr0, 1, pCap->mOutStream.mpPack[0].mLen0, fpPic);
                alogd("virAddr0=[%p], length=[%d]", pCap->mOutStream.mpPack[0].mpAddr0, pCap->mOutStream.mpPack[0].mLen0);
            }
            if(pCap->mOutStream.mpPack[0].mpAddr1 != NULL && pCap->mOutStream.mpPack[0].mLen1 > 0)
            {
                fwrite(pCap->mOutStream.mpPack[0].mpAddr1, 1, pCap->mOutStream.mpPack[0].mLen1, fpPic);
                alogd("virAddr1=[%p], length=[%d]", pCap->mOutStream.mpPack[0].mpAddr1, pCap->mOutStream.mpPack[0].mLen1);
            }
            if(pCap->mOutStream.mpPack[0].mpAddr2 != NULL && pCap->mOutStream.mpPack[0].mLen2 > 0)
            {
                fwrite(pCap->mOutStream.mpPack[0].mpAddr2, 1, pCap->mOutStream.mpPack[0].mLen2, fpPic);
                alogd("virAddr2=[%p], length=[%d]", pCap->mOutStream.mpPack[0].mpAddr2, pCap->mOutStream.mpPack[0].mLen2);
            }
            fclose(fpPic);
            alogd("store jpeg in file[%s]", strFilePath);
        }
        else
        {
            aloge("fatal error! open file[%s] fail!", strFilePath);
        }
    }
    ret = AW_MPI_VENC_ReleaseStream(pCap->mChn, &pCap->mOutStream);
    if(ret != SUCCESS)
    {
        aloge("fatal error! why release stream fail(0x%x)?", ret);
        result = FAILURE;
    }
    AW_MPI_VENC_StopRecvPic(pCap->mChn);
    //can't reset channel now, because in non-tunnel mode, mpi_venc will force release out frames, but we don't want
    //those out frames to be released before we return them.
    //AW_MPI_VENC_ResetChn(mChn); 
    AW_MPI_VENC_DestroyChn(pCap->mChn);
    pCap->mChn = MM_INVALID_CHN;

    return result;
    
_err0:
    return result;
}

static void *GetCSIFrameThread(void *pArg)
{
    VI_DEV ViDev;
    VI_CHN ViCh;
    int ret = 0;
    int i = 0, j = 0;

    VirVi_Cap_S *pCap = (VirVi_Cap_S *)pArg;
    SampleVirViContext *pContext = (SampleVirViContext*)pCap->mpContext;
    ViDev = pCap->Dev;
    ViCh = pCap->Chn;
    alogd("loop Sample_virvi, Cap threadid=0x%lx, ViDev = %d, ViCh = %d", pCap->thid, ViDev, ViCh);
    while (pContext->mConfigPara.GetFrameCount != j)
    {
        if ((ret = AW_MPI_VI_GetFrame(ViDev, ViCh, &pCap->pstFrameInfo, pCap->s32MilliSec)) != 0)
        {
            printf("VI Get Frame failed!\n");
            continue ;
        }
        i++;
        if (i % 60 == 0)
        {
            time_t now;
            struct tm *timenow;
            time(&now);
            timenow = localtime(&now);
            alogd("Cap threadid=0x%lx, ViDev=%d,VirVi=%d,mpts=%lld; local time is %s", pCap->thid, ViDev, ViCh, pCap->pstFrameInfo.VFrame.mpts,asctime(timenow));
#if 0
            FILE *fd;
            char filename[128];
            sprintf(filename, "/tmp/%dx%d_%d.yuv",
                pCap->pstFrameInfo.VFrame.mWidth,
                pCap->pstFrameInfo.VFrame.mHeight,
                i);
            fd = fopen(filename, "wb+");
            fwrite(pCap->pstFrameInfo.VFrame.mpVirAddr[0],
                    pCap->pstFrameInfo.VFrame.mWidth * pCap->pstFrameInfo.VFrame.mHeight,
                    1, fd);
            fwrite(pCap->pstFrameInfo.VFrame.mpVirAddr[1],
                    pCap->pstFrameInfo.VFrame.mWidth * pCap->pstFrameInfo.VFrame.mHeight >> 1,
                    1, fd);
            fclose(fd);
#endif
        }
        if(pContext->mConfigPara.mRawStoreCount > 0 
            && pCap->mRawStoreNum < pContext->mConfigPara.mRawStoreCount 
            && 0 == j%pContext->mConfigPara.mRawStoreInterval)
        {
            saveRawPicture(pCap, &pCap->pstFrameInfo);
            pCap->mRawStoreNum++;
        }
        if(pContext->mConfigPara.mJpegStoreCount > 0 
            && pCap->mJpegStoreNum < pContext->mConfigPara.mJpegStoreCount 
            && 0 == j%pContext->mConfigPara.mJpegStoreInterval)
        {
            saveJpegPicture(pCap, &pCap->pstFrameInfo);
            pCap->mJpegStoreNum++;
        }
        AW_MPI_VI_ReleaseFrame(ViDev, ViCh, &pCap->pstFrameInfo);
        j++;
    }
    return NULL;
}

void VI_HELP(void)
{
    printf("Run CSI0/CSI1 command: ./sample_virvi -path ./sample_virvi.conf\r\n");
}

int initVirVi_Cap_S(VirVi_Cap_S *pThiz)
{
    memset(pThiz, 0, sizeof(VirVi_Cap_S));
    cdx_sem_init(&pThiz->mSemFrameBack, 0);
    VENC_PACK_S *pEncPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));
    memset(pEncPack, 0, sizeof(VENC_PACK_S));
    pThiz->mOutStream.mpPack = pEncPack;
    pThiz->mOutStream.mPackCount = 1;
    return 0;
}
int destroyVirVi_Cap_S(VirVi_Cap_S *pThiz)
{
    free(pThiz->mOutStream.mpPack);
    pThiz->mOutStream.mpPack = NULL;
    pThiz->mOutStream.mPackCount = 0;
    cdx_sem_deinit(&pThiz->mSemFrameBack);
    return 0;
}

int main_sample_virvi(int argc, char *argv[])
{
    int ret = 0,result = 0;
    int vipp_dev, virvi_chn;
    int count = 0;

    //printf("Sample virvi buile time = %s, %s.\r\n", __DATE__, __TIME__);
    SampleVirViContext *pContext = (SampleVirViContext*)malloc(sizeof(SampleVirViContext));
    memset(pContext, 0, sizeof(SampleVirViContext));
    char *pConfigFilePath;
    if (argc > 1)
    {
        /* parse command line param,read sample_virvi.conf */
        if(ParseCmdLine(argc, argv, &pContext->mCmdLinePara) != 0)
        {
            aloge("fatal error! command line param is wrong, exit!");
            result = -1;
            goto _exit;
        }

        if(strlen(pContext->mCmdLinePara.mConfigFilePath) > 0)
        {
            pConfigFilePath = pContext->mCmdLinePara.mConfigFilePath;
        }
        else
        {
            pConfigFilePath = DEFAULT_SAMPLE_VIRVI_CONF_PATH;
        }
    }
    else
    {
        //VI_HELP();
        //exit(0);
        pConfigFilePath = NULL;
    }
    /* parse config file. */
    if(loadSampleVirViConfig(&pContext->mConfigPara, pConfigFilePath) != SUCCESS)
    {
        aloge("fatal error! no config file or parse conf file fail");
        result = -1;
        goto _exit;
    }
    vipp_dev = pContext->mConfigPara.DevNum;
    while (count != pContext->mConfigPara.AutoTestCount)
    {
        printf("======================================.\n");
        printf("Auto Test count : %d. (MaxCount==%d).\n", count, pContext->mConfigPara.AutoTestCount);
        //system("cat /proc/meminfo | grep Committed_AS");
        printf("======================================.\n");
        /* start mpp systerm */
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

        VI_ATTR_S stAttr;

        /* dev:0, chn:0,1,2,3 */
        /* dev:1, chn:0,1,2,3 */
        /* dev:2, chn:0,1,2,3 */
        /* dev:3, chn:0,1,2,3 */
        /*Set VI Channel Attribute*/
        memset(&stAttr, 0, sizeof(VI_ATTR_S));
        stAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        stAttr.memtype = V4L2_MEMORY_MMAP;
        stAttr.format.pixelformat = pContext->mConfigPara.PicFormat; // V4L2_PIX_FMT_SBGGR12; 
        stAttr.format.field = V4L2_FIELD_NONE;
        stAttr.format.colorspace = pContext->mConfigPara.mColorSpace;
        stAttr.format.width = pContext->mConfigPara.PicWidth;
        stAttr.format.height = pContext->mConfigPara.PicHeight;
        stAttr.nbufs = 5;
        stAttr.nplanes = 2;
        stAttr.fps = pContext->mConfigPara.FrameRate;
        /* update configuration anyway, do not use current configuration */
        stAttr.use_current_win = 0;
        stAttr.wdr_mode = 0;
        stAttr.capturemode = V4L2_MODE_VIDEO; /* V4L2_MODE_VIDEO; V4L2_MODE_IMAGE; V4L2_MODE_PREVIEW */
        stAttr.drop_frame_num = 0; // drop 2 second video data, default=0
        AW_MPI_VI_CreateVipp(vipp_dev);
        AW_MPI_VI_SetVippAttr(vipp_dev, &stAttr);
        AW_MPI_VI_EnableVipp(vipp_dev);
#define ISP_RUN 1
#if ISP_RUN
        int iIspDev = 0;
        /* open isp */
//        if (vipp_dev == 0 || vipp_dev == 2) {
//            iIspDev = 1;
//        } else if (vipp_dev == 1 || vipp_dev == 3) {
//            iIspDev = 0;
//        }
        AW_MPI_ISP_Init();
        AW_MPI_ISP_Run(iIspDev);
#endif
        usleep(100*1000);
        // for (virvi_chn = 0; virvi_chn < MAX_VIR_CHN_NUM; virvi_chn++)
        for (virvi_chn = 0; virvi_chn < 1; virvi_chn++)
        {
            initVirVi_Cap_S(&pContext->privCap[vipp_dev][virvi_chn]);
            pContext->privCap[vipp_dev][virvi_chn].Dev = pContext->mConfigPara.DevNum;
            pContext->privCap[vipp_dev][virvi_chn].Chn = virvi_chn;
            pContext->privCap[vipp_dev][virvi_chn].s32MilliSec = 5000;  // 2000;
            pContext->privCap[vipp_dev][virvi_chn].mpContext = (void*)pContext;
            //if (0 == virvi_chn)/* H264, H265, MJPG, Preview(LCD or HDMI), VDA, ISE, AIE, CVBS */
            //{
                ret = hal_virvi_start(vipp_dev, virvi_chn, NULL); /* For H264 */
                if(ret != 0)
                {
                    aloge("virvi start failed!");
                    result = -1;
                    goto _exit;
                }
                pContext->privCap[vipp_dev][virvi_chn].thid = 0;
                ret = pthread_create(&pContext->privCap[vipp_dev][virvi_chn].thid, NULL, GetCSIFrameThread, (void *)&pContext->privCap[vipp_dev][virvi_chn]);
                if (ret < 0)
                {
                    alogd("pthread_create failed, Dev[%d], Chn[%d].\n", pContext->privCap[vipp_dev][virvi_chn].Dev, pContext->privCap[vipp_dev][virvi_chn].Chn);
                    continue;
                }
                else
                {
                    alogd("vipp[%d]virChn[%d]: get csi frame thread id=[0x%lx]", vipp_dev, virvi_chn, pContext->privCap[vipp_dev][virvi_chn].thid);
                }
            //}
        }
        for (virvi_chn = 0; virvi_chn < 1; virvi_chn++)
        {
            pthread_join(pContext->privCap[vipp_dev][virvi_chn].thid, NULL);
        }
        for (virvi_chn = 0; virvi_chn < 1; virvi_chn++)
        {
            ret = hal_virvi_end(vipp_dev, virvi_chn);
            if(ret != 0)
            {
                aloge("virvi end failed!");
                result = -1;
                goto _exit;
            }
        }
        for (virvi_chn = 0; virvi_chn < 1; virvi_chn++)
        {
            destroyVirVi_Cap_S(&pContext->privCap[vipp_dev][virvi_chn]);
        }

        AW_MPI_VI_DisableVipp(vipp_dev);
#if ISP_RUN
        AW_MPI_ISP_Stop(iIspDev);
#endif
        AW_MPI_VI_DestoryVipp(vipp_dev);
        /* exit mpp systerm */
        ret = AW_MPI_SYS_Exit();
        if (ret < 0)
        {
            aloge("sys exit failed!");
            result = -1;
            break;
        }
        count++;
    }
    free(pContext);
    printf("sample_virvi exit!\n");
    return 0;
_exit:
    return result;
}
FINSH_FUNCTION_EXPORT_ALIAS(main_sample_virvi, __cmd_sample_virvi, test mpi_vi);

