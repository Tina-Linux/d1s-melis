/*
************************************************************************************************************************
*                                            audio output format in register table
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : reg_audio_output.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.08.26
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2010.08.26        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __REG_AUDIO_OUTPUT_H__
#define  __REG_AUDIO_OUTPUT_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef enum tag_LION_AUDIO_OUTPUT
{
    LION_AUDIO_OUTPUT_NORMAL        = 0,
    LION_AUDIO_OUTPUT_SPDIF_LPCM,
    LION_AUDIO_OUTPUT_SPDIF_RAWDATA,

    LION_AUDIO_OUTPUT_UNKNOWN       = -1
}
__lion_audio_output_type_e;


#ifdef __cplusplus
}
#endif

#endif     //  ifndef __REG_AUDIO_OUTPUT_H__

/* end of reg_audio_output.h */
