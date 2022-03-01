#ifndef __MOD_SLIB_API_H__
#define __MOD_SLIB_API_H__
#include <typedef.h>

typedef struct SLIB_API_ENTRY
{
    void (*ptr_SLIB_int2str_dec)(int input, char *str);
    void (*ptr_SLIB_uint2str_dec)(unsigned int input, char *str);
    void (*ptr_SLIB_int2str_hex)(int input, char *str, int hex_flag);
    int (*ptr_SLIB_atoi)(const char *s);
    int (*ptr_SLIB_toupper)(int c);
    int (*ptr_SLIB_isspace)(int ch);
    long(*ptr_SLIB_strtol)(const char *str, const char **err, int base);
    __size(*ptr_SLIB_strlen)(const char  *pstr);
    char    *(*ptr_SLIB_strcpy)(char *pdest, const char *psrc);
    char    *(*ptr_SLIB_strncpy)(char *pdest, const char *psrc, __size len_max);
    char    *(*ptr_SLIB_strcat)(char *pdest, const char *pstr_cat);
    char    *(*ptr_SLIB_strncat)(char *pdest, char *pstr_cat, size_t len_max);
    int (*ptr_SLIB_strcmp)(const char *p1_str, const char *p2_str);
    int (*ptr_SLIB_stricmp)(const char *p1_str, const char *p2_str);
    int (*ptr_SLIB_strncmp)(const char *p1_str, const char *p2_str, size_t len_max);
    int (*ptr_SLIB_strnicmp)(const char *p1_str, const char *p2_str, size_t len_max);
    char    *(*ptr_SLIB_strchr)(char *pstr, char srch_char);
    char    *(*ptr_SLIB_strnchr)(char *pstr, size_t len_max, char srch_char);
    char    *(*ptr_SLIB_strchrlast)(char *pstr, char srch_char);
    char    *(*ptr_SLIB_strstr)(char *pstr, char *psrch_str);
    void (*ptr_SLIB_memclr)(void *pmem, size_t size);
    void (*ptr_SLIB_memset)(void *pmem, __u8 data_val, size_t size);
    void    *(*ptr_SLIB_memcpy)(void *pdest, const void *psrc, size_t size);
    int (*ptr_SLIB_memcmp)(const void *p1_mem, const void *p2_mem, size_t size);
    int (*ptr_SLIB_vsnprintf)(char *buf, size_t size, const char *format, va_list arg);
    int (*ptr_SLIB_strnlen)(const char *str, size_t maxlen);

    double(*ptr_SLIB_atof)(const char *s);
    __s8(*ptr_SLIB_sscanf)(const char *buf, const char *fmt, ...);
} slib_api_entry_t;

//not used by share libary module itsef
//this address is depended on the link address of share libary
#define SLIB_MOD_ENTRY_TBL   (0xe8700000)
#define SLIB_MOD_FUNC_ENTRY  ((slib_api_entry_t *)SLIB_MOD_ENTRY_TBL)

#define SLIB_int2str_dec    (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_int2str_dec )
#define SLIB_uint2str_dec   (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_uint2str_dec)
#define SLIB_int2str_hex    (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_int2str_hex )
#define SLIB_atoi           (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_atoi        )
#define SLIB_toupper        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_toupper     )
#define SLIB_isspace        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_isspace     )
#define SLIB_strtol         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strtol      )
#define SLIB_strlen         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strlen      )
#define SLIB_strcpy         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strcpy      )
#define SLIB_strncpy        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strncpy     )
#define SLIB_strcat         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strcat      )
#define SLIB_strncat        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strncat     )
#define SLIB_strcmp         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strcmp      )
#define SLIB_stricmp        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_stricmp     )
#define SLIB_strncmp        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strncmp     )
#define SLIB_strnicmp       (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strnicmp    )
#define SLIB_strchr         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strchr      )
#define SLIB_strnchr        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strnchr     )
#define SLIB_strchrlast     (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strchrlast  )
#define SLIB_strstr         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strstr      )
#define SLIB_memclr         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memclr      )
#define SLIB_memset         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memset      )
#define SLIB_memcpy         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memcpy      )
#define SLIB_memcmp         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_memcmp      )
#define SLIB_vsnprintf      (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_vsnprintf   )
#define SLIB_strnlen        (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_strnlen     )

#define SLIB_atof           (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_atof        )
#define SLIB_sscanf         (SLIB_MOD_FUNC_ENTRY->ptr_SLIB_sscanf      )

#endif  /* __MOD_SLIB_API_H__ */
