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
#ifndef  __DSK_VOLUME_H__
#define  __DSK_VOLUME_H__
#include <kconfig.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
****************************************************************************************************
*Name        : dsk_voice_set_volume
*Prototype   : __s32 dsk_voice_set_volume( __s32 volume)
*Arguments   : volume     input. volume value, ranges AUDIO_DEVICE_VOLUME_MIN~~AUDIO_DEVICE_VOLUME_MAX.
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : set the volume.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_volume_set(__s32 volume);

/*
****************************************************************************************************
*Name        : dsk_voice_get_volume
*Prototype   : __s32 dsk_voice_get_volume(void)
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : get the curret volume value.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_volume_get(void);

/*
****************************************************************************************************
*Name        : dsk_voice_inc_volume
*Prototype   : __s32 dsk_voice_inc_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_volume_inc(void);

/*
****************************************************************************************************
*Name        : dsk_voice_dec_volume
*Prototype   : __s32 dsk_voice_dec_volume(void);
*Arguments   : void
*Return      : the current volume value. -1 means fail.
*Description : increase the volume.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_volume_dec(void);

/*
****************************************************************************************************
*Name        : dsk_voice_on
*Prototype   : __s32 dsk_voice_on(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice on.
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_volume_on(void);

/*
****************************************************************************************************
*Name        : dsk_voice_off
*Prototype   : __s32 dsk_voice_off(void);
*Arguments   : void
*Return      : EPDK_OK    succeed
*              EPDK_FAIL  fail
*Description : make the voice off(mute).
*Other       :
****************************************************************************************************
*/
extern __s32 dsk_volume_off(void);

/*
****************************************************************************************************
*Name        : dsk_voice_is_on
*Prototype   : __bool dsk_voice_is_on( void )
*Arguments   : void
*Return      : the status of the voice, on or off(mute).
*Description : get the status of the voice, on or off(mute).
*Other       :
****************************************************************************************************
*/
extern __bool dsk_volume_is_on(void);

extern __s32 dsk_amplifier_onoff(__s32 flag);//设置功放开关

extern __bool dsk_amplifier_is_on(void);//查询功放开关
extern void dsk_hp_det_pin_det(void);
extern __s32 dsk_volume_app_line_in(__bool app_line_in);    //捷芯要求music,movie and fm声音最大比line in的小一级

#ifdef __cplusplus
}
#endif

#endif     //  ifndef __DSK_VOICE_H__

/* end of dsk_voice.h */
