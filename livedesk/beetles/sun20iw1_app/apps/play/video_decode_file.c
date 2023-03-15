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
//read from code
//#define TEST_H264_FILE__ "f:\\iOS6_chaoqing_4byte.h264"
//#define TEST_H264_FILE__ "f:\\iOS_1920x1080_4byte.h264"
#include "beetles_app.h"

#include <rtthread.h>
#include <emodules/mod_display.h>
#include "robin_i.h"

#define FRAME_BUFFER_SIZE (2*1024*1024)
//#define FRAME_BUFFER_SIZE (1*8*1024)

//#define FRAME_BUFFER_SIZE (36*1024)

#define TEST_H264_FILE__ "f:\\XXX.H264"
//#define TEST_H264_FILE__ "d:\\mytest.h264"    //

extern __s32  robin_set_mediafile(__cedar_media_file_inf *pbuffer);
extern __s32 robin_set_play(void);
extern __s32 robin_set_stop(void);
extern __s32 robin_query_buffer(void);
extern __s32 robin_write_buffer(__u32 buffer_len, __u8 *buffer);
extern __s32 robin_resize_buffer(void);
extern __s32 robin_set_stream_info(__stream_inf_t *streaminf);
extern __s32 robin_set_stream_media_mode(__u8 tvmode);
extern void robin_set_avs_time(__u32 video_pts);
extern void robin_reset_sys_time(void);
extern __s32 robin_set_rotate_mode(__u8 mode);
extern __s32  robin_set_zoom(robin_zoom_e zoom);



#if 0
__s32 video_decode_file(__u8 rotate_mode)
{

    static __s32 count = 0;
    __u32 uFreeMemSize = 0;
    ES_FILE    *fp_video = NULL;
    __s32       copy_size = 0;
    __u8        num[4] = {0};
    __u32       buffer_len = 0;
    robin_open_arg_t arg;
    __s32 ret;
    __u8 i;
    __cedar_media_file_inf *pbuffer = NULL;
    __cedar_media_file_inf *pbuffer_backup;
    __s32 screen_width, screen_height;
    RECT vedio_rect;
    __hdle hvedio_lyr;
    ES_FILE *hdisp;
    /*----------------------------------------------------- */

    eLIBs_printf("open disp success...\n");
    arg.reserve_mem_size = 750 * 1024;
    ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);

    if (EPDK_OK != ret)
    {
        __msg("robin_open fail...\n");

        return EPDK_FAIL;
    }
	//get_screen_size(&screen_width, &screen_height);
    screen_width  = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    screen_height = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

   // dsk_display_get_size(&screen_width, &screen_height);
    vedio_rect.x = 0;
    vedio_rect.y = 0;
    vedio_rect.width = screen_width;
    vedio_rect.height = screen_height;
    eLIBs_printf("vedio screen=%d %d %d %d", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
    hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);

    if (!hvedio_lyr)
    {
        __msg("robin_request_vedio_layer fail...\n");
        goto l_decode_exit;
        //return EPDK_FAIL;
    }

    eLIBs_printf("robin_request_vedio_layer success...\n");

    /*----------------------------------------------------- */
#if 1
    eLIBs_printf("open %s\n", TEST_H264_FILE__);
    fp_video = eLIBs_fopen(TEST_H264_FILE__, "rb");

    if (!fp_video)
    {
        eLIBs_printf("=======open logo file fail=========\n");
        return -1;
        //goto l_decode_exit;
    }

    copy_size = eLIBs_fread(num, 1, 4, fp_video);

    if (copy_size != 4)
    {
        eLIBs_fclose(fp_video);
        fp_video = 0;
        __wrn("copy size =%d\n", copy_size);
        goto l_decode_exit;
        //return 0;
    }

    buffer_len = (num[0] & 0xff) | ((num[1] & 0xff) << 8) | ((num[2] & 0xff) << 16) | ((num[3] & 0xff) << 24);
    pbuffer = (__cedar_media_file_inf *)esMEMS_Malloc(0, sizeof(__cedar_media_file_inf));

    if (pbuffer == 0)
    {
        __wrn("pbuffer malloc failed\n");
        //return EPDK_FAIL;
        goto l_decode_exit;
    }

    pbuffer->stream_buf_inf.buffer = (__u8 *)esMEMS_Malloc(0, FRAME_BUFFER_SIZE);

    if (!pbuffer->stream_buf_inf.buffer)
    {
        __wrn("pbuffer->stream_buf_inf.buffer malloc failed\n");
        //return -1;
        goto l_decode_exit;
    }

    eLIBs_memset(pbuffer->stream_buf_inf.buffer, 0, FRAME_BUFFER_SIZE);
    pbuffer->stream_buf_inf.buffer[0] = num[0];
    pbuffer->stream_buf_inf.buffer[1] = num[1];
    pbuffer->stream_buf_inf.buffer[2] = num[2];
    pbuffer->stream_buf_inf.buffer[3] = num[3];
    copy_size = eLIBs_fread(pbuffer->stream_buf_inf.buffer + 4, 1, buffer_len, fp_video);

    if (copy_size != buffer_len)
    {
        eLIBs_fclose(fp_video);
        fp_video = 0;
        esMEMS_Mfree(0, pbuffer->stream_buf_inf.buffer);
        esMEMS_Mfree(0, pbuffer);
        __wrn("copy_size!=buffer_len\n");
        //return 0;
        goto l_decode_exit;
    }

