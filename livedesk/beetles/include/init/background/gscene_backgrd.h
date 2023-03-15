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
#ifndef __GSCENE_BACKGRD_H__
#define __GSCENE_BACKGRD_H__

#define BG_MAX_NUM          3

typedef enum
{
    BGD_STATUS_SHOW,
    BGD_STATUS_HIDE
} bgd_status_t;

/*获得显示默认背景还是用户设置的背景的标记
 flag : EPDK_TRUE : 默认背景图片
      : EPDK_FALSE: 设置背景图片
 */
__bool gscene_bgd_get_flag(void);


/*设置显示默认背景还是用户设置的背景的标记
 flag : EPDK_TRUE : 默认背景图片
      : EPDK_FALSE: 设置背景图片
 */
void gscene_bgd_set_flag(__bool flag);


/*获得默认背景图片的编号 index:0-BG_MAX_NUM-1
 */
__s32 gscene_bgd_get_default_bg_index(void);


/*设置默认背景图片的编号 index:0-BG_MAX_NUM-1
 */
__s32 gscene_bgd_set_default_bg_index(__u32 index);




/***************************************************************************************************
*Name        : gscene_bgd_init
*Prototype   : __s32 gscene_bgd_init(SIZE *p_size, bgd_status_t status, __fb_type_t ftype);
*Arguments   : p_size   ：背景尺寸
               status   : 初始化背景是否显示
               ftype    : 背景图层格式 FB_TYPE_RGB、 FB_TYPE_YUV(节约带宽）
*Return      :
*Description : 初始化背景图层(main应用程序调用）
*Other       :
***************************************************************************************************/
__s32        gscene_bgd_init(SIZE *p_size, bgd_status_t status, __fb_type_t ftype);

/***************************************************************************************************
*Name        : gscene_bgd_set_state
*Prototype   : void gscene_bgd_set_state(bgd_status_t status);
*Arguments   :
*Return      :
*Description : 设置背景状态
*Other       :
***************************************************************************************************/
void         gscene_bgd_set_state(bgd_status_t status);

/***************************************************************************************************
*Name        : gscene_bgd_get_state
*Prototype   : bgd_status_t gscene_bgd_get_state(void);
*Arguments   :
*Return      :
*Description : 查询背景状态
*Other       :
***************************************************************************************************/
bgd_status_t gscene_bgd_get_state(void);

/***************************************************************************************************
*Name        : gscene_bgd_set_top
*Prototype   : void gscene_bgd_set_top(void);
*Arguments   :
*Return      :
*Description : 背景图层优先级置顶
*Other       :
***************************************************************************************************/
void         gscene_bgd_set_top(void);

/***************************************************************************************************
*Name        : gscene_bgd_set_bottom
*Prototype   : void gscene_bgd_set_bottom(void);
*Arguments   :
*Return      :
*Description : 背景图层优先级置底
*Other       :
***************************************************************************************************/
void         gscene_bgd_set_bottom(void);

/***************************************************************************************************
*Name        : gscene_bgd_save_fb
*Prototype   : __s32 gscene_bgd_save_fb(FB *fb, char* path);
*Arguments   :
*Return      :
*Description : 保存背景图片
*Other       :
***************************************************************************************************/
__s32        gscene_bgd_save_fb(FB *fb, char *path);

/***************************************************************************************************
*Name        : gscene_bgd_set_fb_type
*Prototype   : __s32 gscene_set_load_arg(char *app_name, void *p_arg, __u32 size);
*Arguments   :
*Return      :
*Description : 设置背景图层格式
*Other       :
***************************************************************************************************/
void         gscene_bgd_set_fb_type(__fb_type_t ftype);

/***************************************************************************************************
*Name        : gscene_bgd_refresh
*Prototype   : void gscene_bgd_refresh(void);
*Arguments   :
*Return      :
*Description : 刷新背景图层，从新绘制背景图片
*Other       :
***************************************************************************************************/
void         gscene_bgd_refresh(void);

/***************************************************************************************************
*Name        : gscene_bgd_restore
*Prototype   : void gscene_bgd_refresh(void);
*Arguments   :
*Return      :
*Description : 恢复默认背景图
*Other       :
***************************************************************************************************/
void         gscene_bgd_restore(void);
/***************************************************************************************************
*Name        : gscene_bgd_deinit
*Prototype   : __s32 gscene_bgd_deinit(void);
*Arguments   :
*Return      :
*Description : main 应用程序调用
*Other       :
***************************************************************************************************/
__s32        gscene_bgd_deinit(void);


#endif /* __GSCENE_BACKGRD_H__ */
