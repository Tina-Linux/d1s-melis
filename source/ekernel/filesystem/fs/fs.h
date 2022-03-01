/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fs.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: vfs header file, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:20:32  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __FS_H__
#define __FS_H__

/*
 * This file has definitions for some important file table
 * structures etc.
 */
#include "sys_fsys_i.h"
#include "fstime.h"
#include "supports.h"
#include <typedef.h>
#include <sys_fsys.h>
#include <fcntl.h>
#include <stdio.h>

#define WARN_ON(condition)  do {} while(0)
#define BUG()               do {} while(0)
#define BUG_ON(expr)        do {} while(0)

/* bdev.h : temp use */
#define MINORBITS           20
#define MINORMASK           ((1U << MINORBITS) - 1)
#define MAJOR(dev)          ((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)          ((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma,mi)        (((ma) << MINORBITS) | (mi))

/* default Charset used for input and display */
//-}

/*change Octal to Hexadecimal*/
#ifndef O_DIRECTORY     /* check the <fcntl.h> file*/
#define O_DIRECTORY         0x200000/*010000000*/  /* must be a directory */
#endif

//#define O_NOFOLLOW        00400000  /* don't follow links */
#define O_NOATIME           0x40000/*01000000*/
//#define O_ATOMICLOOKUP    02000000  /* do atomic file lookup */
#ifndef O_BINARY        /* check the <fcntl.h> file*/
#define O_BINARY            0x10000/*00200000*/
#endif

#define S_IRWXUGO           (S_IRWXU|S_IRWXG|S_IRWXO)
#define S_IALLUGO           (S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define S_IRUGO             (S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO             (S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO             (S_IXUSR|S_IXGRP|S_IXOTH)


#ifndef DATA_TYPE_X___le16
#define DATA_TYPE_X___le16
typedef     short int           __le16;
#endif

#ifndef DATA_TYPE_X___le32
#define DATA_TYPE_X___le32
typedef     int                 __le32;
#endif

#ifndef DATA_TYPE_X___le64
#define DATA_TYPE_X___le64
typedef     long long           __le64;
#endif

#ifndef DATA_TYPE_X_le16
#define DATA_TYPE_X_le16
typedef     unsigned short int  le16;
#endif

#ifndef DATA_TYPE_X_le32
#define DATA_TYPE_X_le32
typedef     unsigned int        le32;
#endif

#ifndef DATA_TYPE_X_le64
#define DATA_TYPE_X_le64
typedef     unsigned long long  le64;
#endif


#define FS_STAT_INACTIVE        0x00
#define FS_STAT_ACTIVE          0x01

/*
 * It's silly to have NR_OPEN bigger than NR_FILE, but you can change
 * the file limit at runtime and only root can increase the per-process
 * nr_file rlimit, so it's safe to set up a ridiculously high absolute
 * upper limit on files-per-process.
 *
 * Some programs (notably those using select()) may have to be
 * recompiled to take full advantage of the new limits..
 */
/* Fixed constants first: */
#define NR_MAXOPEN          64      /* Absolute upper limit on fd num */
#define DENTRY_INODE_CNT    (NR_MAXOPEN << 3)

#define BLOCK_SIZE_BITS     10
#define BLOCK_SIZE          (1<<BLOCK_SIZE_BITS)

/* And dynamically-tunable limits and defaults: */

extern __s32 fs_err;

struct inodes_stat_t
{
    int nr_inodes;
    int nr_unused;
    int dummy[5];
};
extern struct inodes_stat_t inodes_stat;

#define NR_FILE             8192    /* this can well be larger on a larger system */

#define MAY_EXEC            1
#define MAY_WRITE           2
#define MAY_READ            4
#define MAY_APPEND          8

#define FMODE_READ          1
#define FMODE_WRITE         2

/* Internal kernel extensions */
#define FMODE_LSEEK         4
#define FMODE_PREAD         8
#define FMODE_PWRITE        FMODE_PREAD /* These go hand in hand */

/* File is being opened for execution. Primary users of this flag are
   distributed filesystems that can use it to achieve correct ETXTBUSY
   behavior for cross-node execution/opening_for_writing of files */
#define FMODE_EXEC          16

#define RW_MASK             1
#define RWA_MASK            2
#define READ                0
#define WRITE               1
#define READA               2       /* read-ahead  - don't block if no resources */
#define SWRITE              3       /* for ll_rw_block() - wait for buffer lock */
#define READ_SYNC           (READ | (1 << BIO_RW_SYNC))
#define READ_META           (READ | (1 << BIO_RW_META))
#define WRITE_SYNC          (WRITE | (1 << BIO_RW_SYNC))
#define WRITE_BARRIER       ((1 << BIO_RW) | (1 << BIO_RW_BARRIER))

