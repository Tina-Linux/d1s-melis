#include <stdlib.h>
#include <stdio.h>
#include <log.h>
#include <init.h>
#include "drv_audio.h"
#include "audio_play.h"
#include "mod_audio.h"

void sleep(int seconds);

static sunxi_driver_audio_t audio_play0, audio_play1;
static sunxi_driver_audio_t audio_rec0, audio_rec1, audio_rec2;

audio_config_mgr_t *audio_playback_config_mgr_create(void)
{
	audio_config_mgr_t *audio_conf_mgr = NULL;

	audio_conf_mgr = malloc(sizeof(audio_config_mgr_t));
	if (!audio_conf_mgr) {
		__err("no memory for create audio config manager\n");
		return NULL;
	}
	memset(audio_conf_mgr, 0, sizeof(audio_config_mgr_t));
	audio_conf_mgr->format = SND_PCM_FORMAT_S16_LE;
	audio_conf_mgr->sampleRate = 16000;
	audio_conf_mgr->channels = 2;
	audio_conf_mgr->periodSize = 1024;//1024;
	audio_conf_mgr->bufferSize = 4096;//(4*1024)
	audio_conf_mgr->can_paused = 1;

	if(NULL == audio_conf_mgr->mixerInfo)
    {
        audio_conf_mgr->mixerInfo = (snd_ctl_info_t*)malloc(sizeof(snd_ctl_info_t));
        if(NULL == audio_conf_mgr->mixerInfo)
        {
            if (audio_conf_mgr) {
                free(audio_conf_mgr);
            }
            __err("fatal error! snd_ctl_info_t malloc fail!");
            return NULL;
        }
        memset(audio_conf_mgr->mixerInfo, 0, sizeof(snd_ctl_info_t));
    }

	return audio_conf_mgr;
}

audio_config_mgr_t *audio_record_config_mgr_create(void)
{
	audio_config_mgr_t *audio_conf_mgr = NULL;

	audio_conf_mgr = malloc(sizeof(audio_config_mgr_t));
	if (!audio_conf_mgr) {
		__err("no memory for create audio config manager\n");
		return NULL;
	}
	memset(audio_conf_mgr, 0, sizeof(audio_config_mgr_t));
	audio_conf_mgr->format = SND_PCM_FORMAT_S16_LE;
	audio_conf_mgr->sampleRate = 16000;
	audio_conf_mgr->channels = 2;
	audio_conf_mgr->periodSize = 1024;//1024;
	audio_conf_mgr->bufferSize = 16384;//(16*1024)
	audio_conf_mgr->can_paused = 1;

	if(NULL == audio_conf_mgr->mixerInfo)
    {
        audio_conf_mgr->mixerInfo = (snd_ctl_info_t*)malloc(sizeof(snd_ctl_info_t));
        if(NULL == audio_conf_mgr->mixerInfo)
        {
            if (audio_conf_mgr) {
                free(audio_conf_mgr);
            }
            __err("fatal error! snd_ctl_info_t malloc fail!");
            return NULL;
        }
        memset(audio_conf_mgr->mixerInfo, 0, sizeof(snd_ctl_info_t));
    }

	return audio_conf_mgr;
}

void audio_config_mgr_release(audio_config_mgr_t *mgr)
{
	if (!mgr) {
		printf("%s: mgr null !\n", __func__);
		return;
	}
	if(mgr->mixerInfo)
	{
        free(mgr->mixerInfo);
        mgr->mixerInfo = NULL;
	}
	free(mgr);
	mgr = NULL;
}


