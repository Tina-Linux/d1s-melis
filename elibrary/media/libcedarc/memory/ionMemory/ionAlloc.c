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
/*
 * ion_alloc.c
 *
 * john.fu@allwinnertech.com
 *
 * ion memory allocate
 *
 */

#include <list.h>
#include <kapi.h>
#include <debug.h>
#include <stdlib.h>
#include "cdc_log.h"
#include "typedef.h"
#include "ionAllocEntry.h"
#include "sc_interface.h"

#ifdef MEM_DEBUG
#define DBG(fmt, ...)           do{ eLIBs_printf("%s line %d, "fmt, __func__, __LINE__, ##__VA_ARGS__); } while(0)
#define BUG()                   do{ DBG("BUG happend!\n"); }while(0)
#define ENTER_FUNCTION(...)     do{ DBG("enter function!\n"); }while(0)
#define LEAVE_FUNCTION(...)     do{ DBG("leave function!\n"); }while(0)
#else
#define BUG()                   do{ DBG("BUG happend!\n"); }while(0)
#define DBG(fmt, ...)           do{ ; }while(0)
#define ENTER_FUNCTION(...)     do{ ; }while(0)
#define LEAVE_FUNCTION(...)     do{ ; }while(0)
#endif

typedef struct melis_alloc_context
{
    struct list_head    list;
    __hdle              lock;
    int                 open_flag;
    int                 ref_cnt;    // reference count
} melis_alloc_context_t;

typedef struct melis_buffer_node
{
    struct list_head    i_list;
    unsigned long       phy;
    void                *vir;
    unsigned int        size;
} melis_buffer_node_t;

static melis_alloc_context_t g_physical_list =
{
    .list       = LIST_HEAD_INIT(g_physical_list.list),
    .lock       = NULL,
    .open_flag  = 0,
    .ref_cnt    = 0,
};

/*funciton begin*/
int ion_alloc_open(void)
{
    ENTER_FUNCTION();
    uint32_t cpu_sr;
    cpu_sr = awos_arch_lock_irq();

    if (g_physical_list.open_flag)
    {
        g_physical_list.ref_cnt++;
        awos_arch_unlock_irq(cpu_sr);
        LEAVE_FUNCTION();
        return 0;
    }

    INIT_LIST_HEAD(&g_physical_list.list);
    g_physical_list.lock        = NULL;
    g_physical_list.open_flag   = 1;
    g_physical_list.ref_cnt++;
    awos_arch_unlock_irq(cpu_sr);

    LEAVE_FUNCTION();
    return 0;
}

void ion_alloc_close(void)
{
    uint32_t               cpu_sr;
    struct list_head       *pos;
    struct list_head       *q;
    melis_buffer_node_t    *tmp;
    unsigned char           err;
    ENTER_FUNCTION();
    cpu_sr = awos_arch_lock_irq();
    if(--g_physical_list.ref_cnt <= 0)
    {
        list_for_each_safe(pos, q, &g_physical_list.list)
        {
            tmp = list_entry(pos, melis_buffer_node_t, i_list);
            list_del(pos);
            esMEMS_Pfree(tmp->vir, (tmp->size + 1023) / 1024);
            free(tmp);
        }
        INIT_LIST_HEAD(&g_physical_list.list);
        g_physical_list.lock      = NULL;
        g_physical_list.open_flag = 0;
    }
    else
    {
        logv("ref cnt: %d > 0, do not free \n", g_physical_list.ref_cnt);
    }
    awos_arch_unlock_irq(cpu_sr);
    LEAVE_FUNCTION();
    return;
}

