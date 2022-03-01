/*
*********************************************************************************************************
*                                                    eMOD
*                                   the Easy Portable/Player Operation System
*                                            power manager sub-system
*
*                                     (c) Copyright 2008-2009, kevin.z China
*                                              All Rights Reserved
*
* File   : sys_pwrman.c
* Version: V1.0
* By     : kevin.z
* Date   : 2009-5-25 16:48
*********************************************************************************************************
*/
#include <arch.h>
#include <string.h>
#include <stdio.h>
#include <port.h>
#include <kapi.h>
#include <log.h>
#include <rtthread.h>
#include <sys_mems.h>
#include <sys_pins.h>
#include <csp_ccm_para.h>
#include <mod_powermanage.h>
#include <mod_defs.h>
#include <melis/standby/standby.h>

extern int             fioctrl(FILE* fp, int Cmd, void *args);

#if defined(CONFIG_DRIVERS_SUNXI_CCU)
    #define CSP_CCM_SYS_CLK_CPUX_C0_PLL         CSP_CCM_SYS_CLK_PLL_CPUX
    #define CSP_CCM_MOD_CLK_BUS_C0_CPU          CSP_CCM_MOD_CLK_CPUX
#endif
__sys_pwrman_para_t    PwrmanStandbyPar = { 0 };
__sys_pwrman_cfg_t     sys_pwrman = { 0 };

/*
*********************************************************************************************************
*                                     POWER MANAGER MAIN TASK
*
* Description: system power manager main task.
*
* Arguments  : none
*
* Returns    : none;
*********************************************************************************************************
*/
static void pwrman_main_task(void *arg)
{
    unsigned long long tmpIdleTime, tmpCurFreq, tmpRefFreq, tmpOldFreq;

    while (1)
    {
        //delay a power manager check prescale
        esKRNL_TimeDly(PWRMAN_CHECK_PRESCALE);

        //calcluate idle task run time
        tmpIdleTime = 100 * IDLE_RUN_PRESCALE;

        //request lock
        esKRNL_SemPend(sys_pwrman.lock, 0, NULL);

        if (!sys_pwrman.cpu_lock)
        {
            //check if need adjust core clock
            tmpCurFreq  = esCLK_GetSrcFreq(CSP_CCM_MOD_CLK_BUS_C0_CPU);
            tmpOldFreq  = tmpCurFreq;
            tmpRefFreq  = (tmpCurFreq * PWRMAN_CHECK_PRESCALE) / 100;
            if (tmpIdleTime * 100 < tmpRefFreq * IDLE_LOWEST_PERCENT)
            {
                if (tmpCurFreq != sys_pwrman.cur_para.core_pll_hi)
                {
                    //need increase core clock frequency
                    tmpCurFreq  = sys_pwrman.cur_para.core_pll_hi;
                    //adjust core pll, core/ahb/apb clock
                    //__wrn("adjust clock to %d", (__u32)tmpCurFreq);
                    esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, tmpCurFreq);
                }
            }
            else if (tmpIdleTime * 100 > tmpRefFreq * IDLE_HIGHEST_PERCENT)
            {
                //need decrease core clock frequency
                if (tmpCurFreq > LOWEST_CORE_CLOCK_FREQ)
                {
                    //check if need accelerate adjust core clock
                    if (tmpCurFreq == (sys_pwrman.cur_para.core_pll_hi - 2 * CORE_CLOCK_FREQ_PRESCALE))
                    {
                        //coarse adjust core clock, adjust core to a frequency, idle will be 20% then
                        //goal_freq * (1-20%) = cur_freq - idl_freq;
                        __u32   tmpIdleFreq = (tmpIdleTime * 100) / PWRMAN_CHECK_PRESCALE;
                        __u32   tmpGoalFreq = ((tmpCurFreq - tmpIdleFreq) * 5) / 4;

                        tmpGoalFreq = (tmpGoalFreq + CORE_CLOCK_FREQ_PRESCALE - 1) / CORE_CLOCK_FREQ_PRESCALE;
                        tmpGoalFreq *= CORE_CLOCK_FREQ_PRESCALE;

                        __inf("Coarse adjust, Goal core clock is %d", tmpGoalFreq);
                        if (tmpGoalFreq >= tmpCurFreq)
                        {
                            tmpCurFreq -= CORE_CLOCK_FREQ_PRESCALE;
                        }
                        else
                        {
                            tmpCurFreq = tmpGoalFreq;
                            if (tmpCurFreq < sys_pwrman.cur_para.core_pll_lo)
                            {
                                tmpCurFreq = sys_pwrman.cur_para.core_pll_lo;
                            }
                        }
                    }
                    else
                    {
                        //fine adjsut core clock
                        tmpCurFreq -= CORE_CLOCK_FREQ_PRESCALE;
                    }

                    if (tmpCurFreq >= sys_pwrman.cur_para.core_pll_lo)
                    {
                        if (tmpOldFreq != tmpCurFreq)
                        {
                            //adjust core pll
                            __wrn("adjust clock to %d", (__u32)tmpCurFreq);
                            esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, tmpCurFreq);
                        }
                    }
                }
            }
            else
            {
                //idle task run time is in a proper range, need not adjust it
                __inf("need not adjust core clock!");
            }
        }
        //release lock
        esKRNL_SemPost(sys_pwrman.lock);
    }
}


