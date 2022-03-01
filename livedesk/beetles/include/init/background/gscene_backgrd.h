/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : gscene_backgrd.c
* By        : Andy.zhang
* Func      : desktop toolbar control interface
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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

