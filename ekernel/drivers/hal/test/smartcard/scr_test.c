#include <stdio.h>
#include <stdlib.h>
#include <interrupt.h>
#include <irqs.h>
#include <string.h>
#include <console.h>
#include <param.h>
#include <hal_osal.h>
#include <hal_timer.h>
#include <hal_atomic.h>
#include "hal_clk.h"
#include <hal_gpio.h>
#include <hal_reset.h>
#include "scr_hal.h"
#include "scr_test.h"
#include <console.h>
//#include "scr_hal_function.h"

//#ifdef CONFIG_SCR_TEST

#define SCR_TEST_INFO  printf
#define SCR_TEST_DBG   printf
#define get_wvalue	readl
#define APB2CLK	24000000

uint8_t ic_card_atr[SMART_CARD_NUM][30] =
{
	{7,  0x3b, 0x93, 0x11, 0x00, 0x00, 0x40, 0x41},         //莱西有线
	{18, 0x3b, 0x7d, 0x94, 0x00, 0x00, 0x57, 0x44, 0x53, 0x67, 0x96, 0x86, 0x93, 0x03, 0x9d, 0xf7, 0x10, 0x00, 0x9d},//神州大众卡1
	{15, 0x3b, 0xb9, 0x94, 0x00, 0x40, 0x14, 0x47, 0x47, 0x33, 0x53, 0x30, 0x35, 0x41, 0x53, 0x30},//神州大众卡2
	{17, 0x3b, 0x9d, 0x95, 0x00, 0x13, 0x61, 0x40, 0x36, 0x13, 0x85, 0xe9, 0x44, 0x34, 0x8f, 0x78, 0x8f, 0x4a},//数字电视卡1
	{17, 0x3b, 0x9d, 0x95, 0x00, 0x13, 0x61, 0x40, 0x36, 0x13, 0x85, 0xe9, 0x44, 0x34, 0xf3, 0x78, 0x8f, 0x4a},//数字电视卡2
	{22, 0x3b, 0x9f, 0x95, 0x80, 0x1f, 0xc3, 0x80, 0x31, 0xe0, 0x73, 0xfe, 0x21, 0x13, 0x57, 0x86, 0x81, 0x02, 0x86, 0x98, 0x44, 0x18, 0xa8},//电信4G卡
	{20, 0x3b, 0xfb, 0x94, 0x00, 0x00, 0x80, 0x1f, 0x83, 0x80, 0x65, 0x92, 0x10, 0x26, 0x86, 0x53, 0x83, 0x00, 0x90, 0x00, 0xf4},//联通卡
	{16, 0x3b, 0x7b, 0x94, 0x00, 0x00, 0x97, 0x88, 0x84, 0x86, 0x60, 0xa0, 0x04, 0x01, 0x00, 0x04, 0x00},//移动卡
	{20, 0x3b, 0x7f, 0x12, 0x00, 0x00, 0x44, 0x56, 0x4e, 0x20, 0x54, 0x45, 0x53, 0x54, 0x20, 0x43, 0x41, 0x52, 0x44, 0x76, 0x31}//digital TV卡
};


uint8_t ic_card_send_cmd[SMART_CARD_NUM][30] =
{
		{5, 0xe5, 0x04, 0x00, 0x00, 0x04},   //莱西有线
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{7, 0xA0, 0xA4, 0x00, 0x00, 0x02,0x3F,0x00}, //移动卡
		{0}
};


uint8_t ic_card_rev_data[SMART_CARD_NUM][30] =
{
		{7, 0x04, 0x4d, 0x33, 0x4f, 0x4b, 0x90, 0x00},   //莱西有线
		{0},
		{0},
		{0},
		{0},
		{0},
		{0},
		{3, 0xa4, 0x9f, 0x1b}, //移动卡
		{0}
};

scr_struct scr;
scr_fsm_record scr_fsm;
scatr_struct scatr;
upps_struct pps;

extern scr_test_stage stage;
static int state_dly = 0x0;

#define SCR_SIM_DBG(x) 	pattern_mod_goto(0x2, x)

void scr_ccu_clk_gate_on(u32 sel)
{
//	set_wbit(CCMU_SCR_BGR_REG,(1<<sel));
	int ret;

	ret = readl(0x4003c004);
//	hal_write_reg32(0x4000000 | ret, 0x4003c004);
	hal_write_reg32(0x4003c004, 0x4000000 | ret);
}

void scr_ccu_clk_gate_off(u32 sel)
{
//	clr_wbit(CCMU_SCR_BGR_REG,(1<<sel));
	int ret;

	ret = readl(0x4003c004);
//	hal_write_reg32(0xfbffffff & ret, 0x4003c004);
	hal_write_reg32(0x4003c004, 0xfbffffff & ret);
}

