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
#include "freetype/config/ftstdlib.h"

long int atol(char *s)
{
    long r = 0;
    int neg = 0;
    switch (*s)
    {
        case '-':
            neg = 1;
        // 这里没有break
        case '+':
            s++;
            break;
    }
    while (*s >= '0' && *s <= '9')
    {
        int n = *s++ - '0';
        if (neg)
        {
            n = -n;
        }
        r = r * 10 + n;
    }
    return r;
}

/*---------------------------------------- EPDK_WIN32 ----------------------------------------*/
#if defined( EPDK_WIN32 )



/*----------------------------------------- EPDK_AC320 ----------------------------------------*/
#elif defined( EPDK_AC320 )



void *ft_srealloc(void *ptr, size_t size)
{
#ifdef SIM_PC_WIN
    return esMEMS_Realloc(NULL, ptr, size);
#else
    char *p;
    char *src;
    size_t i;

    if (ptr == NULL && size == 0)
    {
        return NULL;
    }

    if (ptr == NULL)
    {
        p = (char *)ft_smalloc(size);
        return p;
    }

    if (size == 0)
    {
        ft_sfree(ptr);
        return NULL;
    }

    p = (char *)ft_smalloc(size);

    if (p == NULL)
    {
        return NULL;
    }

    for (src = (char *)ptr, i = 0;  i < size;  i++)
    {
        p[i] = src[i];
    }

    ft_sfree(ptr);
    return p;
#endif
}


/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif
