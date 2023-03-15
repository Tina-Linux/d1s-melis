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
#include <kapi.h>
#include <string.h>
#include <port.h>
#include <log.h>
#include "sram_man.h"
#include <arch.h>

#include <hal_mem.h>

static csp_sram_blk_manager     SramBlkManager;

/*
*********************************************************************************************************
*                                                   eBase
*                                       the Abstract of Hardware
*                                              the OAL of CSP
*
*                               (c) Copyright 2006-2010, AW China
*                                           All Rights Reserved
*
* File      :   sram.c
* Date      :   2011-01-24
* By        :   holigun
* Version   :   V1.00
* Description :
*       由于SRAM基本上属于某些模块的独占后花园，所以没有必要统一管理，这里只提供虚拟地址的查询服务.
*         just for aw1619
* History    :
*               <author>            <date>              <note>
*               victor              2011-01-24          aw1619
*********************************************************************************************************
*/

#define CSP_SRAM_PHY_ADDR_BASE  0x03000000

unsigned long g_sram_virt_baseaddr  = 0;
#define sram_readb(addr)            (*((volatile unsigned char  *)(addr)))
#define sram_readw(addr)            (*((volatile unsigned short *)(addr)))
#define sram_readl(addr)            (*((volatile unsigned long  *)(addr)))
#define sram_writeb(v, addr)        (*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
#define sram_writew(v, addr)        (*((volatile unsigned short *)(addr)) = (unsigned short)(v))
#define sram_writel(v, addr)        (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

static void _modify_reg(unsigned long reg_addr, unsigned long bits_width, unsigned long offset, unsigned long value)
{
    uint32_t    temp = 0;
    uint32_t    mask = ((1 << bits_width) - 1) << offset;

    temp    = sram_readl(reg_addr);
    temp    &= ~(mask);
    value   <<= offset;
    value   &= mask;
    temp    |= value;

    sram_writel(temp, reg_addr);
}

/*
****************************************************************************************************
*
*             SRAM_init
*
*  Description:
*       SRAM init
*
*  Parameters:
*
*  Return value:
*       EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
int32_t sram_init(void)
{
    g_sram_virt_baseaddr = (uint32_t)awos_arch_bus_to_virt(CSP_SRAM_PHY_ADDR_BASE);

    _modify_reg(SRAM_REG_CFG_1, SRAM_C_BOOT_BITS_WIDTH,    SRAM_C_BOOT_OFFSET,    SRAM_C_SWITCH_NORMAL);

    return EBSP_TRUE;
}

/*
****************************************************************************************************
*
*             SRAM_exit
*
*  Description:
*       SRAM exit
*
*  Parameters:
*
*  Return value:
*       EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
int32_t sram_exit(void)
{
    /* it runs in boot mode,c for boot used */
    _modify_reg(SRAM_REG_CFG_1, SRAM_C_BOOT_BITS_WIDTH,    SRAM_C_BOOT_OFFSET,    SRAM_C_SWITCH_BOOT);

    g_sram_virt_baseaddr = 0;
    return EBSP_TRUE;
}



/*
****************************************************************************************************
*
*             SRAM_get_virt_base
*
*  Description:
*       get virt base address，for extension
*
*  Parameters:
*
*  Return value:
*       EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
uint32_t sram_get_virt_base(void)
{
    return g_sram_virt_baseaddr;
}


/*
****************************************************************************************************
*
*             sram_zone_number_get
*
*  Description:
*       get sram zone  number
*
*  Parameters:
*
*  Return value:
*       zone number
****************************************************************************************************
*/
int32_t sram_zone_number_get(void)
{
    return (CSP_SRAM_ZONE_MAX_NR - CSP_SRAM_ZONE_NULL);
}