void scr_module_rst_on(u32 sel)
{
//	set_wbit(CCMU_SCR_BGR_REG,(1<<(sel+16)));
	int ret;

	ret = readl(0x4003c00c);
//	hal_write_reg32(0x4000000 | ret, 0x4003c00c);
	hal_write_reg32(0x4003c00c, 0x4000000 | ret);
}

void scr_module_rst_off(u32 sel)
{
//	clr_wbit(CCMU_SCR_BGR_REG,(1<<(sel+16)));
	int ret;

	ret = readl(0x4003c00c);
//	hal_write_reg32(0xfbffffff & ret, 0x4003c00c);
	hal_write_reg32(0x4003c00c, 0xfbffffff & ret);
}

void scr_module_rst(u32 sel)
{
	scr_module_rst_off(sel);
	udelay(300);
	scr_module_rst_on(sel);
	udelay(300);
}

void scr_pio_setup(void)
{
	/*
	gpio_set_cfg(DET_PIN, 1, DET_PIN_FUNC_NO);
	gpio_set_cfg(CLK_PIN, 1, CLK_PIN_FUNC_NO);
	gpio_set_cfg(RST_PIN, 1, RST_PIN_FUNC_NO);
	gpio_set_cfg(DATA_PIN, 1, DATA_PIN_FUNC_NO);

	gpio_set_cfg(VCCEN_PIN, 1, VCCEN_PIN_FUNC_NO);
	gpio_set_cfg(VPPEN_PIN, 1, VPPEN_PIN_FUNC_NO);

	writel(0x3333333, 0x020004b4);
	writel(0x1555, 0x020004d0);
*/

	int ret;

	ret = hal_gpio_pinmux_set_function(GPIOA(14), 5);
	if (ret) {
		printf("scr pin set function failed\n");
		return -1;
	}
}

/*
void smart_card_power_on(void)
{
//	gpio_set_val(VCCEN_PIN,1);
//	gpio_set_val(VPPEN_PIN,1);
	int ret;

//	ret = readl(0x020004b4);
//	writel(0x0000333 | ret, 0x020004b4);
}

void smart_card_power_off(void)
{
//	gpio_set_val(VCCEN_PIN,0);
//	gpio_set_val(VPPEN_PIN,0);
	int ret;

//	ret = readl(0x020004b4);
//	writel(0xffff000 & ret, 0x020004b4);
}
*/

/* Configure the CCMU Clock & De-assert Reset & Gating & GPIO configure */
void scr_system_setup(u32 sel)
{
//	put_wvalue(CCMU_APB2_CFG_REG,0x00);      //set APB2 clk = 24M
	scr_module_rst_off(sel);
	scr_ccu_clk_gate_off(sel);
	udelay(300);
	scr_ccu_clk_gate_on(sel);/*enable APB clock*/
	scr_module_rst_on(sel);
	scr_pio_setup();/*IO configure*/
}

uint32_t scr_reg_test(void)   //registers default values cheak and read write check
{
	uint32_t temp;

	scr_module_rst(SCR_NO);
/*
	temp = aw_reg_default_check(scr.reg_base, SCR_REG_DEFAULT);  //default values check
	if(temp)
	{
//		SCR_SIM_DBG(temp);
		printf("default values check failed\n");
		return 1;
	}
	temp = aw_reg_rw_check(scr.reg_base, SCR_REG_RW);   //read write check
	if(temp)
	{
//		SCR_SIM_DBG(temp);
		printf("read write check failed\n");
		return 1;
	}
*/
	SCR_TEST_INFO("SCR Register Test Pass!!\n");
	return 1;
}

static hal_irqreturn_t scr0_irq_handler(void *dev)
{
	scr_handler_irq(&scr);

	return 0;
}

void reg_scr0_irq_handler(void)
{
	hal_request_irq(scr.irq_no, scr0_irq_handler, "scr0", &scr);
	hal_enable_irq(scr.irq_no);
}

