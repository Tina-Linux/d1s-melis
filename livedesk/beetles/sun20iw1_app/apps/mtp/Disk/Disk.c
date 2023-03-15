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
#include "Disk.h"
#include "DiskCommon.h"
#include "elibs_stdio.h"
#include "elibs_string.h"
#include "aw_dirent.h"
#include "common.h"
#include <MtpCommon.h>

static void addSubDir(struct Dir *dir, struct Disk *disk)
{
    list_add(&dir->list, &disk->dirList);
    if (dir->parentDir)
    {
        dir->parentDir->subDirCount++;
    }
}

static void addSubFile(struct File *file, struct Disk *disk)
{
    list_add(&file->list, &disk->fileList);
    if (file->parentDir)
    {
        file->parentDir->subFileCount++;
    }
}

static int filter(const ES_DIRENT *dir_ent)
{
    if (eLIBs_strlen(dir_ent->d_name) == 1 && '.' == *dir_ent->d_name)
    {
        return 0;
    }
    if (eLIBs_strlen(dir_ent->d_name) == 2 && !eLIBs_strcmp(dir_ent->d_name, ".."))
    {
        return 0;
    }
    return 1;
}

static int getDirTreeInfo(struct Dir *dir, struct Disk *disk)
{
    ES_DIRENT *namelist = NULL;
    int num = 0, i = 0;
    char pathBuf[PATH_MAX] = {0};

    num = scandir_wrapper(dir->path, &namelist, filter, NULL);
    DLOG("num = %d", num);
    for (i = 0; i < num; i++)
    {
		DLOG("d_name = %s", (namelist + i)->d_name);
        if (eLIBs_strlen((namelist + i)->d_name) +
            eLIBs_strlen(dir->path) + 1 > PATH_MAX)
        {
            eLIBs_printf("path too long : %s %s\n", (namelist + i)->d_name, dir->path);
        }
        if ((namelist + i)->fatdirattr & FSYS_ATTR_DIRECTORY)
        {
            struct Dir *tmpDir = NULL;

            eLIBs_snprintf(pathBuf, sizeof(pathBuf), "%s\\%s", dir->path, (namelist + i)->d_name);
            DLOG("-------------------------------------------------------");
            tmpDir = DirInit(pathBuf, dir);
            addSubDir(tmpDir, disk);
            getDirTreeInfo(tmpDir, disk);
        }
        else 
        {
            struct File *tmpFile = NULL;

            eLIBs_snprintf(pathBuf, sizeof(pathBuf), "%s\\%s", dir->path, (namelist + i)->d_name);
            tmpFile = FileInit(pathBuf, dir);
            addSubFile(tmpFile, disk);
        }
    }
    if (namelist != NULL)
    {
        esMEMS_Mfree(0, namelist);
    }

    DLOG("subDirCount of <%s>: %u", dir->dirName, dir->subDirCount);
    DLOG("subFileCount of <%s>: %u", dir->dirName, dir->subFileCount);

    return 0;
}

extern const char *getDiskType(DiskType type);

