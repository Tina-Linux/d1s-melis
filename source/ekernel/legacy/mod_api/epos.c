/*
 * ===========================================================================================
 *
 *       Filename:  epos.c
 *
 *    Description:  Implementation of Interface needed by melis system API.
 *
 *        Version:  Melis3.0
 *         Create:  2018-03-12 11:57:39
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-28 09:35:44
 *
 * ===========================================================================================
 */

#include "epos.h"
#include <log.h>
#include <time.h>

#ifdef CONFIG_DEBUG_BACKTRACE
#include <backtrace.h>
#endif
#if defined(CONFIG_RISCV)
#include "riscv/sbi/sbi.h"
#endif

static __cache_way_t LockICacheWays[MAX_ICACHE_LOCKED_WAY];
static __cache_way_t LockDCacheWays[MAX_DCACHE_LOCKED_WAY];

static uint32_t timer_error;
melis_malloc_context_t g_mem_leak_list =
{
    .list       = LIST_HEAD_INIT(g_mem_leak_list.list),
    .ref_cnt    = 0,
    .open_flag  = 0,
};

melis_thread_t melis_thread =
{
    .thr_id = {{0, 0, RT_NULL}, }
};

extern void *__internal_malloc(uint32_t size);
extern void __internal_free(void *ptr);
extern void hal_rtc_write_data(int index, uint32_t val);
extern uint32_t hal_rtc_read_data(int index);
extern void syscall_trap(void);
static int32_t find_thread(uint32_t prio);

void epos_memleak_detect_enable(void)
{
    register rt_base_t  temp;

    temp = rt_hw_interrupt_disable();
    if (g_mem_leak_list.open_flag)
    {
        rt_hw_interrupt_enable(temp);
        return;
    }
    INIT_LIST_HEAD(&g_mem_leak_list.list);
    g_mem_leak_list.ref_cnt     = 1;
    g_mem_leak_list.open_flag   = 1;
    rt_hw_interrupt_enable(temp);

    return;
}

void epos_memleak_detect_disable(void)
{
    register rt_base_t      cpu_sr;
    struct list_head        *pos;
    struct list_head        *q;
    uint32_t                count = 0;
    uint32_t                i = 0;

    cpu_sr = rt_hw_interrupt_disable();

    __log("memory leak nodes:");
    list_for_each_safe(pos, q, &g_mem_leak_list.list)
    {
        melis_heap_buffer_node_t *tmp;
        tmp = list_entry(pos, melis_heap_buffer_node_t, i_list);
        rt_kprintf("\t %03d: ptr = 0x%08x, size = 0x%08x, entry = 0x%08x, thread = %s, tick = %d.\n", \
                   count ++, (unsigned long)tmp->vir, (uint32_t)tmp->size, (uint32_t)tmp->entry, tmp->name, tmp->tick);
        for (i = 0; i < CONFIG_MEMORY_LEAK_BACKTRACE_LEVEL; i++)
        {
            if (tmp->caller[i] != NULL)
            {
                rt_kprintf("                backtrace : 0x%08x\n", tmp->caller[i]);
            }
        }
        list_del(pos);
        rt_free(tmp);
    }
    g_mem_leak_list.ref_cnt   = 0;
    g_mem_leak_list.open_flag = 0;

    rt_hw_interrupt_enable(cpu_sr);
    return;
}

void *rt_malloc(rt_size_t size)
{
    void *ptr;
    unsigned long sp;

    melis_heap_buffer_node_t *new = NULL;

    if (rt_interrupt_get_nest() != 0)
    {
        __err("fatal error.");
        software_break();
    }

    ptr = __internal_malloc(size);
    if (ptr == RT_NULL)
    {
        return RT_NULL;
    }

    if (g_mem_leak_list.open_flag)
    {
        sp = (unsigned long)&sp;
        rt_enter_critical();
        new = __internal_malloc(sizeof(melis_heap_buffer_node_t));
        if (new == RT_NULL)
        {
            rt_exit_critical();
            goto RTN;
        }

        rt_memset(new, 0x00, sizeof(melis_heap_buffer_node_t));

        INIT_LIST_HEAD(&new->i_list);

        new->size = size;
        new->vir = ptr;
        new->entry = (unsigned long)rt_thread_self()->entry;
        new->name = (__s8 *)rt_thread_self()->name;
        new->tick = rt_tick_get();
        new->stk = (void *)sp;

#ifdef CONFIG_DEBUG_BACKTRACE
        backtrace(NULL, new->caller, CONFIG_MEMORY_LEAK_BACKTRACE_LEVEL, 3, NULL);
#endif

        /*rt_memcpy(new->stack, (void*)sp, MEMLEAK_STK_DUMPSZ);*/
        list_add(&new->i_list, &g_mem_leak_list.list);

        rt_exit_critical();
    }
RTN:
    return ptr;
}

void rt_free(void *ptr)
{
    struct list_head       *pos;
    struct list_head       *q;
    melis_heap_buffer_node_t   *tmp;

    if (ptr == RT_NULL)
    {
        return;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        __err("fatal error.");
        software_break();
    }

    __internal_free(ptr);

    if (g_mem_leak_list.open_flag)
    {
        rt_enter_critical();

        list_for_each_safe(pos, q, &g_mem_leak_list.list)
        {
            tmp = list_entry(pos, melis_heap_buffer_node_t, i_list);
            if (tmp->vir == ptr)
            {
                list_del(pos);
                __internal_free(tmp);
                break;
            }
        }

        rt_exit_critical();
    }
    return;
}

long esKRNL_CallBack(__pCBK_t cb, void *arg)
{
#if defined(CONFIG_ARM)
    register rt_base_t cpu_sr;
    long fcseid, fcse;
    long ret;

    if ((uint32_t)cb >= NOFCSE_AREA)
    {
        return cb(arg);
    }

    fcse = (uint32_t)cb >> FCSEID_SHIFT;
    fcse <<= FCSEID_SHIFT;

    cpu_sr = rt_hw_interrupt_disable();
    asm volatile("mrc p15, 0, %0, c13, c0, 0\t\n":"=r"(fcseid));
    asm volatile("mcr p15, 0, %0, c13, c0, 0\t\n" :: "r"(fcse));
    rt_hw_interrupt_enable(cpu_sr);

    ret = ((__pCBK_t)(((long)cb) & ((1 << FCSEID_SHIFT) - 1)))(arg);

    asm volatile("mcr p15, 0, %0, c13, c0, 0\t\n" :: "r"(fcseid));

    return ret;
#elif defined(CONFIG_RISCV)
    /*riscv there is no fcse register*/
    return cb(arg);
#endif
}

uint32_t esKRNL_FlagAccept(void* pgrp, uint32_t flags, uint8_t wait_type, uint8_t *err)
{
    rt_err_t    ret;
    rt_uint32_t recved;
    uint8_t result, options;

    if (err)
    {
        *err = OS_NO_ERR;
    }

    options = 0;
    result  = (uint8_t)(wait_type & OS_FLAG_CONSUME);

    if (result != 0)
    {
        wait_type &= ~OS_FLAG_CONSUME;
        options   |= RT_EVENT_FLAG_CLEAR;
    }

    if (wait_type == OS_FLAG_WAIT_SET_ALL)
    {
        options |= RT_EVENT_FLAG_AND;
    }
    else if (wait_type == OS_FLAG_WAIT_SET_ANY)
    {
        options |= RT_EVENT_FLAG_OR;
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ALL)
    {
        __err("error flag type OS_FLAG_WAIT_CLR_ALL");
        software_break();
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ANY)
    {
        __err("error flag type OS_FLAG_WAIT_CLR_ANY");
        software_break();
    }
    else
    {
        __err("error flag type");
        software_break();
    }

    ret = rt_event_recv(pgrp, flags, options, 0, &recved);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_FLAG_ERR_NOT_RDY;
        }
        return 0;
    }

    return recved;
}