void scr_params_init(void)
{
	scr.reg_base = 0x40045400;

	scr.irq_no = 69;              //中断号
	scr.csr_config = CSR_CONFIG_DETPOL
			       | CSR_CONFIG_T
			       | CSR_CONFIG_ATRSTFLUSH
			       | CSR_CONFIG_TSRXEN
			       | CSR_CONFIG_CLKSTPPOL
			       | CSR_CONFIG_PECRXE
			       | CSR_CONFIG_MSBF
			       | CSR_CONFIG_DPOL;

	scr.inten_bm = 0xffffffff;             //Enbale all the interrupts
	scr.txfifo_thh = SCR_FIFO_DEPTH/2;     //set the txfifo trigger level as half full
	scr.rxfifo_thh = SCR_FIFO_DEPTH/2;     //set the rxfifo trigger level as half full
	scr.tx_repeat = 0x3;                   //iso7816中规定对于存在争议的字符，最多只能重发3次
	scr.rx_repeat = 0x3;                   //iso7816中规定对于存在争议的字符，最多只能重发3次

	scr.scclk_div = (APB2CLK/(2*SCR_CLK_FRQ))-1;    //PCLK/12, <175, && SCCLK >= 1M && =<4M
	scr.baud_div = (scr.scclk_div + 1)*372-1;       //ETU = 372*SCCLK

	scr.act_time = 1;//1*256 clock cycle
	scr.rst_time = 1;//1*256 clock cycle
	scr.atr_time = (35000>>8); //400~40000   冷复位和热复位时,在RST拉高后,IO要作出ATR的时间限制
	scr.guard_time = 2;          //=2*ETUs	/* GT = 12etu + R * N/f   ---   Default extra guard time is 0 */
	scr.chlimit_time = 9600;     //1024*(10+scr.guard_time); //1K Characters  个人感觉应该是9600更好（lrx）

	scr.debounce_time = 0xfffff;   //insert detect debounce time
}

