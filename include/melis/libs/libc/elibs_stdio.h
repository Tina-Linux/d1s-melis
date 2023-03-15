/*
*********************************************************************************************************
*                                                   ePDK
*                                  the Easy Portable/Player Development Kits
*                                              elibs sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : stdio.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _ELIBS_STDIO_H_
#define _ELIBS_STDIO_H_

#include <typedef.h>
#include <kapi.h>
#ifdef __cplusplus
extern "C" {
#endif

#define SEEK_SET 0 /* start of stream (see fseek) */
#define SEEK_CUR 1 /* current position in stream (see fseek) */
#define SEEK_END 2 /* end of stream (see fseek) */
#define ES_EOF  -1


#define ELIBS_SEEK_SET 0                            /* start of stream (see fseek)                                  */
#define ELIBS_SEEK_CUR 1                            /* current position in stream (see fseek)                       */
#define ELIBS_SEEK_END 2                            /* end of stream (see fseek)                                    */

#define ELIBS_PART_FMT_UNKONWERR    -3   /* unknow error */
#define ELIBS_PART_FMT_NOACCES      -2   /* part not aceesable, maybe opened by usbd */
#define ELIBS_PART_FMT_NOEXIST      -1   /* part not exist */
#define ELIBS_PART_FMT_NFORMATED     0   /* not format */
#define ELIBS_PART_FMT_FORMATED      1   /* formated */

/* fs attributes */
#define FS_ATTR_READONLY      0x00000001 /* readonly fs */

/* file attributes */
#define FSYS_ATTR_READONLY    0x00000001
#define FSYS_ATTR_HIDDEN      0x00000002
#define FSYS_ATTR_SYSTEM      0x00000004
#define FSYS_FAT_VOLUME_ID    0x00000008
#define FSYS_ATTR_DIRECTORY   0x00000010
#define FSYS_ATTR_ARCHIVE     0x00000020

#define FSYS_FAT_DENTRY_SIZE      0x20

#define MAX_VOLUME_NAME_LEN       512
#define MAX_FS_NAME_LEN           16
#define MAX_DISK_LETTER_LEN       26
#define MAX_DIR_SCAN_LEVEL        32

typedef struct __ESFILE
{
    __u32   tmp;
} ES_FILE;

typedef struct __ESDIR
{
    __u32   tmp;
} ES_DIR;
typedef struct __ESDIRENT
{
    __u32     d_ino;              /* to be POSIX conform                                  */
    __u8      fatdirattr;         /* FAT only. Contains the "DIR_Attr" field of an entry. */
    __u64     d_size;             /* file size, if directory it will be 0                 */
    __u8      d_name[FSYS_DIRNAME_MAX];
} ES_DIRENT;

typedef enum
{
    T_FAT12   = 0x0001,
    T_FAT16   = 0x0002,
    T_FAT32   = 0x0003,
    T_NTFS    = 0x0004
} FS_TYPE;

typedef struct
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
} ES_FMTPARA;

typedef struct
{
    __u16       year;
    __u8        month;
    __u8        day;
    __u8        hour;
    __u8        minute;
    __u8        second;
    __u8        milliseconds;
} ES_TIMESTAMP;

typedef struct
{
    unsigned short  mode;
    unsigned int    nlink;
    long long       size;
    long long       pos;
    ES_TIMESTAMP    atime;
    ES_TIMESTAMP    mtime;
    ES_TIMESTAMP    ctime;
    unsigned long   blksize;
    unsigned long long  blocks;
} __ES_FSTAT;

typedef struct
{
    __u32 f_type;               /* fs type */
    __u32 f_bsize;              /* block size */
    __u32 f_blocks;             /* blocks number */
    __u32 f_bfree;              /* free blocks number */
    __u32 f_files;              /* exist files number */
    __u32 f_namelen;            /* max name lenght */

    __u32 f_attribute;          /* fs attribute like readonly */
    __u32 f_mediaattr;          /* media attribute like movable */
    __u8  f_volletter;          /* like 'c' or 'd' etc */
    __u8  f_volname[MAX_VOLUME_NAME_LEN];  /* volume name */
    __u8  f_fsname[MAX_FS_NAME_LEN];       /* the bind file system name */
} __ES_FSSTAT;

extern int eLIBs_remove(const char * /*ES_FILEname*/);
/*
 * causes the ES_FILE whose name is the string pointed to by ES_FILEname to be
 * removed. Subsequent attempts to open the ES_FILE will fail, unless it is
 * created anew. If the ES_FILE is open, the behaviour of the remove function
 * is implementation-defined.
 * Returns: zero if the operation succeeds, nonzero if it fails.
 */
