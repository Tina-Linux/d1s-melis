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
#include "fsys_debug.h"
#include "fs_timer.h"

#if     FSYS_DEBUG_ON

__hdle timer_hdle = NULL;

/*--------------------------- debug ------------------------*/
static struct timer_cfg_s timer_cfg = {0};
static timers_t     timers =
{
    12,
    {
        {diodevicetime,    0, 0, "dio time"},
        {bufdevicetime,    0, 0, "bio time"},
        {getblktime,       0, 0, "get buf time"},
        {hashtime,         0, 0, "dio hash time"},
        {memcpytime,       0, 0, "mem copy time"},
        {bmaptime,         0, 0, "bmap time"},
        {fatcachetime,     0, 0, "fat cache time"},
        {scanfattime,      0, 0, "fat table time"},
        {oneaccesstime,    0, 0, "one access time"},
        {shrinkcachetime,  0, 0, "shrink cache time"},
        {partiotime,       0, 0, "part io time"},
        {invalidtime,      0, 0, "unused item"},
    }
};

void debug_timerclr(ttype_t target)
{
    int i;

    if (!timer_cfg.on || !timer_hdle)
    {
        return ;
    }

    esTIME_PauseCntr(timer_hdle);
    if (target == alltime)
    {
        for (i = 0; i < timers.count; i++)
        {
            timers.item[i].value = 0;
        }
    }
    else
    {
        for (i = 0; i < timers.count; i++)
        {
            if (timers.item[i].type == target)
            {
                timers.item[i].value = 0;
                break;
            }
        }
        if (i == timers.count)
        {
            __inf("invalid fs debug timer: %d, please check it.", target);
        }
    }
    esTIME_ContiCntr(timer_hdle);
}
static int _isdigit(char ch)
{
	return ((ch >= 0x30) && (ch <= 0x39)) ? 1 : 0;
}

static int __strtol(const char *nptr, char **endptr, int base)
{

   const char *p = nptr;
   long ret;
   char ch;
   unsigned long Overflow;  //溢出
   int sign = 0, flag;


   if(base != 10){
	   __err("PANIC: my_strtol: only support decimalist\n");
	   return 0;
   }

   /*跳过前面多余的空格，并判断正负符号。*/
   do{
	  ch = *p++;
   } while (ch == ' ');

   if (ch == '-'){
	  sign = 1;
	  ch = *p++;
   }

   Overflow = sign ? (unsigned long)(-LONG_MIN) : LONG_MAX;

   for (ret = 0, flag = 0;; ch = *p++)
   {
	  /*把当前字符转换为相应运算中需要的值。*/
	   if (_isdigit(ch)){
		   ch -= '0';
		   ret *= base;
		   ret += ch;
	   }else
		   break;

	   /*如果产生溢出，则置标志位，以后不再做计算。*/
	   if (ret > Overflow){
		   flag = -1;
	   }
   }

   /* 如果溢出，则返回相应的Overflow的峰值。没有溢出，如是符号位为负，则转换为负数。*/
   if (flag < 0){
	  ret = sign ? LONG_MIN : LONG_MAX;
   }else if (sign){
	  ret = -ret;
   }

   /*如字符串不为空，则*endptr等于指向nptr结束符的指针值；否则*endptr等于nptr的首地址。*/
   if (endptr != NULL){
	  *endptr = (char *)(flag ? (p - 1) : nptr);
   }

   return ret;

}

void debug_timerstart(ttype_t target)
{
    int i;
    __u32 temptime;

    if (!timer_cfg.on || !timer_hdle)
    {
        return ;
    }

    temptime = esTIME_QuerryCntr(timer_hdle) << 1;
    esTIME_PauseCntr(timer_hdle);
    for (i = 0; i < timers.count && target != alltime; i++)
    {
        if (timers.item[i].type == target)
        {
            timers.item[i].start = temptime;
            break;
        }
    }
    if (i == timers.count || target == alltime)
    {
        __inf("invalid fs debug timer: %d, please check it.", target);
    }
    esTIME_ContiCntr(timer_hdle);
}

__u32 debug_timerend(ttype_t target)
{
    int i;
    __u32 temptime;
    __u32 ret = 0;

    if (!timer_cfg.on || !timer_hdle)
    {
        return 0;
    }

    temptime = esTIME_QuerryCntr(timer_hdle) << 1;
    esTIME_PauseCntr(timer_hdle);

    for (i = 0; i < timers.count && target != alltime; i++)
    {
        if (timers.item[i].type == target)
        {
            timers.item[i].value += (temptime - timers.item[i].start);
            ret = timers.item[i].value;
            break;
        }
    }
    if (i == timers.count || target == alltime)
    {
        __inf("invalid fs debug timer: %d, please check it.", target);
    }
    esTIME_ContiCntr(timer_hdle);

    return ret;
}

void debug_timershow(ttype_t target)
{
    int i;

    if (!timer_cfg.on || !timer_hdle)
    {
        return;
    }

    esTIME_PauseCntr(timer_hdle);
    if (target == alltime)
    {
        for (i = 0; i < timers.count; i++)
        {
            __inf("[%s", timers.item[i].info);
            __inf(": %d]", timers.item[i].value);
        }
    }
    else
    {
        for (i = 0; i < timers.count; i++)
        {
            if (timers.item[i].type == target)
            {
                __inf("[%s", timers.item[i].info);
                __inf(": %d]", timers.item[i].value);
                break;
            }
        }
        if (i == timers.count)
        {
            __inf("invalid fs debug timer: %d, please check it.", target);
        }
    }
    esTIME_ContiCntr(timer_hdle);
}

static char printbuf[128];
void debug_timerprint(char *fmt, ...)
{
    va_list args;

    if (!timer_cfg.on || !timer_hdle)
    {
        return;
    }

    esTIME_PauseCntr(timer_hdle);

    va_start(args, fmt);
    vsprintf(printbuf, fmt, args);
    va_end(args);
    __inf("%s", printbuf);

    esTIME_ContiCntr(timer_hdle);
}

BOOL fs_timer_parse_option(const char *option)
{
    if (strcmp(option, "request") == 0 || strcmp(option, "REQUEST") == 0)
    {
        timer_hdle = esTIME_RequestCntr(NULL, "FS");
        if (!timer_hdle)
        {
            __inf("Request counter fail");
            return FALSE;
        }
        /* set timer prescale, refrence timer frequency. */
        esTIME_SetCntrPrescale(timer_hdle, 135);
        esTIME_SetCntrValue(timer_hdle, 0);
        esTIME_StartCntr(timer_hdle);
        return TRUE;
    }
    else if (strncmp(option, "0x", 2) == 0 || strncmp(option, "0X", 2) == 0)
    {
        timer_hdle = __strtol(option, NULL, 0);
        __inf("timer handle: %x", timer_hdle);
        return TRUE;
    }
    else if (strcmp(option, "release") == 0 || strcmp(option, "RELEASE") == 0)
    {
        if (timer_hdle)
        {
            esTIME_ReleaseCntr(timer_hdle);
            timer_hdle = NULL;
            return TRUE;
        }
        __inf("no timer to release!");
        return FALSE;
    }
    else if (strcmp(option, "on") == 0 || strcmp(option, "ON") == 0)
    {
        timer_cfg.on = 1;
        return TRUE;
    }
    else if (strcmp(option, "off") == 0 || strcmp(option, "OFF") == 0)
    {
        timer_cfg.on = 0;
        return TRUE;
    }
    __inf("Unknown fs timer option '%s'", option);
    return FALSE;
}

#endif  /* FSYS_DEBUG_ON */