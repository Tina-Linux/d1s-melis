/*
* Copyright (c) 2008-2020 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : ispSimpleCode.c
* Description :
* History :
*   Author  :
*   Date    :
*   Comment :
*
*
*/
#include "include/ispSimpleCode.h"
//#include <dlfcn.h>
#include <sys/time.h>
#include "../isp_version.h"
/****************************************************************************************************************
 * structure
 ***************************************************************************************************************/
typedef struct {
	VencHeaderData			sps_pps_data;
	VencH264Param			h264Param;
	VencMBModeCtrl			h264MBMode;
	VencMBInfo				MBInfo;
	VencFixQP				fixQP;
	VencSuperFrameConfig	sSuperFrameCfg;
	VencH264SVCSkip			SVCSkip; // set SVC and skip_frame
	VencH264AspectRatio		sAspectRatio;
	VencH264VideoSignal		sVideoSignal;
	VencCyclicIntraRefresh	sIntraRefresh;
	VencROIConfig			sRoiConfig[4];
	VeProcSet				sVeProcInfo;
	VencOverlayInfoS		sOverlayInfo;
	VencSmartFun			sH264Smart;
} h264_func_t;

/****************************************************************************************************************
 * define
 ***************************************************************************************************************/
#define logt(fmt, arg...)
#define loge(fmt, arg...)	printf("%s: " fmt " lineNum-%d\n", __FUNCTION__, ##arg, __LINE__);
#define VIDEOCODEC_OK		(0)
#define VIDEOCODEC_FAIL		(-1)
#define ALIGN_XXB(y, x)		(((x) + ((y)-1)) & ~((y)-1))
#define VERSION_STRING		"20201230-V1.0"
#define VBV_TOTAL_SIZE		(12 * 1024 * 1024)

#if (ISP_VERSION == 521)
	#define VENC_STATIC_LIBRARY
#elif (ISP_VERSION == 522)
	#define VENC_DYNAMIC_LIBRARY
#endif
/****************************************************************************************************************
 * vencoder library function define
 ***************************************************************************************************************/
typedef VideoEncoder* (* VideoEncCreateFunc)(VENC_CODEC_TYPE eCodecType);
typedef void (* VideoEncDestroyFunc)(VideoEncoder* pEncoder);
typedef int (* VideoEncInitFunc)(VideoEncoder* pEncoder, VencBaseConfig* pConfig);
typedef int (* AllocInputBufferFunc)(VideoEncoder* pEncoder, VencAllocateBufferParam *pBufferParam);
typedef int (* GetOneAllocInputBufferFunc)(VideoEncoder* pEncoder, VencInputBuffer* pInputbuffer);
typedef int (* FlushCacheAllocInputBufferFunc)(VideoEncoder* pEncoder,	VencInputBuffer *pInputbuffer);
typedef int (* ReturnOneAllocInputBufferFunc)(VideoEncoder* pEncoder,  VencInputBuffer *pInputbuffer);
typedef int (* AddOneInputBufferFunc)(VideoEncoder* pEncoder, VencInputBuffer* pInputbuffer);
typedef int (* VideoEncodeOneFrameFunc)(VideoEncoder* pEncoder);
typedef int (* AlreadyUsedInputBufferFunc)(VideoEncoder* pEncoder, VencInputBuffer* pBuffer);
typedef int (* GetOneBitstreamFrameFunc)(VideoEncoder* pEncoder, VencOutputBuffer* pBuffer);
typedef int (* FreeOneBitStreamFrameFunc)(VideoEncoder* pEncoder, VencOutputBuffer* pBuffer);
typedef int (* VideoEncGetParameterFunc)(VideoEncoder* pEncoder, VENC_INDEXTYPE indexType, void* paramData);
typedef int (* VideoEncSetParameterFunc)(VideoEncoder* pEncoder, VENC_INDEXTYPE indexType, void* paramData);
typedef int (* VideoEncoderSetFreqFunc)(VideoEncoder* pEncoder, int nVeFreq);