#define SEL_IN              1
#define SEL_OUT             2
#define SEL_EX              4

/* public flags for file_system_type */
#define FS_REQUIRES_DEV     1
#define FS_BINARY_MOUNTDATA 2
#define FS_HAS_SUBTYPE      4
#define FS_REVAL_DOT        16384   /* Check the paths ".", ".." for staleness */
#define FS_RENAME_DOES_D_MOVE   32768   /* FS will handle d_move()
                     * during rename() internally.
                     */

/*
 * These are the fs-independent mount-flags: up to 32 flags are supported
 */
#define MS_RDONLY           1       /* Mount read-only */
#define MS_NOSUID           2       /* Ignore suid and sgid bits */
#define MS_NODEV            4       /* Disallow access to device special files */
#define MS_NOEXEC           8       /* Disallow program execution */
#define MS_SYNCHRONOUS      16      /* Writes are synced at once */
#define MS_REMOUNT          32      /* Alter flags of a mounted FS */
#define MS_MANDLOCK         64      /* Allow mandatory locks on an FS */
#define MS_DIRSYNC          128     /* Directory modifications are synchronous */
#define MS_NOATIME          1024    /* Do not update access times. */
#define MS_NODIRATIME       2048    /* Do not update directory access times */
#define MS_BIND             4096
#define MS_MOVE             8192
#define MS_REC              16384
#define MS_VERBOSE          32768   /* War is peace. Verbosity is silence. MS_VERBOSE is deprecated. */
#define MS_SILENT           32768
#define MS_POSIXACL         (1<<16) /* VFS does not apply the umask */
#define MS_UNBINDABLE       (1<<17) /* change to unbindable */
#define MS_PRIVATE          (1<<18) /* change to private */
#define MS_SLAVE            (1<<19) /* change to slave */
#define MS_SHARED           (1<<20) /* change to shared */
#define MS_RELATIME         (1<<21) /* Update atime relative to mtime/ctime. */
#define MS_ACTIVE           (1<<30)
#define MS_NOUSER           (1<<31)

/*
 * Superblock flags that can be altered by MS_REMOUNT
 */
#define MS_RMT_MASK         (MS_RDONLY|MS_SYNCHRONOUS|MS_MANDLOCK)

/*
 * Old magic mount flag and mask
 */
#define MS_MGC_VAL          0xC0ED0000
#define MS_MGC_MSK          0xffff0000

/* Inode flags - they have nothing to superblock flags now */

#define S_SYNC              1       /* Writes are synced at once */
#define S_NOATIME           2       /* Do not update access times */
#define S_APPEND            4       /* Append-only file */
#define S_IMMUTABLE         8       /* Immutable file */
#define S_DEAD              16      /* removed, but still open directory */
#define S_NOQUOTA           32      /* Inode is not counted to quota */
#define S_DIRSYNC           64      /* Directory modifications are synchronous */
#define S_NOCMTIME          128     /* Do not update file c/mtime */
#define S_SWAPFILE          256     /* Do not truncate: swapon got its bmaps */
#define S_PRIVATE           512     /* Inode is fs-internal */

/*
 * Note that nosuid etc flags are inode-specific: setting some file-system
 * flags just means all the inodes inherit those flags by default. It might be
 * possible to override it selectively if you really wanted to with some
 * ioctl() that is not currently implemented.
 *
 * Exception: MS_RDONLY is always applied to the entire file system.
 *
 * Unfortunately, it is possible to change a filesystems flags with it mounted
 * with files in use.  This means that all of the inodes will not have their
 * i_flags updated.  Hence, i_flags no longer inherit the superblock mount
 * flags, so these have to be checked separately. -- rmk@arm.uk.linux.org
 */
#define __IS_FLG(inode,flg)     ((inode)->i_sb->s_flags & (flg))

#define IS_RDONLY(inode)        ((inode)->i_sb->s_flags & MS_RDONLY)
#define IS_SYNC(inode)          (__IS_FLG(inode, MS_SYNCHRONOUS) || ((inode)->i_flags & S_SYNC))
#define IS_DIRSYNC(inode)       (__IS_FLG(inode, MS_SYNCHRONOUS|MS_DIRSYNC) || ((inode)->i_flags & (S_SYNC|S_DIRSYNC)))
#define IS_MANDLOCK(inode)      __IS_FLG(inode, MS_MANDLOCK)
#define IS_NOATIME(inode)       __IS_FLG(inode, MS_RDONLY|MS_NOATIME)

