/*
********************************************************************************
*                                    eMOD
*                   the Easy Portable/Player Develop Kits
*                               mod_cedar sub-system
*                                   vdrv_mp4 module
*
*          (c) Copyright 2009-2010, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : vdrv_mpeg4_oal.c
* Version: V1.0
* By     : Eric_wang
* Date   : 2009-10-27
********************************************************************************
*/
#include <stdlib.h>
#include <typedef.h>
#include "libcedarc.h"
#include <vdecoder.h>
#include "cdc_config.h"
#include "vdrv_common_cfg.h"

#define VDECODE_DEMO_TEST      (0)

#if VDECODE_DEMO_TEST
#include "demoVdecoder.h"
#endif

#ifdef __GNUC__
#include <string.h>
#include <kconfig.h>
#include <log.h>
#endif

#define DEBUG_MAX_FRAME_IN_LIST 16

__s32 waitSemFrameBufTimeOut = 10;
__hdle hve = 0;

typedef struct VDecoderOalContext
{
    VConfig             video_conf;
    VideoStreamInfo     video_info;
    s8    display_already_begin;
    s8    status;
    s8    mode_switched;
    s64   sys_time;
    void (*release_frame_buffer_sem)(void);
    void (*free_vbs_buffer_sem)(void);
} VDecoderOalContext;

VDecoderOalContext *pVDecOalCtx = NULL;

#if 0
__s32 set_vformat(__video_codec_format_t *stream_info_oal, cedarv_stream_info_t *stream_info)
{
    if ((stream_info_oal == NULL) || (stream_info == NULL))
    {
        __err("wrong prameter.");
        return  EPDK_FAIL;
    }

    memset(stream_info_oal, 0, sizeof(__video_codec_format_t));

    switch (stream_info->format)
    {
        case CEDARV_STREAM_FORMAT_MPEG2:
        {
            if (stream_info->sub_format == CEDARV_MPEG2_SUB_FORMAT_MPEG1)
            {
                stream_info_oal->codec_type = VIDEO_MPEG1_ES;
            }
            else
            {
                stream_info_oal->codec_type = VIDEO_MPEG2_ES;
            }

            break;
        }

        case CEDARV_STREAM_FORMAT_MPEG4:
        {
            if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_XVID)
            {
                stream_info_oal->codec_type = VIDEO_XVID;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_DIVX1)
            {
                stream_info_oal->codec_type = VIDEO_DIVX1;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_DIVX2)
            {
                stream_info_oal->codec_type = VIDEO_DIVX2;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_DIVX3)
            {
                stream_info_oal->codec_type = VIDEO_DIVX3;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_DIVX4)
            {
                stream_info_oal->codec_type = VIDEO_DIVX4;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_DIVX5)
            {
                stream_info_oal->codec_type = VIDEO_DIVX5;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_SORENSSON_H263)
            {
                stream_info_oal->codec_type = VIDEO_SORENSSON_H263;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_RMG2)
            {
                stream_info_oal->codec_type = VIDEO_RMG2;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_H263)
            {
                stream_info_oal->codec_type = VIDEO_H263;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_VP6)
            {
                stream_info_oal->codec_type = VIDEO_VP6;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_WMV1)
            {
                stream_info_oal->codec_type = VIDEO_WMV1;
            }
            else if (stream_info->sub_format == CEDARV_MPEG4_SUB_FORMAT_WMV2)
            {
                stream_info_oal->codec_type = VIDEO_WMV2;
            }
            else
            {
                stream_info_oal->codec_type = VIDEO_UNKNOWN;
            }

            break;
        }

        case CEDARV_STREAM_FORMAT_REALVIDEO:
        {
            stream_info_oal->codec_type = VIDEO_RMVB8;
            break;
        }

        case CEDARV_STREAM_FORMAT_H264:
        {
            stream_info_oal->codec_type = VIDEO_H264;
            break;
        }

        case CEDARV_STREAM_FORMAT_VC1:
        {
            stream_info_oal->codec_type = VIDEO_VC1;
            break;
        }

        case CEDARV_STREAM_FORMAT_MJPEG:
        {
            stream_info_oal->codec_type = VIDEO_MJPEG;
            break;
        }

        case CEDARV_STREAM_FORMAT_AVS:
        {
            stream_info_oal->codec_type = VIDEO_AVS;
            break;
        }

        default:
        {
            stream_info_oal->codec_type = VIDEO_UNKNOWN;
            break;
        }
    }

    stream_info_oal->width          = stream_info->video_width   ;
    stream_info_oal->height         = stream_info->video_height  ;
    stream_info_oal->frame_rate     = stream_info->frame_rate    ;
    stream_info_oal->mic_sec_per_frm = stream_info->frame_duration;
    stream_info_oal->priv_inf_len   = stream_info->init_data_len ;
    stream_info_oal->private_inf    = stream_info->init_data     ;
    return EPDK_OK;
}
#endif

