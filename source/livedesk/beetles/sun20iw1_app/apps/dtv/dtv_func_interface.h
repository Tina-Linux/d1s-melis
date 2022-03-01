#ifndef __DTV_FUNC_INTERFACE_H__
#define __DTV_FUNC_INTERFACE_h__

#include <emodules/mod_orange/gui/message/message.h>

__s32 dtv_show_select_number(__gui_msg_t *msg);

void uninit_select_dtv_bmp(void);

__s32 dtv_singal_strength_detect(H_WIN hand);
void init_select_dtv_bmp(void);
void dtv_init(void);
void dtv_uninit(void);
__bool dtv_has_sub_scene(void);


void dtv_subtitle_proc(void);
__s32 dtv_search_program_start(__gui_msg_t  *msg, __u8 scan_type, __u32 index);

__s32 dtv_schedule_list_menu_create(__gui_msg_t  *msg);

void dtv_subtitle_resume(void);

__s32 dtv_preview_menu_create(__gui_msg_t  *msg);
__s32 dtv_setting_menu_create(__gui_msg_t  *msg);

void clear_dtv_select_number(__gui_msg_t *msg);
__bool is_on_tv(void);


#endif
