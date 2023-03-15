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

#include "fm_all_drv.h"
#include <sunxi_hal_twi.h>
#include "dfs_posix.h"
#if (FM_MODULE_TYPE == FM_MODULE_TYPE_ALL)
#include "libc.h"
#include "mod_twi.h"
#include "mod_fm.h"
#include <log.h>
#define TWI_SCLK_400KHZ     (400000) // 400kHz
#define TWI_SCLK_100KHZ     (100000) //100kHz
#define TWI_SCLK_50KHZ      (50000) //50kHz

__s32 fm_play_rtc6207m(__s32 curFreq);
static __s32 fm_read_data(__u8 addr, __u8 *data,__s32 n_byte);
__s32 fm_vol_set(__u8 volume);

__u8 VolValue=0;

#define  FM_SEARCH_CHN_MIN_FREQ         87000
#define  FM_SEARCH_CHN_MAX_FREQ        108000

#define  FM_SEARCH_JAP_MIN_FREQ         76000
#define  FM_SEARCH_JAP_MAX_FREQ         91000
static __u16 max_freq = 0;
static __u16 min_freq = 0;


const __u8 RTC62XX_turnoff[]=
{
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x16,0xAA
};

const __u8 RTC62XX_turnon[]=
{
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x96,0xAA
};


const __u8  RTC62XX_power_up[]=
{
    0x00,0x00, 0x04,0x00, 0x00,0x00, 0x00,0x00, 0x80,0x05, 0xb4,0x00, 0x00,0x00,
    0x40,0x50
};  // last byte is spike threshold, the larger the more, recommand under 0x50, default 0x10

const __u8  RTC62XX_Initial_value[] =
{
	0xC0,0x0f,0x04,0x00,0x00,0x00,0x00,0x0c,0x80,0x05,0xb4,0x00,0xe0,0x00
};	// RSSI_TH = 0x0c


const __u8  RTC62XX_Initial_value1[] =
{
    0x00,0x72,0x00,0xFF,0x00,0x1F,0x03,0xFF,0x56,0xD1,0x13,0xB7,0x00,0x00
};

const __u8  RTC62XX_CKHopping[]=
{
	0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x70,0x00, 0x20,0x00,
	0x05,0x0F, 0x0E,0x85, 0x5A,0xA6, 0xDC,0x53, 0x80,0x00, 0x00,0xA3, 0x00,0xA3,
	0xD0,0x08, 0x7F,0x80, 0x3C,0x08, 0xA6,0xCF, 0x81,0x00, 0x00,0x00, 0x01,0x40,
	0x47,0x00, 0x00,0x00
};


static __s32 iic_read_data(__u16 slave_addr,__u8 reg_addr,__u8*data,__s32 n_byte)
{
    twi_msg_t msg;
    msg.flags  =  0;
    msg.addr   =  slave_addr;
    msg.flags &= ~(TWI_M_RD);
    msg.len    = 1;
    msg.buf    = &reg_addr;
    ioctl(fiic, I2C_RDWR, &msg); 

    msg.flags  = 0;
    msg.addr   =  slave_addr;
    msg.flags |= (TWI_M_RD);
    msg.len    = n_byte;
    msg.buf    = data;
    ioctl(fiic, I2C_RDWR, &msg);
    return EPDK_OK;
}
static __s32 iic_write_data(__u16 slave_addr,__u8 reg_addr,__u8*data,__s32 n_byte)
{
    twi_msg_t msg; 
    int i =0 ;
    uint8_t *buf = malloc(n_byte+1);
    if(buf == NULL)
    {
        return EPDK_FAIL;
    }
    buf[0] = reg_addr;

    for(i=0;i<n_byte;i++)
    {
        buf[i+1] = data[i];
    }
    
    msg.flags  = 0;
    msg.addr   =  slave_addr;
    msg.flags &= ~(TWI_M_RD);
    msg.len    = n_byte+1;
    msg.buf    = buf;
    ioctl(fiic, I2C_RDWR, &msg); 
    
    free(buf);
    return EPDK_OK;
}


static __s32 fm_write_data_rtc6207m(__u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = iic_write_data((0xc8>>1),0xff,data,n_byte);
	if(ret != EPDK_OK)
	{
		__err("fm6207 i2c write error n_byte=%d....~\n",n_byte);
	}
	return ret;
}

static __s32 fm_read_data_rtc6207m( __u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = iic_read_data((0xc9>>1),0xff,data,n_byte);
    if(ret == EPDK_FAIL)
	{
		__err("fm6207 i2c read error n_byte=%d....~\n",n_byte);
	}
	return ret;
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
__s32 fm_area_choose_rtc6207m(__s32 area, void *pbuffer)
{
    __drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

    if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
    {
       max_freq = FM_SEARCH_JAP_MAX_FREQ/100;
        min_freq= FM_SEARCH_JAP_MIN_FREQ/100;
	freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;	
    }
    else if(area == DRV_FM_AREA_CHN_US_EUP)
    {
        max_freq = FM_SEARCH_CHN_MAX_FREQ/100;
        min_freq = FM_SEARCH_CHN_MIN_FREQ/100;
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
* function:      fm_manual_search
*
* description:   手动搜索，使用fm_play
*                返回值是下一个搜索频率点
*
* notes:
*
**********************************************/
__s32 fm_manual_search_rtc6207m(__s32 freq, __u32 search_dir)
{
    return fm_play_rtc6207m(freq);
}

/**********************************************
* function:      fm_mute
*
* description:   静音功能
*
* notes:
*
**********************************************/
__s32 fm_mute_rtc6207m(__s32 voice_onoff)
{
    	__u8  	RTC62XX_set_property[] = {0xC0, 0x0f};
	__u8	RTC62XX_reg_data[18];
	__u8	volume_temp = 0;

	fm_read_data_rtc6207m(&(RTC62XX_reg_data[0]), 18);
	volume_temp = RTC62XX_reg_data[17] & 0x0f;
	RTC62XX_set_property[1] = (RTC62XX_set_property[1] & 0xf0) | volume_temp;
	
    if(voice_onoff==DRV_FM_VOICE_ON)
        RTC62XX_set_property[0] = 0xc0;
    else
        RTC62XX_set_property[0] = 0x00;

    fm_write_data_rtc6207m(&(RTC62XX_set_property[0]), 2);
    return EPDK_OK;
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
__s32 fm_play_rtc6207m(__s32 curFreq)//875000
{   
    __u16  freq_reg_data, loop_counter;
    __u8  RTC6207E_reg_data[24];
    __u8  RTC6207E_channel_start_tune[] = {0xC0,0x0f,0x84,0xCA};	//107.7MHz

    freq_reg_data = (curFreq - 87500)/100;
    RTC6207E_channel_start_tune[3] = freq_reg_data & 0xff;
    RTC6207E_channel_start_tune[2] = (RTC6207E_channel_start_tune[2] & 0xfc) | (freq_reg_data >> 8);

    RTC6207E_channel_start_tune[2]&= 0x7f;
    fm_write_data_rtc6207m(&(RTC6207E_channel_start_tune[0]), 4);

	RTC6207E_channel_start_tune[2]|= 0x80;
     fm_write_data_rtc6207m(&(RTC6207E_channel_start_tune[0]), 4);
	return EPDK_OK;
}


/**
 * @brief Station judge for auto search
 * @In auto search mode,uses this function to judge the frequency if has a station
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param int16 freq:frequency value
 * @return bool: if return true,the frequency has a true station;otherwise doesn't have a station
 * @retval
 */
__bool RTC6207E_ValidStop(__u16 freq)
{
    __u16  freq_reg_data, loop_counter;
    __u8  RTC6207E_reg_data[24];
    __u8  RTC6207E_channel_start_tune[] = {0xC0,0x0f,0x84,0xCA};	//107.7MHz
    __u8  RTC6207E_channel_stop_tune[] = {0xC0,0x0f,0x04,0xCA};	

    __u8  RSSIValue = 0;

    freq_reg_data = (freq - 8750)/10;
    RTC6207E_channel_start_tune[3] = freq_reg_data & 0xff;
    RTC6207E_channel_start_tune[2] = (RTC6207E_channel_start_tune[2] & 0xfc) | (freq_reg_data >> 8);
    RTC6207E_channel_stop_tune[3] = RTC6207E_channel_start_tune[3] ;

	RTC6207E_channel_start_tune[2]&= 0x7f;
    fm_write_data_rtc6207m(&(RTC6207E_channel_start_tune[0]), 4);

	RTC6207E_channel_start_tune[2]|= 0x80;
    fm_write_data_rtc6207m(&(RTC6207E_channel_start_tune[0]), 4);
    esKRNL_TimeDly(8);	
	 
    loop_counter = 0;
    do
    {
        fm_read_data_rtc6207m(&(RTC6207E_reg_data[0]), 2);
        loop_counter++;
    }
    while(((RTC6207E_reg_data[0]&0x40) == 0) && (loop_counter < 0xff));		//for loop_counter, when tune, the loop time must > 60ms

    if(loop_counter == 0xff)
        return 0;

    loop_counter = 0;

    //clear tune bit
    fm_write_data_rtc6207m(&(RTC6207E_channel_stop_tune[0]), 4);
    //wait STC=0
    do
    {
        fm_read_data_rtc6207m(&(RTC6207E_reg_data[0]), 2);
        loop_counter++;
    }
    while(((RTC6207E_reg_data[0]&0x40) != 0) && (loop_counter < 0xff));

    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_Initial_value[0]), 14);
    fm_read_data_rtc6207m(&(RTC6207E_reg_data[0]), 24);


    fm_write_data_rtc6207m((__u8 *)&RTC62XX_Initial_value1[0], 14);

    if((RTC6207E_reg_data[22]&0x80)!= 0)
    {
        return 0;
    }

    fm_read_data_rtc6207m(&(RTC6207E_reg_data[0]), 4);

    RSSIValue = RTC6207E_reg_data[3];
	
    if(RSSIValue > RTC62XX_Initial_value[7])
        return 1;
    else
        return 0;

}


__s32  fm_auto_search_rtc6207m(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 100);

	while(1){
		if((temp > max_freq) || (temp < min_freq))
		{
			return EPDK_FAIL;
		}
		returnValue = RTC6207E_ValidStop(temp*10);

		if (returnValue)
		{
			return temp * 100; 				
		}else{
			return (temp * 100)|0xff000000;
		}
		if(search_dir == DRV_FM_SEARCH_DOWN){
			temp--;
		}else{
			temp++;
		}
	}
	return EPDK_FAIL;
}

void rtc6207m_set_vol(__u8 volume)
{
	__u8  regVal;
    unsigned char RTC62XX_set_property[] = {0xC0,0x0F};	
	
  if(volume>=15)
  {
  	fm_mute(DRV_FM_VOICE_ON);	
  	volume = 15;
  }
  else if(volume==0)
  {
	fm_mute(DRV_FM_VOICE_OFF);	
  }
else
{
	fm_mute(DRV_FM_VOICE_ON);	
	
}    
 VolValue=volume;
    RTC62XX_set_property[1] = (RTC62XX_set_property[1] & 0xf0) | volume;

   fm_write_data_rtc6207m(&(RTC62XX_set_property[0]), 2);
}

__s32 rtc6207m_get_vol(void)
{
	return VolValue;
}
__s32 fm_vol_set_rtc6207m(__u8 volume)
{
	rtc6207m_set_vol(volume);
	return EPDK_OK;
}
__s32 fm_vol_get_rtc6207m(void)
{
	return rtc6207m_get_vol();
}

__bool RTC6207E_Read_ID(void)
{
    __u8 RTC62XX_reg_data[20];

    fm_read_data_rtc6207m(&(RTC62XX_reg_data[0]), 20);

    if(((RTC62XX_reg_data[12] << 8) | RTC62XX_reg_data[13])==0x1688)
    {      
        return 1;
    }
    else
    {
        return 0;
    }
}

__s32  fm_init_rtc6207m(void)
{
	//__s32 ret;	
	//ret = eLIBs_fioctrl(fiic, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_100KHZ);	
	
	//if(ret == EPDK_OK)
	//{
	//	__wrn("set iic clock as 100Khz successfully\n");
	//}
    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_turnoff[0]),30);
    esKRNL_TimeDly(1);	

    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_turnon[0]),30);
    esKRNL_TimeDly(1);	

    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_CKHopping[0]),46);
    esKRNL_TimeDly(1);

    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_power_up[0]),16);
    esKRNL_TimeDly(5);	

	//fm_play(89800);//89800
	//esKRNL_TimeDly(20);	
	//RTC6207E_Read_ID();
	
	 //   __wrn("*****write rtc6207m_set_vol....\n");
	//rtc6207m_set_vol(15);
	fm_mute(DRV_FM_VOICE_ON);
	
    return EPDK_OK;
}
__s32  fm_exit_rtc6207m(void)
{
	fm_write_data_rtc6207m((__u8 *)&(RTC62XX_turnoff[0]), 30);
    return EPDK_OK;
}

void RTC6207E_Power_Up(void)
{
    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_turnoff[0]),30);
    esKRNL_TimeDly(1);
    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_turnon[0]),30);
    esKRNL_TimeDly(1);
    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_CKHopping[0]),46);
    esKRNL_TimeDly(1);
    fm_write_data_rtc6207m((__u8 *)&(RTC62XX_power_up[0]),16);
    esKRNL_TimeDly(5);
}

