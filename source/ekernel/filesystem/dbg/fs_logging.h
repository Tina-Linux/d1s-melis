/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                File system module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsys_logging.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-8
* Descript: log information controller for file system debug.
* Update  : date                auther      ver     notes
*           2011-3-8 14:42:59   Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef __FS_LOGGING_H__
#define __FS_LOGGING_H__

#include <stdarg.h>
#include "fsys_libs.h"
#include "supports.h"
#include <log.h>

/* Logging levels - Determine what gets logged */
#define FS_LOG_LEVEL_DEBUG      ((uint32_t)1 <<  0) /* x = 42 */
#define FS_LOG_LEVEL_TRACE01    ((uint32_t)1 <<  1) /* fs level trace */
#define FS_LOG_LEVEL_QUIET      ((uint32_t)1 <<  2) /* Quietable output */
#define FS_LOG_LEVEL_INFO       ((uint32_t)1 <<  3) /* Volume needs defragmenting */
#define FS_LOG_LEVEL_VERBOSE    ((uint32_t)1 <<  4) /* Forced to continue */
#define FS_LOG_LEVEL_PROGRESS   ((uint32_t)1 <<  5) /* 54% complete */
#define FS_LOG_LEVEL_WARNING    ((uint32_t)1 <<  6) /* You should backup before starting */
#define FS_LOG_LEVEL_ERROR      ((uint32_t)1 <<  7) /* Operation failed, no damage done */
#define FS_LOG_LEVEL_PERROR     ((uint32_t)1 <<  8) /* Message : standard error description */
#define FS_LOG_LEVEL_CRITICAL   ((uint32_t)1 <<  9) /* Operation failed,damage may have occurred */
#define FS_LOG_LEVEL_MDUMP      ((uint32_t)1 <<  10)/* dump data when fread/fwrite/dirread */
#define FS_LOG_LEVEL_TRACE0     ((uint32_t)1 <<  11) /* vfs api level trace */
#define FS_LOG_LEVEL_TRACE1     ((uint32_t)1 <<  12) /* part level trace */
#define FS_LOG_LEVEL_TRACE2     ((uint32_t)1 <<  13) /* dev level trace */
#define FS_LOG_LEVEL_TRACEF     ((uint32_t)1 <<  14) /* trace by function name */
#define FS_LOG_LEVEL_OBJNAME    ((uint32_t)1 <<  15) /* show object name like file name or dev name */

/* Logging style flags - Manage the style of the output */
#define FS_LOG_FLAG_PREFIX      ((uint32_t)1 << 0) /* Prefix messages with "ERROR: ", etc */
#define FS_LOG_FLAG_FILENAME    ((uint32_t)1 << 1) /* Show the file origin of the message */
#define FS_LOG_FLAG_LINE        ((uint32_t)1 << 2) /* Show the line number of the message */
#define FS_LOG_FLAG_FUNCTION    ((uint32_t)1 << 3) /* Show the function name containing the message */
#define FS_LOG_FLAG_ONLYNAME    ((uint32_t)1 << 4) /* Only display the filename, not the pathname */
#define FS_LOG_FLAG_COLOUR      ((uint32_t)1 << 5) /* Colour highlight some messages */

#define FS_LOG_LEVEL_TRACEX     ( FS_LOG_LEVEL_TRACE0 | FS_LOG_LEVEL_TRACE01 | FS_LOG_LEVEL_TRACE1 | FS_LOG_LEVEL_TRACE2)

/* Function prototype for the logging handlers */
typedef int (fs_log_handler)(const char *function, const char *file, int32_t line, uint32_t level, void *data, const char *format, va_list args);

#if    FSYS_DEBUG_ON

__bool  fs_log_parse_option(const char *option);

/* Enable/disable certain log levels */
uint32_t    fs_log_set_levels(uint32_t levels);
uint32_t    fs_log_clear_levels(uint32_t levels);
uint32_t    fs_log_get_levels(void);

