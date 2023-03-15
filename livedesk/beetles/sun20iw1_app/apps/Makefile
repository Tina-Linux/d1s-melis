APPS_PATH=$(MELIS_BASE)/${SOLUTION}/beetles/sun20iw1_app/apps

ccflags-y   +=  -I${APPS_PATH}/lib                                          \
				-I${APPS_PATH}/lib/app_loading                              \
				-I${APPS_PATH}/explorer/FileList                            \
				-I${APPS_PATH}/explorer                                     \
				-I${APPS_PATH}                                              \
				-I${APPS_PATH}/record/record_frame/record_disk_view         \
				-I${APPS_PATH}/record/record_frame/record_list_view         \
				-I${APPS_PATH}/record                                       \
				-I${APPS_PATH}/record/record_frame/record_view              \
				-I${APPS_PATH}/record/record_doc/record_list_view           \
				-I${APPS_PATH}/record/record_ui/record_list_view            \
				-I${APPS_PATH}/record/record_doc/record_view                \
				-I${APPS_PATH}/record/record_ui                             \
				-I${APPS_PATH}/record/record_ui/record_view                 \
				-I${APPS_PATH}/multi_screen_home/ui                         \
				-I${APPS_PATH}/multi_screen_home/desktop_scene              \
				-I${APPS_PATH}/multi_screen_home                            \
				-I${APPS_PATH}/ebook                                        \
				-I${APPS_PATH}/movie                                        \
				-I${APPS_PATH}/music                                        \
				-I${APPS_PATH}/music/widget                                 \
				-I${APPS_PATH}/music/spctrm                                 \
				-I${APPS_PATH}/music/MusicPlayer                            \
				-I${APPS_PATH}/music/MusicMenu                              \
				-I${APPS_PATH}/music/widget/spectrum                        \
				-I${APPS_PATH}/dtv                                          \
				-I${APPS_PATH}/dtv/program_preview/                         \
				-I${APPS_PATH}/dtv/schedule/                                \
				-I${APPS_PATH}/dtv/search_process                           \
				-I${APPS_PATH}/dtv/setting                                  \
				-I${APPS_PATH}/dtv/subtitle                                 \
				-I${APPS_PATH}/play                                         \
				-I$(APPS_PATH)/mtp                                          \
				-I$(APPS_PATH)/mtp/Disk                                     \
				-I${APPS_PATH}/mjpg_decode                                  \
				-I${APPS_PATH}/video_thumb                                  \
				-I${srctree}/ekernel/drivers/include/hal/                   \
				-DEPDK_DEBUG_LEVEL=EPDK_DEBUG_LEVEL_LOG_ALL                 \
				-fshort-enums                                               \
				$(SOLUTION_INCLUDE)
ifdef CONFIG_DRIVERS_USB_GADGET_MTP
ccflags-y   +=	-I$(APPS_PATH)/mtp                                          \
				-I$(APPS_PATH)/mtp/Disk
endif
usrlibs-y +=	-L$(MELIS_BASE)/${elibrary-libs}/   \
				--start-group                       \
				-lsyscall   -llzma                  \
				-lminic     -lpub0                  \
				-lcharenc   -lmediainfo             \
				-lcharset                           \
				-lelibs_ex  -lapp_views             \
				--end-group

# now there is not the following libraries, mark them here for paying attention
#				-lejTTS     -lts_all_rec            \

MOD_NAME    := app_root
SUF_NAME    := axf
PRJ_PATH    := $(MELIS_BASE)/projects/${TARGET_BOARD}/data/UDISK/apps/
TEMP_PATH   := $(MELIS_BASE)/emodules/bin