__bool is_online_rtc6207e(void)
{
    	__u8 RTC62XX_reg_data[20];
	__u16 fm_id;
	__u16 chip_id;

	RTC6207E_Power_Up();
	fm_read_data_rtc6207m(&(RTC62XX_reg_data[0]), 20);
	
	fm_id = (RTC62XX_reg_data[12] << 8) | RTC62XX_reg_data[13];
	chip_id = (RTC62XX_reg_data[14] << 8) | RTC62XX_reg_data[15];
	//eLIBs_printf("---RTC6207E_Read_ID---fm_id=0x%x,chip_id=0x%x\n",fm_id,chip_id);
	if(0x1688 == fm_id && 0x14c0 == chip_id){      
		return 1;
	} else {		
		return 0;
	}
}


//////////////////////////////////////8065


//if using scan noise floor as CCA algorithm,macro QN8065_SCAN_NOISE_FLOOR_CCA will be set to 1 
#define QN8065_SCAN_NOISE_FLOOR_CCA 		1

//if using pilot as CCA algorithm,macro QN8065_PILOT_CCA will be set to 1 
#define QN8065_PILOT_CCA					0

//if want improving some special frequency SNR, macro IMPROVING_SNR will be set to 1
#define IMPROVING_SNR                       1
__u8 QN8065_PreNoiseFloor = 40,QN8065_NoiseFloor = 40;


static   __fm_setting_info_t       rda5820_info = {87000, 108000, 0, 0, 3, 0};

static __s32 fm_write_data(__u8 addr, __u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = iic_write_data((0x20>>1),addr,data,n_byte);
	if(ret != EPDK_OK)
	{
		__err("fm qn8065 i2c write error~\n");
	}
	return ret;
}
static __s32 fm_read_data(__u8 addr, __u8 *data,__s32 n_byte)
{
	__s32 ret;

    ret = iic_read_data((0x20>>1),addr,data,n_byte);
    if(ret == EPDK_FAIL)
	{
		__err("fm qn8065 i2c read error~\n");
	}
	return ret;
}

__u8 QN8065_ReadReg(__u8 adr)
{
	__u8 Data;
	if(EPDK_FAIL==fm_read_data(adr,&Data,1))
		return 0;
	return Data;
}

__s32 QN8065_WriteReg(__u8 adr, __u8 value)
{
 	return fm_write_data(adr,&value,1);
}

void QN8065_Delay(int ms)
{
  if(ms<10)
	{
		esKRNL_TimeDly(1);
	}
	else
	{
		esKRNL_TimeDly(ms/10);
	}
}

__u32 QN8065_FREQ2CHREG(__u32 freq)
{
    return 	(freq-6000)/5;
}

/**********************************************************************
void QN8065_SetMute(__u8 On)
**********************************************************************
Description: on or off mute chip
Parameters:
    On: 1:on mute chip;0:off mute chip
Return Value:
    None
**********************************************************************/	
void QN8065_SetMute(__u8 On)
{
	//mute or unmute by holding DAC 
    //and set low voltage regulator output as 0.9v
    QN8065_WriteReg(QN8065_REG_REF, On?0x14:0x10); 
}

/**********************************************************************
void QN8065_SetCh(__u32 start,__u32 stop,__u8 step) 
**********************************************************************
Description: set channel frequency
Parameters:
    freq:  channel frequency to be set,frequency unit is 10KHZ
Return Value:
    None
**********************************************************************/	
void QN8065_SetCh(__u32 start,__u32 stop,__u8 step) 
{
    __u8 temp;
    //TRACE("start:%d,stop:%d,step:%d \n",start,stop, step);
    start = QN8065_FREQ2CHREG(start);
    //fored the stop is bigger than start
    stop = QN8065_FREQ2CHREG(stop);
	//writing higher bits of channel frequency,CCA channel start,
	//stop and step index at first
	temp = (__u8) ((start >> 8) & QN8065_CH_CH);
	temp |= ((__u8)(start >> 6) & QN8065_CH_CH_START);
	temp |= ((__u8) (stop >> 4) & QN8065_CH_CH_STOP);
	temp |= (step << 6);
	QN8065_WriteReg(QN8065_CH_STEP, temp);
	//writing lower 8 bits of CCA channel start index
	QN8065_WriteReg(QN8065_CH_START, (__u8)start);
	//writing lower 8 bits of CCA channel stop index
	QN8065_WriteReg(QN8065_CH_STOP, (__u8)stop);
	//writing lower 8 bits of channel index
	QN8065_WriteReg(QN8065_CH, (__u8)start);
}

/**********************************************************************
void QN8065_Init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.
Parameters:
    None
Return Value:
    None    
**********************************************************************/
void QN8065_Init(void)
{

	__u8 regdata=0;
	//QN8065_WriteReg(0x07, 0x81); 
	//regdata=QN8065_ReadReg(0x07);
	//__wrn("****regdata=0x%x****\n",regdata);

	QN8065_WriteReg(QN8065_SYSTEM1, 0x81); 
    QN8065_Delay(10);    

    /*********User sets chip working clock **********/
    //Following is where change the input clock wave type,as sine-wave or square-wave.
   // QN8065_WriteReg(QN8065_CCA,QN8065_DIGITAL_CLOCK);
	QN8065_WriteReg(QN8065_CCA,QN8065_SINE_WAVE_CLOCK);
    //Following is where change the input clock frequency.
    
    QN8065_WriteReg(QN8065_XTAL_DIV0, QN8065_XTAL_DIV0_VAL);
    QN8065_WriteReg(QN8065_XTAL_DIV1, QN8065_XTAL_DIV1_VAL);
    QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);    
    QN8065_Delay(10);  
    
    /********User sets chip working clock end ********/
    QN8065_WriteReg(QN8065_CCA1, 0x96);
    QN8065_WriteReg(QN8065_CCOND1, 0xb6);    
    ///select SNR for SM,SNC,HCC MPX   
    QN8065_WriteReg(QN8065_SMSTART,0x80|QN8065_SMSTART_VAL); //set SMSTART
    QN8065_WriteReg(QN8065_SNCSTART,0x80|QN8065_SNCSTART_VAL); //set SNCSTART
    QN8065_WriteReg(QN8065_HCCSTART,0x80|QN8065_HCCSTART_VAL); //set HCCSTART
    //enter receiver mode directly
    QN8065_WriteReg(QN8065_SYSTEM1, 0x11);	
    QN8065_SetMute(1);
}

/**********************************************************************
void QN8065_TuneToCH(__u32 ch)
**********************************************************************
Description: Tune to the specific channel.
Parameters:
	ch:Set the frequency (10kHz) to be tuned,
eg: 101.30MHz will be set to 10130.
Return Value:
	None
**********************************************************************/
void QN8065_TuneToCH(__u32 ch)
{ 
	__u8 sysClk32kFlag = 0;
	__u8 stereoFlag = 0;
	__u8 reg;

    QN8065_SetMute(1);
	if (QN8065_ReadReg(QN8065_XTAL_DIV0) == 0x01)
	{
		sysClk32kFlag = 1;
		stereoFlag = (QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX) ? 0:1;
	}
#if IMPROVING_SNR
	//improving some special stations snr when working clock is 32768Hz
	if (ch == 10030 || ch == 10090 || ch == 10100 || ch == 10110 
		|| ch == 10150 || ch == 10160 || ch == 10220)
	{
		//frequency is added 50k only system clock is 32K.
		if (sysClk32kFlag)
		{
			QN8065_WriteReg(QN8065_XTAL_DIV2, 0x67);
			//add 50K
			ch += 5; 
		}
	}  
		//restore XTAL_DIV2 right value if the station is not a special station 
	else
	{
		QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);   
	}    
#endif
    QN8065_WriteReg(QN8065_REG_FLL,0x84); //enable 2k speed
    //enter idle mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x01);
    //step is 100K.
    QN8065_SetCh(ch, ch, 1); 
    //enter rx mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x11);
    QN8065_Delay(QN8065_CH_SETUP_DELAY_TIME);
    QN8065_WriteReg(QN8065_REG_FLL, 0x04);//disable 2k speed
	if (sysClk32kFlag)
	{
		reg = QN8065_ReadReg(QN8065_CCA1);
		//disable bypass 32k spur rejection filter if is stereo
		if (stereoFlag)
		{
			reg &= ~0x20; 			
		}
		//enable bypass 32k spur rejection filter if is mono
		else
		{
			reg |= 0x20;
		}
		QN8065_WriteReg(QN8065_CCA1, reg);
	}	
    QN8065_SetMute(0);    
}

#if QN8065_SCAN_NOISE_FLOOR_CCA
 /***********************************************************************
Description: scan a noise floor from 87.5M to 108M by step 200K
Parameters:
Return Value:
 1: scan a noise floor successfully.                                                                                         
 0: chip would not normally work. 
**********************************************************************/
__u8 QN8065_ScanNoiseFloor(__u32 start,__u32 stop)
{
	__u8 regValue;
	__u8 timeOut = 255; //time out is 2.55S     
	
	QN8065_WriteReg(QN8065_CCA_TIME,0x00);
	//config CCS from start frequency to  stop frequency by step 200KHZ	
	QN8065_SetCh(start,stop,2);
	//enable CCS
	QN8065_WriteReg(QN8065_CCA2, 0x95); 
	//enter CCA mode,channel index is decided by internal CCA 
	QN8065_WriteReg(QN8065_SYSTEM1,0x12);	
    while(1)
    {
        regValue = QN8065_ReadReg(QN8065_SYSTEM1);        
        //if it seeks a potential channel, the loop will be quited     
        if((regValue & QN8065_CHSC) == 0) break;
        QN8065_Delay(10);   //delay 10ms
        //if it was time out,chip would not normally work.
        if((timeOut--) == 0) 
        {            
            //disable CCS while exit
            QN8065_WriteReg(QN8065_CCA2,0x85);
            return 0;
        }
    }	
    //disable CCS while exit
	QN8065_WriteReg(QN8065_CCA2,0x85);
	QN8065_NoiseFloor = QN8065_ReadReg(QN8065_CCS_NOISE)&0x7f;	
	if(((QN8065_PreNoiseFloor-QN8065_NoiseFloor) > 2) ||((QN8065_NoiseFloor-QN8065_PreNoiseFloor) > 2))
	{
		QN8065_PreNoiseFloor = QN8065_NoiseFloor;
	}
	//TRACE("%d-%d noiseFloor:%d,timeElapsed:%dms\n",start,stop,QN8065_NoiseFloor,(255-timeOut)*10);
	return 1;
}
#endif

/***********************************************************************
void QN8065_RXSetTH(void)
***********************************************************************
Description: Setting the threshold value of automatic scan channel 
	
Return Value:
  	None
***********************************************************************/
void QN8065_RXSetTH(void)
{	
    __u8 rssiTH;
	                            
    rssiTH = (QN8065_PreNoiseFloor-28)+3;
#if IMPROVING_SNR       
    //restore XTAL_DIV2 right value if the station is not a special station 
    QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);     
#endif    
    //set IF counter error range value is 256 = 4*64
    QN8065_WriteReg(QN8065_CCA_CNT, 5);  
    //set CCA time
    QN8065_WriteReg(QN8065_CCA_TIME, QN8065_CCA_NAGC_40MS|QN8065_CCA_NSNR_20MS|QN8065_CCA_NRSSI_10MS);
    //set RSSI threshold for CCA
    QN8065_WriteReg(QN8065_CCA, (QN8065_ReadReg(QN8065_CCA)&~0x3f)|(rssiTH));
    //set QN8065_SNR threshold for CCA	
#if QN8065_PILOT_CCA
	QN8065_WriteReg(QN8065_CCA_SNR_TH, (QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f)|(7));  //pilot CCA the QN8065_SNR thershold is 8
#else
	QN8065_WriteReg(QN8065_CCA_SNR_TH, (QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f)|(8)); 	//normall CCA the QN8065_SNR thereshold is 9
#endif
	//TRACE("rssiTH=%d, snrTH=%d\n", rssiTH,snrTH);	
}