/*
****************************************************************************************************
*
*             sram_zone_info_get
*
*  Description:
*       get sram zone info
*
*  Parameters:
*       zone_index      :   base on 0
*       p_zone_info     :   Total zone number
*  Return value:
*       EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
int32_t sram_zone_info_get(uint32_t zone_index, struct sram_zone_info *p_zone_info)
{
    zone_index ++;

    if (((int32_t)zone_index <= CSP_SRAM_ZONE_NULL) || (zone_index > CSP_SRAM_ZONE_MAX_NR) || (p_zone_info == NULL))
    {
        return EBSP_FALSE;
    }
    memset(p_zone_info, 0, sizeof(csp_sram_zone_info_t));
    p_zone_info->zone_id = (csp_sram_zone_id_t)zone_index;

    return EBSP_TRUE;
}

/*
****************************************************************************************************
*
*             SRAM_switch_mapping
*
*  Description:
*       switch sram zone
*
*  Parameters:
*       zone_name           :   such as CSP_SRAM_ZONE_NULL
*       mapping_to_module   :   such as CSP_SRAM_MODULE_NULL
*  Return value:
*       EBSP_TRUE/EBSP_FALSE
****************************************************************************************************
*/
int32_t sram_switch_mapping(csp_sram_zone_id_t zone_id, csp_sram_module_t mapping_to_module)
{

    switch (zone_id)
    {
        case CSP_SRAM_ZONE_BOOT     :
        {
            if (mapping_to_module == CSP_SRAM_MODULE_NORMAL) /* map to normal mode */
            {
                _modify_reg(SRAM_REG_CFG_1, SRAM_C_BOOT_BITS_WIDTH, SRAM_C_BOOT_OFFSET, SRAM_C_SWITCH_NORMAL);
            }
            else if (mapping_to_module == CSP_SRAM_MODULE_BOOT) /* map to boot mode */
            {
                _modify_reg(SRAM_REG_CFG_1, SRAM_C_BOOT_BITS_WIDTH, SRAM_C_BOOT_OFFSET, SRAM_C_SWITCH_BOOT);
            }
            else
            {
                return EBSP_FALSE;
            }
        }
        break;

        case CSP_SRAM_ZONE_C       :
        {
            if (mapping_to_module == CSP_SRAM_MODULE_CPU_DMA) /* map to AC320/DMA */
            {
                _modify_reg(SRAM_REG_CFG_1, SRAM_C_BOOT_BITS_WIDTH, SRAM_C_BOOT_OFFSET, SRAM_C_SWITCH_NORMAL);/* make sure it runs in normal mode */
                sram_writel(SRAM_C_SWITCH_CPU_DMA,   SRAM_REG_CFG_0);
            }
            else if (mapping_to_module == CSP_SRAM_MODULE_VE) /* map to VE */
            {
                _modify_reg(SRAM_REG_CFG_1, SRAM_C_BOOT_BITS_WIDTH, SRAM_C_BOOT_OFFSET, SRAM_C_SWITCH_NORMAL);  /* make sure it runs in normal mode */
                sram_writel(SRAM_C_SWITCH_VE,    SRAM_REG_CFG_0);
            }
            else
            {
                return EBSP_FALSE;
            }
        }
        break;

        case CSP_SRAM_ZONE_ICACHE   :
        {
        }
        return EBSP_FALSE;

        case CSP_SRAM_ZONE_DCACHE   :
        {
        }
        return EBSP_FALSE;

        default:
        {
        }
        return EBSP_FALSE;
    };
    return EBSP_TRUE;
}
/*
****************************************************************************************************
*
*             get_chip_version
*
*  Description:
*       get chip version
*
*  Parameters:
*
*  Return value:
*       chip version
****************************************************************************************************
*/
uint32_t misc_get_chip_version(void)
{
    uint32_t    temp = 0;
    temp    = sram_readl(SRAM_REG_VERSION);
    temp    |= 1 << 15;
    sram_writel(temp, SRAM_REG_VERSION);
    temp    = sram_readl(SRAM_REG_VERSION);

    temp    >>= 16;
    return temp;
}


