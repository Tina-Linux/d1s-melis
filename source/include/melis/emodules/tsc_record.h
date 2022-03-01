#ifndef _TSC_RECORD_H_
#define _TSC_RECORD_H_

//#include "../../../../eMod/mod_cedar/includes/tsc_interface.h"
#include <mod_defs.h>

extern __s32 TscRecordInit(void);
extern __s32 TscRecordDeInit(void);
extern __s32 TscRecordTune(__u32 freq_khz, __u32 bw_khz, __pCBK_t lock_cb);
extern __s32 TscRecordStart(int cache_size, const char *filepath);
extern __s32 TscRecordStop(void);



#endif  /* _TSC_RECORD_H_ */

