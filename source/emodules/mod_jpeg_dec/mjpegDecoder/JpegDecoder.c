#include "mjpegDecode.h"
#include "JpgencLib.h"
#include "video_coder_i.h"

extern __s32 JpegDecodeRaw(JpegDecoderCtx jpgctx);

unsigned long GetByte(JpegDecoderCtx p, __u32 nBits)
{
    __s32 i;
    __s32 ret = 0;

    for (i = 0; i < nBits; i += 8)
    {
        ret <<= 8;
        ret |= *p->vbvInfo.cur_read_addr++;

        if (p->vbvInfo.cur_read_addr > p->vbvInfo.vbv_buf_end)
        {
            p->vbvInfo.cur_read_addr = p->vbvInfo.vbv_buf_start;
        }

        p->vbvInfo.frame_size--;
    }

    return ret;
}

#define INPUT_BYTE(x) (x=GetByte(jpgctx,1*8))
#define INPUT_2BYTES(x)   (x=GetByte(jpgctx,2*8))

void skip_variable(JpegDecoderCtx jpgctx)
{
    __s32 length;
    INPUT_2BYTES(length);
    length -= 2;

    if (length > 0)
    {
        __s32 i;

        for (i = 0; i < (length >> 2); i++)
        {
            GetByte(jpgctx, 4 * 8);
        }

        for (i = 0; i < (length & 3); i++)
        {
            GetByte(jpgctx, 1 * 8);
        }
    }
}

static __s32 first_marker(JpegDecoderCtx jpgctx)
{
    __u8 c, c2;
    INPUT_BYTE(c);
    INPUT_BYTE(c2);
    __wrn("first_marker c:%x,c2:%x", c, c2);

    if (c != 0xFF || c2 != M_SOI)
    {
        return 0;
    }

    jpgctx->unread_marker = c2;
    return 1;
}

static __s32 next_marker(JpegDecoderCtx jpgctx)
{
    __u8 c;

    for (;;)
    {
        INPUT_BYTE(c);

        while (c != 0xFF)
        {
            INPUT_BYTE(c);
        }

        do
        {
            INPUT_BYTE(c);
        } while (c == 0xFF);

        if (c != 0)
        {
            break;    /* found a valid marker, exit loop */
        }
    }

    __wrn("next_marker c:%x", c);
    jpgctx->unread_marker = c;
    return 1;
}

__s32 get_soi(JpegDecoderCtx jpgctx)
{
    if (jpgctx->saw_SOI)
    {
        return 0;
    }

    jpgctx->saw_SOI = 1;
    return 1;
}

static __s32 get_sof(JpegDecoderCtx jpgctx, __u16 is_prog, __u16 is_arith)
{
    __s32 length;
    __u8  data_precision;
    __s32 ci;
    __s32 c;
    jpeg_component_info *compptr;
    INPUT_2BYTES(length);
    INPUT_BYTE(data_precision);
    INPUT_2BYTES(jpgctx->height);
    INPUT_2BYTES(jpgctx->width);
    __wrn("get_sof w:%d h:%d", jpgctx->width, jpgctx->height);
    INPUT_BYTE(jpgctx->num_components);
    length -= 8;

    if (jpgctx->saw_SOF)
    {
        return 0;
    }

    if (data_precision != BITS_IN_JSAMPLE)
    {
        return 0;
    }

    if (jpgctx->num_components > MAX_COMPONENTS)
    {
        return 0;
    }

    if (length != (jpgctx->num_components * 3))
    {
        return 0;
    }

    for (ci = 0; ci < jpgctx->num_components; ci++)
    {
        compptr = &jpgctx->cur_comp_info[ci];
        compptr->component_index = ci;
        INPUT_BYTE(compptr->component_id);
        INPUT_BYTE(c);
        compptr->h_samp_factor = (c >> 4) & 15;
        compptr->v_samp_factor = (c) & 15;
        INPUT_BYTE(compptr->quant_tbl_no);
    }

    //jpgctx->bitstream += length;
    jpgctx->saw_SOF = 1;
    return 1;
}

static __s32 get_dqt(JpegDecoderCtx jpgctx)
{
    __s32 length;
    __s32 n, i, prec;
    __u32 tmp;
    __u16 *QtabPtr;
    INPUT_2BYTES(length);
    length -= 2;

    while (length > 0)
    {
        INPUT_BYTE(n);
        prec = n >> 4;
        n &= 0x0F;

        if (n >= NUM_QUANT_TBLS)
        {
            return 0;
        }

        QtabPtr = jpgctx->QTab[n];

        for (i = 0; i < DCTSIZE2; i++)
        {
            if (prec)
            {
                INPUT_2BYTES(tmp);
            }
            else
            {
                INPUT_BYTE(tmp);
            }

            QtabPtr[i] = (__u16) tmp;
        }

        length -= DCTSIZE2 + 1;

        if (prec)
        {
            length -= DCTSIZE2;
        }
    }

    if (length != 0)
    {
        return 0;
    }

    return 1;
}

