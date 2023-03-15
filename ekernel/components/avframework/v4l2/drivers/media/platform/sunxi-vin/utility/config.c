
/*
 * config.c for device tree and sensor list parser.
 *
 * Copyright (c) 2017 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 * Yang Feng <yangfeng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_gpio.h>
#include <sunxi_hal_regulator.h>
#include "config.h"
#include "../platform/platform_cfg.h"

#ifdef CONFIG_SOC_SUN8IW19P1
#include "vin_config_sun8iw19p1.h"
#elif CONFIG_SOC_SUN20IW1P1
#include "vin_config_sun20iw1p1.h"
#endif

#ifdef CONFIG_DRIVER_SYSCONFIG
#include <script.h>

struct FetchFunArr {
	char *sub;
	int flag;
	int (*fun)(void *, char *, char *, struct sensor_list *);
};

static int get_mname(void *phandle, char *mainkey, char *subkey,
		     struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, (int *)sc->inst[0].cam_name, sizeof(sc->inst[0].cam_name) >> 2);
}

static int get_twi_addr(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->inst[0].cam_addr, 1);
}

static int get_cam_type(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->inst[0].cam_type, 1);
}

static int get_twi_cci_spi(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->sensor_bus_type, 1);
}

static int get_twi_id(void *phandle, char *mainkey, char *subkey,
		      struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->sensor_bus_sel, 1);
}

static int get_mclk_id(void *phandle, char *mainkey, char *subkey,
		      struct sensor_list *sc)
{
	if (script_parser_fetch(phandle, mainkey, subkey, &sc->mclk_id, 1))
		sc->mclk_id = -1;
	return 0;
}

static int get_pos(void *phandle, char *mainkey, char *subkey,
		   struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, (int *)sc->sensor_pos, sizeof(sc->sensor_pos) >> 2);
}

static int get_isp_used(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->inst[0].is_isp_used, 1);
}

static int get_fmt(void *phandle, char *mainkey, char *subkey,
		   struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->inst[0].is_bayer_raw, 1);
}

static int get_vflip(void *phandle, char *mainkey, char *subkey,
		     struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->inst[0].vflip, 1);
}

static int get_hflip(void *phandle, char *mainkey, char *subkey,
		     struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->inst[0].hflip, 1);
}

static int get_cameravdd_vol(void *phandle, char *mainkey, char *subkey,
			 struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->power[CAMERAVDD].power_vol, 1);
}

static int get_iovdd_vol(void *phandle, char *mainkey, char *subkey,
			 struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->power[IOVDD].power_vol, 1);
}

static int get_avdd_vol(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->power[AVDD].power_vol, 1);
}

static int get_dvdd_vol(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->power[DVDD].power_vol, 1);
}

#ifdef CONFIG_ACTUATOR_MODULE
static int get_afvdd_vol(void *phandle, char *mainkey, char *subkey,
			 struct sensor_list *sc)
{
	return script_parser_fetch(phandle, mainkey, subkey, &sc->power[AFVDD].power_vol, 1);
}
#endif

static int get_gpio_common(void *phandle, char *mainkey, char *subkey,
		     struct sensor_list *sc, struct gpio_config *gc)
{
	int ret = 0;
	script_gpio_set_t gpio_cfg = {0};

	ret = script_parser_fetch(phandle, mainkey, subkey, (int *)&gpio_cfg, sizeof(script_gpio_set_t) >> 2);

	if (ret || (gpio_cfg.port == 0)) {
		gc->gpio = GPIO_INDEX_INVALID;
	} else {
		gc->gpio = (gpio_cfg.port - 1) * 32 + gpio_cfg.port_num;
		gc->data = gpio_cfg.data;
		gc->mul_sel = gpio_cfg.mul_sel;
		gc->pull = gpio_cfg.pull;
		gc->drv_level = gpio_cfg.drv_level;
	}

	vin_log(VIN_LOG_CONFIG, "GPIO %s: %d <%d><%d><%d><%d>\n", subkey, gc->gpio,
			gc->mul_sel, gc->pull, gc->drv_level, gc->data);

	return ret;
}

static int get_power_en(void *phandle, char *mainkey, char *subkey,
			struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[POWER_EN];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_reset(void *phandle, char *mainkey, char *subkey,
		     struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[RESET];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_pwdn(void *phandle, char *mainkey, char *subkey,
		    struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[PWDN];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_sm_hs(void *phandle, char *mainkey, char *subkey,
		    struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[SM_HS];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_sm_vs(void *phandle, char *mainkey, char *subkey,
		    struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[SM_VS];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_af_pwdn(void *phandle, char *mainkey, char *subkey,
		    struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[AF_PWDN];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_flash_en(void *phandle, char *mainkey, char *subkey,
		    struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[FLASH_EN];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static int get_flash_mode(void *phandle, char *mainkey, char *subkey,
		    struct sensor_list *sc)
{
	int ret = 0;
	struct gpio_config *gc = &sc->gpio[FLASH_MODE];

	return get_gpio_common(phandle, mainkey, subkey, sc, gc);
}

static struct FetchFunArr fetch_camera[] = {
	{"mname", 0, get_mname,},
	{"twi_addr", 0, get_twi_addr,},
	{"cam_type", 0, get_cam_type,},
	{"twi_cci_spi", 1, get_twi_cci_spi,},
	{"twi_cci_id", 1, get_twi_id,},
	{"mclk_id", 1, get_mclk_id,},
	{"pos", 1, get_pos,},
	{"isp_used", 1, get_isp_used,},
	{"fmt", 1, get_fmt,},
	{"vflip", 1, get_vflip,},
	{"hflip", 1, get_hflip,},
	{"cameravdd_vol", 1, get_cameravdd_vol,},
	{"iovdd_vol", 1, get_iovdd_vol},
	{"avdd_vol", 1, get_avdd_vol,},
	{"dvdd_vol", 1, get_dvdd_vol,},
	{"power_en", 1, get_power_en,},
	{"reset", 1, get_reset,},
	{"pwdn", 1, get_pwdn,},
	{"sm_hs", 1, get_sm_hs,},
	{"sm_vs", 1, get_sm_vs,},
	{"af_pwdn", 1, get_af_pwdn,},
	{"flash_en", 1, get_flash_en,},
	{"flash_mode", 1, get_flash_mode,},
};

struct FetchCsiFunArr {
	char *sub;
	int flag;
	int (*fun)(void *, char *, char *, int *);
};

#define CSI_PIN_FUNC(PIN_NAME) \
static int get_csi_##PIN_NAME(void *phandle, char *mainkey, char *subkey, int *gpio) \
{ \
	int ret = 0; \
	script_gpio_set_t gpio_cfg = {0}; \
	ret = script_parser_fetch(phandle, mainkey, subkey, (int *)&gpio_cfg, sizeof(script_gpio_set_t) >> 2); \
	if (ret || (gpio_cfg.port == 0)) { \
		*gpio = GPIO_INDEX_INVALID; \
		ret = -1; \
	} else { \
		*gpio = (gpio_cfg.port - 1) * 32 + gpio_cfg.port_num; \
	} \
	return ret; \
}

CSI_PIN_FUNC(pck)
CSI_PIN_FUNC(hsync)
CSI_PIN_FUNC(vsync)
CSI_PIN_FUNC(d0)
CSI_PIN_FUNC(d1)
CSI_PIN_FUNC(d2)
CSI_PIN_FUNC(d3)
CSI_PIN_FUNC(d4)
CSI_PIN_FUNC(d5)
CSI_PIN_FUNC(d6)
CSI_PIN_FUNC(d7)
CSI_PIN_FUNC(d8)
CSI_PIN_FUNC(d9)
CSI_PIN_FUNC(d10)
CSI_PIN_FUNC(d11)
CSI_PIN_FUNC(d12)
CSI_PIN_FUNC(d13)
CSI_PIN_FUNC(d14)
CSI_PIN_FUNC(d15)

static struct FetchCsiFunArr fetch_csi_pins[] = {
	{"pck", 1, get_csi_pck,},
	{"hsync", 1, get_csi_hsync,},
	{"vsync", 1, get_csi_vsync,},
	{"d0", 1, get_csi_d0,},
	{"d1", 1, get_csi_d1,},
	{"d2", 1, get_csi_d2,},
	{"d3", 1, get_csi_d3,},
	{"d4", 1, get_csi_d4,},
	{"d5", 1, get_csi_d5,},
	{"d6", 1, get_csi_d6,},
	{"d7", 1, get_csi_d7,},
};

#endif

int parse_modules_from_device_tree(struct vin_md *vind)
{
	struct vin_power *power = NULL;
	int i = 0;
	int j = 0;

#ifdef CONFIG_DRIVER_SYSCONFIG
	int k = 0;
	char mainkey[16] = {0};
	char property[32] = {0};
	int ret = 0;

	void *phandle = script_get_handle();
	if (phandle == NULL) {
		vin_err("fatal error, fex not initialized.");
		return -1;
	}

	sprintf(mainkey, "vind%d", 0);
	if (script_parser_subkey_count(phandle, mainkey) < 0) {
		vin_err("sysconfig [%s] is not defined\n", mainkey);
		return -1;
	}

	for (i = 0; i < VIN_MAX_CSI; i++) {
		sprintf(mainkey, "vind0/sensor%d", i);
		if (script_parser_subkey_count(phandle, mainkey) < 0) {
			vin_log(VIN_LOG_CONFIG, "sysconfig [%s] is not defined\n", mainkey);
			continue;
		}

		for (j = 0; j < ARRAY_SIZE(fetch_camera); j++) {
			sprintf(property, "sensor%d_%s", i, fetch_camera[j].sub);
			ret = fetch_camera[j].fun(phandle, mainkey, property, &sensors_default[i]);
			if (ret) {
				vin_log(VIN_LOG_CONFIG, "sysconfig [%s]:%s is not defined\n", mainkey, property);
			}
		}

		sensors_default[i].detect_num = 1;

		sprintf(mainkey, "vind0/csi%d", i);
		if (script_parser_subkey_count(phandle, mainkey) < 0) {
			vin_log(VIN_LOG_CONFIG, "sysconfig [%s] is not defined\n", mainkey);
			continue;
		}

		if (ARRAY_SIZE(fetch_csi_pins) != ARRAY_SIZE(vind_csi_parallel_pins[i])) {
			vin_err("vind_csi_parallel_pins array size should be %d\n", ARRAY_SIZE(fetch_csi_pins));
			return -1;
		}

		for (j = 0; j < ARRAY_SIZE(fetch_csi_pins); j++) {
			vind_csi_parallel_pins[i][j] = GPIO_INDEX_INVALID;
			sprintf(property, "csi%d_%s", i, fetch_csi_pins[j].sub);
			ret = fetch_csi_pins[j].fun(phandle, mainkey, property, &vind_csi_parallel_pins[i][j]);
			if (ret) {
				vin_log(VIN_LOG_CONFIG, "sysconfig [%s]:%s is not defined\n", mainkey, property);
			}
			vin_log(VIN_LOG_CONFIG, "GPIO: %d\n", vind_csi_parallel_pins[i][j]);
		}
	}

	for (i = 0; i < VIN_MAX_DEV; i++) {
		j = 0;
		sprintf(mainkey, "vind0/vinc%d", i);
		if (script_parser_subkey_count(phandle, mainkey) < 0) {
			printf("sysconfig [%s] is not defined\n", mainkey);
			continue;
		}

		sprintf(property, "vinc%d_rear_sensor_sel", i);
		if (script_parser_fetch(phandle, mainkey, property, &j, 1))
			j = 0;

		sprintf(property, "vinc%d_csi_sel", i);
		script_parser_fetch(phandle, mainkey, property, &sensors_default[j].csi_sel, 1);

		sprintf(property, "vinc%d_front_sensor_sel", i);
		if (script_parser_fetch(phandle, mainkey, property, &j, 1))
			j = 1;

		sprintf(property, "vinc%d_csi_sel", i);
		script_parser_fetch(phandle, mainkey, property, &sensors_default[j].csi_sel, 1);
	}
#endif

	for (j = 0; j < VIN_MAX_DEV; j++) {
		for (i = 0; i < MAX_POW_NUM; i++) {
			power = &sensors_default[j].power[i];

			if (power->regulator_id < AXP2101_ID_MAX && power->power_vol != 0) {
				power->pmic = rt_calloc(1, sizeof(struct regulator_dev));
				if (power->pmic == NULL) {
					vin_err("fatal error: calloc %d error\n", sizeof(struct regulator_dev));
					return -1;
				}
//				hal_regulator_get(REGULATOR_GET(AXP2101_REGULATOR, power->regulator_id), power->pmic);
			}
		}
	}

	for (i = 0; i < VIN_MAX_DEV; i++) {
		vind->modules[i].sensors = &sensors_default[i];
	}

	return 0;
}

void *sunxi_vin_get_csi_base(int id)
{
	return (void *)vin_csi_base[id];
}

void *sunxi_vin_get_scaler_base(int id)
{
	return (void *)vin_scaler_base[id];
}

void *sunxi_vin_get_isp_base(int id)
{
	return (void *)vin_isp_base[id];
}

int sunxi_vin_get_isp_irq(int id)
{
	return vin_isp_irq[id];
}

void *sunxi_vin_get_mipi_base(int id)
{
	return (void *)vin_mipi_base[id];
}

void *sunxi_vin_get_csi_top_base(void)
{
	return (void *)CSI_TOP_REGS_BASE;
}

void *sunxi_vin_get_csi_ccu_base(void)
{
	return (void *)CSI_CCU_REGS_BASE;
}

int sunxi_vin_get_csi_top_irq(void)
{
	return SUNXI_IRQ_CSI_TOP_PKT;
}

struct vin_clk_info *sunxi_vin_get_clk(int id)
{
	return &vind_default_clk[id];
}

#ifdef CONFIG_SOC_SUN20IW1P1
struct reset_control *sunxi_vin_get_bus_rst(void)
{
	return bus_rst;
}

struct vin_clk_info *sunxi_vin_get_bus_clk(int id)
{
	return &vind_default_bus_clk[id];
}
#endif

struct vin_mclk_info *sunxi_vin_get_mclk(int id)
{
	return &vind_default_mclk[id];
}

#if !defined CONFIG_SOC_SUN20IW1P1
struct vin_clk_info *sunxi_vin_get_isp_clk(int id)
{
	return &vind_default_isp_clk[id];
}
#endif

struct vin_core *sunxi_vin_get_vin_core(int id)
{
	return &sunxi_vinc[id];
}

void *sunxi_vin_get_dma_base(int id)
{
	return (void *)vin_dma_base[id];
}

int sunxi_vin_get_dma_irq(int id)
{
	return vin_dma_irq[id];
}

int *sunxi_vin_get_csi_pins(int id)
{
	return vind_csi_parallel_pins[id];
}

int sunxi_vin_get_csi_pins_num(void)
{
	return ARRAY_SIZE(vind_csi_parallel_pins[0]);
}

