/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                             Video Decoder Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : macc_bsp.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-12-13 8:54
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-13 8:54     kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#include "macc_bsp_cfg.h"
#include "macc_bsp.h"
#include <sys_clock.h>
#include <kapi.h>
#include <log.h>

#define WARNING             __wrn

static __krnl_event_t   *pSemClkAdjust;
static __hdle           hMAccMClk;
static __hdle           hDramMaccClk;
static __hdle           hMaccAhbClk;
static __hdle           hSramBlk;
static __hdle           hMAccLock;

//macc registers base address
__u32   MACC_REGS_BASE;

__u64 MACC_GetMACCVersion(void);

/*
*********************************************************************************************************
*                           CALL-BACK FUNCTION - CHANGE MACC SOURCE CLOCK
*
* Description: change mpeg macc source clock, call-back fuction;
*
* Arguments  : cmd      command for change clock,request change or change done;
*              aux      new source clock value;
*
* Returns    : EPDK_OK/FAIL
*********************************************************************************************************
*/
static __s32 cb_MACC_ClockChange(__u32 cmd, __s32 aux)
{
    __s32 retval = EPDK_FAIL;
    __u8 err;

    if (cmd == CLK_CMD_SCLKCHG_REQ)
    {
        //request semphore to check if it can adjust frequence now
        esKRNL_SemPend(pSemClkAdjust, 0, &err);
        retval = EPDK_OK;
    }

    if (cmd == CLK_CMD_SCLKCHG_DONE)
    {
        //release semphore
        esKRNL_SemPost(pSemClkAdjust);
        retval = EPDK_OK;
    }

    return retval;
}


