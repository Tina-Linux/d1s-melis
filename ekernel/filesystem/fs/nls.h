/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : nls.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:53:15  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __NLS_H__
#define __NLS_H__
#include <typedef.h>

/* unicode character */
//typedef __u16 __wchar_t;
#define __wchar_t __u16

struct nls_table
{
    char *charset;
    char *alias;
    int (*uni2char)(__wchar_t uni, unsigned char *out, int boundlen);
    int (*char2uni)(const unsigned char *rawstring, int boundlen, __wchar_t *uni);
    unsigned char *charset2lower;
    unsigned char *charset2upper;
    struct module *owner;
    struct nls_table *next;
};

/* this value hold the maximum octet of charset */
#define NLS_MAX_CHARSET_SIZE 6 /* for UTF-8 */

/* nls.c */
extern struct nls_table *load_nls(int);
extern void unload_nls(struct nls_table *);
extern int get_current_charset(void);

extern int utf8_mbtowc(__wchar_t *, const __u8 *, int);
//extern int utf8_mbstowcs(__wchar_t *, const __u8 *, int);
extern int utf8_mbstowcs(__u16 *, const __u8 *, int);
extern int utf8_wctomb(__u8 *, __wchar_t, int);
extern int utf8_wcstombs(__u8 *, const __wchar_t *, int);

static inline unsigned char nls_tolower(struct nls_table *t, unsigned char c)
{
    unsigned char nc = t->charset2lower[c];

    return nc ? nc : c;
}

static inline unsigned char nls_toupper(struct nls_table *t, unsigned char c)
{
    unsigned char nc = t->charset2upper[c];

    return nc ? nc : c;
}

static inline int nls_strnicmp(struct nls_table *t, const unsigned char *s1, const unsigned char *s2, int len)
{
    while (len--)
    {
        if (nls_tolower(t, *s1++) != nls_tolower(t, *s2++))
        {
            return 1;
        }
    }

    return 0;
}

#define MODULE_ALIAS_NLS(name)  MODULE_ALIAS("nls_" __stringify(name))

#endif /* __NLS_H__ */