void *ion_alloc_palloc(int size, void *veOps, void *pVeopsSelf)
{
    ENTER_FUNCTION();
    melis_buffer_node_t    *new;
    unsigned char           err;
    melis_buffer_node_t    *tmp;
    new = malloc(sizeof(melis_buffer_node_t));

    if (new == NULL)
    {
        loge("malloc node error.");
        LEAVE_FUNCTION();
        return NULL;
    }

    memset(new, 0x00, sizeof(melis_buffer_node_t));


    INIT_LIST_HEAD(&new->i_list);
    new->size = size;
    new->vir = esMEMS_Palloc((size + 1023) / 1024, 0);

    if (new->vir == NULL)
    {
        free(new);
        loge("palloc failure.");
        LEAVE_FUNCTION();
        return NULL;
    }

    new->phy = esMEMS_VA2PA((unsigned long)new->vir);
    unsigned long cpsr_level = esKRNL_InterruptDisable();
    list_add(&new->i_list, &g_physical_list.list);
    esKRNL_InterruptEnable(cpsr_level);

    LEAVE_FUNCTION();
    return new->vir;
}

void* ion_alloc_no_cache_palloc(int size, void *veOps, void *pVeopsSelf)
{
    unsigned char bIsCache = 0;
    return NULL;//ion_alloc_palloc_base(size, veOps, pVeopsSelf, bIsCache);
}

void ion_alloc_pfree(void * pbuf, void *veOps, void *pVeopsSelf)
{
    unsigned char           err;
    struct list_head       *pos;
    struct list_head       *q;
    melis_buffer_node_t    *tmp;
    unsigned char           found = 0;
    int                     freesize = 0;
    ENTER_FUNCTION();

    if (pbuf == NULL)
    {
        loge("invalid ptr to free.");
        LEAVE_FUNCTION();
        return;
    }

    unsigned long cpsr_level = esKRNL_InterruptDisable();
    list_for_each_safe(pos, q, &g_physical_list.list)
    {
        tmp = list_entry(pos, melis_buffer_node_t, i_list);

        if (tmp->vir == pbuf)
        {
            found = 1;
            freesize = tmp->size;
            esMEMS_Pfree(pbuf, (freesize + 1023) / 1024);
            list_del(pos);
            free(tmp);
            break;
        }
    }
    esKRNL_InterruptEnable(cpsr_level);

    if (found == 0)
    {
        loge("cant found the error memory node need to free!");
    }

    LEAVE_FUNCTION();
    return;
}

static void *__ion_alloc_get_phyaddr(void *pvirtaddr)
{
    unsigned char           err;
    struct list_head       *pos;
    struct list_head       *q;
    melis_buffer_node_t    *tmp;
    unsigned char           found = 0;
    unsigned long           addr_phy = 0;
    ENTER_FUNCTION();
    unsigned long cpsr_level = esKRNL_InterruptDisable();
    list_for_each_safe(pos, q, &g_physical_list.list)
    {
        tmp = list_entry(pos, melis_buffer_node_t, i_list);

        if ((pvirtaddr >= tmp->vir) && ((unsigned long)pvirtaddr < (unsigned long)((unsigned long)tmp->vir + tmp->size)))
        {
            found = 1;
            addr_phy = (unsigned long)tmp->phy + (unsigned long)pvirtaddr - (unsigned long)tmp->vir;
            break;
        }
    }
    esKRNL_InterruptEnable(cpsr_level);

    if (found == 0)
    {
        loge("cant found the error memory node need to free!");
    }

    LEAVE_FUNCTION();
    return (void *)addr_phy;
}

static void *__ion_alloc_get_virtaddr(void *pphyaddr)
{
    unsigned char           err;
    struct list_head       *pos;
    struct list_head       *q;
    melis_buffer_node_t    *tmp;
    unsigned char           found = 0;
    unsigned long           addr_virt = 0;
    ENTER_FUNCTION();
    unsigned long cpsr_level = esKRNL_InterruptDisable();
    list_for_each_safe(pos, q, &g_physical_list.list)
    {
        tmp = list_entry(pos, melis_buffer_node_t, i_list);

        if (((unsigned long)pphyaddr >= tmp->phy) && ((unsigned long)pphyaddr < (tmp->phy + tmp->size)))
        {
            found = 1;
            addr_virt = (unsigned long)tmp->vir + (unsigned long)pphyaddr - (unsigned long)tmp->phy;
            break;
        }
    }
    esKRNL_InterruptEnable(cpsr_level);

    if (found == 0)
    {
        loge("cant found the error memory node need to free!");
    }

    LEAVE_FUNCTION();
    return (void *)addr_virt;
}

