/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, JOHN, China
*                                           All Rights Reserved
*
* File    :
* By      :
* Version :
*********************************************************************************************************
*/

#ifndef _DVB_SUBTITLE_UI_H_
#define _DVB_SUBTITLE_UI_H_

#include "apps.h"
#include "../dtv_i.h"

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))


__s32 dtv_subtitle_switch_onoff(void);

__s32 dtv_subtitle_paint(void *arg);
__s32 dtv_subtitle_delete(void);
__bool dtv_has_subtitle(void);



#endif

#endif /* #ifndef _movie_UI_H_ */