static VideoEncCreateFunc VideoEncCreateDl = NULL;
static VideoEncDestroyFunc VideoEncDestroyDl = NULL;
static VideoEncInitFunc VideoEncInitDl = NULL;
static AllocInputBufferFunc AllocInputBufferDl = NULL;
static GetOneAllocInputBufferFunc GetOneAllocInputBufferDl = NULL;
static FlushCacheAllocInputBufferFunc FlushCacheAllocInputBufferDl = NULL;
static ReturnOneAllocInputBufferFunc ReturnOneAllocInputBufferDl = NULL;
static AddOneInputBufferFunc AddOneInputBufferDl = NULL;
static VideoEncodeOneFrameFunc VideoEncodeOneFrameDl = NULL;
static AlreadyUsedInputBufferFunc AlreadyUsedInputBufferDl = NULL;
static GetOneBitstreamFrameFunc GetOneBitstreamFrameDl = NULL;
static FreeOneBitStreamFrameFunc FreeOneBitStreamFrameDl = NULL;
static VideoEncGetParameterFunc VideoEncGetParameterDl = NULL;
static VideoEncSetParameterFunc VideoEncSetParameterDl = NULL;
static VideoEncoderSetFreqFunc VideoEncoderSetFreqDl = NULL;

/****************************************************************************************************************
 * global variable
 ***************************************************************************************************************/
static void *vencoderLib = NULL;
static h264_func_t h264_func;
// Create in openEnc() and destory in closeEnc()
static VideoEncoder* pVideoEnc = NULL;

static long long GetNowUs(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000000ll + tv.tv_usec;
}

static void init_mb_mode(VencMBModeCtrl *pMBMode, encode_param_t *encode_param)
{
	unsigned int mb_num;

	mb_num = (ALIGN_XXB(16, encode_param->dst_width) >> 4) * (ALIGN_XXB(16, encode_param->dst_height) >> 4);
	pMBMode->p_info = malloc(sizeof(VencMBModeCtrlInfo) * mb_num);
	pMBMode->mode_ctrl_en = 1;
}

static void init_mb_info(VencMBInfo *MBInfo, encode_param_t *encode_param)
{
	if(encode_param->encode_format == VENC_CODEC_H265)
	{
		MBInfo->num_mb = (ALIGN_XXB(32, encode_param->dst_width) *
		ALIGN_XXB(32, encode_param->dst_height)) >> 10;
	}
	else
	{
		MBInfo->num_mb = (ALIGN_XXB(16, encode_param->dst_width) *
		ALIGN_XXB(16, encode_param->dst_height)) >> 8;
	}
	MBInfo->p_para = (VencMBInfoPara *)malloc(sizeof(VencMBInfoPara) * MBInfo->num_mb);
	if(MBInfo->p_para == NULL)
	{
		loge("malloc MBInfo->p_para error");
		return;
	}
}

static void init_fix_qp(VencFixQP *fixQP)
{
	fixQP->bEnable = 1;
	fixQP->nIQp = 35;
	fixQP->nPQp = 35;
}

static void init_super_frame_cfg(VencSuperFrameConfig *sSuperFrameCfg)
{
	sSuperFrameCfg->eSuperFrameMode = VENC_SUPERFRAME_NONE;
	sSuperFrameCfg->nMaxIFrameBits = 30000*8;
	sSuperFrameCfg->nMaxPFrameBits = 15000*8;
}

static void init_svc_skip(VencH264SVCSkip *SVCSkip)
{
	SVCSkip->nTemporalSVC = T_LAYER_4;
	switch(SVCSkip->nTemporalSVC)
	{
	case T_LAYER_4:
		SVCSkip->nSkipFrame = SKIP_8;
		break;
	case T_LAYER_3:
		SVCSkip->nSkipFrame = SKIP_4;
		break;
	case T_LAYER_2:
		SVCSkip->nSkipFrame = SKIP_2;
		break;
	default:
		SVCSkip->nSkipFrame = NO_SKIP;
		break;
	}
}

static void init_aspect_ratio(VencH264AspectRatio *sAspectRatio)
{
	sAspectRatio->aspect_ratio_idc = 255;
	sAspectRatio->sar_width = 4;
	sAspectRatio->sar_height = 3;
}

static void init_video_signal(VencH264VideoSignal *sVideoSignal)
{
	sVideoSignal->video_format = 5;
	sVideoSignal->src_colour_primaries = 0;
	sVideoSignal->dst_colour_primaries = 1;
}

