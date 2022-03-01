/*
**********************************************************************************************************************
*                                                   ePDK
*                                  the Easy Portable/Player Development Kit
*                                              eLibC sub-system
*
*                                (c) Copyright 2007-2008, Steven.ZGJ.China
*                                           All Rights Reserved
*
* File    : stdio.c
* By      : steven.ZGJ
* Version : V1.00
* Purpose : used for ascii c file system
*
**********************************************************************************************************************
*/
#include <typedef.h>
#include <kapi.h>
#include <string.h>
#include <libc.h>
#include <log.h>
#include <mod_charset.h>
#include <stdio.h>
#include <aw_dirent.h>

/*
**********************************************************************************************************************
*             Local Variables
**********************************************************************************************************************
*/


/*
**********************************************************************************************************************
*             Global functions
**********************************************************************************************************************
*/
/*
**********************************************************************************************************************
*
*               remove
*
* causes the file whose name is the string pointed to by filename to be
* removed. Subsequent attempts to open the file will fail, unless it is
* created anew. If the file is open, the behaviour of the remove function
* is implementation-defined.
* Returns: zero if the operation succeeds, nonzero if it fails.
**********************************************************************************************************************
*/
int eLIBs_remove(const char *filename)
{
    return esFSYS_remove(filename);
}
/*
**********************************************************************************************************************
*
*               remove
*
* causes the file whose name is the string pointed to by filename to be
* removed. Subsequent attempts to open the file will fail, unless it is
* created anew. If the file is open, the behaviour of the remove function
* is implementation-defined.
* Returns: zero if the operation succeeds, nonzero if it fails.
**********************************************************************************************************************
*/
int eLIBs_rename(const char *newname, const char *oldname)
{
    return esFSYS_rename(newname, oldname);
}

/*
**********************************************************************************************************************
*
*               eLIBs_fopen
*
* Descritpions:  opens the file whose name is the string pointed to by filename, and
*   associates a stream with it.
*
* Argments:
*           The argument mode points to a string beginning with one of the following
*           sequences:
*           "r"         open text file for reading
*           "w"         create text file for writing, or truncate to zero length
*           "a"         append; open text file or create for writing at eof
*           "rb"        open binary file for reading
*           "wb"        create binary file for writing, or truncate to zero length
*           "ab"        append; open binary file or create for writing at eof
*           "r+"        open text file for update (reading and writing)
*           "w+"        create text file for update, or truncate to zero length
*           "a+"        append; open text file or create for update, writing at eof
*           "r+b"/"rb+" open binary file for update (reading and writing)
*           "w+b"/"wb+" create binary file for update, or truncate to zero length
*           "a+b"/"ab+" append; open binary file or create for update, writing at eof
*
*           Opening a file with read mode ('r' as the first character in the mode
*           argument) fails if the file does not exist or cannot be read.
*           Opening a file with append mode ('a' as the first character in the mode
*           argument) causes all subsequent writes to be forced to the current end of
*           file, regardless of intervening calls to the fseek function. In some
*           implementations, opening a binary file with append mode ('b' as the
*           second or third character in the mode argument) may initially position
*           the file position indicator beyond the last data written, because of the
*           NUL padding.
*           When a file is opened with update mode ('+' as the second or third
*           character in the mode argument), both input and output may be performed
*           on the associated stream. However, output may not be directly followed
*           by input without an intervening call to the fflush fuction or to a file
*           positioning function (fseek, fsetpos, or rewind), and input be not be
*           directly followed by output without an intervening call to the fflush
*           fuction or to a file positioning function, unless the input operation
*           encounters end-of-file. Opening a file with update mode may open or
*           create a binary stream in some implementations. When opened, a stream
*           is fully buffered if and only if it does not refer to an interactive
*           device. The error and end-of-file indicators for the stream are
*           cleared.
* Returns:  a pointer to the object controlling the stream. If the open
*           operation fails, fopen returns a null pointer.
**********************************************************************************************************************
*/
ES_FILE *eLIBs_fopen(const char *filename, const char *mode)
{
    return (ES_FILE *)esFSYS_fopen(filename, mode);
}

/*
**********************************************************************************************************************
*
*               eLIBs_fclose
*
* causes the stream pointed to by stream to be flushed and the associated
* file to be closed. Any unwritten buffered data for the stream are
* delivered to the host environment to be written to the file; any unread
* buffered data are discarded. The stream is disassociated from the file.
* If the associated buffer was automatically allocated, it is deallocated.
* Returns: zero if the stream was succesfully closed, or nonzero if any
*          errors were detected or if the stream was already closed.
**********************************************************************************************************************
*/
int eLIBs_fclose(ES_FILE *stream)
{
    return esFSYS_fclose((__hdle)stream);
}

