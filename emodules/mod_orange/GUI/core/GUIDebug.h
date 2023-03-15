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
#ifndef GUI_DEBUG_H
#define GUI_DEBUG_H
#include "mod_orange_i.h"


#define GUI_DEBUG_LEVEL_NOCHECK       0  /* No run time checks are performed */
#define GUI_DEBUG_LEVEL_CHECK_PARA    1  /* Parameter checks are performed to avoid crashes */
#define GUI_DEBUG_LEVEL_CHECK_ALL     2  /* Parameter checks and consistency checks are performed */
#define GUI_DEBUG_LEVEL_LOG_ERRORS    3  /* Errors are recorded */
#define GUI_DEBUG_LEVEL_LOG_WARNINGS  4  /* Errors & Warnings are recorded */
#define GUI_DEBUG_LEVEL_LOG_ALL       5  /* Errors, Warnings and Messages are recorded. */

#ifndef GUI_DEBUG_LEVEL
#ifdef WIN32
//   #define GUI_DEBUG_LEVEL GUI_DEBUG_LEVEL_LOG_WARNINGS  /* Simulation should log all warnings */
#define GUI_DEBUG_LEVEL GUI_DEBUG_LEVEL_LOG_ALL  /* Simulation should log all warnings  log all inf  mcl */
#else
#define GUI_DEBUG_LEVEL GUI_DEBUG_LEVEL_CHECK_PARA  /* For most targets, min. size is important */
#endif
#endif

/*******************************************************************
*
*               Commandline
*
********************************************************************
*/

#ifdef WIN32
#define GUI_DEBUG_GETCMDLINE() SIM_GetCmdLine()
#else
#define GUI_DEBUG_GETCMDLINE() 0
#endif

/*******************************************************************
*
*               Error macros
*
********************************************************************
*/

/* Make sure the macros are actually defined */

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_ERRORS
#define GUI_DEBUG_ERROROUT(s)              GUI_ErrorOut(s)
#define GUI_DEBUG_ERROROUT1(s,p0)          GUI_ErrorOut1(s,p0)
#define GUI_DEBUG_ERROROUT2(s,p0,p1)       GUI_ErrorOut2(s,p0,p1)
#define GUI_DEBUG_ERROROUT3(s,p0,p1,p2)    GUI_ErrorOut3(s,p0,p1,p2)
#define GUI_DEBUG_ERROROUT4(s,p0,p1,p2,p3) GUI_ErrorOut4(s,p0,p1,p2,p3)
#define GUI_DEBUG_ERROROUT_IF(exp,s)              { if (exp) GUI_DEBUG_ERROROUT(s); }
#define GUI_DEBUG_ERROROUT1_IF(exp,s,p0)          { if (exp) GUI_DEBUG_ERROROUT1(s,p0); }
#define GUI_DEBUG_ERROROUT2_IF(exp,s,p0,p1)       { if (exp) GUI_DEBUG_ERROROUT2(s,p0,p1); }
#define GUI_DEBUG_ERROROUT3_IF(exp,s,p0,p1,p2)    { if (exp) GUI_DEBUG_ERROROUT3(s,p0,p1,p2); }
#define GUI_DEBUG_ERROROUT4_IF(exp,s,p0,p1,p2,p3) { if (exp) GUI_DEBUG_ERROROUT4(s,p0,p1,p2,p3); }
#else
#define GUI_DEBUG_ERROROUT(s)
#define GUI_DEBUG_ERROROUT1(s,p0)
#define GUI_DEBUG_ERROROUT2(s,p0,p1)
#define GUI_DEBUG_ERROROUT3(s,p0,p1,p2)
#define GUI_DEBUG_ERROROUT4(s,p0,p1,p2,p3)
#define GUI_DEBUG_ERROROUT_IF(exp,s)
#define GUI_DEBUG_ERROROUT1_IF(exp,s,p0)
#define GUI_DEBUG_ERROROUT2_IF(exp,s,p0,p1)
#define GUI_DEBUG_ERROROUT3_IF(exp,s,p0,p1,p2)
#define GUI_DEBUG_ERROROUT4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Warning macros
*
********************************************************************
*/

