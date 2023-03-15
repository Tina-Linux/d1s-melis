#include <string.h>
#include "types.h"
#include "attrib.h"
#include "inode.h"
#include "dir.h"
#include "volume.h"
#include "mft.h"
#include "index.h"
#include "ntfstime.h"
#include "lcnalloc.h"
#include "errno.h"
#include "fsys_libs.h"
#include "fsys_libs.h"
#include "ntfs.h"

void ntfs_truncate(struct inode *inode);

static __s32 ntfs_file_open(struct inode *finode, struct file *filp)
{
    int res = 0;

    ntfs_inode *ni = NTFS_I(finode);

    if (ni->da)
    {
        return res;
    }

    ni->da = ntfs_attr_open(
                 ni,
                 AT_DATA,
                 AT_UNNAMED,
                 0);
    if (!ni->da)
    {
        fs_log_perror("bad file.");
        ntfs_inode_close(ni);
        res = -EIO;
    }

    return res;
}

static __s32 ntfs_file_read(struct file *filp, char *buf, __u32 size, __s64 *ppos)
{
    ntfs_inode *ni = NTFS_I(filp->f_dentry->d_inode);
    ntfs_attr *na = ni->da;
    int res = 0;
    unsigned int total = 0;
    s64 offset = *ppos;

    if (!size)
    {
        return 0;
    }
    if (offset + size > na->data_size)
    {
        if (na->data_size < offset)
        {
            goto ok;
        }
        size = na->data_size - offset;
    }

    while (size > 0)
    {
        s64 ret = ntfs_attr_pread(na, offset, size, buf);
        if (ret != (s64)size)
            fs_log_perror("ntfs_attr_pread error at "
                          "offset %d: %d <> %d",
                          (int)offset, (int)size, (int)ret);
        if (ret <= 0 || ret > (s64)size)
        {
            res = (ret < 0) ? -fs_err : -EIO;
            goto exit;
        }
        size -= ret;
        offset += ret;
        total += ret;
    }
ok:
#if defined CONFIG_FSYS_NTFS_RW
    ntfs_inode_update_times(ni, NTFS_UPDATE_ATIME);
#endif
    res = total;
exit:
    *ppos = offset;
    return res;
}

#if defined CONFIG_FSYS_NTFS_RW

__s64 ntfs_file_llseek(struct file *file, __s64 offset, __s32 origin)
{
    long long retval;

    if (file == NULL)
    {
        __err("invalid parameter!");
        return -EINVAL;
    }

    struct inode *inode = file->f_dentry->d_inode;

    switch (origin)
    {
        case SEEK_END:
            offset += inode->i_size;
            break;
        case SEEK_CUR:
            offset += file->f_pos;
    }
    retval = -EINVAL;
    if (offset >= 0 && offset <= inode->i_size)
    {
        if (offset != file->f_pos)
        {
            file->f_pos = offset;
        }
        retval = 0;
    }
    else
    {
        if (offset > inode->i_size)
        {
            inode->i_size = offset;
            ntfs_truncate(inode);
            file->f_pos = offset;
            mark_inode_dirty(inode);
        }
        retval = 0;
    }
#ifdef FSEEK_LEGACY
    return retval;
#else
    return offset;
#endif
}

__s32 ntfs_setattr(struct dentry *dentry, struct iattr *attr)
{
    struct inode *inode = dentry->d_inode;

    ntfs_inode *ni = NTFS_I(dentry->d_inode);
    ntfs_attr *na = ni->da;

    int error = 0;

    if (attr->ia_valid & ATTR_SIZE)
    {
        if (attr->ia_size != inode->i_size)
        {
            inode->i_size = attr->ia_size;
            ntfs_truncate(inode);
            attr->ia_valid &= ~ATTR_SIZE;
        }
    }

    /* set atime、mtime、ctime */
    if (attr->ia_valid & ATTR_ATIME)
    {
        inode->i_atime = timespec_trunc(attr->ia_atime, inode->i_sb->s_time_gran);
    }
    if (attr->ia_valid & ATTR_MTIME)
    {
        inode->i_mtime = timespec_trunc(attr->ia_mtime, inode->i_sb->s_time_gran);
    }
    if (attr->ia_valid & ATTR_CTIME)
    {
        inode->i_ctime = timespec_trunc(attr->ia_ctime, inode->i_sb->s_time_gran);
    }
    /* we have change the time ,makr inode dirty */
    mark_inode_dirty(inode);
out:
    return error;
}

static __s32 ntfs_file_write(struct file *filp, const char *buf, __u32 size, __s64 *ppos)
{
    ntfs_inode *ni = NTFS_I(filp->f_dentry->d_inode);
    ntfs_attr *na = ni->da;
    s64 offset = *ppos;
    int res, total = 0;

    while (size)
    {
        res = ntfs_attr_pwrite(na, offset, size, buf);
        if (res < (s64)size)
            fs_log_perror("ntfs_attr_pwrite partial write (%d: "
                          "%d <> %d)", (int)offset, (int)size, res);
        if (res <= 0)
        {
            res = -fs_err;
            goto exit;
        }
        size -= res;
        offset += res;
        total += res;
    }
    res = total;
    if (total > 0)
    {
        ntfs_inode_update_times(ni, NTFS_UPDATE_MTIME);
    }
exit:
    *ppos = offset;
    return res;
}

void ntfs_truncate(struct inode *inode)
{
    ntfs_inode *ni = NTFS_I(inode);

    ntfs_attr_truncate(ni->da, inode->i_size);
}
#endif  /* FSYS_NTFS_RW */

const struct file_operations ntfs_file_operations =
{
    .open       = ntfs_file_open,
    .read       = ntfs_file_read,
#if defined CONFIG_FSYS_NTFS_RW
    .llseek     = ntfs_file_llseek,
#else
    .llseek     = generic_file_llseek,
#endif
#if defined CONFIG_FSYS_NTFS_RW
    .write      = ntfs_file_write,
    .fsync      = file_fsync,
#endif
};

const struct inode_operations ntfs_file_inode_operations =
{
#if defined CONFIG_FSYS_NTFS_RW
    .setattr    = ntfs_setattr,
    .truncate   = ntfs_truncate,
#endif
};

