/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : icnm_scaler.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-12-1 9:05:40
**********************************************************************************************************************
*/
#ifndef _LION_ICNM_SCALER_H_
#define _LION_ICNM_SCALER_H_

extern __s32 ICNM_InitScaler(void);
extern __s32 ICNM_ExitScaler(void);
extern __s32 ICNM_Scaler(__lion_icnm_icon_t *desicon, __lion_icnm_icon_t *srcicon);

#endif /* #ifndef _LION_ICNM_SCALER_H_ */
