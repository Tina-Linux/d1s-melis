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
#ifndef __APP_MEM_H__C7FF2028_BBAF_4206_B67C_2DEE5D2EAF60__
#define __APP_MEM_H__C7FF2028_BBAF_4206_B67C_2DEE5D2EAF60__
//////////////////////////////////////////////////////////////////////////
#ifndef _WINDOWS//for ePDK

//#include "epdk.h"
#include <emodules/mod_defs.h>
#include <const.h>

typedef char        TCHAR;
#define _T(x)       x
#endif
//////////////////////platform interface//////////////////////////////////
/////////////////////////////////内存类
#ifdef _WINDOWS
//for win32
//申请内存
#define ap_NewMem_B(_p, _size)                  ((_p) = (new BYTE[_size]))//((_p) = (void*)(new BYTE[_size]))
#define ap_NewMem_M(_p, _heap, _size)           ap_NewMem_B(_p, _size)
#define ap_NewMem_P(_p, _page, _mode)           ap_NewMem_B(_p, (_page) * 1024)

#define ap_NewMem_T_B(_p, _type)                ((_p) = new _type)
#define ap_NewMem_T_M(_p, _heap, _type)         ap_NewMem_T_B(_p, _type)

//释放内存
#define ap_DelMem_B(_p, _size)                  (delete [] _p)
#define ap_DelMem_M(_p, _heap)                  (delete [] _p)
#define ap_DelMem_P(_p, _page)                  ap_DelMem_B(_p, (_page) * 1024)

#define ap_DelMem_T_B(_p, _type)                (delete _p)
#define ap_DelMem_T_M(_p, _heap)                (delete _p)

//操作内存
#ifndef ZeroMemory
#define ZeroMemory(_p, _size)                    memset((_p), 0, (_size))
#endif

#else
//for other platform
//eDPK
//申请内存
#define ap_NewMem_B(_p, _size)                    ((_p) = esMEMS_Balloc(_size))
#define ap_NewMem_M(_p, _heap, _size)             ((_p) = esMEMS_Malloc(_heap, _size))
#define ap_NewMem_P(_p, _page, _mode)             ((_p) = esMEMS_Palloc(_page, _mode))

#define ap_NewMem_T_B(_p, _type)                  ((_p) = (_type *)esMEMS_Balloc(sizeof(_type)))
#define ap_NewMem_T_M(_p, _heap, _type)           ((_p) = (_type *)esMEMS_Malloc(_heap, sizeof(_type)))

//释放内存
#define ap_DelMem_B(_p, _size)                    (esMEMS_Bfree(_p, _size))
#define ap_DelMem_M(_p, _heap)                    (esMEMS_Mfree(_heap, _p))
#define ap_DelMem_P(_p, _page)                    (esMEMS_Pfree(_p, _page))

#define ap_DelMem_T_B(_p, _type)                  ap_DelMem_B(_p, sizeof(_type))
#define ap_DelMem_T_M(_p, _heap)                  ap_DelMem_M(_p, _heap)

//操作内存
#ifndef ZeroMemory
#define ZeroMemory(_p, _size)                    eLIBs_memset((_p), 0, (_size))
#endif

#endif //_WINDOWS

//清空某类型内存
#define ZeroTypeMem(_p, _type)                   ZeroMemory(_p, sizeof(_type))

/////////////////////////////////句柄类
#ifdef _WINDOWS
//图片资源
#define ap_dsk_theme_open(_h, _id)               NULL
#define ap_dsk_theme_close(_h)                   NULL

//文件
#define ap_fopen(_f, _name, _mode)               NULL
#define ap_fclose(_f)                            NULL

//图层
#define ap_create_layer(_layer, _pcreateinfo)    NULL
#define ap_destroy_layer(_layer)                 NULL

#else//EDPK_OS
//图片资源
#define ap_dsk_theme_open(_h, _id)               (_h = dsk_theme_open(_id))
#define ap_dsk_theme_close(_h)                   dsk_theme_close(_h)

//文件
#define ap_fopen(_f, _name, _mode)               (_f = eLIBs_fopen(_name, _mode))
#define ap_fclose(_f)                            eLIBs_fclose(_f)

//图层
#define ap_create_layer(_layer, _pcreateinfo)    (_layer = GUI_LyrWinCreate(_pcreateinfo))
#define ap_destroy_layer(_layer)                 GUI_LyrWinDelete(_layer)

#endif //_WINDOWS

////////////////////////platform interface end////////////////////////////


///////////////////////////app interface//////////////////////////////////
//不要 debug memory,修改 DEBUG_APP_MEM 定义即可
#ifdef _WINDOWS

#ifdef _DEBUG
#define DEBUG_APP_MEM         1
#else
#define DEBUG_APP_MEM         0
#endif
#else
#define DEBUG_APP_MEM         0
#endif

#if DEBUG_APP_MEM
//debug mem
extern void *AddDebugAPPMEM(void *p, int line, TCHAR *filename);
extern void *SubDebugAPPMEM(void *p);
extern void _CheckAPPMEM(void);//检查没释放的节点
extern void _EndOfDebugAPPMEM(void);//释放链表
#define CheckAPPMEM()       do \
        {esKSRV_SysInfo(); __msg("  !!!!  CheckAPPMEM, %s\n", __FUNCTION__); _CheckAPPMEM();} while (0)