/***********************************************************************
__s32 QN8065_RXValidCH(__u32 freq);
***********************************************************************
Description: to validate a ch (frequency)(if it's a valid channel)
Freq: specific channel frequency, unit: 10Khz
  eg: 108.00MHz will be set to 10800.
Return Value:
  0: it's not a valid channel
  1: it's a valid channel
 -1: chip does not normally work.
***********************************************************************/
__s32 QN8065_RXValidCH(__u32 freq) 
{
	__u8 regValue;
	__u8 timeOut;
	__u8 isValidChannelFlag;
#if QN8065_PILOT_CCA
	__u8 snr,readCnt,stereoCount=0;
#endif
	
#if QN8065_SCAN_NOISE_FLOOR_CCA
	switch(freq)
	{
		case 8750:
		QN8065_ScanNoiseFloor(8700,8800);	
		QN8065_RXSetTH();
		break;
	case 8810:
    	QN8065_ScanNoiseFloor(8810,9000);	
    	QN8065_RXSetTH();
    	break;
	case 9010:
		QN8065_ScanNoiseFloor(9010,9200);	
		QN8065_RXSetTH();
		break;
	case 9210:
		QN8065_ScanNoiseFloor(9210,9400);
		QN8065_RXSetTH();
		break;
	case 9410:
		QN8065_ScanNoiseFloor(9410,9600);
		QN8065_RXSetTH();
		break;
	case 9610:
		QN8065_ScanNoiseFloor(9610,9800);
		QN8065_RXSetTH();
		break;
	case 9810:
		QN8065_ScanNoiseFloor(9810,10000);
		QN8065_RXSetTH();
		break;	
	case 10010:
		QN8065_ScanNoiseFloor(10010,10200);
		QN8065_RXSetTH();
		break;
		case 10210:
		QN8065_ScanNoiseFloor(10210,10400);
		QN8065_RXSetTH();
		break;
	case 10410:
		QN8065_ScanNoiseFloor(10410,10600);
		QN8065_RXSetTH();
		break;	
	case 10610:
		QN8065_ScanNoiseFloor(10610,10800);
		QN8065_RXSetTH();
		break;		
	default:
		//QN8065_Delay(100); //this delay can improve user display effect
		break;
	}
	//enter idle mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x01);
#endif	
	QN8065_SetCh(freq,freq+10,2);
	//entering into RX mode and CCA mode,channels index decide by CCA.
	QN8065_WriteReg(QN8065_SYSTEM1, 0x12);
	timeOut = 60;  // time out is 300ms
	while(1)
	{
		regValue = QN8065_ReadReg(QN8065_SYSTEM1);        
		//if it seeks a potential channel, the loop will be quited     
		if((regValue & QN8065_CHSC) == 0) break;
		QN8065_Delay(5);   //delay 5ms
		//if it was time out,chip would not normally work.
		if((timeOut--) == 0) return -1;
	}   
	//reading out the rxcca_fail flag of RXCCA status
	isValidChannelFlag = (QN8065_ReadReg(QN8065_STATUS1) & QN8065_RXCCA_FAIL ? 0:1);  
	if(isValidChannelFlag)
	{
#if QN8065_PILOT_CCA
		//set pilot threshold as 1/256
        QN8065_WriteReg(QN8065_CCOND2,0x91);
		QN8065_Delay(100);
		snr = QN8065_ReadReg(QN8065_SNR);
		if(snr< 25)
		{
			isValidChannelFlag = 0;
			for(readCnt=10;readCnt>0;readCnt--)
			{
				QN8065_Delay(2);
				stereoCount += ((QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX) ? 0:1);
				if(stereoCount >= 3) 
				{
					isValidChannelFlag =1;
					break;
				}
			}
		}
		//restore pilot threshold as 2/256
    	QN8065_WriteReg(QN8065_CCOND2,0x92);
		return (isValidChannelFlag);
#else
		return 1;
#endif
	}
	return 0;
}	

/************************************************************************
void QN8065_RXConfigAudio(__u8 volume )
*************************************************************************
Description: config audio 
Parameters:
  volume:control the volume gain,range : 0~47(-47db~0db)    
Return Value:
    None
**********************************************************************/
void QN8065_RXConfigAudio(__u8 volume) 
{
	__u8 regVal;
	
	volume = volume>47 ? 47:volume;
	regVal = QN8065_ReadReg(QN8065_VOL_CTL);
	regVal = (regVal&0xc0)|(volume/6)|((5-volume%6)<<3);
	QN8065_WriteReg(QN8065_VOL_CTL,regVal);                 
}
void QND8065_SetSysMode(__u32 mode) 
{    
    __u8 val;
    switch(mode)        
    {        
        case QND_MODE_SLEEP:                         
            QN8065_WriteReg(QN8065_SYSTEM1, QN8065_ReadReg(QN8065_SYSTEM1)|0x20);         
            break;        
        case QND_MODE_WAKEUP:                             
            QN8065_WriteReg(QN8065_SYSTEM1, QN8065_ReadReg(QN8065_SYSTEM1)&~0x20);    
            QN8065_SetMute(1);
            QN8065_Delay(SLEEP_TO_WAKEUP_DELAY_TIME); //avoid noise from sleep to wakeup mode during.
            QN8065_SetMute(0);        
            break; 
        default:    
            val = (__u8)(mode >> 8);        
            if (val)
            {
                val = val >> 3;
                if(val&0x10)
                {
                    // set to new mode if it's not same as old
                    if((QN8065_ReadReg(QN8065_SYSTEM1) & QN8065_STNBY_RX_MASK) != val)
                    {
                        QN8065_SetMute(1);
                        QN8065_WriteReg(QN8065_SYSTEM1, 0x11); 
                    }
                }    
            }    
            break;        
    }    
}
__s32 fm_vol_set_qn8065(__u8 volume)
{
	__u8  regVal;
	__u8 tmp_vol;
	if(volume>=29)
	{
		tmp_vol = 47;//qn8035 volume range is from 0~47
	}
	else if(volume==0)
	{
		tmp_vol=0;
	}
	else
	{
		tmp_vol=(volume/2)+volume;
	}
	VolValue=tmp_vol;
	QN8065_RXConfigAudio(tmp_vol);
	return EPDK_OK;
}
__s32 fm_vol_get_qn8065(void)
{
	return VolValue;
}
/**********************************************
* function:      fm_iic_write
*
* description:   iic write
*
* notes:         5字节连续写
*
**********************************************/

static void fm_rx_init(void)
{
	return;
}

static void fm_tx_init(void)
{
    return;
}


__s32  fm_init_qn8065(void)//-----------ok
{
	
	//__s32 ret;
	//ret = eLIBs_fioctrl(fiic, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_100KHZ);	
	
	//if(ret == EPDK_OK)
	//{
	//	__msg("set iic clock as 100Khz successfully\n");
	//}
	QN8065_Init();
	QN8065_RXSetTH();
	fm_vol_set(47);
	__wrn("-----init over--------\n");
    return EPDK_OK;
}

