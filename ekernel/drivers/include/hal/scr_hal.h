#include <stdio.h>
#include <interrupt.h>
#include <string.h>
#include "scr_config.h"

#ifndef _SCR_HAL_H_
#define _SCR_HAL_H_

//#ifdef CONFIG_SCR_TEST

typedef struct
{
	volatile uint32_t wptr;
	volatile uint32_t rptr;
	#define  SCR_BUFFER_SIZE_MASK		0xff
	volatile uint8_t buffer[SCR_BUFFER_SIZE_MASK+1];
}scr_buffer, *pscr_buffer;

typedef struct {
	uint32_t reg_base;
	uint32_t irq_no;
	volatile uint32_t irq_accsta;
	volatile uint32_t irq_cursta;
	volatile uint32_t irq_flag;
	//control and status register config
	uint32_t csr_config;
	//interrupt enable bit map
	uint32_t inten_bm;
	//txfifo threshold
	uint32_t txfifo_thh;
	//rxfifo threahold
	uint32_t rxfifo_thh;
	//tx repeat
	uint32_t tx_repeat;
	//rx repeat
	uint32_t rx_repeat;
	//scclk divisor
	uint32_t scclk_div;
	//baud divisor
	uint32_t baud_div;
	//activation/deactivation time, in scclk cycles
	uint32_t act_time;
	//reset time, in scclk cycles
	uint32_t rst_time;
	//ATR limit time, in scclk cycles
	uint32_t atr_time;
	//gaurd time, in ETUs
	uint32_t guard_time;
	//character limit time, in ETUs
	uint32_t chlimit_time;

	uint32_t debounce_time;

	scr_buffer rxbuf;
	scr_buffer txbuf;

	volatile uint32_t detected;
	volatile uint32_t activated;
	#define SCR_ATR_RESP_INVALID  	        0
	#define SCR_ATR_RESP_FAIL				1
	#define SCR_ATR_RESP_OK					2
	volatile uint32_t atr_resp;

	uint32_t chto_flag;

}scr_struct, *pscr_struct;


#define SCR_FSM_MAX_RECORD		1024
typedef struct {
	uint32_t count;
	uint32_t old;
	uint32_t record[SCR_FSM_MAX_RECORD];
}scr_fsm_record, *pscr_fsm_record;


typedef struct{
	uint8_t TS;

	uint8_t TK[15];
	uint8_t TK_NUM;

	uint32_t T;		//Protocol
	uint32_t FMAX; //in MHz
	uint32_t F;
	uint32_t D;
	uint32_t I;   //Max Cunrrent for Program, in mA
	uint32_t P;   //Program Voltage
	uint32_t N;   //Extra Guard Time, in ETUs
}scatr_struct, *pscatr_struct;


typedef struct {
	uint8_t ppss;
	uint8_t pps0;
	uint8_t pps1;
	uint8_t pps2;
	uint8_t pps3;
	uint8_t pck;
} upps_struct, *ppps_struct;


//SCR Test Stage (State Machine)
typedef enum {sts_wait_connect=0,
	          sts_wait_act,
	          sts_wait_atr,
			  sts_warm_reset,
			  sts_wait_atr_again,
			  sts_start_pps,
			  sts_wait_pps_resp,
			  sts_send_cmd,
			  sts_start_deact,
			  sts_wait_deact,
			  sts_wait_disconnect,
			  sts_idle,
			} scr_test_stage;


#endif //_SCR_HAL_H_

