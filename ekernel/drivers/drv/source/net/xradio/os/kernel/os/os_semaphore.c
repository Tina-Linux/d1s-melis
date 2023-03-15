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

#include "kernel/os/os_semaphore.h"
#include "os_util.h"


/**
 * @brief Create and initialize a counting semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @param[in] initCount The count value assigned to the semaphore when it is
 *                      created.
 * @param[in] maxCount The maximum count value that can be reached. When the
 *                     semaphore reaches this value it can no longer be
 *                     released.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_SemaphoreCreate(XR_OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount)
{
//	XR_OS_HANDLE_ASSERT(!XR_OS_SemaphoreIsValid(sem), sem->handle);

	sem->handle = rt_sem_create("os_sem", initCount, RT_IPC_FLAG_PRIO);
	XR_OS_DBG("%s(), handle %p\n", __func__, sem->handle);

	if (sem->handle == NULL) {
		XR_OS_ERR("err %"XR_OS_HANDLE_F"\n", sem->handle);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}

/**
 * @brief Create and initialize a binary semaphore object
 * @note A binary semaphore is equal to a counting semaphore created by calling
         XR_OS_SemaphoreCreate(sem, 0, 1).
 * @param[in] sem Pointer to the semaphore object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_SemaphoreCreateBinary(XR_OS_Semaphore_t *sem)
{
	XR_OS_ERR("XR_OS_SemaphoreCreateBinary() NOT SUPPORT!\n");

	return XR_OS_SemaphoreCreate(sem, 0, 1);
}

/**
 * @brief Delete the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_SemaphoreDelete(XR_OS_Semaphore_t *sem)
{
	rt_err_t ret;

	XR_OS_HANDLE_ASSERT(XR_OS_SemaphoreIsValid(sem), sem->handle);

	ret = rt_sem_delete(sem->handle);
	if (ret != RT_EOK) {
		XR_OS_ERR("err %"XR_OS_BASETYPE_F"\n", ret);
		return XR_OS_FAIL;
	}
	XR_OS_SemaphoreSetInvalid(sem);
	return XR_OS_OK;
}

/**
 * @brief Wait until the semaphore object becomes available
 * @param[in] sem Pointer to the semaphore object
 * @param[in] waitMS The maximum amount of time (in millisecond) the thread
 *                   should remain in the blocked state to wait for the
 *                   semaphore to become available.
 *                   XR_OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_SemaphoreWait(XR_OS_Semaphore_t *sem, XR_OS_Time_t waitMS)
{
	rt_err_t ret;

	XR_OS_DBG("%s(), handle %p, wait %u ms\n", __func__, sem->handle, waitMS);
	XR_OS_HANDLE_ASSERT(XR_OS_SemaphoreIsValid(sem), sem->handle);

	ret = rt_sem_take(sem->handle, XR_OS_CalcWaitTicks(waitMS));
	if (ret != RT_EOK) {
		XR_OS_DBG("%s() fail @ %d, %"XR_OS_TIME_F" ms\n", __func__, __LINE__, waitMS);
		return XR_OS_E_TIMEOUT;
	}

	return XR_OS_OK;
}

/**
 * @brief Release the semaphore object
 * @param[in] sem Pointer to the semaphore object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
XR_OS_Status XR_OS_SemaphoreRelease(XR_OS_Semaphore_t *sem)
{
	rt_err_t ret;

	XR_OS_HANDLE_ASSERT(XR_OS_SemaphoreIsValid(sem), sem->handle);

	ret = rt_sem_release(sem->handle);
	if (ret != RT_EOK) {
		XR_OS_DBG("%s() fail @ %d\n", __func__, __LINE__);
		return XR_OS_FAIL;
	}

	return XR_OS_OK;
}
