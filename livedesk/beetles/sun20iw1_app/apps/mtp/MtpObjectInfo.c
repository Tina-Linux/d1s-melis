#include "MtpObjectInfo.h"
#include "MtpCommon.h"
#include "common.h"
#include "elibs_string.h"

struct MtpObjectInfo *getMtpObjectByNameWithParent(const char *name, MtpObjectHandle parent, struct list_head *objectLists)
{
    struct MtpObjectInfo *object = NULL;
    struct list_head *pos = NULL;
    struct list_head *head = objectLists;

    list_for_each(pos, head)
    {
        object = list_entry(pos, struct MtpObjectInfo, mList);
        DLOG("object parent:%u", object->mParent);
        if (object->mParent == parent)
        {
            if (!eLIBs_strcmp(name, object->mName))
            {
                DLOG("found object(%s) with parent:%u", object->mName, parent);
                return object;
            }
        }
    }

    return NULL;
}

uint32_t *getMtpObjectHandlesList(MtpObjectHandle parent, size_t *arrayNum, struct list_head *objectLists)
{
    struct MtpObjectInfo *object = NULL;
    struct list_head *pos = NULL;
    struct list_head *head = objectLists;
    uint32_t *array = NULL;
    size_t num = 0;
#define OBJECHANDLESTLIST_ARRAY_NUM 10
    size_t defaultNum = OBJECHANDLESTLIST_ARRAY_NUM;

    array = (uint32_t *)esMEMS_Malloc(0, defaultNum * sizeof(uint32_t));
    if (!array)
    {
        eLIBs_printf("malloc failed\n");
        return NULL;
    }
	eLIBs_memset(array, 0x00, defaultNum * sizeof(uint32_t));
#if 0
    list_for_each(pos, head)
    {
        object = list_entry(pos, struct MtpObjectInfo, mList);
        DLOG("object: name(%s) handle(%u) parent(%u)", object->mName, object->mHandle, object->mParent);
    }
#endif
    list_for_each(pos, head)
    {
        object = list_entry(pos, struct MtpObjectInfo, mList);
        DLOG("object: name(%s) handle(%u) parent(%u)", object->mName, object->mHandle, object->mParent);
        if (object->mParent == parent)
        {
            num++;
            if (num > defaultNum)
            {
                uint32_t *array_tmp = array;
                defaultNum += OBJECHANDLESTLIST_ARRAY_NUM;
                array = realloc_wrapper(array, defaultNum * sizeof(uint32_t));
                if (!array)
                {
                    esMEMS_Mfree(0, array_tmp);
                    return NULL;
                }
            }
            DLOG("add handle:%u", object->mHandle);
            array[num - 1] = object->mHandle;
        }
    }
    DLOG("found (parent:%u) ObjectHandlesList num:%u", parent, num);
    *arrayNum = num;

    return array;
}

struct MtpObjectInfo *getMtpObjectByPath(const char *path, struct list_head *objectLists)
{
    struct MtpObjectInfo *object;
    struct list_head *pos = NULL;
    struct list_head *head = objectLists;

    list_for_each(pos, head)
    {
        object = list_entry(pos, struct MtpObjectInfo, mList);
        DLOG("search path, object name:%s", object->mName);
        if (object->mFormat == MTP_FORMAT_ASSOCIATION)
        {
            struct Dir *dir = object->object.dir;
            if (!eLIBs_strcmp(path, dir->path))
            {
                DLOG("found path:%s", dir->path);
                object->print(object);
                return object;
            }
        }
        else
        {
            struct File *file = object->object.file;
            if (!eLIBs_strcmp(path, file->path))
            {
                DLOG("found path:%s", file->path);
                object->print(object);
                return object;
            }
        }
    }

    return NULL;
}

struct MtpObjectInfo *getMtpObjectByHandle(MtpObjectHandle handle, struct list_head *objectLists)
{
    struct MtpObjectInfo *object;
    struct list_head *pos = NULL;
    struct list_head *head = objectLists;

