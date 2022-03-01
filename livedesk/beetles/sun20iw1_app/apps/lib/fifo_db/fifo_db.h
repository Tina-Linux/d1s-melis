/*
************************************************************************************************************************
*                                                        SWAN
*                                                   the Video Player
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : fifo_db.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.06.05
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.06.05       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __fifo_db_h
#define  __fifo_db_h
#include "gp_def.h"

#define FIFO_DB_TITLE_MAX_LEN           63
#define FIFO_DB_KEY_MAX_LEN             768//511
#define FIFO_DB_ITEM_MAX_NR             4//1024
#define FIFO_DB_ITEM_CONTENT_MAX_SIZE   1024//(32*1024)


extern __bool fifo_db_has_exist(const char *title);
extern __s32  create_fifo_db(const char *title, __u32 item_max_nr, __u32 item_size);
extern __s32  delete_fifo_db(const char *title);
extern __s32  get_item_content_from_fifo_db(const char *title, const char *key, void *buf);
extern __s32  add_item_to_fifo_db(const char *title, const char *key, const void *buf);
extern __s32  delete_item_from_fifo_db(const char *title, const char *key);
extern __s32 fifo_init(void);
extern __s32 fifo_deinit(void);

#endif     //  ifndef __fifo_db_h

/* end of fifo_db.h */


