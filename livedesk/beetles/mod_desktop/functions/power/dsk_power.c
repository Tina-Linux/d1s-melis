/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "functions_i.h"
#include <mod_power.h>
#include <kconfig.h>
#define     BATTERY_AVER_CNT    20
#define POWERDEV "/dev/power"
/*
 * 剩余电量level, 判断低点
 */
//static __u32    g_battery[BATTERY_AVER_CNT];
//static __u32    g_battery_cnt = 0;
static __u32    speaker_open_status;
static int power_fp = -1;

__s32 dsk_power_dev_init(void)
{
    power_fp = open(POWERDEV, "rb+");
   
   if(power_fp < 0)
    {
        __msg("power_fp == RT_NULL");
        return EPDK_FAIL;
    }
    return EPDK_OK;
}
__s32 dsk_power_dev_uninit(void)
{
    if(power_fp)
    {
        close(power_fp);
        power_fp = -1;
    }
    return EPDK_OK;
}

//电压级别 DRV_POWER_CMD_BATADC_GET
__s32 dsk_power_get_battery_level(power_level_e *p_level)
{
    /*
    ES_FILE  *power;
    __drv_power_battery_status_t1 pbstatus;
    power_level_e level;


    power = eLIBs_fopen("b:\\HID\\POWER", "rb");
    if( !power )
    {
        __msg(" Power Driver cannot be open !");
        *p_level = DSK_POWER_LEVEL_0;
        return EPDK_FAIL;
    }
    esMODS_MIoctrl(power, DRV_POWER_CMD_BATADC_GET, 0, &pbstatus);
    fclose(power);

    if(!pbstatus.battery_exist)
        return -1;

    //cal last five aver volume
    {
        __u32 i;
        __u32 total_battery = 0;

        if(g_battery_cnt >= BATTERY_AVER_CNT)
        {
            for(i = 0; i < g_battery_cnt - 1; i++)
            {
                g_battery[i] = g_battery[i + 1];
            }
            g_battery_cnt = BATTERY_AVER_CNT;
        }
        else
        {
            g_battery_cnt++;
        }
        g_battery[g_battery_cnt - 1] = pbstatus.bat_vol;

        for(i = 0; i < g_battery_cnt; i++)
        {
            total_battery += g_battery[i];
        }

        pbstatus.bat_vol = total_battery/g_battery_cnt;
    }

    if(pbstatus.bat_vol < 3500)
        level  = DSK_POWER_LEVEL_0;
    else if(pbstatus.bat_vol >= 3500 && pbstatus.bat_vol < 3550)
        level = DSK_POWER_LEVEL_1;
    else if(pbstatus.bat_vol >= 3550 && pbstatus.bat_vol < 3625)
        level = DSK_POWER_LEVEL_2;
    else if(pbstatus.bat_vol >= 3625 && pbstatus.bat_vol < 3750)
        level = DSK_POWER_LEVEL_3;
    else if(pbstatus.bat_vol >= 3750 && pbstatus.bat_vol < 3950)
        level = DSK_POWER_LEVEL_4;
    else
        level = DSK_POWER_LEVEL_5;

    *p_level = level;
    */
    return EPDK_OK;
}

/*
 * 工作电压level,(工具栏显示电池电量级别)
 */
//电量级别 DRV_POWER_CMD_GET_FUELGUAGE
__s32 dsk_power_get_voltage_level(power_level_e *p_level)
{
    power_level_e                   vol_level = 0;
    __drv_power_fuelguage_t         info;
    int                             rest_vol;

    if(power_fp < 0)
    {
        __msg(" Power Driver cannot be open !");
        *p_level = DSK_POWER_LEVEL_5;
        return EPDK_FAIL;
    }

    ioctl(power_fp, DRV_POWER_CMD_GET_FUELGUAGE,&info);

    rest_vol    = info.rest_vol;
    __wrn("info.rest_vol=%d", info.rest_vol);

    if (rest_vol >= 100)
    {
        vol_level = DSK_POWER_LEVEL_5;
    }
    else if (rest_vol >= 80)
    {
        vol_level = DSK_POWER_LEVEL_4;
    }
    else if (rest_vol >= 60)
    {
        vol_level = DSK_POWER_LEVEL_3;
    }
    else if (rest_vol >= 40)
    {
        vol_level = DSK_POWER_LEVEL_2;
    }
    else if (rest_vol >= 20)
    {
        vol_level = DSK_POWER_LEVEL_1;
    }
    else if (rest_vol >= 0)
    {
        vol_level = DSK_POWER_LEVEL_0;
    }
    else
    {
        vol_level = DSK_POWER_LEVEL_0;
    }
    *p_level = vol_level;
    return EPDK_OK;
}

