/*
*********************************************************************************************************
*                                                   ePOS
*                                  the Easy Portable/Player Operation System
*                                              exec sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                            All Rights Reserved
*
* File    : exec.c
* By      : Steven.ZGJ
* Version : V1.00
*********************************************************************************************************
*/

#include <typedef.h>
#include <stdio.h>
#include <rtdef.h>
#include <rtthread.h>
#include "log.h"
#include "ktype.h"
#include "kapi.h"

/*
*********************************************************************************************************
*                                       RESTHEM OPEN
*
* Description: open a resthem file.
*
* Arguments  : file     : the path of open resthem file,
*              pMode    : the mode of open file.
*
* Returns    : the handler of resthem, NULL if failed.
*
* Note       :
*********************************************************************************************************
*/
void *esRESM_ROpen(const char *file, const char *mode)
{
    unsigned char   res_head[16] = {0};
    __resm_rsb_t    *hRes;

    if (file == RT_NULL)
    {
        __err("invalid file path for resthem open.");
        return RT_NULL;
    }

    //allocate resouce manager handler
    hRes = (__resm_rsb_t *)rt_malloc(sizeof(__resm_rsb_t));
    if (hRes == (__resm_rsb_t *)0)
    {
        __err("allocate buffer for res manager failed.");
        return RT_NULL;
    }

    // open res file
    hRes->hResfile = esFSYS_fopen(file, "r");
    if (hRes->hResfile == 0)
    {
        __err("cannot open file %s.", file);
        rt_free((void *)hRes);
        return RT_NULL;
    }
    //--------------------------------------------------------------------------
    //check if the file is a valid res file
    //--------------------------------------------------------------------------
    esFSYS_fread(res_head, 16, 1, hRes->hResfile);
    //check if the file contain valid res magic
    if (!rt_strncmp((char *)res_head, ".resthem", 8) || !rt_strncmp((char *)res_head, ".langdat", 8))
    {
        //current file is a valid res file, set some prameter, and return
        esFSYS_fseek(hRes->hResfile, 0, SEEK_END);
        hRes->size = esFSYS_ftell(hRes->hResfile);
        hRes->offset = 0;
        esFSYS_fseek(hRes->hResfile, 0, SEEK_SET);

        if (hRes->size == 0)
        {
            __err("res file size is zero.");
        }
        return hRes;
    }

    __err("invalid resthem file [%s] access.", file);
    esFSYS_fclose(hRes->hResfile);
    rt_free((void *)hRes);

    return RT_NULL;
}

/*
*********************************************************************************************************
*                                       RESTHEM CLOSE
*
* Description: close a resthem file.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esRESM_RClose(__resm_rsb_t *res)
{
    if (!res || !res->hResfile)
    {
        __err("close res file failure.");
        return 0;
    }
    esFSYS_fclose(res->hResfile);
    rt_free((void *)res);
    return 0;
}

/*
*********************************************************************************************************
*                                       RESTHEM READ
*
* Description: read resthem file.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
uint32_t esRESM_RRead(void *pdata, uint32_t size, uint32_t n, __resm_rsb_t *res)
{
    if (!pdata || !res || !res->hResfile)
    {
        __err("read res file failure.");
        return 0;
    }
    return esFSYS_fread(pdata, size, n, res->hResfile);
}

/*
*********************************************************************************************************
*                                       RESTHEM SEEK
*
* Description: seek resthem file.
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esRESM_RSeek(__resm_rsb_t *res, int32_t Offset, int32_t Whence)
{
    if (!res || !res->hResfile)
    {
        __err("seek res file failure.");
        return 0;
    }
    return esFSYS_fseek(res->hResfile, Offset + res->offset, Whence);
}


