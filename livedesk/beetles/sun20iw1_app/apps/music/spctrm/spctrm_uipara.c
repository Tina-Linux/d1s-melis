/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, JOHN, China
*                                           All Rights Reserved
*
* File    : spctrm_uipara.c
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
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


//»ñµÃExplorer ´°¿Úlayer size
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


