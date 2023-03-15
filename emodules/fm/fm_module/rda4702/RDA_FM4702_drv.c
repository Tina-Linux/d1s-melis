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
#include "epdk.h"
#include "RDA_FM4702_drv.h"
#include  "drv_fm_i.h"

#if(FM_MODULE_TYPE == FM_MODULE_TYPE_RDA4702)
const __u16 FM_SI47XX_DEVICE_ADDR = 0x20;
const __u16 FM_SI47XX_WRITE = 0x00;
const __u16 FM_SI47XX_READ = 0x01;
/*** For writing ***/
/*** REG 0x02 POWERCFG ***/
const __u16 SI_XOSCEN   = 0x8000;
const __u16 SI_AHIZEN   = 0x4000;
const __u16 SI_DMUTE    = 0x4000;
const __u16 SI_MUTE  = 0x0000;
const __u16 SI_MONO     = 0x2000;

const __u16 SI_SKMODE = 0x0400;

const __u16 SI_SEEKUP = 0x0200;
const __u16 SI_SEEKDOWN = 0x0000;
const __u16 SI_SEEK     = 0x0100;
const __u16 SI_SEEK_OFF = 0x0000;
const __u16 SI_DISABLE = 0x0040;
const __u16 SI_ENABLE = 0x0001;

/*** REG 0x03 CHANNEL ***/
const __u16 SI_TUNE = 0x8000;

/*** REG 0x04 SYSCONFIG1 ***/
const __u16 SI_RDSIEN = 0x8000;
const __u16 SI_STCIEN = 0x4000;
const __u16 SI_RDS = 0x1000;

const __u16 SI_DE_50US = 0x0800; // 50us for Europe and Australia
const __u16 SI_GPIO3_INTERRUPT = 0x0010;

const __u16 SI_AGCD_DIS = 0x0400;
const __u16 SI_GPIO2_INTERRUPT = 0x0004;

const __u16 SI_GPIO2_HIGH = 0x000B;
const __u16 SI_GPIO2_LOW = 0x0008;

/*** REG 0x05 SYSCONFIG2 ***/
const __u16 SI_RECOMMENDED_SEEKTH = 0x0800;

const __u16 SI_BAND                     = 0x00C0;

const __u16 SI_BAND_US          = 0x0000; //87.5-108MHz

const __u16 SI_WIDEBAND_JAPAN = 0x0040; //76-108MHZ

const __u16 SI_BAND_JAPAN       = 0x0080; //76-90MHz
const __u16 SI_SPACE                    = 0x0030;
const __u16 SI_SPACE_200KHZ = 0x0000;
const __u16 SI_SPACE_100KHZ = 0x0010;
const __u16 SI_SPACE_50KHZ      = 0x0020;
const __u16 SI_VOLUME           = 0x000F;

/*** For reading ***/
/*** REG 0x0A STATUSRSSI ***/
const __u16 SI_RDSR = 0x8000;
const __u16 SI_STC = 0x4000;
const __u16 SI_SF = 0x2000;

const __u16 SI_AFCRL = 0x1000;
const __u16 SI_RDSE = 0x0E00;

const __u16 SI_ST = 0x0100;
const __u16 SI_RSSI = 0x00FF;

/*** REG 0x0B READCHAN ***/
const __u16 SI_READCHAN = 0x03FF;

/*** REG 0x00 DEVICEID ***/
const __u16 SI_PN = 0xF000;
const __u16 SI_MFGID = 0x0FFF;

/*** REG 0x01 CHIPID ***/
const __u16 REV = 0xFC00;
const __u16 DEV = 0x0200;
const __u16 FIRMWARE = 0x01FF;

/*** User defined Variables ***/
//static __u16 AUTOSEEK=FALSE;
static __u8 WriteRegArray[32];
static __u8 ReadRegArray[32];
const __u16 SI_BottomOfChannel = 0x0000;
__u8 VolumeCtrl;
const __u8 NORVOL = 0x08;
const __u8 MAXVOL = 0x0F;
const __u8 MINVOL = 0x00;
//add by xzd
const __u16 SKSNR_HIGH = 0xf0;
const __u16 SKCNT_HIGH = 0x0f;
const __u16 SKCNT_NORMAL = 0x40;
const __u16 SKSNR_NORMAL = 0x08;
const __u16 SKSNR_LOW  = 0x10;
const __u16 SKCNT_LOW  = 0x10;
// static __u16 SeekedChannels[40];
//static __u8 MaxSeekedChannels=40;
//static Boolean AUTOSEEK=FALSE;
// static __u8 MaxTuneIndex=0;
//static __u8 TuneIndex=0;
static __u16 BandSelect;
static __u16 SpacingSelect;
__u8 ChannelIndex = 0;
const __u8 SEEKTH_TO_VALUE_LIST[] =
{
    0x19, 0x1A, 0x1B, 0x1C,
    0x1D, 0x1E, 0x1F, 0x20
};

#define  FM_SI470X_ADDR          (0x20>>1)

static   __fm_setting_info_t       si470x_info = {87500, 108000, 0, 0, 3, 0};


static   __u32 delaycounter = 0;

