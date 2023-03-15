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
#ifndef SC_INTERFACE_H
#define SC_INTERFACE_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct ScMemOpsS
{
    int (*open)(void);

    void (*close)(void);

    int (*total_size)(void);

    void *(*palloc)(int /*size*/, void *, void *);

    void *(*palloc_no_cache)(int /*size*/, void *, void *);

    void  (*pfree)(void* /*mem*/, void *, void *);

    void (*flush_cache)(void * /*mem*/, int /*size*/);

    void *(*ve_get_phyaddr)(void * /*viraddr*/);

    void *(*ve_get_viraddr)(void * /*phyaddr*/);

    void *(*cpu_get_phyaddr)(void * /*viraddr*/);

    void *(*cpu_get_viraddr)(void * /*phyaddr*/);

    int (*mem_set)(void * /*s*/, int /*c*/, size_t /*n*/);

    int (*mem_cpy)(void * /*dest*/, void * /*src*/, size_t /*n*/);

    int (*mem_read)(void * /*dest */, void * /*src*/, size_t /*n*/);

    int (*mem_write)(void * /*dest*/, void * /*src*/, size_t /*n*/);

    int (*setup)(void);

    int (*shutdown)(void);

    //*the interface just for secureAlloc.c, not used by other function
    //void *(*palloc_secure)(int /*size*/, void*, void*);

    unsigned int (*get_ve_addr_offset)(void);
};

static inline int CdcMemOpen(struct ScMemOpsS *memops)
{
    assert(memops != NULL);
    return memops->open();
}

//* close the memory adapter.
static inline void CdcMemClose(struct ScMemOpsS *memops)
{
    assert(memops != NULL);
    memops->close();
}

static inline int CdcMemTotalSize(struct ScMemOpsS *memops)
{
    assert(memops != NULL);
    return memops->total_size();
}

static inline void *CdcMemPalloc(struct ScMemOpsS *memops, int nSize, void *veOps, void *pVeopsSelf)
{
    assert(memops != NULL);
    return memops->palloc(nSize, veOps, pVeopsSelf);
}

static inline void CdcMemPfree(struct ScMemOpsS *memops, void* pMem, void *veOps, void *pVeopsSelf)
{
    assert(memops != NULL);
    memops->pfree(pMem, veOps, pVeopsSelf);
}

static inline void CdcMemFlushCache(struct ScMemOpsS *memops, void* pMem, int nSize)
{
    assert(memops != NULL);
    memops->flush_cache(pMem, nSize);
}

static inline void *CdcMemGetPhysicAddress(struct ScMemOpsS *memops, void* pVirtualAddress)
{
    assert(memops != NULL);
    return memops->ve_get_phyaddr(pVirtualAddress);
}

static inline void *CdcMemGetVirtualAddress(struct ScMemOpsS *memops, void* pPhysicAddress)
{
    assert(memops != NULL);
    return memops->ve_get_viraddr(pPhysicAddress);
}

static inline void CdcMemSet(struct ScMemOpsS *memops, void* pMem, int nValue, int nSize)
{
    assert(memops != NULL);
    memops->mem_set(pMem, nValue, nSize);
}

static inline void CdcMemCopy(struct ScMemOpsS *memops, void* pMemDst, void* pMemSrc, int nSize)
{
    assert(memops != NULL);
    memops->mem_cpy(pMemDst, pMemSrc, nSize);
}

static inline int CdcMemRead(struct ScMemOpsS *memops, void* pMemDst, void* pMemSrc, int nSize)
{
    assert(memops != NULL);
    memops->mem_read(pMemDst, pMemSrc, nSize);
    return 0;
}

static inline int CdcMemWrite(struct ScMemOpsS *memops,void* pMemDst,  void* pMemSrc, int nSize)
{
    assert(memops != NULL);
    memops->mem_write(pMemDst, pMemSrc, nSize);
    return 0;
}

static inline void *CdcMemGetPhysicAddressCpu(struct ScMemOpsS *memops, void *virt)
{
    assert(memops != NULL);
    return memops->cpu_get_phyaddr(virt);
}

static inline void *CdcMemGetVirtualAddressCpu(struct ScMemOpsS *memops, void *phy)
{
    assert(memops != NULL);
    return memops->cpu_get_viraddr(phy);
}

static inline int CdcMemSetup(struct ScMemOpsS *memops)
{
    assert(memops != NULL);
    return memops->setup();
}

static inline int CdcMemShutdown(struct ScMemOpsS *memops)
{
    assert(memops != NULL);
    return memops->shutdown();
}

#if 0
static inline void *CdcMemPallocSecure(struct ScMemOpsS *memops, int nSize,
                                              void *veOps, void *pVeopsSelf)
{
    assert(memops != NULL);
    return memops->palloc_secure(nSize,veOps, pVeopsSelf);
}
#endif

static inline unsigned int CdcMemPallocGetVeAddrOffset(struct ScMemOpsS *memops)
{
    assert(memops != NULL);
    return memops->get_ve_addr_offset();
}

#ifdef __cplusplus
}
#endif

#endif
