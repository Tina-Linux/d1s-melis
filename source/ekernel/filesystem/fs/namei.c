/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : namei.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: name lookup of vfs, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:51:36  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "limits.h"
#include "fs.h"
#include "namei.h"
#include "file.h"
#include "errno.h"
#include "slab.h"
#include "nls.h"
#include "fsys_debug.h"
#include <log.h>

int nr_files = 0;
struct file *files[NR_MAXOPEN] = {0};

extern kmem_cache_t *names_cachep;
extern kmem_cache_t *filp_cachep;

/* [Feb-1997 T. Schoebel-Theuer]
 * Fundamental changes in the pathname lookup mechanisms (namei)
 * were necessary because of omirr.  The reason is that omirr needs
 * to know the _real_ pathname, not the user-supplied one, in case
 * of symlinks (and also when transname replacements occur).
 *
 * The new code replaces the old recursive symlink resolution with
 * an iterative one (in case of non-nested symlink chains).  It does
 * this with calls to <fs>_follow_link().
 * As a side effect, dir_namei(), _namei() and follow_link() are now
 * replaced with a single function lookup_dentry() that can handle all
 * the special cases of the former code.
 *
 * With the new dcache, the pathname is stored at each inode, at least as
 * long as the refcount of the inode is positive.  As a side effect, the
 * size of the dcache depends on the inode cache and thus is dynamic.
 *
 * [29-Apr-1998 C. Scott Ananian] Updated above description of symlink
 * resolution to correspond with current state of the code.
 *
 * Note that the symlink resolution is not *completely* iterative.
 * There is still a significant amount of tail- and mid- recursion in
 * the algorithm.  Also, note that <fs>_readlink() is not used in
 * lookup_dentry(): lookup_dentry() on the result of <fs>_readlink()
 * may return different results than <fs>_follow_link().  Many virtual
 * filesystems (including /proc) exhibit this behavior.
 */

/* [24-Feb-97 T. Schoebel-Theuer] Side effects caused by new implementation:
 * New symlink semantics: when open() is called with flags O_CREAT | O_EXCL
 * and the name already exists in form of a symlink, try to create the new
 * name indicated by the symlink. The old code always complained that the
 * name already exists, due to not following the symlink even if its target
 * is nonexistent.  The new semantics affects also mknod() and link() when
 * the name is a symlink pointing to a non-existant name.
 *
 * I don't know which semantics is the right one, since I have no access
 * to standards. But I found by trial that HP-UX 9.0 has the full "new"
 * semantics implemented, while SunOS 4.1.1 and Solaris (SunOS 5.4) have the
 * "old" one. Personally, I think the new semantics is much more logical.
 * Note that "ln old new" where "new" is a symlink pointing to a non-existing
 * file does succeed in both HP-UX and SunOs, but not in Solaris
 * and in the old Linux semantics.
 */

/* [16-Dec-97 Kevin Buhr] For security reasons, we change some symlink
 * semantics.  See the comments in "open_namei" and "do_link" below.
 *
 * [10-Sep-98 Alan Modra] Another symlink change.
 */

/* [Feb-Apr 2000 AV] Complete rewrite. Rules for symlinks:
 *  inside the path - always follow.
 *  in the last component in creation/removal/renaming - never follow.
 *  if LOOKUP_FOLLOW passed - follow.
 *  if the pathname has trailing slashes - follow.
 *  otherwise - don't follow.
 * (applied in that order).
 *
 * [Jun 2000 AV] Inconsistent behaviour of open() in case if flags==O_CREAT
 * restored for 2.4. This is the last surviving part of old 4.2BSD bug.
 * During the 2.4 we need to fix the userland stuff depending on it -
 * hopefully we will be able to get rid of that wart in 2.5. So far only
 * XEmacs seems to be relying on it...
 */
/*
 * [Sep 2001 AV] Single-semaphore locking scheme (kudos to David Holland)
 * implemented.  Let's see if raised priority of ->s_vfs_rename_mutex gives
 * any extra contention...
 */

/* In order to reduce some races, while at the same time doing additional
 * checking and hopefully speeding things up, we copy filenames to the
 * kernel data space before using them..
 *
 * POSIX.1 2.4: an empty pathname is invalid (ENOENT).
 * PATH_MAX includes the nul terminator --RR.
 */
static int do_getname(const char *filename, char *page)
{
    unsigned long len = strlen(filename);

    if (len < PATH_MAX)
    {
        strncpy(page, filename, len + 1);
        return 0;
    }
    else
    {
        return -ENAMETOOLONG;
    }
}

char *getname(const char *filename)
{
    char *tmp, *result;

    result = ERR_PTR(-ENOMEM);

    /* temp use, allocate one page from page pool directly */
    /* kernal path name allocate from page pool directly,
     * not use slab allocator now.
     * By Sunny at 2010-6-12.
     */
    //  tmp = __getname();
    tmp = (char *)__get_free_page(GFP_KERNEL);
    if (tmp)
    {
        int retval = do_getname(filename, tmp);

        result = tmp;
        if (retval < 0)
        {
            //          __putname(tmp);
            free_page((unsigned long)tmp);
            result = ERR_PTR(retval);
        }
    }

    return result;
}

void putname(const char *name)
{
    /* kernal path name allocate from page pool,
     * not use slab allocator now.
     * By Sunny at 2010-6-12.
     */
    free_page((unsigned long)name);
    //  __putname(name);
}

static  void follow_dotdot(struct nameidata *nd)
{
}

/**
 * release_open_intent - free up open intent resources
 * @nd: pointer to nameidata
 */
