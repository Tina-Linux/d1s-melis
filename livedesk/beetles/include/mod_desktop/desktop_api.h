/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                               Desktop Module
*
*                                    (c) Copyright 2006-2010, kevin.z China
*                                             All Rights Reserved
*
* File    : desktop_api.h
* By      : kevin.z
* Version : v1.0
* Date    : 2010-9-14 19:13
* Descript:
* Update  : date                auther      ver     notes
*********************************************************************************************************
*/
#ifndef __MOD_DESKTOP_API_H__
#define __MOD_DESKTOP_API_H__
#include <kconfig.h>


typedef struct DESKTOP_API_ENTRY
{
    //================================================================================//
    //========                                          dsk_reg.h                                           ============//
    //================================================================================//
    int32_t(*ptr_dsk_reg_init_para)(void);
    int32_t(*ptr_dsk_reg_deinit_para)(void);
    int32_t(*ptr_dsk_reg_flush)(void);
    void                       *(*ptr_dsk_reg_get_para_by_app)(reg_app_e eApp);
    void                       *(*ptr_dsk_reg_get_default_para_by_app)(reg_app_e eApp);
    __bool(*ptr_dsk_reg_get_app_restore_flag)(reg_app_e eApp);
    int32_t(*ptr_dsk_reg_set_app_restore_flag)(reg_app_e eApp, __bool flag);
    int32_t(*ptr_dsk_reg_save_cur_play_info)(reg_app_e eApp, int32_t index, char *filename, rat_type_t root_type);
    int32_t(*ptr_dsk_reg_set_app_restore_all)(void);
    void (*ptr_dsk_reg_reserved[3])(void);

    //================================================================================//
    //========                                          functions                                           ============//
    //================================================================================//
    //dsk_display_gamma.h
    /*int32_t                     (*ptr_dsk_set_gamma)                        ( __u32 gamma );
    int32_t                       (*ptr_dsk_gamma_on)                         (void);
    int32_t                       (*ptr_dsk_gamma_off)                        (void);
    void                        (*ptr_dsk_gamma_reserved[5])                (void);*/
    //dsk_display_lcd.h
    int32_t(*ptr_dsk_display_lcd_on)(void);
    int32_t(*ptr_dsk_display_lcd_off)(void);
    int32_t(*ptr_dsk_display_set_lcd_brightness)(__lion_bright_t level);
    __lion_bright_t (*ptr_dsk_display_get_lcd_brightness)(void);
    //void                      (*ptr_dsk_display_lcd_reserved[5])          ( void );
    int32_t(*ptr_dsk_display_set_lcd_bright)(int32_t level);
    int32_t(*ptr_dsk_display_get_lcd_bright)(void);
    int32_t(*ptr_dsk_display_set_lcd_constract)(int32_t  value);
    int32_t(*ptr_dsk_display_get_lcd_constract)(void);
    int32_t(*ptr_dsk_display_set_lcd_saturation)(int32_t  value);
    int32_t(*ptr_dsk_display_get_lcd_saturation)(void);
    int32_t(*ptr_dsk_display_set_lcd_hue)(int32_t  value);
    int32_t(*ptr_dsk_display_get_lcd_hue)(void);
    int32_t(*ptr_dsk_display_set_lcd_edge)(int32_t  value);
    int32_t(*ptr_dsk_display_get_lcd_edge)(void);
    int32_t(*ptr_dsk_display_set_lcd_detail)(int32_t  value);
    int32_t(*ptr_dsk_display_get_lcd_detail)(void);
    int32_t(*ptr_dsk_display_set_lcd_denoise)(int32_t  value);
    int32_t(*ptr_dsk_display_get_lcd_denoise)(void);

    int32_t(*ptr_dsk_display_enhance_enable)(void);
    int32_t(*ptr_dsk_display_enhance_disable)(void);
    int32_t(*ptr_dsk_display_get_enhance_enable)(void);
    int32_t(*ptr_dsk_display_enhance_demo_enable)(void);
    int32_t(*ptr_dsk_display_enhance_demo_disable)(void);


    //dsk_display_misc.h
    int32_t(*ptr_dsk_display_hdmi_audio_enable)(__bool flag);
    int32_t(*ptr_dsk_display_get_size)(int32_t *p_width, int32_t *p_height);
    int32_t(*ptr_dsk_display_set_backcolor)(__u32 hlay, __u32 de_color);
    __u32(*ptr_dsk_display_get_backcolor)(__u32 hlay);
    void (*ptr_dsk_display_misc_reserved[5])(void);
    //dsk_display_output.h
    __lion_disp_format_e(*ptr_dsk_display_get_format)(void);
    int32_t(*ptr_dsk_display_set_format)(__lion_disp_format_e type);
    __u32(*ptr_dsk_display_get_output_type)(void);
    int32_t(*ptr_dsk_display_dskfmt2epdkfmt)(__lion_disp_format_e format, enum disp_output_type *output_p);
    __lion_disp_format_e(*ptr_dsk_display_epdkfmt2dskfmt)(enum disp_output_type output, int32_t format);
    int32_t(*ptr_dsk_display_on)(enum disp_output_type output);
    int32_t(*ptr_dsk_display_off)(void);
    void (*ptr_dsk_display_output_reserved[5])(void);
    //dsk_display_enhance.h
    /*int32_t                         (*ptr_dsk_display_enhance_on)               (void);
    int32_t                       (*ptr_dsk_display_enhance_off)              (void);
    int32_t                       (*ptr_dsk_display_get_bright)               (void);
    int32_t                       (*ptr_dsk_display_set_contrast)             (__u32 contrast);
    int32_t                       (*ptr_dsk_display_get_contrast)             (void);
    int32_t                       (*ptr_dsk_display_set_saturation)           (__u32 saturation);
    int32_t                       (*ptr_dsk_display_get_saturation)           (void); */

    //dsk_fw_update.h
    int32_t(*ptr_dsk_fw_update)(void);
    int32_t(*ptr_dsk_start_fw_update)(const char *file);
    void (*ptr_dsk_fw_update_reserved[4])(void);

    //dsk_keytone.h
    int32_t(*ptr_dsk_keytone_init)(const char *keytone_file);
    int32_t(*ptr_dsk_keytone_exit)(void);
    int32_t(*ptr_dsk_keytone_on)(void);
    int32_t(*ptr_dsk_keytone_set_state)(__set_keytone_t state);
    int32_t(*ptr_dsk_keytone_get_state)(__set_keytone_t *p_state);
    void (*ptr_dsk_keytone_set_key_type)(__u32 key_type);
    __u32(*ptr_dsk_keytone_get_key_type)(void);
    void (*ptr_dsk_keytone_reserved[3])(void);

    //dsk_usbh.h
    int32_t(*ptr_dsk_send_insmod_usbh_cmd)(void);
    int32_t(*ptr_dsk_send_rmmod_usbh_cmd)(void);
    __bool(*ptr_dsk_usbh_is_working)(void);
    void (*ptr_dsk_usbh_reserved[5])(void);

    //dsk_audio_if.h
    int32_t(*ptr_dsk_set_audio_if)(__audio_dev_interface_t dev_if_type);
    __audio_dev_interface_t (*ptr_dsk_get_audio_if)(void);
    int32_t(*ptr_dsk_set_hdmi_audio_gate)(__bool flag);
    void (*ptr_dsk_audio_reserved[5])(void);

    //dsk_volume.h
    int32_t(*ptr_dsk_volume_set)(int32_t volume);
    int32_t(*ptr_dsk_volume_get)(void);
    int32_t(*ptr_dsk_volume_inc)(void);
    int32_t(*ptr_dsk_volume_dec)(void);                                               // 减小音量
    int32_t(*ptr_dsk_volume_off)(void);
    int32_t(*ptr_dsk_volume_on)(void);
    __bool(*ptr_dsk_volume_is_on)(void);
    int32_t(*ptr_dsk_amplifier_onoff)(int32_t flag);
    __bool(*ptr_dsk_amplifier_is_on)(void);
    void (*ptr_dsk_hp_det_pin_det)(void);
    int32_t(*ptr_dsk_volume_app_line_in)(__bool app_line_in);
    void (*ptr_dsk_volume_reserved[8])(void);

    //dsk_charset.h
    __epdk_charset_enm_e(*ptr_dsk_get_langres_charset)(void);
    int32_t(*ptr_dsk_set_fs_charset)(__epdk_charset_enm_e charset);
    __epdk_charset_enm_e(*ptr_dsk_get_fs_charset)(void);
    int32_t(*ptr_dsk_set_isn_charset)(__epdk_charset_enm_e charset);
    __epdk_charset_enm_e(*ptr_dsk_get_isn_charset)(void);

    //dsk_langres.h
    int32_t(*ptr_dsk_langres_init)(const char *file);
    void (*ptr_dsk_langres_exit)(void);
    int32_t(*ptr_dsk_langres_set_type)(__u32 language_id);
    int32_t(*ptr_dsk_langres_get_type)(void);
    int32_t(*ptr_dsk_langres_get_menu_len)(__u32 text_id);
    int32_t(*ptr_dsk_langres_get_menu_text)(__u32 text_id, void *buf, __u32 size);

    //dsk_theme.h
    int32_t(*ptr_dsk_theme_init)(const char *file);
    void (*ptr_dsk_theme_exit)(void);
    int32_t(*ptr_dsk_theme_set_style)(__u32 style_id);
    int32_t(*ptr_dsk_theme_get_style)(void);
    HTHEME(*ptr_dsk_theme_open)(__u32 theme_id);
    void (*ptr_dsk_theme_close)(HTHEME handle);
    int32_t(*ptr_dsk_theme_hdl2size)(HTHEME handle);
    void                       *(*ptr_dsk_theme_hdl2buf)(HTHEME handle);
    int32_t(*ptr_dsk_theme_init_ex)(const char *file);
    void (*ptr_dsk_theme_exit_ex)(void);
    HTHEME(*ptr_dsk_theme_open_ex)(__u32 theme_id);
    void (*ptr_dsk_theme_close_ex)(HTHEME handle);
    void (*ptr_dsk_theme_really_close_ex)(HTHEME handle);
    void (*ptr_dsk_theme_really_close_by_id_ex)(__u32 theme_id);
    int32_t(*ptr_dsk_theme_hdl2size_ex)(HTHEME handle);
    void                       *(*ptr_dsk_theme_hdl2buf_ex)(HTHEME handle);

    //dsk_orchid.h
    int32_t(*ptr_dsk_orchid_check)(void);
    int32_t(*ptr_dsk_orchid_createDisk)(void);
    int32_t(*ptr_dsk_orchid_save_npl)(void);
    int32_t(*ptr_dsk_orchid_load_last_npl)(void);
    int32_t(*ptr_dsk_orchid_update)(void);
    void (*ptr_dsk_orchid_reserved[10])(void);

    //dsk_power.h
    int32_t(*ptr_dsk_power_dev_init)(void); 
    int32_t(*ptr_dsk_power_dev_uninit)(void);
    int32_t(*ptr_dsk_power_get_battery_level)(power_level_e *level);                                      // 剩余电量level
    int32_t(*ptr_dsk_power_get_voltage_level)(power_level_e *level);                                      // 工作电压level
    int32_t(*ptr_dsk_power_get_battary_state)(power_state_e *state);                                      // 查询 battary 状态
    int32_t(*ptr_dsk_power_set_off_time)(__u32 time);
    __bool(*ptr_dsk_power_is_low)(void);
    void (*ptr_dsk_speaker_turnoff)(void);
    void (*ptr_dsk_speaker_turnon)(void);
    void (*ptr_dsk_speaker_resume)(void);
    __bool(*ptr_dsk_get_charge_state)(void);
    __bool(*ptr_dsk_power_is_full)(void);
    //void                      (*ptr_dsk_speak_init)                   (void);
    //void                      (*ptr_dsk_speak_exit)                   (void);
    void (*ptr_dsk_speaker_reserved[8])(void);

    //dsk_usbd.h
    void (*ptr_dsk_usbd_remove)(void);
    void (*ptr_dsk_usbd_reserved[10])(void);

    //dsk_auto_off.h
    int32_t(*ptr_dsk_set_auto_off_time)(__u32 time);
    int32_t(*ptr_dsk_power_off)(void);

    //================================================================================//
    //========                                          framework                                           ============//
    //================================================================================//
    //framework/activity.h
    //Activity*                     (*ptr_activity_create)                      (AppletInfo *info);                     /* 创建一个 Activity */
    int32_t(*ptr_activity_set_attr)(Activity *thiz, void *attr);                                                      /* 设置priv属性      */
    void                       *(*ptr_activity_get_attr)(Activity *thiz);                                           /* 获取priv属性      */
    int32_t(*ptr_activity_set_load_para)(char *app_name, char *src_name, void *p_arg, __u32 size);
    int32_t(*ptr_activity_get_load_para)(char *app_name, char *src_name, void *p_arg, __u32 size);
    int32_t(*ptr_activity_load_file)(char *filename);                                                 /* 运行一个文件      */
    int32_t(*ptr_activity_load_app)(char *appname);                                                   /* 运行一个应用程序  */
    H_WIN(*ptr_activity_get_root_win)(Activity *thiz);                                                              /* 链接窗口          */
    int32_t(*ptr_activity_background_top)(void);                                                          /* 使程序进入后台状态*/
    int32_t(*ptr_activity_return_home)(Activity *thiz);                                                               /* 返回主界面        */
    void (*ptr_activity_finish)(Activity *thiz);                                                                    /* 杀掉Activity      */
    //void                      (*ptr_activity_kill_backgrd_except_home)        (void);
    //void                      (*ptr_activity_kill_all_backgrd)            (void);
    void (*ptr_activity_finish_all)(void);
    int32_t(*ptr_activity_suspend_top)(void);
    int32_t(*ptr_activity_resume_top)(void);
    int32_t(*ptr_activity_notify_top)(__gui_msg_t *pmsg);
    int32_t(*ptr_activity_all_able_standby)(void);
    void (*ptr_activity_reserved[6])(void);                                                                         /* 预留 */

    //framework/applet_info_manager.h
    AppletInfoManager          *(*ptr_applet_info_manager_create)(void);
    int32_t(*ptr_applet_info_manager_load_file)(AppletInfoManager *thiz, const char *filename);
    int32_t(*ptr_applet_info_manager_load_dir)(AppletInfoManager *thiz, const char *path);
    int32_t(*ptr_applet_info_manager_get_count)(AppletInfoManager *thiz);
    int32_t(*ptr_applet_info_manager_get)(AppletInfoManager *thiz, __u8 index, AppletInfo **info);
    void (*ptr_applet_info_manager_printf)(AppletInfoManager *thiz);
    void (*ptr_applet_info_manager_destroy)(AppletInfoManager *thiz);
    void (*ptr_applet_info_manager_reserved[10])(void);

    //framework/activity_manager.h
    ActivityManager            *(*ptr_activity_manager_create)(void);                                                                   /* create activity manager */
    Activity                   *(*ptr_activity_manager_top)(ActivityManager *thiz);                                                     /* 查询 toplevel Activity */
    int32_t(*ptr_activity_manager_push)(ActivityManager *thiz, Activity *activity);                                                       /* push activity to manager */
    int32_t(*ptr_activity_manager_pop)(ActivityManager *thiz);                                                                            /* pop activity from manager */
    int32_t(*ptr_activity_manager_top_to_backgrd)(ActivityManager *thiz);                                                                     /* toplevel activity 转为后台activity */
    int32_t(*ptr_activity_manager_add_backgrd)(ActivityManager *thiz, Activity *activity);                                                        /* 添加background activity to ActivityManager*/
    int32_t(*ptr_activity_manager_delete_backgrd)(ActivityManager *thiz, Activity *activity);                                                     /* 删除background activity */
    int32_t(*ptr_activity_manager_backgrd_to_top)(ActivityManager *thiz, Activity *activity);                                                     /* 后台activity 转为toplevel activity*/
    int32_t(*ptr_activity_manager_foreach)(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2);                                        /* 遍历所有background activity */
    int32_t(*ptr_activity_manager_foreach_suspend)(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2);
    int32_t(*ptr_activity_manager_foreach_backgrd)(ActivityManager *thiz, ActivityInterator iter, void *ctx1, void *ctx2);
    void (*ptr_activity_manager_destroy)(ActivityManager *thiz);
    void (*ptr_activity_manager_reserved[8])(void);

    //framework/globals.h
    ActivityManager            *(*ptr_activity_manager_get_default)(void);
    AppletInfoManager          *(*ptr_applet_info_manager_get_default)(void);
    H_WIN(*ptr_message_loop_win_get_default)(void);
    void (*ptr_activity_manager_set_default)(ActivityManager *manager);
    void (*ptr_applet_info_manager_set_defualt)(AppletInfoManager *manager);
    void (*ptr_message_loop_win_set_default)(H_WIN win);
    void (*ptr_globals_reserved[10])(void);

    //msg_srv/msg_emit.h
    int32_t(*ptr_msg_emit_init)(void);
    int32_t(*ptr_msg_emit_deinit)(void);
    int32_t(*ptr_msg_emit_register_hook)(__memit_hook_t *hook);
    int32_t(*ptr_msg_emit_register_tvout_tp_hook)(cb_tvout_tp_hook hook);
    void (*ptr_msg_emit_reserved[9])(void);

    //================================================================================//
    //========                                          engine                                          ============//
    //================================================================================//

    //============================              walkman         ================================//
    //dsk_walkman.h
    int32_t(*ptr_dsk_wkm_init)(dsk_wkm_mode_e work_mode);
    void (*ptr_dsk_wkm_exit)(void);
    int32_t(*ptr_dsk_wkm_open)(void);
    void (*ptr_dsk_wkm_close)(void);
    __bool(*ptr_dsk_wkm_is_open)(void);
    __pCBK_t (*ptr_dsk_wkm_set_cb)(dsk_wkm_event_e event, __pCBK_t cb, void *ctx);
    __pCBK_t (*ptr_dsk_wkm_get_cb)(dsk_wkm_event_e event);
    int32_t(*ptr_dsk_wkm_set_attr)(void *attr);
    void                       *(*ptr_dsk_wkm_get_attr)(void);
    //dsk_wkm_ab.h
    int32_t(*ptr_dsk_wkm_set_ab_a)(void);
    int32_t(*ptr_dsk_wkm_set_ab_b)(void);
    int32_t(*ptr_dsk_wkm_set_ab_loop_count)(__u32 count);
    int32_t(*ptr_dsk_wkm_cancle_ab)(void);
    //dsk_wkm_star.h
    int32_t(*ptr_dsk_wkm_set_star)(const char *file, __u32 star);
    int32_t(*ptr_dsk_wkm_get_star)(const char *file);
    //dsk_wkm_play_speed.h
    int32_t(*ptr_dsk_wkm_set_play_speed)(int32_t play_speed);
    int32_t(*ptr_dsk_wkm_get_play_speed)(void);
    //dsk_wkm_lyric.h
    __bool(*ptr_dsk_wkm_lyric_exist)(void);
    __cedar_lyric_fmt_t (*ptr_dsk_wkm_get_lyric_format)(void);
    int32_t(*ptr_dsk_wkm_get_lyric_item_count)(void);
    int32_t(*ptr_dsk_wkm_get_lyric_item_list)(__cedar_subtitle_item_t  *lyric_item_list, __u32 item_count);
    //dsk_wkm_npl.h
    int32_t(*ptr_dsk_wkm_npl_add_item)(const char *file);
    int32_t(*ptr_dsk_wkm_npl_delete_item)(const char *file);
    int32_t(*ptr_dsk_wkm_npl_clear)(void);
    int32_t(*ptr_dsk_wkm_npl_set_cur)(__u32 index);
    int32_t(*ptr_dsk_wkm_npl_get_cur)(void);
    int32_t(*ptr_dsk_wkm_npl_get_next)(void);
    int32_t(*ptr_dsk_wkm_npl_file2index)(const char *file);
    int32_t(*ptr_dsk_wkm_npl_index2file)(__u32 index, char *file);
    int32_t(*ptr_dsk_wkm_npl_get_total_count)(void);
    int32_t(*ptr_dsk_wkm_npl_save_list)(void);
    int32_t(*ptr_dsk_wkm_npl_load_last_list)(void);
    //dsk_wkm_play_mode.h
    int32_t(*ptr_dsk_wkm_set_play_mode)(orc_play_mode_e  play_mode);
    orc_play_mode_e(*ptr_dsk_wkm_get_play_mode)(void);
    //dsk_wkm_eq.h
    int32_t(*ptr_dsk_wkm_set_eq_mode)(__cedar_audio_eq_t  eq_mode);
    __cedar_audio_eq_t (*ptr_dsk_wkm_get_eq_mode)(void);
    int32_t(*ptr_dsk_wkm_set_eq_value)(const __s8 *eq_value_list);
    int32_t(*ptr_dsk_wkm_get_eq_value)(__s8 *eq_value_list);
    int32_t(*ptr_dsk_wkm_eq_mode2value)(__cedar_audio_eq_t eq_mode, __s8 *eq_value_list);
    //dsk_wkm_media_info.h
    int32_t(*ptr_dsk_wkm_get_now_playing_file)(char *filename);
    int32_t(*ptr_dsk_wkm_get_tag)(__cedar_tag_inf_t  *tag_info_p);
    int32_t(*ptr_dsk_wkm_get_cur_time)(void);
    int32_t(*ptr_dsk_wkm_get_total_time)(void);
    __cedar_audio_fmt_t (*ptr_dsk_wkm_get_audio_encoding)(void);
    __u32(*ptr_dsk_wkm_get_audio_bps)(void);
    __u32(*ptr_dsk_wkm_get_audio_sample_rate)(void);
    int32_t(*ptr_dsk_wkm_get_audio_info)(const char *file, dsk_wkm_audio_info_t *info_p);
    int32_t(*ptr_dsk_wkm_get_album_cover)(const char *file, void *buf, const SIZE *size_p, dsk_wkm_pic_zoom_e zoom);
    //dsk_wkm_ff_rr_speed.h
    int32_t(*ptr_dsk_wkm_set_ff_rr_speed)(__u32 ff_rr_speed);
    int32_t(*ptr_dsk_wkm_get_ff_rr_speed)(void);
    //dsk_wkm_spectrum.h
    int32_t(*ptr_dsk_wkm_enable_spectrum)(void);
    int32_t(*ptr_dsk_wkm_disable_spectrum)(void);
    int32_t(*ptr_dsk_wkm_get_spectrum_info)(__u16 *value_list);
    //dsk_wkm_fsm_ctrl.h
    int32_t(*ptr_dsk_wkm_play_file)(const char *file);
    int32_t(*ptr_dsk_wkm_play_next)(void);
    int32_t(*ptr_dsk_wkm_play_prev)(void);
    int32_t(*ptr_dsk_wkm_stop)(void);
    int32_t(*ptr_dsk_wkm_pause)(void);
    int32_t(*ptr_dsk_wkm_resume_play)(void);
    int32_t(*ptr_dsk_wkm_ff)(void);
    int32_t(*ptr_dsk_wkm_rr)(void);
    int32_t(*ptr_dsk_wkm_jump)(__u32 time);
    __cedar_status_t (*ptr_dsk_wkm_get_fsm_state)(void);
    __bool(*ptr_dsk_wkm_is_playing)(void);
    __bool(*ptr_dsk_wkm_get_pause_state)(void);
    void (*ptr_dsk_wkm_clear_feedback_msgQ)(void);
    feedback_msg_t (*ptr_dsk_wkm_get_feedback_msg)(void);
    int32_t(*ptr_dsk_wkm_monitor_enable)(void);
    int32_t(*ptr_dsk_wkm_monitor_disable)(void);
    //============================              radio       ================================//
    //dsk_radio.h
    int32_t(*ptr_dsk_radio_open)(void);
    __bool(*ptr_dsk_radio_is_open)(void);
    __pCBK_t (*ptr_dsk_radio_set_cb)(dsk_radio_event_e event, __pCBK_t cb, void *ctx);
    __pCBK_t (*ptr_dsk_radio_get_cb)(dsk_radio_event_e event);
    int32_t(*ptr_dsk_radio_mute)(void);
    int32_t(*ptr_dsk_radio_unmute)(void);
	__s32 (*ptr_dsk_radio_get_vol)(void);
    __s32 (*ptr_dsk_radio_set_vol)(__u8 vol);
    int32_t(*ptr_dsk_radio_set_band)(dsk_radio_band_mode_t band_mode);
    int32_t(*ptr_dsk_radio_set_threshold)(dsk_radio_threshold_mode_t threshold);
    int32_t(*ptr_dsk_radio_set_audio_mode)(dsk_radio_audio_mode_t audio_mode);
    int32_t(*ptr_dsk_radio_get_band)(int32_t *cur_band);
    int32_t(*ptr_dsk_radio_get_audio_mode)(int32_t *audio_mode);
    int32_t(*ptr_dsk_radio_get_min_freq)(int32_t *min_freq);
    int32_t(*ptr_dsk_radio_get_max_freq)(int32_t *max_freq);
    int32_t(*ptr_dsk_radio_close)(void);
    //dsk_radio_receive.h
    int32_t(*ptr_dsk_radio_rcv_open)(void);
    int32_t(*ptr_dsk_radio_rcv_close)(void);
    __bool(*ptr_dsk_radio_rcv_is_open)(void);
    int32_t(*ptr_dsk_radio_rcv_set_freq_play)(signed long freq);
    int32_t(*dsk_radio_rcv_set_search_cb)(__pCBK_t cb, void *ctx);
    int32_t(*ptr_dsk_radio_rcv_auto_search)(void);
    int32_t(*ptr_dsk_radio_rcv_get_cur_freq)(__u32 *freq);
    int32_t(*ptr_dsk_radio_rcv_set_search_mode)(__u32 search_mode);
    int32_t(*ptr_dsk_radio_get_search_flag)(__u32 *search_flag);                                       //by cky
    int32_t(*ptr_dsk_radio_set_search_flag)(__u32 search_flag);                                       //by cky
    int32_t(*ptr_dsk_radio_set_manual_way)(__u32 manual_way);                                    //add by Kingvan
    int32_t(*ptr_dsk_radio_set_automanual_mode)(__u32 search_mode);
    //int32_t                         (*ptr_dsk_radio_rcv_search_all)             (void);
    int32_t(*ptr_dsk_radio_rcv_search_all)(__u32 freq_cur, int32_t channel_cur);
    int32_t(*ptr_dsk_radio_rcv_next_freq_play)(void);
    int32_t(*ptr_dsk_radio_rcv_pre_freq_play)(void);
    int32_t(*ptr_dsk_radio_rcv_set_play_list_type)(__play_list_type_e play_list_type);
    int32_t(*ptr_dsk_radio_rcv_get_play_list_type)(int32_t *play_list_type);
    //int32_t                         (*ptr_dsk_radio_rcv_get_search_result_vn)   (int32_t *valid_num);
    int32_t(*ptr_dsk_radio_rcv_get_search_result_vn)(void);
    int32_t(*ptr_dsk_radio_rcv_get_favorite_vn)(int32_t *valid_num);
    int32_t(*ptr_dsk_radio_rcv_get_search_result_chaninfo)(int32_t id, dsk_radio_chan_t *chan_info);
    int32_t(*ptr_dsk_radio_rcv_get_favorite_chaninfo)(int32_t id, dsk_radio_chan_t *chan_info);
    int32_t(*ptr_dsk_radio_rcv_get_favorite)(char *file_name);
    int32_t(*ptr_dsk_radio_rcv_save_favorite)(char *file_name);
    //int32_t                         (*ptr_dsk_radio_rcv_get_search_result)      (char *file_name);
    //int32_t                         (*ptr_dsk_radio_rcv_save_search_result)     (char *file_name);
    int32_t(*ptr_dsk_radio_rcv_get_search_result)(reg_fm_para_t *para);
    int32_t(*ptr_dsk_radio_rcv_save_search_result)(void);
    int32_t(*ptr_dsk_radio_rcv_save_cur_freq)(void);
    int32_t(*ptr_dsk_radio_rcv_add_chan_to_favorite)(__u32 id, dsk_radio_chan_t *chan_info);
    int32_t(*ptr_dsk_radio_rcv_del_chan_from_favorite)(__s32 id);
    int32_t(*ptr_dsk_radio_send_open)(void);
    int32_t(*ptr_dsk_radio_send_close)(void);
    __bool(*ptr_dsk_radio_send_is_open)(void);
    int32_t(*ptr_dsk_radio_send_set_freq_play)(signed long freq);
    int32_t(*ptr_dsk_radio_send_get_cur_freq)(__u32 *freq);
    int32_t(*ptr_dsk_radio_send_set_gain)(unsigned long gain);
    int32_t(*ptr_dsk_radio_send_get_gain)(__u32 *gain);
} desktop_api_entry_t;