static void init_intra_refresh(VencCyclicIntraRefresh *sIntraRefresh)
{
	sIntraRefresh->bEnable = 1;
	sIntraRefresh->nBlockNumber = 10;
}

static void init_roi(VencROIConfig *sRoiConfig)
{
	sRoiConfig[0].bEnable = 1;
	sRoiConfig[0].index = 0;
	sRoiConfig[0].nQPoffset = 10;
	sRoiConfig[0].sRect.nLeft = 0;
	sRoiConfig[0].sRect.nTop = 0;
	sRoiConfig[0].sRect.nWidth = 1280;
	sRoiConfig[0].sRect.nHeight = 320;

	sRoiConfig[1].bEnable = 1;
	sRoiConfig[1].index = 1;
	sRoiConfig[1].nQPoffset = 10;
	sRoiConfig[1].sRect.nLeft = 320;
	sRoiConfig[1].sRect.nTop = 180;
	sRoiConfig[1].sRect.nWidth = 320;
	sRoiConfig[1].sRect.nHeight = 180;

	sRoiConfig[2].bEnable = 1;
	sRoiConfig[2].index = 2;
	sRoiConfig[2].nQPoffset = 10;
	sRoiConfig[2].sRect.nLeft = 320;
	sRoiConfig[2].sRect.nTop = 180;
	sRoiConfig[2].sRect.nWidth = 320;
	sRoiConfig[2].sRect.nHeight = 180;

	sRoiConfig[3].bEnable = 1;
	sRoiConfig[3].index = 3;
	sRoiConfig[3].nQPoffset = 10;
	sRoiConfig[3].sRect.nLeft = 320;
	sRoiConfig[3].sRect.nTop = 180;
	sRoiConfig[3].sRect.nWidth = 320;
	sRoiConfig[3].sRect.nHeight = 180;
}

static void init_enc_proc_info(VeProcSet *ve_proc_set)
{
	ve_proc_set->bProcEnable = 1;
	ve_proc_set->nProcFreq = 60;
}

static int initH264Func(h264_func_t *h264_func, encode_param_t *encode_param)
{
	int bit_rate;
	memset(h264_func, 0, sizeof(h264_func_t));

	//Adaptive bit rate according to dts_width
	if(encode_param->dst_width == 1600)
		bit_rate = 6 * 1024 * 1024;
	else
		bit_rate = 2 * 1024 * 1024;

	h264_func->h264Param.bEntropyCodingCABAC = 1;
	h264_func->h264Param.nBitrate = bit_rate;
	h264_func->h264Param.nFramerate = encode_param->frame_rate;
	h264_func->h264Param.nCodingMode = VENC_FRAME_CODING;
	h264_func->h264Param.nMaxKeyInterval = encode_param->maxKeyFrame;
	h264_func->h264Param.sProfileLevel.nProfile = VENC_H264ProfileHigh;
	h264_func->h264Param.sProfileLevel.nLevel = VENC_H264Level51;
	h264_func->h264Param.sQPRange.nMinqp = 10;
	h264_func->h264Param.sQPRange.nMaxqp = 50;

	h264_func->sH264Smart.img_bin_en = 1;
	h264_func->sH264Smart.img_bin_th = 27;
	h264_func->sH264Smart.shift_bits = 2;
	h264_func->sH264Smart.smart_fun_en = 1;

	//以下函数都是在填充h264_func里面的结构体成员
	//init VencMBModeCtrl
	init_mb_mode(&h264_func->h264MBMode, encode_param);

	//init VencMBInfo
	init_mb_info(&h264_func->MBInfo, encode_param);

	//init VencH264FixQP
	init_fix_qp(&h264_func->fixQP);

	//init VencSuperFrameConfig
	init_super_frame_cfg(&h264_func->sSuperFrameCfg);

	//init VencH264SVCSkip
	init_svc_skip(&h264_func->SVCSkip);

	//init VencH264AspectRatio
	init_aspect_ratio(&h264_func->sAspectRatio);

	//init VencH264AspectRatio
	init_video_signal(&h264_func->sVideoSignal);

	//init CyclicIntraRefresh
	init_intra_refresh(&h264_func->sIntraRefresh);

	//init VencROIConfig
	init_roi(h264_func->sRoiConfig);

	//init proc info
	init_enc_proc_info(&h264_func->sVeProcInfo);

	return VIDEOCODEC_OK;
}