void* ion_alloc_vir2phy_cpu(void * pbuf)
{
    ENTER_FUNCTION();
    void *p = __ion_alloc_get_phyaddr(pbuf);
    LEAVE_FUNCTION();
    return p;
}

void* ion_alloc_phy2vir_cpu(void * pbuf)
{
    ENTER_FUNCTION();
    void *p = __ion_alloc_get_virtaddr(pbuf);
    LEAVE_FUNCTION();
    return p;
}

void* ion_alloc_vir2phy_ve(void * pbuf)
{
    ENTER_FUNCTION();
    void *p = __ion_alloc_get_phyaddr(pbuf);
    LEAVE_FUNCTION();
    return p;
}

void* ion_alloc_phy2vir_ve(void * pbuf)
{
    ENTER_FUNCTION();
    void *p = __ion_alloc_get_virtaddr(pbuf);
    LEAVE_FUNCTION();
    return p;
}

void ion_alloc_flush_cache(void* startAddr, int size)
{
    ENTER_FUNCTION();
    esMEMS_CleanFlushDCacheRegion(startAddr, size);
    LEAVE_FUNCTION();
    return;
}

//return total meminfo with MB
int ion_alloc_get_total_size(void)
{
    ENTER_FUNCTION();
    LEAVE_FUNCTION();
    return TOTAL_MEM_SIZE;
}

int ion_alloc_memset(void* buf, int value, size_t n)
{
    memset(buf, value, n);
    return -1;
}

int ion_alloc_copy(void* dst, void* src, size_t n)
{
    memcpy(dst, src, n);
    return -1;
}

int ion_alloc_read(void* dst, void* src, size_t n)
{
    memcpy(dst, src, n);
    return -1;
}

int ion_alloc_write(void* dst, void* src, size_t n)
{
    memcpy(dst, src, n);
    return -1;
}

int ion_alloc_setup(void)
{
    return -1;
}

int ion_alloc_shutdown(void)
{
    return -1;
}

unsigned int ion_alloc_get_ve_addr_offset(void)
{
	return 0;
}


struct ScMemOpsS _ionMemOpsS =
{
    .open  =              ion_alloc_open,
    .close =              ion_alloc_close,
    .total_size=          ion_alloc_get_total_size,
    .palloc =             ion_alloc_palloc,
    .palloc_no_cache =    ion_alloc_no_cache_palloc,
    .pfree =              ion_alloc_pfree,
    .flush_cache =        ion_alloc_flush_cache,
    .ve_get_phyaddr=      ion_alloc_vir2phy_ve,
    .ve_get_viraddr =     ion_alloc_phy2vir_ve,
    .cpu_get_phyaddr =    ion_alloc_vir2phy_cpu,
    .cpu_get_viraddr =    ion_alloc_phy2vir_cpu,
    .mem_set =            ion_alloc_memset,
    .mem_cpy =            ion_alloc_copy,
    .mem_read =           ion_alloc_read,
    .mem_write =          ion_alloc_write,
    .setup =              ion_alloc_setup,
    .shutdown =           ion_alloc_shutdown,
    //.palloc_secure =      ion_alloc_alloc_drm,
    .get_ve_addr_offset = ion_alloc_get_ve_addr_offset

};

struct ScMemOpsS* __GetIonMemOpsS(void)
{
    logd("*** get __GetIonMemOpsS ***");
    return &_ionMemOpsS;
}
