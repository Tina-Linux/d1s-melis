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

#include "beetles_app.h"

#if 0
#undef __here__
#undef __msg
#define __msg(...)      do{}while(0)
#define __here__                do{}while(0)
#endif



//#define FRAME_BUFFER_SIZE (3*512*1024)
//#define FRAME_BUFFER_SIZE (512*1024)

#define FRAME_BUFFER_SIZE (64*1024)

#define TEST_H264_FILE__ "f:\\xxx.dat"
//#define TEST_H264_FILE__ "d:\\mytest.h264"    //

extern __s32 robin_h264raw_play_start(void);
extern __s32 robin_h264raw_play_stop(void);
extern __s32 robin_h264raw_updata_stream(__u8 *frame_data, __u32 len);


__s32 h264_decode_init(void);
__s32 h264_decode_exit(void);


__s32 screen_width, screen_height;
RECT vedio_rect;
__hdle hvedio_lyr;
ES_FILE *hdisp;
__cedar_media_file_inf *pbuffer = NULL;
__bool f_h264_playing = 0;
__u32 video_buf_max = 0;
__u32 size_mem[20] = {0};


__s32 h264_decode_init(void)
{
    __s32 ret;
    robin_open_arg_t arg1;
    __msg("%s()\n", __func__);

    arg1.reserve_mem_size = 750 * 1024;
    ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg1);
    if (EPDK_OK != ret)
    {
        __msg("robin_open fail...\n");
        return EPDK_FAIL;
    }

    ret = robin_h264raw_play_start();
    return ret;
}

__s32 h264_decode_exit(void)
{
    robin_h264raw_play_stop();

    robin_close();
    return 0;
}


__s32 h264_decode_play(__u8 *buf, __u32 size)
{
    __s32 ret;
    eLIBs_printf("P-V: %d\n", size);

    if (buf == NULL || size == 0)
    {
        __log("[video_decode_play] fail!\n");
        return EPDK_FAIL;
    }

    robin_h264raw_updata_stream(buf, size);
    return EPDK_OK;
}


__u8 first_data = 0;
__s32 h264_decode_file(void)
{
    __u8 times, head[4], *file_buf;
    __s32 ret;
    __u32 file_len, read_len, len, buffer_len;
    ES_FILE *fp_video = NULL;
    __msg("%s()\n\n", __func__);
    __log("[]open %s\n", TEST_H264_FILE__);


    fp_video = eLIBs_fopen(TEST_H264_FILE__, "r");
    if (!fp_video)
    {
        __log("H264 file fail!!!\n");
        return EPDK_FAIL;
    }
    eLIBs_fseek(fp_video, 0, SEEK_END);
    file_len = eLIBs_ftell(fp_video);
    __log("file_len==%d\n", file_len);
    eLIBs_fseek(fp_video, 0, SEEK_SET);
    read_len = 0;
    file_buf = esMEMS_Malloc(0, 1 * 1024 * 1024);
    if (file_buf == NULL)
    {
        __log("error file_buf == NULL\n");
    }
    eLIBs_memset(file_buf, 0x0, 1 * 1024 * 1024);
    times = 0;
    while (read_len < file_len)
    {
        eLIBs_fread(head, 1, 4, fp_video);
        read_len += 4;
        len = (head[0] & 0xff) | ((head[1] & 0xff) << 8) | ((head[2] & 0xff) << 16) | ((head[3] & 0xff) << 24);

        if ((read_len + len) > file_len)
        {
            break;
        }

        eLIBs_fread(file_buf, 1, len, fp_video);

        if (first_data == 0)
        {
            __log("send first data [%d]\n", esKRNL_TimeGet());
            first_data = 1;
        }
        ret = robin_h264raw_updata_stream(file_buf, len);
        read_len += len;
        __msg("\n read_len==%d\n", read_len);
        esKRNL_TimeDly(3);

        times ++;
    }
    __log("-------------decode file end--------------------\n");
    esMEMS_Mfree(0, file_buf);
    return EPDK_OK;
}
