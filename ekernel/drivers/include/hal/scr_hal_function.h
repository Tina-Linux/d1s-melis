#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <interrupt.h>
#include <console.h>
#include <string.h>
#include <param.h>
#include <hal_atomic.h>
#include <sunxi_hal_common.h>
#include "hal_clk.h"
#include "scr_config.h"
#include "scr_hal.h"

#ifndef _SCR_HAL_FUNCTION_H_
#define _SCR_HAL_FUNCTION_H_

#define get_wvalue	readl

void put_wvalue(u32 addr, u32 v)
{
	writel(v, addr);
}

void scr_set_csr_config(pscr_struct pscr, uint32_t config)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val &= ~(0x1ff<<16);
	reg_val |= config & (0x1ff<<16);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

uint32_t scr_get_csr_config(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_CSR_OFF);
}

uint32_t scr_detected(pscr_struct pscr)
{
	return (get_wvalue(pscr->reg_base + SCR_CSR_OFF)>>31);
}

void scr_start_deactivation(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<11);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_start_activation(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<10);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_start_warmreset(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<9);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_stop_clock(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<8);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_restart_clock(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val &= ~(0x1<<8);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_global_interrupt_enable(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<2);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_global_interrupt_disable(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val &= ~(0x1<<2);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_receive_enable(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<1);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_receive_disable(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val &= ~(0x1<<1);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_transmit_enable(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val |= (0x1<<0);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_transmit_disable(pscr_struct pscr)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CSR_OFF);
	reg_val &= ~(0x1<<0);
	put_wvalue(pscr->reg_base + SCR_CSR_OFF, reg_val);
}

void scr_set_interrupt_enable(pscr_struct pscr, uint32_t bm)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_INTEN_OFF);
	reg_val |= bm;
	put_wvalue(pscr->reg_base + SCR_INTEN_OFF, reg_val);
}

void scr_set_interrupt_disable(pscr_struct pscr, uint32_t bm)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_INTEN_OFF);
	reg_val &= ~bm;
	put_wvalue(pscr->reg_base + SCR_INTEN_OFF, reg_val);
}

uint32_t scr_get_interrupt_enable(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_INTEN_OFF);
}

uint32_t scr_get_interrupt_status(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_INTST_OFF);
}

void scr_clear_interrupt_status(pscr_struct pscr, uint32_t bm)
{
	put_wvalue(pscr->reg_base + SCR_INTST_OFF, bm);
}

void scr_flush_txfifo(pscr_struct pscr)
{
	put_wvalue(pscr->reg_base + SCR_FCSR_OFF, (0x1<<2));
}

void scr_flush_rxfifo(pscr_struct pscr)
{
	put_wvalue(pscr->reg_base + SCR_FCSR_OFF, (0x1<<10));
}

uint32_t scr_txfifo_is_empty(pscr_struct pscr)
{
	return (get_wvalue(pscr->reg_base + SCR_FCSR_OFF)&0x1);
}

uint32_t scr_txfifo_is_full(pscr_struct pscr)
{
	return ((get_wvalue(pscr->reg_base + SCR_FCSR_OFF)>>1)&0x1);
}

uint32_t scr_rxfifo_is_empty(pscr_struct pscr)
{
	return ((get_wvalue(pscr->reg_base + SCR_FCSR_OFF)>>8)&0x1);
}

uint32_t scr_rxfifo_is_full(pscr_struct pscr)
{
	return ((get_wvalue(pscr->reg_base + SCR_FCSR_OFF)>>9)&0x1);
}

void scr_set_txfifo_threshold(pscr_struct pscr, uint32_t thh)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_FCNT_OFF);
	reg_val &= ~(0xff<<16);
	reg_val |= (thh&0xff)<<16;
	put_wvalue(pscr->reg_base + SCR_FCNT_OFF, reg_val);
}

void scr_set_rxfifo_threshold(pscr_struct pscr, uint32_t thh)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_FCNT_OFF);
	reg_val &= ~(0xffU<<24);
	reg_val |= (thh&0xff)<<24;
	put_wvalue(pscr->reg_base + SCR_FCNT_OFF, reg_val);
}

uint32_t scr_get_txfifo_count(pscr_struct pscr)
{
	return (get_wvalue(pscr->reg_base + SCR_FCNT_OFF)&0xff);
}

uint32_t scr_get_rxfifo_count(pscr_struct pscr)
{
	return ((get_wvalue(pscr->reg_base + SCR_FCNT_OFF)>>8)&0xff);
}

void scr_set_tx_repeat(pscr_struct pscr, uint32_t repeat)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_RPT_OFF);
	reg_val &= ~(0xf<<0);
	reg_val |= (repeat&0xf)<<0;
	put_wvalue(pscr->reg_base + SCR_RPT_OFF, reg_val);
}

void scr_set_rx_repeat(pscr_struct pscr, uint32_t repeat)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_RPT_OFF);
	reg_val &= ~(0xf<<4);
	reg_val |= (repeat&0xf)<<4;
	put_wvalue(pscr->reg_base + SCR_RPT_OFF, reg_val);
}

void scr_set_scclk_divisor(pscr_struct pscr, uint32_t divisor)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_DIV_OFF);
	reg_val &= ~(0xffff<<0);
	reg_val |= (divisor&0xffff)<<0;
	put_wvalue(pscr->reg_base + SCR_DIV_OFF, reg_val);
}

