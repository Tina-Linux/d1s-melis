/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtbox.h
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      lion txtbox control function implement
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-17         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTBOX_E_H__
#define __TXTBOX_E_H__

#include "txtcfg.h"
#include "gui_i.h"

#define  TXTBOX_DEBUG
#ifndef SIM_PC_WIN
#ifdef  TXTBOX_DEBUG
#define  TXTBOX_PRINTF(...)    do \
    {   \
        __msg(__VA_ARGS__); \
    }while(0);

#define __TXTBOX_HERE__   __msg("here!\n");
#else
#define  TXTBOX_PRINTF(...)
#define __TXTBOX_HERE__
#endif

#else // SIM_PC_WIN

#ifdef  TXTBOX_DEBUG
#define  TXTBOX_PRINTF __msg

#define __TXTBOX_HERE__   __msg("here!\n");
#else
#define  TXTBOX_PRINTF
#define __TXTBOX_HERE__
#endif
#endif// SIM_PC_WIN

#define TXTBOX_OK   0

#define TXTBOX_MEMCPY                   eLIBs_memcpy
#define TXTBOX_MEMSET                   eLIBs_memset
#define TXTBOX_MEMCMP                   eLIBs_strncmp
#define TXTBOX_FREAD                    eLIBs_fread
#define TXTBOX_FWRITE                   eLIBs_fwrite
#define TXTBOX_FSEEK                    eLIBs_fseek
#define TXTBOX_FTELL                    eLIBs_ftell
#define TXTBOX_FOPEN                    eLIBs_fopen
#define TXTBOX_FCLOSE                   eLIBs_fclose
#define TXTBOX_FILE                     ES_FILE
#define TXTBOX_MEMFREE(p)               esMEMS_Mfree(0,p)
#define TXTBOX_MALLOC(size)             esMEMS_Malloc(0,size)
#define TXTBOX_BALLOC(size)             esMEMS_Balloc(size)
#define TXTBOX_BFREE(p,size)            esMEMS_Bfree(p,size)
#define TXTBOX_PMALLOC(size)            esMEMS_Palloc(((size + 1023 )/1024), MEMS_PALLOC_MODE_BND_1K);
#define TXTBOX_PFREE(ptr,size)          esMEMS_Pfree(ptr,((size + 1023 )/1024))
#define TXTBOX_STRCMP                   eLIBs_strcmp
#define TXTBOX_STRLEN                   eLIBs_strlen
#define abs(p)                          ((p) > 0 ? (p):(-p))


#endif
