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
#include <kapi.h>
#include <log.h>

#include "circle_buf.h"
#include "mod_audio.h"

#include "dfs_posix.h"


#define DISABLE_OS_TASK_SWITCH()        esKRNL_SchedLock()
#define ENABLE_OS_TASK_SWITCH()         esKRNL_SchedUnlock()

/*
*********************************************************************************************************
*                           CIRCLE BUFFER CREATE
*
*Description: circle buffer create.
*
*Arguments  : buf_par   pointer to buffer parameter.
*             size      buffer total size;
*
*Return     : result;
*               EPDK_OK     create circle buffer successed;
*               EPDK_FAIL   create circle buffer failed;
*********************************************************************************************************
*/
__s32 CircleBufCreate(__audio_dev_buf_manager_t *buf_par, __u32 size)
{
    if (!buf_par || !size)
    {
        __wrn("[]parameter is invalid when create circle buffer!");
        return EPDK_FAIL;
    }

    //clear buffer parameter
    memset(buf_par, 0, sizeof(__audio_dev_buf_manager_t));
    //request buffer circle buffer
    buf_par->pStart = esMEMS_Palloc((size + 1023) / 1024, 0);

    if (!buf_par->pStart)
    {
        __wrn("Palloc buffer for audio circle buffer failed!");
        return EPDK_FAIL;
    }

    __msg("CircleBufCreate, size=%dk", size / 1024);
    buf_par->uTotalSize = size;
    buf_par->uBufSize = size;
    buf_par->uDataSize = 0;
    buf_par->uFreeSize = buf_par->uBufSize;
    buf_par->pRdPtr = buf_par->pStart;
    buf_par->pWrPtr = buf_par->pStart;
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           CIRCLE BUFFER DESTROY
*
*Description: circle buffer destroy.
*
*Arguments  : buf_par   pointer to buffer parameter.
*
*Return     : result;
*               EPDK_OK     destroy circle buffer successed;
*               EPDK_FAIL   destroy circle buffer failed;
*********************************************************************************************************
*/
__s32 CircleBufDestroy(__audio_dev_buf_manager_t *buf_par)
{
    if (!buf_par)
    {
        __wrn("Circle buffer parameter is invalid when destroy it!");
        return EPDK_FAIL;
    }

    if (buf_par->pStart && buf_par->uTotalSize)
    {
        esMEMS_Pfree(buf_par->pStart, buf_par->uTotalSize / 1024);
    }

    memset(buf_par, 0, sizeof(__audio_dev_buf_manager_t));
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           CIRCLE BUFFER READ
*
*Description: circle buffer read.
*
*Arguments  : buf_par   buffer parameter.
*             buf       dstination buffer address;
*             size      size of data need be read;
*
*Return     : result, size of data actully be read.
*********************************************************************************************************
*/
__s32 CircleBufRead(__audio_dev_buf_manager_t *buf_par, __u8 *buf, __u32 size)
{
    __s32                       result = 0;
    __u32                       tmpSize0, tmpSize1;
    __u8                        *tmpPtr0, *tmpPtr1;

    if (!buf_par || !buf || !size)
    {
        __wrn("No data need be read!");
        return 0;
    }

    if (!buf_par->uDataSize)
    {
        __msg("No audio data in buffer!");
        return 0;
    }

    //calculate current cache data status
    if ((buf_par->pRdPtr + buf_par->uDataSize) > (buf_par->pStart + buf_par->uBufSize))
    {
        tmpPtr0 = buf_par->pRdPtr;
        tmpSize0 = buf_par->pStart + buf_par->uBufSize - buf_par->pRdPtr;
        tmpPtr1 = buf_par->pStart;
        tmpSize1 = buf_par->uDataSize - tmpSize0;
    }
    else
    {
        tmpPtr0 = buf_par->pRdPtr;
        tmpSize0 = buf_par->uDataSize;
        tmpPtr1 = buf_par->pRdPtr + tmpSize0;
        tmpSize1 = 0;
    }

    if ((tmpSize0 + tmpSize1) >= size)
    {
        //cache data is enough for read
        if (tmpSize0 >= size)
        {
            memcpy(buf, tmpPtr0, size);
        }
        else
        {
            memcpy(buf, tmpPtr0, tmpSize0);
            memcpy(buf + tmpSize0, tmpPtr1, size - tmpSize0);
        }

        //update buffer parameter
        DISABLE_OS_TASK_SWITCH();
        buf_par->pRdPtr    += size;
        buf_par->uDataSize -= size;
        buf_par->uFreeSize += size;

        if (buf_par->pRdPtr >= buf_par->pStart + buf_par->uBufSize)
        {
            buf_par->pRdPtr -= buf_par->uBufSize;
        }

        ENABLE_OS_TASK_SWITCH();
        result = size;
    }
    else
    {
        //cache data is not enough for read
        memcpy(buf, tmpPtr0, tmpSize0);

        if (tmpSize1)
        {
            memcpy(buf + tmpSize0, tmpPtr1, tmpSize1);
        }

        //update buffer parameter
        DISABLE_OS_TASK_SWITCH();
        buf_par->pRdPtr    += tmpSize0 + tmpSize1;
        buf_par->uDataSize -= tmpSize0 + tmpSize1;
        buf_par->uFreeSize += tmpSize0 + tmpSize1;

        if (buf_par->pRdPtr >= buf_par->pStart + buf_par->uBufSize)
        {
            buf_par->pRdPtr -= buf_par->uBufSize;
        }

        ENABLE_OS_TASK_SWITCH();
        result = tmpSize0 + tmpSize1;
    }

    return result;
}


__s32 CircleBufReadZero(__audio_dev_buf_manager_t *buf_par, __u8 *buf, __u32 size)
{
    __s32                       result = 0;
    __u32                       tmpSize0, tmpSize1;
    __u8                        *tmpPtr0, *tmpPtr1;

    if (!buf_par || !buf || !size)
    {
        __wrn("No data need be read!");
        return 0;
    }

    if (!buf_par->uDataSize)
    {
        __msg("No audio data in buffer!");
        return 0;
    }

    //calculate current cache data status
    if ((buf_par->pRdPtr + buf_par->uDataSize) > (buf_par->pStart + buf_par->uBufSize))
    {
        tmpPtr0 = buf_par->pRdPtr;
        tmpSize0 = buf_par->pStart + buf_par->uBufSize - buf_par->pRdPtr;
        tmpPtr1 = buf_par->pStart;
        tmpSize1 = buf_par->uDataSize - tmpSize0;
    }
    else
    {
        tmpPtr0 = buf_par->pRdPtr;
        tmpSize0 = buf_par->uDataSize;
        tmpPtr1 = buf_par->pRdPtr + tmpSize0;
        tmpSize1 = 0;
    }

    if ((tmpSize0 + tmpSize1) >= size)
    {
        //cache data is enough for read
        if (tmpSize0 >= size)
        {
            memcpy(buf, tmpPtr0, size);
            memset(buf, 0, size);
        }
        else
        {
            memcpy(buf, tmpPtr0, tmpSize0);
            memset(buf, 0, tmpSize0);
            memcpy(buf + tmpSize0, tmpPtr1, size - tmpSize0);
            memset(buf + tmpSize0, 0, size - tmpSize0);
        }

        //update buffer parameter
        DISABLE_OS_TASK_SWITCH();
        buf_par->pRdPtr    += size;
        buf_par->uDataSize -= size;
        buf_par->uFreeSize += size;

        if (buf_par->pRdPtr >= buf_par->pStart + buf_par->uBufSize)
        {
            buf_par->pRdPtr -= buf_par->uBufSize;
        }

        ENABLE_OS_TASK_SWITCH();
        result = size;
    }
    else
    {
        //cache data is not enough for read
        memcpy(buf, tmpPtr0, tmpSize0);
        memset(buf, 0, tmpSize0);

        if (tmpSize1)
        {
            memcpy(buf + tmpSize0, tmpPtr1, tmpSize1);
            memset(buf + tmpSize0, 0, tmpSize1);
        }

        //update buffer parameter
        DISABLE_OS_TASK_SWITCH();
        buf_par->pRdPtr    += tmpSize0 + tmpSize1;
        buf_par->uDataSize -= tmpSize0 + tmpSize1;
        buf_par->uFreeSize += tmpSize0 + tmpSize1;

        if (buf_par->pRdPtr >= buf_par->pStart + buf_par->uBufSize)
        {
            buf_par->pRdPtr -= buf_par->uBufSize;
        }

        ENABLE_OS_TASK_SWITCH();
        result = tmpSize0 + tmpSize1;
    }

    return result;
}



/*
*********************************************************************************************************
*                           CIRCLE BUFFER WRITE
*
*Description: circle buffer write.
*
*Arguments  : buf_par   buffer parameter.
*             src       source buffer address;
*             size      size of data need be write;
*
*Return     : result, size of data actully be write.
*********************************************************************************************************
*/
__s32 CircleBufWrite(__audio_dev_buf_manager_t *buf_par, __u8 *buf, __u32 size)
{
    __s32                       result = 0;
    __u32                       tmpSize0, tmpSize1;
    __u8                        *tmpPtr0, *tmpPtr1;

    if (!buf_par || !buf || !size)
    {
        __wrn("No data need be write![%p][%p][%d]",buf_par,buf,size);
        return 0;
    }

    if (!buf_par->uFreeSize)
    {
        __err("No free buffer for store audio data!");
        return 0;
    }

    //calculate current free buffer status
    if ((buf_par->pWrPtr + buf_par->uFreeSize) > (buf_par->pStart + buf_par->uBufSize))
    {
        //free buffer is two segment
        tmpPtr0 = buf_par->pWrPtr;
        tmpSize0 = buf_par->pStart + buf_par->uBufSize - buf_par->pWrPtr;
        tmpPtr1 = buf_par->pStart;
        tmpSize1 = buf_par->uFreeSize - tmpSize0;
    }
    else
    {
        tmpPtr0 = buf_par->pWrPtr;
        tmpSize0 = buf_par->uFreeSize;
        tmpPtr1 = buf_par->pWrPtr + tmpSize0;
        tmpSize1 = 0;
    }

    if ((tmpSize0 + tmpSize1) >= size)
    {
        //free buffer is enough for write
        if (tmpSize0 >= size)
        {
            memcpy(tmpPtr0, buf, size);
        }
        else
        {
            memcpy(tmpPtr0, buf, tmpSize0);
            memcpy(tmpPtr1, buf + tmpSize0, size - tmpSize0);
        }

        //update buffer parameter
        DISABLE_OS_TASK_SWITCH();
        buf_par->pWrPtr    += size;
        buf_par->uFreeSize -= size;
        buf_par->uDataSize += size;

        if (buf_par->pWrPtr >= buf_par->pStart + buf_par->uBufSize)
        {
            buf_par->pWrPtr -= buf_par->uBufSize;
        }

        ENABLE_OS_TASK_SWITCH();
        result = size;
    }
    else
    {
        //cache data is not enough for read
        memcpy(tmpPtr0, buf, tmpSize0);

        if (tmpSize1)
        {
            memcpy(tmpPtr1, buf + tmpSize0, tmpSize1);
        }

        //update buffer parameter
        DISABLE_OS_TASK_SWITCH();
        buf_par->pWrPtr    += tmpSize0 + tmpSize1;
        buf_par->uDataSize += tmpSize0 + tmpSize1;
        buf_par->uFreeSize -= tmpSize0 + tmpSize1;

        if (buf_par->pWrPtr >= buf_par->pStart + buf_par->uBufSize)
        {
            buf_par->pWrPtr -= buf_par->uBufSize;
        }

        ENABLE_OS_TASK_SWITCH();
        result = tmpSize0 + tmpSize1;
    }

    //__msg("current data size:%x", buf_par->uDataSize);
    return result;
}


/*
*********************************************************************************************************
*                           CIRCLE BUFFER FLUSH
*
*Description: circle buffer flush.
*
*Arguments  : buf_par   buffer parameter.
*
*Return     :
*********************************************************************************************************
*/
__s32 CircleBufFlush(__audio_dev_buf_manager_t *buf_par)
{
    if (!buf_par)
    {
        __wrn("buffer manager parameter is invalid!");
        return EPDK_FAIL;
    }

    DISABLE_OS_TASK_SWITCH();
    buf_par->pRdPtr  = buf_par->pWrPtr;
    buf_par->uDataSize = 0;
    buf_par->uFreeSize = buf_par->uBufSize;
    ENABLE_OS_TASK_SWITCH();
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                           QUERY BUFFER SIZE
*
*Description: query buffer size.
*
*Arguments  : buf_par   buffer parameter.
*             type      buffer type need be queried, free/data/total...
*
*Return     : buffer size;
*********************************************************************************************************
*/
__s32 CircleBufQuerySize(__audio_dev_buf_manager_t *buf_par, __u32 type)
{
    if (!buf_par)
    {
        __wrn("buffer manager parameter is invalid!");
        return 0;
    }

    switch (type)
    {
        case AUDIO_DEV_QUERY_BUF_SIZE_DATA:
            return buf_par->uDataSize;

        case AUDIO_DEV_QUERY_BUF_SIZE_FREE:
            return buf_par->uFreeSize;

        case AUDIO_DEV_QUERY_BUF_SIZE_TOTAL:
            return buf_par->uBufSize;

        default:
        {
            __wrn("Unknown type when query buffer size!");
            return -1;
        }
    }
}


/*
*********************************************************************************************************
*                           CIRCLE BUFFER RESIZE
*
*Description: circle buffer resize, adjust the buffer size.
*
*Arguments  : buf_par   pointer to buffer parameter.
*             size      buffer size givern from user.
*
*Return     : result;
*               EPDK_OK     resize circle buffer successed;
*               EPDK_FAIL   resize circle buffer failed;
*********************************************************************************************************
*/
__s32 CircleBufResize(__audio_dev_buf_manager_t *buf_par, __s32 size)
{
    if (!buf_par)
    {
        __wrn("Circle buffer parameter is invalid when resize buffer!");
        return EPDK_FAIL;
    }

    DISABLE_OS_TASK_SWITCH();
    __msg("CircleBufResize");
    CircleBufDestroy(buf_par);
    CircleBufCreate(buf_par, size);
    ENABLE_OS_TASK_SWITCH();
    return EPDK_OK;
}