__s32  fm_exit_qn8065(void)//--------------------ok
{
	QND8065_SetSysMode(QND_MODE_SLEEP);
	esKRNL_TimeDly(1);
    return EPDK_OK;
}
__s32 fm_play_qn8065(__s32 freq)
{//888	00
	freq=freq/10;
	__wrn("-----fm_play->freq=%d--------\n",freq);
	QN8065_TuneToCH(freq);
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
__s32 fm_manual_search_qn8065(__s32 freq, __u32 search_dir)
{
    return fm_play(freq);

}


__s32  fm_auto_search_qn8065(__s32 freq, __u32 search_dir)
{
    __u8 wfm_data[2] = {0, 0};
    __s32 temp, flag, index = 0;
    __s8 rssi[4] = {0, 0, 0, 0};
	//DebugPrintf("--------fm_auto_search---------\n");
	//DebugPrintf("000--------freq=%d---------\n",freq);
while(1)
{
	if(freq>108000||freq<87000)
		return EPDK_FAIL;
	if(search_dir==DRV_FM_SEARCH_DOWN)
	{
		fm_mute(DRV_FM_VOICE_OFF);
		if(QN8065_RXValidCH(freq/10))
		{
			//DebugPrintf("--------111freq=%d---------\n",freq);
			return freq;
		}
		else
		{
			//DebugPrintf("--------222freq=%d---------\n",freq);
			return freq|0xff000000;
		}
	}
	else
	{
		fm_mute(DRV_FM_VOICE_OFF);
		if(QN8065_RXValidCH(freq/10))
		{
			//DebugPrintf("--------111freq=%d---------\n",freq);
			return freq;
		}
		else
		{
			//DebugPrintf("--------222freq=%d---------\n",freq);
			return freq|0xff000000;
		}
	}
}
  
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
__s32 fm_area_choose_qn8065(__s32 area, void *pbuffer)
{
    __drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

    if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
    {
        rda5820_info.area_select = 1;
        rda5820_info.max_freq = FM_SEARCH_JAP_MAX_FREQ;
        rda5820_info.max_freq = FM_SEARCH_JAP_MIN_FREQ;

        freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
    }
    else if(area == DRV_FM_AREA_CHN_US_EUP)
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
	 rda5820_info.area_select = 0;
        rda5820_info.max_freq = FM_SEARCH_CHN_MAX_FREQ;
        rda5820_info.min_freq = FM_SEARCH_CHN_MIN_FREQ;

        freq_range->fm_area_min_freq = FM_SEARCH_CHN_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_CHN_MAX_FREQ;
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
__s32 fm_mute_qn8065(__s32 voice_onoff)//2=================================
{
	__u8 data;
	if(voice_onoff==DRV_FM_VOICE_ON)
	{
		//DebugPrintf("---DRV_FM_VOICE_ON---\n");
		QN8065_SetMute(0);
	}
	else if(voice_onoff==DRV_FM_VOICE_OFF)
	{
		data=0x1b;//0x10;
		//DebugPrintf("---DRV_FM_VOICE_OFF---\n");
		QN8065_SetMute(1);
	}
    return EPDK_OK;
}
__bool is_online_qn8065(void) 
{
	__u8 temp;
	__u8 i;
	//for(i = 0; i < 3; i++)
	{
	    temp =QN8065_ReadReg(0x06) & 0xFC;
	    __wrn("QN_id=%02x\n",temp);
	    if(temp == CHIPID_QN8065)
	    {
	        return 1;
	    }
	}
	return 0;
}

///////////////////////////////////////5807

#define READ	1
#define WRITE	0

//#define ADRW 	0x20
//#define ADRR 	0x21
#define             _SHARE_CRYSTAL_24MHz_   //共用24MHz晶振，请打开此宏
//#define             _SHARE_CRYSTAL_12MHz_   //共用12MHz晶振，请打开此宏
//#define             _SHARE_CRYSTAL_32KHz_   //共用32KHz晶振，请打开此宏
//#define             _FM_STEP_50K_           //50K步进，请打开此宏

static __u16 gChipID 	= 0;
static __u8 RDA5807P_REG[10];

/***************************************************
RDA5807P
****************************************************/
__u8 RDA5807P_initialization_reg[]={
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xC0, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xC0, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xC0, 0x01, //02H:
#else
    0xC0, 0x01,
#endif

#if defined(_FM_STEP_50K_)
    0x00, 0x12,        
#else
    0x00, 0x10,
#endif
    0x04, 0x00,        
    0x84, 0xAF, //05H: 		0x80~0x8f越小台越多
    0x00, 0x00,        
    0x5E, 0xC6, //07H:		[7:2]值越小台越多  
    0x50, 0x96,        
    0x00, 0x00,        
    0x40, 0x00, //0AH: 
    0x00, 0x8F,        
    0x00, 0x00,        
    0x00, 0x00,        
    0x00, 0x00,        
    0x00, 0x00,        
    0xF0, 0x05, //10H: 
    0x90, 0x00,        
    0xF4, 0x84,        
    0x70, 0x01,        
    0x40, 0xF0,        
    0x21, 0x80, //15H: 
    0x7A, 0xD0,        
    0x3E, 0x40,        
    0x55, 0xA9,        
    0xE8, 0x48,        
    0x50, 0x80, //1AH:
    0x00, 0x00,        
    0x37, 0xB6,        
    0x40, 0x0C,        
    0x07, 0x9B,        
    0x4C, 0x1D,        
    0x81, 0x11, //20H: 
    0x45, 0xC0,        
};

__u8 RDA5807PE_initialization_reg[]={
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xc4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xc4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xc4, 0x01, //02H:
#else
    0xC0, 0x01,
#endif

#if defined(_FM_STEP_50K_)
    0x1b, 0x92,
    0x0C, 0x00,
#else       //Step 100K
    0x00, 0x10,
    0x04, 0x00,
#endif

    0x86, 0xad, //05H:
    0x80, 0x00,
    0x5F, 0x1A, //07H
    0x5e, 0xc6,
    0x00, 0x00,
    0x40, 0x6e, //0AH: 
    0x2d, 0x80,
    0x58, 0x03,
    0x58, 0x04,
    0x58, 0x04,
    0x58, 0x04,
    0x00, 0x47, //10H: 
    0x90, 0x00,
    0xF5, 0x87,
    0x7F, 0x0B, //13H:
	0x04, 0xF1,
	0x5E, 0xc0, //15H: 0x42, 0xc0
    0x41, 0xe0,
    0x50, 0x6f,
    0x55, 0x92,
    0x00, 0x7d,
    0x10, 0xC0,//1AH
    0x07, 0x80,
    0x41, 0x1d,//1CH,
    0x40, 0x06,
    0x1f, 0x9B,
    0x4c, 0x2b,//1FH.
    0x81, 0x10, //20H: 
    0x45, 0xa0,// 21H

#if defined(_FM_STEP_50K_)
    0x55, 0x3F, //22H
#else
    0x19, 0x3F, //22H
#endif

    0xaf, 0x40, 
	0x06, 0x81,
    0x1b, 0x2a, //25H
    0x0D, 0x04,
	0x80, 0x9F, //0x80, 0x2F, 
    0x17, 0x8A,
    0xD3, 0x49,
    0x11, 0x42,
    0xA0, 0xC4, //2BH
	0x3E, 0xBB,
    0x00, 0x00,
    0x58, 0x04,
    0x58, 0x04, //2FH
    0x58, 0x04,
    0x00, 0x74,
    0x3D, 0x00,
    0x03, 0x0C,
    0x2F, 0x68,
    0x38, 0x77, //35H
};

__u8 RDA5807PH_initialization_reg[]={
#if defined(_SHARE_CRYSTAL_24MHz_)
    0xC4, 0x51, //02H:
#elif defined(_SHARE_CRYSTAL_12MHz_)
    0xC4, 0x11, //02H:
#elif defined(_SHARE_CRYSTAL_32KHz_)
    0xC4, 0x01, //02H:
#else
    0xC0, 0x01,
#endif
    0x00,0x10,
    0x04,0x00,
    0x88,0xB0, //05h://86    如打开20H寄存器，可在此调搜台个数,与SP版一样，0X80搜台最多
    0x40,0x00,
    0x7E,0xc6,					//以下寄存器配置不可省去
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,  //0x0ah
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x06,  //0x10h
    0x00,0x19,// //0x00,0x09,//0830
    0x2a,0x11,
    0x00,0x2e,
    0x2a,0x30,
    0xb8,0x3c,  //0x15h
    0x90,0x00,
    0x2a,0x91,
    0x84,0x12,
    0x00,0xa8,
    0xc4,0x00,  //0x1ah
    0xe0,0x00,
    0x30,0x1d,//0x24,0x9d,1ch
    0x81,0x6a,
    0x46,0x08,
    0x00,0x86,  //0x1fh
    0x06,0x61,// 0x16,0x61, 20h  //0X16为打开第05H寄存器，可在05H调搜台数
    0x00,0x00,
    0x10,0x9e,
    0x25,0x4A,//0x24,0Xc9台多  //   0x23,0x46 //0x25,0x4a //0x25,0xCB  //0x26,0x4c台相对少,但清楚台更多，根据客户需求选择  //23h  搜台门限值调整
    0x04,0x08,//0830
    0x0c,0x16,//0x06,0x08,  //0x06,0x08,//0830  //0x25h
    0xe1,0x05,
    0x3b,0x6c,
    0x2b,0xec,
    0x09,0x0f,
    0x34,0x14,  //0x2ah
    0x14,0x50,
    0x09,0x6d,
    0x2d,0x96,
    0x01,0xda,
    0x2a,0x7b,
    0x08,0x21,   //0x30h
    0x13,0xd5,
    0x48,0x91,
    0x00,0xbc,
    0x08,0x96,//0830  0x34h
    0x15,0x3c,
    0x0b,0x80,
    0x25,0xc7,
    0x00,0x00,
};

__u8 RDA5807N_initialization_reg[]={
//#if defined(_SHARE_CRYSTAL_24MHz_)
    0xE4, 0x51, //02H:
//#elif defined(_SHARE_CRYSTAL_12MHz_)
//   0xC4, 0x11, //02H:
//#elif defined(_SHARE_CRYSTAL_32KHz_)
//    0xC4, 0x01, //02H:
//#else
//    0xC0, 0x01,
//#endif
    0x00, 0x10,
    0x04, 0x00,
    0xc4, 0xef, //05h 		0x86 只判断能量杂台可能多一点,  0xc3 能量跟信噪比,效果会更好
    0x60, 0x00,
    0x42, 0x12,//07h
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,  //0x0ah
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,  //0x10h
    0x00, 0x19,
    0x2a, 0x11,
    0xB0, 0x42,
    0x2A, 0x11,
    0xb8, 0x31,  //0x15h
    0xc0, 0x00,
    0x2a, 0x91,
    0x94, 0x00,
    0x00, 0xa8,
    0xc4, 0x00,  //0x1ah
    0xF7, 0xcF,   
    0x24, 0x14,  //0x1ch
    0x80, 0x6F, 
    0x46, 0x08,
    0x00, 0x86,
    0x06, 0x61, //0x20H
    0x00, 0x00,
    0x10, 0x9E,
    0x23, 0xC8,
    0x04, 0x06,
    0x0E, 0x1C, //0x25H
    0xE1, 0x05,
    0xBB, 0x6C,
    0x2B, 0xEC,
};
#define RDA5807_WR_ADDRESS  0x20       ///<RDA5807 写地址
#define RDA5807_RD_ADDRESS  0x21       ///<RDA5807 读地址
__bool OperationRDAFM_2w(__u8 rw,  __u8 *data,__s32 n_byte)
{
	__twi_dev_para_ex_t  piic;        //要写入TEA5767的数据
	__s32 ret;
    __u64 arg[4] = {0,0,0,0};

	eLIBs_memset(&piic, 0, sizeof(__twi_dev_para_ex_t));

	if(rw == 1)
	{		
        ret = iic_read_data((RDA5807_RD_ADDRESS>>1),0xFF,data,n_byte);
		if(ret != EPDK_OK)
		{
			__err("fm i2c read error~\n");
		}
        return ret;
	}
	else
	{
        ret = iic_write_data((RDA5807_WR_ADDRESS>>1),0xFF,data,n_byte);
        if(ret != EPDK_OK)
		{
			__err("fm i2c write error~\n");
		}
        return ret;
	}
	return 1;
}
__bool ReadRDAFM(__u8 addr, __u8 *data,__s32 n_byte)
{
	__s32 ret;
    ret = iic_read_data((RDA5807_RD_ADDRESS>>1),addr,data,n_byte);
    if(ret == EPDK_FAIL)
	{
		__err("fm i2c read error~\n");
	}
    return ret;
}

#define  FM_SEARCH_CHN_MIN_FREQ         87000
#define  FM_SEARCH_CHN_MAX_FREQ        108000

#define  FM_SEARCH_JAP_MIN_FREQ         76000
#define  FM_SEARCH_JAP_MAX_FREQ         91000
/**********************************************
* function:      fm_area_choose
*
* description:   地区选择，区别不同的起始和终止
*                频率
*
* notes:         输入正确地区返回成功，否则失败
*
**********************************************/
__s32 fm_area_choose_rda5807(__s32 area, void *pbuffer)
{
    __drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

    if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
    {
        //tea5767_info.area_select = 1;
       max_freq = FM_SEARCH_JAP_MAX_FREQ / 100;
        min_freq= FM_SEARCH_JAP_MIN_FREQ / 100;
	freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;	
    }
    else if(area == DRV_FM_AREA_CHN_US_EUP)
    {
       // tea5767_info.area_select = 0;
        max_freq = FM_SEARCH_CHN_MAX_FREQ / 100;
        min_freq = FM_SEARCH_CHN_MIN_FREQ / 100;
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
__s32 fm_stereo_choose_rda5807(__s32 audio_method)
{
	if(audio_method == 0){
		RDA5807P_REG[0] &= 0xdf;
	}else{
		RDA5807P_REG[0] |= 0x20;
	}

	OperationRDAFM_2w(WRITE, &(RDA5807P_REG[0]), 2);

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
__s32 fm_mute_rda5807(__s32 voice_onoff)
{
	//__u8 RDA5807P_Muteon[] ={0x80,0x01}; 
	__u8 RDA5807P_Muteon[] ={0x80,0x01}; //for 5807pe(5807ss)
	//__u8 RDA5807P_Muteclose[] ={0xc0,0x01}; 
	//__u8 RDA5807P_Muteclose[] ={0xc0,0x01}; //for 5807pe(5807ss)
	
	//RDA5807N_initialization_reg
	
	if(voice_onoff == DRV_FM_VOICE_OFF)
	{
	    RDA5807P_Muteon[0] = RDA5807N_initialization_reg[0]&0xbf; 
	    RDA5807P_Muteon[1] = RDA5807N_initialization_reg[1]; 
		__msg("%d\n",voice_onoff);
		 OperationRDAFM_2w(WRITE, &(RDA5807P_Muteon[0]), 2);
	}
	else if(voice_onoff == DRV_FM_VOICE_ON)
	{
	    RDA5807P_Muteon[0] = RDA5807N_initialization_reg[0] | 0x40; 
	    RDA5807P_Muteon[1] = RDA5807N_initialization_reg[1]; 
		__msg("%d\n",voice_onoff);
		OperationRDAFM_2w(WRITE, &(RDA5807P_Muteon[0]), 2);
	}

    return EPDK_OK;
}


/**
 * @brief Cover the frequency to channel value
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param __u16 frequency:covered frequency
 * @return __u16: channel value
 * @retval
 */
__u16 RDA5807P_FreqToChan(__u16 n_frequency) 
{
	__u8 channelSpacing = 0;
	__u16 bottomOfBand = 0;
	__u16 channel = 0;

	if ((RDA5807P_REG[3] & 0x0c) == 0x00) 
		bottomOfBand = 870;
	else if ((RDA5807P_REG[3] & 0x0c) == 0x04)	
		bottomOfBand = 760;
	else if ((RDA5807P_REG[3] & 0x0c) == 0x08)	
		bottomOfBand = 760;
	if ((RDA5807P_REG[3] & 0x03) == 0x00) 
		channelSpacing = 1;
	else if ((RDA5807P_REG[3] & 0x03) == 0x01) 
		channelSpacing = 2;

	channel = (n_frequency - bottomOfBand) / channelSpacing;
	return (channel);
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
__s32 fm_play_rda5807(__s32 curFreq)
{   
	__u8 RDA5807P_channel_start_tune[] ={0xc0,0x01,0x00,0x10}; 	//87.0MHz
	__u16 temp = (__u16)(curFreq / 100);
	__u16 curChan;
	__msg("temp = %d\n", temp);
	curChan=RDA5807P_FreqToChan(temp);
	__msg("temp = %d\n", temp);
	//RDA5807P_channel_start_tune[0] = RDA5807N_initialization_reg[0]; //for 5807pe(5807ss)
	//RDA5807P_channel_start_tune[1] = RDA5807N_initialization_reg[1];
	//RDA5807P_channel_start_tune[2]=curChan>>2;
	//RDA5807P_channel_start_tune[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5807P_initialization_reg[3]&0x0f);	//set tune bit
	
	RDA5807P_REG[0] |=	1<<7;
	RDA5807P_REG[2]=curChan>>2;
	RDA5807P_REG[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5807P_REG[3]&0x0f);	//set tune bit

	//OperationRDAFM_2w(WRITE, &(RDA5807P_channel_start_tune[0]), 4);
	OperationRDAFM_2w(WRITE, &(RDA5807P_REG[0]), 4);
    	esKRNL_TimeDly(8);     //Delay five ms

	   return EPDK_OK;
}


/**
 * @brief Station judge for auto search
 * @In auto search mode,uses this function to judge the frequency if has a station
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param int16 freq:frequency value
 * @return bool: if return true,the frequency has a true station;otherwise doesn't have a station
 * @retval
 */
__bool RDA5807P_ValidStop(__u16 freq)
{
	__u8 RDA5807P_reg_data[4]={0};
	__u8 falseStation = 0;
	__u8 i=0;
	__u16 curChan;

	curChan=RDA5807P_FreqToChan(freq);

	//SetNoMute
	RDA5807P_REG[0] |=	1<<7;//6

	RDA5807P_reg_data[0]=RDA5807P_REG[0];
	RDA5807P_reg_data[1]=RDA5807P_REG[1];
	RDA5807P_reg_data[2]=curChan>>2;
	RDA5807P_reg_data[3]=(((curChan&0x0003)<<6)|0x10) | (RDA5807P_REG[3]&0x0f);
	OperationRDAFM_2w(WRITE,&(RDA5807P_reg_data[0]), 4);

	esKRNL_TimeDly(10);    //Dealy 25 ms

    if (0x5808 == gChipID)
    {
        OperationRDAFM_2w(READ,&(RDA5807P_reg_data[0]), 4);
    	//ReadRDAFM(0x0A,RDA5807P_reg_data,4);
    }
    else
    {
    	do
    	{
    		i++;
    		if(i>5) return 0;

    		esKRNL_TimeDly(5);
    		//read REG0A&0B
    		OperationRDAFM_2w(READ,&(RDA5807P_reg_data[0]), 4);
    		//ReadRDAFM(0x0A,RDA5807P_reg_data,4);
    	}while((RDA5807P_reg_data[0]&0x40)==0);
     }

	//check FM_TURE
	if((RDA5807P_reg_data[2] &0x01)==0) falseStation=1;

	if(freq==960) falseStation=1;

	if(freq==900) falseStation=1;

	if (falseStation==1)
		return 0;
	else
		return 1;
}

/**
 * @brief Get the signal level(RSSI) of the current frequency
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param __u16 curf:frequency value
 * @return __u8: the signal level(RSSI)
 * @retval
 */
__u8 RDA5807P_GetSigLvl( __u16 curf )
{    
    __u8 RDA5807P_reg_data[4]={0};	

    OperationRDAFM_2w(READ,&(RDA5807P_reg_data[0]), 4);
    //ReadRDAFM(0x0A,RDA5807P_reg_data,4);

    return  (RDA5807P_reg_data[2]>>1);  /*返回rssi*/
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
/*
__s32 fm_manual_search(__s32 freq, __u32 search_dir)
{
	__u16 temp = (__u16)(freq / 100);
	__wrn("freq = %d\n", temp);
    return fm_play(temp);
}
*/
__s32 fm_manual_search_rda5807(__s32 freq, __u32 search_dir)
{	
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 100);
	__wrn(" freq = %d,  returnValue = %d  , temp = %d\n", freq, returnValue,temp);
	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ/100) || (temp < FM_SEARCH_CHN_MIN_FREQ/100))
		{
			__wrn(" temp = %d\n", temp);
			return EPDK_FAIL;
		}
		returnValue = RDA5807P_ValidStop(temp);
		__wrn(" freq = %d,   returnValue = %d\n", temp, returnValue);
		if (returnValue)
		{
			__wrn(" temp = %d\n", temp);			
			return temp * 100; 				
		}
		else
		{
			__wrn("  (temp * 100)|0xff000000 = 0x%x\n",(temp * 100)|0xff000000);
			return (temp * 100)|0xff000000;
		}
		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			temp--;
			__wrn(" temp = %d\n", temp);
		}
		else//DRV_FM_SEARCH_UP
		{
			temp++;
			__wrn(" temp = %d\n", temp);
		}
	}
	return EPDK_FAIL;
}
__s32  fm_auto_search_rda5807(__s32 freq, __u32 search_dir)
{
	
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 100);
	__wrn(" freq = %d,  returnValue = %d,  temp = %d\n", freq, returnValue,temp);
	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ/100) || (temp < FM_SEARCH_CHN_MIN_FREQ/100))
		{
			__wrn(" temp = %d\n", temp);
			return EPDK_FAIL;
		}
		returnValue = RDA5807P_ValidStop(temp);
		__wrn(" freq = %d\n,returnValue = %d\n", temp, returnValue);
		if (returnValue)
		{
			__wrn(" temp = %d\n", temp);			
			return temp * 100; 				
		}
		else
		{
			__wrn(" (temp * 100)|0xff000000 = 0x%x\n",(temp * 100)|0xff000000);
			return (temp * 100)|0xff000000;
		}
		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			//__wrn(" temp = %d\n", temp);
			temp--;
			__wrn(" temp = %d\n", temp);
		}
		else
		{
			//__wrn(" temp = %d\n", temp);
			temp++;
			__wrn(" temp = %d\n", temp);
		}
	}
	return EPDK_FAIL;
}