#endif
    eLIBs_memcpy(pbuffer->file_path, "XXX.H264", 9);
    pbuffer->tag_inf_validflag = 0;
    pbuffer->stream_buf_inf.buffer_len = buffer_len + 4;
    pbuffer->stream_buf_inf.frame_rate = 30;
    __log("file_path=%s,buffer=0x%x,len=0x%x\n", pbuffer->file_path, pbuffer->stream_buf_inf.buffer, pbuffer->stream_buf_inf.buffer_len);
    robin_set_mediafile(pbuffer);
    // robin_set_rotate_mode(rotate_mode);
    eLIBs_printf("before robin_set_play\n");
    robin_set_play();
    pbuffer_backup = pbuffer;

    while (1)
    {
        ret = robin_query_buffer();
        if (ret >9)
        {
           // __log(" **********robin_query_buffer failed**************\n");
            esKRNL_TimeDly(1);
            continue;
        }


        if (!(count++ % 30))
        {
            __msg("ringbuffer freesize=0x%x(%d)\n", ret, ret); // printf  ringbuffer memory left size
            //uFreeMemSize = esMEMS_FreeMemSize();
            //__msg("left memory=0x%x(%d)\n",uFreeMemSize,uFreeMemSize);//printf system memory left size
        }


        if (pbuffer->stream_buf_inf.buffer_len <= FRAME_BUFFER_SIZE)
        {
        //	__log("buffer_len[%d]\n",pbuffer->stream_buf_inf.buffer_len);
            ret = robin_write_buffer(pbuffer->stream_buf_inf.buffer_len, pbuffer->stream_buf_inf.buffer);
        }
        else
        {
            __log("frame lenth(%d) large then %d, skip this frame\n", buffer_len, FRAME_BUFFER_SIZE);
        }

        copy_size = eLIBs_fread(num, 1, 4, fp_video);

        if (copy_size != 4)
        {
            __log("**************copy size =%d**************\n", copy_size);
            //eLIBs_fclose(fp_video);
            //fp_video = 0;
            //esMEMS_Mfree(0, pbuffer->stream_buf_inf.buffer);
            //pbuffer->stream_buf_inf.buffer = 0;
            //esMEMS_Mfree(0, pbuffer);
            //pbuffer = 0;
            goto l_decode_exit;
            //return 0;
        }

        buffer_len = (num[0] & 0xff) | ((num[1] & 0xff) << 8) | ((num[2] & 0xff) << 16) | ((num[3] & 0xff) << 24);

        if (buffer_len > FRAME_BUFFER_SIZE)
        {
            eLIBs_printf(" frame lenth(%d) learge then %d\n", buffer_len, FRAME_BUFFER_SIZE);
            continue;
        }

        eLIBs_memset(pbuffer->stream_buf_inf.buffer, 0, FRAME_BUFFER_SIZE);
        pbuffer->stream_buf_inf.buffer[0] = num[0];
        pbuffer->stream_buf_inf.buffer[1] = num[1];
        pbuffer->stream_buf_inf.buffer[2] = num[2];
        pbuffer->stream_buf_inf.buffer[3] = num[3];
        copy_size = eLIBs_fread(pbuffer->stream_buf_inf.buffer + 4, 1, buffer_len, fp_video);

        if (copy_size != buffer_len)
        {
            __log("**************copy_size!=buffer_len**************\n");
            //eLIBs_fclose(fp_video);
            //fp_video = 0;
            //esMEMS_Mfree(0, pbuffer->stream_buf_inf.buffer);
            //pbuffer->stream_buf_inf.buffer = 0;
            //esMEMS_Mfree(0, pbuffer);
            //pbuffer = 0;
            //break;
            goto l_decode_exit;
            //return 0;
        }

        pbuffer->stream_buf_inf.buffer_len = buffer_len + 4;
		esKRNL_TimeDly(5);
        //eLIBs_printf("buffer:0x%x,0x%x,0x%x,0x%x.len=0x%x,copy_size:0x%x\n",pbuffer->stream_buf_inf.buffer[0],pbuffer->stream_buf_inf.buffer[1],
        //pbuffer->stream_buf_inf.buffer[2],pbuffer->stream_buf_inf.buffer[3],pbuffer->stream_buf_inf.buffer_len,copy_size);
    }