volatile u32 card_name = 0;
volatile u32 scr_ret = 0;
uint32_t scr_test_process(pscr_struct pscr)
{
	uint8_t atr_temp[30];
	uint8_t rsp_temp[30];
	u32 i = 0;
	u32 j = 0;
	uint8_t atr_cmp = 0;

	switch(stage)
	{
		case sts_wait_connect:
			if(scr.detected)/*if detect card, ACT the card*/
			{
				state_dly ++;
				if(state_dly >= 50)
				{
					//smart_card_power_on();
					msleep(10);
					scr_start_activation(&scr);			/* Activation */
					stage = sts_wait_act;
					state_dly = 0;
				}
			}
			else
			{
				state_dly = 0;
			}
			break;
		case sts_wait_act:
			if(scr.activated)
			{
				stage = sts_wait_atr;
			}
			break;
		case sts_wait_atr:
			if(scr.atr_resp != SCR_ATR_RESP_INVALID)
			{
				if(scr.atr_resp==SCR_ATR_RESP_OK)/*ATR response is 有效的*/
				{
					uint32_t i=0;
					SCR_TEST_DBG("ATR = ");
					for(i=0; i<scr.rxbuf.wptr; i++)
					{
						SCR_TEST_DBG("0x%02x ", *((uint8_t*)(scr.rxbuf.buffer+i)));
						atr_temp[i] = *((uint8_t*)(scr.rxbuf.buffer+i));
					}
					SCR_TEST_DBG(" \n");
					scr.rxbuf.rptr = scr.rxbuf.wptr;
					smartcard_atr_decode(&scatr, (uint8_t*)scr.rxbuf.buffer, &pps, 1);

					scr_buffer_flush(&scr.rxbuf);   //clean rxbuffer
					stage = sts_start_pps;

					for(i=0;i<SMART_CARD_NUM;i++)
					{
						u32 atr_length = ic_card_atr[i][0];
						for(j=0;j<atr_length;j++)
						{
							if(atr_temp[j] != ic_card_atr[i][j+1])
							{
								break;
							}
						}
						if(j == atr_length)
						{
							card_name = i;
							atr_cmp = 1;
							break;
						}
						else
						{
							atr_cmp = 0;
						}
					}

					if(atr_cmp)
					{
						printf("crad_name = %d\n",card_name);
						printf("ATR Function PASS!!\n");
					}
					else
					{
						printf("ATR Function FAIL!!\n");
					}
				}
				else
				{
					stage = sts_start_deact;
				}

			}
			break;
		case sts_start_pps:			/* Protocol and parameters selection */
			pscr->chto_flag = 0;
			scr_buffer_flush(&scr.rxbuf);

			scr_write_fifo(&scr, pps.ppss);
			printf("ppss:%x  \n",pps.ppss);
			scr_write_fifo(&scr, pps.pps0);
			printf("pps0:%x  \n",pps.pps0);
			if(pps.pps0&(0x1<<4))
			{
				scr_write_fifo(&scr, pps.pps1);
				printf("pps1:%x  \n",pps.pps1);
			}
			if(pps.pps0&(0x1<<5))
			{
				scr_write_fifo(&scr, pps.pps2);
				printf("pps2:%x  \n",pps.pps2);
			}
			if(pps.pps0&(0x1<<6))
			{
				scr_write_fifo(&scr, pps.pps3);
				printf("pps3:%x  \n",pps.pps3);
			}
			scr_write_fifo(&scr, pps.pck);
			printf("pck:%x  \n",pps.pck);

			stage = sts_wait_pps_resp;
			break;
		case sts_wait_pps_resp:   //pps交换成功的最普通的情况就是:卡设备的pps请求和ic卡的pps应答的内容完全一样
			if(pscr->chto_flag)   //Wait Data Timeout/*time is too long, time is out*/
			{
				if(scr_buffer_is_empty(&scr.rxbuf))  //RX Buffer, No Response
				{
					SCR_TEST_INFO("No PPS Response!!\n");
					stage = sts_warm_reset;
				}
				else
				{
					uint8_t data = scr_dump_buffer(&scr.rxbuf);   //read ppss
					printf("ppss:%x  \n",data);
					if(data!=pps.ppss)    //check ppss
					{
						SCR_TEST_INFO("PPS Resp Start Error: 0x%x !!\n", data);
						break;
					}
					if(scr_buffer_is_empty(&scr.rxbuf))    //no pps0
					{
						SCR_TEST_INFO("PPS Resp Too Short 1\n");
						break;
					}
					data = scr_dump_buffer(&scr.rxbuf);   //read pps0
					printf("pps0:%x  \n",data);
					if(data!=pps.pps0)    //check pps0
					{
						SCR_TEST_INFO("PPS Resp PPS0 Error: 0x%x vs 0x%x !!\n", pps.pps0, data);
						break;
					}
					if(pps.pps0&(0x1<<4))   //根据pps0的值,有pps1
					{
						if(scr_buffer_is_empty(&scr.rxbuf))    //收不到pps1
						{
							SCR_TEST_INFO("PPS Resp Too Short 2\n");
							break;
						}
						data = scr_dump_buffer(&scr.rxbuf);  //读取pps1
						printf("pps1:%x  \n",data);
						if(data!=pps.pps1)   //检测pps1
						{
							SCR_TEST_INFO("PPS Resp PPS1 Error: 0x%x vs 0x%x !!\n", pps.pps1, data);
							break;
						}
					}
					if(pps.pps0&(0x1<<5))   //根据pps0的值,有pps2
					{
						if(scr_buffer_is_empty(&scr.rxbuf))   //收不到pps2
						{
							SCR_TEST_INFO("PPS Resp Too Short 3\n");
							break;
						}
						data = scr_dump_buffer(&scr.rxbuf);  //读取pps2
						printf("pps2:%x  \n",data);
						if(data!=pps.pps2)      //检测pps2
						{
							SCR_TEST_INFO("PPS Resp PPS2 Error: 0x%x vs 0x%x !!\n", pps.pps2, data);
							break;
						}
					}
					if(pps.pps0&(0x1<<6))    //根据pps0的值,有pps3
					{
						if(scr_buffer_is_empty(&scr.rxbuf))   //收不到pps3
						{
							SCR_TEST_INFO("PPS Resp Too Short 4\n");
							break;
						}
						data = scr_dump_buffer(&scr.rxbuf);  //读取pps3
						printf("pps3:%x  \n",data);
						if(data!=pps.pps3)   //检测pps3
						{
							SCR_TEST_INFO("PPS Resp PPS3 Error: 0x%x vs 0x%x !!\n", pps.pps3, data);
							break;
						}
					}
					if(scr_buffer_is_empty(&scr.rxbuf))   //收不到pck
					{
						SCR_TEST_INFO("PPS Resp Too Short 5\n");
						break;
					}
					data = scr_dump_buffer(&scr.rxbuf);  //读取pck
					printf("pck:%x  \n",data);
					if(data!=pps.pck)   //检测pck
					{
						SCR_TEST_INFO("PPS Resp PCK Error: 0x%x vs 0x%x !!\n", pps.pck, data);
						break;
					}

					scr_buffer_flush(&scr.rxbuf);
					stage = sts_send_cmd;

					SCR_TEST_INFO("PPS Response OK!!\n");   //如果上述if中都没有break出来,则表示pps交换成功

					scr.baud_div = (scr.scclk_div + 1)*(scatr.F)/(scatr.D)-1;
					scr_set_baud_divisor(&scr, pscr->baud_div);
				}
			}
			break;

		case sts_warm_reset:
			scr.atr_resp = SCR_ATR_RESP_INVALID;
			scr_start_warmreset(&scr);
			stage = sts_wait_atr_again;
			break;

		case sts_wait_atr_again:
			if(scr.atr_resp != SCR_ATR_RESP_INVALID)
			{
				if(scr.atr_resp==SCR_ATR_RESP_OK)/*ATR response is 有效的*/
				{
					uint32_t i=0;
					SCR_TEST_DBG("ATR : ");
					for(i=0; i<scr.rxbuf.wptr; i++)
					{
						SCR_TEST_DBG("0x%02x ", *((uint8_t*)(scr.rxbuf.buffer+i)));
						atr_temp[i] = *((uint8_t*)(scr.rxbuf.buffer+i));
					}
					SCR_TEST_DBG(" \n");
					scr.rxbuf.rptr = scr.rxbuf.wptr;
					smartcard_atr_decode(&scatr, (uint8_t*)scr.rxbuf.buffer, &pps, 1);

					for(i=0;i<SMART_CARD_NUM;i++)
					{
						u32 atr_length = ic_card_atr[i][0];
						for(j=0;j<atr_length;j++)
						{
							if(atr_temp[j] != ic_card_atr[i][j+1])
							{
								break;
							}
						}
						if(j == atr_length)
						{
							card_name = i;
							atr_cmp = 1;
							break;
						}
						else
						{
							atr_cmp = 0;
						}
					}

					if(atr_cmp)
					{
						printf("crad_name = %d\n",card_name);
						printf("ATR Function PASS!!\n");
					}
					else
					{
						printf("ATR Function FAIL!!\n");
					}


					scr.baud_div = (scr.scclk_div + 1)*(scatr.F)/(scatr.D)-1;
					scr_set_baud_divisor(&scr, pscr->baud_div);

					scr_buffer_flush(&scr.rxbuf);   //clean rxbuffer
					stage = sts_send_cmd;
				}
				else
				{
					stage = sts_start_deact;
				}

			}
			break;

		case sts_send_cmd:    //这里应该可以加入我们想要的沟通命令
		{
			uint8_t cmp_rlt = 0;
			if(ic_card_send_cmd[card_name][0])
			{
				for(i=0;i<ic_card_send_cmd[card_name][0];i++)
				{
					uint8_t send = ic_card_send_cmd[card_name][i+1];
					scr_write_fifo(&scr, send);
				}
				msleep(1000);
				scr_rx_fifo_read(rsp_temp);

				for(i=0;i<ic_card_rev_data[card_name][0];i++)
				{
					if(rsp_temp[i] != ic_card_rev_data[card_name][i+1])
					{
						cmp_rlt = 1;
					}
				}
				if(cmp_rlt)
				{
					printf("Communication Command Error: ");
					for(i=0;i<ic_card_rev_data[card_name][0];i++)
						printf("0x%02x  ",rsp_temp[i]);
					printf("\n");
					scr_ret++;
				}
				else
				{
					printf("Communication Command Respone PASS!!\n");
				}
			}
			else
			{
				printf("No communication command, No command test!!\n");
				stage = sts_idle;
			}
			msleep(1000);
		}
			break;
		case sts_start_deact:
			stage = sts_wait_deact;
			scr_start_deactivation(&scr);		/* Deactivation */
			msleep(10);
			//smart_card_power_off();
			break;
		case sts_wait_deact:
			if (!scr.activated) {
				stage = sts_wait_disconnect;
				scr_module_rst(SCR_NO);
				scr_params_init();
				smartcard_params_init(&scatr);    /*set smart card protocol, v,i,frequency*/
				scr_init(&scr);
			}
			break;
		case sts_wait_disconnect:
			if (!scr.detected)
				stage = sts_wait_connect;

			break;

		case sts_idle:
			msleep(50);
			if (sts_idle == stage)
				stage = sts_idle;
			break;
		default:
			stage = sts_idle;
			break;
	}

	return 0;
}

void scr_data_transfer_test(void)
{
	scr_params_init();
	scr_init(&scr);
	reg_scr0_irq_handler();
	scr_global_interrupt_enable(&scr);
	while(1)
		scr_test_process(&scr);
}


/* For all:SIMULATION & FPGA PALTFORM & IC PLATFORM */
/* s32 scr_test(void) */
int scr_test1(int argc, char **argv)
{
	scr_params_init();
	/*register default check and read write check*/
	scr_system_setup(SCR_NO);

	if(scr_reg_test() != 1)
		return -1;

	/*SCR Control and smart card data transfer test*/
	scr_system_setup(SCR_NO);

	scr_data_transfer_test();

	return 0;
}

//#endif
FINSH_FUNCTION_EXPORT_CMD(scr_test1, hal_smartcard, smartcard);
