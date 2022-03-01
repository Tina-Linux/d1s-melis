/*
 * dir.h - Exports for directory handling. Originated from the Linux-NTFS project.
 *
 * Copyright (c) 2002      Anton Altaparmakov
 * Copyright (c) 2005-2006 Yura Pakhuchiy
 * Copyright (c) 2004-2005 Richard Russon
 * Copyright (c) 2005-2008 Szabolcs Szakacsits
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
 * along with this program (in the main directory of the NTFS-3G
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _NTFS_DIR_H
#define _NTFS_DIR_H

#include "types.h"

#define PATH_SEP '/'

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

/*
 * We do not have these under DJGPP, so define our version that do not conflict
 * with other S_IFs defined under DJGPP.
 */
#ifdef DJGPP
#ifndef S_IFLNK
#define S_IFLNK  0120000
#endif
#ifndef S_ISLNK
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_IFSOCK
#define S_IFSOCK 0140000
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif
#endif

/*
 * The little endian Unicode strings $I30, $SII, $SDH, $O, $Q, $R
 * as a global constant.
 */
extern ntfschar NTFS_INDEX_I30[5];
extern ntfschar NTFS_INDEX_SII[5];
extern ntfschar NTFS_INDEX_SDH[5];
extern ntfschar NTFS_INDEX_O[3];
extern ntfschar NTFS_INDEX_Q[3];
extern ntfschar NTFS_INDEX_R[3];

extern u64 ntfs_inode_lookup_by_name(ntfs_inode *dir_ni,
                                     const ntfschar *uname, const int uname_len);

extern ntfs_inode *ntfs_pathname_to_inode(ntfs_volume *vol, ntfs_inode *parent,
        const char *pathname);

#if 0
extern int ntfs_delete(ntfs_inode *ni, ntfs_inode *dir_ni, ntfschar *name,
                       u8 name_len);
#endif
/*
 * File types (adapted from include <linux/fs.h>)
 */
#define NTFS_DT_UNKNOWN     0
#define NTFS_DT_FIFO        1
#define NTFS_DT_CHR     2
#define NTFS_DT_DIR     4
#define NTFS_DT_BLK     6
#define NTFS_DT_REG     8
#define NTFS_DT_LNK     10
#define NTFS_DT_SOCK        12
#define NTFS_DT_WHT     14

#endif /* defined _NTFS_DIR_H */

