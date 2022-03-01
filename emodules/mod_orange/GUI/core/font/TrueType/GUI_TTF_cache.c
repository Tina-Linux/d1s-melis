/*
*********************************************************************************************************
*                                             uC/GUI V4.10
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TTF.c
Purpose     : Implementation of external binary fonts
---------------------------END-OF-HEADER------------------------------
*/

#include "TTF_private.h"

#include "include/ft2build.h"
#include FT_FREETYPE_H

#include FT_MODULE_H

#include FT_CACHE_H
#include FT_CACHE_MANAGER_H

#if GUI_COMPILER_SUPPORTS_FP

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define FAMILY_NAME 0
#define STYLE_NAME  1


static int  SHADOW_SIZE_I = 0;
static GUI_COLOR Colors_aRGB[256];
static U32 fontBuffer[50 * 50]; // for shadow
static GUI_COLOR Colors_MONO[2] = {0, 0xffffffff};

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct
{
    FT_Library        library;           /* the FreeType library            */
    FTC_Manager       cache_manager;     /* the cache manager               */
    FTC_ImageCache    image_cache;       /* the glyph image cache           */
    FTC_SBitCache     sbits_cache;       /* the glyph small bitmaps cache   */
    FTC_CMapCache     cmap_cache;        /* the charmap cache..             */
} FT_CONTEXT;

typedef struct
{
    unsigned MaxFaces; /* If not set the default will be 2 */
    unsigned MaxSizes; /* If not set the default will be 4 */
    U32      MaxBytes; /* If not set the default will be 200000L */
} TF_CACHE_SIZE;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
FT_CONTEXT    _FTContext;
TF_CACHE_SIZE _FTCacheSize;
/*palette*/
#if 0
static GUI_CONST_STORAGE GUI_COLOR Colors33_b[] =
{
    0xffffff, 0xfefefe, 0xfdfdfd, 0xfcfcfc,
    0xfbfbfb, 0xfafafa, 0xf9f9f9, 0xf8f8f8,
    0xf7f7f7, 0xf6f6f6, 0xf5f5f5, 0xf4f4f4,
    0xf3f3f3, 0xf2f2f2, 0xf1f1f1, 0xf0f0f0,
    0xefefef, 0xeeeeee, 0xededed, 0xececec,
    0xebebeb, 0xeaeaea, 0xe9e9e9, 0xe8e8e8,
    0xe7e7e7, 0xe6e6e6, 0xe5e5e5, 0xe4e4e4,
    0xe3e3e3, 0xe2e2e2, 0xe1e1e1, 0xe0e0e0,
    0xdfdfdf, 0xdedede, 0xdddddd, 0xdcdcdc,
    0xdbdbdb, 0xdadada, 0xd9d9d9, 0xd8d8d8,
    0xd7d7d7, 0xd6d6d6, 0xd5d5d5, 0xd4d4d4,
    0xd3d3d3, 0xd2d2d2, 0xd1d1d1, 0xd0d0d0,
    0xcfcfcf, 0xcecece, 0xcdcdcd, 0xcccccc,
    0xcbcbcb, 0xcacaca, 0xc9c9c9, 0xc8c8c8,
    0xc7c7c7, 0xc6c6c6, 0xc5c5c5, 0xc4c4c4,
    0xc3c3c3, 0xc2c2c2, 0xc1c1c1, 0xc0c0c0,
    0xbfbfbf, 0xbebebe, 0xbdbdbd, 0xbcbcbc,
    0xbbbbbb, 0xbababa, 0xb9b9b9, 0xb8b8b8,
    0xb7b7b7, 0xb6b6b6, 0xb5b5b5, 0xb4b4b4,
    0xb3b3b3, 0xb2b2b2, 0xb1b1b1, 0xb0b0b0,
    0xafafaf, 0xaeaeae, 0xadadad, 0xacacac,
    0xababab, 0xaaaaaa, 0xa9a9a9, 0xa8a8a8,
    0xa7a7a7, 0xa6a6a6, 0xa5a5a5, 0xa4a4a4,
    0xa3a3a3, 0xa2a2a2, 0xa1a1a1, 0xa0a0a0,
    0x9f9f9f, 0x9e9e9e, 0x9d9d9d, 0x9c9c9c,
    0x9b9b9b, 0x9a9a9a, 0x999999, 0x989898,
    0x979797, 0x969696, 0x959595, 0x949494,
    0x939393, 0x929292, 0x919191, 0x909090,
    0x8f8f8f, 0x8e8e8e, 0x8d8d8d, 0x8c8c8c,
    0x8b8b8b, 0x8a8a8a, 0x898989, 0x888888,
    0x878787, 0x868686, 0x858585, 0x848484,
    0x838383, 0x828282, 0x818181, 0x808080,
    0x7f7f7f, 0x7e7e7e, 0x7d7d7d, 0x7c7c7c,
    0x7b7b7b, 0x7a7a7a, 0x797979, 0x787878,
    0x777777, 0x767676, 0x757575, 0x747474,
    0x737373, 0x727272, 0x717171, 0x707070,
    0x6f6f6f, 0x6e6e6e, 0x6d6d6d, 0x6c6c6c,
    0x6b6b6b, 0x6a6a6a, 0x696969, 0x686868,
    0x676767, 0x666666, 0x656565, 0x646464,
    0x636363, 0x626262, 0x616161, 0x606060,
    0x5f5f5f, 0x5e5e5e, 0x5d5d5d, 0x5c5c5c,
    0x5b5b5b, 0x5a5a5a, 0x595959, 0x585858,
    0x575757, 0x565656, 0x555555, 0x545454,
    0x535353, 0x525252, 0x515151, 0x505050,
    0x4f4f4f, 0x4e4e4e, 0x4d4d4d, 0x4c4c4c,
    0x4b4b4b, 0x4a4a4a, 0x494949, 0x484848,
    0x474747, 0x464646, 0x454545, 0x444444,
    0x434343, 0x424242, 0x414141, 0x404040,
    0x3f3f3f, 0x3e3e3e, 0x3d3d3d, 0x3c3c3c,
    0x3b3b3b, 0x3a3a3a, 0x393939, 0x383838,
    0x373737, 0x363636, 0x353535, 0x343434,
    0x333333, 0x323232, 0x313131, 0x303030,
    0x2f2f2f, 0x2e2e2e, 0x2d2d2d, 0x2c2c2c,
    0x2b2b2b, 0x2a2a2a, 0x292929, 0x282828,
    0x272727, 0x262626, 0x252525, 0x242424,
    0x232323, 0x222222, 0x212121, 0x202020,
    0x1f1f1f, 0x1e1e1e, 0x1d1d1d, 0x1c1c1c,
    0x1b1b1b, 0x1a1a1a, 0x191919, 0x181818,
    0x171717, 0x161616, 0x151515, 0x141414,
    0x131313, 0x121212, 0x111111, 0x101010,
    0x0f0f0f, 0x0e0e0e, 0x0d0d0d, 0x0c0c0c,
    0x0b0b0b, 0x0a0a0a, 0x090909, 0x080808,
    0x070707, 0x060606, 0x050505, 0x040404,
    0x030303, 0x020202, 0x010101, 0x000000
};