#ifndef USED_BY_DESKTOP     //not used by desktop module itsef

#define DESKTOP_ENTRY_TBL   ((EMOD_TYPE_MOD_DESKTOP << 20) |(0xe0000000))   //this address is depended on the link address of mod_desktop
//#define DESKTOP_ENTRY_TBL   ((EMOD_TYPE_MOD_INIT<<5)||(0xe0000000))       //this address is depended on the link address of mod_desktop

#define DESKTOP_FUNC_ENTRY  ((desktop_api_entry_t *)DESKTOP_ENTRY_TBL)

//================================================================================//
//========                                          reg               ============//
//================================================================================//
#define dsk_reg_init_para                   (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_init_para)
#define dsk_reg_deinit_para                 (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_deinit_para)
#define dsk_reg_flush                       (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_flush)
#define dsk_reg_get_para_by_app             (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_get_para_by_app)
#define dsk_reg_get_default_para_by_app     (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_get_default_para_by_app)
#define dsk_reg_get_app_restore_flag        (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_get_app_restore_flag)
#define dsk_reg_set_app_restore_flag        (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_set_app_restore_flag)
#define dsk_reg_save_cur_play_info          (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_save_cur_play_info)
#define dsk_reg_set_app_restore_all         (DESKTOP_FUNC_ENTRY->ptr_dsk_reg_set_app_restore_all)
#define dsk_reg_get_para_pointer_by_app     dsk_reg_get_para_by_app
#define dsk_reg_get_default_para_pointer_by_app     dsk_reg_get_default_para_by_app