/* Enable/disable certain log flags */
uint32_t    fs_log_set_flags(uint32_t flags);
uint32_t    fs_log_clear_flags(uint32_t flags);
uint32_t    fs_log_get_flags(void);


/* logging information redirect */
int32_t     fs_log_redirect(const char *function, const char *file, int line, uint32_t level, void *data, const char *format, ...);


/* Macros to simplify logging.  One for each level defined above.
 * Note, if DEBUG is not defined, then fs_log_debug/trace have no effect.
 */
#define fs_log_critical(FORMAT, ARGS...)    fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_CRITICAL,NULL,FORMAT,##ARGS)
#define fs_log_error(FORMAT, ARGS...)       fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_ERROR,NULL,FORMAT,##ARGS)
#define fs_log_info(FORMAT, ARGS...)        fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_INFO,NULL,FORMAT,##ARGS)
#define fs_log_perror(FORMAT, ARGS...)      fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_PERROR,NULL,FORMAT,##ARGS)
#define fs_log_progress(FORMAT, ARGS...)    fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_PROGRESS,NULL,FORMAT,##ARGS)
#define fs_log_quiet(FORMAT, ARGS...)       fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_QUIET,NULL,FORMAT,##ARGS)
#define fs_log_verbose(FORMAT, ARGS...)     fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_VERBOSE,NULL,FORMAT,##ARGS)
#define fs_log_warning(FORMAT, ARGS...)     fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_WARNING,NULL,FORMAT,##ARGS)
#define fs_log_debug(FORMAT, ARGS...)       fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_DEBUG,NULL,FORMAT,##ARGS)
#define fs_log_trace0(FORMAT, ARGS...)      fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE0,NULL,FORMAT,##ARGS)
#define fs_log_trace01(FORMAT, ARGS...)     fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE01,NULL,FORMAT,##ARGS)
#define fs_log_trace1(FORMAT, ARGS...)      fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE1,NULL,FORMAT,##ARGS)
#define fs_log_trace2(FORMAT, ARGS...)      fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE2,NULL,FORMAT,##ARGS)
#define fs_log_objname(FORMAT, ARGS...)     fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_OBJNAME,NULL,FORMAT,##ARGS)

#else

#define fs_log_parse_option(option)         (0)

/* Enable/disable certain log levels */
#define fs_log_set_levels                   (0)
#define fs_log_clear_levels                 (0)
#define fs_log_get_levels                   (0)

/* Enable/disable certain log flags */
#define fs_log_set_flags                    (0)
#define fs_log_clear_flags                  (0)
#define fs_log_get_flags                    (0)

/* Macros to simplify logging.  One for each level defined above.
 * Note, if DEBUG is not defined, then fs_log_debug/trace have no effect.
 */
#define fs_log_critical(FORMAT, ARGS...)    do { __err("fs critical err");} while(0)
#define fs_log_error(FORMAT, ARGS...)       do { __err("fs fatal err");} while(0)
#define fs_log_info(FORMAT, ARGS...)        do {  } while(0)
#define fs_log_perror(FORMAT, ARGS...)      do { __err("fs perror.");} while(0)
#define fs_log_progress(FORMAT, ARGS...)    do { __log("fs progress");} while(0)
#define fs_log_quiet(FORMAT, ARGS...)       do {  } while(0)
#define fs_log_verbose(FORMAT, ARGS...)     do {  } while(0)
#define fs_log_warning(FORMAT, ARGS...)     do {  } while(0)
#define fs_log_debug(FORMAT, ARGS...)       do {  } while(0)
#define fs_log_trace0(FORMAT, ARGS...)      do {  } while(0)
#define fs_log_trace01(FORMAT, ARGS...)     do {  } while(0)
#define fs_log_trace1(FORMAT, ARGS...)      do {  } while(0)
#define fs_log_trace2(FORMAT, ARGS...)      do {  } while(0)
#define fs_log_objname(FORMAT, ARGS...)     do {  } while(0)

#endif  /* FSYS_DEBUG_ON */

#endif /* __FS_LOGGING_H__ */
