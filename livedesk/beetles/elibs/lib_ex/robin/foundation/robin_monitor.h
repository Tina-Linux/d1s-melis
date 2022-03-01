/*
************************************************************************************************************************
*                                                        robin
*
*                             Copyright(C), 2009-2010, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : robin_monitor.h
*
* Author      : Gary.Wang
*
* Version     : 1.1.0
*
* Date        : 2009.11.19
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
* Gary.Wang      2009.11.19       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __robin_monitor_h
#define  __robin_monitor_h



typedef enum
{
    ROBIN_CMD_PLAY_FILE = 0,
    ROBIN_CMD_PLAY,
    ROBIN_CMD_PAUSE,
    ROBIN_CMD_STOP,
    ROBIN_CMD_FF,
    ROBIN_CMD_RR,
    ROBIN_CMD_JUMP,

    ROBIN_CMD_
} robin_cmd_e;



extern __s32  robin_monitor_create(__u8 prio);
extern void   robin_monitor_delete(void);
extern __s32  robin_monitor_get_cur_file(char *file, __u32 size);
extern void   robin_wait_no_file(void);



#endif     //  ifndef __robin_monitor_h

/* end of robin_monitor.h */
