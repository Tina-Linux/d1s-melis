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
#ifndef SBM_H
#define SBM_H
#include <pthread.h>

#include "sbmInterface.h"
#include "cdc_log.h"
#include "CdcMessageQueue.h"

#ifdef __cplusplus
extern "C" {
#endif

//SbmInterface* GetSbmInterfaceStream();

SbmInterface* GetSbmInterface(int nType);

#define SBM_FRAME_FIFO_SIZE (2048)  //* store 2048 frames of bitstream data at maximum.
#define MAX_INVALID_STREAM_DATA_SIZE (1*1024*1024) //* 1 MB
#define MAX_NALU_NUM_IN_FRAME (1024)
#define TMP_BUFFER_SIZE (1*1024*1024)

enum SBM_THREAD_CMD
{
    SBM_THREAD_CMD_START = 0,
    SBM_THREAD_CMD_READ  = 1,
    SBM_THREAD_CMD_QUIT  = 2,
    SBM_THREAD_CMD_RESET = 3
};

typedef struct StreamBufferManagerFrame SbmFrame;

struct StreamBufferManagerFrame
{
    SbmInterface    sbmInterface;

    pthread_mutex_t mutex;
    char*           pStreamBuffer;    //* start buffer address
    char*           pStreamBufferEnd;
    int             nStreamBufferSize; //* buffer total size
    char*           pWriteAddr;
    int             nValidDataSize;
    StreamFrameFifo frameFifo;

    CdcMessageQueue* mq;
    pthread_t        mThreadId;
    FramePicFifo     mFramePicFifo;
    sem_t            streamDataSem;
    sem_t            emptyFramePicSem;

    sem_t            resetSem;
    int              bStreamWithStartCode;
    DetectFramePicInfo mDetectInfo;
    SbmConfig mConfig;
    int       nEosFlag;

    int nDebugNum;

    char* pTmpBuffer;     //* for secure video
    int   nTmpBufferSize;

    char* pDetectBufStart;
    char* pDetectBufEnd;

    SbmFrameBufferNode*   pSbmFrameBufferValidQueue;
    SbmFrameBufferNode*   pSbmFrameBufferEmptyQueue;
    SbmFrameBufferNode*   pSbmFrameBufferLastNode;
    SbmFrameBufferNode*   pSbmFrameBuffer;
    pthread_mutex_t       pSbmFrameBufferMutex;

    s32 (*checkBitStreamType)(SbmFrame* pSbm);
    void (*detectOneFramePic)(SbmFrame* pSbm);
    int     bShowLogFlag;
};


#ifdef __cplusplus
}
#endif
#endif
