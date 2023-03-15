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
#include "dsk_radio_i.h"
#include <mod_twi.h>
#include <kconfig.h>

dsk_radio_t *h_radio = NULL;
static __s32 iic_write_bytes(ES_FILE *fiic, __s32 slave_addr, __u8  *addr, __s32 addr_len, __u8 *data, __s32 data_len);
static __s32 iic_read_bytes(ES_FILE *fiic, __s32 slave_addr, __u8  *addr, __s32 addr_len, __u8 *data, __s32 data_len);

//mode:0 32khz, 1:1024hz, 2: 32hz, 3:1hz
__s32 dsk_misc_open_rtc_clk(__s32 mode);
__s32 dsk_misc_close_rtc_clk(void);

/*
**********************************************************************************************************************
*                                               dsk_radio_open
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:      dsk_radio 打开函数(初始化FM播放功能)
*
* parameters:
*
* return:           if success return h_radio handle
*                   if fail return NULL
* modify history:
**********************************************************************************************************************
*/
__s32 dsk_radio_open(void)
{
    if (h_radio)
    {
        return EPDK_OK;
    }

    h_radio = (dsk_radio_t *)esMEMS_Malloc(0, sizeof(dsk_radio_t));

    if (h_radio == NULL)
    {
        __wrn("Error: malloc h_radio failed!");
        return EPDK_FAIL;
    }

    eLIBs_memset(h_radio, 0, sizeof(dsk_radio_t));
    //dsk_misc_open_rtc_clk(0);
    //esKRNL_TimeDly(50);
    /*加载FM驱动*/
    esDEV_Plugin("\\drv\\fm.drv", 0, 0, 1);
    /*打开FM驱动*/
	h_radio->fm_drv = open("/dev/fm",O_RDWR);

    if (h_radio->fm_drv < 0)
    {
        __wrn(" Error: open fm driver failed! ");
        return EPDK_FAIL;
    }

    h_radio->drv_status = ioctl(h_radio->fm_drv, DRV_FM_CMD_INIT,0);
    return EPDK_OK;
}

__bool dsk_radio_is_open(void)
{
    if (h_radio)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

__pCBK_t dsk_radio_set_cb(dsk_radio_event_e event, __pCBK_t cb, void *ctx)
{
    __pCBK_t  cb_old = NULL;

    switch (event)
    {
        case DSK_RADIO_EVENT_SEARCH_SUCCESS:
        {
            cb_old = cb_search_success;
            cb_search_success = cb;
            cb_ctx_search_success = ctx;
            break;
        }

        case DSK_RADIO_EVENT_SEARCH_FAIL:
        {
            cb_old = cb_search_fail;
            cb_search_fail = cb;
            cb_ctx_search_fail = ctx;
            break;
        }

        case DSK_RADIO_EVENT_SEARCH_OVER:
        {
            cb_old = cb_search_over;
            cb_search_over = cb;
            cb_ctx_search_over = ctx;
            break;
        }

        case DSK_RADIO_EVENT_SEARCH_FREQ:           //显示内部搜索过程中的频率
        {
            //cb_old = cb_search_freq;
            //cb_search_freq = cb;
            //cb_ctx_search_freq = ctx;
            break;
        }

        default :
        {
            cb_old = NULL;
        }
    }

    return cb_old;
}

__pCBK_t dsk_radio_get_cb(dsk_radio_event_e event)
{
    __pCBK_t   cb;

    switch (event)
    {
        case DSK_RADIO_EVENT_SEARCH_SUCCESS:
        {
            cb = cb_search_success;
            break;
        }

        default :
        {
            cb = NULL;
        }
    }

    return cb;
}
__s32 dsk_radio_get_vol(void)
{
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }
    //return esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_VOL_GET, 0, 0);
    
	unsigned long arg[4] = {0,0,0,0};
	unsigned long vol =  0;
    arg[2] = &vol;
    ioctl(h_radio->fm_drv, DRV_FM_CMD_VOL_GET, arg);
    return vol;
}

