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
#include "mbook_private.h"

#if 0

/*
**********************************************************************************************************************
*                                               MBOOK_Npl_GetCur
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Npl_GetFilename(void *core, char *filename)
{
    __s32           ret;
    __s32           index;
    cat_core_control(core, CAT_SET_FILE_SOURCE_NPL, (__s32)CAT_SOURCE_CUR_DIR, 0);
    ret = cat_core_control(core, CAT_GET_CUR_PLAY_FILENAME_NPL, (__s32)filename, 0);
    return ret;
}
/*
**********************************************************************************************************************
*                                               MBOOK_Npl_GetCur
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Npl_GetCur(void *core)
{
    __s32           ret;
    __s32           index;
    cat_core_control(core, CAT_SET_FILE_SOURCE_NPL, (__s32)CAT_SOURCE_CUR_DIR, 0);
    ret = cat_core_control(core, CAT_GET_CUR_NUM_NPL, (__s32)&index, 0);
    return index;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Npl_FileToIndex
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Npl_FileToIndex(void *core, const char *file)
{
    __s32           index;
    cat_core_control(core, CAT_SET_FILE_SOURCE_NPL, (__s32)CAT_SOURCE_CUR_DIR, 0);
    cat_core_control(core, CAT_PATH_2_INDEX_NPL, (__s32)file, (__s32)&index);
    return index;
}

/*
**********************************************************************************************************************
*                                               MBOOK_Npl_IndexToFile
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   MBOOK_Npl_IndexToFile(void *core, __u32 index, char *file)
{
    __s32                   ret;
    ret =  cat_core_control(core, CAT_INDEX_2_PATH_NPL, (__s32)file, (__s32)index);
    return ret;
}
#endif
