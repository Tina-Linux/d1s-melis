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
#ifndef  __gp_def_h
#define  __gp_def_h



#ifndef SZ_1K
#define SZ_512       0x00000200U
#define SZ_1K        0x00000400U
#define SZ_2K        0x00000800U
#define SZ_4K        0x00001000U
#define SZ_8K        0x00002000U
#define SZ_16K       0x00004000U
#define SZ_32K       0x00008000U
#define SZ_64K       0x00010000U
#define SZ_128K      0x00020000U
#define SZ_256K      0x00040000U
#define SZ_512K      0x00080000U
#define SZ_1M        0x00100000U
#define SZ_2M        0x00200000U
#define SZ_4M        0x00400000U
#define SZ_8M        0x00800000U
#define SZ_16M       0x01000000U
#define SZ_32M       0x02000000U
#define SZ_64M       0x04000000U
#define SZ_128M      0x08000000U
#define SZ_256M      0x10000000U
#define SZ_512M      0x20000000U
#define SZ_1G        0x40000000U
#define SZ_2G        0x80000000U
#define SZ_4G        0x0100000000ULL
#define SZ_8G        0x0200000000ULL
#define SZ_16G       0x0400000000ULL
#define SZ_32G       0x0800000000ULL
#define SZ_64G       0x1000000000ULL
#endif  // ifndef SZ_1K




#undef  NULL
#define NULL                  0


#undef  RSVD_VALUE
#define RSVD_VALUE            0


#undef  COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(expr)       typedef char assert_type[(expr) ? 1 : -1]


#undef  set_bit
#define set_bit( x, y )                 ( (x) |=  ( 1U << (y) ) )

#undef  clear_bit
#define clear_bit( x, y )               ( (x) &= ~( 1U << (y) ) )

#undef  reverse_bit
#define reverse_bit( x, y )             ( (x) ^=  ( 1U << (y) ) )

#undef  test_bit
#define test_bit( x, y )                ( (x)  &  ( 1U << (y) ) )


#undef  min
#define min( x, y )                     ( (x) < (y) ? (x) : (y) )

#undef  max
#define max( x, y )                     ( (x) > (y) ? (x) : (y) )



#undef  spcl_size_align
#define spcl_size_align( x, y )         ( ( (x) + (y) - 1 ) & ~( (y) - 1 ) )


#undef  _4_char_to_int32
#define _4_char_to_int32(a,b,c,d)       ( (a)<<24 | (b)<<16 | (c)<<8 | (d) )


#undef  MEMBER_OFFSET
#define MEMBER_OFFSET( type, mem )      ( (__u32) (&( ( (type *)0 )->mem ) ) )


#define ABS( x )                        ( (x) >0 ? (x) : -(x) )


#define WREG( x )                       (*(volatile unsigned int *)(x))



#define g_malloc( size )      esMEMS_Malloc( 0, size )
#define g_free( p )           esMEMS_Mfree( 0, p )

#define g_memcpy              eLIBs_memcpy
#define g_memset              eLIBs_memset
#define g_memmove             eLIBs_memmove
#define g_strcmp              eLIBs_strcmp
#define g_strncmp             eLIBs_strncmp
#define g_strcat              eLIBs_strcat
#define g_strncat             eLIBs_strncat
#define g_strcpy              eLIBs_strcpy
#define g_strncpy             eLIBs_strncpy
#define g_strlen              eLIBs_strlen
#define g_toupper             eLIBs_toupper
#define g_strtol              eLIBs_strtol


#define G_FILE                ES_FILE
//#define DIR                   ES_DIR
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


#endif     //  ifndef __gp_def_h


/* end of gp_def.h  */