#endif
#if 0
static GUI_CONST_STORAGE GUI_COLOR Colors33_b[] =
{
    0x000000, 0x010101, 0x020202, 0x030303
    , 0x040404, 0x050505, 0x060606, 0x070707
    , 0x080808, 0x090909, 0x0A0A0A, 0x0B0B0B
    , 0x0C0C0C, 0x0D0D0D, 0x0E0E0E, 0x0F0F0F
    , 0x101010, 0x111111, 0x121212, 0x131313
    , 0x141414, 0x151515, 0x161616, 0x171717
    , 0x181818, 0x191919, 0x1A1A1A, 0x1B1B1B
    , 0x1C1C1C, 0x1D1D1D, 0x1E1E1E, 0x1F1F1F
    , 0x202020, 0x212121, 0x222222, 0x232323
    , 0x242424, 0x252525, 0x262626, 0x272727
    , 0x282828, 0x292929, 0x2A2A2A, 0x2B2B2B
    , 0x2C2C2C, 0x2D2D2D, 0x2E2E2E, 0x2F2F2F
    , 0x303030, 0x313131, 0x323232, 0x333333
    , 0x343434, 0x353535, 0x363636, 0x373737
    , 0x383838, 0x393939, 0x3A3A3A, 0x3B3B3B
    , 0x3C3C3C, 0x3D3D3D, 0x3E3E3E, 0x3F3F3F
    , 0x404040, 0x414141, 0x424242, 0x434343
    , 0x444444, 0x454545, 0x464646, 0x474747
    , 0x484848, 0x494949, 0x4A4A4A, 0x4B4B4B
    , 0x4C4C4C, 0x4D4D4D, 0x4E4E4E, 0x4F4F4F
    , 0x505050, 0x515151, 0x525252, 0x535353
    , 0x545454, 0x555555, 0x565656, 0x575757
    , 0x585858, 0x595959, 0x5A5A5A, 0x5B5B5B
    , 0x5C5C5C, 0x5D5D5D, 0x5E5E5E, 0x5F5F5F
    , 0x606060, 0x616161, 0x626262, 0x636363
    , 0x646464, 0x656565, 0x666666, 0x676767
    , 0x686868, 0x696969, 0x6A6A6A, 0x6B6B6B
    , 0x6C6C6C, 0x6D6D6D, 0x6E6E6E, 0x6F6F6F
    , 0x707070, 0x717171, 0x727272, 0x737373
    , 0x747474, 0x757575, 0x767676, 0x777777
    , 0x787878, 0x797979, 0x7A7A7A, 0x7B7B7B
    , 0x7C7C7C, 0x7D7D7D, 0x7E7E7E, 0x7F7F7F
    , 0x808080, 0x818181, 0x828282, 0x838383
    , 0x848484, 0x858585, 0x868686, 0x878787
    , 0x888888, 0x898989, 0x8A8A8A, 0x8B8B8B
    , 0x8C8C8C, 0x8D8D8D, 0x8E8E8E, 0x8F8F8F
    , 0x909090, 0x919191, 0x929292, 0x939393
    , 0x949494, 0x959595, 0x969696, 0x979797
    , 0x989898, 0x999999, 0x9A9A9A, 0x9B9B9B
    , 0x9C9C9C, 0x9D9D9D, 0x9E9E9E, 0x9F9F9F
    , 0xA0A0A0, 0xA1A1A1, 0xA2A2A2, 0xA3A3A3
    , 0xA4A4A4, 0xA5A5A5, 0xA6A6A6, 0xA7A7A7
    , 0xA8A8A8, 0xA9A9A9, 0xAAAAAA, 0xABABAB
    , 0xACACAC, 0xADADAD, 0xAEAEAE, 0xAFAFAF
    , 0xB0B0B0, 0xB1B1B1, 0xB2B2B2, 0xB3B3B3
    , 0xB4B4B4, 0xB5B5B5, 0xB6B6B6, 0xB7B7B7
    , 0xB8B8B8, 0xB9B9B9, 0xBABABA, 0xBBBBBB
    , 0xBCBCBC, 0xBDBDBD, 0xBEBEBE, 0xBFBFBF
    , 0xC0C0C0, 0xC1C1C1, 0xC2C2C2, 0xC3C3C3
    , 0xC4C4C4, 0xC5C5C5, 0xC6C6C6, 0xC7C7C7
    , 0xC8C8C8, 0xC9C9C9, 0xCACACA, 0xCBCBCB
    , 0xCCCCCC, 0xCDCDCD, 0xCECECE, 0xCFCFCF
    , 0xD0D0D0, 0xD1D1D1, 0xD2D2D2, 0xD3D3D3
    , 0xD4D4D4, 0xD5D5D5, 0xD6D6D6, 0xD7D7D7
    , 0xD8D8D8, 0xD9D9D9, 0xDADADA, 0xDBDBDB
    , 0xDCDCDC, 0xDDDDDD, 0xDEDEDE, 0xDFDFDF
    , 0xE0E0E0, 0xE1E1E1, 0xE2E2E2, 0xE3E3E3
    , 0xE4E4E4, 0xE5E5E5, 0xE6E6E6, 0xE7E7E7
    , 0xE8E8E8, 0xE9E9E9, 0xEAEAEA, 0xEBEBEB
    , 0xECECEC, 0xEDEDED, 0xEEEEEE, 0xEFEFEF
    , 0xF0F0F0, 0xF1F1F1, 0xF2F2F2, 0xF3F3F3
    , 0xF4F4F4, 0xF5F5F5, 0xF6F6F6, 0xF7F7F7
    , 0xF8F8F8, 0xF9F9F9, 0xFAFAFA, 0xFBFBFB
    , 0xFCFCFC, 0xFDFDFD, 0xFEFEFE, 0xFFFFFF
};
#endif
#if 0  //64 curve
static GUI_CONST_STORAGE GUI_COLOR Colors33_b[] =
{
    0x0, 0x10101, 0x20202,
    0x30303, 0x40404, 0x50505, 0x60606,
    0x70707, 0x80808, 0x80808, 0xa0a0a,
    0xa0a0a, 0xc0c0c, 0xc0c0c, 0xc0c0c,
    0xf0f0f, 0xf0f0f, 0xf0f0f, 0x121212,
    0x121212, 0x121212, 0x151515, 0x151515,
    0x151515, 0x181818, 0x181818, 0x181818,
    0x181818, 0x1c1c1c, 0x1c1c1c, 0x1c1c1c,
    0x1c1c1c, 0x202020, 0x202020, 0x202020,
    0x202020, 0x242424, 0x242424, 0x242424,
    0x242424, 0x282828, 0x282828, 0x282828,
    0x282828, 0x282828, 0x2d2d2d, 0x2d2d2d,
    0x2d2d2d, 0x2d2d2d, 0x2d2d2d, 0x323232,
    0x323232, 0x323232, 0x323232, 0x323232,
    0x373737, 0x373737, 0x373737, 0x373737,
    0x373737, 0x3c3c3c, 0x3c3c3c, 0x3c3c3c,
    0x3c3c3c, 0x3c3c3c, 0x3c3c3c, 0x424242,
    0x424242, 0x424242, 0x424242, 0x424242,
    0x424242, 0x484848, 0x484848, 0x484848,
    0x484848, 0x484848, 0x484848, 0x4e4e4e,
    0x4e4e4e, 0x4e4e4e, 0x4e4e4e, 0x4e4e4e,
    0x4e4e4e, 0x545454, 0x545454, 0x545454,
    0x545454, 0x545454, 0x545454, 0x545454,
    0x5b5b5b, 0x5b5b5b, 0x5b5b5b, 0x5b5b5b,
    0x5b5b5b, 0x5b5b5b, 0x5b5b5b, 0x626262,
    0x626262, 0x626262, 0x626262, 0x626262,
    0x626262, 0x626262, 0x696969, 0x696969,
    0x696969, 0x696969, 0x696969, 0x696969,
    0x696969, 0x707070, 0x707070, 0x707070,
    0x707070, 0x707070, 0x707070, 0x707070,
    0x707070, 0x787878, 0x787878, 0x787878,
    0x787878, 0x787878, 0x787878, 0x787878,
    0x7f7f7f, 0x7f7f7f, 0x818181, 0x818181,
    0x818181, 0x818181, 0x818181, 0x818181,
    0x818181, 0x888888, 0x888888, 0x888888,
    0x888888, 0x888888, 0x888888, 0x888888,
    0x888888, 0x909090, 0x909090, 0x909090,
    0x909090, 0x909090, 0x909090, 0x909090,
    0x979797, 0x979797, 0x979797, 0x979797,
    0x979797, 0x979797, 0x979797, 0x9e9e9e,
    0x9e9e9e, 0x9e9e9e, 0x9e9e9e, 0x9e9e9e,
    0x9e9e9e, 0x9e9e9e, 0xa5a5a5, 0xa5a5a5,
    0xa5a5a5, 0xa5a5a5, 0xa5a5a5, 0xa5a5a5,
    0xa5a5a5, 0xacacac, 0xacacac, 0xacacac,
    0xacacac, 0xacacac, 0xacacac, 0xb2b2b2,
    0xb2b2b2, 0xb2b2b2, 0xb2b2b2, 0xb2b2b2,
    0xb2b2b2, 0xb8b8b8, 0xb8b8b8, 0xb8b8b8,
    0xb8b8b8, 0xb8b8b8, 0xb8b8b8, 0xbebebe,
    0xbebebe, 0xbebebe, 0xbebebe, 0xbebebe,
    0xbebebe, 0xc4c4c4, 0xc4c4c4, 0xc4c4c4,
    0xc4c4c4, 0xc4c4c4, 0xc9c9c9, 0xc9c9c9,
    0xc9c9c9, 0xc9c9c9, 0xc9c9c9, 0xcecece,
    0xcecece, 0xcecece, 0xcecece, 0xcecece,
    0xd3d3d3, 0xd3d3d3, 0xd3d3d3, 0xd3d3d3,
    0xd3d3d3, 0xd8d8d8, 0xd8d8d8, 0xd8d8d8,
    0xd8d8d8, 0xdcdcdc, 0xdcdcdc, 0xdcdcdc,
    0xdcdcdc, 0xe0e0e0, 0xe0e0e0, 0xe0e0e0,
    0xe0e0e0, 0xe4e4e4, 0xe4e4e4, 0xe4e4e4,
    0xe4e4e4, 0xe8e8e8, 0xe8e8e8, 0xe8e8e8,
    0xebebeb, 0xebebeb, 0xebebeb, 0xeeeeee,
    0xeeeeee, 0xeeeeee, 0xf1f1f1, 0xf1f1f1,
    0xf1f1f1, 0xf4f4f4, 0xf4f4f4, 0xf6f6f6,
    0xf6f6f6, 0xf8f8f8, 0xf9f9f9, 0xfafafa,
    0xfbfbfb, 0xfcfcfc, 0xfdfdfd, 0xfefefe,
    0xffffff

};
#endif
/*64 line */
#if  1
static   GUI_COLOR Colors33_b[256] =
{
    0xff000000, 0xff010101, 0xff020202, 0xff030303,
    0xff040404, 0xff050505, 0xff060606, 0xff070707,
    0xff080808, 0xff080808, 0xff0a0a0a,
    0xff0a0a0a, 0xff0c0c0c, 0xff0c0c0c, 0xff0c0c0c,
    0xff0f0f0f, 0xff0f0f0f, 0xff0f0f0f, 0xff121212,
    0xff121212, 0xff121212, 0xff151515, 0xff151515,
    0xff151515, 0xff181818, 0xff181818, 0xff181818,
    0xff181818, 0xff1c1c1c, 0xff1c1c1c, 0xff1c1c1c,
    0xff1c1c1c, 0xff202020, 0xff202020, 0xff202020,
    0xff202020, 0xff242424, 0xff242424, 0xff242424,
    0xff242424, 0xff282828, 0xff282828, 0xff282828,
    0xff282828, 0xff282828, 0xff2d2d2d, 0xff2d2d2d,
    0xff2d2d2d, 0xff2d2d2d, 0xff2d2d2d, 0xff323232,
    0xff323232, 0xff323232, 0xff323232, 0xff323232,
    0xff373737, 0xff373737, 0xff373737, 0xff373737,
    0xff373737, 0xff3c3c3c, 0xff3c3c3c, 0xff3c3c3c,
    0xff3c3c3c, 0xff3c3c3c, 0xff3c3c3c, 0xff424242,
    0xff424242, 0xff424242, 0xff424242, 0xff424242,
    0xff424242, 0xff484848, 0xff484848, 0xff484848,
    0xff484848, 0xff484848, 0xff484848, 0xff4e4e4e,
    0xff4e4e4e, 0xff4e4e4e, 0xff4e4e4e, 0xff4e4e4e,
    0xff4e4e4e, 0xff545454, 0xff545454, 0xff545454,
    0xff545454, 0xff545454, 0xff545454, 0xff545454,
    0xff5b5b5b, 0xff5b5b5b, 0xff5b5b5b, 0xff5b5b5b,
    0xff5b5b5b, 0xff5b5b5b, 0xff5b5b5b, 0xff626262,
    0xff626262, 0xff626262, 0xff626262, 0xff626262,
    0xff626262, 0xff626262, 0xff696969, 0xff696969,
    0xff696969, 0xff696969, 0xff696969, 0xff696969,
    0xff696969, 0xff707070, 0xff707070, 0xff707070,
    0xff707070, 0xff707070, 0xff707070, 0xff707070,
    0xff707070, 0xff787878, 0xff787878, 0xff787878,
    0xff787878, 0xff787878, 0xff787878, 0xff787878,
    0xff7f7f7f, 0xff7f7f7f, 0xff818181, 0xff818181,
    0xff818181, 0xff818181, 0xff818181, 0xff818181,
    0xff818181, 0xff888888, 0xff888888, 0xff888888,
    0xff888888, 0xff888888, 0xff888888, 0xff888888,
    0xff888888, 0xff909090, 0xff909090, 0xff909090,
    0xff909090, 0xff909090, 0xff909090, 0xff909090,
    0xff979797, 0xff979797, 0xff979797, 0xff979797,
    0xff979797, 0xff979797, 0xff979797, 0xff9e9e9e,
    0xff9e9e9e, 0xff9e9e9e, 0xff9e9e9e, 0xff9e9e9e,
    0xff9e9e9e, 0xff9e9e9e, 0xffa5a5a5, 0xffa5a5a5,
    0xffa5a5a5, 0xffa5a5a5, 0xffa5a5a5, 0xffa5a5a5,
    0xffa5a5a5, 0xffacacac, 0xffacacac, 0xffacacac,
    0xffacacac, 0xffacacac, 0xffacacac, 0xffb2b2b2,
    0xffb2b2b2, 0xffb2b2b2, 0xffb2b2b2, 0xffb2b2b2,
    0xffb2b2b2, 0xffb8b8b8, 0xffb8b8b8, 0xffb8b8b8,
    0xffb8b8b8, 0xffb8b8b8, 0xffb8b8b8, 0xffbebebe,
    0xffbebebe, 0xffbebebe, 0xffbebebe, 0xffbebebe,
    0xffbebebe, 0xffc4c4c4, 0xffc4c4c4, 0xffc4c4c4,
    0xffc4c4c4, 0xffc4c4c4, 0xffc9c9c9, 0xffc9c9c9,
    0xffc9c9c9, 0xffc9c9c9, 0xffc9c9c9, 0xffcecece,
    0xffcecece, 0xffcecece, 0xffcecece, 0xffcecece,
    0xffd3d3d3, 0xffd3d3d3, 0xffd3d3d3, 0xffd3d3d3,
    0xffd3d3d3, 0xffd8d8d8, 0xffd8d8d8, 0xffd8d8d8,
    0xffd8d8d8, 0xffdcdcdc, 0xffdcdcdc, 0xffdcdcdc,
    0xffdcdcdc, 0xffe0e0e0, 0xffe0e0e0, 0xffe0e0e0,
    0xffe0e0e0, 0xffe4e4e4, 0xffe4e4e4, 0xffe4e4e4,
    0xffe4e4e4, 0xffe8e8e8, 0xffe8e8e8, 0xffe8e8e8,
    0xffebebeb, 0xffebebeb, 0xffebebeb, 0xffeeeeee,
    0xffeeeeee, 0xffeeeeee, 0xfff1f1f1, 0xfff1f1f1,
    0xfff1f1f1, 0xfff4f4f4, 0xfff4f4f4, 0xfff6f6f6,
    0xfff6f6f6, 0xfff8f8f8, 0xfff9f9f9, 0xfffafafa,
    0xfffbfbfb, 0xfffcfcfc, 0xfffdfdfd, 0xfffefefe,
    0xffffffff

};

