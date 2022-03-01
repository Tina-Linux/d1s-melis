/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : file.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: file operations of vfs, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:18:23  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "fs.h"
#include "file.h"
#include "namei.h"
#include "errno.h"
#include "fstime.h"
#include "fsys_debug.h"
#include <log.h>
#include <rtthread.h>
#include <fcntl.h>

extern int  printf(const char *__restrict, ...);
extern int  nr_files;
extern struct file *do_sys_open(const char *filename, int flags, int mode, struct super_block *sb);
extern int32_t  filp_close(struct file *pfile);

typedef struct
{
    char        *s_mode;
    uint32_t    mode;
} __vfs_mode_t;

static const __vfs_mode_t _vfs_valid_modes[] =
{
    { "r",      O_RDONLY                                        },
    { "w",      O_WRONLY  |  O_CREAT  |  O_TRUNC                },
    { "w",      O_WRONLY  |  O_CREAT                            },
    { "a",      O_WRONLY  |  O_CREAT  |  O_APPEND               },
    { "rb",     O_RDONLY  |  O_BINARY                           },
    { "wb",     O_WRONLY  |  O_CREAT  |  O_TRUNC   |  O_BINARY  },
    { "wb",     O_WRONLY  |  O_CREAT  |  O_BINARY               },
    { "ab",     O_WRONLY  |  O_CREAT  |  O_APPEND  |  O_BINARY  },
    { "r+",     O_RDWR                                          },
    { "w+",     O_RDWR    |  O_CREAT  |  O_TRUNC                },
    { "w+",     O_RDWR    |  O_CREAT                            },
    { "a+",     O_RDWR    |  O_CREAT  |  O_APPEND               },
    { "r+b",    O_RDWR    |  O_BINARY                           },
    { "rb+",    O_RDWR    |  O_BINARY                           },
    { "w+b",    O_RDWR    |  O_CREAT  |  O_TRUNC   |  O_BINARY  },
    { "wb+",    O_RDWR    |  O_CREAT  |  O_TRUNC   |  O_BINARY  },
    { "w+b",    O_RDWR    |  O_CREAT  |  O_BINARY               },
    { "wb+",    O_RDWR    |  O_CREAT  |  O_BINARY               },
    { "a+b",    O_RDWR    |  O_CREAT  |  O_APPEND  |  O_BINARY  },
    { "ab+",    O_RDWR    |  O_CREAT  |  O_APPEND  |  O_BINARY  },
};
#define VFS_VALID_MODE_NUM     (sizeof(_vfs_valid_modes) / sizeof(__vfs_mode_t))

int epdk_mode_to_flag_trans(const char *p_mode, __u32 *mode)
{
    int j;

    for (j = 0; j < VFS_VALID_MODE_NUM; j++)
    {
        if (!strcmp(p_mode, _vfs_valid_modes[j].s_mode))
        {
            *mode   = _vfs_valid_modes[j].mode;
            return OK;
        }
    }
    return FAIL;
}

int epdk_flag_to_mode_trans(char **p_mode, __u32 mode)
{
    int j;

    for (j = 0; j < VFS_VALID_MODE_NUM; j++)
    {
        if (_vfs_valid_modes[j].mode == mode)
        {
            *p_mode = _vfs_valid_modes[j].s_mode;
            return 0;
        }
    }
    return -1;
}