extern int eLIBs_rename(const char * /*old*/, const char * /*new*/);
/*
 * causes the ES_FILE whose name is the string pointed to by old to be
 * henceforth known by the name given by the string pointed to by new. The
 * ES_FILE named old is effectively removed. If a ES_FILE named by the string
 * pointed to by new exists prior to the call of the rename function, the
 * behaviour is implementation-defined.
 * Returns: zero if the operation succeeds, nonzero if it fails, in which
 *          case if the ES_FILE existed previously it is still known by its
 *          original name.
 */
extern int eLIBs_fclose(ES_FILE * /*stream*/);
/*
 * causes the stream pointed to by stream to be flushed and the associated
 * ES_FILE to be closed. Any unwritten buffered data for the stream are
 * delivered to the host environment to be written to the ES_FILE; any unread
 * buffered data are discarded. The stream is disassociated from the ES_FILE.
 * If the associated buffer was automatically allocated, it is deallocated.
 * Returns: zero if the stream was succesfully closed, or nonzero if any
 *          errors were detected or if the stream was already closed.
 */

extern int eLIBs_fsync(ES_FILE *stream);
/*
 * request that all data for the open file descriptor named by fildes is to
 * be transferred to the storage device associated with the file described
 * by fildes..
 * Returns: zero if the stream was succesfully synced, or nonzero if any
 *          errors were detected or if the stream was already closed.
 */

extern ES_FILE *eLIBs_fopen(const char * /*ES_FILEname*/, const char * /*mode*/);
/*
 * opens the ES_FILE whose name is the string pointed to by ES_FILEname, and
 * associates a stream with it.
 * The argument mode points to a string beginning with one of the following
 * sequences:
 * "r"         open text ES_FILE for reading
 * "w"         create text ES_FILE for writing, or truncate to zero length
 * "a"         append; open text ES_FILE or create for writing at eof
 * "rb"        open binary ES_FILE for reading
 * "wb"        create binary ES_FILE for writing, or truncate to zero length
 * "ab"        append; open binary ES_FILE or create for writing at eof
 * "r+"        open text ES_FILE for update (reading and writing)
 * "w+"        create text ES_FILE for update, or truncate to zero length
 * "a+"        append; open text ES_FILE or create for update, writing at eof
 * "r+b"/"rb+" open binary ES_FILE for update (reading and writing)
 * "w+b"/"wb+" create binary ES_FILE for update, or truncate to zero length
 * "a+b"/"ab+" append; open binary ES_FILE or create for update, writing at eof
 *
 * Opening a ES_FILE with read mode ('r' as the first character in the mode
 * argument) fails if the ES_FILE does not exist or cannot be read.
 * Opening a ES_FILE with append mode ('a' as the first character in the mode
 * argument) causes all subsequent writes to be forced to the current end of
 * ES_FILE, regardless of intervening calls to the fseek function. In some
 * implementations, opening a binary ES_FILE with append mode ('b' as the
 * second or third character in the mode argument) may initially position
 * the ES_FILE position indicator beyond the last data written, because of the
 * NUL padding.
 * When a ES_FILE is opened with update mode ('+' as the second or third
 * character in the mode argument), both input and output may be performed
 * on the associated stream. However, output may not be ES_DIRectly followed
 * by input without an intervening call to the fflush fuction or to a ES_FILE
 * positioning function (fseek, fsetpos, or rewind), and input be not be
 * ES_DIRectly followed by output without an intervening call to the fflush
 * fuction or to a ES_FILE positioning function, unless the input operation
 * encounters end-of-ES_FILE. Opening a ES_FILE with update mode may open or
 * create a binary stream in some implementations. When opened, a stream
 * is fully buffered if and only if it does not refer to an interactive
 * device. The error and end-of-ES_FILE indicators for the stream are
 * cleared.
 * Returns: a pointer to the object controlling the stream. If the open
 *          operation fails, fopen returns a null pointer.
 */
extern __size eLIBs_fread(void * /*ptr*/,
                          __size /*size*/, __size /*nmemb*/, ES_FILE * /*stream*/);
/*
 * reads into the array pointed to by ptr, up to nmemb members whose size is
 * specified by size, from the stream pointed to by stream. The ES_FILE
 * position indicator (if defined) is advanced by the number of characters
 * successfully read. If an error occurs, the resulting value of the ES_FILE
 * position indicator is indeterminate. If a partial member is read, its
 * value is indeterminate. The ferror or feof function shall be used to
 * distinguish between a read error and end-of-ES_FILE.
 * Returns: the number of members successfully read, which may be less than
 *          nmemb if a read error or end-of-ES_FILE is encountered. If size or
 *          nmemb is zero, fread returns zero and the contents of the array
 *          and the state of the stream remain unchanged.
 */

extern __size eLIBs_fwrite(const void * /*ptr*/,
                           __size /*size*/, __size /*nmemb*/, ES_FILE * /*stream*/);