$(MOD_NAME)-objs    +=	./app_root/app_root.o                                   \
                        ./app_root/app_root_init.o                              \
                        ./app_root/app_root_scene.o                             \
                        ./dtv/dtv.o                                             \
                        ./dtv/dtv_func_interface.o                              \
                        ./dtv/dtv_mid.o                                         \
                        ./dtv/dtv_uipara.o                                      \
                        ./dtv/program_preview/program_preview.o                 \
                        ./dtv/program_preview/program_preview_list.o            \
                        ./dtv/program_preview/program_preview_uipara.o          \
                        ./dtv/schedule/schedule.o                               \
                        ./dtv/schedule/schedule_list.o                          \
                        ./dtv/schedule/schedule_uipara.o                        \
                        ./dtv/search_process/scan_proc.o                        \
                        ./dtv/search_process/scan_proc_uipara.o                 \
                        ./dtv/setting/dtv_rolling_txt.o                         \
                        ./dtv/setting/dtv_setting.o                             \
                        ./dtv/setting/dtv_setting_antenna.o                     \
                        ./dtv/setting/dtv_setting_ca.o                          \
                        ./dtv/setting/dtv_setting_display.o                     \
                        ./dtv/setting/dtv_setting_lcd.o                         \
                        ./dtv/setting/dtv_setting_list.o                        \
                        ./dtv/setting/dtv_setting_manual_search.o               \
                        ./dtv/setting/dtv_setting_media_ca.o                    \
                        ./dtv/setting/dtv_setting_uipara.o                      \
                        ./dtv/subtitle/dtv_subtitle_ui.o                        \
                        ./ebook/tts/jt_tts_char.o                               \
                        ./ebook/tts/sft_decode.o                                \
                        ./ebook/tts/jt_tts_analysis_data.o                      \
                        ./ebook/tts/jt_tts_reg.o                                \
                        ./ebook/tts/jt_tts.o                                    \
                        ./ebook/ebook_uipara.o                                  \
                        ./ebook/page_com.o                                      \
                        ./ebook/app_ebook.o                                     \
                        ./ebook/ebook_tips.o                                    \
                        ./ebook/mbook/mbook_font.o                              \
                        ./ebook/mbook/mbook_layer.o                             \
                        ./ebook/mbook/mbook_decode.o                            \
                        ./ebook/mbook/mbook_analysis.o                          \
                        ./ebook/mbook/mbook_show.o                              \
                        ./ebook/mbook/mbook_npl.o                               \
                        ./ebook/ebook_sset.o                                    \
                        ./explorer/Explorer_UI.o                                \
                        ./explorer/Explorer.o                                   \
                        ./explorer/explorer_dialog_frame.o                      \
                        ./explorer/FileList/FileList.o                          \
                        ./explorer/Explorer_List.o                              \
                        ./fm/app_fm.o                                           \
                        ./fm/fm_sset.o                                          \
                        ./fm/fm_main.o                                          \
                        ./fm/fm_uipara.o                                        \
                        ./fm/fm_ScaleBar.o                                      \
                        ./lib/app_alarm/app_alarm.o                             \
                        ./lib/app_dialog/app_dialog.o                           \
                        ./lib/app_loading/app_loading.o                         \
                        ./lib/app_loading/app_loading_uipara.o                  \
                        ./lib/common/APP_Debug.o                                \
                        ./lib/common/APP_Mem.o                                  \
                        ./lib/common/com_echo.o                                 \
                        ./lib/common/mem_dev.o                                  \
                        ./lib/common/misc.o                                     \
                        ./lib/common/palette.o                                  \
                        ./lib/common/touch_adapter.o                            \
                        ./lib/fifo_db/fifo_db.o                                 \
                        ./lib/sset/sset.o                                       \
                        ./lib/sset/sset_input.o                                 \
                        ./lib/sset/sset_menu_info.o                             \
                        ./lib/sset/sset_menu_lstm.o                             \
                        ./lib/sset/sset_menu_tbar.o                             \
                        ./lib/sset/sset_uipara.o                                \
                        ./lib/sset/toolbar/tbar_com.o                           \
                        ./lib/sset/toolbar/GlideCurve.o                         \
                        ./mjpg_decode/jpeg_dec.o                                \
                        ./movie/app_movie.o                                     \
                        ./movie/common/common.o                                 \
                        ./movie/ff_rr_show/ff_rr_show.o                         \
                        ./movie/ff_rr_show/ff_rr_show_ui.o                      \
                        ./movie/play_sta/play_sta.o                             \
                        ./movie/play_sta/play_sta_ui.o                          \
                        ./movie/prog/prog.o                                     \
                        ./movie/prog/prog_ui.o                                  \
                        ./movie/sub_show/sub_show.o                             \
                        ./movie/sub_show/sub_show_ui.o                          \
                        ./movie/sset/movie_sset.o                               \
                        ./movie/spsc/spsc_ctrl/spsc_ctrl.o                      \
                        ./movie/spsc/spsc_ctrl/spsc_ctrl_ui.o                   \
                        ./movie/spsc/spsc_man.o                                 \
                        ./movie/spsc/spsc_ui.o                                  \
                        ./multi_screen_home/app_multi_screen_home.o             \
                        ./multi_screen_home/desktop_scene/desktop_scene.o       \
                        ./multi_screen_home/ui/multi_screen_ui.o                \
                        ./music/music.o                                         \
                        ./music/music_ui.o                                      \
                        ./music/MusicMenu/music_menu.o                          \
                        ./music/MusicMenu/music_menu_ui.o                       \
                        ./music/MusicPlayer/music_list.o                        \
                        ./music/MusicPlayer/music_lyrics.o                      \
                        ./music/MusicPlayer/music_player.o                      \
                        ./music/MusicPlayer/player_face.o                       \
                        ./music/spctrm/spctrm.o                                 \
                        ./music/spctrm/spctrm_uipara.o                          \
                        ./music/widget/spectrum/spectrum.o                      \
                        ./photo/app_anole.o                                     \
                        ./photo/App_photo.o                                     \
                        ./photo/photo_adapter.o                                 \
                        ./photo/photo_fileinfo.o                                \
                        ./photo/photo_main.o                                    \
                        ./photo/photo_sset.o                                    \
                        ./photo/photo_uipara.o                                  \
                        ./play/ios_h264.o                                       \
                        ./play/video_decode_file.o                              \
                        ./record/app_record.o                                   \
                        ./record/record_comm.o                                  \
                        ./record/record_doc/record_list_view/record_list_view_doc.o     \
                        ./record/record_doc/record_view/record_view_doc.o               \
                        ./record/record_frame/record_disk_view/record_disk_view_frame.o \
                        ./record/record_frame/record_list_view/record_list_view_frame.o \
                        ./record/record_frame/record_view/record_view_frame.o   \
                        ./record/record_ui/record_ui.o                          \
                        ./record/record_ui/record_list_view/record_list_view.o  \
                        ./record/record_ui/record_view/record_view.o            \
                        ./record_video/record_video.o                           \
                        ./setting/alarm_select_song_frm.o                       \
                        ./setting/alarm_select_song_uipara.o                    \
                        ./setting/app_setting.o                                 \
                        ./setting/setting_alarm.o                               \
                        ./setting/setting_alarm_clock.o                         \
                        ./setting/setting_entrance.o                            \
                        ./setting/setting_general.o                             \
                        ./setting/setting_handle.o                              \
                        ./setting/setting_power.o                               \
                        ./setting/setting_tips.o                                \
                        ./setting/setting_uipara.o                              \
                        ./video_thumb/thumb_dec.o                               \
                        ../../elibs/libapps/module_adapter/common_module.o      \
                        ../../elibs/libapps/module_adapter/magic.o

#$(MOD_NAME)-objs += built-in.o
ifdef CONFIG_DRIVERS_USB_GADGET_MTP
$(MOD_NAME)-objs    += 	./mtp/mtp_app.o \
					./mtp/MtpCommon.o \
					./mtp/MtpDataBase.o \
					./mtp/mtp_update_tools.o \
					./mtp/MtpObjectInfo.o \
					./mtp/MtpPacket.o \
					./mtp/MtpProperty.o \
					./mtp/MtpServer.o \
					./mtp/MtpStorage.o \
					./mtp/Disk/Dir.o \
					./mtp/Disk/Disk.o \
					./mtp/Disk/DiskDebug.o \
					./mtp/Disk/File.o 
endif
include $(MELIS_BASE)/scripts/Makefile.mods					
