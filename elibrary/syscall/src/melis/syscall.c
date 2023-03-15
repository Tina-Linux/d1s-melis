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
#include <stdio.h>
#include <kapi.h>
#include <syscall.h>
#include <mod_defs.h>
#include <csp_dma_para.h>
#include "rtthread.h"
#ifdef CONFIG_DEBUG_BACKTRACE
#include <backtrace.h>
#endif

// CONFIG
int32_t esCFG_Exit_Ex(intptr_t *parser)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CFG(esCFG_Exit_Ex), (long)parser);
    return result;
}

int32_t esCFG_GetGPIOSecData(char *GPIOSecName, void *pGPIOCfg, int32_t GPIONum)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_CFG(esCFG_GetGPIOSecData), (long)GPIOSecName, (long)pGPIOCfg, (long)GPIONum);
    return result;
}

int32_t esCFG_GetGPIOSecData_Ex(intptr_t *parser, char *GPIOSecName, void *pGPIOCfg, int32_t GPIONum)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_CFG(esCFG_GetGPIOSecData_Ex), (long)parser, (long)GPIOSecName, (long)pGPIOCfg, (long)GPIONum);
    return result;
}

int32_t esCFG_GetGPIOSecKeyCount(char *GPIOSecName)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CFG(esCFG_GetGPIOSecKeyCount), (long)GPIOSecName);
    return result;
}

int32_t esCFG_GetGPIOSecKeyCount_Ex(intptr_t *parser, char *GPIOSecName)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CFG(esCFG_GetGPIOSecKeyCount_Ex), (long)parser, (long)GPIOSecName);
    return result;
}

int32_t esCFG_GetKeyValue(char *SecName, char *KeyName, int32_t Value[], int32_t Count)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_CFG(esCFG_GetKeyValue), (long)SecName, (long)KeyName, (long)Value, (long)Count);
    return result;
}

int32_t esCFG_GetKeyValue_Ex(intptr_t *parser, char *KeyName, int32_t Value[], int32_t Count)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_CFG(esCFG_GetKeyValue_Ex), (long)parser, (long)KeyName, (long)Value, (long)Count);
    return result;
}

int32_t esCFG_GetSecCount(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_CFG(esCFG_GetSecCount));
    return result;
}

int32_t esCFG_GetSecCount_Ex(intptr_t *parser)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CFG(esCFG_GetSecCount_Ex), (long)parser);
    return result;
}

int32_t esCFG_GetSecKeyCount(char *SecName)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CFG(esCFG_GetSecKeyCount), (long)SecName);
    return result;
}

int32_t esCFG_GetSecKeyCount_Ex(intptr_t *parser, char *SecName)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CFG(esCFG_GetSecKeyCount_Ex), (long)parser, (long)SecName);
    return result;
}

intptr_t *esCFG_Init_Ex(char *path)
{
    intptr_t *result ;
    result = (intptr_t *)__syscall1((long)SYSCALL_CFG(esCFG_Init_Ex), (long)path);
    return result;
}

// CHS
int32_t esCHS_Char2Uni(int32_t type, const uint8_t *str, uint32_t len, uint16_t *uni)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_CHS(esCHS_Char2Uni), (long)type, (long)str, (long)len, (long)uni);
    return result;
}

uint32_t esCHS_GetChLowerTbl(int32_t charset_type, void *buf, uint32_t size)
{
    uint32_t result ;
    result = (uint32_t)__syscall3((long)SYSCALL_CHS(esCHS_GetChLowerTbl), (long)charset_type, (long)buf, (long)size);
    return result;
}

uint32_t esCHS_GetChUpperTbl(int32_t type, void *buf, uint32_t size)
{
    uint32_t result ;
    result = (uint32_t)__syscall3((long)SYSCALL_CHS(esCHS_GetChUpperTbl), (long)type, (long)buf, (long)size);
    return result;
}

int32_t esCHS_Uni2Char(int32_t type, uint16_t uni, uint8_t *str, uint32_t len)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_CHS(esCHS_Uni2Char), (long)type, (long)uni, (long)str, (long)len);
    return result;
}

// CLK
int32_t esCLK_CloseMclk(sys_clk_id_t mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_CloseMclk), (long)mclk);
    return result;
}

int32_t esCLK_GetMclkDiv(sys_clk_id_t mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_GetMclkDiv), (long)mclk);
    return result;
}

sys_clk_id_t esCLK_GetMclkSrc(sys_clk_id_t mclk)
{
    sys_clk_id_t result ;
    result = (sys_clk_id_t)__syscall1((long)SYSCALL_CLK(esCLK_GetMclkSrc), (long)mclk);
    return result;
}

int32_t esCLK_GetRound_Rate(sys_clk_id_t clk, uint32_t rate)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_GetRound_Rate), (long)clk, (long)rate);
    return result;
}

uint32_t esCLK_GetSrcFreq(sys_clk_id_t sclk)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_CLK(esCLK_GetSrcFreq), (long)sclk);
    return result;
}

int32_t esCLK_MclkAssert(sys_clk_id_t r_mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_MclkAssert), (long)r_mclk);
    return result;
}

int32_t esCLK_MclkDeassert(sys_clk_id_t r_mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_MclkDeassert), (long)r_mclk);
    return result;
}

int32_t esCLK_MclkGetRstStatus(sys_clk_id_t r_mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_MclkGetRstStatus), (long)r_mclk);
    return result;
}

int32_t esCLK_MclkOnOff(sys_clk_id_t mclk, uint32_t bOnOff)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_MclkOnOff), (long)mclk, (long)bOnOff);
    return result;
}

int32_t esCLK_MclkRegCb(sys_clk_id_t mclk, __pCB_ClkCtl_t pCb)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_MclkRegCb), (long)mclk, (long)pCb);
    return result;
}

int32_t esCLK_MclkReset(sys_clk_id_t r_mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_MclkReset), (long)r_mclk);
    return result;
}

int32_t esCLK_MclkUnregCb(sys_clk_id_t mclk, __pCB_ClkCtl_t pCb)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_MclkUnregCb), (long)mclk, (long)pCb);
    return result;
}

void esCLK_ModInfo(const char *name)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_CLK(esCLK_ModInfo), (long)name);
    // return result;
}

int32_t esCLK_OpenMclk(sys_clk_id_t mclk)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_CLK(esCLK_OpenMclk), (long)mclk);
    return result;
}

int32_t esCLK_SetMclkDiv(sys_clk_id_t mclk, uint32_t nDiv)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_SetMclkDiv), (long)mclk, (long)nDiv);
    return result;
}

int32_t esCLK_SetMclkSrc(sys_clk_id_t mclk, sys_clk_id_t sclk)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_SetMclkSrc), (long)mclk, (long)sclk);
    return result;
}

int32_t esCLK_SetSrcFreq(sys_clk_id_t sclk, uint32_t nFreq)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_CLK(esCLK_SetSrcFreq), (long)sclk, (long)nFreq);
    return result;
}

void esCLK_SysInfo(const char *name)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_CLK(esCLK_SysInfo), (long)name);
    // return result;
}

// DEV
int32_t esDEV_Close(void *hDev)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DEV(esDEV_Close), (long)hDev);
    return result;
}

void *esDEV_DevReg(const char *classname, const char *name, const __dev_devop_t *pDevOp, void *pOpenArg)
{
    void *result ;
    result = (void *)__syscall4((long)SYSCALL_DEV(esDEV_DevReg), (long)classname, (long)name, (long)pDevOp, (long)pOpenArg);
    return result;
}

int32_t esDEV_DevUnreg(void *hNode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DEV(esDEV_DevUnreg), (long)hNode);
    return result;
}

int32_t esDEV_Insmod(char *modfile, uint32_t devno, void *p_arg)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_DEV(esDEV_Insmod), (long)modfile, (long)devno, (long)p_arg);
    return result;
}

int32_t esDEV_Ioctl(void *hDev, uint32_t cmd, long aux, void *pbuffer)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_DEV(esDEV_Ioctl), (long)hDev, (long)cmd, (long)aux, (long)pbuffer);
    return result;
}

int32_t esDEV_Lock(void *hNode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DEV(esDEV_Lock), (long)hNode);
    return result;
}

void *esDEV_Open(void *hNode, uint32_t Mode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_DEV(esDEV_Open), (long)hNode, (long)Mode);
    return result;
}

int32_t esDEV_Plugin(char *plgmsg, uint32_t devno, void *p_arg, uint8_t prio)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_DEV(esDEV_Plugin), (long)plgmsg, (long)devno, (long)p_arg, (long)prio);
    return result;
}

int32_t esDEV_Plugout(char *plgmsg, uint32_t devno)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_DEV(esDEV_Plugout), (long)plgmsg, (long)devno);
    return result;
}

uint32_t esDEV_Read(void *pdata, uint32_t size, uint32_t n, void *hDev)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_DEV(esDEV_Read), (long)pdata, (long)size, (long)n, (long)hDev);
    return result;
}

int32_t esDEV_Unimod(char *modfile, uint32_t devno)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_DEV(esDEV_Unimod), (long)modfile, (long)devno);
    return result;
}

