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
* File    : GUI__ReadData.c
* Purpose : Implementation of reading data
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Exported code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__Read16
*/
U16 GUI__Read16(const U8 **ppData)
{
    const U8 *pData;
    U16  Value;
    pData = *ppData;
    //__log("ppdata = 0x%p, pData = 0x%p.", ppData, pData);
    Value = *pData;
    Value |= (U16)(*(pData + 1) << 8);
    pData += 2;
    *ppData = pData;
    return Value;
}

/*********************************************************************
*
*       GUI__Read32
*/
U32 GUI__Read32(const U8 **ppData)
{
    const U8 *pData;
    U32  Value;
    pData = *ppData;
    Value = *pData;
    Value |= (*(pData + 1) << 8);
    Value |= ((U32) * (pData + 2) << 16);
    Value |= ((U32) * (pData + 3) << 24);
    pData += 4;
    *ppData = pData;
    return Value;
}

