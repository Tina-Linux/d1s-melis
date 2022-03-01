#ifndef _MY_PRINT_H_
#define _MY_PRINT_H_

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MY_PRINTF   rt_kprintf

#define LOG_INFO    MY_PRINTF
#define LOG_WARN    MY_PRINTF
#define LOG_ERROR   MY_PRINTF

#ifdef __cplusplus
}
#endif

#endif // #ifndef _MY_PRINT_H_