    list_for_each(pos, head)
    {
        object = list_entry(pos, struct MtpObjectInfo, mList);
        DLOG("search handle, object handle:%u", object->mHandle);
        if (object->mHandle == handle)
        {
            DLOG("found handle:%u", handle);
            object->print(object);
            return object;
        }
    }

    return NULL;
}

void MtpObjectSetParent(MtpObjectHandle parent, struct MtpObjectInfo *object)
{
    object->mParent = parent;
}

static void print(struct MtpObjectInfo *info)
{
#if 0
    DLOG("");
    DLOG("MtpObject Info 0x%x: %s", info->mHandle, info->mName);
    DLOG("StorageID: 0x%x mFormat: 0x%x mProtectionStatus: 0x%0x",
         info->mStorageID, info->mFormat, info->mProtectionStatus);
    DLOG("mCompressedSize: %lu mThumbFormat: 0x%x mThumbCompressedSize: %u",
         info->mCompressedSize, info->mThumbFormat, info->mThumbCompressedSize);
    DLOG("mThumbPixWidth: %u mThumbPixHeight: %u",
         info->mThumbPixWidth, info->mThumbPixHeight);
    DLOG("mImagePixWidth: %u mImagePixHeight: %u",
         info->mImagePixWidth, info->mImagePixHeight);
    DLOG("mParent: 0x%x mAsssociationType: 0x%x mAssociationDesc 0x%x",
         info->mParent, info->mAssociationType, info->mAssociationDesc);
    DLOG("mSequenceNumber %u mDateCreated %ld mDateModified: %ld mKeywords: %s",
         info->mSequenceNumber, info->mDateCreated, info->mDateModified, info->mKeywords);
#endif	
}

static void fillObject(struct MtpStorage *mStorage, struct MtpObjectInfo *info)
{
    info->mStorageID = mStorage->mStorageID;
}

void updateObjectInfo(struct MtpObjectInfo *info)
{
    if (info->mName)
    {
        esMEMS_Mfree(0, info->mName);
    }
    if (info->mFormat == MTP_FORMAT_ASSOCIATION)
    {
        struct Dir *dir = info->object.dir;
		info->mName = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(dir->dirName) + 1));
		eLIBs_memset(info->mName, 0x00, sizeof(const char)*(eLIBs_strlen(dir->dirName) + 1));
		eLIBs_memcpy(info->mName, dir->dirName, sizeof(const char)*eLIBs_strlen(dir->dirName));
	
		eLIBs_memset(&info->mDateCreated, 0x00, sizeof(ES_TIMESTAMP));
		eLIBs_memcpy(&info->mDateCreated, &dir->dirAccessTime, sizeof(ES_TIMESTAMP));
		eLIBs_memset(&info->mDateModified, 0x00, sizeof(ES_TIMESTAMP));
		eLIBs_memcpy(&info->mDateModified, &dir->dirModifyTime, sizeof(ES_TIMESTAMP));
    }
    else
    {
        struct File *file = info->object.file;
		info->mName = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(file->fileName) + 1));
		eLIBs_memset(info->mName, 0x00, sizeof(const char)*(eLIBs_strlen(file->fileName) + 1));
		eLIBs_memcpy(info->mName, file->fileName, sizeof(const char)*eLIBs_strlen(file->fileName));

		eLIBs_memset(&info->mDateCreated, 0x00, sizeof(ES_TIMESTAMP));
		eLIBs_memcpy(&info->mDateCreated, &file->fileAccessTime, sizeof(ES_TIMESTAMP));
		eLIBs_memset(&info->mDateModified, 0x00, sizeof(ES_TIMESTAMP));
		eLIBs_memcpy(&info->mDateModified, &file->fileAccessTime, sizeof(ES_TIMESTAMP));
        info->mCompressedSize = file->fileSize;
    }
}

void deleteObjectInfo(struct MtpObjectInfo *info)
{
    list_del(&info->mList);
}

struct MtpObjectInfo *MtpObjectInfoInitWithDir(MtpObjectHandle handle, struct Dir *dir)
{
    struct MtpObjectInfo *mObjectInfo = NULL;

