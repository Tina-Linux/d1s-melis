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
#include "dram_i.h"
#include <csp.h>
#include <kconfig.h>
#include "dram_i.h"
#include <csp.h>
#include <kconfig.h>

#define DRAMC_REG_STORE_CNT  32
uint32_t g_dramc_register[DRAMC_REG_STORE_CNT];


__dram_para_t dram_para;

//dram, pio and ccm virtual address for config dram
uint32_t gDramcVirtAddr = DRAMC_PHY_BASE;
uint32_t gDramPioBase   = DRAM_PIO_BASE;
uint32_t gDramCcmBase   = DRAM_CCM_BASE;

static void DRAMC_initial(void)
{
#ifdef CONFIG_SOC_SUN3IW1
    int32_t time = 0xffffff;

    //initialization WAC
    DRAM_REG_SCTLR = DRAM_REG_SCTLR | 0x1;
    while ((DRAM_REG_SCTLR & 0x1) && time--)
    {
        if (time == 0)
        {
            return ;
        }
    }
#endif
    return ;
}

/*
*************************************************************************************************************
*                                              DRAM parameter setup
* Description: setup/update the dram register parameters and initialization
*
* Arguments: para: dram configure parameters
*
* Returns: EBSP_TRUE
*
* Notes: none
*
*************************************************************************************************************
*/
static uint32_t DRAMC_para_setup(__dram_para_t *para)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t   reg_val = 0;

    //setup SCONR register
    reg_val = (para->ddr8_remap)            |
              (0x1 << 1)                      |
              ((para->bank_size >> 2) << 3)     |
              ((para->cs_num >> 1) << 4)        |
              ((para->row_width - 1) << 5)      |
              ((para->col_width - 1) << 9)      |
              ((para->type ? (para->bwidth >> 4) : (para->bwidth >> 5)) << 13)  |
              (para->access_mode << 15)       |
              (para->type << 16);
    DRAM_REG_SCONR = reg_val;

    //set DRAM control register
    DRAM_REG_SCTLR = DRAM_REG_SCTLR | 0x1 << 19; //enable delay auto refresh mode

    //initialization
    DRAMC_initial();
#endif
    return 0;
}


static void DRAMC_delay_tune(uint32_t clk)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t rp;
    uint32_t reg_val = 0;
    int32_t delay_low, delay_high;

    reg_val = DRAM_REG_SCTLR;
    reg_val >>= 6;
    rp      = reg_val & 0x7;

    if (rp == 0x4)
    {
        reg_val     = DRAM_REG_DDLYR;
        delay_low   = (reg_val >> 8) & 0x3f;
        delay_high  = (reg_val >> 16) & 0x3f;
        if (clk >=  192)
        {
            delay_low   = 1;
            delay_high  = 1;
        }
        else if (clk >= 120)
        {
            delay_low   -= 8;
            delay_high  -= 8;
        }
        if (delay_low < 0)
        {
            delay_low   = 0;
        }
        if (delay_high < 0)
        {
            delay_high  = 0;
        }
        reg_val = (delay_low << 8);
        reg_val |= (delay_high << 16);
        DRAM_REG_DDLYR  = reg_val;
    }
#endif

    return;
}

static void DRAMC_delay_scan(void)
{
#ifdef CONFIG_SOC_SUN3IW1
    //check whether read result is right for the readpipe value
    DRAM_REG_DDLYR = 0x1;
    while (DRAM_REG_DDLYR & 0x1)
    {
        continue;
    }
#endif
    return;
}

