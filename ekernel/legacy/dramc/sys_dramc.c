/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                             Memory driver Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : sys_mem.c
* By      : kevin.z
* Version : v2.0
* Date    : 2010-11-27 12:54
* Descript:
* Update  : date                auther      ver     notes
*           2010-11-27 12:54    kevin.z     2.0     build the file;
*********************************************************************************************************
*/
#include <sys_mems.h>
#include "sram_man.h"
#include <kconfig.h>
#include <string.h>
#include <kapi.h>
#include <port.h>
#include <log.h>


static __hdle h_dram_ahb_clk;

extern void udelay(unsigned int us);/*the function defined in cortex-a7/delay.c file*/

/*
**********************************************************************************************************************
*                                               MEM_EnableDramAcess
*
* Description: eanble dram access.
*
* Arguments  : none
*
* Returns    : result;
*                   EPDK_OK , EPDK_FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t MEM_EnableDramAcess(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               MEM_DisableDramAcess
*
* Description: request all dram user to disable dram access.
*
* Arguments  : none
*
* Returns    : result;
*                   EPDK_OK , EPDK_FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t MEM_DisableDramAcess(void)
{
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               MEM_RegDramAccess
*
* Description: register dram user.
*
* Arguments  : dev_name     name of dram user;
*              dev_type     type of the dram user;
*              dram_access  call function for disable/enable dram access;
*
* Returns    : result       handle of dram user;
*
* Notes      : when adjust dram work frequency, the dram is not accessable, if some
*              device access dram during that period, dram will die. so we need memorize
*              all dram user, when adjust dram work frequency, it will request all user
*              to disable access dram to avoid dram dead.
*
**********************************************************************************************************************
*/
__hdle esMEM_RegDramAccess(uint8_t dev_type, __pCB_ClkCtl_t dram_access)
{
	__err("This function is not implemented!");
    return 0;
}


