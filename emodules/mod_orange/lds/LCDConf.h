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
#ifndef LCDCONF_H
#define LCDCONF_H



/*********************************************************************
*
*                   General configuration of LCD
*
**********************************************************************
*/
/*you can  only select one of the #define */
//#define LCD_SHARP_640_480
//#define LCD_SHARP_480_640
//#define  LCD_NEC_240_320
//#define  LCD_NEC_320_240
#define  LCD_SUMSUNG_320_240
//#define  LCD_SUMSUNG_240_320

#define LCD_NUM_LAYERS     4
//#define GUI_NUM_LAYERS     2
/*********************************************************************
*
*                     configuration layer 0
*
**********************************************************************
*/
/*layer 0*/
#if defined( LCD_NEC_240_320 )  //origin

#define LCD_XSIZE_0       (240) /*modified by mcl*/ /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE_0       (320)/*modified by mcl*/  /* Y-resolution of LCD, Logical coor. */

#elif defined (LCD_NEC_320_240) // change to HOZ screen

#define LCD_XSIZE_0      (320)
#define LCD_YSIZE_0       (240)

#elif defined (LCD_SUMSUNG_320_240)  /*origin*/

#define LCD_XSIZE_0       (800)
#define LCD_YSIZE_0       (480)

#elif defined (LCD_SUMSUNG_240_320) /*change to vertical screen*/

#define LCD_XSIZE_0       (240)
#define LCD_YSIZE_0      (320)

#elif defined (LCD_SHARP_640_480) /*origin*/

#define LCD_XSIZE_0      (640)
#define LCD_YSIZE_0       (480)

#elif defined (LCD_SHARP_480_640)  /*chang to vertical screen*/

#define LCD_XSIZE_0      (480)
#define LCD_YSIZE_0      (640)

#else                               /*default*/

#define LCD_XSIZE_0      (320)
#define LCD_YSIZE_0      (240)

#endif

//#define LCD_BITSPERPIXEL_0 (16)  /*modified by mcl*/
#define LCD_FIXEDPALETTE_0  8888
#define LCD_BITSPERPIXEL_0 (32)

#define LCD_CONTROLLER_0 3375
#define LCD_SWAP_RB_0   1
#define LCD_SWAP_XY_0   0
//#define LCD_FIXEDPALETTE_0 8888


/*layer 1*/

#if defined( LCD_NEC_240_320 )  //origin

#define LCD_XSIZE_1       (240) /*modified by mcl*/ /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE_1       (320)/*modified by mcl*/  /* Y-resolution of LCD, Logical coor. */

#elif defined (LCD_NEC_320_240) // change to HOZ screen

#define LCD_XSIZE_1       (320)
#define LCD_YSIZE_1       (240)

#elif defined (LCD_SUMSUNG_320_240)  /*origin*/

#define LCD_XSIZE_1       (480)
#define LCD_YSIZE_1       (272)

#elif defined (LCD_SUMSUNG_240_320) /*change to vertical screen*/

#define LCD_XSIZE_1       (240)
#define LCD_YSIZE_1       (320)

#elif defined (LCD_SHARP_640_480) /*origin*/

#define LCD_XSIZE_1       (640)
#define LCD_YSIZE_1       (480)

#elif defined (LCD_SHARP_480_640)  /*chang to vertical screen*/

#define LCD_XSIZE_1       (480)
#define LCD_YSIZE_1       (640)

#else                               /*default*/

#define LCD_XSIZE_1       (320)
#define LCD_YSIZE_1       (240)

#endif

//#define LCD_BITSPERPIXEL_1 (32)  /*modified by mcl*/
#define LCD_BITSPERPIXEL_1 (16)

#define LCD_CONTROLLER_1 3475
#define LCD_SWAP_RB_1   1
#define LCD_SWAP_XY_1   0
//#define LCD_FIXEDPALETTE_1 8888



/*layer 2*/

#if defined( LCD_NEC_240_320 )  //origin

#define LCD_XSIZE_2       (240) /*modified by mcl*/ /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE_2       (320)/*modified by mcl*/  /* Y-resolution of LCD, Logical coor. */