#define IS_NOQUOTA(inode)       ((inode)->i_flags & S_NOQUOTA)
#define IS_APPEND(inode)        ((inode)->i_flags & S_APPEND)
#define IS_IMMUTABLE(inode)     ((inode)->i_flags & S_IMMUTABLE)
#define IS_POSIXACL(inode)      __IS_FLG(inode, MS_POSIXACL)

#define IS_DEADDIR(inode)       ((inode)->i_flags & S_DEAD)
#define IS_NOCMTIME(inode)      ((inode)->i_flags & S_NOCMTIME)
#define IS_SWAPFILE(inode)      ((inode)->i_flags & S_SWAPFILE)
#define IS_PRIVATE(inode)       ((inode)->i_flags & S_PRIVATE)

/*
 * Inode flags (FS_IOC_GETFLAGS / FS_IOC_SETFLAGS)
 */
#define FS_SECRM_FL             0x00000001 /* Secure deletion */
#define FS_UNRM_FL              0x00000002 /* Undelete */
#define FS_COMPR_FL             0x00000004 /* Compress file */
#define FS_SYNC_FL              0x00000008 /* Synchronous updates */
#define FS_IMMUTABLE_FL         0x00000010 /* Immutable file */
#define FS_APPEND_FL            0x00000020 /* writes to file may only append */
#define FS_NODUMP_FL            0x00000040 /* do not dump file */
#define FS_NOATIME_FL           0x00000080 /* do not update atime */
/* Reserved for compression usage... */
#define FS_DIRTY_FL             0x00000100
#define FS_COMPRBLK_FL          0x00000200 /* One or more compressed clusters */
#define FS_NOCOMP_FL            0x00000400 /* Don't compress */
#define FS_ECOMPR_FL            0x00000800 /* Compression error */
/* End compression flags --- maybe not all used */
#define FS_BTREE_FL             0x00001000 /* btree format dir */
#define FS_INDEX_FL             0x00001000 /* hash-indexed directory */
#define FS_IMAGIC_FL            0x00002000 /* AFS directory */
#define FS_JOURNAL_DATA_FL      0x00004000 /* Reserved for ext3 */
#define FS_NOTAIL_FL            0x00008000 /* file tail should not be merged */
#define FS_DIRSYNC_FL           0x00010000 /* dirsync behaviour (directories only) */
#define FS_TOPDIR_FL            0x00020000 /* Top of directory hierarchies*/
#define FS_EXTENT_FL            0x00080000 /* Extents */
#define FS_DIRECTIO_FL          0x00100000 /* Use direct i/o */
#define FS_RESERVED_FL          0x80000000 /* reserved for ext2 lib */

#define FS_FL_USER_VISIBLE      0x0003DFFF /* User visible flags */
#define FS_FL_USER_MODIFIABLE   0x000380FF /* User modifiable flags */

#define SYNC_FILE_RANGE_WAIT_BEFORE 1
#define SYNC_FILE_RANGE_WRITE       2
#define SYNC_FILE_RANGE_WAIT_AFTER  4

#include "dcache.h"
#include "namei.h"
//#include "cache.h"
#include "list.h"

struct nameidata;
struct kstatfs;
struct kstat;

extern void inode_init(unsigned long);
extern void files_init(unsigned long);

struct buffer_head;
typedef int (get_block_t)(struct inode *inode, int iblock,
                          struct buffer_head *bh_result, int create);

/*
 * Attribute flags.  These should be or-ed together to figure out what
 * has been changed!
 */
#define ATTR_MODE               1
#define ATTR_UID                2
#define ATTR_GID                4
#define ATTR_SIZE               8
#define ATTR_ATIME              16
#define ATTR_MTIME              32
#define ATTR_CTIME              64
#define ATTR_ATIME_SET          128
#define ATTR_MTIME_SET          256
#define ATTR_FORCE              512 /* Not a change, but a change it */
#define ATTR_ATTR_FLAG          1024
#define ATTR_KILL_SUID          2048
#define ATTR_KILL_SGID          4096
#define ATTR_FILE               8192

/*
 * This is the Inode Attributes structure, used for notify_change().  It
 * uses the above definitions as flags, to know which values have changed.
 * Also, in this manner, a Filesystem can look at only the values it cares
 * about.  Basically, these are the attributes that the VFS layer can
 * request to change from the FS layer.
 *
 * Derek Atkins <warlord@MIT.EDU> 94-10-20
 */