int alsa_set_pcm_params(audio_config_mgr_t *pcmMgr)
{
    snd_pcm_hw_params_t *params;
    snd_pcm_sw_params_t *sw_params;
    int err = -1;
    int dir = 0;

    if (pcmMgr->handle == NULL) {
        __err("PCM is not open yet!");
        return err;
    }
	snd_pcm_prepare(pcmMgr->handle);
    __wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));

    /* HW params */
    snd_pcm_hw_params_alloca(&params);
    err = snd_pcm_hw_params_any(pcmMgr->handle, params);
    if (err < 0) {
        __err("Broken configuration for this PCM: no configurations available!");
        return err;
    }

    err = snd_pcm_hw_params_set_access(pcmMgr->handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        __err("Access type not available");
        return err;
    }

    err = snd_pcm_hw_params_set_format(pcmMgr->handle, params, pcmMgr->format);
    if (err < 0) {
        __err("Sample format not available");
        return err;
    }

    err = snd_pcm_hw_params_set_channels(pcmMgr->handle, params, pcmMgr->channels);
    if (err < 0) {
        __err("Channels count not available");
        return err;
    }

    err = snd_pcm_hw_params_set_rate(pcmMgr->handle, params, pcmMgr->sampleRate, 0);
    if (err < 0) {
        __err("set_rate error!");
        return err;
    }

    err = snd_pcm_hw_params_set_period_size_near(pcmMgr->handle, params, &pcmMgr->periodSize, &dir);
    //err = snd_pcm_hw_params_set_period_size(pcmMgr->handle, params, pcmMgr->periodSize, &dir);
    if (err < 0) {
        __err("set_period_size error!");
        return err;
    }

    // double 1024-sample capacity -> 4
    snd_pcm_uframes_t bufferSize = pcmMgr->bufferSize;//pcmMgr->periodSize * (pcmMgr->channels * snd_pcm_format_physical_width(pcmMgr->format)/8) * 4;
   	err =  snd_pcm_hw_params_set_buffer_size_near(pcmMgr->handle, params, &bufferSize);
    if (err < 0) {
        __err("set_buffer_size error!");
        return err;
    }

    err = snd_pcm_hw_params(pcmMgr->handle, params);
    if (err < 0) {
        __err("Unable to install hw params");
        return err;
    }

    snd_pcm_hw_params_get_period_size(params, &pcmMgr->periodSize, 0);
    snd_pcm_hw_params_get_buffer_size(params, &pcmMgr->bufferSize);
    if (pcmMgr->periodSize == pcmMgr->bufferSize) {
        __err("Can't use period equal to buffer size (%lu == %lu)", pcmMgr->periodSize, pcmMgr->bufferSize);
        return err;
    }

    /* SW params */
	snd_pcm_sw_params_alloca(&sw_params);
	snd_pcm_sw_params_current(pcmMgr->handle, sw_params);

    if (snd_pcm_stream(pcmMgr->handle) == SND_PCM_STREAM_CAPTURE) {
        snd_pcm_sw_params_set_start_threshold(pcmMgr->handle, sw_params, 1);
    } else {
        snd_pcm_uframes_t boundary = 0;
    	snd_pcm_sw_params_get_boundary(sw_params, &boundary);
    	snd_pcm_sw_params_set_start_threshold(pcmMgr->handle, sw_params, pcmMgr->bufferSize/2);
    	/* set silence size, in order to fill silence data into ringbuffer */
    	snd_pcm_sw_params_set_silence_size(pcmMgr->handle, sw_params, boundary);
    }

	snd_pcm_sw_params_set_stop_threshold(pcmMgr->handle, sw_params, pcmMgr->bufferSize);
	snd_pcm_sw_params_set_avail_min(pcmMgr->handle, sw_params, pcmMgr->periodSize);
	err = snd_pcm_sw_params(pcmMgr->handle ,sw_params);
	if (err < 0) {
		__err("Unable to install sw prams!\n");
		return err;
	}

    pcmMgr->frameBytes = snd_pcm_frames_to_bytes(pcmMgr->handle, 1);
    pcmMgr->chunkBytes = snd_pcm_frames_to_bytes(pcmMgr->handle, pcmMgr->periodSize);
    pcmMgr->can_paused = snd_pcm_hw_params_can_pause(params);
    //pcmMgr->bitsPerSample = snd_pcm_format_physical_width(pcmMgr->format);
    //pcmMgr->significantBitsPerSample = snd_pcm_format_width(pcmMgr->format);
    //pcmMgr->bitsPerFrame = pcmMgr->bitsPerSample * pcmMgr->chnCnt;
    //pcmMgr->chunkBytes = pcmMgr->chunkSize * pcmMgr->bitsPerFrame / 8;

    __inf("--------can_paused:%d-------------",pcmMgr->can_paused);
    __inf(">>periodSize: %4d, BufferSize: %4d, frameBytes: %d",  pcmMgr->periodSize, (int)pcmMgr->bufferSize,pcmMgr->frameBytes);

    return 0;
}