/*
**********************************************************************************************************************
*                                               MEM_UnRegDramAccess
*
* Description: unregister dram user.
*
* Arguments  : user     the handle of dram user;
*
* Returns    : result;
*                   EPDK_OK/EPDK_FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esMEM_UnRegDramAccess(__hdle user)
{
	__err("This function is not implemented!");
    return EPDK_FAIL;
}

#ifdef CONFIG_SOC_SUN3IW2
/*
*********************************************************************************************************
*                                   ENABLE DRAM
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t MEM_EnableDramSelfFresh(void)
{
    CSP_DRAMC_master_close();
    udelay(0x1000);
    CSP_DRAM_store_register_data();
    CSP_DRAMC_enter_selfrefresh();
    udelay(0x1000);
    //CSP_DRAMC_access_disable();
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   DISABLE DRAM REFRESH
*
* Description: disable dram refresh;
*
* Arguments  : nDrmClk  dram work clock;
*
* Returns    : reuslt, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t MEM_DisableDramSelfFresh(__u32 nDrmClk)
{
    CSP_DRAMC_set_autofresh_cycle(nDrmClk);
    CSP_DRAMC_exit_selfrefresh();
    CSP_DRAMC_set_autofresh_cycle(nDrmClk);
    CSP_DRAMC_scan_readpipe(nDrmClk);
    CSP_DRAM_restore_register_data();
    CSP_DRAMC_master_open();
    return EPDK_OK;
}

int32_t _RequestDramWorkModeAll(__dram_user_mode_t mode)
{
    __dram_master_t master_de20;
    __dram_master_t master_ve;

    rt_memset(&master_de20, 0x00, sizeof(master_de20));
    rt_memset(&master_ve, 0x00, sizeof(master_ve));

    CSP_DRAMC_set_master_default(DRAM_DEVICE_ROT);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DE20);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_TVIN);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DI);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_TS);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DMA);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_VE);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_MAHB);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DTMB);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_CPU);

    if (mode == DRAM_USER_MODE_VIDEO)
    {
        master_de20.bandwidth_limit0  = 0;
        master_de20.bandwidth_limit1  = 0;
        master_de20.bandwidth_limit2  = 0;

        master_ve.bandwidth_limit0  = 0;
        master_ve.bandwidth_limit0  = 0;
        master_ve.bandwidth_limit0  = 0;

        CSP_DRAMC_set_master_config(DRAM_DEVICE_DE20, &master_de20);
        CSP_DRAMC_set_master_config(DRAM_DEVICE_VE, &master_ve);
    }
    else if (mode == DRAM_USER_MODE_MUSIC)
    {
        master_de20.bandwidth_limit0  = 0;
        master_de20.bandwidth_limit1  = 0;
        master_de20.bandwidth_limit2  = 0;

        master_ve.bandwidth_limit0  = 0;
        master_ve.bandwidth_limit0  = 0;
        master_ve.bandwidth_limit0  = 0;

        CSP_DRAMC_set_master_config(DRAM_DEVICE_DE20, &master_de20);
        CSP_DRAMC_set_master_config(DRAM_DEVICE_VE, &master_ve);
    }

    return EPDK_OK;
}

static int32_t _ReleaseDramWorkModeAll(void)
{
    CSP_DRAMC_set_master_default(DRAM_DEVICE_ROT);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DE20);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_TVIN);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DI);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_TS);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DMA);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_VE);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_MAHB);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_DTMB);
    CSP_DRAMC_set_master_default(DRAM_DEVICE_CPU);

    return EPDK_OK;
}

static void dramc_bandw_task(void *arg)
{
    __dram_bandw_t  bandw0, bandw1;
    uint32_t    time0, time1;
    uint32_t    total_bw0, total_bw1, total_bw;
    uint32_t    cpu_bw;
    uint32_t    dtmb_bw;
    uint32_t    ve_bw;
    uint32_t    disp_bw;
    uint32_t    other_bw;

    rt_memset(&bandw0, 0x00, sizeof(__dram_bandw_t));
    rt_memset(&bandw1, 0x00, sizeof(__dram_bandw_t));
    while (1)
    {
        if (sys_mem.task_flag == 0)
        {
            esKRNL_TimeDly(10);
            continue;
        }

        time0 = esKRNL_TimeGet();
        CSP_DRAMC_get_all_bw_counter(&bandw0);

        esKRNL_TimeDly(100);

        time1 = esKRNL_TimeGet();
        CSP_DRAMC_get_all_bw_counter(&bandw1);

        __log("time0=%d,time1=%d,time1-time0=%d", time0, time1, time1 - time0);
        if ((time1 - time0) != 100)
        {
            esKRNL_TimeDly(10);
            continue;
        }

        total_bw0 = bandw0.cpu_bw + bandw0.dtmb_bw + bandw0.ve_bw + bandw0.disp_bw + bandw0.other_bw;
        total_bw1 = bandw1.cpu_bw + bandw1.dtmb_bw + bandw1.ve_bw + bandw1.disp_bw + bandw1.other_bw;
        if (total_bw1 < total_bw0)
        {
            esKRNL_TimeDly(10);
            continue;
        }

        total_bw  = (total_bw1 - total_bw0);
        cpu_bw    = (bandw1.cpu_bw - bandw0.cpu_bw);
        dtmb_bw   = (bandw1.dtmb_bw - bandw0.dtmb_bw);
        ve_bw     = (bandw1.ve_bw - bandw0.ve_bw);
        disp_bw   = (bandw1.disp_bw - bandw0.disp_bw);
        other_bw  = (bandw1.other_bw - bandw0.other_bw);

        sys_mem.bw_num    += 1;
        sys_mem.bw_total  += total_bw;
        sys_mem.bw_max     = (total_bw > sys_mem.bw_max) ? total_bw : sys_mem.bw_max;
        sys_mem.bw_average = sys_mem.bw_total / sys_mem.bw_num;


        printk("total:%.2fMB,num:%d,max:%.2fMB,average:%.2fMB.", \
               (float)sys_mem.bw_total / (float)1024, \
               sys_mem.bw_num, \
               (float)sys_mem.bw_max / (float)1024, \
               (float)sys_mem.bw_average / (float)1024);
        printk("%s%s%s%s%s%s",    \
               "       total", "         cpu", "        dtmb", "          ve", "        disp", "       other");
        printk("%12.2f%12.2f%12.2f%12.2f%12.2f%12.2f  (MB)",  \
               (float)total_bw / (float)1024, \
               (float)cpu_bw / (float)1024, \
               (float)dtmb_bw / (float)1024, \
               (float)ve_bw / (float)1024, \
               (float)disp_bw / (float)1024, \
               (float)other_bw / (float)1024);
        printk("%12.2f%12.2f%12.2f%12.2f%12.2f%12.2f  (Rate)",    \
               (float)total_bw * 100 / (float)total_bw, \
               (float)cpu_bw * 100 / (float)total_bw, \
               (float)dtmb_bw * 100 / (float)total_bw, \
               (float)ve_bw * 100 / (float)total_bw, \
               (float)disp_bw * 100 / (float)total_bw, \
               (float)other_bw * 100 / (float)total_bw);
    }
}

int32_t mem_init(__dram_para_t *para)
{
    __dram_bwconf_t bwconf;

    rt_memset(&bwconf, 0x00, sizeof(bwconf));

    //initialise sram manager
    mem_sram_init();

    if (para)
    {
        CSP_DRAMC_init((__dram_para_t *)para, 0);
    }


    _ReleaseDramWorkModeAll();

    bwconf.bw_enable = 1;
    bwconf.statistic_unit = 1;
    bwconf.max_statistic = 0xffff;

    CSP_DRAMC_master_counter_general_config(&bwconf);

    sys_mem.task_sem   = esKRNL_SemCreate(1);
    if (sys_mem.task_sem == NULL)
    {
        __err("create sem failed!");
        return EPDK_FAIL;
    }
    sys_mem.task_flag  = 0;

    sys_mem.task_prio = awos_task_create("dramc_bw",  dramc_bandw_task, (void *)0, 0x1000, RT_TIMER_THREAD_PRIO - 4, 10);
    if (!sys_mem.task_prio)
    {
        __err("Create mem manager main task failed!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}
/*
*********************************************************************************************************
*                                   MEMORY EXIT
*
* Description: memory exit;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t mem_exit(void)
{
    //off memory ahb clock
    esCLK_MclkOnOff(h_dram_ahb_clk, CLK_OFF);
    //clock dram clock
    esCLK_CloseMclk(h_dram_ahb_clk);

    //exit sram manager
    mem_sram_exit();

    _ReleaseDramWorkModeAll();


    if (sys_mem.task_prio)
    {
        awos_task_delete(sys_mem.task_prio);
        sys_mem.task_prio = 0;
    }

    if (sys_mem.task_sem)
    {
        esKRNL_SemDel(sys_mem.task_sem, 0, NULL);
        sys_mem.task_sem = 0;
    }

    return EPDK_OK;
}

int32_t esMEM_DramWakeup(void)
{
    return CSP_DRAMC_access_enable();
}

int32_t esMEM_DramSuspend(void)
{
    return CSP_DRAMC_access_disable();
}

int32_t esMEM_BWEnable(void)
{
    esKRNL_SemPend(sys_mem.task_sem, 0, NULL);

    sys_mem.task_flag = 1;
    sys_mem.bw_num     = 0;
    sys_mem.bw_total   = 0;
    sys_mem.bw_max     = 0;
    sys_mem.bw_average = 0;

    esKRNL_SemPost(sys_mem.task_sem);

    return EPDK_OK;
}

int32_t esMEM_BWDisable(void)
{
    esKRNL_SemPend(sys_mem.task_sem, 0, NULL);
    sys_mem.task_flag = 0;
    esKRNL_SemPost(sys_mem.task_sem);

    return EPDK_OK;
}

int32_t esMEM_BWGet(__bw_dev_e mod)
{
    __dram_bandw_t bandw;

    CSP_DRAMC_get_all_bw_counter(&bandw);

    switch (mod)
    {
        case BW_DEVICE_CPU:
            return bandw.cpu_bw;
        case BW_DEVICE_DTMB:
            return bandw.dtmb_bw;
        case BW_DEVICE_VE:
            return bandw.ve_bw;
        case BW_DEVICE_DISP:
            return bandw.disp_bw;
        case BW_DEVICE_OTHER:
            return bandw.other_bw;
        case BW_DEVICE_TOTAL:
            return bandw.total_bw;
        default :
            return 0;
    }
}

int32_t esMEM_MasterSet(__dram_dev_e mod, __dram_master_t *master)
{
    int32_t   cpu_sr;

    cpu_sr = awos_arch_lock_irq();
    CSP_DRAMC_set_master_config(mod, master);
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_OK;
}

int32_t esMEM_MasterGet(__dram_dev_e mod, __dram_master_t *master)
{
    int32_t   cpu_sr;

    cpu_sr = awos_arch_lock_irq();
    CSP_DRAMC_get_master_config(mod, master);
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_OK;
}

/*
    保留1663的优先级调整接口，根据场景设计
    不同master的优先级
*/
int32_t esMEM_RequestDramUsrMode(__dram_user_mode_t mode)
{
    int32_t cpu_sr;

    cpu_sr = awos_arch_lock_irq();
    _RequestDramWorkModeAll(mode);
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_OK;
}

