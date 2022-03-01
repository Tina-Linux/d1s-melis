#ifndef _FMALLDRIVER_H
#define _FMALLDRIVER_H
#define FMALL


#include  "drv_fm_i.h"

/**********************************QN8065's clock source selection**************
1.QN8065's default clock source is 32768HZ.
2.setting QN8065's clock source and clock source type(like sine-wave clock or digital clock).
3.user need to modify clock source according to actual hardware platform.
4.clock formula,the details please refer to the QN8065's datasheet 
  XTAL_DIV = Round(Clock/32768);
  PLL_DLT = Round((28800000*512*XTAL_DIV)/Clock)-446464
*******************************************************************************/
#define QN8065_SINE_WAVE_CLOCK             		0x00    //inject sine-wave clock  
#define QN8065_DIGITAL_CLOCK               		0x80    //inject digital clock,default is inject digital clock
//crystal clock is 24MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0xdc
#define QN8065_XTAL_DIV1_VAL                   	0x6a
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
/*                                             	
//crystal clock is 32768HZ                     	
#define QN8065_XTAL_DIV0_VAL                   	0x01
#define QN8065_XTAL_DIV1_VAL                   	0x80
#define QN8065_XTAL_DIV2_VAL                   	0x6e
                                               	
//crystal clock is 4MHZ                        	
#define QN8065_XTAL_DIV0_VAL                   	0x7a
#define QN8065_XTAL_DIV1_VAL                   	0x68
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 6MHZ                        	
#define QN8065_XTAL_DIV0_VAL                   	0xb7
#define QN8065_XTAL_DIV1_VAL                   	0x68
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 7.3728MHZ                   	
#define QN8065_XTAL_DIV0_VAL                   	0xe1
#define QN8065_XTAL_DIV1_VAL                   	0x80
#define QN8065_XTAL_DIV2_VAL                   	0x6e
                                               	
                                               	
//crystal clock is 8MHZ                        	
#define QN8065_XTAL_DIV0_VAL                   	0xf4
#define QN8065_XTAL_DIV1_VAL                   	0x68
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 10MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0x31
#define QN8065_XTAL_DIV1_VAL                   	0x69
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 11.0592MHZ                  	
#define QN8065_XTAL_DIV0_VAL                   	0x52
#define QN8065_XTAL_DIV1_VAL                   	0x59
#define QN8065_XTAL_DIV2_VAL                   	0x83
                                               	
//crystal clock is 12MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0x6e
#define QN8065_XTAL_DIV1_VAL                   	0x69
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 16MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0xe8
#define QN8065_XTAL_DIV1_VAL                   	0x69
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 20MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0x62
#define QN8065_XTAL_DIV1_VAL                   	0x6a
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 24MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0xdc
#define QN8065_XTAL_DIV1_VAL                   	0x6a
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 26MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0x19
#define QN8065_XTAL_DIV1_VAL                   	0x6b
#define QN8065_XTAL_DIV2_VAL                   	0x66
                                               	
//crystal clock is 27MHZ                       	
#define QN8065_XTAL_DIV0_VAL                   	0x38
#define QN8065_XTAL_DIV1_VAL                   	0xf3
#define QN8065_XTAL_DIV2_VAL                   	0x6e
*/

//QN8065 chip ID
#define CHIPID_QN8065                   		0x8c


/*******************************************************************************
Performance configuration 
*******************************************************************************/
#define QN8065_SMSTART_VAL                     	12
#define QN8065_HCCSTART_VAL                    	18
#define QN8065_SNCSTART_VAL                    	51

/*******************************************************************************
limitation configuration 
*******************************************************************************/    
#define QN8065_CH_SETUP_DELAY_TIME             	300 
#define SLEEP_TO_WAKEUP_DELAY_TIME      		500
/*******************************************************************************
 definition register 
*******************************************************************************/
#define QN8065_SYSTEM1         					0x00
#define QN8065_VOL_CTL                         	0x01
#define QN8065_SNR            					0x02
#define QN8065_RSSISIG         					0x03
#define QN8065_STATUS1         					0x04
#define QN8065_CID1            					0x05
#define QN8065_CID2            					0x06
#define	QN8065_CH								0x07
#define	QN8065_CH_START							0x08
#define	QN8065_CH_STOP							0x09
#define	QN8065_CH_STEP							0x0a
#define QN8065_CCA                             	0x0b
#define	QN8065_CCA_SNR_TH   					0x0c
#define	QN8065_CCA_CNT 					    	0x0d
#define	QN8065_XTAL_DIV0						0x0e
#define	QN8065_XTAL_DIV1						0x0f
#define	QN8065_XTAL_DIV2						0x10
#define QN8065_SMP_HLD_THRD						0x11
#define	QN8065_RXAGC_GAIN						0x12
#define	QN8065_SYSTEM_CTL1						0x13
#define QN8065_REG_TEST							0x14
#define QN8065_RXAGC1                          	0x15
#define QN8065_RXAGC2                          	0x16
#define QN8065_RXAGC3                          	0x17
#define QN8065_RXAGC4                          	0x18
#define	QN8065_CCA1								0x19
#define	QN8065_CCA2								0x1a
#define QN8065_CCA_TIME                        	0x1b
#define QN8065_CCS_NOISE                       	0x1c
#define QN8065_IFCNT1                          	0x1d
#define QN8065_IFCNT2                          	0x1e
#define QN8065_CCA_SNR                         	0x1f
#define QN8065_CCOND1                          	0x20
#define QN8065_CCOND2                          	0x21
#define QN8065_NSM                             	0x22
#define QN8065_NHCC                            	0x23
#define QN8065_HYSTERSIS                       	0x24
#define	QN8065_SMSTART							0x25
#define	QN8065_SNCSTART							0x26
#define	QN8065_HCCSTART							0x27
#define QN8065_RC_CAL_TH                       	0x28
#define QN8065_REG_ADC                         	0x29
#define QN8065_REG_REF                         	0x2a
#define QN8065_REG_PD                          	0x2b
#define QN8065_REG_FLL                         	0x31
/***************************************End************************************/

