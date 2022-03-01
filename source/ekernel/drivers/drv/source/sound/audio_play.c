#include <stdlib.h>
#include <log.h>
#include <list.h>
#include "drv_audio.h"
#include "mod_audio.h"
#include <sunxi_hal_common.h>

/* ----------------------------------------------------------------------------*/
/**
 * @brief  wakeup_sgdma <++>
 *
 * @param pUser
 */
/* ----------------------------------------------------------------------------*/
static void wakeup_sgdma(audio_play_dev_params_t *aplayDevPara, audio_play_dev_usr_t *pUser)
{
    OS_SEM_DATA tmpSemDat = {0};

    __inf("wakeup audio play task by user!");
    DISABLE_OS_TASK_SWITCH();
    aplayDevPara->usr_flag = pUser->usrId;
    //release semaphore to wakeup audio play device
    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

    if (tmpSemDat.OSCnt == 0)
    {
        esKRNL_SemPost(aplayDevPara->pSemProc);
    }

    ENABLE_OS_TASK_SWITCH();

    return;
}

static __s32 _do_with_IO_Ctrl_cmd_START(rt_device_t dev, audio_play_dev_usr_t *tmpUsr)

{
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
	audio_play_dev_params_t *aplayDevPara;
    OS_SEM_DATA tmpSemDat = {0};
    __s8   other_id;
    audio_play_dev_usr_t *other_usr;
    __u8 err = 0;
    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;
	aplayDevPara = &pusr->dev_info.aplay_para;
    hal_assert(pvfy == pusr); 
    
  //  DISABLE_OS_TASK_SWITCH();
	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);

    if (tmpUsr->status == AUDIO_DEV_STAT_RUN)
    {
      //  ENABLE_OS_TASK_SWITCH();
          esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
        __err("the play dev is already runing!");
        return EPDK_FAIL;
    }
#if 0
    //send message to audio play main task
    esKRNL_QPost(aplayDevPara->msg, (__u32)(AUDIO_USR_MSG_START | ((tmpUsr->usrId) << 16)));
    //release semaphore to wakeup audio play device
    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

    if (0 == tmpSemDat.OSCnt)
    {
        esKRNL_SemPost(aplayDevPara->pSemProc);
    }
#endif

	__wrn("[]tmpUsr->usrId:%d\n",tmpUsr->usrId);
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
        audio_cmd_stop(pusr->mgr);
    }
    alsa_prepare_pcm(pusr->mgr);
    audio_cmd_start(pusr->mgr);
    tmpUsr->status = AUDIO_DEV_STAT_RUN;
    aplayDevPara->usr_flag = tmpUsr->usrId;
    esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
   // ENABLE_OS_TASK_SWITCH();
    return EPDK_OK;
}

static __s32 _do_with_IO_Ctrl_cmd_STOP(rt_device_t dev, audio_play_dev_usr_t *tmpUsr)
{
    OS_SEM_DATA             tmpSemDat;
    __s8    other_id;
    audio_play_dev_usr_t *other_usr;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
	audio_play_dev_params_t *aplayDevPara;
	__u8 err = 0;
    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;
	aplayDevPara = &pusr->dev_info.aplay_para;
    hal_assert(pvfy == pusr); 

    if (tmpUsr->status != AUDIO_DEV_STAT_RUN)
    {
        __wrn("play audio not start!");
        return EPDK_FAIL;
    }

    tmpUsr->status = AUDIO_DEV_STAT_IDLE;

    //get other id
    if (tmpUsr->usrId == 0)
    {
        other_id = 1;
    }
    else
    {
        other_id = 0;
    }

    other_usr = &aplayDevPara->usr[other_id];
   // DISABLE_OS_TASK_SWITCH();
	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);

    if (other_usr->status != AUDIO_DEV_STAT_IDLE)
    {
        __wrn("other usr is not idle!");
        //ENABLE_OS_TASK_SWITCH();
        esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
        return EPDK_OK;
    }
    __wrn("[]tmpUsr->usrId:%d\n",tmpUsr->usrId);

    audio_cmd_stop(pusr->mgr);
    //flush user buffer
   // CircleBufFlush(&tmpUsr->buf);
    tmpUsr->status = AUDIO_DEV_STAT_PAUS;
    //clear user flag for process audio data
    aplayDevPara->usr_flag = -1;