static int setEncParam(VideoEncoder *pVideoEnc ,encode_param_t *encode_param)
{
	int result = 0;
	VeProcSet mProcSet;
	mProcSet.bProcEnable = 1;
	mProcSet.nProcFreq = 30;
	mProcSet.nStatisBitRateTime = 1000;
	mProcSet.nStatisFrRateTime  = 1000;
	unsigned int vbv_size;

	VideoEncSetParameterDl(pVideoEnc, VENC_IndexParamProcSet, &mProcSet);

	if(encode_param->encode_format == VENC_CODEC_JPEG)
	{
		loge("VENC CODES JPEG is not supported yet");
		return VIDEOCODEC_FAIL;
	}
	else if(encode_param->encode_format == VENC_CODEC_H264)
	{
		result = initH264Func(&h264_func, encode_param);
		if(result)
		{
			loge("initH264Func error, return \n");
			return VIDEOCODEC_FAIL;
		}

		vbv_size = VBV_TOTAL_SIZE;
		VideoEncSetParameterDl(pVideoEnc, VENC_IndexParamH264Param, &h264_func.h264Param);
		VideoEncSetParameterDl(pVideoEnc, VENC_IndexParamSetVbvSize, &vbv_size);
		VideoEncSetParameterDl(pVideoEnc, VENC_IndexParamProcSet, &h264_func.sVeProcInfo);
	}
	else if(encode_param->encode_format == VENC_CODEC_H265)
	{
		loge("VENC CODES H265 is not supported yet\n");
		return VIDEOCODEC_FAIL;
	}

	return VIDEOCODEC_OK;
}

static void releaseMb(encode_param_t *encode_param)
{
	VencMBInfo *pMBInfo;
	VencMBModeCtrl *pMBMode;
	if(encode_param->encode_format == VENC_CODEC_H264 && h264_func.h264MBMode.mode_ctrl_en)
	{
		pMBInfo = &h264_func.MBInfo;
		pMBMode = &h264_func.h264MBMode;
	}
	else {
		return;
	}

	if(pMBInfo->p_para)
	{
		free(pMBInfo->p_para);
		pMBInfo->p_para = NULL;
	}
	if(pMBMode->p_info)
	{
		free(pMBMode->p_info);
		pMBMode->p_info = NULL;
	}
}

