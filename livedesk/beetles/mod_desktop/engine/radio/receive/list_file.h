/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                                     applet
*
*                                    (c) Copyright 2007-2010, Jackie, China
*                                            All Rights Reserved
*
* File          : list_file.h
* By            : jackie.chen
* Func      : list file
* Version   : v1.0
* ============================================================================================================
* 2010-12-29   jackie.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __FORMAT_OUTPUT_H__
#define __FORMAT_OUTPUT_H__

__s32 get_channel_list_from_file(char *file_name, dsk_radio_ch_list_t *channel_list);
__s32 write_channel_list_to_file(char *file_name, dsk_radio_ch_list_t *channel_list);

#endif

