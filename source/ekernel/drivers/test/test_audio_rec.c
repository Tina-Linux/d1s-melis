#include <stdio.h>
#include <string.h>
#include <mod_ginkgo.h>
#include <mod_defs.h>
#include <mod_audio.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include <log.h>


__mp *ginkgo_hcedar = NULL;
__u32 mid_ginkgo = 0;
#define AUDIO_REC "/dev/audio_rec0"
#define FILE_BUF_SIZE 64*1024

int mp3_audio_rec_test(int argc, const char **argv)
{
#if 0
	__s32 p_hdl_audio	  = NULL;
	FILE	*p_hdl_pcm		= NULL;
	static __u32 temp=0;
	__audio_dev_para_t ply_para	= {0};
	__s32   ret = EPDK_FAIL;
	__u8 * pcm_file_buf = NULL;

	p_hdl_audio = esDEV_fopen(AUDIO_REC,O_WRONLY);
	if(NULL == p_hdl_audio)
	{ 
		__log("AUDIO_REC open failed!!!!!!!!!!!!!\n");
		return -1; 
	}
	__log("AUDIO_REC open ok!!!!!!!!!!!!!\n");
	int args[2]={0};

	pcm_file_buf	  = (__u8 *)malloc(FILE_BUF_SIZE);
	if(NULL == pcm_file_buf)
	{ 
		if(p_hdl_audio)	
		{
		   esDEV_fclose(p_hdl_audio);
		}
		__log("PCM file buffer request failed !!!!!!!!!!!!!\n");
		return -1; 
	}
	__log("PCM file buffer request ok !!!!!!!!!!!!!\n");
#if 1
	ply_para.chn	  = 1;
	ply_para.bps	  = 16;
	ply_para.fs	  = 44100;

	args[0] = 0;
    args[1] = &ply_para;
	ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_SET_PARA,args); 
	if(EPDK_FAIL == ret)
	{
		__log("AUDIO_DEV_CMD_SET_PARA failed1====\n"); 
		return -1;
	}
#endif
	args[0] = AUDIO_REC_USR_MIC;
    args[1] = 0;
	ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_REG_USERMODE, args); 

	args[0] = 0;
    args[1] = 0;
	esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_SET_SAMPCNT, args);

	args[0] = 0;
	args[1] = 0;
	ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_GET_VOLUME, args);
	__log("GET_VOLUME = %d",ret);
	//args[0] = 60;
	//args[1] = 0;
	//ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_SET_VOLUME, args);
	//__log("SET_VOLUME = %d",ret);返回值不是音量值 0-100

	args[0] = 0;
	args[1] = 0;
	ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_START, args);

	if(EPDK_FAIL == ret)
	{ 
		__log("AUDIO_DEV_CMD_START failed====\n");
	}
	esKRNL_TimeDly(5);

	while(1)
	{ 
		/*1. open pcm file*/
		if(NULL == p_hdl_pcm ||temp ==0)
		{ 
			p_hdl_pcm = fopen("e:\\sound.pcm", "ab+");
			if(NULL == p_hdl_pcm)
			{ 
			   __log("test.pcm file is not opened!!!!\n");
			   esKRNL_TimeDly(100);
			   continue; 
			}
		}
		temp++;

		if(temp == -1)
		{
			args[0] = 0;
			args[1] = 0;
			ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_PAUSE, args);
			__log("PAUSE");
		}
		if(temp == -1)
		{
			args[0] = 0;
			args[1] = 0;
			ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_CONTINUE, args);
			__log("CONTINUE");
		}
		if(temp == 5000)
		{
			fclose(p_hdl_pcm);
			args[0] = 0;
			args[1] = 0;
			ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_STOP, args);
			esKRNL_TimeDly(100);
			return 0;
		}
		/*3. certificate available length to output*/
		args[0] = AUDIO_DEV_QUERY_BUF_SIZE_DATA;
		args[1] = 0;
		ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_QUERY_BUFSIZE, args); 
		if( 0 == ret)
		{ 
			//__log("audio buffer is empty, wait next cycle!!!!\n");
			esKRNL_TimeDly(2);
			continue; 
		}
		__log("data len =%d",ret);
		/*4.write data to audio buffer*/
		args[0] = ret;
		args[1] = pcm_file_buf;
		ret = esDEV_fioctl(p_hdl_audio, AUDIO_DEV_CMD_READ_DATA, args); 

		fwrite(pcm_file_buf,1,ret,p_hdl_pcm);

		esKRNL_TimeDly(2); 
	}