/*
**********************************************************************************************************************
*
*               eLIBs_fsync
*
* request that all data for the open file descriptor named by fildes is to
* be transferred to the storage device associated with the file described
* by fildes..
* Returns: zero if the stream was succesfully synced, or nonzero if any
*          errors were detected or if the stream was already closed.
**********************************************************************************************************************
*/
int eLIBs_fsync(ES_FILE *stream)
{
    return esFSYS_fsync((__hdle)stream);
}
/*
**********************************************************************************************************************
*
*               eLIBs_fread
*
* reads into the array pointed to by ptr, up to nmemb members whose size is
* specified by size, from the stream pointed to by stream. The file
* position indicator (if defined) is advanced by the number of characters
* successfully read. If an error occurs, the resulting value of the file
* position indicator is indeterminate. If a partial member is read, its
* value is indeterminate. The ferror or feof function shall be used to
* distinguish between a read error and end-of-file.
* Returns: the number of members successfully read, which may be less than
*          nmemb if a read error or end-of-file is encountered. If size or
*          nmemb is zero, fread returns zero and the contents of the array
*          and the state of the stream remain unchanged.
**********************************************************************************************************************
*/

__size eLIBs_fread(void *ptr, __size size, __size nmemb, ES_FILE *stream)
{
    return esFSYS_fread(ptr, size, nmemb, (__hdle)stream);
}

/*
**********************************************************************************************************************
*
*               fopen
*

* writes, from the array pointed to by ptr up to nmemb members whose size
* is specified by size, to the stream pointed to by stream. The file
* position indicator (if defined) is advanced by the number of characters
* successfully written. If an error occurs, the resulting value of the file
* position indicator is indeterminate.
* Returns: the number of members successfully written, which will be less
*          than nmemb only if a write error is encountered.
**********************************************************************************************************************
*/
__size eLIBs_fwrite(const void *ptr, __size size, __size nmemb, ES_FILE *stream)
{
    return esFSYS_fwrite(ptr, size, nmemb, (__hdle)stream);
}

__s32 eLIBs_ftruncate(ES_FILE *stream, __u32 length)
{
    return esFSYS_ftruncate((__hdle)stream, length);
}

/*
**********************************************************************************************************************
*
*             esFSeek
*
*  Description:
*  API function. Set current position of a file pointer.
*  FS_fseek does not support to position the fp behind end of a file.
*
*  Parameters:
*  pFile       - Pointer to a FS_FILE data structure.
*  Offset      - Offset for setting the file pointer position.
*  Whence      - Mode for positioning the file pointer.
*
*  Return value:
*  ==0         - File pointer has been positioned according to the
*                parameters.
*  ==EPDK_FAIL - An error has occured.
**********************************************************************************************************************
*/
#ifdef FSEEK_LEGACY
__s32 eLIBs_fseek(ES_FILE *stream, __s32 offset, int whence)
{
    return esFSYS_fseek((__hdle)stream, offset, whence);
}

__s32 eLIBs_fllseek(ES_FILE *stream, __s64 Offset, __s32 Whence)
{
    return esFSYS_fseekex((__hdle)stream, Offset, Offset >> 32, Whence);
}
#else
__s32 eLIBs_fseek(ES_FILE *stream, __s32 offset, int whence)
{
    if (esFSYS_fseek((__hdle)stream, offset, whence) >= 0)
    {
        return 0;
    }
    else
    {
        __err("seek error! from %p.", __builtin_return_address(0));
        return -1;
    }
}

__s32 eLIBs_fllseek(ES_FILE *stream, __s64 Offset, __s32 Whence)
{
    if (esFSYS_fseekex((__hdle)stream, Offset, Offset >> 32, Whence) >= 0)
    {
        return 0;
    }
    else
    {
        __err("seek error! from %p.", __builtin_return_address(0));
        return -1;
    }
}
#endif
/*
**********************************************************************************************************************
*
*               fopen
*
    * obtains the current value of the file position indicator for the stream
    * pointed to by stream. For a binary stream, the value is the number of
    * characters from the beginning of the file. For a text stream, the file
    * position indicator contains unspecified information, usable by the fseek
    * function for returning the file position indicator to its position at the
    * time of the ftell call; the difference between two such return values is
    * not necessarily a meaningful measure of the number of characters written
    * or read.
    * Returns: if successful, the current value of the file position indicator.
    *          On failure, the ftell function returns -1L and sets the integer
    *          expression errno to an implementation-defined nonzero value.
**********************************************************************************************************************
*/
__s32 eLIBs_ftell(ES_FILE *stream)
{
    return esFSYS_ftell((__hdle)stream);
}

/*
**************************************************************************************************
*
*             esFTell
*
*  Description:
*  API function. Return position of a file pointer.
*
*  Parameters:
*  pFile       - Pointer to a FS_FILE data structure.
*
*  Return value:
*  >=0         - Current position of the file pointer.
*  ==-1        - An error has occured.
**************************************************************************************************
*/

