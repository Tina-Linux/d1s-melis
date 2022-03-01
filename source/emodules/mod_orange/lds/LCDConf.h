/*
*********************************************************************************************************
*                                                uC/GUI
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
File        : LCDConf_1375_C8_C320x240.h
Purpose     : Sample configuration file
----------------------------------------------------------------------
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
