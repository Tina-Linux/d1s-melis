/*
************************************************************************************************************************
*                                                       RESOURCE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : os_related.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.19
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2008.05.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __os_related_h
#define  __os_related_h

#include "themdec_cfg.h"
#include <elibs_stdio.h>


typedef signed char           int8;
typedef signed short          int16;
typedef signed int            int32;
typedef unsigned char         uint8;
typedef unsigned short        uint16;
typedef unsigned int          uint32;


typedef signed char           __s8;
typedef signed short          __s16;
//typedef signed int            __s32;
typedef unsigned char         __u8;
typedef unsigned short        __u16;
//typedef unsigned int          __u32;

/*---------------------------------------- WIN32_PLATFORM ----------------------------------------*/
#if defined( WIN32_PLATFORM )




#define res_malloc             malloc
#define res_free               free
#define res_rand               rand
#define res_printf             printf

#define RES_FILE             FILE

#define res_fopen            fopen
#define res_fclose           fclose
#define res_fread            fread
#define res_fwrite           fwrite
#define res_fseek            fseek
#define res_remove           remove
#define res_feof             feof
extern uint32 res_mkdir(const char *dirname);

#define OS_NO_ERR                                 1
typedef int32 *res_mutex_t;
#define res_create_mutex( )                         ( (res_mutex_t)1 )
#define res_delete_mutex( res_mutex, err_p )        ( *(err_p) = OS_NO_ERR )
#define res_pend_mutex( res_mutex, err_p )          ( *(err_p) = OS_NO_ERR )
#define res_post_mutex( res_mutex )                 ( OS_NO_ERR )

#define EPDK_OK                         0
#define EPDK_FAIL                      -1

#define WIN32_DATA_PACK                 1
#define __PACKED

/*----------------------------------------- EPOS_PLATFORM ----------------------------------------*/
#elif defined( EPOS_PLATFORM )

#include "mod_orange_i.h"

#define res_malloc( size )     esMEMS_Malloc( 0, size )
#define res_free( p )          esMEMS_Mfree( 0, p )
#define res_rand( )            ( ( *( ( volatile uint32 *)0x1000 ) ) )
#define res_printf             eLIBs_printf
#define res_putchar            eLIBs_putchar

#define RES_FILE           ES_FILE

#define DIR         ES_DIR
#define DIRENT      ES_DIRENT

//#define SEEK_CUR    FSYS_SEEK_CUR
//#define SEEK_END    FSYS_SEEK_END
//#define SEEK_SET    FSYS_SEEK_SET

#define size_t      uint32

/* RES_FILE operaton interfaces */
extern RES_FILE   *res_fopen(const char *filename, const char *mode);
extern int32   res_fclose(RES_FILE *stream);
extern int32   res_remove(const char *filename) ;
extern size_t  res_fread(void *ptr, size_t size, size_t nobj, RES_FILE *stream) ;
extern size_t  res_fwrite(const void *ptr, size_t size, size_t nobj, RES_FILE *stream) ;
extern int32   res_fseek(RES_FILE *stream, long offset, int origin);
extern uint32  res_mkdir(const char *dirname);
extern DIR    *res_opendir(const char *dirname);
extern uint32  res_closedir(DIR *pdir);
extern int32   res_feof(RES_FILE *stream);
extern DIRENT *res_readdir(DIR *pdir);
extern void    res_dirent2name(DIRENT *pdirent, char *pbuffer);
extern void    res_dirent2attr(DIRENT *pdirent, uint8 *pattr);

typedef __krnl_event_t                            *res_mutex_t;
#define res_create_mutex( )                        esKRNL_SemCreate( 1 )
#define res_delete_mutex( res_mutex, err_p )        esKRNL_SemDel( res_mutex, OS_DEL_NO_PEND, err_p )
#define res_pend_mutex( res_mutex, err_p )          esKRNL_SemPend( res_mutex, 0, err_p )
#define res_post_mutex( res_mutex )                 esKRNL_SemPost( res_mutex )

#define __PACKED __packed

/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif




#endif     //  ifndef __os_related_h

/* end of os_related.h  */