/*
*********************************************************************************************************
*                                     SYS_PWM_INIT
*
* Description: system power manager init.
*
* Arguments  : none
*
* Returns    : module init result;
*               EPDK_OK     -   module init successed,
*               EPDK_FAIL   -   module init failed;
*********************************************************************************************************
*/
int32_t pwrman_init(__dram_para_t *para)
{
    int32_t     display_mode;
    int32_t     PMU_exist, power_key_msg, board;
    user_gpio_set_t gpio_set = {0};

    //initialize standby dram parameters
    if (para == NULL)
    {
        __err("invalid parameters to initialize pwrman");
        return EPDK_FAIL;
    }

    rt_memset(&sys_pwrman, 0, sizeof(__sys_pwrman_cfg_t));

    if (esCFG_GetKeyValue("display_mode", "pvp_mode",  &display_mode, 1))
    {
        __msg("fail to get pwrman status");
        display_mode    = 1;
    }

    esCFG_GetKeyValue("power_para", "power_used", &PMU_exist, 1);
    esCFG_GetKeyValue("power_para", "power_msg",  &power_key_msg, 1);
    esCFG_GetKeyValue("power_para", "power_twi",  &board, 1);

    PwrmanStandbyPar.dram_para                  = (*para);
    PwrmanStandbyPar.TvdFlag                    = (display_mode ? 1:0) ;
    PwrmanStandbyPar.PowerCfg.keyvalue          = power_key_msg & 0xff;
    PwrmanStandbyPar.PowerCfg.twi_controller    = board;
    PwrmanStandbyPar.PowerCfg.power_exist       = PMU_exist;

    sys_pwrman.lock = esKRNL_SemCreate(1);
    if (!sys_pwrman.lock)
    {
        __err("create semaphore for pwrman failed!");
        return EPDK_FAIL;
    }

    if (EPDK_OK == esCFG_GetKeyValue("standby_eint_para", "pio_pin", (__s32 *)(&gpio_set), sizeof(gpio_set) / sizeof(int)))
    {
        PwrmanStandbyPar.gpio_hdl   = esPINS_PinGrpReq(&gpio_set, 1);
        PwrmanStandbyPar.port       = gpio_set.port;
        PwrmanStandbyPar.port_num   = gpio_set.port_num;
        esCFG_GetKeyValue("standby_eint_para", "eint_num", (__s32 *)&PwrmanStandbyPar.eint_no, 1);
        esCFG_GetKeyValue("standby_eint_para", "trigger_condition", (__s32 *)&PwrmanStandbyPar.pio_int_trigger, 1);
        __log("port=%d,port_num=%d,mul_sel=%d eint_num=%d trigger=%d.", gpio_set.port, gpio_set.port_num, gpio_set.mul_sel, PwrmanStandbyPar.eint_no, PwrmanStandbyPar.pio_int_trigger);
    }

    //unlock cpu frequency
    sys_pwrman.cpu_lock         = 0;
    sys_pwrman.start_flag       = EPDK_FALSE;
    sys_pwrman.mode_list        = (__pwrman_mode_list_t *)rt_malloc(sizeof(__pwrman_mode_list_t));
    if (!sys_pwrman.mode_list)
    {
        __err("pwrman request memory failed!");
        return EPDK_FAIL;
    }
    //set pwrman parameter to the lowest mode (mode 3)
    sys_pwrman.mode_list->next  = 0;
    sys_pwrman.mode_list->mode  = SYS_PWRMAN_MODE_LEVEL3;

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYS_PWM_START
*
* Description: system power manager start.
*
* Arguments  : none
*
* Returns    : module start result;
*               EPDK_OK     -   module init successed,
*               EPDK_FAIL   -   module init failed;
*********************************************************************************************************
*/
int32_t pwrman_start(void)
{
    uint32_t    tmpvdd;
    __hdle      pTmpFile;
    long        arg[3] = {0,0,0};
    //initiate power manager cofig mode parameters with script
    if (initPwrmanIniCfg(&sys_pwrman) != EPDK_OK)
    {
        __err("Config power manager parameter failed!");
        pwrman_stop();
        return EPDK_FAIL;
    }

    //try to open power device
    sys_pwrman.h_power = fopen("/dev/regulator", "wb");
    if (!sys_pwrman.h_power)
    {
        __log("open power driver failed!");
        return EPDK_FAIL;
    }

    //get pmu type
    arg[0]  = 0ul;
    arg[1]  = 0ul;
    PwrmanStandbyPar.PowerCfg.power_exist   = fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_GET_PMU_TYPE, arg);

    //request lock
    esKRNL_SemPend(sys_pwrman.lock, 0, NULL);

    //set power mode to mode 3, the lowest power mode
    sys_pwrman.cur_para.core_pll_hi = sys_pwrman.mode[3].core_pll_hi;
    sys_pwrman.cur_para.core_pll_lo = sys_pwrman.mode[3].core_pll_lo;
    sys_pwrman.cur_para.dram_pll    = sys_pwrman.mode[3].dram_pll;
    sys_pwrman.cur_para.vdd         = sys_pwrman.mode[3].vdd;

    //check if need adjust vdd now
    arg[0]  = POWER_VOL_DCDC2;
    arg[1]  = 0ul;
    tmpvdd = fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_GET, arg);
    if (tmpvdd < sys_pwrman.cur_para.vdd)
    {
        __inf("Try to adjust vdd to %d", sys_pwrman.cur_para.vdd);

        arg[0]  = POWER_VOL_DCDC2;
        arg[1]  = (unsigned long)sys_pwrman.cur_para.vdd;
        fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_SET, arg);
        tmpvdd = sys_pwrman.cur_para.vdd;
        
        arg[0]  = POWER_VOL_DCDC2;
        arg[1]  = 0ul;
        __inf("Current vdd is %d", fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_GET, arg));
    }

    //set core pll to the highest value, it will turn to the proper value
    __log("Adust core clock to %d", sys_pwrman.cur_para.core_pll_hi);
    esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, sys_pwrman.cur_para.core_pll_hi);

    //adjust dram pll and dram clock
    //__inf("Adust dram clock to %d", sys_pwrman.cur_para.dram_pll);
    //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, sys_pwrman.cur_para.dram_pll);

    if (tmpvdd != sys_pwrman.cur_para.vdd)
    {
        __inf("Try to adjust vdd to %d", sys_pwrman.cur_para.vdd);

        arg[0]  = POWER_VOL_DCDC2;
        arg[1]  = sys_pwrman.cur_para.vdd;
        fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_SET, arg);
    }

    //create power manager server
    sys_pwrman.task_prio    = awos_task_create("pwrm_tsk",  pwrman_main_task, (void *)0, 0x400, RT_TIMER_THREAD_PRIO - 3, 10);
    if (!sys_pwrman.task_prio)
    {
        __log("Create power manager main task failed!");
        pwrman_stop();
        return EPDK_FAIL;
    }
    sys_pwrman.start_flag   = EPDK_TRUE;

    //release lock
    esKRNL_SemPost(sys_pwrman.lock);
