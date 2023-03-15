#include "video_dev.h"
#include "video_v4l2.h"
#include "dfs_posix.h"
#include "g2d_driver.h"

#define CHECK_FRAME_NUM 1
__video_ctrl_t* g_video_ctrl = NULL;

char g_has_started = 0;

/*
	Save a frame of data to local,for testing only
*/
static int video_save_frame(__u8* addr, int width, int height, int cnt)
{
	int ret = 0;
	char path[100];
	eLIBs_sprintf(path, "f:\\y%d.bin", cnt);
	void* fd = esFSYS_fopen(path, "wb");
	if (fd != NULL)
	{
		__u8* temp = (__u8*)addr;
		ret = esFSYS_fwrite(temp, width * height, 1, fd);
		esFSYS_fclose(fd);
	}

	eLIBs_sprintf(path, "f:\\uv%d.bin", cnt);
	fd = esFSYS_fopen(path, "wb");
	if (fd != NULL)
	{
		__u8* temp = (__u8*)addr + width * height;
		ret = esFSYS_fwrite(temp, width * height / 2, 1, fd);
		esFSYS_fclose(fd);
	}
	if (ret)
	{
		__log("save data ok!");
	}
	return ret;
}

/*
request display layer to show and init the config of show
*/
static int video_display_layer_request(void)
{
	unsigned long arg[3];

	arg[0] = MOD_DISP_LAYER_WORK_MODE_SCALER;
	arg[1] = 0;
	arg[2] = 0;
	g_video_ctrl->h_lyr = esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_LAYER_REQUEST, 0, (void*)arg);

	g_video_ctrl->scn_width = esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_GET_SCN_WIDTH, 0, 0);
	g_video_ctrl->scn_height = esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_GET_SCN_HEIGHT, 0, 0);

	if (g_video_ctrl->system == FORMAT_PAL)
	{
		g_video_ctrl->frame_width = PAL_WIDTH;
		g_video_ctrl->frame_height = PAL_HEIGHT;
	}
	else
	{
		g_video_ctrl->frame_width = NTSC_WIDTH;
		g_video_ctrl->frame_height = NTSC_HEIGHT;
	}

	if (g_video_ctrl->disp_buffer == NULL)
	{
		g_video_ctrl->disp_buffer = esMEMS_Palloc((FRAME_PAL_SIZE + 1023) >> 10, 0);//以PAL申请显示buffer，制式切换时不重新申请内存
		memset(g_video_ctrl->disp_buffer, 0x80, FRAME_PAL_SIZE);
	}
	if (g_video_ctrl->temp_buffer == NULL)
	{
		g_video_ctrl->temp_buffer = esMEMS_Palloc((FRAME_PAL_SIZE + 1023) >> 10, 0);//以PAL申请显示buffer，制式切换时不重新申请内存
		memset(g_video_ctrl->temp_buffer, 0x80, FRAME_PAL_SIZE);
	}

	//目前使用YUV420_SP_UVUV格式地址
	g_video_ctrl->layer_para.fb.addr[0] = (unsigned long)g_video_ctrl->disp_buffer;
	g_video_ctrl->layer_para.fb.addr[1] = (unsigned long)g_video_ctrl->disp_buffer + g_video_ctrl->frame_width * g_video_ctrl->frame_height;
	g_video_ctrl->layer_para.fb.addr[2] = (unsigned long)g_video_ctrl->disp_buffer + g_video_ctrl->frame_width * g_video_ctrl->frame_height * 5 / 4;

	if ((g_video_ctrl->rotate_angle == VIDEO_PLAY_ROTATE_90) || (g_video_ctrl->rotate_angle == VIDEO_PLAY_ROTATE_270))
	{
		g_video_ctrl->layer_para.fb.size.width = g_video_ctrl->frame_height;
		g_video_ctrl->layer_para.fb.size.height = g_video_ctrl->frame_width;
	}
	else
	{
		g_video_ctrl->layer_para.fb.size.width = g_video_ctrl->frame_width;
		g_video_ctrl->layer_para.fb.size.height = g_video_ctrl->frame_height;
	}

	g_video_ctrl->layer_para.mode = MOD_DISP_LAYER_WORK_MODE_SCALER;
	g_video_ctrl->layer_para.fb.mode = DISP_MOD_NON_MB_PLANAR;
	g_video_ctrl->layer_para.fb.format = DISP_FORMAT_YUV420_SP_UVUV;//目前使用YUV420_SP_UVUV格式显示
	g_video_ctrl->layer_para.fb.br_swap = 0;
	g_video_ctrl->layer_para.fb.seq = DISP_SEQ_UVUV;
	g_video_ctrl->layer_para.ck_enable = 0;
	g_video_ctrl->layer_para.alpha_en = 0;
	g_video_ctrl->layer_para.alpha_val = 0xff;
	g_video_ctrl->layer_para.pipe = 1;
	g_video_ctrl->layer_para.src_win.x = 0;
	g_video_ctrl->layer_para.src_win.y = 0;
	g_video_ctrl->layer_para.src_win.width = g_video_ctrl->layer_para.fb.size.width;
	g_video_ctrl->layer_para.src_win.height = g_video_ctrl->layer_para.fb.size.height;
	g_video_ctrl->layer_para.scn_win.x = 0;
	g_video_ctrl->layer_para.scn_win.y = 0;
	g_video_ctrl->layer_para.scn_win.width = g_video_ctrl->scn_width;
	g_video_ctrl->layer_para.scn_win.height = g_video_ctrl->scn_height;

	arg[0] = (unsigned long)g_video_ctrl->h_lyr;
	arg[1] = (unsigned long)&g_video_ctrl->layer_para;
	arg[2] = 0;
	//esMODS_MIoctrl(g_video_ctrl->h_disp,MOD_DISP_CMD_LAYER_SET_PARA,0,(void*)arg);
	return EPDK_OK;
}

