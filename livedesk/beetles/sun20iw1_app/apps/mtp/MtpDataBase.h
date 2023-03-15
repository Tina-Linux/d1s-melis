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
#ifndef _MTPDATABASE_H
#define _MTPDATABASE_H

#include "MtpTypes.h"
#include "MtpPacket.h"
#include "MtpObjectInfo.h"
#include "MtpStorage.h"
#include "MtpCommon.h"


struct MtpDataBase
{

    MtpDevicePropertyList *mDeviceProperties;
    MtpObjectFormatList *mCaptureFormats;
    MtpObjectFormatList *mPlaybackFormats;

    Vector mStorageList;

    Vector mRootObjectInfoList;

    struct MtpProperty *(*getDevicePropertyDesc)(MtpDeviceProperty property);
    struct MtpProperty *(*getObjectPropertyDesc)(MtpDeviceProperty property, MtpObjectFormat format);
    MtpResponseCode(*getDevicePropertyValue)(MtpDeviceProperty property, struct MtpPacket *mData);

};

struct MtpDataBase *MtpDataBaseInit(void);
void MtpDataBaseRelease(struct MtpDataBase *mDataBase);
MtpResponseCode MtpDataBaseGetObjectInfo(MtpObjectHandle handle, struct MtpObjectInfo *object, struct MtpDataBase *mDataBase);
void MtpDataBaseAddStorage(struct MtpStorage *mStorage, struct MtpDataBase *mDataBase);
void MtpDataBaseDelStorage(struct MtpStorage *mStorage, struct MtpDataBase *mDataBase);
uint32_t *MtpDataBaseGetObjectHandlesList(MtpStorageID id,
        MtpObjectFormat format,
        MtpObjectHandle parent,
        size_t *arrayNum,
        struct MtpDataBase *mDataBase);
MtpResponseCode MtpDataBaseGetObjectFilePath(MtpObjectHandle handle,
        char *pathBuf, size_t pathBufLen,
        uint64_t *fileLenth,
        MtpObjectFormat *format,
        struct MtpDataBase *mDataBase);
void MtpDataBaseEndSendObject(const char *path, MtpObjectHandle handle,
                              MtpObjectFormat format, bool succeeded,
                              struct MtpDataBase *mDataBase);
MtpResponseCode MtpDataBaseDeleteFile(MtpObjectHandle handle, struct MtpDataBase *mDataBase);

MtpObjectHandle MtpDataBaseBeginSendObject(const char *path, MtpObjectFormat format,
        MtpObjectHandle parent,
        MtpStorageID storage,
        uint64_t size,
        ES_TIMESTAMP *modified,
        struct MtpDataBase *mDataBase);
MtpObjectHandle MtpDataBaseSetObjectPropertyValue(MtpObjectHandle handle,
        MtpObjectProperty property,
        struct MtpPacket *mData,
        struct MtpDataBase *mDataBas);
typedef enum
{
    MTP_TOOLS_FUNCTION_ADD = 0,
    MTP_TOOLS_FUNCTION_REMOVE,
    MTP_TOOLS_FUNCTION_UPDATE,
    MTP_TOOLS_FUNCTION_CUT,
    MTP_TOOLS_FUNCTION_COPY,
} mtp_tools_function_t;

void deletePath(const char *path);
int MtpToolsCommandControl(mtp_tools_function_t contrl, const char *path, MtpObjectFormat format,
                           int group, int permission,
                           MtpStorageID storageID, struct MtpStorage *storage,
                           struct MtpDataBase *mDataBase);
#endif
