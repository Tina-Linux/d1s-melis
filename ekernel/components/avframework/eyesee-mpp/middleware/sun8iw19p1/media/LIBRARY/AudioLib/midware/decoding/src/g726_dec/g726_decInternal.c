#include "alib_log.h"
#include "alib_mem.h"
#include "alib_typedef.h"
#include "alib_utils.h" 
#include <adecoder.h>
#include <ad_cedarlib_com.h>

// #include "Decode_ac320com.h"
#include "g711.h"
#include "g726.h" 
#include "g726_decInternal.h"

#define G726_MAX_FRM_SIZE_BYTES (1024*2*2) // max_frm_samples*max_frm_num*bytes_per_sample

static int g726decode_func(g726_dec_handle__ * dec_handle, unsigned char *puc_input_g726,
                            	int input_len, short *out_pcm_buf)
{
	int isamLen = 0;//dec_handle->sample_proc;  // 解码后长度
	int i = 0;

	short sbitstream = 0;  // 数据容器

	unsigned int uiesidue      = 0;  // 数据有效bit
	unsigned int uitemp        = 0;

	short asG726_enc[SAMPLE_NUM]    = { 0 };
	short temp_compress[SAMPLE_NUM] = { 0 };


	if ((NULL == dec_handle) ||
		(NULL == puc_input_g726) ||
		(NULL == out_pcm_buf))  
	{
		printf("error parameter!! \n");
		return 0;
	}
	// param 检查
	if ((dec_handle->rate != 2) &&
		(dec_handle->rate != 3) &&
		(dec_handle->rate != 4) &&
		(dec_handle->rate != 5))
	{
		printf(" Invalid rate (5/4/3/2) or (40/32/24/16)! Aborted...\n");
		return 0;
	}

	isamLen = 0;

	for (i = 0; i < input_len; i++)
	{
		sbitstream = sbitstream << 8 | puc_input_g726[i];  //填充数据
		uiesidue += 8;
		while (uiesidue >= dec_handle->rate)
		{
			asG726_enc[isamLen++] = (unsigned char) (sbitstream >> (uiesidue - dec_handle->rate));  //  取sbitstream 高 rate位
			uiesidue -= dec_handle->rate;
			switch (uiesidue)
			{
				case 0 :sbitstream = sbitstream & 0x0000; break;  //没有有效位,需要清空buff特别注意!!!!
				case 1 :sbitstream = sbitstream & 0x0001; break;
				case 2 :sbitstream = sbitstream & 0x0003; break;
				case 3 :sbitstream = sbitstream & 0x0007; break;
				case 4 :sbitstream = sbitstream & 0x000F; break;
				case 5 :sbitstream = sbitstream & 0x001F; break;
				case 6 :sbitstream = sbitstream & 0x003F; break;
				case 7 :sbitstream = sbitstream & 0x007F; break;
				case 8 :sbitstream = sbitstream & 0x00FF; break;
				case 9 :sbitstream = sbitstream & 0x01FF; break;
				case 10:sbitstream = sbitstream & 0x03FF; break;
				case 11:sbitstream = sbitstream & 0x07FF; break;
				case 12:sbitstream = sbitstream & 0x0FFF; break;
				case 13:sbitstream = sbitstream & 0x1FFF; break;
				case 14:sbitstream = sbitstream & 0x3FFF; break;
				case 15:sbitstream = sbitstream & 0x7FFF; break;
				case 16:sbitstream = sbitstream & 0xFFFF; break;
				default:break;
			}
		}
	}

//	if (isamLen != dec_handle->sample_proc)
//	{
//		printf("error!!! isamLen = %d\n", isamLen);
//	}

	G726_decode(asG726_enc,
				temp_compress,
				isamLen,
				&(dec_handle->law),
				dec_handle->rate,
				dec_handle->reset,
				&(dec_handle->decoder_state));

	if ('1' == dec_handle->law)		//a率  将pcm数据进行a解压缩,u
	{
		alaw_expand(isamLen, temp_compress, out_pcm_buf);
	}
	else
	{
		ulaw_expand(isamLen, temp_compress, out_pcm_buf);
	}
	return isamLen;
} 

