#ifndef __WIFI_LOG_H__
#define __WIFI_LOG_H__
#if 1
#include <stdio.h>
#define WFLOGE(fmt, args...)	\
    printf("[WIFI-ERROR][%s:%d] " fmt "\n", __func__, __LINE__, ##args)

#define WFLOGI(fmt, args...)	\
    printf("[WIFI-INFO][%s:%d] " fmt "\n", __func__, __LINE__, ##args)

#define WFLOGW(fmt, args...)	\
    printf("[WIFI-WARN][%s:%d] " fmt "\n", __func__, __LINE__, ##args)
#if 1
#define WFLOGD(fmt, args...)	\
    printf("[WIFI-DEBUG][%s:%d] " fmt "\n", __func__, __LINE__, ##args)
#else
#define WFLOGD(fmt, args...)
#endif
#else
#include <ulog/ulog.h>
#define TAG "[AGWIFI]"
#define WFLOGE(fmt, args...)	\
    LOGE(TAG,"[%s:%d]" fmt "\n", __func__, __LINE__, ##args)

#define WFLOGI(fmt, args...)	\
    LOGI(TAG,"[%s:%d]" fmt "\n", __func__, __LINE__, ##args)

#define WFLOGD(fmt, args...)	\
    LOGD(TAG,"[%s:%d]" fmt "\n", __func__, __LINE__, ##args)

#define WFLOGW(fmt, args...)	\
    LOGW(TAG,"[%s:%d]" fmt "\n", __func__, __LINE__, ##args)
#endif
#endif