//================================================================================//
//========                                          functions         ============//
//================================================================================//
//dsk_display_gamma.h
/*#define   dsk_set_gamma                   (DESKTOP_FUNC_ENTRY->ptr_dsk_set_gamma)
#define dsk_gamma_on                        (DESKTOP_FUNC_ENTRY->ptr_dsk_gamma_on)
#define dsk_gamma_off                       (DESKTOP_FUNC_ENTRY->ptr_dsk_gamma_off) */
//dsk_display_lcd.h
#define dsk_display_lcd_on                  (DESKTOP_FUNC_ENTRY->ptr_dsk_display_lcd_on)
#define dsk_display_lcd_off                 (DESKTOP_FUNC_ENTRY->ptr_dsk_display_lcd_off)

#define dsk_display_set_lcd_brightness      (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_brightness)
#define dsk_display_get_lcd_brightness      (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_brightness)

#define dsk_display_set_lcd_bright          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_bright)
#define dsk_display_get_lcd_bright          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_bright)

#define dsk_display_set_lcd_constract       (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_constract)
#define dsk_display_get_lcd_constract       (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_constract)
#define dsk_display_set_lcd_saturation      (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_saturation)
#define dsk_display_get_lcd_saturation      (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_saturation)
#define dsk_display_set_lcd_hue             (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_hue )
#define dsk_display_get_lcd_hue             (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_hue )