int32_t esDEV_Unlock(void *hNode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DEV(esDEV_Unlock), (long)hNode);
    return result;
}

uint32_t esDEV_Write(const void *pdata, uint32_t size, uint32_t n, void *hDev)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_DEV(esDEV_Write), (long)pdata, (long)size, (long)n, (long)hDev);
    return result;
}

rt_err_t rt_device_register(rt_device_t dev, const char *name, rt_uint16_t flags)
{
    rt_err_t result ;
    result = (rt_err_t)__syscall3((long)SYSCALL_DEV(rt_device_register), (long)dev, (long)name, (long)flags);
    return result;
}

rt_err_t rt_device_unregister(rt_device_t dev)
{
    rt_err_t result ;
    result = (rt_err_t)__syscall1((long)SYSCALL_DEV(rt_device_unregister), (long)dev);
    return result;
}

// DMA
int32_t esDMA_ChangeMode(void *dma, int32_t mode)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_DMA(esDMA_ChangeMode), (long)dma, (long)mode);
    return result;
}

int32_t esDMA_DisableINT(void *dma, int32_t type)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_DMA(esDMA_DisableINT), (long)dma, (long)type);
    return result;
}

int32_t esDMA_EnableINT(void *dma, int32_t type)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_DMA(esDMA_EnableINT), (long)dma, (long)type);
    return result;
}

void esDMA_Information(void)
{
    __syscall0((long)SYSCALL_DMA(esDMA_Information));
}

unsigned long esDMA_QueryDst(void *dma)
{
    unsigned long result ;
    result = (unsigned long)__syscall1((long)SYSCALL_DMA(esDMA_QueryDst), (long)dma);
    return result;
}

unsigned long esDMA_QueryRestCount(void *dma)
{
    unsigned long result ;
    result = (unsigned long)__syscall1((long)SYSCALL_DMA(esDMA_QueryRestCount), (long)dma);
    return result;
}

unsigned long esDMA_QuerySrc(void *dma)
{
    unsigned long result ;
    result = (unsigned long)__syscall1((long)SYSCALL_DMA(esDMA_QuerySrc), (long)dma);
    return result;
}

csp_dma_status esDMA_QueryStat(void *dma)
{
    csp_dma_status result ;
    result = (csp_dma_status)__syscall1((long)SYSCALL_DMA(esDMA_QueryStat), (long)dma);
    return result;
}

int32_t esDMA_RegDmaHdler(void *dma, void *hdler, void *arg)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_DMA(esDMA_RegDmaHdler), (long)dma, (long)hdler, (long)arg);
    return result;
}

int32_t esDMA_Release(void *dma)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DMA(esDMA_Release), (long)dma);
    return result;
}

void *esDMA_Request(void)
{
    void *result ;
    result = (void *)__syscall0((long)SYSCALL_DMA(esDMA_Request));
    return result;
}

int32_t esDMA_Restart(void *dma)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DMA(esDMA_Restart), (long)dma);
    return result;
}

int32_t esDMA_Setting(void *dma, void *arg)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_DMA(esDMA_Setting), (long)dma, (long)arg);
    return result;
}

int32_t esDMA_Start(void *dma)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DMA(esDMA_Start), (long)dma);
    return result;
}

int32_t esDMA_Stop(void *dma)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_DMA(esDMA_Stop), (long)dma);
    return result;
}

int32_t esDMA_UnregDmaHdler(void *dma, int32_t type, __pCBK_t hdler)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_DMA(esDMA_UnregDmaHdler), (long)dma, (long)type, (long)hdler);
    return result;
}

// EXEC
uint8_t esEXEC_PCreate(const char *filename, void *p_arg, uint32_t mode, uint32_t *ret)
{
    int8_t result ;
    result = (int8_t)__syscall4((long)SYSCALL_EXEC(esEXEC_PCreate), (long)filename, (long)p_arg, (long)mode, (long)ret);
    return result;
}

int8_t esEXEC_PDel(uint8_t id)
{
    int8_t result ;
    result = (int8_t)__syscall1((long)SYSCALL_EXEC(esEXEC_PDel), (long)id);
    return result;
}

int8_t esEXEC_PDelReq(uint8_t pid)
{
    int8_t result ;
    result = (int8_t)__syscall1((long)SYSCALL_EXEC(esEXEC_PDelReq), (long)pid);
    return result;
}

int8_t esEXEC_Run(const char *pfilename, void *p_arg, uint32_t mode, uint32_t *ret)
{
    int8_t result ;
    result = (int8_t)__syscall4((long)SYSCALL_EXEC(esEXEC_Run), (long)pfilename, (long)p_arg, (long)mode, (long)ret);
    return result;
}

int32_t esFSYS_clearpartupdateflag(const char *path)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_clearpartupdateflag), (long)path);
    return result;
}

int32_t esFSYS_closedir(void *hDir)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_closedir), (long)hDir);
    return result;
}

int32_t esFSYS_fclose(void *hFile)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_fclose), (long)hFile);
    return result;
}

void *esFSYS_fd2file(int32_t fd)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_FSYS(esFSYS_fd2file), (long)fd);
    return result;
}

void esFSYS_ferrclr(void *hFile)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_ferrclr), (long)hFile);
    // return result;
}

int32_t esFSYS_ferror(void *hFile)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_ferror), (long)hFile);
    return result;
}

int32_t esFSYS_file2fd(void *hFile)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_file2fd), (long)hFile);
    return result;
}

int32_t esFSYS_fioctrl(void *hFile, int32_t Cmd, long Aux, void *pBuffer)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_fioctrl), (long)hFile, (long)Cmd, (long)Aux, (long)pBuffer);
    return result;
}

void *esFSYS_fopen(const char *pFileName, const char *pMode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_FSYS(esFSYS_fopen), (long)pFileName, (long)pMode);
    return result;
}

int32_t esFSYS_format(const char *partname, const char *fstype, void *fmtpara)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_FSYS(esFSYS_format), (long)partname, (long)fstype, (long)fmtpara);
    return result;
}

uint32_t esFSYS_fread(void *pData, uint32_t Size, uint32_t N, void *hFile)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_fread), (long)pData, (long)Size, (long)N, (long)hFile);
    return result;
}

int32_t esFSYS_fsdbg(const char *cmd, const char *para)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_fsdbg), (long)cmd, (long)para);
    return result;
}

int32_t esFSYS_fseek(void *hFile, int32_t Offset, int32_t Whence)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_FSYS(esFSYS_fseek), (long)hFile, (long)Offset, (long)Whence);
    return result;
}

int64_t esFSYS_fseekex(void *hFile, int32_t l_off, int32_t h_off, int32_t Whence)
{
    int64_t result ;
    result = (int64_t)__syscall4((long)SYSCALL_FSYS(esFSYS_fseekex), (long)hFile, (long)l_off, (long)h_off, (long)Whence);
    return result;
}

int32_t esFSYS_fsreg(void *hFS)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_fsreg), (long)hFS);
    return result;
}

int32_t esFSYS_fstat(void *hFile, void *stat_buf)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_fstat), (long)hFile, (long)stat_buf);
    return result;
}

int32_t esFSYS_fsunreg(void *hFS)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_fsunreg), (long)hFS);
    return result;
}

int32_t esFSYS_fsync(void *hFile)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_fsync), (long)hFile);
    return result;
}

int32_t esFSYS_ftell(void *hFile)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_ftell), (long)hFile);
    return result;
}

int32_t esFSYS_ftellex(void *hFile, int32_t *l_pos, int32_t *h_pos)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_FSYS(esFSYS_ftellex), (long)hFile, (long)l_pos, (long)h_pos);
    return result;
}

int32_t esFSYS_ftruncate(void  *filehandle, uint32_t length)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_ftruncate), (long)filehandle, (long)length);
    return result;
}

uint32_t esFSYS_fwrite(const void *pData, uint32_t Size, uint32_t N, void *hFile)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_fwrite), (long)pData, (long)Size, (long)N, (long)hFile);
    return result;
}

int32_t esFSYS_getfscharset(const char *partname, int32_t *pCharset)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_getfscharset), (long)partname, (long)pCharset);
    return result;
}

int32_t esFSYS_mkdir(const char *pDirName)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_mkdir), (long)pDirName);
    return result;
}

int32_t esFSYS_mntfs(void *part)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_mntfs), (long)part);
    return result;
}

int32_t esFSYS_mntparts(void *hNode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_mntparts), (long)hNode);
    return result;
}

int32_t esFSYS_open(const char *name, int32_t flag, int32_t prems)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_FSYS(esFSYS_open), (long)name, (long)flag, (long)prems);
    return result;
}

void *esFSYS_opendir(const char *pDirName)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_FSYS(esFSYS_opendir), (long)pDirName);
    return result;
}

int32_t esFSYS_partfslck(char *partname)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_partfslck), (long)partname);
    return result;
}

int32_t esFSYS_partfsunlck(char *partname)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_partfsunlck), (long)partname);
    return result;
}

