#include "sdio.h"


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rtdbg.h>

#include "aw_drv/aw_pio.h"
#include "drivers/mmcsd_core.h"

static struct rt_mmcsd_host *sdio_host;

struct rt_mmcsd_card *xradio_sdio_detect(unsigned int id, int enable)
{
	struct sdio_init init = {
		.ck_port = GPIO_PORT_G,
		.cmd_port = GPIO_PORT_G,
		.d0_port = GPIO_PORT_G,
		.d1_port = GPIO_PORT_G,
		.d2_port = GPIO_PORT_G,
		.d3_port = GPIO_PORT_G,

		.ck_pin = GPIO_PIN_0,
		.cmd_pin = GPIO_PIN_1,
		.d0_pin = GPIO_PIN_2,
		.d1_pin = GPIO_PIN_3,
		.d2_pin = GPIO_PIN_4,
		.d3_pin = GPIO_PIN_5,

		.ck_io_func = IO_FUN_2,
		.cmd_io_func = IO_FUN_2,
		.d0_io_func = IO_FUN_2,
		.d1_io_func = IO_FUN_2,
		.d2_io_func = IO_FUN_2,
		.d3_io_func = IO_FUN_2,
		.bus_width = 4,
	};

	init.irq_num = aw_get_irq_num("SMHC1");
	if (init.irq_num == -1) {
		printf("Get irq channel error!");
		return NULL;
	}
	if (aw_get_instance_space("SMHC1", &init.start_addr, &init.end_addr) != 0) {
		printf("Get instance space error!");
		return NULL;
	}
	init.mod_clk = clk_get("sdmmc1_mod");
	clk_set_parent(init.mod_clk, clk_get("pll_periph0x2"));
	init.rst_clk = clk_get("sdmmc1_rst");
	clk_set_parent(init.rst_clk, clk_get("pll_periph0x2"));
	init.bus_clk = clk_get("sdmmc1_bus");
	clk_set_parent(init.bus_clk, clk_get("pll_periph0x2"));
	sdio_host = sdio_device_init(&init);
	if (!sdio_host) {
		printf("%s,%d faild create sdio_host!\n", __func__, __LINE__);
		return NULL;
	}

	mmcsd_change(sdio_host);
	mmcsd_wait_cd_changed(2000);
	if (!sdio_host->card) {
		printf("%s,%d faild init card!\n", __func__, __LINE__);
		return NULL;
	}

	if (!sdio_host->card->sdio_function[1]) {
		printf("%s,%d no fun1 found!\n", __func__, __LINE__);
		return NULL;
	}
	sdio_enable_func(sdio_host->card->sdio_function[1]);
	rt_thread_mdelay(1);

	return sdio_host->card;
}

void xradio_sdio_deinit(struct rt_mmcsd_card *card)
{
	if (!card)
		return ;
	sdio_disable_func(card->sdio_function[1]);

	mmcsd_change(sdio_host);
	mmcsd_wait_cd_changed(2000);
	sdio_device_deinit(sdio_host);
}