#define dsk_display_set_lcd_edge            (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_edge )
#define dsk_display_get_lcd_edge            (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_edge )
#define dsk_display_set_lcd_detail          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_detail )
#define dsk_display_get_lcd_detail          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_detail )
#define dsk_display_set_lcd_denoise         (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_lcd_denoise )
#define dsk_display_get_lcd_denoise         (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_lcd_denoise )

#define dsk_display_enhance_enable          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_enhance_enable )
#define dsk_display_enhance_disable         (DESKTOP_FUNC_ENTRY->ptr_dsk_display_enhance_disable )
#define dsk_display_get_enhance_enable      (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_enhance_enable )
#define dsk_display_enhance_demo_enable     (DESKTOP_FUNC_ENTRY->ptr_dsk_display_enhance_demo_enable )
#define dsk_display_enhance_demo_disable    (DESKTOP_FUNC_ENTRY->ptr_dsk_display_enhance_demo_disable )

//dsk_display_misc.h
#define dsk_display_hdmi_audio_enable       (DESKTOP_FUNC_ENTRY->ptr_dsk_display_hdmi_audio_enable)
#define dsk_display_get_size                (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_size)
#define dsk_display_set_backcolor           (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_backcolor)
#define dsk_display_get_backcolor           (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_backcolor)
//dsk_display_output.h
#define dsk_display_get_format              (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_format)
#define dsk_display_set_format              (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_format)
#define dsk_display_get_output_type         (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_output_type)
#define dsk_display_dskfmt2epdkfmt          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_dskfmt2epdkfmt)
#define dsk_display_epdkfmt2dskfmt          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_epdkfmt2dskfmt)
#define dsk_display_on                      (DESKTOP_FUNC_ENTRY->ptr_dsk_display_on)
#define dsk_display_off                     (DESKTOP_FUNC_ENTRY->ptr_dsk_display_off)
//dsk_display_enhance.h
/*#define dsk_display_enhance_on                (DESKTOP_FUNC_ENTRY->ptr_dsk_display_enhance_on)
#define dsk_display_enhance_off             (DESKTOP_FUNC_ENTRY->ptr_dsk_display_enhance_off)
#define dsk_display_set_bright              (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_bright)
#define dsk_display_get_bright              (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_bright)
#define dsk_display_set_contrast            (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_contrast)
#define dsk_display_get_contrast            (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_contrast)
#define dsk_display_set_saturation          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_set_saturation)
#define dsk_display_get_saturation          (DESKTOP_FUNC_ENTRY->ptr_dsk_display_get_saturation)*/