int32_t esFSYS_pclose(void *hPart)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_pclose), (long)hPart);
    return result;
}

int32_t esFSYS_pdreg(void *hPD)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_pdreg), (long)hPD);
    return result;
}

int32_t esFSYS_pdunreg(void *hPD)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_pdunreg), (long)hPD);
    return result;
}

void esFSYS_perrclr(void *hPart)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_perrclr), (long)hPart);
    //return result;
}

int32_t esFSYS_perror(void *hPart)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_perror), (long)hPart);
    return result;
}

int32_t esFSYS_pioctrl(void *hPart, uint32_t cmd, long aux, void *pbuffer)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_pioctrl), (long)hPart, (long)cmd, (long)aux, (long)pbuffer);
    return result;
}

void *esFSYS_popen(const char *PartName, const char *pMode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_FSYS(esFSYS_popen), (long)PartName, (long)pMode);
    return result;
}

uint32_t esFSYS_pread(void *pData, uint32_t sector, uint32_t n, void *hPart)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_pread), (long)pData, (long)sector, (long)n, (long)hPart);
    return result;
}

int32_t esFSYS_premove(const char *pFileName)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_premove), (long)pFileName);
    return result;
}

int32_t esFSYS_prename(const char *newfilename, const char *oldfilename)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_prename), (long)newfilename, (long)oldfilename);
    return result;
}

uint32_t esFSYS_pwrite(const void *pData, uint32_t Sector, uint32_t N, void *hPart)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_pwrite), (long)pData, (long)Sector, (long)N, (long)hPart);
    return result;
}

int32_t esFSYS_querypartupdateflag(const char *path, __bool *flag)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_querypartupdateflag), (long)path, (long)flag);
    return result;
}

void *esFSYS_readdir(void *hDir)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_FSYS(esFSYS_readdir), (long)hDir);
    return result;
}

int32_t esFSYS_remove(const char *pFileName)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_remove), (long)pFileName);
    return result;
}

int32_t esFSYS_rename(const char *newname, const char *oldname)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_rename), (long)newname, (long)oldname);
    return result;
}

void esFSYS_rewinddir(void *hDir)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_FSYS(esFSYS_rewinddir), (long)hDir);
}

int32_t esFSYS_rmdir(const char *pDirName)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_FSYS(esFSYS_rmdir), (long)pDirName);
    return result;
}

int32_t esFSYS_setfs(char *partname, uint32_t cmd, int32_t aux, char *para)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_FSYS(esFSYS_setfs), (long)partname, (long)cmd, (long)aux, (long)para);
    return result;
}

int32_t esFSYS_statfs(const char *path, void *buf, uint32_t flags)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_FSYS(esFSYS_statfs), (long)path, (long)buf, (long)flags);
    return result;
}

int32_t esFSYS_statpt(const char *path, void *buf)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_statpt), (long)path, (long)buf);
    return result;
}

int32_t esFSYS_umntfs(void *part, int32_t force)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_umntfs), (long)part, (long)force);
    return result;
}

int32_t esFSYS_umntparts(void *hNode, uint32_t force)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_FSYS(esFSYS_umntparts), (long)hNode, (long)force);
    return result;
}

// HID
int32_t esHID_hiddevreg(void *hNode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_HID(esHID_hiddevreg), (long)hNode);
    return result;
}

int32_t esHID_hiddevunreg(void *hNode, uint32_t mode)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_HID(esHID_hiddevunreg), (long)hNode, (long)mode);
    return result;
}

int32_t esHID_SendMsg(uint32_t msgid)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_HID(esHID_SendMsg), (long)msgid);
    return result;
}

// INPUT
int32_t esINPUT_GetLdevID(void *graber)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_INPUT(esINPUT_GetLdevID), (long)graber);
    return result;
}

int32_t esINPUT_LdevCtl(int32_t LdevId, int32_t cmd, int32_t aux, void *pBuffer)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_INPUT(esINPUT_LdevCtl), (long)LdevId, (long)cmd, (long)aux, (long)pBuffer);
    return result;
}

int32_t esINPUT_LdevFeedback(void *graber, __input_event_packet_t *packet)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_INPUT(esINPUT_LdevFeedback), (long)graber, (long)packet);
    return result;
}

void *esINPUT_LdevGrab(char *ldev, __pCBK_t callback, void *pArg, int32_t aux)
{
    void *result ;
    result = (void *)__syscall4((long)SYSCALL_INPUT(esINPUT_LdevGrab), (long)ldev, (long)callback, (long)pArg, (long)aux);
    return result;
}

int32_t esINPUT_LdevRelease(void *graber)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_INPUT(esINPUT_LdevRelease), (long)graber);
    return result;
}

int32_t esINPUT_RegDev(__input_dev_t *dev)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_INPUT(esINPUT_RegDev), (long)dev);
    return result;
}

int32_t esINPUT_SendEvent(__input_dev_t *dev, uint32_t type, uint32_t code, int32_t value)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_INPUT(esINPUT_SendEvent), (long)dev, (long)type, (long)code, (long)value);
    return result;
}

int32_t esINPUT_UnregDev(__input_dev_t *dev)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_INPUT(esINPUT_UnregDev), (long)dev);
    return result;
}

// INT
int32_t esINT_DisableINT(uint32_t irqno)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_INT(esINT_DisableINT), (long)irqno);
    return result;
}

int32_t esINT_EnableINT(uint32_t irqno)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_INT(esINT_EnableINT), (long)irqno);
    return result;
}

int32_t esINT_InsFIR(uint32_t fiqno, __pISR_t pFirMain, __pCBK_t pFirTail, void *pArg)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_INT(esINT_InsFIR), (long)fiqno, (long)pFirMain, (long)pFirTail, (long)pArg);
    return result;
}

int32_t esINT_InsISR(uint32_t irq, char* name, void *handler, void *argv)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_INT(esINT_InsISR), (long)irq, (long)name, (long)handler, (long)argv);
    return result;
}

int32_t esINT_SetIRQPrio(uint32_t irqno, uint32_t prio)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_INT(esINT_SetIRQPrio), (long)irqno, (long)prio);
    return result;
}

int32_t esINT_UniFIR(uint32_t fiqno, __pISR_t pFirMain, __pCBK_t pFirTail)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_INT(esINT_UniFIR), (long)fiqno, (long)pFirMain, (long)pFirTail);
    return result;
}

int32_t esINT_UniISR(uint32_t irq, void *dev_id)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_INT(esINT_UniISR), (long)irq, (long)dev_id);
    return result;
}

//KRNL
long esKRNL_CallBack(__pCBK_t cb, void *arg)
{
    long result ;
    result = (int32_t)__syscall2((long)SYSCALL_KRNL(esKRNL_CallBack), (long)cb, (long)arg);
    return result;
}

void esKRNL_DumpStack(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_KRNL(esKRNL_DumpStack));
}

//os Flag element
uint32_t esKRNL_FlagAccept(void *pgrp, uint32_t flags, uint8_t wait_type, uint8_t *err)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_KRNL(esKRNL_FlagAccept), (long)pgrp, (long)flags, (long)wait_type, (long)err);
    return result;
}

void *esKRNL_FlagCreate(uint32_t flags, uint8_t *err)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_KRNL(esKRNL_FlagCreate), (long)flags, (long)err);
    return result;
}

unsigned long esKRNL_FlagDel(void *pgrp, uint8_t opt, uint8_t *err)
{
    unsigned long result ;
    result = (unsigned long)__syscall3((long)SYSCALL_KRNL(esKRNL_FlagDel), (long)pgrp, (long)opt, (long)err);
    return result;
}

uint8_t esKRNL_FlagNameGet(void *pgrp, uint8_t *pname, uint8_t *err)
{
    uint8_t result ;
    result = (uint8_t)__syscall3((long)SYSCALL_KRNL(esKRNL_FlagNameGet), (long)pgrp, (long)pname, (long)err);
    return result;
}

void esKRNL_FlagNameSet(void *pgrp, uint8_t *pname, uint8_t *err)
{
    int8_t result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_FlagNameSet), (long)pgrp, (long)pname, (long)err);
}

uint32_t esKRNL_FlagPend(void *pgrp, uint32_t flags, uint32_t waittype_timeout, uint8_t *err)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_KRNL(esKRNL_FlagPend), (long)pgrp, (long)flags, (long)waittype_timeout, (long)err);
    return result;
}

uint32_t esKRNL_FlagPendGetFlagsRdy(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KRNL(esKRNL_FlagPendGetFlagsRdy));
    return result;
}

uint32_t esKRNL_FlagPost(void *pgrp, uint32_t flags, uint8_t opt, uint8_t *err)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_KRNL(esKRNL_FlagPost), (long)pgrp, (long)flags, (long)opt, (long)err);
    return result;
}

uint32_t esKRNL_FlagQuery(void *pgrp, uint8_t *err)
{
    uint32_t result ;
    result = (uint32_t)__syscall2((long)SYSCALL_KRNL(esKRNL_FlagQuery), (long)pgrp, (long)err);
    return result;
}

