/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : LCD_L0_Generic.h
* Purpose :  Color conversion routines for LCD-drivers
*            This file may not be compiled by itself, but needs
*            to be included in every LCD driver
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/

#ifndef LCD_L0_GENERIC_H
#define LCD_L0_GENERIC_H

#ifndef LCD_INCLUDEDRIVER_H     /* includes only for stand-alone compilation, not as included file */
#include <stddef.h>           /* needed for definition of NULL */
#include "LCD_Private.h"      /* private modul definitions & config */
#include "GUI_Private.h"
#include "GUIDebug.h"
#endif


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*           LCD_L0_Color2Index
*/
unsigned int LCD_L0_Color2Index(LCD_COLOR Color);


/*********************************************************************
*
*           LCD_L0_1_Color2Index    added by mcl
*/

unsigned int LCD_L0_1_Color2Index(LCD_COLOR Color);
unsigned int LCD_L0_2_Color2Index(LCD_COLOR Color);
unsigned int LCD_L0_3_Color2Index(LCD_COLOR Color);




/*********************************************************************
*
*       LCD_L0_Index2Color
*/
LCD_COLOR LCD_L0_Index2Color(int Index);

/*********************************************************************
*
*       LCD_L0_1_Index2Color   added by mcl
*/
LCD_COLOR LCD_L0_1_Index2Color(int Index);
LCD_COLOR LCD_L0_2_Index2Color(int Index);
LCD_COLOR LCD_L0_3_Index2Color(int Index);

/*********************************************************************
*
*       LCD_L0_GetIndexMask
*/
unsigned int LCD_L0_GetIndexMask(void) ;


/*********************************************************************
*
*       LCD_L0_1_GetIndexMask  added by mcl
*/
unsigned int LCD_L0_1_GetIndexMask(void) ;
unsigned int LCD_L0_2_GetIndexMask(void) ;
unsigned int LCD_L0_3_GetIndexMask(void) ;

/*********************************************************************
*
*       LCD_L0_GetRect
*/
void LCD_L0_GetRect(LCD_RECT *pRect);


/*********************************************************************
*
*       LCD_L0_1_GetRect   added by mcl
*/
void LCD_L0_1_GetRect(LCD_RECT *pRect) ;
void LCD_L0_2_GetRect(LCD_RECT *pRect) ;
void LCD_L0_3_GetRect(LCD_RECT *pRect) ;
/*************************** End of file ****************************/

#endif
