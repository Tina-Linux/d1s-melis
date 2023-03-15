

#include "alib_log.h"
#include "alib_mem.h"
#include "alib_typedef.h"
#include "alib_utils.h" 
#include <adecoder.h>
#include <ad_cedarlib_com.h>

// #include "Decode_ac320com.h"
#include "g711codec.h"
#include "g711u_decInternal.h"

#define G711U_MAX_FRM_SIZE_BYTES (1024*2*2)     // max_frm_samples*max_frm_num*bytes_per_sample

alib_int32 G711uDecInit(struct __AudioDEC_AC320 *p)
{
    G711U_DEC_PIRV *g711u_priv = NULL;

    g711u_priv = (G711U_DEC_PIRV *)alib_malloc(sizeof(G711U_DEC_PIRV));
    if(NULL == g711u_priv)
    {
        return -1;
    }
    alib_memset(g711u_priv,0,sizeof(g711u_priv));

    g711u_priv->tmp_frm_buff_size = G711U_MAX_FRM_SIZE_BYTES;
    g711u_priv->tmp_frm_buff = (char *)alib_malloc(g711u_priv->tmp_frm_buff_size);
    if(NULL == g711u_priv->tmp_frm_buff)
    {
       alib_free(g711u_priv); 
        return -1;
    }
    alib_memset(g711u_priv->tmp_frm_buff,0,g711u_priv->tmp_frm_buff_size);
    
	p->DecoderCommand->pdecInfoSet = (alib_uint32 *)g711u_priv;
	

    CHECK_EXTERN_RW_API(p);

    alib_loge("g711u_dec_inited");

	return 0;
}


alib_int32  G711uDecExit(struct __AudioDEC_AC320 *p)
{
    G711U_DEC_PIRV *g711u_priv = (G711U_DEC_PIRV *)p->DecoderCommand->pdecInfoSet;

    if(NULL != g711u_priv)
    {
        if(NULL != g711u_priv->tmp_frm_buff)
        {
            alib_free(g711u_priv->tmp_frm_buff);
            g711u_priv->tmp_frm_buff_size = 0;
        }
        alib_free(g711u_priv);
    }

    alib_loge("g711u_dec_exited");

    return 0;
}

alib_int32 G711uframeDec(struct __AudioDEC_AC320 *p, alib_int8 *OutBuffer, alib_int32 *OutBuffLen)
{
    alib_int32 framelen = 0;
    alib_int32 nRead = 0;
    alib_int32 dec_size = 0;
    G711U_DEC_PIRV *g711u_priv = (G711U_DEC_PIRV *)p->DecoderCommand->pdecInfoSet;
    
    p->BsInformation->Samplerate = 8000;
	p->BsInformation->chan = 1;
	p->BsInformation->bitpersample = 16;

    if(0 == p->FileReadInfo->frmFifo->ValidchunkCnt)
    { 
		if(p->DecoderCommand->ulExitMean == 1)
		{
            alib_logw("g711u_dec_no_bs");
			return ERR_AUDIO_DEC_NO_BITSTREAM;
		}
		else
		{
			return ERR_AUDIO_DEC_ABSEND;
		}
    }
    framelen = p->FileReadInfo->frmFifo->inFrames[p->FileReadInfo->frmFifo->rdIdx].len;
    nRead = p->FileReadInfo->FREAD_AC320((void *)(g711u_priv->tmp_frm_buff),framelen,p->FileReadInfo);
    if(nRead != framelen && framelen != 0)
    {
		if(p->DecoderCommand->ulExitMean == 1)
		{
            alib_loge("g711u_dec_no_bs_rq:%d-rt:%d",framelen,nRead);
			return ERR_AUDIO_DEC_NO_BITSTREAM;
		}
		else
		{
			return ERR_AUDIO_DEC_ABSEND;
		}
    }

    dec_size = G711u2PCM((char *)g711u_priv->tmp_frm_buff, (char *)OutBuffer, nRead, 0);

    *OutBuffLen = dec_size;

    return 0;
}
