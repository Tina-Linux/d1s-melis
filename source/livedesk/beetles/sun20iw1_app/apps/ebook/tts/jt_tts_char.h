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

#ifndef __JT_TTS_CHAR_H__
#define __JT_TTS_CHAR_H__

//#include "epdk.h"
//#include "ejTTS.h"
#include <emodules/mod_defs.h>
#include "jt_tts.h"

extern tts_char_parser_play  *_tts_play_struct;


extern void tts_char_parser_init(void);
extern void tts_char_parser_exit(void);
extern int  tts_get_char_encode_type(int encode_type);

extern  int tts_get_txtCharEncodeType(ES_FILE *fd);




#endif /* jt_tts_reg.h */

