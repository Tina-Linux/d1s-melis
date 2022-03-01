/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              FM app sample
*
*                                   (c) Copyright 2006-2007, ANDY, China
*                                            All Rights Reserved
*
* File    : mbook_npl.c
* By      :
* Version : V1.00
* time    : 2009-8-14
**************************************************************************************************************
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