#endif
static GUI_CONST_STORAGE GUI_LOGPALETTE Pal33gray =
{
    256,  /* number of entries */
    0,    /* No transparency */
    &Colors33_b[0]
};



/*********************************************************************
*
*       _CreateARGBPal
*/
static char _CreateARGBPal(GUI_COLOR *pColor)
{
    int i = 0;

    for (i = 0; i < 256; i++)
    {
        *(pColor + i) = LCD_Color2Index((i << 24) | (GUI_Context.Color & 0x00ffffff));
    }

    return  0;
}




/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbFaceRequester
*
* Purpose:
*  The face requester callback function is used by the cache manager
*  to translate a given FTC_FaceID into a new valid FT_Face object, on demand.
*
* Parameter:
*   face_id      - Id to be translated into a real face object.
*   library      - Handle to a FreeType library object (same as _FTContext.library).
*   pRequestData - Pointer given by FTC_Manager_New() that is passed to the requester each time it is called.
*   pFace        - Pointer to FT_Face object.
*
* Return value:
*   0 on success, != 0 on error.
*/
static FT_Error _cbFaceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer *pRequestData, FT_Face *pFace)
{
    GUI_TTF_DATA *pTTF;
    GUI_USE_PARA(pRequestData);
    pTTF = (GUI_TTF_DATA *)face_id;
    return FT_New_Face(library, pTTF->filePath, 0, pFace);  //MCL edit for file access
}

