/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              LARK app sample
*
*                                   (c) Copyright 2006-2007, TERRY, China
*                                            All Rights Reserved
*
* File    : dsk_radio.c
* By      : terry
* Version : V1.00
* time    : 2009-12-01
**************************************************************************************************************
*/

#include "dsk_radio_i.h"

typedef struct
{
    //ES_FILE       *fm_drv;
    __u32           cur_freq;
    __u32           gain;
} dsk_radio_send_t;

static dsk_radio_send_t *dsk_radio_send;

__s32 dsk_radio_send_open(void)
{
    __s32 result = 0;

    if (h_radio == NULL)
    {
        __msg("radio not open!\n");
        return EPDK_FAIL;
    }

    if (dsk_radio_send)
    {
        return EPDK_OK;
    }

    dsk_radio_send = (dsk_radio_send_t *)esMEMS_Malloc(0, sizeof(dsk_radio_send_t));

    if (dsk_radio_send == NULL)
    {
        __msg("dsk_radio_send malloc error!\n");
        return EPDK_FAIL;
    }

    eLIBs_memset(dsk_radio_send, 0, sizeof(dsk_radio_send_t));
    //h_radio->fm_drv = fm_drv;
	result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SEND_INIT, 0);

    if (result == -1)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}


__s32 dsk_radio_send_close(void)
{
    __s32 result = 0;

    if (dsk_radio_send == NULL)
    {
        __msg("radio send not open!\n");
        return EPDK_FAIL;
    }

	result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SEND_EXIT,0);
    esMEMS_Mfree(0, dsk_radio_send);
    dsk_radio_send = NULL;
    return result;
}

__bool dsk_radio_send_is_open(void)
{
    if (dsk_radio_send)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

__s32 dsk_radio_send_set_freq_play(signed long freq)
{
    __s32 result;
    __drv_fm_area_freq_t freq_range;
	unsigned long arg[4] = {0,0,0,0};

    if (dsk_radio_send == NULL)
    {
        __err("radio send not open!\n");
        return EPDK_FAIL;
    }

    freq_range = h_radio->freq_range;

    /*当参数大于波段对应的最大频率时,频率设为波段的最大频率*/
    if (freq < freq_range.fm_area_min_freq)
    {
        freq = freq_range.fm_area_min_freq;
    }
    /*当参数小于波段对应的最小频率时,频率设为波段的最小频率*/
    else if (freq > freq_range.fm_area_max_freq)
    {
        freq = freq_range.fm_area_max_freq;
    }

    //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SEND_START, 0, (void *)freq);
	arg[0] = 0;
	arg[1] = (unsigned long )(void *)(__s64)freq;
	result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SEND_START,(void *)arg);
    if (result == 0)
    {
        dsk_radio_send->cur_freq = freq;
    }

    return result;
}

__s32 dsk_radio_send_get_cur_freq(__u32 *freq)
{
    if (dsk_radio_send == NULL)
    {
        __err("radio send not open!\n");
        return EPDK_FAIL;
    }

    *freq = dsk_radio_send->cur_freq;
    return 0;
}

__s32 dsk_radio_send_set_gain(unsigned long gain)
{
    __s32 result;
	unsigned long arg[4] = {0,0,0,0};

    if (dsk_radio_send == NULL)
    {
        __err("radio send not open!\n");
        return EPDK_FAIL;
    }

    //result = esMODS_MIoctrl(h_radio->fm_drv, DRV_FM_CMD_SEND_PAGAIN_SET, 0, (void *)gain);
	arg[0] = 0;
	arg[1] = (unsigned long)(void *)(__u64)gain;
	result = ioctl(h_radio->fm_drv, DRV_FM_CMD_SEND_PAGAIN_SET,(void *)arg);

    if (result == 0)
    {
        dsk_radio_send->gain = gain;
    }

    return result;
}


__s32 dsk_radio_send_get_gain(__u32 *gain)
{
    if (dsk_radio_send == NULL)
    {
        __err("radio send not open!\n");
        return EPDK_FAIL;
    }

    *gain = dsk_radio_send->gain;
    return 0;
}

