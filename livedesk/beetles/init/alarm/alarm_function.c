#include <string.h>
#include <log.h>
#include "mod_init_i.h"

static __bool alarm_is_open_to_app = EPDK_FALSE;
static __hdle alarm_hdle = NULL;
static __s16  alarm_happen_index = -1;
static __u32  alarm_happen_timeCnt = 0;
static __bool alarm_is_happen[REG_ALARM_NUM] = {0};

// 0为周日
static __u16 _alarm_get_week_day(__u16 year, __u8 month, __u8 day)
{
    __u16   temp;
    __u8    yearH, yearL;
    __u8    table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
    yearH = year / 100;
    yearL = year % 100;

    if (yearH > 19)
    {
        yearL += 100;
    }

    temp = yearL + yearL / 4;
    temp = temp % 7;
    temp = temp + day + table_week[month - 1];

    if (yearL % 4 == 0 && month < 3)
    {
        temp--;
    }

    return (temp % 7);
}

static __u32 _alarm_get_time_minute(__awos_time_t time)
{
    return time.hour * 60 + time.minute;
}
static __u32 _alarm_get_day_minute(void)
{
    return 24 * 60;
}

static __s32 alarm_get_near_on_time(__awos_time_t *time, __s16 *alarmIndex)
{
    reg_system_para_t  *res_sys = NULL;
    __awos_time_t         curtime;
    __awos_date_t          curdate;
    __u16               week;
    __u32               temp = 0xFFFFFFFF, runTime;
    __s32               ret = EPDK_FAIL;
    __s16               i = 0;

    res_sys = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    esTIME_GetTime(&curtime);
    esTIME_GetDate(&curdate);

    week = _alarm_get_week_day(curdate.year, curdate.month, curdate.day);

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        if (res_sys->alarm[i].alarm_status == EPDK_TRUE && alarm_is_happen[i] == EPDK_FALSE)
        {
            if ((week >= 1 && week <= 5 && res_sys->alarm[i].alarm_repeat == REG_ALARM_DUTYDAY)     ||                              //周一至周五
                ((week == 6 || week == 0) && res_sys->alarm[i].alarm_repeat == REG_ALARM_WEEKEND)  ||                               //周末
                (res_sys->alarm[i].alarm_repeat == REG_ALARM_EVERYDAY || res_sys->alarm[i].alarm_repeat == REG_ALARM_ONETIME))      //每天,一次
            {
                if (_alarm_get_time_minute(curtime) <= _alarm_get_time_minute(res_sys->alarm[i].alarm_time))
                {
                    runTime = _alarm_get_time_minute(res_sys->alarm[i].alarm_time) - _alarm_get_time_minute(curtime);
                }
                else
                {
                    runTime = _alarm_get_day_minute() - (_alarm_get_time_minute(curtime) - _alarm_get_time_minute(res_sys->alarm[i].alarm_time));
                }

                if (runTime < temp)
                {
                    time->hour      = res_sys->alarm[i].alarm_time.hour;
                    time->minute    = res_sys->alarm[i].alarm_time.minute;
                    time->second    = res_sys->alarm[i].alarm_time.second;
                    temp = runTime;
                    *alarmIndex = i;
                }

                ret = EPDK_OK;
            }
        }
    }

    return ret;
}

