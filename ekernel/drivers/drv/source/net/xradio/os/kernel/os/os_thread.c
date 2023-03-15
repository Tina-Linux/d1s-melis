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

#include "kernel/os/os_thread.h"
#include "os_util.h"


/**
 * @brief Create and start a thread
 *
 * This function starts a new thread. The new thread starts execution by
 * invoking entry(). The argument arg is passed as the sole argument of entry().
 *
 * @note After finishing execution, the new thread should call XR_OS_ThreadDelete()
 *       to delete itself. Failing to do this and just returning from entry()
 *       will result in undefined behavior.
 *
 * @param[in] thread Pointer to the thread object
 * @param[in] name A descriptive name for the thread. This is mainly used to
 *                 facilitate debugging.
 * @param[in] entry Entry, which is a function pointer, to the thread function
 * @param[in] arg The sole argument passed to entry()
 * @param[in] priority The priority at which the thread will execute
 * @param[in] stackSize The number of bytes the thread stack can hold
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_ThreadCreate(XR_OS_Thread_t *thread, const char *name,
                          XR_OS_ThreadEntry_t entry, void *arg,
                          XR_OS_Priority priority, uint32_t stackSize)
{
	XR_OS_HANDLE_ASSERT(!XR_OS_ThreadIsValid(thread), thread->handle);

	thread->handle = rt_thread_create(name, entry, arg, stackSize,
	                                  priority, 20);

	XR_OS_DBG("%s(), name \"%s\", priority %d, stackSize %u, handle %p\n",
	       __func__, name, priority, stackSize, thread->handle);

	if (thread->handle == NULL) {
		XR_OS_ERR("rt_thread_create() failed!\n");
		XR_OS_ThreadSetInvalid(thread);
		return XR_OS_FAIL;
	} else {
		rt_thread_startup(thread->handle);
	}
	return XR_OS_OK;
}

/**
 * @brief Terminate the thread
 * @note Only memory that is allocated to a thread by the kernel itself is
 *       automatically freed when a thread is deleted. Memory, or any other
 *       resource, that the application (rather than the kernel) allocates
 *       to a thread must be explicitly freed by the application when the task
 *       is deleted.
 * @param[in] thread Pointer to the thread object to be deleted.
 *                   A thread can delete itself by passing NULL in place of a
 *                   valid thread object.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_ThreadDelete(XR_OS_Thread_t *thread)
{
	rt_thread_t handle;
	rt_thread_t curHandle;

	XR_OS_DBG("%s(), handle %p\n", __func__, thread->handle);

	if (thread == NULL) {
//		vTaskDelete(NULL); /* delete self */
//		XR_OS_ERR("thread == NULL, %s delete self NC\n", __FUNCTION__);
		return XR_OS_OK;
	}

	XR_OS_HANDLE_ASSERT(XR_OS_ThreadIsValid(thread), thread->handle);

	handle = thread->handle;
	curHandle = XR_OS_ThreadGetCurrentHandle();
	if (handle == curHandle) {
		/* delete self */
		XR_OS_ThreadSetInvalid(thread);
//		vTaskDelete(NULL);
//		XR_OS_ERR("%s delete self NC\r\n", __FUNCTION__);
	} else {
		/* delete other thread */
		XR_OS_WRN("thread %"XR_OS_HANDLE_F" delete %"XR_OS_HANDLE_F"\n", curHandle, handle);
//		vTaskDelete(handle);
		rt_thread_delete(handle);
		XR_OS_ThreadSetInvalid(thread);
	}

	return XR_OS_OK;
}
