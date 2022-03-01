/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : string op.
* File    : elibs_string.h
*
* By      : Steven.ZGJ
* Version : v1.0
* Date    : 2008-8-22 11:12:54
**********************************************************************************************************************
*/
#ifndef _ELIBS_STRING_H_
#define _ELIBS_STRING_H_
#include <typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/





/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      STANDARD LIBRARY MACRO'S
*
* Note(s) : (1) NO compiler-supplied standard library functions SHOULD be used.
*
*               #### The reference to standard memory functions SHOULD be removed once all custom library
*               functions are implemented WITHOUT reference to ANY standard library function(s).
*
*               See also 'INCLUDE FILES  Note #3'.
*********************************************************************************************************
*/

/* See Note #1.                                         */
//#define  eLIBs_isalpha(a)                 isalpha(a)
//#define  eLIBs_isdigit(a)                 isdigit(a)
//#define  eLIBs_isspace(a)                 isspace(a)
//#define  eLIBs_isprint(a)                 isprint(a)
//#define  eLIBs_isupper(a)                 isupper(a)
//#define  eLIBs_islower(a)                 islower(a)
//#define  eLIBs_toupper(a)                 toupper(a)
//#define  eLIBs_tolower(a)                 tolower(a)

#define  eLIBs_tolong(a, b, c)              strtol((char *)a, (char **)b, c)


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

__size eLIBs_strlen(const char *pstr);
__size eLIBs_strnlen(const char  *str, size_t maxlen);
char    *eLIBs_strcpy(char *pdest, const char *psrc);
char    *eLIBs_strncpy(char *pdest, const char *psrc, __size len_max);
char    *eLIBs_strcat(char *pdest, const char *pstr_cat);
char    *eLIBs_strncat(char *pdest,  char    *pstr_cat, size_t len_max);
int      eLIBs_strcmp(const char *p1_str, const char *p2_str);
int      eLIBs_stricmp(const char *p1_str, const char *p2_str);
int      eLIBs_strncmp(const char *p1_str, const char *p2_str,   size_t len_max);
int      eLIBs_strnicmp(const char *p1_str, const char *p2_str, size_t len_max);
char    *eLIBs_strchr(char *pstr,   char     srch_char);
char    *eLIBs_strnchr(char *pstr,   size_t len_max,  char     srch_char);
char    *eLIBs_strchrlast(char *pstr,   char     srch_char);
char    *eLIBs_strstr(char *pstr,   char    *srch_str);
void     eLIBs_memclr(void *pmem,   size_t size);
void     eLIBs_memset(void *pmem,   __u8 data_val, size_t size);
void    *eLIBs_memcpy(void *pdest, const void *psrc, size_t size);
__bool   eLIBs_memcmp(const void *p1_mem, const void *p2_mem,   size_t size);
void    *eLIBs_memmove(void *dest, const void *src, size_t n);

/*
**********************************************************************************************************************
*                                             MODULE END
**********************************************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif  /* _ELIBS_STING_H_ */