/*
*************************************************************************************************************
*                                         DRAM check type
* Description: check whether the DRAM is DDR or SDR
*
* Arguments: para: dram configure parameters
*
* Returns: 0: SDR
*          1: DDR
* Notes:
*
*************************************************************************************************************
*/
uint32_t CSP_DRAMC_check_type(__dram_para_t *para)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t reg_val = 0;
    uint32_t i;
    uint32_t times = 0;

    //check 8 readpipe values
    for (i = 0; i < 8; i++)
    {
        //readpipe value
        reg_val = DRAM_REG_SCTLR;
        reg_val &= ~(0x7 << 6);
        reg_val |= (i << 6);
        DRAM_REG_SCTLR = reg_val;

        //check whether read result is right for the readpipe value
        DRAMC_delay_scan();

        //check whether the DQS flag is success
        if (DRAM_REG_DDLYR & 0x30)
        {
            //fail
            times++;
            //break;
        }
    }
    //all the readpipe values fail
    if (times == 8)
    {
        //SDR
        para->type = DRAM_TYPE_SDR;
        return 0;
    }
    else
    {
        //DDR
        para->type = DRAM_TYPE_DDR;
        return 1;
    }
#endif
    return 1;
}

/*
*************************************************************************************************************
*                                              CHECK DRAM delay success value
* Description: counting the DQS report success value
*
*    Arguments: bwidth:  DRAM data width
*
* Returns: DQS report success value
*
* Notes: none
*
*************************************************************************************************************
*/
static uint32_t DRAMC_check_delay(uint32_t bwidth)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t   dsize;
    uint32_t   i, j;
    uint32_t   num = 0;
    uint32_t   dflag = 0;

    dsize = ((bwidth == 16) ? 4 : 2);     //data width:  16bits  DQS0~3     8bits  DQS0~1

    for (i = 0; i < dsize; i++)
    {
        switch (i)
        {
            case 0:
                dflag = DRAM_REG_DRPTR0;
                break;
            case 1:
                dflag = DRAM_REG_DRPTR1;
                break;
            case 2:
                dflag = DRAM_REG_DRPTR2;
                break;
            case 3:
                dflag = DRAM_REG_DRPTR3;
                break;
            default:
                break;
        }

        for (j = 0; j < 32; j++)
        {
            if (dflag & 0x1)           // 1 success   0 fail
            {
                num++;
            }
            dflag >>= 1;
        }
    }

    return num;
#endif
    return 0;
}

void CSP_DRAMC_set_autofresh_cycle(uint32_t clk)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t reg_val = 0;
    uint32_t row = 0;
    uint32_t temp = 0;

    row = DRAM_REG_SCONR;
    row &= 0x1E0;
    row >>= 0x5;

    //64 ms/8192 rows = 7.8us per refresh cycle
    if (row == 0xC)
    {
        if (clk >= 1000000)
        {
            //HZ
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while (temp >= (10000000 >> 6))
            {
                temp -= (10000000 >> 6);
                reg_val++;
            }
        }
        else
        {
            //MHZ
            reg_val = (clk * 499) >> 6;
        }
    }
    else if (row == 0xB) //64ms/4096 rows = 15.6us per refresh cycle
    {
        if (clk >= 1000000)
        {
            temp = clk + (clk >> 3) + (clk >> 4) + (clk >> 5);
            while (temp >= (10000000 >> 7))
            {
                temp -= (10000000 >> 7);
                reg_val++;
            }
        }
        else
        {
            reg_val = (clk * 499) >> 5;
        }
    }
    DRAM_REG_SREFR = reg_val;
#endif
    return;
}

