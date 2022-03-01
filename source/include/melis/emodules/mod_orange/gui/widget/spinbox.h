/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    change_private.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  gui button 私有数据结构和常量定义
*
*Others :       None at present.
*
* History :
*
* <Author>          <calendar>       <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_SPINBOX_PRIVATE_H__
#define __GUI_SPINBOX_PRIVATE_H__
#include <kconfig.h>

#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined (CONFIG_SOC_SUN20IW1)
#include "ResAdapter.h"
#endif

#define CTRL_SPINBOX         ("spinbox")


#define BST_PUSHED          (1<<1)
#define BST_FOCUS           (1<<2)
#define BST_LEFT            (1<<3)


#define SPN_CHG              0
#define SPN_STOPCHG          1


typedef struct tagchapara
{
    __u32                       spin_sta;               /*减少图标位置*/
#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined (CONFIG_SOC_SUN20IW1)
    DECLARE_BMP_RES_Pt(fdn_bmp);
    DECLARE_BMP_RES_Pt(fup_bmp);
    DECLARE_BMP_RES_Pt(ufdn_bmp);
    DECLARE_BMP_RES_Pt(ufup_bmp);
#elif defined CONFIG_SOC_SUN3IW1
    void                        *fdn_bmp;               /*减少图标 焦点*/
    void                        *fup_bmp;               /*增加图标 焦点*/
    void                        *ufdn_bmp;              /*减少图标 非焦点*/
    void                        *ufup_bmp;              /*增加图标 非焦点*/
#endif
    __pos_t                     upbmp_pos;              /*减少图标位置*/
    __pos_t                     dnbmp_pos;              /*增加图标位置*/
    __s16                       cur_value;              /*当前值*/
    __s16                       max_value;              /*最大值*/
    __s16                       min_value;              /*最小值*/
    __pos_t                     text_pos;               /*文本位置*/
    __u32                       ftxt_color;             /*文本焦点色*/
    __u32                       uftxt_color;            /*文本非焦点色*/
    GUI_FONT                   *draw_font;              /*字符编码*/
    __epdk_charset_enm_e        draw_code;              /*字符编码*/
    __u32                       bk_color;               /*清屏色*/
} spinbox_para_t;

typedef struct tagSPINBOXDATA
{
    GUI_MEMDEV_Handle   hmem;
    __u32               draw_sts;
    __u32               status;
    __u32               draw_flag;
    __u32               data;
} spinbox_data_t;

typedef spinbox_data_t *pspinboxdata;

#define SPINBOX_DRAWSTATUS(pCtrl) (((pspinboxdata)GUI_CtrlWinGetAddData(pCtrl))->draw_sts)
#define SPINBOX_STATUS(pCtrl)     (((pspinboxdata)GUI_CtrlWinGetAddData(pCtrl))->status)
#define SPINBOX_DRAWFLAG(pCtrl)  (((pspinboxdata)GUI_CtrlWinGetAddData(pCtrl))->draw_flag)
#define SPINBOX_DATA(pCtrl)       (((pspinboxdata)GUI_CtrlWinGetAddData(pCtrl))->data)

__bool RegisterSpinboxControl(void);

__s32  GUI_SpinboxGetCurValue(H_WIN hspin, __s16  *value);
__s32  GUI_SpinboxSetCurValue(H_WIN hspin, __s16  value);
__s32  GUI_SpinboxSetMaxValue(H_WIN hspin, __s16  value);
__s32  GUI_SpinboxSetMinValue(H_WIN hspin, __s16  value);
__s32  GUI_SpinboxSetBkColor(H_WIN hspin, __u32 color);
__s32  GUI_SpinboxSetUnFcsTxtColor(H_WIN hspin, __u32 color);
__s32  GUI_SpinboxSetFcsTxtColor(H_WIN hspin, __u32  color);
__s32  GUI_SpinboxSetFocusUpBmp(H_WIN hspin, void *bmp);
__s32  GUI_SpinboxSetFocusDownBmp(H_WIN hspin, void *bmp);
__s32  GUI_SpinboxSetUnFocusUpBmp(H_WIN hspin, void *bmp);
__s32  GUI_SpinboxSetUnFocusDownBmp(H_WIN hspin, void *bmp);
__s32  GUI_SpinboxSetUpBmpPos(H_WIN hspin, __pos_t *pos);
__s32  GUI_SpinboxSetDownBmpPos(H_WIN hspin, __pos_t *pos);

#endif /* #ifndef __GUI_SPINBOX_PRIVATE_H__ */