void* esKRNL_FlagCreate(uint32_t init_value, uint8_t *err)
{
    void* event;

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_CREATE_ISR;
        }
        __err("create flag in isr.");
        return NULL;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }

    event = (void*)rt_event_create("melis_event", init_value, RT_IPC_FLAG_FIFO);
    if (event == NULL)
    {
        if (err)
        {
            *err = KRNL_FLAG_GRP_DEPLETED;
        }
    }

    return (void*)event;
}

unsigned long esKRNL_FlagDel(void* pgrp, uint8_t opt, uint8_t *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete flag in isr.");
        return (unsigned long)pgrp;
    }

    rt_event_delete(pgrp);
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return 0;
}

uint8_t esKRNL_FlagNameGet(void* pgrp, uint8_t *pname, uint8_t *err)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esKRNL_FlagNameSet(void* pgrp, uint8_t *pname, uint8_t *err)
{
    __err("not support syscall!");
    software_break();
}

uint32_t esKRNL_FlagPend(void* pgrp, uint32_t flags, uint32_t waittype_timeout, uint8_t *err)
{
    rt_uint32_t recved;
    int32_t tmo;
    uint16_t timeout = waittype_timeout & 0xffff;
    uint8_t wait_type = waittype_timeout >> 16;
    uint8_t result, options;
    rt_err_t ret;

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend flag in isr.");
        return 0;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }

    if (timeout == 0)
    {
        tmo = RT_WAITING_FOREVER;
    }
    else
    {
        tmo = timeout;
    }

    options = 0;
    result = (uint8_t)(wait_type & OS_FLAG_CONSUME);
    if (result != 0)
    {
        wait_type &= ~OS_FLAG_CONSUME;
        options   |= RT_EVENT_FLAG_CLEAR;
    }

    if (wait_type == OS_FLAG_WAIT_SET_ALL)
    {
        options |= RT_EVENT_FLAG_AND;
    }
    else if (wait_type == OS_FLAG_WAIT_SET_ANY)
    {
        options |= RT_EVENT_FLAG_OR;
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ALL)
    {
        __err("not support flag type OS_FLAG_WAIT_CLR_ALL!");
        software_break();
    }
    else if (wait_type == OS_FLAG_WAIT_CLR_ANY)
    {
        __err("not support flag type OS_FLAG_WAIT_CLR_ANY!");
        software_break();
    }
    else
    {
        __err("not support flag type!");
        software_break();
    }

    ret = rt_event_recv(pgrp, flags, options, tmo, &recved);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_FLAG_ERR_NOT_RDY;
        }
        return 0;
    }

    return recved;
}

uint32_t esKRNL_FlagPendGetFlagsRdy(void)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

uint32_t esKRNL_FlagPost(void* pgrp, uint32_t flags, uint8_t opt, uint8_t *err)
{
    uint32_t value;

    if (opt == OS_FLAG_CLR)
    {
        __err("not support flag type!");
        software_break();
        return OS_FLAG_INVALID_OPT;
    }

    rt_event_send(pgrp, flags);
    rt_event_control(pgrp, RT_IPC_CMD_GET_STATE, &value);

    if (err)
    {
        *err = OS_NO_ERR;
    }

    return value;
}

uint32_t esKRNL_FlagQuery(void* pgrp, uint8_t *err)
{
    uint32_t value;

    rt_event_control(pgrp, RT_IPC_CMD_GET_STATE, &value);

    if (err)
    {
        *err = OS_NO_ERR;
    }

    return value;
}

void esKRNL_FreePrio(uint32_t prio)
{
    __err("not support syscall!");
    software_break();
}

__pCBK_t esKRNL_GetCallBack(__pCBK_t cb)
{
#if defined(CONFIG_ARM)
    uint32_t fcseid = 0;

    if (((uint32_t)cb >> FCSEID_SHIFT) != 0)
    {
        return cb;
    }

    asm volatile("mrc p15, 0, %0, c13, c0, 0\t\n":"=r"(fcseid));

    return (__pCBK_t)(fcseid | (uint32_t)cb);
#elif defined(CONFIG_RISCV)
    /*riscv there is no fcse register*/
    return cb;
#endif
}

uint8_t esKRNL_GetPrio(uint8_t priolevel)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

unsigned long esKRNL_GetTIDCur(void)
{
    rt_thread_t cur;
    int32_t idx;

    cur = rt_thread_self();
    if (cur == RT_NULL)
    {
        __err("error context.");
        software_break();
        return OS_TASK_NOT_EXIST;
    }

    /* if module thread by checking the entry, return a decimal number within 1~256 the same 
       as esKRNL_TCreate, but not the thread point address.                             ---*/
    if ((((unsigned long)cur->entry) & 0xf0000000ul) != (CONFIG_DRAM_VIRTBASE & 0xf0000000ul))
    {
        rt_enter_critical();
        idx = find_thread(EXEC_prioself);
        if (idx == -1)
        {
            rt_exit_critical();
            __err("cant find self thread id");
            return OS_TASK_NOT_EXIST;
        }
        rt_exit_critical();
#ifdef USE_MELIS_SHORT_THREAD_ID
        return idx | (melis_thread.thr_id[idx - 1].task_pid << 8);
#else
        return (unsigned long)melis_thread.thr_id[idx - 1].rt_thread_id
#endif
    }
    else  // kernel thread.
    {
        return (unsigned long)cur;
    }

    CODE_UNREACHABLE;
}

unsigned long esKRNL_InterruptDisable(void)
{
    return (unsigned long)rt_hw_interrupt_disable();
}

void esKRNL_InterruptEnable(unsigned long level)
{
    rt_hw_interrupt_enable(level);
}

unsigned long esKRNL_MboxAccept(void* pevent)
{
    unsigned long value;
    rt_err_t  err;

    err = rt_mb_recv(pevent, &value, 0);
    if (err != RT_EOK)
    {
        return 0;
    }

    return value;
}

void* esKRNL_MboxCreate(uint32_t msg)
{
    void* mb;

    if (rt_interrupt_get_nest() != 0)
    {
        __err("create mailbox in isr.");
        return NULL;
    }

    mb = (void*)rt_mb_create("melis_app", 1, 0);
    if (mb != NULL)
    {
        rt_mb_send(mb, msg);
    }

    return mb;
}

unsigned long esKRNL_MboxDel(void* pevent, uint8_t opt, uint8_t *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete mailbox in isr.");
        return (unsigned long)pevent;
    }

    rt_mb_delete(pevent);
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return 0;
}

uint32_t esKRNL_MboxPend(void* pevent, uint16_t timeout, uint8_t *err)
{
    rt_ubase_t  value;
    rt_err_t    ret;
    rt_int32_t  tmo;

    if (err)
    {
        *err = OS_NO_ERR;
    }

    if (pevent == NULL)
    {
        __err("invalid parameter.");
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        return 0;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend mailbox in isr.");
        return 0;
    }

    if (rt_critical_level() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_LOCKED;
        }
        __err("pend msgq in sched lock.");
        return 0;
    }

    if (timeout == 0)
    {
        tmo = -1;
    }
    else
    {
        tmo = timeout;
    }

    ret = rt_mb_recv(pevent, &value, tmo);
    if (ret != RT_EOK)
    {
        *err = OS_TIMEOUT;
        return 0;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }
    return value;
}

