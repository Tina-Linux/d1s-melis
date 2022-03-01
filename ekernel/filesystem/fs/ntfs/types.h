/*
 * types.h - Misc type definitions not related to on-disk structure.  Part of
 *       the Linux-NTFS project.
 *
 * Copyright (c) 2000-2004 Anton Altaparmakov
 * Copyright (c)      2006 Szabolcs Szakacsits
 * Copyright (c)      2007 Yura Pakhuchiy
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _NTFS_TYPES_H
#define _NTFS_TYPES_H

#include <typedef.h>

#ifndef u8
typedef __u8  u8;      /* Unsigned types of an exact size */
#endif

#ifndef u16
typedef __u16 u16;
#endif

#ifndef u32
typedef __u32 u32;
#endif

#ifndef u64
typedef __u64 u64;
#endif


#ifndef s8
typedef __s8  s8;     /* Signed types of an exact size */
#endif

#ifndef s16
typedef __s16 s16;
#endif

#ifndef s32
typedef __s32 s32;
#endif

#ifndef s64
typedef __s64 s64;
#endif


#undef  __bitwise
#undef  __force
#define __bitwise
#define __force

#ifndef le16
typedef u16 __bitwise le16;
#endif

#ifdef CONFIG_ARM
#ifndef le64
typedef u64 __bitwise le64;
#endif
#endif

/*
 * Declare sle{16,32,64} to be unsigned because we do not want sign extension
 * on BE architectures.
 */
#ifndef sle16
typedef u16 __bitwise sle16;
#endif

#ifndef sle32
typedef u32 __bitwise sle32;
#endif

#ifndef sle64
typedef u64 __bitwise sle64;
#endif

#ifndef be16
typedef u16 __bitwise be16;
#endif

#ifndef be32
typedef u32 __bitwise be32;
#endif

#ifndef be64
typedef u64 __bitwise be64;
#endif

#ifdef CONFIG_ARM
#ifndef size_t
typedef unsigned int  size_t;
#endif
#endif

typedef le16 ntfschar;      /* 2-byte Unicode character type. */
#define UCHAR_T_SIZE_BITS 1

/*
 * Clusters are signed 64-bit values on NTFS volumes.  We define two types, LCN
 * and VCN, to allow for type checking and better code readability.
 */
typedef s64   VCN;
typedef sle64 leVCN;
typedef s64   LCN;
typedef sle64 leLCN;

/*
 * The NTFS journal $LogFile uses log sequence numbers which are signed 64-bit
 * values.  We define our own type LSN, to allow for type checking and better
 * code readability.
 */
typedef s64   LSN;
typedef sle64 leLSN;

/**
 * enum IGNORE_CASE_BOOL -
 */
typedef enum
{
    CASE_SENSITIVE  = 0,
    IGNORE_CASE     = 1,
} IGNORE_CASE_BOOL;

#define STATUS_OK       (0)
#define STATUS_ERROR        (-1)
#define STATUS_RESIDENT_ATTRIBUTE_FILLED_MFT  (-2)
#define STATUS_KEEP_SEARCHING     (-3)
#define STATUS_NOT_FOUND      (-4)

#endif /* defined _NTFS_TYPES_H */

