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
#include "cdc_log.h"

#include "EncAdapter.h"

//* use method provide by libVE.so to process ve control methods.
//* use method provide by libMemAdapter.so to process physical continue memory allocation.

int EncAdapterInitializeMem(struct ScMemOpsS *memops)
{
    struct ScMemOpsS *_memops = memops;

    if(EncMemAdapterOpen() < 0)
        return -1;
    //If mediaserver died when playing protected media,
    //some hardware is in protected mode, shutdown.
#if 0 // TODO: do it in outside.
{
    SecureMemAdapterOpen();
    SecureMemAdapterClose();
}
#endif
    return 0;
}

void EncAdpaterRelease(struct ScMemOpsS *memops)
{
    struct ScMemOpsS *_memops = memops;
    if(memops)
    {
        EncMemAdapterClose();
    }
    return;
}

//* memory methods.

int __EncAdapterMemOpen(struct ScMemOpsS *memops)
{
    return memops->open();
}

void __EncAdapterMemClose(struct ScMemOpsS *memops)
{
    return memops->close();
}

void* __EncAdapterMemPalloc(struct ScMemOpsS *memops, int nSize, void *veOps, void *pVeopsSelf)
{
    return memops->palloc(nSize, veOps, pVeopsSelf);
}

void* __EncAdapterMemNoCachePalloc(struct ScMemOpsS *memops, int nSize, void *veOps, void *pVeopsSelf)
{
    return memops->palloc_no_cache(nSize, veOps, pVeopsSelf);
}

void __EncAdapterMemPfree(struct ScMemOpsS *memops, void* pMem, void *veOps, void *pVeopsSelf)
{
    memops->pfree(pMem, veOps, pVeopsSelf);
}

void __EncAdapterMemFlushCache(struct ScMemOpsS *memops, void* pMem, int nSize)
{
    memops->flush_cache(pMem, nSize);
}

void* __EncAdapterMemGetPhysicAddress(struct ScMemOpsS *memops, void* pVirtualAddress)
{
    return memops->ve_get_phyaddr(pVirtualAddress);
}

void* __EncAdapterMemGetPhysicAddressCpu(struct ScMemOpsS *memops, void* pVirtualAddress)
{
    return memops->cpu_get_phyaddr(pVirtualAddress);
}

void* __EncAdapterMemGetVirtualAddress(struct ScMemOpsS *memops, void* pPhysicAddress)
{
    return memops->ve_get_viraddr(pPhysicAddress);
}

unsigned int __EncAdapterMemGetVeAddrOffset(struct ScMemOpsS *memops)
{
    return memops->get_ve_addr_offset();
}

unsigned int EncAdapterGetICVersion(void* nTopBaseAddr)
{
   volatile unsigned int value;
   value = *((unsigned int*)((char *)nTopBaseAddr + 0xf0));
   if(value == 0)
   {
        value = *((unsigned int*)((char *)nTopBaseAddr + 0xe4));
        if(value == 0)
        {
            loge("can not get the ve version ,both 0xf0 and 0xe4 is 0x00000000\n");
            return 0;
        }
        else
            return value;
   }
   else
        return (value>>16);
}

void EncAdapterPrintTopVEReg(void* pTopBaseAddr)
{
    int i;
    volatile int *ptr = (int*)pTopBaseAddr;

    logd("--------- register of top level ve base:%p -----------\n",ptr);
    for(i=0;i<16;i++)
    {
        logd("reg%02x:%08x %08x %08x %08x",i*16,ptr[0],ptr[1],ptr[2],ptr[3]);
        ptr += 4;
    }
}

void EncAdapterPrintEncReg(void* pTopBaseAddr)
{
    int i;
    volatile int *ptr = (int*)((unsigned long)pTopBaseAddr + 0xB00);

    logd("--------- register of ve encoder base:%p -----------\n",ptr);
    for(i=0;i<16;i++)
    {
        logd("reg%02x:%08x %08x %08x %08x",i*16,ptr[0],ptr[1],ptr[2],ptr[3]);
        ptr += 4;
    }
}

void EncAdapterPrintIspReg(void* pTopBaseAddr)
{
    CEDARC_UNUSE(__EncAdapterMemGetVirtualAddress);

    int i;
    volatile int *ptr = (int*)((unsigned long)pTopBaseAddr + 0xA00);

    logd("--------- register of ve isp base:%p -----------\n",ptr);
    for(i=0;i<16;i++)
    {
        logd("reg%02x:%08x %08x %08x %08x",i*16,ptr[0],ptr[1],ptr[2],ptr[3]);
        ptr += 4;
    }
}