/**
 * @brief Set FM volume
 * @It has better use the system volume operation to replace this function
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param __u8 level: volume value
 * @return void
 * @retval
 */
//void RDA5807P_SetVolumeLevel(__u8 level)
//{
//	RDA5807P_initialization_reg[7]=( RDA5807P_initialization_reg[7] & 0xf0 ) | level; 
//	OperationRDAFM_2w(WRITE, &(RDA5807P_initialization_reg[0]), 8);
//}

__s32 fm_vol_set_rda5807(__u8 volume)
{
	__u8  regVal;
	__u8 tmp_vol;
	
	VolValue=volume/2;

	RDA5807P_REG[7] =( RDA5807P_REG[7] & 0xf0 ) | VolValue; 
      RDA5807P_REG[3] &= (~(0x10));

	OperationRDAFM_2w(WRITE, &(RDA5807P_REG[0]), 8);

	return EPDK_OK;
}
__s32 fm_vol_get_rda5807(void)
{
	return VolValue*2;
}

/**
 * @brief Initialize RDA5807P
 * @author RDA Ri'an Zeng
 * @date 2008-11-05
 * @param void
 * @return bool:if true,the operation is successful;otherwise is failed
 * @retval
 */
 /*
__s32  fm_init(void)
{	
	
	__u8 RDA5807P_REG1[]={0x00,0x02};
	__bool error_ind = 0;
	
   	__inf("RDA5807P_Intialization1\n");
	error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_REG1[0], 2);
	__inf("RDA5807P_Intialization1\n");
	esKRNL_TimeDly(2);
	error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_initialization_reg[0], sizeof(RDA5807P_initialization_reg));
	__inf("RDA5807P_Intialization3\n");
	esKRNL_TimeDly(30);       //Dealy 500 ms	
    return EPDK_OK;
}*/

__s32  fm_init_rda5807(void)
{
	__u8 error_ind 				= 0;
	__u8 RDA5807P_REGR[10]		= {0x0};
    	__u8 i 						= 0;
	__s32 ret;
		
    	RDA5807P_REG[0] = 0x00;
    	RDA5807P_REG[1] = 0x02;

	error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_REG[0], 2);
	esKRNL_TimeDly(5);
	error_ind = OperationRDAFM_2w(READ, (__u8 *)&RDA5807P_REGR[0], 10);
	esKRNL_TimeDly(5);

    	gChipID = RDA5807P_REGR[8];
    	gChipID = ((gChipID<<8) | RDA5807P_REGR[9]);

    if (gChipID == 0x5804)
    {
		for(i = 0; i < 8; i++ )
		{
    			RDA5807P_REG[i] = RDA5807PE_initialization_reg[i];
		}
	    	error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807PE_initialization_reg[0], 2);
	    	esKRNL_TimeDly(60); 
	 	error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807PE_initialization_reg[0], sizeof(RDA5807PE_initialization_reg));
    }
    else if (gChipID == 0x5808)//5807MP
    {
		//__here__
		for (i = 0; i < 8; i++)
		{
		    RDA5807P_REG[i] = RDA5807N_initialization_reg[i];
		}
		error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807N_initialization_reg[0], 2);
		esKRNL_TimeDly(60); 
		error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807N_initialization_reg[0], sizeof(RDA5807N_initialization_reg));
    }
    else if (gChipID == 0x5801)
    {
        for (i = 0; i < 8; i++)
        {
            RDA5807P_REG[i] = RDA5807PH_initialization_reg[i];
        }
       error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807PH_initialization_reg[0], 2);
       esKRNL_TimeDly(60); 
		error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807PH_initialization_reg[0], sizeof(RDA5807PH_initialization_reg));
    }
    else
    {
        gChipID = RDA5807P_REGR[4];
        gChipID = ((gChipID<<8) | RDA5807P_REGR[5]);
    }
    
    if ((gChipID == 0x5802) || (gChipID == 0x5803))
    {
	
	//__here__
	       gChipID = 0x5802;

	       for (i=0;i<8;i++)
        	{
            		RDA5807P_REG[i] = RDA5807P_initialization_reg[i];
        	}
	       error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_initialization_reg[0], 2);
	       esKRNL_TimeDly(60); 
	       error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_initialization_reg[0], sizeof(RDA5807P_initialization_reg));
    }

    esKRNL_TimeDly(5);//Dealy 50 ms
	
    if(error_ind )
    {
	//__here__
	   return 0;
    }
    else
    {
	//__here__
	   return 1;
    }
	
}

__s32  fm_exit_rda5807(void)
{
	__u8 RDA5807P_poweroff[] ={0x00,0x00};  
	OperationRDAFM_2w(WRITE, &(RDA5807P_poweroff[0]), 2);
    return EPDK_OK;
}

__bool is_online_rda5807(void)
{
	__u8 error_ind 				= 0;
	__u8 RDA5807P_REGR[10]		= {0x0};
	
	error_ind = OperationRDAFM_2w(WRITE, (__u8 *)&RDA5807P_REG[0], 2);
	esKRNL_TimeDly(1);
	
	error_ind = OperationRDAFM_2w(READ, (__u8 *)&RDA5807P_REGR[0], 10);
	esKRNL_TimeDly(1);

    	gChipID = RDA5807P_REGR[8];
    	gChipID = ((gChipID<<8) | RDA5807P_REGR[9]);
		
	if (gChipID == 0x5801 || gChipID == 0x5802 || gChipID == 0x5803 || gChipID == 0x5804 || gChipID == 0x5808)
	{
		return 1;
	}	
	return 0;
}

////////////////////////////RTC6222
//#define __INTERNAL_SEEK__
#define FM_SEEK_50K_STEP                1

#ifdef __INTERNAL_SEEK__
#define RH_BK0_B_Mono_condition_Marco
#endif

#if FM_SEEK_50K_STEP
#define __RTCFM_STEP_50K__                      //打开宏使用50k步进，关闭使用100k步进
#define __RTCFM_STEP_50K_CHOOSE_BEST_FROM_TW0__
#endif

#define USE_IO_32KHZ_CLK				0
#define USE_SD_IO_32KHZ_CLK			1
#define SHARE_CRYSTAL_32KHz			2
#define USE_IO_12MHZ_CLK				3
#define USE_SD_IO_12MHZ_CLK			4
#define SHARE_CRYSTAL_12MHz			5
#define SHARE_CRYSTAL_24MHz			6
#define RTC6222_FM_CLOCK 						SHARE_CRYSTAL_24MHz

static __s32 fm_write_data_rtc6222(__u8 addr, __u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = iic_write_data((0xc8>>1),addr,data,n_byte);
    if(ret != EPDK_OK)
    {
        __err("fm rtc6222 i2c write error~\n");
    }
    return ret;
}
static __s32 fm_read_data_rtc6222(__u8 addr, __u8 *data,__s32 n_byte)
{
    __s32 ret;
    ret = iic_read_data((0xc8>>1),addr,data,n_byte);
    if(ret == EPDK_FAIL)
    {
        __err("fm rtc6222 i2c read error~\n");
    }
    return ret;
}

void OperationRTC6222_write(__u16 in_data, __u8 addr)
{
	__u8 data[2];

	data[0] = in_data>>8;
	data[1] = in_data;
	fm_write_data_rtc6222(addr,&data[0],2);
	//__wrn("write: addr = 0x%4x,in_data = 0x%4x\n",addr,in_data);
}

__u16 OperationRTC6222_read(__u8 addr)
{
	__u8 data[2]={0};
	__u16 read_data;

	fm_read_data_rtc6222(addr,&data[0],2);
	read_data = data[0];
	read_data = read_data<< 8;
	read_data += data[1];	
	//__wrn("addr = 0x%x  read: read_data = 0x%4x,data[0] = 0x%x,data[1] = 0x%x\n",addr,read_data,data[0],data[1]);	
	return read_data;
}
void delay_5ms(__u8 ms)
{
	esKRNL_TimeDly((ms+1)/2);
}

/*************************************************************
*  Radio get ID
*
*************************************************************/
__u8 is_online_rtc6222(void)
{
	__u16 fm_id;
	__u16 chip_id;
	
	fm_id = OperationRTC6222_read(0x00);
	chip_id = OperationRTC6222_read(0x01);
	if(0x1688 == fm_id && 0x18c0 == chip_id){
	    return 1;
	} else {
	    return 0;
	}
}

void RTC6222_init(void)
{
    OperationRTC6222_write(0x16AA,0x00); //Power off
    delay_5ms(2);

    OperationRTC6222_write(0x96AA,0x00);//Power on
    delay_5ms(2);
    //print_all_reg();

    OperationRTC6222_write(0x4008,0x02);//vol = 0a ,disable mute
    
#ifndef __INTERNAL_SEEK__
#ifdef __RTCFM_STEP_50K__
    OperationRTC6222_write(0x1100,0x04);//enable rclk osc
#else
    OperationRTC6222_write(0x1900,0x04);//enable rclk osc
#endif

    OperationRTC6222_write(0x650f,0x05);//0x650f//set seek noise th and spike th ,0x01h=0010bin,00=80(noise_th),01=144(spike_th),and rssi th=0x0c.

    #if ((RTC6222_FM_CLOCK == USE_IO_32KHZ_CLK) || (RTC6222_FM_CLOCK == USE_SD_IO_32KHZ_CLK) || (RTC6222_FM_CLOCK == SHARE_CRYSTAL_32KHz))
        OperationRTC6222_write(0xB000,0x06);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
    #elif ((RTC6222_FM_CLOCK == USE_IO_12MHZ_CLK) || (RTC6222_FM_CLOCK == USE_SD_IO_12MHZ_CLK) || (RTC6222_FM_CLOCK == SHARE_CRYSTAL_12MHz))
       OperationRTC6222_write(0xB001,0x06);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
    #elif (RTC6222_FM_CLOCK == SHARE_CRYSTAL_24MHz)
        OperationRTC6222_write(0xB005,0x06);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
        OperationRTC6222_write(0xB400,0x07);//bit15:ENABLE IC ,bit12:enable check same channal, bit:2-0set rlck choice one from  32k~38.4M
    #endif


    OperationRTC6222_write(0x043C,0x08);//set seek band of freq  start_freq0x043C=1084. x100khz
    OperationRTC6222_write(0x035C,0x09);//set seek start_freq 0x035C=860, x100khz

    OperationRTC6222_write(0x0B10,0x11);//rclk force 0x0Bh= bin1011


    OperationRTC6222_write(0x1008,0x20);//1008 hopping open is better than 1018 nohopping
    #else //__INTERNAL_SEEK__
    OperationRTC6222_write(0x6000,0x05);// Use internal Clock For Seek UP 20151221
    OperationRTC6222_write(0x0700,0x43);// unkown reson  but just do it.
    OperationRTC6222_write(0x0348,0x09);//set seek top bottom of freq  start_freq0x0348=840. x100khz
    OperationRTC6222_write(0x0465,0x08);//set seek top band of freq  start_freq0x0465=1125. x100khz
    #endif //end of __INTERNAL_SEEK__

    delay_5ms(1);
    OperationRTC6222_write((OperationRTC6222_read(0x06)|0x8000),0x06);      // Write Power On
    delay_5ms(1); // For print request delay time because wait power on finish time
}

void RTC6222_off(void)
{
    OperationRTC6222_write(0x16AA,0x00);
}

/**************************************

RTC6218_Set_Property_FM_Volume()

FM_Volumn: 0~15

***************************************/

void RTC6222_set_vol(__u8 FM_Volumn)
{
    __u16 write_byte = 0x4000|((FM_Volumn)&0x0f);//from uplever FM_Volumn:1~16 so need -1
    OperationRTC6222_write(write_byte,0x02);
}

void RTC6222_mute(__bool is_mute)
{
    __u16 write_byte;

    write_byte = OperationRTC6222_read(0x02);
    if (is_mute)
        write_byte &= ~0x4000;
    else
        write_byte |= 0x4000;
    OperationRTC6222_write(write_byte,0x02);
}