#if 0
    //create virtual space for standby module
    if (awos_arch_vmem_create((uint8_t *)STANDBY_ENTRY, STANDBY_SIZE >> 12, MEMS_domain_client) == EPDK_FAIL)
    {
        __log("Create standby module virtual space fail!");
    }

    pTmpFile    = fopen("/data/standby.bin", "rb");
    if (pTmpFile)
    {
        //initialize standby module entery
        sys_pwrman.pStandbyBin  = (void *)STANDBY_ENTRY;

        //clear standby moudle virtual space,
        //the RO and RW section will overlay when load standby module,
        //so the ZI section no need to clear again.
        rt_memset(sys_pwrman.pStandbyBin, 0, STANDBY_SIZE);

        //load standby module RO and RW sections.
        fread(sys_pwrman.pStandbyBin, 1, STANDBY_SIZE, pTmpFile);

        //close file pointer
        fclose((__hdle)pTmpFile);
    }
    else
    {
        __log("try to load standby module failed!");

        //delete standby module virtual space
        awos_arch_vmem_delete((uint8_t *)STANDBY_ENTRY, STANDBY_SIZE >> 12);

        //standby service is disable
        sys_pwrman.pStandbyBin = 0;
    }
#endif
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYS_PWM_STOP
*
* Description: system power manager stop.
*
* Arguments  : none
*
* Returns    : module stop result;
*               EPDK_OK     -   module init successed,
*               EPDK_FAIL   -   module init failed;
*********************************************************************************************************
*/
int32_t pwrman_stop(void)
{
    //init current paramter index to invalid
    sys_pwrman.start_flag       = EPDK_FALSE;

    //delete pwrman main task
    if (sys_pwrman.task_prio)
    {
        //request delete power manager main task
        awos_task_delete(sys_pwrman.task_prio);
        sys_pwrman.task_prio    = 0;
    }

    if (sys_pwrman.h_power)
    {
        fclose((__hdle)sys_pwrman.h_power);
        sys_pwrman.h_power      = 0;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYS_PWM_EXIT
*
* Description: system power manager exit.
*
* Arguments  : none
*
* Returns    : module exit result;
*               EPDK_OK     -   module exit successed,
*               EPDK_FAIL   -   module exit failed;
*********************************************************************************************************
*/
int32_t pwrman_exit(void)
{
    __pwrman_mode_list_t   *tmpMode;

    //release memory
    while (sys_pwrman.mode_list)
    {
        tmpMode = sys_pwrman.mode_list->next;
        k_free(sys_pwrman.mode_list);
        sys_pwrman.mode_list    = tmpMode;
    }

    //delete semaphore
    if (sys_pwrman.lock)
    {
        esKRNL_SemDel(sys_pwrman.lock, 0, NULL);
        sys_pwrman.lock         = NULL;
    }
    if (PwrmanStandbyPar.gpio_hdl)
    {
        esPINS_PinGrpRel(PwrmanStandbyPar.gpio_hdl, 0) ;
        PwrmanStandbyPar.gpio_hdl   = NULL ;
    }
    //unlock cpu frequency
    sys_pwrman.cpu_lock         = 0;

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYSTEM PWM REQUEST PWM MODE
*
* Description: request system power manager mode.
*
* Arguments  : mode     pwrman mode need be requested.
*
* Returns    : request pwrman mode result;
*               EPDK_OK     -   requested pwrman mode successed,
*               EPDK_FAIL   -   requested pwrman mode failed;
*********************************************************************************************************
*/
int32_t esPWRMAN_ReqPwrmanMode(int32_t mode)
{
    __pwrman_mode_list_t   *tmpMode;
    __pwrman_mode_list_t   *tmpPtr;
    __pwrman_mode_cfg_t    tmpPara;
    long  arg[3] = {0,0,0};

    __log("Request pwrman mode:%d", mode);

    //check if mode is valid
    if (((__u32)mode >= PWRMAN_MODE_CFG_CNT) || (!sys_pwrman.mode[mode].valid))
    {
        switch (mode)
        {
            case SYS_PWRMAN_MODE_USBD:
            {
                if (!sys_pwrman.usbd_flag && sys_pwrman.h_power)
                {
                    //process usb cable connect
                    __drv_power_ips_status_t    status;
                    rt_memset(&status, 0, sizeof(__drv_power_ips_status_t));

                    arg[0]  = 0;
                    arg[1]  = (unsigned long)&status;
                    fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_IPS_GET, arg);
                    status.vhold_mode   = 1;
                    arg[0]  = 0;
                    arg[1]  = (unsigned long)&status;
                    fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_IPS_SET, arg);
                }
                sys_pwrman.usbd_flag++;
                mode    = SYS_PWRMAN_MODE_LEVEL0;
                break;
            }

            case SYS_PWRMAN_MODE_USBH:
            {
                if (!sys_pwrman.usbh_flag)
                {
                    //limit
                    __inf("First time usbh mode, maybe need limit current!");
                }

                sys_pwrman.usbh_flag++;
                mode    = SYS_PWRMAN_MODE_LEVEL0;
                break;
            }
            default:
            {
                __log("pwrman mode is invalid");
                return EPDK_FAIL;
            }
        }
    }

    tmpMode = (__pwrman_mode_list_t *)k_malloc(sizeof(__pwrman_mode_list_t));
    if (!tmpMode)
    {
        __log("Request memory by pwrman failed!");
        return EPDK_FAIL;
    }

    tmpMode->next = NULL;
    tmpMode->mode = mode;

    tmpPara.core_pll_hi = 2000 * 1000 * 1000;
    tmpPara.core_pll_lo = 0;
    tmpPara.dram_pll    = 0;
    tmpPara.vdd         = 0;

    //try to get the lock, to avoid conflict
    esKRNL_SemPend(sys_pwrman.lock, 0, NULL);

    //insert mode to mode list
    tmpPtr  = tmpMode;
    tmpMode->next   = sys_pwrman.mode_list;
    sys_pwrman.mode_list    = tmpPtr;

    if (sys_pwrman.start_flag == EPDK_TRUE)
    {
        //check if need adjust parameter
        while (tmpPtr)
        {
            if (tmpPara.core_pll_hi > sys_pwrman.mode[tmpPtr->mode].core_pll_hi)
            {
                tmpPara.core_pll_hi = sys_pwrman.mode[tmpPtr->mode].core_pll_hi;
            }
            if (tmpPara.core_pll_lo < sys_pwrman.mode[tmpPtr->mode].core_pll_lo)
            {
                tmpPara.core_pll_lo = sys_pwrman.mode[tmpPtr->mode].core_pll_lo;
            }
            if (tmpPara.dram_pll < sys_pwrman.mode[tmpPtr->mode].dram_pll)
            {
                tmpPara.dram_pll    = sys_pwrman.mode[tmpPtr->mode].dram_pll;
            }
            if (tmpPara.vdd < sys_pwrman.mode[tmpPtr->mode].vdd)
            {
                tmpPara.vdd         = sys_pwrman.mode[tmpPtr->mode].vdd;
            }

            tmpPtr  = tmpPtr->next;
        }

        //check if need adjust vdd now
        if (sys_pwrman.cur_para.vdd < tmpPara.vdd)
        {
            __inf("Try to adjust vdd to %d", tmpPara.vdd);

            arg[0]  = POWER_VOL_DCDC2;
            arg[1]  = tmpPara.vdd;
            fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_SET, arg);
        }

        //check if need adjust core clock
        if (!sys_pwrman.cpu_lock)
        {
            //set core pll to the highest value, it will turn to the proper value
            esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, tmpPara.core_pll_hi);
            __inf("Adust core clock to %d", tmpPara.core_pll_hi);
        }

        //process dram pll
        if (sys_pwrman.cur_para.dram_pll < tmpPara.dram_pll)
        {
            //adjust dram pll and dram clock
            //esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, tmpPara.dram_pll);
            //__inf("Adust dram clock to %d", tmpPara.dram_pll);
        }

        sys_pwrman.cur_para.core_pll_hi = tmpPara.core_pll_hi;
        sys_pwrman.cur_para.core_pll_lo = tmpPara.core_pll_lo;
        sys_pwrman.cur_para.dram_pll    = tmpPara.dram_pll;
        sys_pwrman.cur_para.vdd         = tmpPara.vdd;
    }

    //release lock
    esKRNL_SemPost(sys_pwrman.lock);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYSTEM PWM RELEASE PWM MODE