static int32_t video_display_layer_release(void)
{
	if (g_video_ctrl->h_lyr)
	{
		unsigned long arg[3];
		arg[0] = g_video_ctrl->h_lyr;
		arg[1] = 0;
		arg[2] = 0;
		esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_LAYER_RELEASE, 0, (void*)arg);
	}
	g_video_ctrl->h_lyr = 0;
	if (g_video_ctrl->disp_buffer)
	{
		esMEMS_Pfree(g_video_ctrl->disp_buffer, (FRAME_PAL_SIZE + 1023) >> 10);
		g_video_ctrl->disp_buffer = NULL;
	}
	if (g_video_ctrl->temp_buffer)
	{
		esMEMS_Pfree(g_video_ctrl->temp_buffer, (FRAME_PAL_SIZE + 1023) >> 10);
		g_video_ctrl->temp_buffer = NULL;
	}
	return EPDK_OK;
}

static int32_t video_display_layer_config(__disp_layer_info_t* info)
{
	unsigned long arg[3];
	arg[0] = (unsigned long)g_video_ctrl->h_lyr;
	arg[1] = (unsigned long)info;
	arg[2] = 0;
	esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_CMD_LAYER_SET_PARA, 0, (void*)arg);

	// static int cnt = 0;
	// if(cnt ==0)
	// {
	// 	cnt ++;
	// 	// struct timeval tv1, tv2;
	// 	// int tv;
	// 	// gettimeofday(&tv1, NULL);
	// 	// tv = (int)((tv1.tv_sec) * 1000000 +  tv1.tv_usec) / 1000;
	// 	// __err("time1 disp= %d ms",tv);
	// }
	return EPDK_OK;
}

static int32_t video_display_layer_open(void)
{
	unsigned long arg[3];
	arg[0] = (unsigned long)g_video_ctrl->h_lyr;
	arg[1] = 0;
	arg[2] = 0;
	esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_CMD_LAYER_OPEN, 0, (void*)arg);
	return EPDK_OK;
}

static int32_t video_display_layer_close(void)
{
	unsigned long arg[3];
	arg[0] = (unsigned long)g_video_ctrl->h_lyr;
	arg[1] = 0;
	arg[2] = 0;
	esMODS_MIoctrl(g_video_ctrl->h_disp, MOD_DISP_CMD_LAYER_CLOSE, 0, (void*)arg);
	return EPDK_OK;
}