void release_open_intent(struct nameidata *nd)
{
    if (nd->intent.open.file->f_dentry == NULL)
    {
        put_filp(nd->intent.open.file);
    }
    else
    {
        fput(nd->intent.open.file);
    }
}

static   struct dentry *do_revalidate(struct dentry *dentry, struct nameidata *nd)
{
    int status = dentry->d_op->d_revalidate(dentry, nd);
    if (status <= 0)
    {
        /*
         * The dentry failed validation.
         * If d_revalidate returned 0 attempt to invalidate
         * the dentry otherwise d_revalidate is asking us
         * to return a fail status.
         */
        if (!status)
        {
            if (!d_invalidate(dentry))
            {
                dput(dentry);
                dentry = NULL;
            }
        }
        else
        {
            dput(dentry);
            dentry = ERR_PTR(status);
        }
    }
    return dentry;
}

/*------------------------------- path walk -----------------------------*/

/*
 * Internal lookup() using the new generic dcache.
 * SMP-safe
 */
static struct dentry *cached_lookup(struct dentry *parent, struct qstr *name, struct nameidata *nd)
{
    struct dentry *dentry = __d_lookup(parent, name);

    if (dentry && dentry->d_op && dentry->d_op->d_revalidate)
    {
        dentry = do_revalidate(dentry, nd);
    }

    return dentry;
}

/*
 * This is called when everything else fails, and we actually have
 * to go to the low-level filesystem to find out what we should do..
 *
 * We get the directory semaphore, and after getting that we also
 * make sure that nobody added the entry to the dcache in the meantime..
 * SMP-safe
 */
static struct dentry *real_lookup(struct dentry *parent, struct qstr *name, struct nameidata *nd)
{
    struct dentry *result;
    struct inode *dir = parent->d_inode;
    struct dentry *dentry = d_alloc(parent, name);

    result = ERR_PTR(-ENOMEM);
    if (dentry)
    {
        result = dir->i_op->lookup(dir, dentry, nd);
        if (result)
        {
            dput(dentry);
        }
        else
        {
            result = dentry;
        }
    }
    return result;
}

/*
 *  It's more convoluted than I'd like it to be, but... it's still fairly
 *  small and for now I'd prefer to have fast path as straight as possible.
 *  It _is_ time-critical.
 */
static int do_lookup(struct nameidata *nd, struct qstr *name,
                     struct dentry **target)
{
    struct dentry *dentry = __d_lookup(nd->dentry, name);

    if (!dentry)
    {
        goto need_lookup;
    }
    if (dentry->d_op && dentry->d_op->d_revalidate)
    {
        goto need_revalidate;
    }
done:
    *target = dentry;
    return 0;

need_lookup:
    dentry = real_lookup(nd->dentry, name, nd);
    if (IS_ERR(PTR_ERR(dentry)))
    {
        goto fail;
    }
    goto done;

need_revalidate:
    dentry = do_revalidate(dentry, nd);
    if (!dentry)
    {
        goto need_lookup;
    }
    if (IS_ERR(PTR_ERR(dentry)))
    {
        goto fail;
    }
    goto done;

fail:
    return PTR_ERR(dentry);
}

/*
 * convert charset to unicode
 * return
 */
int do_char2uni(const char *in, int in_len, __wchar_t *uni)
{
    int len;
    len = nls->char2uni(in, in_len, uni);
    return len;
}

/*
 * Name resolution.
 * This is the basic name resolution function, turning a pathname into
 * the final dentry. We expect 'base' to be positive and a directory.
 *
 * Returns 0 and nd will have valid dentry and mnt on success.
 * Returns error and drops reference to input namei data on failure.
 */
