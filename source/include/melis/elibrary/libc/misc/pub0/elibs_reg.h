/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : eLibs reg module
* File    : elibs_reg.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-12-9 17:16:02
**********************************************************************************************************************
*/
#ifndef _ELIBS_REG_H_
#define _ELIBS_REG_H_
#include <typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELIBS_KEY_NAME_LEN                   47
#define ELIBS_KEY_VALUE_LEN                 255
#define ELIBS_SET_NAME_LEN                   47
#define ELIBS_LINE_MAX_LEN                    ( ELIBS_KEY_NAME_LEN + ELIBS_KEY_VALUE_LEN + 32 )

/* open mode */
#define ELIBS_REG_READONLY                    0
#define ELIBS_REG_READWRITE                   1
#define ELIBS_REG_CREATE                      2

/* global errer */
#define ELIBS_REG_OK                          0
#define ELIBS_REG_ERR                        -1
#define ELIBS_REG_ERR_FILE_END                1
#define ELIBS_REG_ERR_SET_LIST_END            2
#define ELIBS_REG_ERR_INVALID_SET_NAME        3
#define ELIBS_REG_ERR_KEY_LIST_END            4
#define ELIBS_REG_ERR_INVALID_KEY_NAME        5
#define ELIBS_REG_ERR_ILLEGAL_SET_NAME        6
#define ELIBS_REG_ERR_ILLEGAL_KEY_NAME        7
#define ELIBS_REG_ERR_ILLEGAL_KEY_VALUE       8
#define ELIBS_REG_ERR_ALLOC_MEM_FAIL          9
#define ELIBS_REG_ERR_SET_EXIST              10
#define ELIBS_REG_ERR_ILLEGAL_OP             11
#define ELIBS_REG_ERR_KEY_EXIST              12
#define ELIBS_REG_ERR_INVALID_HANDLE         13

typedef enum tag_ELIBS_REG_LANGID
{
    ELIBS_REG_LANGID_ENGLISH = 0x400,
    ELIBS_REG_LANGID_CHINESES = 0x410,
    ELIBS_REG_LANGID_CHINESET = 0x420,
    ELIBS_REG_LANGID_DEUTSCH = 0x430,
    ELIBS_REG_LANGID_ESPANOL = 0x440,
    ELIBS_REG_LANGID_FRANCAIS = 0x450,
    ELIBS_REG_LANGID_ITALIANO = 0x460,
    ELIBS_REG_LANGID_PORTUGUES = 0x470,
    ELIBS_REG_LANGID_NEDERLANDS = 0x480,
    ELIBS_REG_LANGID_PYCCKNN = 0x490,
    ELIBS_REG_LANGID_JAPANESE = 0x4a0
}
__elibs_reg_langid_t;


extern __hdle eLIBs_RegOpenNode(const char *node, __s32 mode);
extern __s32  eLIBs_RegCloseNode(__hdle handle);
extern __s32  eLIBs_RegDeleteNode(const  char *node);
extern __s32  eLIBs_RegGetSetCount(__hdle handle, __u32 *count_p);
extern __s32  eLIBs_RegGetFirstSet(__hdle handle);
extern __s32  eLIBs_RegGetNextSet(__hdle handle, char *set_name);
extern __s32  eLIBs_RegCreateSet(__hdle handle, const char *set_name);
extern __s32  eLIBs_RegDeleteSet(__hdle handle, const char *set_name);
extern __s32  eLIBs_RegGetKeyCount(__hdle handle, const char *set_name, __u32 *count_p);
extern __s32  eLIBs_RegGetFirstKey(__hdle handle, const char *set_name);
extern __s32  eLIBs_RegGetNextKey(__hdle handle, const char *set_name, char *key_name);
extern __s32  eLIBs_RegCreateKey(__hdle handle, const char *set_name, const char *key_name, const char *key_value);
extern __s32  eLIBs_RegDeleteKey(__hdle handle, const char *set_name, const char *key_name);
extern __s32  eLIBs_RegSetKeyValue(__hdle handle, const char *set_name, const char *key_name, const char *key_value);
extern __s32  eLIBs_RegGetKeyValue(__hdle handle, const char *set_name, const char *key_name, char *key_value);
extern __s32  eLIBs_RegGetError(__hdle handle);
extern __s32  eLIBs_RegClrError(__hdle handle);
extern __s32  eLIBs_RegCreatePath(const char *path);
extern __s32  eLIBs_RegDeletePath(const char *path);
extern __s32  eLIBs_RegIni2Reg(const char *ini_file);
extern __s32  eLIBs_RegReg2Ini(const char *ini_file);
extern void   eLIBs_RegSetRootPath(const char *path);
extern void   eLIBs_RegGetRootPath(char        *path);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ELIBS_REG_H_ */