static int encoder_doProcessData(encode_param_t *encode_param,
                                       VencInputBuffer *inputBuffer,
                                       VencOutputBuffer *outputBuffer)
{
	unsigned int done_Number = 0;
	long long time1, time2;
	int ret;

#ifdef INPUTSOURCE_FILE
	AllocInputBufferDl(pVideoEnc, &encode_param->bufferParam);
	inputBuffer = &encode_param->inputBuffer;
#endif

	while(done_Number < encode_param->encode_frame_num)
	{
#ifdef INPUTSOURCE_FILE
		GetOneAllocInputBufferDl(pVideoEnc, inputBuffer);
		unsigned int size1, size2;
		size1 = fread(inputBuffer->pAddrVirY, 1,
		encode_param->src_size, encode_param->in_file);
		size2 = fread(inputBuffer->pAddrVirC, 1,
		encode_param->src_size/2, encode_param->in_file);
		if((size1!= encode_param->src_size) || (size2!= encode_param->src_size/2))
		{
			fseek(encode_param->in_file, 0L, SEEK_SET);
			size1 = fread(inputBuffer->pAddrVirY, 1,
							encode_param->src_size, encode_param->in_file);
			size2 = fread(inputBuffer->pAddrVirC, 1,
							encode_param->src_size/2, encode_param->in_file);
		}
		inputBuffer->bEnableCorp = 0;
		FlushCacheAllocInputBufferDl(pVideoEnc, inputBuffer);

		inputBuffer->nPts += 1*1000/encode_param->frame_rate;

		AddOneInputBufferDl(pVideoEnc, inputBuffer);
		time1 = GetNowUs();
		ret = VideoEncodeOneFrameDl(pVideoEnc);
		if(ret < 0)
		{
			loge("encoder error, goto out");
			return VIDEOCODEC_FAIL;
		}

		time2 = GetNowUs();
		printf("encode frame %d use time is %lldus..\n\n", done_Number,(time2-time1));
		AlreadyUsedInputBufferDl(pVideoEnc, inputBuffer);

		ReturnOneAllocInputBufferDl(pVideoEnc, inputBuffer);

		ret = GetOneBitstreamFrameDl(pVideoEnc, &encode_param->outputBuffer);
		if(ret == -1)
		{
			loge("Error - Get one stream frame fail!");
			return VIDEOCODEC_FAIL;
		}

		fwrite(encode_param->outputBuffer.pData0, 1, encode_param->outputBuffer.nSize0, encode_param->out_file);
		if(encode_param->outputBuffer.nSize1)
		{
			fwrite(encode_param->outputBuffer.pData1, 1, encode_param->outputBuffer.nSize1, encode_param->out_file);
		}

		FreeOneBitStreamFrameDl(pVideoEnc, &encode_param->outputBuffer);
		done_Number++;
#else
		ret = FlushCacheAllocInputBufferDl(pVideoEnc, inputBuffer);
		if(ret)
		{
			loge("FlushCacheAllocInputBuffer error, ret=%d", ret);
			return VIDEOCODEC_FAIL;
		}

		inputBuffer->nPts += 1*1000/encode_param->frame_rate;

		ret = AddOneInputBufferDl(pVideoEnc, inputBuffer);
		if(ret)
		{
			loge("AddOneInputBuffer error, ret=%d", ret);
			return VIDEOCODEC_FAIL;
		}

		time1 = GetNowUs();
		ret = VideoEncodeOneFrameDl(pVideoEnc);
		if(ret < 0)
		{
			loge("encoder error, goto out， ret=%d", ret);
			return VIDEOCODEC_FAIL;
		}
		time2 = GetNowUs();
		logt("encode frame %d use time is %lldus..", done_Number,(time2-time1));

		AlreadyUsedInputBufferDl(pVideoEnc,inputBuffer);

		ret = GetOneBitstreamFrameDl(pVideoEnc, outputBuffer);
		if(ret == -1)
		{
			loge("Error - Get one stream frame fail!");
			return VIDEOCODEC_FAIL;
		}
		done_Number++;
#endif
	}

	return VIDEOCODEC_OK;
}

int FreeEncoderOutputBuffer(VencOutputBuffer *outputBuffer)
{
	int ret;
	ret = FreeOneBitStreamFrameDl(pVideoEnc, outputBuffer);
	return ret;
}

