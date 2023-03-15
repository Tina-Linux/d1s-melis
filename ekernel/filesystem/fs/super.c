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
#include "fs.h"
#include "namei.h"
#include "buffer_head.h"    /* for fsync_super() */
#include "writeback.h"      /* for the emergency remount stuff */
#include "errno.h"
#include "slab.h"
#include "fsys_debug.h"
#include "fsys_libs.h"
#include "ctype.h"

extern struct file_system_type fat_fs_type;
extern struct file *files[];

extern int sync_buffers(struct super_block *sb, int wait);

struct file_system_type *get_fs_type(const char *name);

LIST_HEAD(super_blocks);
/**
 *  alloc_super -   create new superblock
 *  @type:  filesystem type superblock should belong to
 *
 *  Allocates and initializes a new &struct super_block.  alloc_super()
 *  returns a pointer new superblock or %NULL if allocation had failed.
 */
static struct super_block *alloc_super(struct file_system_type *type)
{
    struct super_block *s = calloc(sizeof(struct super_block),  1);
    static struct super_operations default_op;

    if (s)
    {
        INIT_LIST_HEAD(&s->s_locked_inodes);
        INIT_LIST_HEAD(&s->s_dirty);
        INIT_LIST_HEAD(&s->s_io);
        INIT_LIST_HEAD(&s->s_files);
        INIT_LIST_HEAD(&s->s_instances);
        INIT_LIST_HEAD(&s->s_dentrys);
        INIT_LIST_HEAD(&s->s_inodes);
        s->s_maxbytes = MAX_NON_LFS;
        s->s_op = &default_op;
        s->s_time_gran = 1000000000;
    }

    return s;
}

/**
 *  destroy_super   -   frees a superblock
 *  @s: superblock to free
 *
 *  Frees a superblock.
 */
static void destroy_super(struct super_block *s)
{
    //  security_sb_free(s);
    //    list_del(&s->s_list);
    //  kfree(s->s_subtype);
    free(s);
}

/**
 *  put_super   -   drop a temporary reference to superblock
 *  @sb: superblock in question
 *
 *  Drops a temporary reference, frees superblock if there's no
 *  references left.
 */
static void put_super(struct super_block *sb)
{
    fs_log_debug("\"%s\" on %s is destroied.\n", sb->s_fsname, sb->s_volname);

    if (sb->s_type->use_cnt)
    {
        sb->s_type->use_cnt--;
    }
    destroy_super(sb);
}


/*
 * Write out and wait upon all dirty data associated with this
 * superblock.  Filesystem data as well as the underlying block
 * device.  Takes the superblock lock.
 */
int fsync_super(struct super_block *sb)
{
    struct writeback_control wbc =
    {
        .sync_mode  = WB_SYNC_NONE,
        .older_than_this = 0,
        .nr_to_write    = 0,
        .nonblocking    = 1,
        .for_kupdate    = 1,
        .range_cyclic   = 1,
    };

    //  sync_buffers(sb, 0);

    sync_part_inodes(sb, &wbc);
    if (sb->s_dirt && sb->s_op && sb->s_op->write_super)
    {
        sb->s_op->write_super(sb);
    }

    return sync_buffers(sb, 1);
}

/**
 *  generic_shutdown_super  -   common helper for ->kill_sb()
 *  @sb: superblock to kill
 *
 *  generic_shutdown_super() does all fs-independent work on superblock
 *  shutdown.  Typical ->kill_sb() should pick all fs-specific objects
 *  that need destruction out of superblock, call generic_shutdown_super()
 *  and release aforementioned objects.  Note: dentries and inodes _are_
 *  taken care of and do not need specific handling.
 *
 *  Upon calling this function, the filesystem may no longer alter or
 *  rearrange the set of dentries belonging to this super_block, nor may it
 *  change the attachments of dentries to inodes.
 */
int generic_shutdown_super(struct super_block *sb)
{
    int busy = 0;
    const struct super_operations *sop = sb->s_op;

    if (sb->s_root)
    {
        int    i;
        struct dentry *den;

        den = sb->s_root;
        sb->s_root = NULL;
        dput(den);

        for (i = 0; i < NR_MAXOPEN; i++)
        {
            struct file *fp;

            fp = files[i];
            if (fp && fp->f_dentry && fp->f_dentry->d_sb == sb)
            {
                dput(fp->f_dentry);
                fp->f_dentry = NULL;
            }
        }

        shrink_dcache_for_umount(sb, 1);
        fsync_super(sb);
        sb->s_flags &= ~MS_ACTIVE;

        busy = invalidate_inodes(sb);

        if (sop->write_super && sb->s_dirt)
        {
            sop->write_super(sb);
        }
        if (sop->put_super)
        {
            sop->put_super(sb);
        }
    }
    /* should be initialized for __put_super_and_need_restart() */
    list_del_init(&sb->s_list);
    list_del(&sb->s_instances);
    sb->s_part = NULL;

    return busy;
}

static void write_super(struct super_block *sb)
{
    fs_log_trace01("write_super() enter...\n");
    if (sb->s_root && sb->s_dirt)
        if (sb->s_op->write_super)
        {
            sb->s_op->write_super(sb);
        }
}

/*
 * Note: check the dirty flag before waiting, so we don't
 * hold up the sync while mounting a device. (The newly
 * mounted device won't need syncing.)
 */
void sync_supers(void)
{
    struct super_block *sb;

    list_for_each_entry(sb, &super_blocks, s_list)
    {
        if (sb->s_dirt)
        {
            write_super(sb);
        }
    }
}