struct iattr
{
    unsigned int    ia_valid;
    __s64           ia_size;
    struct timespec ia_atime;
    struct timespec ia_mtime;
    struct timespec ia_ctime;
};

/*
 * Includes for diskquotas.
 */

/**
 * enum positive_aop_returns - aop return codes with specific semantics
 *
 * @AOP_WRITEPAGE_ACTIVATE: Informs the caller that page writeback has
 *              completed, that the page is still locked, and
 *              should be considered active.  The VM uses this hint
 *              to return the page to the active list -- it won't
 *              be a candidate for writeback again in the near
 *              future.  Other callers must be careful to unlock
 *              the page if they get this return.  Returned by
 *              writepage();
 *
 * @AOP_TRUNCATED_PAGE: The AOP method that was handed a locked page has
 *              unlocked it and the page might have been truncated.
 *              The caller should back up to acquiring a new page and
 *              trying again.  The aop will be taking reasonable
 *              precautions not to livelock.  If the caller held a page
 *              reference, it should drop it before retrying.  Returned
 *              by readpage(), prepare_write(), and commit_write().
 *
 * address_space_operation functions return these large constants to indicate
 * special semantics to the caller.  These are much larger than the bytes in a
 * page to allow for functions that return the number of bytes operated on in a
 * given page.
 */

enum positive_aop_returns
{
    AOP_WRITEPAGE_ACTIVATE  = 0x80000,
    AOP_TRUNCATED_PAGE      = 0x80001,
};

/*
 * oh the beauties of C type declarations.
 */
struct page;
struct writeback_control;

/*
 * Use sequence counter to get consistent i_size on 32-bit processors.
 */
#if BITS_PER_LONG==32 && defined(CONFIG_SMP)
#define __NEED_I_SIZE_ORDERED
#define i_size_ordered_init(inode) seqcount_init(&inode->i_size_seqcount)
#else
#define i_size_ordered_init(inode) do { } while (0)
#endif

struct inode
{
    struct hlist_node   i_hash;
    struct list_head    i_list;
    struct list_head    i_sb_list;
    struct list_head    i_dentry;
    unsigned long       i_ino;
    atomic_t            i_count;
    unsigned int        i_nlink;
    unsigned long       i_version;
    __s64               i_size;

    struct dentry       *debug_dentry;

    struct list_head    i_dirty_buffers;
    struct list_head    i_dirty_data_buffers;

    struct timespec     i_atime;
    struct timespec     i_mtime;
    struct timespec     i_ctime;
    unsigned int        i_blkbits;
    unsigned long       i_blocks;
    unsigned short      i_bytes;
    int                 i_mode;
    //  struct mutex        i_mutex;
    //  struct rw_semaphore i_alloc_sem;
    const struct inode_operations   *i_op;
    const struct file_operations    *i_fop; /* former ->i_op->default_file_ops */
    struct super_block  *i_sb;
    struct list_head    i_devices;

    int                 i_cindex;

    __u32               i_generation;

    unsigned long       i_state;
    unsigned long       dirtied_when;       /* jiffies of first dirtying */

    unsigned int        i_flags;

    void                *i_private;         /* fs or device private pointer */
};

struct file
{
    struct dentry       *f_dentry;
    const struct file_operations    *f_op;
    unsigned int        f_flags;
    int                 f_mode;
    int                 f_dev;
    __s64               f_pos;
    int                 f_err;
    atomic_t            f_count;
    /* needed for tty driver, and maybe others */
    void                *private_data;
    int                 f_fd;
};

typedef struct dirent_s
{
    __u32               d_ino;                          /* to be POSIX conform        */
    __u8                fatdirattr;                     /* file attib, look at FSYS_ATTR_XXX. */
    __u64               d_size;                         /* file size, if directory it will be 0 */
    char                d_name[FSYS_DIRNAME_MAX];       /* name of dirent */
} __fsys_dirent_t;

#define get_file(x)     atomic_inc(&(x)->f_count)
#define file_count(x)   atomic_read(&(x)->f_count)

#define MAX_NON_LFS     ((1UL<<31) - 1)

/* Page cache limit. The filesystems should put that into their s_maxbytes
   limits, otherwise bad things can happen in VM. */
#if BITS_PER_LONG==32
#define MAX_LFS_FILESIZE    (((__u64)PAGE_CACHE_SIZE << (BITS_PER_LONG-1))-1)
#elif BITS_PER_LONG==64
#define MAX_LFS_FILESIZE    0x7fffffffffffffffUL
#endif