/*********************************************************************
*
*       _CheckInit
*
* Purpose:
*   Initializes TrueType-engine and caches.
*/
static int _CheckInit(void)
{
    /* Initialize library */
    if (!_FTContext.library)
    {
        if (FT_Init_FreeType(&_FTContext.library))
        {
            return 1; /* Could not initialize FreeType */
        }
    }

    /* Initialize cache manager */
    if (!_FTContext.cache_manager)
    {
        if (FTC_Manager_New(_FTContext.library,
                            _FTCacheSize.MaxFaces,
                            _FTCacheSize.MaxSizes,
                            _FTCacheSize.MaxBytes,
                            (FTC_Face_Requester)_cbFaceRequester,
                            NULL, &_FTContext.cache_manager))
        {
            return 1; /* Could not initialize cache manager */
        }

        /* Initialize charmap cache */
        if (FTC_CMapCache_New(_FTContext.cache_manager, &_FTContext.cmap_cache))
        {
            return 1; /* Could not initialize charmap cache */
        }

        /* Initialize glyph image cache */
        if (FTC_ImageCache_New(_FTContext.cache_manager, &_FTContext.image_cache))
        {
            return 1; /* Could not initialize glyph image cache */
        }

        /* Initialize bitmap cache */
        if (FTC_SBitCache_New(_FTContext.cache_manager, &_FTContext.sbits_cache))
        {
            return 1; /* Could not initialize small bitmaps cache */
        }
    }

    return 0;
}

