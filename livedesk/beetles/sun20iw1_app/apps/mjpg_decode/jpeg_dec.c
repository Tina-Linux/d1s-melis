//by valley,in hisino,2018

#include "beetles_app.h"
#include "mod_codec_cmd.h"


typedef struct _VirtualDE_DispVideoFb_
{
    __s32   id;
    void   *addr[3];
    //__u32   addr_right[3];   //used when in frame packing 3d mode
    void   *addr_right[3];
    __bool  interlace;
    __bool  top_field_first;
    __u32   frame_rate; // *FRAME_RATE_BASE(现在定为1000)
    __u32   flag_addr;//dit maf flag address
    __u32   flag_stride;//dit maf flag line stride
    __bool  maf_valid;
    __bool  pre_frame_valid;
    __bool  discontinous;
} stJpg_DispVideoFb;


#if 1
#define __msg__(...)        do{(eLIBs_printf("MSG:L%d(%s):", __LINE__, eLIBs_strchrlast((char *)__FILE__,'/')+1),      \
                                    eLIBs_printf(__VA_ARGS__));} while(0)
#define __here__                do{eLIBs_printf("@L%d(%s)\n", __LINE__, eLIBs_strchrlast((char *)__FILE__,'/')+1);} while(0)
#define __inf__(...)      eLIBs_printf(__VA_ARGS__)
#else
#define __msg__(...)        do{}while(0)
#define __here__                do{}while(0)
#define __inf__(...)        do{}while(0)
#endif


#define YUV_SIZE                                (SCREEN_WIDTH*SCREEN_HEIGHT + 100*1024)
#define IMG_DATA_BUF_SIZE               (2*1024*1024)
__u16 pic_w = 0;
__u16 pic_h = 0;
__s32 v_layer;
__u32 mod_id;
__mp *vcoder;
__mp *jpgdc;
ES_FILE *p_disp;
stJpegFrameInfo jpeg_info;
__disp_layer_info_t layer_para;