/*
*********************************************************************************************************
*                                   SRAM BLOCK MANAGER INITIALISE
*
* Description: sram block manager initialise;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t mem_sram_init(void)
{
    int32_t                 i;
    csp_sram_zone_info_t    tmpSramBlk;

    //initialise sram csp
    sram_init();

    //clear sram block manager
    memset((void *)&SramBlkManager, 0, sizeof(csp_sram_blk_manager));

    //request memory for sram block manager
    SramBlkManager.nBlkCnt = sram_zone_number_get();
    if (SramBlkManager.nBlkCnt < 0)
    {
        __wrn("sram zone number is invalid!");
        mem_sram_exit();
        return EPDK_FAIL;
    }

    SramBlkManager.pBlkLst = (csp_sram_blk_node_t *)hal_malloc(SramBlkManager.nBlkCnt * sizeof(csp_sram_blk_node_t));
    if (SramBlkManager.pBlkLst == (csp_sram_blk_node_t *)0)
    {
        __wrn("request memory for sram manager failed!");
        mem_sram_exit();
        return EPDK_FAIL;
    }
    memset((void *)SramBlkManager.pBlkLst, 0, SramBlkManager.nBlkCnt * sizeof(csp_sram_blk_node_t));

    for (i = 0; i < SramBlkManager.nBlkCnt; i++)
    {
        SramBlkManager.pBlkLst[i].pLock = esKRNL_SemCreate(1);
        if (!SramBlkManager.pBlkLst[i].pLock)
        {
            __wrn("try to create semaphore for sram manager failed!");
            mem_sram_exit();
            return EPDK_FAIL;
        }

        sram_zone_info_get(i, &tmpSramBlk);
        SramBlkManager.pBlkLst[i].module    = CSP_SRAM_MODULE_NULL;
        SramBlkManager.pBlkLst[i].zone_id   = tmpSramBlk.zone_id;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   SRAM BLOCK MANAGER EXIT
*
* Description: sram block manager exit;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t mem_sram_exit(void)
{
    int32_t     i;

    if (SramBlkManager.pBlkLst)
    {
        //release semaphores
        for (i = 0; i < SramBlkManager.nBlkCnt; i++)
        {
            if (SramBlkManager.pBlkLst[i].pLock)
            {
                esKRNL_SemDel(SramBlkManager.pBlkLst[i].pLock, 0, NULL);
                SramBlkManager.pBlkLst[i].pLock = 0;
            }
        }

        //release memory
        hal_free((void *)SramBlkManager.pBlkLst);
        SramBlkManager.pBlkLst = 0;
    }

    //exit sram csp
    sram_exit();

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                   SWITCH SRAM BLOCK
*
* Description: switch sram block;
*
* Arguments  : hSram    handle of the sram block;
*              uMap     sram map
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esMEM_SramSwitchBlk(intptr_t* hSram, csp_sram_module_t uMap)
{
    csp_sram_blk_node_t   *tmpBlk   = (csp_sram_blk_node_t *)hSram;
    int32_t               i;
    int32_t               result    = EPDK_FAIL;

    //check if the sram block handle is valid
    for (i = 0; i < SramBlkManager.nBlkCnt; i++)
    {
        if (tmpBlk == &SramBlkManager.pBlkLst[i])
        {
            if (tmpBlk->bUse)
            {
                result = sram_switch_mapping(tmpBlk->zone_id, uMap);
                if (result == EBSP_TRUE)
                {
                    tmpBlk->module  = uMap;
                    result          = EPDK_OK;
                }
                break;
            }
        }
    }

    return result;
}

/*
*********************************************************************************************************
*                                   REQUEST THE GIVEN SRAM BLOCK
*
* Description: request the given sram block.
*
* Arguments  : uBlock   sram block number;
*              uMode    request sram wait mode;
*
* Returns    : result, handle of the sram block;
*
* Note       :
*********************************************************************************************************
*/
intptr_t* esMEM_SramReqBlk(csp_sram_module_t uBlock, csp_sram_req_mode_e uMode)
{
    int8_t      err;
    int32_t     idx = -1, i;

    for (i = 0; i < SramBlkManager.nBlkCnt; i++)
    {
        if (SramBlkManager.pBlkLst[i].module == uBlock)
        {
            idx = i;
            break;
        }
    }

    if (idx != -1)
    {
        switch (uMode)
        {
            case SRAM_REQ_MODE_WAIT:
            {
                esKRNL_SemPend(SramBlkManager.pBlkLst[idx].pLock, 0, &err);
                if (err != 0)
                {
                    err = 1;
                }
                else
                {
                    err = 0;
                }
                break;
            }

            case SRAM_REQ_MODE_NWAIT:
            {
                err = esKRNL_SemAccept(SramBlkManager.pBlkLst[idx].pLock);
                if (!err)
                {
                    err = 1;
                }
                else
                {
                    err = 0;
                }

                break;
            }

            default:
            {
                __wrn("mode is invalid for request sram block!");
                err = 1;
                break;
            }
        }

        if (!err)
        {
            SramBlkManager.pBlkLst[idx].bUse = 1;
            return (intptr_t*)&SramBlkManager.pBlkLst[idx];
        }
    }

    return 0;
}



/*
*********************************************************************************************************
*                                   RELEASE SRAM BLOCK WITH HANDLE
*
* Description: release sram block with handle.
*
* Arguments  : hSram    handle of the sram block;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esMEM_SramRelBlk(intptr_t* hSram)
{
    csp_sram_blk_node_t     *tmpBlk = (csp_sram_blk_node_t *)hSram;
    int32_t                 i;
    int32_t                 result  = EPDK_FAIL;

    //check if the sram block handle is valid
    for (i = 0; i < SramBlkManager.nBlkCnt; i++)
    {
        if (tmpBlk == &SramBlkManager.pBlkLst[i])
        {
            if (tmpBlk->bUse)
            {
                tmpBlk->bUse = 0;
                esKRNL_SemPost(tmpBlk->pLock);
                result = EPDK_OK;
                break;
            }
        }
    }

    return result;
}
