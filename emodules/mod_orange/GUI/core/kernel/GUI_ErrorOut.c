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
*      Static code
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
**********************************************************************
*/
/*********************************************************************
*
*       GUI_ErrorOut
*/
void GUI_ErrorOut(const char *s)
{
    GUI_X_ErrorOut(s);
}

/*********************************************************************
*
*       GUI_ErrorOut1
*/
void GUI_ErrorOut1(const char *s, int p0)
{
    char ac[MAXLEN + 10];
    char *sOut = ac;
    GUI_MEMSET((U8 *)ac, 0, sizeof(ac));
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI_ErrorOut(ac);
}

/*********************************************************************
*
*       GUI_ErrorOut2
*/
void GUI_ErrorOut2(const char *s, int p0, int p1)
{
    char ac[MAXLEN + 20];
    char *sOut = ac;
    GUI_MEMSET((U8 *)ac, 0, sizeof(ac));
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI__AddSpaceHex(p1, 8, &sOut);
    GUI_ErrorOut(ac);
}

/*********************************************************************
*
*       GUI_ErrorOut3
*/
void GUI_ErrorOut3(const char *s, int p0, int p1, int p2)
{
    char ac[MAXLEN + 30];
    char *sOut = ac;
    GUI_MEMSET((U8 *)ac, 0, sizeof(ac));
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI__AddSpaceHex(p1, 8, &sOut);
    GUI__AddSpaceHex(p2, 8, &sOut);
    GUI_ErrorOut(ac);
}

/*********************************************************************
*
*       GUI_ErrorOut4
*/
void GUI_ErrorOut4(const char *s, int p0, int p1, int p2, int p3)
{
    char ac[MAXLEN + 40] = {0};
    char *sOut = ac;
    GUI_MEMSET((U8 *)ac, 0, sizeof(ac));
    _CopyString(ac, s, MAXLEN);
    sOut += strlen(sOut);
    GUI__AddSpaceHex(p0, 8, &sOut);
    GUI__AddSpaceHex(p1, 8, &sOut);
    GUI__AddSpaceHex(p2, 8, &sOut);
    GUI__AddSpaceHex(p3, 8, &sOut);
    GUI_ErrorOut(ac);
}

/*************************** End of file ****************************/