/*********************************************************************
*
*       _RequestGlyph
*
* Purpose:
*   Draws a glyph (if DoRender == 1) and returns its width
*/
static int _RequestGlyph(U16P c, unsigned DoRender)
{
    GUI_TTF_CS        *pCS;
    FTC_ImageTypeRec *pImageType;
    FTC_FaceID         face_id;
    FT_Face            face;
    FT_UInt            glyph_index;
    FTC_SBitRec       *pSBit;
    FTC_ScalerRec      scaler;
    FT_Size            size;
    int                r;
    const LCD_PIXELINDEX  *pTrans;  //mcl add
    r = -1;
    /* Get object pointer */
    pCS        = (GUI_TTF_CS *)GUI_Context.pAFont->p.pFontData;
    face_id    = (FTC_FaceID)pCS->pTTF;
    pImageType = (FTC_ImageTypeRec *)pCS->aImageTypeBuffer;

    /* Request face object from cache */
    if (FTC_Manager_LookupFace(_FTContext.cache_manager, face_id, &face))
    {
        return r;
    }

    /* Request size object from cache */
    scaler.face_id = face_id;
    scaler.width   = pImageType->width;
    scaler.height  = pImageType->height;
    scaler.pixel   = 1;

    if (FTC_Manager_LookupSize(_FTContext.cache_manager, &scaler, &size))
    {
        return r;
    }

    /* Request glyph index from cache */
    glyph_index = FTC_CMapCache_Lookup(_FTContext.cmap_cache, face_id, 0, c);

    /* Request bitmap from cache */
    if (FTC_SBitCache_Lookup(_FTContext.sbits_cache, pImageType, glyph_index, &pSBit, NULL))
    {
        return r;
    }

    // GUI_SetDrawMode(0);
    //pTrans=LCD_GetpPalConvTable(&Pal33gray);  // mcl add  for gray 256
    if (pSBit->buffer)
    {
        if (DoRender)
        {
            switch (GUI_LayerInfo.colorMode[GUI_Context.SelLayer])
            {
                case PIXEL_COLOR_ARGB8888:
                    if (GUI_Context.FontMode == LCD_FONTMODE_32BPPSHADOW)
                    {
                        U32 i;    // for shadow
                        U32 bufferSize = (pSBit->width) * (pSBit->height);   //mcl  2008.12.15

                        for (i = 0; i < bufferSize; i++)
                        {
                            U8 pixel = *(pSBit->buffer + i);
                            fontBuffer[i] = LCD_Color2Index((pixel << 24) | (GUI_Context.Color & 0x00ffffff));
                        }

                        shadow(fontBuffer, pSBit->width, pSBit->height);
                        pTrans = fontBuffer;
                        SHADOW_SIZE_I = 2;
                    }
                    else
                    {
                        SHADOW_SIZE_I = 0;
                        _CreateARGBPal(Colors_aRGB);   //  mcl  2008.12.02
                        pTrans = Colors_aRGB;
                    }

                    break;

                case PIXEL_MONO_8BPP:
                {
                    SHADOW_SIZE_I = 0;

                    switch (GUI_Context.FontMode)
                    {
                        case LCD_FONTMODE_8BPP32:
                        {
                            int bufferSize = 0;
                            int i;
                            unsigned char *pPixel = pSBit->buffer;
                            int rows =  pSBit->height;
                            int width = pSBit->width;
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                            {
                                U8 Pixel = *(pPixel + i) ;

                                if (Pixel > 15)
                                {
                                    *(pPixel + i) = Pixel / 16 + GUI_Context.Color;    // last 32 palette reserved  for fonts
                                }
                                else
                                {
                                    *(pPixel + i) = 0;
                                }
                            }
                        }
                        break;

                        case LCD_FONTMODE_8BPP32_FRAME:
                        {
                            int bufferSize = 0;
                            int i;
                            unsigned char *pPixel =  pSBit->buffer;
                            int rows = pSBit->height;
                            int width = pSBit->width;
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                            {
                                U8 Pixel = *(pPixel + i) ;

                                if (Pixel > 15)
                                {
                                    *(pPixel + i) = Pixel / 16 + GUI_Context.Color;    // last 32 palette reserved  for fonts
                                }
                                else
                                {
                                    *(pPixel + i) = 0;
                                }
                            }

                            shadow_Dilation(pPixel,  GUI_Context.pAFont->Font_EXT.pFrame8bpp32,  width, rows);
                        }
                        break;

                        case LCD_FONTMODE_8BPP256:
                            pTrans = NULL;
                            break;

                        case  LCD_FONTMODE_8BPP128_1:
                        {
                            int bufferSize = 0;
                            int i;
                            unsigned char *pPixel = pSBit->buffer;
                            int rows = pSBit->height;
                            int width = pSBit->width;
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                            {
                                U8 Pixel = *(pPixel + i) ;
                                *(pPixel + i) = Pixel / 2;
                            }
                        }
                        break;

                        case LCD_FONTMODE_8BPP128_2:
                        {
                            int bufferSize = 0;
                            int i;
                            unsigned char *pPixel =  pSBit->buffer;
                            int rows = pSBit->height;
                            int width = pSBit->width;
                            pTrans = NULL;  // set  pTrans NULL to use user's palette
                            bufferSize =  rows * width;

                            for (i = 0; i < bufferSize; i++)      // mcl add for  8 bpp  fonts
                            {
                                U8 Pixel = *(pPixel + i) ;

                                if (Pixel)
                                {
                                    *(pPixel + i) = Pixel / 2 + 128;    // last 32 palette reserved  for fonts
                                }
                            }
                        }
                        break;
                    }
                }
                break;

                case PIXEL_MONO_2BPP:
                    SHADOW_SIZE_I = 0;
                    pTrans = NULL;
                    break;

                case PIXEL_COLOR_RGB565:
                    SHADOW_SIZE_I = 0;
                    pTrans = LCD_GetpPalConvTable(&Pal33gray);
                    break;

                default:
                    __msg(" wrong color mode\n");
            }

            /* draw font bitmap  */

            if (GUI_LayerInfo.colorMode[GUI_Context.SelLayer] == PIXEL_MONO_8BPP  && GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME)
                /* Rendering cache data using the bitmap routine */
                LCD_DrawBitmap((int)(GUI_Context.DispPosX[GUI_Context.SelLayer] + pSBit->left),
                               (int)(GUI_Context.DispPosY[GUI_Context.SelLayer] - pSBit->top + GUI_Context.pAFont->Baseline),
                               pSBit->width + 4,
                               pSBit->height + 4,
                               1, 1, 8,     // mcl change 1 to 8
                               pSBit->pitch + 4,
                               GUI_Context.pAFont->Font_EXT.pFrame8bpp32,
                               // pSBit->buffer,
                               pTrans
                              );
            else
            {
                /* Rendering cache data using the bitmap routine */
                LCD_DrawBitmap((int)(GUI_Context.DispPosX[GUI_Context.SelLayer] + pSBit->left),
                               (int)(GUI_Context.DispPosY[GUI_Context.SelLayer] - pSBit->top + GUI_Context.pAFont->Baseline),
                               pSBit->width,
                               pSBit->height,
                               1, 1, 8,     // mcl change 1 to 8
                               pSBit->pitch,
                               pSBit->buffer,
                               pTrans
                              );
            }

            /* else if(((SFTE_Face)(GUI_Context.pAFont->Font_EXT.faceHandle))->glyph->bitmap.pixel_mode == SFTE_PIXEL_MODE_MONO)
              {
                     // Rendering cache data using the bitmap routine
                LCD_DrawBitmap(GUI_Context.DispPosX + pSBit->left,
                        GUI_Context.DispPosY - pSBit->top + GUI_Context.pAFont->Baseline,
                        pSBit->width,
                        pSBit->height,
                        1, 1, 1,
                        pSBit->pitch,
                        pSBit->buffer,
                   &LCD_BKCOLORINDEX
                   );
              }
            */
        }

        r = pSBit->xadvance;

        if (GUI_Context.FontMode == LCD_FONTMODE_32BPPMAG2 || GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME_MAG)
        {
            r = r * 2;
        }
    }
    else
    {
        pImageType->flags = FT_LOAD_DEFAULT;

        if (FTC_SBitCache_Lookup(_FTContext.sbits_cache, pImageType, glyph_index, &pSBit, NULL))
        {
            return r;
        }

        // pImageType->flags = FT_LOAD_MONOCHROME;
        pImageType->flags = FT_LOAD_DEFAULT;
        r = face->glyph->metrics.horiAdvance >> 6;

        if (GUI_Context.FontMode == LCD_FONTMODE_32BPPMAG2 || GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME_MAG)
        {
            r = r * 2;
        }
    }

    return r;
}





