/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY'S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS'SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY'S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __DMA_SUN20IW2_H__
#define __DMA_SUN20IW2_H__


#if defined(CONFIG_ARCH_ARM_CORTEX_M33)
#define DMA0
#define DMA_IRQ_NUM		(13)	/* DMA0 0~7 channel irq non-secure */
#define DMA_SECURE_IRQ_NUM	(12)	/* DMA0 0~7 channel irq secure */
#define NR_MAX_CHAN		 8	/* total of channels */
#define START_CHAN_OFFSET	 0
#define SUNXI_CLK_DMA		 CLK_BUS_DMA0
#define SUNXI_CLK_MBUS_DMA	 CLK_MBUS_DMA0
#define SUNXI_RST_DMA		 RST_DMA0
//#define ENABLE_SECURE_DMA
#elif defined(CONFIG_ARCH_RISCV_C906)
#define DMA0
#define DMA_IRQ_NUM		(31)	/* DMA0 8-15 channel irq non-secure */
#define DMA_SECURE_IRQ_NUM	(30)	/* DMA0 8-15 channel irq secure */
#define NR_MAX_CHAN		 8	/* total of channels */
#define START_CHAN_OFFSET        8
#define SUNXI_CLK_DMA            CLK_BUS_DMA0
#define SUNXI_CLK_MBUS_DMA       CLK_MBUS_DMA0
#define SUNXI_RST_DMA            RST_DMA0
#else /* DSP */
#define DMA1
#define DMA_IRQ_NUM		(14)	/* DMA1 0~15 channel irq non-secure */
#define DMA_SECURE_IRQ_NUM	(13)	/* DMA1 0~15 channel irq secure */
#define NR_MAX_CHAN		 16	/* total of channels */
#define START_CHAN_OFFSET        0
#define SUNXI_CLK_DMA            CLK_BUS_DMA1
#define SUNXI_CLK_MBUS_DMA       CLK_MBUS_DMA1
#define SUNXI_RST_DMA            RST_DMA1
#endif

#if defined(DMA0)
#define SUNXI_DMAC_PBASE	(0x40000000ul)
#elif defined(DMA1)
#define SUNXI_DMAC_PBASE	(0x40001000ul)
#endif
/*
 * The source DRQ type and port corresponding relation
 */
#define DRQSRC_SDRAM		0
#define DRQSRC_LSPSRAM		1
/* #define DRQSRC_SPDIF		2 */
#define DRQSRC_DAUDIO_0_RX	3
/* #define DRQSRC_DAUDIO_1_RX	4 */
/* #define DRQSRC_DAUDIO_2_RX	5 */
#define DRQSRC_MAD_RX		6
#define DRQSRC_AUDIO_CODEC	7
#define DRQSRC_DMIC		8
#define DRQSRC_SPDIF		9
#define DRQSRC_LSPSRAM_CTRL	10
#define DRQSRC_FLASH_CTRL       11
#define DRQSRC_GPADC		12
#define DRQSRC_CODEC_DAC_RX	13
#define DRQSRC_UART0_RX		14
#define DRQSRC_UART1_RX		15
#define DRQSRC_UART2_RX         16
/* #define DRQSRC_UART3_RX	17 */
/* #define DRQSRC_UART4_RX      18 */
/* #define DRQSRC_UART5_RX      19 */
/* #define DRQSRC_RESEVER       20 */
/* #define DRQSRC_RESEVER       21 */
#define DRQSRC_SPI0_RX		22
#define DRQSRC_SPI1_RX		23
/* #define DRQSRC_SPI2_RX	24 */
/* #define DRQSRC_SPI3_RX	25 */
/* #define DRQSRC_RESEVER       26 */
/* #define DRQSRC_RESEVER       27 */
/* #define DRQSRC_SS_MBUS       28 */
/* #define DRQSRC_RESEVER       29 */
#define DRQSRC_OTG_EP1		30
#define DRQSRC_OTG_EP2		31
#define DRQSRC_OTG_EP3          32
#define DRQSRC_OTG_EP4          33
#define DRQSRC_OTG_EP5          34
#define DRQSRC_TWI0_RX		43
#define DRQSRC_TWI1_RX		44

/*
 * The destination DRQ type and port corresponding relation
 */
#define DRQDST_SDRAM		0
#define DRQDST_LSPSRAM		1
/* #define DRQDST_SPDIF		2 */
#define DRQDST_DAUDIO_0_TX	3
/* #define DRQDST_DAUDIO_1_TX	4 */
/* #define DRQDST_DAUDIO_2_TX	5 */
#define DRQDST_MAD_TX		6
#define DRQDST_AUDIO_CODEC	13
#define DRQSRC_DMIC	        8
#define DRQDST_SPDIF	        9
#define DRQDST_LSPSRAM_CTRL	10
#define DRQDST_FLASH_CTRL       11
/* #define DRQSRC_RESEVER       12 */
#define DRQDST_IR_TX		41
#define DRQDST_UART0_TX		14
#define DRQDST_UART1_TX		15
#define DRQDST_UART2_TX         16
/* #define DRQDST_UART3_TX      17 */
/* #define DRQDST_UART4_TX	18 */
/* #define DRQDST_UART5_TX	19 */
/* #define DRQSRC_RESEVER       20 */
/* #define DRQSRC_RESEVER       21 */
#define DRQDST_SPI0_TX		22
#define DRQDST_SPI1_TX          23
/* #define DRQDST_SPI2_TX       24 */
/*#define DRQDST_SPI3_TX        25 */
/* #define DRQSRC_RESEVER       26 */
/* #define DRQSRC_RESEVER       27 */
/* #define DRQSRC_SS_MBUS	28 */
/* #define DRQSRC_RESEVER       29 */
#define DRQDST_OTG_EP1		30
#define DRQDST_OTG_EP2		31
#define DRQDST_OTG_EP3          32
#define DRQDST_OTG_EP4          33
#define DRQDST_OTG_EP5          34
/* #define DRQDST_LEDC          42 */
#define DRQDST_TWI0_TX		43
#define DRQDST_TWI1_TX          44
#endif /*__DMA_SUN20IW2_H__  */