#define FL_POSIX            1
#define FL_FLOCK            2
#define FL_ACCESS           8   /* not trying to lock, just looking */
#define FL_EXISTS           16  /* when unlocking, test for existence */
#define FL_LEASE            32  /* lease held on this file */
#define FL_CLOSE            64  /* unlock on close */
#define FL_SLEEP            128 /* A blocking lock */


struct kstatfs
{
    __u32 f_type;           /* fs type */
    __u32 f_bsize;          /* block size */
    __u32 f_blocks;         /* blocks number */
    __u32 f_bfree;          /* free blocks number */
    __u32 f_files;          /* exist files number */
    __u32 f_namelen;        /* max name lenght */

    __u32 f_attribute;      /* fs attribute like readonly */
    __u32 f_mediaattr;      /* media attribute like movable */
    __u8  f_volletter;      /* like 'c' or 'd' etc */
    __u8  f_volname[MAX_VOLUME_NAME_LEN];       /* volume name */
    __u8  f_fsname[MAX_FS_NAME_LEN];            /* the bind file system name */
};

extern struct list_head super_blocks;

struct super_block
{
    struct list_head        s_list;     /* Keep this first */
    unsigned long           s_blocksize;
    unsigned char           s_blocksize_bits;
    unsigned char           s_dirt;
    unsigned long long      s_maxbytes; /* Max file size */
    struct file_system_type         *s_type;
    const struct super_operations   *s_op;
    unsigned long           s_flags;
    unsigned long           s_magic;
    struct dentry           *s_root;
    int                     s_syncing;
    int                     s_need_sync_fs;

    struct list_head        s_inodes;   /* all inodes */
    struct list_head        s_dirty;    /* dirty inodes */
    struct list_head        s_io;       /* parked for writeback */
    struct list_head        s_dentrys;
    struct list_head        s_files;
    struct list_head        s_locked_inodes;/* inodes being synced */

    __hdle                  s_part;
    struct list_head        s_instances;

    int                     s_frozen;

    //  char s_id[32];              /* Informational name */
    void                    *s_fs_info; /* Filesystem private info */

    /* Granularity of c/m/atime in ns.
       Cannot be worse than a second */
    __u32                   s_time_gran;

    char                    s_letter;
    char                    s_fsname[MAX_FS_NAME_LEN];              /* Informational name */
    char                    s_volname[MAX_VOLUME_NAME_LEN];
};

/*
 * Snapshotting support.
 */
enum
{
    SB_UNFROZEN         = 0,
    SB_FREEZE_WRITE     = 1,
    SB_FREEZE_TRANS     = 2,
};

/*
 * VFS helper functions..
 */
extern int  vfs_create(struct inode *, struct dentry *, int, struct nameidata *);
extern int  vfs_mkdir(struct inode *, struct dentry *, int);
//extern int vfs_mknod(struct inode *, struct dentry *, int, dev_t);
extern int  vfs_symlink(struct inode *, struct dentry *, const char *, int);
extern int  vfs_link(struct dentry *, struct inode *, struct dentry *);
extern int  vfs_rmdir(struct inode *, struct dentry *);
extern int  vfs_unlink(struct inode *, struct dentry *);
extern int  vfs_rename(struct inode *, struct dentry *, struct inode *, struct dentry *);

extern int  do_unlink(const char *pathname, struct super_block *sb);

extern int  do_rename(const char *oldname, const char *newname, struct super_block *sb);

/*
 * VFS dentry helper functions.
 */
extern void dentry_unhash(struct dentry *dentry);

/*
 * File types
 *
 * NOTE! These match bits 12..15 of stat.st_mode
 * (ie "(i_mode >> 12) & 15").
 */
#define DT_UNKNOWN      0
#define DT_FIFO         1
#define DT_CHR          2
#define DT_DIR          4
#define DT_BLK          6
#define DT_REG          8
#define DT_LNK          10
#define DT_SOCK         12
#define DT_WHT          14

#define OSYNC_METADATA  (1<<0)
#define OSYNC_DATA      (1<<1)
#define OSYNC_INODE     (1<<2)

/*
 * This is the "filldir" function type, used by readdir() to let
 * the kernel specify what kind of dirent layout it wants to have.
 * This allows the kernel to read directories into kernel space or
 * to have different dirent layouts depending on the binary type.
 */
typedef int (*filldir_t)(void *dirent, const char  *name,
                         const int name_len, const int name_type, const __s64 pos,
                         const __u64 ino, const unsigned dt_type, __u64 d_size);

