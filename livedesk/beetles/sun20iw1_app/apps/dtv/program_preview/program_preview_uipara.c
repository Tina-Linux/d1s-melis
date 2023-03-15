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
#ifndef __PROGRAM_PREVIEW_PARA_H__
#define __PROGRAM_PREVIEW_PARA_H__
#include "program_preview_uipara.h"

//缩略图的大小
//现在播放全部由线程播放
//所以缩略图的坐标(program_pre)请在dtv_uipara.c中修改

program_preview_uipara_t program_pre_480_272 =
{
    {0, 0, 300, 44},               //title
    {300, 0, 180, 44},             //cur_program
    {0, 44, 300, 190},             //program_pre
    {0, 44, 190, 38 * 5},        //36*5=180   //listbar
    {
        {0, 190 + 44, 480, (272 - 190 - 44)}, //cur_next_prog[0]
        {0, 190 + 44 + (272 - 190 - 44) / 2, 480, (272 - 190 - 44)}, //cur_next_prog[1]
    },
    5                             // LISTBAR_ITEM_NUM

};

program_preview_uipara_t program_pre_800_480 =
{
    {0, 0, 311, 35},               //title
    {0, 0, 180, 35},             //cur_program
    {0, 44, 620, 414},             //program_pre
    {0, 44, 180, 38 * 10}, //36*5=180   //listbar
    {
        {0, 414 + 44, 800 / 2, (480 - 414 - 44)}, //cur_next_prog[0]
        {0, 414 + 44, 800 / 2, (480 - 414 - 44)}, //cur_next_prog[1]
    },
    10                             // LISTBAR_ITEM_NUM

};


static program_preview_uipara_t program_pre_720_576 =
{
    {0, 0, 300, 44},               //title
    {300, 0, 180, 44},             //cur_program
    {0, 44, 540, 436},             //program_pre
    {540, 44, 180, 38 * 10}, //36*5=180   //listbar
    {
        {0, 576 - 22, 720 / 2, 22}, //cur_next_prog[0]
        {720 / 2, 576 - 22, 720 / 2, 22}, //cur_next_prog[1]
    },
    10                             // LISTBAR_ITEM_NUM

};


static program_preview_uipara_t program_pre_720_480 =
{
    {0, 0, 300, 44},               //title
    {300, 0, 180, 44},             //cur_program
    {0, 44, 540, 436},             //program_pre
    {540, 44, 180, 38 * 10}, //36*5=180   //listbar
    {
        {0, 414 + 44, 720 / 2, (480 - 414 - 44)}, //cur_next_prog[0]
        {720 / 2, 414 + 44, 720 / 2, (480 - 414 - 44)}, //cur_next_prog[1]
    },
    10                              // LISTBAR_ITEM_NUM

};

program_preview_uipara_t *program_preview_get_uipara(void)
{
    SIZE rect;
    dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
    //  __msg("screen width:%d, height:%d\n",rect.width, rect.height);

    if ((rect.width == 480) && (rect.height == 272))
    {
        return &program_pre_480_272;
    }
    else if ((rect.width == 800) && (rect.height == 480))
    {
        return &program_pre_800_480;
    }
    else if ((rect.width == 720) && (rect.height == 576))
    {
        return &program_pre_720_576;
    }
    else if ((rect.width == 720) && (rect.height == 480))
    {
        return &program_pre_720_480;
    }
    else
    {
        return &program_pre_800_480;
    }
}

#endif