__pCBK_t esKRNL_GetCallBack(__pCBK_t cb)
{
    __pCBK_t result ;
    result = (__pCBK_t)__syscall1((long)SYSCALL_KRNL(esKRNL_GetCallBack), (long)cb);
    return result;
}

unsigned long esKRNL_GetTIDCur(void)
{
    unsigned long result ;
    result = (unsigned long)__syscall0((long)SYSCALL_KRNL(esKRNL_GetTIDCur));
    return result;
}

unsigned long esKRNL_InterruptDisable(void)
{
    unsigned long result;
    result = (unsigned long)__syscall0((long)SYSCALL_KRNL(esKRNL_InterruptDisable));
    return result;
}

void esKRNL_InterruptEnable(unsigned long level)
{
    __syscall1((long)SYSCALL_KRNL(esKRNL_InterruptEnable), (long)level);
}

//os Mailbox element
unsigned long esKRNL_MboxAccept(void *pevent)
{
    unsigned long result ;
    result = (unsigned long)__syscall1((long)SYSCALL_KRNL(esKRNL_MboxAccept), (long)pevent);
    return result;
}

void *esKRNL_MboxCreate(uint32_t msg)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_KRNL(esKRNL_MboxCreate), (long)msg);
    return result;
}

unsigned long esKRNL_MboxDel(void *pevent, uint8_t opt, uint8_t *err)
{
    unsigned long result ;
    result = (unsigned long)__syscall3((long)SYSCALL_KRNL(esKRNL_MboxDel), (long)pevent, (long)opt, (long)err);
    return result;
}

uint32_t esKRNL_MboxPend(void *pevent, uint16_t timeout, uint8_t *err)
{
    uint32_t result ;
    result = (uint32_t)__syscall3((long)SYSCALL_KRNL(esKRNL_MboxPend), (long)pevent, (long)timeout, (long)err);
    return result;
}

uint8_t esKRNL_MboxPost(void *pevent, unsigned long msg)
{
    uint8_t result ;
    result = (uint8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_MboxPost), (long)pevent, (long)msg);
    return result;
}

uint8_t esKRNL_MboxPostOpt(void *pevent, uint32_t msg, uint8_t opt)
{
    uint8_t result ;
    result = (uint8_t)__syscall3((long)SYSCALL_KRNL(esKRNL_MboxPostOpt), (long)pevent, (long)msg, (long)opt);
    return result;
}

uint8_t esKRNL_MboxQuery(void *pevent, void *p_mbox_data)
{
    uint8_t result ;
    result = (uint8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_MboxQuery), (long)pevent, (long)p_mbox_data);
    return result;
}

void esKRNL_MemLeakChk(uint32_t en)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KRNL(esKRNL_MemLeakChk), (long)en);
}

//os Mutex element
void *esKRNL_MutexCreate(uint8_t prio, uint8_t *err)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_KRNL(esKRNL_MutexCreate), (long)prio, (long)err);
    return result;
}

unsigned long esKRNL_MutexDel(void *pevent, uint8_t opt, uint8_t *err)
{
    unsigned long result ;
    result = (unsigned long)__syscall3((long)SYSCALL_KRNL(esKRNL_MutexDel), (long)pevent, (long)opt, (long)err);
    return result;
}

void esKRNL_MutexPend(void *pevent, uint16_t timeout, uint8_t *err)
{
    uint32_t result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_MutexPend), (long)pevent, (long)timeout, (long)err);
}

uint8_t esKRNL_MutexPost(void *pevent)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_MutexPost), (long)pevent);
    return result;
}

//os Queue element
unsigned long esKRNL_QAccept(void *pevent, uint8_t *err)
{
    unsigned long result ;
    result = (unsigned long)__syscall2((long)SYSCALL_KRNL(esKRNL_QAccept), (long)pevent, (long)err);
    return result;
}

void *esKRNL_QCreate(uint16_t size)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_KRNL(esKRNL_QCreate), (long)size);
    return result;
}

void *esKRNL_QDel(void *pevent, uint8_t opt, uint8_t *err)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_KRNL(esKRNL_QDel), (long)pevent, (long)opt, (long)err);
    return result;
}

uint8_t esKRNL_QFlush(void *pevent)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_QFlush), (long)pevent);
    return result;
}

unsigned long esKRNL_QPend(void *pevent, uint16_t timeout, uint8_t *err)
{
    unsigned long result ;
    result = (unsigned long)__syscall3((long)SYSCALL_KRNL(esKRNL_QPend), (long)pevent, (long)timeout, (long)err);
    return result;
}

uint8_t esKRNL_QPost(void *pevent, unsigned long msg)
{
    uint8_t result ;
    result = (uint8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_QPost), (long)pevent, (long)msg);
    return result;
}

uint8_t esKRNL_QPostFront(void *pevent, void *msg)
{
    int8_t result ;
    result = (int8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_QPostFront), (long)pevent, (long)msg);
    return result;
}

uint8_t esKRNL_QPostOpt(void *pevent, void *msg, uint8_t opt)
{
    uint8_t result ;
    result = (uint8_t)__syscall3((long)SYSCALL_KRNL(esKRNL_QPostOpt), (long)pevent, (long)msg, (long)opt);
    return result;
}

uint8_t esKRNL_QQuery(void *pevent, __krnl_q_data_t *p_q_data)
{
    uint8_t result ;
    result = (uint8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_QQuery), (long)pevent, (long)p_q_data);
    return result;
}

void esKRNL_SchedLock(void)
{
    uint16_t result ;
    result = __syscall0((long)SYSCALL_KRNL(esKRNL_SchedLock));
}

void esKRNL_SchedUnlock(void)
{
    uint16_t result ;
    result = __syscall0((long)SYSCALL_KRNL(esKRNL_SchedUnlock));
}

//os Semphore element
uint16_t esKRNL_SemAccept(void *psem)
{
    uint16_t result ;
    result = (uint16_t)__syscall1((long)SYSCALL_KRNL(esKRNL_SemAccept), (long)psem);
    return result;
}

void *esKRNL_SemCreate(uint16_t cnt)
{
    void *result ;
    result = (void *)(__syscall1((long)SYSCALL_KRNL(esKRNL_SemCreate), (long)cnt));
    return result;
}

void *esKRNL_SemDel(void *sem, uint8_t opt, uint8_t *err)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_KRNL(esKRNL_SemDel), (long)sem, (long)opt, (long)err);
    return result;
}

void esKRNL_SemPend(void *sem, uint16_t timeout, uint8_t *err)
{
    int32_t result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_SemPend), (long)sem, (long)timeout, (long)err);
    return;
}

uint8_t esKRNL_SemPost(void *sem)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_SemPost), (long)sem);
    return result;
}

uint8_t esKRNL_SemQuery(void *sem, OS_SEM_DATA *p_sem_data)
{
    uint8_t result ;
    result = (uint8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_SemQuery), (long)sem, (long)p_sem_data);
    return result;
}

void esKRNL_SemSet(void *pevent, uint16_t cnt, uint8_t *err)
{
    int8_t result ;
    result = __syscall3((long)SYSCALL_KRNL(esKRNL_SemSet), (long)pevent, (long)cnt, (long)err);
    return;
}

//os Socket element
__krnl_sktfrm_t *esKRNL_SktAccept(void *skt, uint8_t user)
{
    __krnl_sktfrm_t *result ;
    result = (__krnl_sktfrm_t *)__syscall2((long)SYSCALL_KRNL(esKRNL_SktAccept), (long)skt, (long)user);
    return result;
}

void *esKRNL_SktCreate(uint32_t depth, uint32_t dbuf_attr, uint32_t mbuf_attr)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_KRNL(esKRNL_SktCreate), (long)depth, (long)dbuf_attr, (long)mbuf_attr);
    return result;
}

int32_t esKRNL_SktDel(void *skt, uint8_t opt)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_KRNL(esKRNL_SktDel), (long)skt, (long)opt);
    return result;
}

__krnl_sktfrm_t *esKRNL_SktPend(void *skt, uint8_t user, uint32_t timeout)
{
    __krnl_sktfrm_t *result ;
    result = (__krnl_sktfrm_t *)__syscall3((long)SYSCALL_KRNL(esKRNL_SktPend), (long)skt, (long)user, (long)timeout);
    return result;
}

int32_t esKRNL_SktPost(void *skt, uint8_t user, __krnl_sktfrm_t *frm)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_KRNL(esKRNL_SktPost), (long)skt, (long)user, (long)frm);
    return result;
}

//os Task element
/*int8_t esKRNL_TaskDel(uint32_t prio)
{
    int8_t result ;
    result = (int8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskDel), (long)prio);
    return result;
}

int8_t esKRNL_TaskDelReq(uint32_t prio_ex)
{
    int8_t result ;
    result = (int8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskDelReq), (long)prio_ex);
    return result;
}*/


void esKRNL_TaskPrefEn(uint32_t en)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskPrefEn), (long)en);
}

uint8_t esKRNL_TaskQuery(uint32_t prio, __krnl_tcb_t *p_task_data)
{
    uint8_t result ;
    result = (uint8_t)__syscall2((long)SYSCALL_KRNL(esKRNL_TaskQuery), (long)prio, (long)p_task_data);
    return result;
}

