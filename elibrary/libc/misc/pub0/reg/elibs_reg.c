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
#include "elibs_reg_private.h"

/*
**********************************************************************************************************************
*             Local Variables
**********************************************************************************************************************
*/


/*
**********************************************************************************************************************
*             Global functions
**********************************************************************************************************************
*/
/*
**********************************************************************************************************************
*                                               eLIBs_RegOpenNode
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
__hdle eLIBs_RegOpenNode(const char *node, __s32 mode)
{
    return esSVC_RegOpenNode(node, mode);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegCloseNode
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
__s32  eLIBs_RegCloseNode(__hdle handle)
{
    return esSVC_RegCloseNode(handle);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegDelNode
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
__s32  eLIBs_RegDeleteNode(const  char *node)
{
    return esSVC_RegDeleteNode(node);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetSetCount
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
__s32  eLIBs_RegGetSetCount(__hdle handle, __u32 *count_p)
{
    return esSVC_RegGetSetCount(handle, count_p);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetFirstSet
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
__s32  eLIBs_RegGetFirstSet(__hdle handle)
{
    return esSVC_RegGetFirstSet(handle);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetNextSet
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
__s32  eLIBs_RegGetNextSet(__hdle handle, char *set_name)
{
    return esSVC_RegGetNextSet(handle, set_name);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegCreateSet
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
__s32  eLIBs_RegCreateSet(__hdle handle, const char *set_name)
{
    return esSVC_RegCreateSet(handle, set_name);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegDeleteSet
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
__s32  eLIBs_RegDeleteSet(__hdle handle, const char *set_name)
{
    return esSVC_RegDeleteSet(handle, set_name);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetKeyCount
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
__s32  eLIBs_RegGetKeyCount(__hdle handle, const char *set_name, __u32 *count_p)
{
    return esSVC_RegGetKeyCount(handle, set_name, count_p);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetFirstKey
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
__s32  eLIBs_RegGetFirstKey(__hdle handle, const char *set_name)
{
    return esSVC_RegGetFirstKey(handle, set_name);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetNextKey
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
__s32  eLIBs_RegGetNextKey(__hdle handle, const char *set_name, char *key_name)
{
    return esSVC_RegGetNextKey(handle, set_name, key_name);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegCreateKey
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
__s32  eLIBs_RegCreateKey(__hdle handle, const char *set_name, const char *key_name, const char *key_value)
{
    return esSVC_RegCreateKey(handle, set_name, key_name, key_value);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegDeleteKey
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
__s32  eLIBs_RegDeleteKey(__hdle handle, const char *set_name, const char *key_name)
{
    return esSVC_RegDeleteKey(handle, set_name, key_name);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegSetKeyValue
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
__s32  eLIBs_RegSetKeyValue(__hdle handle, const char *set_name, const char *key_name, const char *key_value)
{
    return esSVC_RegSetKeyValue(handle, set_name, key_name, key_value);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetKeyValue
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
__s32  eLIBs_RegGetKeyValue(__hdle handle, const char *set_name, const char *key_name, char *key_value)
{
    return esSVC_RegGetKeyValue(handle, set_name, key_name, key_value);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetError
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
__s32  eLIBs_RegGetError(__hdle handle)
{
    return esSVC_RegGetError(handle);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegClrError
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
__s32  eLIBs_RegClrError(__hdle handle)
{
    return esSVC_RegClrError(handle);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegCreatePath
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
__s32  eLIBs_RegCreatePath(const char *path)
{
    return esSVC_RegCreatePath(path);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegDeletePath
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
__s32  eLIBs_RegDeletePath(const char *path)
{
    return esSVC_RegDeletePath(path);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegIni2Reg
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
__s32  eLIBs_RegIni2Reg(const char *ini_file)
{
    return esSVC_RegIni2Reg(ini_file);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegReg2Ini
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
__s32  eLIBs_RegReg2Ini(const char *ini_file)
{
    return esSVC_RegReg2Ini(ini_file);
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegSetRootPath
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
void   eLIBs_RegSetRootPath(const char *path)
{
    esSVC_RegSetRootPath(path);
    return;
}
/*
**********************************************************************************************************************
*                                               eLIBs_RegGetRootPath
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
void   eLIBs_RegGetRootPath(char *path)
{
    esSVC_RegGetRootPath(path);
    return;
}
