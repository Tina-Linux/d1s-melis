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
#include <sys_device.h>
#include <sys_fsys.h>
#include <sys_time.h>
#include <log.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <kapi.h>

static __hdle     hRTC = 0;

/*
*********************************************************************************************************
*                                   RTC INITIALISE
*
* Description: rtc initialise;
*
* Arguments  : none;
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t rtc_init(void)
{
#if 0
    //plug-in rtc device
    esDEV_Plugin("\\drv\\rtc.drv", 0, 0, 1);

    //open rtc device
    hRTC = esFSYS_fopen("b:\\HWSC\\RTC", "r+");
    if (!hRTC)
    {
        __log("try to open rtc device failed!");
    }
#endif
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                   RTC EXIT
*
* Description: rtc exit;
*
* Arguments  : none
*
* Returns    : result, EPDK_OK/EPDK_FAIL;
*
* Note       :
*********************************************************************************************************
*/
int32_t rtc_exit(void)
{
#if 0
    //close rtc device handle
    if (hRTC)
    {
        esFSYS_fclose((__hdle)hRTC);
        hRTC = 0;
    }

    //plug-out rtc device
    esDEV_Plugout("\\drv\\rtc.drv", 0);
#endif
    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                            GET RTC TIME
*
* Description: get rtc time;
*
* Arguments  : time     pointer to time for return current time;
*
* Returns    : result
*               EPDK_OK/EPDK_FAIL
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esTIME_GetTime(__awos_time_t *time)
{
    if (!hRTC)
    {
        struct timeval tm_val;
        struct tm tm;

        memset(&tm_val, 0, sizeof(struct timeval));
        memset(&tm, 0, sizeof(struct tm));

        gettimeofday(&tm_val, NULL);
        localtime_r(&tm_val.tv_sec, &tm);

        time->hour = tm.tm_hour;
        time->minute = tm.tm_min;
        time->second = tm.tm_sec;
        return EPDK_OK;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_GET_TIME, 0, time);
}


/*
*********************************************************************************************************
*                                            SET RTC TIME
*
* Description: set system time;
*
* Arguments  : time     pointer to time for return current system time;
*
* Returns    : EPDK_OK/EPDK_FAIL
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esTIME_SetTime(__awos_time_t *time)
{
    if (!hRTC)
    {
        struct timeval tm_val;
        struct tm tm;
        clock_t fixed_time = 0;
        struct timeval sys_time = {0};

        memset(&tm_val, 0, sizeof(struct timeval));
        memset(&tm, 0, sizeof(struct tm));

        gettimeofday(&tm_val, NULL);
        localtime_r(&tm_val.tv_sec, &tm);

        tm.tm_hour = time->hour;
        tm.tm_min = time->minute;
        tm.tm_sec = time->second;

        fixed_time = mktime(&tm);

        sys_time.tv_sec = fixed_time;
        settimeofday(&sys_time, NULL);

        return EPDK_OK;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_SET_TIME, 0, time);
}

/*
*********************************************************************************************************
*                                            GET RTC DATE
*
* Description: get system date;
*
* Arguments  : date     pointer to date for return;
*
* Returns    : EPDK_OK/EPDK_FAIL
*
* Notes      : only process 2009-01-01, 00:00:00 ~ 2024-12-31, 23:59:59,
*
*********************************************************************************************************
*/
int32_t esTIME_GetDate(__awos_date_t *date)
{
    if (!hRTC)
    {
        struct timeval tm_val;
        struct tm tm;

        memset(&tm_val, 0, sizeof(struct timeval));
        memset(&tm, 0, sizeof(struct tm));

        gettimeofday(&tm_val, NULL);
        localtime_r(&tm_val.tv_sec, &tm);

        date->year = tm.tm_year + 1900;
        date->month = tm.tm_mon + 1;
        date->day = tm.tm_mday;

        return EPDK_OK;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_GET_DATE, 0, date);
}


/*
*********************************************************************************************************
*                                            SET RTC DATE
*
* Description: set system date
*
* Arguments  : date     pointer to date;
*
* Returns    : EPDK_OK/EPDK_FAIL
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esTIME_SetDate(__awos_date_t *date)
{
    if (!hRTC)
    {
        struct timeval tm_val;
        struct tm tm;
        clock_t fixed_time = 0;
        struct timeval sys_time = {0};

        memset(&tm_val, 0, sizeof(struct timeval));
        memset(&tm, 0, sizeof(struct tm));

        gettimeofday(&tm_val, NULL);
        localtime_r(&tm_val.tv_sec, &tm);

        tm.tm_year = date->year - 1900;
        tm.tm_mon = date->month - 1;
        tm.tm_mday = date->day;

        fixed_time = mktime(&tm);

        sys_time.tv_sec = fixed_time;
        settimeofday(&sys_time, NULL);

        return EPDK_OK;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_SET_DATE, 0, date);
}


/*
*********************************************************************************************************
*                                            REQUEST ALARM
*
* Description: request alarm from system;
*
* Arguments  : mode     type of alarm need be requested;
*
* Returns    : result, handle of alarm;
*
* Notes      :
*
*********************************************************************************************************
*/
__hdle esTIME_RequestAlarm(__u32 mode)
{
    if (!hRTC)
    {
        __wrn("rtc device handle is invalid!");
        return (__hdle)0;
    }

    return 0;//(__hdle)fioctrl(hRTC, RTC_CMD_REQUEST_ALARM, mode, 0);
}


/*
*********************************************************************************************************
*                                            RELEASE ALARM
*
* Description: release system alarm;
*
* Arguments  : alarm    handle of system alarm need be released;
*
* Returns    : result   EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esTIME_ReleaseAlarm(__hdle alarm)
{
    if (!hRTC)
    {
        __wrn("rtc device handle is invalid!");
        return EPDK_FAIL;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_RELEASE_ALARM, 0, (void *)alarm);
}


/*
*********************************************************************************************************
*                                            START ALARM
*
* Description: start system alarm;
*
* Arguments  : alarm    alarm handle;
*              time     duration of alarm comming from now;
*              mode     mode of the alarm;
*
* Returns    : result,  EPDK_OK/EPDK_FAIL.
*
* Notes      :
*********************************************************************************************************
*/
int32_t esTIME_StartAlarm(__hdle alarm, uint32_t time)
{
    if (!hRTC)
    {
        __wrn("rtc device handle is invalid!");
        return EPDK_FAIL;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_START_ALARM, time, (void *)alarm);
}


/*
*********************************************************************************************************
*                                            STOP ALARM
*
* Description: stop system alarm;
*
* Arguments  : alarm    system alarm handle;
*
* Returns    : result,  EPDK_OK/EPDK_FAIL;
*
* Notes      :
*
*********************************************************************************************************
*/
int32_t esTIME_StopAlarm(__hdle alarm)
{
    if (!hRTC)
    {
        __wrn("rtc device handle is invalid!");
        return EPDK_FAIL;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_STOP_ALARM, 0, (void *)alarm);

}
/*
*********************************************************************************************************
*                                            QUERRY ALARM
*
* Description: querry the alarm time;
*
* Arguments  : alarm    alarm handle;
*
* Returns    : time;
*
* Notes      :
*
*********************************************************************************************************
*/
uint32_t esTIME_QuerryAlarm(__hdle alarm)
{
    if (!hRTC)
    {
        __wrn("rtc device handle is invalid!");
        return 0;
    }

    return 0;//fioctrl(hRTC, RTC_CMD_QUERY_ALARM, 0, (void *)alarm);
}