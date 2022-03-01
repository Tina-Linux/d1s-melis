/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : sset.c
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/
#include "sset_i.h"

__s32   set_scn_size;

//////////////////////////////////////////////////////////////////////////
#define CALC_CENTER_(_TotalLen, _Num, _PerLen)  (((_TotalLen) / (_Num) - (_PerLen)) / 2)
#define CENTER_VALUE_(_TotalLen, _PerLen)       CALC_CENTER_(_TotalLen, 1, _PerLen)
//////////////////////////////////////////////////////////////////////////
#define MY_SSET_LAYOUT   sset_layout_800x480
#define MY_SCREEN_SIZE   SCREEN_SIZE_800_480
#define MY_LCD_W         800
#define MY_LCD_H         480

#define TBAR_ITEM_NUM                6

#define TBAR_RT_W                    MY_LCD_W
#define TBAR_RT_H                    118
#define TBAR_RT_X                    0
#define TBAR_RT_Y                    (MY_LCD_H - TBAR_RT_H)

#define TBAR_ITEM_X                  0
#define TBAR_ITEM_Y                  0
#define TBAR_ITEM_W                  (MY_LCD_W / TBAR_ITEM_NUM)
#define TBAR_ITEM_H                  TBAR_RT_H

#define TBAR_ITEM_BMP_W              70//35
#define TBAR_ITEM_BMP_H              70//30
#define TBAR_ITEM_BMP_X              CENTER_VALUE_(TBAR_ITEM_W, TBAR_ITEM_BMP_W)
#define TBAR_ITEM_BMP_Y              CENTER_VALUE_(TBAR_ITEM_H, TBAR_ITEM_BMP_H)


#define LSTM_BMP_W                   246
#define LSTM_BMP_H                   57
#define LSTM_BMP_X                   0
#define LSTM_BMP_Y                   0

#define LSTM_RT_W                    LSTM_BMP_W
#define LSTM_RT_H                    (LSTM_BMP_H)
#define LSTM_RT_X                    (MY_LCD_W - LSTM_RT_W)
#define LSTM_RT_Y                    46



#define APP_MAX_LIST_RT_H            (LSTM_BMP_H * APP_MAX_LIST_ITEM_COUNT)


