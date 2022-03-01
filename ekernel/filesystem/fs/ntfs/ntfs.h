#ifndef __NTFS_H__
#define __NTFS_H__

/* NTFS support compress or not */
#define FSYS_NTFS_COMPRESS_EN   (0)

#define NTFS_SUPER_MAGIC 0x4d44

#define NTFS_CODE_PAGE "cp936"

#define __MB_CUR_MAX    3       /* for UTF8 */

#define NTFS_MKMODE(a, m) (m & (a & FILE_ATTR_READONLY ? S_IRUGO|S_IXUGO : S_IRWXUGO))

#define ntfs_calloc(size) calloc(1, (int)size)

#define ntfs_malloc(size) malloc((int)size)

#endif  /* __NTFS_H__ */