uint8_t esKRNL_MboxPost(void* pevent, unsigned long msg)
{
    rt_err_t  ret;

    if (pevent == NULL)
    {
        __err("invalid paramter.");
        return OS_ERR_PEVENT_NULL;
    }

    ret = rt_mb_send(pevent, msg);
    if (ret != RT_EOK)
    {
        __err("post paramter error.");
        return OS_MBOX_FULL;
    }

    return OS_NO_ERR;
}

uint8_t esKRNL_MboxPostOpt(void* pevent, uint32_t msg, uint8_t opt)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

uint8_t esKRNL_MboxQuery(void* pevent, void *p_mbox_data)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esKRNL_MemLeakChk(uint32_t en)
{
    if (en)
    {
        epos_memleak_detect_enable();
    }
    else
    {
        epos_memleak_detect_disable();
    }

    return;
}

void* esKRNL_MutexCreate(uint8_t prio, uint8_t *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_CREATE_ISR;
        }
        __err("create mutex in isr.");
        return NULL;
    }

    //prio = prio;
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return (void*)rt_mutex_create("melis_mutex", 0);
}

unsigned long esKRNL_MutexDel(void* pevent, uint8_t opt, uint8_t *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete mutex in isr.");
        return (unsigned long)pevent;
    }

    //opt = opt;

    rt_mutex_delete(pevent);

    if (err)
    {
        *err = OS_NO_ERR;
    }
    return 0;
}

void esKRNL_MutexPend(void* pevent, uint16_t timeout, uint8_t *err)
{
    rt_err_t ret;

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend mutex in isr.");
        return;
    }

    if (err)
    {
        *err = OS_NO_ERR;
    }
    //timeout = timeout;

    ret = rt_mutex_take(pevent, RT_WAITING_FOREVER);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_TIMEOUT;
        }
    }

    return;
}

uint8_t esKRNL_MutexPost(void* pevent)
{
    rt_err_t err;
    if (rt_interrupt_get_nest() != 0)
    {
        __err("post mutex in isr.");
        return OS_ERR_POST_ISR;
    }

    err = rt_mutex_release(pevent);
    if (err != RT_EOK)
    {
        return OS_ERR_PIP_LOWER;
    }

    return OS_NO_ERR;
}

unsigned long esKRNL_QAccept(void* pevent, uint8_t *err)
{
    unsigned long value;
    rt_err_t ret;

    if (pevent == NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        __err("invalid parameter.");
        return 0;
    }

    ret = rt_mb_recv(pevent, &value, 0);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = KRNL_Q_EMPTY;
        }
        return 0;
    }
    else
    {
        if (err)
        {
            *err = OS_NO_ERR;
        }
    }

    return value;
}

void* esKRNL_QCreate(uint16_t size)
{
    if (rt_interrupt_get_nest() != 0)
    {
        __err("create msgq in isr.");
        return NULL;
    }
    return (void*)rt_mb_create("melis_app", size, 0);
}

void* esKRNL_QDel(void* pevent, uint8_t opt, uint8_t *err)
{
    if (pevent == NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        __err("invalid parameter.");
        return NULL;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        __err("delete msgq in isr.");
        return pevent;
    }
    rt_mb_delete(pevent);
    if (err)
    {
        *err = OS_NO_ERR;
    }

    return NULL;
}

uint8_t esKRNL_QFlush(void* pevent)
{
    if (pevent == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    rt_mb_control(pevent, RT_IPC_CMD_RESET, RT_NULL);

    return OS_NO_ERR;
}

unsigned long esKRNL_QPend(void* pevent, uint16_t timeout, uint8_t *err)
{
    unsigned long value;
    int32_t tmo;
    rt_err_t ret;

    if (err)
    {
        *err = OS_NO_ERR;
    }

    if (pevent == NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        __err("invalid parameter.");
        return 0;
    }
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend msgq in isr.");
        return 0;
    }

    if (rt_critical_level() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_LOCKED;
        }
        __err("pend msgq in sched lock.");
        return 0;
    }

    if (timeout == 0)
    {
        tmo = -1;
    }
    else
    {
        tmo = timeout;
    }

    ret = rt_mb_recv(pevent, &value, tmo);
    if (ret != RT_EOK)
    {
        if (err)
        {
            *err = OS_TIMEOUT;
        }
        return 0;
    }

    return value;
}

uint8_t esKRNL_QPost(void* pevent, unsigned long msg)
{
    rt_err_t ret;

    if (pevent == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    ret = rt_mb_send(pevent, msg);
    if (ret != RT_EOK)
    {
        if (ret == RT_EFULL)
        {
            return KRNL_Q_FULL;
        }
        else
        {
            return OS_TIMEOUT;
        }
    }

    return OS_NO_ERR;
}

uint8_t esKRNL_QPostFront(void* pevent, void *msg)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

uint8_t esKRNL_QPostOpt(void* pevent, void *msg, uint8_t opt)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

uint8_t esKRNL_QQuery(void* pevent, __krnl_q_data_t *p_q_data)
{
    uint32_t entry;

    if (!pevent || !p_q_data)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    rt_mb_control(pevent, RT_IPC_CMD_GET_STATE, &entry);
    p_q_data->OSNMsgs = entry;

    return OS_NO_ERR;
}

void esKRNL_SchedLock(void)
{
    rt_enter_critical();
}

void esKRNL_SchedUnlock(void)
{
    rt_exit_critical();
}

uint16_t esKRNL_SemAccept(void *psem)
{
    rt_err_t ret;
    rt_sem_t  sem = (rt_sem_t)psem;

    if (sem == RT_NULL)
    {
        return 0;
    }

    ret = rt_sem_trytake(sem);
    if (ret == RT_EOK)
    {
        return 1;
    }

    return 0;
}

void* esKRNL_SemCreate(uint16_t count)
{
    if (rt_interrupt_get_nest() != 0)
    {
        __err("create sem in isr.");
        return NULL;
    }

    return (void*)rt_sem_create("melis_sem", count, 0);
}

void* esKRNL_SemDel(void* sem, uint8_t opt, uint8_t *err)
{
    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_DEL_ISR;
        }
        __err("delete sem in isr.");
        return sem;
    }
    if (sem == RT_NULL)
    {
        if (err)
        {
            *err =  OS_ERR_PEVENT_NULL;
        }
        return sem;
    }

    rt_sem_delete(sem);

    if (err)
    {
        *err = OS_NO_ERR;
    }

    return RT_NULL;
}

void esKRNL_SemPend(void* sem, uint16_t timeout, uint8_t *err)
{
    rt_err_t ret;

    if (sem == RT_NULL)
    {
        if (err)
        {
            *err = OS_ERR_PEVENT_NULL;
        }
        /*__err("invalid parameter.");*/
        return;
    }

    if (rt_interrupt_get_nest() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_ISR;
        }
        __err("pend sem in isr.");
        return;
    }

    if (rt_critical_level() != 0)
    {
        if (err)
        {
            *err = OS_ERR_PEND_LOCKED;
        }
        __err("pend sem in sched lock.");
        return;
    }

    if (timeout == 0)
    {
        ret = rt_sem_take(sem, RT_WAITING_FOREVER);
    }
    else
    {
        ret = rt_sem_take(sem, timeout);
    }

    if (err)
    {
        if (ret != RT_EOK)
        {
            *err = OS_TIMEOUT;
        }
        else
        {
            *err = OS_NO_ERR;
        }
    }

    return;
}

