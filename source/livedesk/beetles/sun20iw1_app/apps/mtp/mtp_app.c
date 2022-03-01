#include "MtpServer.h"
#include "Disk.h"
#include "common.h"
#include "mtp_app.h"
#include "mtp.h"

#define KB(size)    (size*1024)
#define MB(size)    (KB(size)*1024)

#define AID_MEDIA_RW    1023
static struct MtpStorage *storage;

typedef struct mtp_storage_tag
{
    struct MtpStorage *storage;
    uint32_t id;
    char path[256];
    char description[32];
    uint64_t reserve_space;
    uint64_t max_file_size;
	struct mtp_storage_tag *next;
} mtp_storage_t;


typedef struct
{
    struct MtpServer *server;
    mtp_storage_t *storage_array;
    uint32_t storage_array_size;
} mtp_handle_t;

static mtp_storage_t gStorageArray[] =
{
    {NULL, 65537, "e:", "Local Disk", 0, 0},
//	{NULL, 65539, "g:", "g:", 0, 0},
//	{NULL, 65540, "f:", "f:", 0, 0},
};

static mtp_handle_t gMtpHandle =
{
    .server = NULL,
    .storage_array = gStorageArray,
    .storage_array_size = sizeof(gStorageArray) / sizeof(mtp_storage_t),
};
static uint32_t hotplug_id = 65539, usbd_plug = 0;
static mtp_storage_t hotplug_storage_head = {NULL,};
extern void sendStoreAdded(struct MtpServer *mServer, MtpStorageID id); 
extern void sendStoreRemoved(struct MtpServer *mServer, MtpStorageID id);
static void hotplug_storage_add(mtp_storage_t *mtp_storage)
{
	mtp_storage_t *temp = &hotplug_storage_head;
	while (temp->next)
	{
		temp = temp->next;
	}
	temp->next = mtp_storage;
	mtp_storage->next = NULL;
}
static mtp_storage_t *hotplug_storage_find(unsigned char *path)
{
	mtp_storage_t *temp = hotplug_storage_head.next;
	while (temp)
	{
		if (eLIBs_strncmp(temp->path, path, eLIBs_strlen(path)) == 0)
		{
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}
static int hotplug_storage_del(mtp_storage_t *mtp_storage)
{
	mtp_storage_t *temp = &hotplug_storage_head;
	while (temp->next)
	{
		if (mtp_storage == temp->next)
		{
			temp->next = mtp_storage->next;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}
static int hotplug_storage_Mtpadd(void)
{	
	mtp_storage_t *hotplug_storage = hotplug_storage_head.next;
	while (hotplug_storage)
	{
		hotplug_storage->storage = MtpStorageInit(hotplug_storage->id, hotplug_storage->path, hotplug_storage->description,
											hotplug_storage->reserve_space, hotplug_storage->max_file_size);
		if (!hotplug_storage->storage)
		{
			
			ELOG("MtpStorageInit fail\n");
			eLIBs_free(hotplug_storage);
			hotplug_storage = NULL;
			return ;
		}
		/* Add storage into MtpServer */
		MtpServerAddStorage(hotplug_storage->storage, gMtpHandle.server);
		/* Add storage into MtpDatabase */
		MtpDataBaseAddStorage(hotplug_storage->storage, gMtpHandle.server->mDataBase);
		hotplug_storage = hotplug_storage->next;
	}
	return 0;
}

static int hotplug_storage_release(void)
{	
	mtp_storage_t *storage = &hotplug_storage_head.next;
	mtp_storage_t *temp = NULL;

	while (!storage)
	{
		temp = storage->next;
		sendStoreRemoved(gMtpHandle.server, storage->storage->mStorageID);
		MtpDataBaseDelStorage(storage->storage, gMtpHandle.server->mDataBase);
		MtpServerDelStorage(storage->storage, gMtpHandle.server);
		MtpStorageRelease(storage->storage);
		storage->storage = NULL;
		hotplug_storage_del(storage);
		eLIBs_free(storage);
		storage = temp;
	}
	return 0;
}
static int mtp_server_init(void)
{
    uint32_t i;

    DLOG("MtpServer init!\n");
    /* MtpServer init */
    gMtpHandle.server = MtpServerInit(AID_MEDIA_RW, 0664, 0775);
    if (!gMtpHandle.server)
    {
        return -1;
    }
    /* MtpStorage init */
    for (i = 0; i < gMtpHandle.storage_array_size; i++)
    {
        mtp_storage_t *storage = &gMtpHandle.storage_array[i];
        storage->storage = MtpStorageInit(storage->id, storage->path, storage->description,
											storage->reserve_space, storage->max_file_size);
        if (!storage->storage)
        {
            return -1;
        }
        /* Add storage into MtpServer */
        MtpServerAddStorage(storage->storage, gMtpHandle.server);
        /* Add storage into MtpDatabase */
        MtpDataBaseAddStorage(storage->storage, gMtpHandle.server->mDataBase);
    }
	hotplug_storage_Mtpadd();
    return 0;
}

static void mtp_server_exit(void)
{
    uint32_t i;
    DLOG("MtpServer exit!\n");
    if (!gMtpHandle.server)
    {
        return;
    }
    for (i = 0; i < gMtpHandle.storage_array_size; i++)
    {
        mtp_storage_t *storage = &gMtpHandle.storage_array[i];
        if (storage->storage != NULL)
        {
//			sendStoreRemoved(gMtpHandle.server, storage->storage->mStorageID);
			MtpDataBaseDelStorage(storage->storage, gMtpHandle.server->mDataBase);
			MtpServerDelStorage(storage->storage, gMtpHandle.server);
            MtpStorageRelease(storage->storage);
            storage->storage = NULL;
        }
    }
	hotplug_storage_release();
    MtpServerRelease(gMtpHandle.server);
    gMtpHandle.server = NULL;
}

static int melis_usb_uevent_detect(int socket)
{
#define UEVENT_MSG_LEN 1024
    char buf[UEVENT_MSG_LEN];
    int len, i = 0;

    memset(buf, 0, sizeof(buf));
    if (len < 1)
    {
        return -1;
    }
    if (len >= UEVENT_MSG_LEN)
    {
        return -1;
    }
    buf[len] = '\0';
    while (i < len)
    {
        int l = strlen(buf + i) + 1;
        if (strstr(&buf[i], "USB_STATE=CONNECTED"))
        {
            return MTP_STATE_INIT;
        }
        if (strstr(&buf[i], "USB_STATE=DISCONNECTED"))
        {
            return MTP_STATE_EXIT;
        }
        i += l;
    }

    return -1;
}
void usb_uevent_handle(Mtp_Command_t command, unsigned char partname)
{	
    int ret = -1;	
	unsigned char path[4] = {0};
	mtp_storage_t *hotplug_storage = NULL;
	unsigned char *equipment[3] = {"SDMMC-DISK", "SCSI_DISK", "USB_CDROM"};
	unsigned char equipment_name[MAX_FS_NAME_LEN] = {0};
    switch (command)
    {
        case MTP_STATE_INIT:
            if (gMtpHandle.server != NULL)
            {
                DLOG("mtp didn't release\n");
                return;
            }
            if (mtp_server_init() != 0)
            {
                DLOG("mtp server init failed\n");
                return ;
            }
            gMtpHandle.server->run(gMtpHandle.server);
			usbd_plug = 1;
            break;
        case MTP_STATE_EXIT:
			usbd_plug = 0;
            mtp_server_exit();
            break;
		case MTP_STATE_ADD:
			hotplug_storage = eLIBs_malloc(sizeof(mtp_storage_t));
			eLIBs_memset(hotplug_storage, 0x00, sizeof(mtp_storage_t));
			hotplug_storage->id = ++hotplug_id;
			DLOG("partname:%c\n", partname);
			eLIBs_sprintf(hotplug_storage->path, "%c:", partname);
			eLIBs_GetPTName(hotplug_storage->path, equipment_name);
			DLOG("equipment_name:%s\n", equipment_name);
			if (eLIBs_strncmp(equipment_name, "SDMMC-DISK", strlen("SDMMC-DISK")) == 0)
			{
				eLIBs_memcpy(hotplug_storage->description, "SDMMC", sizeof("SDMMC"));
			}
			else if (eLIBs_strncmp(equipment_name, "SCSI_DISK", strlen("SCSI_DISK")) == 0)
    		{
				eLIBs_memcpy(hotplug_storage->description, "USB_DISK", sizeof("USB_DISK"));
			}
			else
			{
				eLIBs_memcpy(hotplug_storage->description, "unkown", sizeof("unkown"));
			}
			DLOG("id:%d\n", hotplug_storage->id);
			DLOG("path:%s\n", hotplug_storage->path);
			DLOG("description:%s\n", hotplug_storage->description);
			hotplug_storage_add(hotplug_storage);
			if (usbd_plug == 0)
			{
				ELOG("mtp not init\n");
				return ;
			}
	        hotplug_storage->storage = MtpStorageInit(hotplug_storage->id, hotplug_storage->path, hotplug_storage->description,
												hotplug_storage->reserve_space, hotplug_storage->max_file_size);
	        if (!hotplug_storage->storage)
	        {
	        	
				ELOG("MtpStorageInit fail\n");
				eLIBs_free(hotplug_storage);
				hotplug_storage = NULL;
	            return ;
	        }
	        /* Add storage into MtpServer */
	        MtpServerAddStorage(hotplug_storage->storage, gMtpHandle.server);
	        /* Add storage into MtpDatabase */
	        MtpDataBaseAddStorage(hotplug_storage->storage, gMtpHandle.server->mDataBase);
			sendStoreAdded(gMtpHandle.server, hotplug_storage->storage->mStorageID);
			break;
		case MTP_STATE_DEL:		
			eLIBs_sprintf(path, "%c:", partname);
			DLOG("path:%s\n", path);
			hotplug_storage = hotplug_storage_find(path);
			if (hotplug_storage == NULL)
			{
				ELOG("path:%s not in mtp database\n", path);
				return ;
			}
			hotplug_storage_del(hotplug_storage);
			if (usbd_plug == 0)
			{
				ELOG("mtp not init\n");
				return ;
			}	
			sendStoreRemoved(gMtpHandle.server, hotplug_storage->storage->mStorageID);
			MtpDataBaseDelStorage(hotplug_storage->storage, gMtpHandle.server->mDataBase);
			MtpServerDelStorage(hotplug_storage->storage, gMtpHandle.server);
			MtpStorageRelease(hotplug_storage->storage);
			eLIBs_free(hotplug_storage);
			hotplug_storage = NULL;
			break;
		default:
			break;
    }
    return;
}

typedef struct
{
    uint32_t action;    /* 0:add, 1:remove, 2:update */
    uint32_t type;      /* 0:file, 2:dir */
    uint32_t srcPathLen;    /* src path buffer length */
    uint32_t destPathLen;   /* dest path buffer length */
    char *path;     /* object path */
} mtp_command_t;

#define MTP_FIFO_NAME   "/tmp/.mtp_fifo"

static void mtp_local_fs_update_handle(ES_FILE *fp)
{
    int ret;
    uint32_t id, i;
    char buf[512];
    char *srcPath = NULL, *destPath = NULL;
    mtp_command_t *command;
    struct MtpServer *server = NULL;
    struct MtpStorage *storage = NULL;
	MtpObjectFormat format = 0;
    mtp_tools_function_t func;
    enum
    {
        MTP_TOOLS_TYPE_FILE = 0,
        MTP_TOOLS_TYPE_DIR,
    } type;


    memset(buf, 0, sizeof(buf));
    ret = eLIBs_fread(buf, sizeof(char), sizeof(buf), fp);
    command = (mtp_command_t *)buf;
    DLOG("ret = %d, command: action:%u, type:%u, srcPathLen:%u, destPathLen:%u\n",
           ret, command->action, command->type, command->srcPathLen, command->destPathLen);

    if (ret < sizeof(mtp_command_t))
    {
        DLOG("invalid mtp command size!\n");
        return ;
    }

    command->path = (char *)&command[1];
    DLOG("sLen:%u, dLen:%u, path: %s\n", command->srcPathLen, command->destPathLen, command->path);

    if (command->srcPathLen > 1)
    {
        srcPath = command->path;
    }
    else
    {
        DLOG("invalid src path length!\n");
        return ;
    }
    if (command->destPathLen > 1)
    {
        destPath = command->path + command->srcPathLen;
    }

    func = command->action;

    if (command->type == MTP_TOOLS_TYPE_FILE)
    {
        format = MTP_FORMAT_UNDEFINED;
    }
    else if (command->type == MTP_TOOLS_TYPE_DIR)
    {
        format = MTP_FORMAT_ASSOCIATION;
    }

    for (i = 0; i < gMtpHandle.storage_array_size; i++)
    {
        char *p = gMtpHandle.storage_array[i].path;
        if (!strncmp(p, srcPath, strlen(p)))
        {
            id = gMtpHandle.storage_array[i].id;
            storage = gMtpHandle.storage_array[i].storage;
        }
    }

    server = gMtpHandle.server;
    if (!server)
    {
        DLOG("MtpServer not connect.");
        return;
    }

    switch (func)
    {
        case MTP_TOOLS_FUNCTION_ADD:
        case MTP_TOOLS_FUNCTION_REMOVE:
        case MTP_TOOLS_FUNCTION_UPDATE:
            ret = MtpToolsCommandControl(func, srcPath, format,
                                         server->mFileGroup, server->mFilePermission,
                                         id, storage,
                                         gMtpHandle.server->mDataBase);
            break;
        case MTP_TOOLS_FUNCTION_CUT:
            ret = MtpToolsCommandControl(MTP_TOOLS_FUNCTION_ADD, destPath, format,
                                         server->mFileGroup, server->mFilePermission,
                                         id, storage,
                                         gMtpHandle.server->mDataBase);
            ret += MtpToolsCommandControl(MTP_TOOLS_FUNCTION_REMOVE, srcPath, format,
                                          server->mFileGroup, server->mFilePermission,
                                          id, storage,
                                          gMtpHandle.server->mDataBase);
            break;
        case MTP_TOOLS_FUNCTION_COPY:
            ret = MtpToolsCommandControl(MTP_TOOLS_FUNCTION_ADD, destPath, format,
                                         server->mFileGroup, server->mFilePermission,
                                         id, storage,
                                         gMtpHandle.server->mDataBase);
            break;
        default:
            ret = -1;
            DLOG("unknow mtp tools function:%d\n", func);
            break;
    }
    if (ret < 0)
    {
        DLOG("handle mtp command failed\n");
    }
    return ;
}
