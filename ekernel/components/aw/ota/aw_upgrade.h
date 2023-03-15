#ifndef __AW_UPGRADE_H__
#define __AW_UPGRADE_H__

#ifdef __cplusplus
extern "C"{
#endif

//#define UPGRADE_APP_VERSION "v7_20191123"

#define UPGRADE_SLICE_RETRY (2)
#define UPGRADE_SLICE_ACTION_WRITE (1)
#define UPGRADE_SLICE_ACTION_VERIFY (2)
#define CHUNK (16384)
//#define CHUNK (10272)

#define UPGRADE_FROM_FILE (1)
#define UPGRADE_FROM_BUFFER (2)

#define DEFAULT_SOURCE_FILE "/data/update/update.bin"

int aw_upgrade_slice(uint8_t* target,
		      uint8_t* buffer,
		      uint32_t offset,
		      uint32_t size,
		      uint32_t flag);

int aw_upgrade_end(uint32_t flag);

#ifdef __cplusplus
}
#endif

#endif