uint8_t esKRNL_TaskResume(uint32_t prio)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskResume), (long)prio);
    return result;
}

long esKRNL_TaskNameSet(uint32_t prio, char *name)
{
    long result ;
    result = (long)__syscall2((long)SYSCALL_KRNL(esKRNL_TaskNameSet), (long)prio, (long)name);
    return result;
}

uint8_t esKRNL_TaskSuspend(uint32_t prio)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TaskSuspend), (long)prio);
    return result;
}

uint32_t esKRNL_TCreate(void (*entry)(void *p_arg), void *p_arg, uint32_t stksize, uint16_t id_priolevel)
{
    uint32_t result ;
    result = (uint32_t) __syscall4((long)SYSCALL_KRNL(esKRNL_TCreate), (long)entry, (long)p_arg, (long)stksize, (long)id_priolevel);
    return result;
}

int8_t esKRNL_TDel(uint32_t prio)
{
    int8_t result ;
    result = (int8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TDel), (long)prio);
    return result;
}

int8_t esKRNL_TDelReq(uint32_t prio)
{
    int8_t result ;
    result = (int8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TDelReq), (long)prio);
    return result;
}

//os Tick operation
uint32_t esKRNL_Time(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KRNL(esKRNL_Time));
    return result;
}

void esKRNL_TimeDly(uint16_t ticks)
{
    int8_t result ;
    result = __syscall1((long)SYSCALL_KRNL(esKRNL_TimeDly), (long)ticks);
    return;
}

uint8_t esKRNL_TimeDlyResume(uint32_t prio_ex)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TimeDlyResume), (long)prio_ex);
    return result;
}

uint32_t esKRNL_TimeGet(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KRNL(esKRNL_TimeGet));
    return result;
}

void esKRNL_TimeSet(uint32_t ticks)
{
    uint32_t result ;
    result = __syscall1((long)SYSCALL_KRNL(esKRNL_TimeSet), (long)ticks);
}

//os timer operation
void *esKRNL_TmrCreate(uint32_t period, uint8_t opt, OS_TMR_CALLBACK callback, void *callback_arg)
{
    void *result ;
    result = (void *)__syscall4((long)SYSCALL_KRNL(esKRNL_TmrCreate), (long)period, (long)opt, (long)callback, (long)callback_arg);
    return result;
}

int32_t esKRNL_TmrDel(void *ptmr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrDel), (long)ptmr);
    return result;
}

int16_t esKRNL_TmrError(void *ptmr)
{
    int16_t result ;
    result = (int16_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrError), (long)ptmr);
    return result;
}

uint32_t esKRNL_TmrRemainGet(void *ptmr)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrRemainGet), (long)ptmr);
    return result;
}

int32_t esKRNL_TmrStart(void *ptmr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrStart), (long)ptmr);
    return result;
}

uint8_t esKRNL_TmrStateGet(void  *ptmr)
{
    uint8_t result ;
    result = (uint8_t)__syscall1((long)SYSCALL_KRNL(esKRNL_TmrStateGet), (long)ptmr);
    return result;
}

int32_t esKRNL_TmrStop(void *ptmr, int8_t opt, void *callback_arg)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_KRNL(esKRNL_TmrStop), (long)ptmr, (long)opt, (long)callback_arg);
    return result;
}

uint16_t esKRNL_Version(void)
{
    uint16_t result ;
    result = (uint16_t)__syscall0((long)SYSCALL_KRNL(esKRNL_Version));
    return result;
}

long esKRNL_Ioctrl(void *hdle, int cmd, void *args)
{
    long result;
    result = (long) __syscall3((long)SYSCALL_KRNL(esKRNL_Ioctrl), (long)hdle, (long)cmd, (long)args);
    return result;
}

// KRSV
int32_t esKSRV_ClearWatchDog(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_ClearWatchDog));
    return result;
}
/*
uint32_t esKSRV_close_logo(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_close_logo));
    return result;
}
*/
int32_t esKSRV_DisableWatchDog(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_DisableWatchDog));
    return result;
}

int32_t esKSRV_DisableWatchDogSrv(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_DisableWatchDogSrv));
    return result;
}

int32_t esKSRV_EnableWatchDog(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_EnableWatchDog));
    return result;
}

int32_t esKSRV_EnableWatchDogSrv(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_EnableWatchDogSrv));
    return result;
}

int32_t esKSRV_GetAddPara(__ksrv_add_para *ksrv_add_para)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KSRV(esKSRV_GetAddPara), (long)ksrv_add_para);
    return result;
}

int32_t esKSRV_GetDramCfgPara(__dram_para_t *drampara)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KSRV(esKSRV_GetDramCfgPara), (long)drampara);
    return result;
}

unsigned long esKSRV_GetHighMsg(void)
{
    unsigned long result ;
    result = (unsigned long)__syscall0((long)SYSCALL_KSRV(esKSRV_GetHighMsg));
    return result;
}

unsigned long esKSRV_GetLowMsg(void)
{
    unsigned long result ;
    result = (unsigned long)__syscall0((long)SYSCALL_KSRV(esKSRV_GetLowMsg));
    return result;
}

unsigned long esKSRV_GetMsg(void)
{
    unsigned long result ;
    result = (unsigned long)__syscall0((long)SYSCALL_KSRV(esKSRV_GetMsg));
    return result;
}

int32_t esKSRV_GetPara(uint32_t type, void *format, void *para)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_KSRV(esKSRV_GetPara), (long)type, (long)format, (long)para);
    return result;
}

uint8_t esKSRV_GetSocID(void)
{
    uint8_t result ;
    result = (uint8_t)__syscall0((long)SYSCALL_KSRV(esKSRV_GetSocID));
    return result;
}

int32_t esKSRV_GetTargetPara(void *targetpara)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KSRV(esKSRV_GetTargetPara), (long)targetpara);
    return result;
}

uint32_t esKSRV_GetVersion(__epdk_ver_type_t type)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_KSRV(esKSRV_GetVersion), (long)type);
    return result;
}

void *esKSRV_Get_Display_Hld(void)
{
    void *result ;
    result = (void *)__syscall0((long)SYSCALL_KSRV(esKSRV_Get_Display_Hld));
    return result;
}

void esKSRV_Save_Mixture_Hld(int mid, void *mp)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_KSRV(esKSRV_Save_Mixture_Hld), (long)mid, (long)mp);
}

void esKSRV_Get_Mixture_Hld(int *mid, unsigned long *mp)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_KSRV(esKSRV_Get_Mixture_Hld), (long)mid, (long)mp);
}

int32_t esKSRV_memcpy(void *pdest, const void *psrc, size_t size)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_KSRV(esKSRV_memcpy), (long)pdest, (long)psrc, (long)size);
    return result;
}

int32_t esKSRV_memset(void *pmem, uint8_t value, size_t size)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_KSRV(esKSRV_memset), (long)pmem, (long)value, (long)size);
    return result;
}

void esKSRV_PowerOff(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_PowerOff));
    return;
}

uint32_t esKSRV_Random(uint32_t max)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_KSRV(esKSRV_Random), (long)max);
    return result;
}
/*
uint32_t esKSRV_release_logo_buf(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_release_logo_buf));
    return result;
}
*/
void esKSRV_Reset(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_Reset));
    return;
}

void esKSRV_Save_Display_Hld(void *hld)
{
    __s32 result ;
    result = (__s32)__syscall1((long)SYSCALL_KSRV(esKSRV_Save_Display_Hld), (long)hld);
}

int32_t esKSRV_SendMsg(uint32_t msgid, uint32_t prio)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_KSRV(esKSRV_SendMsg), (long)msgid, (long)prio);
    return result;
}

int32_t esKSRV_SendMsgEx(void *msg)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_KSRV(esKSRV_SendMsgEx), (long)msg);
    return result;
}

int32_t esKSRV_SysInfo(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_KSRV(esKSRV_SysInfo));
    return result;
}

// MEM
void *esMEMS_Balloc(unsigned long size)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_MEMS(esMEMS_Balloc), (long)size);
    return result;
}

void esMEMS_Bfree(void *addr, unsigned long size)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MEMS(esMEMS_Bfree), (long)addr, (long)size);
}

void *esMEMS_Calloc(void *heap, uint32_t n, uint32_t m)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_MEMS(esMEMS_Calloc), (long)heap, (long)n, (long)m);
    return result;
}

void esMEMS_CleanDCache(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_CleanDCache));
}

void esMEMS_CleanDCacheRegion(void *adr, uint32_t bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_CleanDCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_CleanFlushCache(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_CleanFlushCache));
}

void esMEMS_CleanFlushCacheRegion(void *adr, uint32_t bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_CleanFlushCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_CleanFlushDCache(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_CleanFlushDCache));
}

void esMEMS_CleanFlushDCacheRegion(void *adr, uint32_t bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_CleanFlushDCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_CleanInvalidateCacheAll(void)
{
    __syscall0((long)SYSCALL_MEMS(esMEMS_CleanInvalidateCacheAll));
}

void esMEMS_FlushCache(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_FlushCache));
}

