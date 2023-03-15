/* drivers/video/sunxi/disp2/disp/lcd/tft08006.h
 *
 * Copyright (c) 2021 Allwinnertech Co., Ltd.
 *
 * tft08006 panel driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _TFT08006_H
#define _TFT08006_H

#include "panels.h"

extern struct __lcd_panel tft08006_panel;

extern s32 bsp_disp_get_panel_info(u32 screen_id, struct disp_panel_para *info);

#endif /*End of file*/