__s64 eLIBs_flltell(ES_FILE *stream)
{
    __s32 l_pos;
    __s32 h_pos;

    if (esFSYS_ftellex((__hdle)stream, &l_pos, &h_pos) == EPDK_OK)
    {
        return ((0x00000000FFFFFFFF & ((__s64)l_pos)) | (((__s64)h_pos) << 32));
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*
*               eLIBs_fioctrl
*
    * obtains the current value of the file position indicator for the stream
    * pointed to by stream. For a binary stream, the value is the number of
    * characters from the beginning of the file. For a text stream, the file
    * position indicator contains unspecified information, usable by the fseek
    * function for returning the file position indicator to its position at the
    * time of the ftell call; the difference between two such return values is
    * not necessarily a meaningful measure of the number of characters written
    * or read.
    * Returns: if successful, the current value of the file position indicator.
    *          On failure, the ftell function returns -1L and sets the integer
    *          expression errno to an implementation-defined nonzero value.
**********************************************************************************************************************
*/
__s32 eLIBs_fioctrl(ES_FILE *stream, __s32 Cmd, __s32 Aux, void *pBuffer)
{
    return esFSYS_fioctrl((__hdle)stream, Cmd, Aux, pBuffer);
}

/*
**********************************************************************************************************************
*
*               fopen
*
    * obtains the current value of the file position indicator for the stream
    * pointed to by stream. For a binary stream, the value is the number of
    * characters from the beginning of the file. For a text stream, the file
    * position indicator contains unspecified information, usable by the fseek
    * function for returning the file position indicator to its position at the
    * time of the ftell call; the difference between two such return values is
    * not necessarily a meaningful measure of the number of characters written
    * or read.
    * Returns: if successful, the current value of the file position indicator.
    *          On failure, the ftell function returns -1L and sets the integer
    *          expression errno to an implementation-defined nonzero value.
**********************************************************************************************************************
*/
__s32 eLIBs_ferror(ES_FILE *stream)
{
    return esFSYS_ferror((__hdle)stream);
}
/*
**********************************************************************************************************************
*
*               fopen
*
    * obtains the current value of the file position indicator for the stream
    * pointed to by stream. For a binary stream, the value is the number of
    * characters from the beginning of the file. For a text stream, the file
    * position indicator contains unspecified information, usable by the fseek
    * function for returning the file position indicator to its position at the
    * time of the ftell call; the difference between two such return values is
    * not necessarily a meaningful measure of the number of characters written
    * or read.
    * Returns: if successful, the current value of the file position indicator.
    *          On failure, the ftell function returns -1L and sets the integer
    *          expression errno to an implementation-defined nonzero value.
**********************************************************************************************************************
*/
void eLIBs_ferrclr(ES_FILE *stream)
{
    esFSYS_ferrclr((__hdle)stream);
}
/*
**********************************************************************************************************************
*
*               opendir
*
* Descriptions: open dir
* Returns: dir
**********************************************************************************************************************
*/
ES_DIR *eLIBs_opendir(const char *dirname)
{
    return (ES_DIR *)esFSYS_opendir(dirname);
}

/*
**********************************************************************************************************************
*
*               closedir
*
* Returns: zero if the operation succeeds, nonzero if it fails.
**********************************************************************************************************************
*/
__s32 eLIBs_closedir(ES_DIR *pDir)
{
    return esFSYS_closedir((__hdle)pDir);
}

/*
**********************************************************************************************************************
*
*               readdir
*
**********************************************************************************************************************
*/
ES_DIRENT *eLIBs_readdir(ES_DIR *pDir)
{
    return (ES_DIRENT *)esFSYS_readdir((__hdle)pDir);
}

/*
**********************************************************************************************************************
*
*               rewinddir
*
* Descriptions: make the point of read to start
* Returns: none
**********************************************************************************************************************
*/
void eLIBs_rewinddir(ES_DIR *pDir)
{
    esFSYS_rewinddir((__hdle)pDir);
}

/*
**********************************************************************************************************************
*
*               mkdir
*
* create a dir item
* Returns: zero if the operation succeeds, nonzero if it fails.
**********************************************************************************************************************
*/
int eLIBs_mkdir(const char *dirname)
{
    return esFSYS_mkdir(dirname);
}

/*
**********************************************************************************************************************
*
*               rmdir
*
* remove dir item
* Returns: zero if the operation succeeds, nonzero if it fails.
**********************************************************************************************************************
*/
int eLIBs_rmdir(const char *dirname)
{
    return esFSYS_rmdir(dirname);
}
/**
**********************************************************************************************************************
*
* @fn - void eLIBs_DirEnt2Attr(ES_DIRENT *pDirEnt, __u32 *pAttr)
*
* @brief - The function stores the attributes of the directory entry referred by pDirEnt
* @param - pDirEnt : Pointer to a directory entry, read by FS_ReadDir()
* @param - pAttr : Pointer to store attributes
* @return - none
*
**********************************************************************************************************************
*/
void eLIBs_DirEnt2Attr(ES_DIRENT *pDirEnt, __u32 *pAttr)
{
    *pAttr = pDirEnt->fatdirattr;
}

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
void eLIBs_DirEnt2Name(ES_DIRENT *pDirEnt, char *pBuffer)
{
    int nameLen;
    nameLen = eLIBs_strlen((const char *) pDirEnt->d_name);
    eLIBs_memcpy(pBuffer, pDirEnt->d_name, nameLen + 1);
}

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
__s32 eLIBs_DirEnt2Size(ES_DIRENT *pDirEnt)
{
    return FSYS_FAT_DENTRY_SIZE;
}

/**
**********************************************************************************************************************
*
* @fn - __u32 eLIBs_DirEnt2Time(ES_DIRENT *pDirEnt)
*
* @brief - The function returns the size in bytes of the directory entry referred by pDirEnt
* @param - pDirEnt : directory entry read out by readdir() function
* @return - A timestamp is packed value with the following format:
*            0-4  Second divided by 2
*            5-10  Minute (0 – 59)
*           11-15  Hour (0-23)
*           16-20  Day of month (1-31)
*           21-24  Month (January  1; February  2 etc)
*           25-31  Year offset from 1980  add 1980 to get current year.
*           To convert a timestamp to a ES_FILETIME structure, please use the function
*           FS_TimeStampToFileTime.
*
**********************************************************************************************************************
*/
__u32 eLIBs_DirEnt2Time(ES_DIRENT *pDirEnt)
{
    return 0;
}

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
*
**********************************************************************************************************************
*/
__s64 eLIBs_GetNumFiles(ES_DIR *pDir)
{
    ES_DIRENT   *direntp = NULL;
    __s64        FileCnt = 0;

    if (pDir == NULL)
    {
        return -1;
    }

    while ((direntp = (ES_DIRENT *)esFSYS_readdir((__hdle)pDir)) != NULL)
    {
        if (!(direntp->fatdirattr & FSYS_ATTR_DIRECTORY))
        {
            FileCnt++;
        }
    }

    /* rewind directory postion to begin */
    esFSYS_rewinddir((__hdle)pDir);
    return FileCnt;
}

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
__s32 eLIBs_GetNumSubItems(ES_DIR *pDir)
{
    ES_DIRENT   *direntp = NULL;
    __s32        ItemCnt = 0;

    if (pDir == NULL)
    {
        return -1;
    }

    while ((direntp = (ES_DIRENT *)esFSYS_readdir((__hdle)pDir)) != NULL)
    {
        if (direntp->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            /* skip "." and ".." sub directory */
            if (direntp->d_name[0] == '.')
            {
                continue;
            }
        }

        ItemCnt++;
    }

    /* rewind directory postion to begin */
    esFSYS_rewinddir((__hdle)pDir);
    return ItemCnt;
}

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
*       FS_ATTR_ARCHIVE     - Applications can use this attribute to mark files for backup
*                             or removal.
*       FS_ATTR_DIRECTORY   - The given pName is a directory.
*       FS_ATTR_HIDDEN      - The file or directory is hidden. It is not included in an ordinary
*                             directory listing
*       FS_ATTR_READ_ONLY   - The file or directory is read-only. Applications can read the file
*                             but cannot write to it or delete it. In the case of a directory,
*                             applications cannot delete it
*       FS_ATTR_SYSTEM      - The file or directory is part of, or is used exclusively by, the
*                             operating system.
*   ==0xff   In case of any error.
*
**********************************************************************************************************************
*/
#define S_IFMT          00170000
#define S_IFDIR         0040000
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)

__u32 eLIBs_GetFileAttributes(const char *pName)
{
    __ES_FSTAT  fstat;
    __hdle      fp;
    __u32       attribute;
    fp = esFSYS_fopen(pName, "rb");

    if (!fp)
    {
        return 0;
    }

    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return 0;
    }

    esFSYS_fclose(fp);
    /* convert inode mode to file attributes */
    attribute = 0;

    if (S_ISDIR(fstat.mode))
    {
        attribute |= FSYS_ATTR_DIRECTORY;
    }

    /* maybe should support other attributes ??? */
    return attribute;
}

