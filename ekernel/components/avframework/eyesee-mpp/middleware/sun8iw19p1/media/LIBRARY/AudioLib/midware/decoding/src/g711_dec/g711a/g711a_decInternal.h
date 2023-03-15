#ifndef __G711A_DECINTERNAL__
#define __G711A_DECINTERNAL__

#include "alib_typedef.h"
// #include "Decode_ac320com.h"
// #include <adecoder.h>
#include <ad_cedarlib_com.h>

typedef struct bs_stream_info_s
{
    int chl_num;
    int bit_depth;
    int sample_rate;
}BS_STREAM_INFO_S;

typedef struct g711a_dec_priv_s
{
    char *tmp_frm_buff;
    int tmp_frm_buff_size;

    BS_STREAM_INFO_S bs_strm_info;
}G711A_DEC_PIRV;


alib_int32 	G711aDecExit(struct __AudioDEC_AC320 *p);
alib_int32 	G711aDecInit(struct __AudioDEC_AC320 *p);
alib_int32	G711aframeDec(struct __AudioDEC_AC320 *p, alib_int8 *OutBuffer,alib_int32 *OutBuffLen);

#endif