#if 0
    //send message to audio play main task
    esKRNL_QPost(aplayDevPara->msg, (__u32)(AUDIO_USR_MSG_STOP | ((tmpUsr->usrId) << 16)));
    //release semaphore to wakeup audio play device
    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

    if (tmpSemDat.OSCnt == 0)
    {
        esKRNL_SemPost(aplayDevPara->pSemProc);
    }
#endif
    esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);

  //  ENABLE_OS_TASK_SWITCH();
    return EPDK_OK;
}

static __s32 _do_with_IO_Ctrl_cmd_SET_PARA(sunxi_driver_audio_t *pUsr, 
                                           const __audio_dev_para_t *pAudioFmt, 
                                           audio_play_dev_usr_t *tmpUsr)
{
    __s32   result = EPDK_OK;
	__u8 err = 0;
    if (!pAudioFmt)
    {
        __wrn("parameter is invalid when set aduio format!");
        return EPDK_FAIL;
    }

#ifdef CONFIG_SOC_SUN8IW19P1
    if (pAudioFmt->bps != 16 && pAudioFmt->bps != 24)
    {
        __wrn("woidth is only support 16 bit or 24 bit, now is [%d]", pAudioFmt->bps);
        return EPDK_FAIL;
    }
#elif defined CONFIG_SOC_SUN20IW1P1
    if (pAudioFmt->bps != 16 && pAudioFmt->bps != 20)
    {
        __wrn("woidth is only support 16 bit or 20 bit, now is [%d]", pAudioFmt->bps);
        return EPDK_FAIL;
    }
#endif

    if (tmpUsr->status == AUDIO_DEV_STAT_RUN)
    {
        __wrn("the play dev is runing!");
       // return EPDK_FAIL;
    }

    esKRNL_SemPend(pUsr->dev_info.aplay_para.pSemProc, 200, &err);

    switch (pAudioFmt->bps) {
	    case 16:
		    pUsr->mgr->format = SND_PCM_FORMAT_S16_LE;
		    break;
	    case 24:
		    pUsr->mgr->format = SND_PCM_FORMAT_S24_LE;
		    break;
	    default:
		    __err("%u bits not supprot\n", pAudioFmt->bps);
		    pUsr->mgr->format = SND_PCM_FORMAT_S16_LE;
		    esKRNL_SemPost(pUsr->dev_info.aplay_para.pSemProc);
		    return EPDK_FAIL;
	}
	pUsr->mgr->channels = pAudioFmt->chn;
	pUsr->mgr->sampleRate = pAudioFmt->fs;

    result = alsa_set_pcm_params(pUsr->mgr);
    if (result < 0) {
		__err("audio set pcm param error:%d\n", result);
		esKRNL_SemPost(pUsr->dev_info.aplay_para.pSemProc);
		return result;
	}
    esKRNL_SemPost(pUsr->dev_info.aplay_para.pSemProc);
    return result;
}

static __s32 _do_with_IO_Ctrl_cmd_GET_PARA(__audio_dev_para_t *tmpAudioFmt, audio_config_mgr_t *pcmMgr)
{
    if (!tmpAudioFmt)
    {
        __wrn("parameter is invalid when set aduio format!");
        return EPDK_FAIL;
    }
    
    switch (pcmMgr->format) {
	    case SND_PCM_FORMAT_S16_LE:
		    tmpAudioFmt->bps = 16;
		    break;
	    case SND_PCM_FORMAT_S24_LE:
		    tmpAudioFmt->bps = 24;
		    break;
	    default:
		    __err("%d format are not known\n", pcmMgr->format);
		    return EPDK_FAIL;
	}

    tmpAudioFmt->chn = pcmMgr->channels;
    tmpAudioFmt->fs  = pcmMgr->sampleRate;
    return EPDK_OK;
}

