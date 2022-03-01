#ifndef _AW_MEMORY_H_
#define _AW_MEMORY_H_

/*
 * =====================================================================================
 *
 *       Filename:  memory.h
 *
 *    Description:  get memory space for nna input
 *
 *        Version:  1.0
 *        Created:  07/18/2019 04:26:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jilinglin 
 *        Company:  allwinnertech.com
 *
 * =====================================================================================
 */ 
#include <stddef.h>

typedef struct MemBufInfoT {
	int          dataFd;
    void*        virAddr;
	void*        phyAddr;
	unsigned int bufferSize;
} MemBufInfo;

typedef struct MemParameter {
    int iommuFlag;       
} MemParameter;

struct AwMemOpsS
{
    int (*open)(MemParameter);

    void (*close)(void);

    int (*total_size)(void);

    int (*palloc)(MemBufInfo *,int /* bufferInfo number*/);

    void  (*pfree)(MemBufInfo *,int /* bufferInfo number*/);

    void (*flush_cache)(void * /*mem*/, int /*size*/);

    void *(*cpu_get_phyaddr)(void * /*viraddr*/);

    void *(*cpu_get_viraddr)(void * /*phyaddr*/);

    int (*mem_set)(void * /*s*/, int /*c*/, size_t /*n*/);

    int (*mem_cpy)(void * /*dest*/, void * /*src*/, size_t /*n*/);

    int (*mem_read)(void * /*dest */, void * /*src*/, size_t /*n*/);

    int (*mem_write)(void * /*dest*/, void * /*src*/, size_t /*n*/);

    int (*setup)(void);

    int (*shutdown)(void);

};

struct AwMemOpsS* GetMemAdapterOpsS();

static inline int AwMemOpen(struct AwMemOpsS *memops,MemParameter parm)
{
    return memops->open(parm);
}

//* close the memory adapter.
static inline void AwMemClose(struct AwMemOpsS *memops)
{
    memops->close();
}

static inline int AwMemTotalSize(struct AwMemOpsS *memops)
{
    return memops->total_size();
}

static inline int AwMemPalloc(struct AwMemOpsS *memops, MemBufInfo* buf_info, int number)
{
    return memops->palloc(buf_info, number);
}

static inline void AwMemPfree(struct AwMemOpsS *memops, MemBufInfo* buf_info,int number)
{
    memops->pfree(buf_info, number);
}

static inline void AwMemFlushCache(struct AwMemOpsS *memops, void* pMem, int nSize)
{
    memops->flush_cache(pMem, nSize);
}

static inline void AwMemSet(struct AwMemOpsS *memops, void* pMem, int nValue, int nSize)
{
   memops->mem_set(pMem, nValue, nSize);
}

static inline void AwMemCopy(struct AwMemOpsS *memops, void* pMemDst, void* pMemSrc, int nSize)
{
    memops->mem_cpy(pMemDst, pMemSrc, nSize);
}

static inline int AwMemRead(struct AwMemOpsS *memops, void* pMemDst, void* pMemSrc, int nSize)
{
    memops->mem_read(pMemDst, pMemSrc, nSize);
    return 0;
}

static inline int AwMemWrite(struct AwMemOpsS *memops,void* pMemDst,  void* pMemSrc, int nSize)
{
    (void)memops; /*not use memops */
    memops->mem_write(pMemDst, pMemSrc, nSize);
    return 0;
}

static inline void *AwMemGetPhysicAddressCpu(struct AwMemOpsS *memops, void *virt)
{
    return memops->cpu_get_phyaddr(virt);
}

static inline void *AwMemGetVirtualAddressCpu(struct AwMemOpsS *memops, void *phy)
{
    return memops->cpu_get_viraddr(phy);
}

static inline int AwMemSetup(struct AwMemOpsS *memops)
{
    return memops->setup();
}

static inline int AwMemShutdown(struct AwMemOpsS *memops)
{
    return memops->shutdown();
}


#endif