/*
 * writes, from the array pointed to by ptr up to nmemb members whose size
 * is specified by size, to the stream pointed to by stream. The ES_FILE
 * position indicator (if defined) is advanced by the number of characters
 * successfully written. If an error occurs, the resulting value of the ES_FILE
 * position indicator is indeterminate.
 * Returns: the number of members successfully written, which will be less
 *          than nmemb only if a write error is encountered.
 */
extern __s32 eLIBs_fioctrl(ES_FILE * /*stream*/,
                           __s32 /*Cmd*/, __s32 /*Aux*/, void * /*pBuffer*/);
/*
 * ioctrl, if stream is a device file, this function will send cmd to the device,
 * aux, pbuffer is the cmd arg.
 *Returns: a __s32 result.
 */
extern __s32 eLIBs_ftruncate(ES_FILE * /*stream*/, __u32 /*length*/);

extern __s32 eLIBs_fseek(ES_FILE * /*stream*/, __s32 /*offset*/, int /*whence*/);
/*
 * sets the ES_FILE position indicator for the stream pointed to by stream.
 * For a binary stream, the new position is at the signed number of
 * characters specified by offset away from the point specified by whence.
 * The specified point is the beginning of the ES_FILE for SEEK_SET, the
 * current position in the ES_FILE for SEEK_CUR, or end-of-ES_FILE for SEEK_END.
 * A binary stream need not meaningfully support fseek calls with a whence
 * value of SEEK_END.
 * For a text stream, either offset shall be zero, or offset shall be a
 * value returned by an earlier call to the ftell function on the same
 * stream and whence shall be SEEK_SET.
 * The fseek function clears the end-of-ES_FILE indicator and undoes any
 * effects of the ungetc function on the same stream. After an fseek call,
 * the next operation on an update stream may be either input or output.
 * Returns: nonzero only for a request that cannot be satisfied.
 */

extern __s32 eLIBs_ftell(ES_FILE * /*stream*/);
/*
 * obtains the current value of the ES_FILE position indicator for the stream
 * pointed to by stream. For a binary stream, the value is the number of
 * characters from the beginning of the ES_FILE. For a text stream, the ES_FILE
 * position indicator contains unspecified information, usable by the fseek
 * function for returning the ES_FILE position indicator to its position at the
 * time of the ftell call; the difference between two such return values is
 * not necessarily a meaningful measure of the number of characters written
 * or read.
 * Returns: if successful, the current value of the ES_FILE position indicator.
 *          On failure, the ftell function returns -1L and sets the integer
 *          expression errno to an implementation-defined nonzero value.
 */

extern __s32 eLIBs_fllseek(ES_FILE * /*stream*/, __s64 /*offset*/, __s32 /*whence*/);
/*
 * sets the ES_FILE position indicator for the stream pointed to by stream.
 * For a binary stream, the new position is at the signed number of
 * characters specified by offset away from the point specified by whence.
 * The specified point is the beginning of the ES_FILE for SEEK_SET, the
 * current position in the ES_FILE for SEEK_CUR, or end-of-ES_FILE for SEEK_END.
 * A binary stream need not meaningfully support fseek calls with a whence
 * value of SEEK_END.
 * For a text stream, either offset shall be zero, or offset shall be a
 * value returned by an earlier call to the ftell function on the same
 * stream and whence shall be SEEK_SET.
 * The fseek function clears the end-of-ES_FILE indicator and undoes any
 * effects of the ungetc function on the same stream. After an fseek call,
 * the next operation on an update stream may be either input or output.
 * Returns: nonzero only for a request that cannot be satisfied.
 */

extern __s64 eLIBs_flltell(ES_FILE * /*stream*/);
/*
 * obtains the current value of the ES_FILE position indicator for the stream
 * pointed to by stream. For a binary stream, the value is the number of
 * characters from the beginning of the ES_FILE. For a text stream, the ES_FILE
 * position indicator contains unspecified information, usable by the fseek
 * function for returning the ES_FILE position indicator to its position at the
 * time of the ftell call; the difference between two such return values is
 * not necessarily a meaningful measure of the number of characters written
 * or read.
 * Returns: if successful, the current value of the ES_FILE position indicator.
 *          On failure, the ftell function returns -1L and sets the integer
 *          expression errno to an implementation-defined nonzero value.
 */

extern void eLIBs_rewind(ES_FILE * /*stream*/);
/*
 * sets the ES_FILE position indicator for the stream pointed to by stream to
 * the beginning of the ES_FILE. It is equivalent to
 *          (void)fseek(stream, 0L, SEEK_SET)
 * except that the error indicator for the stream is also cleared.
 * Returns: no value.
 */

extern void eLIBs_ferrclr(ES_FILE * /*stream*/);
/*
 * clears the end-of-ES_FILE and error indicators for the stream pointed to by
 * stream. These indicators are cleared only when the ES_FILE is opened or by
 * an explicit call to the clearerr function or to the rewind function.
 * Returns: no value.
 */