/* These macros are for out of kernel modules to test that
 * the kernel supports the unlocked_ioctl and compat_ioctl
 * fields in struct file_operations. */
#define HAVE_COMPAT_IOCTL   1
#define HAVE_UNLOCKED_IOCTL 1

/*
 * NOTE:
 * read, write, poll, fsync, readv, writev, unlocked_ioctl and compat_ioctl
 * can be called without the big kernel lock held in all filesystems.
 */
struct file_operations
{
    __s64(*llseek)(struct file *, __s64, __s32);
    __s32(*read)(struct file *, char *, __u32, __s64 *);
    __s32(*write)(struct file *, const char *, __u32, __s64 *);
    __s32(*readdir)(struct file *, void *, filldir_t);
    __s32(*ioctl)(struct inode *, struct file *, __u32, __u32, void *);
    __s32(*open)(struct inode *, struct file *);
    __s32(*close)(struct file *);
    __s32(*flush)(struct file *);
    __s32(*release)(struct inode *, struct file *);
    __s32(*fsync)(struct file *, struct dentry *, __s32);
    __s32(*fasync)(__s32, struct file *, __s32);
};

struct inode_operations
{
    __s32(*create)(struct inode *, struct dentry *, __s32, struct nameidata *);
    struct dentry *(*lookup)(struct inode *, struct dentry *, struct nameidata *);
    __s32(*unlink)(struct inode *, struct dentry *);
    __s32(*mkdir)(struct inode *, struct dentry *, __s32);
    __s32(*rmdir)(struct inode *, struct dentry *);
    __s32(*rename)(struct inode *, struct dentry *, struct inode *, struct dentry *);
    void (*truncate)(struct inode *);
    __s32(*setattr)(struct dentry *, struct iattr *);
    __s32(*getattr)(struct dentry *, struct kstat *);
};

/*
 * NOTE: write_inode, delete_inode, clear_inode, put_inode can be called
 * without the big kernel lock held in all filesystems.
 */
struct super_operations
{
    struct inode *(*alloc_inode)(struct super_block *);
    void (*destroy_inode)(struct inode *);

    void (*read_inode)(struct inode *);

    void (*dirty_inode)(struct inode *);
    __s32(*write_inode)(struct inode *, __s32);
    void (*put_inode)(struct inode *);
    void (*drop_inode)(struct inode *);
    void (*delete_inode)(struct inode *);
    void (*put_super)(struct super_block *);
    void (*write_super)(struct super_block *);
    __s32(*sync_fs)(struct super_block *, __s32);
    void (*write_super_lockfs)(struct super_block *);
    void (*unlockfs)(struct super_block *);
    int (*statfs)(struct super_block *, struct kstatfs *, __u32);
    void (*clear_inode)(struct inode *);
};

/* Inode state bits.  Protected by inode_lock. */
#define I_DIRTY_SYNC        1   /* Not dirty enough for O_DATASYNC */
#define I_DIRTY_DATASYNC    2   /* Data-related inode changes pending */
#define I_DIRTY_PAGES       4   /* Data-related inode changes pending */
#define __I_LOCK            3
#define I_LOCK              (1 << __I_LOCK)
#define I_FREEING           16
#define I_CLEAR             32
#define I_NEW               64
#define I_WILL_FREE         128

#define I_DIRTY             (I_DIRTY_SYNC | I_DIRTY_DATASYNC | I_DIRTY_PAGES)

extern void __mark_inode_dirty(struct inode *, int);
static inline void mark_inode_dirty(struct inode *inode)
{
    __mark_inode_dirty(inode, I_DIRTY);
}

static inline void mark_inode_dirty_sync(struct inode *inode)
{
    __mark_inode_dirty(inode, I_DIRTY_SYNC);
}

/**
 * inc_nlink - directly increment an inode's link count
 * @inode: inode
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.  Currently,
 * it is only here for parity with dec_nlink().
 */
static inline void inc_nlink(struct inode *inode)
{
    inode->i_nlink++;
}

static inline void inode_inc_link_count(struct inode *inode)
{
    inc_nlink(inode);
    mark_inode_dirty(inode);
}

/**
 * drop_nlink - directly drop an inode's link count
 * @inode: inode
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.  In cases
 * where we are attempting to track writes to the
 * filesystem, a decrement to zero means an imminent
 * write when the file is truncated and actually unlinked
 * on the filesystem.
 */
static inline void drop_nlink(struct inode *inode)
{
    inode->i_nlink--;
}

