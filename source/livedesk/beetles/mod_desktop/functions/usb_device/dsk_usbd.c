/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dsk_usbd.c
* By        : Andy.zhang
* Func      : usbd op
* Version   : v1.0
* ============================================================================================================
* 2010-1-8 9:46:40  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include "functions_i.h"

void dsk_usbd_remove(void)
{
__wrn("need_fix");
#if 0 /*langaojie temp test*/
    ES_FILE     *usbm_dev_hadle = NULL;
    __s32       ret = 0;

    usbm_dev_hadle = eLIBs_fopen("b:\\HWSC\\hwsc", "r");

    if (usbm_dev_hadle == NULL)
    {
        __wrn("ERR: eLIBs_fopen failed\n");
        return ;
    }

    ret = esMODS_MIoctrl(usbm_dev_hadle, DEV_IOC_USR_HWSC_APP_RMMOD_USBD, 0, NULL);

    if (ret == EPDK_FAIL)
    {
        __inf("ERR: execute DEV_IOC_USR_HWSC_APP_RMMOD_USBD failed\n");
        eLIBs_fclose(usbm_dev_hadle);
        return ;
    }

    eLIBs_fclose(usbm_dev_hadle);
#endif
}
