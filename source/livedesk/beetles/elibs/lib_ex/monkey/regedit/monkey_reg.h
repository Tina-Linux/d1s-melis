/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : monkey_reg.h
* By        : Andy.zhang
* Func      : monkey core interface
* Version   : v1.0
* ============================================================================================================
* 2009-11-3 9:39:42  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MONKEY_REG_H__
#define __MONKEY_REG_H__

__s32   mkr_setColor(MkColor mColor);
MkColor mkr_getColor(void);
__s32   mkr_setFont(MkFont mFont);
MkFont  mkr_getFont(void);
__s32   mkr_setRotate(MkRotate mRotate);
MkRotate mkr_getRotate(void);
__s32   mkr_setSwitch(MkSwitch mMode);
MkSwitch mkr_getSwitch(void);
__s32   mkr_setBackground(MkBack mBack);
MkBack  mkr_getBackground(void);
__s32   mkr_setBright(MkBright mBright);
MkBright mkr_getBright(void);
__s32   mkr_setAutoPage(MkAutoP  mAutoP);
MkAutoP mkr_getAutoPage(void);

#endif /* monkey_reg.h */