/**
 * clear_nlink - directly zero an inode's link count
 * @inode: inode
 *
 * This is a low-level filesystem helper to replace any
 * direct filesystem manipulation of i_nlink.  See
 * drop_nlink() for why we care about i_nlink hitting zero.
 */
static inline void clear_nlink(struct inode *inode)
{
    inode->i_nlink = 0;
}

static inline void inode_dec_link_count(struct inode *inode)
{
    drop_nlink(inode);
    mark_inode_dirty(inode);
}

extern void touch_atime(struct dentry *dentry);
static inline void file_accessed(struct file *file)
{
    if (!(file->f_flags & O_NOATIME))
    {
        touch_atime(file->f_dentry);
    }
}

extern int shrink_icache_memory(int nr);

struct file_system_type
{
    const char *name;
    int fs_flags;
    int use_cnt;
    int status;
    int (*mount)(__hdle);
    int (*unmount)(struct super_block *, int);
    int (*identify)(__hdle);
    int (*format)(__hdle, __hdle);
    struct file_system_type *next;
    struct list_head fs_supers;
};

struct fmt_para
{
    int cluster_size;           //cluster size
    int fs_size;                //file system size
    int tracks;                 //tracek per cylinder
    int sectors;                //sectors per tracks
    int force_umnt;             //force unmount the part if mounted
    int quick;                  //quick format
    int compress;               //set filesystem to have compress attrib
    int subtype;                //sub fs type like fat12/fat16
    int reserve[2];             //reserved space
    char label[12];             //volume label, max len is 11 bytes
};

/* fs recursive mutex structure */
struct fsys_mutex
{
    __hdle          sem;        /* semaphore controlling the lock   */
    __hdle          owner;      /* Thread holding this mutex        */
    __s32           nref;       /* Number of enterances             */
};


extern __s32    get_sb_bdev(struct file_system_type *fs_type,
                            __s32 flags, __hdle part, void *data,
                            __s32(*fill_super)(struct super_block *, void *, __s32));
int             generic_shutdown_super(struct super_block *sb);
int             deactivate_super(struct super_block *sb, int force);

/* fs/open.c */

extern char     *getname(const char *);

/* fs/dcache.c */
extern int      vfs_caches_init(unsigned long);

//#define __getname()   kmem_cache_alloc(names_cachep, SLAB_KERNEL)
//#define __putname(name)   kmem_cache_free(names_cachep, (void *)(name))
extern void     putname(const char *name);

/* fs/char_dev.c */
#define CHRDEV_MAJOR_HASH_SIZE  255

/* fs/block_dev.c */
#define BDEVNAME_SIZE           32  /* Largest string for a blockdev identifier */

/* Invalid inode operations -- fs/bad_inode.c */
//extern void make_bad_inode(struct inode *);
extern int      is_bad_inode(struct inode *);
extern void     make_bad_inode(struct inode *);

extern const struct file_operations read_fifo_fops;
extern const struct file_operations write_fifo_fops;
extern const struct file_operations rdwr_fifo_fops;

extern int fs_may_remount_ro(struct super_block *);

/*
 * return READ, READA, or WRITE
 */
#define bio_rw(bio)             ((bio)->bi_rw & (RW_MASK | RWA_MASK))

/*
 * return data direction, READ or WRITE
 */
#define bio_data_dir(bio)       ((bio)->bi_rw & 1)

//extern int    check_disk_change(struct block_device *);
//extern int    __invalidate_device(struct block_device *);
//extern int    invalidate_partition(struct gendisk *, int);
extern void     unmount_check(struct super_block *sb);

extern int      invalidate_inodes(struct super_block *);
extern int      write_inode_now(struct inode *, int);
extern __s32    file_fsync(struct file *, struct dentry *, __s32);


extern long     do_fsync(struct file *file, int datasync);
extern void     sync_supers(void);
extern void     sync_filesystems(int wait);
extern void     __fsync_super(struct super_block *sb);
extern struct super_block   *path_to_sb(const char *pFullName, const char **pFileName);

extern int      open_namei(const char *, int, int, struct nameidata *);
extern int      may_open(struct nameidata *, int, int);

extern int      kernel_read(struct file *, unsigned long, char *, unsigned long);
extern struct file  *open_exec(const char *);

/* needed for stackable file system support */
extern __s64    default_llseek(struct file *file, __s64 offset, int origin);

extern __s64    vfs_llseek(struct file *file, __s64 offset, int origin);

extern void     inode_init_once(struct inode *inode);
extern void     iput(struct inode *);
extern struct inode     *igrab(struct inode *);
extern int      iunique(struct super_block *, int);
extern int      inode_needs_sync(struct inode *inode);
extern void     generic_delete_inode(struct inode *inode);
extern void     generic_drop_inode(struct inode *inode);

