/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_willow sub-system
*
*                               (c) Copyright 2006-2009, ALLWINNERME, China
*                                           All Rights Reserved
*
* File   : fb_file.h
* Version: V1.0
* By     : xiechuanrong
*********************************************************************************************************
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