__s32 jpg_display_layer_request(void)
{
    __u64 arg[3] = {0};

    if (p_disp == 0)
    {
        __here__;
       // p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");
		p_disp  = esKSRV_Get_Display_Hld();

        if (p_disp == 0)
        {
            eLIBs_printf("open disp failed.\n");
            return EPDK_FAIL;
        }
    }

    //  eLIBs_memset(&pvideo_ctrl->layer_para, 0, sizeof(__disp_layer_info_t));
    arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
    arg[1] = 0;
    arg[2] = 0;
    v_layer = esMODS_MIoctrl(p_disp, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);

    if (v_layer == 0)
    {
        __log("request layer0 fail");
    }
    else
    {
        layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
        layer_para.pipe = 0;
        arg[0] = v_layer;
        arg[1] = &layer_para;
        arg[2] = 0;
        esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
        esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);   // close first
    }

    return EPDK_OK;
}
__s32 jpg_display_layer_release(void)
{
    __u64 arg[3] = {0};
    arg[0] = v_layer;
    arg[1] = 0;
    arg[2] = 0;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
    esMODS_MIoctrl(p_disp, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
    return EPDK_OK;
}


__s32 mjpg_decode(void)
{
    ES_FILE *fp, *fp1;
    __s32 ret = -1;
    __u32 file_size, tmpsize;
    __u32 yuv_size = 0;
    ES_FILE *y_fp = NULL;
    ES_FILE *uv_fp = NULL;
    __u64 arg[3] = {0};
    __u32 frame_id = 0;
    stJpg_DispVideoFb   VideoFb;
    eLIBs_memset(&VideoFb, 0, sizeof(stJpg_DispVideoFb));
    __log("[YG] mjpg_decode start 1 \n");
    jpg_display_layer_request();
    mod_id = esMODS_MInstall("d:\\mod\\vcoder.mod", 0);

    if (!mod_id)
    {
        __msg__("install vcoder failed...\n");
        return EPDK_FAIL;
    }

    if (!vcoder)
    {
        vcoder = esMODS_MOpen(mod_id, 0);

        if (!vcoder)
        {
            __msg__("open mod ailed!\n");
            return EPDK_FAIL;
        }
    }

    __log("[YG] mjpg_decode start 2\n");
    fp = eLIBs_fopen("f:\\test1.jpg", "r+");

    if (fp == NULL)
    {
        eLIBs_printf("open  file err!\n");
        return EPDK_FAIL;
    }

    eLIBs_printf("open fp success\n");
    eLIBs_fseek(fp, 0, SEEK_END);
    file_size =  eLIBs_ftell(fp);
    eLIBs_fseek(fp, 0, SEEK_SET);
    jpeg_info.jpegData = esMEMS_Malloc(0, file_size);

    if (jpeg_info.jpegData == NULL)
    {
        __log("jpeg_info->jpegData malloc fail,file_size:%d\n", file_size);
    }

    tmpsize = eLIBs_fread(jpeg_info.jpegData, 1, file_size, fp);

    if (tmpsize != file_size)
    {
        __log("write file err! %d- %d\n", tmpsize, file_size);
    }

    jpeg_info.jpegData_len = tmpsize;
    yuv_size = 3000 * 3000 * 3 / 2;
    jpeg_info.yuv_buf = (__u32)esMEMS_Palloc(((yuv_size + 1023) >> 10), 0);

    if (jpeg_info.yuv_buf == NULL)
    {
        __log("jpeg_info->yuv_buf malloc fail,size:%d\n", yuv_size);
    }

    eLIBs_CleanFlushDCacheRegion((void *)jpeg_info.yuv_buf, yuv_size);
    ret = esMODS_MIoctrl(vcoder, MPEJ_CODEC_CMD_DECODER, 0, &jpeg_info);
    __wrn("[]ret:%d\n", ret);
    __wrn("[]pic_width:%d,pic_height:%d\n", jpeg_info.pic_width,jpeg_info.pic_height);
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /*初始化调用一次即可*/
    layer_para.fb.format        = DISP_FORMAT_YUV420_P;//DISP_FORMAT_YUV420_SP_UVUV;
    layer_para.fb.seq           = DISP_SEQ_UVUV;
    layer_para.fb.mode          = DISP_MOD_NON_MB_PLANAR;
    layer_para.fb.br_swap       = 0;
    layer_para.fb.cs_mode      = DISP_BT709;
    layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
    layer_para.pipe             = 0;
    layer_para.prio             = 0xFF;
    layer_para.alpha_en         = 0;
    layer_para.alpha_val        = 0;
    layer_para.ck_enable        = 0;
    layer_para.src_win.x        = 0;
    layer_para.src_win.y        = 0;
    layer_para.src_win.width    = jpeg_info.pic_width;
    layer_para.src_win.height   = jpeg_info.pic_height;
    layer_para.scn_win.x        = 0;
    layer_para.scn_win.y        = 0;
    layer_para.scn_win.width    = esMODS_MIoctrl(p_disp, MOD_DISP_GET_SCN_WIDTH, 0, 0);
    layer_para.scn_win.height   = esMODS_MIoctrl(p_disp, MOD_DISP_GET_SCN_HEIGHT, 0, 0);
    layer_para.fb.addr[0]       = (__u32)jpeg_info.y_buf;
    //layer_para.fb.addr[1]       = (void *)jpeg_info.u_buf;
    // layer_para.fb.addr[2]       = (void *)jpeg_info.v_buf;
    layer_para.fb.addr[2]       = (__u32)jpeg_info.u_buf;
    layer_para.fb.addr[1]       = (__u32)jpeg_info.v_buf;
    layer_para.fb.size.width = jpeg_info.pic_width;
    layer_para.fb.size.height = jpeg_info.pic_height;
    arg[0] = v_layer;
    arg[1] = &layer_para;
    arg[2] = 0;
	
    __wrn("[scn_win]width:%d,height:%d\n", layer_para.scn_win.width,layer_para.scn_win.height);
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
   // esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_TOP, 0, (void *)arg);
  //  esMODS_MIoctrl(p_disp, MOD_DISP_CMD_VIDEO_START, 0, (void *)arg);
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_OPEN, 0, (void *)arg);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //1.配置参数
    VideoFb.id = frame_id;
    VideoFb.addr[0] = (void *)jpeg_info.y_buf;
    // VideoFb.addr[1] = (void *)jpeg_info.u_buf;
    // VideoFb.addr[2] = (void *)jpeg_info.v_buf;
    VideoFb.addr[2] = (void *)jpeg_info.u_buf;
    VideoFb.addr[1] = (void *)jpeg_info.v_buf;
    VideoFb.interlace = 0;
    VideoFb.top_field_first = 0;
    VideoFb.pre_frame_valid = 0;
    arg[0] = v_layer;
    arg[1] = (__u32)&VideoFb;
    __wrn("[]v_layer:0x%x,p_disp:0x%x\n", v_layer, p_disp);
    esKRNL_TimeDly(500);
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_VIDEO_SET_FB, 0, (void *)arg);
    __wrn("MPEJ_CODEC_CMD_DECODER ret:%d,\n", ret);
    esKRNL_TimeDly(500);
    {
        /////////////////////////////////////////////////////////////
        /*第二张图片*/
        if (jpeg_info.jpegData)
        {
            esMEMS_Mfree(0, jpeg_info.jpegData);
            jpeg_info.jpegData = 0;
        }

        if (jpeg_info.yuv_buf != 0)
        {
            esMEMS_Pfree((void *)jpeg_info.yuv_buf, ((yuv_size + 1023) >> 10));
            jpeg_info.yuv_buf = 0;
        }

        fp1 = eLIBs_fopen("f:\\test2.jpg", "r+");

        if (fp1 == NULL)
        {
            eLIBs_printf("open  file err!\n");
            return EPDK_FAIL;
        }

        eLIBs_printf("open fp1 success\n");
        eLIBs_fseek(fp1, 0, SEEK_END);
        file_size =  eLIBs_ftell(fp1);
        eLIBs_fseek(fp1, 0, SEEK_SET);
        jpeg_info.jpegData = esMEMS_Malloc(0, file_size);

        if (jpeg_info.jpegData == NULL)
        {
            __log("1 jpeg_info->jpegData malloc fail,file_size:%d\n", file_size);
        }

        tmpsize = eLIBs_fread(jpeg_info.jpegData, 1, file_size, fp1);

        if (tmpsize != file_size)
        {
            __log("1 write file err! %d- %d\n", tmpsize, file_size);
        }

        jpeg_info.jpegData_len = tmpsize;
        yuv_size = 3000 * 3000 * 3 / 2;
        jpeg_info.yuv_buf = (__u32)esMEMS_Palloc(((yuv_size + 1023) >> 10), 0);

        if (jpeg_info.yuv_buf == NULL)
        {
            __log("jpeg_info->yuv_buf malloc fail,size:%d\n", yuv_size);
        }

        eLIBs_CleanFlushDCacheRegion((void *)jpeg_info.yuv_buf, yuv_size);
        ret = esMODS_MIoctrl(vcoder, MPEJ_CODEC_CMD_DECODER, 0, &jpeg_info);
        //1.配置参数
        VideoFb.id = frame_id + 1;
        VideoFb.addr[0] = (void *)jpeg_info.y_buf;
        // VideoFb.addr[1] = (void *)jpeg_info.u_buf;
        // VideoFb.addr[2] = (void *)jpeg_info.v_buf;
        VideoFb.addr[2] = (void *)jpeg_info.u_buf;
        VideoFb.addr[1] = (void *)jpeg_info.v_buf;
        VideoFb.interlace = 0;
        VideoFb.top_field_first = 0;
        VideoFb.pre_frame_valid = 0;
        arg[0] = v_layer;
        arg[1] = &VideoFb;
        __wrn("[1]v_layer:0x%x,p_disp:0x%x\n", v_layer, p_disp);
        esMODS_MIoctrl(p_disp, MOD_DISP_CMD_VIDEO_SET_FB, 0, (void *)arg);
        esKRNL_TimeDly(500);
        ////////////////////////////////////////////////////////////////////////
    }
    __wrn("1 mjpg decode picture(w,h):%d,%d\n", jpeg_info.pic_width, jpeg_info.pic_height);
    __wrn("1 yuv addr:0x%x,0x%x,0x%x\n", jpeg_info.y_buf, jpeg_info.u_buf, jpeg_info.v_buf);