static __s32 video_hw_rotate(void* src, int src_width, int src_height, void* dst, __video_play_rotate_t rot_angle)
{
	g2d_blt_h   blit;
	int dst_width = 0, dst_height = 0;
	int ret;

	memset(&blit, 0, sizeof(g2d_blt_h));
	switch (rot_angle)
	{
	case VIDEO_PLAY_ROTATE_0:
		blit.flag_h = G2D_ROT_0;
		dst_width = src_width;
		dst_height = src_height;
		break;
	case VIDEO_PLAY_ROTATE_90:
		blit.flag_h = G2D_ROT_90;
		dst_width = src_height;
		dst_height = src_width;
		break;
	case VIDEO_PLAY_ROTATE_180:
		blit.flag_h = G2D_ROT_180;
		dst_width = src_width;
		dst_height = src_height;
		break;
	case VIDEO_PLAY_ROTATE_270:
		blit.flag_h = G2D_ROT_270;
		dst_width = src_height;
		dst_height = src_width;
		break;
	case VIDEO_PLAY_ROTATE_HFLIP:
		blit.flag_h = G2D_ROT_H;
		dst_width = src_width;
		dst_height = src_height;
		break;
	case VIDEO_PLAY_ROTATE_VFLIP:
		blit.flag_h = G2D_ROT_V;
		dst_width = src_width;
		dst_height = src_height;
		break;
	default:
		__wrn("fatal error! rot_angle[%d] is invalid!\n", rot_angle);
		blit.flag_h = G2D_BLT_NONE_H;
		break;
	}

	blit.src_image_h.bbuff = 1;
	blit.src_image_h.use_phy_addr = 1;
	blit.src_image_h.color = 0xff;
	blit.src_image_h.format = G2D_FORMAT_YUV420UVC_U1V1U0V0;//g2d_fmt_enh//yuv420sp uv combine
	blit.src_image_h.laddr[0] = (unsigned int)esMEMS_VA2PA((unsigned long)src);
	blit.src_image_h.laddr[1] = (unsigned int)esMEMS_VA2PA((unsigned long)src + src_width * src_height);
	blit.src_image_h.laddr[2] = 0;//esMEMS_VA2PA((uint32_t)src+src_width * src_height * 5 / 4);
	blit.src_image_h.width = src_width;
	blit.src_image_h.height = src_height;
	blit.src_image_h.align[0] = 0;
	blit.src_image_h.align[1] = 0;
	blit.src_image_h.align[2] = 0;
	blit.src_image_h.clip_rect.x = 0;
	blit.src_image_h.clip_rect.y = 0;
	blit.src_image_h.clip_rect.w = src_width;
	blit.src_image_h.clip_rect.h = src_height;
	blit.src_image_h.gamut = G2D_BT709;
	blit.src_image_h.bpremul = 0;
	blit.src_image_h.alpha = 0xff;
	blit.src_image_h.mode = G2D_GLOBAL_ALPHA;

	blit.dst_image_h.bbuff = 1;
	blit.dst_image_h.use_phy_addr = 1;
	blit.dst_image_h.color = 0xff;
	blit.dst_image_h.format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
	blit.dst_image_h.laddr[0] = (unsigned int)esMEMS_VA2PA((unsigned long)dst);
	blit.dst_image_h.laddr[1] = (unsigned int)esMEMS_VA2PA((unsigned long)dst + dst_width * dst_height);
	blit.dst_image_h.laddr[2] = 0;//esMEMS_VA2PA((uint32_t)dst+src_width * dst_height * 5 / 4);
	blit.dst_image_h.width = dst_width;
	blit.dst_image_h.height = dst_height;
	blit.dst_image_h.align[0] = 0;
	blit.dst_image_h.align[1] = 0;
	blit.dst_image_h.align[2] = 0;
	blit.dst_image_h.clip_rect.x = 0;
	blit.dst_image_h.clip_rect.y = 0;
	blit.dst_image_h.clip_rect.w = dst_width;
	blit.dst_image_h.clip_rect.h = dst_height;
	blit.dst_image_h.gamut = G2D_BT709;
	blit.dst_image_h.bpremul = 0;
	blit.dst_image_h.alpha = 0xff;
	blit.dst_image_h.mode = G2D_GLOBAL_ALPHA;

	ret = ioctl(g_video_ctrl->h_rotate, G2D_CMD_BITBLT_H, (unsigned long)&blit);

	eLIBs_CleanFlushDCacheRegion(dst, dst_width * dst_height * 3 / 2);
	return ret;
}

