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
#include "sys_service.h"

//register sevice enable or not
#if SVC_REG_EN

#define START_THREAD                    1
#define END_THREAD                      0

/*
**********************************************************************************************************************
*                                               REG_Init
*
* Description: initialise register table module.
*
* Arguments  : void
*
* Returns    : EPDK_OK        operation succeed
*              EPDK_FAIL      operation fail
* Notes      :
*
**********************************************************************************************************************
*/
int32_t SVC_RegInit(void)
{
    return REG_Init();
}

/*
**********************************************************************************************************************
*                                               REG_Exit
*
* Description: exit register table module.
*
* Arguments  : void
*
* Returns    : EPDK_OK        operation succeed
*              EPDK_FAIL      operation fail
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t SVC_RegExit(void)
{
    return REG_Exit();
}


/*
**********************************************************************************************************************
*                                               esSVC_RegCloseNode
*
* Description: close node.
*
* Arguments  : handle         a handle of a node
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegCloseNode(void* handle)
{
    return REG_CloseNode(handle);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegClrError
*
* Description: get error code of current operation.
*
* Arguments  : hdl        a handle of a node
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegClrError(void* handle)
{
    return REG_ClrError(handle);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegCreateKey
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
int32_t esSVC_RegCreateKey(void* hNode, const char *set_name, const char *key_name, const char *key_value)
{
    return REG_CreateKey(hNode, set_name, key_name, key_value);
}

/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description: create a path
*
* Arguments  : path           register path
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegCreatePath(const char *path)
{
    return REG_CreatePath(path);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegCreateSet
*
* Description: create a set based on specified name.
*
* Arguments  : handle         a handle of a node
*              set_name       set name
*
* Returns    : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_SET_EXIST        the set has existed.
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegCreateSet(void* handle, const char *set_name)
{
    return REG_CreateSet(handle, set_name);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegDeleteKey
*
* Description: delete a key from specified set.
*
* Arguments  : handle         a handle of node
*              set_name       set name
*              key_name       key name
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegDeleteKey(void* hNode, const char *set_name, const char *key_name)
{
    return REG_DeleteKey(hNode, set_name, key_name);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegDeleteNode
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
int32_t esSVC_RegDeleteNode(const  char *node)
{
    return REG_DeleteNode(node);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegDeletePath
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
int32_t esSVC_RegDeletePath(const char *path)
{
    return REG_DeletePath(path);
}

/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description: delete a set. Once the set is deleted, all the keys that pertain to the set are deteled.
*
* Arguments  : handle         a handle of a node
*              set_name       set name
*
* Returns    : EPDK_OK/FAIl
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegDeleteSet(void* handle, const char *set_name)
{
    return REG_DeleteSet(handle, set_name);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetError
*
* Description: get error code of current operation.
*
* Arguments  : hdl        a handle of a node
*
* Returns    : error code
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetError(void* handle)
{
    return REG_GetError(handle);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetFirstKey
*
* Description: the function is used to organise key list for GetNextKey(). NOTE: before call
*              GetNextKey(), GetFirstKey() must be called first.
*
* Arguments  : handle         a handle of node
*              set_name       set name
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetFirstKey(void* handle, const char *set_name)
{
    return REG_GetFirstKey(handle, set_name);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetFirstSet
*
* Description: the function is used to organise set list for GetNextSet(). NOTE: before call
*              GetNextSet(), GetFirstSet() must be called first.
*
* Arguments  : handle         a handle of a node
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetFirstSet(void* handle)
{
    return REG_GetFirstSet(handle);
}

/*
**********************************************************************************************************************
*                                               FUNCTION
*
* Description:
*
* Arguments  : handle         a handle of a node
*              set_name       set name
*              count_p        output. the total key count of the specified set
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetKeyCount(void* handle, const char *set_name, uint32_t *count_p)
{
    return REG_GetKeyCount(handle, set_name, count_p);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetKeyValue
*
* Description: get the value of specified key.
*
* Arguments  : handle         a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      output. a buffer to store key value. The size of the buffer must NOT be
*                             less than KEY_VALUE_LEN+1, (the constant is defined in head file).
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetKeyValue(void* hNode, const char *set_name, const char *key_name, char *key_value)
{
    return REG_GetKeyValue(hNode, set_name, key_name, key_value);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetNextKey
*
* Description: get next key name of specified set.
*
* Arguments  : hNode          a handle of node
*              set_name       set name
*              key_name       output. a buffer to store key name. The size of the buffer must NOT be
*                             less than KEY_NAME_LEN+1, (the constant is defined in head file).
*
* Returns    : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_KEY_LIST_END     to the end of key list
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetNextKey(void* hNode, const char *set_name, char *key_name)
{
    return REG_GetNextKey(hNode, set_name, key_name);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetNextSet
*
* Description: get next set name.
*
*Arguments   : handle         a handle of a node
*              set_name       output. a buffer to store set name. The size of the buffer must NOT be
*                             less than SET_NAME_LEN+1, (the constant is defined in head file).
*
*Return      : REG_OK                   operation succeed
*              REG_ERR                  operation fail
*              REG_ERR_SET_LIST_END     to the end of set list
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetNextSet(void* handle, char *set_name)
{
    return REG_GetNextSet(handle, set_name);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetRootPath
*
* Description: get register root path.
*
* Arguments  : path           register root path
*
* Returns    : void
*
* Notes      :
*
**********************************************************************************************************************
*/
void esSVC_RegGetRootPath(char *path)
{
    REG_GetRootPath(path);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegGetSetCount
*
* Description: get the total count of set of node.
*
* Arguments  : handle         a handle of a node
*              count_p        output. the total count of set of node.
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegGetSetCount(void* handle, uint32_t *count_p)
{
    return REG_GetSetCount(handle, count_p);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegIni2Reg
*
* Description: create a register file corresponding to ini file.
*
* Arguments  : ini_file       full name of ini file
*
* Returns    : EPDK_OK         operation succeed
*              EPDK_ERR        operation fail
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegIni2Reg(const char *ini_file)
{
    return REG_Ini2Reg(ini_file);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegOpenNode
*
* Description: open a node in certain mode.
*
* Arguments  : node           node to be open
*              mode           open mode. optional mode: REG_READONLY, REG_READWRITE and REG_CREATE.
*                             in REG_READONLY mode, node can be only read and can be open several
*                             handles at the same time. In REG_READWRITE mode, node can be read and
*                             write, and can be open only one hdl at the same time. In REG_CREATE
*                             mode, create a new node.
*
* Returns    : NULL           operation fail
*             !NULL          operation succeed, a hdl of node.
*
* Notes      :
*
**********************************************************************************************************************
*/
void* esSVC_RegOpenNode(const char *node, int32_t mode)
{
    return REG_OpenNode(node, mode);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegReg2Ini
*
* Description: create a ini file corresponding to register file
*
* Arguments  : ini_file       full name of ini file
*
* Returns    : EPDK_OK         operation succeed
*              EPDK_ERR        operation fail
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegReg2Ini(const char *ini_file)
{
    return REG_Reg2Ini(ini_file);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegSetKeyValue
*
* Description: set the value of specified key.
*
* Arguments  : hNode          a handle of a node
*              set_name       set name
*              key_name       key name
*              key_value      key value
*              create_mode    creating mode about key. optional creating mode : CREATE_KEY and
*                             NOT_CREATE_KEY. In CREATE_KEY mode, if the specified key doesn't
*                             exist, a new key may be created. In NOT_CREATE_KEY mode, a new key
*                             can't be created if the specified key doesn't exist.
*
* Returns    : EPDK_OK/FAIL
*
* Notes      :
*
**********************************************************************************************************************
*/
int32_t esSVC_RegSetKeyValue(void* hNode, const char *set_name, const char *key_name, const char *key_value)
{
    return REG_SetKeyValue(hNode, set_name, key_name, key_value);
}

/*
**********************************************************************************************************************
*                                               esSVC_RegSetRootPath
*
* Description:
*
* Arguments  : path           register root path
*
* Returns    : void
*
* Notes      : set register root path.
*
**********************************************************************************************************************
*/
void esSVC_RegSetRootPath(const char *path)
{
    REG_SetRootPath(path);
}

#endif  /* SVC_REG_EN */