/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : endians.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: definitions related to handling of byte ordering, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:14:58  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __ENDIANS_H__
#define __ENDIANS_H__

#include "fstypedef.h"
/*
 * Notes:
 *  We define the conversion functions including typecasts since the
 * defaults don't necessarily perform appropriate typecasts.
 *  Also, using our own functions means that we can change them if it
 * turns out that we do need to use the unaligned access macros on
 * architectures requiring aligned memory accesses...
 */

#       define __BYTE_ORDER 1
#       define __LITTLE_ENDIAN 1
#       define __BIG_ENDIAN 0

#ifndef __BYTE_ORDER
#   if defined(_BYTE_ORDER)
#       define __BYTE_ORDER _BYTE_ORDER
#       define __LITTLE_ENDIAN _LITTLE_ENDIAN
#       define __BIG_ENDIAN _BIG_ENDIAN
#   elif defined(BYTE_ORDER)
#       define __BYTE_ORDER BYTE_ORDER
#       define __LITTLE_ENDIAN LITTLE_ENDIAN
#       define __BIG_ENDIAN BIG_ENDIAN
#   elif defined(__BYTE_ORDER__)
#       define __BYTE_ORDER __BYTE_ORDER__
#       define __LITTLE_ENDIAN __LITTLE_ENDIAN__
#       define __BIG_ENDIAN __BIG_ENDIAN__
#   elif (defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)) || \
            defined(WORDS_LITTLEENDIAN)
#       define __BYTE_ORDER 1
#       define __LITTLE_ENDIAN 1
#       define __BIG_ENDIAN 0
#   elif (!defined(_LITTLE_ENDIAN) && defined(_BIG_ENDIAN)) || \
            defined(WORDS_BIGENDIAN)
#       define __BYTE_ORDER 0
#       define __LITTLE_ENDIAN 1
#       define __BIG_ENDIAN 0
#   else
#       error "__BYTE_ORDER is not defined."
#   endif
#endif

#define __ntfs_bswap_constant_16(x)     \
    (__u16)((((__u16)(x) & 0xff00) >> 8) |    \
            (((__u16)(x) & 0x00ff) << 8))

#define __ntfs_bswap_constant_32(x)         \
    (u32)((((__u32)(x) & 0xff000000u) >> 24) |    \
          (((__u32)(x) & 0x00ff0000u) >>  8) |    \
          (((__u32)(x) & 0x0000ff00u) <<  8) |    \
          (((__u32)(x) & 0x000000ffu) << 24))

#define __ntfs_bswap_constant_64(x)             \
    (__u64)((((__u64)(x) & 0xff00000000000000ull) >> 56) |    \
            (((__u64)(x) & 0x00ff000000000000ull) >> 40) |  \
            (((__u64)(x) & 0x0000ff0000000000ull) >> 24) |  \
            (((__u64)(x) & 0x000000ff00000000ull) >>  8) |  \
            (((__u64)(x) & 0x00000000ff000000ull) <<  8) |  \
            (((__u64)(x) & 0x0000000000ff0000ull) << 24) |  \
            (((__u64)(x) & 0x000000000000ff00ull) << 40) |  \
            (((__u64)(x) & 0x00000000000000ffull) << 56))

#ifdef HAVE_BYTESWAP_H
#   include <byteswap.h>
#else
#   define bswap_16(x) __ntfs_bswap_constant_16(x)
#   define bswap_32(x) __ntfs_bswap_constant_32(x)
#   define bswap_64(x) __ntfs_bswap_constant_64(x)
#endif

//#if defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)

#define __s16_to_cpu(x) (( __u16)(x))
#define __s32_to_cpu(x) (( __u32)(x))
#define __le64_to_cpu(x) (( __u64)(x))

#define __cpu_to_le16(x) (( le16)(x))
#define __cpu_to_le32(x) (( le32)(x))
#define __cpu_to_le64(x) (( le64)(x))

#define __constant_le16_to_cpu(x) (( __u16)(x))
#define __constant_le32_to_cpu(x) (( __u32)(x))
#define __constant_le64_to_cpu(x) (( __u64)(x))

#define __constant_cpu_to_le16(x) (( le16)(x))
#define __constant_cpu_to_le32(x) (( le32)(x))
#define __constant_cpu_to_le64(x) (( le64)(x))

#define ___constant_swab16(x) \
    ((__u16)( \
              (((__u16)(x) & (__u16)0x00ffU) << 8) | \
              (((__u16)(x) & (__u16)0xff00U) >> 8) ))

