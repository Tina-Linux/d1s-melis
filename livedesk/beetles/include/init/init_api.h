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
#ifndef __MOD_INIT_API_H__
#define __MOD_INIT_API_H__
#include <kconfig.h>

typedef struct INIT_API_ENTRY
{
    /* gscene_backgrd.h */
    __s32           (*ptr_gscene_bgd_init)(SIZE *p_size, bgd_status_t status, __fb_type_t ftype);
    void            (*ptr_gscene_bgd_set_state)(bgd_status_t status);
    bgd_status_t    (*ptr_gscene_bgd_get_state)(void);
    void            (*ptr_gscene_bgd_set_top)(void);
    void            (*ptr_gscene_bgd_set_bottom)(void);
    __s32           (*ptr_gscene_bgd_save_fb)(FB *fb, char *path);
    void            (*ptr_gscene_bgd_set_fb_type)(__fb_type_t ftype);
    void            (*ptr_gscene_bgd_refresh)(void);
    void            (*ptr_gscene_bgd_restore)(void);
    __s32           (*ptr_gscene_bgd_deinit)(void);
    __bool          (*ptr_gscene_bgd_get_flag)(void);
    void            (*ptr_gscene_bgd_set_flag)(__bool flag);
    __s32           (*ptr_gscene_bgd_get_default_bg_index)(void);
    __s32           (*ptr_gscene_bgd_set_default_bg_index)(__u32 index);
    void            (*ptr_gscene_bgd_reserv[6])(void);

    /* gscene_headbar.h */
    signed long     (*ptr_gscene_hbar_create)(H_WIN parent, __hbar_format_t format);
    __s32           (*ptr_gscene_hbar_set_state)(HBarState state);
    __s32           (*ptr_gscene_hbar_get_state)(HBarState *p_state);
    __s32           (*ptr_gscene_hbar_set_title)(char *title, __u32 len);              /* title 为utf8编码格式字符串，len<=32 ,设置hbar title 区域字符串*/
    __s32           (*ptr_gscene_hbar_set_sig_status)(__u8 st, __u8 qa, __bool en);
    __s32           (*ptr_gscene_hbar_get_screen_rect)(RECT *rect);
    __s32           (*ptr_gscene_hbar_delete)(void);
    void            (*ptr_gscene_hbar_set_music_state)(__bg_audio_t state);
    __s32           (*ptr_gscene_hbar_update_text)(__s32 id);
    __s32           (*ptr_gscene_hbar_reserv[9])(void);

    //display\g_display_switch_output.h
    __s32           (*ptr_g_display_switch_output)(__lion_disp_format_e mode);
    void            (*ptr_g_display_switch_output_reserved[5])(void);

    //display\g_close_screen.h
    __s32           (*ptr_g_enable_close_scn)(void);
    __s32           (*ptr_g_disable_close_scn)(void);
    __s32           (*ptr_g_set_close_scn_time)(__u32 time);
    __s32           (*ptr_g_get_close_scn_time)(void);
    __s32           (*ptr_g_close_screen)(void);
    void            (*ptr_g_enable_standby)(void);
    void            (*ptr_g_disable_standby)(void);
    __bool          (*ptr_g_is_enable_standby)(void);
    void            (*ptr_g_close_scn_reserved[1])(void);

    //mini_music\mini_music.h
    __s32           (*ptr_mini_music_open)(H_WIN parent, __s32 x, __s32 y);
    __s32           (*ptr_mini_music_close)(void);
    __bool          (*ptr_mini_music_scene_exist)(void);

    //bookengine.h
    __s32           (*ptr_bookengine_open)(void);
    __s32           (*ptr_bookengine_close)(void);

    //tp_adjust_scene
    H_WIN           (*ptr_scene_adjust_create)(H_WIN);
    __s32           (*ptr_scene_adjust_destroy)(H_WIN);

    //alarm_function.h
    __bool          (*ptr_alarm_is_exist)(void);
    reg_alarm_para_t       *(*ptr_alarm_get_happening)(void);
    __bool(*ptr_alarm_check_run)(void);
    void (*ptr_alarm_settingTime_cleanHappen)(__s16 alarmIndex);
    __s32(*ptr_standby_startup_alarm)(void);
    void (*ptr_alarm_enter_app)(void);
    void (*ptr_alarm_exit_app)(void);

    //重新设置关屏定时器
    void (*ptr_init_reset_close_scn)(void);

    //重新设置关机定时器
    void (*ptr_init_reset_auto_off)(void);
} init_api_entry_t;

#ifndef USED_BY_INIT     //not used by init module itsef

//this address is depended on the link address of mod_desktop
#define INIT_ENTRY_TBL                  ((EMOD_TYPE_MOD_INIT << 20) |(0xe0000000))
#define INIT_FUNC_ENTRY                 ((init_api_entry_t *)INIT_ENTRY_TBL)

