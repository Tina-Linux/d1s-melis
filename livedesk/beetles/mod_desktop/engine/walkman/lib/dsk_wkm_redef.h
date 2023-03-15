/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