int alsa_open_pcm(audio_config_mgr_t *pcmMgr, const char *card, int pcmFlag)
{
    int err;
    int open_mode = 0;
    snd_pcm_stream_t stream;

    if (pcmMgr->handle != NULL) {
        __wrn("PCM is opened already!");
        return 0;
    }
    __log("open pcm! card:[%s], pcmFlag:[%d](0-cap;1-play)", card, pcmFlag);

    // 0-cap; 1-play
    stream = (pcmFlag == 0) ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK;
    memset(pcmMgr->cardName, 0, sizeof(pcmMgr->cardName));
    strncpy(pcmMgr->cardName, card, sizeof(pcmMgr->cardName));

    //open_mode |= 0x00010000;  // not to used the auto resample

    err = snd_pcm_open(&pcmMgr->handle, card, stream, open_mode);
    if (err < 0) {
        __err("PCM open error: %d", err);
        return err;
    }

	return 0;
}

void alsa_close_pcm(audio_config_mgr_t *pcmMgr)
{
    if (pcmMgr->handle == NULL) {
        __err("PCM is not open yet!");
        return;
    }
    __wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));
    snd_pcm_close(pcmMgr->handle);
    pcmMgr->handle = NULL;
}

void alsa_prepare_pcm(audio_config_mgr_t *pcmMgr)
{
    __msg("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));

    if (pcmMgr->handle == NULL) {
        __err("PCM is not open yet!");
        return;
    }
    snd_pcm_prepare(pcmMgr->handle);
    __wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));
	
}


ssize_t alsa_read_pcm(audio_config_mgr_t *pcmMgr, void *data, size_t rcount)
{
    ssize_t ret;
    ssize_t result = 0;
    int err = 0;
    char cardName[sizeof(pcmMgr->cardName)] = {0};

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }
    //if (rcount != pcmMgr->periodSize)/*do not change the parameter [rcount]*/
    //    rcount = pcmMgr->periodSize;

    if (pcmMgr == NULL || data == NULL) {
        __err("invalid input parameter(pcmMgr=%p, data=%p)!", pcmMgr, data);
        return -1;
    }

    while (rcount > 0) {
        ret = snd_pcm_readi(pcmMgr->handle, data, rcount);
        if (ret == -EAGAIN || (ret >= 0 && (size_t)ret < rcount)) {
            snd_pcm_wait(pcmMgr->handle, 100);
        } else if (ret == -EPIPE) {
            snd_pcm_prepare(pcmMgr->handle);
            //if(pcmMgr->read_pcm_aec)    // for aec condition,need to return directly and re-trigger cap dma again
            //{
            //    __err("aec_rtn_drtly");
            //    return ret;
            //}
        } else if (ret == -ESTRPIPE) {
            __log("need recover(%s)!", strerror(errno));
            snd_pcm_recover(pcmMgr->handle, ret, 0);
        } else if (ret < 0) {
            __err("read error: %d", (ret));
            return ret;
        }

        if (ret > 0) {
            result += ret;
            rcount -= ret;
            data += ret * pcmMgr->frameBytes;
        }
    }

    return result;
}