#define TVD_STATUS_REG 0x05c01090
static void video_show_task(void* arg)
{
	//int change_flag = 1;
	//int hah = 0;
	__u32 reg_val;
#if 0
	static __u32 tvd_lock = 0;
	static __u32 nTvdStableCnt = 0;
#endif

	volatile __u32			tvd_lock_stable = 0;
	while (1)
	{
		if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
		{
			__log("delete video_show_task!");
			goto _img_show_task_exit;
		}

#if 0
		reg_val = *((volatile unsigned int*)TVD_STATUS_REG);

		if (reg_val & (1 << 0)) // lock
		{
			__log("%s int reg_val=%x", "lock", reg_val);
			*((volatile __u32*)TVD_STATUS_REG) = reg_val;
			tvd_lock = 1;
			//continue;
		}
		else if (reg_val & (1 << 1)) // unlock
		{
			__log("%s int reg_val=%x", "unlock", reg_val);
			*((volatile __u32*)TVD_STATUS_REG) = reg_val;
			tvd_lock = 0;
			nTvdStableCnt = 0;
			tvd_lock_stable = 0;
			//esKSRV_SendMsg(KMSG_USR_SYSTEM_TVD_PLUGOUT, KMSG_PRIO_LOW);
			continue;
		}

		// 待制式稳定之后，才采集数据
		if (tvd_lock == 1)
		{
			nTvdStableCnt = (nTvdStableCnt >= 10) ? 10 : (nTvdStableCnt + 1);

			if (nTvdStableCnt >= 10)
			{
				// __log("tvd system stable...");
				tvd_lock_stable = 1;
			}
			else if (nTvdStableCnt == 9)
			{
				// __log("tvd system stable1...");
				tvd_lock_stable = 1;
				//esKSRV_SendMsg(KMSG_USR_SYSTEM_TVD_PLUGIN, KMSG_PRIO_LOW);
			}
			else
			{
				tvd_lock_stable = 0;
			}
		}
#endif
		if (v4l2_update_frame((unsigned long*)&g_video_ctrl->temp_buffer) == EPDK_OK)
		{
			//hah = (hah>10)?20:(hah+1);
			if (1)//hah>10)
			{
				video_hw_rotate((void*)g_video_ctrl->temp_buffer, g_video_ctrl->frame_width, g_video_ctrl->frame_height, g_video_ctrl->disp_buffer, g_video_ctrl->rotate_angle);

				//memcpy(g_video_ctrl->disp_buffer,temp_addr,720*576*3/2);
				esKSRV_memset((void*)g_video_ctrl->temp_buffer, 0, (FRAME_PAL_SIZE + 1023) >> 10);

				if ((g_video_ctrl->rotate_angle == VIDEO_PLAY_ROTATE_90) || (g_video_ctrl->rotate_angle == VIDEO_PLAY_ROTATE_270))
				{
					g_video_ctrl->layer_para.fb.size.width = g_video_ctrl->frame_height;
					g_video_ctrl->layer_para.fb.size.height = g_video_ctrl->frame_width;
					g_video_ctrl->layer_para.src_win.width = g_video_ctrl->frame_height;
					g_video_ctrl->layer_para.src_win.height = g_video_ctrl->frame_width;
					g_video_ctrl->layer_para.scn_win.width = g_video_ctrl->scn_height;
					g_video_ctrl->layer_para.scn_win.height = g_video_ctrl->scn_width;
				}
				else
				{
					g_video_ctrl->layer_para.fb.size.width = g_video_ctrl->frame_width;
					g_video_ctrl->layer_para.fb.size.height = g_video_ctrl->frame_height;
					g_video_ctrl->layer_para.src_win.width = g_video_ctrl->frame_width;
					g_video_ctrl->layer_para.src_win.height = g_video_ctrl->frame_height;
					g_video_ctrl->layer_para.scn_win.width = g_video_ctrl->scn_width;
					g_video_ctrl->layer_para.scn_win.height = g_video_ctrl->scn_height;
				}
				//__log("set config!");
				video_display_layer_config(&g_video_ctrl->layer_para);
			}
		}
		else
		{
			esKRNL_TimeDly(1);
		}
	}
_img_show_task_exit:

	esKRNL_TDel(EXEC_prioself);
}