uint8_t esKRNL_SemPost(void* sem)
{
    rt_err_t ret;

    if (sem == RT_NULL)
    {
        return OS_ERR_PEVENT_NULL;
    }

    ret = rt_sem_release(sem);
    if (ret != RT_EOK)
    {
        return OS_SEM_OVF;
    }

    return OS_NO_ERR;
}

//TODO, need investigation
uint8_t esKRNL_SemQuery(void* sem, OS_SEM_DATA *p_sem_data)
{
    uint16_t value;

    if (sem == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PEVENT_NULL;
    }

    if (p_sem_data == NULL)
    {
        __err("invalid parameter.");
        return OS_ERR_PDATA_NULL;
    }

    rt_memset(p_sem_data, 0x00, sizeof(OS_SEM_DATA));
    rt_sem_control(sem, RT_IPC_CMD_GET_STATE, &value);

    p_sem_data->OSCnt = value;
    return OS_NO_ERR;
}

void esKRNL_SemSet(void* sem, uint16_t cnt, uint8_t *err)
{
    unsigned long value = cnt;

    if (sem == NULL)
    {
        __err("invalid parameter.");
        return;
    }

    rt_sem_control(sem, RT_IPC_CMD_RESET, (void *)value);

    if (err)
    {
        *err = OS_NO_ERR;
    }
    return;
}

__krnl_sktfrm_t *esKRNL_SktAccept(void* skt, uint8_t user)
{
    unsigned long       value;
    rt_err_t            err = RT_ERROR;
    __krnl_sktfrm_t     *ret;
    __krnl_skt_t        *tmp_skt = (__krnl_skt_t *)skt;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return NULL;
    }

    if (user == KRNL_SKT_USR_IN)
    {
        //if the user is data output type, request the input queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_in, &value, 0);
    }
    else if (user == KRNL_SKT_USR_OUT)
    {
        //if the user is data input type, request the output queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_out, &value, 0);
    }
    else
    {
        __err("fatal error, not support skt method.");
        software_break();
    }

    if (err == RT_EOK)
    {
        tmp_skt->err = OS_NO_ERR;
        ret = (__krnl_sktfrm_t *)value;
    }
    else
    {
        tmp_skt->err = -1;
        ret = NULL;
    }

    return ret;
}

