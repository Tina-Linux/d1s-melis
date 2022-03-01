/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : layer.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:28:56
**********************************************************************************************************************
*/

#ifndef __APP_LOAD_PARA_H__
#define __APP_LOAD_PARA_H__


#define SEARCH_MAX_KEYWORD_LEN   1024


/**********************************************************************************************************************/
/* explorer 加载参数 */
/**********************************************************************************************************************/

/*!
  * \brief
  *     加载类型
*/
typedef enum
{
    EXPLR_APP_MAIN,
    EXPLR_APP_MUSIC,
    EXPLR_APP_PHOTO,
    EXPLR_APP_MOVIE,
    EXPLR_APP_EBOOK,
    EXPLR_APP_GAME,
    EXPLR_APP_SEARCH,
    EXPLR_APP_FLASH,
    EXPLR_APP_ALL
} explr_type_t;
/*!
  * \brief
  *     explorer加载模式枚举
*/
typedef enum
{
    /*! 从根浏览*/
    EXPLR_APP_ROOT,
    /*! 恢复浏览*/
    EXPLR_APP_RESTORE
} explr_mode_t;
/*!
  * \brief
  *     explorer的加载参数
*/
typedef struct
{
    /*! 应用类型*/
    explr_type_t type;
    /*! 加载模式*/
    explr_mode_t mode;
    /*! 是否需要文件操作：0不要，1，要*/
    __u32        file_op_flag;
    /*! 是否需要显示缩略图 0不要，1要*/
    __u32 miniature_flag;

    __u32 search_type;
    char  search_word[SEARCH_MAX_KEYWORD_LEN];
    void        *attr;
} explr_arg_t;

#endif /* __APP_LOAD_PARA_H__ */