__u8 RTC6222_set_freq(__u16 channel_freq)
{
    __u16 write_byte;
    __u8 timeout = 0;
    __u16 Seek_Status = 0;
    __u16 Seek_Status2 = 0;//no used
    __u16 read_buf; //no used

#ifndef __INTERNAL_SEEK__

#ifndef __RTCFM_STEP_50K__
    channel_freq *= 10;
#endif

    OperationRTC6222_write(0x8000|channel_freq,0x03);

    //New Algorithm part 1=on 0=off by Danny 20150724
    do{
        delay_5ms(8);               // 8 = 40ms(Default) 2 = 10ms(Speed limit) by Danny 20150724
        timeout++;
        Seek_Status = ((0xFC00 & OperationRTC6222_read(0x0B)) >> 10);   //add by Danny 20150702   
       //Print_Reg(1);           //if open this trace log will make more than 30 fake channel and assert restart. so  just for test.
    }while ((Seek_Status == 0) && ((0x4000 & OperationRTC6222_read(0x0A))== 0) && (timeout < 200));

    
    write_byte = channel_freq & (~0x8000);
    OperationRTC6222_write(write_byte,0x03);

    while(0x4000 & OperationRTC6222_read(0x0A)){};

            Seek_Status2 = ((0xFC00 & OperationRTC6222_read(0x0B)) >> 10);   //add by Danny 20150702
        if(Seek_Status == 0)            //When Get Seek_Satus BK0_B[15:10] all 6b' = 0, Return 1. add by Danny 20150703
        {
            return 1;
        }
        else
        {
            return 0;
        }

#else //__INTERNAL_SEEK__
    #ifndef __RTCFM_STEP_50K__
        channel_freq *= 10; // For ax2073b_cola platform version need in RTC6222 20151222
    #endif
    //__wrn("\nChannel_freq= %d\r",channel_freq);

    write_byte = OperationRTC6222_read(0x05);
    write_byte &= (~0x8000);
    OperationRTC6222_write(write_byte,0x05);
    
    write_byte &= (~0x8000);
    OperationRTC6222_write(write_byte,0x03);
    
    OperationRTC6222_write(0x8000|channel_freq,0x03);


    do
    {
        delay_5ms(6);
        read_buf = OperationRTC6222_read(0x0A);
        timeout++;
    } while ((!(0x4000 & read_buf)) && (timeout < 200));

    write_byte = channel_freq & (~0x8000);
    OperationRTC6222_write(write_byte,0x03);
    
    while((0x4000 & OperationRTC6222_read(0x0A)) && (timeout < 200));

    return 0;

#endif //__INTERNAL_SEEK__
}

#ifdef __INTERNAL_SEEK__
u8 RTC6222_seek(u16 channel_freq)
{
    __u16  write_byte;
#ifndef __RTCFM_STEP_50K__
    channel_freq *= 10;
#endif
    write_byte = OperationRTC6222_read(0x05);
    __u16 read_buf;
    __u8 return_value = 0;

    write_byte &= (~0x8000);
    OperationRTC6222_write(write_byte,0x05);

    write_byte |= 0x8000;
    OperationRTC6222_write(write_byte,0x05);

    do
    {
        read_buf = OperationRTC6222_read(0x0A);
        delay_5ms(10);      //time out determined by reg0x04_9:8 seek time 0:40ms 1:160 ms
    }
    while(!(0x4000&read_buf));
    delay_5ms(40); 

    if((0x6000&read_buf) == 0x6000)//must judge at here and not move it at end of fuction
    {
        return_value = 2;       //reach band top limited, all transport 0xff because app level has handled top limited freq like 110Mhz
    }
    else
    {
        return_value = 1;       //tell app lever that need to read cur_freq from driver lever.
    }

#ifdef RH_BK0_B_Mono_condition_Marco
        if((return_value == 1) && (((OperationRTC6222_read(0x0B)) & 0x0200) == 0))  // RH comment No.1 if BK0_B[9]=0'b0, mono station, don't save the channel and do the next seek. or BK0_E:13bit =1 and \BK0_C freq -BK0_EG freq\>100     
        {
            return_value = 0;           
        }
#else
        if((return_value == 1) && (((OperationRTC6222_read(0x0A)) & 0x0400) == 0))  // RH comment No.2 if BK0_A[10]=0'b0, mono station, don't save the channel and do the next seek
        {
            return_value = 0;           
        }   
#endif
    write_byte &= (~0x8000);
    OperationRTC6222_write(write_byte,0x05);

    while(0x4000&OperationRTC6222_read(0x0A));

    return return_value;
}

#else //__INTERNAL_SEEK__
__u8 RTC6222_seek(__u16 channel_freq)
{
    //u16 read_buf;
    __u8 RSSIValue= 0;
    __u8 Status0 = 0;
    __u16 Reg_B = 0x0000;
    __u16 Reg_C = 0x0000;

    #ifndef __RTCFM_STEP_50K__
    channel_freq *= 10;
    #endif

    #ifdef __RTCFM_STEP_50K_CHOOSE_BEST_FROM_TW0__
    __u16 IF_Shift_1 = 0, IF_Shift_2 = 0;
    __u8 Status1 =0;
    __u8 shiftindex=0; //请看函数头的返回值说明，0表示当前上层传给底层的頻点，1表示上层传给底层的頻点再+50k。在搜到台时来告诉上层存台和放音选择正确的頻点。
    #endif
    
    Status0 = RTC6222_set_freq(channel_freq);  // Check Seek_States is 0 ornot by Danny 20150702 

    if(Status0) //Get one ture Channnel
    {
        #ifdef __RTCFM_STEP_50K_CHOOSE_BEST_FROM_TW0__
        if((0x0040 & OperationRTC6222_read(0x0E)) != 0)
        {
            IF_Shift_1 = (~(0x003F & OperationRTC6222_read(0x0E))+1) &0x003F; //2's complement
        }
        else
        {
            IF_Shift_1 = ((0x007F & OperationRTC6222_read(0x0E))); //2's complement
        }        
        Status1 = RTC6222_set_freq(channel_freq+5);       

        if(Status1)
        {
            if((0x0040 & OperationRTC6222_read(0x0E)) != 0)
            {
                IF_Shift_2 = (~(0x003F & OperationRTC6222_read(0x0E))+1) &0x003F; //2's complement
            }
            else
            {
                IF_Shift_2 = ((0x007F & OperationRTC6222_read(0x0E))); //2's complement
            }            
            if(IF_Shift_1 < IF_Shift_2)
            {
                RTC6222_set_freq(channel_freq);
                shiftindex = 0;
            }
            else
            {
                RTC6222_set_freq(channel_freq+5);
                shiftindex = 1;
            }            
        }
        else
        {
            RTC6222_set_freq(channel_freq); //Back Channel 50KHz for Return 1
        }
        RSSIValue = (0x00FF & (OperationRTC6222_read(0x0B)));
        Reg_C = (OperationRTC6222_read(0x0C));
        return 1+shiftindex;
		
        #else
		
        return 1;
		
        #endif    
    }
    else
    {
        return 0;
    }

        Reg_B = (OperationRTC6222_read(0x0B));
        Reg_C = (OperationRTC6222_read(0x0C));
        //Reg_D = (OperationRTC6222_read(0x0D));
        //Reg_E = (OperationRTC6222_read(0x0E));
        RSSIValue = (0x00FF & Reg_B);       
        //Seek_Status = ((0xFC00 & OperationRTC6222_read(0x0B)) >> 10); //add by Danny 20150702
       
}
#endif //__INTERNAL_SEEK__

__u16 RTC6222_read_cur_seeking_freq(void)
{
#ifdef __RTCFM_STEP_50K__
    return (0x7fff&OperationRTC6222_read(0x0C));
#else
    return (0x7fff&OperationRTC6222_read(0x0C)) / 10;
#endif
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
__s32 fm_area_choose_rtc6222(__s32 area, void *pbuffer)
{
	__drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

	if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
	{
		//tea5767_info.area_select = 1;
		max_freq = FM_SEARCH_JAP_MAX_FREQ / 10;
		min_freq = FM_SEARCH_JAP_MIN_FREQ / 10;
		freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
		freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
	}
	else if(area == DRV_FM_AREA_CHN_US_EUP)
	{
		// tea5767_info.area_select = 0;
		max_freq = FM_SEARCH_CHN_MAX_FREQ / 10;
		min_freq = FM_SEARCH_CHN_MIN_FREQ / 10;
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
* function:      fm_mute
*
* description:   静音功能
*
* notes:
*
**********************************************/
__s32 fm_mute_rtc6222(__s32 voice_onoff)
{
	//RTC6222_mute(voice_onoff);
	return EPDK_OK;
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
__s32 fm_play_rtc6222(__s32 curFreq)
{
	RTC6222_set_freq(curFreq/10);
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
__s32 fm_manual_search_rtc6222(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 10);

	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ / 10) || (temp < FM_SEARCH_CHN_MIN_FREQ / 10))
		{
			return EPDK_FAIL;
		}

		returnValue = RTC6222_seek(temp);

		if(returnValue)
		{
			return temp * 10;
		}
		else
		{
			return (temp * 10) | 0xff000000;
		}

		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			temp--;
		}
		else//DRV_FM_SEARCH_UP
		{
			temp++;
		}
	}

	return EPDK_FAIL;
}
__s32  fm_auto_search_rtc6222(__s32 freq, __u32 search_dir)
{
	__bool returnValue = 0;
	__u16 temp = (__u16)(freq / 10);

	while(1)
	{
		if((temp > FM_SEARCH_CHN_MAX_FREQ / 10) || (temp < FM_SEARCH_CHN_MIN_FREQ / 10))
		{
			return EPDK_FAIL;
		}

		returnValue = RTC6222_seek(temp);

		if(returnValue)
		{
			return temp * 10;
		}
		else
		{
			return (temp * 10) | 0xff000000;
		}

		if(search_dir == DRV_FM_SEARCH_DOWN)
		{
			temp--;
		}
		else
		{
			temp++;
		}
	}

	return EPDK_FAIL;
}

__s32 fm_vol_set_rtc6222(__u8 volume)
{
	VolValue=volume;
	RTC6222_set_vol(volume/2);
	return EPDK_OK;
}
__s32 fm_vol_get_rtc6222(void)
{
	return VolValue;
}
__s32  fm_init_rtc6222(void)
{
	/*__s32 ret;
	ret = eLIBs_fioctrl(fiic, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_100KHZ);	
	
	if(ret == EPDK_OK)
	{
		__msg("set iic clock as 100Khz successfully\n");
	}*/
	RTC6222_init();
	return EPDK_OK;
}

__s32  fm_exit_rtc6222(void)
{
	RTC6222_off();
	return EPDK_OK;
}


///////////////////////////////////////bk1080

#define CHIP_DEV_ID 0x80

static __hdle hdl_1, hdl_2;//hdl_1:dat		hdl_2:clk

void I2c_delay(void)
{
	__u32 i;
	for(i=0; i<2048; i++)	//512
		;
}

////////////////////////////////////////   IO   ////////////////////////////////////////////////////////////////////
static void MPIO1_INEN(void)
{
	__u32 is_output = 0;
	
	esPINS_SetPinIO(hdl_1, 0, 0);  

	I2c_delay();
}

static void MPIO1_OUTEN(void)
{
	__u32 is_output = 1;

    esPINS_SetPinIO(hdl_1, 1, 0);

    I2c_delay();
}

static int  MPIO1_INVALUE(void)
{
	__u32 value = 0;
	
	value = esPINS_ReadPinData(hdl_1, NULL);
	
    return value;
}

static void MPIO1_OUTVALUE(__u32 value)
{
    esPINS_WritePinData(hdl_1, value & 0x01, 0);
    I2c_delay();
}

/*static void MPIO2_INEN(void)
{
	__u32 is_output = 0;
	
	OSAL_PIN_GPIO_direction_ops(hdl_2, &is_output, 0);    

	I2c_delay(400);
}*/

static void MPIO2_OUTEN(void)
{
	__u32 is_output = 1;

	//OSAL_GPIO_Set_One_PIN_IO_Status(hdl_2, is_output, 0);
	esPINS_SetPinIO(hdl_2, 1, 0);  

    I2c_delay();
}

/*static int  MPIO2_INVALUE(void)
{
	__u32 value = 0;
	
	OSAL_PIN_GPIO_data_ops(hdl_2, &value, 1);
	
    return value;
}*/

static void MPIO2_OUTVALUE(__u32 value)
{
    //OSAL_GPIO_Write_One_PIN_Value(hdl_2, value & 0x01, 0);
    esPINS_WritePinData(hdl_2, value & 0x01, 0);
	
    I2c_delay();
}


////////////////////////////////////////HV PARALLEL:TD043MGEB1 800_480 ////////////////////////////////////////////////////////////////////


//申请IO,设置IO状态为输出HIGHT
static void  I2C_io_init(__u32 sel)
{	
    return;
	user_gpio_set_t  gpio_set[3];

    gpio_set[0].port = 5;
    gpio_set[0].port_num = 1;
    gpio_set[0].mul_sel = 1;
    gpio_set[0].pull = 1;
    gpio_set[0].drv_level = 1;
    gpio_set[0].data = 0;
    __log("langaojie esPINS_PinGrpReq");
    hdl_1 = esPINS_PinGrpReq(&gpio_set[0], 1);//DAT

    gpio_set[1].port = 5;
    gpio_set[1].port_num = 0;
    gpio_set[1].mul_sel = 1;
    gpio_set[1].pull = 1;
    gpio_set[1].drv_level = 1;
    gpio_set[1].data = 0;
    hdl_2 = esPINS_PinGrpReq(&gpio_set[1], 1);//CLK


	MPIO1_OUTEN();
	MPIO2_OUTEN();

	
	MPIO1_OUTVALUE(1);
	MPIO2_OUTVALUE(1);
}

