#include <errno.h>
#include <dfs_posix.h>
#include "File.h"
#include "list.h"
#include "DiskCommon.h"
#include "common.h"
#include "elibs_string.h"
#include "elibs_stdio.h"
#include "MtpCommon.h"

static void getFileInfo(struct File *file)
{
    __hdle fp;
    __ES_FSTAT fstat;
    fp = esFSYS_fopen(file->path, "rb");
    if (!fp)
    {
        return ;
    }

	eLIBs_memset( &fstat, 0x00, sizeof(__ES_FSTAT));
    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return ;
    }


    file->fileSize = fstat.size;
    file->fileAccessTime.year         = fstat.atime.year;
    file->fileAccessTime.month        = fstat.atime.month;
    file->fileAccessTime.day          = fstat.atime.day;
    file->fileAccessTime.hour         = fstat.atime.hour;
    file->fileAccessTime.minute       = fstat.atime.minute;
    file->fileAccessTime.second       = fstat.atime.second;
    file->fileAccessTime.milliseconds = fstat.atime.milliseconds;

    file->fileModifyTime.year         = fstat.mtime.year;
    file->fileModifyTime.month        = fstat.mtime.month;
    file->fileModifyTime.day          = fstat.mtime.day;
    file->fileModifyTime.hour         = fstat.mtime.hour;
    file->fileModifyTime.minute       = fstat.mtime.minute;
    file->fileModifyTime.second       = fstat.mtime.second;
    file->fileModifyTime.milliseconds = fstat.mtime.milliseconds;
    esFSYS_fclose(fp);
    DLOG(" <%s> size: %luM %luK %lubytes", file->fileName, file->fileSize >> 20, file->fileSize >> 10, file->fileSize & ((1 << 10) - 1));
    return ;
}

void updateFileInfo(struct File *file)
{
    getFileInfo(file);
}

void deleteFileInfo(struct File *file)
{
    list_del(&file->list);
}

extern char *getFileNameFromPath(const char *path, char *name, size_t *size);

static bool renameTo(const char *newPath, struct File *file)
{
    int ret;
    char buf[128];
    size_t len = sizeof(buf);

    if (melis_access(newPath) == EPDK_OK)
    {
        return false;
    }
    ret = rename(file->path, newPath);
    if (ret != 0)
    {
        eLIBs_printf("rename oldpath:%s to newpath:%s failed, errno:%d",
               file->path, newPath, errno);
        return false;
    }
    if (file->path)
    {
        esMEMS_Mfree(0, (void *)file->path);
    }
	file->path = (char *)esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(newPath) + 1));
	eLIBs_memset(file->path, 0x00, sizeof(const char)*(eLIBs_strlen(newPath) + 1));
	eLIBs_memcpy(file->path, newPath, sizeof(const char)*eLIBs_strlen(newPath));
    if (file->fileName)
    {
        esMEMS_Mfree(0, (void *)file->fileName);
    }
    if (getFileNameFromPath(newPath, buf, &len) != NULL)
    {
		file->fileName = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(buf) + 1));
		eLIBs_memset(file->fileName, 0x00, sizeof(const char)*(eLIBs_strlen(buf) + 1));
		eLIBs_memcpy(file->fileName, buf, sizeof(const char)*eLIBs_strlen(buf));
    }
    getFileInfo(file);

    return true;
}

struct File *FileInit(const char *path, struct Dir *parentDir)
{
    struct File *file = NULL;
    size_t len;
    char buf[128];

    if (!path)
    {
        return NULL;
    }

    DLOG(" path:%s", path);
    file = (struct File *)esMEMS_Malloc(0, sizeof(struct File));
	
	file->path = (char *)esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(path) + 1));
	eLIBs_memset(file->path, 0x00, sizeof(const char)*(eLIBs_strlen(path) + 1));
	eLIBs_memcpy(file->path, path, sizeof(const char)*eLIBs_strlen(path));
    len = sizeof(buf);
    if (getFileNameFromPath(path, buf, &len) != NULL)
    {
		file->fileName = (char *)esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(buf) + 1));
		eLIBs_memset(file->fileName, 0x00, sizeof(const char)*(eLIBs_strlen(buf) + 1));
		eLIBs_memcpy(file->fileName, buf, sizeof(const char)*eLIBs_strlen(buf));
        DLOG("filename: %p, %s", file->fileName, file->fileName);
    }

    getFileInfo(file);

    INIT_LIST_HEAD(&file->list);

    file->parentDir = parentDir;

    file->renameTo = renameTo;

    DLOG("create File:%p", file);
    return file;
}

void FileRelease(struct File *file)
{
    DLOG("file: %p", file);
    if (!file)
    {
        return ;
    }
    DLOG("path:%p, %s", file->path, file->path);
    if (file->path)
    {
        esMEMS_Mfree(0, (void *)file->path);
    }
    DLOG("name:%p, %s", file->fileName, file->fileName);
    if (file->fileName)
    {
        esMEMS_Mfree(0, (void *)file->fileName);
    }
    esMEMS_Mfree(0, (void *)file);

    return ;
}