#if 0

    if (jpeg_info.y_buf != 0)
    {
        y_fp = eLIBs_fopen("f:\\y_data.dat", "w+");
        eLIBs_fwrite((void *)jpeg_info.y_buf, 1, jpeg_info.pic_width * jpeg_info.pic_height, y_fp);
        __log("save y buf\n");
    }

    if (jpeg_info.u_buf != 0)
    {
        uv_fp = eLIBs_fopen("f:\\u_data.dat", "w+");
        eLIBs_fwrite((void *)jpeg_info.u_buf, 1, jpeg_info.pic_width * jpeg_info.pic_height / 2, uv_fp);
        __log("save uv buf\n");
    }

#endif
__exit:
    /*eLIBs_fclose(y_fp);*/
    /*eLIBs_fclose(uv_fp);*/
    eLIBs_fclose(fp);
    eLIBs_fclose(fp1);

    if (jpeg_info.jpegData)
    {
        esMEMS_Mfree(0, jpeg_info.jpegData);
    }

    if (jpeg_info.yuv_buf != 0)
    {
        esMEMS_Pfree((void *)jpeg_info.yuv_buf, ((yuv_size + 1023) >> 10));
        jpeg_info.yuv_buf = 0;
    }

    jpg_display_layer_release();
    esMODS_MClose(vcoder);
    vcoder = 0;
    esMODS_MUninstall(mod_id);
    mod_id = 0;
    __log("----------------------mjpg_decode over---------------------\n");
    return EPDK_OK;
}


