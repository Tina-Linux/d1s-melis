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
* File    : GUI_FontIntern.h
* Purpose : Internal decalrations used in font files
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/

#ifndef  GUI_FONTINTERN_H    /* Guard against multiple inclusion */
#define  GUI_FONTINTERN_H

#include "mod_orange_i.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif


extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font8ASCII_Prop;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font10S_ASCII_FontProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font10ASCIIProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font16ASCIIProp1;

extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font16_1_FontProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP FontComic24BProp_ASCII;

extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font24B_ASCII_CharInfo[95];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font24_ASCII_CharInfo[95];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font32B_ASCII_CharInfo[95];

extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font32_ASCII_CharInfo[95];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_CharInfoF8x13_ASCII[96];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_CharInfoF8x15B_ASCII[96];
extern GUI_CONST_STORAGE GUI_CHARINFO CharInfo18B_ASCII[97];
extern GUI_CONST_STORAGE GUI_CHARINFO CharInfo24B_ASCII[96];
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font13ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font13B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font13H_ASCII_CharInfo[95];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font13HB_ASCII_CharInfo[95];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font16_HK_CharInfo[169];
extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font16B_ASCII_CharInfo[95];


#if defined(__cplusplus)
}
#endif


#endif   /* Guard against multiple inclusion */

/*************************** End of file ****************************/
