#ifndef __INIT_H__
#define __INIT_H__

#include "background/gscene_backgrd.h"
#include "headbar/gscene_headbar.h"
#include "display/g_display_switch_output.h"
#include "display/g_close_screen.h"
#include "mini_music/mini_music.h"
#include "tp_adjust/tp_adjust.h"
#include "screen_lock/screen_lock.h"
#include "bookengine/bookengine.h"
#include "ui/tp_adjust_scene/scene_adjust.h"
#include "gscene_init_server.h"
#include <kconfig.h>

#include "alarm/alarm_function.h"

#include "init_api.h"

__s32 init_memdev_create(H_LYR hlyr);
__s32 init_memdev_delete(void);

#endif /* __INIT_H__ */