void scr_set_baud_divisor(pscr_struct pscr, uint32_t divisor)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_DIV_OFF);
	reg_val &= ~(0xffffU<<16);
	reg_val |= (divisor&0xffff)<<16;
	put_wvalue(pscr->reg_base + SCR_DIV_OFF, reg_val);
}

uint32_t scr_get_scclk_divisor(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_DIV_OFF)&0xffff;
}

uint32_t scr_get_baud_divisor(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_DIV_OFF)>>16;
}

void scr_set_activation_time(pscr_struct pscr, uint32_t scclk)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_LTIM_OFF);
	reg_val &= ~(0xff<<0);
	reg_val |= (scclk&0xff)<<0;
	put_wvalue(pscr->reg_base + SCR_LTIM_OFF, reg_val);
}

void scr_set_reset_time(pscr_struct pscr, uint32_t scclk)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_LTIM_OFF);
	reg_val &= ~(0xff<<8);
	reg_val |= (scclk&0xff)<<8;
	put_wvalue(pscr->reg_base + SCR_LTIM_OFF, reg_val);
}

void scr_set_atrlimit_time(pscr_struct pscr, uint32_t scclk)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_LTIM_OFF);
	reg_val &= ~(0xff<<16);
	reg_val |= (scclk&0xff)<<16;
	put_wvalue(pscr->reg_base + SCR_LTIM_OFF, reg_val);
}

uint32_t scr_get_line_time(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_LTIM_OFF);
}

void scr_set_guard_time(pscr_struct pscr, uint32_t etu)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CTIM_OFF);
	reg_val &= ~(0xff<<0);
	reg_val |= (etu&0xff)<<0;
	put_wvalue(pscr->reg_base + SCR_CTIM_OFF, reg_val);
}

void scr_set_chlimit_time(pscr_struct pscr, uint32_t etu)
{
	uint32_t reg_val;

	reg_val = get_wvalue(pscr->reg_base + SCR_CTIM_OFF);
	reg_val &= ~(0xffffU<<16);
	reg_val |= (etu&0xffff)<<16;
	put_wvalue(pscr->reg_base + SCR_CTIM_OFF, reg_val);
}

void scr_set_debounce_time(pscr_struct pscr, uint32_t etu)
{
	put_wvalue(pscr->reg_base + SCR_DBE_TIME, etu);
}

uint32_t scr_get_character_time(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_CTIM_OFF);
}

void scr_auto_vpp_enable(pscr_struct pscr)
{
//	set_wbit(pscr->reg_base + SCR_LCTL_OFF, 0x1<<5);
	int ret;

	ret = readl(pscr->reg_base + SCR_LCTL_OFF);
	writel(0x20 | ret, pscr->reg_base + SCR_LCTL_OFF);
}

void scr_auto_vpp_disable(pscr_struct pscr)
{
//	clr_wbit(pscr->reg_base + SCR_LCTL_OFF, 0x1<<5);
	int ret;

	ret = readl(pscr->reg_base + SCR_LCTL_OFF);
	writel(0xdf & ret, pscr->reg_base + SCR_LCTL_OFF);
}

void scr_write_fifo(pscr_struct pscr, uint8_t data)
{
	while(get_wvalue(pscr->reg_base + SCR_FCSR_OFF)&0x2);
	put_wvalue(pscr->reg_base + SCR_FIFO_OFF, (uint32_t)data);
}

uint8_t scr_read_fifo(pscr_struct pscr)
{
	return ((uint8_t)get_wvalue(pscr->reg_base + SCR_FIFO_OFF));
}

uint32_t scr_get_fsm(pscr_struct pscr)
{
	return get_wvalue(pscr->reg_base + SCR_FSM_OFF);
}

uint32_t scr_buffer_is_empty(pscr_buffer pbuf)
{
	return (pbuf->wptr==pbuf->rptr);
}

uint32_t scr_buffer_is_full(pscr_buffer pbuf)
{
	return ((pbuf->wptr^pbuf->rptr)==(SCR_BUFFER_SIZE_MASK+1));
}

void scr_buffer_flush(pscr_buffer pbuf)
{
	pbuf->wptr = pbuf->rptr = 0;
}


extern uint32_t scr_init(pscr_struct pscr);
extern void scr_handler_irq(pscr_struct pscr);
extern void scr_fsm_record_start(pscr_struct pscr, pscr_fsm_record pfsm);
extern void scr_fsm_record_run(pscr_struct pscr, pscr_fsm_record pfsm);
extern void scr_fsm_decode(uint32_t val);

extern void scr_fill_buffer(pscr_buffer pbuf, uint8_t data);
extern uint8_t scr_dump_buffer(pscr_buffer pbuf);

extern uint32_t scr_test_interrupt_status(pscr_struct pscr);

u32 scr_rx_fifo_read(u8 *buffer);

extern uint32_t smartcard_params_init(pscatr_struct pscatr);

extern uint32_t smartcard_atr_decode(pscatr_struct pscatr, uint8_t* pdata, ppps_struct pps, uint32_t with_ts);

//#endif

#endif //_SCR_HAL_H_