static int __link_path_walk(const char *name, struct nameidata *nd)
{
    struct dentry *next;
    struct inode *inode;
    int err;
    unsigned int lookup_flags = nd->flags;
    int len, left;
    __wchar_t uni;

    len = 0;
    left = strlen(name);

    /* skip all prefix '\\' */
    while (*name == '\\')
    {
        /* we must consider wide charset condition */
        len = do_char2uni(name, left, &uni);
        if (len <= 0)
        {
            err = -EINVAL;   /* convert encount error */
            goto return_err;
        }
        if (len != 1)        /* must be ASIIC charset */
        {
            break;
        }
        left--;
        name++;
    }
    if (!*name)
    {
        goto return_reval;
    }

    inode = nd->dentry->d_inode;

    /* At this point we know we have a real path component. */
    for (;;)
    {
        unsigned long hash;
        struct qstr this;
        unsigned int c;

        nd->flags |= LOOKUP_CONTINUE;

        this.name = name;
        c = *(const unsigned char *)name;

        hash = init_name_hash();
        do
        {
            len = do_char2uni(name, left, &uni);
            if (len <= 0)
            {
                err = -EINVAL;   /* convert encount error */
                goto return_out;
            }
            left -= len;
            while (c && (len--))
            {
                name++;
                hash = partial_name_hash(c, hash);
                c = *(const unsigned char *)name;
            }
        } while (c && (c != '\\'));
        this.len = name - (const char *) this.name;
        this.hash = end_name_hash(hash);

        /* remove trailing slashes? */
        if (!c)
        {
            goto last_component;
        }

        /* skip all postfix '\\' */
        while (*name == '\\')
        {
            /* we must consider wide charset condition */
            len = do_char2uni(name, left, &uni);
            if (len <= 0)
            {
                err = -EINVAL;   /* convert encount error */
                goto return_out;
            }
            if (len != 1)        /* must be ASIIC charset */
            {
                break;
            }
            left--;
            name++;
        }
        if (!*name)
        {
            goto last_with_slashes;
        }

        /*
         * "." and ".." are special - ".." especially so because it has
         * to be able to know about the current root directory and
         * parent relationships.
         */
        if (this.name[0] == '.') switch (this.len)
            {
                default:
                    break;
                case 2:
                    if (this.name[1] != '.')
                    {
                        break;
                    }
                    follow_dotdot(nd);
                    inode = nd->dentry->d_inode;
                /* fallthrough */
                case 1:
                    continue;
            }
        /*
         * See if the low-level filesystem might want
         * to use its own hash..
         */
        if (nd->dentry->d_op && nd->dentry->d_op->d_hash)
        {
            err = nd->dentry->d_op->d_hash(nd->dentry, &this);
            if (err < 0)
            {
                break;
            }
        }
        /* This does the actual lookups.. */
        err = do_lookup(nd, &this, &next);
        if (err)
        {
            break;
        }

        err = -ENOENT;
        inode = next->d_inode;
        if (!inode)
        {
            goto out_dput;
        }
        err = -ENOTDIR;
        if (!inode->i_op)
        {
            goto out_dput;
        }

        dput(nd->dentry);
        //next->d_parent = nd->dentry;
        nd->dentry = next;

        err = -ENOTDIR;
        if (!inode->i_op->lookup)
        {
            break;
        }
        continue;
        /* here ends the main loop */

last_with_slashes:
        lookup_flags |= LOOKUP_DIRECTORY;
last_component:
        /* Clear LOOKUP_CONTINUE iff it was previously unset */
        nd->flags &= lookup_flags | ~LOOKUP_CONTINUE;
        if (lookup_flags & LOOKUP_PARENT)
        {
            goto lookup_parent;
        }
        if (this.name[0] == '.') switch (this.len)
            {
                default:
                    break;
                case 2:
                    if (this.name[1] != '.')
                    {
                        break;
                    }
                    follow_dotdot(nd);
                    inode = nd->dentry->d_inode;
                /* fallthrough */
                case 1:
                    goto return_reval;
            }
        if (nd->dentry->d_op && nd->dentry->d_op->d_hash)
        {
            err = nd->dentry->d_op->d_hash(nd->dentry, &this);
            if (err < 0)
            {
                break;
            }
        }
        err = do_lookup(nd, &this, &next);
        if (err)
        {
            break;
        }
        inode = next->d_inode;
        dput(nd->dentry);
        //next->d_parent = nd->dentry;
        nd->dentry = next;
        err = -ENOENT;
        if (!inode)
        {
            break;
        }
        if (lookup_flags & LOOKUP_DIRECTORY)
        {
            err = -ENOTDIR;
            if (!inode->i_op || !inode->i_op->lookup)
            {
                break;
            }
        }
        goto return_base;
lookup_parent:
        nd->last = this;
        nd->last_type = LAST_NORM;
        if (this.name[0] != '.')
        {
            goto return_base;
        }
        if (this.len == 1)
        {
            nd->last_type = LAST_DOT;
        }
        else if (this.len == 2 && this.name[1] == '.')
        {
            nd->last_type = LAST_DOTDOT;
        }
        else
        {
            goto return_base;
        }
return_reval:
        /*
         * We bypassed the ordinary revalidation routines.
         * We may need to check the cached dentry for staleness.
         */
        if (nd->dentry && nd->dentry->d_sb &&
            (nd->dentry->d_sb->s_type->fs_flags & FS_REVAL_DOT))
        {
            err = -ESTALE;
            /* Note: we do not d_invalidate() */
            if (!nd->dentry->d_op->d_revalidate(nd->dentry, nd))
            {
                break;
            }
        }
return_base:
        return 0;
out_dput:
        dput(next);
        break;
    }
return_out:
    dput(nd->dentry);
return_err:
    return err;
}

static int __path_lookup_intent_open(const char *name,
                                     unsigned int lookup_flags, struct nameidata *nd,
                                     int open_flags, int create_mode)
{
    struct file *filp = get_empty_filp();
    int err;

    if (filp == NULL)
    {
        return -ENFILE;
    }

    nd->intent.open.file = filp;
    nd->intent.open.flags = open_flags;
    nd->intent.open.create_mode = create_mode;
    nd->last_type = LAST_ROOT; /* if there are only slashes... */
    nd->flags = lookup_flags | LOOKUP_OPEN;
    err = __link_path_walk(name, nd);

    if (IS_ERR(PTR_ERR(nd->intent.open.file)))
    {
        if (err == 0)
        {
            err = PTR_ERR(nd->intent.open.file);
            dput(nd->dentry);
        }
    }
    else if (err != 0)
    {
        release_open_intent(nd);
    }
    return err;
}

static struct dentry *lookup_hash(struct nameidata *nd)
{
    struct dentry *dentry, *base;
    struct inode *inode;
    struct qstr *name;
    int err;

    base = nd->dentry;
    inode = base->d_inode;
    name = &nd->last;

    /*
     * See if the low-level filesystem might want
     * to use its own hash..
     */
    if (base->d_op && base->d_op->d_hash)
    {
        err = base->d_op->d_hash(base, name);
        dentry = ERR_PTR(err);
        if (err < 0)
        {
            goto out;
        }
    }

    dentry = cached_lookup(base, name, nd);
    if (!dentry)
    {
        struct dentry *new = d_alloc(base, name);
        dentry = ERR_PTR(-ENOMEM);
        if (!new)
        {
            goto out;
        }
        dentry = inode->i_op->lookup(inode, new, nd);
        if (!dentry)
        {
            dentry = new;
        }
        else
        {
            dput(new);
        }
    }
out:
    return dentry;
}