extern __s32 eLIBs_ferror(ES_FILE * /*stream*/);
/*
 * tests the error indicator for the stream pointed to by stream.
 * Returns: nonzero iff the error indicator is set for stream.
 */


extern ES_DIR *eLIBs_opendir(const char * /*dirname*/);
/*
 * Descriptions: open dir
 * Returns: ES_DIR
 */

extern __s32 eLIBs_closedir(ES_DIR * /*pDir*/);
/*
 * Returns: zero if the operation succeeds, nonzero if it fails.
 */

extern ES_DIRENT *eLIBs_readdir(ES_DIR * /*pDir*/);
/*
 *
 */

extern void eLIBs_rewinddir(ES_DIR * /* pDir*/);
/*
 * Descriptions: make the point of read to start
 * Returns: none
 */

extern int eLIBs_mkdir(const char * /*dirname*/);
/*
 * create a dir item
 * Returns: zero if the operation succeeds, nonzero if it fails.
 */

extern int eLIBs_rmdir(const char * /*dirname*/);
/*
 * remove dir item
 * Returns: zero if the operation succeeds, nonzero if it fails.
 */
/**
**********************************************************************************************************************
*
* @fn - void eLIBs_DirEnt2Attr(ES_DIRENT *pDirEnt, __u32 *pAttr)
*
* @brief - The function stores the attributes of the directory entry referred by pDirEnt
* @param - pDirEnt : Pointer to a directory entry, read by FS_ReadDir()
* @param - pAttr : Pointer to store attributes
* @return - none
**********************************************************************************************************************
*/
extern void eLIBs_DirEnt2Attr(ES_DIRENT *pDirEnt, __u32 *pAttr);

/**
**********************************************************************************************************************
*
* @fn - void eLIBs_DirEnt2Name(ES_DIRENT *pDirEnt, char *pBuffer)
*
* @brief - The function stores the name of the directory entry referred by pDirEnt.
* @param - pDirEnt : Pointer to a directory entry, read by FS_ReadDir()
* @param - pBuffer : Pointer to the buffer that will receive the text
* @return - none
*
**********************************************************************************************************************
*/
extern void eLIBs_DirEnt2Name(ES_DIRENT *pDirEnt, char *pBuffer);

/**
**********************************************************************************************************************
*
* @fn - __s32 eLIBs_DirEnt2Size(ES_DIRENT *pDirEnt)
*
* @brief - The function returns the size in bytes of the directory entry referred by pDirEnt.
* @param - pDirEnt : directory entry read out by readdir() function
* @return - If pDirEnt is valid, the size of the directory entry will be returned. Otherwise the
*           return value is 0.
*
**********************************************************************************************************************
*/
extern __s32 eLIBs_DirEnt2Size(ES_DIRENT *pDirEnt);

/**
**********************************************************************************************************************
*
* @fn - __u32 eLIBs_DirEnt2Time(ES_DIRENT *pDirEnt)
*
* @brief - The function returns the size in bytes of the directory entry referred by pDirEnt
* @param - pDirEnt : directory entry read out by readdir() function
* @return - A timestamp is packed value with the following format:
*            0-4  Second divided by 2
*            5-10  Minute (0 ¨C 59)
*           11-15  Hour (0-23)
*           16-20  Day of month (1-31)
*           21-24  Month (January  1; February  2 etc)
*           25-31  Year offset from 1980  add 1980 to get current year.
*           To convert a timestamp to a ES_FILETIME structure, please use the function
*           FS_TimeStampToFileTime.
*
**********************************************************************************************************************
*/
extern __u32 eLIBs_DirEnt2Time(ES_DIRENT *pDirEnt);

/**
**********************************************************************************************************************
*
* @fn - __s64 eLIBs_GetNumFiles(ES_DIR *pDir)
*
* @brief - The function returns the number of files in a directory opened by FS_OpenDir.
* @param - pDir : Pointer to directory structure
* @return - Number of files in a directory
*           If pDir is valid, the number of file in the directory will be returned. Otherwise the
*           return value is -1.
**********************************************************************************************************************
*/
extern __s64 eLIBs_GetNumFiles(ES_DIR *pDir);

/**
**********************************************************************************************************************
*
* @fn - __s32 eLIBs_GetNumSubItems(ES_DIR *pDir)
*
* @brief - The function returns the number of sub items in a directory opened by FS_OpenDir.
* @param - pDir : Pointer to directory structure
* @return - Number of sub items in a directory
*           If pDir is valid, the number of sub items in the directory will be returned. Otherwise the
*           return value is -1.
*
**********************************************************************************************************************
*/
extern __s32 eLIBs_GetNumSubItems(ES_DIR *pDir);