static __s32 alarm_get_near_on_time_standby(__awos_time_t *time, __awos_date_t *date)
{
    reg_system_para_t  *res_sys = NULL;
    __awos_time_t       curtime;
    __u16               week, mDay;
    __u32               temp = 0xFFFFFFFF, runTime;
    __s32               ret = EPDK_FAIL;
    __s16               i = 0, alarmIndex = 0;

    res_sys = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    eLIBs_memset(&curtime, 0x00, sizeof(curtime));
    esTIME_GetTime(&curtime);
    esTIME_GetDate(date);

    week = _alarm_get_week_day(date->year, date->month, date->day);

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        if (res_sys->alarm[i].alarm_status == EPDK_TRUE)
        {
            if ((week >= 1 && week <= 5 && res_sys->alarm[i].alarm_repeat == REG_ALARM_DUTYDAY)     ||                              //周一至周五
                ((week == 6 || week == 0) && res_sys->alarm[i].alarm_repeat == REG_ALARM_WEEKEND)  ||                               //周末
                (res_sys->alarm[i].alarm_repeat == REG_ALARM_EVERYDAY || res_sys->alarm[i].alarm_repeat == REG_ALARM_ONETIME))      //每天,一次
            {
                if (_alarm_get_time_minute(curtime) <= _alarm_get_time_minute(res_sys->alarm[i].alarm_time))
                {
                    runTime = _alarm_get_time_minute(res_sys->alarm[i].alarm_time) - _alarm_get_time_minute(curtime);
                }
                else
                {
                    runTime = _alarm_get_day_minute() - (_alarm_get_time_minute(curtime) - _alarm_get_time_minute(res_sys->alarm[i].alarm_time));
                }

                if (runTime < temp)
                {
                    time->hour      = res_sys->alarm[i].alarm_time.hour;
                    time->minute    = res_sys->alarm[i].alarm_time.minute;
                    time->second    = res_sys->alarm[i].alarm_time.second;
                    temp = runTime;
                    alarmIndex = i;
                }

                ret = EPDK_OK;
            }
        }
    }

    if (ret == EPDK_OK)
    {
        if (_alarm_get_time_minute(curtime) > _alarm_get_time_minute(res_sys->alarm[alarmIndex].alarm_time))
        {
            date->day += 1;
        }
    }
    else
    {
        temp = 0xFFFFFFFF;

        for (i = 0; i < REG_ALARM_NUM; i++)
        {
            if (res_sys->alarm[i].alarm_status == EPDK_TRUE)
            {
                if ((week >= 1 && week <= 5 && res_sys->alarm[i].alarm_repeat == REG_ALARM_WEEKEND)     ||  //周一至周五设置周末
                    ((week == 6 || week == 0) && res_sys->alarm[i].alarm_repeat == REG_ALARM_DUTYDAY))      //周末设置周一至周五
                {
                    if (_alarm_get_time_minute(curtime) <= _alarm_get_time_minute(res_sys->alarm[i].alarm_time))
                    {
                        runTime = _alarm_get_time_minute(res_sys->alarm[i].alarm_time) - _alarm_get_time_minute(curtime);
                    }
                    else
                    {
                        runTime = _alarm_get_day_minute() - (_alarm_get_time_minute(curtime) - _alarm_get_time_minute(res_sys->alarm[i].alarm_time));
                    }

                    if (runTime < temp)
                    {
                        time->hour      = res_sys->alarm[i].alarm_time.hour;
                        time->minute    = res_sys->alarm[i].alarm_time.minute;
                        time->second    = res_sys->alarm[i].alarm_time.second;

                        if (week >= 1 && week <= 5 && res_sys->alarm[i].alarm_repeat == REG_ALARM_WEEKEND)
                        {
                            date->day = date->day + (6 - week);
                        }
                        else if ((week == 6 || week == 0) && res_sys->alarm[i].alarm_repeat == REG_ALARM_DUTYDAY)
                        {
                            if (week == 6)
                            {
                                date->day = date->day + 2;
                            }
                            else
                            {
                                date->day = date->day + 1;
                            }
                        }

                        temp = runTime;
                    }

                    ret = EPDK_OK;
                }
            }
        }
    }

    if (date->month == 1 || date->month == 3 || date->month == 5 ||
        date->month == 7 || date->month == 8 || date->month == 10 || date->month == 12)     //31天每月
    {
        mDay = 31;
    }
    else                                                                                    //30天每月
    {
        if (date->month != 2)
        {
            mDay = 30;
        }
        else
        {
            if ((date->year % 4 == 0 && date->year % 100 != 0) || date->year % 400 == 0) //闰年
            {
                mDay = 29;
            }
            else                                                                    //平年
            {
                mDay = 28;
            }
        }
    }

    if (date->day > mDay)
    {
        date->day = date->day - mDay;
        date->month = ((date->month + 1) > 12) ? 1 : (date->month + 1);
    }

    return ret;
}

static void alarm_open(__u32 second)
{
    int result = 0;

    //request alarm
    alarm_hdle = esTIME_RequestAlarm(0);

    if (!alarm_hdle)
    {
        __wrn("request the alarm_hdle failed!");
        return;
    }

    //enable and start alarm
    if (esTIME_StartAlarm(alarm_hdle, second) != EPDK_OK)
    {
        __wrn("start alarm_hdle failed!");
        esTIME_ReleaseAlarm(alarm_hdle);
        return;
    }
}

static void alarm_close(void)
{
    //you can stop alarm any time if you want

    esTIME_StopAlarm(alarm_hdle);

    //release the alarm, there is only one alarm

    esTIME_ReleaseAlarm(alarm_hdle);

    alarm_hdle = 0;
}