static __s32 _do_with_IO_Ctrl_cmd_PAUSE(rt_device_t dev, audio_play_dev_usr_t *tmpUsr)
{
    OS_SEM_DATA tmpSemDat = {0};
    __s8    other_id;
    audio_play_dev_usr_t *other_usr;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
	audio_play_dev_params_t *aplayDevPara;
	__u8 err = 0;
    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;
	aplayDevPara = &pusr->dev_info.aplay_para;
    hal_assert(pvfy == pusr); 
    
    if (tmpUsr->status != AUDIO_DEV_STAT_RUN)
    {
        __wrn("Audio play device status is(%x), can't change to pause!", tmpUsr->status);
        return EPDK_FAIL;
    }
	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);

   // DISABLE_OS_TASK_SWITCH();
#if 0
    tmpUsr->status = AUDIO_DEV_STAT_PAUS;
    //send message to audio play main task
    esKRNL_QPost(aplayDevPara->msg, (__u32)(AUDIO_USR_MSG_PAUS | ((tmpUsr->usrId) << 16)));
    //release semaphore to wakeup audio play device
    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

    if (tmpSemDat.OSCnt == 0)
    {
        esKRNL_SemPost(aplayDevPara->pSemProc);
    }
    
#endif
   	audio_cmd_pause(pusr->mgr);
    tmpUsr->status = AUDIO_DEV_STAT_PAUS;
    esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
  //  ENABLE_OS_TASK_SWITCH();
    return EPDK_OK;
}

//static __s32 _do_with_IO_Ctrl_cmd_CONTINUE(audio_play_dev_params_t *aplayDevPara, audio_play_dev_usr_t *tmpUsr)
static __s32 _do_with_IO_Ctrl_cmd_CONTINUE(rt_device_t dev, audio_play_dev_usr_t *tmpUsr)
{
    OS_SEM_DATA tmpSemDat = {0};
    __s8    other_id;
    audio_play_dev_usr_t *other_usr;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
	audio_play_dev_params_t *aplayDevPara;
	__u8 err = 0;
    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;
	aplayDevPara = &pusr->dev_info.aplay_para;
    hal_assert(pvfy == pusr); 
    
    if (tmpUsr->status != AUDIO_DEV_STAT_PAUS)
    {
        __wrn("Audio play device status (%x) is not paus!", tmpUsr->status);
        return EPDK_FAIL;
    }
	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);

    //DISABLE_OS_TASK_SWITCH();
   // tmpUsr->status = AUDIO_DEV_STAT_RUN;

   #if 0
    //send message to audio play main task
    esKRNL_QPost(aplayDevPara->msg, (__u32)(AUDIO_USR_MSG_CONTI | ((tmpUsr->usrId) << 16)));
    //release semaphore to wakeup audio play device
    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

    if (tmpSemDat.OSCnt == 0)
    {
        esKRNL_SemPost(aplayDevPara->pSemProc);
    }
#endif
	audio_cmd_resume(pusr->mgr);
    tmpUsr->status = AUDIO_DEV_STAT_RUN;
    esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
 
   // ENABLE_OS_TASK_SWITCH();
    return EPDK_OK;
}


static __s32 _do_with_IO_Ctrl_cmd_GET_SAMPLE_CNT(audio_play_dev_usr_t *tmpUsr)
{
    return 0;
}

static __s32 _do_with_IO_Ctrl_cmd_SET_SAMPLE_CNT(audio_play_dev_usr_t *tmpUsr, __s32 aux)
{
    return EPDK_OK;
}


static __s32 _do_with_IO_Ctrl_cmd_DATA_FINISH(audio_play_dev_params_t *aplayDevPara, audio_play_dev_usr_t *tmpUsr)
{
    OS_SEM_DATA tmpSemDat = {0};
	__u8 err = 0;
    //check if user is valid
    if ((tmpUsr != &aplayDevPara->usr[tmpUsr->usrId]) || (!aplayDevPara->usr[tmpUsr->usrId].used))
    {
        __wrn("audio play device handle is invaid when write audio data!");
        return EPDK_FAIL;
    }

  //  DISABLE_OS_TASK_SWITCH();
    esKRNL_SemPend(aplayDevPara->pSemProc, 200, &err);
    tmpUsr->playDataEnd = 1;
    aplayDevPara->usr_flag = tmpUsr->usrId;
   // ENABLE_OS_TASK_SWITCH();
    esKRNL_SemPost(aplayDevPara->pSemProc);

    #if 0
    //release semaphore to wakeup audio play device
    esKRNL_SemQuery(aplayDevPara->pSemProc, &tmpSemDat);

    if (0 == tmpSemDat.OSCnt)
    {
        esKRNL_SemPost(aplayDevPara->pSemProc);
    }
#endif
    return EPDK_OK;
}


