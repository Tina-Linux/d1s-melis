#ifndef _DRV_AUDIO_H_
#define _DRV_AUDIO_H_

#include "pcm.h"
#include "control.h"
#include "audio_play.h"
#include "audio_rec.h"
#include <typedef.h>
#include <kapi.h>

#define SOUND_CARD_AUDIOCODEC   "audiocodec"

// for v459
// for play
#define AUDIO_LINEOUT_VOL	"LINEOUT volume"            // set play volume

// for cap
#define AUDIO_MICIN_VOL		"MIC1 gain volume"          // set record volume
#define AUDIO_LINEININ_VOL  "LINEIN gain volume"        // set record volume

// for F133
// for play
#define AUDIO_DACL_VOL      "DACL digital volume"       // set play volume
#define AUDIO_DACR_VOL      "DACR digital volume"       // set play volume
#define AUDIO_HPOUT_VOL     "HPOUT gain volume"         // set play volume

#define ADC1_SWITCH_ONOFF     "MIC1 input switch"         // PGA switch
#define ADC2_SWITCH_ONOFF     "MIC2 input switch"         // PGA switch
#define ADC3_SWITCH_ONOFF     "MIC3 input switch"         // PGA switch




#ifdef CONFIG_SOC_SUN8IW19P1

#define AUDIO_VOLUME_MIN    (0)
#define AUDIO_VOLUME_MAX    (31)

#elif defined CONFIG_SOC_SUN20IW1P1

#define AUDIO_VOLUME_MIN    (0)
#define AUDIO_VOLUME_MAX    (255)

#endif

#define DISABLE_OS_TASK_SWITCH()    esKRNL_SchedLock()
#define ENABLE_OS_TASK_SWITCH()     esKRNL_SchedUnlock()

typedef struct AUDIO_CONFIG_MGR{
	snd_pcm_t *handle;
	char cardName[16];

	snd_pcm_format_t format;
	unsigned int sampleRate;
	unsigned int channels;
	snd_pcm_uframes_t periodSize;
	snd_pcm_uframes_t bufferSize;

	unsigned long frameBytes;
	unsigned long chunkBytes;

	unsigned in_aborting;
	unsigned int capture_duration;
	unsigned int can_paused;
	snd_ctl_info_t *mixerInfo;
} audio_config_mgr_t;

typedef struct SUNXI_DRIVER_AUDIO
{
    struct rt_device   base;
    int32_t            dev_id;
    int32_t            status;
    audio_config_mgr_t *mgr;
    union {
		audio_play_dev_params_t aplay_para;
		audio_rec_dev_params_t  arec_para;
	} dev_info;
} sunxi_driver_audio_t;

typedef enum AUDIO_USR_MSG_TYPE
{
    AUDIO_USR_MSG_NONE = 0,     //none message

    AUDIO_USR_MSG_START,        //user start audio device
    AUDIO_USR_MSG_STOP,         //user stop audio device
    AUDIO_USR_MSG_PAUS,         //user pause audio device
    AUDIO_USR_MSG_CONTI,        //user continue device

    AUDIO_USR_MSG_FLUSH_BUF,    //user flush audio data in buffer
    AUDIO_USR_MSG_SWITCH_IF,        //switch audio hardware interface
    AUDIO_USR_MSG_SWITCH_IF_DONE,   //switch audio hardware interface done
    AUDIO_USR_MSG_ADJUST_VOLUME,    //adjust audio device volume
    AUDIO_USR_MSG_RESTORE_FS,       //restore sample rate

    AUDIO_USR_MSG_SET_CHAN_STEREO,  //set audio channel mode to stereo
    AUDIO_USR_MSG_SET_CHAN_LEFT,    //set audio channel mode to left
    AUDIO_USR_MSG_SET_CHAN_RIGHT,   //set audio channel mode to right
    AUDIO_USR_MSG_GET_CHAN_MODE,    //get current audio channel mode
    AUDIO_USR_MSG_
} audio_usr_msg_type_t;

typedef enum AUDIO_DEV_STAT
{
    AUDIO_DEV_STAT_IDLE = 0,
    AUDIO_DEV_STAT_RUN,
    AUDIO_DEV_STAT_PAUS,
    AUDIO_DEV_STAT_
} audio_dev_stat_t;

extern audio_config_mgr_t *audio_playback_config_mgr_create(void);
extern audio_config_mgr_t *audio_record_config_mgr_create(void);
extern void audio_config_mgr_release(audio_config_mgr_t *mgr);
extern int alsa_set_pcm_params(audio_config_mgr_t *pcmMgr);
extern int alsa_open_pcm(audio_config_mgr_t *pcmMgr, const char *card, int pcmFlag);
extern void alsa_close_pcm(audio_config_mgr_t *pcmMgr);
extern void alsa_prepare_pcm(audio_config_mgr_t *pcmMgr);
extern ssize_t alsa_read_pcm(audio_config_mgr_t *pcmMgr, void *data, size_t rcount);
extern ssize_t alsa_write_pcm(audio_config_mgr_t *pcmMgr, void *data, size_t wcount);
extern int alsa_mixer_set_volume(audio_config_mgr_t *pcmMgr, int playFlag, int channel, long value);
extern int alsa_mixer_get_volume(audio_config_mgr_t *pcmMgr, int playFlag, long *value);
extern int audio_cmd_start(audio_config_mgr_t *pcmMgr);
extern int audio_cmd_stop(audio_config_mgr_t *pcmMgr);
extern int audio_cmd_pause(audio_config_mgr_t *pcmMgr);
extern int audio_cmd_resume(audio_config_mgr_t *pcmMgr);

extern int audio_cmd_drop(audio_config_mgr_t *pcmMgr);
extern int audio_cmd_drain(audio_config_mgr_t *pcmMgr);
extern int audio_get_playback_avail(audio_config_mgr_t *pcmMgr);

#endif

