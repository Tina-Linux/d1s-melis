
#ifndef _SCR_CONFIG_H_
#define _SCR_CONFIG_H_

#include <sunxi_hal_common.h>
//#ifdef CONFIG_SCR_TEST

#define SCR0_TEST
//#define SCR1_TEST
//#define SCR2_TEST
//#define SCR3_TEST

#define SMART_CARD_NUM        9

#define APB2CLK               APB2_BUS_CLK
#define SCR_CLK_FRQ           4000000         //Hz

#define CSR_CONFIG_DETPOL					(0x0<<24)
#define CSR_CONFIG_T					    (0x0<<22)
#define CSR_CONFIG_ATRSTFLUSH				(0x1<<21)
#define CSR_CONFIG_TSRXEN					(0x1<<20)
#define CSR_CONFIG_CLKSTPPOL				(0x0<<19)
#define CSR_CONFIG_PECRXE					(0x0<<18)
#define CSR_CONFIG_MSBF						(0x0<<17)
#define CSR_CONFIG_DPOL						(0x0<<16)

#ifdef SCR0_TEST
	#define SCR_BASE              SCR0_BASE
	#define GIC_SRC_SCR           GIC_SRC_SCR0
	#define SCR_NO                0

	//IO define
	#define DET_PIN               GPIO_B(7)
	#define DET_PIN_FUNC_NO       5
	#define CLK_PIN               GPIO_B(4)
	#define CLK_PIN_FUNC_NO       5
	#define RST_PIN               GPIO_B(6)
	#define RST_PIN_FUNC_NO       5
	#define DATA_PIN              GPIO_B(5)
	#define DATA_PIN_FUNC_NO      5

	#define VCCEN_PIN             GPIO_D(18)
	#define VCCEN_PIN_FUNC_NO     1
	#define VPPEN_PIN             GPIO_D(19)
	#define VPPEN_PIN_FUNC_NO     1

#endif

#ifdef SCR1_TEST
	#define SCR_BASE              SCR1_BASE
	#define GIC_SRC_SCR           GIC_SRC_SCR1
	#define SCR_NO                1

	//IO define
	#define DET_PIN               GPIO_D(10)
	#define DET_PIN_FUNC_NO       4
	#define CLK_PIN               GPIO_D(11)
	#define CLK_PIN_FUNC_NO       4
	#define RST_PIN               GPIO_D(12)
	#define RST_PIN_FUNC_NO       4
	#define DATA_PIN              GPIO_D(13)
	#define DATA_PIN_FUNC_NO      4

	#define VCCEN_PIN             GPIO_D(18)
	#define VCCEN_PIN_FUNC_NO     1
	#define VPPEN_PIN             GPIO_D(19)
	#define VPPEN_PIN_FUNC_NO     1
#endif

#ifdef SCR2_TEST
	#define SCR_BASE              SCR2_BASE
	#define GIC_SRC_SCR           GIC_SRC_SCR2
	#define SCR_NO                2

	//IO define
	#define DET_PIN               GPIO_D(10)
	#define DET_PIN_FUNC_NO       4
	#define CLK_PIN               GPIO_D(11)
	#define CLK_PIN_FUNC_NO       4
	#define RST_PIN               GPIO_D(12)
	#define RST_PIN_FUNC_NO       4
	#define DATA_PIN              GPIO_D(13)
	#define DATA_PIN_FUNC_NO      4

	#define VCCEN_PIN             GPIO_D(18)
	#define VCCEN_PIN_FUNC_NO     1
	#define VPPEN_PIN             GPIO_D(19)
	#define VPPEN_PIN_FUNC_NO     1
#endif

#ifdef SCR3_TEST
	#define SCR_BASE              SCR3_BASE
	#define GIC_SRC_SCR           GIC_SRC_SCR3
	#define SCR_NO                3

	//IO define
	#define DET_PIN               GPIO_D(10)
	#define DET_PIN_FUNC_NO       4
	#define CLK_PIN               GPIO_D(11)
	#define CLK_PIN_FUNC_NO       4
	#define RST_PIN               GPIO_D(12)
	#define RST_PIN_FUNC_NO       4
	#define DATA_PIN              GPIO_D(13)
	#define DATA_PIN_FUNC_NO      4

	#define VCCEN_PIN             GPIO_D(18)
	#define VCCEN_PIN_FUNC_NO     1
	#define VPPEN_PIN             GPIO_D(19)
	#define VPPEN_PIN_FUNC_NO     1
