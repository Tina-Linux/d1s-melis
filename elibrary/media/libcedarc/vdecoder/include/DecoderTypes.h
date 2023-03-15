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
#ifndef DECODER_TYPES_H
#define DECODER_TYPES_H

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define CDX_SUCCESS 0
#define CDX_FAILURE (-1)

typedef long long cdx_int64;
typedef unsigned long long cdx_uint64;

typedef int cdx_int32;
typedef unsigned int cdx_uint32;

typedef short cdx_int16;
typedef unsigned short cdx_uint16;

typedef unsigned char cdx_uint8;
typedef char cdx_int8;

typedef unsigned long cdx_ulong;
typedef long cdx_long;

typedef char cdx_char;
typedef int cdx_bool;
typedef void cdx_void;
typedef unsigned long cdx_size;
typedef signed long cdx_ssize;

typedef float cdx_float;

typedef struct DecoderListNodeS DecoderListNodeT;
typedef struct DecoderListS DecoderListT;

#ifdef AWP_DEBUG
#define CDX_INTERFACE
#else
#define CDX_INTERFACE static inline
#endif

typedef cdx_int32 cdx_err;
#if 0
#define CDX_TRUE 1
#define CDX_FALSE 0
#endif //if 0
#define DecoderOffsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define DecoderContainerOf(ptr, type, member) ({ \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - DecoderOffsetof(type,member) ); })

#if 0
enum CdxMediaTypeE
{
    CDX_MEDIA_UNKNOWN = -1,
    CDX_MEDIA_VIDEO = 0,
    CDX_MEDIA_AUDIO,
    CDX_MEDIA_SUBTITLE,
    CDX_MEDIA_DATA,
};
#endif //if 0
#endif
