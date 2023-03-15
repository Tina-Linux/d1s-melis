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
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <kapi.h>
#include <syscall.h>
#include <aw_dirent.h>

int dup(int fd)
{
    int result = 0;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(dup), (long)fd);
    return result;
}

int dup2(int oldfd, int newfd)
{
    int result = 0;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(dup2), (long)oldfd, (long)newfd);
    return result;
}

int _close_r(struct _reent *ptr, int fd)
{
    int  result = 0;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(_close_r), (unsigned long)ptr, (long)fd);
    return result;
}

int closedir(DIR *d)
{
    int result = 0;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(closedir), (long)d);
    return result;
}

int fioctrl(FILE *fp, int Cmd, void *args)
{
    int  result = 0;
    result = (int)__syscall3((long)SYSCALL_NLIBOPS(fioctrl), (long)fp, (long)Cmd, (long)args);
    return result;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    int result;
    result = (int)__syscall3((long)SYSCALL_NLIBOPS(_fstat_r), (unsigned long)ptr, (unsigned long)fd, (unsigned long)pstat);
    return result;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    int result;
    result = (int)__syscall4((long)SYSCALL_NLIBOPS(_lseek_r), (unsigned long)ptr, (unsigned long)fd, (unsigned long)pos, (unsigned long)whence);
    return result;
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
    int result;
    result = (int)__syscall3((long)SYSCALL_NLIBOPS(_rename_r), (unsigned long)ptr, (unsigned long)old, (unsigned long)new);
    return result;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    int result;
    result = (int)__syscall3((long)SYSCALL_NLIBOPS(_stat_r), (unsigned long)ptr, (unsigned long)file, (unsigned long)pstat);
    return result;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    int result;
    result = (int)__syscall3((long)SYSCALL_NLIBOPS(_mkdir_r), (unsigned long)ptr, (unsigned long)name, (unsigned long)mode);
    return result;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    int result;
    result = (int)__syscall4((long)SYSCALL_NLIBOPS(_open_r), (unsigned long)ptr, (unsigned long)file, (unsigned long)flags, (unsigned long)mode);
    return result;
}

int fsync(int fildes)
{
    int result = 0;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(fsync), (long)fildes);
    return result;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    int result;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(_unlink_r), (unsigned long)ptr, (unsigned long)file);
    return result;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    _ssize_t result;
    result = (_ssize_t)__syscall4((long)SYSCALL_NLIBOPS(_read_r), (unsigned long)ptr, (unsigned long)fd, (unsigned long)buf, (unsigned long)nbytes);
    return result;
}

_ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t nbytes)
{
    _ssize_t result;
    result = (_ssize_t)__syscall4((long)SYSCALL_NLIBOPS(_write_r), (unsigned long)reent, (unsigned long)fd, (unsigned long)buf, (unsigned long)nbytes);
    return result;
}

int _gettimeofday_r(struct _reent *ptr, struct timeval *tv, void *__tzp)
{
    int result;
    result = (int)__syscall3((long)SYSCALL_NLIBOPS(_gettimeofday_r), (unsigned long)ptr, (unsigned long)tv, (unsigned long)__tzp);
    return result;
}

int settimeofday(const struct timeval *tm_val, const struct timezone *tz)
{
    int result;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(settimeofday), (unsigned long)tm_val, (unsigned long)tz);
    return result;
}

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void *result;
    result = (void *)__syscall2((long)SYSCALL_NLIBOPS(_malloc_r), (unsigned long)ptr, (unsigned long)size);
    return result;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void *result;
    result = (void *)__syscall3((long)SYSCALL_NLIBOPS(_realloc_r), (unsigned long)ptr, (unsigned long)old, (unsigned long)newlen);
    return result;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void *result;
    result = (void *)__syscall3((long)SYSCALL_NLIBOPS(_calloc_r), (unsigned long)ptr, (unsigned long)size, (unsigned long)len);
    return result;
}

void _free_r(struct _reent *ptr, void *addr)
{
    void *result;
    result = (void *)__syscall2((long)SYSCALL_NLIBOPS(_free_r), (unsigned long)ptr, (unsigned long)addr);
}

void _isatty_r(struct _reent *ptr, void *addr)
{
    void *result;
    result = (void *)__syscall2((long)SYSCALL_NLIBOPS(_isatty_r), (unsigned long)ptr, (unsigned long)addr);
}

void _isatty(int fd)
{
    void *result;
    result = (void *)__syscall1((long)SYSCALL_NLIBOPS(_isatty), (unsigned long)fd);
}

void __libc_init_array(void)
{
    void *result;
    result = (void *)__syscall0((long)SYSCALL_NLIBOPS(__libc_init_array));
}

#if 0
void _exit(int status)
{
    int result;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(_exit), (unsigned long)status);
}
#endif

void _system(const char *s)
{
    int result;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(_system), (unsigned long)s);
}

void __malloc_lock(struct _reent *ptr)
{
}

void __malloc_unlock(struct _reent *ptr)
{
}

int ftruncate(int fd, off_t length)
{
    int result = 0;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(ftruncate), (long)fd, (long)length);
    return result;
}

int ioctl(int fildes, int cmd, ...)
{
    int result = 0;
    void *arg;
    va_list ap;

    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);

    result = (int)__syscall3((long)SYSCALL_NLIBOPS(ioctl), (long)fildes, (long)cmd, (long)arg);

    return result;
}

int mkdir(const char *path, mode_t mode)
{
    int result = 0;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(mkdir), (long)path, (long)mode);
    return result;
}

DIR *opendir(const char *name)
{
    DIR *dir = NULL;
    dir = (DIR *)__syscall1((long)SYSCALL_NLIBOPS(opendir), (long)name);
    return dir;
}

struct dirent *readdir(DIR *d)
{
    dirent_s *dir = NULL;
    dir = (dirent_s *)__syscall1((long)SYSCALL_NLIBOPS(readdir), (long)d);
    return dir;
}

int remove(const char *pathname)
{
    int result = 0;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(remove), (long)pathname);
    return result;
}

void rewinddir(DIR *d)
{
    __syscall1((long)SYSCALL_NLIBOPS(rewinddir), (long)d);
    return;
}

int rmdir(const char *pathname)
{
    int result = 0;
    result = (int)__syscall1((long)SYSCALL_NLIBOPS(rmdir), (long)pathname);
    return result;
}

int statfs(const char *path, struct statfs *buf)
{
    int result = 0;
    result = (int)__syscall2((long)SYSCALL_NLIBOPS(statfs), (long)path, (long)buf);
    return result;
}