#if 0
static void __check_pic_wh(__u8 *buf);

#if 1
void app_print_buf(__u8 *buf, __u32 len)
{
    __u32 i, j, m;

    //DMSG_INFO("%s()\n",__func__);
    if (!len)
    {
        return;
    }

    m = len / 32;
    //DMSG_INFO("len==%d\n",len);
    __inf("app_buf[%d]==\n", len);

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < 16; j++)
        {
            __inf("%02x ", *buf);
            buf ++;
        }

        __inf("- ");

        for (j = 0; j < 16; j++)
        {
            __inf("%02x ", *buf);
            buf ++;
        }

        __inf("\n");
    }

    m = len % 32;

    if (m)
    {
        if (m > 16)
        {
            for (j = 0; j < 16; j++)
            {
                __inf("%02x ", *buf);
                buf ++;
            }

            __inf("- ");
            m -= 16;
        }

        for (i = 0; i < m; i++)
        {
            __inf("%02x ", *buf);
            buf ++;
        }

        __inf("\n");
    }
}
#endif

__s32 jpeg_show_init(SIZE pic_size)
{
    __s32 ret;
    __u32 arg[3];
    __disp_layer_info_t layer_para;
    __msg__("%s()\n", __func__);
    __msg__("pic_size[w*h]==[%d*%d]\n", pic_size.width, pic_size.height);
    __msg__("mod_id==0x%x\n", mod_id);
    __msg__("vcoder==0x%x\n", vcoder);

    if (!mod_id)
    {
        __here__;
        mod_id = esMODS_MInstall("d:\\mod\\vcoder.mod", 0);

        if (!mod_id)
        {
            __msg__("install vcoder failed...\n");
            return EPDK_FAIL;
        }

        if (!vcoder)
        {
            __here__;
            vcoder = esMODS_MOpen(mod_id, 0);

            if (!vcoder)
            {
                __msg__("open mod ailed!\n");
                return EPDK_FAIL;
            }
        }
    }

    if (p_disp == 0)
    {
        __here__;
        p_disp = eLIBs_fopen("b:\\DISP\\DISPLAY", "r+");

        if (p_disp == 0)
        {
            eLIBs_printf("open disp failed.\n");
            return EPDK_FAIL;
        }
    }

    if (v_layer == 0)
    {
        __here__;
        arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
        arg[1] = 0;
        arg[2] = 0;
        v_layer = (__hdle)esMODS_MIoctrl(p_disp, MOD_DISP_LAYER_REQUEST, 0, (void *)arg);
        __msg__("v_layer==0x%x\n", v_layer);

        if (v_layer == 0)
        {
            eLIBs_printf("request v_layer failed.\n");
            return EPDK_FAIL;
        }
    }

    __here__;
    layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
    layer_para.fb.addr[0] = 0;
    layer_para.fb.addr[1] = 0;
    layer_para.fb.addr[2] = 0;
    layer_para.fb.size.width    = pic_size.width;
    layer_para.fb.size.height   = pic_size.height;
    __here__;
    layer_para.fb.mode          = DISP_MOD_MB_UV_COMBINED;
    //layer_para.fb.format        = DISP_FORMAT_YUV422;
    layer_para.fb.format        = DISP_FORMAT_YUV420;
    layer_para.fb.seq           = DISP_SEQ_UVUV;
    __here__;
    layer_para.fb.br_swap       = 0;
    layer_para.ck_enable        = 0;
    layer_para.alpha_en         = 1;
    layer_para.alpha_val        = 0xff;
    layer_para.pipe             = 0;
    layer_para.src_win.x        = 0;
    layer_para.src_win.y        = 0;
    layer_para.src_win.width    = pic_size.width;
    layer_para.src_win.height   = pic_size.height;

    //layer_para.scn_win.width    = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_WIDTH, 0, 0);
    //layer_para.scn_win.height   = eLIBs_fioctrl(p_disp, DISP_CMD_SCN_GET_HEIGHT, 0, 0);
    if (pic_size.height > pic_size.width)
    {
        //480/800=288/480
        //layer_para.scn_win.width = pic_size.width * SCREEN_HEIGHT / SCREEN_WIDTH;
        layer_para.scn_win.width = pic_size.width * SCREEN_HEIGHT / pic_size.height;
    }
    else
    {
        layer_para.scn_win.width = SCREEN_WIDTH;
    }

    //__msg__("scn_win.width==%d\n", layer_para.scn_win.width);
    layer_para.scn_win.x = (SCREEN_WIDTH - layer_para.scn_win.width) / 2;
    layer_para.scn_win.y = 0;
    layer_para.scn_win.height = SCREEN_HEIGHT;
    __msg__("scn_win[x,y,w,h]==[%d,%d,%d,%d]\n", layer_para.scn_win.x, layer_para.scn_win.y,
            layer_para.scn_win.width, layer_para.scn_win.height);
    arg[0] = (__u32)v_layer;
    arg[1] = (__u32)&layer_para;
    arg[2] = 0;
    __here__;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void *)arg);
    __here__;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
    arg[0] = (__u32)v_layer;
    arg[1] = 0;
    arg[2] = 0;
    __here__;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);
    __here__;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_OPEN, 0, (void *)arg);
    __here__;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_VIDEO_START, 0, (void *)arg);
    __here__;
    return EPDK_OK;
}