#elif defined (LCD_NEC_320_240) // change to HOZ screen

#define LCD_XSIZE_2       (320)
#define LCD_YSIZE_2       (240)

#elif defined (LCD_SUMSUNG_320_240)  /*origin*/

#define LCD_XSIZE_2       (480)
#define LCD_YSIZE_2       (272)

#elif defined (LCD_SUMSUNG_240_320) /*change to vertical screen*/

#define LCD_XSIZE_2       (240)
#define LCD_YSIZE_2       (320)

#elif defined (LCD_SHARP_640_480) /*origin*/

#define LCD_XSIZE_2       (640)
#define LCD_YSIZE_2       (480)

#elif defined (LCD_SHARP_480_640)  /*chang to vertical screen*/

#define LCD_XSIZE_2       (480)
#define LCD_YSIZE_2       (640)

#else                               /*default*/

#define LCD_XSIZE_2       (320)
#define LCD_YSIZE_2       (240)

#endif

#define LCD_BITSPERPIXEL_2  (16)  /*modified by mcl*/

#define LCD_CONTROLLER_2 3575
#define LCD_SWAP_RB_2   1
#define LCD_SWAP_XY_2   0

/*layer  3*/
#if defined( LCD_NEC_240_320 )  //origin

#define LCD_XSIZE_3       (240) /*modified by mcl*/ /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE_3       (320)/*modified by mcl*/  /* Y-resolution of LCD, Logical coor. */

#elif defined (LCD_NEC_320_240) // change to HOZ screen

#define LCD_XSIZE_3       (320)
#define LCD_YSIZE_3       (240)

#elif defined (LCD_SUMSUNG_320_240)  /*origin*/

#define LCD_XSIZE_3       (480)
#define LCD_YSIZE_3       (272)

#elif defined (LCD_SUMSUNG_240_320) /*change to vertical screen*/

#define LCD_XSIZE_3       (240)
#define LCD_YSIZE_3       (320)

#elif defined (LCD_SHARP_640_480) /*origin*/

#define LCD_XSIZE_3       (640)
#define LCD_YSIZE_3       (480)

#elif defined (LCD_SHARP_480_640)  /*chang to vertical screen*/

#define LCD_XSIZE_3       (480)
#define LCD_YSIZE_3       (640)

#else                               /*default*/

#define LCD_XSIZE_3       (320)
#define LCD_YSIZE_3       (240)

#endif

#define LCD_BITSPERPIXEL_3 (16)  /*modified by mcl*/

#define LCD_CONTROLLER_3 3675
#define LCD_SWAP_RB_3   1
#define LCD_SWAP_XY_3   0

/*********************************************************************
*
*                   Full bus configuration
*
**********************************************************************
*/

/*layer  4   mcl add  08.11.19*/
#if defined( LCD_NEC_240_320 )  //origin

#define LCD_XSIZE_4       (240) /*modified by mcl*/ /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE_4       (320)/*modified by mcl*/  /* Y-resolution of LCD, Logical coor. */

#elif defined (LCD_NEC_320_240) // change to HOZ screen

#define LCD_XSIZE_4       (320)
#define LCD_YSIZE_4       (240)

#elif defined (LCD_SUMSUNG_320_240)  /*origin*/

#define LCD_XSIZE_4       (480)
#define LCD_YSIZE_4       (272)

#elif defined (LCD_SUMSUNG_240_320) /*change to vertical screen*/

#define LCD_XSIZE_4       (240)
#define LCD_YSIZE_4       (320)

#elif defined (LCD_SHARP_640_480) /*origin*/

#define LCD_XSIZE_4       (640)
#define LCD_YSIZE_4       (480)

#elif defined (LCD_SHARP_480_640)  /*chang to vertical screen*/

#define LCD_XSIZE_4       (480)
#define LCD_YSIZE_4       (640)

#else                               /*default*/

#define LCD_XSIZE_4       (320)
#define LCD_YSIZE_4       (240)

#endif

#define LCD_BITSPERPIXEL_4 (16)  /*modified by mcl*/

#define LCD_CONTROLLER_4 3675
#define LCD_SWAP_RB_4   1
#define LCD_SWAP_XY_4   0

#endif