static __s32 get_dht(JpegDecoderCtx jpgctx)
{
    __s32 length;
    __u8 bits[17];
    __s32 i;
    __s32 index;
    __s32 count;
    __s32 v;
    __s32 code_max;
    JHUFF_TBL  *htblptr;
    __s32 nb;
    __s32 code;
    INPUT_2BYTES(length);
    length -= 2;
    jpgctx->HasDht = 1;

    while (length > 16)
    {
        INPUT_BYTE(index);

        if (index & 0x10)
        {
            /* AC table definition */
            index -= 0x10;
            htblptr = &jpgctx->ac_huff_tbl[index];
        }
        else
        {
            /* DC table definition */
            htblptr = &jpgctx->dc_huff_tbl[index];
        }

        if (index < 0 || index >= NUM_HUFF_TBLS)
        {
            return 0;
        }

        bits[0] = 0;
        count = 0;

        for (i = 1; i <= 16; i++)
        {
            htblptr->offset[i - 1] = (__u8)count;
            INPUT_BYTE(bits[i]);
            count += bits[i];
        }

        length -= 1 + 16;

        if (count > 256 || count > length)
        {
            return 0;
        }

        code_max = 0;

        for (i = 0; i < count; i++)
        {
            INPUT_BYTE(v);

            if (v > code_max)
            {
                code_max = v;
            }

            htblptr->symbol[i] = (__u8)v;
        }

        length -= count;
        {
            code = 0;

            for (i = 1; i <= 16; i++)
            {
                nb = bits[i];
                htblptr->start_code[i - 1] = (__u16)code;
                code += nb;
                code <<= 1;
            }

            for (i = 16; i >= 1; i--)
            {
                if (bits[i] == 0)
                {
                    htblptr->start_code[i - 1] = 0xffff;
                }
                else
                {
                    break;
                }
            }
        }
    }

    if (length != 0)
    {
        return 0;
    }

    return 1;
}

static __s32 get_sos(JpegDecoderCtx jpgctx)
{
    __s32 length;
    __s32 i, ci, n, c, cc;
    jpeg_component_info *compptr;

    if (!jpgctx->saw_SOF)
    {
        return 0;
    }

    INPUT_2BYTES(length);
    INPUT_BYTE(n); /* Number of components */

    if (length != (n * 2 + 6) || n < 1 || n > MAX_COMPS_IN_SCAN)
    {
        return 0;
    }

    jpgctx->comps_in_scan = n;

    for (i = 0; i < n; i++)
    {
        INPUT_BYTE(cc);
        INPUT_BYTE(c);

        for (ci = 0; ci < jpgctx->num_components; ci++)
        {
            compptr = &jpgctx->cur_comp_info[ci];

            if (cc == compptr->component_id)
            {
                goto id_found;
            }
        }

        return 0;
id_found:
        jpgctx->sos_nb_blocks[i] = compptr->h_samp_factor * compptr->v_samp_factor;
        jpgctx->seq_comp_id[i] = compptr->component_id;
        jpgctx->sos_h_count[i] = (__u8)compptr->h_samp_factor;
        jpgctx->sos_v_count[i] = (__u8)compptr->v_samp_factor;
        compptr->dc_tbl_no = (c >> 4) & 15;
        compptr->ac_tbl_no = (c) & 15;
    }

    /* Collect the additional scan parameters Ss, Se, Ah/Al. */
    INPUT_BYTE(c);
    INPUT_BYTE(cc);
    INPUT_BYTE(ci);

    if (c != 0 || cc != 63 || ci != 0)
    {
        return 0;
    }

    jpgctx->next_restart_num = 0;
    return 1;
}

static __s32 get_dri(JpegDecoderCtx jpgctx)
{
    __s32 length;
    __u32 tmp;
    INPUT_2BYTES(length);
    __wrn("get_dri length:%d", length);

    if (length != 4)
    {
        return 0;
    }

    INPUT_2BYTES(tmp);
    __wrn("get_dri tmp:%d", tmp);
    jpgctx->restart_interval = tmp;
    return 1;
}

