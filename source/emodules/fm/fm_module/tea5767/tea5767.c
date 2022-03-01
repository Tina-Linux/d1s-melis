/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              hello world sample
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File    : tea5767.c
* By      : xueli
* Version : V1.00
*********************************************************************************************************
*/
#include  "tea5767.h"

#if(FM_MODULE_TYPE == FM_MODULE_TYPE_TEA5767)

#define  FM_TEA5767_ADDR          (0xC0>>1)

static   __fm_setting_info_t       tea5767_info = {87000, 108000, 0, 0, 3, 0};

static   __u8  fm_tea5767_hlsi = 1;///???
static   __u8  fm_freq_table[4] = {0, 5, 7, 10};

/**********************************************
* function:      fm_iic_write
*
* description:   iic write
*
* notes:         5字节连续写
*
**********************************************/
static __s32 fm_iic_write(__u8 *wfm_data)
{
    __twi_dev_para_ex_t  piic;
    __u8 addr = 0xff;
    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    wfm_data[2] &= ~(1 << 3);
    wfm_data[2] |= (tea5767_info.stereo_select << 3);
    wfm_data[3] &= ~(1 << 5);
    wfm_data[3] |= (tea5767_info.area_select << 5);
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = FM_TEA5767_ADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = wfm_data;
    piic.byte_count      = 5;
    return  eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, (void *)&piic);
}

/**********************************************
* function:      fm_iic_read
*
* description:   iic write
*
* notes:         5字节连续读
*
**********************************************/
static __s32 fm_iic_read(__u8 *wfm_data)
{
    __twi_dev_para_ex_t  piic;
    __u8 addr = 0xff;
    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = FM_TEA5767_ADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = wfm_data;
    piic.byte_count      = 5;
    return  eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);
}

//由频率计算PLL
static __s32  fm_get_pll(__s32 freq)
{
    __s32 pll;

    if (fm_tea5767_hlsi == 1)
    {
        pll = (((freq + 225) * 4000) / 32768);  //:k
    }
    else
    {
        pll = (((freq - 225) * 4000) / 32768);  //:k
    }

    return pll;
}
//由PLL计算频率
static __s32 fm_get_freq(__s32 pll)
{
    __s32 freq;

    if (fm_tea5767_hlsi == 1)
    {
        freq = (pll * 8192 - 225000) / 1000;  //:KHz
    }
    else
    {
        freq = (pll * 8192 + 25000) / 1000;  //:KHz
    }

    return freq;
}

__s32  fm_init(void)
{
    return EPDK_OK;
}