__s32 dsk_radio_set_vol(__u8 vol)
{
    __u64 arg[4] = {0,0,0,0};
    if(h_radio == NULL)
    {
        __err("radio not open!\n");
        return EPDK_FAIL;
    }
    //return esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_VOL_SET, vol, 0);
    arg[0] = vol;
    arg[1] = 0;
    return ioctl(h_radio->fm_drv, DRV_FM_CMD_VOL_SET,(void*)arg);
}

__s32 dsk_radio_mute(void)
{
    unsigned long arg[4] = {0,0,0,0};
    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    //return esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_OFF, 0);
    arg[0] = DRV_FM_VOICE_OFF;
    arg[1] = 0;
    return ioctl(h_radio->fm_drv, DRV_FM_CMD_MUTE,(void*)arg);
}

__s32 dsk_radio_unmute(void)
{
	unsigned long arg[4] = {0,0,0,0};
	if (h_radio == NULL)
	{
		__err("radio not open!");
		return EPDK_FAIL;
	}

	//return esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_ON, 0);
	arg[0] = DRV_FM_VOICE_ON;
	arg[1] = 0;
	return ioctl(h_radio->fm_drv, DRV_FM_CMD_MUTE,(void*)arg);
}

__s32 dsk_radio_set_band(dsk_radio_band_mode_t band_mode)
{
    __s32 result = 0;
	unsigned long arg[4] = {0,0,0,0};
	
    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    if (band_mode == DSK_RADIO_BAND_JAPAN && h_radio->cur_band != DSK_RADIO_BAND_JAPAN)
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DVR_FM_AREA_JAPAN, &h_radio->freq_range);
        arg[0] = DVR_FM_AREA_JAPAN;
        arg[1] = (unsigned long)&h_radio->freq_range;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, (void*)arg);
        h_radio->cur_band = DSK_RADIO_BAND_JAPAN;
    }
    else if (band_mode == DSK_RADIO_BAND_SCHOOL && h_radio->cur_band != DSK_RADIO_BAND_SCHOOL)
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_SCHOOL, &h_radio->freq_range);
        arg[0] = DRV_FM_AREA_CHN_SCHOOL;
        arg[1] = (unsigned long)&h_radio->freq_range;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, (void*)arg);
        h_radio->cur_band = DSK_RADIO_BAND_SCHOOL;
    }
    else
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_US_EUP, &h_radio->freq_range);
    	arg[0] = DRV_FM_AREA_CHN_US_EUP;
        arg[1] = (unsigned long)&h_radio->freq_range;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, (void*)arg);
        h_radio->cur_band = DSK_RADIO_BAND_US_EUP;
    }

    return result;
}

__s32 dsk_radio_set_threshold(dsk_radio_threshold_mode_t threshold)
{
    __s32 result = 0;
	 unsigned long arg[4] = {0,0,0,0};

    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    if (threshold == DSK_RADIO_THRESHOLD_HIGH)
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_HIGH, 0);
		arg[0] = DRV_FM_SLEVEL_HIGH;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET,(void*)arg);
    }
    else if (threshold == DSK_RADIO_THRESHOLD_LOW)
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_LOW, 0);
        arg[0] = DRV_FM_SLEVEL_HIGH;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET,(void*)arg);
    }
    else
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_NORMAL, 0);
        arg[0] = DRV_FM_SLEVEL_NORMAL;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET,(void*)arg);
    }

    return result;
}

__s32 dsk_radio_set_audio_mode(dsk_radio_audio_mode_t audio_mode)
{
    __s32 result = 0;
	unsigned long arg[4] = {0,0,0,0};

    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    if (audio_mode == DSK_RADIO_AUDIO_MONO)
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_MONO, 0);
        arg[0] = DRV_FM_MONO;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET,(void*)arg);
        h_radio->audio_mode = DSK_RADIO_AUDIO_MONO;
    }
    else
    {
        //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_STEREO, 0);
        arg[0] = DRV_FM_STEREO;
        result = ioctl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET,(void*)arg);
        h_radio->audio_mode = DSK_RADIO_AUDIO_STEREO;
    }

    return result;
}

