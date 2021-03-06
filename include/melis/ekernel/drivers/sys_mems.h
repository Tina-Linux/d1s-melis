/*
*********************************************************************************************************
*                                                    ePOS
*                                   the Easy Portable/Player Operation System
*                                               mems sub-system
*
*                          (c) Copyright 1992-2006, Jean J. Labrosse, Weston, FL
*                                             All Rights Reserved
*
* File    : mems.h
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/
#ifndef _SYS_MEMS_H_
#define _SYS_MEMS_H_
#include <ktype.h>
#include <csp_dram_para.h>

//**********************************************************************************************************
//* define level 0( system level)
//************************************************

#define MEMS_domain_user    0x0f
#define MEMS_domain_system  0x0f
#define MEMS_domain_client  0x0e

typedef enum
{
    CSP_SRAM_ZONE_NULL      =   0x00,

    CSP_SRAM_ZONE_BOOT,                 /* 可使用的模块为: BOOT或NORMAL. 注意:OS务必将这个区域设置为NORMAL  */
    CSP_SRAM_ZONE_C,                   /* 可使用的模块为: CPU_DMA或VE  */
    CSP_SRAM_ZONE_ICACHE,               /* not used */
    CSP_SRAM_ZONE_DCACHE,               /* not used */

    CSP_SRAM_ZONE_MAX_NR
} csp_sram_zone_id_t;

//------Module--------------------------------------
typedef enum
{
    CSP_SRAM_MODULE_NULL    =   0x00,
    CSP_SRAM_MODULE_BOOT,             /* boot模块  : zone c1,c2,c3作启动用   */
    CSP_SRAM_MODULE_NORMAL,           /* normal模块: zone c1(CPU_DMA/VE),zone c2(DE_FE),zone c3(DE_BE) */
    CSP_SRAM_MODULE_CPU_DMA,
    CSP_SRAM_MODULE_VE,

    CSP_SRAM_MODULE_MAX_NR
} csp_sram_module_t;

typedef enum __SRAM_REQ_MODE
{
    SRAM_REQ_MODE_WAIT  = 0,    //request sram block in wait mode
    SRAM_REQ_MODE_NWAIT = 1,    //request sram block in none-wait mode
    SRAM_REQ_MODE_
} csp_sram_req_mode_e;

typedef struct  sram_zone_info
{
    csp_sram_zone_id_t  zone_id;
    uint32_t            reserved;  //uint32_t zone_size;
} csp_sram_zone_info_t;

typedef struct __SRAM_BLK_NODE
{
    intptr_t            *pLock;     //sram block lock
    csp_sram_zone_id_t  zone_id;       //sram block id
    csp_sram_module_t   module;       //sram map
    int32_t             bUse;       //flag to mark if the sram block is free
} csp_sram_blk_node_t;

typedef struct __SRAM_BLK_MANAGER
{
    csp_sram_blk_node_t *pBlkLst;   //sram block node list
    int32_t             nBlkCnt;    //sram block count
} csp_sram_blk_manager;

typedef struct __SYS_MEM_CFG
{
    intptr_t            *task_prio;
    intptr_t            *task_sem;
    uint8_t             task_flag;
    uint32_t            bw_num;
    uint32_t            bw_total;
    uint32_t            bw_max;
    uint32_t            bw_average;

    //define parameter for switch work mode and user mode
    int32_t             vid_flag ;
    int32_t             pic_flag ;
    int32_t             tv_flag ;
} __sys_mem_cfg_t;

typedef enum __DRAM_ACCESS_CMD
{
    DRAM_ACCESS_DISABLE_REQ = 0,    /* dram璇锋眰绂佹璁块棶 */
    DRAM_ACCESS_DISABLE_CANCEL,     /* dram鍙栨秷绂佹璁块棶 */
    DRAM_ACCESS_
} __dram_access_cmd_t;

typedef enum __DRAM_WORK_MODE
{
    DRAM_WORK_MODE_LCD,             /* 褰撳墠涓篖CD宸ヤ綔妯″紡锛?    闇?瑕侀厤缃瓺RAM鍙傛暟婊¤冻绯荤粺骞冲潎甯﹀闇?姹?*/
    DRAM_WORK_MODE_TV,              /* 褰撳墠涓篢V宸ヤ綔妯″紡锛?     闇?瑕侀厤缃瓺RAM鍙傛暟婊¤冻绯荤粺鐬椂甯﹀闇?姹?*/
    DRAM_WORK_MODE_TV_480P,         /* 褰撳墠涓篢V  480P宸ヤ綔妯″紡锛岄渶瑕侀厤缃瓺RAM鍙傛暟婊¤冻绯荤粺鐬椂甯﹀闇?姹?*/
    DRAM_WORK_MODE_TV_576P,         /* 褰撳墠涓篢V  720P宸ヤ綔妯″紡锛岄渶瑕侀厤缃瓺RAM鍙傛暟婊¤冻绯荤粺鐬椂甯﹀闇?姹?*/
    DRAM_WORK_MODE_TV_720P,         /* 褰撳墠涓篢V  720P宸ヤ綔妯″紡锛岄渶瑕侀厤缃瓺RAM鍙傛暟婊¤冻绯荤粺鐬椂甯﹀闇?姹?*/
    DRAM_WORK_MODE_TV_1080P,        /* 褰撳墠涓篢V 1080P宸ヤ綔妯″紡锛岄渶瑕侀厤缃瓺RAM鍙傛暟婊¤冻绯荤粺鐬椂甯﹀闇?姹?*/
    DRAM_WORK_MODE_
} __dram_work_mode_t;

typedef enum __DRAM_USER_MODE
{
    DRAM_USER_MODE_NONE = 0,
    DRAM_USER_MODE_VIDEO,
    DRAM_USER_MODE_MUSIC,
    DRAM_USER_MODE_PICTURE,
    DRAM_USER_MODE_ENCODER,
    DRAM_USER_MODE_
} __dram_user_mode_t;

//************************************************
// 鍑芥暟瀹氫箟
__s32 MEM_EnableDramSelfFresh(void);
__s32 MEM_DisableDramSelfFresh(__u32 nDrmClk);
__s32 MEM_DisableDramAcess(void);
__s32 MEM_EnableDramAcess(void);

//**********************************************************************************************************


#endif  /* _KRNL_H_ */