/*
**********************************************************************************************************************
*                                               DRAM_check_ddr_readpipe
*
* Description: check the best dram readpipe value
*
* Arguments  : none
*
* Returns    : regurn EBSP_TRUE
*
* Notes      :
*
**********************************************************************************************************************
*/
uint32_t CSP_DRAMC_scan_readpipe(uint32_t clk)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t   i, rp_val = 0, rp_best = 0;
    uint32_t   reg_val = 0;
    uint32_t   readpipe[8];
    __dram_para_t *para = &dram_para;

    if (para->type == DRAM_TYPE_DDR)
    {
        //DDR type
        for (i = 0; i < 5; i++)
        {
            //readpipe value fill
            reg_val = DRAM_REG_SCTLR;
            reg_val &= ~(0x7 << 6);
            reg_val |= (i << 6);
            DRAM_REG_SCTLR = reg_val;

            //initialization
            DRAMC_initial();

            //check whether read result is right for the readpipe value
            DRAMC_delay_scan();

            readpipe[i] = 0;
            if ((((DRAM_REG_DDLYR >> 4) & 0x3) == 0x0) && (((DRAM_REG_DDLYR >> 4) & 0x1) == 0x0))
            {
                readpipe[i] = DRAMC_check_delay(para->bwidth);
            }

            //get the best readpipe value
            if (rp_val < readpipe[i])
            {
                rp_val = readpipe[i];
                rp_best = i;
            }
        }

        //set the best readpipe and check it
        reg_val = DRAM_REG_SCTLR;
        reg_val &= ~(0x7 << 6);
        reg_val |= (rp_best << 6);
        DRAM_REG_SCTLR = reg_val;

        //check whether read result is right for the readpipe value
        DRAMC_delay_scan();

        //tune ddr delay line value
        DRAMC_delay_tune(clk);
    }
    else
    {
        //SDR type
        //set up the sdr parameters
        reg_val = DRAM_REG_SCONR;
        reg_val &= ~(0x1 << 16);
        reg_val &= ~(0x3 << 13);
        DRAM_REG_SCONR = reg_val;

        //initialization
        DRAMC_initial();

        if (clk <= 90)
        {
            rp_best = 1;
        }
        else if (clk <= 150)
        {
            rp_best = 2;
        }
        else
        {
            rp_best = 3;
        }

        //set the best readpipe and check it
        reg_val = DRAM_REG_SCTLR;
        reg_val &= ~(0x7 << 6);
        reg_val |= rp_best << 6;
        DRAM_REG_SCTLR = reg_val;
    }
#endif
    return 0;
}

/*
*************************************************************************************************************
*                                         DRAM check size
* Description: check DRAM size :16MB/32MB/64MB
*
* Arguments: para: dram configure parameters
*
* Returns: 0: find the match size
*          1: can't find the match size
* Notes:
*
*************************************************************************************************************
*/
uint32_t CSP_DRAMC_get_dram_size(void)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t   colflag = 10, rowflag = 13;
    uint32_t   i = 0;
    uint32_t   val1 = 0;
    uint32_t   count = 0;
    uint32_t   addr1, addr2;
    __dram_para_t *para = &dram_para;

    //--------------------column test begin---------------------------------
    para->col_width = colflag;
    para->row_width = rowflag;

    //fill the parameter and initial
    DRAMC_para_setup(para);

    //scan the best readpipe
    CSP_DRAMC_scan_readpipe(para->clk);

    //bank0 10 column or 9 column test
    for (i = 0; i < 32; i++)
    {
        *((__u32 *)(0x80000200 + i)) = 0x11111111;
        *((__u32 *)(0x80000600 + i)) = 0x22222222;
    }
    for (i = 0; i < 32; i++)
    {
        val1 = *((__u32 *)(0x80000200 + i));
        if (val1 == 0x22222222)
        {
            count++;
        }
    }

    //not 10 column
    if (count == 32)
    {
        colflag = 9;
    }
    else
    {
        colflag = 10;
    }

    //--------------------row test begin-------------------------------
    count = 0;
    para->col_width = colflag;
    para->row_width = rowflag;

    //fill the parameter and initial
    DRAMC_para_setup(para);

    //scan the best readpipe
    //CSP_DRAMC_scan_readpipe();

    if (colflag == 10)
    {
        addr1 = 0x80400000;
        addr2 = 0x80c00000;
    }
    else
    {
        addr1 = 0x80200000;
        addr2 = 0x80600000;
    }
    //bank0 13 row or 12 row test
    for (i = 0; i < 32; i++)
    {
        *((__u32 *)(addr1 + i)) = 0x33333333;
        *((__u32 *)(addr2 + i)) = 0x44444444;
    }
    for (i = 0; i < 32; i++)
    {
        val1 = *((__u32 *)(addr1 + i));
        if (val1 == 0x44444444)
        {
            count++;
        }
    }

    if (count == 32) //not 13 row
    {
        rowflag = 12;
    }
    else
    {
        rowflag = 13;
    }

    para->col_width = colflag;
    para->row_width = rowflag;

    //return size type
    //(para->row_width != 13) ? (para->size = 16) : ( (para->col_width == 10) ? (para->size = 64) : (para->size = 32) );
    if (para->row_width != 13)
    {
        para->size = 16;
    }
    else if (para->col_width == 10)
    {
        para->size = 64;
    }
    else
    {
        para->size = 32;
    }

    //set DRAM refresh interval register
    CSP_DRAMC_set_autofresh_cycle(para->clk);

    //fill the parameter and initial
    para->access_mode = 0;
    DRAMC_para_setup(para);

    //scan the best readpipe
    //CSP_DRAMC_scan_readpipe();
