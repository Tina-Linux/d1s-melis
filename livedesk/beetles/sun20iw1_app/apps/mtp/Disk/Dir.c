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
#include <errno.h>
#include <dfs_posix.h>
#include "Dir.h"
#include "File.h"
#include "DiskCommon.h"
#include "elibs_stdio.h"
#include "elibs_string.h"
#include "common.h"
#include "MtpCommon.h"

static void getFileInfo(struct Dir *dir)
{
    __hdle fp;
    __ES_FSTAT fstat;
    fp = esFSYS_fopen(dir->path, "rb");
    if (!fp)
    {
        return ;
    }

	eLIBs_memset( &fstat, 0x00, sizeof(__ES_FSTAT));
    if (EPDK_OK != esFSYS_fstat(fp, &fstat))
    {
        return ;
    }


    dir->dirSize = fstat.size;
    dir->dirAccessTime.year         = fstat.atime.year;
    dir->dirAccessTime.month        = fstat.atime.month;
    dir->dirAccessTime.day          = fstat.atime.day;
    dir->dirAccessTime.hour         = fstat.atime.hour;
    dir->dirAccessTime.minute       = fstat.atime.minute;
    dir->dirAccessTime.second       = fstat.atime.second;
    dir->dirAccessTime.milliseconds = fstat.atime.milliseconds;

    dir->dirModifyTime.year         = fstat.mtime.year;
    dir->dirModifyTime.month        = fstat.mtime.month;
    dir->dirModifyTime.day          = fstat.mtime.day;
    dir->dirModifyTime.hour         = fstat.mtime.hour;
    dir->dirModifyTime.minute       = fstat.mtime.minute;
    dir->dirModifyTime.second       = fstat.mtime.second;
    dir->dirModifyTime.milliseconds = fstat.mtime.milliseconds;
    esFSYS_fclose(fp);

    DLOG(" <%s> size: %luM %luK %lubytes", dir->dirName, dir->dirSize >> 20, dir->dirSize >> 10, dir->dirSize);
    return ;
}

char *getFileNameFromPath(const char *path, char *name, size_t *size)
{
    char *ptr = NULL;
    ptr = eLIBs_strchrlast((char *)path, '\\');
    if (ptr != NULL)
    {
        size_t len = eLIBs_strlen(++ptr);
        if (len < 1 || len > *size)
        {
            goto err;
        }
        *size = len + 1;
        eLIBs_strcpy(name, ptr);
        return name;
    }
err:
//    eLIBs_printf("path unknown: %s\n", path);
    return NULL;
}

void updateDirInfo(struct Dir *dir)
{
    getFileInfo(dir);
}

void deleteDirInfo(struct Dir *dir)
{
    list_del(&dir->list);
}

static bool renameTo(const char *newPath, struct Dir *dir)
{
    int ret;
    char buf[128];
    size_t len = sizeof(buf);

    if (melis_access(newPath) == EPDK_OK)
    {
        return false;
    }
    ret = rename(dir->path, newPath);
    if (ret != 0)
    {
        eLIBs_printf("rename oldpath:%s to newpath:%s failed, errno:%d",
               dir->path, newPath, errno);
        return false;
    }
    if (dir->path)
    {
        esMEMS_Mfree(0, dir->path);
    }
	
    dir->path = (char *)esMEMS_Malloc(0, sizeof(const char)*eLIBs_strlen(newPath));
    eLIBs_memset(dir->path, 0x00, sizeof(const char)*eLIBs_strlen(newPath));
    eLIBs_memcpy(dir->path, newPath, sizeof(const char)*eLIBs_strlen(newPath));
    if (dir->dirName)
    {
        esMEMS_Mfree(0, dir->dirName);
    }
    if (getFileNameFromPath(newPath, buf, &len) != NULL)
    {
		dir->dirName = (char *)esMEMS_Malloc(0, sizeof(const char)*eLIBs_strlen(buf));
		eLIBs_memset(dir->dirName, 0x00, sizeof(const char)*eLIBs_strlen(buf));
		eLIBs_memcpy(dir->dirName, buf, sizeof(const char)*eLIBs_strlen(buf));
    }
    getFileInfo(dir);
    return true;
}

struct Dir *DirInit(const char *path, struct Dir *parentDir)
{
    struct Dir *dir;
    size_t len;
    char buf[128];

    if (!path)
    {
        return NULL;
    }
    DLOG(" path:%s", path);
    dir = (struct Dir *)esMEMS_Malloc(0, sizeof(struct Dir));
    eLIBs_memset(dir, 0, sizeof(struct Dir));

    dir->path = (char *)esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(path) + 1));
    eLIBs_memset(dir->path, 0x00, sizeof(const char)*(eLIBs_strlen(path) + 1));
    eLIBs_memcpy(dir->path, path, sizeof(const char)*eLIBs_strlen(path));

    len = sizeof(buf);
    if (getFileNameFromPath(path, buf, &len) != NULL)
    {
		dir->dirName = (char *)esMEMS_Malloc(0, sizeof(char)*(eLIBs_strlen(buf) + 1));
		eLIBs_memset(dir->dirName, 0x00, sizeof(char)*(eLIBs_strlen(buf) + 1));
		eLIBs_memcpy(dir->dirName, buf, sizeof(char)*eLIBs_strlen(buf));
    }

    getFileInfo(dir);
    INIT_LIST_HEAD(&dir->list);
    dir->parentDir = parentDir;

    dir->renameTo = renameTo;

    DLOG("init !! dir:%p", dir);
    return dir;
}

void DirRelease(struct Dir *dir)
{
    DLOG("exit !! dir:%p", dir);
    if (!dir)
    {
        return ;
    }
    if (dir->path)
    {
        esMEMS_Mfree(0, dir->path);
    }
    if (dir->dirName)
    {
        esMEMS_Mfree(0, dir->dirName);
    }
    esMEMS_Mfree(0, dir);
}