int OpenEncoder(VENC_CODEC_TYPE type)
{
	loge("isp preview vencode version:%s", VERSION_STRING);

	if(type > VENC_CODEC_VP8 || type < VENC_CODEC_H264)
	{
		loge("Error - typs is illegal, please check the video codec tpye!");
		return VIDEOCODEC_FAIL;
	}
#ifdef VENC_DYNAMIC_LIBRARY
	loge("Link vencode function with dynamic library");
	//********load dynamic library********
	vencoderLib = dlopen("/usr/lib/libvencoder.so", RTLD_NOW);
	if(vencoderLib == NULL)
	{
		loge("Could not open /usr/lib/libvencoder.so library: %s", dlerror());
		return VIDEOCODEC_FAIL;
	}

	/* Clear any existing error */
	dlerror();

	VideoEncCreateDl              = (VideoEncCreateFunc)dlsym(vencoderLib, "VideoEncCreate");
	VideoEncDestroyDl             = (VideoEncDestroyFunc)dlsym(vencoderLib, "VideoEncDestroy");
	VideoEncInitDl                = (VideoEncInitFunc)dlsym(vencoderLib, "VideoEncInit");
	AllocInputBufferDl            = (AllocInputBufferFunc)dlsym(vencoderLib, "AllocInputBuffer");
	GetOneAllocInputBufferDl      = (GetOneAllocInputBufferFunc)dlsym(vencoderLib, "GetOneAllocInputBuffer");
	FlushCacheAllocInputBufferDl  = (FlushCacheAllocInputBufferFunc)dlsym(vencoderLib, "FlushCacheAllocInputBuffer");
	ReturnOneAllocInputBufferDl   = (ReturnOneAllocInputBufferFunc)dlsym(vencoderLib, "ReturnOneAllocInputBuffer");
	AddOneInputBufferDl           = (AddOneInputBufferFunc)dlsym(vencoderLib, "AddOneInputBuffer");
	VideoEncodeOneFrameDl         = (VideoEncodeOneFrameFunc)dlsym(vencoderLib, "VideoEncodeOneFrame");
	AlreadyUsedInputBufferDl      = (AlreadyUsedInputBufferFunc)dlsym(vencoderLib, "AlreadyUsedInputBuffer");
	GetOneBitstreamFrameDl        = (GetOneBitstreamFrameFunc)dlsym(vencoderLib, "GetOneBitstreamFrame");
	FreeOneBitStreamFrameDl       = (FreeOneBitStreamFrameFunc)dlsym(vencoderLib, "FreeOneBitStreamFrame");
	VideoEncGetParameterDl        = (VideoEncGetParameterFunc)dlsym(vencoderLib, "VideoEncGetParameter");
	VideoEncSetParameterDl        = (VideoEncSetParameterFunc)dlsym(vencoderLib, "VideoEncSetParameter");
	VideoEncoderSetFreqDl         = (VideoEncoderSetFreqFunc)dlsym(vencoderLib, "VideoEncoderSetFreq");
#elif defined VENC_STATIC_LIBRARY
	loge("Link vencode function with static library");
	VideoEncCreateDl              = &VideoEncCreate;
	VideoEncDestroyDl             = &VideoEncDestroy;
	VideoEncInitDl                = &VideoEncInit;
	AllocInputBufferDl            = &AllocInputBuffer;
	GetOneAllocInputBufferDl      = &GetOneAllocInputBuffer;
	FlushCacheAllocInputBufferDl  = &FlushCacheAllocInputBuffer;
	ReturnOneAllocInputBufferDl   = &ReturnOneAllocInputBuffer;
	AddOneInputBufferDl           = &AddOneInputBuffer;
	VideoEncodeOneFrameDl         = &VideoEncodeOneFrame;
	AlreadyUsedInputBufferDl      = &AlreadyUsedInputBuffer;
	GetOneBitstreamFrameDl        = &GetOneBitstreamFrame;
	FreeOneBitStreamFrameDl       = &FreeOneBitStreamFrame;
	VideoEncGetParameterDl        = &VideoEncGetParameter;
	VideoEncSetParameterDl        = &VideoEncSetParameter;
	VideoEncoderSetFreqDl         = &VideoEncoderSetFreq;
#endif

	if(!VideoEncCreateDl || !VideoEncDestroyDl || !VideoEncInitDl || !AllocInputBufferDl \
	|| !GetOneAllocInputBufferDl || !FlushCacheAllocInputBufferDl || !ReturnOneAllocInputBufferDl \
	|| !AddOneInputBufferDl || !VideoEncodeOneFrameDl || !AlreadyUsedInputBufferDl || !GetOneBitstreamFrameDl \
	|| !FreeOneBitStreamFrameDl || !VideoEncGetParameterDl || !VideoEncSetParameterDl || !VideoEncoderSetFreqDl)
	{
#ifdef VENC_DYNAMIC_LIBRARY
		dlclose(vencoderLib);
		vencoderLib = NULL;
		loge("Could not find symbol: %s", dlerror());
#endif
		loge("Could not get vencode function symbol");
		VideoEncCreateDl = NULL;
		VideoEncDestroyDl = NULL;
		VideoEncInitDl = NULL;
		AllocInputBufferDl = NULL;
		GetOneAllocInputBufferDl = NULL;
		FlushCacheAllocInputBufferDl = NULL;
		ReturnOneAllocInputBufferDl = NULL;
		AddOneInputBufferDl = NULL;
		VideoEncodeOneFrameDl = NULL;
		AlreadyUsedInputBufferDl = NULL;
		GetOneBitstreamFrameDl = NULL;
		FreeOneBitStreamFrameDl = NULL;
		VideoEncGetParameterDl = NULL;
		VideoEncSetParameterDl = NULL;
		VideoEncoderSetFreqDl = NULL;

		return VIDEOCODEC_FAIL;
	}
	//********end load********************
	pVideoEnc = VideoEncCreateDl(type);
	if(pVideoEnc == NULL)
	{
		loge("Error - Create Video encoder fail!");
		return VIDEOCODEC_FAIL;
	}
	return VIDEOCODEC_OK;
}