static void video_check_task(void* arg)
{
	int std_new = -1;//0:ntsc 1 pal 用于获取新的制式
	u32 format_new = 0; //用于获取新的视频格式  YUV420_P YUV420_UVUV_SP...

	int change_cnt = 0;
	g_video_ctrl->system = -1;
	__u32 reg_val;
#if 0
	static __u32 tvd_lock = 0;
	static __u32 nTvdStableCnt = 0;
#endif
	volatile __u32			tvd_lock_stable = 1;

	while (1)
	{
		if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
		{
			__log("delete video_check task!");
			goto _video_check_task_exit;
		}
#if 0
		reg_val = *((volatile unsigned int*)TVD_STATUS_REG);

		if (reg_val & (1 << 0)) // lock
		{
			__log("%s int reg_val=%x", "lock", reg_val);
			*((volatile __u32*)TVD_STATUS_REG) = reg_val;
			tvd_lock = 1;
			//continue;
		}
		else if (reg_val & (1 << 1)) // unlock
		{
			__log("%s int reg_val=%x", "unlock", reg_val);
			*((volatile __u32*)TVD_STATUS_REG) = reg_val;
			tvd_lock = 0;
			nTvdStableCnt = 0;
			tvd_lock_stable = 0;
			//esKSRV_SendMsg(KMSG_USR_SYSTEM_TVD_PLUGOUT, KMSG_PRIO_LOW);
			continue;
		}

		// 待制式稳定之后，才采集数据
		if (tvd_lock == 1)
		{
			nTvdStableCnt = (nTvdStableCnt >= 10) ? 10 : (nTvdStableCnt + 1);

			if (nTvdStableCnt >= 10)
			{
				// __log("tvd system stable...");
				tvd_lock_stable = 1;
			}
			else if (nTvdStableCnt == 9)
			{
				// __log("tvd system stable1...");
				tvd_lock_stable = 1;
				//esKSRV_SendMsg(KMSG_USR_SYSTEM_TVD_PLUGIN, KMSG_PRIO_LOW);
			}
			else
			{
				tvd_lock_stable = 0;
			}
		}
#endif
		if (tvd_lock_stable == 1 && !v4l2_get_system(&std_new, &format_new))
		{
			if (std_new != g_video_ctrl->system)
			{
				change_cnt = (change_cnt >= CHECK_FRAME_NUM) ? (CHECK_FRAME_NUM) : change_cnt + 1;
			}
			else
			{
				change_cnt = 0;
			}

			if (change_cnt >= CHECK_FRAME_NUM)
			{
				if (std_new == FORMAT_NTSC)
				{
					g_video_ctrl->frame_width = NTSC_WIDTH;
					g_video_ctrl->frame_height = NTSC_HEIGHT;
					__log("switch to NTSC!!");
				}
				else
				{
					g_video_ctrl->frame_width = PAL_WIDTH;
					g_video_ctrl->frame_height = PAL_HEIGHT;
					__log("switch to PAL!!");
				}

				g_video_ctrl->system = std_new;

				//continue;
				g_video_ctrl->layer_para.fb.addr[1] = g_video_ctrl->layer_para.fb.addr[0] + g_video_ctrl->frame_width * g_video_ctrl->frame_height;
				g_video_ctrl->layer_para.fb.addr[2] = 0;//g_video_ctrl->layer_para.fb.addr[0] + width * height * 5 / 4;
				g_video_ctrl->layer_para.fb.size.width = g_video_ctrl->frame_width;
				g_video_ctrl->layer_para.fb.size.height = g_video_ctrl->frame_height;
				g_video_ctrl->layer_para.src_win.width = g_video_ctrl->frame_width;
				g_video_ctrl->layer_para.src_win.height = g_video_ctrl->frame_height;

				if (g_video_ctrl->t_show_task)
				{
					int i = 0;
					do
					{
						esKRNL_TimeDlyResume(g_video_ctrl->t_show_task);
						esKRNL_TimeDly(1);
						++i;
						printf("i = %d\n", i);
						if (i == 100)
						{
							__err("wait viedo_show task exit failed, kill it!!");
							esKRNL_TDel(g_video_ctrl->t_show_task);
							g_video_ctrl->t_show_task = -1;
							break;/////////////////////////////////
						}
					} while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(g_video_ctrl->t_show_task));
				}
				v4l2_stop();
				v4l2_exit();

				v4l2_init(g_video_ctrl->channel);
				v4l2_start();
				__log("create  video_show_task !!!!!");
				g_video_ctrl->t_show_task = esKRNL_TCreate(video_show_task, NULL, 0x2000, KRNL_priolevel3);
			}
			else
			{
				esKRNL_TimeDly(1);
			}
		}
		else
		{
			esKRNL_TimeDly(1);
		}
	}
