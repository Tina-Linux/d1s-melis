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
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _BITSTREAM_MANAGER_H_
#define _BITSTREAM_MANAGER_H_

#include <pthread.h>

typedef struct StreamInfo
{
    int         nStreamOffset;
    int         nStreamLength;
    long long   nPts;
    int         nFlags;
    int         nID;

    int         CurrQp;
    int         avQp;
    int         nGopIndex;
    int         nFrameIndex;
    int         nTotalIndex;
    unsigned int nThumbLen;
    unsigned int nThumbPreExifDataLen;
}StreamInfo;

typedef struct BSListQ
{
    StreamInfo* pStreamInfos;
    int         nMaxFrameNum;
    int         nValidFrameNum;
    int         nUnReadFrameNum;
    int         nReadPos;
    int         nWritePos;
}BSListQ;

typedef struct BitStreamManager
{
    unsigned char  bEncH264Nalu;
    pthread_mutex_t mutex;
    char*           pStreamBuffer;
    char*           pStreamBufferPhyAddrEnd;
    char*           pStreamBufferPhyAddr;
    int             nStreamBufferSize;
    int             nWriteOffset;
    int             nValidDataSize;
    BSListQ         nBSListQ;
    struct ScMemOpsS *memops;
    void            *veOps;
    void            *pVeopsSelf;
}BitStreamManager;

BitStreamManager* BitStreamCreate(unsigned char bIsVbvNoCache, int nBufferSize, struct ScMemOpsS *memops,
                                      void *veOps, void *pVeopsSelf);
void BitStreamDestroy(BitStreamManager* handle);
void* BitStreamBaseAddress(BitStreamManager* handle);
void* BitStreamBasePhyAddress(BitStreamManager* handle);
void* BitStreamEndPhyAddress(BitStreamManager* handle);
int BitStreamBufferSize(BitStreamManager* handle);
int BitStreamFreeBufferSize(BitStreamManager* handle);
int BitStreamFrameNum(BitStreamManager* handle);
int BitStreamWriteOffset(BitStreamManager* handle);
int BitStreamAddOneBitstream(BitStreamManager* handle, StreamInfo* pStreamInfo);
StreamInfo* BitStreamGetOneBitstream(BitStreamManager* handle);
StreamInfo* BitStreamGetOneBitstreamInfo(BitStreamManager* handle);

int BitStreamReturnOneBitstream(BitStreamManager* handle, StreamInfo* pStreamInfo);
int BitStreamReset(BitStreamManager* handle, struct ScMemOpsS *memops);

#endif //_BITSTREAM_MANAGER_H_

#ifdef __cplusplus
}
#endif /* __cplusplus */
