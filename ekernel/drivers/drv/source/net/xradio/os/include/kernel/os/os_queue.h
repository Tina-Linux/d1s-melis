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

#ifndef _KERNEL_XR_OS_RTTHREAD_XR_OS_QUEUE_H_
#define _KERNEL_XR_OS_RTTHREAD_XR_OS_QUEUE_H_

#include "kernel/os/os_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Queue object definition
 */
typedef struct XR_OS_Queue {
    rt_mq_t     handle;
    uint32_t    itemSize;
} XR_OS_Queue_t;

XR_OS_Status XR_OS_QueueCreate(XR_OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize);
XR_OS_Status XR_OS_QueueDelete(XR_OS_Queue_t *queue);
XR_OS_Status XR_OS_QueueSend(XR_OS_Queue_t *queue, const void *item, XR_OS_Time_t waitMS);
XR_OS_Status XR_OS_QueueReceive(XR_OS_Queue_t *queue, void *item, XR_OS_Time_t waitMS);

/**
 * @brief Check whether the queue object is valid or not
 * @param[in] queue Pointer to the queue object
 * @return 1 on valid, 0 on invalid
 */
static inline int XR_OS_QueueIsValid(XR_OS_Queue_t *queue)
{
	return (queue->handle != XR_OS_INVALID_HANDLE);
}

/**
 * @brief Set the queue object to invalid state
 * @param[in] queue Pointer to the queue object
 * @return None
 */
static inline void XR_OS_QueueSetInvalid(XR_OS_Queue_t *queue)
{
	queue->handle = XR_OS_INVALID_HANDLE;
}

/**
 * @brief Create and initialize a message queue object
 * @note A message queue is a queue with each data item can store a pointer.
 *       The size of each data item (message) is equal to sizeof(void *).
 * @param[in] queue Pointer to the message queue object
 * @param[in] queueLen The maximum number of items that the message queue can
 *                     hold at any one time.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
static inline XR_OS_Status XR_OS_MsgQueueCreate(XR_OS_Queue_t *queue, uint32_t queueLen)
{
	return XR_OS_QueueCreate(queue, queueLen, sizeof(void *));
}

/**
 * @brief Delete the message queue object
 * @param[in] queue Pointer to the message queue object
 * @retval XR_OS_Status, XR_OS_OK on success
 */
static inline XR_OS_Status XR_OS_MsgQueueDelete(XR_OS_Queue_t *queue)
{
	return XR_OS_QueueDelete(queue);
}

/**
 * @brief Send (write) a message to the back of the message queue
 * @param[in] queue Pointer to the message queue object
 * @param[in] msg A message, which is a pointer, to be copied into the queue
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for space to become available on the
 *                   message queue, should the message queue already be full.
 *                   XR_OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
static inline XR_OS_Status XR_OS_MsgQueueSend(XR_OS_Queue_t *queue, void *msg, XR_OS_Time_t waitMS)
{
	return XR_OS_QueueSend(queue, &msg, waitMS);
}

/**
 * @brief Receive (read) a message from the message queue
 * @param[in] queue Pointer to the message queue object
 * @param[in] msg Pointer to the message buffer into which the received message
 *                will be copied. A message is a pointer.
 * @param[in] waitMS The maximum amount of time the thread should remain in the
 *                   blocked state to wait for message to become available on
 *                   the message queue, should the message queue already be
 *                   empty.
 *                   XR_OS_WAIT_FOREVER for waiting forever, zero for no waiting.
 * @retval XR_OS_Status, XR_OS_OK on success
 */
static inline XR_OS_Status XR_OS_MsgQueueReceive(XR_OS_Queue_t *queue, void **msg, XR_OS_Time_t waitMS)
{
	return XR_OS_QueueReceive(queue, msg, waitMS);
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_XR_OS_RTTHREAD_XR_OS_QUEUE_H_ */
