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
#ifndef __SSET_H__
#define __SSET_H__


/********** sset   widget  id    ***********/
#define     SSET_SUBMENU_ID     3000                /* 控件listmenu 的ID 号             */
#define     SSET_MENU_ID        ID_TOOL_BAR         /* 控件toolbar  的ID 号             */

#define     SSET_NAME_MAX           128
#define     SSET_TEXT_MAX           128
#define     MENU_CNT                5                   /* 菜单级数         */

typedef enum tag_SSET_MENU_TYPE
{
    MENU_TYPE_TBAR,
    MENU_TYPE_LSTM

} __sset_menu_type_e;

/********** ssetting in message ***********/
typedef enum tag_SSET_IN_MSG
{
    SSET_IN_UPDATE_INFO = 0x4050,
    SSET_IN_INFO_UPDATE,
    SSET_STRING_UPDATE,//..
    SSET_ITEM_UPDATE,//..

    SSET_IN_INFO_NUM,

} __sset_in_msg_e;

/**********app send msg to setting***********/
typedef enum tag_APP_TO_SSET_MSG
{
    SSET_APP2SSET_PREV = SSET_IN_INFO_NUM,                  /*返回上一级菜单*/
    SSET_APP2SSET_TO_TBAR,          /*返回到第一级菜单*/
    //..SSET_STRING_UPDATE, // 30234384
    //..SSET_ITEM_UPDATE,   // 30234384

    SSET_APP2SET_NUM,
} __app_to_sset_msg;



/************ item type define  ***********/
typedef enum tag_SSET_ITEM_TYPE
{
    TYPE_NORMAL,
    TYPE_SPIN,
    TYPE_SLIDE,
    TYPE_SWITCH,
    TYPE_OPT,
    TYPE_TBAR,

} __sset_item_type_t;

/********** user define message ***********/
typedef struct tag_SSET_ITEM_NCMSG
{
    __u16           msg_id;
    __u32           msg_value;
} __sset_item_ncmsg_t;


typedef struct tag_SSET_ITEM_ATTR
{
    __u32                       icon[8];                /* ITEM图标资源ID号     */
    /*********************************************************************************
        type = TYPE_TBAR
        icon[0] = active focus bmp
        icon[1] = active unfocus bmp
        icon[2] = unactive focus bmp
        icon[3] = unactive unfocus bmp
    ***********************************************
        type = TYPE_NORMAL
        icon[0] = item focus bmp
        icon[1] = item unfocus bmp
    ***********************************************
        type = TYPE_SPIN
        icon[0] = item focus bmp
        icon[1] = item unfocus bmp
        icon[2] = spinbox back bmp
        icon[3] = spinbox dec fcs bmp
        icon[4] = spinbox inc fcs bmp
        icon[5] = spinbox dec ufcs bmp
        icon[6] = spinbox inc ufcs bmp
    ***********************************************
        type = TYPE_BTTN
        icon[0] = item focus bmp
        icon[1] = item unfocus bmp
        icon[2] = button focus bmp
        icon[3] = button unfocus bmp
    ***********************************************
        type = TYPE_SLIDE
        icon[0] = item focus bmp
        icon[1] = item unfocus bmp
        icon[2] = slide head bmp
        icon[3] = slide tail bmp
        icon[4] = slide cursor bmp
    ***********************************************
        type = TYPE_OPT
        icon[0] = item focus bmp
        icon[1] = item unfocus bmp
        icon[2] = item opt bmp
    ***********************************************
    *********************************************************************************/
    __u32                       string[2];              /* ITEM文本资源ID号     */

    __u32                       cur_value;              /* 当前值               */
    __u32                       max_value;              /* 最大值               */
    __u32                       min_value;              /* 最小值               */

    __u32                       opt;                    /* OPT选种标示          */
    char                        text[SSET_TEXT_MAX];    /* ITEM文本             */

} __sset_item_attr_t;


/**********  sset  date  struct  ***********/
typedef struct tag_ITEM
{
    char                        name[SSET_NAME_MAX];    /* ITEM的条目名称       */
    __u32                       item_sta;               /* ITEM的条目是否有效   */

    __sset_item_type_t          item_type;              /* ITEM的风格           */

    __sset_item_attr_t          item_attr;              /* ITEM的条目私有数据   */

    __sset_item_ncmsg_t         item_msg;               /* ITEM的消息码         */

    struct tag_ITEM             *father;
    struct tag_ITEM             *childlst;
    struct tag_ITEM             *prev;
    struct tag_ITEM             *next;

} __sset_item_t;
typedef enum SSET_FOCUSE_STA//..
{
    SSET_FOCUSE_NORMAL = 0,
    SSET_FOCUSE_PRESS,
    SSET_FOCUSE_STATE_MAX
} SSET_FOCUSE_STA;


/*判断sset tbar 的图层是否是隐藏还是现实状态状态*/
__bool SSET_tbar_is_show(void);

/*判断当前SSET 是否是TBAR  状态，1为是，0为不是*/
__bool SSET_court_is_tbar(void);

/*把tbar show 出来*/
void SSET_show_tbar(void);

void SSET_hide_tbar(void);
void SSET_set_curt_lyr_top(void);
__sset_item_t *_sset_menu_get_pmenu(void);
void SSET_item_update(__u32 icon_id, __u32 index, __u32 fcs_sta);




#endif  /* __SSET_H__ */