void* esKRNL_SktCreate(uint32_t depth, uint32_t dbuf_attr, uint32_t mbuf_attr)
{
    uint32_t    i, dbufsize, mbufsize, mallocsize;
    void        *dbufarray;
    void        *mbufarray;
    void        *tmpptr;

    __krnl_skt_t    *pSKT;
    __krnl_sktfrm_t *frmbufarray;

    dbufsize = (dbuf_attr & ~KRNL_SKT_BUF_TYP_MASK);
    mbufsize = (mbuf_attr & ~KRNL_SKT_BUF_TYP_MASK);

    //calculate the memory size for create the socket
    mallocsize = sizeof(__krnl_skt_t) + sizeof(__krnl_sktfrm_t) * depth;

    //check data buffer type and calculate data buffer size
    if (dbufsize)
    {
        if ((dbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            mallocsize += dbufsize * depth;
        }
    }

    //check message buffer type and calculate message buffer size
    if (mbufsize)
    {
        if ((mbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            mallocsize += mbufsize * depth;
        }
    }

    pSKT = (__krnl_skt_t *)rt_malloc(mallocsize);
    if (pSKT == RT_NULL)
    {
        __err("Malloc memory failed!");
        goto _err0_createsocket;
    }
    rt_memset(pSKT, 0, mallocsize);

    //process the address of frame
    frmbufarray = (__krnl_sktfrm_t *)((unsigned long)pSKT +  sizeof(__krnl_skt_t));
    tmpptr = (void *)((unsigned long)frmbufarray + sizeof(__krnl_sktfrm_t) * depth);

    //process the address for data buffer
    if (dbufsize)
    {
        if ((dbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            dbufarray = tmpptr;
            tmpptr = (void *)((unsigned long)dbufarray + dbufsize * depth);
        }
        else
        {
            dbufarray = rt_malloc(dbufsize * depth);
            if (dbufarray == (void *)0)
            {
                __err("alloc memory failed!");
                goto _err1_createsocket;
            }
            rt_memset(dbufarray, 0, dbufsize * depth);
        }
    }
    else
    {
        dbufarray = RT_NULL;
    }

    //process the address for message buffer
    if (mbufsize)
    {
        if ((mbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_UNSEQ)
        {
            mbufarray = tmpptr;
        }
        else
        {
            mbufarray = rt_malloc(mbufsize * depth);
            if (mbufarray == (void *)0)
            {
                __err("Balloc memory failed!");
                goto _err2_createsocket;
            }
            rt_memset(mbufarray, 0, mbufsize * depth);
        }
    }
    else
    {
        mbufarray = (void *)0;
    }

    //create input queue and output queue for socket
    pSKT->mb_in = rt_mb_create("socket-i", depth, 0);
    if (pSKT->mb_in == RT_NULL)
    {
        __err("Input queue create failed!");
        goto _err3_CreateSocket;
    }
    pSKT->mb_out = rt_mb_create("socket-o", depth, 0);
    if (pSKT->mb_out == RT_NULL)
    {
        __err("Out queue create failed!");
        goto _err4_CreateSocket;
    }

    pSKT->depth = depth;                            /* the depth of the input and out Q                             */
    pSKT->bufsize = dbufsize;                       /* data buffer size                                             */
    pSKT->msgsize = mbufsize;                       /* message buffer size                                          */
    pSKT->dbufmod = dbuf_attr & KRNL_SKT_BUF_TYP_MASK; /* data buffer type                                          */
    pSKT->mbufmod = mbuf_attr & KRNL_SKT_BUF_TYP_MASK; /* message buffer type                                       */
    pSKT->dbufaddr = dbufarray;                     /* pointer to data buffer start addr                            */
    pSKT->mbufaddr = mbufarray;                     /* pointer to message buffer start addr                         */

    //put the message frame into the input queue
    for (i = 0; i < depth; i ++)
    {
        frmbufarray[i].size = sizeof(__krnl_sktfrm_t);

        if (dbufsize)
        {
            frmbufarray[i].data = (void *)((unsigned long)dbufarray + i * dbufsize);
            frmbufarray[i].datsize = dbufsize;
        }

        if (mbufsize)
        {
            frmbufarray[i].msg = (void *)((unsigned long)mbufarray + i * mbufsize);
            frmbufarray[i].msgsize = mbufsize;
        }

        if (rt_mb_send(pSKT->mb_in, (unsigned long)&frmbufarray[i]) != RT_EOK)
        {
            __err("fatal error, socket inbox not filled.");
            software_break();
        }
    }

    return (void*)pSKT;

_err4_CreateSocket:
    rt_mb_delete(pSKT->mb_in);
_err3_CreateSocket:
    if (mbufsize && ((mbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ))
    {
        rt_free(mbufarray);
    }
_err2_createsocket:
    if (dbufsize && ((dbuf_attr & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ))
    {
        rt_free(dbufarray);
    }
_err1_createsocket:
    rt_free(pSKT);
_err0_createsocket:
    return NULL;
}

int32_t esKRNL_SktDel(void* skt, uint8_t opt)
{
    rt_err_t ret;
    __krnl_skt_t *tmp_skt = (__krnl_skt_t *)skt;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return -1;
    }

    ret = rt_mb_delete(tmp_skt->mb_in);
    if (ret != RT_EOK)
    {
        return -1;
    }

    ret = rt_mb_delete(tmp_skt->mb_out);
    if (ret != RT_EOK)
    {
        return -1;
    }

    //release data buffer
    if ((tmp_skt->dbufmod & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ)
    {
        rt_free(tmp_skt->dbufaddr);
        tmp_skt->dbufaddr = NULL;
    }

    //release message buffer
    if ((tmp_skt->mbufmod & KRNL_SKT_BUF_PHY_SEQMASK) == KRNL_SKT_BUF_PHY_SEQ)
    {
        rt_free(tmp_skt->mbufaddr);
        tmp_skt->mbufaddr = NULL;
    }

    rt_free(tmp_skt);
    return 0;
}

__krnl_sktfrm_t *esKRNL_SktPend(void* skt, uint8_t user, uint32_t timeout)
{
    __krnl_skt_t    *tmp_skt = (__krnl_skt_t *)skt;
    rt_err_t        err = RT_ERROR;
    int32_t         tmo;
    unsigned long   value;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return NULL;
    }

    if (timeout == 0)
    {
        tmo = RT_WAITING_FOREVER;
    }
    else
    {
        tmo = timeout;
    }

    if (user == KRNL_SKT_USR_IN)
    {
        //if the user is data output type, request the input queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_in, &value, tmo);
    }
    else if (user == KRNL_SKT_USR_OUT)
    {
        //if the user is data input type, request the output queue, get message frame
        err = rt_mb_recv(tmp_skt->mb_out, &value, tmo);
    }
    else
    {
        __err("fatal error, not support skt method.");
        software_break();
    }

    if (err == RT_EOK)
    {
        return (__krnl_sktfrm_t *)value;
    }

    return NULL;
}

int32_t esKRNL_SktPost(void* skt, uint8_t user, __krnl_sktfrm_t *frm)
{
    __krnl_skt_t    *tmp_skt = (__krnl_skt_t *)skt;

    if (!tmp_skt || !tmp_skt->mb_out || !tmp_skt->mb_in)
    {
        __err("error of socket.");
        software_break();
        return -1;
    }

    if (user == KRNL_SKT_USR_IN)
    {
        //if the user is data output type, post the data buffer to other modules
        return rt_mb_send(tmp_skt->mb_out, (unsigned long)frm);
    }
    else if (user == KRNL_SKT_USR_OUT)
    {
        //if the user is data input type, post the free buffer to other modules
        return rt_mb_send(tmp_skt->mb_in, (unsigned long)frm);
    }
    else
    {
        __err("fatal error, not support skt method.");
        software_break();
    }

    return -1;
}
/* must be protected by scheduler lock */
static int32_t find_thread(uint32_t prio)
{
    int32_t i, idx = 0;
    uint8_t found;

    found = 0;
    if (prio == EXEC_prioself)
    {
        for (i = 0; i < MELIS_THREAD_COUNT; i ++)
        {
            if (melis_thread.thr_id[i].rt_thread_id == rt_thread_self())
            {
                found = 1;
                break;
            }
        }

        if ((i == MELIS_THREAD_COUNT) || !found)
        {
            __err("fatal error, cant found thread self. i: %d, found: %d.", i, found);
            software_break();
            return -1;
        }

        idx = i + 1;
    }
    else
    {
#ifdef USE_MELIS_SHORT_THREAD_ID
        if (prio > MELIS_THREAD_COUNT)
        {
            __err("fatal error, melis thread id: %d, prio: %d.", idx, prio);
            software_break();
            return -1;
        }
        idx =  prio & 0xff;
#else
        for (i = 0; i < MELIS_THREAD_COUNT; i ++)
        {
            if ((uint32_t)melis_thread.thr_id[i].rt_thread_id == prio)
            {
                found = 1;
                break;
            }
        }

        if ((i == MELIS_THREAD_COUNT) || !found)
        {
            __err("fatal error, cant found thread self. i: %d, found: %d.", i, found);
            return -1;
        }

        idx = i + 1;
#endif
    }

    if ((idx > MELIS_THREAD_COUNT) || (idx <= 0))
    {
        __err("fatal error, melis thread id: %d, prio: %d.", idx, prio);
        software_break();
        return -1;
    }

    return idx;
}

static rt_int8_t __task_del(uint32_t prio_ex)
{
    int32_t     idx = 0;
    rt_thread_t tmp;
    uint32_t    prio = prio_ex;

#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
#endif
    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (!melis_thread.thr_id[idx - 1].rt_thread_id)
    {
        rt_exit_critical();
        __err("fatal error, cant found right thread handle.");
        return OS_TASK_NOT_EXIST;
    }

    __msg("thread %d(entry: 0x%p) being deleted!", idx, melis_thread.thr_id[idx - 1].rt_thread_id->entry);

    tmp = melis_thread.thr_id[idx - 1].rt_thread_id;
    melis_thread.thr_id[idx - 1].rt_thread_id = RT_NULL;
    melis_thread.thr_id[idx - 1].running = 0;
    melis_thread.thr_id[idx - 1].task_pid = 0;

    if (prio == EXEC_prioself)
    {
        void rt_thread_exit(void);
        rt_exit_critical();
        rt_thread_exit();
        CODE_UNREACHABLE;
    }
    else
    {
        rt_thread_delete(tmp);
        rt_exit_critical();
    }

    return 0;
}

static rt_int8_t __thread_del_req(uint32_t prio_ex)
{
    int32_t idx;
    uint32_t prio = prio_ex;

#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
    if ((prio > MELIS_THREAD_COUNT) || (prio <= 0))
    {
        __err("fatal error,  prio: %d.", prio);
        software_break();
        return OS_PRIO_INVALID;
    }
#endif

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id == RT_NULL)
    {
        rt_exit_critical();
        __msg("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (prio == EXEC_prioself)
    {
        rt_exit_critical();
        return melis_thread.thr_id[idx - 1].running;
    }
    else
    {
#ifdef USE_MELIS_SHORT_THREAD_ID
        if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
        {
            rt_exit_critical();
            __wrn("thread %d task_pid change!", prio);
            return OS_TASK_NOT_EXIST;
        }
#endif
        melis_thread.thr_id[idx - 1].running = OS_TASK_DEL_REQ;
        rt_exit_critical();
        return OS_NO_ERR;
    }
}

void esKRNL_TaskChangePrio(void)
{
    __err("not support syscall!");
    software_break();
}

rt_int8_t esKRNL_TaskDel(uint32_t prio)
{
    return __task_del(prio);
}

rt_int8_t esKRNL_TaskDelReq(uint32_t prio_ex)
{
    return __thread_del_req(prio_ex);
}

void esKRNL_TaskPrefEn(uint32_t en)
{
#ifdef CONFIG_RT_USING_TASK_PERF_MONITOR
    void rt_perf_init(void);
    void rt_perf_exit(void);
    if (en)
    {
        rt_perf_init();
    }
    else
    {
        rt_perf_exit();
    }
#endif
    return;
}

uint8_t esKRNL_TaskQuery(uint32_t prio_ex, __krnl_tcb_t *p_task_data)
{
    int32_t idx;
    uint32_t prio = prio_ex;

    if (p_task_data == NULL)
    {
        return OS_ERR_PDATA_NULL;
    }
#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
    if (prio >= 0xff)
    {
        if (prio != OS_PRIO_SELF)
        {
            __err("error, invalid prio");
            software_break();
            return (OS_PRIO_INVALID);
        }
    }

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }
    if (melis_thread.thr_id[idx - 1].rt_thread_id == NULL)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }
    if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
    {
        rt_exit_critical();
        __wrn("thread %d task_pid change!", prio);
        return OS_TASK_NOT_EXIST;
    }

    rt_exit_critical();

    p_task_data->OSTCBPrio = idx;
#else
    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread(entry 0x%p) has been deleted!", ((rt_thread_t)prio)->entry);
        return OS_TASK_NOT_EXIST;
    }
    rt_exit_critical();

    p_task_data->OSTCBPrio = (uint32_t)melis_thread.thr_id[idx - 1].rt_thread_id;
#endif

    return OS_NO_ERR;
}

long esKRNL_TaskNameSet(uint32_t prio_ex, char *name)
{
    int32_t     idx = 0;
    rt_thread_t tmp;
    long        ret = RT_EOK;
    uint32_t prio = prio_ex;

    if(NULL == name)
    {
        __err("parameter err!");
        return OS_TASK_NOT_EXIST;
    }
#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
#endif
    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("thread %d has been deleted!", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (!melis_thread.thr_id[idx - 1].rt_thread_id)
    {
        rt_exit_critical();
        __err("fatal error, cant found right thread handle.");
        return OS_TASK_NOT_EXIST;
    }
#ifdef USE_MELIS_SHORT_THREAD_ID
    if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
    {
        rt_exit_critical();
        __wrn("thread %d task_pid change!", prio);
        return OS_TASK_NOT_EXIST;
    }
#endif
    __msg("thread %d(entry: 0x%p) set name %s!", idx, melis_thread.thr_id[idx - 1].rt_thread_id->entry, name);

    tmp = melis_thread.thr_id[idx - 1].rt_thread_id;
    ret = rt_thread_nameset(tmp, name);
    rt_exit_critical();

    return ret;
}

uint8_t esKRNL_TaskResume(uint32_t prio_ex)
{
    int32_t     idx;
    uint32_t    level;
    uint32_t    prio = prio_ex;

#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
#endif

    level = rt_hw_interrupt_disable();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_hw_interrupt_enable(level);
        __err("fatal error, cant found thread");
        software_break();
        return (OS_TASK_NOT_EXIST);
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id)
    {
#ifdef USE_MELIS_SHORT_THREAD_ID
        if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
        {
            rt_hw_interrupt_enable(level);
            return OS_TASK_NOT_EXIST;
        }
#endif
        if (rt_thread_resume(melis_thread.thr_id[idx - 1].rt_thread_id) != RT_EOK)
        {
            return OS_TASK_NOT_SUSPENDED;
        }

        melis_thread.thr_id[idx - 1].running = 1;

        rt_hw_interrupt_enable(level);
        /* re-schedule */
        rt_schedule();
    }
    else
    {
        rt_hw_interrupt_enable(level);
    }

    return OS_NO_ERR;
}

uint8_t esKRNL_TaskSuspend(uint32_t prio_ex)
{
    int32_t     idx;
    uint32_t    prio = prio_ex;

#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
    if ((prio > MELIS_THREAD_COUNT) || (prio <= 0))
    {
        __err("fatal error,  prio: %d.", prio);
        software_break();
        return OS_PRIO_INVALID;
    }
#else
    if (prio == (uint32_t)rt_thread_idle_gethandler())                    /* Not allowed to suspend idle task    */
    {
        return (OS_TASK_SUSPEND_IDLE);
    }
#endif

    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("fatal error, cant found thread");
        software_break();
        return (OS_TASK_NOT_EXIST);
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id == rt_thread_self())
    {
#ifdef USE_MELIS_SHORT_THREAD_ID
        if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
        {
            rt_exit_critical();
            return OS_TASK_NOT_EXIST;
        }
#endif
        melis_thread.thr_id[idx - 1].running = 0;
        rt_thread_suspend(rt_thread_self());
    }
    else if (melis_thread.thr_id[idx - 1].rt_thread_id)
    {
#ifdef USE_MELIS_SHORT_THREAD_ID
        if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
        {
            rt_exit_critical();
            return OS_TASK_NOT_EXIST;
        }
#endif
        melis_thread.thr_id[idx - 1].running = 0;
        rt_thread_suspend(melis_thread.thr_id[idx - 1].rt_thread_id);
    }
    else
    {
        __err("fatal error, should never happed!");
        software_break();
    }
    rt_exit_critical();

    /* re-schedule */
    rt_schedule();

    return (OS_NO_ERR);
}

uint32_t esKRNL_TCreate(void (*entry)(void *p_arg), void *p_arg, uint32_t stksize, uint16_t id_priolevel)
{
    uint8_t     prio;
    uint8_t     priolevel = id_priolevel & 0xff;
    uint32_t    slice = 20;
    int32_t     i;
    char        name[RT_NAME_MAX] = {0};
    static      uint16_t task_id = 0;

#ifdef CONFIG_ARM
    if ((uint32_t)entry < (1 << FCSEID_SHIFT))
    {
        __err("fatal error, cant not support fcse function!");
        software_break();
        return 0;
    }
#endif

    rt_enter_critical();

    for (i = 0; i < MELIS_THREAD_COUNT; i ++)
    {
        if (melis_thread.thr_id[i].rt_thread_id == RT_NULL)
        {
            melis_thread.thr_id[i].rt_thread_id = 0x1;
            break;
        }
    }

    rt_exit_critical();

    if (i >= MELIS_THREAD_COUNT)
    {
        __err("error when creating new melis thread.");
        software_break();
        return 0;
    }

    if ((priolevel == KRNL_priolevel0) || (priolevel == KRNL_priolevel1) || (priolevel == KRNL_priolevel2))
    {
        //audio feed PCM task should be in high priority.
        //drv_audio 'audio_play_main_task'
        prio = 9;
    }
    else if (priolevel == 0x38)
    {
        prio = 10;
    }
    else if (priolevel == 0x39)
    {
        prio = 11;
    }
    else if (priolevel == 0x3b)
    {
        //let vdrv_maintask thread as quick as possiable.
        //prio = 12;
        prio = 2;
    }
    else if (priolevel == 0x58)
    {
        prio = 13;
    }
    else if (priolevel == 0x59)
    {
        prio = 14;
    }
    else if (priolevel == 0x78)
    {
        prio = 16;
    }
    else if (priolevel == 0x79)
    {
        prio = 17;
    }
    else if (priolevel == 0x7A)
    {
        prio = 18;
    }
    else if (priolevel == 0x7B)
    {
        prio = 19;
    }
    else if (priolevel == 0x7C)
    {
        prio = 20;
    }
    else if (priolevel == 0x7D)
    {
        prio = 21;
    }
    else if (priolevel == 0x7E)
    {
        prio = 22;
    }
    else if (priolevel == 0x7F)
    {
        prio = 23;
    }
    else
    {
        prio = 24;
        slice = 15;
    }

    rt_snprintf(name, sizeof(name), "0x%8X", entry/*MELIS_TASK_PREFIX"%03d", i + 1*/);
    melis_thread.thr_id[i].rt_thread_id = rt_thread_create(name, entry, p_arg, stksize, prio, slice);
    if (melis_thread.thr_id[i].rt_thread_id == NULL)
    {
        __err("failure to create melis thread.!");
        software_break();
        return 0;
    }

    melis_thread.thr_id[i].running = 1;
    rt_enter_critical();
    task_id++;
    if (task_id == 0)
        task_id = 1;
    melis_thread.thr_id[i].task_pid = task_id;
    uint32_t return_id = task_id;
    rt_exit_critical();
    rt_thread_startup(melis_thread.thr_id[i].rt_thread_id);

#ifdef USE_MELIS_SHORT_THREAD_ID
    /*return a number within 1~256 decimal */
    //uint32_t return_id = task_id;
    return (i + 1) | (return_id << 8);
#else
    return (uint32_t)melis_thread.thr_id[i].rt_thread_id;
#endif
}


//just return to rt_thread_exit from user entry.
rt_int8_t esKRNL_TDel(uint32_t prio)
{
    return __task_del(prio);
}

rt_int8_t esKRNL_TDelReq(uint32_t prio_ex)
{
    return __thread_del_req(prio_ex);
}

uint32_t esKRNL_Time(void)
{
    return rt_tick_get();
}

void esKRNL_TimeDly(uint16_t ticks)
{
    rt_thread_delay(ticks);
}

uint8_t esKRNL_TimeDlyResume(uint32_t prio_ex)
{
    register rt_base_t temp;
    int32_t idx;
    uint32_t prio = prio_ex;

#ifdef USE_MELIS_SHORT_THREAD_ID
    prio = prio_ex & 0xff;
    if (prio > MELIS_THREAD_COUNT)
    {
        __err("fatal error, melis thread prio: %d.", prio);
        software_break();
        return -1;
    }
#endif
    rt_enter_critical();
    idx = find_thread(prio);
    if (idx == -1)
    {
        rt_exit_critical();
        __err("cant find specify thread:0x%08x.", prio);
        return OS_TASK_NOT_EXIST;
    }

    if (melis_thread.thr_id[idx - 1].rt_thread_id == RT_NULL)
    {
        rt_exit_critical();
        __err("cant find specify thread:0x%08x.", prio);
        return OS_TASK_NOT_EXIST;
    }
#ifdef USE_MELIS_SHORT_THREAD_ID
    if (melis_thread.thr_id[idx - 1].task_pid != ((prio_ex >> 8) & 0xffff))
    {
        rt_exit_critical();
        __wrn("prio_ex=0x%08x, the thread not exist:0x%08x.task_id=0x%08x", prio_ex, prio, melis_thread.thr_id[idx - 1].task_pid);
        return OS_TASK_NOT_EXIST;
    }
#endif

    rt_exit_critical();

    temp = rt_hw_interrupt_disable();
    if (melis_thread.thr_id[idx - 1].rt_thread_id->stat != RT_THREAD_SUSPEND)
    {
        rt_hw_interrupt_enable(temp);
        return OS_TASK_NOT_EXIST;
    }

    melis_thread.thr_id[idx - 1].rt_thread_id->error = -RT_EINVAL;
    rt_thread_resume(melis_thread.thr_id[idx - 1].rt_thread_id);
    rt_hw_interrupt_enable(temp);

    return OS_NO_ERR;
}
uint32_t esKRNL_TimeGet(void)
{
    return rt_tick_get();
}

void esKRNL_TimeSet(uint32_t ticks)
{
    rt_tick_set(ticks);
}

void* esKRNL_TmrCreate(uint32_t period, uint8_t opt, OS_TMR_CALLBACK callback, void *callback_arg)
{
    uint8_t flag = 0;
    char name[16] = { '\0', };

    if (period == 0)
    {
        return RT_NULL;
    }

    period = (period < 10) ? 1 : (period / 10);
    if ((opt & OS_TMR_OPT_PRIO_MASK) == OS_TMR_OPT_PRIO_LOW)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("create timer in isr.");
            return NULL;
        }
        flag |= RT_TIMER_FLAG_SOFT_TIMER;
    }

    if ((opt & OS_TMR_OPT_NORMAL_MASK) == OS_TMR_OPT_PERIODIC)
    {
        flag |= RT_TIMER_FLAG_PERIODIC;
    }

    rt_memset(name, 0, 16);
    rt_snprintf(name, sizeof(name), "os-tmr-""%04d", period);
    return (void*)rt_timer_create(name, callback, callback_arg, period, flag);
}