static void ReleaseFrameBufferSem(void)
{
    ReleaseFrmBufSem();
    return;
}

static void FreeVbsBufferSem(void)
{
    VDrvFreeVbsBuffer();
}

void *VDecDev_MInit(__s32 *ret)
{
    u8  i = 0;
    VideoDecoder *pVideoDec = NULL;

    pVDecOalCtx = (VDecoderOalContext *)malloc(sizeof(VDecoderOalContext));
    if(pVDecOalCtx == NULL)
    {
        __err("L:%d malloc failed.", __LINE__);
        return NULL;
    }
    memset(pVDecOalCtx, 0, sizeof(VDecoderOalContext));
    pVDecOalCtx->release_frame_buffer_sem = ReleaseFrameBufferSem;
    pVDecOalCtx->free_vbs_buffer_sem = FreeVbsBufferSem;

    while (!hve)
    {
        hve = esSVC_ResourceReq(RESOURCE_VE_HW, RESOURCE_REQ_MODE_NWAIT, 0);

        if (!hve)
        {
            if (i >= 5)
            {
                return NULL;
            }

            i++;
            esKRNL_TimeDly(1);
        }
    }

#if VDECODE_DEMO_TEST
    demoVdecoder((__s32 *)ret);
#endif

    pVideoDec = CreateVideoDecoder();

    if (pVideoDec == NULL)
    {
        esSVC_ResourceRel(hve);
        hve = 0;
    }

    //* configurations for decoder.
    pVDecOalCtx->video_conf.bSecureosEn = 0;
    pVDecOalCtx->video_conf.bDisable3D = 0;
    pVDecOalCtx->video_conf.bDispErrorFrame = 0;
    pVDecOalCtx->video_conf.bNoBFrames = 0;
    pVDecOalCtx->video_conf.bRotationEn = 0;
    pVDecOalCtx->video_conf.nRotateDegree = 0;
    pVDecOalCtx->video_conf.bScaleDownEn = 0;
    pVDecOalCtx->video_conf.nHorizonScaleDownRatio = 0;
    pVDecOalCtx->video_conf.nVerticalScaleDownRatio = 0;
    pVDecOalCtx->video_conf.eOutputPixelFormat = PIXEL_FORMAT_YV12;//PIXEL_FORMAT_NV21;
    pVDecOalCtx->video_conf.nAlignStride = 32;
    pVDecOalCtx->video_conf.bVirMallocSbm = 0;
    pVDecOalCtx->video_conf.nDisplayHoldingFrameBufferNum = NUM_OF_PICTURES_KEEP_IN_LIST;
    pVDecOalCtx->video_conf.nDeInterlaceHoldingFrameBufferNum = NUM_OF_PICTURES_KEEPPED_BY_DEINTERLACE;
    pVDecOalCtx->video_conf.nRotateHoldingFrameBufferNum = NUM_OF_PICTURES_KEEPPED_BY_ROTATE;
    pVDecOalCtx->video_conf.nDecodeSmoothFrameBufferNum = NUM_OF_PICTURES_FOR_EXTRA_SMOOTH;

    return pVideoDec;
}

