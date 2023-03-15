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
#include  "rda5820.h"

#if(FM_MODULE_TYPE == FM_MODULE_TYPE_RDA5820)

#define  FM_RDA5820_ADDR          (0x22>>1)

static   __fm_setting_info_t       rda5820_info = {87000, 108000, 0, 0, 3, 0};
static   __u8  fm_freq_table[4] = {0, 8, 32, 50};

static  __u8 RDA5820_reg_addr[] =
{
    0x05,   //seek threshold, LNA port, volume,
    0x07,
    0x13,
    0x15,   //vco
    0x16,
    0x1c,
    0x22, //FM_TRUE
    0x47, //tx rds
    0x56, //sovle tx frequency change
    0x58,
    0x59,
};

static  __u8 RDA5820_reg_data[] =
{
    0x88, 0x4f, //05h register
    0x78, 0x00, //07h register
    0x00, 0x08, //13h register
    0x14, 0x20, //15h register
    0xc0, 0x00, //16h reg...
    0x31, 0x26, //1ch reg...
    0x9c, 0x24, //22h reg...        // 0x9624,
    0xf6, 0x60, //47h register  // tx rds
    0xfc, 0x00, //sovle tx frequency change
    0x07, 0xff,
    0x33, 0xff,
};
/**********************************************
* function:      fm_iic_write
*
* description:   iic write
*
* notes:         5字节连续写
*
**********************************************/
static __s32 fm_iic_write(__u8  addr, __u8 *wfm_data)
{
    __twi_dev_para_ex_t  piic;
    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = FM_RDA5820_ADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = wfm_data;
    piic.byte_count      = 2;
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
static __s32 fm_iic_read(__u8  addr, __u8 *wfm_data)
{
    __twi_dev_para_ex_t  piic;
    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));
    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = FM_RDA5820_ADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = wfm_data;
    piic.byte_count      = 2;
    return  eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);
}
static void fm_rx_init(void)
{
    __s32 i;

    for (i = 0; i < 8; i++)
    {
        fm_iic_write(RDA5820_reg_addr[i], RDA5820_reg_data + i * 2);
    }

    return;
}

static void fm_tx_init(void)
{
    __s32 i;
    __u8  wfm_data[2] = {0, 0};

    for (i = 0; i < 8; i++)
    {
        fm_iic_write(RDA5820_reg_addr[i], RDA5820_reg_data + i * 2);
    }

    fm_iic_read(0x40, wfm_data);
    wfm_data[1] &= 0xf0;
    wfm_data[1] |= 0x01;
    fm_iic_write(0x40, wfm_data);
    return;
}

__s32 fm_get_status(void)
{
    __u8 fm_data[2];
    __s32 ret = EPDK_FAIL;

    if (EPDK_FAIL == fm_iic_read(0x40, fm_data))
    {
        return EPDK_FAIL;
    }

    ret = fm_data[1] & 0x0f;

    if (ret == 0)
    {
        ret = FM_DRV_STATUS_RECEIVE;
    }
    else if (ret == 1)
    {
        ret = FM_DRV_STATUS_SEND;
    }
    else
    {
        ret = EPDK_FAIL;
    }

    return ret;
}