unsigned char OperationSi47XX_2w(unsigned char mode, unsigned char *buf, unsigned char len) //mode = 1 read;mode = 0 write
{
    __twi_dev_para_ex_t  piic;
    __s32 ret = EPDK_FAIL;
    __u8 reg_addr = 0;
    buf[0] &= (~0x40);              //打开静音功能
    piic.slave_addr_flag = 0;
    piic.slave_addr      = FM_SI470X_ADDR;
    piic.byte_addr_width = 0;
    piic.byte_addr       = &reg_addr;
    piic.data            = buf;
    piic.byte_count      = len;
    ret = eLIBs_fioctrl(fiic, mode, 0, (void *)&piic);

    if (ret == EPDK_FAIL)
    {
        //FAIL 处理
        __wrn("IIC WRITE ERR\n");
        return EPDK_FAIL;
    }

    //__wrn("IIC WRITE OK\n");
    return EPDK_OK;
}

/*******************************************************************************
 * Function:      FM_SI47XX_WriteReg
 *
 * Parameters:    RegAddr, representing address of register of FM_SI47XX from 02h to 05h
 *
 * __u16 InputData, content of register for writing in FM_SI47XX
 *
 * Returns:       Fail or ok
 *
 * Description:   Routine to write content into registers in FM_SI47XX
 ******************************************************************************/
