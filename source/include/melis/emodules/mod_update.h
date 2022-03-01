/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                 Update Module
*
*                                 (c) Copyright 2006-2010, All winners Co,Ld.
*                                             All Rights Reserved
*
* File    : mod_update.h
* By      : james
* Version : 1.0.0
* Date    : 2011-08-24
* Descript:
* Update  : date              author         version   notes
*           2011-08-24        james deng     1.0.0     build the file.
*********************************************************************************************************
*/
#ifndef _MOD_UPDATE_H_
#define _MOD_UPDATE_H_

// 升级的状态，通过UPDATE_CMD_STATUS命令可以获取当前的状态信息
typedef enum __UPDATE_STATUS
{
    UPDATE_STATUS_ERROR,
    UPDATE_STATUS_PREPARE,
    UPDATE_STATUS_START,
    UPDATE_STATUS_FINISH
} __update_status_t;

typedef enum __UPDATE_CMD_SET           // 升级命令列表(IOCtrol)
{
    UPDATE_CMD_START = 0x01,            // 开始升级, pbuffer = (file name)
    UPDATE_CMD_STATUS,                  // 升级状态
    UPDATE_CMD_RATIO,                   // 升级进度
	UPDATE_CMD_CHECK_PROG  = UPDATE_CMD_RATIO,				// 检查进度, return ratio(0-100), -1 = fail
    UPDATE_CMD_STOP,                    // 终止升级
	UPDATE_CMD_FORCE_USB_UPDATE,				// 检查进度, return ratio(0-100), -1 = fail
    UPDATE_CMD_NULL,                // 空命令
} __update_cmd_set_t;

#endif /* _MOD_UPDATE_H_ */


