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
#include "encode.h"
#include <libc.h>

#define w       32          // word size in bits
#define r       20          // based on security estimates

#define P32     0xB7E15163  // Magic constants for key setup
#define Q32     0x9E3779B9

// derived constants
#define bytes   (w >> 3)                    // bytes per word
#define c       ((b + bytes - 1) >> 2)      // key in words, rounded up
#define R24     ((r<<1) + 4)
#define lgw     5                           // log2(w) -- wussed out

// Rotations
#define ROTL(x,y) (((x)<<((y)&(w-1))) | ((x)>>(w-((y)&(w-1)))))
#define ROTR(x,y) (((x)>>((y)&(w-1))) | ((x)<<(w-((y)&(w-1)))))

typedef struct tag_RC6_HANDLE
{
    unsigned int S[R24];
} RC6_HANDLE_t;

/*
*********************************************************************************************************
* Function   : Initial
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
HENCODE Initial(void *param0, void *param1, __u32 index)
{
    unsigned char *K;
    // key len
    int b;
    int i, j, s, v;
    // Big enough for max b
    unsigned int L[(32 + bytes - 1) >> 2];
    unsigned int A, B;
    RC6_HANDLE_t *handle;
    handle = (RC6_HANDLE_t *)eLIBs_malloc(sizeof(RC6_HANDLE_t));

    if (!handle)
    {
        return NULL;
    }

    // 如果用户传入密码则采用用户的密码
    if (NULL == param0)
    {
        eLIBs_free(handle);
        handle = NULL;
        return NULL;
    }
    else
    {
        K = (__u8 *)param0;
        b = *(int *)param1;
    }

    // 产生密钥的种子
    L[c - 1] = 0;

    for (i = b - 1; i >= 0; i--)
    {
        L[i >> 2] = (L[i >> 2] << 8) + K[i];
    }

    handle->S[0] = P32;

    for (i = 1; i <= 2 * r + 3; i++)
    {
        handle->S[i] = handle->S[i - 1] + Q32;
    }

    A = B = i = j = 0;
    v = R24;

    if (c > v)
    {
        v = c;
    }

    v *= 3;

    for (s = 1; s <= v; s++)
    {
        A = handle->S[i] = ROTL(handle->S[i] + A + B, 3);
        B = L[j] = ROTL(L[j] + A + B, A + B);
        i = (i + 1) % R24;
        j = (j + 1) % c;
    }

    return handle;
}

#if 0
/*
*********************************************************************************************************
* Function   : Encode
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 Encode(HENCODE hEncode, void *ibuf, void *obuf)
{
    RC6_HANDLE_t *handle = (RC6_HANDLE_t *)hEncode;
    unsigned int *ct;
    unsigned int *pt;
    unsigned int A, B, C, D, t, u, x;
    int i;

    if (NULL == hEncode || NULL == ibuf || NULL == obuf)
    {
        return -1;
    }

    pt = (unsigned int *)ibuf;
    ct = (unsigned int *)obuf;
    A = pt[0];
    B = pt[1];
    C = pt[2];
    D = pt[3];
    B += handle->S[0];
    D += handle->S[1];

    for (i = 2; i <= 2 * r; i += 2)
    {
        t = ROTL(B * (2 * B + 1), lgw);
        u = ROTL(D * (2 * D + 1), lgw);
        A = ROTL(A ^ t, u) + handle->S[i];
        C = ROTL(C ^ u, t) + handle->S[i + 1];
        x = A;
        A = B;
        B = C;
        C = D;
        D = x;
    }

    A += handle->S[2 * r + 2];
    C += handle->S[2 * r + 3];
    ct[0] = A;
    ct[1] = B;
    ct[2] = C;
    ct[3] = D;
    return 0;
}
#endif

/*
*********************************************************************************************************
* Function   : Decode
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 Decode(HENCODE hEncode, void *ibuf, void *obuf)
{
    RC6_HANDLE_t *handle = (RC6_HANDLE_t *)hEncode;
    unsigned int *ct;
    unsigned int *pt;
    unsigned int A, B, C, D, t, u, x;
    int i;

    if (NULL == hEncode || NULL == ibuf || NULL == obuf)
    {
        return -1;
    }

    ct = (unsigned int *)ibuf;
    pt = (unsigned int *)obuf;
    A = ct[0];
    B = ct[1];
    C = ct[2];
    D = ct[3];
    C -= handle->S[(r << 1) + 3];
    A -= handle->S[(r << 1) + 2];

    for (i = 2 * r; i >= 2; i -= 2)
    {
        x = D;
        D = C;
        C = B;
        B = A;
        A = x;
        u = ROTL(D * ((D << 1) + 1), lgw);
        t = ROTL(B * ((B << 1) + 1), lgw);
        C = ROTR(C - handle->S[i + 1], t) ^ u;
        A = ROTR(A - handle->S[i],     u) ^ t;
    }

    D -= handle->S[1];
    B -= handle->S[0];
    pt[0] = A;
    pt[1] = B;
    pt[2] = C;
    pt[3] = D;
    return 0;
}

/*
*********************************************************************************************************
* Function   : UnInitial
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
__s32 UnInitial(HENCODE hEncode)
{
    if (NULL == hEncode)
    {
        return -1;
    }

    eLIBs_free(hEncode);
    return 0;
}