//dsk_fw_update.h
#define dsk_fw_update                       (DESKTOP_FUNC_ENTRY->ptr_dsk_fw_update)
#define dsk_start_fw_update                 (DESKTOP_FUNC_ENTRY->ptr_dsk_start_fw_update)


//dsk_keytone.h
#define dsk_keytone_init                    (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_init)
#define dsk_keytone_exit                    (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_exit)
#define dsk_keytone_on                      (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_on)
#define dsk_keytone_set_state               (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_set_state)
#define dsk_keytone_get_state               (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_get_state)
#define dsk_keytone_set_key_type            (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_set_key_type)
#define dsk_keytone_get_key_type            (DESKTOP_FUNC_ENTRY->ptr_dsk_keytone_get_key_type)


//dsk_usbh.h
#define dsk_send_insmod_usbh_cmd            (DESKTOP_FUNC_ENTRY->ptr_dsk_send_insmod_usbh_cmd)
#define dsk_send_rmmod_usbh_cmd             (DESKTOP_FUNC_ENTRY->ptr_dsk_send_rmmod_usbh_cmd)
#define dsk_usbh_is_working                 (DESKTOP_FUNC_ENTRY->ptr_dsk_usbh_is_working)

//dsk_audio_if.h
#define dsk_set_audio_if                    (DESKTOP_FUNC_ENTRY->ptr_dsk_set_audio_if)
#define dsk_get_audio_if                    (DESKTOP_FUNC_ENTRY->ptr_dsk_get_audio_if)
#define dsk_set_hdmi_audio_gate             (DESKTOP_FUNC_ENTRY->ptr_dsk_set_hdmi_audio_gate)

