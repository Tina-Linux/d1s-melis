/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : adjust.c
* By      :
* Version : V1.00
*********************************************************************************************************
*/
#include <log.h>
#include <emodules/mod_touchpanel.h>
#include "../mod_init_i.h"


/*
**********************************************************************************************************************
*                                            tpdrv_open
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

int tpdrv_open(void)
{
    /*
    ES_FILE  *hdle = NULL;
    hdle = eLIBs_fopen("b:\\input\\TP", "r+");

    if (!hdle)
    {
        __wrn("open tp dev file fail\n");
        return NULL;
    }
*/
    int hdle;
    hdle = open("/dev/tpadc_rtp", O_WRONLY);

    return hdle;
}

/*
**********************************************************************************************************************
*                                            tpdrv_close
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 tpdrv_close(int hdle)
{
    if (hdle)
    {
        close(hdle);
    }

    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                            tp_adjust
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

__s32 tp_adjust(int hdle, __u8 cnt, void *pbuf)
{
    __s32   ret;
    __u64 args[4];
    args[0] = cnt;
    args[1] = pbuf;
    ret = ioctl(hdle, 0, &args);
    return ret;
    //return EPDK_FAIL;
}


