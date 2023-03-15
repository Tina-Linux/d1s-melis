/*
 * drivers/char/sunxi-scr/sunxi-scr.h
 *
 * Copyright (C) 2016 Allwinner.
 * fuzhaoke <fuzhaoke@allwinnertech.com>
 *
 * SUNXI SCR Register Definition
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __SUNXI_HAL_SCR_H__
#define __SUNXI_HAL_SCR_H__

//#include "scr/smartcard.h"
#include <spinlock.h>
#include <hal_clk.h>
#include <hal_reset.h>
#include <hal_gpio.h>

#define MAX_ATR_LEN	33
#define GPIO_NUM	7

/* ====================  For debug  =============================== */
#define SCR_ENTER()    pr_info("%s()%d - %s\n", __func__, __LINE__, "Enter ...")
#define SCR_EXIT()	    pr_info("%s()%d - %s\n", __func__, __LINE__, "Exit")
#define SCR_DBG(fmt, arg...) pr_debug("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SCR_INFO(fmt, arg...) pr_info("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SCR_WARN(fmt, arg...) pr_warn("%s()%d - "fmt, __func__, __LINE__, ##arg)
#define SCR_ERR(fmt, arg...)   pr_err("%s()%d - "fmt, __func__, __LINE__, ##arg)

/* ioctrol command */
enum scr_cmd {
	SCR_IOCGSTATUS = 0,
	SCR_IOCRESET,
	SCR_IOCGATR,
	SCR_IOCGPARA,
	SCR_IOCSPARA,
	SCR_IOCGATRPARA,
	SCR_IOCGPPSPARA,
	SCR_IOCWRDATA,
};

enum scr_card_sta {
	SCR_CARD_OUT = 0,
	SCR_CARD_IN,
};

/* ATR data */
struct scr_atr {
	unsigned char atr_len;
	unsigned char atr_data[MAX_ATR_LEN];
};

/* current card parameters & status */
struct scr_card_para {
	uint16_t f;
	uint16_t d;
	uint16_t freq;
	uint8_t recv_no_parity;
	uint8_t protocol_type;
};

/* parse ATR data */
struct smc_atr_para {
	uint8_t TS;

	uint8_t TK[15];
	uint8_t TK_NUM;

	uint8_t T;    /* Protocol */
	uint8_t FMAX; /* unit: MHz */
	uint32_t F;
	uint32_t D;
	uint32_t I;   /* Max Cunrrent for Program, in mA */
	uint32_t P;   /* Pogram Voltage */
	uint32_t N;   /* Extra Guard Time, in ETUs */
};

/* parse ATR data for PPS */
struct smc_pps_para {
	uint8_t ppss;
	uint8_t pps0;
	uint8_t pps1;
	uint8_t pps2;
	uint8_t pps3;
	uint8_t pck;
};

/* write cmd and read data immediately */
struct scr_wr_data {
	uint8_t *cmd_buf;
	uint32_t cmd_len;
	uint8_t *rtn_data;
	uint32_t rtn_len;
	uint8_t psw1;
	uint8_t psw2;
};

enum scr_atr_status {
	SCR_ATR_RESP_INVALID,
	SCR_ATR_RESP_FAIL,
	SCR_ATR_RESP_AGAIN,
	SCR_ATR_RESP_OK,
};

typedef enum
{
	SCR_PIN_ERR = -3,
	SCR_CLK_ERR = -2,
	SCR_IRQ_ERR = -1,
	SCR_OK = 0,
} hal_scr_status_t;

typedef struct sunxi_scr {
	hal_clk_t clk;
	hal_clk_t pclk;

	hal_clk_id_t clk_id;
	hal_clk_type_t clk_type;

	hal_clk_id_t pclk_id;
	hal_clk_type_t pclk_type;

	hal_reset_id_t reset_id;
	hal_reset_type_t reset_type;

	struct reset_control *reset;

	gpio_pin_t pin;
	uint8_t pin_mux;
	uint8_t pin_drv;

	uint16_t irq_num;
	uint32_t reg_base;

	uint32_t clk_freq;
	hal_spinlock_t rx_lock;

	bool suspended;

	/* smart card register parameters */
	uint32_t inten_bm;	/* interrupt enable bit map */
	uint32_t txfifo_thh;	/* txfifo threshold */
	uint32_t rxfifo_thh;	/* rxfifo threahold */
	uint32_t tx_repeat;	/* tx repeat */
	uint32_t rx_repeat;	/* rx repeat */
	uint32_t scclk_div;	/* scclk divisor */
	uint32_t baud_div;	/* baud divisor */
	uint8_t act_time;	/* active/deactive time, in scclk cycles */
	uint8_t rst_time;	/* reset time, in scclk cycles */
	uint8_t atr_time;	/* ATR limit time, in scclk cycles */
	uint32_t guard_time;	/* gaurd time, in ETUs */
	uint32_t chlimit_time;	/* character limit time, in ETUs */

	/* some necessary flags */
//	volatile uint8_t atr_resp;
	uint8_t atr_resp;
//	volatile uint8_t rx_transmit_status;
	uint8_t rx_transmit_status;

	struct scr_card_para card_para;
	struct scr_atr scr_atr_des;
	struct smc_atr_para smc_atr_para;
	struct smc_pps_para smc_pps_para;

//	wait_queue_head_t scr_poll;
	struct timer_list *poll_timer; // fix me
	bool card_in;
	bool card_last;
} hal_scr_t;

typedef struct {
	uint32_t gpio;
	uint8_t enable_mux;
	uint8_t disable_mex;
} scr_gpio_t;

/* test function */
void hal_scr_test(enum scr_cmd cmd, void *arg);
hal_scr_status_t hal_scr_init(void);
/*
void smartcard_atr_decode(struct smc_atr_para *pscatr, struct smc_pps_para *psmc_pps,
			uint8_t *pdata, uint8_t with_ts);
void smartcard_ta1_decode(struct smc_atr_para *psmc_atr, uint8_t ta1)
void smartcard_tb1_decode(struct smc_atr_para *psmc_atr, uint8_t tb1)
*/
#endif
