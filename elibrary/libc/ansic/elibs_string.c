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
#include <typedef.h>
#include <mod_slib.h>

__size eLIBs_strlen(const char  *pstr)
{
    return SLIB_strlen(pstr);
}

__size eLIBs_strnlen(const char  *str, size_t maxlen)
{
    return SLIB_strnlen(str, maxlen);
}

char *eLIBs_strcpy(char *pdest, const char *psrc)
{
    return SLIB_strcpy(pdest, psrc);
}

char *eLIBs_strncpy(char *pdest, const char *psrc, __size len_max)
{
    return SLIB_strncpy(pdest, psrc, len_max);
}

char *eLIBs_strcat(char *pdest, const char *pstr_cat)
{
    return SLIB_strcat(pdest, pstr_cat);
}

char *eLIBs_strncat(char *pdest, char *pstr_cat, size_t len_max)
{
    return SLIB_strncat(pdest, pstr_cat, len_max);
}

int eLIBs_strcmp(const char *p1_str, const char *p2_str)
{
    return SLIB_strcmp(p1_str, p2_str);
}

int eLIBs_stricmp(const char *p1_str, const char *p2_str)
{
    return SLIB_stricmp(p1_str, p2_str);
}

int eLIBs_strncmp(const char *p1_str, const char *p2_str, size_t len_max)
{
    return SLIB_strncmp(p1_str, p2_str, len_max);
}

int eLIBs_strnicmp(const char *p1_str, const char *p2_str, size_t len_max)
{
    return SLIB_strnicmp(p1_str, p2_str, len_max);
}

char *eLIBs_strchr(char *pstr, char srch_char)
{
    return SLIB_strchr(pstr, srch_char);
}

char *eLIBs_strnchr(char *pstr, size_t len_max, char srch_char)
{
    return SLIB_strnchr(pstr, len_max, srch_char);
}

char *eLIBs_strchrlast(char *pstr, char srch_char)
{
    return SLIB_strchrlast(pstr, srch_char);
}

char *eLIBs_strstr(char *pstr, char *psrch_str)
{
    return SLIB_strstr(pstr, psrch_str);
}

void eLIBs_memclr(void *pmem, size_t size)
{
    SLIB_memset((void *)pmem, (__u8)0, (size_t)size);
}

void eLIBs_memset(void *pmem, __u8 data_val, size_t size)
{
    SLIB_memset(pmem, data_val, size);
}

void *eLIBs_memcpy(void *pdest, const void *psrc, size_t size)
{
    return SLIB_memcpy(pdest, psrc, size);
}

__bool eLIBs_memcmp(const void *p1_mem, const void *p2_mem, size_t size)
{
    return SLIB_memcmp(p1_mem, p2_mem, size);
}

void *eLIBs_memmove(void *dest, const void *src, size_t n)
{
    char *tmp = (char *)dest, *s = (char *)src;

    if (s < tmp && tmp < s + n)
    {
        tmp += n;
        s += n;

        while (n--)
        {
            *(--tmp) = *(--s);
        }
    }
    else
    {
        while (n--)
        {
            *tmp++ = *s++;
        }
    }

    return dest;
}
