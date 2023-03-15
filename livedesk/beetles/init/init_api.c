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
#include "mod_init_i.h"
#include <kconfig.h>

/* gscene_backgrd.h */
__attribute__((section("INIT_API_TBL"))) const init_api_entry_t init_api =
{
    gscene_bgd_init,
    gscene_bgd_set_state,
    gscene_bgd_get_state,
    gscene_bgd_set_top,
    gscene_bgd_set_bottom,
    gscene_bgd_save_fb,
    gscene_bgd_set_fb_type,
    gscene_bgd_refresh,
    gscene_bgd_restore,
    gscene_bgd_deinit,
    gscene_bgd_get_flag,
    gscene_bgd_set_flag,
    gscene_bgd_get_default_bg_index,
    gscene_bgd_set_default_bg_index,
    {
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
    }
,

    /* gscene_headbar.h */
    gscene_hbar_create,
    gscene_hbar_set_state,
    gscene_hbar_get_state,
    gscene_hbar_set_title,
    gscene_hbar_set_sig_status,
    gscene_hbar_get_screen_rect,
    gscene_hbar_delete,
    gscene_hbar_set_music_state,
    gscene_hbar_update_text,
	{
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
    },

    //display\g_display_switch_output.h
    g_display_switch_output,
    {
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
    },

    //display\g_close_screen.h
    g_enable_close_scn,
    g_disable_close_scn,
    g_set_close_scn_time,
    g_get_close_scn_time,
    g_close_screen,
    g_enable_standby,
    g_disable_standby,
    g_is_enable_standby,
	{
    	NULL,
    },

    //mini_music\mini_music.h
    mini_music_open,
    mini_music_close,
    mini_music_scene_exist,

    //bookengine.h
    bookengine_open,
    bookengine_close,
    //tp_adjust_scene.h
    scene_adjust_create,
    scene_adjust_destroy,
    //alarm_function.h
    alarm_is_exist,
    alarm_get_happening,
    alarm_check_run,
    alarm_settingTime_cleanHappen,
    standby_startup_alarm,
    alarm_enter_app,
    alarm_exit_app,

    init_reset_close_scn,
    init_reset_auto_off,
};