/**
**********************************************************************************************************************
*
* @fn - __u32 eLIBs_GetFileAttributes(const char * pName)
*
* @brief - retrieves attributes for a specified file or directory.
* @param - pName : Pointer to a string that specifies the name of a file or directory.
* @param - pAttr : bitmap result
* @return -
*   >=0     Attributes of the given file or directory:
*       FSYS_ATTR_ARCHIVE   - Applications can use this attribute to mark files for backup
*                             or removal.
*       FSYS_ATTR_DIRECTORY - The given pName is a directory.
*       FSYS_ATTR_HIDDEN    - The file or directory is hidden. It is not included in an ordinary
*                             directory listing
*       FSYS_ATTR_READ_ONLY - The file or directory is read-only. Applications can read the file
*                             but cannot write to it or delete it. In the case of a directory,
*                             applications cannot delete it
*       FSYS_ATTR_SYSTEM    - The file or directory is part of, or is used exclusively by, the
*                             operating system.
*   ==0xff   In case of any error.
*
**********************************************************************************************************************
*/
extern __u32 eLIBs_GetFileAttributes(const char *pName);

/**
**********************************************************************************************************************
*
* @fn - int eLIBs_SetFileAttributes(const char * pName, __u32 Attributes)
*
* @brief -  set attributes for a specified file or directory.
* @param - pName : Pointer to a string that specifies the name of a file or directory.
* @param - Attr  : Attributes to be set to the file or directory.
* @return -
*    <> 0  In case of any  error. The attributes can be one or more of the following values:
*      FSYS_ATTR_ARCHIVE    - The file or directory is an archive file or directory.
*                             Applications can use this attribute to mark files for backup or removal.
*      FSYS_ATTR_HIDDEN     - The file or directory is hidden.
*                             It is not included in an ordinary directory listing
*      FSYS_ATTR_READ_ONLY  - The file or directory is read-only.
*                             Applications can read the file but cannot write to it or delete it.
*                             In the case of a directory, applications cannot delete it
*      FSYS_ATTR_SYSTEM     - The file or directory is part of, or is used exclusively by, the operating system.
*    == 0  Attributes have been successfully set.
*
**********************************************************************************************************************
*/
extern int eLIBs_SetFileAttributes(const char *pName, __u32 Attributes);

/**
**********************************************************************************************************************
*
* @fn - __s64 eLIBs_GetDirSize(const char * dirpath)
*
* @brief - Get total size of a directory.
* @param - dirpath : directory path name
* @return - >=0   Direntory size of in bytes.
*           == -1 too deep dir level
*           == -2 malloc fail
*           == -3 open dir error
*
**********************************************************************************************************************
*/
extern __s64 eLIBs_GetDirSize(const char *dirpath);

/**
**********************************************************************************************************************
*
* @fn - __s64 eLIBs_GetFileSize(ES_FILE * pFile)
*
* @brief - Get the current file size of a file.
* @param - filename : Pointer to file name.
* @return - File size of in bytes.
*           == -1 open file failed
*           == -2 system call failed
*
**********************************************************************************************************************
*/
extern __s64 eLIBs_GetFileSize(const char *filename);

/**
**********************************************************************************************************************
*
* @fn - int eLIBs_Format(const char* partName, FSYS_FORMAT_INFO *pFormatInfo)
*
* @brief - get the attribute bitmap for the given dirctory entry
* @param - partName : partition name
* @param - fsname: fs type, now only support "fat"
* @param - fmtpara : struction ES_FMTPARA handle for format para, NULL cause all default para using
* @return -
*       == 0    - format successfully
*       <> 0    - format failure
**********************************************************************************************************************
*/
extern int eLIBs_format(const char *partname, const char *fsname, __hdle fmtpara);

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolName
*
* format one partition with given format name
*
* Returns: the volume name if ok.
**********************************************************************************************************************
*/
extern int eLIBs_GetVolName(const char *partname, char *vn);

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolNameCharset
*
* format one partition with given format name
*
* Returns: the volume name if ok.
**********************************************************************************************************************
*/
extern int eLIBs_GetVolNameCharset(const char *partname);

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolTSpace
*
* format one partition with given format name
*
* Returns: the total byte size of volume.
**********************************************************************************************************************
*/
extern __u64 eLIBs_GetVolTSpace(const char *partname);

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolFSpace
*
* format one partition with given format name
*
* Returns: the free byte size of volume.
**********************************************************************************************************************
*/
extern __u64 eLIBs_GetVolFSpace(const char *partname);

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolFSpace
*
* format one partition with given format name
*
* Returns: the byte size of cluster.
**********************************************************************************************************************
*/
extern __u32 eLIBs_GetVolClustSize(const char *partname);

/*
**********************************************************************************************************************
*
*               eLIBs_GetFSName
*
* format one partition with given format name
*
* Returns: the bind file system name with part.
**********************************************************************************************************************
*/
extern __s32 eLIBs_GetFSName(const char *partname, char *fsn);

/*
**********************************************************************************************************************
*
*               eLIBs_GetFSAttribute
*
* get the attributes of one partition specially by partname
*
* Returns:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error
**********************************************************************************************************************
*/
extern __s32 eLIBs_GetFSAttribute(const char *partname, __u32 *attribute);