/* gscene_backgrd.h */
#define gscene_bgd_init                 (INIT_FUNC_ENTRY->ptr_gscene_bgd_init           )
#define gscene_bgd_set_state            (INIT_FUNC_ENTRY->ptr_gscene_bgd_set_state      )
#define gscene_bgd_get_state            (INIT_FUNC_ENTRY->ptr_gscene_bgd_get_state      )
#define gscene_bgd_set_top              (INIT_FUNC_ENTRY->ptr_gscene_bgd_set_top        )
#define gscene_bgd_set_bottom           (INIT_FUNC_ENTRY->ptr_gscene_bgd_set_bottom     )
#define gscene_bgd_save_fb              (INIT_FUNC_ENTRY->ptr_gscene_bgd_save_fb        )
#define gscene_bgd_set_fb_type          (INIT_FUNC_ENTRY->ptr_gscene_bgd_set_fb_type    )
#define gscene_bgd_refresh              (INIT_FUNC_ENTRY->ptr_gscene_bgd_refresh        )
#define gscene_bgd_restore              (INIT_FUNC_ENTRY->ptr_gscene_bgd_restore        )
#define gscene_bgd_deinit               (INIT_FUNC_ENTRY->ptr_gscene_bgd_deinit         )
#define gscene_bgd_get_flag             (INIT_FUNC_ENTRY->ptr_gscene_bgd_get_flag       )
#define gscene_bgd_set_flag             (INIT_FUNC_ENTRY->ptr_gscene_bgd_set_flag       )
#define gscene_bgd_get_default_bg_index         (INIT_FUNC_ENTRY->ptr_gscene_bgd_get_default_bg_index       )
#define gscene_bgd_set_default_bg_index         (INIT_FUNC_ENTRY->ptr_gscene_bgd_set_default_bg_index       )

/* gscene_headbar.h */
#define gscene_hbar_create              (INIT_FUNC_ENTRY->ptr_gscene_hbar_create        )
#define gscene_hbar_set_state           (INIT_FUNC_ENTRY->ptr_gscene_hbar_set_state     )
#define gscene_hbar_get_state           (INIT_FUNC_ENTRY->ptr_gscene_hbar_get_state     )
#define gscene_hbar_set_title           (INIT_FUNC_ENTRY->ptr_gscene_hbar_set_title     )
#define gscene_hbar_set_sig_status      (INIT_FUNC_ENTRY->ptr_gscene_hbar_set_sig_status        )
#define gscene_hbar_get_screen_rect     (INIT_FUNC_ENTRY->ptr_gscene_hbar_get_screen_rect)
#define gscene_hbar_delete              (INIT_FUNC_ENTRY->ptr_gscene_hbar_delete        )
#define gscene_hbar_set_music_state     (INIT_FUNC_ENTRY->ptr_gscene_hbar_set_music_state)
#define gscene_hbar_update_text         (INIT_FUNC_ENTRY->ptr_gscene_hbar_update_text)

//display\g_display_switch_output.h
#define g_display_switch_output         (INIT_FUNC_ENTRY->ptr_g_display_switch_output   )

//display\g_close_screen.h
#define g_enable_close_scn          /*eLIBs_printf("file:%s, ln:%d enable close scn\n", __FILE__, __LINE__);*/(INIT_FUNC_ENTRY->ptr_g_enable_close_scn      )
#define g_disable_close_scn         /*eLIBs_printf("file:%s, ln:%d disable close scn\n", __FILE__, __LINE__);*/(INIT_FUNC_ENTRY->ptr_g_disable_close_scn        )
#define g_set_close_scn_time(x)     /*eLIBs_printf("file:%s, ln:%d close scn time=%d\n", __FILE__, __LINE__, x);*/(INIT_FUNC_ENTRY->ptr_g_set_close_scn_time(x)     )
#define g_get_close_scn_time        /*eLIBs_printf("file:%s, ln:%d close scn time=%d\n", __FILE__, __LINE__, x);*/(INIT_FUNC_ENTRY->ptr_g_get_close_scn_time        )

#define g_close_screen                  (INIT_FUNC_ENTRY->ptr_g_close_screen            )
#define g_enable_standby                (INIT_FUNC_ENTRY->ptr_g_enable_standby          )
#define g_disable_standby               (INIT_FUNC_ENTRY->ptr_g_disable_standby         )
#define g_is_enable_standby             (INIT_FUNC_ENTRY->ptr_g_is_enable_standby       )

//mini_music\mini_music.h
#define mini_music_open                 (INIT_FUNC_ENTRY->ptr_mini_music_open           )
#define mini_music_close                (INIT_FUNC_ENTRY->ptr_mini_music_close          )
#define mini_music_scene_exist          (INIT_FUNC_ENTRY->ptr_mini_music_scene_exist    )

//bookengine.h
#define bookengine_open                 (INIT_FUNC_ENTRY->ptr_bookengine_open           )
#define bookengine_close                (INIT_FUNC_ENTRY->ptr_bookengine_close          )

//tp_adjust_scene.h
#define scene_adjust_create             (INIT_FUNC_ENTRY->ptr_scene_adjust_create       )
#define scene_adjust_destroy            (INIT_FUNC_ENTRY->ptr_scene_adjust_destroy      )

//alarm_function.h
#define alarm_is_exist                  (INIT_FUNC_ENTRY->ptr_alarm_is_exist            )
#define alarm_get_happening             (INIT_FUNC_ENTRY->ptr_alarm_get_happening           )
#define alarm_check_run                 (INIT_FUNC_ENTRY->ptr_alarm_check_run               )
#define alarm_settingTime_cleanHappen   (INIT_FUNC_ENTRY->ptr_alarm_settingTime_cleanHappen )
#define standby_startup_alarm           (INIT_FUNC_ENTRY->ptr_standby_startup_alarm         )
#define alarm_enter_app                 (INIT_FUNC_ENTRY->ptr_alarm_enter_app               )
#define alarm_exit_app                  (INIT_FUNC_ENTRY->ptr_alarm_exit_app                )

#define init_reset_close_scn            (INIT_FUNC_ENTRY->ptr_init_reset_close_scn              )
#define init_reset_auto_off             (INIT_FUNC_ENTRY->ptr_init_reset_auto_off               )

#endif /* USED_BY_INIT */

#endif /* __MOD_INIT_API_H__ */
