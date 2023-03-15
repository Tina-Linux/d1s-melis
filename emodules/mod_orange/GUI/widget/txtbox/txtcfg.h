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
#ifndef ___CONFIG__H__
#define ___CONFIG__H__      1





//------------------------------------------------------------------------------------------------------------
// define constant
//------------------------------------------------------------------------------------------------------------
#ifndef OK
#define OK                  0
#endif //OK

#ifndef TRUE
#define TRUE                    1
#endif //OK

#ifndef FALSE
#define FALSE                   0
#endif //OK

#define TXT_WHITE       { 255, 255, 255, 255 }
#define TXT_BLACK       {   0,   0,   0, 255 }
#define TXT_RED         {   0,   0, 255, 255 }
#define TXT_GREEN       {   0, 255,   0, 255 }
#define TXT_BLUE        { 255,   0,   0, 255 }

//------------------------------------------------------------------------------------------------------------
// config marco
//------------------------------------------------------------------------------------------------------------

#define SCOTT_DEBUG 1       // open debug function

//#define DOUBLE_FRAMEBUFFER


//#define  TXTMENU_DEBUG
#ifndef SIM_PC_WIN
#ifdef  TXTMENU_DEBUG
#define TXTMENU_PRINTF    __inf
#else
#define TXTMENU_PRINTF(...)
#endif


//#define  SYSTEM_DEBUG

#ifdef  SYSTEM_DEBUG
#define SYSTEM_PRINTF    __inf
#else
#define SYSTEM_PRINTF(...)
#endif

//#define  ITEM_DEBUG

#ifdef  ITEM_DEBUG
#define ITEM_PRINTF    __inf
#else
#define ITEM_PRINTF(...)
#endif


//#define  POS_DEBUG

#ifdef  POS_DEBUG
#define POS_PRINTF    __inf
#else
#define POS_PRINTF(...)
#endif

#define __HERE__
#define Msg(...)
#define Err(...)
#ifndef NULL
#define NULL    0
#endif // NULL
#else

#ifdef  TXTMENU_DEBUG
#define TXTMENU_PRINTF    __inf
#else
#define TXTMENU_PRINTF   __inf
#endif


//#define  SYSTEM_DEBUG

#ifdef  SYSTEM_DEBUG
#define SYSTEM_PRINTF    __inf
#else
#define SYSTEM_PRINTF  __inf
#endif

//#define  ITEM_DEBUG

#ifdef  ITEM_DEBUG
#define ITEM_PRINTF    __inf
#else
#define ITEM_PRINTF   __inf
#endif


//#define  POS_DEBUG

#ifdef  POS_DEBUG
#define POS_PRINTF    __inf
#else
#define POS_PRINTF  __inf
#endif

#define __HERE__
#define Msg
#define Err
#ifndef NULL
#define NULL    0
#endif // NULL

#endif
//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //___CONFIG__H__