_video_check_task_exit:
	esKRNL_TDel(EXEC_prioself);
}

int video_dev_init(void)
{
	if (g_video_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	g_video_ctrl->h_disp = esKSRV_Get_Display_Hld();
	if (g_video_ctrl->h_disp == NULL)
	{
		__err("can't get disp handle");
		goto open_err;
	}


	g_video_ctrl->h_rotate = open("/dev/g2d", O_WRONLY);
	if (g_video_ctrl->h_rotate < 0)
	{
		__err("can't open g2d dev");
		goto open_err;
	}

	g_video_ctrl->h_video_dev = open("/dev/video4", O_RDWR | O_NONBLOCK, 0);
	if (g_video_ctrl->h_video_dev < 0)
	{
		__err("can't open video dev");
		goto open_err;
	}

	return EPDK_OK;

open_err:

	if (g_video_ctrl->h_video_dev > 0)
	{
		close(g_video_ctrl->h_video_dev);
		g_video_ctrl->h_video_dev = -1;
	}

	if (g_video_ctrl->h_rotate > 0)
	{
		close(g_video_ctrl->h_rotate);
		g_video_ctrl->h_rotate = -1;
	}
	return EPDK_FAIL;
}

int video_dev_exit(void)
{
	if (g_video_ctrl == NULL)
	{
		return EPDK_FAIL;
	}

	if (g_video_ctrl->h_video_dev > 0)
	{
		__err("langaojie fd = %d", g_video_ctrl->h_video_dev);
		close(g_video_ctrl->h_video_dev);
		g_video_ctrl->h_video_dev = -1;
	}

	if (g_video_ctrl->h_rotate > 0)
	{
		close(g_video_ctrl->h_rotate);
		g_video_ctrl->h_rotate = -1;
	}
	return EPDK_OK;
}

int video_start_show(void)
{
	if(g_has_started)
		return EPDK_OK;
	video_display_layer_request();

	g_video_ctrl->t_check_task = esKRNL_TCreate(video_check_task, NULL, 0x2000, KRNL_priolevel2);
	if(g_video_ctrl->t_check_task  == 0)
		return EPDK_FAIL;

	g_has_started = 1;
	return EPDK_OK;
}

int video_stop_show(void)
{
	if(!g_has_started)
		return EPDK_OK;
	if (g_video_ctrl->t_check_task)
	{
		int i = 0;
		do
		{
			esKRNL_TimeDlyResume(g_video_ctrl->t_check_task);
			esKRNL_TimeDly(1);
			++i;
			if (i == 100)
			{
				__err("wait check task exit failed, kill it!!");
				esKRNL_TDel(g_video_ctrl->t_check_task);
				g_video_ctrl->t_check_task = -1;
				break;
			}
		} while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(g_video_ctrl->t_check_task));
	}

	if (g_video_ctrl->t_show_task)
	{
		int i = 0;
		do
		{
			esKRNL_TimeDlyResume(g_video_ctrl->t_show_task);
			esKRNL_TimeDly(1);
			++i;
			if (i == 100)
			{
				__err("wait viedo_show task exit failed, kill it!!");
				esKRNL_TDel(g_video_ctrl->t_show_task);
				g_video_ctrl->t_show_task = -1;
				break;
			}
		} while (OS_TASK_NOT_EXIST != esKRNL_TDelReq(g_video_ctrl->t_show_task));
	}

	v4l2_stop();
	v4l2_exit();
	video_display_layer_release();
	g_has_started = 0;
	return EPDK_OK;
}