int32_t esKRNL_TmrDel(void* ptmr)
{
    rt_err_t ret;
    rt_uint8_t flag = 0;

    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return 0;
    }

    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);
    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("delete timer in isr.");
            return 0;
        }
    }

    ret = rt_timer_delete((rt_timer_t)ptmr);

    return (ret == RT_EOK) ? 1 : 0;
}

int16_t esKRNL_TmrError(void* ptmr)
{
    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return timer_error;
    }

    return 0;
}

uint32_t esKRNL_TmrRemainGet(void* ptmr)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

int32_t esKRNL_TmrStart(void* ptmr)
{
    rt_err_t err;
    uint8_t flag = 0;

    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        software_break();
        return 0;
    }

    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);
    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("start timer in isr.");
            return 0;
        }
    }

    err = rt_timer_start((rt_timer_t)ptmr);

    return (err == RT_EOK) ?  1 : 0;
}

uint8_t esKRNL_TmrStateGet(void* ptmr)
{
    uint8_t flag = 0;

    if (ptmr == NULL)
    {
        __err("invalid parameter.");
        return 0;
    }
    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);

    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("delete timer in isr.");
            return 0;
        }
    }

    if (flag & RT_TIMER_FLAG_ACTIVATED)
    {
        return OS_TMR_STATE_RUNNING;
    }
    else
    {
        return OS_TMR_STATE_STOPPED;
    }
}