__bool dsk_power_is_full(void)
{
    __drv_power_fuelguage_t         info;

    if(power_fp < 0)
    {
        __msg(" Power Driver cannot be open !");
        return EPDK_FALSE;
    }

    ioctl(power_fp, DRV_POWER_CMD_GET_FUELGUAGE,&info);

    if (1 == info.is_full_charge)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

__bool dsk_power_is_low(void)
{
    __s32           ret;
    power_level_e   level;
    __drv_power_battery_status_t3   bat_sta;
    static __s32    low_power_cnt = 0;

    if(power_fp < 0)
    {
        __msg(" Power Driver cannot be open !");
        return EPDK_FALSE;
    }

    ret  = dsk_power_get_voltage_level(&level);

    if (ret < 0)
    {
        return EPDK_FALSE;
    }

    eLIBs_memset(&bat_sta, 0, sizeof(__drv_power_battery_status_t3));
    ioctl(power_fp, DRV_POWER_CMD_GET_DCIN,&bat_sta);

    __msg("***********bat_sta.charge_status=%d, level=%d***********", bat_sta.charge_status, level);
    __msg("low_power_cnt=%d", low_power_cnt);

    if (0 == bat_sta.charge_status && level == DSK_POWER_LEVEL_0)   // 低电
    {
        low_power_cnt++;

        if (low_power_cnt > 10)
        {
            __msg("low_power_cnt > 10!!!");
            return EPDK_TRUE;
        }
    }
    else
    {
        low_power_cnt = 0;
    }

    return EPDK_FALSE;
}

__s32 dsk_power_set_off_time(__u32 time)
{
    //  __gui_msg_t msg;
    //  __s32 ret;
    //
    //  g_memset(&msg, 0, sizeof(__gui_msg_t));
    //  msg.id          = DSK_MSG_SET_AUTO_OFF;
    //  msg.h_srcwin    = NULL;
    //  msg.h_deswin    = GUI_WinGetHandFromName(APP_DESKTOP_MWIN);
    //  msg.dwAddData1  = (__u32)time;
    //  msg.dwAddData2  = (__u32)0;
    //
    //  ret = GUI_SendNotifyMessage(&msg);
    //
    //  return ret;
    return EPDK_OK;
}

__s32 dsk_power_get_battary_state(power_state_e *state)
{
    /*
    ES_FILE *power;
    __drv_power_fuelguage_t     info;

    power = eLIBs_fopen("b:\\HID\\POWER", "rb");
    if( !power )
    {
        __msg(" Power Driver cannot be open !");
        return EPDK_FALSE;
    }

    esMODS_MIoctrl(power, DRV_POWER_CMD_GET_FUELGUAGE, 0, &info);

    if(info.charge_status & 0x2)
    {
        if(info.charge_status & 0x4)
        {
            if(info.charge_status & 0x1)
            {
                *state = DSK_POWER_BATTARY_CHARGE;
            }
            else
            {
                *state = DSK_POWER_BATTARY_FULL;
            }
        }
        else
        {
            *state = DSK_POWER_BATTARY_FULL;
        }
    }
    else
    {
        *state = DSK_POWER_BATTARY_FULL;
    }

    fclose( power );
    */
    return EPDK_OK;
}

//__s32 dsk_power_off(void);
//{
//  __gui_msg_t msg;
//
//  msg.id          = DSK_MSG_POWER_OFF;
//  msg.h_srcwin    = NULL;
//  msg.h_deswin    = GUI_WinGetHandFromName(APP_DESKTOP_MWIN);
//  msg.dwAddData1  = 0;
//  msg.dwAddData2  = 0;
//  msg.p_arg       = 0;
//
//  return GUI_SendNotifyMessage(&msg);
//}


void dsk_speaker_turnoff(void)
{
/*
    ES_FILE *pfile;

    __inf("speaker control off.....");
    pfile = eLIBs_fopen("b:\\HID\\POWER", "rb");

    if (pfile == 0)
    {
        __msg("b:\\HID\\POWER cannot open ");
        return;
    }

    //Get the previous status
    speaker_open_status = esMODS_MIoctrl(pfile, DRV_POWER_CMD_PWREPT_GET, POWER_EPT_3, 0);
    //Pull Low
    esMODS_MIoctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_3, (void *)((POWER_EPT_MOS_N) | (POWER_EPT_GATE_ON)));
    fclose(pfile);
    */
}

void dsk_speaker_turnon(void)
{
/*
    ES_FILE *pfile;

    __inf("speaker control On.....");
    pfile = eLIBs_fopen("b:\\HID\\POWER", "rb");

    if (pfile == 0)
    {
        __msg("b:\\HID\\POWER cannot open ");
        return;
    }

    // Get the previous status
    speaker_open_status = esMODS_MIoctrl(pfile, DRV_POWER_CMD_PWREPT_GET, POWER_EPT_3, 0);
    //Pull High
    esMODS_MIoctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_3, (void *)((POWER_EPT_MOS_P) | (POWER_EPT_GATE_ON)));
    fclose(pfile);
    */
}

void dsk_speaker_resume(void)
{
/*
    ES_FILE *pfile;

    __inf("speaker control resume.....");
    pfile = eLIBs_fopen("b:\\HID\\POWER", "rb");

    if (pfile == 0)
    {
        __msg("b:\\HID\\POWER cannot open ");
        return;
    }

    esMODS_MIoctrl(pfile, DRV_POWER_CMD_PWREPT_SET, POWER_EPT_3, (void *)speaker_open_status);     //return the previous status
    fclose(pfile);
    */
}

__bool dsk_get_charge_state(void)
{
    __drv_power_battery_status_t3   bat_sta;
    if(power_fp < 0)
    {
        __msg("power_fp == RT_NULL");
        return -1;
    }
    

    eLIBs_memset(&bat_sta, 0, sizeof(__drv_power_battery_status_t3));
    ioctl(power_fp, DRV_POWER_CMD_GET_DCIN,(void *)&bat_sta);

    if (0 == bat_sta.charge_status) // 非充电
    {
        __wrn("no in charge");
        return EPDK_FALSE;
    }
    else//充电
    {
        __wrn("in charge");
        return EPDK_TRUE;
    }
}