#define StartDebugAPPMEM()  do \
        {esKSRV_SysInfo(); __msg("  !!!!  StartDebugAPPMEM, %s\n", __FUNCTION__); _CheckAPPMEM();} while (0)

#define EndOfDebugAPPMEM()  do \
        {_EndOfDebugAPPMEM(); esKSRV_SysInfo(); __msg("  !!!!  EndOfDebugAPPMEM, %s\n", __FUNCTION__);} while (0)

//申请内存
#define APP_NewMem_B(_p, _size)                 do \
    { \
        ap_NewMem_B(_p, _size); \
        AddDebugAPPMEM(_p, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_NewMem_M(_p, _heap, _size)          do \
    { \
        ap_NewMem_M(_p, _heap, _size); \
        AddDebugAPPMEM(_p, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_NewMem_P(_p, _page, _mode)          do \
    { \
        ap_NewMem_P(_p, _page, _mode); \
        AddDebugAPPMEM(_p, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_NewMem_T_B(_p, _type)               do \
    { \
        ap_NewMem_T_B(_p, _type); \
        AddDebugAPPMEM(_p, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_NewMem_T_M(_p, _heap, _type)        do \
    { \
        ap_NewMem_T_M(_p, _heap, _type); \
        AddDebugAPPMEM(_p, __LINE__, _T(__FILE__)); \
    } while (0)

//释放内存
#define APP_DelMem_B(_p, _size)                 if (_p){SubDebugAPPMEM(_p); ap_DelMem_B(_p, _size); _p = NULL;}
#define APP_DelMem_M(_p, _heap)                 if (_p){SubDebugAPPMEM(_p); ap_DelMem_M(_p, _heap); _p = NULL;}
#define APP_DelMem_P(_p, _page)                 if (_p){SubDebugAPPMEM(_p); ap_DelMem_P(_p, _page); _p = NULL;}

#define APP_DelMem_T_B(_p, _type)               APP_DelMem_B(_p, sizeof(_type))
#define APP_DelMem_T_M(_p, _heap)               APP_DelMem_M(_p, _heap)

/////////////////////////////////句柄类
//图片资源
#define APP_dsk_theme_open(_h, _id)             do \
    { \
        ap_dsk_theme_open(_h, _id); \
        AddDebugAPPMEM((void*)_h, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_dsk_theme_close(_h)                 do { SubDebugAPPMEM((void*)_h); ap_dsk_theme_close(_h); } while (0)

//文件
#define APP_fopen(_f, _name, _mode)             do \
    { \
        ap_fopen(_f, _name, _mode); \
        AddDebugAPPMEM((void*)_f, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_fclose(_f)                          if (_f) {SubDebugAPPMEM((void*)_f); ap_fclose(_f); _f = NULL;}

//图层
#define APP_create_layer(_layer, _pcreateinfo)  do \
    { \
        ap_create_layer(_layer, _pcreateinfo); \
        AddDebugAPPMEM((void*)_layer, __LINE__, _T(__FILE__)); \
    } while (0)

#define APP_destroy_layer(_layer)               if (_layer) {SubDebugAPPMEM((void*)_layer); ap_destroy_layer(_layer); _layer = NULL;}

#else

#define CheckAPPMEM()          NULL
#define EndOfDebugAPPMEM()     NULL
#define StartDebugAPPMEM()     NULL

//eDPK
//申请内存
#define APP_NewMem_B(_p, _size)                 ap_NewMem_B(_p, _size)
#define APP_NewMem_M(_p, _heap, _size)          ap_NewMem_M(_p, _heap, _size)
#define APP_NewMem_P(_p, _page, _mode)          ap_NewMem_P(_p, _page, _mode)

#define APP_NewMem_T_B(_p, _type)               ap_NewMem_T_B(_p, _type)
#define APP_NewMem_T_M(_p, _heap, _type)        ap_NewMem_T_M(_p, _heap, _type)

//释放内存
#define APP_DelMem_B(_p, _size)                 if (_p){ap_DelMem_B(_p, _size); _p = NULL;}
#define APP_DelMem_M(_p, _heap)                 if (_p){ap_DelMem_M(_p, _heap); _p = NULL;}
#define APP_DelMem_P(_p, _page)                 if (_p){ap_DelMem_P(_p, _page); _p = NULL;}

#define APP_DelMem_T_B(_p, _type)               APP_DelMem_B(_p, sizeof(_type))
#define APP_DelMem_T_M(_p, _heap)               APP_DelMem_M(_p, _heap)

/////////////////////////////////句柄类
//图片资源
#define APP_dsk_theme_open(_h, _id)             ap_dsk_theme_open(_h, _id)
#define APP_dsk_theme_close(_h)                 ap_dsk_theme_close(_h) //dsk_theme_close内部已有判断

//文件
#define APP_fopen(_f, _name, _mode)             ap_fopen(_f, _name, _mode)
#define APP_fclose(_f)                          if (_f) {ap_fclose(_f); _f = NULL;}

//图层
#define APP_create_layer(_layer, _pcreateinfo)  ap_create_layer(_layer, _pcreateinfo)
#define APP_destroy_layer(_layer)               if (_layer) {ap_destroy_layer(_layer); _layer = NULL;}

#endif //DEBUG_APP_MEM
///////////////////////////app interface end//////////////////////////////

#endif //__APP_MEM_H__C7FF2028_BBAF_4206_B67C_2DEE5D2EAF60__
//End of this file
