/*
************************************************************************************************************************
*                                                        BITMAP
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : bmp_op.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.07.20
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
* Gary.Wang      2009.07.20       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __bmp_op_h
#define  __bmp_op_h



//typedef __u32  HBMP;


extern HBMP  bmp_open(const void *bmp_buf);
extern __s32 bmp_close(HBMP hbmp);
extern __u32 bmp_get_bitcount(HBMP hbmp);
extern __u32 bmp_get_width(HBMP hbmp);
extern __u32 bmp_get_height(HBMP hbmp);
extern __u32 bmp_get_size(HBMP hbmp);

extern void  bmp_get_matrix(const void *bmp_buf, void *matrix);



#endif     //  ifndef __bmp_op_h

/* end of bmp_op.h  */
