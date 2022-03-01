#ifndef __COMMON_H__
#define __COMMON_H__
#include "sys_mems.h"
#ifndef F_OK
#define F_OK 0
#endif
//#define FORCE_DEBUG
//#define DEBUG
#if defined(DEBUG) || defined(FORCE_DEBUG)
#define DLOG(fmt, arg...) eLIBs_printf("[%s:%d] "fmt"\n", __FUNCTION__, __LINE__, ##arg)
#else
#define DLOG(fmt, arg...)
#endif
#define ELOG(fmt, arg...) eLIBs_printf("[%s:%u] "fmt"\n", __FUNCTION__, __LINE__, ##arg)
#endif