void jpeg_show_exit(void)
{
    __u32 arg[3] = {0};
    //__u8 err = 0;
    //__u8 i = 0;
    __msg__("%s()\n", __func__);
    esMODS_MClose(vcoder);
    vcoder = 0;
    esMODS_MUninstall(mod_id);
    mod_id = 0;
    pic_w = 0;
    pic_h = 0;
    __msg__("v_layer==0x%x\n", v_layer);

    if (v_layer)
    {
        __here__;
        arg[0] = (__u32)v_layer;
        arg[1] = 0;
        arg[2] = 0;
        esMODS_MIoctrl(p_disp, DISP_CMD_VIDEO_STOP, 0, (void *)arg);
        esMODS_MIoctrl(p_disp, MOD_DISP_CMD_LAYER_CLOSE, 0, (void *)arg);
        esMODS_MIoctrl(p_disp, MOD_DISP_LAYER_RELEASE, 0, (void *)arg);
        v_layer = 0;
    }

    eLIBs_fclose(p_disp);
    p_disp = 0;
    __here__;

    if (jpeg_info.c_buf)
    {
        __here__;
        eLIBs_free((void *)jpeg_info.c_buf);
    }

    if (jpeg_info.y_buf)
    {
        __here__;
        eLIBs_free((void *)jpeg_info.y_buf);
    }
}

