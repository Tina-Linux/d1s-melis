/*
************************************************************************************************************************
*                                                       RESOURCE
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : themdec.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2008.05.22
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
* Gary.Wang      2008.05.22       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __themdec_h
#define  __themdec_h


typedef void *HRES;


extern HRES   OpenRes(char *szAppFile, __u32 mode);
extern __s32  CloseRes(HRES hRes);

extern __u32  GetResAddr(HRES hRes, __u16 StyleID, __u16 ID);
extern __u32  ReadRes(HRES hRes, __u32 Address, __u32 Length, void *Buffer);

extern __u16  GetResType(HRES hRes, __u16 StyleID, __u16 ID);
extern __u32  GetResSize(HRES hRes, __u16 StyleID, __u16 ID);
extern __s32  GetRes(HRES hRes, __u16 StyleID, __u16 ID, void *Buffer, __u32 Length);


#endif     //  ifndef __themdec_h

/* end of reg_read.h  */
