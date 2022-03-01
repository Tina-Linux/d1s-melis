/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            system encrypt sub-system
*
*                                     (c) Copyright 2009-2010, kevin.z China
*                                              All Rights Reserved
*
* File   : sys_encrypt_i.h
* Version: V1.0
* By     : kevin.z
* Date   : 2010-1-28 14:27
*********************************************************************************************************
*/
#ifndef _SYS_ENCRYPT_I_H_
#define _SYS_ENCRYPT_I_H_
#include <typedef.h>

#define MULTI_MEDIA_CFG     (1<<24)
#define VIDEO_CFG           (1<<16)
#define AUDIO_CFG           (2<<16)
#define PICTURE_CFG         (3<<16)
#define TEXT_CFG            (4<<16)


typedef struct __VIDEO_ENCRYPT_ITEM
{
    char        format[16];
    int32_t     support;
    uint32_t    s_width;
    uint32_t    s_height;
    uint32_t    reserved;

} __video_encrypt_item;

typedef struct __MULTI_MEDIA_PARA
{
    __video_encrypt_item    video[32];

} __multi_media_para_t;


typedef struct __SYS_ENCRYPT_PARA
{
    char                    magic[64];  //"magic.epos.it.is.just.a.dream"
    __multi_media_para_t    multimedia; //multi-media parameters
    unsigned long long      reserved[2];//for aligned
} __sys_encrypt_para_t;

#endif  /* _SYS_ENCRYPT_I_H_ */

