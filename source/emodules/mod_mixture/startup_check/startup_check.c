/*
 * =====================================================================================
 *
 *       Filename:  startup_check.c
 *
 *       Description:  check the power of bat when startup , if bat too low ,
 *                      stop go on and play warning voice
 *
 *       Version:  1.0
 *       Create: 2021-4-23 11:41:34
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *       Author:  liujiaming@allwinnertech.com
 *       Organization:  PDC-PD1
 *       Last Modified:  2021-4-23 11:41:20
 *
 * =====================================================================================
 */
#include <dfs_posix.h>
#include <rtthread.h>
#include "startup_check.h"
#include <mod_power.h>

#define AMP_ON (0)
#define AMP_OFF (1)
#define AMP_VOL (39)
#define POWER_LOW       0
#define POWER_HIGHT     1
#define CHARG_IN        1
#define CHARG_OUT       0
#define POWER_STA_NULL  (-1)
#define LOW_POWER_VOICE_PATH "d:\\res\\sounds\\low_power_audio.wav"
#define DEV_AUDIO "/dev/audio_play0"

static char *load_voice_file(const char *path, __u32 *len);
static void voice_file_play(char *buf, __u32 len);
static void amp_output_ctl(bool onoff);
static void play_lowpower_wrn_voice(const char *path);
static __s32 get_current_powerinfo(void);


static char *load_voice_file(const char *path, __u32 *len)
{
    __u32 buf_len = 0;
    char *buf = NULL;
    ES_FILE *file_handle = NULL;

    if (NULL == path)
    {
        __wrn("NULL == path");
        return NULL;
    }

    file_handle = eLIBs_fopen(path, "rb");
    if (NULL == file_handle)
    {
        __wrn("NULL == file_handle");
        return NULL;
    }

    eLIBs_fseek(file_handle, 0, SEEK_END);
    buf_len = (__u32)eLIBs_ftell(file_handle);
    *len = buf_len;
    buf = (char *)eLIBs_malloc(buf_len);
    if (NULL == buf)
    {
        __wrn("malloc fail  buf_len = %d",buf_len);
        return NULL;
    }
    eLIBs_fseek(file_handle, 0, SEEK_SET);
    eLIBs_fread(buf, 1, buf_len, file_handle);
    eLIBs_fclose(file_handle);
    return buf;

}

static void voice_file_play(char *buf, __u32 len)
{
    int audio_dev = -1;
    const __wave_header_t *header = (__wave_header_t *)buf;
    unsigned long arg[4] = {0};
    __audio_dev_para_t audio_dev_para;
    char *data_buf = NULL;
    __u32 data_len = 0;
    __hdle amp_hdle = NULL;

    if(sizeof(__wave_header_t) >= len)
    {
        __wrn("parameter error len = %d",len);
        return;
    }

    data_buf = buf + sizeof(__wave_header_t);
    data_len = header->uSampDataSize;
    if ((data_len + sizeof(__wave_header_t)) > len)
    {
        __wrn("data len need reset");
        data_len = len - sizeof(__wave_header_t);
    }
    eLIBs_memset(&audio_dev_para, 0, sizeof(__audio_dev_para_t));
    audio_dev_para.bps = header->uBitsPerSample;
    audio_dev_para.chn = header->uChannels;
    audio_dev_para.fs = header->uSampleRate;

    audio_dev = open(DEV_AUDIO, O_RDWR);
    if (0 > audio_dev)
    {
        __wrn("0 > audio_dev");
        return;
    }

    arg[0] = AMP_VOL;
    arg[2] = 0;
    ioctl(audio_dev, AUDIO_DEV_CMD_SET_VOLUME, arg);

    amp_output_ctl(AMP_ON);

    arg[0] = 0;
    arg[1] = (unsigned long)&audio_dev_para;
    ioctl(audio_dev, AUDIO_DEV_CMD_SET_PARA, arg);


    arg[0] = AUDIO_PLAY_USR_KEY;
    arg[1] = 0;
    ioctl(audio_dev, AUDIO_DEV_CMD_REG_USERMODE, arg);

    arg[0] = 0;
    arg[1] = 0;
    ioctl(audio_dev, AUDIO_DEV_CMD_START, arg);

    write(audio_dev, data_buf, data_len);

    arg[0] = 0;
    arg[1] = 0;
    ioctl(audio_dev, AUDIO_DEV_CMD_DRAIN, arg);
    esKRNL_TimeDly(15);

    amp_output_ctl(AMP_OFF);
    if (0 <= audio_dev)
    {
        close(audio_dev);
        audio_dev = -1;
    }  
}

static void amp_output_ctl(bool onoff)
{
    __hdle hdle = NULL;
    user_gpio_set_t gpio_set[1] = {0};

    if (EPDK_OK == esCFG_GetKeyValue("audio_para", "PA_SHDN", (__s32 *)gpio_set, sizeof(user_gpio_set_t) / 4))
    {
        hdle = esPINS_PinGrpReq(gpio_set, 1);
        if(!hdle)
        {
            __wrn("get amp io fail");
            return;
        }
    }
    esPINS_WritePinData(hdle, onoff, "PA_SHDN");
}

void check_if_lowpower(void)
{
    __s32 power_sta;
    power_sta = get_current_powerinfo();
    if(power_sta == (CHARG_OUT & POWER_LOW))
    {
        __wrn("power too low , please connect the charger!");
        play_lowpower_wrn_voice(LOW_POWER_VOICE_PATH);
        while(1)
        {
            esKRNL_TimeDly(1000);
        }
    }
    else
    {
        __wrn("startup power check ok");
    }
}

static void play_lowpower_wrn_voice(const char *path)
{
    __u32 buf_len = 0;
    __s32 ret = EPDK_FAIL;
    char *buf = NULL;

    buf = load_voice_file(path, &buf_len);
    if(buf == NULL)
    {
        __wrn("load voice file fail");
        return;
    }
    voice_file_play(buf,buf_len);
    eLIBs_free(buf);
}

static __s32 get_current_powerinfo(void)
{
    __drv_power_fuelguage_t         info;
    __s32 ret;
    int power_fp = -1;
    info.rest_vol = -1;
    info.bat_power = -1;
    info.charge_status = -1;
    power_fp = open(POWERDEV, O_RDONLY);
    if(power_fp < 0)
    {
        __wrn("power dev open fail");
        return -1;
    }

    while(info.rest_vol == POWER_STA_NULL || info.charge_status == POWER_STA_NULL)
    {
        ioctl(power_fp, DRV_POWER_CMD_GET_FUELGUAGE,&info);
        esKRNL_TimeDly(1);
    }
    close(power_fp);
    power_fp = -1;

    if(info.charge_status == CHARG_IN)//if startup with DCIN ,don't care about the power level.
    {
        __wrn("dcin ok");
        return CHARG_IN;
    }

    info.bat_power -= 50;//adjust the lcd backlight cause the Vbat shake.
    __log("power info vol %d  dcin %d  bat_power = %dmv",info.rest_vol,info.charge_status,info.bat_power);

    if(info.bat_power > 3450)
    {
        return POWER_HIGHT;
    }
    else
    {
        return POWER_LOW;
    }
}
