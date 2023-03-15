/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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