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
#include "beetles_app.h"

#include "spctrm_i.h"

#include "spctrm_uipara.h"

static __spctrm_uipara_t spectrum_uipara_400_240 =
{
    {
        270,    40, 160,    80          //frame win
    },
    {
        //0, 0, (16+16/3)*NUM_SPCTRM, 60
        0, 0, 160, 61
    },
    ID_MUSIC_PARTICLE_BMP,  //height of  particle is 60, ID_MUSIC_PARTICLE_UF_800X480_BMP
    ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_800X480_BMP
};

static __spctrm_uipara_t spectrum_uipara_800_480 =
{
    {
        //428,  140,    160,    80          //frame win
        400, 284 - 46 + 25, 300, 98
    },
    {
        //0, 0, (16+16/3)*NUM_SPCTRM, 60
        //0, 0, 29, 96
        0, 0, 300, 98
    },
    ID_MUSIC_PARTICLE_BMP,  //height of  particle is 60, ID_MUSIC_PARTICLE_UF_800X480_BMP
    ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_800X480_BMP
};


static __spctrm_uipara_t spectrum_uipara_720_480 =
{
    {
        //428,  140,    160,    80          //frame win
        400 - 80, 284 - 46 + 25, 300, 98
    },
    {
        //0, 0, (16+16/3)*NUM_SPCTRM, 60
        //0, 0, 29, 96
        0, 0, 300, 98
    },
    ID_MUSIC_PARTICLE_BMP,  //height of  particle is 60, ID_MUSIC_PARTICLE_UF_800X480_BMP
    ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_800X480_BMP
};

static __spctrm_uipara_t spectrum_uipara_720_576 =
{
    {
        //428,  140,    160,    80          //frame win
        400 - 80, 284 - 46 + 25, 300, 98
    },
    {
        //0, 0, (16+16/3)*NUM_SPCTRM, 60
        //0, 0, 29, 96
        0, 0, 300, 98
    },
    ID_MUSIC_PARTICLE_BMP,  //height of  particle is 60, ID_MUSIC_PARTICLE_UF_800X480_BMP
    ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_800X480_BMP
};

static __spctrm_uipara_t spectrum_uipara_800x480 =
{
    {
        624, 140, 315, 200                      //frame win
    },
    {
        //0, 0, (16+16/3)*NUM_SPCTRM, 60
        0, 0, 315, 200
    },
    ID_MUSIC_PARTICLE_BMP,  //height of  particle is 60, ID_MUSIC_PARTICLE_UF_800X480_BMP
    ID_MUSIC_BOUY_BMP//ID_MUSIC_BALL_UF_800X480_BMP
};

__spctrm_uipara_t *spctrm_get_uipara(void)
{
    __s32           screen_width;
    __s32           screen_height;
    dsk_display_get_size(&screen_width, &screen_height);

    if ((screen_width == 800) && (screen_height == 480))
    {
        return &spectrum_uipara_800_480;
    }
    else if ((screen_width == 720) && (screen_height == 480))
    {
        return &spectrum_uipara_720_480;
    }
    else if ((screen_width == 720) && (screen_height == 576))
    {
        return &spectrum_uipara_720_576;
    }
    else if ((screen_width == 400) && (screen_height == 240))
    {
        return &spectrum_uipara_400_240;
    }

    return &spectrum_uipara_800x480;
}


//获得Explorer 窗口layer size
__s32 spctrm_get_screen_rect(RECT *screen_rect)
{
    __spctrm_uipara_t *ui_param;
    ui_param = spctrm_get_uipara();
    screen_rect->x = ui_param->para_frmwin.x;
    screen_rect->y = ui_param->para_frmwin.y;
    screen_rect->width = ui_param->para_frmwin.width;
    screen_rect->height = ui_param->para_frmwin.height;
    return EPDK_OK;
}