rt_err_t audio_play_init(rt_device_t dev)
{
    int i = 0;
	sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;

	__wrn("[PLAY]audio_play_init,dev:%p\n",dev);

	if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

	if(pusr->status == 1)
	{
		__wrn("audio play engine already opended.!\n");
		return EPDK_OK;
	}

    pusr->mgr = audio_playback_config_mgr_create();


    //Semaphore 1: for wakeup task by isr or user task
    pusr->dev_info.aplay_para.pSemProc = esKRNL_SemCreate(1);
    if (!pusr->dev_info.aplay_para.pSemProc)
    {
        __err("Create seamaphore for audio play device failed!");
        return EPDK_FAIL;
    }

    //message queue: Queue messages from users to main task
    pusr->dev_info.aplay_para.msg = esKRNL_QCreate(AUDIO_PLAY_MSGQ_SIZE);

    if (!pusr->dev_info.aplay_para.msg)
    {
        __err("Create message queue for audio play device failed!");
        return EPDK_FAIL;
    }

    //request resource init some parameter for user to play audio
    for (i = 0; i < AUDIO_PLAY_USER_MAX; i++)
    {
        if (CircleBufCreate(&pusr->dev_info.aplay_para.usr[i].buf, AUDIO_PLAY_USER_BUF_SIZE) != EPDK_OK)
        {
            __err("Create circle buffer for audio play device failed!");
            return EPDK_FAIL;
        }

        pusr->dev_info.aplay_para.usr[i].used = 0;
        pusr->dev_info.aplay_para.usr[i].usrId = i;
    }

    pusr->dev_info.aplay_para.pBufferSize = AUDIO_PLAY_TMP_BUF_SIZE;
    pusr->dev_info.aplay_para.pBuffer = malloc(pusr->dev_info.aplay_para.pBufferSize);
    if (!pusr->dev_info.aplay_para.pBuffer)
    {
        __err("malloc tmpBuf for audio play dma buffer failed!");
        return EPDK_FAIL;
    }
#if 0
    //create main task for audio play
    pusr->dev_info.aplay_para.playMainTaskPrio = esKRNL_TCreate(audio_play_main_task,
                                                                (void *)pusr,
                                                                0x20000,
                                                                KRNL_priolevel1);

    if (!pusr->dev_info.aplay_para.playMainTaskPrio)
    {
        __err("create task for audio play device failed!");
        return EPDK_FAIL;
    }
#endif
    pusr->dev_info.aplay_para.usrCnt = 0;
    pusr->dev_info.aplay_para.activeUsrId = INVALID_PLAY_USR_ID;

    return EPDK_OK;
}

rt_err_t audio_play_exit(rt_device_t dev)
{
	sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
	__s32       i   = 0;
	__u8        err = 0;
	__u8        playTaskPrio = 0;

	__wrn("[PLAY]audio_play_exit\n");

	if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

#if 0
    playTaskPrio = pusr->dev_info.aplay_para.playMainTaskPrio;

    //if audio play main task has been created, delete it
    if (playTaskPrio)
    {
        i = 0;

        do  //wake up audio play device main task, and wait it exit
        {
            esKRNL_TimeDlyResume(playTaskPrio);
            esKRNL_TimeDly(1);
            ++i;

            if (i == 10)
            {
                __wrn("wait audio play device main task exit failed, kill it!!");
                //wait audio play main task exit failed, kill it
                //esKRNL_TDel(pusr->dev_info.aplay_para.playMainTaskPrio);
                //pusr->dev_info.aplay_para.playMainTaskPrio = 0;
                //break;/////////////////////////////////
            }
        } while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(playTaskPrio));
    }
