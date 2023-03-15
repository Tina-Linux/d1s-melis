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
#include <string.h>
#include <stdio.h>
#include <port.h>
#include <log.h>
#include <sys_fsys.h>
#include "pwrman_script.h"
#include <sys_powerman.h>

static int32_t _kw_mode_proc(void);
static int32_t _kw_corepllhi_proc(void);
static int32_t _kw_coreplllo_proc(void);
static int32_t _kw_drampll_proc(void);
static int32_t _kw_vdd_proc(void);

static __pwrman_script_psr_t    psr_script;
static __sys_pwrman_cfg_t       *pPwmCfg;
static int32_t                  cur_pwm_idx;

const int8_t valid_str_tbl[256] =
{
    //      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
    /*0*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*1*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*2*/   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /*3*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    /*4*/   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*5*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
    /*6*/   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /*7*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

#define PWRMAN_KEYWORD_CNT            (sizeof(func_map)/sizeof(__pwrman_script_func_mapt_t))
static __pwrman_script_func_mapt_t    func_map[] =
{
    {"[MODE",           _kw_mode_proc       },
    {"core_pll_hi",     _kw_corepllhi_proc  },
    {"core_pll_lo",     _kw_coreplllo_proc  },
    {"dram_pll",        _kw_drampll_proc    },
    {"vdd",             _kw_vdd_proc        },
};


/*
*********************************************************************************************************
*                                     GET PARAMETER VALUE
*
* Description: get parameter value.
*
* Arguments  : none
*
* Returns    : parameter value;
*********************************************************************************************************
*/
static uint32_t _get_para_value(void)
{
    uint32_t    value = 0;

    //skip space or table
    while ((*(char *)psr_script.cur_ptr == '\t') || (*(char *)psr_script.cur_ptr == ' '))
    {
        psr_script.cur_ptr++;
        if (psr_script.cur_ptr >= psr_script.script_end)
        {
            return 0;
        }
    }

    //check '='
    if (*psr_script.cur_ptr != '=')
    {
        return 0;
    }
    psr_script.cur_ptr++;

    //skip space or table
    while ((*(char *)psr_script.cur_ptr == '\t') || (*(char *)psr_script.cur_ptr == ' '))
    {
        psr_script.cur_ptr++;
        if (psr_script.cur_ptr >= psr_script.script_end)
        {
            return 0;
        }
    }

    //get parameter value
    while ((*(char *)psr_script.cur_ptr >= '0') && (*(char *)psr_script.cur_ptr <= '9'))
    {
        value = value * 10 + (*psr_script.cur_ptr - '0');
        psr_script.cur_ptr ++;
    }

    return value;
}


/*
*********************************************************************************************************
*                                     KEY WORD MODE PROCESS
*
* Description: key word mode process.
*
* Arguments  : none
*
* Returns    : key word mode process result;
*               EPDK_OK     -   key word mode process successed,
*               EPDK_FAIL   -   key word mode process failed;
*
* Note       : key word mode need be layout like this'[MODEXX]'
*********************************************************************************************************
*/
static int32_t _kw_mode_proc(void)
{
    //check if the key word is valid
    if (((psr_script.string[5] >= '0') && (psr_script.string[5] <= '9'))
        && ((psr_script.string[6] >= '0') && (psr_script.string[6] <= '9'))
        && (psr_script.string[7] == ']'))
    {
        //key word of 'MODEXX' is valid, process it
        if (cur_pwm_idx >= PWRMAN_MODE_CFG_CNT)
        {
            return EPDK_FAIL;
        }

        pPwmCfg->mode[cur_pwm_idx].valid = 1;
        cur_pwm_idx ++;
    }

    return EPDK_OK;
}




/*
*********************************************************************************************************
*                                     KEY WORD COREPLLHI PROCESS
*
* Description: key word corepllhi process.
*
* Arguments  : none
*
* Returns    : key word corepllhi process result;
*               EPDK_OK     -   key word corepllhi process successed,
*               EPDK_FAIL   -   key word corepllhi process failed;
*********************************************************************************************************
*/
static int32_t _kw_corepllhi_proc(void)
{
    if ((cur_pwm_idx < 0) || (cur_pwm_idx >= PWRMAN_MODE_CFG_CNT))
    {
        return EPDK_FAIL;
    }

    pPwmCfg->mode[cur_pwm_idx].core_pll_hi = _get_para_value();

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                     KEY WORD COREPLLLO PROCESS
*
* Description: key word coreplllo process.
*
* Arguments  : none
*
* Returns    : key word coreplllo process result;
*               EPDK_OK     -   key word coreplllo process successed,
*               EPDK_FAIL   -   key word coreplllo process failed;
*********************************************************************************************************
*/
static int32_t _kw_coreplllo_proc(void)
{
    if ((cur_pwm_idx < 0) || (cur_pwm_idx >= PWRMAN_MODE_CFG_CNT))
    {
        return EPDK_FAIL;
    }

    pPwmCfg->mode[cur_pwm_idx].core_pll_lo = _get_para_value();

    return EPDK_OK;
}



/*
*********************************************************************************************************
*                                     KEY WORD DRAMPLL PROCESS
*
* Description: key word drampll process.
*
* Arguments  : none
*
* Returns    : key word drampll process result;
*               EPDK_OK     -   key word drampll process successed,
*               EPDK_FAIL   -   key word drampll process failed;
*********************************************************************************************************
*/
static int32_t _kw_drampll_proc(void)
{
    if ((cur_pwm_idx < 0) || (cur_pwm_idx >= PWRMAN_MODE_CFG_CNT))
    {
        return EPDK_FAIL;
    }

    pPwmCfg->mode[cur_pwm_idx].dram_pll = _get_para_value();

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     KEY WORD VDD PROCESS
*
* Description: key word vdd process.
*
* Arguments  : none
*
* Returns    : key word vdd process result;
*               EPDK_OK     -   key word vdd process successed,
*               EPDK_FAIL   -   key word vdd process failed;
*********************************************************************************************************
*/
static int32_t _kw_vdd_proc(void)
{
    if ((cur_pwm_idx < 0) || (cur_pwm_idx >= PWRMAN_MODE_CFG_CNT))
    {
        return EPDK_FAIL;
    }

    pPwmCfg->mode[cur_pwm_idx].vdd = _get_para_value();

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     KEY WORD MODE PROCESS
*
* Description: key word mode process.
*
* Arguments  : none
*
* Returns    : key word mode process result;
*               EPDK_OK     -   key word mode process successed,
*               EPDK_FAIL   -   key word mode process failed;
*
* Note       : key word mode need be layout like this'[MODEXX]'
*********************************************************************************************************
*/
static int32_t skip_semicolon_comment_line(void)
{
    //skip semicolon ; comment text
    while ((*(char *)psr_script.cur_ptr != NEWLINE) && (*(char *)psr_script.cur_ptr != ENTER))
    {
        psr_script.cur_ptr++;
        if (psr_script.cur_ptr >= psr_script.script_end)
        {
            return SCRIPT_PARSE_ERR_EOF;
        }
    }

    //skip blank lines
    while ((*(char *)psr_script.cur_ptr == NEWLINE) || (*(char *)psr_script.cur_ptr == ENTER))
    {
        psr_script.cur_ptr++;
        if (psr_script.cur_ptr >= psr_script.script_end)
        {
            return SCRIPT_PARSE_ERR_EOF;
        }
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     PWM SCRIPT INIT
*
* Description: pwm script init.
*
* Arguments  : none
*
* Returns    : pwm script init result;
*               EPDK_OK     -   pwm script init successed,
*               EPDK_FAIL   -   pwm script init failed;
*********************************************************************************************************
*/
static int32_t pwrmanScriptInit(void)
{
    __hdle      tmpFp;
    __hdle      tmpFpd = 0;
    __hdle      tmpFpf = 0;
    uint32_t    ret;
    int32_t     flag;
    long long   tmpSize;

    memset(&psr_script, 0, sizeof(__pwrman_script_psr_t));

    //open pwm script file
    tmpFpd  = fopen("/data/pwrman_cfg.ini", "rb");
    tmpFpf  = fopen("/mnt/f/rootfs/pwrman_cfg.ini", "rb");
    tmpFp   = tmpFpd ? tmpFpd : tmpFpf;

    fseek(tmpFp, 0, SEEK_END);
    tmpSize = ftell(tmpFp);
    if (!tmpSize)
    {
        __log("pwm script file is invalid!");
    }
    fseek(tmpFp, 0, SEEK_SET);
    psr_script.buf_size     = (tmpSize + 1023) / 1024;
    psr_script.buf_start    = k_malloc(psr_script.buf_size * 1024);
    if (!psr_script.buf_start)
    {
        __log("Request memory for read pwm script file failed!");
    }

    if (fread(psr_script.buf_start, 1, tmpSize, tmpFp) != tmpSize)
    {
        __log("Read pwm script file failed!");
    }
    psr_script.script_end   = psr_script.buf_start + tmpSize;
    psr_script.cur_ptr      = psr_script.buf_start;

    //close pwm script
    fclose((__hdle)tmpFp);

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     PWM SCRIPT EXIT
*
* Description: pwm script exit.
*
* Arguments  : none
*
* Returns    : pwm script exit result;
*               EPDK_OK     -   pwm script exit successed,
*               EPDK_FAIL   -   pwm script exit failed;
*********************************************************************************************************
*/
static int32_t pwrmanScriptExit(void)
{
    if (psr_script.buf_start && psr_script.buf_size)
    {
        k_free(psr_script.buf_start);
        psr_script.buf_start = 0;
        psr_script.buf_size = 0;
    }

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                     PWM SCRIPT INIT
*
* Description: pwm script init.
*
* Arguments  : none
*
* Returns    : pwm script init result;
*               EPDK_OK     -   pwm script init successed,
*               EPDK_FAIL   -   pwm script init failed;
*********************************************************************************************************
*/
static int32_t getOneLineScriptString(void)
{
    uint8_t *tmpStr    = psr_script.string;

    //skip remark lines
    while (*psr_script.cur_ptr == REMARK)
    {
        if (skip_semicolon_comment_line() != EPDK_OK)
        {
            return EPDK_FAIL;
        }
    }

    //skip blank
    while ((*(char *)psr_script.cur_ptr == '\t') || (*(char *)psr_script.cur_ptr == ' '))
    {
        psr_script.cur_ptr++;
        if (psr_script.cur_ptr >= psr_script.script_end)
        {
            return SCRIPT_PARSE_ERR_EOF;
        }
    }

    //get string
    while (valid_str_tbl[*(__s8 *)psr_script.cur_ptr])
    {
        *tmpStr++ = *psr_script.cur_ptr;
        psr_script.cur_ptr++;
        if (psr_script.cur_ptr >= psr_script.script_end)
        {
            return SCRIPT_PARSE_ERR_EOF;
        }
    }

    *tmpStr = '\0';

    return EPDK_OK;
}



/*
*********************************************************************************************************
*                                     PROCESS PWM SCRIPT STRING
*
* Description: process pwm script string.
*
* Arguments  : none
*
* Returns    : process pwm string result;
*               EPDK_OK     -   process pwm script string successed,
*               EPDK_FAIL   -   process pwm script string failed;
*********************************************************************************************************
*/
static int32_t processScriptString(void)
{
    int32_t     i;

    for (i = 0; i < PWRMAN_KEYWORD_CNT; i++)
    {
        if (strncmp(func_map[i].keystr, (const char *)psr_script.string, strlen(func_map[i].keystr)) == 0)
        {
            __msg("Find a keyword!");
            func_map[i].func();
            break;
        }
    }

    return skip_semicolon_comment_line();
}


/*
*********************************************************************************************************
*                                     INITIATE PWM PARAMETER WITH SCRIPT
*
* Description: initiate pwm parameter with script.
*
* Arguments  : none
*
* Returns    : initiate pwm parameter result;
*               EPDK_OK     -   initiate pwm parameter successed,
*               EPDK_FAIL   -   initiate pwm parameter failed;
*********************************************************************************************************
*/
int32_t initPwrmanIniCfg(__sys_pwrman_cfg_t *pSysPwrmanCfg)
{
    int32_t     result;

    cur_pwm_idx = 0;
    pPwmCfg     = pSysPwrmanCfg;

    //init script parser
    if (pwrmanScriptInit() != EPDK_OK)
    {
        __log("Pwm script parser init failed!");
    }

    //parser script
    do
    {
        //get script string
        result = getOneLineScriptString();
        if (result != EPDK_OK)
        {
            if (result == SCRIPT_PARSE_ERR_EOF)
            {
                break;
            }

            __log("get script string failed!");
        }

        //process script parameter
        result = processScriptString();
        if (result != EPDK_OK)
        {
            __wrn("process pwm script finished!");
            break;
        }

    } while (1);

    pwrmanScriptExit();

    return EPDK_OK;
}
