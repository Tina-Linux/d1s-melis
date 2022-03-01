/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_WaitKey.C
* Purpose : Implementation of GUI_WaitKey
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_WaitKey

int GUI_WaitKey(void) {
    int r;

    do
    {
       r = GUI_GetKey();
        if (r)
        {
            break;
        }
        if (!GUI_Exec())
        {
        GUI_X_WAIT_EVENT();      // Wait for event (keyboard, mouse or whatever)
        }
    }while (1);

    return r;
}
*/
#define DRV_KEY_CMD_QRYKEY 1
//extern ES_FILE * keyfile;
int keyfile = -1;

int GUI_WaitKey(void)
{
    int r;

    do
    {
        r = ioctl(keyfile, DRV_KEY_CMD_QRYKEY, 0);

        if (r)
        {
            break;
        }

        GUI_X_WAIT_EVENT();      /* Wait for event (keyboard, mouse or whatever) */
    } while (1);

    return r;
}

/*************************** End of file ****************************/