/*
*********************************************************************************************************
*                                       CLOSE MACC CLOCK MODULE
*
* Description: This function close the mpeg acc clock module, unregister the mpeg acc
*              module from clock management;
*
* Arguments  : none;
*
* Returns    : EPDK_OK/FAIL
*********************************************************************************************************
*/
static __s32 MACC_CloseClk(void)
{
    __u8        err;
    //register call-back fucntion for adjust ve clock
    esCLK_MclkUnregCb(CSP_CCM_MOD_CLK_VE, cb_MACC_ClockChange);

    //unregister ve module clock
    if (hMAccMClk)
    {
        esCLK_CloseMclk(hMAccMClk);
        hMAccMClk = 0;
    }

    //unregister macc ahb clock
    if (hMaccAhbClk)
    {
        esCLK_CloseMclk(hMaccAhbClk);
        hMaccAhbClk = 0;
    }

    //unregister macc dram access clock
    if (hDramMaccClk)
    {
        esCLK_CloseMclk(hDramMaccClk);
        hDramMaccClk = 0;
    }

    //delete semphore
    if (pSemClkAdjust)
    {
        esKRNL_SemDel(pSemClkAdjust, OS_DEL_ALWAYS, &err);
        pSemClkAdjust = 0;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   MACC INITIALISE
*
* Description: macc initialise;
*
* Arguments  : nModType type of the video stream;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/

void setClockGatingOn(void)
{
    esCLK_OpenMclk(CSP_CCM_MOD_CLK_BUS_VE);
    esCLK_OpenMclk(CSP_CCM_MOD_CLK_MBUS_VE);
}
void setClockGatingOff(void)
{
    esCLK_CloseMclk(CSP_CCM_MOD_CLK_BUS_VE);
    esCLK_CloseMclk(CSP_CCM_MOD_CLK_MBUS_VE);
}
static void resetDec(void)
{
   // esCLK_CloseMclk(CSP_CCM_MOD_CLK_VE);
   // esCLK_OpenMclk(CSP_CCM_MOD_CLK_VE);
    esCLK_MclkAssert(CSP_CCM_MOD_RST_BUS_VE);
    esCLK_MclkDeassert(CSP_CCM_MOD_RST_BUS_VE);
	return;
}
__s32 MACC_Init(__macc_mod_type_e nModType)
{
	int ret = -1, i = 0;
    u32 rate = 0;
    __s32 irq = -1;
    __u8 err = -1;
    __u32           tmpVal;
    unsigned long veVersion = 0;

    MACC_REGS_BASE = 0x01c0e000;
    __wrn("[YG]MACC_REGS_BASE:0x%x\n",MACC_REGS_BASE);

    //esCLK_MclkAssert(CSP_CCM_MOD_RST_BUS_VE);
    esCLK_MclkDeassert(CSP_CCM_MOD_RST_BUS_VE);
    //ret = esCLK_OpenMclk(CSP_CCM_SYS_CLK_PLL_VE);
    ret = esCLK_OpenMclk(CSP_CCM_MOD_CLK_VE);
	setClockGatingOn();
   // ret = esCLK_CloseMclk(CSP_CCM_MOD_CLK_VE);
  //  if (ret)
    {
     //   __wrn("ve couldn't disable clock! return %d\n", ret);
      //  return -1;
    }
	resetDec();
    ret = esCLK_CloseMclk(CSP_CCM_MOD_CLK_VE);

    #if defined (CONFIG_DRIVERS_SUNXI_CLK)
    sys_clk_id_t ve_parent_clk = CSP_CCM_SYS_CLK_PERI1X2_PLL;
    #elif defined (CONFIG_DRIVERS_SUNXI_CCU)
    sys_clk_id_t ve_parent_clk = CSP_CCM_SYS_CLK_PLL_VE;
    #endif

    ret = esCLK_SetMclkSrc(CSP_CCM_MOD_CLK_VE, ve_parent_clk);
    if (ret)
    {
        __wrn("ve clk set parent failed! return %d\n", ret);
        return -1;
    }

    rate = esCLK_GetRound_Rate(CSP_CCM_MOD_CLK_VE, 576*1000000);
    if (rate < 0)
    {
        __wrn("ve clk round rate failed! return %ld\n", rate);
        return -1;
    }
    __wrn("[YG]rate:%d\n",rate);
    ret = esCLK_SetSrcFreq(CSP_CCM_MOD_CLK_VE, 432*1000000);
    if (ret)
    {
        __wrn("ve clk set rate failed! return %d\n", ret);
        return -1;
    }

    rate = esCLK_GetSrcFreq(CSP_CCM_MOD_CLK_VE);
    if (rate < 0)
    {
        __wrn("ve clk get rate failed! return %ld\n", rate);
        return -1;
    }
    __wrn("[YG]rate:%d\n",rate);

    ret = esCLK_OpenMclk(CSP_CCM_MOD_CLK_VE);
    if (ret)
    {
        __wrn("ve couldn't enable clock! return %d\n", ret);
        return -1;
    }

#if 0
    tmpVal &= ~(0x03 << 16);
    tmpVal |= 0x02 << 16;
    tmpVal &= ~0x7;
    __log("[YG]tmpVal:0x%x\n",tmpVal);

    WriteReg(MACC_REG_MODESEL_REG, tmpVal);
#endif
//	esKRNL_TimeDly(50);
	MACC_ModuleSel(nModType);
    //__log("[YG]nModType:0x%x\n",nModType);

    //32bits ddr2
    tmpVal = ReadReg(MACC_REG_MODESEL_REG);
    tmpVal &= ~(0x03 << 16);
    tmpVal |= 0x02 << 16;
    WriteReg(MACC_REG_MODESEL_REG, tmpVal);
     __wrn("[YG]nModType:0x%x\n",nModType);
           
     //disable all interupt, clear all interupt pending
    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        {
            tmpVal = ReadReg(MPEG_REG_VE_CTL);
            WriteReg(MPEG_REG_VE_CTL, tmpVal & ~(0x0f << 3));
            WriteReg(MPEG_REG_VE_STAT, tmpVal | 0x0f);
            break;
        }

        case MACC_MODULE_H264_DEC:
        {
            tmpVal = ReadReg(H264_REG_FUNCTION_CTRL);
            WriteReg(H264_REG_FUNCTION_CTRL, tmpVal & ~(0x1f << 3));
            tmpVal = ReadReg(H264_REG_FUNCTION_STATUS);
            WriteReg(H264_REG_FUNCTION_STATUS, tmpVal | 0x1f);
            break;
        }

        case MACC_MODULE_VC1_DEC:
        {
            tmpVal = ReadReg(VC1_DEC_CTRL_REG);
            WriteReg(VC1_DEC_CTRL_REG, tmpVal & (~0x0f));
            tmpVal = ReadReg(VC1_STATUS_REG);
            WriteReg(VC1_STATUS_REG, tmpVal | 0x0f);
            break;
        }

        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        default:
        {
            __wrn("macc sub-module not support!\n");
        }
    }
        __wrn("[YG]\n");

	return   EPDK_OK;
}
/*
*********************************************************************************************************
*                                       MACC EXIT
*
* Description: This function is used to close the macc hardware module;
*
* Arguments  : none
*
* Returns    : close macc hardware module result;
*********************************************************************************************************
*/
#if 1
__s32 MACC_Exit(__macc_mod_type_e nModType)
{
    __wrn("exit macc module (%d)\n", nModType);
    esCLK_CloseMclk(CSP_CCM_MOD_CLK_VE);
	esCLK_MclkAssert(CSP_CCM_MOD_RST_BUS_VE);
	setClockGatingOff();
	return EPDK_OK;
}
#else
__s32 MACC_Exit(__macc_mod_type_e nModType)
{
    __inf("exit macc module (%d)\n", nModType);
    //disable macc hardware module interupt
    esINT_DisableINT(INTC_IRQNO_MACC);
    //reset ve core
    MACC_ResetVeCore(nModType, MACC_RESET_ENABLE);

    //off clock
    if (hMAccMClk)
    {
        //disable ve module clock
        esCLK_MclkOnOff(hMAccMClk, CLK_OFF);
    }

    if (hMaccAhbClk)
    {
        //disable mpeg acc ahb clock
        esCLK_MclkOnOff(hMaccAhbClk, CLK_OFF);
    }

    if (hDramMaccClk)
    {
        //disable mpeg acc module clock
        esCLK_MclkOnOff(hDramMaccClk, CLK_OFF);
    }

    //close module clock
    MACC_CloseClk();

    //release sram
    if (hSramBlk)
    {
        esMEM_SramRelBlk(hSramBlk);
        hSramBlk = 0;
    }

    //release macc clock lock
    if (hMAccLock)
    {
        esSVC_ResourceRel(hMAccLock);
        hMAccLock = 0;
    }

    return EPDK_OK;
}
#endif
/*
*********************************************************************************************************
*                                       SET MACC MODULE CLOCK
*
* Description: This function set macc module clock;
*
* Arguments  : nFreq    freqrence value while need be set;
*
* Returns    : EPDK_OK/FAIL
*********************************************************************************************************
*/
__s32 MACC_SetClk(__u32 nFreq)
{
    //the clock should be multiple of 6Mhz
    nFreq /= 6 * 1000 * 1000;
    nFreq *= 6 * 1000 * 1000;
   // esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_VE_PLL, nFreq);
    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                                       MACC QUERY INTERUPT
*
* Description: This function is used to query interrupt pending and clear pending bits if some pending is be set
*
* Arguments  : nModType     type of sub module in video engine;
*              nIntType     the type of interupt in MACC core;
*                           SLICE_END_I  -   video engine finish interupt
*                           ERROR_I   -   video engine error interupt
*                           VLD_DATA_REQ_I  -   video engine memory request interupt
*
* Returns    : bit31~24     main interrupt no
*              bit23~16     sub interrupt no
*              bit0         0 - current type of interupt not happen,
*                           1 - current type of interupt happen;
**********************************************************************************************************************
*/
__s32 MACC_QueryINT(__macc_mod_type_e nModType, __u32 nIntType)
{
    __u32       tmpVal;
    __s32       result;

    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        {
            //get macc status value
            tmpVal = ReadReg(MPEG_REG_VE_STAT);

            if (tmpVal & (1 << nIntType))
            {
                //clear interupt pending
                WriteReg(MPEG_REG_VE_STAT, (tmpVal & ~0x0f) | (1 << nIntType));
                tmpVal = 1;
            }
            else
            {
                tmpVal = 0;
            }

            result = (tmpVal | (INTC_IRQNO_MACC << 24) | (nIntType << 16));
            break;
        }

        case MACC_MODULE_H264_DEC:
        {
            //get macc status value
            tmpVal = ReadReg(H264_REG_FUNCTION_STATUS);

            if (tmpVal & nIntType)
            {
                //clear interupt pending
                WriteReg(H264_REG_FUNCTION_STATUS, (tmpVal & ~0x0f) | nIntType);
                tmpVal = 1;
            }
            else
            {
                tmpVal = 0;
            }

            result = (tmpVal | (INTC_IRQNO_MACC << 24) | (nIntType << 16));
            break;
        }

        case MACC_MODULE_VC1_DEC:
        {
            //get macc status value
            tmpVal = ReadReg(VC1_STATUS_REG);

            if (tmpVal & (1 << nIntType))
            {
                //clear interupt pending
                WriteReg(VC1_STATUS_REG, (tmpVal & ~0x0f) | (1 << nIntType));
                tmpVal = 1;
            }
            else
            {
                tmpVal = 0;
            }

            result = (tmpVal | (INTC_IRQNO_MACC << 24) | (nIntType << 16));
            break;
        }

        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        default:
        {
            __wrn("macc sub-module not support!\n");
            result = 0;
        }
    }

    return result;
}

/*
**********************************************************************************************************************
*                                       MACC QUERY INTERUPT
*
* Description: This function is similar with MACC_QueryINT and used for x8intra
*
**********************************************************************************************************************
*/
__s32 MACC_X8QueryINT(__macc_mod_type_e nModType, __u32 nIntType)
{
    __u32           tmpVal;
    /* get macc status value */
    tmpVal = ReadReg(VC1_STATUS_REG);

    if (tmpVal & (1 << nIntType))
    {
        /* clear interupt pending */
        WriteReg(VC1_STATUS_REG, (tmpVal & ~0x0f) | (1 << nIntType));
        tmpVal = 1;
    }
    else
    {
        tmpVal = 0;
    }

    return (tmpVal | (INTC_IRQNO_MACC << 24) | (nIntType << 16));
}

/*
**********************************************************************************************************************
*                                       MACC ENABLE INTERUPT
*
* Description: This function is used to enable the given type of interupt in MACC core;
*
* Arguments  : nIntType     the type of interupt in MACC core;
*                           SLICE_END_I  -   video engine finish interupt
*                           ERROR_I   -   video engine error interupt
*                           VLD_DATA_REQ_I  -   video engine memory request interupt
*              mode         reserved;
*
* Returns    : EPDK_OK/FAIL
**********************************************************************************************************************
*/
__s32 MACC_EnableINT(__macc_mod_type_e nModType, __u32 nIntType)
{
    __u32           tmpVal;
    __wrn("MACC_EnableINT, nModType:%d, nIntType:%d\n", nModType, nIntType);

    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        {
            //clear the interupt pending of the given type of interupt
            tmpVal = ReadReg(MPEG_REG_VE_STAT);
            WriteReg(MPEG_REG_VE_STAT, tmpVal | (1 << nIntType));
            //enable the given type of interupt
            tmpVal = ReadReg(MPEG_REG_VE_CTL);
            tmpVal |= 1 << (nIntType + 3);
            WriteReg(MPEG_REG_VE_CTL, tmpVal);
            break;
        }

        case MACC_MODULE_H264_DEC:
        {
            //clear the interupt pending of the given type of interupt
            tmpVal = ReadReg(H264_REG_FUNCTION_STATUS);
            WriteReg(H264_REG_FUNCTION_STATUS, tmpVal | (1 << nIntType));
            //enable the given type of interupt
            tmpVal = ReadReg(H264_REG_FUNCTION_CTRL);
            tmpVal |= 1 << nIntType;
            WriteReg(H264_REG_FUNCTION_CTRL, tmpVal);
            break;
        }

        case MACC_MODULE_VC1_DEC:
        {
            //clear the interupt pending of the given type of interupt
            tmpVal = ReadReg(VC1_STATUS_REG);
            WriteReg(VC1_STATUS_REG, tmpVal | (1 << nIntType));
            //enable the given type of interupt
            tmpVal = ReadReg(VC1_DEC_CTRL_REG);
            tmpVal |= 1 << (nIntType + 3);
            WriteReg(VC1_DEC_CTRL_REG, tmpVal);
            break;
        }

        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        default:
        {
            __wrn("macc sub-module not support!\n");
        }
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   MACC DISABLE INTERRUPT
*
* Description: macc disable interrupt
*
* Arguments  : nModType     macc module type;
*              nIntType     interrupt type;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
__s32 MACC_DisableINT(__macc_mod_type_e nModType, __u32 nIntType)
{
    __u32           tmpVal;

    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        {
            tmpVal = ReadReg(MPEG_REG_VE_CTL);
            tmpVal &= ~(1 << (nIntType + 3));
            WriteReg(MPEG_REG_VE_CTL, tmpVal);
            break;
        }

        case MACC_MODULE_H264_DEC:
        {
            tmpVal = ReadReg(H264_REG_FUNCTION_STATUS);
            tmpVal &= ~(1 << nIntType);
            WriteReg(H264_REG_FUNCTION_STATUS, tmpVal);
            break;
        }

        case MACC_MODULE_VC1_DEC:
        {
            tmpVal = ReadReg(VC1_DEC_CTRL_REG);
            tmpVal &= ~(1 << nIntType);
            WriteReg(VC1_DEC_CTRL_REG, tmpVal);
            break;
        }

        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        default:
        {
            __wrn("macc sub-module not support!\n");
        }
    }

    return EPDK_OK;
}


/*
**********************************************************************************************************************
*                               MACC INSTALL INTERRUPT HANDLER
*
* Description: install interrupt handler;
*
* Arguments  : nModType macc module type;
*              pIsr     interrupt process handler;
*              arg      argument for the isr;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
**********************************************************************************************************************
*/
__s32 MACC_InstIsr(__macc_mod_type_e nModType, __pISR_t pIsr, void *arg)
{
    return esINT_InsISR(INTC_IRQNO_MACC, "MACC", pIsr, arg);
}


/*
**********************************************************************************************************************
*                               MACC UNINSTALL INTERRUPT HANDLER
*
* Description: uninstall interrupt handler;
*
* Arguments  : nModType macc module type;
*              pIsr     interrupt process handler;
*              arg      argument for the isr;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
**********************************************************************************************************************
*/
__s32 MACC_UninstIsr(__macc_mod_type_e nModType, __pISR_t pIsr)
{
    return esINT_UniISR(INTC_IRQNO_MACC, pIsr);
}


/*
*********************************************************************************************************
*                                   MACC RESET VE CORE
*
* Description: macc reset ve core;
*
* Arguments  : nModType module type;
*              nRstMode mode of reset ve core;
*                       MACC_RESET_DISABLE  disable macc reset, macc will work;
*                       MACC_RESET_ENABLE   enable macc reset, macc will not work;
*
* Returns    : result, EPDK_OK/FAIL
*
* Note       :
*********************************************************************************************************
*/
#if 0
__s32 MACC_ResetVeCore(__macc_mod_type_e nModType, __s32 nRstMode)
{
    //disable macc to access sdram
    MACC_DramAcsCfg(MACC_DRAM_ACS_DISABLE);

    //disable ve clock
    if (hMAccMClk)
    {
        //disable ve module clock
        esCLK_MclkReset(hMAccMClk, 1);
    }

    //enable ve clock
    if (nRstMode == MACC_RESET_DISABLE)
    {
        if (hMAccMClk)
        {
            //enable ve module clock
            esCLK_MclkReset(hMAccMClk, 0);
        }
    }

    //enable macc to access sdram
    MACC_DramAcsCfg(MACC_DRAM_ACS_ENABLE);
    return EPDK_OK;
}

#endif
/*
*********************************************************************************************************
*                                   MACC MODULE SELECT
*
* Description: macc module select;
*
* Arguments  : nModType type of ve hardware module need be selected;
*
* Returns    : result, EPDK_OK/FAIL
*
* Note       :
*********************************************************************************************************
*/
__s32 MACC_ModuleSel(__macc_mod_type_e nModType)
{
    __u32   tmpRegVal;
    __s32   result = EPDK_OK;

    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        case MACC_MODULE_H264_DEC:
        case MACC_MODULE_VC1_DEC:
        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        {
            tmpRegVal = ReadReg(MACC_REG_MODESEL_REG);
            tmpRegVal &= ~(0x0f << 0);
            tmpRegVal |= nModType;
            WriteReg(MACC_REG_MODESEL_REG, tmpRegVal);
            break;
        }

        default:
        {
            __wrn("unknown ve hardware module to be selected!\n");
            result = EPDK_FAIL;
            break;
        }
    }

    return result;
}