static __s32 read_markers(JpegDecoderCtx jpgctx)
{
    jpgctx->HasDht = 0;

    for (;;)
    {
        if (jpgctx->unread_marker == 0)
        {
            if (!jpgctx->saw_SOI)
            {
                if (!first_marker(jpgctx))
                {
                    return JPEG_MARKER_ERROR;
                }
            }
            else
            {
                if (!next_marker(jpgctx)) //not found next mark
                {
                    return JPEG_MARKER_ERROR;
                }
            }
        }

        switch (jpgctx->unread_marker)
        {
            case M_SOI:
                if (!get_soi(jpgctx))
                {
                    return JPEG_MARKER_ERROR;
                }

                break;

            case M_SOF0:                /* Baseline */
            case M_SOF1:                /* Extended sequential, Huffman */
                if (!get_sof(jpgctx, 0, 0))
                {
                    return JPEG_MARKER_ERROR;
                }

                break;

            /* Currently unsupported SOFn types */
            case M_SOF2:                /* Progressive, Huffman */
            case M_SOF3:                /* Lossless, Huffman */
            case M_SOF5:                /* Differential sequential, Huffman */
            case M_SOF6:                /* Differential progressive, Huffman */
            case M_SOF7:                /* Differential lossless, Huffman */
            case M_JPG:                 /* Reserved for JPEG extensions */
            case M_SOF9:                /* Extended sequential, arithmetic */
            case M_SOF10:               /* Progressive, arithmetic */
            case M_SOF11:               /* Lossless, arithmetic */
            case M_SOF13:               /* Differential sequential, arithmetic */
            case M_SOF14:               /* Differential progressive, arithmetic */
            case M_SOF15:               /* Differential lossless, arithmetic */
                return JPEG_FORMAT_UNSUPPORT;//?

            case M_SOS:
                if (!get_sos(jpgctx))
                {
                    return JPEG_MARKER_ERROR;
                }

            //no break here
            case M_EOI:
                jpgctx->unread_marker = 0; /* processed the marker */
                return JPEG_PARSER_OK;

            case M_DHT:
                if (!get_dht(jpgctx))
                {
                    return JPEG_MARKER_ERROR;
                }

                break;

            case M_DQT:
                if (!get_dqt(jpgctx))
                {
                    return JPEG_MARKER_ERROR;
                }

                break;

            case M_DRI:
                if (! get_dri(jpgctx))
                {
                    return JPEG_MARKER_ERROR;
                }

                break;

            case M_COM : //skip com
            case M_DAC : //D_ARITH_CODING_NOT_SUPPORTED ?
            case M_APP0:
            case M_APP1: //no need to process EXIF for jpeg decoder
            case M_APP2:
            case M_APP3:
            case M_APP4:
            case M_APP5:
            case M_APP6:
            case M_APP7:
            case M_APP8:
            case M_APP9:
            case M_APP10:
            case M_APP11:
            case M_APP12:
            case M_APP13:
            case M_APP14:
            case M_APP15:
            case M_DNL:
                //! may be some marker need process
                skip_variable(jpgctx);
                break;

            case M_RST0: /* these are all parameterless */
            case M_RST1:
            case M_RST2:
            case M_RST3:
            case M_RST4:
            case M_RST5:
            case M_RST6:
            case M_RST7:
            case M_TEM:
                break;

            default:  /* must be DHP, EXP, JPGn, or RESn */
                return JPEG_MARKER_ERROR;//?
                //break;
        }
        if (jpgctx->unread_marker == M_SOS || jpgctx->unread_marker == M_EOI)
        {
            jpgctx->unread_marker = 0;
            break;
        }

        jpgctx->unread_marker = 0;
    }
}

__s32 JpegDecoderMain(JpegDecoderCtx jpgctx, stJpegFrameInfo *jpginfo)
{
    __s32 ret;
    jpgctx->saw_SOI = 0;
    jpgctx->saw_SOF = 0;
    jpgctx->unread_marker = 0;
	
    ret = read_markers(jpgctx);

    if (JPEG_PARSER_OK != ret)
    {
        __wrn("read marker fail");
        return PIC_DEC_ERR_COMMON;//DEC_ERROR;
    }

    jpgctx->yuv_buf = jpginfo->yuv_buf;
    ret = JpegDecodeRaw(jpgctx);
	
    jpginfo->y_buf = jpgctx->y_buf;
    jpginfo->u_buf = jpgctx->u_buf;
    jpginfo->v_buf = jpgctx->v_buf;
//    esKRNL_TimeDly(2);
    return ret;
}