/*-------------------------- create(open) file ----------------------------*/

/*  Check whether we can create an object with dentry child in directory
 *  dir.
 *  1. We can't do it if child already exists (open has special treatment for
 *     this case, but since we are inlined it's OK)
 *  2. We can't do it if dir is read-only (done in permission())
 *  3. We should have write and exec permissions on dir
 *  4. We can't do it if dir is immutable (done in permission())
 */
static   int may_create(struct inode *dir, struct dentry *child,
                        struct nameidata *nd)
{
    if ((dir->i_sb->s_flags) & MS_RDONLY)
    {
        return -EROFS;
    }
    if (child->d_inode)
    {
        return -EEXIST;
    }
    if (IS_DEADDIR(dir))
    {
        return -ENOENT;
    }
    return 0;
}

int vfs_create(struct inode *dir, struct dentry *dentry, int mode,
               struct nameidata *nd)
{
    int error = may_create(dir, dentry, nd);

    if (error)
    {
        return error;
    }

    if (!dir->i_op || !dir->i_op->create)
    {
        return -EACCES;    /* shouldn't it be ENOSYS? */
    }
    mode &= S_IALLUGO;
    mode |= S_IFREG;
    error = dir->i_op->create(dir, dentry, mode, nd);
    return error;
}

int may_open(struct nameidata *nd, int acc_mode, int flag)
{
    struct dentry *dentry = nd->dentry;
    struct inode *inode = dentry->d_inode;

    if (!inode)
    {
        return -ENOENT;
    }

    if (!(dentry->d_sb->s_flags & MS_ACTIVE))
    {
        return -EBADSLT;
    }

    if (S_ISDIR(inode->i_mode) && (flag & FMODE_WRITE))
    {
        return -EISDIR;
    }

    if (IS_RDONLY(inode) && (flag & FMODE_WRITE))
    {
        return -EROFS;
    }
    /*
     * An append-only file must be opened in append mode for writing.
     */
    if (IS_APPEND(inode))
    {
        if ((flag & FMODE_WRITE) && !(flag & O_APPEND))
        {
            return -EPERM;
        }
        if (flag & O_TRUNC)
        {
            return -EPERM;
        }
    }

    if ((flag & O_TRUNC) && inode->i_op->truncate)
    {
        invalidate_inode_buffers(inode);

        inode->i_size = 0;
        inode->i_op->truncate(inode);
    }

    return 0;
}

static int open_namei_create(struct nameidata *nd, struct dentry *target,
                             int flag, int mode)
{
    int error;
    struct dentry *dir = nd->dentry;

    error = vfs_create(dir->d_inode, target, mode, nd);
    dput(nd->dentry);
    nd->dentry = target;
    if (error)
    {
        return error;
    }
    /* Don't check for write permission, don't truncate */
    return may_open(nd, 0, flag & ~O_TRUNC);
}

/*
 *  open_namei()
 *
 * namei for open - this is in fact almost the whole open-routine.
 *
 * Note that the low bits of "flag" aren't the same as in the open
 * system call - they are 00 - no permissions needed
 *            01 - read permission needed
 *            10 - write permission needed
 *            11 - read/write permissions needed
 * which is a lot more logical, and also allows the "no perm" needed
 * for symlinks (where the permissions are checked later).
 * SMP-safe
 */
int open_namei(const char *pathname, int flag,
               int mode, struct nameidata *nd)
{
    int error;
    struct dentry *dir, *target;
    int count = 0;
    int lookup_flag = 0;

    if (flag & O_DIRECTORY)
    {
        lookup_flag = LOOKUP_DIRECTORY;
    }

    /*
     * The simplest case - just a plain lookup.
     */
    if (!(flag & O_CREAT))
    {
        error = __path_lookup_intent_open(pathname, lookup_flag, nd,
                                          flag, 0);
        if (error)
        {
            return error;
        }
        goto ok;
    }

    /*
     * Create - we need to know the parent.
     */
    error = __path_lookup_intent_open(pathname, LOOKUP_PARENT | LOOKUP_CREATE,
                                      nd, flag, mode);

    if (error)
    {
        return error;
    }

    /*
     * We have the parent and last component. First of all, check
     * that we are not asked to creat(2) an obvious directory - that
     * will not do.
     */
    error = -EISDIR;
    if (nd->last_type != LAST_NORM || nd->last.name[nd->last.len])
    {
        goto exit;
    }

    dir = nd->dentry;
    nd->flags &= ~LOOKUP_PARENT;
    target = lookup_hash(nd);

    error = PTR_ERR(target);
    if (IS_ERR(error))
    {
        goto exit;
    }

    if (IS_ERR(PTR_ERR(nd->intent.open.file)))
    {
        error = PTR_ERR(nd->intent.open.file);
        goto exit_dput;
    }

    /* Negative dentry, just create the file */
    if (!target->d_inode)
    {
        error = open_namei_create(nd, target, flag, mode);
        if (error)
        {
            goto exit;
        }
        return 0;
    }

    /*
     * It already exists.
     */
    error = -EEXIST;
    if (flag & O_EXCL)
    {
        goto exit_dput;
    }

    dput(nd->dentry);
    nd->dentry = target;

    /*
     * Can not overwrite a directory.
     */
    error = -EISDIR;
    if (target->d_inode && S_ISDIR(target->d_inode->i_mode))
    {
        goto exit;
    }
ok:
    error = may_open(nd, 0, flag);
    if (error)
    {
        goto exit;
    }
    return 0;

exit_dput:
    dput(target);
exit:
    if (!IS_ERR(PTR_ERR(nd->intent.open.file)))
    {
        release_open_intent(nd);
    }
    dput(nd->dentry);
    return error;
}