s16 VDecDev_MExit(void *pDev)     //struct VDEC_DEVICE *pDev
{
#if 0
    VideoDecoder *pVideoDec = NULL;

    if (pDev == NULL)
    {
        loge("L:%d para error.", __LINE__);
        return CEDARV_RESULT_ERR_INVALID_PARAM;
    }

    pVideoDec = (VideoDecoder *)pDev;

    if(pVideoDec)
    {
        free(pVideoDec);
        pVideoDec = NULL;
    }
#endif

    if (hve != 0)
    {
        esSVC_ResourceRel(hve);
        hve = 0;
    }

    if(pVDecOalCtx)
    {
        if(pVDecOalCtx->video_info.pCodecSpecificData)
        {
            free(pVDecOalCtx->video_info.pCodecSpecificData);
            pVDecOalCtx->video_info.pCodecSpecificData = NULL;
        }

        free(pVDecOalCtx);
        pVDecOalCtx = NULL;
    }

    return EPDK_OK;
}

long VDecDev_MIoctrl(void *pDecDev, __s32 cmd, __s32 aux, void *pbuffer)
{
    long    result;
    VideoDecoder *pVideoDec;
    VideoDecoderContext *pVideoDecCtx;

    pVideoDec = (VideoDecoder *)pDecDev;

    if (pVideoDec == NULL)
    {
        __err("wrong pareameter.");
        return EPDK_FAIL;
    }

    pVideoDecCtx = (VideoDecoderContext *)pVideoDec;

    switch (cmd)
    {
        case VDRV_INTERNAL_CMD_QUERY_FRAME:
        {
            VideoPicture     *picture = NULL;
            __display_info_t *tmpDispFrm = (__display_info_t *)pbuffer;

            picture = RequestPicture(pVideoDec, 0/*the major stream*/);
            if (picture != NULL)
            {
                pVDecOalCtx->display_already_begin = 1;

                tmpDispFrm->bProgressiveSrc               = picture->bIsProgressive;
                tmpDispFrm->bTopFieldFirst                = picture->bTopFieldFirst;

                if (picture->bIsProgressive)
                {
                    tmpDispFrm->eRepeatField = REPEAT_FIELD_NONE;
                }
                else if (picture->bRepeatTopField)
                {
                    tmpDispFrm->eRepeatField = REPEAT_FIELD_TOP;
                }
                else
                {
                    tmpDispFrm->eRepeatField = REPEAT_FIELD_NONE;
                }

                if (picture->ePixelFormat == PIXEL_FORMAT_YUV_MB32_420)
                {
                    tmpDispFrm->pVideoInfo.color_format  = PIC_CFMT_YCBCR_420;
                }
                else if (picture->ePixelFormat == PIXEL_FORMAT_YUV_MB32_422)
                {
                    tmpDispFrm->pVideoInfo.color_format  = PIC_CFMT_YCBCR_422;
                }
                else if (picture->ePixelFormat == PIXEL_FORMAT_YUV_MB32_444)
                {
                    tmpDispFrm->pVideoInfo.color_format  = PIC_CFMT_YCBCR_444;
                }
                else
                {
                    tmpDispFrm->pVideoInfo.color_format  = picture->ePixelFormat;
                }

                /*__log("fmt = %d.", tmpDispFrm->pVideoInfo.color_format);*/
                tmpDispFrm->pVideoInfo.frame_rate         = picture->nFrameRate;//INVALID_FRAME_RATE;
                tmpDispFrm->pVideoInfo.eAspectRatio       = picture->nAspectRatio;
                tmpDispFrm->pPanScanInfo.uNumberOfOffset  = 0;
                tmpDispFrm->horizontal_scale_ratio        = 0;
                tmpDispFrm->vertical_scale_ratio          = 0;
                tmpDispFrm->src_rect.uStartX              = 0;
                tmpDispFrm->src_rect.uStartY              = 0;
                tmpDispFrm->src_rect.uWidth               = picture->nWidth;
                tmpDispFrm->src_rect.uHeight              = picture->nHeight;
                tmpDispFrm->dst_rect.uStartX              = picture->nLeftOffset;
                tmpDispFrm->dst_rect.uStartY              = picture->nTopOffset;
                tmpDispFrm->dst_rect.uWidth               = picture->nRightOffset - picture->nLeftOffset;;
                tmpDispFrm->dst_rect.uHeight              = picture->nBottomOffset - picture->nTopOffset;

                if (picture->nPts == (u64)(-1))
                {
                    tmpDispFrm->uPts                      = (u64)(-1);        //equal to 0xffffffff ffffffff
                }
                else
                {
                    tmpDispFrm->uPts                      = picture->nPts / 1000;
                }

                tmpDispFrm->top_index                     = picture->nID;
                tmpDispFrm->top_y                         = (unsigned long)picture->pData0;
                tmpDispFrm->top_u                         = (unsigned long)picture->pData1;
                tmpDispFrm->top_v                         = (unsigned long)picture->pData2;
                tmpDispFrm->maf_valid                     = 0;
                tmpDispFrm->flag_addr                     = NULL;
                tmpDispFrm->flag_stride                   = 0;
                tmpDispFrm->pre_frame_valid               = 1;
                tmpDispFrm->b10BitPicFlag                 = picture->b10BitPicFlag;
                tmpDispFrm->nLbcMod                       = picture->nLbcLossyComMod;
                tmpDispFrm->bIsLossy                      = picture->bIsLossy;
                tmpDispFrm->bRcEn                         = picture->bRcEn;
                tmpDispFrm->nLbcSize                      = picture->nLbcSize;
#if 0
                eLIBs_printf("QueryFrame: maf_valid[%d], pre_frame_valid[%d], flag_addr[0x%x], flag_stride[0x%x]\n",
                             tmpDispFrm->maf_valid, tmpDispFrm->pre_frame_valid, tmpDispFrm->flag_addr, tmpDispFrm->flag_stride);

                if ((tmpDispFrm->horizontal_scale_ratio != 0 && tmpDispFrm->horizontal_scale_ratio != 0xff)
                    || (tmpDispFrm->vertical_scale_ratio != 0 && tmpDispFrm->vertical_scale_ratio != 0xff))
                {
                    eLIBs_printf("scale! h[%d], v[%d]\n", tmpDispFrm->horizontal_scale_ratio, tmpDispFrm->vertical_scale_ratio);
                }

#endif
                /*__log("s.uWidth=%d, s.uHeight = %d, s.uStartX = %d, s.uStart=%d", tmpDispFrm->src_rect.uWidth, tmpDispFrm->src_rect.uHeight, tmpDispFrm->src_rect.uStartX, tmpDispFrm->src_rect.uStartY);*/
                /*__log("d.uWidth=%d, d.uHeight = %d, d.uStartX = %d, d.uStart=%d", tmpDispFrm->dst_rect.uWidth, tmpDispFrm->dst_rect.uHeight, tmpDispFrm->dst_rect.uStartX, tmpDispFrm->dst_rect.uStartY);*/
                return EPDK_OK;
            }
            else
            {
                return EPDK_FAIL;
            }
        }

        case VDRV_INTERNAL_CMD_RELEASE_FRAME:
        {
            Fbm*         pFbm;
            int          nStreamIndex = 0;
            VideoPicture *pPicture;

            pFbm = pVideoDecCtx->pFbm[nStreamIndex];
            if(pFbm == NULL)
            {
                return EPDK_FAIL;
            }

            pPicture = FbmIndexToPointer((u8)aux, pFbm);
            if (pPicture == NULL)
            {
                return EPDK_FAIL;
            }

            int ret = ReturnPicture(pVideoDec, pPicture);
            if (ret != 0)
            {
                __err("fatal error! Return Picture() fail ret[%d]", ret);
                return EPDK_FAIL;
            }

            if (pVDecOalCtx->release_frame_buffer_sem != NULL)
            {
                pVDecOalCtx->release_frame_buffer_sem();
            }

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_TV_MODE:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_QUERY_VBSBUFFER:
        {
            __ring_bs_buf_t    *pbsbuf = (__ring_bs_buf_t *)pbuffer;
            result = RequestVideoStreamBuffer(pVideoDec,
                                              768 * 1024,
                                              (char**)&pbsbuf->pData0,
                                              (int *)(&pbsbuf->uSizeGot0),
                                              (char**)&pbsbuf->pData1,
                                              (int *)(&pbsbuf->uSizeGot1),
                                              0);
            if (result != CEDARV_RESULT_OK)
            {
                return EPDK_FAIL;
            }

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_DATAINPUT:
        {
            VideoStreamDataInfo        data_info;
            __vdec_datctlblk_t         *data_info_oal;

            memset(&data_info, 0x00, sizeof(data_info));

            data_info_oal = (__vdec_datctlblk_t *)pbuffer;
            if (data_info_oal == NULL)
            {
                __err("fatal error, parameter invalid.");
                return EPDK_FAIL;
            }
            data_info.pData = data_info_oal->data;
            data_info.nLength = (u32)aux;
            data_info.nStreamIndex = data_info_oal->videoStreamIndex;
            //data_info.bIsFirstPart = 1;
            //data_info.bIsLastPart = 1;

            if (data_info_oal->CtrlBits & FIRST_PART_CTRL)
            {
                data_info.bIsFirstPart = 1;
            }

            if (data_info_oal->CtrlBits & LAST_PART_CTRL)
            {
                data_info.bIsLastPart = 1;
            }

            if (data_info_oal->CtrlBits & PTS_VALID_CTRL)
            {
                data_info.nPts = data_info_oal->uPTS;
                //newlib_printf("%s line %d, pts = %lld.\n", __func__, __LINE__, data_info.pts);
            }
            else
            {
                //__err("fatal error, invalid pts.");
                data_info.nPts = (u64)(-1);
            }
            data_info.nPcr = -1;

            result = SubmitVideoStreamData(pVideoDec, &data_info, 0);
            if (result != CEDARV_RESULT_OK)
            {
                return EPDK_FAIL;
            }

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_DATA_END:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_OPEN_DEV:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_OPEN:
        {
            result = InitializeVideoDecoder(pVideoDec, &pVDecOalCtx->video_info,  &pVDecOalCtx->video_conf);
            if (result < 0)
            {
                return EPDK_FAIL;
            }

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_CLOSE_DEV:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_CLOSE:
        {
            DestroyVideoDecoder(pVideoDec);
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_PREVIEW:
        {
            pVDecOalCtx->status = CEDARV_STATUS_PREVIEW;
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_PLAY:
        {
            if (pVDecOalCtx->status != CEDARV_STATUS_PLAY && pVDecOalCtx->status != CEDARV_STATUS_STOP)
            {
                pVDecOalCtx->mode_switched = 1;
            }

            pVDecOalCtx->status = CEDARV_STATUS_PLAY;
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_STOP:
        {
            if (pVDecOalCtx->status != CEDARV_STATUS_PLAY && pVDecOalCtx->status != CEDARV_STATUS_FORWARD && pVDecOalCtx->status != CEDARV_STATUS_BACKWARD)
            {
                return EPDK_FAIL;
            }

            pVDecOalCtx->status = CEDARV_STATUS_STOP;
            pVDecOalCtx->mode_switched = 1;
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_FF:
        {
            if (pVDecOalCtx->status != CEDARV_STATUS_PLAY)
            {
                return EPDK_FAIL;
            }

            pVDecOalCtx->status = CEDARV_STATUS_FORWARD;
            pVDecOalCtx->mode_switched = 1;
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_RR:
        {
            if (pVDecOalCtx->status != CEDARV_STATUS_PLAY)
            {
                return EPDK_FAIL;
            }

            pVDecOalCtx->status = CEDARV_STATUS_BACKWARD;
            pVDecOalCtx->mode_switched = 1;
            return EPDK_OK;
        }

        /* configurations for vdecoder, need fix.*/
        case VDRV_INTERNAL_CMD_SET_VDEC_CONFIG_INFO:
        {
            __wrn("Setting video decoder config info is not implemented yet!");
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_VBSINF:
        {
            __video_codec_format_t *stream_info_oal;
            stream_info_oal = (__video_codec_format_t *)pbuffer;

            switch (stream_info_oal->codec_type)
            {
                case VIDEO_MPEG1_ES:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_MPEG1;
                    break;
                }

                case VIDEO_MPEG2_ES:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_MPEG2;
                    break;
                }

                case VIDEO_XVID:
                case VIDEO_DIVX3:
                case VIDEO_DIVX4:
                case VIDEO_DIVX5:
                case VIDEO_SORENSSON_H263:
                case VIDEO_RMG2:
                case VIDEO_H263:
                case VIDEO_VP6:
                case VIDEO_WMV1:
                case VIDEO_WMV2:
                case VIDEO_DIVX2:
                case VIDEO_DIVX1:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_MPEG4;

                    if (stream_info_oal->codec_type == VIDEO_XVID)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_XVID;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_DIVX1)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_MSMPEG4V1;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_DIVX2)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_MSMPEG4V2;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_DIVX3)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_DIVX3;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_DIVX4)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_DIVX4;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_DIVX5)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_DIVX5;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_SORENSSON_H263)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_SORENSSON_H263;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_H263)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_H263;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_RMG2)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_RXG2; //fixed me.
                    }
                    else if (stream_info_oal->codec_type == VIDEO_VP6)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_VP6;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_WMV1)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_WMV1;
                    }
                    else if (stream_info_oal->codec_type == VIDEO_WMV2)
                    {
                        pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_WMV2;
                    }
                    else
                    {
                        __wrn("unknown mpeg4 sub codec type!");
                    }

                    break;
                }

                case VIDEO_RMVB8:
                case VIDEO_RMVB9:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_RX;//fixed me.
                    break;
                }

                case VIDEO_H264:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_H264;
                    break;
                }

                case VIDEO_VC1:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_WMV3;
                    break;
                }

                case VIDEO_AVS:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_AVS;
                    break;
                }

                case VIDEO_MJPEG:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_MJPEG;
                    break;
                }

                case VIDEO_VP8:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_VP8;
                    break;
                }

                case VIDEO_H265:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_H265;
                    break;
                }

                case VIDEO_JPEG:
                default:
                {
                    pVDecOalCtx->video_info.eCodecFormat = VIDEO_CODEC_FORMAT_UNKNOWN;
                    return EPDK_FAIL;
                }
            }

            if (stream_info_oal->video_bs_src == CEDARLIB_FILE_FMT_TS)
            {
                pVDecOalCtx->video_info.bIsFramePackage = 0;
            }
            else
            {
                pVDecOalCtx->video_info.bIsFramePackage = 1;
            }


            pVDecOalCtx->video_info.nWidth = stream_info_oal->width;
            pVDecOalCtx->video_info.nHeight = stream_info_oal->height;
            pVDecOalCtx->video_info.nFrameRate = stream_info_oal->frame_rate;
            pVDecOalCtx->video_info.nFrameDuration = stream_info_oal->mic_sec_per_frm;
            pVDecOalCtx->video_info.nAspectRatio = 1000;
            pVDecOalCtx->video_info.nCodecSpecificDataLen = stream_info_oal->priv_inf_len;
            if (pVDecOalCtx->video_info.nCodecSpecificDataLen > 0)
            {
                s32   nSize = pVDecOalCtx->video_info.nCodecSpecificDataLen;
                char *pMem    = (char *)malloc(nSize);

                if (pMem == NULL)
                {
                    pVDecOalCtx->video_info.nCodecSpecificDataLen = 0;
                    __err("memory alloc fail.");
                    return EPDK_FAIL;
                }

                if (stream_info_oal->private_inf)
                {
                    memcpy(pMem, stream_info_oal->private_inf, nSize);
                    // the memory is allocate in heap in parser, so need to be free here.
                    //free(info->init_data);
                    //info->init_data = NULL;
                }
                pVDecOalCtx->video_info.pCodecSpecificData = pMem;
            }
            pVDecOalCtx->video_info.bIs3DStream = stream_info_oal->is3DMode;

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_DECODE:
        {
            int      bDecodeKeyFrameOnly = 0;
            int      bDropBFrameIfDelay = 0;
            int64_t  nCurrentTimeUs = 0;

            if (vdrv_com->avsync)
            {
                pVDecOalCtx->sys_time = esMODS_MIoctrl(vdrv_com->avsync, DRV_AVS_CMD_GET_VID_TIME, DRV_AVS_TIME_TOTAL, 0);
            }
            else
            {
                pVDecOalCtx->sys_time = 0;
            }

            if(pVDecOalCtx->mode_switched)
            {
                ResetVideoDecoder(pVideoDec);
                if (pVDecOalCtx->free_vbs_buffer_sem != NULL)
                {
                    pVDecOalCtx->free_vbs_buffer_sem();
                }
                if(pVDecOalCtx->release_frame_buffer_sem != NULL)
                {
                    pVDecOalCtx->release_frame_buffer_sem();
                }
                pVDecOalCtx->mode_switched = 0;
                return VDRV_PIC_DEC_ERR_VBS_UNDERFLOW;
            }

            if(pVDecOalCtx->status == CEDARV_STATUS_BACKWARD || pVDecOalCtx->status == CEDARV_STATUS_FORWARD)
            {
                bDecodeKeyFrameOnly = 1;
                bDropBFrameIfDelay = 0;
                nCurrentTimeUs = 0;
            }
            else
            {
                if (pVideoDecCtx->videoStreamInfo.eCodecFormat == VIDEO_CODEC_FORMAT_H264
                    && pVDecOalCtx->display_already_begin == 0) //*stream media need to show first frame
                {
                    //* for H264, when decoding the first data unit containing PPS, we have to promise
                    //* there is more than one bitstream frame for decoding.
                    //* that is because the decoder uses the start code of the second bitstream frame
                    //* to judge PPS end.
                    if (SbmStreamFrameNum(pVideoDecCtx->pSbm[0]) < 2)
                    {
                        return VDRV_PIC_DEC_ERR_VBS_UNDERFLOW;
                    }

                    if ((pVideoDecCtx->nSbmNum == 2) && (pVideoDecCtx->vconfig.bDisable3D != 1))
                    {
                        if (SbmStreamFrameNum(pVideoDecCtx->pSbm[1]) < 2)
                        {
                            return VDRV_PIC_DEC_ERR_VBS_UNDERFLOW;
                        }
                    }
                }

                if (pVDecOalCtx->status == CEDARV_STATUS_PREVIEW)
                {
                    __err("call DecodeVideoStream() in Preview mode");
                    bDecodeKeyFrameOnly = 0;
                    bDropBFrameIfDelay = 0;
                    nCurrentTimeUs = 0;
                }
                else
                {
                    bDecodeKeyFrameOnly = 0;
                    bDropBFrameIfDelay = 1;
                    nCurrentTimeUs = pVDecOalCtx->sys_time;
                    nCurrentTimeUs *= 1000;
                }

            }
            result = DecodeVideoStream(pVideoDec, 0, bDecodeKeyFrameOnly, bDropBFrameIfDelay, nCurrentTimeUs);

            switch (result)
            {
                case VDECODE_RESULT_OK:
                case VDECODE_RESULT_FRAME_DECODED:
                {
                    return VDRV_PIC_DEC_ERR_NONE;
                }

                case VDECODE_RESULT_KEYFRAME_DECODED:
                {
                    if (pVDecOalCtx->status == CEDARV_STATUS_BACKWARD
                        || pVDecOalCtx->status == CEDARV_STATUS_FORWARD
                        || pVDecOalCtx->status == CEDARV_STATUS_PREVIEW)
                    {
                        VideoEngineReset(pVideoDecCtx->pVideoEngine);
                    }
                    return VDRV_PIC_DEC_I_FRAME;
                }

                case VDECODE_RESULT_NO_FRAME_BUFFER:
                {
                    return VDRV_PIC_DEC_ERR_NO_FRAMEBUFFER;
                }

                case VDECODE_RESULT_NO_BITSTREAM:
                case VDECODE_RESULT_CONTINUE:
                {
                    return VDRV_PIC_DEC_ERR_VBS_UNDERFLOW;
                }

                default:
                {
                    return VDRV_PIC_DEC_ERR_VFMT_ERR;
                }
            }
        }

        case VDRV_INTERNAL_CMD_QUERY_STATUS_CHANGE_PROGRESS:
        {
            return VDRV_TRUE;
        }

        case VDRV_INTERNAL_CMD_SET_STATUS_CHANGE_PROGRESS:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_ROTATE:
        {
            __log("[VDRV_INTERNAL_CMD_SET_ROTATE]aux:%d\n",aux);
            if(aux > 5)
            {
                return EPDK_FAIL;
            }

            pVDecOalCtx->video_conf.nRotateDegree = aux;

            if(aux != 0)
            {
                pVDecOalCtx->video_conf.bRotationEn = 1;
            }

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_QUERY_VBSBUFFER_USAGE:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_TOTALMEMSIZE:
        {
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_RESET:
        {
            ResetVideoDecoder(pVideoDec);
            if (pVDecOalCtx->free_vbs_buffer_sem != NULL)
            {
                pVDecOalCtx->free_vbs_buffer_sem();
            }
            if(pVDecOalCtx->release_frame_buffer_sem != NULL)
            {
                pVDecOalCtx->release_frame_buffer_sem();
            }
            pVDecOalCtx->mode_switched = 0;
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_GET_STREAM_INFO:
        {
            VideoStreamInfo        pVideoInfo;
            __video_codec_format_t *pvformat = (__video_codec_format_t *)pbuffer;

            memset(&pVideoInfo, 0, sizeof(VideoStreamInfo));
            GetVideoStreamInfo(pVideoDec, &pVideoInfo);

            memset(pvformat, 0, sizeof(__video_codec_format_t));
            pvformat->width           = pVideoInfo.nWidth;
            pvformat->height          = pVideoInfo.nHeight;
            pvformat->frame_rate      = pVideoInfo.nFrameRate;
            pvformat->mic_sec_per_frm = pVideoInfo.nFrameDuration;

            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_GET_TAG:
        {
            __wrn("VDRV_INTERNAL_CMD_GET_TAG is not implemented yet!");
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_TAG:
        {
            __wrn("VDRV_INTERNAL_CMD_SET_TAG is not implemented yet!");
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_CHKSPT:
        {
            __wrn("VDRV_INTERNAL_CMD_CHKSPT is not implemented yet!");
            return EPDK_TRUE;
        }

        case VDRV_INTERNAL_CMD_ENABLE_MAF:
        {
            if (0 == aux)
            {
                pVDecOalCtx->video_conf.bSupportMaf = 0;
            }
            else
            {
                __log("maf default enable! But Be careful!");
            }
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_GET_OFQ_VALID_SIZE:
        {
            result = FbmValidPictureNum(pVideoDecCtx->pFbm[0]);
            return result;
        }

        case VDRV_INTERNAL_CMD_GET_VE_STATUS:
        {
            __wrn("VDRV_INTERNAL_CMD_GET_VE_STATUS is not implemented yet!");
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_PRODUCT_TYPE:
        {
            return CEDARV_RESULT_OK;
        }

        case VDRV_INTERNAL_CMD_SET_REF_FRAME_NUM:
        {
            __wrn("VDRV_INTERNAL_CMD_SET_REF_FRAME_NUM is not implemented yet!");
            return EPDK_OK;
        }

        case VDRV_INTERNAL_CMD_SET_RAWH264:
        {
            return CEDARV_RESULT_OK;
        }

        case VDRV_INTERNAL_CMD_USER_SET_SCALEDOWN:
        {
            if(aux > 0)
            {
                pVDecOalCtx->video_conf.bScaleDownEn = 1;
                pVDecOalCtx->video_conf.nHorizonScaleDownRatio = aux;
                pVDecOalCtx->video_conf.nVerticalScaleDownRatio = aux;
            }
            __log("[VDRV_INTERNAL_CMD_USER_SET_SCALEDOWN] ratio:%d\n",aux);

            return CEDARV_RESULT_OK;
        }

        case VDRV_INTERNAL_CMD_USER_SET_LBC_MODE:
        {
            if(aux > 0)
            {
                pVDecOalCtx->video_conf.nLbcLossyComMod = aux;
                pVDecOalCtx->video_conf.bIsLossy = 1;
                pVDecOalCtx->video_conf.bRcEn = 0;
            }
            __log("[VDRV_INTERNAL_CMD_USER_SET_LBC_MODE] mode:%d\n",aux);

            return CEDARV_RESULT_OK;
        }

        default:
        {
            __wrn("Unknown cmd[%x]", cmd);
            return EPDK_FAIL;
        }
    }
}