#endif
    //release memory resource
    for (i = 0; i < AUDIO_PLAY_USER_MAX; i++)
    {
        CircleBufDestroy(&pusr->dev_info.aplay_para.usr[i].buf);
    }
    
    //release dma buffer
    if (pusr->dev_info.aplay_para.pBuffer)
    {
        free(pusr->dev_info.aplay_para.pBuffer);
        pusr->dev_info.aplay_para.pBuffer      = NULL;
        pusr->dev_info.aplay_para.pBufferSize  = 0;
    }

    //delete semaphore
    if (pusr->dev_info.aplay_para.pSemProc)
    {
        esKRNL_SemDel(pusr->dev_info.aplay_para.pSemProc, OS_DEL_ALWAYS, &err);
        pusr->dev_info.aplay_para.pSemProc = 0;
    }

    //delete message queue
    if (pusr->dev_info.aplay_para.msg)
    {
        esKRNL_QDel(pusr->dev_info.aplay_para.msg, 0, &err);
        pusr->dev_info.aplay_para.msg = 0;
    }

    return EPDK_OK;
}

rt_err_t audio_play_open(rt_device_t dev, rt_uint16_t oflag)
{
    int err;
    unsigned char i = 0;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
    audio_play_dev_usr_t *ppUser = NULL;

	__wrn("[PLAY]audio_play_open,dev:%p,%d\n",dev,dev->device_id);

	if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }

    i = dev->device_id;

    if (i >= AUDIO_PLAY_USER_MAX) //device_id is only support 0 or 1
    {
        __wrn("open device id is %d", i);
        return EPDK_OK;
    }
    
    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;
    
	if(pusr->status == 1)
	{
        __log("device is opening,device_id:%d\n", dev->device_id);
        return EPDK_OK;		
	}
    hal_assert(pvfy == pusr);

    err = alsa_open_pcm(pusr->mgr, SOUND_CARD_AUDIOCODEC, 1);
    if (err < 0) {
        __err("audio play device open error: %d", err);
        return err;
    }

    err = alsa_set_pcm_params(pusr->mgr);
    if (err < 0) {
		__err("audio set pcm param error:%d\n", err);
		return err;
	}
    if (pusr->dev_info.aplay_para.usr[i].used)
    {
        __wrn("usr %d is used!", i);
        return EPDK_OK;
    }

    ppUser = pusr->dev_info.aplay_para.usr + i;
    //assign the free audio play device to user
    ppUser->used     = 1;
    ppUser->status   = AUDIO_DEV_STAT_IDLE;
    pusr->dev_info.aplay_para.usrCnt++;
    pusr->status = 1;
	
    return EPDK_OK;
}

rt_err_t audio_play_close(rt_device_t dev)
{
    unsigned char i = 0;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
    audio_play_dev_usr_t  *ppUser = NULL;
    __u8 err = 0;

	__wrn("[PLAY]audio_play_close,dev:%p\n",dev);

	if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }

    i = dev->device_id;

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    ppUser = pusr->dev_info.aplay_para.usr + i;
    //check whether the handle is valid
    if (!ppUser->used
        || (ppUser != pusr->dev_info.aplay_para.usr + ppUser->usrId))
    {
        __wrn("audio play device handle is invaid when close device!");
        return EPDK_FAIL;
    }

   	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);

    //check if the user has been free
    if (ppUser->used)
    {
        ppUser->status = AUDIO_DEV_STAT_IDLE;
        pusr->dev_info.aplay_para.usrCnt--;
        ppUser->used = 0;
    }

    esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);

    pusr->status = 0;
    audio_play_exit(dev);

    alsa_close_pcm(pusr->mgr);
    audio_config_mgr_release(pusr->mgr);
    
    return EPDK_OK;
}

rt_size_t audio_play_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    __wrn("No RX in DAC!!");
    return EPDK_OK;
}

rt_size_t audio_play_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
    audio_play_dev_usr_t *ppUser = NULL;
    unsigned char        i = 0;
    __u32                tmpWrSize   = 0;
    rt_size_t                f_size = 0;
    __u8 err = 0;
    __u32 frames = 0;
	__msg("[PLAY]audio_play_write\n");

	if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }
    if (!size)
    {
        __err("None audio data when write to audio play device!");
        return EPDK_OK;
    }

    i = dev->device_id;

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);
	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);

    ppUser = pusr->dev_info.aplay_para.usr + i;

    if (ppUser->status != AUDIO_DEV_STAT_RUN)
    {
        __err("this id is not runing.");
        esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
        return EPDK_OK;
    }