__s32 dsk_radio_get_band(__s32 *cur_band)
{
    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    *cur_band = h_radio->cur_band;
    return EPDK_OK;
}

__s32 dsk_radio_get_audio_mode(__s32 *audio_mode)
{
    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    *audio_mode = h_radio->audio_mode;
    return EPDK_OK;
}

__s32 dsk_radio_get_min_freq(__s32 *min_freq)
{
    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    *min_freq = h_radio->freq_range.fm_area_min_freq;
    return EPDK_OK;
}

__s32 dsk_radio_get_max_freq(__s32 *max_freq)
{
    if (h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    *max_freq = h_radio->freq_range.fm_area_max_freq;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               dsk_radio_control
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:      dsk_radio功能操作接口,参数1一般为输入参数，参数2为输出参数
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
/*__s32 dsk_radio_control(__s32 cmd,__s32 param1,__s32 param2)
{
    __s32 result = 0;

    if(h_radio == NULL)
    {
        __err("radio not open!");
        return EPDK_FAIL;
    }

    switch(cmd)
    {
        case DSK_RADIO_MUTE:
        {
            result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_OFF, 0);
            break;
        }
        case DSK_RADIO_UNMUTE:
        {
            result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_MUTE, DRV_FM_VOICE_ON, 0);
            break;
        }
        case DSK_RADIO_SET_BAND:
        {
            if(param1 == DSK_RADIO_BAND_JAPAN && h_radio->cur_band != DSK_RADIO_BAND_JAPAN)
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DVR_FM_AREA_JAPAN, &h_radio->freq_range);
                h_radio->cur_band = DSK_RADIO_BAND_JAPAN;
            }
            else if(param1 == DSK_RADIO_BAND_SCHOOL && h_radio->cur_band != DSK_RADIO_BAND_SCHOOL)
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_SCHOOL, &h_radio->freq_range);
                h_radio->cur_band = DSK_RADIO_BAND_SCHOOL;
            }
            else
            {

                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_AREA_SET, DRV_FM_AREA_CHN_US_EUP, &h_radio->freq_range);

                h_radio->cur_band = DSK_RADIO_BAND_US_EUP;

            }
            break;
        }
        case DSK_RADIO_SET_HTRESHOLD:
        {
            if(param1 == DSK_RADIO_THRESHOLD_HIGH)
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_HIGH, 0);
            }
            else if(param1 == DSK_RADIO_THRESHOLD_LOW)
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_LOW, 0);
            }
            else
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SLEVEL_SET, DRV_FM_SLEVEL_NORMAL, 0);
            }
            break;
        }
        case DSK_RADIO_SET_AUDIO_MODE:
        {
            if(param1 == DSK_RADIO_AUDIO_MONO)
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_MONO, 0);
                h_radio->audio_mode = DSK_RADIO_AUDIO_MONO;
            }
            else
            {
                result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_STEREO_SET, DRV_FM_STEREO, 0);
                h_radio->audio_mode = DSK_RADIO_AUDIO_STEREO;

            }
            break;
        }
        case DSK_RADIO_GET_BAND:
        {
            __s32 *band = (__s32 *)param2;
            *band = h_radio->cur_band;
            break;
        }
        case DSK_RADIO_GET_FREQ:
        {
            __u32 freq;

//          __u32 *ret = (__u32 *)param2;

            if(h_radio->drv_status == FM_DRV_STATUS_SEND)
            {
                dsk_radio_send_get_cur_freq(&freq);
            }
            else
            {

                dsk_radio_rcv_get_cur_freq(&freq);

            }
            *((__u32 *)param2) = freq;
            break;
        }
        case DSK_RADIO_GET_AUDIO_MODE:
        {
            __u32 *track = (__u32 *)param2;
            *track = h_radio->audio_mode;

            break;
        }
        case DSK_RADIO_GET_MIN_FREQ:
        {
            __s32 *min_freq = (__s32 *)param2;
            *min_freq = h_radio->freq_range.fm_area_min_freq;
            break;
        }
        case DSK_RADIO_GET_MAX_FREQ:
        {
            __s32 *max_freq = (__s32 *)param2;
            *max_freq = h_radio->freq_range.fm_area_max_freq;
            break;
        }
        case DSK_RADIO_REC_OPEN:
        {
            if(h_radio->drv_status == FM_DRV_STATUS_SEND)
            {
                dsk_radio_send_close();
                h_radio->rcv_handle = dsk_radio_rcv_open();
                if(h_radio->rcv_handle == NULL)
                {
                    result = -1;
                }
            }
            else
            {

                h_radio->rcv_handle = dsk_radio_rcv_open();
                if(h_radio->rcv_handle == NULL)
                {
                    result = -1;
                }
            }
            h_radio->drv_status = FM_DRV_STATUS_RECEIVE;
            break;
        }
        case DSK_RADIO_REC_SET_FREQ_PLAY:
        {

            result = dsk_radio_rcv_set_freq_play(param1);
            if(result > 0)
            {
                h_radio->cur_freq = result;
                result = 0;
            }

            break;
        }
        case DSK_RADIO_REC_NEXT_FREQ_PLAY:
        {
            result = dsk_radio_rcv_next_freq_play();
            break;
        }
        case DSK_RADIO_REC_PRE_FREQ_PLAY:
        {
            result = dsk_radio_rcv_pre_freq_play();
            break;
        }
        case DSK_RADIO_REC_SET_AUTO_SEARCH_MODE:
        {
            result = dsk_radio_rcv_set_search_mode(param1);
            break;
        }
        case DSK_RADIO_REC_AUTO_SEARCH:
        {
            result = dsk_radio_rcv_auto_search();
            if(result > 0)
            {
                h_radio->cur_freq = result;
                result = 0;
            }
            break;
        }
        case DSK_RADIO_REC_AUTO_SEARCH_ALL:
        {
            result = dsk_radio_rcv_search_all();
            break;
        }

        case DSK_RADIO_REC_SET_PLAY_LIST_TYPE:
        {
            result = dsk_radio_rcv_set_play_list_type( (__play_list_type_e)param1);
            break;
        }

        case DSK_RADIO_REC_GET_PLAY_LIST_TYPE:
        {
            result = dsk_radio_rcv_get_play_list_type( (__s32 *)param1);
            break;
        }
        case DSK_RADIO_REC_GET_SEARCH_RESULT_VN:
        {
            result = dsk_radio_rcv_get_search_result_vn( (__s32 *)param1);
            break;
        }
        case DSK_RADIO_REC_GET_FAVORITE_VN:
        {
            result = dsk_radio_rcv_get_favorite_vn( (__s32 *)param1);
            break;
        }
        case DSK_RADIO_REC_GET_SEARCH_RESULT_CHAN:
        {
            result = dsk_radio_rcv_get_search_result_chaninfo( (__s32 )param1, (dsk_radio_chan_t *)param2);
            break;
        }
        case DSK_RADIO_REC_GET_FAVORITE_CHAN:
        {
            result = dsk_radio_rcv_get_favorite_chaninfo( (__s32 )param1, (dsk_radio_chan_t *)param2);
            break;
        }
        case DSK_RADIO_REC_GET_FAVORITE:
        {
            result = dsk_radio_rcv_get_favorite((char *)param1);
            break;
        }
        case DSK_RADIO_REC_SAVE_FAVORITE:
        {
            result = dsk_radio_rcv_save_favorite((char *)param1);
            break;
        }
        case DSK_RADIO_REC_GET_SEARCH_RESULT:
        {
            result = dsk_radio_rcv_get_search_result((char *)param1);
            break;
        }
        case DSK_RADIO_REC_SAVE_SEARCH_RESULT:
        {
            result = dsk_radio_rcv_save_search_result((char *)param1);
            break;
        }
        case DSK_RADIO_REC_ADD_CHAN_TO_FAVORITE:
        {
            result = dsk_radio_rcv_add_chan_to_favorite( (__s32 )param1, (dsk_radio_chan_t *)param2);
            break;
        }
        case DSK_RADIO_REC_DEL_CHAN_FROM_FAVORITE:
        {
            result = dsk_radio_rcv_del_chan_from_favorite( (__s32 )param1);
            break;
        }
        case DSK_RADIO_REC_CLOSE:
        {
            result = dsk_radio_rcv_close();
            h_radio->rcv_handle = NULL;
            break;
        }
        case DSK_RADIO_SEND_OPEN:
        {
            h_radio->drv_status = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_QUERY_STATUS, 0, 0);
            if(h_radio->drv_status == FM_DRV_STATUS_RECEIVE)
            {
                dsk_radio_rcv_close();
                h_radio->send_handle = dsk_radio_send_open();
                if(h_radio->send_handle == NULL)
                {
                    result = -1;
                }
            }
            else
            {
                h_radio->send_handle = dsk_radio_send_open();
                if(h_radio->send_handle == NULL)
                {
                    result = -1;
                }
            }
            h_radio->drv_status = FM_DRV_STATUS_SEND;
            break;
        }
        case DSK_RADIO_SEND_SET_FREQ_PLAY:
        {
            result = dsk_radio_send_set_freq_play(param1);
            break;
        }
        case DSK_RADIO_SEND_SET_GAIN:
        {
            result = dsk_radio_send_set_gain(param1);
            break;
        }
        case DSK_RADIO_SEND_GET_GAIN:
        {
            __u32 *gain = (__u32 *)param2;
            result = dsk_radio_send_get_gain(gain);
            break;
        }
        case DSK_RADIO_SEND_CLOSE:
        {
            result = dsk_radio_send_close();
            h_radio->send_handle = NULL;
            break;
        }
        default:
            break;
    }
    return result;
}*/

/*
**********************************************************************************************************************
*                                               dsk_radio_close
*
* author:        terry.zeng
*
* date:             2009-12-01
*
* Description:       关闭dsk_radio，释放相关资源
*
* parameters:
*
* return:           if success return 0
*                   if fail return -1
* modify history:
**********************************************************************************************************************
*/
__s32 dsk_radio_close(void)
{
    __s32 result = 0;

    if (h_radio == NULL)
    {
        __msg("radio not open!");
        return EPDK_FAIL;
    }

    if (!(h_radio->fm_drv < 0))
    {
        //if(h_radio->send_handle != NULL)
        {
            dsk_radio_send_close();
            //h_radio->send_handle = NULL;
        }
        //if(h_radio->rcv_handle != NULL)
        {
            dsk_radio_rcv_close();
            //h_radio->rcv_handle = NULL;
        }
        /*退出FM驱动*/
		result = ioctl(h_radio->fm_drv, DRV_FM_CMD_EXIT,0);
        /*关闭FM驱动*/
		result = close(h_radio->fm_drv);
    }

    /*卸载FM驱动*/
    esDEV_Plugout("\\drv\\fm.drv", 0);
    /*free fm_engine,so the memory can be used for other*/
    esMEMS_Mfree(0, h_radio);
    h_radio = NULL;
    //dsk_misc_close_rtc_clk();
    return result;
}
#if 0
static __s32 iic_write_bytes(ES_FILE *fiic, __s32 slave_addr, __u8  *addr, __s32 addr_len, __u8 *data, __s32 data_len)
{
    __s32 ret;
    __twi_dev_para_ex_t  piic;

    if (NULL == fiic)
    {
        __wrn("iic hdl is null...");
        return EPDK_FAIL;
    }

    {
        __s32 i;
        __wrn("slave_addr=0x%x", slave_addr);
        __inf("addr");

        for (i = 0 ; i < addr_len ; i++)
        {
            __inf("0x%x ", addr[i]);
        }

        __inf("");
        __inf("data");

        for (i = 0 ; i < data_len ; i++)
        {
            __inf("0x%x ", data[i]);
        }

        __inf("");
    }

    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = slave_addr;
    piic.byte_addr_width = addr_len;
    piic.byte_addr       = addr;
    piic.data            = data;
    piic.byte_count      = data_len;
    ret = esMODS_MIoctrl(fiic, TWI_WRITE_SPEC_RS, 0, &piic);
    return ret;
}

static __s32 iic_read_bytes(ES_FILE *fiic, __s32 slave_addr, __u8  *addr, __s32 addr_len, __u8 *data, __s32 data_len)
{
    __s32 ret;
    __twi_dev_para_ex_t  piic;

    if (NULL == fiic)
    {
        __wrn("iic hdl is null...");
        return EPDK_FAIL;
    }

    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = slave_addr;
    piic.byte_addr_width = addr_len;
    piic.byte_addr       = addr;
    piic.data            = data;
    piic.byte_count      = data_len;
    ret = esMODS_MIoctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);
    return ret;
}
#endif
//mode:0 32khz, 1:1024hz, 2: 32hz, 3:1hz
__s32 dsk_misc_open_rtc_clk(__s32 mode)
{
    __wrn("need fix");
    return EPDK_FAIL;
#if 0 /*langaojie temp test*/
    ES_FILE *pfile;
    pfile = eLIBs_fopen("b:\\HWSC\\RTC", "r+");

    if (NULL == pfile)
    {
        __wrn("open RTC fail...");
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(pfile, RTC_CMD_SETCLOCKOUT, 0x80, NULL);
    eLIBs_fclose(pfile);
    __wrn("dsk_misc_open_rtc_clk ok...");
    return EPDK_OK;
    /*
    __u8 buf[2];
    __s32 ret;
    ES_FILE* fiic;

    ret = EPDK_OK;
    fiic = NULL;

    fiic = eLIBs_fopen("b:\\BUS\\twi0", "r+");
    if(NULL == fiic)
    {
        __wrn("open iic hdl fail...");
        ret = EPDK_FAIL;
        goto end;
    }

    eLIBs_memset(buf, 0, sizeof(buf));
    buf[0] = 0x0d;

    //ret = iic_read_bytes(fiic, (0xA2>>1), buf, 1, buf+1, 1);
    //if(EPDK_FAIL == ret)
    //{
    //   __wrn("iic_read_bytes fail...");
    //    ret = EPDK_FAIL;
    //    goto end;
    //}

    //__wrn("before write ,buf[2]=0x%x", buf[1]);

    buf[1] &= ~0x03;

    if(0 == mode)
    {
        buf[1] |= 0x0;
    }
    else if(1 == mode)
    {
        buf[1] |= 0x1;
    }
    else if(2 == mode)
    {
        buf[1] |= 0x2;
    }
    else if(3 == mode)
    {
        buf[1] |= 0x3;
    }
    else//default
    {
        buf[1] |= 0x0;
    }

    buf[1] |= 0x01<<7;
    __wrn("buf[1]=0x%x", buf[1]);

    ret = iic_write_bytes(fiic, (0xA2>>1), buf, 1, buf+1, 1);
    if(EPDK_FAIL == ret)
    {
        __wrn("iic_read_bytes fail...");
        ret = EPDK_FAIL;
        goto end;
    }

    end:;

    if(fiic)
    {
        eLIBs_fclose(fiic);
        fiic = NULL;
    }

    if(EPDK_OK == ret)
    {
        __wrn("misc_open_rtc_clk ok...");
    }

    return ret;
    */
#endif
}

__s32 dsk_misc_close_rtc_clk(void)
{
    __wrn("need fix");
    return EPDK_FAIL;
#if 0 /*langaojie temp test*/ 
    ES_FILE *pfile;
    pfile = eLIBs_fopen("b:\\HWSC\\RTC", "r+");

    if (NULL == pfile)
    {
        __wrn("open RTC fail...");
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(pfile, RTC_CMD_SETCLOCKOUT, 0x00, NULL);
    eLIBs_fclose(pfile);
    __wrn("dsk_misc_close_rtc_clk ok...");
    return EPDK_OK;
#endif
}
