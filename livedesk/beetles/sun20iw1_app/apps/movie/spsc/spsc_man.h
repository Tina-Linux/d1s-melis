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
#ifndef __MOVIE_SPSC_H__
#define  __MOVIE_SPSC_H__

#include "beetles_app.h"
#include "spsc_ui.h"

//spsc_scene发往父窗口的消息
typedef enum
{
    spsc_scene_msg_timeout = 0,
    spsc_scene_msg_rr,
    spsc_scene_msg_infor,
    spsc_scene_msg_tvout,
    spsc_scene_msg_switch,
    spsc_scene_msg_num
} movie_spsc_scene_msg_t;

//父窗口发往该窗口的消息
typedef enum
{
    movie_spsc_scene_update_fileinfo = GUI_MSG_ + 1

} movie_spsc_scene_recv_msg_t;


typedef struct
{
    H_WIN hparent;
    __s32 scene_id;
    __s32 sub_state;
} movie_spsc_create_para_t;

void *movie_spsc_scene_create(movie_spsc_create_para_t *create_para);
__s32 movie_spsc_scene_delete(void *handle);
__s32 movie_spsc_scene_suspend(void *handle);
__s32 movie_spsc_scene_reseum(void *handle);
H_WIN movie_spsc_scene_get_hwnd(void *handle);
__s32 movie_spsc_scene_get_lyr_scn(RECT *rect);
__s32 __movie_spsc_hide_layer_win(void *handle);

#endif