/**
**********************************************************************************************************************
*
* @fn - int eLIBs_SetFileAttributes(const char * pName, __u32 Attributes)
*
* @brief -  set attributes for a specified file or directory.
* @param - pName : Pointer to a string that specifies the name of a file or directory.
* @param - Attr  : Attributes to be set to the file or directory.
*                  The attributes can be one or more of the following values:
*      FS_ATTR_ARCHIVE      - The file or directory is an archive file or directory.
*                             Applications can use this attribute to mark files for backup or removal.
*      FS_ATTR_HIDDEN       - The file or directory is hidden.
*                             It is not included in an ordinary directory listing
*      FS_ATTR_READ_ONLY    - The file or directory is read-only.
*                             Applications can read the file but cannot write to it or delete it.
*                             In the case of a directory, applications cannot delete it
*      FS_ATTR_SYSTEM       - The file or directory is part of, or is used exclusively by, the operating system.
* @return -
*    == 0  Attributes have been successfully set.
*    <> 0  In case of any  error.
*
**********************************************************************************************************************
*/
int eLIBs_SetFileAttributes(const char *pName, __u32 Attributes)
{
    return 0;
}

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
__s64 eLIBs_GetDirSize(const char *dirpath)
{
    ES_DIR          *dirp;
    ES_DIRENT       *direntp;
    char            *sub_dir;
    __s64           size = 0;
    static int      dir_level = 0;
    static __s64    ret_size;

    if (dir_level > MAX_DIR_SCAN_LEVEL)
    {
        return -1;
    }

    dir_level++;
    sub_dir = esMEMS_Malloc(0, 2048);

    if (!sub_dir)
    {
        return -2;
    }

    dirp = (ES_DIR *)esFSYS_opendir(dirpath);

    if (!dirp)
    {
        esMEMS_Mfree(0, sub_dir);
        return -3;
    }

    while ((direntp = (ES_DIRENT *)esFSYS_readdir((__hdle)dirp)) != NULL)
    {
        if (direntp->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            /* skip "." and ".." sub directory */
            if (direntp->d_name[0] == '.')
            {
                continue;
            }

            eLIBs_strcpy(sub_dir, dirpath);
            eLIBs_strcat(sub_dir, "\\");
            eLIBs_strcat(sub_dir, (const char *)direntp->d_name);
            ret_size = eLIBs_GetDirSize(sub_dir);

            if (ret_size < 0)
            {
                size = ret_size;
                break;
            }
            else
            {
                size += ret_size;
            }
        }
        else
        {
            size += direntp->d_size;
        }
    }

    esFSYS_closedir((__hdle)dirp);
    esMEMS_Mfree(0, sub_dir);
    dir_level--;
    return size;
}

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
__s64 eLIBs_GetFileSize(const char *filename)
{
    __ES_FSTAT fstat;
    __hdle fp;
    fp = esFSYS_fopen(filename, "rb");

    if (!fp)
    {
        return -1;
    }

    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return -2;
    }

    esFSYS_fclose(fp);
    return (__s64)(fstat.size);
}

