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
#include <hal_clk.h>
#include <hal_reset.h>
#include <ktype.h>
#include <log.h>
#include <csp_ccm_para.h>

#if defined (CONFIG_DRIVERS_SUNXI_CLK)  /*CONFIG_DRIVERS_SUNXI_CLK  is definition of old api for sun8iw19*/
sys_reset_type_t    clk_r_type  = 0;
#elif defined (CONFIG_DRIVERS_SUNXI_CCU)
sys_reset_type_t    clk_r_type  = CSP_CCM_SUNXI_RESET;
#endif

#if defined (CONFIG_DRIVERS_SUNXI_CCU)
/*
*********************************************************************************************************
*                                   SET SOURCE CLOCK FREQUENCY
*
* Description: In hal layer, there is overlap macro-encode definition in FIXED_CCU field 
*              and CCU field arrange[0~1], so system-api layer need to distinguish the 
*              macro-encode for sys_clk_id_t clock type.
*
* Arguments  : sclk:    macro-encode for sys_clk_id_t clock type;
*
* Returns    : result:  field type definition for FIXED_CCU and CCU
*
* Note       : FIXED_CCU includes oscillator of 24M & LOSC, CCU includes SoC's source and module clock
*********************************************************************************************************
*/
int32_t check_ccu_clk_type(sys_clk_id_t sclk)
{
    if(sclk >= CSP_CCM_MOD_CLK_MAX)
    {
        return CSP_CCM_SUNXI_UNKOWN_CCU;
    }

    if(sclk < CSP_CCM_SYS_CLK_OSC12M)
    {
        return CSP_CCM_SUNXI_FIXED_CCU;
    }
    return CSP_CCM_SUNXI_CCU;

}
#endif

