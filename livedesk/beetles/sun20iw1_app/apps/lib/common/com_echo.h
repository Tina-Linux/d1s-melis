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
#ifndef __COM_ECHO_H__
#define __COM_ECHO_H__

#include "apps.h"

//type:AUDIO_REC_USR_MIC/AUDIO_REC_USR_FM....
//auto_clean_buf:0代表不会自动清空buffer(录音应用)，为1代表自动清空buffer(非录音的应用)
//详细调用方法参见SDK的app_root_scene.c和record_view_doc.c
__s32 com_audio_echo_open(__audio_rec_user_t type, __s32 auto_clean_buf);

//详细调用方法参见SDK的app_root_scene.c和record_view_doc.c
__s32 com_audio_echo_close(void);

//该函数一般不要调用，按照默认的就行了
//0<= pre_gain <= 3
__s32 com_audio_echo_set_mic_pre_gain(__s32 pre_gain);

//该函数一般不要调用，按照默认的就行了
//0<= gain <= 7
__s32 com_audio_echo_set_mic_gain(__s32 gain);

//增加mic音量，-32到32，可以调节mic音量
__s32 com_audio_echo_gain_incr(void);

//减小mic音量 -32到32，可以调节mic音量
__s32 com_audio_echo_gain_decr(void);

//该函数不调用，调试用
__s32 com_audio_echo_pt_regs(__u32 start, __u32 end);

/*aux: 0<= aux <= 10 设置混响延时次数，默认为3*/
__s32 com_audio_echo_set_delay_num(__s32 aux);

/*aux:10<= aux <= 3000,默认为100，以毫秒为单位*/
__s32 com_audio_echo_set_delay_time(__s32 aux);


#endif//__COM_ECHO_H__


