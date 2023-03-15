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
#ifndef  __robin_ab_h
#define  __robin_ab_h

#include <kconfig.h>

/***************************************************************************************************
*Name        : robin_set_ab_a
*Prototype   : __s32  robin_set_ab_a( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the A point in AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_a(void);
extern __s32  robin_set_ab_a_v2(void);


/***************************************************************************************************
*Name        : robin_set_ab_b
*Prototype   : __s32  robin_set_ab_b( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : set the current time point as the B point in AB play mode and start AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_b(void);
extern __s32  robin_set_ab_b_v2(void);

/***************************************************************************************************
*Name        : robin_set_ab_loop_count
*Prototype   : __s32  robin_set_ab_loop_count( __u32 count )
*Arguments   : count     input. AB loop count.
*Return      : ==0       succeed
*              !=0       fail
*Description : set the loop count in AB play mode.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_ab_loop_count(__u32 count);
extern __s32  robin_set_ab_loop_count_v2(__u32 count);

/***************************************************************************************************
*Name        : robin_cancle_ab
*Prototype   : __s32  robin_cancle_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : cancle AB play.
*Other       :
***************************************************************************************************/
extern __s32  robin_cancle_ab(void);
extern __s32  robin_cancle_ab_v2(void);
/***************************************************************************************************
*Name        : robin_enable_ab
*Prototype   : __s32  robin_enable_ab( void )
*Arguments   : void
*Return      : ==0       succeed
*              !=0       fail
*Description : enable AB play.
*Other       :
***************************************************************************************************/
extern __s32  robin_enable_ab(void);
extern __s32  robin_set_audio_ab_mode(__u32 mode);


#endif     //  ifndef __robin_ab_h