ssize_t alsa_write_pcm(audio_config_mgr_t *pcmMgr, void *data, size_t wcount)
{
    ssize_t ret;
    ssize_t result = 0;
    int err = 0;
    char cardName[sizeof(pcmMgr->cardName)] = {0};

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    if (snd_pcm_state(pcmMgr->handle) == SND_PCM_STATE_SUSPENDED) {
         while ((err = snd_pcm_resume(pcmMgr->handle)) == -EAGAIN) {
             __err("snd_pcm_resume again!");
             sleep(1);
        }
        switch(snd_pcm_state(pcmMgr->handle))
        {
            case SND_PCM_STATE_XRUN:
            {
                snd_pcm_drop(pcmMgr->handle);
                break;
            }
            case SND_PCM_STATE_SETUP:
                break;
            default:
            {
                __wrn("pcm_lib_state:%s",snd_pcm_state_name(snd_pcm_state(pcmMgr->handle)));
                snd_pcm_prepare(pcmMgr->handle);
                break;
            }
        }
	    alsa_set_pcm_params(pcmMgr);
    }

    while (wcount > 0) {
        if (snd_pcm_state(pcmMgr->handle) == SND_PCM_STATE_SETUP) {
            snd_pcm_prepare(pcmMgr->handle);
        }
        ret = snd_pcm_writei(pcmMgr->handle, data, wcount);
        if (ret == -EAGAIN || (ret >= 0 && (size_t)ret < wcount)) {
            snd_pcm_wait(pcmMgr->handle, 100);
        } else if (ret == -EPIPE) {
            //__err("xrun!");
            snd_pcm_prepare(pcmMgr->handle);
        } else if (ret == -EBADFD) {
            //__err("careful! current pcm state: %d", snd_pcm_state(pcmMgr->handle));
            snd_pcm_prepare(pcmMgr->handle);
        } else if (ret == -ESTRPIPE) {
            __err("need recover!");
            snd_pcm_recover(pcmMgr->handle, ret, 0);
        } else if (ret < 0) {
            __err("write error! ret:%d", ret);
            //0-cap; 1-play
            alsa_close_pcm(pcmMgr);
            //FIXME: reopen
            __err("cardName:[%s], pcmFlag:[play]", pcmMgr->cardName);
            strncpy(cardName, pcmMgr->cardName, sizeof(cardName));
            ret = alsa_open_pcm(pcmMgr, cardName, 1);
            if (ret < 0) {
                __err("alsa_open_pcm failed!");
                return ret;
            }
            ret = alsa_set_pcm_params(pcmMgr);
            if (ret < 0) {
                __err("alsa_set_pcm_params failed!");
                return ret;
            }
            if (pcmMgr->handle != NULL) {
                //snd_pcm_reset(pcmMgr->handle);
                snd_pcm_prepare(pcmMgr->handle);
                snd_pcm_start(pcmMgr->handle);
            }
            __log("set pcm prepare finished!");
            return ret;
        }

        if (ret > 0) {
            result += ret;
            wcount -= ret;
            data += ret * pcmMgr->frameBytes;
        }
    }

    return result;
}

int alsa_mixer_set_volume(audio_config_mgr_t *pcmMgr, int playFlag, int channel, long value)
{
    int err = 0;
    int err_l = 0, err_r = 0;
    long volume_l = 0, volume_r = 0;
    
    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    long realVol = value * AUDIO_VOLUME_MAX / 60;     //volume range is (0-60),so scale it to (0-255)
    if (realVol < 0 || realVol > AUDIO_VOLUME_MAX)
    {
        __err("want to setAIOVol[0,60], playFlag[%d], but usr value=%ld is invalid!", playFlag, value);
        return -1;
    }

    if (playFlag) {
#ifdef CONFIG_SOC_SUN8IW19P1

        err = snd_ctl_set(pcmMgr->cardName, AUDIO_LINEOUT_VOL, realVol);
#elif defined CONFIG_SOC_SUN20IW1P1

        switch (channel)
        {
            case AUDIO_DEV_CHANNEL_STEREO:
            {
                volume_l = realVol;
                volume_r = realVol;
                break;
            }
            case AUDIO_DEV_CHANNEL_LEFT:
            {
                volume_l = realVol;
                volume_r = 0;
                break;
            }
            case AUDIO_DEV_CHANNEL_RIGHT:
            {
                volume_l = 0;
                volume_r = realVol;
                break;
            }
            default:
            {
                __log("Exit volume setting!");
                return EPDK_OK;
            }
            __msg("Volume setting done!");
        }

        err_l = snd_ctl_set(pcmMgr->cardName, AUDIO_DACL_VOL, volume_l);
        err_r = snd_ctl_set(pcmMgr->cardName, AUDIO_DACR_VOL, volume_r);
        err   = ((err_l < err_r)? err_l : err_r);
#endif
    } else {
        err = snd_ctl_set(pcmMgr->cardName, AUDIO_MICIN_VOL, realVol);

    }
    __log("Now card: %s set Capturegain to:%ld, err:%d", pcmMgr->cardName, realVol, err);

    return err;
}

