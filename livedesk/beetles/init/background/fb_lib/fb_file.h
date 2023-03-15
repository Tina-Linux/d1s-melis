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
#ifndef _FB_FILE_H_
#define _FB_FILE_H_
#include <kapi.h>
#include <emodules/mod_defs.h>
#include <const.h>

#define BACKLAYER_MOD_STRETCH       0
#define BACKLAYER_MOD_RATIO         1

#if (EPDK_ARCH == EPDK_ARCH_SUNII)
#define SCALE_OUT_MAX_WIDTH         1920
#define SCALE_OUT_MAX_HEIGHT        2048
#else
#define SCALE_OUT_MAX_WIDTH         1024
#define SCALE_OUT_MAX_HEIGHT        1024
#endif

#ifndef fopen
#define fopen(filename, mode) esFSYS_fopen(filename, mode)
#endif
#ifndef fclose
#define fclose(file_hdle) esFSYS_fclose(file_hdle)
#endif
#ifndef fread
#define fread(pdata, blksize, blkcnt, hFile) esFSYS_fread(pdata, blksize, blkcnt, hFile)
#endif
#ifndef fwrite
#define fwrite(pdata, blksize, blkcnt, hFile) esFSYS_fwrite(pdata, blksize, blkcnt, hFile)
#endif
#ifndef fseek
#define fseek(hFile, offset, whence) esFSYS_fseek(hFile, offset, whence)
#endif
#ifndef ftell
#define ftell(hFile) esFSYS_ftell(hFile)
#endif
#ifndef palloc
#define palloc(npage, mode) esMEMS_Palloc(npage, mode)
#endif
#ifndef pfree
#define pfree(pblk, npage) esMEMS_Pfree(pblk, npage)
#endif
#ifdef SIM_PC_WIN
#pragma pack(1)
#endif
typedef struct __fb_file_header
{
    __u32 version;
    __u32 magic_id;
    __u32 height;
    __u32 width;
    __u32 fmt_type;
    __u32 cs_mode;
    __u32 pix_fmt;
    __u32 pix_seq;
    __u32 mod_or_br_swap;
    __u32 data_len[3];
    __u32 offset_data[3];
    __u32 reserve[6];
} __fb_file_header_t;


extern __s32 create_fb_file(FB *framebuffer, __u8 *filename);

extern __s32 get_fb_from_file(FB *framebuffer, __u8 *buffer, SIZE *screen_size, __u8 mod, __u8 *filename);



#endif