l_decode_exit:
    //esKRNL_TimeDly(200);
    eLIBs_printf("[YG3]\n");
    robin_set_stop();
    //robin_close();
    eLIBs_printf("[YG2]\n");

    if (pbuffer->stream_buf_inf.buffer)
    {
        esMEMS_Mfree(0, pbuffer->stream_buf_inf.buffer);
        pbuffer->stream_buf_inf.buffer = NULL;
    }

    eLIBs_printf("[YG434]\n");

    if (pbuffer)
    {
        esMEMS_Mfree(0, pbuffer);
        pbuffer = NULL;
    }

    eLIBs_printf("[YG9999999999999]\n");
    robin_set_cmd_stop();
    eLIBs_printf("[YG55555]\n");

    if (hvedio_lyr)
    {
        robin_release_video_layer((void *)hvedio_lyr);
        hvedio_lyr = NULL;
    }

    if (fp_video)
    {
        eLIBs_fclose(fp_video);
        fp_video = NULL;
    }
	__log("[YG1]\n");
	robin_close();

    __log("[YG2]\n");
    return 0;
}
#else
__s32 video_decode_file(__u8 rotate_mode)
{
    static __s32 count = 0;
    __u32 uFreeMemSize = 0;
    ES_FILE    *fp_video = NULL;
    __s32       copy_size = 0;
    __u8        num[4] = {0};
    __u32       buffer_len = 0;
    robin_open_arg_t arg;
    __s32 ret;
    __u8 i;
    __cedar_media_file_inf *pbuffer = NULL;
    __cedar_media_file_inf *pbuffer_backup;
    __s32 screen_width, screen_height;
    RECT vedio_rect;
    __hdle hvedio_lyr;
    ES_FILE *hdisp;

    eLIBs_printf("open disp success...\n");
    arg.reserve_mem_size = 750 * 1024;
    ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);

    if (EPDK_OK != ret)
    {
        __err("robin_open fail...\n");
        return EPDK_FAIL;
    }
	//get_screen_size(&screen_width, &screen_height);
    screen_width  = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    screen_height = g_fioctrl(robin_hdis, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

    //dsk_display_get_size(&screen_width, &screen_height);
    vedio_rect.x = 0;
    vedio_rect.y = 0;
    vedio_rect.width = screen_width;
    vedio_rect.height = screen_height;
    eLIBs_printf("vedio screen=%d %d %d %d\n", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
    hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);

    if (!hvedio_lyr)
    {
        __err("robin_request_vedio_layer fail...");
        goto l_decode_exit;
    }

    eLIBs_printf("open %s\n", TEST_H264_FILE__);
    
    fp_video = eLIBs_fopen(TEST_H264_FILE__, "rb");
    if (!fp_video)
    {
        eLIBs_printf("open file fail...\n");
        return -1;
    }

    copy_size = eLIBs_fread(num, 1, 4, fp_video);
    if (copy_size != 4)
    {
        eLIBs_fclose(fp_video);
        fp_video = 0;
        __err("copy size:%d", copy_size);
        goto l_decode_exit;
    }

    buffer_len = (num[0] & 0xff) | ((num[1] & 0xff) << 8) | ((num[2] & 0xff) << 16) | ((num[3] & 0xff) << 24);
    pbuffer = (__cedar_media_file_inf *)esMEMS_Malloc(0, sizeof(__cedar_media_file_inf));

    if (pbuffer == 0)
    {
        __err("pbuffer malloc failed...");
        goto l_decode_exit;
    }

    pbuffer->stream_buf_inf.buffer = (__u8 *)esMEMS_Malloc(0, FRAME_BUFFER_SIZE);
    if (!pbuffer->stream_buf_inf.buffer)
    {
        __err("pbuffer->stream_buf_inf.buffer malloc failed...");
        goto l_decode_exit;
    }

    eLIBs_memset(pbuffer->stream_buf_inf.buffer, 0, FRAME_BUFFER_SIZE);
    pbuffer->stream_buf_inf.buffer[0] = num[0];
    pbuffer->stream_buf_inf.buffer[1] = num[1];
    pbuffer->stream_buf_inf.buffer[2] = num[2];
    pbuffer->stream_buf_inf.buffer[3] = num[3];
    copy_size = eLIBs_fread(pbuffer->stream_buf_inf.buffer + 4, 1, buffer_len, fp_video);
    if (copy_size != buffer_len)
    {
        __err("copy_size != buffer_len");
        goto l_decode_exit;
    }

    eLIBs_memcpy(pbuffer->file_path, "XXX.H264", 9);
    pbuffer->tag_inf_validflag = 0;
    pbuffer->stream_buf_inf.buffer_len = buffer_len + 4;
    eLIBs_printf("file_path=%s,buffer=0x%x,len=0x%x\n", pbuffer->file_path, pbuffer->stream_buf_inf.buffer, pbuffer->stream_buf_inf.buffer_len);
    robin_set_mediafile(pbuffer);
    //robin_set_zoom(ROBIN_ZOOM_USERDEF);
    //robin_set_rotate_mode(rotate_mode);		// 用于设置视频旋转角度
    robin_set_play();
    pbuffer_backup = pbuffer;

    while (1)
    {
        ret = robin_query_buffer();
        // The transfering of data will continue if there is enough space.
        if (ret < pbuffer->stream_buf_inf.buffer_len)
        {
            esKRNL_TimeDly(1);
            continue;
        }

        if (pbuffer->stream_buf_inf.buffer_len <= FRAME_BUFFER_SIZE)
        {
            ret = robin_write_buffer(pbuffer->stream_buf_inf.buffer_len, pbuffer->stream_buf_inf.buffer);
        }
        else
        {
            __err("frame length:%d larger than %d, skip this frame", buffer_len, FRAME_BUFFER_SIZE);
        }

        copy_size = eLIBs_fread(num, 1, 4, fp_video);
        if (copy_size != 4)
        {
            __err("copy size:%d != 4", copy_size);
            goto l_decode_exit;
        }

        buffer_len = (num[0] & 0xff) | ((num[1] & 0xff) << 8) | ((num[2] & 0xff) << 16) | ((num[3] & 0xff) << 24);

        if (buffer_len > FRAME_BUFFER_SIZE)
        {
            __err(" frame length:%d larger than %d", buffer_len, FRAME_BUFFER_SIZE);
            continue;
        }

        eLIBs_memset(pbuffer->stream_buf_inf.buffer, 0, FRAME_BUFFER_SIZE);
        pbuffer->stream_buf_inf.buffer[0] = num[0];
        pbuffer->stream_buf_inf.buffer[1] = num[1];
        pbuffer->stream_buf_inf.buffer[2] = num[2];
        pbuffer->stream_buf_inf.buffer[3] = num[3];
        copy_size = eLIBs_fread(pbuffer->stream_buf_inf.buffer + 4, 1, buffer_len, fp_video);

        if (copy_size != buffer_len)
        {
            __err("copy_size:%d != frame length:%d ", copy_size, buffer_len);
            goto l_decode_exit;
        }

        pbuffer->stream_buf_inf.buffer_len = buffer_len + 4;
    }

l_decode_exit:
	eLIBs_printf("[]l_decode_exit!!!\n");
    robin_set_stop();

    if (pbuffer->stream_buf_inf.buffer)
    {
        esMEMS_Mfree(0, pbuffer->stream_buf_inf.buffer);
        pbuffer->stream_buf_inf.buffer = NULL;
    }

    if (pbuffer)
    {
        esMEMS_Mfree(0, pbuffer);
        pbuffer = NULL;
    }

    robin_set_cmd_stop();

    if (hvedio_lyr)
    {
        robin_release_video_layer((void *)hvedio_lyr);
        hvedio_lyr = NULL;
    }

    if (fp_video)
    {
        eLIBs_fclose(fp_video);
        fp_video = NULL;
    }

	robin_close();
    return 0;
}

#endif