int alsa_mixer_get_volume(audio_config_mgr_t *pcmMgr, int playFlag, long *value)
{
    int err = 0;

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    if (playFlag)
    {
#ifdef CONFIG_SOC_SUN20IW1P1

        long volume_l = 0;
        long volume_r = 0;

        err = snd_ctl_get(pcmMgr->cardName, AUDIO_DACL_VOL, pcmMgr->mixerInfo);
        if (err < 0)
        {
            __err("snd_ctl_get the Playback left Volume failed!");
            return err;
        }
        volume_l = (pcmMgr->mixerInfo->value);
        
        err = snd_ctl_get(pcmMgr->cardName, AUDIO_DACR_VOL, pcmMgr->mixerInfo);
        if (err < 0)
        {
            __err("snd_ctl_get the Playback right Volume failed!");
            return err;
        }
        volume_r = (pcmMgr->mixerInfo->value);
        *value = ((volume_l < volume_r)? volume_r : volume_l);
#else

        err = snd_ctl_get(pcmMgr->cardName, AUDIO_LINEOUT_VOL, pcmMgr->mixerInfo);
        if (err < 0)
        {
            __err("snd_ctl_get the Playback Volume failed!");
            return err;
        }
        *value = (pcmMgr->mixerInfo->value);
#endif

    } 
    else 
    {
        err = snd_ctl_get(pcmMgr->cardName, AUDIO_MICIN_VOL, pcmMgr->mixerInfo);
        if (err < 0)
        {
            __err("snd_ctl_get the Capture Gain failed!\n");
            return err;
        }
        *value = (pcmMgr->mixerInfo->value);
    }
    /* Avoid errors cause by trunc */
    int   n = *value * 60 / AUDIO_VOLUME_MAX;
    float x = *value * (60.0) / AUDIO_VOLUME_MAX;
    if (x > (float)n)
    {
        n = n + 1;
    }
    else
    {
        n = n;
    }

    *value = (long)n;        //volume range(0-60)
    __log("playback getVolume:%ld, dst:%ld, err:%d", pcmMgr->mixerInfo->value, *value, err);

    return err;
}

int audio_cmd_start(audio_config_mgr_t *pcmMgr)
{
    int ret = 0;

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    ret = snd_pcm_start(pcmMgr->handle);
    if (ret < 0) {
		__err("unable to start PCM stream, return %d\n", ret);
		return ret;
	}
	__wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));

    return EPDK_OK;
}

int audio_cmd_stop(audio_config_mgr_t *pcmMgr)
{
    int ret = 0;

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    #if 0 
    ret = snd_pcm_drain(pcmMgr->handle);
	if (ret < 0)
	{
		__err("stop failed!, return %d\n", ret);
		return ret;
	}
	#endif
    ret = snd_pcm_pause(pcmMgr->handle, 1);
    if (ret < 0)
    {
        __err("pause failed!, return %d\n", ret);
        return ret;
    }
    __wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));

    __msg("[audio_cmd_stop]\n");
    return EPDK_OK;
}

int audio_cmd_pause(audio_config_mgr_t *pcmMgr)
{
    int ret = 0;
    __wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));

    if(pcmMgr->can_paused)
    {
        ret = snd_pcm_pause(pcmMgr->handle, 1);
    }
    else
    {
        ret = snd_pcm_drop(pcmMgr->handle); 
    }

    if (ret < 0)
    {
        __err("pause failed!, return %d\n", ret);
        return ret;
    }
    return EPDK_OK;
}

int audio_cmd_drop(audio_config_mgr_t *pcmMgr)
{
    int ret = 0;

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    ret = snd_pcm_drop(pcmMgr->handle); 

    if (ret < 0)
    {
        __err("pause failed!, return %d\n", ret);
        return ret;
    }
    return EPDK_OK;
}

