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
#include <log.h>
#include "mod_init_i.h"
#include "scene_adjust_uipara.h"

static scene_adjust_para_t scene_uipara_800_480 =
{
    {PIXEL_COLOR_ARGB8888, 0, 0, 800, 480, {0}},
    {0, 0, 800, 480, {0}},
    {
        {100, 100, 50, 50, {0}},
        {700, 100, 50, 50, {0}},
        {700, 400, 50, 50, {0}},
        {100, 400, 50, 50, {0}},
        {400, 240, 50, 50, {0}},
    },
};

static scene_adjust_para_t scene_uipara_400_240 =
{
    {PIXEL_COLOR_ARGB8888, 0, 0, 400, 240, {0}},
    {0, 0, 400, 240, {0}},
    {
        {60, 60, 30, 30, {0}},
        {340, 60, 30, 30, {0}},
        {340, 180, 30, 30, {0}},
        {60, 180, 30, 30, {0}},
        {200, 105, 30, 30, {0}},
    },
};
static scene_adjust_para_t scene_uipara_480_272 =
{
    {PIXEL_COLOR_ARGB8888, 0, 0, 480, 272, {0}},
    {0, 0, 480, 272,{0}},
    {
        {60, 60, 50, 50, {0}},
        {420, 60, 50, 50, {0}},
        {420, 212, 50, 50, {0}},
        {60, 212, 50, 50, {0}},
        {240, 130, 50, 50, {0}},
    },
};
scene_adjust_para_t *scene_adjust_get_uipara(void)
{
    __u32 width;
    __u32 height;
    dsk_display_get_size((__s32 *)&width, (__s32 *)&height);
    __msg("width=%d, height=%d\n", width, height);

    if (width == 800 && height == 480)
    {
        return &scene_uipara_800_480;
    }
    else if (width == 480 && height == 272)
    {
        return &scene_uipara_480_272;
    }
    else if (width == 400 && height == 240)
    {
        return &scene_uipara_400_240;
    }
    else
    {
        return &scene_uipara_800_480;
    }
}