/*--------------------------- create dir --------------------------*/

int vfs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
    int error = may_create(dir, dentry, NULL);
    if (error)
    {
        return error;
    }

    if (!dir->i_op || !dir->i_op->mkdir)
    {
        return -EPERM;
    }

    mode &= (S_IRWXUGO | S_ISVTX);
    error = dir->i_op->mkdir(dir, dentry, mode);

    return error;
}

int sys_mkdirat(const char *pathname, struct super_block *sb, int mode)
{
    int error = 0;
    char *tmp;
    struct dentry *dentry;
    struct nameidata nd;

    tmp = getname(pathname);
    error = PTR_ERR(tmp);
    if (IS_ERR(PTR_ERR(tmp)))
    {
        goto out_err;
    }

    nd.dentry = dget(sb->s_root);
    nd.last_type = LAST_ROOT; /* if there are only slashes... */
    nd.flags = LOOKUP_PARENT;
    error = __link_path_walk(tmp, &nd);
    if (error)
    {
        goto out;
    }

    /*
     * you can't mkdir like '.' or '..'
     */
    error = -EEXIST;
    if (nd.last_type != LAST_NORM)
    {
        goto out_unlock;
    }

    nd.flags &= ~LOOKUP_PARENT;
    nd.flags |= LOOKUP_CREATE;
    nd.intent.open.flags = O_EXCL;
    dentry = lookup_hash(&nd);
    if (IS_ERR(PTR_ERR(dentry)))
    {
        goto out_unlock;
    }

    error = vfs_mkdir(nd.dentry->d_inode, dentry, mode);

    dput(dentry);
out_unlock:
    dput(nd.dentry);
out:
    putname(tmp);
out_err:
    return error;
}

/*----------------------------- remove dir -------------------------*/

/*
 * It's inline, so penalty for filesystems that don't use sticky bit is
 * minimal.
 */
static   int check_sticky(struct inode *dir, struct inode *inode)
{
    return 0;
}

/*
 *  Check whether we can remove a link victim from directory dir, check
 *  whether the type of victim is right.
 *  1. We can't do it if dir is read-only (done in permission())
 *  2. We should have write and exec permissions on dir
 *  3. We can't remove anything from append-only dir
 *  4. We can't do anything with immutable dir (done in permission())
 *  5. If the sticky bit on dir is set we should either
 *  a. be owner of dir, or
 *  b. be owner of victim, or
 *  c. have CAP_FOWNER capability
 *  6. If the victim is append-only or immutable we can't do antyhing with
 *     links pointing to it.
 *  7. If we were asked to remove a directory and victim isn't one - ENOTDIR.
 *  8. If we were asked to remove a non-directory and victim isn't one - EISDIR.
 *  9. We can't remove a root or mountpoint.
 * 10. We don't allow removal of NFS sillyrenamed files; it's handled by
 *     nfs_async_unlink().
 */
static int may_delete(struct inode *dir, struct dentry *victim, int isdir)
{
    int error;

    if (!victim->d_inode)
    {
        return -ENOENT;
    }

    BUG_ON(victim->d_parent->d_inode != dir);

    if ((dir->i_sb->s_flags) & MS_RDONLY)
    {
        return -EROFS;
    }
    if (atomic_read(&(victim->d_count)) > 1)
    {
        return -EBUSY;
    }
    if (IS_APPEND(dir))
    {
        return -EPERM;
    }
    if (check_sticky(dir, victim->d_inode) || IS_APPEND(victim->d_inode) ||
        IS_IMMUTABLE(victim->d_inode))
    {
        return -EPERM;
    }
    if (isdir)
    {
        if (!S_ISDIR(victim->d_inode->i_mode))
        {
            return -ENOTDIR;
        }
    }
    else if (S_ISDIR(victim->d_inode->i_mode))
    {
        return -EISDIR;
    }
    if (IS_DEADDIR(dir))
    {
        return -ENOENT;
    }
    if (victim->d_flags & DCACHE_NFSFS_RENAMED)
    {
        return -EBUSY;
    }
    return 0;
}


/*
 * We try to drop the dentry early: we should have
 * a usage count of 2 if we're the only user of this
 * dentry, and if that is true (possibly after pruning
 * the dcache), then we drop the dentry now.
 *
 * A low-level filesystem can, if it choses, legally
 * do a
 *
 *  if (!d_unhashed(dentry))
 *      return -EBUSY;
 *
 * if it cannot handle the case of removing a directory
 * that is still in use by something else..
 */
void dentry_unhash(struct dentry *dentry)
{
    dget(dentry);
    if (atomic_read(&dentry->d_count) == 2)
    {
        __d_drop(dentry);
    }
}

int vfs_rmdir(struct inode *dir, struct dentry *dentry)
{
    int error = may_delete(dir, dentry, 1);

    if (error)
    {
        return error;
    }

    if (!dir->i_op || !dir->i_op->rmdir)
    {
        return -EPERM;
    }

    dentry_unhash(dentry);

    error = dir->i_op->rmdir(dir, dentry);
    if (!error)
    {
        dentry->d_inode->i_flags |= S_DEAD;
    }

    if (!error)
    {
        d_delete(dentry);
    }
    dput(dentry);

    return error;
}