/*
**********************************************************************************************************************
*
*               eLIBs_GetPTName
*
* get name of a part
*
* Returns: .
**********************************************************************************************************************
*/
__s32 eLIBs_GetPTName(const char *partname, char *ptn);


/*
**********************************************************************************************************************
*
*               eLIBs_IsPartFormated
*
* detect if part has filesystem (has been formated in know fs type)
*
* Returns:
* ELIBS_PART_FMT_UNKONWERR    -3    unknow error
* ELIBS_PART_FMT_NOACCES      -2    part not aceesable, maybe opened by usbd
* ELIBS_PART_FMT_NOEXIST      -1    part not exist
* ELIBS_PART_FMT_NFORMATED     0    not format
* ELIBS_PART_FMT_FORMATED      1    formated
**********************************************************************************************************************
*/
__s32 eLIBs_IsPartFormated(const char *partname);

extern __s32 eLIBs_disklock(char *diskname);
/*
**********************************************************************************************************************
*  Description:
*               - hold one disk from been fs-unmounted by other applications.
*
*  Parameters:
*   diskname    - disk letter follow up with ":\", such as "c:\" , 'd:\' etc.
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error
**********************************************************************************************************************
*/
extern __s32 eLIBs_diskunlock(char *diskname);
/*
*  Description:
*               - give up occupying one disk. the disk then can be unmount from bind-fs
*
*  Parameters:
*   diskname    - disk letter follow up with ":\", such as "c:\" , 'd:\' etc.
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error.
**********************************************************************************************************************
*/
extern int eLIBs_GetDiskArray(__u8 diskArray[]);
/*
**********************************************************************************************************************
*
*             eLIBs_GetDiskArray
*
*  Description:
*               - get current disk array list,
*                 if one disk mount, disk member of the disk letter array will be disk letter
*                 if one disk unmount, disk member of the disk letter array will be '\0'
*
*  Parameters:
*   diskArray   - disk letter array, array size must be larger than MAX_DISK_LETTER_LEN
*
*  Return value:
*   EPDK_OK     - ok
*   EPDK_FAIL   - some thing error.
**********************************************************************************************************************
*/
extern int eLIBs_feof(ES_FILE *stream);
/*
**********************************************************************************************************************
*
*             eLIBs_feof
*
*  Description:
*               - determines whether the end of stream has been reached.
*
*  Parameters:
*   stream      - Pointer to ES_FILE structure.
*
*  Return value:
*               -nonzero if the end of stream has been reached.
**********************************************************************************************************************
*/
extern int eLIBs_fgetc(ES_FILE *stream);
/*
**********************************************************************************************************************
*
*             eLIBs_fgetc
*
*  Description:
*               - reads a single character as an unsigned char converted to
*                 an int, from the stream pointed to by stream, and advances the
*                 associated file position indicator.
*
*  Parameters:
*   stream      - Pointer to ES_FILE structure.
*
*  Return value:
*               - the character read as an int or ES_EOF to indicate an error or end of file.
**********************************************************************************************************************
*/
extern char *eLIBs_fgets(char *string, int n, ES_FILE *stream);
/*
**********************************************************************************************************************
*
*             eLIBs_fgets
*
*  Description:
*               - reads at most one less than the number of characters specified by n from
*                 the stream pointed to by stream into the array pointed to by string. No
*                 additional characters are read after a new-line character (which is
*                 retained) or after end-of-file. A null character is written immediately
*                 after the last character read into the array.
*
*  Parameters:
*   string      - Storage location for data.
*   n           - Maximum number of characters to read.
*   stream      - Pointer to ES_FILE structure.
*
*  Return value:
*               - string if successful. If end-of-file is encountered and no characters
*                 have been read into the array, the contents of the array remain
*                 unchanged and a null pointer is returned. If a read error occurs
*                 during the operation, the array contents are indeterminate and a
*                 null pointer is returned.
**********************************************************************************************************************
*/
extern int eLIBs_fputc(int c, ES_FILE *stream);
/*
**********************************************************************************************************************
*
*             eLIBs_fputc
*
*  Description:
*               - writes the character specified by c (converted to an unsigned char) to
*                 the output stream pointed to by stream, at the position indicated by the
*                 asociated file position indicator (if defined), and advances the
*                 indicator appropriately. If the file position indicator is not defined,
*                 the character is appended to the output stream.
*
*  Parameters:
*   c           - Character to be written
*   stream      - Pointer to ES_FILE structure.
*
*  Return value:
*               - the character written. ES_EOF when write error occurs.
**********************************************************************************************************************
*/
extern int eLIBs_fputs(const char *string, ES_FILE *stream);
/*
**********************************************************************************************************************
*
*             eLIBs_fputs
*
*  Description:
*               - writes the string pointed to by string to the stream pointed to by stream.
*                 The terminating null character is not written.
*
*  Parameters:
*   string      - Pointer to Output string.
*   stream      - Pointer to ES_FILE structure.
*
*  Return value:
*               - a nonnegative value if successful, ES_EOF when write error occurs.
**********************************************************************************************************************
*/

