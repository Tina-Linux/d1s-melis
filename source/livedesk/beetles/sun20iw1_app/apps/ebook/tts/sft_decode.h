/*
************************************************************************************************************************
*                                                       SFTE 2.0
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : sft_decode.h
*
* Author      : Bayden.Chen
*
* Version     : 1.1.0
*
* Date        : 2009.11.23
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.11.23       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __sft_decode_h
#define  __sft_decode_h

ES_FILE *sft_fopen(const char *file, const char *mode);

__s32 sft_fclose(ES_FILE *fp);

__size sft_fread(void *pbuf, __size size, __size num, ES_FILE *fp);

__s32 sft_fseek(ES_FILE *fp, __s32 offset, __s32 whence);

__s32 sft_ftell(ES_FILE *fp);

#endif     //  ifndef __sft_decode_h

/* end of sfte_os.h  */