__s32 jpeg_show(__u8 *buf, __u32 len)
{
    __s32 ret = EPDK_OK;
    __disp_video_fb_t tmpPara;
    __u32 arg[3];

    //__msg__("%s()\n",__func__);
    if (len == 0)
    {
        return EPDK_FAIL;
    }

    if (len > IMG_DATA_BUF_SIZE)
    {
        __wrn("err: too big img size: %d!!!!!!\n", len);
        return EPDK_FAIL;
    }

#if 1
    //__here__;
    len -= 0x12;
    buf += 0x12;
    buf[0] = 0xff;
    buf[1] = 0xd8;
#endif
    jpeg_info.jpegData = buf;
    jpeg_info.jpegData_len = len;
    __check_pic_wh(buf);
    jpeg_info.pic_height = 0;
    jpeg_info.pic_width = 0;
    ret = esMODS_MIoctrl(vcoder, MPEJ_CODEC_CMD_DECODER, 0, &jpeg_info);

    if (ret != EPDK_OK)
    {
        __msg__("jpeg decoder failed!\n");
        __msg__("jpegData_len==%d\n", jpeg_info.jpegData_len);
        __msg__("img head[]:\n");
        app_print_buf((__u8 *)(jpeg_info.jpegData), 64);
        __msg__("img tail[]:\n");
        app_print_buf((__u8 *)(jpeg_info.jpegData) + jpeg_info.jpegData_len - 64, 64);
        esKRNL_TimeDly(1000);
        return EPDK_FAIL;
    }

    //__msg__("jpeg decode succeed!\n");
    //__msg__("pic_height==%d\n",jpeg_info.pic_height);
    //__msg__("pic_width==%d\n",jpeg_info.pic_width);
    tmpPara.id = 0;
    tmpPara.addr[0] = jpeg_info.y_buf;
    tmpPara.addr[1] = jpeg_info.c_buf;
    tmpPara.addr[2] = 0;
#if 0   //SAVE_DEC_FILE
    {
        static int i = 0;
        ES_FILE *fp;
        esKRNL_TimeDly(100);

        if (i == 0)
        {
            i = 1;
            fp = eLIBs_fopen("f:\\y.dat", "wb+");

            if (fp)
            {
                eLIBs_fwrite((void *)jpeg_info.y_buf, 1, jpeg_info.pic_height * jpeg_info.pic_width, fp);
                eLIBs_fclose(fp);
                eLIBs_printf("write y.dat succeed\n");
            }
            else
            {
                eLIBs_printf("open y.dat failed!\n");
            }

            fp = eLIBs_fopen("f:\\c.dat", "wb+");

            if (fp)
            {
                eLIBs_fwrite((void *)jpeg_info.c_buf, 1, jpeg_info.pic_height * jpeg_info.pic_width / 2, fp);
                eLIBs_fclose(fp);
                eLIBs_printf("write c.dat succeed\n");
            }
            else
            {
                eLIBs_printf("open c.dat failed!\n");
            }

            fp = eLIBs_fopen("f:\\adb.jpg", "wb+");

            if (fp)
            {
                eLIBs_fwrite((void *)jpeg_info.jpegData, 1, jpeg_info.jpegData_len, fp);
                eLIBs_fclose(fp);
                eLIBs_printf("write adb.jpg succeed\n");
            }
            else
            {
                eLIBs_printf("open adb.jpg failed!\n");
            }
        }
    }
#endif
    arg[0] = (__u32)v_layer;
    arg[1] = (__u32)&tmpPara;
    arg[2] = 0;
    esMODS_MIoctrl(p_disp, MOD_DISP_CMD_VIDEO_SET_FB, 0, (void *)arg);
    //__here__;
    return EPDK_OK;
}