#else
    __ginkgo_media_file_inf media_file;     // 录制文件信息
    __s32 audio_fd = -1;
	__s32 ret = 0;


    mid_ginkgo = esMODS_MInstall("d:\\mod\\ginkgo.mod", 0);
    if (!mid_ginkgo)
    {
        __err("open ginkgo fail!\n");
        return -1;
    }
	__log("\n esMODS_MInstall ok\n");
    ginkgo_hcedar = esMODS_MOpen(mid_ginkgo, 0);
	__log("\n esMODS_MOpen ok\n");

	memset(&media_file,0,sizeof(__ginkgo_media_file_inf));

	media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_MPEG12_LAYER3;
	media_file.audio_inf.uChn			= GINKGO_AUDIO_CHN_STEREO;
	media_file.audio_inf.uSampWidth		= GINKGO_AUDIO_SAMP_WIDTH_16;
	media_file.audio_inf.uAudioBps 		= 320000;
	media_file.audio_inf.uAudioSampRate	= 48000;
	media_file.audio_inf.uAudioSource  	= GINKGO_AUDIO_SRC_MIC;
	media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_MPEG12_LAYER3;
	media_file.audio_inf.uChn		   	= GINKGO_AUDIO_CHN_STEREO;
	media_file.uFileFmt  				= GINKGO_MEDIA_FILE_FMT_MP3;
	media_file.file_path = "f:\\REC200001010001.mp3";


    ret = esMODS_MIoctrl(ginkgo_hcedar, GINKGO_SET_MEDIAFILE, 0, (void *)&media_file);
	if (ret < 0)
    {
        __err("\n\nInput media file to ginkgo failed!\n\n");
        return -1;
    }
    else
    {
        __log("\n\nInput media file to ginkgo OK!\n\n");
    }
	//esMODS_MIoctrl(ginkgo_hcedar, GINKGO_CMD_STOP, 0, NULL);
    esMODS_MIoctrl(ginkgo_hcedar, GINKGO_CMD_START, 0, NULL);
	__log("\n GINKGO_CMD_START ok\n");

    return 0;

#endif
}

int wav_audio_rec_test(int argc, const char **argv)
{
    __ginkgo_media_file_inf media_file;     // 录制文件信息
    __s32 audio_fd = -1;
	__s32 ret = 0;


    mid_ginkgo = esMODS_MInstall("d:\\mod\\ginkgo.mod", 0);
    if (!mid_ginkgo)
    {
        __err("open ginkgo fail!\n");
        return -1;
    }
	__log("\n esMODS_MInstall ok\n");
    ginkgo_hcedar = esMODS_MOpen(mid_ginkgo, 0);
	__log("\n esMODS_MOpen ok\n");

	memset(&media_file,0,sizeof(__ginkgo_media_file_inf));

	media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_ADPCM;
	media_file.audio_inf.uChn			= GINKGO_AUDIO_CHN_STEREO;
	media_file.audio_inf.uSampWidth		= GINKGO_AUDIO_SAMP_WIDTH_16;
	media_file.audio_inf.uAudioBps 		= 320000;
	media_file.audio_inf.uAudioSampRate	= 48000;
	media_file.audio_inf.uAudioSource  	= GINKGO_AUDIO_SRC_MIC;
	media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_ADPCM;
	media_file.audio_inf.uChn		   	= GINKGO_AUDIO_CHN_STEREO;
	media_file.uFileFmt  				= GINKGO_MEDIA_FILE_FMT_WAV;
	media_file.file_path = "f:\\REC200001010001.wav";


    ret = esMODS_MIoctrl(ginkgo_hcedar, GINKGO_SET_MEDIAFILE, 0, (void *)&media_file);
	if (ret < 0)
    {
        __err("\n\nInput media file to ginkgo failed!\n\n");
        return -1;
    }
    else
    {
        __log("\n\nInput media file to ginkgo OK!\n\n");
    }
	//esMODS_MIoctrl(ginkgo_hcedar, GINKGO_CMD_STOP, 0, NULL);
    esMODS_MIoctrl(ginkgo_hcedar, GINKGO_CMD_START, 0, NULL);
	__log("\n GINKGO_CMD_START ok\n");

    return 0;
}

void record_close(void)
{
	int ret = -1;
	printf("=======record_close start==========\n");
	esMODS_MIoctrl(ginkgo_hcedar, GINKGO_CMD_STOP, 0, NULL);
	while(1)
	{
		 ret = esMODS_MIoctrl(ginkgo_hcedar, GINKGO_CMD_GET_STATUS, 0, NULL);
		 if(ret != GINKGO_STAT_RECORD)
		 {
			break;
		 }
		 esKRNL_TimeDly(5); 
	}
	
	esMODS_MClose(ginkgo_hcedar);
	//esDEV_fclose(audio_fd);
	//audio_fd = -1;
	esMODS_MUninstall(mid_ginkgo);
	ginkgo_hcedar = NULL;	
	mid_ginkgo = 0;
	printf("=======record_close end==========\n");
	return;
}

FINSH_FUNCTION_EXPORT_ALIAS(mp3_audio_rec_test, __cmd_mp3_rec_test, mp3_rec_test);
FINSH_FUNCTION_EXPORT_ALIAS(wav_audio_rec_test, __cmd_wav_rec_test, wav_rec_test);
FINSH_FUNCTION_EXPORT_ALIAS(record_close, __cmd_record_close, record_close_test);