/*
    保留1663的优先级调整接口，还原所有master的
    优先级
*/
int32_t esMEM_ReleaseDramUsrMode(__dram_user_mode_t mode)
{
    int32_t   cpu_sr;

    cpu_sr = awos_arch_lock_irq();
    _ReleaseDramWorkModeAll();
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_OK;
}

#elif defined CONFIG_SOC_SUN8IW19

/*
*********************************************************************************************************
*                               WAKEUP DRAM
*
* Description: allow dram accessed;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esMEM_DramWakeup(void)
{
    __err("This function is not implemented!");
    return 0;
}


/*
*********************************************************************************************************
*                                   DRAM SUSPEND
*
* Description: forbid dram access;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t esMEM_DramSuspend(void)
{
    __err("This function is not implemented!");
    return 0;
}

/*
*********************************************************************************************************
*                                   QUERRY DRAM ACCESSABLE STATUS
*
* Description: querry dram accessable status;
*
* Arguments  : none
*
* Returns    : result, status of the dram accessable;
*
* Note       :
*********************************************************************************************************
*/
int32_t esMEM_BWEnable(void)
{
    __err("This function is not implemented!");
    return 0;
}

int32_t esMEM_BWDisable(void)
{
    __err("This function is not implemented!");
    return 0;
}

