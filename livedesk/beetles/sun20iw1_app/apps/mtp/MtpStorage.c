#include "MtpStorage.h"
#include "mtp.h"
#include "MtpCommon.h"
#include "elibs_stdio.h"
#include "elibs_string.h"

uint64_t MtpStorageGetFreeSpace(struct MtpStorage *storage)
{
	return storage->mFreeSpace;
}

struct MtpStorage *MtpStorageInit(MtpStorageID id, const char *path,
                                  const char *description,
                                  uint64_t reserveSpace,
                                  uint64_t maxFileSize)
{
    struct MtpStorage *mStorage = NULL;

    mStorage = (struct MtpStorage *)esMEMS_Malloc(0, sizeof(struct MtpStorage));

	eLIBs_memset(mStorage, 0x00, sizeof(struct MtpStorage));
    mStorage->mDisk = DiskInit(path);
    if (!mStorage->mDisk)
    {
        eLIBs_printf("mDisk init failed, path:%s\n", path);
        esMEMS_Mfree(0, mStorage);
        return NULL;
    }

	mStorage->mFilePath = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(path) + 1));
	eLIBs_memset(mStorage->mFilePath, 0x00, sizeof(const char)*(eLIBs_strlen(path) + 1));
    eLIBs_memcpy(mStorage->mFilePath, path, sizeof(const char)*eLIBs_strlen(path));

	mStorage->mDescription = esMEMS_Malloc(0, sizeof(const char)*(eLIBs_strlen(description) + 1));
	eLIBs_memset(mStorage->mDescription, 0x00, sizeof(const char)*(eLIBs_strlen(description) + 1));
    eLIBs_memcpy(mStorage->mDescription, description, sizeof(const char)*eLIBs_strlen(description));

    mStorage->mMaxCapacity = mStorage->mDisk->dMaxCap;
    mStorage->mFreeSpace = mStorage->mDisk->dFreeSpace;

    mStorage->mReserveSpace = reserveSpace;
    mStorage->mMaxFileSize = maxFileSize;

    mStorage->mStorageID = id;

    return mStorage;
}

void MtpStorageRelease(struct MtpStorage *mStorage)
{
    if (!mStorage)
    {
        return;
    }
    if (mStorage->mDisk)
    {
        DiskRelease(mStorage->mDisk);
    }
    if (mStorage->mFilePath)
    {
        esMEMS_Mfree(0, mStorage->mFilePath);
    }
    if (mStorage->mDescription)
    {
        esMEMS_Mfree(0, mStorage->mDescription);
    }
    esMEMS_Mfree(0, mStorage);
    return;
}
