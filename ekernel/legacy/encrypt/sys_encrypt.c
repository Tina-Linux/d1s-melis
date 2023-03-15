/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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