int32_t esMEM_BWGet(__bw_dev_e mod)
{
    __err("This function is not implemented!");
    return 0;
}

int32_t esMEM_MasterSet(__dram_dev_e mod, __dram_master_t *master)
{
    __err("This function is not implemented!");
    return 0;
}

int32_t esMEM_MasterGet(__dram_dev_e mod, __dram_master_t *master)
{
    __err("This function is not implemented!");
    return 0;
}

/*
*********************************************************************************************************
*                                     REQUEST DRAM USER MODE
*
* Description: request dram user mode;
*
* Arguments  : mode     user mode need be requested;
*
* Returns    : result,  EPDK_OK, EPDK_FAIL
*
* Notes      : request dram user mode, music/picture/video, for ex.
*********************************************************************************************************
*/
int32_t esMEM_RequestDramUsrMode(__dram_user_mode_t mode)
{
    __err("This function is not implemented!");
    return 0;
}


/*
*********************************************************************************************************
*                                   RELEASE DRAM USER MODE
*
* Description: release dram user mode;
*
* Arguments  : mode     user mode need be released;
*
* Returns    : result,  EPDK_OK, EPDK_FAIL
*
* Notes      :
*********************************************************************************************************
*/
int32_t esMEM_ReleaseDramUsrMode(__dram_user_mode_t mode)
{
    __err("This function is not implemented!");
    return 0;
}

#endif
