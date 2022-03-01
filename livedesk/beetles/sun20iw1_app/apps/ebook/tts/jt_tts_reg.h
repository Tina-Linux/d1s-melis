/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : monkey_reg.h
* By        : Andy.zhang
* Func      : monkey core interface
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#ifndef __JT_TTS_REG_H__
#define __JT_TTS_REG_H__

#include <emodules/mod_defs.h>
#include "eJTTS.h"


__u8  tts_get_volume(__u32  h_tts, __u8 level_count);
__u8  tts_get_pitch(__u32 h_tts, __u8 level_count);
__u8  tts_get_speed(__u32 h_tts, __u8 level_count);
__u8 tts_get_speak_style(__u32 h_tts, __u8 level_count);
jtErrCode  tts_set_volume(__u32 h_tts, __u8 volume_level, __u8 level_count);
jtErrCode  tts_set_speed(__u32 h_tts, __u8 speed_level, __u8 level_count);
jtErrCode  tts_set_pitch(__u32 h_tts, __u8 pitch_level, __u8 level_count);
jtErrCode  tts_set_speak_style(__u32 h_tts, __u8 speak_style_level);
__u32  tts_get_voice_speaker(void);
__u32  tts_set_voice_speaker(__s32 voice_speaker);


#endif /* jt_tts_reg.h */

