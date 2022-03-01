#include <stdlib.h>
#include <log.h>
#include <list.h>
#include <sunxi_hal_common.h>
#include "drv_audio.h"
#include "mod_audio.h"

/*
*********************************************************************************************************
*                           REQUEST RESOURCE FOR AUDIO RECORD DEVICE
*
*Description: request resource for audio record device.
*
*Arguments  : none;
*
*Return     : result;
*               EPDK_OK,    request resource for audio record device successed;
*               EPDK_FAIL,  request resource for audio recrod device failed;
*********************************************************************************************************
*/
static __s32 ReqResForRecDev(sunxi_driver_audio_t *pUser)
{
    audio_rec_dev_params_t *arecDevPara = NULL;
    
    if(!pUser)
    {
        __err("audio rec device is NULL");
        return EPDK_FAIL;
    }
	__wrn("[REC]ReqResForRecDev\n");
    arecDevPara = &pUser->dev_info.arec_para;
    //create sampore for wakeup task by isr or user task
    arecDevPara->pcm_cnt  = 0;
    arecDevPara->pSemProc = esKRNL_SemCreate(0);

    if (!arecDevPara->pSemProc)
    {
        __wrn("Create seamaphore for audio rec device failed!");
        return EPDK_FAIL;
    }

    //create message queue for send message to main task
    arecDevPara->msg = esKRNL_QCreate(AUDIO_REC_MSGQ_SIZE);
	__wrn("[REC]arecDevPara->msg :%p\n",arecDevPara->msg );

    if (!arecDevPara->msg)
    {
        __wrn("Create message queue for audio rec device failed!");
        return EPDK_FAIL;
    }

    //request resource init some parameter for user to rec audio
    if (CircleBufCreate(&arecDevPara->buf, AUDIO_REC_USR_BUF_SIZE) != EPDK_OK)
    {
        __wrn("Create circle buffer for audio rec device failed!");
        return EPDK_FAIL;
    }

    arecDevPara->pBufferSize = AUDIO_REC_TMP_BUF_SIZE;
    arecDevPara->pBuffer = malloc(arecDevPara->pBufferSize);
    if (!arecDevPara->pBuffer)
    {
        __err("malloc tmpBuf for audio rec dma buffer failed!");
        return EPDK_FAIL;
    }

    //create main task for audio rec
    arecDevPara->main_task_prio = esKRNL_TCreate(audio_rec_task,
                                    (void *)pUser,
                                    0x8000,
                                    KRNL_priolevel1);

    if (!arecDevPara->main_task_prio)
    {
        __wrn("create task for audio rec device failed!");
        return EPDK_FAIL;
    }

    __inf("----------------- audio rec device task prio:%x", arecDevPara->main_task_prio);
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                           REQUEST RESOURCE FOR AUDIO RECORD DEVICE
*
*Description: release audio record device resource.
*
*Arguments  : none;
*
*Return     : result;
*               EPDK_OK,    release audio record device resource successed;
*               EPDK_FAIL,  release audio recrod device resource failed;
*********************************************************************************************************
*/
static __s32 RelRecDevRes(sunxi_driver_audio_t *pUser)
{
    __s32       i;
    __u8        err;
    audio_rec_dev_params_t *arecDevPara = NULL;
        
    if(!pUser)
    {
        __err("audio rec device is NULL");
        return EPDK_FAIL;
    }

    arecDevPara = &pUser->dev_info.arec_para;

    //if audio rec main task has been created, delete it
    if (arecDevPara->main_task_prio)
    {
        i = 0;
        //wake up audio rec device main task, and wait it exit
        esKRNL_TimeDlyResume(arecDevPara->main_task_prio);

        while (esKRNL_TDelReq(arecDevPara->main_task_prio) != OS_TASK_NOT_EXIST)
        {
            i++;

            if (i == 10)
            {
                __wrn("wait audio rec device main task exit failed, kill it!!");
                //wait audio rec main task exit failed, kill it
                esKRNL_TDel(arecDevPara->main_task_prio);
                arecDevPara->main_task_prio = 0;
                break;
            }

            esKRNL_TimeDlyResume(arecDevPara->main_task_prio);
            esKRNL_TimeDly(1);
        }
    }

    //release memory resource
    CircleBufDestroy(&arecDevPara->buf);

    //release dma buffer
    if (arecDevPara->pBuffer)
    {
        free(arecDevPara->pBuffer);
        arecDevPara->pBuffer      = NULL;
        arecDevPara->pBufferSize  = 0;
    }

    //delete semaphore
    if (arecDevPara->pSemProc)
    {
        esKRNL_SemDel(arecDevPara->pSemProc, OS_DEL_ALWAYS, &err);
        arecDevPara->pSemProc = 0;
    }

    //delete message queue
    if (arecDevPara->msg)
    {
        esKRNL_QDel(arecDevPara->msg, 0, &err);
        arecDevPara->msg = 0;
    }
	__log("[REC]RelRecDevRes\n");

    return EPDK_OK;
}

rt_err_t audio_rec_init(rt_device_t dev)
{
    int i = 0, usrIdx = 0;
	rt_err_t ret;
	int32_t counter = 0;
	sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;

	__log("[REC]audio_rec_init\n");

	if (dev == NULL)
    {
        __err("audio record device is NULL!");
        return -EINVAL;
    }

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

	if(pusr->status == 1)
	{
		__log("audio record engine already opended.!\n");
		return EPDK_OK;
	}

    pusr->mgr = audio_record_config_mgr_create();

    //request resource for audio record device
    if (ReqResForRecDev(pusr) != EPDK_OK)
    {
        __wrn("Request resource for audio record device failed!");
        //release some resource that has been requested
        RelRecDevRes(pusr);
        return EPDK_OK;
    }

    return EPDK_OK;
}

rt_err_t audio_rec_open(rt_device_t dev, rt_uint16_t oflag)
{
    int err;
    int open_mode = 0;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;

	__msg("[REC]audio_rec_open\n");

	if (dev == NULL)
    {
        __err("audio record device is NULL!");
        return -EINVAL;
    }

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    err = alsa_open_pcm(pusr->mgr, SOUND_CARD_AUDIOCODEC, 0);
    if (err < 0) {
        __err("audio record device open error: %d", err);
        return err;
    }
#if 0
    err = alsa_set_pcm_params(pusr->mgr);
    if (err < 0) {
		__err("audio set pcm param error:%d\n", err);
		return err;
	}
#endif
    //init audio record device status to idle
    pusr->dev_info.arec_para.status = AUDIO_DEV_STAT_IDLE;
    
	pusr->status = 1;
	
    return EPDK_OK;
}

rt_err_t audio_rec_close(rt_device_t dev)
{
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;

	__msg("[REC]audio_rec_close\n");

	if (dev == NULL)
    {
        __err("audio record device is NULL!");
        return -EINVAL;
    }

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    RelRecDevRes(pusr);
    pusr->status = 0;
	
	__wrn("[AUDIO_DEV_CMD_REG_USERMODE]:%d\n",pusr->dev_info.arec_para.usemode);
	switch(pusr->dev_info.arec_para.usemode)
	{
		case AUDIO_REC_USR_LINEIN:
		{
			__wrn("disable AUDIO_REC_USR_LINEIN \n");
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 23, 0);//linein L switch
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 26, 0);//linein R switch
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 10, 0);//linein R gain
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 11, 0);//linein L gain
			break;
		}
		case AUDIO_REC_USR_FMIN:
		{
			__wrn("disable AUDIO_REC_USR_FMIN \n");
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 22, 0);//fmin L switch
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 25, 0);//fmin R switch
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 8,  0);//fmin L gain (no used)
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 9,  0);//fmin R gain (no used)
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 13, 0);//adc1_2 gain switch
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 15, 160);//adc1 gain
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 16, 160);//adc2 gain
			break;
		}
		case AUDIO_REC_USR_MIC:
		{
			__wrn("disable AUDIO_REC_USR_MIC3 \n");
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 27, 0);//mic3  switch
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 5, 0);//mic1 gain switch //must be zero!
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 6, 0);//mic2 gain switch //must be zero!
			snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 7, 0);//mic3 gain switch //must be zero!

			break;
			//pusr->mgr->channels = 3;//代表adc序号，mic在adc3
			//result = alsa_set_pcm_params(pusr->mgr);
		}
        case AUDIO_REC_USR_MIXER:
        {
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 23, 0);//linein L switch
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 26, 0);//linein R switch
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 10, 0);//linein R gain
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 11, 0);//linein L gain
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 22, 0);//fmin L switch
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 25, 0);//fmin R switch
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 8,  0);//fmin L gain (no used)
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 9,  0);//fmin R gain (no used)
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 13, 0);//adc1_2 gain switch
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 15, 160);//adc1 gain
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 16, 160);//adc2 gain
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 27, 0);//mic3  switch
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 5, 0);//mic1 gain switch //must be zero!
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 6, 0);//mic2 gain switch //must be zero!
            snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 7, 0);//mic3 gain switch //must be zero!
            break;
        }
		default:
		{
			__err("not support mode:%d\n",pusr->dev_info.arec_para.usemode);
		}
		pusr->dev_info.arec_para.usemode = AUDIO_REC_USR_NONE; 
	}
	
    alsa_close_pcm(pusr->mgr);
    audio_config_mgr_release(pusr->mgr);

    return EPDK_OK;
}

