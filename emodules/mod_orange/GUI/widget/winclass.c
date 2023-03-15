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
#ifndef __WINCLASS_C__
#define __WINCLASS_C__

#include "winclass_i.h"

#define LEN_CCITABLE    26

pctlclassinfo ccitable[LEN_CCITABLE];

/*
**********************************************************************************************************************
*                                               GUI_InitControlClass
*
* author:
*
* date:             2009-8-6
*
* Description:      gui initcontrolclass
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool GUI_InitControlClass()
{
    int i;

    for (i = 0; i < LEN_CCITABLE; i++)
    {
        ccitable[i] = NULL;
    }

    return ORANGE_TRUE;
}

/*
**********************************************************************************************************************
*                                               GUI_TerminateControlClass
*
* author:
*
* date:             2009-8-6
*
* Description:      gui terminatecontrolclass
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_TerminateControlClass()
{
    GUI_EmptyControlClassInfoTable();
}

/*
**********************************************************************************************************************
*                                               GUI_GetControlClassInfo
*
* author:
*
* date:             2009-7-31
*
* Description:      gui getcontrolclassinfo
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

pctlclassinfo GUI_GetControlClassInfo(const char *szClassName)
{
    pctlclassinfo   cci;
    cci = ccitable[0];

    while (cci)
    {
        if (orange_strcmp(cci->name, szClassName) == 0)
        {
            break;
        }

        cci = cci->next;
    }

    return cci;
}

/*
**********************************************************************************************************************
*                                               GUI_ControlClassDataOp
*
* author:
*
* date:             2009-7-31
*
* Description:      gui controlclassdataop
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SetControlClassData(__gui_winclass_t *pWndClass)
{
    pctlclassinfo cci;
    cci = GUI_GetControlClassInfo(pWndClass->spClassName);

    if (!cci)
    {
        return ORANGE_FAIL;
    }

    pWndClass->dwStyle      = cci->dwStyle;
    pWndClass->dwExStyle    = cci->dwExStyle;
    pWndClass->WinProc      = cci->ControlProc;
    pWndClass->dwAddData    = cci->dwAddData;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_ControlClassDataOp
*
* author:
*
* date:             2009-7-31
*
* Description:      gui controlclassdataop
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_GetControlClassData(__gui_winclass_t *pWndClass)
{
    pctlclassinfo cci;
    cci = GUI_GetControlClassInfo(pWndClass->spClassName);

    if (!cci)
    {
        return ORANGE_FAIL;
    }

    cci->dwStyle            = pWndClass->dwStyle;
    cci->dwExStyle          = pWndClass->dwExStyle;
    cci->ControlProc        = pWndClass->WinProc;
    cci->dwAddData          = pWndClass->dwAddData;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_GetCtrlClassAddData
*
* author:
*
* date:             2009-7-31
*
* Description:      gui getctrlclassadddata
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_GetCtrlClassAddData(const char *szClassName, __u32 *pAddData)
{
    pctlclassinfo cci;
    cci = GUI_GetControlClassInfo(szClassName);

    if (cci)
    {
        *pAddData = cci->dwAddData;
        return ORANGE_OK;
    }

    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               GUI_SetCtrlClassAddData
*
* author:
*
* date:             2009-7-31
*
* Description:      gui setctrlclassadddata
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_SetCtrlClassAddData(const char *szClassName, __u32 dwAddData)
{
    pctlclassinfo cci;
    cci = GUI_GetControlClassInfo(szClassName);

    if (cci)
    {
        cci->dwAddData = dwAddData;
        return ORANGE_OK;
    }

    return ORANGE_FAIL;
}

/*
**********************************************************************************************************************
*                                               GUI_AddNewControlClass
*
* author:
*
* date:             2009-7-31
*
* Description:      gui addnewcontrolclass
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_AddNewControlClass(pwinclass pWndClass)
{
    __gui_ctlclass_info_t  *cci, *newcci;
    char                    szClassName [GUI_CLASSNAME_MAX + 2];
    orange_strncpy(szClassName, pWndClass->spClassName, GUI_CLASSNAME_MAX + 1);
    newcci = (__gui_ctlclass_info_t *)orange_malloc(sizeof(__gui_ctlclass_info_t));

    if (newcci == NULL)
    {
        ORANGE_WRN("malloc memory failed!\n");
        return ORANGE_FAIL;
    }

    newcci->next        = NULL;
    orange_strcpy(newcci->name, szClassName);
    newcci->dwStyle     = pWndClass->dwStyle;
    newcci->dwExStyle   = pWndClass->dwExStyle;
    newcci->ControlProc = pWndClass->WinProc;
    newcci->dwAddData   = pWndClass->dwAddData;
    newcci->nUseCount   = 0;
    cci = ccitable[0];

    if (cci)
    {
        while (cci->next)
        {
            cci = cci->next;
        }

        cci->next = newcci;
    }
    else
    {
        ccitable[0] = newcci;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_DeleteControlClass
*
* author:
*
* date:             2009-7-31
*
* Description:      gui deletecontrolclass
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_DeleteControlClass(const char *szClassName)
{
    __gui_ctlclass_info_t *head, *cci, *prev;
    __s32                  i = 0;
    char szName [GUI_CLASSNAME_MAX + 1];

    if (szClassName == NULL)
    {
        ORANGE_WRN("class name can not be null@\n");
        return ORANGE_FAIL;
    }

    orange_strncpy(szName, szClassName, GUI_CLASSNAME_MAX);
    head = ccitable [0];
    cci = head;
    prev = head;

    while (cci)
    {
        if (orange_strcmp(cci->name, szName) == 0)
        {
            break;
        }

        prev = cci;
        cci = cci->next;
    }

    if (!cci)
    {
        return ORANGE_FAIL;
    }

    if (cci->nUseCount != 0)
    {
        return ORANGE_FAIL;
    }

    if (cci == head)
    {
        ccitable [i] = cci->next;
        orange_mfree(cci);
    }
    else
    {
        prev->next = cci->next;
        orange_mfree(cci);
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_EmptyControlClassInfoTable
*
* author:
*
* date:             2009-7-31
*
* Description:      gui emptycontrolclassinfotable
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void GUI_EmptyControlClassInfoTable()
{
    __gui_ctlclass_info_t *cci, *temp;
    int i;

    for (i = 0; i < LEN_CCITABLE; i++)
    {
        cci = ccitable [i];

        while (cci)
        {
            temp = cci->next;
            orange_mfree(cci);
            cci = temp;
        }
    }
}

#endif
