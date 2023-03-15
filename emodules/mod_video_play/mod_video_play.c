#include "video_dev.h"
#include "video_v4l2.h"
#include <sys/time.h>
#include "dfs_posix.h"

/*
*********************************************************************************************************
*                                     MInit
*
* Description: module init function,
*
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*               esle return EPDK_FAIL
*********************************************************************************************************
*/
__s32 Video_Play_MInit(void)
{
	g_video_ctrl = malloc(sizeof(__video_ctrl_t));
	if (g_video_ctrl == NULL) {
		__err("video_play.mod Minit err!");
		return EPDK_FAIL;
	}
	memset(g_video_ctrl, 0, sizeof(__video_ctrl_t));
	return EPDK_OK;
}
/*
*********************************************************************************************************
*                                     MExit
*
* Description: module exit function,
*
* Arguments  : void
*
* Returns    : if success return EPDK_OK
*				esle return EPDK_FAIL
*
*********************************************************************************************************
*/
__s32 Video_Play_MExit(void)
{
	if (g_video_ctrl != NULL) {
		free(g_video_ctrl);
		g_video_ctrl = NULL;
	}
	return EPDK_OK;
}
/*
*********************************************************************************************************
*                                     MOpen
*
* Description: module open function
*
* Arguments  : 	mid: ID number returned during installation
				mod：0
*
* Returns    : module handle
*
*********************************************************************************************************
*/
__mp *Video_Play_MOpen(__u32 mid, __u32 mode)
{
	int32_t ret = EPDK_FAIL;

	g_video_ctrl->mid = mid;
	ret = video_dev_init();

	return (__mp *)g_video_ctrl;
}

/*
*********************************************************************************************************
*                                     MClose
*
* Description: Close the module
*
* Arguments  : mp module handle
*
* Returns    : if success return EPDK_OK
*               esle return EPDK_FAIL
*********************************************************************************************************
*/
__s32 Video_Play_MClose(__mp *mp)
{
	video_dev_exit();
	__log("Video_Play_MClose");
	return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     vply_MRead
*
* Description: do nothing
*
* Arguments  : none
*
* Returns    : n
*
*********************************************************************************************************
*/
__u32 Video_Play_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
	return n * size;
}

/*
*********************************************************************************************************
*                                     vply_MWrite
*
* Description: do nothing
*
* Arguments  : none
*
* Returns    : n
*
*********************************************************************************************************
*/
__u32 Video_Play_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
	return n * size;
}

/*
*********************************************************************************************************
*                                     MIoctrl
*
* Description: module control
*
* Arguments  : mp：         module handle
*              cmd： VIDEO_PLAY_START、VIDEO_PLAY_STOP、VIDEO_PLAY_SET_ROTATE_ANGLE...
*              aux:
*              pbuffer:
*
* Returns    : ?
*
*********************************************************************************************************
*/
long Video_Play_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
	long retVal = EPDK_OK;
	switch (cmd) {
	case VIDEO_PLAY_START: {
		retVal = video_start_show();
		break;
	}

	case VIDEO_PLAY_STOP: {
		retVal = video_stop_show();
		break;
	}

	case VIDEO_PLAY_SET_ROTATE_ANGLE: {
		g_video_ctrl->rotate_angle = (__video_play_rotate_t)(aux);
		break;
	}

	case VIDEO_PLAY_GET_ROTATE_ANGLE: {
		retVal = g_video_ctrl->rotate_angle;
		break;
	}

	case VIDEO_PLAY_SET_CHANNEL: {
		retVal = v4l2_set_channel(aux);
		if (retVal == EPDK_OK) {
			g_video_ctrl->channel = aux;
		}
		break;
	}
	case VIDEO_PLAY_GET_CHANNEL: {
		retVal = g_video_ctrl->channel;
		break;
	}

	case VIDEO_PLAY_GET_SYSTEM: {
		retVal = g_video_ctrl->system;
		break;
	}

	case VIDEO_PLAY_GET_SIGNALE_DECT: {
		int system;
		u32 format;
		retVal = v4l2_get_system(&system, &format);
		break;
	}

	case VIDEO_PLAY_SET_SCREEN_RECT: {
		g_video_ctrl->layer_para.scn_win = *(__disp_rect_t *)pbuffer;
		break;
	}

	case VIDEO_PLAY_GET_SCREEN_RECT: {
		retVal = (long)&g_video_ctrl->layer_para.scn_win;
		break;
	}

	case VIDEO_PLAY_SET_SOURCE_RECT: {
		g_video_ctrl->layer_para.src_win = *(__disp_rect_t *)pbuffer;
		break;
	}

	case VIDEO_PLAY_GET_SOURCE_RECT: {
		retVal = (long)&g_video_ctrl->layer_para.src_win;
		break;
	}

	case VIDEO_PLAY_SET_CONTRAST: {
		retVal = v4l2_set_contrast(aux);
		break;
	}

	case VIDEO_PLAY_GET_CONTRAST: {
		retVal = v4l2_get_contrast();
		break;
	}
	case VIDEO_PLAY_SET_BRIGHTNESS: {
		retVal = v4l2_set_brightness(aux);
		break;
	}

	case VIDEO_PLAY_GET_BRIGHTNESS: {
		retVal = v4l2_get_brightness();
		break;
	}

	case VIDEO_PLAY_SET_SATURATION: {
		retVal = v4l2_set_saturation(aux);
		break;
	}

	case VIDEO_PLAY_GET_SATURATION: {
		retVal = v4l2_get_saturation();
		break;
	}

	case VIDEO_PLAY_SET_SHARPNESS: {
		retVal = v4l2_set_sharpness(aux);
		break;
	}

	case VIDEO_PLAY_GET_SHARPNESS: {
		retVal = v4l2_get_sharpness();
		break;
	}

	case VIDEO_PLAY_SET_HUE: {
		retVal = v4l2_set_hue(aux);
		break;
	}

	case VIDEO_PLAY_GET_HUE: {
		retVal = v4l2_get_hue();
		break;
	}

	default: {
		retVal = EPDK_FAIL;
		break;
	}

	}
	return retVal;
}