/*********************************************************************
*
*       _DispChar
*/
static void _DispChar(U16P c)
{
    int xDist;
    U16P unicode;
#if 0

    /*for chinese*/
    if (c < 128)
    {
        unicode = (U16P)c;
        __inf("c 0x%x\n", c);
    }
    else
    {
        unicode = GB2312_to_Unicode(c);
        __inf("charcode  0x%x\n", c);
        __inf("unicode 0x%x\n", unicode);
    }

    xDist = _RequestGlyph(unicode, 1);

    if (xDist >= 0)
    {
        GUI_Context.DispPosX += xDist;
#endif

        if (c == 0xffff)
        {
            xDist = _RequestGlyph(0x20, 1) + SHADOW_SIZE_I;  //need edit

            if (xDist >= 0)
            {
                GUI_Context.DispPosX[GUI_Context.SelLayer] += xDist;
            }

            return ;
        }
        else
        {
            xDist = _RequestGlyph(c, 1);  //need edit

            if (xDist >= 0)
            {
                GUI_Context.DispPosX[GUI_Context.SelLayer] += xDist;
            }
        }
    }

    /*********************************************************************
    *
    *       _ClearLine
    *
    * Purpose:
    *   If text should be rendered not in transparent mode first the whole line
    *   needs to be cleared, because internally the characters always are drawn in
    *   transparent mode to be sure, that also compound characters are drawn well.
    */
    static void _ClearLine(const char GUI_UNI_PTR * s, int Len)
    {
        /*  U16 c;
          int xDist, yDist, xSize, x0, y0;
          LCD_COLOR OldColor;
          OldColor = GUI_GetColor();
          GUI_SetColor((GUI_Context.TextMode & GUI_TM_REV) ? GUI_GetColor() : GUI_GetBkColor());
          xDist = 0;
          yDist = GUI_Context.pAFont->YDist * GUI_Context.pAFont->YMag;
          x0    = GUI_Context.DispPosX;
          y0    = GUI_Context.DispPosY;
          while (--Len >= 0) {
            c     = GUI_UC__GetCharCodeInc(&s);
            xSize = _RequestGlyph(c, 0);
            if (xSize >= 0) {
              xDist += xSize;
            }
          }
          LCD_FillRect(x0, y0, x0 + xDist - 1, y0 + yDist - 1);
          GUI_SetColor(OldColor);
          */
    }
    /*********************************************************************
    *
    *       _DispLine
    */
    static void _DispLine(const char GUI_UNI_PTR * s, int Len)
    {
        U16 Char;
        int OldMode;

        /* Clear if not transparency mode has been selected */
        if (!(GUI_Context.TextMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)))
        {
            _ClearLine(s, Len);
        }

        /* Draw characters always transparent */
        OldMode = GUI_Context.TextMode;
        GUI_Context.DrawMode |= GUI_DM_TRANS;

        while (--Len >= 0)
        {
            Char = GUI_UC__GetCharCodeInc(&s);
            GUI_Context.pAFont->pfDispChar(Char);
        }

        GUI_Context.DrawMode = OldMode;
    }
    /*********************************************************************
    *
    *       _GetCharDistX
    */
    static int _GetCharDistX(U16P c)
    {
        int xDist;
        // xDist = _RequestGlyph(c, 0);
        xDist = _RequestGlyph(c, 0);
        return (xDist >= 0) ? xDist : 0;
    }
    /*********************************************************************
    *
    *       _GetFontInfo
    */
    static void _GetFontInfo(const GUI_FONT GUI_UNI_PTR * pFont, GUI_FONTINFO * pfi)
    {
        pfi->Baseline = pFont->Baseline;
        pfi->LHeight  = pFont->LHeight;
        pfi->CHeight  = pFont->CHeight;
        pfi->Flags    = GUI_FONTINFO_FLAG_PROP;
    }
    /*********************************************************************
    *
    *       _IsInFont
    */
    static char _IsInFont(const GUI_FONT GUI_UNI_PTR * pFont, U16 c)
    {
        FTC_FaceID face_id;
        FT_Face    face;
        FT_UInt    glyph_index;
        /* Get object pointer */
        face_id    = (FTC_FaceID)pFont->p.pFontData;

        /* Request face object from cache */
        if (FTC_Manager_LookupFace(_FTContext.cache_manager, face_id, &face))
        {
            return 1;
        }

        glyph_index = FTC_CMapCache_Lookup(_FTContext.cmap_cache, face_id, 0, c);
        return glyph_index ? 1 : 0;
    }
    /*********************************************************************
    *
    *       _GetName
    */
    static int _GetName(GUI_FONT * pFont, char *pBuffer, int NumBytes, int Index)
    {
        FTC_FaceID   face_id;
        FT_Face      face;
        int          Len;
        const char *pName = 0;
        /* Get object pointer */
        face_id = (FTC_FaceID)((GUI_TTF_CS *)pFont->p.pFontData)->pTTF;

        /* Request face object from cache */
        if (FTC_Manager_LookupFace(_FTContext.cache_manager, face_id, &face))
        {
            return 1;
        }

        switch (Index)
        {
            case FAMILY_NAME:
                pName = face->family_name;
                break;

            case STYLE_NAME:
                pName = face->style_name;
                break;
        }

        Len = strlen(pName);

        if (Len >= NumBytes)
        {
            Len = NumBytes - 1;
        }

        strncpy(pBuffer, pName, Len);
        *(pBuffer + Len) = 0;
        return 0;
    }
    /*********************************************************************
    *
    *       _APIList
    */
    static const tGUI_ENC_APIList _APIList =
    {
        NULL,
        NULL,
        _DispLine
    };
    /*********************************************************************
    *
    *       Public code
    *
    **********************************************************************
    */
    /*********************************************************************
    *
    *       GUI_TTF_CreateFont
    *
    * Purpose:
    *   Creates a new GUI_FONT object by a given TTF font file available in memory.
    *
    * Parameters:
    *   pFont      - Pointer to a GUI_FONT structure to be initialized by this function.
    *   pCS        - Pointer to a GUI_TTF_CS structure which contains information about
    *                file location, file size, pixel size and face index.
    */
    int GUI_TTF_CreateFont_cache(GUI_FONT * pFont, GUI_TTF_CS * pCS)
    {
        FTC_ImageTypeRec *pImageType;
        FTC_FaceID         face_id;
        FTC_ScalerRec      scaler;
        FT_Face            face;
        FT_Size            size;
        FT_GlyphSlot       slot;
        FT_Glyph           glyph;
        FT_UInt            glyph_index;
        /* Check initialization */
        _CheckInit();     // mcl  Initializes TrueType-engine and caches.
        /* face_id is nothing but the address of the font file */
        face_id = (FTC_FaceID)pCS->pTTF;

        /* Request face object from cache */
        if (FTC_Manager_LookupFace(_FTContext.cache_manager, face_id, &face))
        {
            return 1;
        }

        /* Set image type attributes */
        pImageType          = (FTC_ImageTypeRec *)pCS->aImageTypeBuffer;
        pImageType->face_id = face_id;
        pImageType->width   = 0;
        pImageType->height  = pCS->PixelHeight;
        //pImageType->flags   = FT_LOAD_MONOCHROME;  //   mono  mcl
        // pImageType->flags   = FT_LOAD_NO_BITMAP;
        pImageType->flags   = FT_LOAD_DEFAULT;  //   mono   mcl
        /* Request size object from cache */
        scaler.face_id = face_id;//
        scaler.width   = pImageType->width;
        scaler.height  = pImageType->height;
        scaler.pixel   = 1;

        if (FTC_Manager_LookupSize(_FTContext.cache_manager, &scaler, &size))
        {
            return 1;
        }

        /* Magnification is always 1 */
        pFont->XMag = 1;
        pFont->YMag = 1;
        /* Set function pointers */
        pFont->p.pFontData    = pCS;
        pFont->pfDispChar     = _DispChar;
        pFont->pfGetCharDistX = _GetCharDistX;
        pFont->pfGetFontInfo  = _GetFontInfo;
        pFont->pfIsInFont     = _IsInFont;
        pFont->pafEncode      = &_APIList;
        /* Calculate baseline and vertical size */
        pFont->Baseline = face->size->metrics.ascender >> 6;
        pFont->YSize    = pFont->Baseline - (face->size->metrics.descender >> 6);
        pFont->YDist    = pFont->YSize;
        slot            = face->glyph;
        /* Calculate lowercase height */
        glyph_index = FTC_CMapCache_Lookup(_FTContext.cmap_cache, face_id, 0, 'g');

        if (FTC_ImageCache_Lookup(_FTContext.image_cache, pImageType, glyph_index, &glyph, NULL))
        {
            return 1;
        }

        pFont->LHeight = slot->metrics.horiBearingY >> 6;
        /* Calculate capital height */
        glyph_index = FTC_CMapCache_Lookup(_FTContext.cmap_cache, face_id, 0, 'M');

        if (FTC_ImageCache_Lookup(_FTContext.image_cache, pImageType, glyph_index, &glyph, NULL))
        {
            return 1;
        }

        pFont->CHeight = slot->metrics.height >> 6;
        pFont->Font_EXT.pFrame8bpp32 = (U8 *)esMEMS_Malloc(0, (pCS->PixelHeight * 2 + 8) * (pCS->PixelHeight * 2 + 8));
        /* Select the currently created font */
        return 0;
    }
    /*********************************************************************
    *
    *       GUI_TTF_GetFamilyName
    */
    int GUI_TTF_GetFamilyName_cache(GUI_FONT * pFont, char *pBuffer, int NumBytes)
    {
        return _GetName(pFont, pBuffer, NumBytes, FAMILY_NAME);
    }
    /*********************************************************************
    *
    *       GUI_TTF_GetStyleName
    */
    int GUI_TTF_GetStyleName_cache(GUI_FONT * pFont, char *pBuffer, int NumBytes)
    {
        return _GetName(pFont, pBuffer, NumBytes, STYLE_NAME);
    }
    /*********************************************************************
    *
    *       GUI_TTF_SetCacheSize
    *
    * Purpose:
    *   Sets the maximum number of font faces, size objects and bytes used by the cache
    */
    void GUI_TTF_SetCacheSize_cache(unsigned MaxFaces, unsigned MaxSizes, U32 MaxBytes)
    {
        _FTCacheSize.MaxFaces = MaxFaces;
        _FTCacheSize.MaxSizes = MaxSizes;
        _FTCacheSize.MaxBytes = MaxBytes;
    }
    /*********************************************************************
    *
    *       GUI_TTF_DestroyCache
    *
    * Purpose:
    *   Destroys the FreeType font cache after emptying it.
    */
    void GUI_TTF_DestroyCache_cache(void)
    {
        if (_FTContext.cache_manager)
        {
            FTC_Manager_Done(_FTContext.cache_manager);
            _FTContext.cache_manager = 0;
        }
    }
    /*********************************************************************
    *
    *       GUI_TTF_Done
    */
    void GUI_TTF_Done_cache(GUI_FONT * pFont)
    {
        if (GUI_Context.pAFont == pFont)
        {
            __inf("Error: the font is inuse, cann't be deleted.(GUI_TTF.c)\n");
            return;
        }

        GUI_TTF_DestroyCache_cache();

        if (pFont->Font_EXT.pFrame8bpp32)
        {
            esMEMS_Mfree(0, pFont->Font_EXT.pFrame8bpp32);
            pFont->Font_EXT.pFrame8bpp32 = NULL;
        }

        if (_FTContext.library)
        {
            FT_Done_Library(_FTContext.library);
            _FTContext.library = 0;
        }
    }
#else
void GUI_TTF_C(void);
void GUI_TTF_C(void) {} /* Avoid empty object files */
#endif
    /*************************** End of file ****************************/