//dsk_volume.h
#define dsk_volume_set                      (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_set)
#define dsk_volume_get                      (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_get)
#define dsk_volume_inc                      (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_inc)
#define dsk_amplifier_onoff                 (DESKTOP_FUNC_ENTRY->ptr_dsk_amplifier_onoff)
#define dsk_amplifier_is_on                 (DESKTOP_FUNC_ENTRY->ptr_dsk_amplifier_is_on)
#define dsk_hp_det_pin_det                  (DESKTOP_FUNC_ENTRY->ptr_dsk_hp_det_pin_det)


#define dsk_volume_dec                      (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_dec)
#define dsk_volume_off                      (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_off)
#define dsk_volume_on                       (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_on)
#define dsk_volume_is_on                    (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_is_on)
#define dsk_volume_app_line_in              (DESKTOP_FUNC_ENTRY->ptr_dsk_volume_app_line_in)

//dsk_charset.h
#define dsk_get_langres_charset             (DESKTOP_FUNC_ENTRY->ptr_dsk_get_langres_charset)
#define dsk_set_fs_charset                  (DESKTOP_FUNC_ENTRY->ptr_dsk_set_fs_charset)
#define dsk_get_fs_charset                  (DESKTOP_FUNC_ENTRY->ptr_dsk_get_fs_charset)
#define dsk_set_isn_charset                 (DESKTOP_FUNC_ENTRY->ptr_dsk_set_isn_charset)
#define dsk_get_isn_charset                 (DESKTOP_FUNC_ENTRY->ptr_dsk_get_isn_charset)

//dsk_langres.h
#define dsk_langres_init                    (DESKTOP_FUNC_ENTRY->ptr_dsk_langres_init)
#define dsk_langres_exit                    (DESKTOP_FUNC_ENTRY->ptr_dsk_langres_exit)
#define dsk_langres_set_type                (DESKTOP_FUNC_ENTRY->ptr_dsk_langres_set_type)
#define dsk_langres_get_type                (DESKTOP_FUNC_ENTRY->ptr_dsk_langres_get_type)
#define dsk_langres_get_menu_len            (DESKTOP_FUNC_ENTRY->ptr_dsk_langres_get_menu_len)
#define dsk_langres_get_menu_text           (DESKTOP_FUNC_ENTRY->ptr_dsk_langres_get_menu_text)

//dsk_theme.h
#define dsk_theme_init                      (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_init)
#define dsk_theme_exit                      (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_exit)
#define dsk_theme_set_style                 (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_set_style)
#define dsk_theme_get_style                 (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_get_style)
#define dsk_theme_open                      (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_open)
#define dsk_theme_close                     (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_close)
#define dsk_theme_hdl2size                  (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_hdl2size)
#define dsk_theme_hdl2buf                   (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_hdl2buf)

#define dsk_theme_init_ex                       (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_init_ex)
#define dsk_theme_exit_ex                       (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_exit_ex)
//#define dsk_theme_set_style                   (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_set_style)
//#define dsk_theme_get_style                   (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_get_style)
#define dsk_theme_open_ex                       (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_open_ex)
#define dsk_theme_close_ex                  (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_close_ex)
#define dsk_theme_really_close_ex                   (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_really_close_ex)
#define dsk_theme_really_close_by_id_ex                 (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_really_close_by_id_ex)

#define dsk_theme_hdl2size_ex                   (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_hdl2size_ex)
#define dsk_theme_hdl2buf_ex                    (DESKTOP_FUNC_ENTRY->ptr_dsk_theme_hdl2buf_ex)

// ./dsk_sys/dsk_orchid.h
#define dsk_orchid_check                    (DESKTOP_FUNC_ENTRY->ptr_dsk_orchid_check)
#define dsk_orchid_createDisk               (DESKTOP_FUNC_ENTRY->ptr_dsk_orchid_createDisk)
#define dsk_orchid_save_npl                 (DESKTOP_FUNC_ENTRY->ptr_dsk_orchid_save_npl)
#define dsk_orchid_load_last_npl            (DESKTOP_FUNC_ENTRY->ptr_dsk_orchid_load_last_npl)
#define dsk_orchid_update                   (DESKTOP_FUNC_ENTRY->ptr_dsk_orchid_update)

//./dsk_sys/dsk_power.h
#define dsk_power_dev_init                  (DESKTOP_FUNC_ENTRY->ptr_dsk_power_dev_init)
#define dsk_power_dev_uninit                (DESKTOP_FUNC_ENTRY->ptr_dsk_power_dev_uninit)
#define dsk_power_get_voltage_level         (DESKTOP_FUNC_ENTRY->ptr_dsk_power_get_voltage_level)
#define dsk_power_get_battary_state         (DESKTOP_FUNC_ENTRY->ptr_dsk_power_get_battary_state)
#define dsk_power_set_off_time              (DESKTOP_FUNC_ENTRY->ptr_dsk_power_set_off_time)
#define dsk_power_is_low                    (DESKTOP_FUNC_ENTRY->ptr_dsk_power_is_low)
#define dsk_speaker_turnoff                 (DESKTOP_FUNC_ENTRY->ptr_dsk_speaker_turnoff)
#define dsk_speaker_turnon                  (DESKTOP_FUNC_ENTRY->ptr_dsk_speaker_turnon)
#define dsk_speaker_resume                  (DESKTOP_FUNC_ENTRY->ptr_dsk_speaker_resume)
#define dsk_get_charge_state                (DESKTOP_FUNC_ENTRY->ptr_dsk_get_charge_state)

//#define dsk_speak_init                        (DESKTOP_FUNC_ENTRY->ptr_dsk_speak_init)
//#define dsk_speak_exit                        (DESKTOP_FUNC_ENTRY->ptr_dsk_speak_exit)

// ./dsk_sys/dsk_usbd.h
#define dsk_usbd_remove                     (DESKTOP_FUNC_ENTRY->ptr_dsk_usbd_remove)

//dsk_auto_off.h
#define dsk_set_auto_off_time(x)                /*eLIBs_printf("file:%s, ln:%d auto off time=%d\n", __FILE__, __LINE__, x);*/(DESKTOP_FUNC_ENTRY->ptr_dsk_set_auto_off_time(x))
#define dsk_power_off                       (DESKTOP_FUNC_ENTRY->ptr_dsk_power_off)
#define dsk_power_is_full                   (DESKTOP_FUNC_ENTRY->ptr_dsk_power_is_full)


