/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Hid System Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_hid.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-27 12:29
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-27 12:29    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#include <kapi.h>


/*
*********************************************************************************************************
*                                   HID SEND MESSAGE
*
* Description: hid send message to system message pool;
*
* Arguments  : msgid    message id;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esHID_SendMsg(uint32_t msgid)
{
    esKSRV_SendMsg(msgid, KMSG_PRIO_LOW);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   HID DEVICE REGISTER
*
* Description: hid device register;
*
* Arguments  : hNode    device node;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       : hid not support device register now;
*********************************************************************************************************
*/
int32_t esHID_hiddevreg(__hdle hNode)
{
    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                   HID DEVICE UNREGISTER
*
* Description: hid device unregister;
*
* Arguments  : hNode    device node;
*              mode     unregister mode;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       : hid not support device register now;
*********************************************************************************************************
*/
int32_t  esHID_hiddevunreg(__hdle hNode, uint32_t mode)
{
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   HID SYSTEM INITIALISE
*
* Description: hid system initialise;
*
* Arguments  : none;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t HID_Init(void)
{
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   HID SYSTEM EXIT
*
* Description: hid system exit;
*
* Arguments  : none;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t HID_Exit(void)
{
    return EPDK_OK;
}


