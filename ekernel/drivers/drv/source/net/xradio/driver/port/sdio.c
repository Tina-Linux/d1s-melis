#include "sdio.h"
#include "delay.h"

#ifdef __MELIS__

unsigned int sdio_align_size(xr_drv_sdio_t *func, unsigned int sz)
{
	/* Do a first check with the controller, in case it
	 * wants to increase the size up to a point where it
	 * might need more than one block.
	 */
	if (sz && sz & 3) {
		sz &= ~0x03;
		sz += 0x04;
	}

	/* The controller is simply incapable of transferring the size
	 * we want in decent manner, so just return the original size.
	 */
	return sz;
}

#elif (defined(__CONFIG_ARCH_STM32))

#elif (defined(__CONFIG_CHIP_XRADIO))
#include "sys/xr_debug.h"

#include "../common/platform.h"

void SDIO_PowerON(xr_drv_sdio_t *card)
{
	SDC_InitTypeDef sdc_param = { 0 };

#ifdef CONFIG_DETECT_CARD
	sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
#endif
	sdc_param.debug_mask = 0;
	//sdc_param.debug_mask = ROM_WRN_MASK|ROM_ERR_MASK|ROM_ANY_MASK;
	sdc_param.dma_use = 0;

	/* power on sdio host */
	card->host = HAL_SDC_Create(CONFIG_SDC_ID, &sdc_param);
	HAL_SDC_Init(card->host);
}

void SDIO_PowerOFF(xr_drv_sdio_t *card)
{
	HAL_SDC_Deinit(CONFIG_SDC_ID);
	HAL_SDC_Destory(card->host);
	card->host = NULL;
}

int SDIO_Initialize_Cards(xr_drv_sdio_t *card)
{
	int ret;

	ret = mmc_rescan(card, CONFIG_SDC_ID);
	if (ret) {
		return ret;
	}

	return 0;
}

int SDIO_DeInitialize_Cards(xr_drv_sdio_t *card)
{
	int ret;

	ret = mmc_card_deinit(card);
	if (ret) {
	    return ret;
	}

	return 0;
}

#elif (defined(CONFIG_OS_RTTHREAD))
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rtdbg.h>

#include "delay.h"

#include "aw_drv/aw_pio.h"
#include "drivers/mmcsd_core.h"

unsigned char sdio_readb(xr_drv_sdio_t *card, unsigned int func_num, unsigned int addr, int *err_ret)
{
	return sdio_io_readb(card->sdio_function[func_num], addr, err_ret);
}

void sdio_writeb(xr_drv_sdio_t *card, unsigned int func_num, const unsigned char b, unsigned int addr, int *err_ret)
{
	int32_t ret;

	ret = sdio_io_writeb(card->sdio_function[func_num], addr, b);
	if (err_ret)
		*err_ret = ret;
}

int sdio_memcpy_fromio(xr_drv_sdio_t *card, unsigned int func_num, void *dst, unsigned int addr, int count)
{
	return sdio_io_read_multi_incr_b(card->sdio_function[func_num], addr, dst, count);
}

int sdio_memcpy_toio(xr_drv_sdio_t *card, unsigned int func_num, unsigned int addr, const void *src, int count)
{
	return sdio_io_write_multi_incr_b(card->sdio_function[func_num], addr, (rt_uint8_t *)src, count);
}

unsigned int sdio_align_size(xr_drv_sdio_t *card, unsigned int sz)
{
	/* Do a first check with the controller, in case it
	 * wants to increase the size up to a point where it
	 * might need more than one block.
	 */
	if (sz && sz & 3) {
		sz &= ~0x03;
		sz += 0x04;
	}

	/* The controller is simply incapable of transferring the size
	 * we want in decent manner, so just return the original size.
	 */
	return sz;
}

#ifndef CONFIG_USE_RTTHREAD_PORT
static struct rt_mmcsd_host *sdio_host;

xr_drv_sdio_t *xradio_sdio_detect(unsigned int id, int enable)
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
	xr_sdio_enable_func(sdio_host->card, FN1);
	xr_drv_msleep(1);

	return sdio_host->card;
}

void xradio_sdio_deinit(xr_drv_sdio_t *card)
{
	if (!card)
		return ;
	xr_sdio_disable_func(card, FN1);

	mmcsd_change(sdio_host);
	mmcsd_wait_cd_changed(2000);
	sdio_device_deinit(sdio_host);
}
#endif

#elif (defined(CONFIG_OS_NUTTX))
#include <stdio.h>
#define SDIO_DBG printf
#include "sdio.h"
static void SDIO_PowerON(xr_drv_sdio_t *card)
{
	SDC_InitTypeDef sdc_param = { 0 };

#ifdef CONFIG_DETECT_CARD
	sdc_param.cd_mode = CARD_ALWAYS_PRESENT;
#endif
	sdc_param.debug_mask = ROM_WRN_MASK|ROM_ERR_MASK;
#ifdef CONFIG_SDC_DMA_USED
	sdc_param.dma_use = 1;
#else
	sdc_param.dma_use = 0;
#endif
	card->host = hal_sdc_create(CONFIG_SDC_ID, &sdc_param);

	hal_sdc_init(card->host);
}

static void SDIO_PowerOFF(xr_drv_sdio_t *card)
{
	hal_sdc_deinit(CONFIG_SDC_ID);
	hal_sdc_destroy(card->host);
}

static int SDIO_Initialize_Cards(xr_drv_sdio_t *card)
{
	int ret;

	ret = mmc_rescan(card, CONFIG_SDC_ID);
	if (ret) {
		return ret;
	}

	return 0;
}

static int SDIO_DeInitialize_Cards(xr_drv_sdio_t *card)
{
	int ret;

	ret = mmc_card_deinit(card);
	if (ret) {
	    return ret;
	}

	return 0;
}

xr_drv_sdio_t *xradio_sdio_detect(unsigned int id, int enable)
{
	xr_drv_sdio_t *card;
	SDCard_InitTypeDef card_param = { 0 };
	card_param.type = MMC_TYPE_SDIO;
	card_param.debug_mask = ROM_INF_MASK | ROM_WRN_MASK | ROM_ERR_MASK;

	mmc_card_create(CONFIG_SDC_ID, &card_param);
	card = mmc_card_open(CONFIG_SDC_ID);
	SDIO_PowerON(card);

	SDIO_Initialize_Cards(card);

	sdio_enable_func(card, FN1);

	OS_MSleep(1);
	mmc_card_close(CONFIG_SDC_ID);

	return card;
}

void xradio_sdio_deinit(xr_drv_sdio_t *card)
{
	if (!card)
		return ;
	sdio_disable_func(card, FN1);

	SDIO_DeInitialize_Cards(card);

	SDIO_PowerOFF(card);

	mmc_card_delete(CONFIG_SDC_ID);
}

#endif /* __MELIS__ */