#endif
    return 0;
}

/*
*********************************************************************************************************
*                                   DRAM SET PAD DRIVE
*
* Description: dram set pad drive;
*
* Arguments  : clk_freq
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_set_pad_drive(__u32 clk_freq)
{

}


/*
*********************************************************************************************************
*                                   DRAM INITIALISE
*
* Description: dram initialise;
*
* Arguments  : para     dram configure parameters
*              mode     mode for initialise dram
*
* Returns    : EBSP_FALSE: dram parameter is invalid or read_write failed
*               EBSP_TRUE: initialzation success
* Note       :
*********************************************************************************************************
*/
int32_t CSP_DRAMC_init(__dram_para_t *para, int32_t mode)
{
#ifdef CONFIG_SOC_SUN3IW1
    uint32_t reg_val = 0;

    //get dramc, pio and ccmu virtual address
    gDramcVirtAddr = (__u32)CSP_OSAL_PHY_2_VIRT(DRAMC_PHY_BASE, DRAMC_MEM_SIZE);
    gDramPioBase   = (__u32)CSP_OSAL_PHY_2_VIRT(DRAM_PIO_BASE, DRAM_PIO_MEMSIZE);
    gDramCcmBase   = (__u32)CSP_OSAL_PHY_2_VIRT(DRAM_CCM_BASE, DRAM_CCM_MEMSIZE);

    //dram parameter is invalid
    if (!para)
    {
        return 1;
    }

    if (!mode)
    {
        dram_para = *para;
        return 0;
    }
    return 0;//1663?úboot0à???3?ê??ˉò?′??ío?
    //wait initialization finish
    while (DRAM_REG_SCTLR & 0x1)
    {
        continue;
    }

    //set the default value:
    para->base          = 0x80000000;
    para->size          = 64;
    para->access_mode   = DRAM_ACS_SEQUENCE;
    para->cs_num        = 1;
    para->ddr8_remap    = 0;
    para->type          = DRAM_TYPE_DDR;
    para->bwidth        = 16;
    para->col_width     = 10;
    para->row_width     = 13;
    para->bank_size     = 4;
    para->cas           = 3;

    dram_para = *para;
    //memcpy(&dram_para, para, sizeof(__dram_para_t));

    //set DRAM PAD driving strength
    //reg_val = 0x555;      //level 0: 0x000;   level 1: 0x555;   level 2: 0xAAA;   level 3: 0xFFF
    //SDR_PAD_DRV_REG = reg_val;

    reg_val = SDR_PAD_DRV_REG;
    SDR_PAD_DRV_REG = reg_val;

    //set SDRAM PAD TYPE
    reg_val = SDR_PAD_PUL_REG;
    (dram_para.type == DRAM_TYPE_DDR) ? (reg_val |= (0x1 << 16)) : (reg_val &= ~(0x1 << 16));
    SDR_PAD_PUL_REG = reg_val;


    //set MCTL Timing 0 Register
    reg_val = (SDR_T_CAS << 0) | (SDR_T_RAS << 3) | (SDR_T_RCD << 7) | (SDR_T_RP << 10) | (SDR_T_WR << 13) | (SDR_T_RFC << 15) | (SDR_T_XSR << 19) | (SDR_T_RC << 28);
    DRAM_REG_STMG0R = reg_val;

    //set MCTL Timing 1 Register
    reg_val = (SDR_T_INIT << 0) | (SDR_T_INIT_REF << 16) | (SDR_T_WTR << 20) | (SDR_T_RRD << 22) | (SDR_T_XP << 25);
    DRAM_REG_STMG1R = reg_val;

    //set extend mode register
    reg_val = 0x1 << 16;
    DRAM_REG_SEXTMR = reg_val;

    //parameters setup and update
    DRAMC_para_setup(&dram_para);

    //check DRAM TYPE
    CSP_DRAMC_check_type(&dram_para);

    //update SDRAM PAD TYPE
    reg_val = SDR_PAD_PUL_REG;
    (dram_para.type == DRAM_TYPE_DDR) ? (reg_val |= (0x1 << 16)) : (reg_val &= ~(0x1 << 16));
    SDR_PAD_PUL_REG = reg_val;

    //check DRAM SIZE
    CSP_DRAMC_get_dram_size();
#endif

    return 0;
}