int audio_cmd_drain(audio_config_mgr_t *pcmMgr)
{
    int ret = 0;

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    ret = snd_pcm_drain(pcmMgr->handle);

    if (ret < 0)
    {
        __err("drain failed!, return %d\n", ret);
        return ret;
    }
    return EPDK_OK;
}

int audio_cmd_resume(audio_config_mgr_t *pcmMgr)
{
    int ret = 0;

    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }

    if(pcmMgr->can_paused)
    {
        ret = snd_pcm_pause(pcmMgr->handle, 0);
    }
    else
    {
        ret = snd_pcm_prepare(pcmMgr->handle); 
    }
    __wrn("[]pcm status:%d\n",snd_pcm_state(pcmMgr->handle));

    if (ret < 0)
    {
        __err("resume failed!, return %d\n", ret);
        return ret;
    }
    return EPDK_OK;
}
/* Get the available(writeable) space for playback  byte*/
int audio_get_playback_avail(audio_config_mgr_t *pcmMgr)
{
	snd_pcm_sframes_t size = 0;
    if (pcmMgr->handle == NULL)
    {
        __err("PCM is not open yet!");
        return EPDK_OK;
    }	
	snd_pcm_delay(pcmMgr->handle,&size);
	return snd_pcm_frames_to_bytes(pcmMgr->handle,size);
}

static void init_audio_play_device(rt_device_t dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;

    dev->init       = audio_play_init;
    dev->open       = audio_play_open;
    dev->close      = audio_play_close;
    dev->read       = audio_play_read;
    dev->write      = audio_play_write;
    dev->control    = audio_play_control;
    dev->user_data  = usr_data;

    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        return;
    }
}

static void init_audio_rec_device(rt_device_t dev, void *usr_data, char *dev_name)
{
    rt_err_t ret;

    dev->init       = audio_rec_init;
    dev->open       = audio_rec_open;
    dev->close      = audio_rec_close;
    dev->read       = audio_rec_read;
    dev->write      = audio_rec_write;
    dev->control    = audio_rec_control;
    dev->user_data  = usr_data;

    ret = rt_device_register(dev, dev_name, RT_DEVICE_FLAG_RDWR);
    if (ret != 0)
    {
        __err("fatal error, ret %d, register device to rtsystem failure.\n", ret);
        return;
    }
}

int sunxi_driver_audio_init(void)
{
    int i;
    rt_device_t device0, device1, device2, device3, device4;

    for (i = 0; i < AUDIO_PLAY_USER_MAX; ++i)
    {
        if(i == 0)
        {
            device0 = &audio_play0.base;
            device0->device_id = 0;
            audio_play0.dev_id = 0;
            audio_play0.status = 0;
            init_audio_play_device(device0, &audio_play0, "audio_play0");
        }
        if(i == 1)
        {
            device1 = &audio_play1.base;
            device1->device_id = 1;
            audio_play1.dev_id = 1;
            audio_play1.status = 0;
            init_audio_play_device(device1, &audio_play1, "audio_play1");
        }
    }
    for (i = 0; i < AUDIO_REC_USER_MAX; ++i)
    {
        if(i == 0)
        {
            device2 = &audio_rec0.base;
            device2->device_id = 0;
            audio_rec0.dev_id = 0;
            audio_rec0.status = 0;
            init_audio_rec_device(device2, &audio_rec0, "audio_rec0");
        }
        if(i == 1)
        {
            device3 = &audio_rec1.base;
            device3->device_id = 1;
            audio_rec1.dev_id = 1;
            audio_rec1.status = 0;
            init_audio_rec_device(device3, &audio_rec1, "audio_rec1");
        }
        if(i == 2)
        {
            device4 = &audio_rec2.base;
            device4->device_id = 2;
            audio_rec2.dev_id = 2;
            audio_rec2.status = 0;
            init_audio_rec_device(device4, &audio_rec2, "audio_rec2");
        }
    }    

    return 0;
}

late_initcall(sunxi_driver_audio_init);


