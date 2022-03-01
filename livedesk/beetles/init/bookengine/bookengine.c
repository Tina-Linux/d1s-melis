/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                 (c) Copyright 2007-2010, Jackie, China
*                                       All Rights Reserved
*
* File      : bookengine.c
* By        : Jackie.chen
* Func      : bookengine
* Version   : v1.0
* ======================================================================================================================
* 2011-03-08  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/

#include "mod_init_i.h"
#include <log.h>

static __u8                bookengine_mid = 0;        // mod id
static __mp               *bookengine_mod = NULL;               // mod¾ä±ú

__s32 bookengine_open(void)
{
    /*already open*/
    if (bookengine_mod)
    {
        return 0;
    }

    if (0== bookengine_mid)
    {
        /* install bookengine module */
        bookengine_mid = esMODS_MInstall(BEETLES_APP_ROOT"mod\\bookengine.mod", 0);

        if (!bookengine_mid)
        {
            __msg("Install bookengine module failed");
            return -1;
        }

        __msg("install bookengine OK");
    }

    if (NULL == bookengine_mod)
    {
        /* open bookengine module */
        bookengine_mod = esMODS_MOpen(bookengine_mid, 0);

        if (bookengine_mod == (__mp *)0)
        {
            if (bookengine_mid)
            {
                esMODS_MUninstall(bookengine_mid);
                bookengine_mid  = 0;
            }

            __msg("esMODS_MOpen failed");
            return -1;
        }

        __msg("bookengine module open OK");
    }

    return 0;
}

__s32 bookengine_close(void)
{
    if (bookengine_mod)
    {
        esMODS_MClose(bookengine_mod);
    }

    if (bookengine_mid)
    {
        esMODS_MUninstall(bookengine_mid);
    }

    bookengine_mod = NULL;
    bookengine_mid = 0;
    return 0;
}