static void __jpg_init(__u16 w, __u16 h)
{
    __u32 n;
    SIZE size;
    __msg__("%s()\n", __func__);
    __msg__("cur w*h==[%d*%d]\n", w, h);
    __msg__("last w*h==[%d*%d]\n", pic_w, pic_h);
#if 0
    jpeg_info.jpegData_len = 0;

    if (jpeg_info.jpegData == NULL)
    {
        __here__;
        jpeg_info.jpegData = eLIBs_malloc(IMG_DATA_BUF_SIZE);

        if (jpeg_info.jpegData == NULL)
        {
            __msg__("allocate mem fail!!!");
            return;
        }
    }

#endif
    n = ((w * h + 1023) / 1024 + 100) * 1024;

    if (jpeg_info.c_buf == NULL)
    {
        jpeg_info.c_buf = (__u32)eLIBs_malloc(n);

        if (jpeg_info.c_buf == 0)
        {
            __wrn("allocate mem failed!!!\n");
            return;
        }

        eLIBs_memset((void *)(jpeg_info.c_buf), 0, n);
    }

    if (jpeg_info.y_buf == 0)
    {
        jpeg_info.y_buf = (__u32)eLIBs_malloc(n);

        if (jpeg_info.y_buf == 0)
        {
            __wrn("allocate mem failed!!!\n");
            return;
        }

        eLIBs_memset((void *)(jpeg_info.y_buf), 0, n);
    }

    size.width = w;
    size.height = h;
    //__here__;
    //esKRNL_TimeDly(300);
    //__here__;
    jpeg_show_init(size);
    //__here__;
    //esKRNL_TimeDly(300);
    //__here__;
    pic_w = w;
    pic_h = h;
}

static void __check_pic_wh(__u8 *buf)
{
    __u8 *p8;
    __u16 h, w;
    //__msg__("%s()\n",__func__);
    p8 = buf + 0x91;
    h = *p8 << 8;
    p8 ++;
    h += *p8;
    p8 ++;
    w = *p8 << 8;
    p8 ++;
    w += *p8;

    //__msg__("[w,h]==[%d,%d]\n",w,h);
    if (w && h && (w != pic_w || h != pic_h))
    {
        __here__;

        if (!(pic_w && pic_h))
        {
            GUI_RECT rect;
            __here__;
            rect.x0 = 0;
            rect.y0 = 0;
            rect.x1 = SCREEN_WIDTH - 1;
            rect.y1 = SCREEN_HEIGHT - 1;
            //GUI_SetBkColor(GUI_BLACK);
            //GUI_ClearRectEx(&rect);
        }

        __jpg_init(w, h);
        //__here__;
    }
}

#endif

