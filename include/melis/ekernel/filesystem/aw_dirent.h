#ifndef __RTT_DIRENT_H__
#define __RTT_DIRENT_H__

/*
* dirent.h - format of directory entries
 * Ref: http://www.opengroup.org/onlinepubs/009695399/basedefs/dirent.h.html
 */

/* File types */
#define FT_REGULAR      0   /* regular file */
#define FT_SOCKET       1   /* socket file  */
#define FT_DIRECTORY    2   /* directory    */
#define FT_USER         3   /* user defined */

#define DT_UNKNOWN      0x00
#define DT_REG          0x01
#define DT_DIR          0x02

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_DIR_STRUCTURE
typedef struct
{
    int     fd;         /* directory file */
    char    buf[512];
    int     num;
    int     cur;
} DIR;
#endif

#ifndef HAVE_DIRENT_STRUCTURE
struct dirent
{
    uint8_t     d_type;         /* The type of the file */
    uint8_t     d_namlen;       /* The length of the not including the terminating null file name */
    uint16_t    d_reclen;       /* length of this record */
    char        d_name[256];    /* The null-terminated file name */
};
typedef struct dirent dirent_s;
#endif

struct statfs
{
    size_t      f_bsize;    /* block size */
    size_t      f_blocks;   /* total data blocks in file system */
    size_t      f_bfree;    /* free blocks in file system */
};


int             closedir(DIR *);
int             fsync(int fildes);
int             ftruncate(int fd, off_t length);
int             mkdir(const char *path, mode_t mode);
DIR             *opendir(const char *);
struct dirent   *readdir(DIR *);
int             readdir_r(DIR *, struct dirent *, struct dirent **);
void            rewinddir(DIR *);
int             rmdir(const char *pathname);
void            seekdir(DIR *, long int);
long            telldir(DIR *);
int             statfs(const char *path, struct statfs *buf);
//int             stat(const char *file, struct stat *buf);


#ifdef __cplusplus
}
#endif

#endif
