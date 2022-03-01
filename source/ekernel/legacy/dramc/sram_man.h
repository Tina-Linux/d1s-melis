/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                             SRAM Manager Module
*
*                                    (c) Copyright 2006-2011, kevin.z China
*                                             All Rights Reserved
*
* File    : sram_man.c
* By      : kevin.z
* Version : v1.0
* Date    : 2010-12-23 13:15
* Descript:
* Update  : date                auther      ver     notes
*           2010-12-23 13:15    kevin.z     1.0     build the file;
*********************************************************************************************************
*/

#ifndef __SRAM_MAN_H__
#define __SRAM_MAN_H__
#include <typedef.h>

#define SRAM_REG_CFG_0              (g_sram_virt_baseaddr + 0x00ul)
#define SRAM_REG_CFG_1              (g_sram_virt_baseaddr + 0x04ul)
#define SRAM_REG_VERSION            (g_sram_virt_baseaddr + 0x24ul)

#define SRAM_C_SWITCH_CPU_DMA       0
#define SRAM_C_SWITCH_VE            1

/* ------c for normal & boot mode------ */
//--bits width--
#define SRAM_C_BOOT_BITS_WIDTH      1
//--offset--
#define SRAM_C_BOOT_OFFSET          24
//--value--
#define SRAM_C_SWITCH_NORMAL        0
#define SRAM_C_SWITCH_BOOT          1

int32_t mem_sram_init(void);
int32_t mem_sram_exit(void);

#endif  //__SRAM_MAN_H__