__s32  fm_init(void)
{
    __u8    fm_data[2] = {0x00, 0x02};

    if (EPDK_FAIL == fm_iic_write(0x02, fm_data))
    {
        return EPDK_FAIL;
    }

    esKRNL_TimeDly(1);
    fm_data[0] = 0xC0;
    fm_data[1] = 0x01;

    if (EPDK_FAIL == fm_iic_write(0x02, fm_data))
    {
        return EPDK_FAIL;
    }

    esKRNL_TimeDly(1);
    fm_data[0] = 0x0;
    fm_data[1] = 0x0;

    if (EPDK_FAIL == fm_iic_write(0x02, fm_data))
    {
        return EPDK_FAIL;
    }

    esKRNL_TimeDly(1);
    fm_data[0] = 0xC0;
    fm_data[1] = 0x01;

    if (EPDK_FAIL == fm_iic_write(0x02, fm_data))
    {
        return EPDK_FAIL;
    }

    fm_rx_init();
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
    __u8 wfm_data[2] = {0, 0};
    __s32 temp, flag, index = 0;
    __s8 rssi[4] = {0, 0, 0, 0};
    fm_iic_read(0x02, wfm_data);
    wfm_data[0] &= ~(0x01 << 6);
    fm_iic_write(0x02, wfm_data);
    wfm_data[0] = 0;
    wfm_data[1] = 0;
    fm_iic_write(0x40, wfm_data);

    while (1)
    {
        if ((freq > rda5820_info.max_freq) || (freq < rda5820_info.min_freq))
        {
            return EPDK_FAIL;
        }

        if (rda5820_info.area_select == 1)
        {
            wfm_data[0] = ((freq - 76000) / 100) >> 2;
            wfm_data[1] = (((freq - 76000) / 100) & 0x03) << 6;
        }
        else
        {
            wfm_data[0] = ((freq - 87000) / 100) >> 2;
            wfm_data[1] = (((freq - 87000) / 100) & 0x03) << 6;
        }

        wfm_data[1] |= 0x01 << 4;
        wfm_data[1] |= rda5820_info.area_select << 2;
        fm_iic_write(0x03, wfm_data);
        temp = 0;

        do
        {
            if (temp++ > 10)
            {
                return EPDK_FAIL;
            };

            esKRNL_TimeDly(5);

            fm_iic_read(0x0A, wfm_data);
        } while (!(wfm_data[0] & 0x40));

        fm_iic_read(0x0B, wfm_data);
        rssi[index ++] = wfm_data[0] >> 1;
        flag = 0;

        if (index == 3)
        {
            if (((rssi[2] - rssi[0]) >= 10) && ((rssi[1] - rssi[0]) >= 10))
            {
                flag = 1;
            }

            rssi[0] = rssi[1];
            rssi[1] = rssi[2];
            index = 2;
        }

        //__inf("%d,    %d", wfm_data[0]>>1, wfm_data[0]&0x01);
        //__inf("    freq = %d\n", freq);
        if ((wfm_data[0] & 0x01) && (freq != 96000))
        {
            //__inf("     auto judge\n");
            return freq;
        }

        if (flag)
        {
            //_inf("     my judge\n");
            return freq;
        }

        if (search_dir == DRV_FM_SEARCH_DOWN)
        {
            freq -= 100;
        }
        else
        {
            freq += 100;
        }
    }
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
    __u8  wfm_data[2] = {0, 0};

    if ((freq > rda5820_info.max_freq) || (freq < rda5820_info.min_freq))
    {
        return EPDK_FAIL;
    }

    fm_iic_read(0x02, wfm_data);
    wfm_data[0] &= ~(0x01 << 6);
    fm_iic_write(0x02, wfm_data);
    wfm_data[0] = 0;
    wfm_data[1] = 0;
    fm_iic_write(0x40, wfm_data);

    if (rda5820_info.area_select == 1)
    {
        wfm_data[0] = ((freq - 76000) / 100) >> 2;
        wfm_data[1] = (((freq - 76000) / 100) & 0x03) << 6;
    }
    else
    {
        wfm_data[0] = ((freq - 87000) / 100) >> 2;
        wfm_data[1] = (((freq - 87000) / 100) & 0x03) << 6;
    }

    wfm_data[1] |= 0x01 << 4;
    wfm_data[1] |= rda5820_info.area_select << 2;
    fm_iic_write(0x03, wfm_data);
    esKRNL_TimeDly(5);
    fm_iic_read(0x02, wfm_data);
    wfm_data[0] |= (0x01 << 6);
    fm_iic_write(0x02, wfm_data);
    fm_iic_read(0x0B, wfm_data);
    __wrn("%d\n", wfm_data[0]);
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
**********************************************/                    //OK
__s32 fm_area_choose(__s32 area, void *pbuffer)
{
    __drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

    if ((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
    {
        rda5820_info.area_select = 1;
        rda5820_info.max_freq = FM_SEARCH_JAP_MAX_FREQ;
        rda5820_info.max_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
    }
    else if (area == DRV_FM_AREA_CHN_US_EUP)
    {
        rda5820_info.area_select = 0;
        rda5820_info.max_freq = FM_SEARCH_CHN_MAX_FREQ;
        rda5820_info.min_freq = FM_SEARCH_CHN_MIN_FREQ;
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
**********************************************/                 //OK
__s32 fm_stereo_choose(__s32 audio_method)
{
    if (audio_method == DRV_FM_MONO)
    {
        rda5820_info.stereo_select = 1;
    }
    else if (audio_method == DRV_FM_STEREO)
    {
        rda5820_info.stereo_select = 0;
    }
    else
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32 fm_pa_gain(__u8 pagain)
{
    __u8  wfm_data[2] = {0, 0};
    __u8  temp;

    switch (pagain)
    {
        case DRV_FM_SLEVEL_HIGH:
            temp = 50;
            break;

        case DRV_FM_SLEVEL_NORMAL:
            temp = 32;
            break;

        case DRV_FM_SLEVEL_LOW:
            temp = 12;
            break;

        default:
            temp = 32;
            break;
    }

    fm_iic_read(0x42, wfm_data);
    wfm_data[1] &= ~0x3f;
    wfm_data[1] |= temp;
    fm_iic_write(0x42, wfm_data);
    return EPDK_OK;
}

__s32 fm_signal_gain(__u8 sigain)
{
    __u8  wfm_data[2] = {0, 0};
    fm_iic_read(0x42, wfm_data);
    wfm_data[0] &= ~0x07;
    wfm_data[0] |= sigain;
    fm_iic_write(0x42, wfm_data);
    return EPDK_OK;
}

/**********************************************
* function:      fm_send
*
* description:   发射功能
*
* notes:
*
**********************************************/
__s32 fm_send_on(void)
{
    fm_tx_init();
    return EPDK_OK;
}

__s32 fm_send(__s32 freq)
{
    __u8  wfm_data[2] = {0, 0};

    if ((freq > rda5820_info.max_freq) || (freq < rda5820_info.min_freq))
    {
        return EPDK_FAIL;
    }

    if (rda5820_info.area_select == 1)
    {
        wfm_data[0] = ((freq - 76000) / 100) >> 2;
        wfm_data[1] = (((freq - 76000) / 100) & 0x03) << 6;
    }
    else
    {
        wfm_data[0] = ((freq - 87000) / 100) >> 2;
        wfm_data[1] = (((freq - 87000) / 100) & 0x03) << 6;
    }

    wfm_data[1] |= 0x01 << 4;
    wfm_data[1] |= rda5820_info.area_select << 2;
    fm_iic_write(0x03, wfm_data);
    return EPDK_OK;
}

__s32 fm_send_off(void)
{
    __u8  wfm_data[2] = {0, 0};
    fm_iic_write(0x40, wfm_data);
    fm_rx_init();
    return EPDK_OK;
}
/**********************************************
* function:      fm_mute
*
* description:   静音功能
*
* notes:
*
**********************************************/     //OK
__s32 fm_mute(__s32 voice_onoff)
{
    __u8 wfm_data[2];
    fm_iic_read(0x02, wfm_data);

    if (voice_onoff == DRV_FM_VOICE_OFF)
    {
        wfm_data[0] &= ~(0x01 << 6);
    }
    else if (voice_onoff == DRV_FM_VOICE_ON)
    {
        wfm_data[0] |= (0x01 << 6);
    }
    else
    {
        return EPDK_FAIL;
    }

    fm_iic_write(0x02, wfm_data);
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
**********************************************/             //OK
__s32 fm_signal_level(__s32 signal_level)
{
    __u8 wfm_data[2];
    __twi_dev_para_ex_t  piic;
    __u8 addr = 0x05;
    eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));

    if (signal_level == DRV_FM_SLEVEL_HIGH)
    {
        rda5820_info.signal_level = 3;
    }
    else if (signal_level == DRV_FM_SLEVEL_NORMAL)
    {
        rda5820_info.signal_level = 2;
    }
    else if (signal_level == DRV_FM_SLEVEL_LOW)
    {
        rda5820_info.signal_level = 1;
    }
    else
    {
        return EPDK_FAIL;
    }

    piic.slave_addr_flag = TWI_SLAVE_ADDRESS_07BIT;
    piic.slave_addr      = FM_RDA5820_ADDR;
    piic.byte_addr_width = 1;
    piic.byte_addr       = &addr;
    piic.data            = wfm_data;
    piic.byte_count      = 2;
    eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS, 0, (void *)&piic);
    wfm_data[0] &= 0x80;
    wfm_data[0] |= fm_freq_table[rda5820_info.signal_level];
    eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS, 0, (void *)&piic);
    return EPDK_OK;
}

#endif