static __s32 standby_alarm_start(__awos_time_t alarmTime, __awos_date_t date)
{
    if (alarm_hdle != NULL)
    {
        alarm_close();
    }

    //esTIME_GetTime(&alarmTime);

    alarm_open((date.day - 1) * 24 * 60 * 60 + alarmTime.hour * 60 * 60 + (alarmTime.minute) * 60);

    __wrn("standby alarm:%d:%d, day:%d", alarmTime.hour, alarmTime.minute, date.day);

    return EPDK_OK;
}

static void alarm_sendMsg_to_ini(void)
{
    __gui_msg_t     msg;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.id          = DSK_MSG_ALARM;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)0;
    msg.dwAddData2  = (__u32)0;

    if (msg.h_deswin)
    {
        GUI_SendMessage(&msg);
    }
}

__bool alarm_is_exist(void)
{
    __s16   i = 0;
    reg_system_para_t  *res_sys = NULL;

    res_sys = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        if (res_sys->alarm[i].alarm_status == EPDK_TRUE)
        {
            return EPDK_TRUE;
        }
    }

    return EPDK_FALSE;
}

reg_alarm_para_t *alarm_get_happening(void)
{
    __s16   i = 0;
    reg_system_para_t  *res_sys = NULL;

    res_sys = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    //return &res_sys->alarm[0];//for test

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        if (alarm_is_happen[i] == EPDK_TRUE)
        {
            return &res_sys->alarm[i];
        }
    }

    return NULL;
}

__bool alarm_check_run(void)
{
    reg_system_para_t   *res_sys = NULL;
    __awos_time_t       curtime, nearTime;
    __u32               curTimeCnt, nearTimeCnt;
    __s16               alarmIndex = 0;

    eLIBs_memset(&curtime, 0x00, sizeof(curtime));
    eLIBs_memset(&nearTime, 0x00, sizeof(nearTime));

    res_sys = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    esTIME_GetTime(&curtime);

    curTimeCnt  = _alarm_get_time_minute(curtime);

    if (curTimeCnt - alarm_happen_timeCnt == 1 && alarm_happen_index != -1) //1分钟后清楚当前发生标记
    {
        alarm_is_happen[alarm_happen_index] = EPDK_FALSE;
        alarm_happen_index                  = -1;

        standby_startup_alarm();                                            //设置下一个闹钟
    }

    if (alarm_is_open_to_app == EPDK_TRUE)                                  //闹铃已经开启,进入应用
    {
        return EPDK_FALSE;
    }

    if (alarm_get_near_on_time(&nearTime, &alarmIndex) == EPDK_FAIL)        //不存在闹铃
    {
        return EPDK_FALSE;
    }

    nearTimeCnt = _alarm_get_time_minute(nearTime);

    if (curTimeCnt == nearTimeCnt && alarm_is_happen[alarmIndex] == EPDK_FALSE)
    {
        __wrn("%02d:%02d, alarm to app!", res_sys->alarm[alarmIndex].alarm_time.hour, res_sys->alarm[alarmIndex].alarm_time.minute);
        alarm_is_happen[alarmIndex] = EPDK_TRUE;
        alarm_happen_timeCnt        = curTimeCnt;
        alarm_happen_index          = alarmIndex;

        if (res_sys->alarm[alarmIndex].alarm_repeat == REG_ALARM_ONETIME)
        {
            res_sys->alarm[alarmIndex].alarm_status = EPDK_FALSE;
        }

        alarm_sendMsg_to_ini();                                             //闹铃时间到了, 发送消息到AP
        return EPDK_TRUE;
    }

    return EPDK_FALSE;                                                      //闹铃时间未到
}

void alarm_settingTime_cleanHappen(__s16 alarmIndex)
{
    alarm_is_happen[alarmIndex] = EPDK_FALSE;           //清除当前闹钟发生标记
}

__s32 standby_startup_alarm(void)
{
    __awos_time_t   time;
    __awos_date_t   date;

    memset(&time, 0x00, sizeof(time));
    memset(&date, 0x00, sizeof(date));

    if (alarm_get_near_on_time_standby(&time, &date) == EPDK_OK)
    {
        standby_alarm_start(time, date);
    }

    return EPDK_OK;
}

void alarm_enter_app(void)
{
    alarm_is_open_to_app    = EPDK_TRUE;
}

void alarm_exit_app(void)
{
    alarm_is_open_to_app    = EPDK_FALSE;
}
