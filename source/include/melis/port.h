/*
 * =====================================================================================
 *
 *       Filename:  port.h
 *
 *    Description:  porting layer type defnition for os vendor.
 *
 *        Version:  2.0
 *         Create:  2017-11-03 19:45:30
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-04-23 15:50:24
 *
 * =====================================================================================
 */

#ifndef __RTT_PORT_H__
#define __RTT_PORT_H__
#include <typedef.h>

typedef int32_t (*pfun_bsp_init_t)(void);
typedef void (*pfun_krnl_init_t)(void *);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_create <create melis thread and make it ready to run.>
 *
 * @param[in] name, task name.
 * @param[in] entry, entry point of the thread.
 * @param[in] parameter,  parameter for the entry function.
 * @param[in] stack_size, stack size of the task.
 * @param[in] priority, the priority of the task.
 * @param[in] tick, time slice for round-robin scheduler.
 *
 * @return  thread handle.
 */
/* ----------------------------------------------------------------------------*/
__hdle awos_task_create(const char *name, void (*entry)(void *parameter), \
                        void *parameter, uint32_t stack_size, uint8_t priority, \
                        uint32_t tick);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_delete <delete a thread>
 *
 * @param[in] thread, thread handle.
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
int32_t awos_task_delete(__hdle thread);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_task_self <>
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
__hdle awos_task_self(void);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  awos_init_bootstrap <++>
 *
 * @param rt_system_bsp_init
 * @param kernel_init
 * @param ticks
 *
 * @return void
 */
/* ----------------------------------------------------------------------------*/
void awos_init_bootstrap(pfun_bsp_init_t rt_system_bsp_init, pfun_krnl_init_t kernel_init, uint64_t ticks);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_malloc_align <++>
 *
 * @param size
 * @param align
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
void *k_malloc_align(uint32_t size, uint32_t align);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_free_align <++>
 *
 * @param ptr
 */
/* ----------------------------------------------------------------------------*/
void k_free_align(void *ptr, uint32_t size);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_malloc <++>
 *
 * @param size
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
void *k_malloc(uint32_t size);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_free <++>
 *
 * @param ptr
 */
/* ----------------------------------------------------------------------------*/
void k_free(void *ptr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  sprintf <++>
 *
 * @param buf
 * @param format
 * @param ...
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
int sprintf(char *buf, const char *format, ...);


/* ----------------------------------------------------------------------------*/
/**
 * @brief  vsprintf <++>
 *
 * @param buf
 * @param format
 * @param arg_ptr
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
int vsprintf(char *buf, const char *format, va_list arg_ptr);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_calloc <++>
 *
 * @param count
 * @param size
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
void *k_calloc(uint32_t count, uint32_t size);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_realloc <++>
 *
 * @param mem_address
 * @param newsize
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
void *k_realloc(void *mem_address, uint32_t newsize);

/* ----------------------------------------------------------------------------*/
/**
 * @brief  k_realloc <++>
 *
 * @param mem_address
 * @param newsize
 *
 * @return
 */
/* ----------------------------------------------------------------------------*/
#endif

