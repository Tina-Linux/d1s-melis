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
* File    : GUI_Warn.c
* Purpose : Logging (used only at higher debug levels)
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include <string.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define MAXLEN 50

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _CopyString
*/
static void _CopyString(char *d, const char *s, int MaxLen)
{
    while ((MaxLen > 0) && *s)
    {
        *d++ = *s++;
        MaxLen--;
    }

    *d = 0;
}

/*********************************************************************
*
*       Public code
*
*  Note: These routines are needed only in higher debug levels.
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_Warn
*/
void GUI_Warn(const char *s)
{
    GUI_X_Warn(s);
}

/*********************************************************************
*
*       GUI_Warn1
*/
void GUI_Warn1(const char *s, int p0)
{
    char ac[MAXLEN + 10];
    char *sOut = ac;
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI_Warn(ac);
}

/*********************************************************************
*
*       GUI_Warn2
*/
void GUI_Warn2(const char *s, int p0, int p1)
{
    char ac[MAXLEN + 20];
    char *sOut = ac;
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI__AddSpaceHex(p1, 8, &sOut);
    GUI_Warn(ac);
}

/*********************************************************************
*
*       GUI_Warn3
*/
void GUI_Warn3(const char *s, int p0, int p1, int p2)
{
    char ac[MAXLEN + 30];
    char *sOut = ac;
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI__AddSpaceHex(p1, 8, &sOut);
    GUI__AddSpaceHex(p2, 8, &sOut);
    GUI_Warn(ac);
}

/*********************************************************************
*
*       GUI_Warn4
*/
void GUI_Warn4(const char *s, int p0, int p1, int p2, int p3)
{
    char ac[MAXLEN + 40];
    char *sOut = ac;
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI__AddSpaceHex(p1, 8, &sOut);
    GUI__AddSpaceHex(p2, 8, &sOut);
    GUI__AddSpaceHex(p3, 8, &sOut);
    GUI_Warn(ac);
}

/*************************** End of file ****************************/