/*
 * Call the ->sync_fs super_op against all filesytems which are r/w and
 * which implement it.
 *
 * This operation is careful to avoid the livelock which could easily happen
 * if two or more filesystems are being continuously dirtied.  s_need_sync_fs
 * is used only here.  We set it against all filesystems and then clear it as
 * we sync them.  So redirtied filesystems are skipped.
 *
 * But if process A is currently running sync_filesytems and then process B
 * calls sync_filesystems as well, process B will set all the s_need_sync_fs
 * flags again, which will cause process A to resync everything.  Fix that with
 * a local mutex.
 *
 * (Fabian) Avoid sync_fs with clean fs & wait mode 0
 */
void sync_filesystems(int wait)
{
    struct super_block *sb;

    list_for_each_entry(sb, &super_blocks, s_list)
    {
        if (!sb->s_op->sync_fs)
        {
            continue;
        }
        if (sb->s_flags & MS_RDONLY)
        {
            continue;
        }
        sb->s_need_sync_fs = 1;
    }

    list_for_each_entry(sb, &super_blocks, s_list)
    {
        if (!sb->s_need_sync_fs)
        {
            continue;
        }
        sb->s_need_sync_fs = 0;
        if (sb->s_flags & MS_RDONLY)
        {
            continue;    /* hm.  Was remounted r/o meanwhile */
        }
        if (sb->s_root && (wait || sb->s_dirt))
        {
            sb->s_op->sync_fs(sb, wait);
        }
        /* restart only when sb is no longer on the list */
    }
}

struct super_block *path_to_sb(const char *pFullName, const char **pFileName)
{
    char    *s;
    char     sym;
    struct super_block *sb;

    sym = *pFullName;

    /* Find correct FSL (device:unit:name) */
    s = strchr((char *)pFullName, ':');

    /* Scan for device name */
    if (s)
    {
        char n = *pFullName;

        n = (n >= 'A' && n <= 'Z') ? n + ('a' - 'A') : n;

        if ((int)(s - pFullName) != 1
            || n < 'a' || n > 'z')
        {
            return NULL;
        }

        if (pFileName)
        {
            *pFileName = ++s;
        }

        list_for_each_entry(sb, &super_blocks, s_list)
        {
            if (sb->s_letter == n || sb->s_letter == n - ('a' - 'A'))
            {
                return sb;
            }
        }
    }
    else
    {
        if (pFileName)
        {
            *pFileName = pFullName;
        }

        list_for_each_entry(sb, &super_blocks, s_list)
        {
            if (sb->s_letter == 'c' || sb->s_letter == 'C')
            {
                return sb;
            }
        }
    }

    return NULL;
}

__s32 get_sb_bdev(struct file_system_type *fs_type,
                  __s32 flags, __hdle part, void *data,
                  __s32(*fill_super)(struct super_block *, void *, __s32))
{
    struct super_block *s;
    int error = 0;
    char *partname;
    char  *buf = " (c:)";
    int i;

    s = alloc_super(fs_type);
    if (!s)
    {
        goto error_s;
    }
    fs_type->use_cnt++;
    s->s_type = fs_type;
    s->s_flags = flags;
    s->s_part = part;

    /* 设置分区数据结构中的文件系统私有句柄 */
    esFSYS_pioctrl(part, PART_IOC_SYS_SETFSPRIV, (long)s, NULL);

    /* 获取并设置分区的分区设备名和文件系统名 */
    esFSYS_pioctrl(part, PART_IOC_SYS_GETNAME, 0, &partname);
    strncpy(s->s_volname, partname, MAX_VOLUME_NAME_LEN);
    strncpy(s->s_fsname, fs_type->name, MAX_FS_NAME_LEN);

    /* 获取分区的盘符 */
    esFSYS_pioctrl(part, PART_IOC_SYS_GETLETTER, 0, &s->s_letter);

    list_add_tail(&s->s_list, &super_blocks);
    list_add(&s->s_instances, &fs_type->fs_supers);

    error = fill_super(s, data, flags & MS_SILENT ? 1 : 0);
    if (error)
    {
        deactivate_super(s, 1);
        esFSYS_pioctrl(part, PART_IOC_SYS_SETFSPRIV, 0, NULL);
        goto error;
    }

    /* 设置卷标后的盘符"xxx (D:)" */
    i = strlen(s->s_volname);
    buf[2] = toupper(s->s_letter);
    strncpy(&s->s_volname[i], buf, MAX_VOLUME_NAME_LEN - i);

    s->s_flags |= MS_ACTIVE;
    return 0;

error_s:
    error = PTR_ERR(s);
error:
    return error;
}

/**
 *  deactivate_super    -   drop an active reference to superblock
 *  @s: superblock to deactivate
 *
 *  Drops an active reference to superblock, acquiring a temprory one if
 *  there is no active references left.  In that case we lock superblock,
 *  tell fs driver to shut it down and drop the temporary reference we
 *  had just acquired.
 *  if (force==1 && busy==1) or busy==0, return 0, else return -EBUSY.
 */
int deactivate_super(struct super_block *s, int force)
{
    int busy;
    struct file_system_type *fs = s->s_type;

    shrink_dcache_for_umount(s, 0);
    busy = invalidate_inodes(s);
    if (busy && !force)
    {
        return -EBUSY;
    }

    /* 文件系统为空闲状态或强制卸载 */
    generic_shutdown_super(s);
    invalidate_buffers(s, 1, 0);
    put_super(s);

    /* 始终返回成功状态 */
    return 0;
}


void unmount_check(struct super_block *sb)
{
    //  int res;
    //  struct file_system_type *fs;
    //
    //    if(!sb)
    //        return;
    //
    //    fs = sb->s_type;
    //
    //  if(!(sb->s_flags & MS_ACTIVE) && sb->s_frozen){
    //      res = deactivate_super(sb, 0);
    //        invalidate_buffers(sb, 1, 0);
    //      if(res==0){
    //          put_super(sb);
    //        }
    //    }
}