//设置IO状态为输出LOW,释放IO
static void I2C_io_exit(__u32 sel)
{
return ;
	MPIO1_OUTVALUE(0);
	MPIO2_OUTVALUE(0);

    esPINS_PinGrpRel(hdl_1, 0);
    esPINS_PinGrpRel(hdl_2, 0);
}




//BK1080 initialization value
__u16 HW_Reg[]=
{
	0x0008,
	0x1080,
	0x0201,	
	0x0000,
	0x40C0,
	0x0A1F,
	0x002E,
	0x02FF,
	0x5B11,
	0x0000,
	0x411E,
	0x0000,
	0xCE00,
	0x0000,
	0x0000,
	0x1000,
	0x2dc6,//reg16
	0x8000,//reg17
	0x13FF,
	0x9852,
	0x0000,
	0x0000,
	0x0008,
	0x0000,
	0x51E1,
	0xA8DC,//reg25
	0x2645,
	0x00E4,
	0x1CD8,
	0x3A50,
	0xEAF0,
	0x3000,//31
	0x0200,//	0x0200屏蔽假台
	0x0000,
};

void Delay1us(void)//please revise this function according to your system
{
	//__u16 i;
	//for(i=0;i<10;i++);
	__u32 i;
	for(i=0; i<512; i++)	//512
		;
}
//#define SDA_1   MPIO1_OUTVALUE(1) 
//#define SCL_1   MPIO2_OUTVALUE(1) 

//#define SDA_0   MPIO1_OUTVALUE(0)         
//#define SCL_0   MPIO2_OUTVALUE(0)         


//#define SDA_out MPIO1_OUTEN()
//#define SDA_in  MPIO1_INEN()
//#define SDA     MPIO1_INVALUE()


#define BK_DATA_HIGH()   MPIO1_OUTVALUE(1) //sbit_spi_sdio=1;
#define BK_DATA_LOW()    MPIO1_OUTVALUE(0)//sbit_spi_sdio=0;
#define BK_DATA_READ()   MPIO1_INVALUE() // sbit_spi_sdio
#define SDADIROUT()      MPIO1_OUTEN()
#define SDADIRIN()       MPIO1_INEN()//MPIO1_OUTVALUE(1) //sbit_spi_sdio=1;

#define SCLDIROUT()       
#define BK_CLK_HIGH()    MPIO2_OUTVALUE(1) //sbit_spi_clk=1;
#define BK_CLK_LOW()     MPIO2_OUTVALUE(0) //sbit_spi_clk=0;

/*************************************************
  Function:       BEKEN_I2C_init
  Description:    BEKEN I2C initialize
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_init(void)
{
    SDADIROUT();                  //SDA output
    SCLDIROUT();                  //SCL output
    BK_CLK_HIGH();
    BK_DATA_HIGH();
}

/*************************************************
  Function:       BEKEN_I2C_Start
  Description:    BEKEN I2C transaction start
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_Start(void)
{
    BEKEN_I2C_init();
    Delay1us();
    BK_DATA_LOW();
    Delay1us();
    BK_CLK_LOW();
    Delay1us();
    BK_DATA_HIGH();
}

/*************************************************
  Function:       BEKEN_I2C_Stop
  Description:    BEKEN I2C transaction end
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_Stop(void)
{
    SDADIROUT();
    BK_DATA_LOW();
    Delay1us();
    BK_CLK_HIGH();
    Delay1us();
    BK_DATA_HIGH();
    Delay1us();
}

/*************************************************
  Function:       BEKEN_I2C_ack
  Description:    send ACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_ack(void)
{
    SDADIROUT();

    BK_CLK_LOW();
    BK_DATA_LOW();
    Delay1us();

    BK_CLK_HIGH();
    Delay1us();
    BK_CLK_LOW();
}      

/*************************************************
  Function:       BEKEN_I2C_nack
  Description:    send NACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_nack(void)
{
    SDADIROUT();

    BK_CLK_LOW();
    BK_DATA_HIGH();
    Delay1us();

    BK_CLK_HIGH();
    Delay1us();
    BK_CLK_LOW();
}

/*************************************************
  Function:       BEKEN_I2C_ReceiveACK
  Description:    wait ACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         ack flag
*************************************************/
__u8 BEKEN_I2C_ReceiveACK(void)
{
    __u32 ackflag;
    SDADIRIN();
    Delay1us();
    BK_CLK_HIGH();
    Delay1us();
    ackflag = (__u8)BK_DATA_READ();
    BK_CLK_LOW();
    Delay1us();
    return ackflag;
}

/*************************************************
  Function:       BEKEN_I2C_sendbyte
  Description:    write a byte
  Input:          a byte written
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_sendbyte(__u8 I2CSendData)
{
   
    __u8  i;
    SDADIROUT();
    Delay1us();;
    for(i = 0;i < 8;i++)
    {
        if(I2CSendData & 0x80)
        {
            BK_DATA_HIGH();     //if high bit is 1,SDA= 1
        }
        else
            BK_DATA_LOW();                               //else SDA=0

       	Delay1us();
        BK_CLK_HIGH();
       	Delay1us();
        I2CSendData <<= 1;                          //shift left 1 bit
        BK_CLK_LOW();
    }                       
}

/*************************************************
  Function:       BEKEN_I2c_Readbyte
  Description:    read a byte
  Input:          None
                          
  Output:         None
  Return:         a byte read
*************************************************/
__u8 BEKEN_I2c_Readbyte(void)
{
    __u8 i;
    __u8 ucRDData = 0;                     //return value
    
    SDADIRIN();
    Delay1us();;
    for(i = 0;i < 8;i++)
    {
        BK_CLK_HIGH();
        ucRDData <<= 1;
        Delay1us();
        if(BK_DATA_READ())
            ucRDData|=0x01;
        BK_CLK_LOW();
        Delay1us();
    }
	//DebugPrintf("----BEKEN_I2c_Readbyte ucRDData = %d\n",ucRDData);
    return(ucRDData);
}

/*************************************************
  Function:       BEKEN_I2c_Read
  Description:    read a array
  Input:          reg: register number
                  len:the length of read data   
                          
  Output:         pBuff: a buffer contain read data
  Return:         None
*************************************************/
void BEKEN_I2c_Read(__u8 reg,__u8 *pBuf,__u8 len)
{
    __u8 i;
    __u8 Addr;
        
    Addr=CHIP_DEV_ID;//device address.

    BEKEN_I2C_Start();

    BEKEN_I2C_sendbyte(Addr);   
    BEKEN_I2C_ReceiveACK();

    reg=reg<<1;
    reg|=0x01;
    BEKEN_I2C_sendbyte(reg);
    BEKEN_I2C_ReceiveACK();

    for(i=0;i<len-1;i++)
    {
        pBuf[i] = BEKEN_I2c_Readbyte();
        BEKEN_I2C_ack();
    }
    pBuf[i] = BEKEN_I2c_Readbyte();
    BEKEN_I2C_nack();
    
    BEKEN_I2C_Stop();
 
}

/*************************************************
  Function:       BEKEN_I2c_Write
  Description:    write a array
  Input:          reg: register number
                  pBuff: a buffer contain written data
                  len:the length of written data      
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2c_Write(__u8 reg,__u8 *pBuf,__u8 len)
{
    __u8 i;
    __u8 Addr;

    Addr=CHIP_DEV_ID;//device address.

    BEKEN_I2C_Start();

    BEKEN_I2C_sendbyte(Addr);
    BEKEN_I2C_ReceiveACK();

    reg=reg<<1;
    BEKEN_I2C_sendbyte(reg);   
    BEKEN_I2C_ReceiveACK();
	 
    for(i=0;i<len;i++)
    {
        BEKEN_I2C_sendbyte(pBuf[i]);
        BEKEN_I2C_ReceiveACK(); 
    }
    BEKEN_I2C_Stop();
}


/**************************************************         
Function: EnableWire2()
Parameter: 
	b_enable: 1: wire2;0: wire3;                                         

Description:
	Enable wire2 or wire 3 mode
**************************************************/  
void EnableWire2(__u8 b_enable)
{
	//if(b_enable)
	//	sbit_spi_reset=0;
	//else
	//	sbit_spi_reset=1;
}

/**************************************************         
Function: BK1080_FreqToChan()
Parameter: 
	frequency: FM frequency *10 ,such as  1017,933,1077..                                      

Description:
	change from frequency to channel
**************************************************/ 

__u16 BK1080_FreqToChan(__u16 frequency)
{
	__u16 bottomOfBand;
	__u8 channelSpacing;
	__u16 channel;
	
	bottomOfBand = 875;
	channelSpacing=1;
		
	channel = (frequency - bottomOfBand) / channelSpacing;
	
	return (channel);	

}

/**************************************************         
Function: Chip_wire2_Initialize()
Parameter: 
	None                                         

Description:
	BK1080 I2C mode initialization
**************************************************/ 
void Chip_wire2_Initialize(void)
{
	__u8 index;
	__u8 bk1080_writeData8[68];

	//esKRNL_TimeDly(100);//delay 1s,please revise delay time according to your MCU
	//EnableWire2(1);

	for(index = 0; index < 34; index++)
	{
		bk1080_writeData8[index*2] = (HW_Reg[index] >> 8)&0xff;
		bk1080_writeData8[index*2+1] = (HW_Reg[index])&0xff;
	}

	BEKEN_I2c_Write(0,&(bk1080_writeData8[0]),68);//start from reg2,total 60 byte

	esKRNL_TimeDly(25);//delay 250ms
	bk1080_writeData8[25*2+1]&=0x7f;
	BEKEN_I2c_Write(25,&(bk1080_writeData8[25*2]),2);
	bk1080_writeData8[25*2+1]|=0x80;
	BEKEN_I2c_Write(25,&(bk1080_writeData8[25*2]),2);

	esKRNL_TimeDly(6);//delay 60ms

}

void  bk1080_off(void)
{
	__u8 TmpData8[2];

	BEKEN_I2c_Read(2,TmpData8,2);
	TmpData8[1] = 0x40;
	BEKEN_I2c_Write(2,&TmpData8[0],2);
}


/**************************************************         
Function: BK1080_SetFreq()
Parameter: 
	curFreq: FM frequency *10 ,such as  1017,933,1077..                                      

Description:
	set FM to a frequency
**************************************************/ 
void  BK1080_SetFreq(__u16 curFreq)
{  
	__u16 curChan;
	__u8 TmpData8[4];

	BEKEN_I2c_Read(2,TmpData8,4); //read reg2,with 4 bytes

	curChan=BK1080_FreqToChan(curFreq); /*stop seek and tune*/

	TmpData8[0]&=0xfe; 
	TmpData8[2] =0x0;//reset tune
	TmpData8[3]=curChan&0xff;
	BEKEN_I2c_Write(2,TmpData8,4); //write reg2,with 4 bytes


	TmpData8[2]=(curChan>>8)&0xff;
	TmpData8[2] |= 0x80;//start tune
	TmpData8[3]=curChan&0xff;
	BEKEN_I2c_Write(2,TmpData8,4); //write reg2,with 4 bytes
}


/**************************************************         
Function: BK1080_ValidStop()

Parameter:
	freq:FM frequency *10 ,such as  1017,933,1077...  
	start_freq:lowest frequency,in China,start_freq=875
Description:
	check a frequency is a valid.
Return:
	1:ok
	0:false	
**************************************************/
__u8 BK1080_ValidStop(__u16 freq,__u16 start_freq)
{	
	static __u16 last_tuned_freq=0;//static variable
	static __u16 g_last_freq_deviation_value=0;
	__u16 cur_freq_deviation;
	__u8 TmpData8[8];

	BK1080_SetFreq(freq);

	esKRNL_TimeDly(7);//delay 70ms

////////////////////////////////////
//new added 2009-05-30
	BEKEN_I2c_Read(0x07,TmpData8,2);//start from reg 0x7,with 2bytes

	cur_freq_deviation=TmpData8[0];
	cur_freq_deviation<<=8;
	cur_freq_deviation|=TmpData8[1];
	cur_freq_deviation=cur_freq_deviation>>4;

////////////////////////////////////

	BEKEN_I2c_Read(7,TmpData8,8);//start from reg7,with 8bytes
	
	if(TmpData8[6]&0x10)    //check AFCRL bit12
	{
		last_tuned_freq=freq;//save last tuned freqency
		g_last_freq_deviation_value=cur_freq_deviation;
		return 0;//false
	}

	if(TmpData8[7]<5) //RSSI<10                //5    5~10	越大台越少
	{
		last_tuned_freq=freq;//save last tuned freqency
		g_last_freq_deviation_value=cur_freq_deviation;
		return 0;//false
	}

	if( (TmpData8[1]&0xf) <2) //SNR<2                //2	1~3 越大台越少
	{
		last_tuned_freq=freq;//save last tuned freqency
		g_last_freq_deviation_value=cur_freq_deviation;
		return 0;//false
	}

//add frequency devation check

	if( (cur_freq_deviation>=280)&&(cur_freq_deviation<=(0xfff-280)))//              200~300	越大台越多
	{
		last_tuned_freq=freq;//save last tuned freqency
		g_last_freq_deviation_value=cur_freq_deviation;

		return 0;
		
	}


//new added 2009-05-30
	if( (freq>start_freq)&&( (freq-last_tuned_freq)==1) )
	{
		if(g_last_freq_deviation_value&0x800)
		{
			last_tuned_freq=freq;//save last tuned freqency
			g_last_freq_deviation_value=cur_freq_deviation;
			return 0;		
		}

		if(g_last_freq_deviation_value<50)
		{
			last_tuned_freq=freq;//save last tuned freqency
			g_last_freq_deviation_value=cur_freq_deviation;
			return 0;
		}

	}

	if( (freq>=start_freq)&&( (last_tuned_freq-freq)==1) )
	{
		if( (g_last_freq_deviation_value&0x800)==0)
		{
			last_tuned_freq=freq;//save last tuned freqency
			g_last_freq_deviation_value=cur_freq_deviation;
			return 0;
		}

		if(g_last_freq_deviation_value>(0xfff-50) )
		{
			last_tuned_freq=freq;//save last tuned freqency
			g_last_freq_deviation_value=cur_freq_deviation;
			return 0;
		}

	}

	
	last_tuned_freq=freq;//save last tuned freqency
	g_last_freq_deviation_value=cur_freq_deviation;


	return 1; //OK

}


