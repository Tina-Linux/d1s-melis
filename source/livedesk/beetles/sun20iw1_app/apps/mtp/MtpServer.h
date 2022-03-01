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