int32_t esKRNL_TmrStop(void* ptmr, rt_int8_t opt, void *callback_arg)
{
    uint8_t     flag = 0;
    rt_err_t    ret;

    if (ptmr == NULL)
    {
        __err("paramer invalid.");
        software_break();
        return 0;
    }

    rt_timer_control(ptmr, RT_TIMER_CTRL_GET_STATE, &flag);
    if (flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        if (rt_interrupt_get_nest() != 0)
        {
            __err("stop timer in isr.");
            return 0;
        }
    }

    ret = rt_timer_stop((rt_timer_t)ptmr);
    return (ret == RT_EOK) ?  EPDK_TRUE : EPDK_FAIL;
}

uint16_t esKRNL_Version(void)
{
#define KRNL_VERSION            313u
    return KRNL_VERSION;
}

static inline void dump_stack(rt_int32_t len)
{
    int32_t     i;
    rt_uint32_t sp;
    rt_uint32_t *buf;

    buf = (rt_uint32_t *)&sp;

    rt_kprintf("\n\rdump stack memory,");
#ifdef CONFIG_RTTKERNEL
    rt_kprintf("thread %s, entry 0x%p:", rt_thread_self()->name, rt_thread_self()->entry);
#endif
    for (i = 0; i < len; i ++)
    {
        if ((i % 4) == 0)
        {
            rt_kprintf("\n\r0x%p: ", buf + i);
        }
        rt_kprintf("0x%08x ", buf[i]);
    }
    rt_kprintf("\n\r");

    return;
}

void *esMEMS_Balloc(unsigned long size)
{
    return rt_malloc(size);
}

void esMEMS_Bfree(void *addr, unsigned long size)
{
    rt_free(addr);
}

void *esMEMS_Calloc(void *heap, uint32_t n, uint32_t m)
{
    return rt_calloc(n, m);
}

void esMEMS_CleanDCache(void)
{
    return awos_arch_clean_dcache();
}

