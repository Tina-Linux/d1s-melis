/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _KERNEL_XR_OS_RTTHREAD_XR_OS_THREAD_H_
#define _KERNEL_XR_OS_RTTHREAD_XR_OS_THREAD_H_

#include "kernel/os/os_common.h"
#include "kernel/os/os_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* thread priority */
#define XR_OS_THREAD_PRIO_DRV_BH   XR_OS_PRIORITY_HIGH
#define XR_OS_THREAD_PRIO_DRV_WORK XR_OS_PRIORITY_ABOVE_NORMAL
#define XR_OS_THREAD_PRIO_NET80211 XR_OS_PRIORITY_ABOVE_NORMAL
#define XR_OS_THREAD_PRIO_SYS_CTRL XR_OS_PRIORITY_ABOVE_NORMAL
#define XR_OS_THREAD_PRIO_WPAS     XR_OS_PRIORITY_ABOVE_NORMAL
#define XR_OS_THREAD_PRIO_HOSTAPD  XR_OS_PRIORITY_ABOVE_NORMAL
#define XR_OS_THREAD_PRIO_DRV_RX   XR_OS_PRIORITY_NORMAL
#define XR_OS_THREAD_PRIO_LWIP     XR_OS_PRIORITY_NORMAL
#define XR_OS_THREAD_PRIO_CONSOLE  XR_OS_PRIORITY_ABOVE_NORMAL
#define XR_OS_THREAD_PRIO_APP      XR_OS_PRIORITY_NORMAL

/** @brief Thread entry definition, which is a pointer to a function */
typedef void (*XR_OS_ThreadEntry_t)(void *arg);

/** @brief Thread handle definition */
typedef rt_thread_t XR_OS_ThreadHandle_t;

/**
 * @brief Thread object definition
 */
typedef struct XR_OS_Thread {
    rt_thread_t handle;
} XR_OS_Thread_t;

XR_OS_Status XR_OS_ThreadCreate(XR_OS_Thread_t *thread, const char *name,
                          XR_OS_ThreadEntry_t entry, void *arg,
                          XR_OS_Priority priority, uint32_t stackSize);
XR_OS_Status XR_OS_ThreadDelete(XR_OS_Thread_t *thread);

/**
 * @brief Check whether the thread object is valid or not
 * @param[in] thread Pointer to the thread object
 * @return 1 on valid, 0 on invalid
 */
static inline int XR_OS_ThreadIsValid(XR_OS_Thread_t *thread)
{
	return (thread->handle != XR_OS_INVALID_HANDLE);
}

/**
 * @brief Set the thread object to invalid state
 * @param[in] thread Pointer to the thread object
 * @return None
 */
static inline void XR_OS_ThreadSetInvalid(XR_OS_Thread_t *thread)
{
	thread->handle = XR_OS_INVALID_HANDLE;
}

/**
 * @brief Sleep for the given milliseconds
 *
 * This function causes the calling thread to sleep and block for the given
 * milliseconds.
 *
 * @param[in] msec Milliseconds to sleep
 * @return None
 */
static inline void XR_OS_ThreadSleep(XR_OS_Time_t msec)
{
	rt_thread_delay((rt_tick_t)XR_OS_MSecsToTicks(msec));
}

/**
 * @brief Yield to another thread of equal priority
 *
 * Yielding is where a thread volunteers to leave the running state, without
 * being pre-empted, and before its time slice has been fully utilized.
 *
 * @return None
 */
static inline void XR_OS_ThreadYield(void)
{
	rt_thread_yield();
}

/**
 * @brief Get the handle of the current running thread
 * @return Handle of the current running thread
 */
static inline XR_OS_ThreadHandle_t XR_OS_ThreadGetCurrentHandle(void)
{
	return (XR_OS_ThreadHandle_t)rt_thread_self();
}

/**
 * @brief Suspend the thread scheduler
 *
 * Suspending the scheduler prevents a context switch from occurring but leaves
 * interrupts enabled. If an interrupt requests a context switch while the
 * scheduler is suspended, then the request is held pending and is performed
 * only when the scheduler is resumed (un-suspended).
 *
 * @return None
 */
static inline void XR_OS_ThreadSuspendScheduler(void)
{
	rt_enter_critical();
}

/**
 * @brief Resume the thread scheduler
 *
 * Resume scheduler activity, following a previous call to
 * XR_OS_ThreadSuspendScheduler(), by transitioning the scheduler into the
 * active state from the suspended state.
 *
 * @return None
 */
static inline void XR_OS_ThreadResumeScheduler(void)
{
	rt_exit_critical();
}

/**
 * @brief Check whether the thread scheduler is running or not
 * @return 1 on runing, 0 on not running
 */
static inline int XR_OS_ThreadIsSchedulerRunning(void)
{
	// TODO
	printf("[ERR] %s() NOT SUPPORT!", __func__);
	return 1;
//	return (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_XR_OS_RTTHREAD_XR_OS_THREAD_H_ */
