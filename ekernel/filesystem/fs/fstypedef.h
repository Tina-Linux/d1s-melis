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
#ifndef     __FS_TYPEDEF_H__
#define     __FS_TYPEDEF_H__


#define     __bitwise


/* fs sub system typedef */
#ifndef DATA_TYPE_X___u64
#define DATA_TYPE_X___u64
typedef     unsigned long long        __u64;
#endif

#ifndef DATA_TYPE_X___u32
#define DATA_TYPE_X___u32
typedef     unsigned int              __u32;
#endif

#ifndef DATA_TYPE_X___u16
#define DATA_TYPE_X___u16
typedef     unsigned short            __u16;
#endif

#ifndef DATA_TYPE_X___u8
#define DATA_TYPE_X___u8
typedef     unsigned char             __u8;
#endif

#ifndef DATA_TYPE_X___s64
#define DATA_TYPE_X___s64
typedef     signed long long          __s64;
#endif

#ifndef DATA_TYPE_X___s32
#define DATA_TYPE_X___s32
typedef     signed int                __s32;
#endif

#ifndef DATA_TYPE_X___s16
#define DATA_TYPE_X___s16
typedef     signed short              __s16;
#endif

#ifndef DATA_TYPE_X___s8
#define DATA_TYPE_X___s8
typedef     signed char               __s8;
#endif


#ifndef DATA_TYPE_X_u64
#define DATA_TYPE_X_u64
typedef     __u64                     u64;
#endif

#ifndef DATA_TYPE_X_u32
#define DATA_TYPE_X_u32
typedef     __u32                     u32;
#endif

#ifndef DATA_TYPE_X_u16
#define DATA_TYPE_X_u16
typedef     __u16                     u16;
#endif

#ifndef DATA_TYPE_X_u8
#define DATA_TYPE_X_u8
typedef     __u8                      u8;
#endif

#ifndef DATA_TYPE_X_s64
#define DATA_TYPE_X_s64
typedef     __s64                     s64;
#endif

#ifndef DATA_TYPE_X_s32
#define DATA_TYPE_X_s32
typedef     __s32                     s32;
#endif

#ifndef DATA_TYPE_X_s16
#define DATA_TYPE_X_s16
typedef     __s16                     s16;
#endif

#ifndef DATA_TYPE_X_s8
#define DATA_TYPE_X_s8
typedef     __s8                      s8;
#endif

#ifndef DATA_TYPE_X___le16
#define DATA_TYPE_X___le16
typedef     short int                 __le16;
#endif

#ifndef DATA_TYPE_X___le32
#define DATA_TYPE_X___le32
typedef     int                       __le32;
#endif

#ifndef DATA_TYPE_X___le64
#define DATA_TYPE_X___le64
typedef     long long                 __le64;
#endif

#ifndef DATA_TYPE_X_le16
#define DATA_TYPE_X_le16
typedef     unsigned short int        le16;
#endif

#ifndef DATA_TYPE_X_le32
#define DATA_TYPE_X_le32
typedef     unsigned int              le32;
#endif

#ifndef DATA_TYPE_X_le64
#define DATA_TYPE_X_le64
typedef     unsigned long long        le64;
#endif


#ifndef DATA_TYPE_X___be16
#define DATA_TYPE_X___be16
typedef     __u16                     __be16;
#endif

#ifndef DATA_TYPE_X___be32
#define DATA_TYPE_X___be32
typedef     __u32                     __be32;
#endif

#ifndef DATA_TYPE_X___be64
#define DATA_TYPE_X___be64
typedef     __u64                     __be64;
#endif

#ifndef DATA_TYPE_X_be16
#define DATA_TYPE_X_be16
typedef     __u16                     be16;
#endif

#ifndef DATA_TYPE_X_be32
#define DATA_TYPE_X_be32
typedef     __u32                     be32;
#endif

#ifndef DATA_TYPE_X_be64
#define DATA_TYPE_X_be64
typedef     __u64                     be64;
#endif



typedef     __u8        u_int8_t;
typedef     __s8        int8_t;
typedef     __u16       u_int16_t;
typedef     __s16       int16_t;
typedef     __u32       u_int32_t;
typedef     __u8        uint8_t;
typedef     __u16       uint16_t;
typedef     __u64       u_int64_t;
#ifdef CONFIG_ARM
typedef     __s64       int64_t;
#endif


//typedef     unsigned long   dev_t;
//typedef     unsigned int    uid_t;
//typedef     unsigned int    gid_t;
typedef     unsigned short  umode_t;
//typedef     unsigned short  mode_t;
#ifdef CONFIG_ARM
typedef     unsigned int    size_t;
#endif
typedef     signed long long  loff_t;
typedef     long            off_t;
//typedef     long            clock_t;

#ifdef      CONFIG_LBD
typedef     u64             sector_t;
#else
typedef     unsigned long   sector_t;
#endif

/* some basic macro define */
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof((x)[0]))
#define DIV_ROUND_UP(n,d)   (((n) + (d) - 1) / (d))

#endif      /* __FS_TYPEDEF_H__ */