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
