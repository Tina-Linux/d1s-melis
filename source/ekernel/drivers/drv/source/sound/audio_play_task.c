#include <log.h>
#include "audio_play.h"
#include "drv_audio.h"

/*
*********************************************************************************************************
*                           REQUEST AUDIO PLAY RESOURCE
*
*Description: audio play device main task.
*
*Arguments  : parg;
*
*Return     :
*********************************************************************************************************
*/
void audio_play_main_task(void *parg)
{
    __u8                        err;
    unsigned long                       usr_msg;
    audio_play_dev_usr_t        *tmpUsr = 0;
    audio_play_dev_usr_t        *tmpUsr1 = 0;
    __audio_dev_buf_manager_t   *tmpBuf = 0;    
    __audio_dev_buf_manager_t   *tmpBuf1 = 0;
    __s32                       tmpSampDataSize = 0;
    snd_pcm_uframes_t           frames = 0;
    sunxi_driver_audio_t        *pUsr = (sunxi_driver_audio_t *)parg;
    audio_play_dev_params_t     *aplayDevPara = &pUsr->dev_info.aplay_para;
    __u32                       tmpDmaSize = aplayDevPara->pBufferSize;
    __u8                        *tmpDmaBuf = aplayDevPara->pBuffer;
    OS_SEM_DATA                 tmpSemDat = {0};

    while (1)
    {
        //check if any request to delete audio play device main task
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            __wrn("delete audio play device main task!");
            goto _audio_play_task_exit;
        }

        //wait user or isr to wakeup main task
        esKRNL_SemPend(aplayDevPara->pSemProc, 200, &err);

        if (err != OS_NO_ERR)
        {
            continue;
        }

_try_to_get_user_msg:
        //check if any message from user
        usr_msg = esKRNL_QAccept(aplayDevPara->msg, &err);       //temporarily, only the lower 32bits of usr_msg are used

        //      __msg("play task get msg..");

        if (err == OS_NO_ERR)
        {
            switch (usr_msg & 0xffff)
            {
                case AUDIO_USR_MSG_START:
                {
                    __s8   other_id;
                    audio_play_dev_usr_t *other_usr;
                    tmpUsr = &aplayDevPara->usr[(usr_msg >> 16) & 0xffff];

                    if (tmpUsr->usrId == 0)
                    {
                        other_id = 1;
                    }
                    else
                    {
                        other_id = 0;
                    }

                    other_usr = &aplayDevPara->usr[other_id];

                    if (other_usr->status == AUDIO_DEV_STAT_RUN)
                    {
                        audio_cmd_stop(pUsr->mgr);
                        //flush user buffer
                        CircleBufFlush(&other_usr->buf);
                    }

                    //alsa_prepare_pcm(pUsr->mgr);
                    audio_cmd_start(pUsr->mgr);
                    tmpUsr->status = AUDIO_DEV_STAT_RUN;
                    aplayDevPara->usr_flag = -1;
                    break;
                }

                case AUDIO_USR_MSG_STOP:
                {
                    tmpUsr = &aplayDevPara->usr[(usr_msg >> 16) & 0xffff];
                    audio_cmd_stop(pUsr->mgr);
                    //flush user buffer
                    CircleBufFlush(&tmpUsr->buf);
                    tmpUsr->status = AUDIO_DEV_STAT_IDLE;
                    //clear user flag for process audio data
                    aplayDevPara->usr_flag = -1;
                    break;
                }

                case AUDIO_USR_MSG_FLUSH_BUF:
                {
                    tmpUsr = &aplayDevPara->usr[(usr_msg >> 16) & 0xffff];
                    CircleBufFlush(&tmpUsr->buf);
                    break;
                }

                case AUDIO_USR_MSG_PAUS:
                {
                    tmpUsr = &aplayDevPara->usr[(usr_msg >> 16) & 0xffff];
                    //pause audio play device
                    audio_cmd_pause(pUsr->mgr);
                    tmpUsr->status = AUDIO_DEV_STAT_PAUS;
                    break;
                }

                case AUDIO_USR_MSG_CONTI:
                {
                    tmpUsr = &aplayDevPara->usr[(usr_msg >> 16) & 0xffff];
                    //continue audio play device
                    audio_cmd_resume(pUsr->mgr);
                    tmpUsr->status = AUDIO_DEV_STAT_RUN;
                    break;
                }

                case AUDIO_USR_MSG_RESTORE_FS:
                {
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

        tmpUsr = NULL;
        tmpUsr1 = NULL;
        tmpBuf = NULL;
        tmpBuf1 = NULL;

        if (aplayDevPara->usr[0].status == AUDIO_DEV_STAT_RUN)
        {
            tmpUsr = &aplayDevPara->usr[0];
            tmpBuf = &tmpUsr->buf;
        }

        if (aplayDevPara->usr[1].status == AUDIO_DEV_STAT_RUN)
        {
            __msg("usr1 is runing");
            tmpUsr1 = &aplayDevPara->usr[1];
            tmpBuf1 = &tmpUsr1->buf;
        }

        if (!tmpUsr && !tmpUsr1)
        {
            __msg("No active user or activue user is paused!");
            continue;
        }
        
        //////////////////////////////////////////
        //audio play main task is wakeup by user//
        /////////////////////////////////////////
        if (aplayDevPara->usr_flag == 0 || aplayDevPara->usr_flag == 1)
        {
            //__msg("wakeup by user!");
            tmpUsr = &aplayDevPara->usr[aplayDevPara->usr_flag];
            tmpBuf = &tmpUsr->buf;

            //if flag for active user is set up
            if (tmpUsr->status != AUDIO_DEV_STAT_RUN)
            {
                //no user need be responsed
                __wrn("the usr is not runing!");
                continue;
            }
            while(1)
            {
                memset(tmpDmaBuf, 0, tmpDmaSize);
                tmpSampDataSize = CircleBufRead(tmpBuf, tmpDmaBuf, tmpDmaSize);
                if (tmpSampDataSize > 0)
                {
                    frames = tmpSampDataSize / pUsr->mgr->frameBytes;
                    if(alsa_write_pcm(pUsr->mgr, tmpDmaBuf, frames) < 0)
                    {
                        __err("write pcm data error!");
                    }
                }
                else
                {
                    //get no data, clear user flag
                    __wrn("get no data..");
                    aplayDevPara->usr_flag = -1;
                        //release semaphore to wakeup audio play device
                    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

                    if (tmpSemDat.OSCnt == 0)
                    {
                        esKRNL_SemPost(aplayDevPara->pSemProc);
                    }
                    break;
                }
            }
        }
        continue;
    }

_audio_play_task_exit:
    __inf("-------------------- audio play main task end!");
    esKRNL_TDel(EXEC_prioself);
}

