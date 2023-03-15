// WebRtcAudioTest.c : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>

#include "ring_buffer.h"

#include "echo_cancellation.h"
//#include "webrtc/modules/audio_processing/agc/include/gain_control.h"
//#include "webrtc/modules/audio_processing/ns/include/noise_suppression.h"
//#include "webrtc/common_audio/signal_processing/include/signal_processing_library.h"

#include <math.h>

#define NN 160				//samples

#define PCM_BUFF_LEN 4096	//samples
#define PCM_READ 1024		//samples 

int WebRtcAecTest(void)
{

	short *far_frame = NULL;			//aec_proc
	short *near_frame = NULL;
	short *out_frame = NULL;

	short *pcm_ref = NULL;  //	防止驱动修改读写长度
	short *pcm_micin = NULL;
	short *pcm_aecout = NULL;
	short *pcm_drcout = NULL;

	Ring_buf_manager ref_441_mono;
	Ring_buf_manager micin_441_mono;
	Ring_buf_manager out_441_mono;

	ref_441_mono.buf_size = 20480;
	micin_441_mono.buf_size = 20480;
	out_441_mono.buf_size = 20480; //bytes  // must be ==  N * read_size
	
	void *aecmInst = NULL;

	far_frame = (short *)malloc(NN * sizeof(short));
	if(far_frame == NULL)
	{
        printf("aec fatal error far_frame buffer malloc failed!\n");
        return -1;
	}

	near_frame = (short *)malloc(NN * sizeof(short));
	if(near_frame == NULL)
	{
        printf("aec fatal error near_frame buffer malloc failed!\n");
        goto ERR_EXIT0;
	}

	out_frame = (short *)malloc(NN * sizeof(short));
	if(out_frame == NULL)
	{
        printf("aec fatal error out_frame buffer malloc failed!\n");
        goto ERR_EXIT1;
	}

	pcm_ref = (short *)malloc(PCM_BUFF_LEN);
	if(pcm_ref == NULL)
	{
        printf("aec fatal error pcm_ref buffer malloc failed!\n");
        goto ERR_EXIT2;
	}

	pcm_micin = (short *)malloc(PCM_BUFF_LEN);
	if(pcm_micin == NULL)
	{
        printf("aec fatal error pcm_micin buffer malloc failed!\n");
        goto ERR_EXIT3;
	}

	pcm_aecout = (short *)malloc(PCM_BUFF_LEN);
	if(pcm_aecout == NULL)
	{
        printf("aec fatal error pcm_aecout buffer malloc failed!\n");
        goto ERR_EXIT4;
	}
	
	ring_buf_init(&ref_441_mono);
	ring_buf_init(&micin_441_mono);
	ring_buf_init(&out_441_mono);
	
	FILE *fp_far  = fopen("f:\\audio_data_t3\\ref_441.pcm", "rb");


	FILE *fp_near = fopen("f:\\audio_data_t3\\micin_441.pcm", "rb");


	FILE *fp_out  = fopen("f:\\audio_data_t3\\aec_out_441.pcm", "wb");

	//fread(temp, sizeof(short), 640, fp_far); //

	do 
	{
		if(!fp_far || !fp_near || !fp_out)
		{
			printf("WebRtcAecTest open file err \n");
			break;
		}

		WebRtcAec_Create(&aecmInst);
		WebRtcAec_Init(aecmInst, 16000, 16000);

		AecConfig config;
		config.nlpMode = kAecNlpAggressive;
		WebRtcAec_set_config(aecmInst, config);

		while(1)
		{
			if (PCM_READ == fread(pcm_ref, sizeof(short), PCM_READ, fp_far))
			{
				if (PCM_READ == fread(pcm_micin, sizeof(short), PCM_READ, fp_near))    // get the pcm_write_read_data;
                {
    				//save micin_ref to ringbuff
    				ring_buf_write_data(&micin_441_mono, (char *)pcm_micin, PCM_READ * sizeof(short));
    				ring_buf_write_data(&ref_441_mono, (char *)pcm_ref, PCM_READ * sizeof(short));

    				//get the data from ringbuff and aec_proc
    				while (ring_buf_get_valid_size(&micin_441_mono) >= (NN * sizeof(short)))
    				{
    					ring_buf_read_update_and_copy(&micin_441_mono, NN * 2, (unsigned char *)near_frame);
    					ring_buf_read_update_and_copy(&ref_441_mono, NN * 2, (unsigned char *)far_frame);

    					WebRtcAec_BufferFarend(aecmInst, far_frame, NN);//对参考声音(回声)的处理
    					WebRtcAec_Process(aecmInst, near_frame, NULL, out_frame, NULL, NN, 0, 0);//回声消除

    					ring_buf_write_data(&out_441_mono, (char *)out_frame, NN * sizeof(short));
    				}

    				//get the aec_out buffer
    				if (ring_buf_get_valid_size(&out_441_mono) >= PCM_READ * sizeof(short))
    				{
    					ring_buf_read_update_and_copy(&out_441_mono, PCM_READ * sizeof(short), (unsigned char *)pcm_aecout);
    					
    					fwrite(pcm_aecout, sizeof(short), PCM_READ, fp_out);
    				}
				}
			}
			else
			{
				break;
			}
		}
	} while (0);

    WebRtcAec_Free(aecmInst);

	fclose(fp_far);
	fclose(fp_near);
	fclose(fp_out);

	ring_buf_destroy(&ref_441_mono);
	ring_buf_destroy(&micin_441_mono);
	ring_buf_destroy(&out_441_mono);

ERR_EXIT5:
    if(NULL != pcm_aecout)
    {
        free(pcm_aecout);
        pcm_aecout = NULL;
    }
ERR_EXIT4:
    if(NULL != pcm_micin)
    {
        free(pcm_micin);
        pcm_micin = NULL;
    }
ERR_EXIT3:
    if(NULL != pcm_ref)
    {
        free(pcm_ref);
        pcm_ref = NULL;
    }
ERR_EXIT2:
	if(NULL != out_frame)
    {
        free(out_frame);
        out_frame = NULL;
    }
ERR_EXIT1:
    if(NULL != near_frame)
    {
        free(near_frame);
        near_frame = NULL;
    }
ERR_EXIT0:
    if(NULL != far_frame)
    {
        free(far_frame);
        far_frame = NULL;
    }
	return 0;
}

int main_aec_test(int argc, const char **argv)
{
	WebRtcAecTest();

	printf("声音增益，降噪结束...\n");

	getchar();
	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(main_aec_test, __cmd_aec_test, aec_test_demo);