/*
*********************************************************************************************************
*                                   SET SOURCE CLOCK FREQUENCY
*
* Description: set source clock frequency;
*
* Arguments  : sclk     source clock number;
*              nFreq    frequency, the source clock will change to;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_SetSrcFreq(sys_clk_id_t sclk, uint32_t nFreq)
{
    sys_clk_type_t      clk_type    = 0;
    hal_clk_t           pclk = (hal_clk_t)NULL;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type    = check_ccu_clk_type(sclk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type)
    {
        __wrn("sclk=%d, type is unknow!", sclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type)
    {
        sclk = sclk - CSP_CCM_SYS_CLK_MAX;
    }

#endif
    pclk = hal_clock_get(clk_type, sclk);
    hal_clk_set_rate(pclk, nFreq);
    hal_clock_put(pclk);
    return 0;
}

/*
*********************************************************************************************************
*                                   GET SOURCE CLOCK FREQUENCY
*
* Description: get source clock frequency;
*
* Arguments  : sclk     source clock number need get frequency;
*
* Returns    : result   frequency of the source clock;
*
* Note       :
*********************************************************************************************************
*/
uint32_t esCLK_GetSrcFreq(sys_clk_id_t sclk)
{
    sys_clk_type_t      clk_type    = 0;
    hal_clk_t           pclk = (hal_clk_t)NULL;
    uint32_t            grate = 0;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type    = check_ccu_clk_type(sclk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type)
    {
        __wrn("sclk=%d, type is unknow!", sclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type)
    {
        sclk = sclk - CSP_CCM_SYS_CLK_MAX;
    }

#endif
    pclk = hal_clock_get(clk_type, sclk);
    grate = hal_clk_get_rate(pclk);
    hal_clock_put(pclk);

    return grate;
}

/*
*********************************************************************************************************
*                                   OPEN MODULE CLOCK
*
* Description: open module clock;
*
* Arguments  : mclk     number of module clock which need be open;
*
* Returns    : result   handle of the module clock;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_OpenMclk(sys_clk_id_t mclk)
{
    sys_clk_type_t      clk_type    = 0;
    hal_clk_t           pclk = (hal_clk_t)NULL;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type    = check_ccu_clk_type(mclk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type)
    {
        __wrn("mclk=%d, type is unknow!", mclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type)
    {
        mclk = mclk - CSP_CCM_SYS_CLK_MAX;
    }

#endif
    pclk = hal_clock_get(clk_type, mclk);
    hal_clock_enable(pclk);
    hal_clock_put(pclk);
    return 0;
}

/*
*********************************************************************************************************
*                                   CLOSE MODULE CLOCK
*
* Description: close module clock;
*
* Arguments  : hMclk    number of module clock which need be close;
*
* Returns    : result   EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_CloseMclk(sys_clk_id_t mclk)
{
    sys_clk_type_t      clk_type    = 0;
    hal_clk_t           pclk = (hal_clk_t)NULL;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type    = check_ccu_clk_type(mclk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type)
    {
        __wrn("mclk=%d, type is unknow!", mclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type)
    {
        mclk = mclk - CSP_CCM_SYS_CLK_MAX;
    }

#endif

    pclk = hal_clock_get(clk_type, mclk);
    hal_clock_disable(pclk);
    hal_clock_put(pclk);
    return 0;
}

/*
*********************************************************************************************************
*                           MODULE CLOCK REGISTER CALL-BACK FUNCTION
*
* Description: register call-back function for module clock;
*
* Arguments  : mclk     number of module clock;
*              pCb      call-back function for adjust clock;
*
* Returns    : result   EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkRegCb(sys_clk_id_t mclk, __pCB_ClkCtl_t pCb)
{
    __err("not support syscall!");
    return 0;
}

/*
*********************************************************************************************************
*                           MODULE CLOCK UNREGISTER CALL-BACK FUNCTION
*
* Description: unregister call-back function for module clock;
*
* Arguments  : mclk     number of module clock;
*              pCb      call-back function for adjust clock;
*
* Returns    : result   EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkUnregCb(sys_clk_id_t mclk, __pCB_ClkCtl_t pCb)
{
    __err("not support syscall!");
    return 0;
}

/*
*********************************************************************************************************
*                                   SET MODUEL CLOCK SOURCE
*
* Description: set module clock source;
*
* Arguments  : mclk     number of module clock whose source need be set;
*              sclk     number of source clock which the module clock will use;
*
* Returns    : result   EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_SetMclkSrc(sys_clk_id_t mclk, sys_clk_id_t sclk)
{
    sys_clk_type_t      clk_type_m    = 0, clk_type_s    = 0;
    hal_clk_t           pclkm = (hal_clk_t)NULL;
    hal_clk_t           pclks = (hal_clk_t)NULL;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type_m    = check_ccu_clk_type(mclk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type_m)
    {
        __wrn("mclk=%d, type is unknow!", mclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type_m)
    {
        mclk = mclk - CSP_CCM_SYS_CLK_MAX;
    }

    clk_type_s    = check_ccu_clk_type(sclk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type_s)
    {
        __wrn("sclk=%d, type is unknow!", sclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type_s)
    {
        sclk = sclk - CSP_CCM_SYS_CLK_MAX;
    }
#endif

    pclkm = hal_clock_get(clk_type_m, mclk);
    pclks = hal_clock_get(clk_type_s, sclk);
    hal_clk_set_parent(pclkm, pclks);
    hal_clock_put(pclks);
    hal_clock_put(pclkm);
    return 0;
}

/*
*********************************************************************************************************
*                                   GET MODUEL CLOCK SOURCE
*
* Description: get module clock source;
*
* Arguments  : mclk     number of module clock whose source need be get;
*
* Returns    : result   source clock number;
*
* Note       :
*********************************************************************************************************
*/
sys_clk_id_t esCLK_GetMclkSrc(sys_clk_id_t mclk)
{
    sys_clk_type_t      clk_type    = 0;
    hal_clk_t           parentclk   = 0;
    hal_clk_t           pclk        = 0;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type    = check_ccu_clk_type(mclk);
    
    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type)
    {
        __wrn("mclk=%d, type is unknow!", mclk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type)
    {
        mclk = mclk - CSP_CCM_SYS_CLK_MAX;
    }

#endif

    pclk        = hal_clock_get(clk_type, mclk);
    parentclk   = hal_clk_get_parent(pclk);

    hal_clock_put(pclk);

#if defined (CONFIG_DRIVERS_SUNXI_CLK)

    return parentclk;

#elif defined (CONFIG_DRIVERS_SUNXI_CCU)

    return ((struct clk_core *)parentclk)->hw->id;

#endif
}

/*
*********************************************************************************************************
*                                   GET CLOCK ROUND RATE
*
* Description: get module clock round rate;
*
* Arguments  : clk      number of module clock who want to be calculated the rate depending on the hal 
*                       layer's division table;
*
* Returns    : result   source clock number;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_GetRound_Rate(sys_clk_id_t clk, unsigned int rate)
{
    sys_clk_type_t  clk_type    = 0;
    hal_clk_t       pclk        = 0;
    uint32_t        rountrate   = 0;

#if defined (CONFIG_DRIVERS_SUNXI_CCU)

    clk_type    = check_ccu_clk_type(clk);

    if(CSP_CCM_SUNXI_UNKOWN_CCU == clk_type)
    {
        __wrn("clk=%d, type is unknow!", clk);
        return EPDK_FAIL;
    }
    else if(CSP_CCM_SUNXI_CCU == clk_type)
    {
        clk = clk - CSP_CCM_SYS_CLK_MAX;
    }

#endif
    pclk        = hal_clock_get(clk_type, clk);
    rountrate   = hal_clk_round_rate(pclk, rate);

    hal_clock_put(pclk);

    return rountrate;
}

/*
*********************************************************************************************************
*                                   SET MODUEL CLOCK DIVISION
*
* Description: set module clock division;
*
* Arguments  : hMclk    number of module clock whose division need be set;
*              nDiv     division of the module clock;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_SetMclkDiv(sys_clk_id_t mclk, uint32_t nDiv)
{
    __err("not support syscall!");
    return 0;
}

/*
*********************************************************************************************************
*                                   GET MODUEL CLOCK DIVISION
*
* Description: get module clock division;
*
* Arguments  : hMclk    number of module clock;
*
* Returns    : result   division of the module clock;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_GetMclkDiv(sys_clk_id_t mclk)
{
    __err("not support syscall!");
    return 0;
}

/*
*********************************************************************************************************
*                                   MODUEL CLOCK ON/OFF
*
* Description: module clock on/off;
*
* Arguments  : hMclk    number of module clock;
*              bOnOff   on or off;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkOnOff(sys_clk_id_t mclk, uint32_t bOnOff)
{
    __err("not support syscall!");
    return 0;
    //hal_clock_enable()/hal_clock_disable();
}

/*
*********************************************************************************************************
*                                   MODUEL CLOCK ASSERT
*
* Description: module clock assert;
*
* Arguments  : r_mclk   number of module clock whose definition in csp_ccm_para.h;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkAssert(sys_clk_id_t r_mclk)
{
    int32_t ret;
    struct reset_control *reset;
    reset = hal_reset_control_get(clk_r_type, r_mclk);
    ret = hal_reset_control_assert(reset);
    hal_reset_control_put(reset);
    return ret;
}

/*
*********************************************************************************************************
*                                   MODUEL CLOCK DEASSERT
*
* Description: module clock deassert;
*
* Arguments  : r_mclk   number of module clock whose definition in csp_ccm_para.h;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkDeassert(sys_clk_id_t r_mclk)
{
    int32_t ret;
    struct reset_control *reset;
    reset = hal_reset_control_get(clk_r_type, r_mclk);
    ret = hal_reset_control_deassert(reset);
    hal_reset_control_put(reset);
    return ret;
}

/*
*********************************************************************************************************
*                                   MODUEL CLOCK RESET
*
* Description: module clock reset;
*
* Arguments  : r_mclk   reset number of module clock whose definition in csp_ccm_para.h;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkReset(sys_clk_id_t r_mclk)
{
    int32_t ret;
    struct reset_control *reset;
    reset = hal_reset_control_get(clk_r_type, r_mclk);
    ret = hal_reset_control_reset(reset);
    hal_reset_control_put(reset);
    return ret;
}

/*
*********************************************************************************************************
*                                   MODUEL CLOCK GET RESET STATUS
*
* Description: module clock get reset status;
*
* Arguments  : r_mclk   reset number of module clock whose definition in csp_ccm_para.h;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esCLK_MclkGetRstStatus(sys_clk_id_t r_mclk)
{
    int32_t ret;
    struct reset_control *reset;
    reset = hal_reset_control_get(clk_r_type, r_mclk);
    ret = hal_reset_control_status(reset);
    hal_reset_control_put(reset);
    return ret;
}

void esCLK_SysInfo(const char *name)
{
    __err("not support syscall!");
    return;
}

void esCLK_ModInfo(const char *name)
{
    __err("not support syscall!");
    return;
}