static __sset_uipara_t sset_layout_480x272 =
{
    /* lcd        */
    {MY_LCD_W, MY_LCD_H},           //lcd

    /* layer        */
    //tbar_lyr
    {
        TBAR_RT_X, TBAR_RT_Y, TBAR_RT_W, TBAR_RT_H,          // scn
        0, 0, TBAR_RT_W, TBAR_RT_H,                          // src
        TBAR_RT_W, TBAR_RT_H,                                // fb
        PIXEL_COLOR_ARGB8888,                              //..PIXEL_MONO_8BPP,                                                   // fmt
        1                                                    // pipe
    },
    //lstm_lyr
    {
        LSTM_RT_X, LSTM_RT_Y, LSTM_RT_W, LSTM_RT_H,          // scn
        0, 0, LSTM_RT_W, LSTM_RT_H,                  // src
        LSTM_RT_W, APP_MAX_LIST_RT_H,                        // fb
        PIXEL_COLOR_ARGB8888,                                                   // fmt
        1                                                    // pipe
    },

    /* frame        */
    {0, 0, 0, 0},                                            //tbar_frm
    {0, 0, LSTM_RT_W, APP_MAX_LIST_RT_H},                    //lstm_frm

    /* control        */
    {0, 0, LSTM_RT_W, APP_MAX_LIST_RT_H},                    //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {0, 0, TBAR_ITEM_W, TBAR_ITEM_H},                        //menu_tbar_item
    {CENTER_VALUE_(720 / 5, TBAR_ITEM_BMP_W), 0, TBAR_ITEM_BMP_W, TBAR_ITEM_BMP_H}, //menu_tbar_bmp
    {CENTER_VALUE_(720 / 5, TBAR_ITEM_BMP_W) - 5, TBAR_ITEM_BMP_H, TBAR_ITEM_BMP_W, 16},        //menu_tbar_txt

    {0, 0, LSTM_BMP_W, LSTM_BMP_H},         //menu_lstm_bmp
    {30, 5, 0, 0},          //menu_lstm_txt
    {0, 0, LSTM_BMP_H, 0},          //menu_lstm_fcs
    {5, 11, 9, 9},          //menu_opt_bmp

    {150, 3, 15, 16},       //menu_spin_inc_bmp
    {14, 3, 15, 16},        //menu_spin_dec_bmp
    {90, 7, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

static __sset_uipara_t sset_layout_800x480 =
{
    /* lcd        */
    {MY_LCD_W, MY_LCD_H},           //lcd

    /* layer        */
    //tbar_lyr
    {
        TBAR_RT_X, TBAR_RT_Y, TBAR_RT_W, TBAR_RT_H,          // scn
        0, 0, TBAR_RT_W, TBAR_RT_H,                          // src
        TBAR_RT_W, TBAR_RT_H,                                // fb
        PIXEL_COLOR_ARGB8888,                                // fmt
        1                                                    // pipe
        //0,480-113,800,133,
        //0,0,800,133,
        //800,133,
        //PIXEL_COLOR_ARGB8888,                                // fmt
        / 1
    },
    //lstm_lyr
    {
        LSTM_RT_X, LSTM_RT_Y, LSTM_RT_W, LSTM_RT_H,          // scn
        0, 0, LSTM_RT_W, LSTM_RT_H,                  // src
        LSTM_RT_W, APP_MAX_LIST_RT_H,                        // fb
        PIXEL_COLOR_ARGB8888,                                                   // fmt
        1                                                    // pipe
    },

    /* frame        */
    {0, 0, 0, 0},                                            //tbar_frm
    {0, 0, LSTM_RT_W, APP_MAX_LIST_RT_H},                    //lstm_frm

    /* control        */
    {0, 0, LSTM_RT_W, APP_MAX_LIST_RT_H},                    //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {0, 0, TBAR_ITEM_W, TBAR_ITEM_H},                        //menu_tbar_item
    {TBAR_ITEM_BMP_X, 0, TBAR_ITEM_BMP_W, TBAR_ITEM_BMP_H}, //menu_tbar_bmp
    {TBAR_ITEM_BMP_X - 5, TBAR_ITEM_BMP_H, TBAR_ITEM_BMP_W, 16},        //menu_tbar_txt
    //{0,0,150,46},                                         //menu_tbar_item
    //{133, 0,35, 30},                           //menu_tbar_bmp
    //{133-5,30,35,16}                          //menu_tbar_txt


    {0, 0, LSTM_BMP_W, LSTM_BMP_H},         //menu_lstm_bmp
    {30, 15, 0, 0},         //menu_lstm_txt
    {0, 0, LSTM_BMP_H, 0},          //menu_lstm_fcs
    {5, 11, 9, 9},          //menu_opt_bmp

    {150, 3, 15, 16},       //menu_spin_inc_bmp
    {14, 3, 15, 16},        //menu_spin_dec_bmp
    {90, 7, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};


static __sset_uipara_t sset_layout_720x480 =
{
    /* lcd        */
    {MY_LCD_W - 80, MY_LCD_H},          //lcd

    /* layer        */
    //tbar_lyr
    {
        TBAR_RT_X, TBAR_RT_Y, TBAR_RT_W - 80, TBAR_RT_H,        // scn
        0, 0, TBAR_RT_W - 80, TBAR_RT_H,                        // src
        TBAR_RT_W - 80, TBAR_RT_H,                              // fb
        PIXEL_COLOR_ARGB8888,                                // fmt
        1                                                    // pipe
        //0,480-113,800,133,
        //0,0,800,133,
        //800,133,
        //PIXEL_COLOR_ARGB8888,                                // fmt
        / 1
    },
    //lstm_lyr
    {
        LSTM_RT_X - 80, LSTM_RT_Y, LSTM_RT_W, LSTM_RT_H,        // scn
        0, 0, LSTM_RT_W, LSTM_RT_H,                  // src
        LSTM_RT_W, APP_MAX_LIST_RT_H,                        // fb
        PIXEL_COLOR_ARGB8888,                                                   // fmt
        1                                                    // pipe
    },

    /* frame        */
    {0, 0, 0, 0},                                            //tbar_frm
    {0, 0, LSTM_RT_W, APP_MAX_LIST_RT_H},                    //lstm_frm

    /* control        */
    {0, 0, LSTM_RT_W, APP_MAX_LIST_RT_H},                    //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {0, 0, 720 / 5, TBAR_ITEM_H},                    //menu_tbar_item
    {TBAR_ITEM_BMP_X, 10, TBAR_ITEM_BMP_W, TBAR_ITEM_BMP_H}, //menu_tbar_bmp
    {TBAR_ITEM_BMP_X - 5, TBAR_ITEM_BMP_H + 10, TBAR_ITEM_BMP_W, 16},       //menu_tbar_txt
    //{0,0,150,46},                                         //menu_tbar_item
    //{133, 0,35, 30},                           //menu_tbar_bmp
    //{133-5,30,35,16}                          //menu_tbar_txt


    {0, 0, LSTM_BMP_W, LSTM_BMP_H},         //menu_lstm_bmp
    {30, 15, 0, 0},         //menu_lstm_txt
    {0, 0, LSTM_BMP_H, 0},          //menu_lstm_fcs
    {5, 11, 9, 9},          //menu_opt_bmp

    {150, 3, 15, 16},       //menu_spin_inc_bmp
    {14, 3, 15, 16},        //menu_spin_dec_bmp
    {90, 7, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};




__sset_uipara_t *SSET_Get_Uipara(void)
{
    __s32                   lcd_width;
    __s32                   lcd_height;
    dsk_display_get_size(&lcd_width, &lcd_height);
    eLIBs_printf("lcd_width=%d lcd_height=%d \n", lcd_width, lcd_height);

    if (lcd_width == 800 && lcd_height == 480)
    {
        set_scn_size = MY_SCREEN_SIZE;
        return &sset_layout_800x480;
    }
    else if (lcd_width == 720 && ((lcd_height == 480) || (lcd_height == 576)))
    {
        set_scn_size = SCREEN_SIZE_720_480;
        return &sset_layout_720x480;
    }
    else
    {
        set_scn_size = MY_SCREEN_SIZE;
        return &MY_SSET_LAYOUT;
    }
}
//////////////////////////////////////////////////////////////////////////

#if 0//减少内存占用

static __sset_uipara_t sset_layout_1920x1080 =
{
    /* lcd        */
    {1920, 1080},           //lcd

    /* layer        */
    {0, 950, 1920, 95, 200, 0, 400, 95, 800, 95, 0, 1}, //tbar_lyr
    {1680, 156, 202, 408, 0, 0, 202, 816, 202, 816, 0, 1}, //lstm_lyr

    /* frame        */
    {0, 0, 0, 0},           //tbar_frm
    {0, 0, 202, 816},       //lstm_frm

    /* control        */
    {0, 0, 202, 816},       //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {450, 0, 100, 95},      //menu_tbar_item
    {10, 10, 72, 72},       //menu_tbar_bmp
    {0, 0, 0, 0},           //menu_tbar_txt

    {0, 0, 202, 51},        //menu_lstm_bmp
    {30, 11, 0, 0},         //menu_lstm_txt
    {0, 0, 51, 0},          //menu_lstm_fcs
    {5, 14, 24, 24},        //menu_opt_bmp

    {150, 3, 32, 40},       //menu_spin_inc_bmp
    {14, 3, 32, 40},        //menu_spin_dec_bmp
    {90, 5, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

static __sset_uipara_t sset_layout_1024x768 =
{
    /* lcd        */
    {1024, 768},        //lcd

    /* layer        */
    {0, 505, 1024, 95, 200, 0, 400, 95, 800, 95, 0, 1}, //tbar_lyr
    {800, 156, 202, 408, 0, 0, 202, 816, 202, 816, 0, 1},   //lstm_lyr

    /* frame        */
    {0, 0, 0, 0},           //tbar_frm
    {0, 0, 202, 816},       //lstm_frm

    /* control        */
    {0, 0, 202, 816},       //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {20, 0, 100, 95},       //menu_tbar_item
    {10, 10, 72, 72},       //menu_tbar_bmp
    {0, 0, 0, 0},           //menu_tbar_txt

    {0, 0, 202, 51},        //menu_lstm_bmp
    {30, 11, 0, 0},         //menu_lstm_txt
    {0, 0, 51, 0},          //menu_lstm_fcs
    {5, 14, 24, 24},        //menu_opt_bmp

    {150, 3, 32, 40},       //menu_spin_inc_bmp
    {14, 3, 32, 40},        //menu_spin_dec_bmp
    {90, 5, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

static __sset_uipara_t sset_layout_1280x720 =
{
    /* lcd        */
    {1280, 720},        //lcd

    /* layer        */
    {0, 600, 1280, 95, 200, 0, 400, 95, 800, 95, 0, 1}, //tbar_lyr
    {1040, 156, 202, 408, 0, 0, 202, 816, 202, 816, 0, 1}, //lstm_lyr

    /* frame        */
    {0, 0, 0, 0},           //tbar_frm
    {0, 0, 202, 816},       //lstm_frm

    /* control        */
    {0, 0, 202, 816},       //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {130, 0, 100, 95},      //menu_tbar_item
    {10, 10, 72, 72},       //menu_tbar_bmp
    {0, 0, 0, 0},           //menu_tbar_txt

    {0, 0, 202, 51},        //menu_lstm_bmp
    {30, 11, 0, 0},         //menu_lstm_txt
    {0, 0, 51, 0},          //menu_lstm_fcs
    {5, 14, 24, 24},        //menu_opt_bmp

    {150, 3, 32, 40},       //menu_spin_inc_bmp
    {14, 3, 32, 40},        //menu_spin_dec_bmp
    {90, 5, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

static __sset_uipara_t sset_layout_800x480 =
{
    /* lcd        */
    {800, 480},         //lcd

    /* layer        */
    {0, 385, 800, 95, 200, 0, 400, 95, 800, 95, 0, 1},      //tbar_lyr
    {598, 36, 202, 408, 0, 0, 202, 816, 202, 816, 0, 1},    //lstm_lyr

    /* frame        */
    {0, 0, 0, 0},           //tbar_frm
    {0, 0, 202, 816},       //lstm_frm

    /* control        */
    {0, 0, 202, 816},       //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {0, 0, 100, 95},        //menu_tbar_item
    {10, 10, 72, 72},       //menu_tbar_bmp
    {0, 0, 0, 0},           //menu_tbar_txt

    {0, 0, 202, 51},        //menu_lstm_bmp
    {30, 11, 0, 0},         //menu_lstm_txt
    {0, 0, 51, 0},          //menu_lstm_fcs
    {5, 14, 24, 24},        //menu_opt_bmp

    {150, 3, 32, 40},       //menu_spin_inc_bmp
    {14, 3, 32, 40},        //menu_spin_dec_bmp
    {90, 7, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

static __sset_uipara_t sset_layout_720x576 =
{
    /* lcd        */
    {720, 576},         //lcd

    /* layer        */
    {15, 450, 690, 95, 200, 0, 400, 95, 690, 95, 0, 1},     //tbar_lyr
    {500, 36, 202, 480, 0, 0, 202, 816, 202, 816, 0, 1},    //lstm_lyr

    /* frame        */
    {0, 0, 0, 0},           //tbar_frm
    {0, 0, 202, 816},       //lstm_frm

    /* control        */
    {0, 0, 202, 816},       //lstm_ctl
    {2, 2, 196, 45},        //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {20, 0, 100, 95},       //menu_tbar_item
    {10, 10, 72, 72},       //menu_tbar_bmp
    {0, 0, 0, 0},           //menu_tbar_txt

    {0, 0, 202, 51},        //menu_lstm_bmp
    {30, 11, 0, 0},         //menu_lstm_txt
    {0, 0, 51, 0},          //menu_lstm_fcs
    {5, 14, 24, 24},        //menu_opt_bmp

    {150, 3, 32, 40},       //menu_spin_inc_bmp
    {14, 3, 32, 40},        //menu_spin_dec_bmp
    {90, 5, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

static __sset_uipara_t sset_layout_720x480 =
{
    /* lcd        */
    {720, 480},         //lcd

    /* layer        */
    {15, 380, 690, 95, 200, 0, 400, 95, 690, 95, 0, 1},     //tbar_lyr
    {500, 36, 202, 408, 0, 0, 202, 816, 202, 816, 0, 1},    //lstm_lyr

    /* frame        */
    {0, 0, 0, 0},           //tbar_frm
    {0, 0, 202, 816},       //lstm_frm

    /* control        */
    {0, 0, 202, 816},       //lstm_ctl
    {10, 5, 180, 40},       //spin_ctl
    {0, 10, 200, 32},       //slid_ctl
    {0, 0, 0, 0},           //stch_ctl

    /* item         */
    {20, 0, 100, 95},       //menu_tbar_item
    {10, 10, 72, 72},       //menu_tbar_bmp
    {0, 0, 0, 0},           //menu_tbar_txt

    {0, 0, 202, 51},        //menu_lstm_bmp
    {30, 11, 0, 0},         //menu_lstm_txt
    {0, 0, 51, 0},          //menu_lstm_fcs
    {5, 14, 24, 24},        //menu_opt_bmp

    {150, 3, 32, 40},       //menu_spin_inc_bmp
    {14, 3, 32, 40},        //menu_spin_dec_bmp
    {90, 5, 0, 0},          //menu_spin_txt

    {0, 0, 20, 5},      //menu_slid_head_bmp
    {0, 0, 20, 5},      //menu_slid_tail_bmp
    {0, 0, 20, 5},      //menu_slid_curr_bmp

    {0, 0, 0, 0},       //menu_stch_bmp

};

__sset_uipara_t *SSET_Get_Uipara(void)
{
    __s32                   lcd_width;
    __s32                   lcd_height;
    dsk_display_get_size(&lcd_width, &lcd_height);

    if (lcd_width == 720 && lcd_height == 480)
    {
        set_scn_size = SCREEN_SIZE_720_480;
        return &sset_layout_720x480;
    }
    else if (lcd_width == 720 && lcd_height == 576)
    {
        set_scn_size = SCREEN_SIZE_720_576;
        return &sset_layout_720x576;
    }
    else if (lcd_width == 800 && lcd_height == 480)
    {
        set_scn_size = SCREEN_SIZE_800_480;
        return &sset_layout_800x480;
    }
    else if (lcd_width == 1280 && lcd_height == 720)
    {
        set_scn_size = SCREEN_SIZE_1280_720;
        return &sset_layout_1280x720;
    }
    else if (lcd_width == 1024 && lcd_height == 768)
    {
        set_scn_size = SCREEN_SIZE_1024_768;
        return &sset_layout_1024x768;
    }
    else if (lcd_width == 1920 && lcd_height == 1080)
    {
        set_scn_size = SCREEN_SIZE_1920_1080;
        return &sset_layout_1920x1080;
    }
    else
    {
        return NULL;
    }
}

#endif