/*
**********************************************************************************************************************
*
*               format
*
* format one partition with given format name
*
* Returns: zero if the operation succeeds, nonzero if it fails.
**********************************************************************************************************************
*/
int eLIBs_format(const char *partname, const char *fsname, __hdle fmtpara)
{
    return esFSYS_format(partname, fsname, fmtpara);
}

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
int eLIBs_GetVolName(const char *partname, char *vn)
{
    __ES_FSSTAT buf;

    if (!esFSYS_statfs(partname, (__hdle)&buf, FSYS_KSTATUS_VOLNAME))
    {
        eLIBs_strncpy(vn, (const char *)buf.f_volname, MAX_VOLUME_NAME_LEN);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolNameCharset
*
* get volume name charset type.
*
* Returns: the charset type value, define in sys_charset.h.
**********************************************************************************************************************
*/
int eLIBs_GetVolNameCharset(const char *partname)
{
    __s32         Charset = EPDK_CHARSET_ENM_UNKNOWN;
    __ES_FSSTAT buf;

    if (!esFSYS_statfs(partname, (__hdle)&buf, FSYS_KSTATUS_VOLNAME))
    {
        if (eLIBs_stricmp((const char *)buf.f_fsname, FSYS_NTFS_NAME) == 0)
        {
            /* NTFS volume name charset is convert to local charset already */
            esFSYS_getfscharset(partname, &Charset);
        }

        /* other file system volume name can't be sure the charset type */
    }

    return Charset;
}


/*
**********************************************************************************************************************
*               eLIBs_GetVolTSpace
*
* format one partition with given format name
*
* Returns: the total byte size of volume.
**********************************************************************************************************************
*/
__u64 eLIBs_GetVolTSpace(const char *partname)
{
    __ES_FSSTAT  buf;
    __u64       res;

    if (!esFSYS_statfs(partname, (__hdle)&buf, (FSYS_KSTATUS_BLOCKS | FSYS_KSTATUS_BSIZE)))
    {
        res = (__u64)buf.f_blocks * (__u64)buf.f_bsize;
        return res;
    }

    return 0LL;
}

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
__u64 eLIBs_GetVolFSpace(const char *partname)
{
    __ES_FSSTAT  buf;
    __u64       res;

    if (!esFSYS_statfs(partname, (__hdle)&buf, (FSYS_KSTATUS_BFREE | FSYS_KSTATUS_BSIZE)))
    {
        res = (__u64)buf.f_bfree * (__u64)buf.f_bsize;
        return res;
    }

    return 0LL;
}

/*
**********************************************************************************************************************
*
*               eLIBs_GetVolClustSize
*
* format one partition with given format name
*
* Returns: the byte size of cluster.
**********************************************************************************************************************
*/
__u32 eLIBs_GetVolClustSize(const char *partname)
{
    __ES_FSSTAT  buf;

    if (!esFSYS_statfs(partname, (__hdle)&buf, FSYS_KSTATUS_BSIZE))
    {
        return buf.f_bsize;
    }

    return 0LL;
}

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
__s32 eLIBs_GetFSName(const char *partname, char *fsn)
{
    __ES_FSSTAT  buf;

    if (!esFSYS_statfs(partname, (__hdle)&buf, FSYS_KSTATUS_FSNAME))
    {
        eLIBs_strncpy(fsn, (const char *)buf.f_fsname, MAX_FS_NAME_LEN);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

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
__s32 eLIBs_GetFSAttribute(const char *partname, __u32 *attribute)
{
    __ES_FSSTAT  buf;

    if (!esFSYS_statfs(partname, (__hdle)&buf, FSYS_KSTATUS_ATTR) && attribute)
    {
        *attribute = buf.f_attribute;
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

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
__s32 eLIBs_GetPTName(const char *partname, char *ptn)
{
    struct kstatpt buf;

    if ((NULL == partname) || (NULL == ptn))
    {
        __err("invalid parameter.");
        return EPDK_FAIL;
    }

    memset(&buf, 0x00, sizeof(buf));

    if (!esFSYS_statpt(partname, (__hdle)&buf))
    {
        eLIBs_strncpy(ptn, (const char *)buf.p_name, MAX_PART_NAME_LEN);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*
*               eLIBs_PTHasFS
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
#define     LIB_PARTSTATUS_RAWUSED     (1<<2)      /* 直接分区使用    */
#define     LIB_PARTATTR_FS            (1<<4)      /* 分区带有可识别的文件系统 */
__s32 eLIBs_IsPartFormated(const char *partname)
{
    struct kstatpt buf;

    if (NULL == partname)
    {
        __err("invalid parameter.");
        return EPDK_FAIL;
    }

    memset(&buf, 0x00, sizeof(buf));

    if (!esFSYS_statpt(partname, (__hdle)&buf))
    {
        char tmp_buf[3];
        ES_DIR *DIR;
        eLIBs_strncpy(tmp_buf, partname, 2);

        if (buf.p_status == LIB_PARTSTATUS_RAWUSED)
        {
            return -2;
        }

        if (buf.p_attr & LIB_PARTATTR_FS)
        {
            DIR = eLIBs_opendir(tmp_buf);

            if (DIR)
            {
                eLIBs_closedir(DIR);
                return 1;
            }
            else
            {
                return -3;
            }
        }
        else
        {
            DIR = eLIBs_opendir(tmp_buf);

            if (DIR)
            {
                eLIBs_closedir(DIR);
                return -3;
            }
            else
            {
                return 0;
            }
        }
    }

    return -1;
}

/*
**********************************************************************************************************************
*
*             eLIBs_disklock
*
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
__s32 eLIBs_disklock(char *diskname)
{
    return esFSYS_partfslck(diskname);
}

/*
**********************************************************************************************************************
*
*             eLIBs_diskunlock
*
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
__s32 eLIBs_diskunlock(char *diskname)
{
    return esFSYS_partfsunlck(diskname);
}

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
int eLIBs_GetDiskArray(__u8 diskArray[])
{
    __u8 i;
    __u8 disk_letter[4];
    ES_DIR *dirp;
    disk_letter[1] = ':';
    disk_letter[2] = '\\';
    disk_letter[3] = '\0';

    for (i = 0; i < MAX_DISK_LETTER_LEN; i++)
    {
        disk_letter[0] = i + 'A';
        dirp = eLIBs_opendir((const char *)disk_letter);

        if (!dirp)
        {
            diskArray[i] = '\0';
            continue;
        }

        eLIBs_closedir(dirp);
        diskArray[i] = i + 'A';
    }

    return EPDK_OK;
}

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
int eLIBs_feof(ES_FILE *stream)
{
    __ES_FSTAT fstat;
    esFSYS_fstat((__hdle)stream, &fstat);
    return (fstat.pos == fstat.size);
}

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
int eLIBs_fgetc(ES_FILE *stream)
{
    char ch;

    if (esFSYS_fread(&ch, 1, 1, (__hdle)stream) != 1)
    {
        return ES_EOF;
    }

    return (int)ch;
}

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
char *eLIBs_fgets(char *string, int n, ES_FILE *stream)
{
    char *p;
    char ch;

    if ((n <= 0) || (string == NULL))
    {
        return NULL;
    }

    p = string;

    while (--n)
    {
        if (esFSYS_fread(&ch, 1, 1, (__hdle)stream) != 1)
        {
            break;
        }

        if ((*p++ = ch) == '\n')
        {
            break;
        }
    }

    if (p > string)
    {
        *p = '\0';
        return string;
    }

    return NULL;
}

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
int eLIBs_fputc(int c, ES_FILE *stream)
{
    char ch = (char)c;
    return (esFSYS_fwrite(&ch, 1, 1, (__hdle)stream) == 1) ? c : ES_EOF;
}

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
int eLIBs_fputs(const char *string, ES_FILE *stream)
{
    int num = 0;
    num = eLIBs_strlen(string);
    return (esFSYS_fwrite(string, 1, num, (__hdle)stream) == num) ? num : ES_EOF;
}

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
int eLIBs_GetFileATime(const char *filename, ES_TIMESTAMP *atime)
{
    __hdle fp;
    __ES_FSTAT fstat;
    fp = esFSYS_fopen(filename, "rb");

    if (!fp)
    {
        return EPDK_FAIL;
    }

    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return EPDK_FAIL;
    }

    atime->year         = fstat.atime.year;
    atime->month        = fstat.atime.month;
    atime->day          = fstat.atime.day;
    atime->hour         = fstat.atime.hour;
    atime->minute       = fstat.atime.minute;
    atime->second       = fstat.atime.second;
    atime->milliseconds = fstat.atime.milliseconds;
    esFSYS_fclose(fp);
    return EPDK_OK;
}

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
int eLIBs_GetFileMTime(const char *filename, ES_TIMESTAMP *mtime)
{
    __hdle fp;
    __ES_FSTAT fstat;
    fp = esFSYS_fopen(filename, "rb");

    if (!fp)
    {
        return EPDK_FAIL;
    }

    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return EPDK_FAIL;
    }

    mtime->year         = fstat.mtime.year;
    mtime->month        = fstat.mtime.month;
    mtime->day          = fstat.mtime.day;
    mtime->hour         = fstat.mtime.hour;
    mtime->minute       = fstat.mtime.minute;
    mtime->second       = fstat.mtime.second;
    mtime->milliseconds = fstat.mtime.milliseconds;
    esFSYS_fclose(fp);
    return EPDK_OK;
}

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
int eLIBs_GetFileCTime(const char *filename, ES_TIMESTAMP *ctime)
{
    __hdle fp;
    __ES_FSTAT fstat;
    fp = esFSYS_fopen(filename, "rb");

    if (!fp)
    {
        return EPDK_FAIL;
    }

    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return EPDK_FAIL;
    }

    ctime->year         = fstat.ctime.year;
    ctime->month        = fstat.ctime.month;
    ctime->day          = fstat.ctime.day;
    ctime->hour         = fstat.ctime.hour;
    ctime->minute       = fstat.ctime.minute;
    ctime->second       = fstat.ctime.second;
    ctime->milliseconds = fstat.ctime.milliseconds;
    esFSYS_fclose(fp);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*                       eLIBs_GetDirATime
*
*  Description:
*               - Get the last aceess time of a direntory.
*
*  Parameters:
*   dirname     - Pointer to direntory name.
*   atime       - Pointer to access timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/
int eLIBs_GetDirATime(const char *dirname, ES_TIMESTAMP *atime)
{
    __hdle dirp;
    __ES_FSTAT fstat;
    dirp = esFSYS_opendir(dirname);

    if (!dirp)
    {
        return EPDK_FAIL;
    }

    if (EPDK_OK != esFSYS_fstat(dirp, &fstat))
    {
        return EPDK_FAIL;
    }

    atime->year         = fstat.atime.year;
    atime->month        = fstat.atime.month;
    atime->day          = fstat.atime.day;
    atime->hour         = fstat.atime.hour;
    atime->minute       = fstat.atime.minute;
    atime->second       = fstat.atime.second;
    atime->milliseconds = fstat.atime.milliseconds;
    esFSYS_closedir(dirp);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*                       eLIBs_GetDirMTime
*
*  Description:
*               - Get the last modify time of a direntory.
*
*  Parameters:
*   dirname     - Pointer to direntory name.
*   atime       - Pointer to modify timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/
int eLIBs_GetDirMTime(const char *dirname, ES_TIMESTAMP *mtime)
{
    __hdle dirp;
    __ES_FSTAT fstat;
    dirp = esFSYS_opendir(dirname);

    if (!dirp)
    {
        return EPDK_FAIL;
    }

    if (EPDK_OK != esFSYS_fstat(dirp, &fstat))
    {
        return EPDK_FAIL;
    }

    mtime->year         = fstat.mtime.year;
    mtime->month        = fstat.mtime.month;
    mtime->day          = fstat.mtime.day;
    mtime->hour         = fstat.mtime.hour;
    mtime->minute       = fstat.mtime.minute;
    mtime->second       = fstat.mtime.second;
    mtime->milliseconds = fstat.mtime.milliseconds;
    esFSYS_closedir(dirp);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*
*                       eLIBs_GetDirCTime
*
*  Description:
*               - Get the create time of a direntory.
*
*  Parameters:
*   dirname     - Pointer to direntory name.
*   atime       - Pointer to create timestamp.
*
*  Return value:
*   EPDK_OK     - successful
*   EPDK_FAIL   - some thing failed.
**********************************************************************************************************************
*/
int eLIBs_GetDirCTime(const char *dirname, ES_TIMESTAMP *ctime)
{
    __hdle dirp;
    __ES_FSTAT fstat;
    dirp = esFSYS_opendir(dirname);

    if (!dirp)
    {
        return EPDK_FAIL;
    }

    if (EPDK_OK != esFSYS_fstat(dirp, &fstat))
    {
        return EPDK_FAIL;
    }

    ctime->year         = fstat.ctime.year;
    ctime->month        = fstat.ctime.month;
    ctime->day          = fstat.ctime.day;
    ctime->hour         = fstat.ctime.hour;
    ctime->minute       = fstat.ctime.minute;
    ctime->second       = fstat.ctime.second;
    ctime->milliseconds = fstat.ctime.milliseconds;
    esFSYS_closedir(dirp);
    return EPDK_OK;
}

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
__s32 eLIBs_GetFSCharset(const char *partname, __s32 *pCharset)
{
    if ((NULL == partname) || (NULL == pCharset))
    {
        return EPDK_FAIL;
    }

    return esFSYS_getfscharset(partname, pCharset);
}

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
__s32 eLIBs_file2fd(ES_FILE *hFile)
{
    return esFSYS_file2fd((__hdle)hFile);
}

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
ES_FILE *eLIBs_fd2file(__s32 fd)
{
    return (ES_FILE *)esFSYS_fd2file(fd);
}


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
int eLIBs_open(const char *name, int flag, int prems)
{
    return esFSYS_open(name, flag, prems);
}

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
int eLIBs_fstat(ES_FILE *hFile, __ES_FSTAT *buffer)
{
    return esFSYS_fstat((__hdle)hFile, buffer);
}

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
int eLIBs_statfs(const char *path, __ES_FSSTAT *buffer)
{
    return esFSYS_statfs(path, (__hdle)buffer, FSYS_KSTATUS_ALL);
}

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
typedef void (*pfn_printf)(void);

//0xc200c000 is the base system call table of direct.
static pfn_printf kernel_printf = NULL;

int eLIBs_printf(const char *fmt, ...)
{
#if 0
    if (kernel_printf == NULL)
    {
        kernel_printf = *((pfn_printf *)0xc200c000);
    }

    void *args = __builtin_apply_args();
    void *ret = __builtin_apply(kernel_printf, args, 0x400);
    __builtin_return(ret);
    return 0;
#else
    va_list args;
    int     rc;
    va_start(args, fmt);
    rc = eLIBs_vprintf(fmt, args);
    va_end(args);
    return rc;
#endif
}

/*
 *static pfn_printf newlibc_printf = NULL;
 *int newlib_printf(const char *fmt, ...)
 *{
 *    if(newlibc_printf == NULL)
 *    {
 *        newlibc_printf = *((pfn_printf*)0xc200c004);
 *    }
 *
 *    void *args = __builtin_apply_args();
 *    void *ret = __builtin_apply(newlibc_printf, args, 0x400);
 *    __builtin_return(ret);
 *
 *    return 0;
 *}
 */

int printk(const char *fmt, ...) __attribute__((alias("eLIBs_printf")));
int eLIBs_vprintf(const char *format, va_list args)
{
    /* I think 1024 byte buffer space big enough, if you don't think so.
     * you can modify it if you kown what you are doing.
     * by sunny at 2010-11-13 17:21:36.
     */
    int log_level = 0;
    char *p;
    static char buffer[1024];
    int         rc;
    memset(buffer, 0x00, 1024);
    /* construct print format buffer */
    rc = eLIBs_vscnprintf(buffer, 1024, format, args);
    /* print formated buffer to output */
#ifdef CONFIG_DYNAMIC_LOG_LEVEL_SUPPORT
    log_level = get_log_level();
    p = (char *)&buffer;
    if (p[0] != '<' || p[1] < '0' || p[1] > '7' || p[2] != '>')
    {
        if (log_level > (OPTION_LOG_LEVEL_CLOSE))
        {
            esSIOS_putstr(buffer);
        }
    }
    else
    {
        if (log_level >= (p[1] - '0'))
        {
            esSIOS_putstr(&buffer[3]);
        }
    }
#else
    esSIOS_putstr(buffer);
#endif

    return rc;
}

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
int eLIBs_GetDevName(const char *pPartName, char *pDevName, int DevNameLen)
{
    struct kstatpt buf;

    if ((NULL == pPartName) || (NULL == pDevName))
    {
        __err("invalid parameter.");
        return EPDK_FAIL;
    }

    memset(&buf, 0x00, sizeof(buf));

    if (EPDK_OK == esFSYS_statpt(pPartName, (__hdle)&buf))
    {
        eLIBs_strncpy(pDevName, (const char *)buf.p_devname, DevNameLen);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

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
int eLIBs_GetDevParts(const char *pDevName, char PartsArray[], int PartsArrayLen)
{
    __u32   i;
    __u32   j;
    __u8    PartName[4];
    ES_DIR *dirp;
    char    tmpDevName[MAX_DEV_NODE_NAME_LEN];
    PartName[1] = ':';
    PartName[2] = '\\';
    PartName[3] = '\0';

    for (i = 0, j = 0; i < MAX_DISK_LETTER_LEN; i++)
    {
        PartName[0] = i + 'A';
        /* check partition exist or not */
        dirp = eLIBs_opendir((const char *)PartName);

        if (NULL == dirp)
        {
            continue;
        }

        eLIBs_closedir(dirp);
        /* get device name */
        eLIBs_GetDevName((const char *)PartName, tmpDevName, MAX_DEV_NODE_NAME_LEN);

        if (0 == eLIBs_strcmp(pDevName, tmpDevName))
        {
            PartsArray[j++] = i + 'A';
        }
    }

    PartsArray[j] = '\0';
    return EPDK_OK;
}


__s32 eLIBs_GetTime(void *time)
{
    return esTIME_GetTime((__awos_time_t *)time);
}

__s32 eLIBs_GetDate(void *date)
{
    return esTIME_GetDate((__awos_date_t *)date);
}

int32_t eLIBs_setfs(char *partname, uint32_t cmd, int32_t aux, char *para)
{
    esFSYS_setfs(partname, cmd, aux, para);
}
