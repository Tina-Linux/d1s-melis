#include <typedef.h>
#include <log.h>
#include <libc.h>
#include "ScanPri.h"
#include "Scan.h"
#include "rat_common.h"
#include "rat.h"
#include "rat_npl.h"
#include <kapi.h>

extern __krnl_event_t *g_rat_sem;

__rat_list_t *g_rat_npl[RAT_MEDIA_TYPE_MAX] = {0};

__rat_list_t g_null_rat_npl = {0};


/*
****************************************************************************************************
*
*
*
*  Description:
*               -   open npl by media type.
*
*  Parameters:
*   rat_media_type_t: media type
*
*  Return value:    npl handle
*   NULL        -
*   others      -
****************************************************************************************************
*/
HRATNPL rat_npl_open(rat_media_type_t media_type)
{
    RAT_ENTER_CRITICAL_SECTION;

    if (g_rat_npl[media_type])
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return (HRATNPL)g_rat_npl[media_type];
    }

    RAT_LEAVE_CRITICAL_SECTION;
    return (HRATNPL)RAT_NULL_NPL;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   close npl by media type.
*
*  Parameters:
*   hRatNpl:     npl handle
*
*  Return value:
*   NULL        -
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_close(HRATNPL hRatNpl)
{
    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    return EPDK_OK;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get current file index by file name.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.

*
*  Return value:
*   __s32           success, index of the file in npl
*   -1          -   fail, no such file.
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_file2index(HRATNPL hRatNpl, const char *full_file_path)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    ScanedEntry_t *pScanEntry;
    __s32 index;

    if (NULL == hRatNpl)
    {
        return -1;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    __ScanMoveCursorToFirst(rat_list->hscan, &rat_list->NplCursor);
    index = 0;

    do
    {
        pScanEntry = __ScanGetCurrentCursorData(rat_list->hscan, &rat_list->NplCursor);

        if (NULL == pScanEntry)
        {
            RAT_LEAVE_CRITICAL_SECTION;
            return -1;
        }

        if (0 == eLIBs_strcmp(full_file_path, pScanEntry->Path))
        {
            RAT_LEAVE_CRITICAL_SECTION;
            return index;
        }

        index++;
    } while (EPDK_OK == __ScanMoveCursorToNext(rat_list->hscan, &rat_list->NplCursor));

    RAT_LEAVE_CRITICAL_SECTION;
    return -1;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get full file name by index.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:    success: EPDK_OK   fail: EPDK_FAIL
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_index2file(HRATNPL hRatNpl, __u32 index, char *file)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    ScanedEntry_t *pScanEntry;
    __s32 n;

    if (NULL == hRatNpl)
    {
        return EPDK_FAIL;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return EPDK_FAIL;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0 || index >= pScanInfo->CurCnt)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return EPDK_FAIL;
    }

    __ScanMoveCursorToFirst(rat_list->hscan, &rat_list->NplCursor);
    n = 0;

    while (n < index)
    {
        if (EPDK_OK != __ScanMoveCursorToNext(rat_list->hscan, &rat_list->NplCursor))
        {
            break;
        }

        n++;
    }

    if (n == index)
    {
        pScanEntry = __ScanGetCurrentCursorData(rat_list->hscan, &rat_list->NplCursor);

        if (!pScanEntry)
        {
            file[0] = 0;
            RAT_LEAVE_CRITICAL_SECTION;
            return EPDK_FAIL;
        }
        else
        {
            eLIBs_strcpy(file, pScanEntry->Path);
            RAT_LEAVE_CRITICAL_SECTION;
            return EPDK_OK;
        }
    }
    else
    {
        file[0] = 0;
        RAT_LEAVE_CRITICAL_SECTION;
        return EPDK_FAIL;
    }
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get next index in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:    success: next index   fail: -1
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_get_next(HRATNPL hRatNpl)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    ScanedEntry_t *pScanEntry;
    __s32 n;
    __s32 ret;

    if (NULL == hRatNpl)
    {
        return -1;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0 || rat_list->npl_cur_index >= pScanInfo->CurCnt)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    n = -1;

    if (RAT_PLAY_MODE_ONLY_ONCE == rat_list->play_mode)
    {
        if (-1 == rat_list->npl_cur_index)
        {
            rat_list->npl_cur_index = 0;
        }

        n = -1;
    }
    else if (RAT_PLAY_MODE_ROTATE_ONE == rat_list->play_mode)
    {
        if (-1 == rat_list->npl_cur_index)
        {
            rat_list->npl_cur_index = 0;
        }

        n = rat_list->npl_cur_index;
    }
    else if (RAT_PLAY_MODE_ROTATE_ALL == rat_list->play_mode)
    {
        if (rat_list->npl_cur_index < pScanInfo->CurCnt - 1)
        {
            rat_list->npl_cur_index++;
        }
        else
        {
            __s32 count;
            count = ScanFilesOneTime((ScanInfo_t *)(rat_list->hscan));

            if (count > 0)
            {
                rat_list->npl_cur_index++;
            }
            else
            {
                rat_list->npl_cur_index = 0;
            }
        }

        n = rat_list->npl_cur_index;
    }
    else if (RAT_PLAY_MODE_SEQUENCE == rat_list->play_mode)
    {
        if (rat_list->npl_cur_index < pScanInfo->CurCnt - 1)
        {
            rat_list->npl_cur_index++;
            n = rat_list->npl_cur_index;
        }
        else
        {
            __s32 count;
            count = ScanFilesOneTime((ScanInfo_t *)(rat_list->hscan));

            if (count > 0)
            {
                rat_list->npl_cur_index++;
                n = rat_list->npl_cur_index;
            }
            else
            {
                //rat_list->npl_cur_index = -1;
                n = -1;
            }
        }
    }
    else
    {
        rat_list->npl_cur_index = eLIBs_ramdom(pScanInfo->CurCnt);

        if (rat_list->npl_cur_index < 0)
        {
            rat_list->npl_cur_index = 0;
        }

        if (rat_list->npl_cur_index > pScanInfo->CurCnt - 1)
        {
            rat_list->npl_cur_index = pScanInfo->CurCnt - 1;
        }

        n = rat_list->npl_cur_index;
    }

    RAT_LEAVE_CRITICAL_SECTION;
    return n;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get cur index in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:    success: cur index   fail: -1
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_get_cur(HRATNPL hRatNpl)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    ScanedEntry_t *pScanEntry;
    __s32 n;
    __s32 ret;

    if (NULL == hRatNpl)
    {
        return -1;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0 || rat_list->npl_cur_index >= pScanInfo->CurCnt)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    ret = rat_list->npl_cur_index;
    RAT_LEAVE_CRITICAL_SECTION;
    return ret;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get prev index in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:
*   __s32       -  prev index in npl
*   -1          -  no item
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_get_prev(HRATNPL hRatNpl)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    ScanedEntry_t *pScanEntry;
    __s32 n;
    __s32 ret;

    if (NULL == hRatNpl)
    {
        return -1;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0 || rat_list->npl_cur_index >= pScanInfo->CurCnt)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    n = -1;

    if (RAT_PLAY_MODE_ONLY_ONCE == rat_list->play_mode)
    {
        n = -1;
    }
    else if (RAT_PLAY_MODE_ROTATE_ONE == rat_list->play_mode)
    {
        if (-1 == rat_list->npl_cur_index)
        {
            rat_list->npl_cur_index = 0;
        }

        n = rat_list->npl_cur_index;
    }
    else if (RAT_PLAY_MODE_ROTATE_ALL == rat_list->play_mode)
    {
        rat_list->npl_cur_index--;

        if (rat_list->npl_cur_index < 0)
        {
            rat_list->npl_cur_index = 0;
            n = -1;
        }
        else
        {
            n = rat_list->npl_cur_index;
        }
    }
    else if (RAT_PLAY_MODE_SEQUENCE == rat_list->play_mode)
    {
        rat_list->npl_cur_index--;

        if (rat_list->npl_cur_index < 0)
        {
            rat_list->npl_cur_index = 0;
            n = -1;
        }
        else
        {
            n = rat_list->npl_cur_index;
        }
    }
    else
    {
        rat_list->npl_cur_index = eLIBs_ramdom(pScanInfo->CurCnt - 1);
        n = rat_list->npl_cur_index;
    }

    RAT_LEAVE_CRITICAL_SECTION;
    return n;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   set cur index in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:    success: EPDK_OK   fail: EPDK_FAIL
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_set_cur(HRATNPL hRatNpl, __s32 index)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    ScanedEntry_t *pScanEntry;
    __s32 n;
    __s32 ret;

    if (NULL == hRatNpl)
    {
        return -1;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);

    if (pScanInfo->CurCnt <= 0 || index >= pScanInfo->CurCnt)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    rat_list->npl_cur_index = index;
    RAT_LEAVE_CRITICAL_SECTION;
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   set play mode in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*   play_mode   -   play mode
*
*  Return value:    success: EPDK_OK   fail: EPDK_FAIL
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_set_play_mode(HRATNPL hRatNpl, rat_play_mode_e play_mode)
{
    __rat_list_t *rat_list = NULL;

    if (NULL == hRatNpl)
    {
        return EPDK_FAIL;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return EPDK_FAIL;
    }

    rat_list->play_mode = play_mode;
    RAT_LEAVE_CRITICAL_SECTION;
    return EPDK_OK;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get play mode in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:    success: EPDK_OK   fail: EPDK_FAIL
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_get_play_mode(HRATNPL hRatNpl)
{
    __rat_list_t *rat_list;
    rat_play_mode_e play_mode;

    if (NULL == hRatNpl)
    {
        return RAT_PLAY_MODE_ONLY_ONCE;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return RAT_PLAY_MODE_ONLY_ONCE;
    }

    play_mode = rat_list->play_mode;
    RAT_LEAVE_CRITICAL_SECTION;
    return play_mode;
}

/*
****************************************************************************************************
*
*
*
*  Description:
*               -   get total count in npl.
*
*  Parameters:
*   hRatNpl     -   handle to npl information.
*
*  Return value:    total count in npl
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_get_total_count(HRATNPL hRatNpl)
{
    __rat_list_t *rat_list;
    ScanInfo_t *pScanInfo;
    __s32 ret;

    if (NULL == hRatNpl)
    {
        return -1;
    }

    if (RAT_NULL_NPL == (unsigned long)hRatNpl)
    {
        return -1;
    }

    RAT_ENTER_CRITICAL_SECTION;
    rat_list = (__rat_list_t *)hRatNpl;

    if (NULL == rat_list->hscan)
    {
        RAT_LEAVE_CRITICAL_SECTION;
        return -1;
    }

    pScanInfo = (ScanInfo_t *)(rat_list->hscan);
    ret = pScanInfo->CurCnt;
    RAT_LEAVE_CRITICAL_SECTION;
    return ret;
}

