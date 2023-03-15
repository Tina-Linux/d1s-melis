/*
* Copyright (c) 2008-2020 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : ispSimpleCode.h
* Description :
* History :
*   Author  :
*   Date    :
*   Comment :
*
*
*/

#ifndef ISPSIMPLECODE_H
#define ISPSIMPLECODE_H

#include "vencoder.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vencode_command_e {
	VENCODE_CMD_HEAD_PPSSPS = 0x00,
	VENCODE_CMD_STREAM		= 0x01,
} vencode_command;

typedef struct {
	unsigned int src_width;
	unsigned int src_height;
	unsigned int dst_width;
	unsigned int dst_height;
	unsigned int encode_frame_num;
	unsigned int encode_format;
	int frame_rate;
	int maxKeyFrame;

#ifdef INPUTSOURCE_FILE
	unsigned int src_size;
	unsigned int dts_size;
	FILE *in_file;
	FILE *out_file;
	char input_path[256];
	char output_path[256];
	VencAllocateBufferParam bufferParam;
#endif

	VencInputBuffer inputBuffer;
	VencOutputBuffer outputBuffer;
	VencHeaderData sps_pps_data;
	VENC_PIXEL_FMT picture_format;
}encode_param_t;

int OpenEncoder(VENC_CODEC_TYPE type);

int PrepareEncoder(encode_param_t *encode_param);

int StartEncoder(encode_param_t *encode_param, VencInputBuffer *input, VencOutputBuffer *output, vencode_command type);

int FreeEncoderOutputBuffer(VencOutputBuffer *outputBuffer);

int CloseEncoder(encode_param_t *encode_param);
#ifdef __cplusplus
}
#endif

#endif

