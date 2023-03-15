#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <hal_clk.h>
#include "platform_resource.h"

u32 g_irq_no[] = {
	105,/*tcon-lcd0*/
};

u32 g_reg_base[] = {
	0x40700000,/*de0*/
	0x40B40000,/*disp_if_top*/
	0x40B41000,/*tcon_lcd0*/
};

struct clk_info_t g_clk_no[] = {
	{
		"clk_de0",
		CLK_DE,
		CLK_DEVICE,
		RST_DE,
		NULL,
		NULL,
		NULL,
	},
	{
		"clk_bus_de0",
		CLK_BUS_DE,
		(hal_clk_id_t)-1,
		(hal_reset_id_t)-1,
		NULL,
		NULL,
		NULL,
	},
	{
		"clk_mbus_de0",
		CLK_MBUS_DE,
		(hal_clk_id_t)-1,
		(hal_reset_id_t)-1,
		NULL,
		NULL,
		NULL,
	},
	{
		"clk_bus_dpss_top0",
		CLK_BUS_DISPLAY,
		(hal_clk_id_t)-1,
		RST_DISPLAY,
		NULL,
		NULL,
		NULL,
	},
	{
		"clk_tcon0",
		CLK_LCD,
		CLK_DEVICE,
		RST_LCD,
		NULL,
		NULL,
		NULL,
	},
	{
		"clk_bus_tcon0",
		CLK_BUS_LCD,
		(hal_clk_id_t)-1,
		(hal_reset_id_t)-1,
		NULL,
		NULL,
		NULL,
	},
};

u32 g_irq_no_len = sizeof(g_irq_no) / sizeof(u32);
u32 g_reg_base_len = sizeof(g_reg_base) / sizeof(u32);
u32 g_clk_no_len = sizeof(g_clk_no) / sizeof(struct clk_info_t);
