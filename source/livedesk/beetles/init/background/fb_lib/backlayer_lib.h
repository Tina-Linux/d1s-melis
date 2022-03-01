/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_willow sub-system
*
*                               (c) Copyright 2006-2009, ALLWINNERME, China
*                                           All Rights Reserved
*
* File   : backlayer_lib.h
* Version: V1.0
* By     : xiechuanrong
*********************************************************************************************************
*/
#ifndef _BACKLAYER_LIB_H_
#define _BACKLAYER_LIB_H_
//#include "epdk.h"
#include <emodules/mod_defs.h>

#ifndef memset
#define memset(buf, val, size) eLIBs_memset(buf, val, size)
#endif
#ifndef palloc
#define palloc(npage, mode) esMEMS_Palloc(npage, mode)
#endif
#ifndef pfree
#define pfree(pblk, npage) esMEMS_Pfree(pblk, npage)
#endif
#define JPG_MODE 0
#define BMP_MODE 1
extern __s32 backlayer_create_layer(__mp *de_hdle);
extern void  backlayer_delete_layer(__mp *de_hdle, unsigned long layer_handle);
extern void  backlayer_set_fb(__mp *de_hdle, unsigned long layer_handle, char *fname, FB *fb, __u8 mod);
extern void  backlayer_set_top(__mp *de_hdle, unsigned long layer_handle);
extern void  backlayer_set_bottom(__mp *de_hdle, unsigned long layer_handle);
__u32 get_logo_mode(void);

#endif