__hdle esFSYS_fopen(const char *pFileName, const char *pMode)
{
    struct super_block  *sb;
    struct file         *retval;
    const char          *realname;
    unsigned long       cpu_sr;
    uint32_t            flags;
    int32_t             error = 0, temp_open_nr;

    if (pFileName == NULL)
    {
        __err("invalid parameter.");
        return NULL;
    }

    fs_log_trace0("fopn:%s,", pFileName);

    if (esFSYS_vfslock())
    {
        fs_log_warning("open file err when enter vfs mutex\n");
        return NULL;
    }

    /* 打开的文件个数是否已经达到上限 */
    cpu_sr  = awos_arch_lock_irq();

    temp_open_nr    = ++nr_files;

    awos_arch_unlock_irq(cpu_sr);

    if (temp_open_nr >= NR_MAXOPEN)
    {
        fs_log_warning("max file or dir handle reached!\n");
        error   = -EMFILE;
        goto dec_out;
    }

    /*  分析打开模式 */
    if (epdk_mode_to_flag_trans(pMode, &flags) == FAIL)
    {
        fs_log_warning("mode not supported!\n");
        error = -EBADRQC;
        goto dec_out;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb = path_to_sb(pFileName, &realname);
    if (!sb)
    {
        fs_log_warning("part \"%c\" not accessable!\n", *pFileName);
        error = -ENOENT;
        goto dec_out;
    }

    /* 虚拟文件系统打开操作内部流程 */
    retval  = do_sys_open(realname, flags, 0, sb);

    if (IS_ERR_OR_NULL(PTR_ERR(retval)))
    {
        error   = PTR_ERR(retval);
        goto dec_out;
    }
    unmount_check(sb);
    if (retval)
    {
        fs_log_trace0("fd:%d,", retval->f_fd);
    }

    /* 成功打开文件 */
    goto out;

dec_out:
    nr_files--;
out:
    if (error)
    {
        fs_log_warning("error(%d) meet in fopen: %s\n", error, pFileName);
        if (error != EPDK_FAIL)
        {
            fs_err  = error;
        }
        retval = NULL;
    }
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", error);
    return (__hdle)retval;
}

int32_t esFSYS_open(const char *name, int32_t flag, int32_t prems)
{
    struct super_block  *sb;
    struct file         *retval;
    const char          *realname;
    unsigned long       cpu_sr;     
    int32_t             error = 0, temp_open_nr;

    fs_log_trace0("opn:%s,", name);

    if (esFSYS_vfslock())
    {
        fs_log_warning("open file err when enter vfs mutex\n");
        return 0;
    }

    /* 打开的文件个数是否已经达到上限 */
    cpu_sr = awos_arch_lock_irq();

    temp_open_nr = ++nr_files;

    awos_arch_unlock_irq(cpu_sr);

    if (temp_open_nr >= NR_MAXOPEN)
    {
        fs_log_warning("max file or dir handle reached!\n");
        error = -EMFILE;
        goto dec_out;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb = path_to_sb(name, &realname);
    if (!sb)
    {
        fs_log_warning("part \"%c\" not accessable!\n", *name);
        error = -ENOENT;
        goto dec_out;
    }

    /* 虚拟文件系统打开操作内部流程 */
    retval = do_sys_open(realname, flag, 0, sb);

    if (IS_ERR_OR_NULL(PTR_ERR(retval)))
    {
        error   = PTR_ERR(retval);
        goto dec_out;
    }

    unmount_check(sb);

    if (retval)
    {
        fs_log_trace0("fd:%d,", retval->f_fd);
    }

    /* 成功打开文件 */
    goto out;

dec_out:
    nr_files--;
out:
    if (error)
    {
        fs_log_warning("error(%d) meet in open: %s\n", error, name);
        if (error != EPDK_FAIL)
        {
            fs_err = error;
        }
        retval = NULL;
    }
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", error);

    if (NULL == retval)
    {
        /* open failed */
        return -1;
    }
    return retval->f_fd;
}



int32_t esFSYS_fclose(__hdle hFile)
{
    int retval  = EPDK_FAIL;

    /* 关闭死文件为非法操作,
     * 当块设备被拔出时file->f_dentry将会被修改为NULL,
     * f_dentry被释放掉,此时关闭文件视为合法操作.
     * By sunny at 2010-10-11.
     */
    if (!hFile)
    {
        fs_err = EBADF;
        return EPDK_FAIL;
    }
    fs_log_trace0("fcls:%d\n", ((struct file *)hFile)->f_fd);

    if (esFSYS_vfslock())
    {
        fs_log_warning("close file err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    retval  = filp_close((struct file *)hFile);
    unmount_check(NULL);

    /* 文件数减一 */
    nr_files--;
    if (retval && retval != EPDK_FAIL)
    {
        fs_err  = retval;
        retval  = EPDK_FAIL;
    }
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", retval);
    return retval;
}

/*
 * get fd accord to the file handle.
 */
int32_t esFSYS_file2fd(__hdle hFile)
{
    struct file *pfile;

    if (NULL == hFile)
    {
        fs_log_warning("invalid parameter for file2fd\n");
        return -1;
    }
    pfile   = (struct file *)hFile;

    /* return f_fd of structure file directly */
    return pfile->f_fd;
}

/*
 * get file handle accord to fd.
 */
extern struct file *files[];
__hdle esFSYS_fd2file(__s32 fd)
{
    int i;

    for (i = 0; i < NR_MAXOPEN; i++)
    {
        if (files[i])
        {
            if (files[i]->f_fd == fd)
            {
                return (__hdle)files[i];
            }
        }
    }

    /* find failed */
    return NULL;
}

int32_t esFSYS_fsync(__hdle hFile)
{
    struct file     *file = (struct file *)hFile;
    struct dentry   *dentry;
    struct inode    *inode;
    int             err = 0;

    if (hFile == NULL)
    {
        __err("invalid parameter.");
        return EPDK_FAIL;
    }

    fs_log_trace0("fsync:%d,", file->f_fd);
    if (file->f_dentry)
    {
        fs_log_objname("%s,", file->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        fs_log_warning("fsync file err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    /* 文件不时以写的方式打开 */
    if (!(file->f_mode & FMODE_WRITE))
    {
        fs_log_warning("%s is not open by write mode.\n", file->f_dentry->d_name.name);
        err = -EACCES;
        goto out;
    }

    /* 是否是死的分区文件系统 */
    if (! file->f_dentry)
    {
        err = -EDEADLK;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    dentry  = file->f_dentry;
    inode   = dentry->d_inode;

    err     = -EINVAL;
    if (!file->f_op || !file->f_op->fsync)
    {
        goto out;
    }

    /* We need to protect against concurrent writers.. */
    err = file->f_op->fsync(file, dentry, 0);

out:
    if (err && err != EPDK_FAIL)
    {
        file->f_err = err;
        err = EPDK_FAIL;
    }

    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", err);
    return err;
}

int32_t esFSYS_remove(const char *pFileName)
{
    struct super_block  *sb;
    const char          *realname;
    int                 error = 0;

    fs_log_trace0("rmf:%s,", pFileName);

    if (esFSYS_vfslock())
    {
        fs_log_warning("rmfile err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    /* 获取由pFileName路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    sb  = path_to_sb(pFileName, &realname);

    if (!sb)
    {
        fs_log_error("part \"%c\" not accessable!\n", *pFileName);
        error = -ENOENT;
        goto out;
    }

    error = do_unlink(realname, sb);

    unmount_check(sb);

out:
    if (error && error != EPDK_FAIL)
    {
        fs_log_warning("rmfile return value:%d\n", error);
        fs_err = error;
        error = EPDK_FAIL;
    }
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", error);

    return error;
}


int32_t esFSYS_fstat(__hdle hFile, void *stat_buf)
{
    if (hFile == NULL)
    {
        __err("invalid parameter!");
        return EPDK_FAIL;
    }

    struct kstat *fstat = (struct kstat *)(stat_buf);

    if (!((struct file *)hFile)->f_dentry)
    {
        __err("dentry is null!");
        return EPDK_FAIL;
    }
    struct inode *inode = ((struct file *)hFile)->f_dentry->d_inode;

    fs_log_trace0("fstat\n");

    if (esFSYS_vfslock())
    {
        fs_log_warning("fstat err when enter vfs mutex\n");
        return EPDK_FAIL;
    }
    fstat->mode     = inode->i_mode;
    fstat->nlink    = inode->i_nlink;
    fstat->size     = inode->i_size;

    fs_timespec_to_kstamp(&(fstat->atime), inode->i_atime);
    fs_timespec_to_kstamp(&(fstat->mtime), inode->i_mtime);
    fs_timespec_to_kstamp(&(fstat->ctime), inode->i_ctime);

    fstat->blksize  = 1 << inode->i_blkbits;
    fstat->blocks   = inode->i_blocks;
    fstat->pos      = ((struct file *)hFile)->f_pos;

    esFSYS_vfsunlock();
    fs_log_trace0("O\n");
    return EPDK_OK;
}

uint32_t esFSYS_fread(void *pData, uint32_t Size, uint32_t N, __hdle hFile)
{
    int         total;
    int32_t     ret = 0;

    if (hFile == NULL)
    {
        __err("invalid parameter!");
        return 0;
    }

    fs_log_trace0("fr:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
    {
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        fs_log_warning("fread err when enter vfs mutex\n");
        return 0;
    }

    debug_timerclr(alltime);
    debug_timerstart(oneaccesstime);

    /* 是否是死文件 */
    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err  = -EDEADLK;
        ret     = 0;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    /* 文件不时以读的方式打开 */
    if (!(((struct file *)hFile)->f_mode & FMODE_READ))
    {
        fs_log_warning("%s is not open by read mode.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err   = -EACCES;
        ret = 0;
        goto out;
    }

    /* 文件句柄对应的节点无fread操作 */
    if (!((struct file *)hFile)->f_op || !((struct file *)hFile)->f_op->read)
    {
        fs_log_error("%s has no fread method.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err   = -EPERM;
        ret = 0;
        goto out;
    }

    /* 非设备文件 */
    if (((struct file *)hFile)->f_dev == 0)
    {
        /* 判断是否为0，避免除0异常 */
        total   = Size * N;
        if (!total)
        {
            ret = 0;
            fs_log_warning("0 read req. %s(fd:%d)\n", ((struct file *)hFile)->f_dentry->d_name.name, ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 读取total个字节的数据 */
        ret = ((struct file *)hFile)->f_op->read((struct file *)hFile, pData, total, &(((struct file *)hFile)->f_pos));

        fs_log_trace0("R%d,F%d,", ret, (int)(((struct file *)hFile)->f_pos) - ret);

        /* 处理返回信息 */
        if (ret < 0)/* 读取数据错误,直接返回0 */
        {
            __err("io error, ret = %d.", ret);
            ret = 0;
        }
        else if (ret == total) /* 读取数据成功 */
        {
            ret = N;
        }
        else
        {
            /* ret = ret / Size; */
            if (((struct file *)hFile)->f_pos == ((struct file *)hFile)->f_dentry->d_inode->i_size)
            {
                if (!ret)
                    fs_log_debug("EOF, %s(fd:%d).\n", ((struct file *)hFile)->f_dentry->d_name.name, ((struct file *)hFile)->f_fd);

                ((struct file *)hFile)->f_err = FSYS_ERR_EOF;
            }
        }
    }
    else /* 设备文件 */
    {
        if (!N)
        {
            ret = 0;
            fs_log_warning("0 read req. %s(fd:%d)\n", ((struct file *)hFile)->f_dentry->d_name.name, ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 读取total个字节的数据 */
        ret = ((struct file *)hFile)->f_op->read((struct file *)hFile, pData, N, (__s64 *)&Size);
        fs_log_trace0("R%dS,F%d,", ret, Size);
    }

out:
    debug_timerend(oneaccesstime);
    debug_timershow(alltime);

    esFSYS_vfsunlock();
    fs_log_trace0("O");
    return (__u32)ret;
}

uint32_t esFSYS_fwrite(const void *pData, uint32_t Size, uint32_t N, __hdle hFile)
{
    int     total;
    int     ret = 0;

    if (hFile == NULL)
    {
        __err("invalid parameter!");
        return 0;
    }

    fs_log_trace0("fw:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
    {
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        fs_log_warning("fwrite err when enter vfs mutex\n");
        return 0;
    }

    /* 文件不时以写的方式打开 */
    if (!(((struct file *)hFile)->f_mode & FMODE_WRITE))
    {
        fs_log_warning("%s is not open by write mode.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err   = -EACCES;
        ret = 0;
        goto out;
    }

    /* 是否是死的分区文件系统 */
    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err  = -EDEADLK;
        ret = 0;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    /* 文件句柄对应的节点无fwrite操作 */
    if (!((struct file *)hFile)->f_op || !((struct file *)hFile)->f_op->write)
    {
        fs_log_error("%s has no fwrite method.\n", ((struct file *)hFile)->f_dentry->d_name.name);
        ((struct file *)hFile)->f_err   = -EPERM;
        ret = 0;
        goto out;
    }

    /* 非设备文件 */
    if (((struct file *)hFile)->f_dev == 0)
    {
        /* 判断是否为0，避免除0异常 */
        total   = Size * N;
        if (!total)
        {
            ret = 0;
            fs_log_warning("0 write req. %s(fd:%d)\n", ((struct file *)hFile)->f_dentry->d_name.name, ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 写入total字节个数据 */
        ret = ((struct file *)hFile)->f_op->write(((struct file *)hFile), pData, total, &(((struct file *)hFile)->f_pos));

        fs_log_trace0("W%d,T%d,", ret, (int)((struct file *)hFile)->f_pos - ret);

        /* 处理返回信息 */
        if (ret == total)
        {
            ret = N;
        }
        else
        {
            fs_log_warning("write bytes less than you want.\n");
            ret = ret / Size;
            ((struct file *)hFile)->f_err = fs_err;
        }
    }
    else
    {
        if (!N)
        {
            ret = 0;
            fs_log_warning("0 write req. %s(fd:%d)\n", ((struct file *)hFile)->f_dentry->d_name.name, ((struct file *)hFile)->f_fd);
            goto out;
        }

        /* 读取total个字节的数据 */
        ret = ((struct file *)hFile)->f_op->write((struct file *)hFile, pData, N, (__s64 *)&Size);
        fs_log_trace0("W%dS,T%d,", ret, Size);
    }

out:
    esFSYS_vfsunlock();
    fs_log_trace0("O\n");
    return ret;
}

int64_t generic_file_llseek(struct file *file, int64_t offset, int32_t origin)
{
    long long   retval = 0;

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
        file->f_err = retval;
        __err("seek file %s error.", file->f_dentry->d_name.name);

        return -EINVAL;
    }
#ifdef FSEEK_LEGACY
    return retval;
#else
    return offset;
#endif
}

int64_t no_llseek(struct file *file, int64_t offset, int32_t origin)
{
    return -ESPIPE;
}

int64_t esFSYS_fseekex(__hdle hFile, int32_t l_off, int32_t h_off, int32_t Whence)
{
    int64_t         res;
    int64_t     Offset = (0x00000000FFFFFFFF & ((int64_t)l_off)) | (((int64_t)h_off) << 32);
    int64_t     (*fn)(struct file *, int64_t, int32_t);

    if (hFile == NULL)
    {
        __err("invalid parameter.");
        return EPDK_FAIL;
    }

    fs_log_trace0("fllsk:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
    {
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        __err("fseekex err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err  = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;
        res     = EPDK_FAIL;
        __err("file is not accessable.\n");
        goto out;
    }

    fn  = no_llseek;
    if (((struct file *)hFile)->f_op && ((struct file *)hFile)->f_op->llseek)
    {
        fn = ((struct file *)hFile)->f_op->llseek;
    }

    res = fn((struct file *)hFile, Offset, Whence);

out:
    esFSYS_vfsunlock();
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));
	if (res < 0)
    {
        __err("seek error! from %p.", __builtin_return_address(0));
    }
    return res;
}

int32_t esFSYS_fseek(__hdle hFile, int32_t Offset, int32_t Whence)
{
    int32_t     res;
    int32_t     (*fn)(struct file *, int64_t, int32_t);

    if (hFile == NULL)
    {
        __err("invalid parameter.");
        return EPDK_FAIL;
    }

    fs_log_trace0("fsk:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
    {
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        fs_log_warning("fseek err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err  = -EDEADLK;
        ((struct file *)hFile)->f_err   = fs_err;
        res     = EPDK_FAIL;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    fn  = no_llseek;
    if (((struct file *)hFile)->f_op && ((struct file *)hFile)->f_op->llseek)
    {
        fn  = ((struct file *)hFile)->f_op->llseek;
    }
    res = fn((struct file *)hFile, Offset, Whence);

out:
    esFSYS_vfsunlock();
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    if (res < 0)
    {
        __err("seek error! from %p.", __builtin_return_address(0));
    }

    return res;
}

int32_t esFSYS_ftellex(__hdle hFile, int32_t *l_pos, int32_t *h_pos)
{
    int         res;
    int64_t     x;

    if (hFile == NULL)
    {
        __err("invalid parameter!");
        return EPDK_FAIL;
    }

    fs_log_trace0("flltl:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
    {
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        fs_log_warning("ftellex err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;
        res = EPDK_FAIL;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    x = ((struct file *)hFile)->f_pos;
    *l_pos = x;
    *h_pos = x >> 32;
    res = EPDK_OK;

out:
    esFSYS_vfsunlock();
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    return res;
}

int32_t esFSYS_ftell(__hdle hFile)
{
    int64_t     res;

    if (hFile == NULL)
    {
        __err("invalid parameter!");
        return EPDK_FAIL;
    }
    fs_log_trace0("ftel:%d,", ((struct file *)hFile)->f_fd);
    if (((struct file *)hFile)->f_dentry)
    {
        fs_log_objname("%s,", ((struct file *)hFile)->f_dentry->d_name.name);
    }

    if (esFSYS_vfslock())
    {
        fs_log_warning("ftell err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    if (!((struct file *)hFile)->f_dentry)
    {
        fs_err  = -EDEADLK;
        ((struct file *)hFile)->f_err = fs_err;

        res = EPDK_FAIL;
        fs_log_warning("file is not accessable.\n");
        goto out;
    }

    res = ((struct file *)hFile)->f_pos;

out:
    esFSYS_vfsunlock();
    fs_log_trace0("O%d\n", (int)(((struct file *)hFile)->f_pos));

    return res;
}

void generic_fillattr(struct inode *inode, struct kstat *stat)
{
    if (inode == NULL)
    {
        __err("invalid parameter.");
        return;
    }

    stat->mode      = inode->i_mode;
    stat->nlink     = inode->i_nlink;

    fs_timespec_to_kstamp(&(stat->atime), inode->i_atime);
    fs_timespec_to_kstamp(&(stat->mtime), inode->i_mtime);
    fs_timespec_to_kstamp(&(stat->ctime), inode->i_ctime);

    stat->size      = inode->i_size;
    stat->blocks    = inode->i_blocks;
    stat->blksize   = (1 << inode->i_blkbits);
}

int32_t esFSYS_fioctrl(__hdle hFile, int32_t Cmd, int32_t Aux, void *pBuffer)
{
    struct inode    *ino = NULL;
    int32_t         ret;

    if (hFile == NULL)
    {
        __err("invalid parameter!");
        return EPDK_FAIL;
    }

    /* NOTE ! you can't print any information in "fioctrl",
     * because UART dirver will call "fioctrl"
     * by sunny, 2009.8.19 */
    //fs_log_trace0("fctl:%d,%d,%d,",((struct file *)hFile)->f_fd, Cmd, Aux);
    //if(((struct file *)hFile)->f_dentry)
    //    fs_log_objname("%s,",((struct file *)hFile)->f_dentry->d_name.name);

    if (((struct file *)hFile)->f_dentry && ((struct file *)hFile)->f_dentry->d_inode)
    {
        ino = ((struct file *)hFile)->f_dentry->d_inode;
    }
    else
    {
        fs_log_warning("file is not accessable.\n");
        fs_err = -EBADFD;
        ((struct file *)hFile)->f_err = fs_err;
        ret = EPDK_FAIL;
        goto out;
    }

    if (IS_FSIOCSYS(Cmd))
    {
        switch (Cmd)
        {
            case FS_IOC_SYS_GETATTR:
                generic_fillattr(ino, (struct kstat *) pBuffer);
                ret = EPDK_OK;
                break;
            default:
                fs_err = -EPERM;
                ret = EPDK_FAIL;
                break;
        }
        goto out;
    }

    if (((struct file *)hFile)->f_op && ((struct file *)hFile)->f_op->ioctl)
    {
        ret = ((struct file *)hFile)->f_op->ioctl(ino, (struct file *)hFile, Cmd, Aux, pBuffer);
    }
    else
    {
        fs_err  = -EPERM;
        ret     = EPDK_FAIL;
    }

out:

    //fs_log_trace0("O%d", ret);
    return ret;
}

int32_t esFSYS_ferror(__hdle hFile)
{
    int error;

    if (!hFile)
    {
        __err("invalid parameter!");
        return EPDK_FAIL;
    }

    if (((struct file *)hFile)->f_err > 0)
    {
        error = -(((struct file *)hFile)->f_err);
    }
    else
    {
        error = ((struct file *)hFile)->f_err;
    }
    return error;
}

void esFSYS_ferrclr(__hdle hFile)
{
}

int32_t esFSYS_rename(const char *newname, const char *oldname)
{
    struct super_block  *oldsb;
    struct super_block  *newsb;
    const char          *realnewname;
    const char          *realoldname;
    int                 error = 0;

    fs_log_trace0("rename: from %s to %s,", oldname, newname);

    if (esFSYS_vfslock())
    {
        fs_log_warning("rename err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    /* 获取由oldname路径名中的盘符指定的分区指针，并析取盘符以外的路径名 */
    oldsb   = path_to_sb(oldname, &realoldname);
    if (!oldsb)
    {
        fs_log_error("part \"%c\" not accessable!\n", *oldname);
        error = -ENOENT;
        goto out;
    }

    newsb   = path_to_sb(newname, &realnewname);
    if (!newsb)
    {
        fs_log_error("part \"%c\" not accessable!\n", *newname);
        error = -ENOENT;
        goto out;
    }

    if (newsb != oldsb)
    {
        fs_log_error("rename in different parts!\n");
        error = -ENOENT;
        goto out;
    }

    error = do_rename(realoldname, realnewname, newsb);
    unmount_check(newsb);

out:
    if (error && error != EPDK_FAIL)
    {
        fs_log_warning("rmfile return value:%d\n", error);
        fs_err = error;
        error = EPDK_FAIL;
    }
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", error);

    return error;
}

int32_t esFSYS_ftruncate(__hdle filehandle, uint32_t length)
{
    int32_t error       = EPDK_FAIL;
    struct iattr        newattr;
    struct file         *pfile  ;
    struct inode        *ino ;
    struct timespec     now;

    if (esFSYS_vfslock())
    {
        fs_log_warning("truncate err when enter vfs mutex\n");
        return EPDK_FAIL;
    }

    if (!filehandle)
    {
        fs_err  = -ENOENT;
        error   = EPDK_FAIL;
        goto out;
    }

    pfile = (struct file *)filehandle ;
    if (!pfile->f_dentry)
    {
        fs_err  = -ENOENT;
        error   = EPDK_FAIL;
        goto out;
    }

    fs_log_trace0("truncate:  %s\n",  pfile->f_dentry->d_name.name);

    ino                 = pfile->f_dentry->d_inode;
    newattr.ia_size     = length;
    //fix me, which time we should change  ?
    newattr.ia_valid    = ATTR_SIZE | ATTR_CTIME;
    now                 = current_fs_time(ino->i_sb);
    newattr.ia_ctime    = now;

    if (ino->i_op->setattr)
    {
        error = ino->i_op->setattr(pfile->f_dentry, &newattr);
        if (!error)
        {
            error   = EPDK_OK;
        }
    }
    else
    {
        error   = EPDK_FAIL;
        fs_err  = -EINVAL;
        fs_log_error("setattr is not implement!\n");
    }

out:
    esFSYS_vfsunlock();

    fs_log_trace0("O%d\n", error);

    return error;
}