/*
*********************************************************************************************************
*                                   DRAM EXIT
*
* Description: dram exit;
*
* Arguments  : none;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
uint32_t CSP_DRAMC_exit(void)
{
    return 0;
}

/*
*********************************************************************************************************
*                                   DRAM SET EMRS
*
* Description: set EMRS for DDR memory and Mobile memory;
*
* Arguments  : emrs_id  EMRS ID;
*              emrs_val EMRS register value;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
uint32_t CSP_DRAMC_set_emrs(uint32_t emrs_id, uint32_t emrs_val)
{
    uint32_t reg_val = 0;

    //set extend mode register
    reg_val  = (emrs_id  & 0x3) << 13;
    reg_val |=  emrs_val & 0x1fff;
    DRAM_REG_SEXTMR = reg_val;

    //update EMRS to external memory
    DRAM_REG_SCTLR |= 0x1 << 18;

    //waiting
    while (DRAM_REG_SCTLR & (0x1 << 18))
    {
        continue;
    }

    return 0;
}

void CSP_DRAMC_master_close(void)
{
    DRAM_REG_MAE &= ~(0xfffc);
    return;
}
void CSP_DRAMC_master_open(void)
{
    DRAM_REG_MAE |= 0xffff;
    return;
}
void CSP_DRAM_store_register_data(void)
{
    int32_t i = 0;
    for (i = 0; i < DRAMC_REG_STORE_CNT; i++)
    {
        g_dramc_register[i] = __REG(gDramcVirtAddr + i * 4);
    }
    return;
}
void CSP_DRAM_restore_register_data(void)
{
    int32_t i = 0;
    for (i = 0; i < DRAMC_REG_STORE_CNT; i++)
    {
        __REG(gDramcVirtAddr + i * 4) = g_dramc_register[i];
    }
    return;
}
/*
*********************************************************************************************************
*                                   DRAM ENTER SELF REFRESH
*
* Description: dram enter self-refresh;
*
* Arguments  : none
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_enter_selfrefresh(void)
{
    uint32_t reg_val = 0;
    //enter dram self fresh mode
    reg_val = DRAM_REG_SCTLR;
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    DRAM_REG_SCTLR = reg_val | (0x1 << 1);
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    reg_val = DRAM_REG_SCTLR;
    //wait dram enter self-refresh status
    while (!(DRAM_REG_SCTLR & (0x1 << 11)))
    {
        //__msg("DRAM_REG_SCTLR = 0x%x",DRAM_REG_SCTLR);
        continue;
    }
}

/*
*********************************************************************************************************
*                                   DRAM EXIT SELF REFRESH
*
* Description: dram exit self-refresh;
*
* Arguments  : none;
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_exit_selfrefresh(void)
{
    //exit selfrefresh
    DRAM_REG_SCTLR &= ~(0x1 << 1);

    //wait dram exit self-refresh status
    while (DRAM_REG_SCTLR & (0x1 << 11))
    {
        continue;
    }
}

/*
*********************************************************************************************************
*                                   DRAM POWER CONTROL
*
* Description: dram power down or not;
*
* Arguments  : switch    1: enter power down mode
*                        0: exit power down mode
*
* Returns    : result, EBSP_TRUE;
*
* Note       :
*********************************************************************************************************
*/
void CSP_DRAMC_power_down(uint32_t down)
{
    if (down)
    {
        DRAM_REG_SCTLR |= (0x1 << 2);
    }
    else
    {
        DRAM_REG_SCTLR &= ~(0x1 << 2);
    }
}

