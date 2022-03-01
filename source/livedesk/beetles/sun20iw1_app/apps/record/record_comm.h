#ifndef __RECORD_COMM_H__
#define __RECORD_COMM_H__

#include "beetles_app.h"
#include "apps.h"

extern __s32 record_strcmplast(const char *src1, const char *src2);

extern __u32 record_file_num(const char *dirname, const char *filter);

extern char *record_get_fileName(const char *dirname, __u32 index, const char *filter, char *fileName);

extern void record_time_to_char(char *buff, __awos_time_t time, char sp);

extern void record_date_to_char(char *buff, __awos_date_t date, char sp);

extern void record_ms_to_char(char *buff, __u32 time, char sp);

extern void record_s_to_char(char *buff, __u32 time, char sp);




















#endif//__RECORD_COMM_H__