#endif


#define SCR_INTSTA_DEACT					(0x1<<23)
#define SCR_INTSTA_ACT						(0x1<<22)
#define SCR_INTSTA_INS						(0x1<<21)
#define SCR_INTSTA_REM						(0x1<<20)
#define SCR_INTSTA_ATRDONE					(0x1<<19)
#define SCR_INTSTA_ATRFAIL					(0x1<<18)
#define SCR_INTSTA_CHTO						(0x1<<17)  //Character Timout
#define SCR_INTSTA_CLOCK					(0x1<<16)
#define SCR_INTSTA_RXPERR					(0x1<<12)
#define SCR_INTSTA_RXDONE					(0x1<<11)
#define SCR_INTSTA_RXFTH					(0x1<<10)
#define SCR_INTSTA_RXFFULL					(0x1<<9)
#define SCR_INTSTA_TXPERR					(0x1<<4)
#define SCR_INTSTA_TXDONE					(0x1<<3)
#define SCR_INTSTA_TXFTH					(0x1<<2)
#define SCR_INTSTA_TXFEMPTY					(0x1<<1)
#define SCR_INTSTA_TXFDONE					(0x1<<0)



#define SCR_FIFO_DEPTH			    16

#define SCR_CSR_OFF					(0x000)
#define SCR_INTEN_OFF				(0x004)
#define SCR_INTST_OFF				(0x008)
#define SCR_FCSR_OFF				(0x00c)
#define SCR_FCNT_OFF				(0x010)
#define SCR_RPT_OFF					(0x014)
#define SCR_DIV_OFF					(0x018)
#define SCR_LTIM_OFF				(0x01c)
#define SCR_CTIM_OFF				(0x020)
#define SCR_LCTL_OFF				(0x030)
#define SCR_DBE_TIME                (0x040)
#define SCR_FSM_OFF					(0x03c)
#define SCR_FIFO_OFF				(0x100)



#if 0
#define LAI_XI           0
#define SHEN_ZHOU_1      1
#define SHEN_ZHOU_2      2
#define SHU_ZI_1         3
#define SHU_ZI_2         4
#define DIAN_XI_4G       5
#define LIAN_TONG        6
#define YI_DONG          7
#define DIGITAL_VIDEO    8
#endif

#if 0
//SCR Card State FSM
#define SCR_CARD_IDLE							0x0
#define SCR_CARD_ACT							0x1
#define SCR_CARD_ATR							0x2
#define SCR_CARD_DATA							0x3
#define SCR_CARD_DEACT						    0x4
//SCR Active/Deactive State FSM
#define SCR_ACT_INACTIVE					    0x0
#define SCR_ACT_A1								0x1  // Vcc shall be powered
#define SCR_ACT_A2								0x2  // I/O shall be put in reception mode
#define SCR_ACT_A3								0x3  // Vpp shall be raised to idle state (if AutoVPP)
#define SCR_ACT_ACTIVE						    0x4  // CLK shall be provided with a suitable and stable clock
#define SCR_ACT_D1								0x5  // State L on RST
#define SCR_ACT_D2								0x6  // State L on CLK
#define SCR_ACT_D3								0x7  // Vpp inactive (if AutoVPP)
#define SCR_ACT_D4								0x8  // State A on I/O
#define SCR_ACT_D5								0x9  // Vcc inactive
//SCR ATR State FSM
#define SCR_ATR_IDLE							0x0
#define SCR_ATR_RST								0x1
#define SCR_ATR_WAIT							0x2
#define SCR_ATR_START							0x3
#define SCR_ATR_DONEx							0x4
#define SCR_ATR_FAILx							0x5
//SCR ATR Structure State FSM
#define SCR_STR_IDLE							0x0
#define SCR_STR_TS								0x1
#define SCR_STR_T0								0x2
#define SCR_STR_TX								0x3
#define SCR_STR_TK								0x4
#define SCR_STR_TCK								0x5
#define SCR_STR_DONE							0x6
#define SCR_STR_FAIL							0x7
#endif


//#endif

#endif//_SCR_CONFIG_H_