#if 0
    f_size = (rt_size_t)CircleBufQuerySize(&ppUser->buf, AUDIO_DEV_QUERY_BUF_SIZE_FREE);

    if (f_size < size)
    {
        __wrn("f_size %d, size %d.", f_size, size);
        wakeup_sgdma(&pusr->dev_info.aplay_para, ppUser);
        return EPDK_OK;
    }

    tmpWrSize = CircleBufWrite(&ppUser->buf, (__u8 *)buffer, size);
   // __wrn("f_size %d, size %d, tmpWrSize = %d.", f_size, size, tmpWrSize);
    wakeup_sgdma(&pusr->dev_info.aplay_para, ppUser);
#endif

  //  tmpSampDataSize = CircleBufRead(tmpBuf, tmpDmaBuf, tmpDmaSize);
    if (size > 0)
    {
        frames = size / pusr->mgr->frameBytes;
        if(alsa_write_pcm(pusr->mgr, buffer, frames) < 0)
        {
            __err("write pcm data error!");
        }
    }
    else
    {
        //get no data, clear user flag
        __wrn("get no data..");
    }
    esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
    return size;
}

rt_err_t audio_play_control(rt_device_t dev, int cmd, void *arg)
{
    unsigned long *ubuffer = NULL;
    unsigned long aux = 0;
    unsigned long chn = 0;
    unsigned long *pbuffer = NULL;
    sunxi_driver_audio_t *pusr = NULL, *pvfy = NULL;
    long result = EPDK_OK;
    unsigned char        i = 0;
    audio_play_dev_usr_t *ppUser = NULL;
    OS_SEM_DATA          tmpSemDat = {0};

    __msg("[PLAY]audio_play_control\n");

	if (dev == NULL)
    {
        __err("audio play device is NULL!");
        return -EINVAL;
    }

    if(arg != NULL)
    {
        ubuffer = (unsigned long *) arg;
        aux     = (unsigned long)   ubuffer[0];
        pbuffer = (unsigned long *) ubuffer[1];
        chn     = (unsigned long)   ubuffer[2];
    }

    i = dev->device_id;

    pvfy = container_of(dev, sunxi_driver_audio_t, base);
    pusr = (sunxi_driver_audio_t *)dev->user_data;

    hal_assert(pvfy == pusr);

    ppUser = pusr->dev_info.aplay_para.usr + i;

    switch (cmd)
    {
        case AUDIO_DEV_CMD_START:
            __wrn("AUDIO_DEV_CMD_START");
            return _do_with_IO_Ctrl_cmd_START(dev, ppUser);

        case AUDIO_DEV_CMD_STOP:
            __wrn("AUDIO_DEV_CMD_STOP");
            return _do_with_IO_Ctrl_cmd_STOP(dev, ppUser);

        case AUDIO_DEV_CMD_SET_PARA:
            __msg("AUDIO_DEV_CMD_SET_PARA");
            return _do_with_IO_Ctrl_cmd_SET_PARA(pusr, (__audio_dev_para_t *)pbuffer, ppUser);

        case AUDIO_DEV_CMD_GET_PARA:
            __msg("AUDIO_DEV_CMD_GET_PARA");
            return _do_with_IO_Ctrl_cmd_GET_PARA((__audio_dev_para_t *)pbuffer, pusr->mgr);

        case AUDIO_DEV_CMD_PAUSE:
            __msg("AUDIO_DEV_CMD_PAUSE");
            return _do_with_IO_Ctrl_cmd_PAUSE(dev, ppUser);

        case AUDIO_DEV_CMD_CONTINUE:
            __msg("AUDIO_DEV_CMD_CONTINUE");
            return _do_with_IO_Ctrl_cmd_CONTINUE(dev, ppUser);

        case AUDIO_DEV_CMD_GET_SAMPCNT:
            __msg("AUDIO_DEV_CMD_GET_SAMPCNT");
            return _do_with_IO_Ctrl_cmd_GET_SAMPLE_CNT(ppUser);

        case AUDIO_DEV_CMD_SET_SAMPCNT:
            __msg("AUDIO_DEV_CMD_GET_SAMPCNT");
            return _do_with_IO_Ctrl_cmd_SET_SAMPLE_CNT(ppUser, 0);

        case AUDIO_DEV_CMD_SET_VOLUME:
        {
            __msg("AUDIO_DEV_CMD_SET_VOLUME.channel volume = %d", aux);

            return alsa_mixer_set_volume(pusr->mgr, 1, chn, aux);
        }

        case AUDIO_DEV_CMD_GET_VOLUME:
        {
            __msg("AUDIO_DEV_CMD_GET_VOLUME");

            long playvolume = 0;
            if(alsa_mixer_get_volume(pusr->mgr, 1, &playvolume) == EPDK_OK)
                return playvolume;
            else
                return -1;
        }
        case AUDIO_DEV_CMD_SET_CHAN_MODE:
        {
            __msg("AUDIO_DEV_CMD_SET_CHAN_MODE");
            long channel_volume = 0;

            if(alsa_mixer_get_volume(pusr->mgr, 1, (long *)&channel_volume) == EPDK_OK)
            {
                alsa_mixer_set_volume(pusr->mgr, 1, aux, channel_volume);
            }
            else
            {
                __log("Fail in get channel volume!");
                return EPDK_FAIL;
            }

            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_FLUSH_BUF:
        {
			__u8 err = 0;
            __wrn("AUDIO_DEV_CMD_FLUSH_BUF");
          //  DISABLE_OS_TASK_SWITCH();
          	esKRNL_SemPend(pusr->dev_info.aplay_para.pSemProc, 200, &err);
			audio_cmd_drop(pusr->mgr);
			esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
            #if 0
            //send message to audio play main task
            esKRNL_QPost(pusr->dev_info.aplay_para.msg, (__u32)(AUDIO_USR_MSG_FLUSH_BUF | ((ppUser->usrId) << 16)));
            //release semaphore to wakeup audio play device
            esKRNL_SemQuery(pusr->dev_info.aplay_para.pSemProc, &tmpSemDat);

            if (tmpSemDat.OSCnt == 0)
            {
                esKRNL_SemPost(pusr->dev_info.aplay_para.pSemProc);
            }
            #endif
           // ENABLE_OS_TASK_SWITCH();
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_DRAIN:		//用来等缓冲区数据播完
        {
            __wrn("AUDIO_DEV_CMD_FLUSH_BUF");
			audio_cmd_drain(pusr->mgr);

            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_QUERY_BUFSIZE:
        {
            __msg("AUDIO_DEV_CMD_QUERY_BUFSIZE");

			result = audio_get_playback_avail(pusr->mgr);
			return result;
        }

        case AUDIO_DEV_CMD_RESIZE_BUF:
        {
            __msg("AUDIO_DEV_CMD_RESIZE_BUF");

            if (ppUser->status != AUDIO_DEV_STAT_IDLE)
            {
                __wrn("Can't resize buffer when user has started!");
                return EPDK_FAIL;
            }

          //  CircleBufResize(&ppUser->buf, aux);
            return EPDK_OK;
        }

        case AUDIO_DEV_CMD_DATA_FINISH:
            __msg("AUDIO_DEV_CMD_DATA_FINISH");
            return _do_with_IO_Ctrl_cmd_DATA_FINISH(&pusr->dev_info.aplay_para, ppUser);

        case AUDIO_DEV_CMD_WRITE_DATA:
            __msg("AUDIO_DEV_CMD_WRITE_DATA");
            //function of this command is same as MWrite,/////////////////////////////////
            //because of the lock (one user fread/fwrite only) of file system,
            //add an ioControl interface to write data, to avoid the limit
            result = audio_play_write(dev, 0, pbuffer, aux);
            __msg("audio_play_write");
            break;//end case

        case AUDIO_DEV_CMD_MUTE:   // onoff  :1 静音   : 0 不静音
        {
            #if 0
            BSP_ADDA_Mute_OnOff(aux);

            if (g_drvAudio)
            {
                g_drvAudio->mute_flag = aux;

                if (1 == aux)
                {
                    BSP_ADDA_FlushFifo(AUDIO_BSP_DEV_TYPE_PLAY);
                }
            }
            #endif
            return EPDK_OK;
        }

        default:
        {
            __wrn("Unknown audio play device command(%x)!", cmd);
            return EPDK_FAIL;
        }
    }

    return result;
}

