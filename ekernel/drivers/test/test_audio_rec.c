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

void record_close(void);

int mp3_audio_rec_test(int argc, const char **argv)
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

    media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_MPEG12_LAYER3;
    media_file.audio_inf.uChn			= GINKGO_AUDIO_CHN_MONO;
    media_file.audio_inf.uSampWidth		= GINKGO_AUDIO_SAMP_WIDTH_16;
    media_file.audio_inf.uAudioBps 		= 320000;
    media_file.audio_inf.uAudioSampRate	= 48000;
    media_file.audio_inf.uAudioSource  	= GINKGO_AUDIO_SRC_MIC;
    media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_MPEG12_LAYER3;
    media_file.audio_inf.uChn		   	= GINKGO_AUDIO_CHN_MONO;
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

#if 0
    esKRNL_TimeDly(10);
    __log("Please enter 'q' or Ctrl-C to quit mp3_rec_test command.\n");
    while (1)
    {
        char ch = getchar();
        if (ch == 'q' || ch == 3)
        {
            record_close();
            break;
        }
    }
#endif
    return 0;
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
    media_file.audio_inf.uChn			= GINKGO_AUDIO_CHN_MONO;
    media_file.audio_inf.uSampWidth		= GINKGO_AUDIO_SAMP_WIDTH_16;
    media_file.audio_inf.uAudioBps 		= 320000;
    media_file.audio_inf.uAudioSampRate	= 48000;
    media_file.audio_inf.uAudioSource  	= GINKGO_AUDIO_SRC_MIC;
    media_file.audio_inf.uAbsFmt		= GINKGO_ABS_TYPE_ADPCM;
    media_file.audio_inf.uChn		   	= GINKGO_AUDIO_CHN_MONO;
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
#if 0
    esKRNL_TimeDly(10);
    __log("Please enter 'q' or Ctrl-C to quit wav_rec_test command.\n");
    while (1)
    {
        char ch = getchar();
        if (ch == 'q' || ch == 3)
        {
            record_close();
            break;
        }
    }
#endif
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