/*******************************************************************************
 definition operation bit of register
*******************************************************************************/
#define QN8065_CCA_CH_DIS      					0x01
#define QN8065_CHSC            					0x02
#define QN8065_RDSEN           					0x08
#define QN8065_CH_CH		    				0x03
#define QN8065_CH_CH_START     					0x0c
#define QN8065_CH_CH_STOP      					0x30
#define QN8065_STNBY_MODE           		    0x20
#define QN8065_RX_MODE         					0x10
#define QN8065_IMR             					0x40
#define QN8065_ST_MO_RX                        	0x01
#define QN8065_STNBY_RX_MASK                   	0x30
#define QN8065_RXCCA_MASK                      	0x03
#define QN8065_RX_CCA                          	0x02
#define QN8065_RXCCA_FAIL                      	0x08
#define QN8065_RX_MONO                         	0x04
                         	
#define QN8065_CCA_NSNR_20MS                   	(0x00 << 2)
#define QN8065_CCA_NSNR_40MS                   	(0x01 << 2)
#define QN8065_CCA_NSNR_60MS                   	(0x02 << 2)
#define QN8065_CCA_NSNR_100MS                  	(0x03 << 2)
                                        	
#define QN8065_CCA_NRSSI_10MS                  	(0x00 << 4)
#define QN8065_CCA_NRSSI_20MS                  	(0x01 << 4)
#define QN8065_CCA_NRSSI_40MS                  	(0x02 << 4)
                                	
#define QN8065_CCA_NRSSI_60MS                  	(0x03 << 4)
                               	
#define QN8065_CCA_NAGC_10MS                   	(0x00 << 6)
#define QN8065_CCA_NAGC_20MS                   	(0x01 << 6)
#define QN8065_CCA_NAGC_40MS                   	(0x02 << 6)
                               	
#define QN8065_CCA_NAGC_60MS                   	(0x03 << 6)

/***************************************End************************************/

#define QND_MODE_SLEEP                  0     
#define QND_MODE_WAKEUP                 1


#define QN8065_GetMonoMode() \
        QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX        
#define QN8065_GetRssi() \
        QN8065_ReadReg(QN8065_RSSISIG)
#define QNM_GetChipNumber()\
        QN8065_ReadReg(QN8065_CID2) & 0xfc        
        // eg: 
        // if(QNM_GetChipNumber() == CHIPID_QN8065) 
        // {
        //    // QN8065 processing
        // }
        
extern void QN8065_Init(void);
extern void QN8065_TuneToCH(__u32 ch);
extern void QN8065_RXConfigAudio(__u8 volume);
extern __u8 QN8065_ScanNoiseFloor(__u32 start,__u32 stop);
extern void QN8065_RXSetTH(void);
extern __s32 QN8065_RXValidCH(__u32 freq); 


#define  FM_MAX_FREQ               108000
#define  FM_MIN_FREQ                87000

typedef struct __FM_SETTING_INFO
{
    __s32   min_freq;
    __s32   max_freq;
    __s8    area_select;
    __s8    stereo_select;
    __s8    signal_level;
    __s8    reserved;

}__fm_setting_info_t;

extern   __s32  fm_init(void);
extern   __s32  fm_exit(void);
extern   __s32  fm_play(__s32 freq);
extern   __s32  fm_auto_search(__s32 freq, __u32 search_dir);
extern   __s32  fm_manual_search(__s32 freq, __u32 search_dir);
extern   __s32  fm_area_choose(__s32 area, void *pbuffer);
extern   __s32  fm_stereo_choose(__s32 audio_method);
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_mute(__s32 voice_onoff);
extern   __s32 fm_vol_set(__u8 volume);
extern   __s32 fm_vol_get(void);
extern   __s32  fm_signal_level(__s32 signal_level);

extern   __s32  fm_send_on(void);
extern   __s32  fm_send(__s32 freq);
extern   __s32  fm_send_off(void);
extern   __s32  fm_pa_gain(__u8 pagain);

extern   __s32  fm_get_status(void);


#endif
