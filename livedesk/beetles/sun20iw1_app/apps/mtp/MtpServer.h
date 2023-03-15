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
#ifndef _MTPSERVER_H
#define _MTPSERVER_H

#include "MtpPacket.h"
#include "MtpDataBase.h"
#include "MtpStorage.h"
#include "MtpCommon.h"
#include <stdbool.h>
#include <pthread.h>
#include <emodules/mod_defs.h>
#include <libc.h>
#include <drivers/sys_time.h>
#include <emodules/mod_orchid.h>
#include <emodules/mod_charset.h>
#include <emodules/mod_willow.h>
#include <emodules/mod_orange.h>
#include <emodules/mod_cedar.h>
#include <emodules/mod_audio.h>
#include "elibs/lib_ex.h"
enum {
	MTP_STATUS_RUN = 0,
	MTP_STATUS_EXIT
};
struct MtpServer
{
    int mFd;
    pthread_t mTid;
    int status;

    bool mPtp;
    bool mSessionOpen;
    MtpSessionID mSessionID;

    struct MtpPacket *mRequestPacket;
    struct MtpPacket *mResponsePacket;
    struct MtpPacket *mDataPacket;

    struct MtpDataBase *mDataBase;
    Vector mStorageList;

    Vector mObjectEditList;

    int mFileGroup;
    int mFilePermission;
    int mDirectoryPermission;


    MtpObjectHandle mSendObjectHandle;
    MtpObjectFormat mSendObjectFormat;
    char *mSendObjectFilePath;
    size_t mSendObjectFileSize;

    int (*run)(struct MtpServer *mServer);
    void (*send_object_added)(void);
    void (*send_object_removed)(void);
    void (*send_device_property_changed)(void);
    void (*add_storage)(void);
    void (*remove_storage)(void);
};

struct MtpServer *MtpServerInit(int fileGroup, int filePerm, int directoryPerm);
void MtpServerRelease(struct MtpServer *mServer);

void MtpServerAddStorage(struct MtpStorage *mStorage, struct MtpServer *mServer);
void MtpServerDelStorage(struct MtpStorage *mStorage, struct MtpServer *mServer);
struct MtpStorage *MtpServerFindStorage(unsigned char *path, struct MtpServer *mServer);
#endif
