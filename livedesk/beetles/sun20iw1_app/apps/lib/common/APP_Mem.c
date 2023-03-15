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
//#include "StdAfx.h"

#ifdef _WINDOWS
// C 运行时头文件
#include <malloc.h>
#include <memory.h>
#include <crtdbg.h>

#include "..\AppDebug.h"

#pragma warning( disable : 4996 )

#endif

///////////////////////////////
#include "APP_Mem.h"
///////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifndef _ASSERT
#define _ASSERT
#endif

#ifndef ASSERT
#define ASSERT _ASSERT
#endif

#ifdef _WINDOWS
#define APP_strncpy(_dst, _src, _n)     _tcsncpy(_dst, _src, _n)
#else
#define APP_strncpy(_dst, _src, _n)     eLIBs_strncpy(_dst, _src, _n)
#endif

#ifndef __msg
#define __msg                           DEBUG_Print
#endif

//////////////////////////////////////////////////////////////////////////
#if DEBUG_APP_MEM

//定义记录链表
#define DEBUG_APP_MEM_FILE_NAME_MAX_MEMLEN    64
typedef struct tag_DEBUG_APP_MEM_LINK
{
    void  *pMEM;
    int   line;
    TCHAR file_name[DEBUG_APP_MEM_FILE_NAME_MAX_MEMLEN];

    struct tag_DEBUG_APP_MEM_LINK *next;
    struct tag_DEBUG_APP_MEM_LINK *prev;
} DEBUG_APP_MEM_LINK_T;

//链表头
static DEBUG_APP_MEM_LINK_T g_DebugAPPMEM_Head = {NULL, 0, {0}, NULL, NULL};
//链表尾指针
static DEBUG_APP_MEM_LINK_T *g_DebugAPPMEM_Tail = &g_DebugAPPMEM_Head;

//统计申请内存次数
static int gs_NEW_MEM_COUNT = 0;
//统计申请节点次数 //TODO:to remove
static int gs_NEW_LINKNODE_COUNT = 0;

#define NewDebugAPPMEMNode(_p)  do {ap_NewMem_T_B(_p, DEBUG_APP_MEM_LINK_T); if (_p){gs_NEW_LINKNODE_COUNT++;}} while(0)
#define DelDebugAPPMEMNode(_p)  do {if (_p){gs_NEW_LINKNODE_COUNT--;} ap_DelMem_T_B(_p, DEBUG_APP_MEM_LINK_T);} while(0)

//增加一个node记录
void *AddDebugAPPMEM(void *p, int line, TCHAR *filename)
{
    gs_NEW_MEM_COUNT++;
    //ASSERT(g_DebugAPPMEM_Tail);
    APP_strncpy(g_DebugAPPMEM_Tail->file_name, filename, DEBUG_APP_MEM_FILE_NAME_MAX_MEMLEN - 1);
    g_DebugAPPMEM_Tail->line = line;
    g_DebugAPPMEM_Tail->pMEM = p;
    NewDebugAPPMEMNode(g_DebugAPPMEM_Tail->next);

    if (g_DebugAPPMEM_Tail->next)
    {
        ZeroTypeMem(g_DebugAPPMEM_Tail->next, DEBUG_APP_MEM_LINK_T);
        g_DebugAPPMEM_Tail->next->prev = g_DebugAPPMEM_Tail;
        g_DebugAPPMEM_Tail = g_DebugAPPMEM_Tail->next;
    }

    return p;
}

//查找一个node记录
static DEBUG_APP_MEM_LINK_T *FindDebugAPPMEMNode(void *p)
{
    DEBUG_APP_MEM_LINK_T *Cur = &g_DebugAPPMEM_Head;

    while (Cur->pMEM != p)
    {
        if (Cur->next)
        {
            Cur = Cur->next;
        }
        else
        {
            break;
        }
    }

    if (Cur->pMEM == p)
    {
        return Cur;
    }
    else
    {
        return NULL;
    }
}

//去掉一个node记录
void *SubDebugAPPMEM(void *p)
{
    DEBUG_APP_MEM_LINK_T *Node;
    gs_NEW_MEM_COUNT--;
    Node = FindDebugAPPMEMNode(p);

    if (Node)
    {
        if (Node == &g_DebugAPPMEM_Head)
        {
            Node->file_name[0] = 0;
            Node->line         = 0;
            Node->pMEM         = NULL;
            //ZeroTypeMem(Node, DEBUG_APP_MEM_LINK_T);
        }
        else
        {
            if (Node->prev)//前节点next指向后节点
            {
                Node->prev->next = Node->next;
            }

            if (Node->next)//后节点prev指向前节点
            {
                Node->next->prev = Node->prev;
            }

            ASSERT(Node != g_DebugAPPMEM_Tail);
            DelDebugAPPMEMNode(Node);
        }
    }
    else
    {
        __msg("not find mem node, need check\n");
    }

    ASSERT(g_DebugAPPMEM_Tail);
    return Node;
}

//打印信息
#ifdef _WINDOWS

#define DEBUG_APP_MEM_LINK_NOTFREE_TIP(_pNode)   \
    if ((_pNode)->pMEM) \
    { \
        DebugPrint(_T("%s(%d):memory not free\n"), (_pNode)->file_name, (_pNode)->line); \
    }

#else

#define DEBUG_APP_MEM_LINK_NOTFREE_TIP(_pNode)   \
    if ((_pNode)->pMEM) \
    { \
        __msg("MEM [ %x ] isnot free, code at %s(%d):\n", (_pNode)->pMEM, (_pNode)->file_name, (_pNode)->line); \
    }

#endif

//检查记录
void _CheckAPPMEM(void)
{
    DEBUG_APP_MEM_LINK_T *Cur = &g_DebugAPPMEM_Head;
    __msg("!!!!!!!!!!!!!!!now gs_NEW_MEM_COUNT = %d !!!!!!!!!!!!!!!\n", gs_NEW_MEM_COUNT);

    while (Cur)
    {
        DEBUG_APP_MEM_LINK_NOTFREE_TIP(Cur);
        Cur = Cur->next;
    }
}

//释放链表
void _EndOfDebugAPPMEM(void)
{
    DEBUG_APP_MEM_LINK_T *next;
    DEBUG_APP_MEM_LINK_T *Cur = &g_DebugAPPMEM_Head;
    __msg("!!!!!!!!!!!!!!!now gs_NEW_MEM_COUNT = %d !!!!!!!!!!!!!!!\n", gs_NEW_MEM_COUNT);
    DEBUG_APP_MEM_LINK_NOTFREE_TIP(Cur);
    Cur = Cur->next;

    while (Cur)
    {
        DEBUG_APP_MEM_LINK_NOTFREE_TIP(Cur);
        next = Cur->next;
        DelDebugAPPMEMNode(Cur);
        Cur = next;
    }

    g_DebugAPPMEM_Tail = &g_DebugAPPMEM_Head;
    ZeroTypeMem(g_DebugAPPMEM_Tail, DEBUG_APP_MEM_LINK_T);
    ASSERT(gs_NEW_LINKNODE_COUNT == 0);

    if (gs_NEW_MEM_COUNT)
    {
        __err("gs_NEW_MEM_COUNT != 0");
    }
}

#endif //DEBUG_APP_MEM