alib_int32 G726DecInit(struct __AudioDEC_AC320 *p)
{
    G726_DEC_PIRV *g726_priv = NULL;
    g726_dec_handle__ *g726_dec_hdl = NULL;

    g726_priv = (G726_DEC_PIRV *)alib_malloc(sizeof(G726_DEC_PIRV));
    if(NULL == g726_priv)
    {
        return -1;
    }
    alib_memset(g726_priv,0,sizeof(g726_priv));

    g726_priv->tmp_frm_buff_size = G726_MAX_FRM_SIZE_BYTES;
    g726_priv->tmp_frm_buff = (char *)alib_malloc(g726_priv->tmp_frm_buff_size);
    if(NULL == g726_priv->tmp_frm_buff)
    {
       alib_free(g726_priv); 
        return -1;
    }
    alib_memset(g726_priv->tmp_frm_buff,0,g726_priv->tmp_frm_buff_size);

    g726_dec_hdl = (g726_dec_handle__ *)malloc(sizeof(g726_dec_handle__));
    if(NULL == g726_dec_hdl)
    {
        if(NULL != g726_priv->tmp_frm_buff)
        {
            free(g726_priv->tmp_frm_buff);
        }

        if(NULL != g726_priv)
        {
            free(g726_priv);
        }
        return -1;
    }

    switch(p->BsInformation->bitrate)
    {
        case 16000:
        {
            g726_dec_hdl->rate = 2;
            break;
        }
        case 24000:
        {
            g726_dec_hdl->rate = 3;
            break;
        }
        case 32000:
        {
            g726_dec_hdl->rate = 4;
            break;
        }
        case 40000:
        {
            g726_dec_hdl->rate = 5;
            break;
        }
        default:
        {
            alib_loge("g726_dec_invalid_bitrate:%d",p->BsInformation->bitrate);
            g726_dec_hdl->rate = 4;
            break;
        } 
    }

    if(1 == p->BsInformation->g726_enc_law)
    {
        g726_dec_hdl->law = '1';
    }
    else if(0 == p->BsInformation->g726_enc_law)
    {
        g726_dec_hdl->law = '0';
    }
    else
    {
        alib_loge("g726_dec_invalid_law:%d",p->BsInformation->g726_enc_law);
        g726_dec_hdl->law = '1';
    }
	g726_dec_hdl->reset = 1;

    g726_priv->g726_dec_hdl = (void *)g726_dec_hdl;
    
	p->DecoderCommand->pdecInfoSet = (alib_uint32 *)g726_priv; 

    CHECK_EXTERN_RW_API(p);

    alib_logw("g726_dec_inited:%d-%d",p->BsInformation->bitrate,p->BsInformation->g726_enc_law);
	return 0;
}


alib_int32  G726DecExit(struct __AudioDEC_AC320 *p)
{
    G726_DEC_PIRV *g726_priv = (G726_DEC_PIRV *)p->DecoderCommand->pdecInfoSet;

    if(NULL != g726_priv)
    {
        if(NULL != g726_priv->tmp_frm_buff)
        {
            alib_free(g726_priv->tmp_frm_buff);
            g726_priv->tmp_frm_buff_size = 0;
        }

        if(NULL != g726_priv->g726_dec_hdl)
        {
            alib_free(g726_priv->g726_dec_hdl);
        }
        alib_free(g726_priv);
    }
    
    alib_logw("g726_dec_exited");

    return 0;
}

alib_int32 G726frameDec(struct __AudioDEC_AC320 *p, alib_int8 *OutBuffer, alib_int32 *OutBuffLen)
{
    alib_int32 framelen = 0;
    alib_int32 nRead = 0;
    alib_int32 dec_samples = 0;
    g726_dec_handle__ *g726_dec_hdl = NULL;
    G726_DEC_PIRV *g726_priv = (G726_DEC_PIRV *)p->DecoderCommand->pdecInfoSet;
    
    g726_dec_hdl = (g726_dec_handle__ *)g726_priv->g726_dec_hdl;
    
    p->BsInformation->Samplerate = 8000;
	p->BsInformation->chan = 1;
	p->BsInformation->bitpersample = 16;
    if(0 == p->FileReadInfo->frmFifo->ValidchunkCnt)
    { 
		if(p->DecoderCommand->ulExitMean == 1)
		{
            alib_loge("g726_dec_no_bs");
			return ERR_AUDIO_DEC_NO_BITSTREAM;
		}
		else
		{
            alib_loge("g726_dec_bs_end");
			return ERR_AUDIO_DEC_ABSEND;
		}
    }
    framelen = p->FileReadInfo->frmFifo->inFrames[p->FileReadInfo->frmFifo->rdIdx].len;
    nRead = p->FileReadInfo->FREAD_AC320((void *)(g726_priv->tmp_frm_buff),framelen,p->FileReadInfo);
    if(nRead != framelen && framelen != 0)
    {
		if(p->DecoderCommand->ulExitMean == 1)
		{
            alib_loge("g726_dec_no_bs_rq:%d-rt:%d",framelen,nRead);
			return ERR_AUDIO_DEC_NO_BITSTREAM;
		}
		else
		{
			return ERR_AUDIO_DEC_ABSEND;
		}
    }

    if(0 == p->Decinitedflag)
    {
        p->Decinitedflag = 1;
        g726_dec_hdl->reset = 1;
        dec_samples = g726decode_func(g726_dec_hdl, (unsigned int *)g726_priv->tmp_frm_buff, nRead, (short *)OutBuffer);
        g726_dec_hdl->reset = 0;
    }
    else
    { 
        dec_samples = g726decode_func(g726_dec_hdl, (unsigned int *)g726_priv->tmp_frm_buff, nRead, (short *)OutBuffer);
    } 

	// alib_loge("g726_dec_frm:%d-%d",nRead,dec_samples);
    *OutBuffLen = dec_samples*2;

    return 0;
}


