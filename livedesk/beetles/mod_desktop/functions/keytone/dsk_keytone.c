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
#include "functions_i.h"
#include <sys_mems.h>
#include <kconfig.h>
#include "dfs_posix.h"
#define USE_KEY_TONE 0

typedef struct __WAVE_HEADER1
{
    __u32       uRiffFcc;       // four character code, "RIFF"
    __u32       uFileLen;       // file total length, don't care it
    __u32       uWaveFcc;       // four character code, "WAVE"
    __u32       uFmtFcc;        // four character code, "fmt "
    __u32       uFmtDataLen;    // Length of the fmt data (=16)
    __u16       uWavEncodeTag;  // WAVE File Encoding Tag
    __u16       uChannels;      // Channels: 1 = mono, 2 = stereo
    __u32       uSampleRate;    // Samples per second: e.g., 44100
    __u32       uBytesPerSec;   // sample rate * block align
    __u16       uBlockAlign;    // channels * bits/sample / 8
    __u16       uBitsPerSample; // 8 or 16
    __u32       uDataFcc;       // four character code "data"
    __u32       uSampDataSize;  // Sample data size(n)
} __wave_header_t1;

/*********************************************************************************************************************/
typedef struct tag_DKTone
{
    __s32  f_audiodev;
    __u8     *tonebuf;
    __set_keytone_t  state;
    __u32    wavsize;
} DKTone;

static __u32 g_wav_sample_size  = 0;
static DKTone *keytone          = NULL;
static __u32 g_key_type         = KEYTONE_KEY_TYPE_DOWN | KEYTONE_KEY_TYPE_REPEAT;
/*********************************************************************************************************************/

/*
****************************************************************************************************
*Name        : dsk_keytone_init
*Prototype   : __s32 dsk_keytone_init( const char *keytone_file )
*Arguments   : keytone_file  input. the keytone file.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : init the keytone module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_init(const char *keytone_file)
{
    ES_FILE                 *pfile2 = NULL;
    __audio_dev_para_t      pbuf2;
    __wave_header_t1        wav;
    unsigned long arg[4] = {0};
    reg_system_para_t       *setting_reg_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

#if (USE_KEY_TONE==0)
    return EPDK_OK;//112350
#endif
    keytone = (DKTone *)g_malloc(sizeof(DKTone));

    if (!keytone)
    {
        __err(" DKTone malloc error ");
        return EPDK_FAIL;
    }

    eLIBs_memset(keytone, 0x00, sizeof(DKTone));

    keytone->f_audiodev = open("/dev/audio_play1", O_RDWR);

    if (keytone->f_audiodev < 0)
    {
        __wrn("keytone->f_audiodev == NULL");
    }

    //  keytone->state = SET_KEYTONE_ON;
    keytone->state  = setting_reg_para->keytone;
    pfile2          = eLIBs_fopen(keytone_file, "rb");

    if (pfile2 == 0)
    {
        __err("%s cannot open ", keytone_file);
        return EPDK_OK;
    }

    eLIBs_memset(&wav, 0x00, sizeof(__wave_header_t1));
    eLIBs_fread(&wav, 1, sizeof(__wave_header_t1), pfile2);

    g_wav_sample_size   = wav.uSampDataSize;
    keytone->wavsize    = wav.uSampDataSize;
    keytone->tonebuf    = (__u8 *)esMEMS_Malloc(0, g_wav_sample_size);

    if (!keytone->tonebuf)
    {
        __err(" tonebuf malloc error ");
        return EPDK_FAIL;
    }

    eLIBs_memset(keytone->tonebuf, 0x00, g_wav_sample_size);
    eLIBs_fread(keytone->tonebuf, 1, g_wav_sample_size, pfile2);
    eLIBs_fclose(pfile2);
    pfile2 = NULL;

    eLIBs_memset(&pbuf2, 0x00, sizeof(__audio_dev_para_t));

    pbuf2.bps   = wav.uBitsPerSample;
    pbuf2.chn   = wav.uChannels;
    pbuf2.fs    = wav.uSampleRate;
    arg[0] = 0;
    arg[1] = (unsigned long)&pbuf2;
    ioctl(keytone->f_audiodev, AUDIO_DEV_CMD_SET_PARA,(void*)arg);
    //esMODS_MIoctrl(keytone->f_audiodev, AUDIO_DEV_CMD_REG_USERMODE, AUDIO_PLAY_USR_KEY, 0);
    ioctl(keytone->f_audiodev, AUDIO_DEV_CMD_START, 0);
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_keytone_exit
*Prototype   : __s32 dsk_keytone_exit(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : exit the keytone module.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_exit(void)
{
#if (USE_KEY_TONE==0)
    return EPDK_OK;//112350
#endif

    if (keytone)
    {
        ioctl(keytone->f_audiodev, AUDIO_DEV_CMD_STOP, 0);
        close(keytone->f_audiodev);
        esMEMS_Mfree(0, (void *)keytone->tonebuf);
        g_free(keytone);
        keytone = NULL;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

static __s32 g_mute_status = 0;

/*
****************************************************************************************************
*Name        : dsk_keytone_on
*Prototype   : __s32 dsk_keytone_on(void)
*Arguments   : void
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : ring the tone.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_on(void)
{
#if (USE_KEY_TONE==0)
    return EPDK_OK;//112350
#endif

    if ((keytone->state == SET_KEYTONE_ON) && (keytone->tonebuf) && (keytone->f_audiodev))
    {
        g_mute_status = dsk_amplifier_is_on();
        dsk_amplifier_onoff(1);//打开功放
        esKRNL_TimeDly(10); //(20);
        //ioctl(keytone->f_audiodev, AUDIO_DEV_CMD_FLUSH_BUF, 0);
        //eLIBs_fwrite(keytone->tonebuf, 1, keytone->wavsize, keytone->f_audiodev);
        write(keytone->f_audiodev,keytone->tonebuf,keytone->wavsize);
        ioctl(keytone->f_audiodev, AUDIO_DEV_CMD_DRAIN, 0);
        esKRNL_TimeDly(15); //(50);

        if (1 == g_mute_status)
        {
            dsk_amplifier_onoff(1);//打开功放
        }
        else
        {
            dsk_amplifier_onoff(0);//关闭功放
        }
    }

    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_keytone_set_state
*Prototype   : dsk_keytone_set_state(__set_keytone_t state)
*Arguments   : state       input. gate status, on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : set the keytone gate status. on or off.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_set_state(__set_keytone_t state)
{
#if (USE_KEY_TONE==0)
    return EPDK_OK;//112350
#endif
    keytone->state = state;
    return EPDK_OK;
}



/*
****************************************************************************************************
*Name        : dsk_keytone_get_state
*Prototype   : __s32 dsk_keytone_get_state(__set_keytone_t *p_state)
*Arguments   : p_state     output. the keytone gate status. on or off.
*Return      : EPDK_OK     succeed
*              EPDK_FAIL   fail
*Description : get the keytone gate status. on or off.
*Other       :
****************************************************************************************************
*/
__s32 dsk_keytone_get_state(__set_keytone_t *p_state)
{
#if (USE_KEY_TONE==0)
    return EPDK_OK;//112350
#endif
    *p_state = keytone->state;
    return EPDK_OK;
}

void dsk_keytone_set_key_type(__u32 key_type)
{
#if (USE_KEY_TONE==0)
    return ;//112350
#endif
    g_key_type = key_type;
}

__u32 dsk_keytone_get_key_type(void)
{
#if (USE_KEY_TONE==0)
    return EPDK_OK;//112350
#endif
    return g_key_type;
}