*
* Description: release system power manager mode.
*
* Arguments  : mode     pwrman mode need be release.
*
* Returns    : release pwrman mode result;
*               EPDK_OK     -   release pwrman mode successed,
*               EPDK_FAIL   -   release pwrman mode failed;
*********************************************************************************************************
*/
int32_t esPWRMAN_RelPwrmanMode(int32_t mode)
{
    __pwrman_mode_list_t   *tmpPtr;
    __pwrman_mode_cfg_t    tmpPara;
    long  arg[3] = {0,0,0};

    __inf("Release pwrman mode:%d", mode);

    //check if mode is valid
    if (((__u32)mode >= PWRMAN_MODE_CFG_CNT) || (!sys_pwrman.mode[mode].valid))
    {
        switch (mode)
        {
            //release usb device power mode
            case SYS_PWRMAN_MODE_USBD:
            {
                if (sys_pwrman.usbd_flag > 0)
                {
                    sys_pwrman.usbd_flag--;
                    if (!sys_pwrman.usbd_flag && sys_pwrman.h_power)
                    {
                        //process usb cable connect
                        __drv_power_ips_status_t    status;
                        rt_memset(&status, 0, sizeof(__drv_power_ips_status_t));

                        arg[0]  = 0ul;
                        arg[1]  = (unsigned long)&status;
                        fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_IPS_GET, arg);

                        status.vhold_mode   = 0;
                        arg[0]  = 0ul;
                        arg[1]  = (unsigned long)&status;
                        fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_IPS_SET, arg);
                    }
                }
                else
                {
                    __log("pwrman mode (usb) request and release is not in pairs!");
                }

                mode = SYS_PWRMAN_MODE_LEVEL0;
                break;
            }

            //release usb host power mode
            case SYS_PWRMAN_MODE_USBH:
            {
                if (sys_pwrman.usbh_flag > 0)
                {
                    sys_pwrman.usbh_flag--;
                }
                else
                {
                    __log("pwrman mode (usb) request and release is not in pairs!");
                }
                mode = SYS_PWRMAN_MODE_LEVEL0;
                break;
            }
            default:
            {
                __log("pwrman mode is invalid");
                return EPDK_FAIL;
            }
        }
    }

    tmpPara.core_pll_hi = 2000 * 1000 * 1000;
    tmpPara.core_pll_lo = 0;
    tmpPara.dram_pll    = 0;
    tmpPara.vdd         = 0;

    //try to get the lock, to avoid conflict
    esKRNL_SemPend(sys_pwrman.lock, 0, NULL);

    //remove mode from mode list
    tmpPtr  = sys_pwrman.mode_list;
    if (sys_pwrman.mode_list->mode == mode)
    {
        sys_pwrman.mode_list    = sys_pwrman.mode_list->next;
        k_free(tmpPtr);
    }
    else
    {
        while (tmpPtr->next)
        {
            if (tmpPtr->next->mode == mode)
            {
                __pwrman_mode_list_t *tmpNode;

                tmpNode = tmpPtr->next;
                tmpPtr->next    = tmpPtr->next->next;
                k_free(tmpNode);
                break;
            }
            tmpPtr = tmpPtr->next;
        }
    }

    if (sys_pwrman.start_flag == EPDK_TRUE)
    {
        //check if need adjust parameter
        tmpPtr  = sys_pwrman.mode_list;
        while (tmpPtr)
        {
            if (tmpPara.core_pll_hi > sys_pwrman.mode[tmpPtr->mode].core_pll_hi)
            {
                tmpPara.core_pll_hi = sys_pwrman.mode[tmpPtr->mode].core_pll_hi;
            }
            if (tmpPara.core_pll_lo < sys_pwrman.mode[tmpPtr->mode].core_pll_lo)
            {
                tmpPara.core_pll_lo = sys_pwrman.mode[tmpPtr->mode].core_pll_lo;
            }
            if (tmpPara.dram_pll < sys_pwrman.mode[tmpPtr->mode].dram_pll)
            {
                tmpPara.dram_pll    = sys_pwrman.mode[tmpPtr->mode].dram_pll;
            }
            if (tmpPara.vdd < sys_pwrman.mode[tmpPtr->mode].vdd)
            {
                tmpPara.vdd         = sys_pwrman.mode[tmpPtr->mode].vdd;
            }

            tmpPtr  = tmpPtr->next;
        }

        //check if need adjust vdd now
        if (sys_pwrman.cur_para.vdd < tmpPara.vdd)
        {
            __inf("Try to adjust vdd to %d", tmpPara.vdd);
            arg[0]  = POWER_VOL_DCDC2;
            arg[1]  = tmpPara.vdd;
            fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_SET, arg);
        }

        //check if need adjust core clock
        if (!sys_pwrman.cpu_lock)
        {
            //set core pll to the highest value, it will turn to the proper value
            esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, tmpPara.core_pll_hi);
            __inf("Adust core clock to %d", tmpPara.core_pll_hi);
        }

        //process dram pll
        if (tmpPara.dram_pll < sys_pwrman.cur_para.dram_pll)
        {
            //adjust dram pll and dram clock
            //    esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_SDRAM_PLL, tmpPara.dram_pll);
            //    __inf("Adust dram clock to %d", tmpPara.dram_pll);
        }

        //check if need adjust vdd now
        if (tmpPara.vdd < sys_pwrman.cur_para.vdd)
        {
            __inf("Try to adjust vdd to %d", tmpPara.vdd);
            arg[0]  = POWER_VOL_DCDC2;
            arg[1]  = tmpPara.vdd;
            fioctrl(sys_pwrman.h_power, DRV_POWER_CMD_VOUT_SET, arg);
        }

        sys_pwrman.cur_para.core_pll_hi = tmpPara.core_pll_hi;
        sys_pwrman.cur_para.core_pll_lo = tmpPara.core_pll_lo;
        sys_pwrman.cur_para.dram_pll    = tmpPara.dram_pll;
        sys_pwrman.cur_para.vdd         = tmpPara.vdd;
    }

    //release lock
    esKRNL_SemPost(sys_pwrman.lock);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYSTEM PWM GET PWM MODE
