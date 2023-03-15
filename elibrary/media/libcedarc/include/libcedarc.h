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
#ifndef LIBCEDARC_H
#define LIBCEDARC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vdecoder.h"
#include "sbm.h"
#include "fbm.h"
#include "veInterface.h"
#include "sbmInterface.h"
#include "videoengine.h"

#define DEBUG_MAX_FRAME_IN_LIST 16

typedef struct VideoDecoderDebugContext
{
    int      nFrameNum;
    int      nFrameNumDuration;
    int      nThreshHoldNum;
    float    nMaxCostTime;
    float    nCostTimeList[DEBUG_MAX_FRAME_IN_LIST];
    int64_t  nStartTime;
    int64_t  nDuration;
    int64_t  nCurrentTime;
    int64_t  nHardwareCurrentTime;
    int64_t  nHardwareDuration;
    int64_t  nHardwareTotalTime;
    int64_t  nGetOneFrameFinishTime;
    float    nExtraDecodeTimeList[DEBUG_MAX_FRAME_IN_LIST];
    float    nFrameTimeList[DEBUG_MAX_FRAME_IN_LIST];
    float    nFrameSmoothTime;
    int      nWaitingForDispPicNum[DEBUG_MAX_FRAME_IN_LIST];
    int      nValidDisplayPic;
    int      nStartIndexExtra;
    unsigned int *pFrameTime;
    int      nFrameTimeNum;
    unsigned int *pDispFrameNum;
    float    nMax_hardDecodeFps;
    float    nMin_hardDecodeFps;
}VideoDecoderDebugContext;

typedef struct VideoDecoderContext
{
    VConfig             vconfig;
    VideoStreamInfo     videoStreamInfo;
    VideoEngine*        pVideoEngine;
    int                 nFbmNum;
    Fbm*                pFbm[2];
    int                 nSbmNum;
    SbmInterface*       pSbm[2];
    VideoStreamDataInfo partialStreamDataInfo[2];

    VideoDecoderDebugContext debug;
    struct ScMemOpsS *memops;
    //int                 nIonFd;
    int                 nSavePicCount;
    int                 reserved_mem;
}VideoDecoderContext;

typedef enum CEDARV_STREAM_FORMAT
{
    CEDARV_STREAM_FORMAT_UNKNOW,
    CEDARV_STREAM_FORMAT_MPEG2,
    CEDARV_STREAM_FORMAT_MPEG4,
    CEDARV_STREAM_FORMAT_REALVIDEO,
    CEDARV_STREAM_FORMAT_H264,
    CEDARV_STREAM_FORMAT_VC1,
    CEDARV_STREAM_FORMAT_AVS,
    CEDARV_STREAM_FORMAT_MJPEG,
    CEDARV_STREAM_FORMAT_VP8,
    CEDARV_STREAM_FORMAT_H265,
    CEDARV_STREAM_FORMAT_NETWORK
} cedarv_stream_format_e;

typedef enum CEDARV_SUB_FORMAT
{
    CEDARV_SUB_FORMAT_UNKNOW = 0,
    CEDARV_MPEG2_SUB_FORMAT_MPEG1,
    CEDARV_MPEG2_SUB_FORMAT_MPEG2,
    CEDARV_MPEG4_SUB_FORMAT_XVID,
    CEDARV_MPEG4_SUB_FORMAT_DIVX3,
    CEDARV_MPEG4_SUB_FORMAT_DIVX4,
    CEDARV_MPEG4_SUB_FORMAT_DIVX5,
    CEDARV_MPEG4_SUB_FORMAT_SORENSSON_H263,
    CEDARV_MPEG4_SUB_FORMAT_H263,
    CEDARV_MPEG4_SUB_FORMAT_RMG2,        //* H263 coded video stream muxed in '.rm' file.
    CEDARV_MPEG4_SUB_FORMAT_VP6,
    CEDARV_MPEG4_SUB_FORMAT_WMV1,
    CEDARV_MPEG4_SUB_FORMAT_WMV2,
    CEDARV_MPEG4_SUB_FORMAT_DIVX2,        //MSMPEG4V2
    CEDARV_MPEG4_SUB_FORMAT_DIVX1,        //MSMPEG4V1
} cedarv_sub_format_e;

typedef enum CEDARV_RESULT
{
    CEDARV_RESULT_OK                      = 0x0,      //* operation success;
    CEDARV_RESULT_FRAME_DECODED           = 0x1,      //* decode operation decodes one frame;
    CEDARV_RESULT_KEYFRAME_DECODED        = 0x3,      //* decode operation decodes one key frame;
    CEDARV_RESULT_NO_FRAME_BUFFER         = 0x4,      //* fail when try to get an empty frame buffer;
    CEDARV_RESULT_NO_BITSTREAM            = 0x5,      //* fail when try to get bitstream frame;

    CEDARV_RESULT_ERR_FAIL                = -1,       //* operation fail;
    CEDARV_RESULT_ERR_INVALID_PARAM       = -2,       //* failure caused by invalid function parameter;
    CEDARV_RESULT_ERR_INVALID_STREAM      = -3,       //* failure caused by invalid video stream data;
    CEDARV_RESULT_ERR_NO_MEMORY           = -4,       //* failure caused by memory allocation fail;
    CEDARV_RESULT_ERR_UNSUPPORTED         = -5,       //* failure caused by not supported stream content;
} cedarv_result_e;

typedef enum CEDARV_STATUS
{
    CEDARV_STATUS_STOP,
    CEDARV_STATUS_PAUSE,
    CEDARV_STATUS_PLAY,
    CEDARV_STATUS_FORWARD,
    CEDARV_STATUS_BACKWARD,
    CEDARV_STATUS_PREVIEW
} cedarv_status_e;


#ifdef __cplusplus
}
#endif

#endif

