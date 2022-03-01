/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Miracle.MCL.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUIConf.h
* Purpose : Configures abilities, fonts etc.
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/
#ifndef GUICONF_H
#define GUICONF_H
#include <kconfig.h>

#define GUI_EPOS_CB                 (1)             /* 回调函数支持epos的虚拟内存机制                               */

#define GUI_OS                      (1)             /* Compile with multitasking support                            */
#define GUI_SUPPORT_TOUCH           (0)             /* Support a touch screen (req. win-manager)                    */
#define GUI_SUPPORT_UNICODE         (1)             /* Support mixed ASCII/UNICODE strings                          */
#define GUI_SUPPORT_CURSOR          (0)


#define GUI_NUM_LAYERS              (32)  //32

#define GUI_DEFAULT_FONT            &GUI_Font6x8
//#define GUI_ALLOC_SIZE            12500           /* Size of dynamic memory ... For WM and memory devices         */
//#define GUI_ALLOC_SIZE            GUI_MEMDEV_WIDTH * GUI_MEMDEV_HEIGHT * GUI_MEMDEV_PIXELFORMAT * 2
#define GUI_ALLOC_SIZE              CONFIG_GUI_MEMDEV_WIDTH * CONFIG_GUI_MEMDEV_HEIGHT * CONFIG_GUI_MEMDEV_PIXELFORMAT * 2
//800*480*4*2     /* Size of dynamic memory ... For WM and memory devices         */

/*********************************************************************
*
*         Configuration of available packages
*/

#define GUI_SW_LAYER 1

#define GUI_SUPPORT_CHINESE         0   /*added by mcl*/
#define GUI_WINSUPPORT              0   //MCL  /* Window manager package available */
#define GUI_SUPPORT_MEMDEV          1   /* Memory devices available */
#define GUI_SUPPORT_AA              0   /* Anti aliasing available */

#define GUI_SUPPORT_SKIN            0   /* skin switch */

#endif  /* Avoid multiple inclusion */