    mObjectInfo = (struct MtpObjectInfo *)esMEMS_Malloc(0, sizeof(struct MtpObjectInfo));
	eLIBs_memset(mObjectInfo, 0x00, sizeof(struct MtpObjectInfo));

    mObjectInfo->mHandle = handle;
    mObjectInfo->mFormat = MTP_FORMAT_ASSOCIATION;

    INIT_LIST_HEAD(&mObjectInfo->mList);

    dir->object = mObjectInfo;

	mObjectInfo->mName = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(dir->dirName) + 1));
	eLIBs_memset(mObjectInfo->mName, 0x00, sizeof(const char)*(eLIBs_strlen(dir->dirName) + 1));
	eLIBs_memcpy(mObjectInfo->mName, dir->dirName, sizeof(const char)*eLIBs_strlen(dir->dirName));

	eLIBs_memset(&mObjectInfo->mDateCreated, 0x00, sizeof(ES_TIMESTAMP));
    eLIBs_memcpy(&mObjectInfo->mDateCreated, &dir->dirAccessTime, sizeof(ES_TIMESTAMP));
	eLIBs_memset(&mObjectInfo->mDateModified, 0x00, sizeof(ES_TIMESTAMP));
    eLIBs_memcpy(&mObjectInfo->mDateModified, &dir->dirModifyTime, sizeof(ES_TIMESTAMP));
    mObjectInfo->mCompressedSize = dir->dirSize;

    mObjectInfo->print = print;
    mObjectInfo->fillObject = fillObject;

    mObjectInfo->object.dir = dir;

    DLOG("init !!!object:%p, name:%s!!!", mObjectInfo, mObjectInfo->mName);
    return mObjectInfo;
}

struct MtpObjectInfo *MtpObjectInfoInitWithFile(MtpObjectHandle handle, struct File *file)
{
    struct MtpObjectInfo *mObjectInfo = NULL;

    mObjectInfo = (struct MtpObjectInfo *)esMEMS_Malloc(0, sizeof(struct MtpObjectInfo));
	eLIBs_memset(mObjectInfo, 0x00, sizeof(struct MtpObjectInfo));

    mObjectInfo->mHandle = handle;
    /* TODO according file->fileType */
    mObjectInfo->mFormat = MTP_FORMAT_TEXT;
    INIT_LIST_HEAD(&mObjectInfo->mList);

    file->object = mObjectInfo;

	DLOG("file->fileName:%s", file->fileName);
	mObjectInfo->mName = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(file->fileName) + 1));
	eLIBs_memset(mObjectInfo->mName, 0x00, sizeof(const char)*(eLIBs_strlen(file->fileName) + 1));
	eLIBs_memcpy(mObjectInfo->mName, file->fileName, sizeof(const char)*eLIBs_strlen(file->fileName));

    mObjectInfo->mDateCreated = file->fileAccessTime;
    mObjectInfo->mDateModified = file->fileModifyTime;
    mObjectInfo->mCompressedSize = file->fileSize;

    mObjectInfo->print = print;
    mObjectInfo->fillObject = fillObject;

    mObjectInfo->object.file = file;
    DLOG("init !!!object:%p!!!", mObjectInfo);
    return mObjectInfo;
}

void MtpObjectInfoRelease(struct MtpObjectInfo *mObjectInfo)
{
    DLOG("exit !!!object:%p!!!", mObjectInfo);
    if (!mObjectInfo)
    {
        return;
    }
    if (mObjectInfo->mName)
    {
        esMEMS_Mfree(0, mObjectInfo->mName);
    }
    esMEMS_Mfree(0, mObjectInfo);
}

void MtpObjectInfoListRelease(struct list_head *head)
{

    while (!list_empty(head))
    {
        struct MtpObjectInfo *object = NULL;
        object = list_first_entry(head, struct MtpObjectInfo, mList);
        DLOG("object name:%s, list:%p, object:%p", object->mName, &object->mList, object);
        list_del(&object->mList);
        MtpObjectInfoRelease(object);
    }
    esMEMS_Mfree(0, head);
}
