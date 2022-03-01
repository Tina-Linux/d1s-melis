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
    },

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
