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
#ifndef __PHOTO_ADAPTER_H__F8AD6949_0199_47E9_BB76_7202D21318BD__
#define __PHOTO_ADAPTER_H__F8AD6949_0199_47E9_BB76_7202D21318BD__

#include "beetles_app.h"

#define _PHOTO_USE_MEMDEV_       1

//////////////////////////////////////////////////////////////////////////
//虚拟键
#define VK_PHOTO_MENU_POP                 VK_MENU
#define VK_PHOTO_MMENU_ADD_ITEM           VK_RIGHT
#define VK_PHOTO_MMENU_SUB_ITEM           VK_LEFT
#define VK_PHOTO_ESC                      VK_ESCAPE
#define VK_PHOTO_SMENU_LOOP               VK_SUBMENU
#define VK_PHOTO_SMENU_ENTER              VK_ENTER
#define VK_PHOTO_VOLUME_ADD               GUI_MSG_KEY_RIGHT
#define VK_PHOTO_VOLUME_SPEED_ADD         GUI_MSG_KEY_LONGRIGHT
#define VK_PHOTO_VOLUME_SUB               GUI_MSG_KEY_LEFT
#define VK_PHOTO_VOLUME_SPEED_SUB         GUI_MSG_KEY_LONGLEFT
//////////////////////////////////////////////////////////////////////////
#define APP_Temp_ResBmpDraw(_id, _x, _y)  do \
    { \
        load_bmp_res_t _res_bmp; \
        create_bmp_res(_id, _res_bmp); \
        GUI_BMP_RES_Draw(_res_bmp, _x, _y); \
        destroy_bmp_res(_res_bmp); \
    } while(0)
//////////////////////////////////////////////////////////////////////////
#define DEBUG_photo              __msg
//////////////////////////////////////////////////////////////////////////
typedef enum
{
    PHOTO_BGMUSIC_OFF,
    PHOTO_BGMUSIC_ON,
    PHOTO_BGMUSIC_MAX,
} photo_bgmusic_t;
/************************************************************************/
//取保存值
/************************************************************************/
#define get_photo_value(_d, _func)     (_d = _func())
extern __u32 photo_get_ratio(void);
extern __u32 photo_get_speedflag(void);
extern __u32 photo_get_effect(void);
extern __u32 photo_get_bgmusic(void);

/************************************************************************/
//设置保存值
/************************************************************************/
#define set_photo_value(_d, _func)     (_func(_d))
extern __u32 photo_set_ratio(__u32 ratio);
extern __u32 photo_set_speedflag(__u32 speedflag);
extern __u32 photo_set_effect(__u32 effect);
extern __u32 photo_set_bgmusic(__u32 bgmusic);

//保存断点
extern __s32 photo_save_play_info(rat_type_t root_type);
//////////////////////////////////////////////////////////////////////////
//滚动长字符串
extern __s32 PhotoLongStringShow(H_LYR hLayer, RECT *pRt, char *String, GUI_COLOR TxtClr);
//销毁滚动长字符串实例
extern void  PhotoLongStringDestroy(void);

/************************************************************************/
// 画BMP某一区域
// pFileData --- 资源函数获得的数据(也是bmp文件数据)
// x，y      --- 目标坐标
// x0, y0, x1, y1 --- 源区域(即bmp图片某一区域)
// return values  --- EPDK_OK : 成功; EPDK_FAIL : 失败.
// Remarks : 本函数拷贝了三次,效率较低,TODO:改到底层实现为一次拷贝,提高效率.
/************************************************************************/
__s32 APP_BMP_Draw(const void *pFileData, int x, int y, int x0, int y0, int x1, int y1);


typedef struct
{
    void *data;
    __s32 data_size;
} RAM_RES_BMP_T;
#define RAM_RES_BMP_DATA(_res)      ((_res)->data)
/************************************************************************/
// 删除组合图片
/************************************************************************/
void DestroyRamBmpData(RAM_RES_BMP_T *res);

/************************************************************************/
// 由一组图片资源(拉伸)组成内存图片资源
// res_id        --- 图片资源id数组
// id_count      --- 图片资源id数组大小
// direction     --- 拉伸方向, 0 : x方向; 非0 : y方向
// len           --- 拉伸方向的长度
// return values --- 内存图片数据指针
// Remarks : 数组中间的id图片为拉伸图片
/************************************************************************/
RAM_RES_BMP_T *CreateRamBmpData(__s32 res_id[], __s32 id_count, __s32 direction, __s32 len);

#endif //__PHOTO_ADAPTER_H__F8AD6949_0199_47E9_BB76_7202D21318BD__
//End of this file
