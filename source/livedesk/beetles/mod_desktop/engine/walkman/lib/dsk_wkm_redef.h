/*
************************************************************************************************************************
*                                                        dsk_walkman
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : dsk_wkm_redef.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.09.29
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
* Gary.Wang      2009.09.29       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __dsk_wkm_redef_h
#define  __dsk_wkm_redef_h


#ifndef g_malloc
#define g_malloc( size )      esMEMS_Malloc( 0, size )
#define g_free( p )           esMEMS_Mfree( 0, p )
#endif  // #ifndef g_malloc


#ifndef g_memcpy
#define g_memcpy              eLIBs_memcpy
//#define g_memset              eLIBs_memset
#define g_memmove             eLIBs_memmove
//#define g_strcmp              eLIBs_strcmp
//#define g_strncmp             eLIBs_strncmp
#define g_strcat              eLIBs_strcat
#define g_strncat             eLIBs_strncat
//#define g_strcpy              eLIBs_strcpy
//#define g_strncpy             eLIBs_strncpy
#define g_strlen              eLIBs_strlen
#define g_toupper             eLIBs_toupper
#define g_strtol              eLIBs_strtol
#endif  // #ifndef g_memcpy


#ifndef G_FILE
#define G_FILE                ES_FILE
#define DIR                   ES_DIR
#define DIRENT                ES_DIRENT
/* file operaton interfaces */
#define g_fopen               eLIBs_fopen
#define g_fclose              eLIBs_fclose
#define g_fseek               eLIBs_fseek
#define g_remove              eLIBs_remove
#define g_rename              eLIBs_rename
#define g_fread               eLIBs_fread
#define g_fwrite              eLIBs_fwrite
#define g_ftell               eLIBs_ftell
#define g_mkdir               eLIBs_mkdir
#define g_rmdir               eLIBs_rmdir
#define g_opendir             eLIBs_opendir
#define g_closedir            eLIBs_closedir
#define g_readdir             eLIBs_readdir
#define g_dirent2name         eLIBs_DirEnt2Name
#define g_dirent2attr         eLIBs_DirEnt2Attr
#define g_fioctrl             esMODS_MIoctrl
#endif  // #ifndef G_FILE


#ifndef g_QAccept
typedef __krnl_event_t       *g_Queue;
#define g_QAccept             esKRNL_QAccept
#define g_QCreate             esKRNL_QCreate
#define g_QDel                esKRNL_QDel
#define g_QFlush              esKRNL_QFlush
#define g_QPend               esKRNL_QPend
#define g_QPost               esKRNL_QPost
#define g_QPostFront          esKRNL_QPostFront
#define g_QPostOpt            esKRNL_QPostOpt
#define g_QQuery              esKRNL_QQuery
#endif  // #ifndef g_QAccept


#ifndef g_delay
#define g_delay               esKRNL_TimeDly
#endif  // #ifndef g_delay


#ifndef g_create_mutex
typedef __krnl_event_t  *g_mutex_t;
#define g_create_mutex( )        esKRNL_SemCreate( 1 )
#define g_delete_mutex( x, y )   esKRNL_SemDel( x, OS_DEL_NO_PEND, y )
#define g_pend_mutex( x, y )     esKRNL_SemPend( x, 0, y )
#define g_post_mutex( x )        esKRNL_SemPost( x )
#endif  // #ifndef g_create_mutex



#endif     //  ifndef __dsk_wkm_redef_h

/* end of dsk_wkm_redef.h  */
