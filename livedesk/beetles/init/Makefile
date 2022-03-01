ccflags-y   +=  -DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL         \
                -DUSED_BY_INIT                      \
                $(SOLUTION_INCLUDE) 

usrlibs-y   +=  -L$(srctree)/${elibrary-libs}/      \
				--start-group                       \
				-lsyscall   -llzma                  \
				-lminic     -lpub0                  \
				--end-group
#-lbsposal 
MOD_NAME    := init
SUF_NAME    := axf
PRJ_PATH    := $(MELIS_BASE)/projects/${TARGET_BOARD}/data/UDISK/apps
TEMP_PATH   := $(MELIS_BASE)/emodules/bin

ifeq ($(CONFIG_SOC_SUN3IW2),y)
$(MOD_NAME)-objs += alarm/alarm_function.o
$(MOD_NAME)-objs += headbar/prog_bar/prog_bar.o
$(MOD_NAME)-objs += headbar/prog_bar/prog_bar_ui.o

$(MOD_NAME)-objs += init_mod.o
$(MOD_NAME)-objs += headbar/gscene_headbar.o
$(MOD_NAME)-objs += headbar/headbar_com.o
$(MOD_NAME)-objs += headbar/headbar_uipara.o
$(MOD_NAME)-objs += ui/init_server.o
endif

ifeq ($(CONFIG_SOC_SUN3IW1),y)
$(MOD_NAME)-objs += init_mod_1663.o
$(MOD_NAME)-objs += headbar/gscene_headbar_1663.o
$(MOD_NAME)-objs += headbar/headbar_com_1663.o
$(MOD_NAME)-objs += headbar/headbar_uipara_1663.o
$(MOD_NAME)-objs += ui/init_server_1663.o
endif


ifeq ($(CONFIG_SOC_SUN20IW1),y)
$(MOD_NAME)-objs += alarm/alarm_function.o
$(MOD_NAME)-objs += headbar/prog_bar/prog_bar.o
$(MOD_NAME)-objs += headbar/prog_bar/prog_bar_ui.o

$(MOD_NAME)-objs += init_mod.o
$(MOD_NAME)-objs += headbar/gscene_headbar.o
$(MOD_NAME)-objs += headbar/headbar_com.o
$(MOD_NAME)-objs += headbar/headbar_uipara.o
$(MOD_NAME)-objs += ui/init_server.o
endif

$(MOD_NAME)-objs += init_api.o
$(MOD_NAME)-objs += mod_init.o
$(MOD_NAME)-objs += magic.o
$(MOD_NAME)-objs += background/gscene_backgrd.o
$(MOD_NAME)-objs += background/fb_lib/backlayer_lib.o
$(MOD_NAME)-objs += background/fb_lib/fb_file.o
$(MOD_NAME)-objs += bookengine/bookengine.o
$(MOD_NAME)-objs += display/g_close_screen.o
$(MOD_NAME)-objs += display/g_display_switch_output.o
$(MOD_NAME)-objs += mini_music/mini_music.o
$(MOD_NAME)-objs += mini_music/mini_music_scene.o
$(MOD_NAME)-objs += mini_music/mini_music_uipara.o
$(MOD_NAME)-objs += screen_lock/scnlock_uipara.o
$(MOD_NAME)-objs += screen_lock/screen_lock.o
$(MOD_NAME)-objs += tp_adjust/tp_adjust.o
$(MOD_NAME)-objs += ui/live_init.o
$(MOD_NAME)-objs += ui/assistant_scene/assistant_scene.o
$(MOD_NAME)-objs += ui/dialog_scene/dialog_scene.o
$(MOD_NAME)-objs += ui/dialog_scene/dialog_uipara.o
$(MOD_NAME)-objs += ui/dialog_scene/progress_dialog.o
$(MOD_NAME)-objs += ui/tp_adjust_scene/scene_adjust.o
$(MOD_NAME)-objs += ui/tp_adjust_scene/scene_adjust_uipara.o

include $(MELIS_BASE)/scripts/Makefile.mods