int do_rmdir(const char *pathname, struct super_block *sb)
{
    int error = 0;
    char *name;
    struct dentry *dentry;
    struct nameidata nd;

    name = getname(pathname);
    if (IS_ERR(PTR_ERR(name)))
    {
        return PTR_ERR(name);
    }

    nd.dentry = dget(sb->s_root);
    nd.last_type = LAST_ROOT; /* if there are only slashes... */
    nd.flags = LOOKUP_PARENT;
    error = __link_path_walk(name, &nd);

    if (error)
    {
        goto exit;
    }

    switch (nd.last_type)
    {
        case LAST_DOTDOT:
            error = -ENOTEMPTY;
            goto exit1;
        case LAST_DOT:
            error = -EINVAL;
            goto exit1;
        case LAST_ROOT:
            error = -EBUSY;
            goto exit1;
    }
    dentry = lookup_hash(&nd);
    error = PTR_ERR(dentry);
    if (IS_ERR(PTR_ERR(dentry)))
    {
        goto exit2;
    }
    error = vfs_rmdir(nd.dentry->d_inode, dentry);
    dput(dentry);
exit2:
exit1:
    dput(nd.dentry);
exit:
    putname(name);
    return error;
}


void path_release(struct nameidata *nd)
{
    dput(nd->dentry);
}


/*
 * The worst of all namespace operations - renaming directory. "Perverted"
 * doesn't even start to describe it. Somebody in UCB had a heck of a trip...
 * Problems:
 *  a) we can get into loop creation. Check is done in is_subdir().
 *  b) race potential - two innocent renames can create a loop together.
 *     That's where 4.4 screws up. Current fix: serialization on
 *     sb->s_vfs_rename_sem. We might be more accurate, but that's another
 *     story.
 *  c) we have to lock _three_ objects - parents and victim (if it exists).
 *     And that - after we got ->i_sem on parents (until then we don't know
 *     whether the target exists at all, let alone whether it is a directory
 *     or not). Solution: ->i_zombie. Taken only after ->i_sem. Always taken
 *     on link creation/removal of any kind. And taken (without ->i_sem) on
 *     directory that will be removed (both in rmdir() and here).
 *  d) some filesystems don't support opened-but-unlinked directories,
 *     either because of layout or because they are not ready to deal with
 *     all cases correctly. The latter will be fixed (taking this sort of
 *     stuff into VFS), but the former is not going away. Solution: the same
 *     trick as in rmdir().
 *  e) conversion from fhandle to dentry may come in the wrong moment - when
 *     we are removing the target. Solution: we will have to grab ->i_zombie
 *     in the fhandle_to_dentry code. [FIXME - current nfsfh.c relies on
 *     ->i_sem on parents, which works but leads to some truely excessive
 *     locking].
 */
int vfs_rename_dir(struct inode *old_dir, struct dentry *old_dentry,
                   struct inode *new_dir, struct dentry *new_dentry)
{
    int error;
    struct inode *target;

    if (old_dentry->d_inode == new_dentry->d_inode)
    {
        return 0;
    }

    error = may_delete(old_dir, old_dentry, 1);
    if (error)
    {
        return error;
    }

    if (!new_dentry->d_inode)
    {
        error = may_create(new_dir, new_dentry, NULL);
    }
    else
    {
        error = may_delete(new_dir, new_dentry, 1);
    }
    if (error)
    {
        return error;
    }

    if (!old_dir->i_op || !old_dir->i_op->rename)
    {
        return -EPERM;
    }

    error = -EINVAL;

    target = new_dentry->d_inode;
    if (IS_DEADDIR(old_dir) || IS_DEADDIR(new_dir))
    {
        error = -ENOENT;
    }
    else
    {
        error = old_dir->i_op->rename(old_dir, old_dentry, new_dir, new_dentry);
    }
    if (target)
    {
        if (!error)
        {
            target->i_flags |= S_DEAD;
        }
        if (d_unhashed(new_dentry))
        {
            d_rehash(new_dentry);
        }
        dput(new_dentry);
    }
    if (!error)
    {
        d_move(old_dentry, new_dentry);
    }

    return error;
}

int vfs_rename_other(struct inode *old_dir, struct dentry *old_dentry,
                     struct inode *new_dir, struct dentry *new_dentry)
{
    int error;

    if (old_dentry->d_inode == new_dentry->d_inode)
    {
        return 0;
    }

    error = may_delete(old_dir, old_dentry, 0);
    if (error)
    {
        return error;
    }

    if (!new_dentry->d_inode)
    {
        error = may_create(new_dir, new_dentry, NULL);
    }
    else
    {
        error = may_delete(new_dir, new_dentry, 0);
    }
    if (error)
    {
        return error;
    }

    if (!old_dir->i_op || !old_dir->i_op->rename)
    {
        return -EPERM;
    }

    error = old_dir->i_op->rename(old_dir, old_dentry, new_dir, new_dentry);
    if (error)
    {
        return error;
    }
    /* The following d_move() should become unconditional */
    d_move(old_dentry, new_dentry);
    return 0;
}


int vfs_rename(struct inode *old_dir, struct dentry *old_dentry,
               struct inode *new_dir, struct dentry *new_dentry)
{
    int error;
    if (S_ISDIR(old_dentry->d_inode->i_mode))
    {
        error = vfs_rename_dir(old_dir, old_dentry, new_dir, new_dentry);
    }
    else
    {
        error = vfs_rename_other(old_dir, old_dentry, new_dir, new_dentry);
    }
    return error;
}

