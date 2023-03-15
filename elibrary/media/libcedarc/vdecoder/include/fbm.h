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
#ifndef FBM_H
#define FBM_H

#include "vdecoder.h"
#include <pthread.h>
#include "veInterface.h"

#define MB_WTH 8
#define CODE_MODE_Y_BITS 3
#define CODE_MODE_C_BITS 2
#define BLK_MODE_BITS    2
#define C_DTS_BITS       4
#define ALIGN 128

enum BUFFER_TYPE
{
    BUF_TYPE_REFERENCE_DISP = 0,
    BUF_TYPE_ONLY_REFERENCE,
    BUF_TYPE_ONLY_DISP,
};

typedef struct FRAMENODEFLAG FrameNodeFlag;
struct FRAMENODEFLAG
{
    int          bUsedByDecoder;
    int          bUsedByRender;
    int          bInValidPictureQueue;
    int          bAlreadyDisplayed;
    int          bNeedRelease;
};

typedef struct FRAMENODE FrameNode;
struct FRAMENODE
{
    FrameNodeFlag Flag;
    VideoPicture  vpicture;
    FrameNode*    pNext;
};

typedef struct FRAMEBUFFERMANAGER
{
    pthread_mutex_t mutex;
    int             nMaxFrameNum;
    int             nEmptyBufferNum;
    int             nValidPictureNum;
    int             nReleaseBufferNum;
    FrameNode*      pEmptyBufferQueue;
    FrameNode*      pValidPictureQueue;
    FrameNode*      pReleaseBufferQueue;
    int             bThumbnailMode;
    FrameNode*      pFrames;

    int             bUseGpuBuf;
    int             nAlignValue;
    void*           pFbmInfo;
    int             nDecoderHoldingNum;
    int             nRenderHoldingNum;
    int             nWaitForDispNum;
    struct ScMemOpsS *memops;
    VeOpsS*           sVeops;
    void*             pVeOpsSelf;
    int               bShowLogFlag;
}Fbm;

typedef struct FBMCREATEINFO
{
    int nFrameNum;
    int nDecoderNeededMiniFrameNum;
    int nWidth;
    int nHeight;
    int ePixelFormat;
    int bThumbnailMode;

    int bGpuBufValid;
    int nAlignStride;
    int nBufferType;
    int bProgressiveFlag;
    int bIsSoftDecoderFlag;
    struct ScMemOpsS *memops;
    int b10BitStreamFlag;
    VeOpsS*           veOpsS;
    void*             pVeOpsSelf;
}FbmCreateInfo;

VideoPicture *FbmIndexToPointer(u8 index, Fbm *pFbm);

u8 FbmPointerToIndex(VideoPicture *picture, Fbm *pFbm);

Fbm* FbmCreate(FbmCreateInfo* pFbmCreateInfo, VideoFbmInfo* pFbmInfo);

void FbmDestroy(Fbm* pFbm);

void FbmFlush(Fbm* pFbm);

int FbmGetBufferInfo(Fbm* pFbm, VideoPicture* pVPicture);

int FbmTotalBufferNum(Fbm* pFbm);

int FbmEmptyBufferNum(Fbm* pFbm);

int FbmValidPictureNum(Fbm* pFbm);

VideoPicture* FbmRequestBuffer(Fbm* pFbm);

void FbmReturnBuffer(Fbm* pFbm, VideoPicture* pVPicture, int bValidPicture);

void FbmShareBuffer(Fbm* pFbm, VideoPicture* pVPicture);

VideoPicture* FbmRequestPicture(Fbm* pFbm);

int FbmReturnPicture(Fbm* pFbm, VideoPicture* pPicture);

VideoPicture* FbmNextPictureInfo(Fbm* pFbm);

int FbmAllocatePictureBuffer(Fbm* pFbm,
    VideoPicture* pPicture,
    int* nAlignValue,
    int nWidth,
    int nHeight);

int FbmFreePictureBuffer(Fbm* pFbm, VideoPicture* pPicture);
int FbmGetAlignValue(Fbm* pFbm);

FbmBufInfo* FbmGetVideoBufferInfo(VideoFbmInfo* pFbmInf);
VideoPicture* FbmSetFbmBufAddress(VideoFbmInfo* pFbmInfo,
    VideoPicture* pPicture,
    int bForbidUseFlag);
int FbmNewDispRelease(Fbm* pFbm);
int FbmSetNewDispRelease(VideoFbmInfo* pFbmInfo);
VideoPicture* FbmRequestReleasePicture(VideoFbmInfo* pFbmInfo);
VideoPicture* FbmReturnReleasePicture(VideoFbmInfo* pFbmInfo,
    VideoPicture* pVpicture,
    int bForbidUseFlag);
unsigned int FbmGetBufferOffset(Fbm* pFbm, int isYBufFlag);

int FbmGetDisplayBufferNum(Fbm* pFbm);
void FbmDebugPrintStatus(Fbm* pFbm);
#endif
