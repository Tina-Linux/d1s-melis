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

#ifndef _XR_OS_DEBUG_H_
#define _XR_OS_DEBUG_H_

#include <stdio.h>
#include "sys/xr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __CONFIG_BOOTLOADER

#define XR_OS_DBG_ON           0
#define XR_OS_WRN_ON           1
#define XR_OS_ERR_ON           1
#define XR_OS_ABORT_ON         0

#define XR_OS_HANDLE_CHECK     1
#define XR_OS_RESOURCE_TRACE   0 /* trace OS resource or not */

#else /* __CONFIG_BOOTLOADER */

#define XR_OS_DBG_ON           0
#define XR_OS_WRN_ON           0
#define XR_OS_ERR_ON           0
#define XR_OS_ABORT_ON         0

#define XR_OS_HANDLE_CHECK     0
#define XR_OS_RESOURCE_TRACE   0 /* trace OS resource or not */

#endif /* __CONFIG_BOOTLOADER */

#define XR_OS_SYSLOG       printf
#define XR_OS_ABORT()      sys_abort()

/* Define (sn)printf formatters for some types */
#define XR_OS_BASETYPE_F   "ld"
#define XR_OS_HANDLE_F     "p"
#define XR_OS_TIME_F       "u"

#define XR_OS_LOG(flags, fmt, arg...)  \
    do {                            \
        if (flags)                  \
            XR_OS_SYSLOG(fmt, ##arg);  \
    } while (0)

#define XR_OS_DBG(fmt, arg...)     XR_OS_LOG(XR_OS_DBG_ON, "[os] "fmt, ##arg)
#define XR_OS_WRN(fmt, arg...)     XR_OS_LOG(XR_OS_WRN_ON, "[os W] "fmt, ##arg)
#define XR_OS_ERR(fmt, arg...)                         \
    do {                                            \
        XR_OS_LOG(XR_OS_ERR_ON, "[os E] %s():%d, "fmt,    \
               __func__, __LINE__, ##arg);          \
        if (XR_OS_ABORT_ON)                            \
            XR_OS_ABORT();                             \
    } while (0)

#define XR_OS_HANDLE_ASSERT(exp, handle)               \
    if (XR_OS_HANDLE_CHECK && !(exp)) {                \
        XR_OS_ERR("handle %"XR_OS_HANDLE_F"\n", handle);  \
        return XR_OS_E_PARAM;                          \
    }

#ifdef __cplusplus
}
#endif

#endif /* _XR_OS_DEBUG_H_ */