void esMEMS_CleanDCacheRegion(void *adr, uint32_t bytes)
{
	awos_arch_mems_clean_dcache_region(adr, bytes);
}

void esMEMS_CleanFlushCache(void)
{
    syscall_trap();
    return ;//awos_arch_clean_flush_cache();
}

void esMEMS_CleanFlushCacheRegion(void *adr, uint32_t bytes)
{
    syscall_trap();
}

void esMEMS_CleanFlushDCache(void)
{
    syscall_trap();
}

void esMEMS_CleanFlushDCacheRegion(void *adr, uint32_t bytes)
{
    if (!adr || !bytes)
    {
        return;
    }

    if (((bytes % 32) != 0) || ((unsigned long)adr) & 0x1f)
    {
    }

#ifndef CONFIG_CPU_DCACHE_DISABLE
    awos_arch_mems_clean_flush_dcache_region((unsigned long)adr, bytes);
#endif
}

void esMEMS_CleanInvalidateCacheAll(void)
{
    syscall_trap();
}

void esMEMS_FlushCache(void)
{
    syscall_trap();
}

void esMEMS_FlushCacheRegion(void *adr, uint32_t bytes)
{
    syscall_trap();
}

void esMEMS_FlushDCache(void)
{
    syscall_trap();
    return ;//awos_arch_flush_dcache();
}

void esMEMS_FlushDCacheRegion(void *adr, uint32_t bytes)
{
    awos_arch_mems_flush_dcache_region((unsigned long)adr, bytes);
}

void esMEMS_FlushICache(void)
{
#if defined(CONFIG_ARM)
    return awos_arch_flush_icache();
#elif defined(CONFIG_RISCV)
    return sbi_remote_fence_i(0);
#endif
}

void esMEMS_FlushICacheRegion(void *adr, uint32_t bytes)
{
    syscall_trap();
}

uint32_t esMEMS_FreeMemSize(void)
{
    rt_uint32_t total, used, max_used;
    rt_uint32_t aval;

    rt_memory_info(&total, &used, &max_used);
    aval = total - used;

    __log("aval memory: 0x%08x.", aval);
    return aval;
}

uint32_t esMEMS_GetIoVAByPA(uint32_t phyaddr, uint32_t size)
{
    return phyaddr;//awos_arch_bus_to_virt(phyaddr);
}

int32_t esMEMS_HeapCreate(void *heapaddr, uint32_t initnpage)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esMEMS_HeapDel(void *heap)
{
    __err("not support syscall!");
    software_break();
    return;
}

int32_t esMEMS_Info(void)
{
    rt_uint32_t total, used, max_used;

    rt_memory_info(&total, &used, &max_used);

    __log("Total heap size 0x%08x, used 0x%08x, max_used 0x%08x, aval margin 0x%08x.", \
          total, used, max_used, total - max_used);

    return 0;
}

int32_t esMEMS_LockCache_Init(void)
{
    int32_t i;

    for (i = 0; i < MAX_ICACHE_LOCKED_WAY; i++)
    {
        LockICacheWays[i].addr    = (void *)(0xffffffff); /* invalid address   */
        LockICacheWays[i].lockbit = 0;                   /* invalid lockway bit*/
    }

    for (i = 0; i < MAX_DCACHE_LOCKED_WAY; i++)
    {
        LockDCacheWays[i].addr    = (void *)(0xffffffff); /* invalid address   */
        LockDCacheWays[i].lockbit = 0;                   /* invalid lockway bit*/
    }

    return 0;
}

int32_t esMEMS_LockDCache(void *addr, uint32_t size)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

int32_t esMEMS_LockICache(void *addr, uint32_t size)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void *esMEMS_Malloc(void *heap, uint32_t size)
{
    return rt_malloc(size);
}

void esMEMS_Mfree(void *heap, void *ptr)
{
    rt_free(ptr);
    return;
}

void *esMEMS_Palloc(uint32_t npage, uint32_t mode)
{
    void    *ptr;

    ptr =  rt_malloc_align(npage * 1024, 4*1024);
    if (ptr == NULL)
    {
        __err("palloc failure.");
        esMEMS_Info();
        // software_break();
        return NULL;
    }

    /* palloc must be 1k aligned at least. */
    if (((unsigned long)ptr) & 0x3ff)
    {
        __err("palloc failure, not aligned to 1k. npage = %d, ptr = 0x%p, caller %p.", npage, ptr, __builtin_return_address(0));
        dump_stack(64);
        //software_break();
        return NULL;
    }

    /* not support more than 1k aligned attribute */
    if (mode != 0)
    {
        __err("palloc failure, mod not support! mode = %d, caller %p.", mode, __builtin_return_address(0));
        // software_break();
        return NULL;
    }

    return ptr;
}

void esMEMS_Pfree(void *mblk, uint32_t npage)
{
    if (mblk == NULL)
    {
        return;
    }

    rt_free_align(mblk);
    return;
}

int32_t esMEMS_PhyAddrConti(void *mem, unsigned long size)
{
    unsigned long   start = (unsigned long )mem;

    if (size > CONFIG_DRAM_VIRTBASE)
    {
        return 0;
    }

    if ((start >= CONFIG_DRAM_VIRTBASE) && ((start + size) <= (CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE)))
    {
        return 1;
    }

    return 0;
}

void *esMEMS_Realloc(void *heap, void *f, uint32_t size)
{
    return rt_realloc(f, size);
}

uint32_t esMEMS_TotalMemSize(void)
{
    return CONFIG_DRAM_SIZE;
}

int32_t esMEMS_UnlockDCache(void *addr)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

int32_t esMEMS_UnlockICache(void *addr)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

unsigned long esMEMS_VA2PA(unsigned long vaddr)
{
    return awos_arch_vaddr_map2_phys(vaddr);
}

void *esMEMS_VMalloc(uint32_t size)
{
    syscall_trap();
    return NULL;
}

int32_t esMEMS_VMCreate(void *pBlk, uint32_t npage, rt_int8_t domain)
{
    __err("not support syscall!");
    software_break();
    return 0;
}

void esMEMS_VMDelete(void *pBlk, uint32_t npage)
{
    __err("not support syscall!");
    software_break();
    return;
}

void esMEMS_VMfree(void *ptr)
{
    syscall_trap();
}

int printk(const char *fmt, ...)
{
    va_list args;
    rt_size_t length;
    static char printk_log_buf[RT_CONSOLEBUF_SIZE];
    char *p;
    int log_level;
    register rt_base_t level;

    va_start(args, fmt);
    level = rt_hw_interrupt_disable();
    rt_enter_critical();

    length = rt_vsnprintf(printk_log_buf, sizeof(printk_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
    {
        length = RT_CONSOLEBUF_SIZE - 1;
    }
#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
    log_level = get_log_level();
    p = (char *)&printk_log_buf;
    if (p[0] != '<' || p[1] < '0' || p[1] > '7' || p[2] != '>')
    {
        if (log_level > (OPTION_LOG_LEVEL_CLOSE))
        {
            rt_kprintf("%s", (char *)&printk_log_buf);
        }
    }
    else
    {
        if (log_level >= (p[1] - '0'))
        {
            rt_kprintf("%s", (char *)&printk_log_buf[3]);
        }
    }
#else
    rt_kprintf("%s", (char *)&printk_log_buf);
#endif
    rt_exit_critical();
    rt_hw_interrupt_enable(level);
    va_end(args);

    return length;
}

void *k_malloc(rt_size_t sz) __attribute__((alias("rt_malloc")));
void k_free(void *ptr) __attribute__((alias("rt_free")));
