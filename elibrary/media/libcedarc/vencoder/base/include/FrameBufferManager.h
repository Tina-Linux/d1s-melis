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
#ifndef _FRAME_BUFFER_MANAGER_H
#define _FRAME_BUFFER_MANAGER_H

typedef struct VencInputBufferInfo VencInputBufferInfo;

struct VencInputBufferInfo
{
    VencInputBuffer           inputbuffer;
    VencInputBufferInfo*    next;
};

typedef struct AllocateBufferManage
{
    unsigned int                min_num;
    unsigned int                max_num;
    unsigned int                buffer_num;
    unsigned int           quene_buffer_num;
    VencInputBufferInfo*   allocate_queue;
    VencInputBufferInfo*   allocate_buffer;
    pthread_mutex_t           mutex;
}AllocateBufferManage;

typedef struct InputBufferList
{
    unsigned int                size_of_list;
    unsigned int           max_size;

    VencInputBufferInfo*   used_quene;
    VencInputBufferInfo*   input_quene;
    VencInputBufferInfo*   empty_quene;
    VencInputBufferInfo*   buffer_quene;
    pthread_mutex_t           mutex;
}InputBufferList;

typedef struct FrameBufferManager
{
   AllocateBufferManage ABM_inputbuffer;
   InputBufferList      inputbuffer_list;
   unsigned int         size_y;
   unsigned int         size_c;
   unsigned int         added_buff_num;
   struct ScMemOpsS     *memops;
   VeOpsS*              veops;
   void*                pVeOpsSelf;
}FrameBufferManager;

FrameBufferManager* FrameBufferManagerCreate(int num, struct ScMemOpsS *memops,
                                                                    void *veOps, void *pVeopsSelf);
void FrameBufferManagerDestroy(FrameBufferManager* fbm);
int AddInputBuffer(FrameBufferManager* fbm, VencInputBuffer *inputbuffer);
int GetInputBuffer(FrameBufferManager* fbm, VencInputBuffer *inputbuffer);
int AddUsedInputBuffer(FrameBufferManager* fbm, VencInputBuffer *inputbuffer);
int GetUsedInputBuffer(FrameBufferManager* fbm, VencInputBuffer *inputbuffer);

int AllocateInputBuffer(FrameBufferManager* fbm, VencAllocateBufferParam *buffer_param);
int GetOneAllocateInputBuffer(FrameBufferManager* fbm, VencInputBuffer* inputbuffer);
int FlushCacheAllocateInputBuffer(FrameBufferManager* fbm, VencInputBuffer *inputbuffer);
int ReturnOneAllocateInputBuffer(FrameBufferManager* fbm, VencInputBuffer *inputbuffer);
void FreeAllocateInputBuffer(FrameBufferManager* fbm);

int ResetFrameBuffer(FrameBufferManager* fbm);
unsigned int GetUnencodedBufferNum(FrameBufferManager* fbm);

#endif //_FRAME_BUFFER_MANAGER_H

#ifdef __cplusplus
}
#endif /* __cplusplus */
