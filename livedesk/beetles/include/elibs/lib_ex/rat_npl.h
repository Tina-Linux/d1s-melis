#ifndef __RAT_NPL_H__
#define __RAT_NPL_H__

#define RAT_NULL_NPL ((unsigned long)&g_null_rat_npl)

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
HRATNPL rat_npl_open(rat_media_type_t media_type);

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
__s32 rat_npl_close(HRATNPL hRatNpl);

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
*   NULL        -   fail, no such file.
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_file2index(HRATNPL hRatNpl, const char *full_file_path);

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
__s32 rat_npl_index2file(HRATNPL hRatNpl, __u32 index, char *file);

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
__s32 rat_npl_get_next(HRATNPL hRatNpl);

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
__s32 rat_npl_get_cur(HRATNPL hRatNpl);

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
*   NULL        -   .
*   others      -
****************************************************************************************************
*/
__s32 rat_npl_get_prev(HRATNPL hRatNpl);

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
__s32 rat_npl_set_cur(HRATNPL hRatNpl, __s32 index);

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
__s32 rat_npl_set_play_mode(HRATNPL hRatNpl, rat_play_mode_e play_mode);

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
__s32 rat_npl_get_play_mode(HRATNPL hRatNpl);

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
__s32 rat_npl_get_total_count(HRATNPL hRatNpl);

#endif//__RAT_NPL_H__
