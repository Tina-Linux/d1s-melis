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
#ifndef  __robin_h__
#define  __robin_h__


#include "robin/ab/robin_ab.h"
#include "robin/channel/robin_channel.h"
#include "robin/eq/robin_eq.h"
#include "robin/ff_rr_speed/robin_ff_rr_speed.h"
#include "robin/fsm_ctrl/robin_fsm_ctrl.h"
#include "robin/lyric/robin_lyric.h"
#include "robin/media_info/robin_media_info.h"
#include "robin/npl/robin_npl.h"
#include "robin/play_mode/robin_play_mode.h"
#include "robin/play_speed/robin_play_speed.h"
#include "robin/spectrum/robin_spectrum.h"
#include "robin/star/robin_star.h"
#include "robin/subtitle/robin_subtitle.h"
#include "robin/track/robin_track.h"
#include "robin/video_layer/robin_video_layer.h"
#include "robin/video_win/robin_video_win.h"
#include "robin/vision_effect/robin_vision_effect.h"
#include "robin/volume/robin_volume.h"
#include "robin/zoom/robin_zoom.h"
#include "robin/disp_output/robin_disp_output.h"
#include "robin/palette/robin_palette.h"


typedef enum
{
    ROBIN_MODE_AUDIO_MIN  = 0,
    ROBIN_MODE_AUDIO_MAX,
    ROBIN_MODE_VIDEO_MIN,
    ROBIN_MODE_VIDEO_MAX,

    ROBIN_MODE_UNKNOWN    = -1
} robin_mode_e;

typedef struct
{
    __u32                   reserve_mem_size;  //指定cedar预留给应用的内存，以字节为单位，一般为500K-1M就足够了
    CedarStopMode           cedar_stop_mode;   //CEDAR_STOP_MODE_KEEP_PLUGINS:停止播放时保留所有插件，CEDAR_STOP_MODE_UNINSTALL_PLUGINS停止播放时卸载所有插件
    CedarFileSwitchVplyMode switch_vplay_mode; // CEDAR_FILE_SWITCH_VPLY_MODE_GAP:2个文件切换时，隔断的，中间有黑屏, CEDAR_FILE_SWITCH_VPLY_MODE_SEAMLESS:无缝, 不黑屏.


} robin_open_arg_t;


/***************************************************************************************************
*Name        : robin_open
*Prototype   : __s32  robin_open( robin_mode_e robin_mode, void *arg_p )
*Arguments   : robin_mode    input. the work mode of the cuckoo_core module.
*              arg_p       input. arguments, if NULL，default value will be set.
*Return      : ==0    succeed
*              !=0    fail
*Description : open cuckoo_core.
*Other       :
***************************************************************************************************/
extern __s32  robin_open(robin_mode_e robin_mode, robin_open_arg_t *arg_p);

/***************************************************************************************************
*Name        : robin_close
*Prototype   : void  robin_close( void )
*Arguments   : void
*Return      : void
*Description : close cuckoo_core.
*Other       :
***************************************************************************************************/
extern void  robin_close(void);

/***************************************************************************************************
*Name        : robin_set_mode
*Prototype   : __s32  robin_set_mode( robin_mode_e robin_mode )
*Arguments   : robin_mode    input. the work mode of the cuckoo_core module.
*Return      : ==0    succeed
*              !=0    fail
*Description : set the work mode of the cuckoo_core module.
*Other       :
***************************************************************************************************/
extern __s32  robin_set_mode(robin_mode_e robin_mode, robin_open_arg_t *arg_p);

/***************************************************************************************************
*Name        : robin_get_mode
*Prototype   : robin_mode_e  robin_get_mode( void )
*Arguments   : void
*Return      : the work mode of the cuckoo_core module.
*Description : get the work mode of the cuckoo_core module.
*Other       :
***************************************************************************************************/
extern robin_mode_e  robin_get_mode(void);

extern __bool robin_is_open(void);


#endif     //  ifndef __robin_h__

/* end of __robin_h__.h */
