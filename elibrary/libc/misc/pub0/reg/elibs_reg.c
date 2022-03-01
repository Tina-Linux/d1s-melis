/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : reg
* File    : elibs_reg.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-9-22 16:08:58
**********************************************************************************************************************
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
