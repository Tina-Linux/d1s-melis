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
#include <log.h>
#include <libc.h>
#include "audio_rec.h"
#include "drv_audio.h"
#include "mod_audio.h"

/*
*********************************************************************************************************
*                           AUDIO RECORD DEVICE MAIN TASK
*
*Description: audio rec device main task.
*
*Arguments  : parg;
*
*Return     :
*********************************************************************************************************
*/
void audio_rec_task(void *parg)
{
    __u8                        err;
    unsigned long               usr_msg;
    size_t                      frames = 0;
    ssize_t                     ret = 0;
    unsigned long               tmpSampDataSize = 0;
    sunxi_driver_audio_t        *pUsr = (sunxi_driver_audio_t *)parg;
    audio_rec_dev_params_t     *arecDevPara = &pUsr->dev_info.arec_para;
    __u32                       tmpDmaSize = arecDevPara->pBufferSize;
    __u8                        *tmpDmaBuf = arecDevPara->pBuffer;
    __audio_dev_buf_manager_t   *tmpBuf = &arecDevPara->buf;
    __wrn("-------------------- audio rec main task run!");

    while (1)
    {
        //check if any request to delete audio rec device main task
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            __msg("delete audio rec device main task!");
            goto _audio_rec_task_exit;
        }

        //wait user or isr to wakeup main task
        esKRNL_SemPend(arecDevPara->pSemProc, 200, &err);

        if (err != OS_NO_ERR)
        {
            continue;
        }

_try_to_get_user_msg:
        //check if any message from user
        usr_msg = esKRNL_QAccept(arecDevPara->msg, &err);

        if (err == OS_NO_ERR)
        {
            switch (usr_msg)
            {
                case AUDIO_USR_MSG_START:
                {
                    if (arecDevPara->status == AUDIO_DEV_STAT_IDLE)
                    {
                        audio_cmd_start(pUsr->mgr);
                        arecDevPara->status = AUDIO_DEV_STAT_RUN;
                    }
                    else
                    {
                        __wrn("can not start rec when rec is not idle");
                    }

                    break;
                }

                case AUDIO_USR_MSG_STOP:
                {
                    //check if need switch active user
                    if ((arecDevPara->status == AUDIO_DEV_STAT_RUN) || (arecDevPara->status == AUDIO_DEV_STAT_PAUS))
                    {
                        audio_cmd_stop(pUsr->mgr);
                        arecDevPara->status = AUDIO_DEV_STAT_IDLE;
                    }

                    break;
                }

                case AUDIO_USR_MSG_PAUS:
                {
                    if (arecDevPara->status == AUDIO_DEV_STAT_RUN)
                    {
                        //pause audio rec device
                        audio_cmd_pause(pUsr->mgr);
                        arecDevPara->status = AUDIO_DEV_STAT_PAUS;
                    }

                    break;
                }

                case AUDIO_USR_MSG_CONTI:
                {
                    //set the user status to be run
                    if (arecDevPara->status == AUDIO_DEV_STAT_PAUS)
                    {
                        //continue audio rec device
                        audio_cmd_resume(pUsr->mgr);
                        arecDevPara->status = AUDIO_DEV_STAT_RUN;
                    }

                    break;
                }

                default:
                {
                    __wrn("Ignore user message(%lx)!", usr_msg);
                    break;
                }
            }

            goto _try_to_get_user_msg;
        }

        if (arecDevPara->status != AUDIO_DEV_STAT_RUN)
        {
            __wrn("audio record device is not run!");
            continue;
        }

        frames = tmpDmaSize / pUsr->mgr->frameBytes;
        ret = alsa_read_pcm(pUsr->mgr, tmpDmaBuf, frames);
        if(ret < 0)
        {
            __err("read pcm data error!");
            continue;
        }

        tmpSampDataSize = ret * pUsr->mgr->frameBytes;
        
        //check if user buffer is full, if so, we need not save current frame
        if (CircleBufQuerySize(tmpBuf, AUDIO_DEV_QUERY_BUF_SIZE_FREE) >= tmpSampDataSize)
        {
            //write audio data to user cache from dma buffer
            CircleBufWrite(tmpBuf, tmpDmaBuf, tmpSampDataSize);
            //__log("[]%d,\n",tmpSampDataSize);
            //flush cache for dma transfer data
            esMEMS_CleanDCacheRegion(tmpDmaBuf, tmpSampDataSize);
        }
        else
        {
            __wrn("audio record buffer overflow!");
        }
		esKRNL_SemPost(arecDevPara->pSemProc);
        continue;
    }

_audio_rec_task_exit:
    __inf("-------------------- audio rec main task end!");
    esKRNL_TDel(EXEC_prioself);
}