/*
*********************************************************************************************************
*                                   MACC ENABLE VE CORE
*
* Description: macc enable macc core;
*
* Arguments  : nModType type of ve hardware module need be selected;
*
* Returns    : result, EPDK_OK/FAIL
*
* Note       :
*********************************************************************************************************
*/
__s32 MACC_EnableCore(__macc_mod_type_e nModType)
{
    __s32   result = EPDK_FAIL;

    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        case MACC_MODULE_H264_DEC:
        case MACC_MODULE_VC1_DEC:
        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        {
            //enable ve clock
            if (hMAccMClk)
            {
                //enable ve module clock
                esCLK_MclkOnOff(hMAccMClk, CLK_ON);
                result = EPDK_OK;
            }

            break;
        }

        default:
        {
            __wrn("unknown ve hardware module to be selected!\n");
            break;
        }
    }

    return result;
}


/*
*********************************************************************************************************
*                                   MACC DISABLE VE CORE
*
* Description: macc disable macc core;
*
* Arguments  : nModType type of ve hardware module need be selected;
*
* Returns    : result, EPDK_OK/FAIL
*
* Note       :
*********************************************************************************************************
*/
__s32 MACC_DisableCore(__macc_mod_type_e nModType)
{
    __s32   result = EPDK_FAIL;

    switch (nModType)
    {
        case MACC_MODULE_MPEG_DEC:
        case MACC_MODULE_H264_DEC:
        case MACC_MODULE_VC1_DEC:
        case MACC_MODULE_RMVB_DEC:
        case MACC_MODULE_MPEG_ENC:
        case MACC_MODULE_ISP_CORE:
        case MACC_MODULE_AVC_ENC :
        {
            //enable ve clock
            if (hMAccMClk)
            {
                //enable ve module clock
                esCLK_MclkOnOff(hMAccMClk, CLK_OFF);
                result = EPDK_OK;
            }

            break;
        }

        default:
        {
            __wrn("unknown ve hardware module to be selected!\n");
            break;
        }
    }

    return result;
}


