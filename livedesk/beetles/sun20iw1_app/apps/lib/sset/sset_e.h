/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : sset_e.h
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __SSET_E_H__
#define __SSET_E_H__

#include "apps.h"
#include "../common/palette.h"
#include "sset.h"





//////////////////////////////////////////////////////////////////////////
#define SSET_DEFAULLT_TIME_SPEED    500

/*
////////////
// #define DECLARE_BMP_RES_Pt(_name)   \
//  void* _name; \
//  HTHEME _name ## _h
//
// #define Create_BMP_RES_Pt(_p, _id)   do {_p ## _h = dsk_theme_open(_id); _p = dsk_theme_hdl2buf(_p ## _h);}while(0)
//
// #define Destroy_BMP_RES_Pt(_p)       dsk_theme_close(_p ## _h)
/////////////
//////////////////////////////////////////////////////////////////////////
*/

/***********************************************************************************/
typedef     void   *H_BMK;//..

typedef enum tag_SSET_ITEM_INSERT_MODE
{
    SSET_INSERT_CHLD,
    SSET_INSERT_PREV,
    SSET_INSERT_NEXT

} __sset_item_insert_mode_e;

typedef enum tag_SSET_ITEM_DELETE_MODE
{
    SSET_DELETE_CHLD,
    SSET_DELETE_SELF

} __sset_item_delete_mode_e;

/********** sset notify  message ***********/
typedef enum tag_SSET_NC_MSG
{
    SSET_NC_STA_CHG     = 0x5000,
    SSET_NC_STA_NEXT,
    SSET_NC_SET_EXIT,
    SSET_NC_TIMER

} __sset_nc_msg_e;


typedef enum tag_SSET_NODELIST_STA
{
    NODELIST_ON,
    NODELIST_OFF,
    NODELIST_DEL,

} __sset_nodelist_sta_e;


/***********************************************************************************/

/************ item para define  ***********/
typedef struct tag_SSET_ITEM_PARA
{
    char                        name[SSET_NAME_MAX];    /* ITEM的条目名称       */
    __u32                       item_sta;               /* ITEM的条目是否有效   */

    __sset_item_type_t          item_type;              /* ITEM的风格           */

    __sset_item_attr_t          item_attr;              /* ITEM的条目私有数据   */

    __sset_item_ncmsg_t         item_msg;               /* ITEM的消息码         */

    __u32                       n;                      /* ITEM子菜单中的条目数 */
    struct tag_SSET_ITEM_PARA   *childarray;            /* ITEM子菜单地址       */

} __sset_item_para_t;


/********** create para define  ***********/
typedef struct tag_SSET_CREATE
{
    char                        *name;                  /* 场景名称           */
    __u16                       sid;                    /* 场景ID             */
    H_WIN                       pwin;                   /* 场景的父窗口       */
    H_WIN                       powner;                 /* 场景的依附窗口     */
    __sset_item_para_t          *attr;                  /* 场景的私有数据     */

    GUI_FONT                    *bfont;                 /* 场景的大字体       */
    GUI_FONT                    *sfont;                 /* 场景的小字体       */

    HRES                        icon_res;               /* 场景的图标资源句柄 */
    HLANG                       lang_res;               /* 场景的语言资源句柄 */
    __u32                       icon_style;             /* 场景的图标资源风格 */
    __u32                       lang_style;             /* 场景的语言资源风格 */
    __epdk_charset_enm_e        draw_code;              /* 场景的字体编码方式 */

    __u32                       time_speed;
} __sset_create_para_t;

/***********************************************************************************/
/********** functions define  ***********/
/*
*************************************************************************************
*                                       SSET_Create
*
*Description: SSET场景创建
*
*Arguments  : para  场景创建参数指针
*
*Return     : SSET场景的管理窗口
*
*************************************************************************************
*/
H_WIN SSET_Create(__sset_create_para_t *para);
/*
*************************************************************************************
*                                       _menu_task
*
*Description: SSET场景销毁
*
*Arguments  :
*
*Return     : NULL
*
*************************************************************************************
*/
__s32 SSET_Destroy(H_WIN h_win);
/*
*************************************************************************************
*                                       SSET_UserPara_Insert
*
*Description: 插入一组条目
*
*Arguments  : pitem 要插入条目的插入节点指针
*             para  插入条目的头指针
*             num   插入的条目数
*
*Return     : NULL
*
************************************************************************************
*/
__s32 SSET_Insert(char *name, __sset_item_para_t *para, __u32 num, __sset_item_insert_mode_e mode);
/*
************************************************************************************
*                                       SSET_UserPara_Delete
*
*Description: 删除一组条目
*
*Arguments  : pitem 删除条目所挂载的节点指针
*
*Return     : NULL
*
************************************************************************************
*/
__s32 SSET_Delete(char *name, __sset_item_delete_mode_e mode);
/*
************************************************************************************
*                                       SSET_EnnableNode
*
*Description: 更新一个节点条目信息
*
*Arguments  : pitem 要更新的节点
*             para  参数指针
*Return     :
*
************************************************************************************
*/
__s32 SSET_EnnableNode(char *name);
/*
************************************************************************************
*                                       SSET_DisableNode
*
*Description: 更新一个节点条目信息
*
*Arguments  : pitem 要更新的节点
*             para  参数指针
*Return     :
*
************************************************************************************
*/
__s32 SSET_DisableNode(char *name);
/*
************************************************************************************
*                                       SSET_SetAttr
*
*Description: 更新一个节点条目信息
*
*Arguments  : pitem 要更新的节点
*             para  参数指针
*Return     :
*
************************************************************************************
*/
__s32 SSET_SetAttr(char *name, __sset_item_attr_t *attr);
/*
************************************************************************************
*                                       SSET_GetAttr
*
*Description: 更新一个节点条目信息
*
*Arguments  : pitem 要更新的节点
*             para  参数指针
*Return     :
*
************************************************************************************
*/
__s32 SSET_GetAttr(char *name, __sset_item_attr_t *attr);

/*
************************************************************************************
*                                       SSET_Get_Nodelist
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************
*/
__s32 SSET_Get_Nodelist(char *name, __sset_nodelist_sta_e *sta);

/*
************************************************************************************
*                                       SSET_Set_Nodelist
*
*Description:
*
*Arguments  :
*
*Return     :
*
************************************************************************************
*/
__s32 SSET_Set_Nodelist(char *name, __sset_nodelist_sta_e sta);


#endif  /* __SSET_E_H__ */
