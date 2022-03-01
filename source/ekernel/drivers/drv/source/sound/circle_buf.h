/*
*********************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                             audio driver Module
*
*                                    (c) Copyright 2008-2009, kevin China
*                                             All Rights Reserved
*
* File    : circle_buf.h
* By      : kevin
* Version : V1.0
* Date    : 2009-4-2 19:46
*********************************************************************************************************
*/
#ifndef _CIRCLE_BUF_H_
#define _CIRCLE_BUF_H_

typedef struct __AUDIO_DEV_BUF_MANAGER
{
    __u8        *pStart;        //buffer start address
    __u32       uTotalSize;     //buffer total size
    __u32       uBufSize;       //buffer use size, should be part or full of total size

    __u8        *pRdPtr;        //read pointer
    __u8        *pWrPtr;        //write pointer

    __u32       uDataSize;      //data size
    __u32       uFreeSize;      //free buffer size

} __audio_dev_buf_manager_t;


extern __s32 CircleBufCreate(__audio_dev_buf_manager_t *buf_par, __u32 size);
extern __s32 CircleBufDestroy(__audio_dev_buf_manager_t *buf_par);
extern __s32 CircleBufRead(__audio_dev_buf_manager_t *buf_par, __u8 *buf, __u32 size);
extern __s32 CircleBufReadZero(__audio_dev_buf_manager_t *buf_par, __u8 *buf, __u32 size);

extern __s32 CircleBufWrite(__audio_dev_buf_manager_t *buf_par, __u8 *buf, __u32 size);
extern __s32 CircleBufFlush(__audio_dev_buf_manager_t *buf_par);
extern __s32 CircleBufQuerySize(__audio_dev_buf_manager_t *buf_par, __u32 type);
extern __s32 CircleBufResize(__audio_dev_buf_manager_t *buf_par, __s32 size);


#endif /* _CIRCLE_BUF_I_H_ */

