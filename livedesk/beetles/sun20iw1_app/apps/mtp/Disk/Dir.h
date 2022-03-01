#ifndef _DIR_H
#define _DIR_H
#include "list.h"
#include "File.h"
#include "elibs_stdio.h"

typedef uint64_t DirSize;

struct Dir
{
    int objectNum;
    struct list_head list;
    char *path;
    char *dirName;
    uint32_t subDirCount;
    uint32_t subFileCount;

    DirSize dirSize;
    ES_TIMESTAMP dirAccessTime;
    ES_TIMESTAMP dirModifyTime;

    struct Dir *parentDir;
    void *object;

    bool (*renameTo)(const char *newPath, struct Dir *dir);
};

struct Dir *DirInit(const char *path, struct Dir *parentDir);
void DirRelease(struct Dir *dir);

uint32_t DirGetObjectCount(struct Dir *dir);
void updateDirInfo(struct Dir *dir);
void deleteDirInfo(struct Dir *dir);
#endif