//================================================================================//
//========                                          framework         ============//
//================================================================================//
//framework/activity.h
//#define activity_create                     (DESKTOP_FUNC_ENTRY->ptr_activity_create                        )
#define activity_set_attr                   (DESKTOP_FUNC_ENTRY->ptr_activity_set_attr                      )
#define activity_get_attr                   (DESKTOP_FUNC_ENTRY->ptr_activity_get_attr                      )
#define activity_set_load_para              (DESKTOP_FUNC_ENTRY->ptr_activity_set_load_para                 )
#define activity_get_load_para              (DESKTOP_FUNC_ENTRY->ptr_activity_get_load_para                 )
#define activity_load_file                  (DESKTOP_FUNC_ENTRY->ptr_activity_load_file                     )
#define activity_load_app                   (DESKTOP_FUNC_ENTRY->ptr_activity_load_app                      )
#define activity_get_root_win               (DESKTOP_FUNC_ENTRY->ptr_activity_get_root_win                  )
#define activity_background_top             (DESKTOP_FUNC_ENTRY->ptr_activity_background_top                )
#define activity_return_home                (DESKTOP_FUNC_ENTRY->ptr_activity_return_home                   )
#define activity_finish                     (DESKTOP_FUNC_ENTRY->ptr_activity_finish                        )
//#define activity_kill_backgrd_except_home   (DESKTOP_FUNC_ENTRY->ptr_activity_kill_backgrd_except_home      )
//#define activity_kill_all_backgrd           (DESKTOP_FUNC_ENTRY->ptr_activity_kill_all_backgrd              )
#define activity_finish_all                 (DESKTOP_FUNC_ENTRY->ptr_activity_finish_all                    )
#define activity_suspend_top                (DESKTOP_FUNC_ENTRY->ptr_activity_suspend_top                   )
#define activity_resume_top                 (DESKTOP_FUNC_ENTRY->ptr_activity_resume_top                    )
#define activity_notify_top                 (DESKTOP_FUNC_ENTRY->ptr_activity_notify_top                    )
#define activity_all_able_standby           (DESKTOP_FUNC_ENTRY->ptr_activity_all_able_standby              )

//framework/applet_info_manager.h
#define applet_info_manager_create          (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_create             )
#define applet_info_manager_load_file       (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_load_file          )
#define applet_info_manager_load_dir        (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_load_dir           )
#define applet_info_manager_get_count       (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_get_count          )
#define applet_info_manager_get             (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_get                )
#define applet_info_manager_printf          (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_printf             )
#define applet_info_manager_destroy         (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_destroy            )

//framework/activity_manager.h
#define activity_manager_create             (DESKTOP_FUNC_ENTRY->ptr_activity_manager_create                )
#define activity_manager_top                (DESKTOP_FUNC_ENTRY->ptr_activity_manager_top                   )
#define activity_manager_push               (DESKTOP_FUNC_ENTRY->ptr_activity_manager_push                  )
#define activity_manager_pop                (DESKTOP_FUNC_ENTRY->ptr_activity_manager_pop                   )
#define activity_manager_top_to_backgrd     (DESKTOP_FUNC_ENTRY->ptr_activity_manager_top_to_backgrd        )
#define activity_manager_add_backgrd        (DESKTOP_FUNC_ENTRY->ptr_activity_manager_add_backgrd           )
#define activity_manager_delete_backgrd     (DESKTOP_FUNC_ENTRY->ptr_activity_manager_delete_backgrd        )
#define activity_manager_backgrd_to_top     (DESKTOP_FUNC_ENTRY->ptr_activity_manager_backgrd_to_top        )
#define activity_manager_foreach            (DESKTOP_FUNC_ENTRY->ptr_activity_manager_foreach               )
#define activity_manager_foreach_suspend    (DESKTOP_FUNC_ENTRY->ptr_activity_manager_foreach_suspend       )
#define activity_manager_foreach_backgrd    (DESKTOP_FUNC_ENTRY->ptr_activity_manager_foreach_backgrd       )
#define activity_manager_destroy            (DESKTOP_FUNC_ENTRY->ptr_activity_manager_destroy               )

//framework/globals.h
#define activity_manager_get_default        (DESKTOP_FUNC_ENTRY->ptr_activity_manager_get_default           )
#define applet_info_manager_get_default     (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_get_default        )
#define message_loop_win_get_default        (DESKTOP_FUNC_ENTRY->ptr_message_loop_win_get_default           )
#define activity_manager_set_default        (DESKTOP_FUNC_ENTRY->ptr_activity_manager_set_default           )
#define applet_info_manager_set_defualt     (DESKTOP_FUNC_ENTRY->ptr_applet_info_manager_set_defualt        )
#define message_loop_win_set_default        (DESKTOP_FUNC_ENTRY->ptr_message_loop_win_set_default           )

//msg_srv/msg_emit.h
#define msg_emit_init                       (DESKTOP_FUNC_ENTRY->ptr_msg_emit_init                          )
#define msg_emit_deinit                     (DESKTOP_FUNC_ENTRY->ptr_msg_emit_deinit                        )
#define msg_emit_register_hook              (DESKTOP_FUNC_ENTRY->ptr_msg_emit_register_hook                 )
#define msg_emit_register_tvout_tp_hook     (DESKTOP_FUNC_ENTRY->ptr_msg_emit_register_tvout_tp_hook        )


//================================================================================//
//========                                  engine         =======================//
//================================================================================//
//============================              walkman        =======================//
//dsk_walkman.h
#define dsk_wkm_init                        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_init)
#define dsk_wkm_exit                        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_exit)
#define dsk_wkm_open                        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_open)
#define dsk_wkm_is_open                     (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_is_open)
#define dsk_wkm_close                       (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_close)
#define dsk_wkm_set_cb                      (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_cb)
#define dsk_wkm_get_cb                      (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_cb)
#define dsk_wkm_set_attr                    (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_attr)
#define dsk_wkm_get_attr                    (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_attr)

