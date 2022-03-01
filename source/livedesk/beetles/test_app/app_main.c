/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                             app_main file
*
*                               (c) Copyright 2019-2021, by allwinner in China
*                                           All Rights Reserved
*
* File    : app_main.c
* By      : huangyixiu
* Version : V1.0                                                           2019.12.11
*********************************************************************************************************
*/
#include <typedef.h>
#include <libc.h>
#include <log.h>
#include <kconfig.h>
#include <kapi.h>
#include <rtthread.h>


void testprintf(void *arg)
{
	while(1)
	{
		rt_thread_delay(100);
		//printk("[%s, %s, %d] hello world!\n", __FILE__, __FUNCTION__, __LINE__);
		//__log("[%s, %s, %d] hello !\n", __FILE__, __FUNCTION__, __LINE__);
	}
}


__s32 test_app(void *p_arg)
{
    esKRNL_TCreate(testprintf, NULL, 0x1000, KRNL_priolevel6);
}