*
* Description: get system power manager mode.
*
* Arguments  : time     time for system power off, based on seconds;
*
* Returns    : none;
*********************************************************************************************************
*/
void esPWRMAN_EnterStandby(uint32_t power_off_flag)
{
#ifdef CONFIG_STANDBY
	standby_enter(STANDBY_NORMAL);
#endif
}


/*
*********************************************************************************************************
*                                     SYSTEM POWER MANAGER USER EVENT NOTIFY
*
* Description: system power manager user ervent notify. If some user event comming, we need update
*              cpu frequency to response user event.
*
* Arguments  : none;
*
* Returns    : none;
*********************************************************************************************************
*/
void esPWRMAN_UsrEventNotify(void)
{
    if (sys_pwrman.start_flag != EPDK_TRUE)
    {
        __inf("pwrman has not started!");
        return;
    }

    //request lock to avoid conflict
    if (!esKRNL_SemAccept(sys_pwrman.lock))
    {
        //we can't get lock now, to avoid system response slow, just renturn
        return;
    }

    if (!sys_pwrman.cpu_lock)
    {
        //set core pll to the highest value
        esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, sys_pwrman.cur_para.core_pll_hi);
        __inf("Adust core clock to %d", sys_pwrman.cur_para.core_pll_hi);
    }

    //release lock
    esKRNL_SemPost(sys_pwrman.lock);

    return;
}