#define ___constant_swab32(x) \
    ((__u32)( \
              (((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
              (((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
              (((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
              (((__u32)(x) & (__u32)0xff000000UL) >> 24) ))
#define ___constant_swab64(x) \
    ((__u64)( \
              (__u64)(((__u64)(x) & (__u64)0x00000000000000ffULL) << 56) | \
              (__u64)(((__u64)(x) & (__u64)0x000000000000ff00ULL) << 40) | \
              (__u64)(((__u64)(x) & (__u64)0x0000000000ff0000ULL) << 24) | \
              (__u64)(((__u64)(x) & (__u64)0x00000000ff000000ULL) <<  8) | \
              (__u64)(((__u64)(x) & (__u64)0x000000ff00000000ULL) >>  8) | \
              (__u64)(((__u64)(x) & (__u64)0x0000ff0000000000ULL) >> 24) | \
              (__u64)(((__u64)(x) & (__u64)0x00ff000000000000ULL) >> 40) | \
              (__u64)(((__u64)(x) & (__u64)0xff00000000000000ULL) >> 56) ))


#define be16_to_cpu(val)    ((__u16)___constant_swab16((__be16)(val)))
#define be32_to_cpu(val)    ((__u32)___constant_swab32((__be32)(val)))
#define be64_to_cpu(val)    ((__u64)___constant_swab64((__be64)(val)))


#define le16_to_cpu(x)      (__u16)__s16_to_cpu((le16)(x))
#define le32_to_cpu(x)      (__u32)__s32_to_cpu((le32)(x))
#define le64_to_cpu(x)      (__u64)__le64_to_cpu((le64)(x))

#define le16_to_cpup(x)     (__u16)__s16_to_cpu(*(const le16*)(x))
#define le32_to_cpup(x)     (__u32)__s32_to_cpu(*(const le32*)(x))
#define le64_to_cpup(x)     (__u64)__le64_to_cpu(*(const le64*)(x))

/* Signed from LE to CPU conversion. */

#define sle16_to_cpu(x)     (s16)__s16_to_cpu((sle16)(x))
#define sle32_to_cpu(x)     (s32)__s32_to_cpu((sle32)(x))
#define sle64_to_cpu(x)     (s64)__le64_to_cpu((sle64)(x))

#define sle16_to_cpup(x)    (s16)__s16_to_cpu(*(const sle16*)(x))
#define sle32_to_cpup(x)    (s32)__s32_to_cpu(*(const sle32*)(x))
#define sle64_to_cpup(x)    (s64)__le64_to_cpu(*(const sle64*)(x))

/* Unsigned from CPU to LE conversion. */

#define cpu_to_le16(x)      (le16)__cpu_to_le16((__u16)(x))
#define cpu_to_le32(x)      (le32)__cpu_to_le32((__u32)(x))
#define cpu_to_le64(x)      (le64)__cpu_to_le64((__u64)(x))

#define cpu_to_le16p(x)     (le16)__cpu_to_le16(*(const __u16*)(x))
#define cpu_to_le32p(x)     (le32)__cpu_to_le32(*(const __u32*)(x))
#define cpu_to_le64p(x)     (le64)__cpu_to_le64(*(const __u64*)(x))

/* Signed from CPU to LE conversion. */

#define cpu_to_sle16(x)     ( sle16)__cpu_to_le16((s16)(x))
#define cpu_to_sle32(x)     ( sle32)__cpu_to_le32((s32)(x))
#define cpu_to_sle64(x)     ( sle64)__cpu_to_le64((s64)(x))

#define cpu_to_sle16p(x)    ( sle16)__cpu_to_le16(*(const s16*)(x))
#define cpu_to_sle32p(x)    ( sle32)__cpu_to_le32(*(const s32*)(x))
#define cpu_to_sle64p(x)    ( sle64)__cpu_to_le64(*(const s64*)(x))

/* Constant endianness conversion defines. */

#define const_le16_to_cpu(x)    (__u16)__constant_le16_to_cpu((le16)(x))
#define const_le32_to_cpu(x)    (__u32)__constant_le32_to_cpu((le32)(x))
#define const_le64_to_cpu(x)    (__u64)__constant_le64_to_cpu((le64)(x))

#define const_cpu_to_le16(x)    (le16)__constant_cpu_to_le16((__u16)(x))
#define const_cpu_to_le32(x)    (le32)__constant_cpu_to_le32((__u32)(x))
#define const_cpu_to_le64(x)    (le64)__constant_cpu_to_le64((__u64)(x))
#define const_cpu_to_sle64(x)   (le64)__constant_cpu_to_sle64((__s64)(x))

#ifdef __CHECKER__
static void ntfs_endian_self_test(void)
{
    /* Should not generate warnings. */
    (le16)cpu_to_le16((__u16)1);
    (le32)cpu_to_le32((__u32)1);
    (le64)cpu_to_le64((__u64)1);
    (sle16)cpu_to_sle16((s16)1);
    (sle32)cpu_to_sle32((s32)1);
    (sle64)cpu_to_sle64((s64)1);
    (__u16)le16_to_cpu((le16)1);
    (__u32)le32_to_cpu((le32)1);
    (__u64)le64_to_cpu((le64)1);
    (s16)sle16_to_cpu((sle16)1);
    (s32)sle32_to_cpu((sle32)1);
    (s64)sle64_to_cpu((sle64)1);
    (le16)const_cpu_to_le16((__u16)1);
    (le32)const_cpu_to_le32((__u32)1);
    (le64)const_cpu_to_le64((__u64)1);
    (__u16)const_le16_to_cpu((le16)1);
    (__u32)const_le32_to_cpu((le32)1);
    (__u64)const_le64_to_cpu((le64)1);

    /*
     * TODO: Need some how to test that warnings are actually generated,
     * but without flooding output with them and vice-versa print warning
     * in case if some one warning is not triggered, but should.  (Yura)
     *
     * I think it can only be done in  a ./configure like script / shell
     * script that will compile known good  and known bad code and pipe the
     * output from sparse to a file, then  grep the file for the wanted
     * warnings/lack thereof and then it would  say "Tests: PASS " or
     * "Tests: FAILED" or whatever.  And you can then  hook that into a
     * "make test" make target or similar so it is only  done when one
     * wants to do it...  (Anton)
     *
     * Also we can look on sparse self test script.  (Yura)
     */
}
#endif

static inline void le32_add_cpu(__le32 *var, u32 val)
{
    *var = cpu_to_le32(le32_to_cpu(*var) + val);
}

#endif  /* __ENDIANS_H__ */