int __do_rename(const char *oldname, const char *newname, struct super_block *sb)
{
    int error = 0;
    struct dentry *old_dir, * new_dir;
    struct dentry *old_dentry, *new_dentry;
    struct nameidata oldnd, newnd;

    oldnd.dentry = dget(sb->s_root);
    oldnd.last_type = LAST_ROOT; /* if there are only slashes... */
    oldnd.flags = LOOKUP_PARENT;
    error = __link_path_walk(oldname, &oldnd);
    if (error)
    {
        goto exit;
    }

    newnd.dentry = dget(sb->s_root);
    newnd.last_type = LAST_ROOT; /* if there are only slashes... */
    newnd.flags = LOOKUP_PARENT;
    error = __link_path_walk(newname, &newnd);
    if (error)
    {
        goto exit1;
    }

    old_dir = oldnd.dentry;
    error = -EBUSY;
    if (oldnd.last_type != LAST_NORM)
    {
        goto exit2;
    }

    new_dir = newnd.dentry;
    if (newnd.last_type != LAST_NORM)
    {
        goto exit2;
    }

    old_dentry = lookup_hash(&oldnd);
    error = PTR_ERR(old_dentry);
    if (IS_ERR(PTR_ERR(old_dentry)))
    {
        goto exit3;
    }
    /* source must exist */
    error = -ENOENT;
    if (!old_dentry->d_inode)
    {
        goto exit4;
    }
    /* unless the source is a directory trailing slashes give -ENOTDIR */
    if (!S_ISDIR(old_dentry->d_inode->i_mode))
    {
        error = -ENOTDIR;
        if (oldnd.last.name[oldnd.last.len])
        {
            goto exit4;
        }
        if (newnd.last.name[newnd.last.len])
        {
            goto exit4;
        }
    }
    new_dentry = lookup_hash(&newnd);
    error = PTR_ERR(new_dentry);
    if (IS_ERR(PTR_ERR(new_dentry)))
    {
        goto exit4;
    }

    error = vfs_rename(old_dir->d_inode, old_dentry,
                       new_dir->d_inode, new_dentry);
    dput(new_dentry);
exit4:
    dput(old_dentry);
exit3:
exit2:
    path_release(&newnd);
exit1:
    path_release(&oldnd);
exit:
    return error;
}

int do_rename(const char *oldname, const char *newname, struct super_block *sb)
{
    int error;
    char *from;
    char *to;

    from = getname(oldname);
    if (IS_ERR(PTR_ERR(from)))
    {
        return PTR_ERR(from);
    }
    to = getname(newname);
    error = PTR_ERR(to);
    if (!IS_ERR(PTR_ERR(to)))
    {
        error = __do_rename(from, to, sb);
        putname(to);
    }
    putname(from);
    return error;
}

/*-----------------------------remove file -------------------------*/
int vfs_unlink(struct inode *dir, struct dentry *dentry)
{
    int error = may_delete(dir, dentry, 0);

    if (error)
    {
        return error;
    }

    if (!dir->i_op || !dir->i_op->unlink)
    {
        return -EPERM;
    }

    error = dir->i_op->unlink(dir, dentry);

    /* We don't d_delete() NFS sillyrenamed files--they still exist. */
    if (!error && !(dentry->d_flags & DCACHE_NFSFS_RENAMED))
    {
        d_delete(dentry);
    }

    return error;
}


/*
 * Make sure that the actual truncation of the file will occur outside its
 * directory's i_mutex.  Truncate can take a long time if there is a lot of
 * writeout happening, and we don't want to prevent access to the directory
 * while waiting on the I/O.
 */
int do_unlink(const char *pathname, struct super_block *sb)
{
    int error = 0;
    char *name;
    struct dentry *dentry;
    struct nameidata nd;
    struct inode *inode = NULL;

    name = getname(pathname);
    if (IS_ERR(PTR_ERR(name)))
    {
        return PTR_ERR(name);
    }

    nd.dentry = dget(sb->s_root);
    nd.last_type = LAST_ROOT; /* if there are only slashes... */
    nd.flags = LOOKUP_PARENT;
    error = __link_path_walk(name, &nd);
    if (error)
    {
        goto exit;
    }

    error = -EISDIR;
    if (nd.last_type != LAST_NORM)
    {
        goto exit1;
    }
    dentry = lookup_hash(&nd);
    error = PTR_ERR(dentry);
    if (!IS_ERR(PTR_ERR(dentry)))
    {
        /* Why not before? Because we want correct error value */
        if (nd.last.name[nd.last.len])
        {
            goto slashes;
        }
        inode = dentry->d_inode;
        if (inode)
        {
            atomic_inc(&inode->i_count);
        }
        error = vfs_unlink(nd.dentry->d_inode, dentry);
exit2:
        dput(dentry);
    }
    if (inode)
    {
        iput(inode);    /* truncate the inode here */
    }
exit1:
    dput(nd.dentry);
exit:
    fsync_dev(sb);
    putname(name);
    return error;

slashes:
    error = !dentry->d_inode ? -ENOENT : S_ISDIR(dentry->d_inode->i_mode) ? -EISDIR : -ENOTDIR;

    goto exit2;
}