void esMEMS_FlushCacheRegion(void *adr, uint32_t bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_FlushCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_FlushDCache(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_FlushDCache));
}

void esMEMS_FlushDCacheRegion(void *adr, uint32_t bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_FlushDCacheRegion), (long)adr, (long)bytes);
}

void esMEMS_FlushICache(void)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_FlushICache));
}

void esMEMS_FlushICacheRegion(void *adr, uint32_t bytes)
{
    __syscall2((long)SYSCALL_MEMS(esMEMS_FlushICacheRegion), (long)adr, (long)bytes);
}

uint32_t esMEMS_FreeMemSize(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_FreeMemSize));
    return result;
}

uint32_t esMEMS_GetIoVAByPA(uint32_t phyaddr, uint32_t size)
{
    uint32_t result;
    result = (uint32_t)__syscall1((long)SYSCALL_MEMS(esMEMS_GetIoVAByPA), (long)phyaddr);
    return result;
}

int32_t esMEMS_HeapCreate(void *heapaddr, uint32_t initnpage)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MEMS(esMEMS_HeapCreate), (long)heapaddr, (long)initnpage);
    return result;
}

void esMEMS_HeapDel(void *heap)
{
    int32_t result ;
    result = __syscall1((long)SYSCALL_MEMS(esMEMS_HeapDel), (long)heap);
}

int32_t esMEMS_Info(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_Info));
    return result;
}

void *esMEMS_Malloc(void *heap, uint32_t size)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_MEMS(esMEMS_Malloc), (long)heap, (long)size);
    return result;
}

void esMEMS_Mfree(void *heap, void *ptr)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MEMS(esMEMS_Mfree), (long)heap, (long)ptr);
}

void *esMEMS_Palloc(uint32_t npage, uint32_t mode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_MEMS(esMEMS_Palloc), (long)npage, (long)mode);
    return result;
}

void esMEMS_Pfree(void *mblk, uint32_t npage)
{
    uint32_t result ;
    result = (uint32_t)__syscall2((long)SYSCALL_MEMS(esMEMS_Pfree), (long)mblk, (long)npage);
}

void *esMEMS_Realloc(void *heap, void *f, uint32_t size)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_MEMS(esMEMS_Realloc), (long)heap, (long)f, (long)size);
    return result;
}

uint32_t esMEMS_TotalMemSize(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_TotalMemSize));
    return result;
}

unsigned long esMEMS_VA2PA(unsigned long vaddr)
{
    unsigned long result ;
    result = (unsigned long)__syscall1((long)SYSCALL_MEMS(esMEMS_VA2PA), (long)vaddr);
    return result;
}

void *esMEMS_VMalloc(uint32_t size)
{
    void *result;
    result = (void *)__syscall0((long)SYSCALL_MEMS(esMEMS_VMalloc));
    return result;
}

int32_t esMEMS_VMCreate(void *pBlk, uint32_t npage, int8_t domain)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_MEMS(esMEMS_VMCreate), (long)pBlk, (long)npage, (long)domain);
    return result;
}

void esMEMS_VMDelete(void *pBlk, uint32_t npage)
{
    uint32_t result ;
    result = (uint32_t)__syscall2((long)SYSCALL_MEMS(esMEMS_VMDelete), (long)pBlk, (long)npage);
}

void esMEMS_VMfree(void *ptr)
{
    uint32_t result;
    result = (uint32_t)__syscall0((long)SYSCALL_MEMS(esMEMS_VMfree));
}

int32_t esMEM_BWDisable(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_MEM(esMEM_BWDisable));
    return result;
}

int32_t esMEM_BWEnable(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_MEM(esMEM_BWEnable));
    return result;
}

int32_t esMEM_BWGet(__bw_dev_e mod)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MEM(esMEM_BWGet), mod);
    return result;
}

int32_t esMEM_DramSuspend(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_MEM(esMEM_DramSuspend));
    return result;
}

int32_t esMEM_DramWakeup(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_MEM(esMEM_DramWakeup));
    return result;
}

int32_t esMEM_MasterGet(__dram_dev_e mod, __dram_master_t *master)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MEM(esMEM_MasterGet), (long)mod, (long)master);
    return result;
}

int32_t esMEM_MasterSet(__dram_dev_e mod, __dram_master_t *master)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MEM(esMEM_MasterSet), (long)mod, (long)master);
    return result;
}

void *esMEM_RegDramAccess(uint8_t dev_type, __pCB_ClkCtl_t dram_access)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_MEM(esMEM_RegDramAccess), (long)dev_type, (long)dram_access);
    return result;
}

int32_t esMEM_ReleaseDramUsrMode(__dram_user_mode_t mode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MEM(esMEM_ReleaseDramUsrMode), (long)mode);
    return result;
}

int32_t esMEM_RequestDramUsrMode(__dram_user_mode_t mode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MEM(esMEM_RequestDramUsrMode), (long)mode);
    return result;
}

/*sram*/
int32_t esMEM_SramRelBlk(intptr_t *hSram)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MEM(esMEM_SramRelBlk), (long)hSram);
    return result;
}

intptr_t *esMEM_SramReqBlk(csp_sram_module_t uBlock, csp_sram_req_mode_e uMode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_MEM(esMEM_SramReqBlk), (long)uBlock, (long)uMode);
    return result;
}

int32_t esMEM_SramSwitchBlk(intptr_t *hSram, csp_sram_module_t uMap)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MEM(esMEM_SramSwitchBlk), (long)hSram, (long)uMap);
    return result;
}

int32_t esMEM_UnRegDramAccess(void *user)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MEM(esMEM_UnRegDramAccess), (long)user);
    return result;
}

// MODS
int32_t esMODS_MClose(__mp *mp)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MODS(esMODS_MClose), (long)mp);
    return result;
}

uint32_t esMODS_MInstall(const char *mfile, uint8_t mode)
{
    uint32_t result ;
    result = (uint32_t)__syscall2((long)SYSCALL_MODS(esMODS_MInstall), (long)mfile, (long)mode);
    return result;
}

long esMODS_MIoctrl(__mp *mp, uint32_t cmd, int32_t aux, void *pbuffer)
{
    long result ;
    result = (long)__syscall4((long)SYSCALL_MODS(esMODS_MIoctrl), (long)mp, (long)cmd, (long)aux, (long)pbuffer);
    return result;
}

__mp *esMODS_MOpen(uint8_t id, uint8_t mode)
{
    __mp *result ;
    result = (__mp *)__syscall2((long)SYSCALL_MODS(esMODS_MOpen), (long)id, (long)mode);
    return result;
}

uint32_t esMODS_MRead(void *pdata, uint32_t size, uint32_t n, __mp *mp)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_MODS(esMODS_MRead), (long)pdata, (long)size, (long)n, (long)mp);
    return result;
}

int32_t esMODS_MUninstall(uint8_t id)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MODS(esMODS_MUninstall), (long)id);
    return result;
}

uint32_t esMODS_MWrite(const void *pdata, uint32_t size, uint32_t n, __mp *mp)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_MODS(esMODS_MWrite), (long)pdata, (long)size, (long)n, (long)mp);
    return result;
}

// MSTUB
unsigned long esMSTUB_GetFuncEntry(int32_t id, uint32_t funcnum)
{
    uint32_t result ;
    result = (uint32_t)__syscall2((long)SYSCALL_MSTUB(esMSTUB_GetFuncEntry), (long)id, (long)funcnum);
    return result;
}

int32_t esMSTUB_RegFuncTbl(int32_t id, void *tbl)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_MSTUB(esMSTUB_RegFuncTbl), (long)id, (long)tbl);
    return result;
}

int32_t esMSTUB_UnRegFuncTbl(int32_t id)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_MSTUB(esMSTUB_UnRegFuncTbl), (long)id);
    return result;
}

// PINS
int32_t esPINS_ClearPending(void *hPin)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PINS(esPINS_ClearPending), (long)hPin);
    return result;
}

int32_t esPINS_DisbaleInt(void *hPin)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PINS(esPINS_DisbaleInt), (long)hPin);
    return result;
}

int32_t esPINS_EnbaleInt(void *hPin)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PINS(esPINS_EnbaleInt), (long)hPin);
    return result;
}

int32_t esPINS_GetPinGrpStat(void *hPin, user_gpio_set_t *pGrpStat, uint32_t GrpSize, __bool bFromHW)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_PINS(esPINS_GetPinGrpStat), (long)hPin, (long)pGrpStat, (long)GrpSize, (long)bFromHW);
    return result;
}

int32_t esPINS_GetPinStat(void *hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bFromHW)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_PINS(esPINS_GetPinStat), (long)hPin, (long)pPinStat, (long)pPinName, (long)bFromHW);
    return result;
}

int32_t esPINS_PinGrpRel(void *hPin, int32_t bRestore)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_PINS(esPINS_PinGrpRel), (long)hPin, (long)bRestore);
    return result;
}