/*
*********************************************************************************************************
*                                     SYSTEM POWER MANAGER LOCK CPU FREQUENCY
*
* Description: system power manager lock cpu frequency to the highest value.
*
* Arguments  : none;
*
* Returns    : none;
*********************************************************************************************************
*/
int32_t esPWRMAN_LockCpuFreq(void)
{
    __inf("Lock cpu frequency!");

    //try to get semaphore for adjust cpu clock
    esKRNL_SemPend(sys_pwrman.lock, 0, NULL);
    sys_pwrman.cpu_lock ++;

    if (sys_pwrman.start_flag == EPDK_TRUE)
    {
        //set core pll to the highest value
        esCLK_SetSrcFreq(CSP_CCM_SYS_CLK_CPUX_C0_PLL, sys_pwrman.cur_para.core_pll_hi);
        __inf("Adust core clock to %d", sys_pwrman.cur_para.core_pll_hi);
    }

    //release lock
    esKRNL_SemPost(sys_pwrman.lock);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYSTEM POWER MANAGER UNLOCK CPU FREQUENCY
*
* Description: system power manager unlock cpu frequency.
*
* Arguments  : none;
*
* Returns    : none;
*********************************************************************************************************
*/
int32_t esPWRMAN_UnlockCpuFreq(void)
{
    __inf("Unlock cpu frequency!");

    //try to get semaphore for adjust cpu clock
    esKRNL_SemPend(sys_pwrman.lock, 0, NULL);
    if (sys_pwrman.cpu_lock > 0)
    {
        sys_pwrman.cpu_lock--;
    }
    else
    {
        __log("frequency lock/unlock is invalid!!!");
    }

    //release lock
    esKRNL_SemPost(sys_pwrman.lock);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYSTEM PWM REGISTER DEVICE
*
* Description: register device to power manager.
*
* Arguments  : device   power user device.
*              cb       call-back functin.
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*********************************************************************************************************
*/
int32_t esPWRMAN_RegDevice(__sys_pwrman_dev_e device, __pCB_DPMCtl_t cb, void *parg)
{
    __pwrman_dev_node_t    *tmpNode, *tmpLink;
    int32_t         cpu_sr;

    // request & clear memory for the device node
    tmpNode = k_malloc(sizeof(__pwrman_dev_node_t));
    if (!tmpNode)
    {
        __log("allocate memory for device power management node failed");
        return EPDK_FAIL;
    }
    rt_memset(tmpNode, 0, sizeof(__pwrman_dev_node_t));

    // check device power management node register already or not
    tmpLink = sys_pwrman.pDevList;
    while (tmpLink)
    {
        if (tmpLink->devid == device)
        {
            __log("device [%d] power management node registered already", device);
            return EPDK_OK;
        }
        tmpLink     = tmpLink->next;
    }

    // set device power management node
    tmpNode->devid  = device;
    tmpNode->cb     = cb;
    tmpNode->parg   = parg;

    cpu_sr  = awos_arch_lock_irq();
    if (!sys_pwrman.pDevList)
    {
        // pDevList is empty now, insert new node to the head of pDevList.
        sys_pwrman.pDevList = tmpNode;
        tmpNode->prev       = NULL;
        tmpNode->next       = NULL;
    }
    else
    {
        // the device node within pDevList should sort ascending by devid.
        // find the last device node smaller then new node within pDevList.
        tmpLink = sys_pwrman.pDevList;
        while ((__s32)(tmpLink->devid) < (__s32)device)
        {
            if (!tmpLink->next)
            {
                // the last device node reached,
                // insert the new node to the tail of pDevList.
                tmpLink->next   = tmpNode;
                tmpNode->prev   = tmpLink;
                tmpNode->next   = NULL;
                awos_arch_unlock_irq(cpu_sr);
                return EPDK_OK;
            }
            tmpLink = tmpLink->next;
        }
        // insert new node to the previous of target device node.
        tmpLink->prev->next = tmpNode;
        tmpNode->prev       = tmpLink->prev;
        tmpNode->next       = tmpLink;
        tmpLink->prev       = tmpNode;
    }
    awos_arch_unlock_irq(cpu_sr);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     SYSTEM PWM UNREGISTER DEVICE
*
* Description: unregister device to power manager.
*
* Arguments  : device   power user device.
*              cb       call-back functin.
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*********************************************************************************************************
*/
int32_t esPWRMAN_UnregDevice(__sys_pwrman_dev_e device, __pCB_DPMCtl_t cb)
{
    __pwrman_dev_node_t *tmpNode = 0, *tmpLink = 0;
    int32_t               cpu_sr;

    // input the node to the list sequentially
    cpu_sr  = awos_arch_lock_irq();
    tmpLink = sys_pwrman.pDevList;
    while (tmpLink && (tmpLink->devid == device) && (tmpLink->cb == cb))
    {
        tmpLink = tmpLink->next;
    }
    awos_arch_unlock_irq(cpu_sr);

    if (!tmpNode)
    {
        __log("invalid device node [%d] to unregister device power management");
        return EPDK_FAIL;
    }

    // adjust the device node relationship links
    if (tmpNode->prev)
    {
        tmpNode->prev->next = tmpNode->next;
    }
    if (tmpNode->next)
    {
        tmpNode->next->prev = tmpNode->prev;
    }
    // destory device node links
    tmpNode->next   = NULL;
    tmpNode->prev   = NULL;

    // k_free node memory.
    k_free(tmpNode);
    return EPDK_OK;
}

int32_t esPWRMAN_GetStandbyPara(__sys_pwrman_para_t *pStandbyPara)
{
    rt_memcpy(pStandbyPara, &PwrmanStandbyPar, sizeof(__sys_pwrman_para_t));
    return EPDK_OK;
}


int32_t esPWRMAN_SetStandbyMode(uint32_t standby_mode)
{
    PwrmanStandbyPar.standby_mode   = standby_mode;
    return EPDK_OK;
}