extern int eLIBs_GetFileATime(const char *filename, ES_TIMESTAMP *atime);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetFileATime
*
*  Description:
*               - Get the last access time of a file.
*
*  Parameters:
*   filename    - Pointer to file name.
*   atime       - Pointer to access timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern int eLIBs_GetFileMTime(const char *filename, ES_TIMESTAMP *mtime);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetFileMTime
*
*  Description:
*               - Get the last modify time of a file.
*
*  Parameters:
*   filename    - Pointer to file name.
*   atime       - Pointer to modify timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern int eLIBs_GetFileCTime(const char *filename, ES_TIMESTAMP *ctime);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetFileMTime
*
*  Description:
*               - Get the create time of a file.
*
*  Parameters:
*   filename    - Pointer to file name.
*   atime       - Pointer to create timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern int eLIBs_GetDirATime(const char *dirname, ES_TIMESTAMP *atime);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetDirATime
*
*  Description:
*               - Get the last aceess time of a direntory.
*
*  Parameters:
*   filename    - Pointer to direntory name.
*   atime       - Pointer to access timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern int eLIBs_GetDirMTime(const char *dirname, ES_TIMESTAMP *mtime);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetDirMTime
*
*  Description:
*               - Get the last modify time of a direntory.
*
*  Parameters:
*   filename    - Pointer to direntory name.
*   atime       - Pointer to modify timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern int eLIBs_GetDirCTime(const char *dirname, ES_TIMESTAMP *ctime);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetDirCTime
*
*  Description:
*               - Get the create time of a direntory.
*
*  Parameters:
*   filename    - Pointer to direntory name.
*   atime       - Pointer to create timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/


extern __s32 eLIBs_GetFSCharset(const char *partname, __s32 *pCharset);
/*
**********************************************************************************************************************
*
*                       eLIBs_GetFSCharset
*
*  Description:
*               - Get FS current charset type.
*
*  Parameters:
*   partname    - Pointer to fs part path.
*   pCharset    - Pointer to store charset type buffer.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/


__s32 eLIBs_file2fd(ES_FILE *hFile);
/*
**********************************************************************************************************************
*
*                       eLIBs_file2fd
*
*  Description:
*               - get fd accord to the file handle.
*
*  Parameters:
*   hFile       - handle to opened file.
*
*  Return value:
*               fd
**********************************************************************************************************************
*/

ES_FILE *eLIBs_fd2file(__s32 fd);
/*
**********************************************************************************************************************
*
*                       eLIBs_fd2file
*
*  Description:
*               - Get file handle accord to fd.
*
*  Parameters:
*   fd          - fd.
*
*  Return value:
*               file handle, NULL if failed.
**********************************************************************************************************************
*/

int eLIBs_open(const char *name, int flag, int prems);
/*
**********************************************************************************************************************
*
*                       eLIBs_open
*
*  Description:
*               -
*
*  Parameters:
*
*  Return value:
*
**********************************************************************************************************************
*/

int eLIBs_fstat(ES_FILE *hFile, __ES_FSTAT *buffer);
/*
**********************************************************************************************************************
*
*                       eLIBs_fstat
*
*  Description:
*               -
*
*  Parameters:
*
*  Return value:
*
**********************************************************************************************************************
*/


int eLIBs_statfs(const char *path, __ES_FSSTAT *buffer);
/*
**********************************************************************************************************************
*
*                       eLIBs_statfs
*
*  Description:
*               -
*
*  Parameters:
*
*  Return value:
*
**********************************************************************************************************************
*/

