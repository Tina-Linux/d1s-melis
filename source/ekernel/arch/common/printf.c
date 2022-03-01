#include <_ansi.h>
#include <reent.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtthread.h>
#include <kconfig.h>

#define in_interrupt(...)       rt_interrupt_get_nest()

int _printf_r(struct _reent *ptr,
              const char *__restrict fmt, ...)
{
    int ret;
    va_list ap;

    _REENT_SMALL_CHECK_INIT(ptr);
    va_start(ap, fmt);
    ret = _vfprintf_r(ptr, _stdout_r(ptr), fmt, ap);
    va_end(ap);
    return ret;
}

#ifdef _NANO_FORMATTED_IO
int
_iprintf_r(struct _reent *, const char *, ...)
_ATTRIBUTE((__alias__("_printf_r")));
#endif

#ifndef _REENT_ONLY

void enter_io_critical(void)
{
    if (!in_interrupt())
    {
        rt_enter_critical();
    }
}

void exit_io_critical(void)
{
    if (!in_interrupt())
    {
        rt_exit_critical();
    }
}

int printf(const char *__restrict fmt, ...)
{
    int ret = 0;

    enter_io_critical();

    va_list ap;
    struct _reent *ptr = _REENT;

    _REENT_SMALL_CHECK_INIT(ptr);
    va_start(ap, fmt);
    ret = _vfprintf_r(ptr, _stdout_r(ptr), fmt, ap);
    va_end(ap);

    exit_io_critical();

    return ret;
}

#ifdef _NANO_FORMATTED_IO
int
iprintf(const char *, ...)
_ATTRIBUTE((__alias__("printf")));
#endif
#endif /* ! _REENT_ONLY */

void dump_system_information(void)
{
    struct rt_object_information *information;
    struct rt_object *object;
    struct rt_list_node *node;
    rt_thread_t temp;
    rt_tick_t  duration;
    rt_uint32_t total, used, max_used;
    rt_uint8_t *ptr;
    char *stat;
    rt_ubase_t stk_free;

    rt_enter_critical();
    rt_kprintf("\r\n");
    rt_kprintf("    -----------------------------------------------TSK Usage Report----------------------------------------------------------\n");
    rt_kprintf("        name     errno    entry       stat   prio     tcb     slice stacksize      stkfree  lt    si   so       stack_range  \n");

    information = rt_object_get_information(RT_Object_Class_Thread);
    RT_ASSERT(information != RT_NULL);
    for (node = information->object_list.next; node != &(information->object_list); node = node->next)
    {
        rt_uint8_t status;
        rt_uint32_t pc = 0xdeadbeef;

        object = rt_list_entry(node, struct rt_object, list);
        temp = (rt_thread_t)object;


        status = (temp->stat & RT_THREAD_STAT_MASK);

        if (status == RT_THREAD_READY)
        {
            stat = "running";
        }
        else if (status == RT_THREAD_SUSPEND)
        {
            stat = "suspend";
        }
        else if (status == RT_THREAD_INIT)
        {
            stat = "initing";
        }
        else if (status == RT_THREAD_CLOSE)
        {
            stat = "closing";
        }
        else
        {
            stat = "unknown";
        }

        ptr = (rt_uint8_t *)temp->stack_addr;
        while (*ptr == '#')
        {
            ptr ++;
        }

        stk_free = (rt_ubase_t) ptr - (rt_ubase_t) temp->stack_addr;
        rt_kprintf("%15s%5ld   0x%lx  %9s %4d   0x%lx  %3ld %8d    %8ld    %02ld  %04d %04d  [0x%lx-0x%lx]\n", \
                   temp->name,
                   temp->error,
                   (rt_ubase_t)temp->entry,
                   stat,
                   temp->current_priority,
                   (rt_ubase_t)temp,
                   temp->init_tick,
                   temp->stack_size,
                   stk_free,
                   temp->remaining_tick, temp->sched_i, temp->sched_o, (rt_ubase_t)temp->stack_addr, (rt_ubase_t)temp->stack_addr+temp->stack_size);
    }

    rt_kprintf("    -------------------------------------------------------------------------------------------------------------------------\n");
    rt_memory_info(&total, &used, &max_used);
    rt_kprintf("\n\r    memory info:\n\r");
    rt_kprintf("\tTotal  0x%08x\n\r" \
               "\tUsed   0x%08x\n\r" \
               "\tMax    0x%08x\n\r", \
               total, used, max_used);
    rt_kprintf("    ------------------------------------------------memory information-------------------------------------------------------\n");

    rt_exit_critical();

    return;
}

void dump_memory(uint32_t *buf, int32_t len)
{
    int i;

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

void dump_register_memory(char *name, unsigned long addr, int32_t len)
{
    int i;
    int32_t check_virtual_address(unsigned long vaddr);

    if (check_virtual_address(addr) && check_virtual_address(addr + len * sizeof(int)))
    {
        rt_kprintf("\n\rdump %s memory:", name);
        dump_memory((uint32_t *)addr, len);
    }
    else
    {
        rt_kprintf("\n\r%s register corrupted!!\n", name);
    }

    return;
}

int g_cli_direct_read = 0;

void panic_goto_cli(void)
{
    if (g_cli_direct_read > 0)
    {
        rt_kprintf("%s can not be reentrant!\n", __func__);
        return;
    }
    g_cli_direct_read = 1;
#ifdef CONFIG_RT_USING_FINSH
    void finsh_thread_entry(void *parameter);
    finsh_thread_entry(NULL);
#endif
}

