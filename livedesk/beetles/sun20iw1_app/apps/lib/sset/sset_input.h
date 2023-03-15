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
#ifndef __SSET_INPUT_H__
#define __SSET_INPUT_H__

/*
*************************************************************************************
*                                       SSET_ItemAttr_Create
*
*Description: SSET内部参数数据创建
*
*Arguments  : para  ：用户输入数据头指针
*
*Return     : 内部数据结构头指针
*
*************************************************************************************
*/
__sset_item_t *SSET_ItemAttr_Create(__sset_item_para_t  *para);

/*
*************************************************************************************
*                                       SSET_ItemAttr_Destroy
*
*Description: SSET内部参数数据销毁
*
*Arguments  : phead  : 内部数据指针
*
*Return     : NULL
*
*************************************************************************************
*/
__s32 SSET_ItemAttr_Destroy(__sset_item_t *phead);

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Insert
*
*Description: 插入一组条目
*
*Arguments  : pitem 要插入条目的插入节点
*             para  插入条目的头指针
*             num   插入的条目数
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_ItemAttr_Insert(__sset_item_t *pitem, __sset_item_para_t *para, __u32 num, __sset_item_insert_mode_e mode);

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Delete
*
*Description: 删除一组条目
*
*Arguments  : pitem 删除条目所挂载的节点
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_ItemAttr_Delete(__sset_item_t *pitem, __sset_item_delete_mode_e mode);

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Search
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__sset_item_t *SSET_ItemAttr_Search(char *name, __sset_item_t *pitem);

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Ennable
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_ItemAttr_Ennable(__sset_item_t *pitem);

/*
************************************************************************************************************************
*                                       SSET_ItemAttr_Disable
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_ItemAttr_Disable(__sset_item_t *pitem);

/*
************************************************************************************************************************
*                                       SSET_Item_SetAttr
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_Item_SetAttr(__sset_item_t *pitem, __sset_item_attr_t *attr);

/*
************************************************************************************************************************
*                                       SSET_Item_GetAttr
*
*Description: 释放用户数据所占内存
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_Item_GetAttr(__sset_item_t *pitem, __sset_item_attr_t *attr);


#endif  /* __SSET_INPUT_H__ */