static struct file *__dentry_open(struct dentry *dentry,
                                  int flags, struct file *f,
                                  __s32(*open)(struct inode *, struct file *))
{
    struct inode *inode;
    int error;
    int i;

    f->f_flags  = flags;
    f->f_mode   = ((flags + 1) & O_ACCMODE) | FMODE_LSEEK | FMODE_PREAD | FMODE_PWRITE;
    inode       = dentry->d_inode;

    f->f_dentry = dentry;
    f->f_pos    = flags & O_APPEND ? inode->i_size : 0;
    f->f_op     = inode->i_fop;

    if (!open && f->f_op)
    {
        open = f->f_op->open;
    }
    if (open)
    {
        error   = open(inode, f);
        if (error)
        {
            goto cleanup_all;
        }
    }

    f->f_flags &= ~(O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC);

    for (i = 0; i < NR_MAXOPEN; i++)
    {
        if (files[i] == 0)
        {
            files[i] = f;
            f->f_fd = i;
            break;
        }
    }
    if (i == NR_MAXOPEN)
    {
        int jj = 0;
        __err("file count > NR_MAXOPEN!,open fail!\n");
        for(jj=0;jj<NR_MAXOPEN;jj++)
        {
            __err("[%d] name:%c:\\%s\n",jj,files[jj]->f_dentry->d_sb->s_letter,files[jj]->f_dentry->d_iname);
        }

        error = -EMFILE;
        goto cleanup_all;
    }

    return f;

cleanup_all:
    f->f_dentry = NULL;

    put_filp(f);
    dput(dentry);
    return ERR_PTR(error);
}

/**
 * nameidata_to_filp - convert a nameidata to an open filp.
 * @nd: pointer to nameidata
 * @flags: open flags
 *
 * Note that this function destroys the original nameidata
 */
struct file *nameidata_to_filp(struct nameidata *nd, int flags)
{
    struct file *filp;

    /* Pick up the filp from the open intent */
    filp = nd->intent.open.file;
    /* Has the filesystem initialised the file for us? */
    if (filp->f_dentry == NULL)
    {
        filp = __dentry_open(nd->dentry, flags, filp, NULL);
    }
    else
    {
        dput(nd->dentry);
    }
    return filp;
}

struct file *do_sys_open(const char *filename, int flags, int mode, struct super_block *sb)
{
    struct nameidata nd;
    struct file *retval = NULL;
    int ret;
    char *tmp;
    int namei_flags;

    /* 在文件系统内部分配文件名空间并复制用户空间传来的文件名字符串 */
    tmp = getname(filename);
    if (IS_ERR(PTR_ERR(tmp)))
    {
        return (struct file *)ERR_PTR(PTR_ERR(tmp));
    }

    namei_flags = flags;
    if ((namei_flags + 1) & O_ACCMODE)
    {
        namei_flags++;
    }

    /* 始终从根目录开始 */
    nd.dentry = dget(sb->s_root);

    /* 对每级目录进行打开遍历 */
    ret = open_namei(tmp, namei_flags, mode, &nd);
    if (!ret)  /* == EPDK_OK */
    {
        retval = nameidata_to_filp(&nd, flags);
    }

    /* 释放文件名空间 */
    putname(tmp);

    return retval;
}

__s32 filp_close(struct file *pfile)
{
    int retval = 0;

    /* flush文件的所有缓存 */
    if (pfile->f_op && pfile->f_op->flush)
    {
        retval = pfile->f_op->flush(pfile);
    }
    if (retval)
    {
        goto out;
    }

    /* 文件系统close */
    if (pfile->f_op && pfile->f_op->close)
    {
        retval = pfile->f_op->close(pfile);
    }
    if (retval)
    {
        goto out;
    }

    /* 安全起见，回写磁盘上的所有缓存(暂时) */
    if (pfile->f_dentry && pfile->f_dentry->d_sb)
    {
        retval = fsync_dev(pfile->f_dentry->d_sb);
    }

    /* 销毁struct file数据结构，并递减dentry的引用计数 */
    fput(pfile);

out:
    return retval;
}

static void file_free(struct file *f)
{
    kmem_cache_free(filp_cachep, f);
}


/* Find an unused file structure and return a pointer to it.
 * Returns NULL, if there are no more free file structures or
 * we run out of memory.
 */
struct file *get_empty_filp(void)
{
    struct file *f;

    f = kmem_cache_alloc(filp_cachep, GFP_KERNEL);
    if (f == NULL)
    {
        goto fail;
    }

    memset(f, 0, sizeof(*f));

    atomic_set(&f->f_count, 1);

    return f;

fail:
    return NULL;
}

void fput(struct file *file)
{
    struct dentry *dentry = file->f_dentry;

    /* dentry maybe NULL,
     * By sunny, 2010.3.3
     */
    //  struct inode *inode = dentry->d_inode;
    struct inode *inode = NULL;

    //  might_sleep();
    if (file->f_op && file->f_op->release && dentry)
    {
        inode = dentry->d_inode;
        file->f_op->release(inode, file);
    }

    /* 标记file为死节点 */
    file->f_dentry = NULL;
    files[file->f_fd] = NULL;
    file_free(file);
    dput(dentry);
}

void fget(struct file *file)
{
    atomic_inc(&file->f_count);
}

void put_filp(struct file *file)
{
    if (atomic_dec_and_test(&file->f_count))
    {
        file_free(file);
    }
}

/* -------------------------------- debug -------------------------*/
#if     FSYS_DEBUG_ON
int __debug_dump_openfiles(void)
{
    int i;

    __log("Open files number: %d (Max:%d)", nr_files, NR_MAXOPEN);

    for (i = 0; i < NR_MAXOPEN; i++)
    {
        if (files[i])
        {
            struct dentry *de;

            __log("   fd: %d  ", i);
            if (de = files[i]->f_dentry)
            {
                if (S_ISDIR(de->d_inode->i_mode))
                {
                    __log("d ");
                }
                else
                {
                    __log("f ");
                }

                __log("%s          [%c: %dbytes]", de->d_name.name, de->d_sb->s_letter, (int)de->d_inode->i_size);
            }
            else
            {
                __log("- [on plugout media]");
            }
        }
    }
    return 0;
}
#endif  /* FSYS_DEBUG_ON */
