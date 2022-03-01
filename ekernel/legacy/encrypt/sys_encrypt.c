/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            system encrypt sub-system
*
*                                     (c) Copyright 2009-2010, kevin.z China
*                                              All Rights Reserved
*
* File   : sys_encrypt.c
* Version: V1.0
* By     : kevin.z
* Date   : 2010-1-28 15:05
*********************************************************************************************************
*/
#include "sys_encrypt_i.h"
#include <string.h>
#include <log.h>

const __sys_encrypt_para_t sys_para =
{
    //magic
    "magic.epos.it.is.just.a.dream",

    //multi-media parameter
    {
        //video parameter
        {
            {"mpeg2",   1, 1920, 1080, 0},
            {"mpeg4",   1, 1920, 1080, 0},
            {"mjpeg",   1, 1920, 1080, 0},
            {"rmvb",    1, 1920, 1080, 0},
            {"h264",    1, 1920, 1080, 0},
            {"vc1",     1, 1920, 1080, 0},
            {"wmv1",    1, 1920, 1080, 0},
            {"wmv2",    1, 1920, 1080, 0},
            {"vp6",     1, 1920, 1080, 0},
            {"avs",     1, 1920, 1080, 0},
            {"null",    0,    0,    0, 0},
        },

        //audio parameter

        //picture parameter
    },
    {
        0, 0
    }
};


int32_t encrypt_para_init(void)
{
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     encrypt_para_get
*
* Description: get encrypt parameter.
*
* Arguments  : type         type of parmaeter;
*              format       format of the type;
*              para         parameter;
*
* Returns    : result;
*               EPDK_OK     -   get parameter successed,
*               EPDK_FAIL   -   get parameter failed;
*********************************************************************************************************
*/
int32_t esKSRV_GetPara(uint32_t type, void *format, void *para)
{
    int32_t   result = EPDK_FAIL;

    switch (type & 0xff000000)
    {
        case MULTI_MEDIA_CFG:
        {
            switch (type & 0x00ff0000)
            {
                case VIDEO_CFG:
                {
                    const __video_encrypt_item    *tmpVideo  = &sys_para.multimedia.video[0];
                    while (strcasecmp(tmpVideo->format, "null") != 0)
                    {
                        if (strcasecmp(tmpVideo->format, format) == 0)
                        {
                            *(__video_encrypt_item *)para = *tmpVideo;
                            result = EPDK_OK;
                            break;
                        }
                        tmpVideo++;
                    }
                    break;
                }

                default:
                {
                    __wrn("type error!");
                    break;
                }
            }
            break;
        }

        default:
        {
            __wrn("type error!");
            break;
        }
    }

    return result;
}

