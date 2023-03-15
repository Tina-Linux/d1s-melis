/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Distributed under the terms of the GNU GPL, version 2
 *
 * Please see semaphore.c for documentation of these functions
 */
#ifndef __LINUX_MSGQUEUE_H
#define __LINUX_MSGQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MELIS__

#elif (defined(__CONFIG_ARCH_STM32))

#include "xr_rtos/xr_msgqueue.h"

typedef struct xr_msgqueue xr_drv_msgqueue_t;

#define xr_drv_msgqueue_init(queue, n) XR_OS_MsgQueueCreate(queue, n)
#define xr_drv_msgqueue_destroy(queue) XR_OS_MsgQueueDelete(queue)
#define xr_drv_msgqueue_send(queue, msg) XR_OS_MsgQueueSend(queue, msg, 0)
#define xr_drv_msgqueue_receive(queue, msg) XR_OS_MsgQueueReceive(queue, msg, XR_DRV_WAIT_FOREVER)

#elif (defined(__CONFIG_CHIP_XRADIO))

#include "kernel/os/os_queue.h"

typedef OS_Queue_t xr_drv_msgqueue_t;

#define xr_drv_msgqueue_init(queue, n) XR_OS_MsgQueueCreate(queue, n)
#define xr_drv_msgqueue_destroy(queue) XR_OS_MsgQueueDelete(queue)
#define xr_drv_msgqueue_send(queue, msg) XR_OS_MsgQueueSend(queue, msg, 0)
#define xr_drv_msgqueue_receive(queue, msg, timeout) XR_OS_MsgQueueReceive(queue, msg, timeout)

#elif (defined(CONFIG_OS_RTTHREAD) || defined(CONFIG_OS_YUNOS) || (defined(CONFIG_OS_NUTTX)))

#include "kernel/os/os_queue.h"

typedef XR_OS_Queue_t xr_drv_msgqueue_t;

#define xr_drv_msgqueue_init(queue, n) XR_OS_MsgQueueCreate(queue, n)
#define xr_drv_msgqueue_destroy(queue) XR_OS_MsgQueueDelete(queue)
#define xr_drv_msgqueue_send(queue, msg) XR_OS_MsgQueueSend(queue, msg, 0)
#define xr_drv_msgqueue_receive(queue, msg, timeout) XR_OS_MsgQueueReceive(queue, msg, timeout)
/*
#elif (defined(CONFIG_OS_NUTTX))

#include <mqueue.h>

typedef struct nuttx_msg_queue {
	char mqname[16];
	mqd_t queue;
}xr_drv_msgqueue_t;

int nuttx_xr_drv_msgqueue_init(xr_drv_msgqueue_t *queue_hd,uint32_t queueLen);
int nuttx_xr_drv_msgqueue_send(xr_drv_msgqueue_t *queue_hd,void *msg);
int nuttx_xr_drv_msgqueue_receive(xr_drv_msgqueue_t *queue_hd,void *msg,uint32_t timeout);
int nuttx_xr_drv_msgqueue_destroy(xr_drv_msgqueue_t *queue_hd);

#define xr_drv_msgqueue_init      nuttx_xr_drv_msgqueue_init
#define xr_drv_msgqueue_destroy   nuttx_xr_drv_msgqueue_destroy
#define xr_drv_msgqueue_send      nuttx_xr_drv_msgqueue_send
#define xr_drv_msgqueue_receive   nuttx_xr_drv_msgqueue_receive
*/
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LINUX_MSGQUEUE_H */