/*
*********************************************************************************************************
*                                   MACC GET VERSION
*
* Description: get macc version, different chip may have different version of video engine.
*
* Arguments  :
*
* Returns    : ve version, like 0x1617, 0x1623 and so on.
*
* Note       :
*********************************************************************************************************
*/
__u64 MACC_GetMACCVersion(void)
{
    __u32 value1,value2;
    __u64       tmpVal = 0;
   // tmpVal = ReadReg(MACC_REG_VERSION_REG);

  //__log("[YGG]%d,%d\n",sizeof(unsigned long long),sizeof(__u64));

    value1 = ReadReg(MACC_REGS_BASE + 0xe0);
    value2 = ReadReg(MACC_REGS_BASE + 0xe4);
  //  __log("[]%x,%x\n",value1,value2);
    tmpVal = ((unsigned long)value1 << 32) | ((unsigned long)value2);
    //__log("[]ve version:%llx\n",tmpVal);
    return tmpVal;
   // return (tmpVal >> 16);
}


/*
*********************************************************************************************************
*                                   MACC DRAM ACCESS CONFIG
*
* Description: config if dram is accessable by macc;
*
* Arguments  : mode     mode if dram is accessable or not;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
__s32 MACC_DramAcsCfg(__macc_dram_acs_mode_e mode)
{
    __s32   result = EPDK_FAIL;

    if (hDramMaccClk)
    {
        switch (mode)
        {
            case MACC_DRAM_ACS_DISABLE:
            {
                esCLK_MclkOnOff(hDramMaccClk, CLK_OFF);
                result = EPDK_OK;
                break;
            }

            case MACC_DRAM_ACS_ENABLE:
            {
                esCLK_MclkOnOff(hDramMaccClk, CLK_ON);
                result = EPDK_OK;
                break;
            }

            default:
            {
                __wrn("unknown mode when config macc access dram!\n");
                break;
            }
        }
    }

    return result;
}


/*
*********************************************************************************************************
*                                   MACC SWITCH SRAM MAP
*
* Description: switch sram map;
*
* Arguments  : map      map of the sram;
*                       CSP_SRAM_MODULE_CPU_DMA
*                       CSP_SRAM_MODULE_VE
*                       CSP_SRAM_MODULE_SIE0
*                       CSP_SRAM_MODULE_SIE1
*                       CSP_SRAM_MODULE_SIE2
*                       CSP_SRAM_MODULE_ACE
*                       CSP_SRAM_MODULE_EMAC
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
#if 0
__s32 MACC_SwitchSramMap(__sram_block_map_e map)
{
    if (hSramBlk)
    {
        return esMEM_SramSwitchBlk(hSramBlk, map);
    }
    else
    {
        return EPDK_FAIL;
    }
}
#endif
