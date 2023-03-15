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
