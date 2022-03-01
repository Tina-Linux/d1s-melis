/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : standard lib
* File    : elibs_stdlib.h
*
* By      : Steven.ZGJ
* Version : v1.0
* Date    : 2008-8-22 11:11:25
**********************************************************************************************************************
*/
#ifndef _ELIBS_STDLIB_H_
#define _ELIBS_STDLIB_H_
#include <typedef.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
**********************************************************************
*             format_transformed prototypes
**********************************************************************
*/
#define  HEX_x   'x'
#define  HEX_X   'X'
//This function is to convert an 'int' data 'input' to a string in decimalism,
//and the string converted is in 'str'.
extern void eLIBs_int2str_dec(int input, char *str);
//This function is to convert an 'int' data 'input' to a string in hex,
//and the string converted is in 'str'.
extern void eLIBs_int2str_hex(int input, char *str, int hex_flag);
// This function is to convert an 'unsigned int' data 'input' to a string in decimalism, and
// the  string converted is in 'str'.
extern void eLIBs_uint2str_dec(unsigned int input, char *str);


extern int eLIBs_toupper(int /*c*/);

extern int  eLIBs_isspace(int ch);
extern long eLIBs_strtol(const char *str, const char **err, int base);

extern int  eLIBs_ramdom(int max);

extern void *eLIBs_malloc(size_t size);
extern void eLIBs_free(void *ptr);
extern void *eLIBs_realloc(void *ptr, size_t size);
extern int eLIBs_atoi(const char *s);
extern double eLIBs_atof(const char *s);



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ELIBS_STDLIB_H_ */