/*
*********************************************************************************************************
*                                   DRAM SET ACCESS BLOCK SIZE
*
* Description: dram set hardware access block size;
*
* Arguments  : mod      hardware module id;
*              blksize  access block size;
*
* Returns    : result, EBSP_TRUE/EBSP_FALSE;
*
* Note       :
*********************************************************************************************************
*/

#ifdef CONFIG_SOC_SUN3IW2
int32_t CSP_DRAMC_set_master_config(__dram_dev_e mod, __dram_master_t *master)
{
    MC_MnCR0((__u32)mod) &= ~0xffffffff;
    MC_MnCR0((__u32)mod) |= master->bandwidth_limit0 << 16;
    MC_MnCR0((__u32)mod) |= master->command_number << 8;
    MC_MnCR0((__u32)mod) |= master->master_n_wait_time << 4;
    MC_MnCR0((__u32)mod) |= master->master_n_qos_value << 2;
    MC_MnCR0((__u32)mod) |= master->master_limit_enable << 0;
    MC_MnCR1((__u32)mod) &= ~0xffffffff;
    MC_MnCR1((__u32)mod) |= master->bandwidth_limit2 << 16;
    MC_MnCR1((__u32)mod) |= master->bandwidth_limit1 << 0;

    return 0;
}
#endif

