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
#include <mod_cedar.h>
#include <mod_defs.h>
#include <mod_audio.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include <log.h>
#include <libc.h>

extern int fioctrl(ES_FILE* fp, int Cmd, void *args);

__mp *audio_robin_hcedar = NULL;
 __u32 audio_mid_cedar = 0;
int audio_fd = -1;

#define MAX_FILENAME  260     /* max file name length */

void main_audio_close_test(void);

char *get_name(const char *name)
{
    char *tempname = NULL;

    if (!name)
    {
        __err("get_name: input argument is NULL!\n");
        return NULL;
    }

    if(strlen(name) < 2)
    {
        __err("get_name: the length of input argument is not enough!\n");
        return NULL;
    }

    if ((((name[0] >= 'a' && name[0] <= 'z') ||
         (name[0] >= 'A' && name[0] <= 'Z')) &&
        name[1] == ':') || (name[0] == '/'))
    {
        /* get space for name */
        tempname = malloc(MAX_FILENAME);
        memset(tempname, '\0', MAX_FILENAME);

        if (!tempname)
        {
            __err("Malloc failed\n");
            return NULL;
        }
        strcpy(tempname, name);
    }
    else
    {
        __err("the input argument  format is incorrect!\n");
    }

    return tempname;
}

void free_name(char *name)
{
    free(name);
}

#define AUDIO_DEV "/dev/audio_play0"

int main_audio_play_test(int argc, const char **argv)
{
    __cedar_media_file_inf  file_info;
   // FILE* audio_fp = NULL;

    if (argv[1] == NULL)
    {
        __err("file name can not be not!\n");
        return -1;
    }
    char *filename = get_name(argv[1]);
    printf("aplay filename = %s\n", filename);

#if 0
    audio_fd = open(AUDIO_DEV, O_WRONLY);
    if(audio_fd < 0)
    {
        __err("Failed to open audio device, ret:%d", audio_fd);
        return -1;
    }

    unsigned long args[4]={0};
    args[0] = 160;
    args[1] = 0;
    ioctl(audio_fd, AUDIO_DEV_CMD_SET_VOLUME, args);
#endif
    memset(&file_info, 0, sizeof(__cedar_media_file_inf));
    strcpy(file_info.file_path, filename);
    free_name(filename);
    filename = NULL;

    audio_mid_cedar = esMODS_MInstall("d:\\mod\\cedar.mod", 0);
    if (!audio_mid_cedar)
    {
        __err("open cedar fail!\n");
        return -1;
    }
    audio_robin_hcedar = esMODS_MOpen(audio_mid_cedar, 0);

    esMODS_MIoctrl(audio_robin_hcedar, CEDAR_CMD_SET_MEDIAFILE, 0, (void *)&file_info);
    esMODS_MIoctrl(audio_robin_hcedar, CEDAR_CMD_SET_VOL, 30, NULL);
    esMODS_MIoctrl(audio_robin_hcedar, CEDAR_CMD_STOP, 0, NULL);
    esMODS_MIoctrl(audio_robin_hcedar, CEDAR_CMD_PLAY, 0, NULL);
#if 0
    esKRNL_TimeDly(10);
    __log("Please enter 'q' or Ctrl-C to quit audio_play_test command.\n");
    while (1)
    {
        char ch = getchar();
        if (ch == 'q' || ch == 3)
        {
            main_audio_close_test();
            break;
        }
    }
#endif
    return 0;
}

void main_audio_close_test(void)
{
	int ret = -1;
	int status =  0;
	printf("main_audio_close_test \n");
	esMODS_MIoctrl(audio_robin_hcedar, CEDAR_CMD_STOP, 0, NULL);
	
	while(1)
	{
	    status = esMODS_MIoctrl(audio_robin_hcedar, CEDAR_CMD_GET_STATUS, 0, NULL);
        if (status == CEDAR_STAT_STOP)
        {
            break;
        }
        esKRNL_TimeDly(10);	
	}
	if(audio_robin_hcedar != NULL)
	{
    	esMODS_MClose(audio_robin_hcedar);
    	audio_robin_hcedar = NULL;
    }
 #if 0
    if(audio_fd > 0)
    {
    	close(audio_fd);
    	audio_fd = -1;
    }
  #endif
    if(audio_mid_cedar != 0)
    {
    	esMODS_MUninstall(audio_mid_cedar);
     	audio_mid_cedar = 0;
    }
    printf("main_audio_close_test end\n"); 
	
}

FINSH_FUNCTION_EXPORT_ALIAS(main_audio_play_test, __cmd_audio_play_test, audio_play_test);
FINSH_FUNCTION_EXPORT_ALIAS(main_audio_close_test, __cmd_play_close, play_close_test);