__s32 FM_SI47XX_WriteReg(WRITE_REG_ADDR RegAddr, __u16 InputData)
{
    __u8 NumOfBytesToWrite;
    __twi_dev_para_ex_t iic_para = {0};
    __s32 ret = EPDK_FAIL;
    __u8 reg_addr = 0;
    NumOfBytesToWrite = RegAddr * 2;
    WriteRegArray[RegAddr * 2 - 2] = ((InputData & 0xFF00) >> 8);
    WriteRegArray[RegAddr * 2 - 1] = (InputData & 0x00FF);
    iic_para.slave_addr         = FM_SI470X_ADDR; //设备地址，值为7bit地址，不含读写标志位
    iic_para.slave_addr_flag    = 0;
    iic_para.byte_addr          = &reg_addr;    //如果访问设备的寄存器地址不是8位的话，需要传入最高byte的地址， 这里以8位为例，所以直接取地址
    iic_para.byte_addr_width    = 0;            //设备寄存器地址的长度，以byte为单位，这里以8位为例，所以取1, 如果>1，填所需长度，最大到3
    iic_para.data               = WriteRegArray;         //数据buffer
    iic_para.byte_count         = NumOfBytesToWrite;          //写入数据长度
    ret = eLIBs_fioctrl(fiic, TWI_WRITE_SPEC_RS,  0, (void *)&iic_para);

    if (ret == EPDK_FAIL)
    {
        //FAIL 处理
        __msg("IIC WRITE ERR\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
/*******************************************************************************
 * Function:      FM_SI47XX_ReadReg
 *
 * Parameters:    RegAddr, representing address of register of FM_SI47XX from 00h to 0Ah
 *
 * Returns:       none
 *
 * Description:  Routine to read content into registers in FM_SI47XX
 ******************************************************************************/
__u16 FM_SI47XX_ReadReg(READ_REG_ADDR RegAddr)
{
    __u8 NumOfBytesToRead;
    __twi_dev_para_ex_t iic_para = {0};
    __s32 ret = EPDK_FAIL;
    __u8 reg_addr = 0;
    NumOfBytesToRead = RegAddr * 2;
    iic_para.slave_addr         = FM_SI470X_ADDR; //设备地址，值为7bit地址，不含读写标志位
    iic_para.slave_addr_flag    = 0;
    iic_para.byte_addr          = &reg_addr;    //如果访问设备的寄存器地址不是8位的话，需要传入最高byte的地址， 这里以8位为例，所以直接取地址
    iic_para.byte_addr_width    = 0;            //设备寄存器地址的长度，以byte为单位，这里以8位为例，所以取1, 如果>1，填所需长度，最大到3
    iic_para.data               = ReadRegArray;         //数据buffer
    iic_para.byte_count         = NumOfBytesToRead;          //读取数据长度
    //如果不是标准iic，即不发restart的读操作
    ret = eLIBs_fioctrl(fiic, TWI_READ_SPEC_RS,  0, (void *)&iic_para);

    if (ret == EPDK_FAIL)
    {
        //FAIL 处理
        __msg("IIC READ ERR\n");
        return 0;
    }

    ret = ((ReadRegArray[RegAddr * 2 - 2] << 8) | ReadRegArray[RegAddr * 2 - 1]);
    return ret;
}
static __u32 hpiob;
static __u32 PIOB5_STATUS;
void ENABLE_PIOB_OUTPUT()
{
#if 0
    //mpio3_pio = esPINS_PIOReq(PIOC_PIOGRP_B,DDR_MPIO3,PIOC_OUTPUT_EN|PIOC_INPUT_EN);
    hpiob = esPINS_PIOReq(PIOC_PIOGRP_B, 1 << 5, PIOC_OUTPUT_EN);
    esPINS_PIOPull(hpiob, PIOC_PULL_ENABLE);
#endif
}

void PIOB5_OUTPUT0()
{
#if 0
    esPINS_PIODataOut(hpiob, 0);
#else
    //PA0 pull down
    __hdle           hdl;
    user_gpio_set_t  gpio_set[1];
    gpio_set->port = 1;
    gpio_set->port_num = 0;
    gpio_set->mul_sel = 1;
    gpio_set->pull = 1;
    gpio_set->drv_level = 1;
    gpio_set->data = 0;
    hdl = OSAL_GPIO_Request(gpio_set, 1);
    OSAL_GPIO_Release(hdl, 2);
#endif
}

void PIOB5_OUTPUT1()
{
#if 0
    esPINS_PIODataOut(hpiob, 1);
#else
    //PA0 pull up
    __hdle hdl;
    user_gpio_set_t  gpio_set[1];
    gpio_set->port = 1;
    gpio_set->port_num = 0;
    gpio_set->mul_sel = 1;
    gpio_set->pull = 1;
    gpio_set->drv_level = 1;
    gpio_set->data = 1;
    hdl = OSAL_GPIO_Request(gpio_set, 1);
    OSAL_GPIO_Release(hdl, 2);
#endif
}

void fm_reset_on(void)
{
    //__msg("FM SI470x Reset\n");
    // __msg("FM SI470x Reset\n");
    //   __msg("FM SI470x Reset\n");
    //     __msg("FM SI470x Reset\n");
    /*
       //kaiwei,hongyi
        ES_FILE *pfile;
        pfile = eLIBs_fopen("b:\\HID\\POWER", "r+");
        //Pull Low
        eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_2, (void *)((POWER_EPT_MOS_N) | (POWER_EPT_GATE_ON)));
        eLIBs_fclose(pfile);
    */
    //guanyu
    ENABLE_PIOB_OUTPUT();
    PIOB5_OUTPUT0();
}

void fm_reset_off(void)
{
    //  __msg("FM SI470x Reset Off\n");
    /*
        //kaiwei,hongyi
        ES_FILE *pfile;
        pfile = eLIBs_fopen("b:\\HID\\POWER", "r+");
        //Pull High
        eLIBs_fioctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_2, (void *)((POWER_EPT_MOS_P) | (POWER_EPT_GATE_ON)));
        eLIBs_fclose(pfile);
    */
    //guanyu
    ENABLE_PIOB_OUTPUT();
    PIOB5_OUTPUT1();
}


/*******************************************************************************
 * Function:      FM_SI47XX_ReadChannel
 *
 * Parameters:    none
 *
 * Returns:       __u16
 *
 * Description:  Routine to return current available channel on FM_SI47XX
 ******************************************************************************/
__u16 FM_SI47XX_ReadChannel(void)
{
    return FM_SI47XX_ReadReg(R0Bh);
}

__s32 fm_get_status(void)
{
    __s32 ret = EPDK_OK;
    return ret;
}
/*******************************************************************************
 * Function:      FM_SI47XX_Init
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to initialize FM_SI47XX
 ******************************************************************************/
/*** Initialization of FM_SI47XX module ***/
void FM_SI47XX_Init(void)
{
    VolumeCtrl = MAXVOL;
    BandSelect = SI_BAND_US;
    SpacingSelect = SI_SPACE_100KHZ;
    fm_reset_on();
    esKRNL_TimeDly(5);
    fm_reset_off();
    esKRNL_TimeDly(5);
}


/* Quanta modify by Jacky.Sun 20051202-00000100-*****-G80 */
/*******************************************************************************
 * Function:      FM_SI47XX_PowerUp
 *
 * Parameters:    Boolean  bRDSEnable, enable RDS function
 *
 * Returns:       none
 *
 * Description:  Routine to turn on FM_SI47XX
 ******************************************************************************/

void FM_SI47XX_PowerUp(void)
{
#if 0
    //  unsigned char RSSI=0;
    int i = 0;
    int id = 0;
    id = FM_SI47XX_ReadReg(R00h);
    __msg("R00h: 0x%x\n", id);
    //id = FM_SI47XX_ReadReg(R01h);
    //__msg("before power up R01h: 0x%x\n", id );

    for (i = 1; i < 32; i++)
    {
        WriteRegArray[i] = 0x00;
        ReadRegArray[i] = 0x00;
    }

    FM_SI47XX_WriteReg(W07h, SI_XOSCEN | 0x0100); //8100
    esKRNL_TimeDly(50); //Wait 800ms for oscillator startup
    FM_SI47XX_WriteReg(W02h, SI_DMUTE | SI_ENABLE);//4001
    esKRNL_TimeDly(1);
    FM_SI47XX_WriteReg(W04h, SI_STCIEN); //4004
    esKRNL_TimeDly(1);
#else
    WriteRegArray[0] = 0x00;
    WriteRegArray[1] = 0x00;
    WriteRegArray[2] = 0x00;
    WriteRegArray[3] = 0x00;
    WriteRegArray[4] = 0x00;
    WriteRegArray[5] = 0x00;
    WriteRegArray[6] = 0x00;
    WriteRegArray[7] = 0x00; //Band: 87.5~108 SPACE: 100k VOLUME = 0dB
    WriteRegArray[8] = 0x00;
    WriteRegArray[9] = 0x00;
    WriteRegArray[10] = 0x81;
    OperationSi47XX_2w(0, WriteRegArray, 11);
    esKRNL_TimeDly(60);
    WriteRegArray[0] = 0x40;
    WriteRegArray[1] = 0x01;
    WriteRegArray[2] = 0x00;
    WriteRegArray[3] = 0x00;
    WriteRegArray[4] = 0x90;
    WriteRegArray[5] = 0x04;
    WriteRegArray[6] = 0x0c;
    WriteRegArray[7] = 0x1f; //Band: 87.5~108 SPACE: 100k VOLUME = 0dB
    WriteRegArray[8] = 0x00;
    WriteRegArray[9] = 0x48;
    OperationSi47XX_2w(0, WriteRegArray, 10);
    esKRNL_TimeDly(2);
    //send CMD
    //FM_SI47XX_WriteReg(W05h, 0x4800);
#endif
    //FM_SI47XX_WriteReg(W05h,SI_RECOMMENDED_SEEKTH | BandSelect | SpacingSelect | VolumeCtrl);
    //esKRNL_TimeDly(1);                                      //地区频段设置
    //FM_SI47XX_WriteReg(W06h,SKSNR_NORMAL|SKSNR_NORMAL);     //信号强度中
    //FM_SI47XX_WriteReg(W06h,SKSNR_LOW|SKCNT_LOW);           //信号强度低
    //FM_SI47XX_WriteReg(W06h,SKSNR_HIGH|SKCNT_HIGH);         //信号强度高
    //id = FM_SI47XX_ReadReg(R01h);
    //__msg("after power up R01h: 0x%x\n", id );
}
__s32  fm_init(void)
{
    //__msg("fm_init here\n");
    //__msg("fm_init here\n");
    //__msg("fm_init here\n");
    //__msg("fm_init here\n");
    FM_SI47XX_Init();
    FM_SI47XX_PowerUp();
    return EPDK_OK;
}


/*******************************************************************************
 * Function:      ReadSTC
 *
 * Parameters:    none
 *
 * Returns:       __u8
 *
 * Description:  Routine to return STC bit in FM_SI47XX
 ******************************************************************************/
__u8 ReadSTC(void)
{
    esKRNL_TimeDly(5);

    if (FM_SI47XX_ReadReg(R0Ah) & SI_STC)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
 * Function:      ReadSF
 *
 * Parameters:    none
 *
 * Returns:       __u8
 *
 * Description:  Routine to return SF bit in FM_SI47XX
 ******************************************************************************/
__u8 ReadSF(void)
{
    esKRNL_TimeDly(6);

    if (FM_SI47XX_ReadReg(R0Ah) & SI_SF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
 * Function:      ReadST
 *
 * Parameters:    none
 *
 * Returns:       __u8
 *
 * Description:  Routine to return ST(stereo) bit in FM_SI47XX/01
 ******************************************************************************/
__u8 ReadST(void)
{
    if (FM_SI47XX_ReadReg(R0Ah) & SI_ST)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
 * Function:      CfgFM_SI47XXRstAsOutput
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to configure GPIO53 on AD6527
 ******************************************************************************/

void CfgFM_SI47XXRstAsOutput(void)
{
    //DlGpioConfigure(FM_RST); // pull to high
}

/*******************************************************************************
 * Function:      ReadRDS
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to read RDS bit on FM_SI47XX/01
 ******************************************************************************/

__u8 ReadRDS(void)
{
    if (FM_SI47XX_ReadReg(R04h) & SI_RDS)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*******************************************************************************
 * Function:      ReadRDSR
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to read RDSR bit on FM_SI47XX/01
 ******************************************************************************/

__s32 ReadRDSR(void)
{
    if (FM_SI47XX_ReadReg(R0Ah) & SI_RDSR)
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}
/*******************************************************************************
 * Function:      ReadRDSE
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to read RDSE bits on FM_SI47XX/01
 ******************************************************************************/

__s32 ReadRDSE(void)
{
    return ((FM_SI47XX_ReadReg(R0Ah) & SI_RDSE) >> 9);
}
/*******************************************************************************
 * Function:      ReadRDS_BLOCK
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to read RDSA, RDSB, RDSC and RDSD on FM_SI47XX/01
 ******************************************************************************/

__u16 ReadRDS_BLOCK(__s32 block)
{
    __u16 reg_val;

    switch (block)
    {
        case 0:
            reg_val = FM_SI47XX_ReadReg(R0Ch);
            break;

        case 1:
            reg_val = FM_SI47XX_ReadReg(R0Dh);
            break;

        case 2:
            reg_val =  FM_SI47XX_ReadReg(R0Eh);
            break;

        case 3:
            reg_val = FM_SI47XX_ReadReg(R0Fh);
            break;

        default:
            reg_val  = 0;
            break;
    }

    return reg_val;
}
/*******************************************************************************
 * Function:      FM_SI47XX_ReadRSSI
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to read RSSI bits on FM_SI47XX/01
 ******************************************************************************/

__u8 FM_SI47XX_ReadRSSI(void)
{
    return ((FM_SI47XX_ReadReg(R0Ah) & SI_RSSI));
}
/* End of modification by Brian 20051111-00001100-02083-G80 */
/*******************************************************************************
 * Function:      FM_SI47XX_ChanToFreq
 *
 * Parameters:    Channel- channel on FM radio, its  value starts at 0x0000
 *
 * Returns:       __u16
 *
 * Description:  Routine to translate channel to frequency value
 ******************************************************************************/
__u16 FM_SI47XX_ChanToFreq(__u16 Channel)
{
    __u16 Info;
    __u16 BottomOfFreq;
    Info = WriteRegArray[W05h * 2];
    /* 100KHZ as one unit */

    if ((Info & SI_BAND) == SI_BAND_US)
    {
        BottomOfFreq = 875;
    }
    else
    {
        BottomOfFreq = 760;
    }

    if ((Info & SI_SPACE) == SI_SPACE_200KHZ)
    {
        return (BottomOfFreq + (Channel << 1));
    }
    else if ((Info & SI_SPACE) == SI_SPACE_100KHZ)
    {
        return (BottomOfFreq + Channel);
    }
    else
    {
        return (BottomOfFreq + (Channel >> 1));
    }
}


__u16 FM_SI47XX_ReadFreq(void)
{
    return (FM_SI47XX_ChanToFreq(FM_SI47XX_ReadReg(R0Bh) & SI_READCHAN));
}

/*******************************************************************************
 * Function:      FM_SI47XX_RST_LOW
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to set GPIO53 on AD6527 as low voltage
 ******************************************************************************/

void FM_SI47XX_RST_LOW(void)
{
    //M_DLGpioAssertLine(M_DLGpioLine(FM_RST));
}
/*******************************************************************************
 * Function:      FM_SI47XX_RST_HIGH
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to set GPIO53 on AD6527 as high voltage
 ******************************************************************************/

void FM_SI47XX_RST_HIGH(void)
{
    //M_DLGpioDeassertLine(M_DLGpioLine(FM_RST));
}
/*******************************************************************************
 * Function:      FM_SI47XX_2Wire
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to configure FM_SI47XX to operate in 2-wire mode
 ******************************************************************************/
void FM_SI47XX_2Wire(void)
{
}

/*******************************************************************************
 * Function:      FM_SI47XX_OpenRDS
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to turn on RDS function of SI4701
 ******************************************************************************/

void FM_SI47XX_OpenRDS(void)
{
    FM_SI47XX_WriteReg(W04h, FM_SI47XX_ReadReg(R04h) | SI_RDS);
}
/*******************************************************************************
 * Function:      FM_SI47XX_CloseRDS
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to turn off RDS function of SI4701
 ******************************************************************************/

void FM_SI47XX_CloseRDS(void)
{
    FM_SI47XX_WriteReg(W04h, FM_SI47XX_ReadReg(R04h) & 0xCC3F);
}


__s32 fm_open_rds(void)
{
    FM_SI47XX_CloseRDS();
    return EPDK_OK;
}

__s32 fm_close_rds(void)
{
    FM_SI47XX_CloseRDS();
    return EPDK_OK;
}

__s32 fm_rds_rdsr(void)
{
    __s32 ret;
    ret = ReadRDSR();
    return ret;
}
__s32 fm_rds_rdse(void)
{
    __s32 ret;
    ret = ReadRDSE();
    return ret;
}
__s32 fm_rds_block(__s32 block)
{
    __s32 ret;
    ret = ReadRDS_BLOCK(block);
    return ret;
}
/* End of modification by Brian 20051111-00001100-02083-G80 */

/*******************************************************************************
 * Function:      FM_SI47XX_SetBand
 *
 * Parameters:    band- set 0 for changing band to USA,EUROPE
 *
 *                                                        set 1 for changing band to JAPAN
 *
 * Returns:       none
 *
 * Description:  Routine to set FM band

 ******************************************************************************/
void FM_SI47XX_SetBand(__u8 band)
{
    if (band == 1)
    {
        BandSelect = SI_BAND_US;
    }
    else
    {
        BandSelect = SI_BAND_JAPAN;
    }

    FM_SI47XX_WriteReg(W05h, SI_RECOMMENDED_SEEKTH | BandSelect | SpacingSelect | VolumeCtrl);
}

/*******************************************************************************
 * Function:      FM_SI47XX_SetSpacing
 *
 * Parameters:    spacing- set 0 for changing channel spacing to 200KHZ
 *
 *                                                           set 1 for changing channel spacing to 100KHZ
 *
 *                                                           set 2 for changing channel spacing to 50KHZ
 *
 * Returns:       none
 *
 * Description:  Routine to set FM band
 ******************************************************************************/
void FM_SI47XX_SetSpacing(__u8 spacing)
{
    switch (spacing)
    {
        case 0:
            SpacingSelect = SI_SPACE_200KHZ;
            break;

        case 1:
            SpacingSelect = SI_SPACE_100KHZ;
            break;

        case 2:
            SpacingSelect = SI_SPACE_50KHZ;
            break;

        default:
            break;
    }

    FM_SI47XX_WriteReg(W05h, SI_RECOMMENDED_SEEKTH | BandSelect | SpacingSelect | VolumeCtrl);
}

/* Quanta modify by Brian 20051111-00001100-02083-G80 */

/*******************************************************************************
 * Function:      FM_SI47XX_SetSeekDepth
 *
 * Parameters:    SeekDepth
 *
 *                                                        set 1 for changing band to JAPAN
 *
 * Returns:       none
 *
 * Description:  Routine to set seeking depth of FM channel
 ******************************************************************************/

void FM_SI47XX_SetSeekDepth(__u8 SeekDepth)
{
    FM_SI47XX_WriteReg(W05h, (SeekDepth << 8) | BandSelect | SpacingSelect | VolumeCtrl);
}



/*******************************************************************************
 * Function:      FM_SI47XX_PowerDown
 *
 * Parameters:    none
 *
 * Returns:       none
 *
 * Description:  Routine to turn off FM_SI47XX
 ******************************************************************************/

void FM_SI47XX_PowerDown(void)
{
    __msg("radio-si470x: power down\n");
#if 0
    /*** Set DISABLE bit=1, perform internal power-down sequence and set ENABLE bit=0 later by device itself ***/
    FM_SI47XX_WriteReg(W02h, SI_ENABLE | SI_DISABLE);
#else
    PIOB5_OUTPUT0();
#endif
}




__s32  fm_exit(void)
{
    FM_SI47XX_PowerDown();
    return EPDK_OK;
}


unsigned char input[12];
unsigned char output[32];
char Si4702_FM_Seek_Start(__u32 seek_mode,
                          unsigned char channel_space,
                          unsigned short *pChannel_Freq,
                          unsigned char *SeekFail,
                          unsigned char *valid_channel)
{
    unsigned short freq_reg_data, loop_counter = 0;
    input[0] = 0x41;
#if 1

    switch (seek_mode)
    {
        case DRV_FM_SEARCH_DOWN://SEEKDOWN_HALT:
        {
            input[0] = 0x45;
            break;
        }

        //case SEEKDOWN_WRAP:
        //{
        //  input[0] = 0x41;
        //  break;
        //}
        case DRV_FM_SEARCH_UP://SEEKUP_HALT:
        {
            input[0] = 0x47;
            break;
        }

            //case SEEKUP_WRAP:
            //{
            //  input[0] = 0x43;
            //  break;
            //}
    }

#endif
    //input[0] = 0x47;
    //set seek bit
    OperationSi47XX_2w(0, &(input[0]), 1);

    //wait STC=1
    do
    {
        OperationSi47XX_2w(1, &(output[0]), 4);
        loop_counter++;
        esKRNL_TimeDly(2);//delay 20ms
        //display freq
        freq_reg_data = ((output[2] << 8) | output[3]) & 0x3ff;
        *pChannel_Freq = 8750 + freq_reg_data * (channel_space / 10);
        __msg("%d\n", *pChannel_Freq);
    } while (((output[0] & 0x40) == 0) && (loop_counter < 0xffff)); //for loop_counter, when seek, the loop time must > 12s at the worst case

    //60ms*((108-87.5)/0.1+1)= 12s
    if (loop_counter == 0xffff)
    {
        return 1;
    }

    loop_counter = 0;

    if ((output[0] & 0x20) != 0)
    {
        *SeekFail = 1;
    }
    else
    {
        *SeekFail = 0;
    }

    //you can check AFC Rail here
    if ((output[0] & 0x10) != 0)
    {
        *valid_channel = 0;
    }
    else
    {
        *valid_channel = 1;
    }

    input[0] = 0x40;
    //clear seek bit
    OperationSi47XX_2w(0, &(input[0]), 1);

    //wait STC=0
    do
    {
        OperationSi47XX_2w(1, &(output[0]), 1);
        loop_counter++;
    } while (((output[0] & 0x40) != 0) && (loop_counter < 0xff));

    if (loop_counter >= 0xff)
    {
        return 1;
    }

    //you can read REG0A&0B for channel number or RSSI here
    OperationSi47XX_2w(1, &(output[0]), 4);
    freq_reg_data = ((output[2] << 8) | output[3]) & 0x3ff;
    *pChannel_Freq = 8750 + freq_reg_data * (channel_space / 10);
    return 0;
}

char Si4702_FM_Seek(__u32 seek_mode,
                    unsigned char channel_space,
                    unsigned short *pChannel_Freq,
                    unsigned char *SeekFail)
{
    unsigned char valid_channel;
    unsigned short loop_counter = 0;

    do
    {
        if (Si4702_FM_Seek_Start(seek_mode, channel_space, pChannel_Freq, SeekFail, &valid_channel) != 0)
        {
            return 1;
        }

        loop_counter++;
    } while ((valid_channel == 0) && (loop_counter < 0xff) && (*SeekFail == 0));

    if (loop_counter >= 0xff)
    {
        return 1;
    }

    return 0;
}

char Si4702_FM_Tune_Freq(unsigned short channel_freq, unsigned char channel_space)
{
    unsigned short freq_reg_data, loop_counter = 0;
    input[0] = 0x40;    //107.7MHz
    input[1] = 0x01;
    input[2] = 0x80;
    input[3] = 0xCA;
    //set tune bit
    freq_reg_data = (channel_freq - 8750) / (channel_space / 10);
    input[3] = freq_reg_data & 0xff;
    input[2] = input[2] | (freq_reg_data >> 8);
    OperationSi47XX_2w(0, &(input[0]), 4);

    //wait STC=1
    do
    {
        OperationSi47XX_2w(1, &(output[0]), 2);
        loop_counter++;
        esKRNL_TimeDly(2);//delay 20ms
    } while (((output[0] & 0x40) == 0) && (loop_counter < 0x7f));   //for loop_counter, when tune, the loop time must > 60ms

    if (loop_counter == 0x7f)
    {
        return 1;
    }

    loop_counter = 0;
    input[0] = 0x40;
    input[1] = 0x01;
    input[2] = 0x00;
    //clear tune bit
    OperationSi47XX_2w(0, &(input[0]), 4);

    //wait STC=0
    do
    {
        OperationSi47XX_2w(1, &(output[0]), 2);
        loop_counter++;
    } while (((output[0] & 0x40) != 0) && (loop_counter < 0x7f));

    if (loop_counter >= 0x7f)
    {
        return 1;
    }

    return 0;
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
    __u16 SeekedChannel;
    __s32 scanok;
    //__inf("\n**********************auto start************************\n");
    //__msg("input freq: %d\n", freq);

    if (freq > si470x_info.max_freq)
    {
        return -1;
    }

    if (si470x_info.area_select == 1)
    {
        if (freq < 76000)
        {
            freq = 76000;
        }

        SeekedChannel = ((freq - 76000) / 100);
    }
    else
    {
        if (freq < 87000)
        {
            freq = 87000;
        }

        if (freq < 87500 && freq >= 87000)
        {
#if 1

            if (freq == 87100)
            {
                FM_SI47XX_WriteReg(W03h, 0 | SI_TUNE);
                delaycounter = 0;

                while (ReadSTC() != 1 && delaycounter < 250)
                {
                    delaycounter++;
                }

                esKRNL_TimeDly(5);
                FM_SI47XX_WriteReg(W03h, 0);
            }

#endif
            return freq | 0xff000000;
        }

        SeekedChannel = ((freq - 87500) / 100);
    }

#if 0
    /*** Set the fm default value***/
    FM_SI47XX_WriteReg(W02h, 0x0000);
    esKRNL_TimeDly(4);
    /*** Set the begin channel***/
    FM_SI47XX_WriteReg(W03h, SeekedChannel);
    SeekedChannel = FM_SI47XX_ReadChannel();

    if (si470x_info.area_select == 1)
    {
        freq  = SeekedChannel * 100 + 76000;
        __msg("real pre search %d \n", freq);
    }
    else
    {
        freq  = SeekedChannel * 100 + 87500;
        __msg("real pre search %d \n", freq);
    }

    /*** Write address 0x02 to set SEEK and SEEKUP bit ***/

    if (search_dir == DRV_FM_SEARCH_DOWN)
    {
        FM_SI47XX_WriteReg(W02h, SI_DMUTE | SI_SEEK | SI_SEEKDOWN | SI_ENABLE);
        //__msg("seek down\n");
    }
    else
    {
        FM_SI47XX_WriteReg(W02h, SI_DMUTE | SI_SEEK | SI_SKMODE | SI_SEEKUP);
        //__msg("seek up\n");
    }

    /*** Read channel status after seeking at address 0x0B and clear ***/
    delaycounter = 0;

    while (ReadSTC() != 1 && delaycounter < 250)
    {
        delaycounter++;
    }

    esKRNL_TimeDly(20);

    if (ReadSF() == 1)
    {
        __msg("Seek failure\n\n", freq);
        scanok = FALSE;
    }
    else
    {
        /*** Clear STC and SF bit by setting SEEK bit to low ***/
        scanok = TRUE;
    }

    esKRNL_TimeDly(20);
    SeekedChannel = FM_SI47XX_ReadChannel();

    if (scanok)
    {
        if (si470x_info.area_select == 1)
        {
            freq  = SeekedChannel * 100 + 76000;
        }
        else
        {
            freq  = SeekedChannel * 100 + 87500;
        }

        //__msg("scanok = %d, freq = %d\n", scanok, freq);
    }

    FM_SI47XX_WriteReg(W02h, SI_DMUTE | SI_ENABLE);//4001
    __msg("output freq:: %d\n", freq);
    __inf("**********************auto end************************\n\n");
#elif 0
    FM_SI47XX_WriteReg(W03h, SeekedChannel | SI_TUNE);
    delaycounter = 0;

    while (ReadSTC() != 1 && delaycounter < 250)
    {
        delaycounter++;
    }

    if (delaycounter >= 250)
    {
        __inf("ReadSTC() != 1\n");
    }

    esKRNL_TimeDly(5);
    FM_SI47XX_WriteReg(W03h, SeekedChannel);
    delaycounter = 0;

    while (ReadSTC()  == 1 && delaycounter < 250)
    {
        delaycounter++;
    }

    if (delaycounter >= 250)
    {
        __inf("ReadSTC()  == 1\n");
    }

    {
        __u16 value;
        __u8  RSSI;
        value = FM_SI47XX_ReadReg(R0Ah);
        RSSI  = (value & 0x00FF);

        if ((value & 0x3000) == 0x0000 && RSSI > 0x19) //RSSI 灵敏度
        {
            __inf("R0AH:0x%x, Freq: %d\n", value, freq);
            return (freq);
        }
        else
        {
            return (freq | 0xff000000);
        }
    }

#else
    {
        unsigned short Freq;
        unsigned char SeekFail;
        fm_mute(DRV_FM_VOICE_OFF);
        Si4702_FM_Seek(search_dir, 100, &Freq, &SeekFail);
        __msg("%d, %d\n\n\n\n", Freq * 10, SeekFail);

        if (SeekFail == 0)
        {
            return (Freq * 10);
        }
        else
        {
            return -1;
        }
    }
#endif
    return freq;
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
    __u16 tunechannel;
    __msg("radio-si470x: tune  %d\n", freq);
    fm_mute(DRV_FM_VOICE_ON);

    if (si470x_info.area_select == 1)
    {
        tunechannel = ((freq - 76000) / 100);
        __msg("tunechannel : %d\n", tunechannel);
    }
    else
    {
        if (freq < 87500)
        {
            freq = 87500;
        }

        tunechannel = ((freq - 87500) / 100);
        __msg("tunechannel : %d\n", tunechannel);
    }

    //FM_SI47XX_WriteReg(W03h,FM_SI47XX_FreqToChan(freq) | SI_TUNE);
    FM_SI47XX_WriteReg(W03h, tunechannel | SI_TUNE);
    delaycounter = 0;

    while (ReadSTC() != 1 && delaycounter < 250)
    {
        delaycounter++;
    }

    __msg("play begin\n");
    esKRNL_TimeDly(5);
    FM_SI47XX_WriteReg(W03h, tunechannel);
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
        si470x_info.area_select = 1;
        si470x_info.max_freq = FM_SEARCH_JAP_MAX_FREQ;
        si470x_info.min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_min_freq = FM_SEARCH_JAP_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_JAP_MAX_FREQ;
        //FM_SI47XX_SetBand(0);
        __msg("Area Choose Jpan\n");
    }
    else if (area == DRV_FM_AREA_CHN_US_EUP)
    {
        si470x_info.area_select = 0;
        si470x_info.max_freq = FM_SEARCH_CHN_MAX_FREQ;
        si470x_info.min_freq = FM_SEARCH_CHN_MIN_FREQ;
        freq_range->fm_area_min_freq = FM_SEARCH_CHN_MIN_FREQ;
        freq_range->fm_area_max_freq = FM_SEARCH_CHN_MAX_FREQ;
        //FM_SI47XX_SetBand(1);
        __msg("Area Choose China USA\n");
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
    __u16 value;
    value = FM_SI47XX_ReadReg(R02h);

    if (audio_method == DRV_FM_MONO)
    {
        __msg("DRV_FM_MONO\n");
        si470x_info.stereo_select = 1;
        value |= (SI_MONO);
        FM_SI47XX_WriteReg(W02h, value);
    }
    else if (audio_method == DRV_FM_STEREO)
    {
        __msg("DRV_FM_STEREO\n");
        si470x_info.stereo_select = 0;
        value &= (~SI_MONO);
        FM_SI47XX_WriteReg(W02h, value);
    }
    else
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32 fm_pa_gain(__u8 pagain)
{
    return EPDK_OK;
}

__s32 fm_signal_gain(__u8 sigain)
{
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
    __u16 value;
    value = FM_SI47XX_ReadReg(R02h);

    if (voice_onoff == DRV_FM_VOICE_OFF)
    {
        __msg("DRV_FM_VOICE_OFF\n");
        value &= (~SI_DMUTE);
        FM_SI47XX_WriteReg(W02h, value);
    }
    else if (voice_onoff == DRV_FM_VOICE_ON)
    {
        __msg("DRV_FM_VOICE_ON\n");
        value |= (SI_DMUTE);
        FM_SI47XX_WriteReg(W02h, value);
    }

    return EPDK_OK;
}
/**********************************************
* function:      fm_signal_level
*
* description:   信号强度选择，要求信号强大越高，收到的电台越少
*                要求信号强大越高，收到的电台越多，但无效电台也多
*
* notes:
*
**********************************************/             //OK
__s32 fm_signal_level(__s32 signal_level)
{
#if 0

    if (signal_level == DRV_FM_SLEVEL_HIGH)
    {
        __msg("DRV_FM_SLEVEL_HIGH\n");
        si470x_info.signal_level = 3;
        FM_SI47XX_WriteReg(W06h, SKSNR_HIGH | SKCNT_HIGH);
    }
    else if (signal_level == DRV_FM_SLEVEL_NORMAL)
    {
        __msg("DRV_FM_SLEVEL_NORMAL\n");
        si470x_info.signal_level = 2;
        FM_SI47XX_WriteReg(W06h, SKSNR_NORMAL | SKSNR_NORMAL);
    }
    else if (signal_level == DRV_FM_SLEVEL_LOW)
    {
        __msg("DRV_FM_SLEVEL_LOW\n");
        si470x_info.signal_level = 1;
        FM_SI47XX_WriteReg(W06h, SKSNR_LOW | SKCNT_LOW);
    }
    else
    {
        return EPDK_FAIL;
    }

#endif
    return EPDK_OK;
}

#endif

