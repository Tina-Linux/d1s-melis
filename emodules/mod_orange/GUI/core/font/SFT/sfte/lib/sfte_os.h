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
#ifndef  __sfte_os_h
#define  __sfte_os_h

#include <typedef.h>
#include <elibs_stdio.h>

//#define TEST_MALLOC_MEM_LEAK
//#define TEST_FILE_MEM_LEAK

/*--------------------------------------- WINDOWS OS platform ------------------------------------*/
#if 0 //def SFT_OS_WIN32
#include <string.h>

#define t_malloc              malloc
#define t_free                free

#define g_memcpy              memcpy
#define g_memset              memset
#define g_memmove             memmove
#define g_strcmp              strcmp
#define g_strncmp             strncmp
#define g_strcat              strcat
#define g_strncat             strncat
#define g_strcpy              strcpy
#define g_strncpy             strncpy
#define g_strlen              strlen
#define g_toupper             toupper
#define g_strtol              strtol

#define G_FILE                FILE
#define t_fopen               eLIBs_fopen
#define t_fclose              eLIBs_fclose
#define g_fseek               eLIBs_fseek
#define g_remove              remove
#define g_rename              rename
#define g_feof                feof
extern size_t  g_fread(void *ptr, size_t size, size_t nobj, G_FILE *stream) ;
extern size_t  g_fwrite(const void *ptr, size_t size, size_t nobj, G_FILE *stream) ;
extern __s32   g_mkdir(const char *dirname);
extern __s32   g_rmdir(const char *dirname);

#define g_delay               Sleep

#define __PACKED__



/*----------------------------------------- EPOS OS -------------------------------------------*/
#else


//#include <string.h>

#define t_malloc( size )      esMEMS_Malloc( 0, size )
#define t_free( p )           esMEMS_Mfree( 0, p )

#define g_memcpy              eLIBs_memcpy
#define g_memset              eLIBs_memset
#define g_memmove             eLIBs_memmove
#define g_strcmp              eLIBs_strcmp
#define g_strncmp             eLIBs_strncmp
#define g_strcat                    strcat
#define g_strncat                   strncat
#define g_strcpy              eLIBs_strcpy
#define g_strncpy             eLIBs_strncpy
#define g_strlen              eLIBs_strlen
#define g_toupper             eLIBs_toupper
#define g_strtol              eLIBs_strtol


#define G_FILE                ES_FILE
#define DIR                   ES_DIR
#define DIRENT                ES_DIRENT
/* file operaton interfaces */
#define t_fopen               sft_fopen
#define t_fclose              sft_fclose
#define g_fseek               sft_fseek
#define g_fread               sft_fread
#define g_ftell               sft_ftell
//#define g_fwrite              sft_fwrite

#define g_mkdir               eLIBs_mkdir
#define g_rmdir               eLIBs_rmdir
#define g_opendir             eLIBs_opendir
#define g_closedir            eLIBs_closedir
#define g_readdir             eLIBs_readdir
#define g_dirent2name         eLIBs_DirEnt2Name
#define g_dirent2attr         eLIBs_DirEnt2Attr
#define g_fioctrl             eLIBs_fioctrl
#define g_remove              eLIBs_remove
#define g_rename              eLIBs_rename
extern __s32 g_feof(G_FILE *stream);


#define g_delay               esKRNL_TimeDly


#define __PACKED__            __packed


/*----------------------------------------- platform end -----------------------------------------*/
#endif

#define EVENT_OK                                  0
#define EVENT_ERR                                 1


typedef __u32  *g_mutex_t;
extern g_mutex_t g_create_mutex(void);
extern g_mutex_t g_delete_mutex(g_mutex_t mutex, __u8 *err_p);
extern void      g_pend_mutex(g_mutex_t mutex, __u8 *err_p);
extern __u8     g_post_mutex(g_mutex_t mutex);


#if 0//defined( TEST_FILE_MEM_LEAK )
extern __u32   file_count;
extern G_FILE  *g_fopen(const char *f, const char *m);
extern __s32    g_fclose(G_FILE *fp);
#else
#define g_fopen           t_fopen
#define g_fclose          t_fclose
#endif  // #if defined( TEST_FILE_MEM_LEAK )

#if defined( TEST_MALLOC_MEM_LEAK )
extern __u32   malloc_count;
extern void     *malloc_addr;
#define g_malloc( size )  ( malloc_addr = t_malloc( size ), \
                            __msg("MALLOC ! Address is %x. at line %u of file %s. the number of malloc is %u.\n",\
                                  malloc_addr, __LINE__,__FILE__, ++malloc_count), \
                            malloc_addr )
#define g_free( p )       ( __msg("FREE ! Address is %x. at line %u of file %s. the number of malloc is %u.\n", \
                                  p, __LINE__, __FILE__, --malloc_count), \
                            t_free( p ) )
#else
#define g_malloc          t_malloc
#define g_free            t_free
#endif  // #if defined( TEST_MALLOC_MEM_LEAK )



#endif     //  ifndef __sfte_os_h

/* end of sfte_os.h  */