rt_size_t audio_rec_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    __u32                tmpRdSize = 0;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;

	__msg("[REC]audio_rec_read\n");

	if (dev == NULL)
    {
        __err("audio record device is NULL!");
        return -EINVAL;
    }

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    if (!size)
    {
        __wrn("None audio sample data need be read!");
        return 0;
    }

    //write data to audio data to circle buffer
    tmpRdSize = CircleBufRead(&pusr->dev_info.arec_para.buf, (__u8 *)buffer, size);

    return tmpRdSize;
}

rt_size_t audio_rec_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    __wrn("No TX in ADC!!");
    return EPDK_OK;
}

rt_err_t audio_rec_control(rt_device_t dev, int cmd, void *arg)
{
    unsigned long *ubuffer = NULL;
    unsigned long aux = 0;
    unsigned long *pbuffer = NULL;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;

    __msg("[REC]audio_rec_control\n");

    if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }

    if(arg != NULL)
    {
        ubuffer = (unsigned long *)arg;
        aux     = ubuffer[0];
        pbuffer = (unsigned long *)ubuffer[1];
    }
    
    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    switch (cmd)
    {
        //register audio rec device usr mode: fm/mic/line in
        case AUDIO_DEV_CMD_REG_USERMODE:
        {
        	__s32   result = EPDK_OK;
            __wrn("[AUDIO_DEV_CMD_REG_USERMODE]:%d\n",aux);
        	if(aux == AUDIO_REC_USR_LINEIN)
        	{
        	    __wrn("enble AUDIO_REC_USR_LINEIN \n");
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 23, 1);//linein L switch
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 26, 1);//linein R switch
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 10, 1);//linein R gain
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 11, 1);//linein L gain
        	}
        	else if(aux == AUDIO_REC_USR_FMIN)
        	{
        		__wrn("enble AUDIO_REC_USR_FMIN \n");
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 22, 1);//fmin L switch
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 25, 1);//fmin R switch
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 8, 1);//fmin L gain (no used)
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 9, 1);//fmin R gain (no used)
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 13, 1);//adc1_2 gain switch
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 15, 180);//adc1 gain
                snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 16, 180);//adc2 gain
        	}
        	else if(aux == AUDIO_REC_USR_MIC)
        	{
        		__wrn("enble AUDIO_REC_USR_MIC3 \n");
				snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 27, 1);//mic3 enable
                snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 7, 31);//mic3 gain
        	}
            else if(aux == AUDIO_REC_USR_MIXER)
        	{
        		__wrn("enble AUDIO_REC_USR_MIXER \n");
                unsigned long select = *(unsigned long*)pbuffer;
                __err("select = 0x%x",select);
                if(select & AUDIO_REC_FM_L)
                {
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 22, 1);//fmin L switch
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 13, 1);//adc1_2 gain switch
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 15, 160);//adc1 gain
                }
                if(select & AUDIO_REC_FM_R)
                {
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 25, 1);//fmin R switch
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 13, 1);//adc1_2 gain switch
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 16, 160);//adc2 gain
                }
                if(select & AUDIO_REC_LINEIN_L)
                {
				    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 23, 1);//linein L switch
				    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 11, 1);//linein L gain
                }
                if(select & AUDIO_REC_LINEIN_R)
                {
				    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 26, 1);//linein R switch
				    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 10, 1);//linein R gain
                }
                if(select & AUDIO_REC_MIC)
                {
				    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 27, 1);//mic3 enable
                    snd_ctl_set_bynum(SOUND_CARD_AUDIOCODEC, 7, 31);//mic3 gain
                }
        	}
        	else
        	{
				__err("not support mode:%d\n",aux);
        	}
        	pusr->dev_info.arec_para.usemode = aux; 
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_SET_PLAYMODE:
        {
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_START:
        {
            __wrn("AUDIO_DEV_CMD_START");
            DISABLE_OS_TASK_SWITCH();
            //send message to audio rec main task
            esKRNL_QPost(pusr->dev_info.arec_para.msg, (unsigned long)(AUDIO_USR_MSG_START));
            //release semaphore to wakeup audio rec device
            __wrn("AUDIO_DEV_CMD_START");
            esKRNL_SemPost(pusr->dev_info.arec_para.pSemProc);
            ENABLE_OS_TASK_SWITCH();

            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_STOP:
        {
            __log("AUDIO_DEV_CMD_STOP");
            DISABLE_OS_TASK_SWITCH();
            //send message to audio rec main task
            esKRNL_QPost(pusr->dev_info.arec_para.msg, (unsigned long)(AUDIO_USR_MSG_STOP));
            //release semaphore to wakeup audio rec device
            esKRNL_SemPost(pusr->dev_info.arec_para.pSemProc);
            ENABLE_OS_TASK_SWITCH();

			
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_SET_PARA:
        {
            __log("AUDIO_DEV_CMD_SET_PARA");

            __audio_dev_para_t  *tmpAudioFmt = (__audio_dev_para_t *)pbuffer;
            __s32   result = EPDK_OK;

            if (!tmpAudioFmt)
            {
                __wrn("parameter is invalid when set aduio format!");
                return EPDK_FAIL;
            }

            DISABLE_OS_TASK_SWITCH();
            switch (tmpAudioFmt->bps) {
                case 16:
            	    pusr->mgr->format = SND_PCM_FORMAT_S16_LE;
            	    break;
                case 24:
            	    pusr->mgr->format = SND_PCM_FORMAT_S24_LE;
            	    break;
                default:
            	    __err("%u bits not supprot\n", tmpAudioFmt->bps);
            	    pusr->mgr->format = SND_PCM_FORMAT_S16_LE;
            	    return EPDK_FAIL;
            }
            pusr->mgr->channels = tmpAudioFmt->chn;
            pusr->mgr->sampleRate = tmpAudioFmt->fs;
            __wrn("use mode:%d\n",pusr->dev_info.arec_para.usemode);
            //if(pusr->dev_info.arec_para.usemode == AUDIO_REC_USR_MIC)
            //{
			//	//pusr->mgr->channels = 3;//代表adc序号，mic在adc3
			//	__log("mic input,set adc3 enble\n");
            //}
			//pusr->mgr->channels = 3;//代表adc序号，mic在adc3

			__log("[%d][%d][%d]\n",tmpAudioFmt->bps,tmpAudioFmt->chn,tmpAudioFmt->fs);
            result = alsa_set_pcm_params(pusr->mgr);
            if (result < 0) {
            	__err("audio set pcm param error:%d\n", result);
            	ENABLE_OS_TASK_SWITCH();
            	return result;
            }
            ENABLE_OS_TASK_SWITCH();
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_GET_PARA:
        {
            __log("AUDIO_DEV_CMD_GET_PARA");

            __audio_dev_para_t  *tmpAudioFmt = (__audio_dev_para_t *)pbuffer;

            if (!tmpAudioFmt)
            {
                __wrn("parameter is invalid when set aduio format!");
                return EPDK_FAIL;
            }

            DISABLE_OS_TASK_SWITCH();
            switch (pusr->mgr->format) {
        	    case SND_PCM_FORMAT_S16_LE:
        		    tmpAudioFmt->bps = 16;
        		    break;
        	    case SND_PCM_FORMAT_S24_LE:
        		    tmpAudioFmt->bps = 24;
        		    break;
        	    default:
        		    __err("%d format are not known\n", pusr->mgr->format);
        		    return EPDK_FAIL;
        	}

            tmpAudioFmt->chn = pusr->mgr->channels;
            tmpAudioFmt->fs  = pusr->mgr->sampleRate;
            ENABLE_OS_TASK_SWITCH();
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_PAUSE:
        {
            __msg("AUDIO_DEV_CMD_PAUSE");
            if (pusr->dev_info.arec_para.status != AUDIO_DEV_STAT_RUN)
            {
                __wrn("Audio rec device status is(%x), can't change to pause!", pusr->dev_info.arec_para.status);
                return EPDK_FAIL;
            }

            DISABLE_OS_TASK_SWITCH();
            //send message to audio rec main task
            esKRNL_QPost(pusr->dev_info.arec_para.msg, (unsigned long)(AUDIO_USR_MSG_PAUS));
            //release semaphore to wakeup audio rec device
            esKRNL_SemPost(pusr->dev_info.arec_para.pSemProc);
            ENABLE_OS_TASK_SWITCH();
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_CONTINUE:
        {
            __msg("AUDIO_DEV_CMD_CONTINUE");
            if (pusr->dev_info.arec_para.status != AUDIO_DEV_STAT_PAUS)
            {
                __wrn("Audio rec device status (%x) is not paus!", pusr->dev_info.arec_para.status);
                return EPDK_FAIL;
            }

            DISABLE_OS_TASK_SWITCH();
            //send message to audio rec main task
            esKRNL_QPost(pusr->dev_info.arec_para.msg, (unsigned long)(AUDIO_USR_MSG_CONTI));
            //release semaphore to wakeup audio rec device
            esKRNL_SemPost(pusr->dev_info.arec_para.pSemProc);
            ENABLE_OS_TASK_SWITCH();
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_SET_VOLUME:
        {
            __msg("AUDIO_DEV_CMD_SET_VOLUME");
            return alsa_mixer_set_volume(pusr->mgr, 0, 0, aux);
        }

        case AUDIO_DEV_CMD_GET_VOLUME:
        {
            __msg("AUDIO_DEV_CMD_GET_VOLUME");

            long playvolume = 0;
            if(alsa_mixer_get_volume(pusr->mgr, 0, &playvolume) == EPDK_OK)
                return playvolume;
            else
                return -1;
        }

        case AUDIO_DEV_CMD_FLUSH_BUF:
        {
            __msg("AUDIO_DEV_CMD_FLUSH_BUF");
            CircleBufFlush(&pusr->dev_info.arec_para.buf);
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_QUERY_BUFSIZE:
        {
            __msg("AUDIO_DEV_CMD_QUERY_BUFSIZE");
            return CircleBufQuerySize(&pusr->dev_info.arec_para.buf, aux);
        }

        case AUDIO_DEV_CMD_READ_DATA:
        {
           // __log("AUDIO_DEV_CMD_READ_DATA");
            //function of this command is same as MRead,
            //because of the lock (one user fread/fwrite only) of file system,
            //add an ioctrl interface to read data, to avoid the limit
            __u32                   tmpRdSize = 0;
            int size_bps,chn;

            if (!aux)
            {
                __wrn("None audio sample data need be read!");
                return 0;
            }

            //write data to audio data to circle buffer
            tmpRdSize = CircleBufRead(&pusr->dev_info.arec_para.buf, (__u8 *)pbuffer, aux);
            DISABLE_OS_TASK_SWITCH();
            switch (pusr->mgr->format) {
        	    case SND_PCM_FORMAT_S16_LE:
        		    size_bps = 2;
        		    break;
        	    case SND_PCM_FORMAT_S24_LE:
        		    size_bps = 3;
        		    break;
        	    default:
        		    __err("%d format are not known\n", pusr->mgr->format);
        		    return EPDK_FAIL;
        	}

            chn = pusr->mgr->channels;
            pusr->dev_info.arec_para.pcm_cnt += (tmpRdSize/chn/size_bps);
            ENABLE_OS_TASK_SWITCH();
           // __log("[AUDIO_DEV_CMD_READ_DATA]%d,\n",tmpRdSize);
            return tmpRdSize;
        }

        case AUDIO_DEV_CMD_GET_SAMPCNT:
        {
            __u32 temp;
            DISABLE_OS_TASK_SWITCH();
            temp = pusr->dev_info.arec_para.pcm_cnt;
            ENABLE_OS_TASK_SWITCH();
            return temp;
        }

        case AUDIO_DEV_CMD_SET_SAMPCNT:
        {
            DISABLE_OS_TASK_SWITCH();
            pusr->dev_info.arec_para.pcm_cnt = aux;
            ENABLE_OS_TASK_SWITCH();
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_RESIZE_BUF:
        default:
        {
            __wrn("Unknown audio rec device command(%x)!", cmd);
            return EPDK_FAIL;
        }
    }
}

