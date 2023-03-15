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
