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
#ifndef _MTPOBJECTINFO_H
#define _MTPOBJECTINFO_H

#include "mtp.h"
#include "aw_list.h"
#include "Dir.h"
#include "File.h"
#include "MtpStorage.h"
#include "elibs_stdio.h"


typedef union
{
    struct Dir *dir;
    struct File *file;
} MtpObject;

struct MtpObjectInfo
{
    MtpObjectHandle mHandle;
    MtpStorageID mStorageID;
    MtpObjectFormat mFormat;
    MtpObjectHandle mParent;
    struct list_head mList;

    uint16_t mProtectionStatus;
    uint64_t mCompressedSize;
    MtpObjectFormat mThumbFormat;
    uint32_t mThumbCompressedSize;
    uint32_t mThumbPixWidth;
    uint32_t mThumbPixHeight;
    uint32_t mImagePixWidth;
    uint32_t mImagePixHeight;
    uint32_t mImagePixDepth;
    uint16_t mAssociationType;
    uint32_t mAssociationDesc;
    uint32_t mSequenceNumber;
    char *mName;
    ES_TIMESTAMP mDateCreated;
    ES_TIMESTAMP mDateModified;
    char *mKeywords;

    MtpObject object;

    void (*print)(struct MtpObjectInfo *info);
    void (*fillObject)(struct MtpStorage *mStorage, struct MtpObjectInfo *info);
};

struct MtpObjectInfo *MtpObjectInfoInitWithDir(MtpObjectHandle handle, struct Dir *dir);
struct MtpObjectInfo *MtpObjectInfoInitWithFile(MtpObjectHandle handle, struct File *file);
void MtpObjectInfoRelease(struct MtpObjectInfo *mObjectInfo);
void MtpObjectInfoListRelease(struct list_head *head);
void MtpObjectSetParent(MtpObjectHandle parent, struct MtpObjectInfo *object);
struct MtpObjectInfo *getMtpObjectByHandle(MtpObjectHandle handle, struct list_head *objectLists);
struct MtpObjectInfo *getMtpObjectByPath(const char *path, struct list_head *objectLists);
struct MtpObjectInfo *getMtpObjectByNameWithParent(const char *name, MtpObjectHandle parent, struct list_head *objectLists);
uint32_t *getMtpObjectHandlesList(MtpObjectHandle parent, size_t *arrayNum, struct list_head *objectLists);
void deleteObjectInfo(struct MtpObjectInfo *info);
void updateObjectInfo(struct MtpObjectInfo *info);
#endif
