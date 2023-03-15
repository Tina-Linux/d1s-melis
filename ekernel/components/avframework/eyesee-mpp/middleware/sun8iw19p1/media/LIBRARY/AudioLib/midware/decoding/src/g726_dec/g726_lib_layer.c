

#include "alib_log.h"
#include "alib_mem.h"
#include "alib_typedef.h"
#include "alib_utils.h"
#include <adecoder.h>
#include <ad_cedarlib_com.h>

// #include "Decode_ac320com.h"
#include "g726_decInternal.h"

#include "g726_export_api.h"

#define ALIB_DEBUG 1
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "g726 Decoder"
#endif



struct __AudioDEC_AC320 *LibLayerDecInit(void)
{
	struct  __AudioDEC_AC320  *ADECAC320 ;

	ADECAC320 = (struct __AudioDEC_AC320 *)alib_malloc(sizeof(struct __AudioDEC_AC320));
	if(!ADECAC320)
	    return 0;
	alib_memset(ADECAC320, 0, sizeof(struct __AudioDEC_AC320));

	ADECAC320->DecInit = G726DecInit;
    ADECAC320->DecFrame = G726frameDec; 
	ADECAC320->DecExit = G726DecExit;
	alib_loge("g726_dec_init successs...");

	return ADECAC320;
}

alib_int32 LibLayerDecExit(struct __AudioDEC_AC320 *p)
{
	alib_free(p);
	alib_logd("g726_dec_exit successs...");

	return 0;
}