/* Make sure the macros are actually defined */

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_WARNINGS
#define GUI_DEBUG_WARN(s)              GUI_Warn(s)
#define GUI_DEBUG_WARN1(s,p0)          GUI_Warn1(s,p0)
#define GUI_DEBUG_WARN2(s,p0,p1)       GUI_Warn2(s,p0,p1)
#define GUI_DEBUG_WARN3(s,p0,p1,p2)    GUI_Warn3(s,p0,p1,p2)
#define GUI_DEBUG_WARN4(s,p0,p1,p2,p3) GUI_Warn4(s,p0,p1,p2,p3)
#define GUI_DEBUG_WARN_IF(exp,s)                  { if (exp) GUI_DEBUG_WARN(s); }
#define GUI_DEBUG_WARN1_IF(exp,s,p0)              { if (exp) GUI_DEBUG_WARN1(s,p0); }
#define GUI_DEBUG_WARN2_IF(exp,s,p0,p1)           { if (exp) GUI_DEBUG_WARN2(s,p0,p1); }
#define GUI_DEBUG_WARN3_IF(exp,s,p0,p1,p2)        { if (exp) GUI_DEBUG_WARN3(s,p0,p1,p2); }
#define GUI_DEBUG_WARN4_IF(exp,s,p0,p1,p2,p3)     { if (exp) GUI_DEBUG_WARN4(s,p0,p1,p2,p3); }
#else
#define GUI_DEBUG_WARN(s)
#define GUI_DEBUG_WARN1(s,p0)
#define GUI_DEBUG_WARN2(s,p0,p1)
#define GUI_DEBUG_WARN3(s,p0,p1,p2)
#define GUI_DEBUG_WARN4(s,p0,p1,p2,p3)
#define GUI_DEBUG_WARN_IF(exp,s)
#define GUI_DEBUG_WARN1_IF(exp,s,p0)
#define GUI_DEBUG_WARN2_IF(exp,s,p0,p1)
#define GUI_DEBUG_WARN3_IF(exp,s,p0,p1,p2)
#define GUI_DEBUG_WARN4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Logging macros
*
********************************************************************
*/
/* Make sure the macros are actually defined */

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_ALL
#define GUI_DEBUG_LOG(s)              GUI_Log(s)
#define GUI_DEBUG_LOG1(s,p0)          GUI_Warn1(s,p0)
#define GUI_DEBUG_LOG2(s,p0,p1)       GUI_Warn2(s,p0,p1)
#define GUI_DEBUG_LOG3(s,p0,p1,p2)    GUI_Warn3(s,p0,p1,p2)
#define GUI_DEBUG_LOG4(s,p0,p1,p2,p3) GUI_Warn4(s,p0,p1,p2,p3)
#define GUI_DEBUG_LOG_IF(exp,s)                   { if (exp) GUI_DEBUG_LOG(s); }
#define GUI_DEBUG_LOG1_IF(exp,s,p0)               { if (exp) GUI_DEBUG_LOG1(s,p0); }
#define GUI_DEBUG_LOG2_IF(exp,s,p0,p1)            { if (exp) GUI_DEBUG_LOG2(s,p0,p1); }
#define GUI_DEBUG_LOG3_IF(exp,s,p0,p1,p2)         { if (exp) GUI_DEBUG_LOG3(s,p0,p1,p2); }
#define GUI_DEBUG_LOG4_IF(exp,s,p0,p1,p2,p3)      { if (exp) GUI_DEBUG_LOG4(s,p0,p1,p2,p3); }
#else
#define GUI_DEBUG_LOG(s)
#define GUI_DEBUG_LOG1(s,p0)
#define GUI_DEBUG_LOG2(s,p0,p1)
#define GUI_DEBUG_LOG3(s,p0,p1,p2)
#define GUI_DEBUG_LOG4(s,p0,p1,p2,p3)
#define GUI_DEBUG_LOG_IF(exp,s)
#define GUI_DEBUG_LOG1_IF(exp,s,p0)
#define GUI_DEBUG_LOG2_IF(exp,s,p0,p1)
#define GUI_DEBUG_LOG3_IF(exp,s,p0,p1,p2)
#define GUI_DEBUG_LOG4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Asserts
*
********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_ERRORS
#define GUI_DEBUG_ASSERT(exp)                     { if (!exp) GUI_DEBUG_ERROROUT(#exp); }
#else
#define GUI_DEBUG_ASSERT(exp)
#endif

#endif /* LCD_H */




/*************************** End of file ****************************/