static void getDiskInfo(struct Disk *disk)
{
	__ES_FSSTAT  stat;
    __u64       res;
	
    eLIBs_memset(&stat, 0, sizeof(__ES_FSSTAT));
    if (esFSYS_statfs(disk->dMountDir, (__hdle)&stat, (FSYS_KSTATUS_BLOCKS | FSYS_KSTATUS_BSIZE | FSYS_KSTATUS_BFREE)) != 0)
    {
		DLOG("esFSYS_statfs disk->dMountDir:%s\n", disk->dMountDir);
		return ;
	}

    DLOG("f_type:0x%04X, len:%u", stat.f_type, sizeof(stat.f_type));
    DLOG("f_bsize:%u, len:%u", stat.f_bsize, sizeof(stat.f_bsize));
    DLOG("f_blocks:%lu, len:%u", stat.f_blocks, sizeof(stat.f_blocks));
    DLOG("f_bfree:%lu, len:%u", stat.f_bfree, sizeof(stat.f_bfree));
    DLOG("f_files:%lu, len:%u", stat.f_files, sizeof(stat.f_files));
    DLOG("f_namelen:%u, len:%u", stat.f_namelen, sizeof(stat.f_namelen));

    DLOG("");
    disk->dFsType = stat.f_type;
    DLOG(" Filesystem Type: %s", getDiskType(disk->dFsType));
    disk->dMaxCap = (DiskMaxCapacity)((DiskMaxCapacity)stat.f_bsize * (DiskMaxCapacity)stat.f_blocks);
    DLOG(" disk->dMaxCap: %lu", disk->dMaxCap);
    disk->dFreeSpace = (DiskFreeSpace)((DiskFreeSpace)stat.f_bsize * (DiskFreeSpace)stat.f_bfree);
    DLOG(" disk->dFreeSpace: %lu", disk->dFreeSpace);
	return ;

}
struct Disk *DiskInit(const char *path)
{
    size_t len = 0;
    struct Disk *disk = NULL;

    disk = (struct Disk *)esMEMS_Malloc(0, sizeof(struct Disk));
    DLOG("disk:%x", disk);
	eLIBs_memset(disk, 0x00, sizeof(struct Disk));

	if (sizeof(const char)*eLIBs_strlen(path) > sizeof(char)*128) {
		len = sizeof(const char)*(eLIBs_strlen(path) + 1);
	} else {
		len = sizeof(char)*128;
	}
	disk->dMountDir = esMEMS_Malloc(0, len);
    DLOG("disk->dMountDir:%x", disk->dMountDir);
	eLIBs_memset(disk->dMountDir, 0x00, len);
	eLIBs_memcpy(disk->dMountDir, path, sizeof(const char)*eLIBs_strlen(path));
    INIT_LIST_HEAD(&disk->dirList);
    INIT_LIST_HEAD(&disk->fileList);

    disk->addSubDir = addSubDir;
    disk->addSubFile = addSubFile;
    getDiskInfo(disk);
    disk->dDirRoot = DirInit(disk->dMountDir, NULL);
    DLOG("======================================================");
    getDirTreeInfo(disk->dDirRoot, disk);
    DLOG("======================================================");

    return disk;
}

void  DiskRelease(struct Disk *disk)
{
    struct list_head *pos = NULL;
    struct list_head *head = &disk->dirList;
    struct list_head *headFile = &disk->fileList;

    if (!disk)
    {
        return ;
    }

    DLOG("here");
    DLOG("disk:%x", disk);
    DLOG("disk->dMountDir:%x", disk->dMountDir);
    if (disk->dMountDir)
    {
		DLOG("here");
        esMEMS_Mfree(0, disk->dMountDir);
        disk->dMountDir = NULL;
    }
    DLOG("here");

    if (disk->dDirRoot)
    {
        DirRelease(disk->dDirRoot);
    }

    DLOG("head:%p", head);
#if 0
    list_for_each(pos, head)
    {
        struct Dir *dir = NULL;

        dir = list_entry(pos, struct Dir, list);
        //deleteDirInfo(dir);
        DLOG("dirName: %s", dir->dirName);
    }
#endif
    while (!list_empty(head))
    {
        struct Dir *dir = NULL;
        dir = list_first_entry(head, struct Dir, list);
        DLOG("dir name:%s, list:%p, dir:%p", dir->dirName, &dir->list, dir);
        list_del(&dir->list);
        DirRelease(dir);
    }
    while (!list_empty(headFile))
    {
        struct File *file = NULL;
        file = list_first_entry(headFile, struct File, list);
        DLOG("file name:%s, list:%p", file->fileName, &file->list);
        list_del(&file->list);
        FileRelease(file);
    }
    esMEMS_Mfree(0, disk);
}