//dsk_wkm_ab.h
#define dsk_wkm_set_ab_a                    (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_ab_a)
#define dsk_wkm_set_ab_b                    (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_ab_b)
#define dsk_wkm_set_ab_loop_count           (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_ab_loop_count)
#define dsk_wkm_cancle_ab                   (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_cancle_ab)
//dsk_wkm_star.h
#define dsk_wkm_set_star                    (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_star)
#define dsk_wkm_get_star                    (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_star)
//dsk_wkm_play_speed.h
#define dsk_wkm_set_play_speed              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_play_speed)
#define dsk_wkm_get_play_speed              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_play_speed)
//dsk_wkm_lyric.h
#define  dsk_wkm_lyric_exist                (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_lyric_exist)
#define dsk_wkm_get_lyric_format            (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_lyric_format)
#define dsk_wkm_get_lyric_item_count        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_lyric_item_count)
#define dsk_wkm_get_lyric_item_list         (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_lyric_item_list)
//dsk_wkm_npl.h
#define dsk_wkm_npl_add_item                (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_add_item)
#define dsk_wkm_npl_delete_item             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_delete_item)
#define dsk_wkm_npl_clear                   (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_clear)
#define dsk_wkm_npl_set_cur                 (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_set_cur)
#define dsk_wkm_npl_get_cur                 (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_get_cur)
#define dsk_wkm_npl_get_next                (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_get_next)
#define dsk_wkm_npl_file2index              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_file2index)
#define dsk_wkm_npl_index2file              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_index2file)
#define dsk_wkm_npl_get_total_count         (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_get_total_count)
#define dsk_wkm_npl_save_list               (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_save_list)
#define dsk_wkm_npl_load_last_list          (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_npl_load_last_list)
//dsk_wkm_play_mode.h
#define dsk_wkm_set_play_mode               (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_play_mode)
#define dsk_wkm_get_play_mode               (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_play_mode)
//dsk_wkm_eq.h
#define dsk_wkm_set_eq_mode                 (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_eq_mode)
#define dsk_wkm_get_eq_mode                 (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_eq_mode)
#define dsk_wkm_set_eq_value                (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_eq_value)
#define dsk_wkm_get_eq_value                (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_eq_value)
#define dsk_wkm_eq_mode2value               (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_eq_mode2value)
//dsk_wkm_media_info.h
#define dsk_wkm_get_now_playing_file        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_now_playing_file)
#define dsk_wkm_get_tag                     (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_tag)
#define dsk_wkm_get_cur_time                (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_cur_time)
#define dsk_wkm_get_total_time              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_total_time)
#define dsk_wkm_get_audio_encoding          (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_audio_encoding)
#define dsk_wkm_get_audio_bps               (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_audio_bps)
#define dsk_wkm_get_audio_sample_rate       (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_audio_sample_rate)
#define dsk_wkm_get_audio_info              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_audio_info)
#define dsk_wkm_get_album_cover             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_album_cover)
//dsk_wkm_ff_rr_speed.h
#define dsk_wkm_set_ff_rr_speed             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_set_ff_rr_speed)
#define dsk_wkm_get_ff_rr_speed             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_ff_rr_speed)
//dsk_wkm_spectrum.h
#define dsk_wkm_enable_spectrum             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_enable_spectrum)
#define dsk_wkm_disable_spectrum            (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_disable_spectrum)
#define dsk_wkm_get_spectrum_info           (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_spectrum_info)
//dsk_wkm_fsm_ctrl.h
#define dsk_wkm_play_file                   (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_play_file)
#define dsk_wkm_play_next                   (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_play_next)
#define dsk_wkm_play_prev                   (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_play_prev)
#define dsk_wkm_stop                        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_stop)
#define dsk_wkm_pause                       (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_pause)
#define dsk_wkm_resume_play                 (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_resume_play)
#define dsk_wkm_ff                          (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_ff)
#define dsk_wkm_rr                          (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_rr)
#define dsk_wkm_jump                        (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_jump)
#define dsk_wkm_get_fsm_state               (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_fsm_state)
#define dsk_wkm_is_playing                  (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_is_playing)
#define dsk_wkm_get_pause_state             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_pause_state)
#define dsk_wkm_clear_feedback_msgQ         (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_clear_feedback_msgQ)
#define dsk_wkm_get_feedback_msg            (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_get_feedback_msg)
#define dsk_wkm_monitor_enable              (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_monitor_enable)
#define dsk_wkm_monitor_disable             (DESKTOP_FUNC_ENTRY->ptr_dsk_wkm_monitor_disable)
//============================              radio       ================================//
//dsk_radio.h
#define dsk_radio_open                      (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_open)
#define dsk_radio_is_open                   (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_is_open)
#define dsk_radio_set_cb                    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_cb)
#define dsk_radio_get_cb                    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_cb)
#define dsk_radio_mute                      (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_mute)
#define dsk_radio_unmute                    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_unmute)
#define dsk_radio_get_vol                   (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_vol)
#define dsk_radio_set_vol                   (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_vol)
#define dsk_radio_set_band                  (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_band)
#define dsk_radio_set_threshold             (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_threshold)
#define dsk_radio_set_audio_mode            (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_audio_mode)
#define dsk_radio_get_band                  (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_band)
#define dsk_radio_get_audio_mode            (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_audio_mode)
#define dsk_radio_get_min_freq              (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_min_freq)
#define dsk_radio_get_max_freq              (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_max_freq)
#define dsk_radio_close                     (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_close)
//dsk_radio_receive.h
#define dsk_radio_rcv_open                  (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_open)
#define dsk_radio_rcv_close                 (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_close)
#define dsk_radio_rcv_is_open               (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_is_open)
#define dsk_radio_rcv_set_freq_play         (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_set_freq_play)
#define dsk_radio_rcv_set_search_cb         (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_set_search_cb)
#define dsk_radio_rcv_auto_search           (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_auto_search)
#define dsk_radio_rcv_get_cur_freq          (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_cur_freq)
#define dsk_radio_rcv_set_search_mode       (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_set_search_mode)

#define dsk_radio_get_search_flag           (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_get_search_flag) //add by cky
#define dsk_radio_set_search_flag           (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_search_flag) //add by cky

#define dsk_radio_set_manual_way            (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_manual_way)//add by Kingvan
#define dsk_radio_set_automanual_mode       (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_set_automanual_mode)
#define dsk_radio_rcv_search_all            (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_search_all)
#define dsk_radio_rcv_next_freq_play        (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_next_freq_play)
#define dsk_radio_rcv_pre_freq_play         (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_pre_freq_play)
#define dsk_radio_rcv_set_play_list_type    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_set_play_list_type)
#define dsk_radio_rcv_get_play_list_type    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_play_list_type)
#define dsk_radio_rcv_get_search_result_vn  (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_search_result_vn)
#define dsk_radio_rcv_get_favorite_vn       (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_favorite_vn)
#define dsk_radio_rcv_get_search_result_chaninfo    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_search_result_chaninfo)
#define dsk_radio_rcv_get_favorite_chaninfo (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_favorite_chaninfo)
#define dsk_radio_rcv_get_favorite          (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_favorite)
#define dsk_radio_rcv_save_favorite         (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_save_favorite)
#define dsk_radio_rcv_get_search_result     (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_get_search_result)
#define dsk_radio_rcv_save_search_result    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_save_search_result)
#define dsk_radio_rcv_save_cur_freq         (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_save_cur_freq)
#define dsk_radio_rcv_add_chan_to_favorite  (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_add_chan_to_favorite)
#define dsk_radio_rcv_del_chan_from_favorite    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_rcv_del_chan_from_favorite)

#define dsk_radio_send_open                     (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_open                                )
#define dsk_radio_send_close                    (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_close                               )
#define dsk_radio_send_is_open                  (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_is_open                             )
#define dsk_radio_send_set_freq_play            (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_set_freq_play                       )
#define dsk_radio_send_get_cur_freq             (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_get_cur_freq                        )
#define dsk_radio_send_set_gain                 (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_set_gain                            )
#define dsk_radio_send_get_gain                 (DESKTOP_FUNC_ENTRY->ptr_dsk_radio_send_get_gain                            )
#endif  //USED_BY_DESKTOP

#endif  //__MOD_DESKTOP_API_H__