__s32  fm_exit(void)
{
    return EPDK_OK;
}
/**********************************************
* function:      fm_auto_search
*
* description:   自动搜索，支持向上搜索和向下搜索
*
* notes:         搜索到一个频点后退出，返回值是一个XX.X MHz单位
*
**********************************************/
__s32  fm_auto_search(__s32 freq, __u32 search_dir)
{
    __u8 wfm_data[5] = {0x69, 0x97, 0xb0, 0x11, 0x00};
    __s32 pll, temp, ret_freq;

    if ((freq > tea5767_info.max_freq) || (freq < tea5767_info.min_freq))
    {
        return EPDK_FAIL;
    }

    fm_tea5767_hlsi = 1;
    pll = fm_get_pll(freq);
    wfm_data[0] = pll / 256 + 0xc0;
    wfm_data[1] = pll % 256;

    if (search_dir == DRV_FM_SEARCH_DOWN)
    {
        wfm_data[2] = (tea5767_info.signal_level << 5) | (0x01 << 4);
    }
    else
    {
        wfm_data[2] = (tea5767_info.signal_level << 5) | (0x09 << 4);
    }

    wfm_data[3] = 0x11;
    wfm_data[4] = 0x00;
    fm_iic_write(wfm_data);
    temp = 0;

    do
    {
        ++ temp;

        if (temp > 10)
        {
            return EPDK_FAIL;
        }

        esKRNL_TimeDly(10);
        wfm_data[0] = 0x0;
        fm_iic_read(wfm_data);
    } while (!(wfm_data[0] & 0x80)); //搜台成功标志

    if (wfm_data[0] & 0x40)   //搜台到最高频率
    {
        //if(((wfm_data[3] >> 4) & 0x0f) < fm_freq_table[tea5767_info.signal_level])
        {
            return EPDK_FAIL;
        }
    }

    temp = wfm_data[0] & 0x3f;
    pll  = (temp << 8) + wfm_data[1];
    ret_freq = fm_get_freq(pll);
    temp = ret_freq % 100;

    if (temp >= 50)
    {
        ret_freq += 100 - temp;
    }
    else
    {
        ret_freq -= temp;
    }

    return ret_freq;
}
/**********************************************
* function:      fm_auto_search1
*
* description:   自动搜索，支持向上搜索和向下搜索
*
* notes:         搜索到一个频点后退出，返回值是一个XX.X MHz单位
*
**********************************************/
__s32  fm_auto_search1(__s32 freq, __u32 search_dir)
{
    __u8    wfm_data[5] = {0x69, 0x97, 0xb0, 0x11, 0x00};
    __s32   pll, temp, ret_freq;
    __s32   adc_level1, adc_level2;

    if ((freq > tea5767_info.max_freq) || (freq < tea5767_info.min_freq))
    {
        return EPDK_FAIL;
    }

_auto_search_continue_:
    fm_tea5767_hlsi = 1;
    pll = fm_get_pll(freq);
    wfm_data[0] = pll / 256 + 0xc0;
    wfm_data[1] = pll % 256;

    if (search_dir == DRV_FM_SEARCH_DOWN)
    {
        wfm_data[2] = (tea5767_info.signal_level << 5) | (0x01 << 4);
    }
    else
    {
        wfm_data[2] = (tea5767_info.signal_level << 5) | (0x09 << 4);
    }

    wfm_data[3] = 0x11;
    wfm_data[4] = 0x00;
    fm_iic_write(wfm_data);
    temp = 0;

    do
    {
        ++ temp;

        if (temp > 10)
        {
            return EPDK_FAIL;
        }

        esKRNL_TimeDly(10);
        wfm_data[0] = 0x0;
        fm_iic_read(wfm_data);
    } while (!(wfm_data[0] & 0x80)); //搜台成功标志

    if (wfm_data[0] & 0x40)   //搜台到最高频率
    {
        if (((wfm_data[3] >> 4) & 0x0f) < fm_freq_table[tea5767_info.signal_level])
        {
            return EPDK_FAIL;
        }
    }

    temp = wfm_data[0] & 0x3f;
    pll  = (temp << 8) + wfm_data[1];
    adc_level1 = (wfm_data[3] & 0xf0) >> 4;
    ret_freq = fm_get_freq(pll);
    temp = wfm_data[2] & 0x7f;

    if ((temp < 0x31) || (temp > 0x3E))
    {
        freq = ret_freq;
        goto _auto_search_continue_;
    }

    fm_tea5767_hlsi = 0;
    pll = fm_get_pll(freq);
    wfm_data[0] = pll / 256;
    wfm_data[1] = pll % 256;
    wfm_data[2] = 0x00;
    wfm_data[3] = 0x11;
    wfm_data[4] = 0x00;
    fm_iic_write(wfm_data);
    esKRNL_TimeDly(10);
    fm_iic_read(wfm_data);
    adc_level2 = (wfm_data[3] & 0xf0) >> 4;
    temp = wfm_data[2] & 0x7f;

    if ((temp < 0x31) || (temp > 0x3E))
    {
        freq = ret_freq;
        goto _auto_search_continue_;
    }

    if (((adc_level1 - adc_level2) > -3) && ((adc_level1 < adc_level2) < 3))
    {
        temp = ret_freq % 100;

        if (temp >= 50)
        {
            ret_freq += 100 - temp;
        }
        else
        {
            ret_freq -= temp;
        }
    }
    else
    {
        freq = ret_freq;
        goto _auto_search_continue_;
    }

    return ret_freq;
}
/**********************************************
* function:      fm_play
*
* description:   根据输入的频率，播放此频率的
*                电台节目，即使没有节目只有
*                噪音，照样播出
*
* notes:         只返回成功
*
**********************************************/
__s32 fm_play(__s32 freq)
{
    __u8  adc_level1, adc_level2;
    __u8  wfm_data[5];
    __s32 pll;

    if ((freq > tea5767_info.max_freq) || (freq < tea5767_info.min_freq))
    {
        return EPDK_FAIL;
    }

    fm_tea5767_hlsi = 1;
    pll = fm_get_pll(freq);
    wfm_data[0] = pll / 256 + 0x80;
    wfm_data[1] = pll % 256;
    wfm_data[2] = 0x10;
    wfm_data[3] = 0x11;
    wfm_data[4] = 0x00;
    fm_iic_write(wfm_data);
    esKRNL_TimeDly(10);
    fm_iic_read(wfm_data);
    adc_level1 = (wfm_data[3] & 0xf0) >> 4;
    fm_tea5767_hlsi = 0;
    pll = fm_get_pll(freq);
    wfm_data[0] = pll / 256 + 0x80;
    wfm_data[1] = pll % 256;
    wfm_data[2] = 0x00;
    wfm_data[3] = 0x11;
    wfm_data[4] = 0x00;
    fm_iic_write(wfm_data);
    esKRNL_TimeDly(10);
    fm_iic_read(wfm_data);
    adc_level2 = (wfm_data[3] & 0xf0) >> 4;

    if (adc_level1 < adc_level2)
    {
        fm_tea5767_hlsi = 0;
        pll = fm_get_pll(freq);
        wfm_data[0] = pll / 256;
        wfm_data[1] = pll % 256;
        wfm_data[2] = 0x00;
        wfm_data[3] = 0x11;
        wfm_data[4] = 0x00;
        fm_iic_write(wfm_data);
    }
    else
    {
        fm_tea5767_hlsi = 1;
        pll = fm_get_pll(freq);
        wfm_data[0] = pll / 256;
        wfm_data[1] = pll % 256;
        wfm_data[2] = 0x10;
        wfm_data[3] = 0x11;
        wfm_data[4] = 0x00;
        fm_iic_write(wfm_data);
    }

    return EPDK_OK;
}
/**********************************************
* function:      fm_manual_search
*
* description:   手动搜索，使用fm_play
*                返回值是下一个搜索频率点
*
* notes:
*
**********************************************/
__s32 fm_manual_search(__s32 freq, __u32 search_dir)
{
    return fm_play(freq);
}
/**********************************************
* function:      fm_area_choose
*
* description:   地区选择，区别不同的起始和终止
*                频率
*
* notes:         输入正确地区返回成功，否则失败
*
**********************************************/
__s32 fm_area_choose(__s32 area, void *pbuffer)
{
    __drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

    if ((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
    {
        tea5767_info.area_select = 1;
        tea5767_info.max_freq = FM_SEARCH_JAP_MAX_FREQ;
        tea5767_info.max_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
    }
    else if (area == DRV_FM_AREA_CHN_US_EUP)
    {
        tea5767_info.area_select = 0;
        tea5767_info.max_freq = FM_SEARCH_CHN_MAX_FREQ;
        tea5767_info.min_freq = FM_SEARCH_CHN_MIN_FREQ;
        freq_range->fm_area_min_freq = FM_SEARCH_CHN_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_CHN_MAX_FREQ;
    }
    else
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
/**********************************************
* function:      fm_stereo_choose
*
* description:   音质选择，立体声和普通声音
*
* notes:
*
**********************************************/
__s32 fm_stereo_choose(__s32 audio_method)
{
    if (audio_method == DRV_FM_MONO)
    {
        tea5767_info.stereo_select = 1;
    }
    else if (audio_method == DRV_FM_STEREO)
    {
        tea5767_info.stereo_select = 0;
    }
    else
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
/**********************************************
* function:      fm_mute
*
* description:   静音功能
*
* notes:
*
**********************************************/
__s32 fm_mute(__s32 voice_onoff)
{
    __u8 wfm_data[1];
    __twi_dev_para_ex_t  piic;
    __u8 addr = 0xff;
    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = FM_TEA5767_ADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = wfm_data;
    piic.byte_count      = 1;
    eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);

    if (voice_onoff == DRV_FM_VOICE_OFF)
    {
        wfm_data[0] |=   0x01 << 7;
        wfm_data[0] &= ~(0x01 << 6);
    }
    else if (voice_onoff == DRV_FM_VOICE_ON)
    {
        wfm_data[0] &= ~(0x01 << 7);
        wfm_data[0] &= ~(0x01 << 6);
    }
    else
    {
        return EPDK_FAIL;
    }

    eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, (void *)&piic);
    return EPDK_OK;
}
/**********************************************
* function:      fm_signal_level
*
* description:   信号强度选择，要求信号强大越高，收到的电台越少
*                   要求信号强大越高，收到的电台越多，但无效电台也多
*
* notes:
*
**********************************************/
__s32 fm_signal_level(__s32 signal_level)
{
    if (signal_level == DRV_FM_SLEVEL_HIGH)
    {
        tea5767_info.signal_level = 3;
    }
    else if (signal_level == DRV_FM_SLEVEL_NORMAL)
    {
        tea5767_info.signal_level = 2;
    }
    else if (signal_level == DRV_FM_SLEVEL_LOW)
    {
        tea5767_info.signal_level = 1;
    }
    else
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
__s32  fm_send_on(void)
{
    return EPDK_FAIL;
}

__s32  fm_send(__s32 freq)
{
    return EPDK_FAIL;
}

__s32  fm_send_off(void)
{
    return EPDK_FAIL;
}

__s32  fm_pa_gain(__u8 pagain)
{
    return EPDK_FAIL;
}

__s32  fm_get_status(void)
{
    return FM_DRV_STATUS_RECEIVE;
}

#endif

