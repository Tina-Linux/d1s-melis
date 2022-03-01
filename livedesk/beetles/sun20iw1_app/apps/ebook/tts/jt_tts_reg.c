/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : monkey_reg.c
* By        : Andy.zhang
* Func      : monkey core interface
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "app_ebook_i.h"
#include "jt_tts.h"
#include "jt_tts_reg.h"

#define MK_REG_NODE     "\\ebook\\ebook"


/*TTS –ßπ˚…Ë÷√*/
__u8  tts_get_volume(__u32  h_tts, __u8 level_count)
{
    long tts_volume_value;
    __u8 volume_level;
    jtTTS_GetParam(h_tts, jtTTS_PARAM_VOLUME,
                   &tts_volume_value);
    volume_level = (tts_volume_value - jtTTS_VOLUME_MIN) * level_count / (jtTTS_VOLUME_MAX - jtTTS_VOLUME_MIN);
    return volume_level;
}
__u8  tts_get_pitch(__u32 h_tts, __u8 level_count)
{
    long tts_pitch_value;
    __u8 pitch_level;
    jtTTS_GetParam(h_tts, jtTTS_PARAM_PITCH,
                   &tts_pitch_value);
    pitch_level = (tts_pitch_value - jtTTS_PITCH_MIN) * level_count / (jtTTS_PITCH_MAX - jtTTS_PITCH_MIN);
    return pitch_level;
}
__u8  tts_get_speed(__u32 h_tts, __u8 level_count)
{
    long tts_speed_value;
    __u8 speed_level;
    jtTTS_GetParam(h_tts, jtTTS_PARAM_SPEED,
                   &tts_speed_value);
    speed_level = (tts_speed_value - jtTTS_SPEED_MIN) * level_count / (jtTTS_SPEED_MAX - jtTTS_SPEED_MIN);
    return speed_level;
}
__u8 tts_get_speak_style(__u32 h_tts, __u8 level_count)
{
    long tts_speak_style_value;
    __u8 speak_style_level;
    jtTTS_GetParam(h_tts, jtTTS_PARAM_SPEAK_STYLE,
                   &tts_speak_style_value);
    speak_style_level =  tts_speak_style_value + 1;
    return (speak_style_level);
}


jtErrCode  tts_set_volume(__u32 h_tts, __u8 volume_level, __u8 level_count)
{
    jtErrCode           dwError;            // ¥ÌŒÛ¬Î
    long                      tts_volume_value;
    tts_volume_value =  jtTTS_VOLUME_MIN + (jtTTS_VOLUME_MAX - jtTTS_VOLUME_MIN) / level_count * volume_level ;
    dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_VOLUME,
                             tts_volume_value);
    return  dwError;
}
jtErrCode  tts_set_speed(__u32 h_tts, __u8 speed_level, __u8 level_count)
{
    jtErrCode           dwError;            // ¥ÌŒÛ¬Î
    long                      tts_speed_value;

    switch (speed_level)
    {
        case 0:
            tts_speed_value = jtTTS_VOLUME_MIN;
            break;

        case 1:
            tts_speed_value = jtTTS_VOLUME_MIN / 2;
            break;

        case 2:
            tts_speed_value = 0;
            break;

        case 3:
            tts_speed_value = jtTTS_SPEED_MAX / 2;
            break;

        case 4:
            tts_speed_value = jtTTS_SPEED_MAX;
            break;

        default:
            tts_speed_value = 0;
            break;
    }

    dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_SPEED,
                             tts_speed_value);
    return  dwError;
}
jtErrCode  tts_set_pitch(__u32 h_tts, __u8 pitch_level, __u8 level_count)
{
    jtErrCode           dwError;            // ¥ÌŒÛ¬Î
    long                      tts_pitch_value;
    tts_pitch_value =  jtTTS_VOLUME_MIN + (jtTTS_VOLUME_MAX - jtTTS_VOLUME_MIN) / level_count * pitch_level;
    dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_PITCH,
                             tts_pitch_value);
    return  dwError;
}
jtErrCode  tts_set_speak_style(__u32 h_tts, __u8 speak_style_level)
{
    jtErrCode           dwError;            // ¥ÌŒÛ¬Î

    switch (speak_style_level)
    {
        case 0:
            dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_SPEAK_STYLE,
                                     jtTTS_SPEAK_STYLE_CLEAR);
            break;

        case 1:
            dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_SPEAK_STYLE,
                                     jtTTS_SPEAK_STYLE_NORMAL);
            break;

        case 2:
            dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_SPEAK_STYLE,
                                     jtTTS_SPEAK_STYLE_PLAIN);
            break;

        case 3:
            dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_SPEAK_STYLE,
                                     jtTTS_SPEAK_STYLE_VIVID);
            break;

        default:
            dwError = jtTTS_SetParam(h_tts, jtTTS_PARAM_SPEAK_STYLE,
                                     jtTTS_SPEAK_STYLE_NORMAL);
            break;
    }

    return  dwError;
}


__s32 reg_tts_speaker_set(__s32 type)
{
    __hdle  h_node;
    char *type_str[] = {"0", "1", "2", "3", "4", "5"};
    return EPDK_OK;
}

__s32 reg_tts_speaker_get(void)
{
    __hdle         h_node;
    __s32          ret = 3;
    return ret;
}

__u32  tts_get_voice_speaker(void)
{
    __u32  tts_speaker_index;
    tts_speaker_index =  reg_tts_speaker_get();
    __msg("tts_speaker_index=%d\n", tts_speaker_index);
    return  tts_speaker_index;
}
__u32  tts_set_voice_speaker(__s32 voice_speaker)
{
    __u32  ret;
    __msg("voice_speaker=%d\n", voice_speaker);
    ret = reg_tts_speaker_set(voice_speaker);
    tts_speaker_deint();
    return  ret;
}
/*TTS –ßπ˚…Ë÷√*/