/*
**********************************************************************************************************************
*                                               DRAM_Setwaitnum
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t CSP_DRAMC_set_master_configuration(__dram_dev_e mod, __dram_master_t *master)
{
#ifdef CONFIG_SOC_SUN3IW1
    switch (mod)
    {
        case DRAM_DEVICE_DAHB:
        {
            DRAM_REG_MCR0 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR0 |= master->dram_prio_level << 28;
            DRAM_REG_MCR0 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR0 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR0 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_IAHB:
        {
            DRAM_REG_MCR1 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR1 |= master->dram_prio_level << 28;
            DRAM_REG_MCR1 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR1 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR1 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_DDMA:
        {
            DRAM_REG_MCR2 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR2 |= master->dram_prio_level << 28;
            DRAM_REG_MCR2 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR2 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR2 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_NDMA:
        {
            DRAM_REG_MCR3 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR3 |= master->dram_prio_level << 28;
            DRAM_REG_MCR3 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR3 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR3 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_SD0:
        {
            DRAM_REG_MCR4 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR4 |= master->dram_prio_level << 28;
            DRAM_REG_MCR4 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR4 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR4 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_SD1:
        {
            DRAM_REG_MCR5 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR5 |= master->dram_prio_level << 28;
            DRAM_REG_MCR5 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR5 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR5 |= master->dram_request_num << 12;
            break;
        }

        case DRAM_DEVICE_DEBE:
        {
            DRAM_REG_MCR6 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR6 |= master->dram_prio_level << 28;
            DRAM_REG_MCR6 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR6 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR6 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_DEFE:
        {
            DRAM_REG_MCR7 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR7 |= master->dram_prio_level << 28;
            DRAM_REG_MCR7 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR7 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR7 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_VE:
        {
            DRAM_REG_MCR8 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR8 |= master->dram_prio_level << 28;
            DRAM_REG_MCR8 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR8 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR8 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_CSI:
        {
            DRAM_REG_MCR9 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR9 |= master->dram_prio_level << 28;
            DRAM_REG_MCR9 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR9 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR9 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_DeInterlace:
        {
            DRAM_REG_MCR10 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR10 |= master->dram_prio_level << 28;
            DRAM_REG_MCR10 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR10 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR10 |= master->dram_request_num << 12;
            break;
        }
        case DRAM_DEVICE_TVD:
        {
            DRAM_REG_MCR11 &= ~(0x3fffffff << 0);
            DRAM_REG_MCR11 |= master->dram_prio_level << 28;
            DRAM_REG_MCR11 |= master->dram_threshold0 << 0;
            DRAM_REG_MCR11 |= master->dram_threshold1 << 16;
            DRAM_REG_MCR11 |= master->dram_request_num << 12;
            break;
        }
        default:
        {
            return 1;
        }
    }
#endif
    return 0;
}

int32_t CSP_DRAMC_get_master_config(__dram_dev_e mod, __dram_master_t *master)
{
#ifdef CONFIG_SOC_SUN3IW2
    master->bandwidth_limit0     = (MC_MnCR0((__u32)mod) >> 16) & 0xffff;
    master->command_number       = (MC_MnCR0((__u32)mod) >> 8) & 0x00ff;
    master->master_n_wait_time   = (MC_MnCR0((__u32)mod) >> 4) & 0x000f;
    master->master_n_qos_value   = (MC_MnCR0((__u32)mod) >> 2) & 0x0003;
    master->master_limit_enable  = (MC_MnCR0((__u32)mod) >> 0) & 0x0001;
    master->bandwidth_limit2     = (MC_MnCR1((__u32)mod) >> 16) & 0xffff;
    master->bandwidth_limit1     = (MC_MnCR1((__u32)mod) >> 0) & 0xffff;
#endif

    return 0;
}

uint32_t dram_get_clock(void)
{
    uint32_t N, K, M;
    uint32_t clock;

    M = ((DRAM_CCM_SDRAM_PLL_REG >> 0) & 0x3)  + 1;
    K = ((DRAM_CCM_SDRAM_PLL_REG >> 4) & 0x3)  + 1;
    N = ((DRAM_CCM_SDRAM_PLL_REG >> 8) & 0x1f) + 1;
    clock = (24 * N * K) / M / 2;

    return clock;
}

/*
*********************************************************************************************************
*                                   DRAM GET CLOCK
*
* Description: dram get clock;
*
* Arguments  : on   dram clock gate on or off;
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
#ifdef CONFIG_SOC_SUN3IW1
int32_t CSP_DRAMC_set_bandwidth_window_configuration()
{
    uint32_t clock;

    clock = dram_get_clock();

    DRAM_REG_BWCR &= ~(0xffff << 0);
    DRAM_REG_BWCR |= clock << 0;
    return 0;
}
#endif

int32_t dram_gate_clock(__u32 on)
{
    if (on)
    {
        DRAM_CCM_SDRAM_PLL_REG &= ~(0x1 << 15);
    }
    else
    {
        DRAM_CCM_SDRAM_PLL_REG |= 0x1 << 15;
    }

    return 0;
}

int32_t CSP_DRAMC_access_enable(void)
{
    dram_gate_clock(0);
    return 0;
}

int32_t CSP_DRAMC_access_disable(void)
{
    dram_gate_clock(1);
    return 0;
}

int32_t CSP_DRAMC_set_master_default(__dram_dev_e mod)
{
    MC_MnCR0((__u32)mod) &= ~0xffffffff;
    MC_MnCR1((__u32)mod) &= ~0xffffffff;

    return 0;
}

int32_t CSP_DRAMC_master_counter_general_config(__dram_bwconf_t *bwconf)
{
    MC_GCGCR &= ~0xffffffff;
    MC_GCGCR |= bwconf->max_statistic << 16;
    MC_GCGCR |= bwconf->statistic_unit << 1;
    MC_GCGCR |= bwconf->bw_enable << 0;

    return 0;
}

int32_t CSP_DRAMC_get_all_bw_counter(__dram_bandw_t *bandw)
{
    bandw->cpu_bw   = MC_CPU_BWCR;
    bandw->dtmb_bw  = MC_DTMB_BWCR;
    bandw->ve_bw    = MC_VE_BWCR;
    bandw->disp_bw  = MC_DISP_BWCR;
    bandw->other_bw = MC_OTHER_BWCR;
    bandw->total_bw = MC_TOTAL_BWCR;

    return 0;
}