int PrepareEncoder(encode_param_t *encode_param)
{
	int ret;
	VencBaseConfig baseConfig;

	if(pVideoEnc == NULL)
	{
		loge("Error - Video encoder has not been create normally, should call openEnc first");
		return VIDEOCODEC_FAIL;
	}

	encode_param->inputBuffer.bEnableCorp = 0;
	memset(&baseConfig, 0 ,sizeof(VencBaseConfig));
	baseConfig.nInputWidth= encode_param->src_width;
	baseConfig.nInputHeight = encode_param->src_height;
	baseConfig.nStride = encode_param->src_width;
	baseConfig.nDstWidth = encode_param->dst_width;
	baseConfig.nDstHeight = encode_param->dst_height;
	baseConfig.eInputFormat = encode_param->picture_format;
	baseConfig.bEncH264Nalu = 0;
	ret = setEncParam(pVideoEnc, encode_param);
	if(ret)
	{
		loge("Error - Set Encode Paramant fail");
		goto out;
	}

	ret = VideoEncInitDl(pVideoEnc, &baseConfig);
	if(ret)
	{
		loge("Error - Video Encode Init fail");
		goto out;
	}
	return VIDEOCODEC_OK;

out:
	CloseEncoder(encode_param);
	return VIDEOCODEC_FAIL;
}

int StartEncoder(encode_param_t *encode_param, VencInputBuffer *input, VencOutputBuffer *output, vencode_command type)
{
	int ret;
	int head_num;

	if(type == VENCODE_CMD_HEAD_PPSSPS && encode_param->encode_format == VENC_CODEC_H264)
	{
#ifdef INPUTSOURCE_FILE
		ret = VideoEncGetParameterDl(pVideoEnc, VENC_IndexParamH264SPSPPS, &encode_param->sps_pps_data);
		fwrite(encode_param->sps_pps_data.pBuffer, 1, encode_param->sps_pps_data.nLength, encode_param->out_file);
		logt("sps_pps_data.nLength: %d\n", encode_param->sps_pps_data.nLength);
		for(head_num = 0; head_num < encode_param->sps_pps_data.nLength; head_num++)
			logt("the sps_pps :%02x\n", *(encode_param->sps_pps_data.pBuffer+head_num));
#else
		ret = VideoEncGetParameterDl(pVideoEnc, VENC_IndexParamH264SPSPPS, &encode_param->sps_pps_data);
		if(ret)
		{
			CloseEncoder(encode_param);
			loge("get video parameter fail");
			return VIDEOCODEC_FAIL;
		}
		for(head_num = 0; head_num < encode_param->sps_pps_data.nLength; head_num++)
			logt("the sps_pps :%02x\n", *(encode_param->sps_pps_data.pBuffer+head_num));
#endif
	}
	else if(type == VENCODE_CMD_STREAM && encode_param->encode_format == VENC_CODEC_H264)
	{
		ret = encoder_doProcessData(encode_param, input, output);
		if(ret)
		{
			CloseEncoder(encode_param);
			loge("process encoder data fail");
			return VIDEOCODEC_FAIL;
		}
	}
	else {
		loge("unknown cmd = %d or is not supported encoder format = %d", type, encode_param->encode_format);
		return VIDEOCODEC_FAIL;
	}

	return VIDEOCODEC_OK;
}

int CloseEncoder(encode_param_t *encode_param)
{
	if(pVideoEnc)
	{
		VideoEncDestroyDl(pVideoEnc);
		pVideoEnc = NULL;
	}
#ifdef VENC_DYNAMIC_LIBRARY
	//libvencoder.so should be dlclose after other Func run
	if(vencoderLib)
	{
			dlclose(vencoderLib);
			vencoderLib = NULL;
	}
#endif

#ifdef INPUTSOURCE_FILE
	if(encode_param->in_file)
		fclose(encode_param->in_file);

	if(encode_param->out_file)
		fclose(encode_param->out_file);
#endif
	releaseMb(encode_param);
	return VIDEOCODEC_OK;
}