void *esPINS_PinGrpReq(user_gpio_set_t *pGrpStat, uint32_t GrpSize)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_PINS(esPINS_PinGrpReq), (long)pGrpStat, (long)GrpSize);
    return result;
}

int32_t esPINS_QueryInt(void *hPin, __bool *pIntStatus)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_PINS(esPINS_QueryInt), (long)hPin, (long)pIntStatus);
    return result;
}

int32_t esPINS_ReadPinData(void *hPin, const char *pPinName)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_PINS(esPINS_ReadPinData), (long)hPin, (long)pPinName);
    return result;
}

int32_t esPINS_RegIntHdler(void *hPin, __pCBK_t hdler, void *arg)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_PINS(esPINS_RegIntHdler), (long)hPin, (long)hdler, (long)arg);
    return result;
}

int32_t esPINS_SetIntMode(void *hPin, uint32_t IntMode)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_PINS(esPINS_SetIntMode), (long)hPin, (long)IntMode);
    return result;
}

int32_t esPINS_SetPinDrive(void *hPin, uint32_t DriveLevel, const char *pPinName)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_PINS(esPINS_SetPinDrive), (long)hPin, (long)DriveLevel, (long)pPinName);
    return result;
}

int32_t esPINS_SetPinIO(void *hPin, __bool bOut, const char *pPinName)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_PINS(esPINS_SetPinIO), (long)hPin, (long)bOut, (long)pPinName);
    return result;
}

int32_t esPINS_SetPinPull(void *hPin, uint32_t PullStat, const char *pPinName)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_PINS(esPINS_SetPinPull), (long)hPin, (long)PullStat, (long)pPinName);
    return result;
}

int32_t esPINS_SetPinStat(void *hPin, user_gpio_set_t *pPinStat, const char *pPinName, __bool bSetUserStat)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_PINS(esPINS_SetPinStat), (long)hPin, (long)pPinStat, (long)pPinName, (long)bSetUserStat);
    return result;
}

int32_t esPINS_UnregIntHdler(void *hPin, __pCBK_t hdler)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_PINS(esPINS_UnregIntHdler), (long)hPin, (long)hdler);
    return result;
}

int32_t esPINS_WritePinData(void *hPin, uint32_t Value, const char *pPinName)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_PINS(esPINS_WritePinData), (long)hPin, (long)Value, (long)pPinName);
    return result;
}


// PWRMAN
void esPWRMAN_EnterStandby(uint32_t power_off_flag)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_PWRMAN(esPWRMAN_EnterStandby), (long)power_off_flag);
    return;
}

int32_t esPWRMAN_GetStandbyPara(__sys_pwrman_para_t *pStandbyPara)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PWRMAN(esPWRMAN_GetStandbyPara), (long)pStandbyPara);
    return result;
}

int32_t esPWRMAN_LockCpuFreq(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_PWRMAN(esPWRMAN_LockCpuFreq));
    return result;
}

int32_t esPWRMAN_RegDevice(__sys_pwrman_dev_e device, __pCB_DPMCtl_t cb, void *parg)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_PWRMAN(esPWRMAN_RegDevice), (long)device, (long)cb, (long)parg);
    return result;
}

int32_t esPWRMAN_RelPwrmanMode(int32_t mode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PWRMAN(esPWRMAN_RelPwrmanMode), (long)mode);
    return result;
}

int32_t esPWRMAN_ReqPwrmanMode(int32_t mode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PWRMAN(esPWRMAN_ReqPwrmanMode), (long)mode);
    return result;
}

int32_t esPWRMAN_SetStandbyMode(uint32_t standby_mode)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_PWRMAN(esPWRMAN_SetStandbyMode), (long)standby_mode);
    return result;
}

int32_t esPWRMAN_UnlockCpuFreq(void)
{
    int32_t result ;
    result = (int32_t)__syscall0((long)SYSCALL_PWRMAN(esPWRMAN_UnlockCpuFreq));
    return result;
}

int32_t esPWRMAN_UnregDevice(__sys_pwrman_dev_e device, __pCB_DPMCtl_t cb)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_PWRMAN(esPWRMAN_UnregDevice), (long)device, (long)cb);
    return result;
}

void esPWRMAN_UsrEventNotify(void)
{
    uint32_t result ;
    result = (uint32_t)__syscall0((long)SYSCALL_PWRMAN(esPWRMAN_UsrEventNotify));
    return;
}

// RESM
int32_t esRESM_RClose(__resm_rsb_t *res)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_RESM(esRESM_RClose), (long)res);
    return result;
}

void *esRESM_ROpen(const char *file, const char *mode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_RESM(esRESM_ROpen), (long)file, (long)mode);
    return result;
}

uint32_t esRESM_RRead(void *pdata, uint32_t size, uint32_t n, __resm_rsb_t *res)
{
    uint32_t result ;
    result = (uint32_t)__syscall4((long)SYSCALL_RESM(esRESM_RRead), (long)pdata, (long)size, (long)n, (long)res);
    return result;
}

int32_t esRESM_RSeek(__resm_rsb_t *res, int32_t Offset, int32_t Whence)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_RESM(esRESM_RSeek), (long)res, (long)Offset, (long)Whence);
    return result;
}

//SIOS
uint8_t esSIOS_getchar(void)
{
    uint8_t result ;
    result = (uint8_t)__syscall0((long)SYSCALL_SIOS(esSIOS_getchar));
    return result;
}

void esSIOS_gets(char *str)
{
    __syscall1((long)SYSCALL_SIOS(esSIOS_gets), (long)str);
}

void esSIOS_putarg(uint32_t arg, char format)
{
    __syscall2((long)SYSCALL_SIOS(esSIOS_putarg), (long)arg, (long)format);
}

void esSIOS_putstr(const char *str)
{
    __syscall1((long)SYSCALL_SIOS(esSIOS_putstr), (long)str);
}

void esSIOS_setbaud(uint32_t baud)
{
    __syscall1((long)SYSCALL_SIOS(esSIOS_setbaud), (long)baud);
}

// SVC
int32_t esSVC_RegCloseNode(void *handle)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegCloseNode), (long)handle);
    return result;
}

int32_t esSVC_RegClrError(void *handle)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegClrError), (long)handle);
    return result;
}

int32_t esSVC_RegCreateKey(void *hNode, const char *set_name, const char *key_name, const char *key_value)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_SVC(esSVC_RegCreateKey), (long)hNode, (long)set_name, (long)key_name, (long)key_value);
    return result;
}

int32_t esSVC_RegCreatePath(const char *path)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegCreatePath), (long)path);
    return result;
}

int32_t esSVC_RegCreateSet(void *handle, const char *set_name)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_SVC(esSVC_RegCreateSet), (long)handle, (long)set_name);
    return result;
}

int32_t esSVC_RegDeleteKey(void *hNode, const char *set_name, const char *key_name)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_SVC(esSVC_RegDeleteKey), (long)hNode, (long)set_name, (long)key_name);
    return result;
}

int32_t esSVC_RegDeleteNode(const  char *node)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegDeleteNode), (long)node);
    return result;
}

int32_t esSVC_RegDeletePath(const char *path)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegDeletePath), (long)path);
    return result;
}

int32_t esSVC_RegDeleteSet(void *handle, const char *set_name)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_SVC(esSVC_RegDeleteSet), (long)handle, (long)set_name);
    return result;
}

int32_t esSVC_RegGetError(void *handle)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegGetError), (long)handle);
    return result;
}

int32_t esSVC_RegGetFirstKey(void *handle, const char *set_name)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_SVC(esSVC_RegGetFirstKey), (long)handle, (long)set_name);
    return result;
}

int32_t esSVC_RegGetFirstSet(void *handle)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegGetFirstSet), (long)handle);
    return result;
}

int32_t esSVC_RegGetKeyCount(void *handle, const char *set_name, uint32_t *count_p)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_SVC(esSVC_RegGetKeyCount), (long)handle, (long)set_name, (long)count_p);
    return result;
}

int32_t esSVC_RegGetKeyValue(void *hNode, const char *set_name, const char *key_name, char *key_value)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_SVC(esSVC_RegGetKeyValue), (long)hNode, (long)set_name, (long)key_name, (long)key_value);
    return result;
}

int32_t esSVC_RegGetNextKey(void *hNode, const char *set_name, char *key_name)
{
    int32_t result ;
    result = (int32_t)__syscall3((long)SYSCALL_SVC(esSVC_RegGetNextKey), (long)hNode, (long)set_name, (long)key_name);
    return result;
}

int32_t esSVC_RegGetNextSet(void *handle, char *set_name)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_SVC(esSVC_RegGetNextSet), (long)handle, (long)set_name);
    return result;
}

void esSVC_RegGetRootPath(char *path)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegGetRootPath), (long)path);
    //return result;
}

int32_t esSVC_RegGetSetCount(void *handle, uint32_t *count_p)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_SVC(esSVC_RegGetSetCount), (long)handle, (long)count_p);
    return result;
}

int32_t esSVC_RegIni2Reg(const char *ini_file)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegIni2Reg), (long)ini_file);
    return result;
}