//extern struct inode *ilookup5_nowait(struct super_block *sb,
//      unsigned long hashval, int (*test)(struct inode *, void *),
//      void *data);
//extern struct inode *ilookup5(struct super_block *sb, unsigned long hashval,
//      int (*test)(struct inode *, void *), void *data);
//extern struct inode *ilookup(struct super_block *sb, unsigned long ino);
//
//extern struct inode * iget5_locked(struct super_block *, unsigned long, int (*test)(struct inode *, void *), int (*set)(struct inode *, void *), void *);
//extern struct inode * iget_locked(struct super_block *, unsigned long);
//extern void unlock_new_inode(struct inode *);
//
//static inline struct inode *iget(struct super_block *sb, unsigned long ino)
//{
//  struct inode *inode = iget_locked(sb, ino);
//
//  if (inode && (inode->i_state & I_NEW)) {
//      sb->s_op->read_inode(inode);
//      unlock_new_inode(inode);
//  }
//
//  return inode;
//}

extern void     __iget(struct inode *inode);
extern void     clear_inode(struct inode *);
extern void     destroy_inode(struct inode *);
extern struct inode *   new_inode(struct super_block *);
extern struct inode *   new_inode_nocache(struct super_block *);
extern void     unlock_new_inode(struct inode *inode);
extern int      __remove_suid(struct dentry *, int);
extern int      should_remove_suid(struct dentry *);
extern int      remove_suid(struct dentry *);

extern struct inode     *iget5_locked(struct super_block *sb, unsigned long hashval,
                                  int (*test)(struct inode *, void *),
                                  int (*set)(struct inode *, void *), void *data);
extern struct inode     *iget_locked(struct super_block *sb, unsigned long ino);
extern void     __insert_inode_hash(struct inode *, unsigned long hashval);
extern void     remove_inode_hash(struct inode *);
static inline void  insert_inode_hash(struct inode *inode)
{
    __insert_inode_hash(inode, inode->i_ino);
}
extern void     init_special_inode(struct inode *inode, umode_t mode, dev_t rdev);

extern struct file  *get_empty_filp(void);

extern int      __fat_get_block(struct inode *inode, int iblock,
                            unsigned int *max_blocks,
                            int *phys, int create);

extern int64_t    no_llseek(struct file *file, __s64 offset, __s32 origin);
extern __s64    generic_file_llseek(struct file *file, __s64 offset, __s32 origin);
extern int      generic_file_open(struct inode *inode, struct file *filp);
extern int      nonseekable_open(struct inode *inode, struct file *filp);

extern int      fsync_dev(struct super_block *sb);

#define special_file(m) (S_ISCHR(m)||S_ISBLK(m)||S_ISFIFO(m)||S_ISSOCK(m))

extern void     generic_fillattr(struct inode *, struct kstat *);
void            inode_add_bytes(struct inode *inode, __s64 bytes);
void            inode_sub_bytes(struct inode *inode, __s64 bytes);
__s64           inode_get_bytes(struct inode *inode);
void            inode_set_bytes(struct inode *inode, __s64 bytes);

extern int      vfs_readdir(struct file *, filldir_t, void *);
extern int      vfs_ioctl(struct file *, unsigned int, unsigned int, unsigned long);

extern struct file_system_type  *get_fs_type(const char *name);
extern struct super_block       *get_super(__hdle);

extern int      dcache_dir_open(struct inode *, struct file *);
extern int      dcache_dir_close(struct inode *, struct file *);
extern __s64    dcache_dir_lseek(struct file *, __s64, int);
extern int      dcache_readdir(struct file *, void *, filldir_t);

extern void     file_update_time(struct file *file);

/* vfs lock and unlock */
extern int      esFSYS_vfslock(void);
extern int      esFSYS_vfsunlock(void);

extern struct nls_table     *nls;    /* Charset used for input and display */

extern void     *alloc_large_system_hash(const char *tablename,
                                     unsigned long bucketsize,
                                     unsigned long numentries,
                                     int scale,
                                     int flags,
                                     unsigned int *_hash_shift,
                                     unsigned int *_hash_mask,
                                     unsigned long limit);
/* fstime.c */
extern const struct timezone    sys_tz;
extern struct timespec          current_fs_time(struct super_block *sb);
extern struct ktimestamp        *fs_timespec_to_kstamp(struct ktimestamp *dest, struct timespec ts);

#endif  /* __FS_H__ */