extern int eLIBs_GetDevName(const char *pPartName, char *pDevName, int DevNameLen);
/*
**********************************************************************************************************************
*
*                                   eLIBs_GetDevName
*
*  Description:
*               - Get device name by partition path.
*
*  Parameters:
*   pPartName   - pointer to partition name , eg : "F:\\".
*   pDevName    - pointer to device name, output buffer.
*   DevNameLen  - length of device name buffer.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern int eLIBs_GetDevParts(const char *pDevName, char PartsArray[], int PartsArrayLen);
/*
**********************************************************************************************************************
*
*                                   eLIBs_GetDevParts
*
*  Description:
*               - Get partitions of device specially by pDevName.
*
*  Parameters:
*   pDevName        - pointer to device name.
*   pPartsArray     - pointer to partition array, partition output format : "FGH\0".
*   PartsArrayLen   - length of partition array buffer.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/

extern __s32 eLIBs_GetTime(void *time);
extern __s32 eLIBs_GetDate(void *date);

/*
*********************************************************************************************************
*                                           ELIBS_VSNPRINTF
*
* Description: Write formatted output using a pointer to a list of arguments.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              args     -Pointer to list of arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
extern int eLIBs_vsnprintf(char *buffer, size_t size, const char *format, va_list arg);

/*
*********************************************************************************************************
*                                           ELIBS_VSCNPRINTF
*
* Description: Write formatted output using a pointer to a list of arguments.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              args     -Pointer to list of arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
extern int eLIBs_vscnprintf(char *buffer, size_t size, const char *format, va_list args);

/*
*********************************************************************************************************
*                                           ELIBS_SNPRINTF
*
* Description: Write formatted data to a string.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              ...      -Optional arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
extern int eLIBs_snprintf(char *buffer, size_t size, const char *format, ...);

/*
*********************************************************************************************************
*                                           ELIBS_SCNPRINTF
*
* Description: Write formatted data to a string.
*
* Arguments  :
*              buffer   -Storage location for output
*              size     -Maximum number of characters to store
*              format   -Format-control string
*              ...      -Optional arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*********************************************************************************************************
*/
extern int eLIBs_scnprintf(char *buffer, size_t size, const char *format, ...);

/*
*********************************************************************************************************
*                                           ELIBS_SPRINTF
*
* Description: Write formatted output using a pointer to a list of arguments.
*
* Arguments  :
*              buffer   -Storage location for output
*              format   -Format-control string
*              args     -Pointer to list of arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*
*********************************************************************************************************
*/
extern int eLIBs_vsprintf(char *buffer, const char *format, va_list args);

/*
*********************************************************************************************************
*                                           ELIBS_SPRINTF
*
* Description: Write formatted data to a string.
*
* Arguments  :
*              buffer   -Storage location for output
*              format   -Format-control string
*              ...      -Optional arguments
*
* Returns    : the number of bytes stored in buffer, not counting the terminating null character.
*
* Note       : The sprintf function formats and stores a series of characters and values in buffer.
*              Each argument (if any) is converted and output according to the corresponding format
*              specification in format. The format consists of ordinary characters and has the same
*              form and function as the format argument for ANSI-C printf.
*              but the int64(or long long) format control string is : "%qd".
*
* Example:
*           {
*               char    buffer[512];
*               char    s[] = "computer";
*               char    c = 'l';
*               int     i = 35;
*               float   fp = 1.7320534f;
*               __int64 i64 = 0xFFFFFFFFFFFF;
*               int     len;
*
*               len  = eLIBs_sprintf(buffer,        "\tString:    %s\n", s);
*               len += eLIBs_sprintf(buffer + len,  "\tCharacter: %c\n", c);
*               len += eLIBs_sprintf(buffer + len,  "\tInteger:   %d\n", i);
*               len += eLIBs_sprintf(buffer + len,  "\tint64:     %qd\n",i64);
*               len += eLIBs_sprintf(buffer + len,  "\tReal:      %f\n", fp);
*
*               eLIBs_printf("Output:\n%s\ncharacter count = %d\n", buffer, len);
*           }
*           eLIBs_printf output result
*           Output:
*           String:    computer
*           Character: l
*           Integer:   35
*           int64:     281474976710655
*           Real:      1.732053
*
*           character count = 99
*********************************************************************************************************
*/
extern int eLIBs_sprintf(char *buffer, const char *format, ...);

/*
******************************************************************************************************************
*
*Function Name : eLIBs_printf
*
*Description : This function is to formatedly output through UART, similar to ANSI C function printf().
*                This function can support and only support the following Conversion Specifiers:
*              %d       Signed decimal integer.
*              %u       Unsigned decimal integer.
*              %x       Unsigned hexadecimal integer, using hex digits 0f.
*              %X       Unsigned hexadecimal integer, using hex digits 0F.
*              %c       Single character.
*              %s       Character string.
*              %p       A pointer.
*              %f       float
*              %qd      64 bit signed decimal integer.
*
*Input : refer to ANSI C function printf().
*
*Output : void, different form ANSI C function printf().
*
*call for : void int_to_string_dec( int input, char * str ), defined in format_transformed.c.
*           void int_to_string_hex( int input, char * str );  defined in format_transformed.c.
*           void Uint_to_string_dec( unsigned int input, char * str );  defined in format_transformed.c.
*           void UART_putchar( int ch); defined in boot loader.
*           void UART_puts( const char * string ); defined in boot loader.
*
*Others : None at present.
*
*******************************************************************************************************************
*/
typedef void (*libc_printf)(const char *fmt, ...);

extern int eLIBs_printf(const char *fmt, ...);
extern int eLIBs_vprintf(const char *format, va_list args);
#define newlib_printf  ((libc_printf)(*((uint32_t*)0xc200c004)))

#ifdef __cplusplus
}
#endif

#endif /* end of stdio.h */