/**************************************************         
Function: BK1080_SearchByTune()

Parameter:	 
	searched_freq:a buffer for search frequency list
Description:
	search all the frequency.
Return:
	the number of the searched frequency.
**************************************************/
__u8 BK1080_SearchByTune(__u16 * searched_freq)
{
	__u16 freq;
	__u8 i=0;

	for(freq=875;freq<1080;freq++)
	{
		if(BK1080_ValidStop(freq,875))
		{
			searched_freq[i++]=freq;
		}
	}
	
	return i;

}




/********************************************************/



////////////////////////////////////////////





__s32 fm_vol_set_bk1080(__u8 volume)
{
#if 0
	__u8 TmpData8[2];

	VolValue=volume;
	BEKEN_I2c_Read(5,TmpData8,2); //read reg5,with 2 bytes
	
	TmpData8[1] = volume/2;
	BEKEN_I2c_Write(5,&(TmpData8[0]),2);	
#endif
	return EPDK_OK;
}
__s32 fm_vol_get_bk1080(void)
{
	return VolValue;
}

__u16 fm_read_id_bk1080(void)//待验证
{
	__u8 TmpData8[4];
	__u16 fm_id;	

	BEKEN_I2c_Read(1,TmpData8,2); //read reg2,with 4 bytes
	fm_id = (TmpData8[0]&0x00ff);
	fm_id = (fm_id<<8) + (TmpData8[1]&0x00ff);
	//DebugPrintf("fm_id = 0x%x\n",fm_id);

	return fm_id;
}

__bool is_online_bk1080(void)
{
	//I2C_io_init(1);
		
	if(fm_read_id_bk1080() == 0x1080)	
		return 1;
	else
		return 0;
}

__s32  fm_init_bk1080(void)//-----------ok
{
	
	//__s32 ret;
	//ret = eLIBs_fioctrl(fiic, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_100KHZ);	
	
	//if(ret == EPDK_OK)
	//{
	//	__msg("set iic clock as 100Khz successfully\n");
	//}
	if(hdl_1 == NULL)
	{
#if 0
		I2C_io_init(1);
#endif
	}
	Chip_wire2_Initialize();
	
	__wrn("-----init over--------\n");
    return EPDK_OK;
}

__s32  fm_exit_bk1080(void)//--------------------ok
{
	bk1080_off();
	//esKRNL_TimeDly(1);
    return EPDK_OK;
}
__s32 fm_play_bk1080(__s32 freq)
{
	freq=freq/100;
	__wrn("-----fm_play->freq=%d--------\n",freq);
	BK1080_SetFreq(freq);
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
__s32 fm_manual_search_bk1080(__s32 freq, __u32 search_dir)
{
    return fm_play_bk1080(freq);

}


__s32  fm_auto_search_bk1080(__s32 freq, __u32 search_dir)
{
    __u8 wfm_data[2] = {0, 0};
    __s32 temp, flag, index = 0;
    __s8 rssi[4] = {0, 0, 0, 0};
	//DebugPrintf("--------fm_auto_search---------\n");
	//DebugPrintf("--------freq=%d---------\n",freq);
	while(1)
	{
		if(freq>108000||freq<87000)
			return EPDK_FAIL;
		if(search_dir==DRV_FM_SEARCH_DOWN)
		{
			fm_mute(DRV_FM_VOICE_OFF);
			if(BK1080_ValidStop(freq/100,870))
			{
				//DebugPrintf("--------111freq=%d---------\n",freq);
				return freq;
			}
			else
			{
				//DebugPrintf("--------222freq=%d---------\n",freq);
				return freq|0xff000000;
			}
		}
		else
		{
			fm_mute(DRV_FM_VOICE_OFF);
			if(BK1080_ValidStop(freq/100,870))
			{
				//DebugPrintf("--------111freq=%d---------\n",freq);
				return freq;
			}
			else
			{
				//DebugPrintf("--------222freq=%d---------\n",freq);
				return freq|0xff000000;
			}
		}
	}
	  
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
__s32 fm_area_choose_bk1080(__s32 area, void *pbuffer)
{
    __drv_fm_area_freq_t   *freq_range = (__drv_fm_area_freq_t *)pbuffer;

    if((area == DVR_FM_AREA_JAPAN) || (area == DRV_FM_AREA_CHN_SCHOOL))
    {
       max_freq = FM_SEARCH_JAP_MAX_FREQ/100;
        min_freq= FM_SEARCH_JAP_MIN_FREQ/100;
	freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;	
    }
    else if(area == DRV_FM_AREA_CHN_US_EUP)
    {
        max_freq = FM_SEARCH_CHN_MAX_FREQ/100;
        min_freq = FM_SEARCH_CHN_MIN_FREQ/100;
	 freq_range->fm_area_min_freq = FM_SEARCH_CHN_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_CHN_MAX_FREQ;	
    }
    else
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}



/////////////////////////fm all
__u8 fm_type = 0;
#define FM_RTC6222		1
#define FM_RTC6207E		2
#define FM_QN8065		3
#define FM_RDA5807		4
#define FM_BK1080		5	//两线模式,使硬件IIC失效
__s32  fm_init(void)
{
	__s32 ret;	
	__u64 arg[4] = {0,0,0,0};
	//ret = eLIBs_fioctrl(fiic, TWI_SET_SCL_CLOCK, 0, (void *)TWI_SCLK_100KHZ);	
	//arg[0] = 0;
	//arg[1] = TWI_SCLK_100KHZ;
	//ret = eLIBs_fioctrl(fiic, TWI_SET_SCL_CLOCK,(void *)arg);//langaojie 		
	//if(ret == EPDK_OK)
	//{
	//	__wrn("set iic clock as 100Khz successfully\n");
	//}

	//read id
	if(is_online_rtc6222()){
		fm_type = FM_RTC6222;
 	}else if(is_online_rtc6207e()){
		fm_type = FM_RTC6207E;
	}else if(is_online_qn8065()){
		fm_type = FM_QN8065;
	}else if(is_online_rda5807()){
		fm_type = FM_RDA5807;
	}
	#if 0//langaojie temp test
	else if(is_online_bk1080())
	{
		fm_type = FM_BK1080;
	}
	#endif
	else{
		return -1;
	}
	__log("----------- fm_type = %d----------\n",fm_type);

	//init
	if(fm_type == FM_RTC6222)
		fm_init_rtc6222();		
	else if(fm_type == FM_RTC6207E)
		fm_init_rtc6207m();		
	else if(fm_type == FM_QN8065)
		fm_init_qn8065();
	else if(fm_type == FM_RDA5807)
		fm_init_rda5807();
#if 0//langaojie temp test
	else if(fm_type == FM_BK1080)
		fm_init_bk1080();
#endif
	else
		return -1;
	__log("fm init ok");
	return 0;
}

__s32  fm_exit(void)
{
	if(fm_type == FM_RTC6207E)
		fm_exit_rtc6207m();		
	else if(fm_type == FM_QN8065)
		fm_exit_qn8065();	
	else if(fm_type == FM_RDA5807)
		fm_exit_rda5807();
	else if(fm_type == FM_RTC6222)
		fm_exit_rtc6222();
	else if(fm_type == FM_BK1080)
		fm_exit_bk1080();
	
    	return EPDK_OK;
}
__s32  fm_play(__s32 freq)
{
	if(fm_type == FM_RTC6207E)
		fm_play_rtc6207m(freq);		
	else if(fm_type == FM_QN8065)
		fm_play_qn8065(freq);	
	else if(fm_type == FM_RDA5807)
		fm_play_rda5807(freq);
	else if(fm_type == FM_RTC6222)
		fm_play_rtc6222(freq);
	else if(fm_type == FM_BK1080)
		fm_play_bk1080(freq);
	
    	return EPDK_OK;	
}
__s32  fm_auto_search(__s32 freq, __u32 search_dir)
{
	if(fm_type == FM_RTC6207E)
		return fm_auto_search_rtc6207m(freq,search_dir);		
	else if(fm_type == FM_QN8065)
		return fm_auto_search_qn8065(freq,search_dir);	
	else if(fm_type == FM_RDA5807)
		return fm_auto_search_rda5807(freq,search_dir);
	else if(fm_type == FM_RTC6222)
		return fm_auto_search_rtc6222(freq,search_dir);
	else if(fm_type == FM_BK1080)
		return fm_auto_search_bk1080(freq,search_dir);
	
    	return EPDK_FAIL;
}
__s32  fm_manual_search(__s32 freq, __u32 search_dir)
{
	if(fm_type == FM_RTC6207E)
		fm_manual_search_rtc6207m(freq,search_dir);		
	else if(fm_type == FM_QN8065)
		fm_manual_search_qn8065(freq,search_dir);
	else if(fm_type == FM_RDA5807)
		fm_manual_search_rda5807(freq,search_dir);
	else if(fm_type == FM_RTC6222)
		fm_manual_search_rtc6222(freq,search_dir);
	else if(fm_type == FM_BK1080)
		fm_manual_search_bk1080(freq,search_dir);
	
    	return EPDK_OK;	
}
__s32  fm_area_choose(__s32 area, void *pbuffer)
{
	if(fm_type == FM_RTC6207E)
		fm_area_choose_rtc6207m(area,pbuffer);		
	else if(fm_type == FM_QN8065)
		fm_area_choose_qn8065(area,pbuffer);
	else if(fm_type == FM_RDA5807)
		fm_area_choose_rda5807(area,pbuffer);
	else if(fm_type == FM_RTC6222)
		fm_area_choose_rtc6222(area,pbuffer);
	else if(fm_type == FM_BK1080)
		fm_area_choose_bk1080(area,pbuffer);
	
    	return EPDK_OK;	
}
__s32  fm_mute(__s32 voice_onoff)
{
	if(fm_type == FM_RTC6207E)
		fm_mute_rtc6207m(voice_onoff);		
	else if(fm_type == FM_QN8065)
		fm_mute_qn8065(voice_onoff);	
	else if(fm_type == FM_RDA5807)
		fm_mute_rda5807(voice_onoff);
	else if(fm_type == FM_RTC6222)
		fm_mute_rtc6222(voice_onoff);
	//else if(fm_type == FM_BK1080)
	//	fm_mute_bk1080(voice_onoff);
	
    	return EPDK_OK;
}
__s32 fm_vol_set(__u8 volume)
{
	if(fm_type == FM_RTC6207E)
		fm_vol_set_rtc6207m(volume);		
	else if(fm_type == FM_QN8065)
		fm_vol_set_qn8065(volume);	
	else if(fm_type == FM_RDA5807)
		fm_vol_set_rda5807(volume);
	else if(fm_type == FM_RTC6222)
		fm_vol_set_rtc6222(volume);
	else if(fm_type == FM_BK1080)
		fm_vol_set_bk1080(volume);
	
    	return EPDK_OK;
}
__s32 fm_vol_get(void)
{
	if(fm_type == FM_RTC6207E)
		return fm_vol_get_rtc6207m();		
	else if(fm_type == FM_QN8065)
		return fm_vol_get_qn8065();	
	else if(fm_type == FM_RDA5807)
		return fm_vol_get_rda5807();
	else if(fm_type == FM_RTC6222)
		return fm_vol_get_rtc6222();
	else if(fm_type == FM_BK1080)
		return fm_vol_get_bk1080();
	
    	return EPDK_FAIL;
}
__s32  fm_get_status(void)
{
    	return FM_DRV_STATUS_RECEIVE;
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
__s32  fm_signal_level(__s32 signal_level)
{
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
__s32  fm_send(__s32 freq)
{
    	return EPDK_OK;	
}
__s32  fm_send_on(void)
{
    	return EPDK_OK;
}
__s32  fm_send_off(void)
{
    	return EPDK_OK;
}
__s32  fm_pa_gain(__u8 pagain)
{
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
    return EPDK_OK;
}
__s32 fm_signal_gain(__u8 sigain)
{   
	return EPDK_OK;
}

#endif