void *esSVC_RegOpenNode(const char *node, int32_t mode)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_SVC(esSVC_RegOpenNode), (long)node, (long)mode);
    return result;
}

int32_t esSVC_RegReg2Ini(const char *ini_file)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegReg2Ini), (long)ini_file);
    return result;
}

int32_t esSVC_RegSetKeyValue(void *hNode, const char *set_name, const char *key_name, const char *key_value)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_SVC(esSVC_RegSetKeyValue), (long)hNode, (long)set_name, (long)key_name, (long)key_value);
    return result;
}

void esSVC_RegSetRootPath(const char *path)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_SVC(esSVC_RegSetRootPath), (long)path);
    //return result;
}

int32_t esSVC_ResourceRel(void *hRes)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_SVC(esSVC_ResourceRel), (long)hRes);
    return result;
}

void *esSVC_ResourceReq(uint32_t res, uint32_t mode, uint32_t timeout)
{
    void *result ;
    result = (void *)__syscall3((long)SYSCALL_SVC(esSVC_ResourceReq), (long)res, (long)mode, (long)timeout);
    return result;
}

//TIMER
int32_t esTIME_ContiCntr(void *hCntr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_ContiCntr), (long)hCntr);
    return result;
}

int32_t esTIME_GetDate(__awos_date_t *date)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_GetDate), (long)date);
    return result;
}

int32_t esTIME_GetTime(__awos_time_t *time)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_GetTime), (long)time);
    return result;
}

int32_t esTIME_PauseCntr(void *hCntr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_PauseCntr), (long)hCntr);
    return result;
}

uint32_t esTIME_QuerryAlarm(void *alarm)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_TIME(esTIME_QuerryAlarm), (long)alarm);
    return result;
}

uint32_t esTIME_QuerryCntr(void *hCntr)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_TIME(esTIME_QuerryCntr), (long)hCntr);
    return result;
}

int32_t esTIME_QuerryCntrStat(void *hCntr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_QuerryCntrStat), (long)hCntr);
    return result;
}

uint32_t esTIME_QuerryTimer(int32_t timer_id)
{
    uint32_t result ;
    result = (uint32_t)__syscall1((long)SYSCALL_TIME(esTIME_QuerryTimer), (long)timer_id);
    return result;
}

int32_t esTIME_ReleaseAlarm(void *alarm)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_ReleaseAlarm), (long)alarm);
    return result;
}

int32_t esTIME_ReleaseCntr(void *hCntr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_ReleaseCntr), (long)hCntr);
    return result;
}

int32_t esTIME_ReleaseTimer(int32_t timer_id)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_ReleaseTimer), (long)timer_id);
    return result;
}

void *esTIME_RequestAlarm(uint32_t mode)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_TIME(esTIME_RequestAlarm), (long)mode);
    return result;
}

void *esTIME_RequestCntr(__pCB_ClkCtl_t cb, char *pUsr)
{
    void *result ;
    result = (void *)__syscall2((long)SYSCALL_TIME(esTIME_RequestCntr), (long)cb, (long)pUsr);
    return result;
}

int32_t esTIME_RequestTimer(__csp_timer_req_type_t *tmrType, __pCBK_t pHdlr, void *pArg, char *pUsr)
{
    int32_t result ;
    result = (int32_t)__syscall4((long)SYSCALL_TIME(esTIME_RequestTimer), (long)tmrType, (long)pHdlr, (long)pArg, (long)pUsr);
    return result;
}

int32_t esTIME_SetCntrPrescale(void *hCntr, int32_t prescl)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_TIME(esTIME_SetCntrPrescale), (long)hCntr, (long)prescl);
    return result;
}

int32_t esTIME_SetCntrValue(void *hCntr, uint32_t value)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_TIME(esTIME_SetCntrValue), (long)hCntr, (long)value);
    return result;
}

int32_t esTIME_SetDate(__awos_date_t *date)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_SetDate), (long)date);
    return result;
}

int32_t esTIME_SetTime(__awos_time_t *time)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_SetTime), (long)time);
    return result;
}

int32_t esTIME_StartAlarm(void *alarm, uint32_t time)
{
    int32_t result ;
    result = (int32_t)__syscall2((long)SYSCALL_TIME(esTIME_StartAlarm), (long)alarm, (long)time);
    return result;
}

int32_t esTIME_StartCntr(void *hCntr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_StartCntr), (long)hCntr);
    return result;
}

int32_t esTIME_StartTimer(int32_t timer_id)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_StartTimer), (long)timer_id);
    return result;
}

int32_t esTIME_StopAlarm(void *alarm)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_StopAlarm), (long)alarm);
    return result;
}

int32_t esTIME_StopCntr(void *hCntr)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_StopCntr), (long)hCntr);
    return result;
}

int32_t esTIME_StopTimer(int32_t timer_id)
{
    int32_t result ;
    result = (int32_t)__syscall1((long)SYSCALL_TIME(esTIME_StopTimer), (long)timer_id);
    return result;
}

#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
int get_log_level(void)
{
    int32_t result;
    result = (int32_t)__syscall0((long)SYSCALL_KRNL(get_log_level));
    return result;
}

void set_log_level(int level)
{
    int32_t result;
    result = (int32_t)__syscall1((long)SYSCALL_KRNL(set_log_level), (long)level);
}
#endif

#ifdef CONFIG_DEBUG_BACKTRACE
int backtrace(char *name, void *output[], int size, int offset, print_function p)
{
    int32_t result;
    result = (int32_t)__syscall5((long)SYSCALL_KRNL(backtrace), (unsigned long)name, (unsigned long)output,
                                 (unsigned long)size, (unsigned long)offset, (unsigned long)p);
    return result;
}
#endif

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    int32_t result;
    result = (int32_t)__syscall5((long)SYSCALL_KRNL(select), (unsigned long)nfds, (unsigned long)readfds,
                                 (unsigned long)writefds, (unsigned long)exceptfds, (unsigned long)exceptfds);
    return result;
}
void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset)
{
    unsigned long result;
    result = (unsigned long)__syscall6((long)SYSCALL_KRNL(select), (unsigned long)addr, (unsigned long)length,
                                 (unsigned long)prot, (unsigned long)flags, (unsigned long)fd,(unsigned long)offset);
    return (void *)result;
}

#ifdef CONFIG_KASAN

#define __alias(symbol) __attribute__((alias(#symbol)))

#define DEFINE_ASAN_LOAD_STORE(size)                    \
    void __asan_load##size(unsigned long addr)          \
    {                               \
        __syscall1((long)SYSCALL_KASANOPS(__asan_load##size), (unsigned long)addr); \
    }                               \
    __alias(__asan_load##size)                  \
    void __asan_load##size##_noabort(unsigned long);        \
    void __asan_store##size(unsigned long addr)         \
    {                               \
        __syscall1((long)SYSCALL_KASANOPS(__asan_store##size), (unsigned long)addr); \
    }                               \
    __alias(__asan_store##size)                 \
    void __asan_store##size##_noabort(unsigned long);       \

DEFINE_ASAN_LOAD_STORE(1);
DEFINE_ASAN_LOAD_STORE(2);
DEFINE_ASAN_LOAD_STORE(4);
DEFINE_ASAN_LOAD_STORE(8);
DEFINE_ASAN_LOAD_STORE(16);

void __asan_loadN(unsigned long addr, size_t size)
{
    __syscall2((long)SYSCALL_KASANOPS(__asan_loadN), (unsigned long)addr, (unsigned long)size);
}

__alias(__asan_loadN)
void __asan_loadN_noabort(unsigned long, size_t);

void __asan_storeN(unsigned long addr, size_t size)
{
    __syscall2((long)SYSCALL_KASANOPS(__asan_storeN), (unsigned long)addr, (unsigned long)size);
}

__alias(__asan_storeN)
void __asan_storeN_noabort(unsigned long, size_t);

void __asan_poison_stack_memory(const void *addr, size_t size)
{
    __syscall2((long)SYSCALL_KASANOPS(__asan_poison_stack_memory), (unsigned long)addr, (unsigned long)size);
}

void __asan_unpoison_stack_memory(const void *addr, size_t size)
{
    __syscall2((long)SYSCALL_KASANOPS(__asan_unpoison_stack_memory), (unsigned long)addr, (unsigned long)size);
}

void __asan_alloca_poison(unsigned long addr, size_t size)
{
    __syscall2((long)SYSCALL_KASANOPS(__asan_alloca_poison), (unsigned long)addr, (unsigned long)size);
}

void __asan_allocas_unpoison(const void *stack_top, const void *stack_bottom)
{
    __syscall2((long)SYSCALL_KASANOPS(__asan_alloca_unpoison), (unsigned long)stack_top, (unsigned long)stack_bottom);
}

void __asan_handle_no_return(void)
{
    __syscall0((long)SYSCALL_KASANOPS(__asan_handle_no_return));
}

void __asan_unregister_globals(void)
{
    __syscall0((long)SYSCALL_KASANOPS(__asan_unregister_globals));
}

void __asan_register_globals(void)
{
    __syscall0((long)SYSCALL_KASANOPS(__asan_register_globals));
}
#endif
