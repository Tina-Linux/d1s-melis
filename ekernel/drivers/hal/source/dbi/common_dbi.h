/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
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

#ifndef __COMMON_DBI_I_H__
#define __COMMON_DBI_I_H__

#ifdef __cplusplus
extern "C" {
#endif

/* debug */
#define DP printf("[DEBUG] %s, %s, %d \n", __FILE__, __func__, __LINE__);

void dbi_print_regs(unsigned long from, unsigned long to);
void check_reg(void);

#define SPI_FIFO_DEPTH (128)
#define MAX_FIFU 64
#define BULK_DATA_BOUNDARY 64       /* can modify to adapt the application */
#define SPI_MAX_FREQUENCY 100000000 /* spi controller just support 100Mhz */
#define SPI_HIGH_FREQUENCY 60000000 /* sample mode threshold frequency  */
#define SPI_LOW_FREQUENCY 24000000  /* sample mode threshold frequency  */
#define SPI_MOD_CLK 50000000    /* sample mode frequency  */

//#define DBI_BASE  		SUNXI_DBI1_PBASE

#define SPI_CTRL_REG	(0x04)
#define SPI_TC_REG      (0x08)       /* transfer control register */
#define SPI_FIFO_CTL_REG (0x18) /* fifo control register */

#define DBI_CTRL_REG0	(0x100)
#define DBI_CTRL_REG1	(0x104)
#define DBI_CTRL_REG2	(0x108)
#define DBI_TIMER_REG	(0x10C)
#define DBI_SIZE_REG	(0x110)
#define DBI_INT_REG		(0x120)
#define DBI_DBG_REG0	(0x124)
#define DBI_DBG_REG1	(0x128)

#define DBI_TXFIFO		(0x200)
#define DBI_RXFIFO		(0x300)

#define SPI_CR_DBI_EN   (0x1	  << 4)
#define SPI_CR_SELECT_DBI_MODE   (0x1	  << 3)
#define SPI_CR_SRST     (0x1 << 31) /* soft reset, write 1 will clear SPI control, auto clear to 0 */

#define SPI_TC_SDM      (0x1 << 13) /* master sample data mode, 1: normal sample mode;0:delay sample mode. */
#define SPI_TC_SDC      (0x1 << 11) /* master sample data control, 1: delay--high speed operation;0:no delay. */

#define SPI_FIFO_CTL_TX_DRQEN    (0x1 << 24) /* txFIFO DMA request enable,1:enable,0:disable */
#define SPI_FIFO_CTL_RX_DRQEN    (0x1 << 8) /* rxFIFO DMA request enable,1:enable,0:disable */
#define SPI_FIFO_CTL_TX_LEVEL    (0xFF << 16) /* txFIFO empty request trigger level,default 0x40 */
#define SPI_FIFO_CTL_DRQEN_MASK (SPI_FIFO_CTL_TX_DRQEN | SPI_FIFO_CTL_RX_DRQEN)

#define SPI_FIFO_CTL_RX_RST      (0x1 << 15) /* rxFIFO reset, write 1, auto clear to 0 */
#define SPI_FIFO_CTL_TX_RST      (0x1 << 31) /* txFIFO reset, write 1, auto clear to 0 */
#define SPI_FIFO_CTL_RX_LEVEL    (0xFF << 0) /* rxFIFO reday request trigger level,default 0x1 */

#define DBI_CR_READ		(0x1	  << 31)
#define DBI_CR_LSB_FIRST	(0x1	  << 19)
#define DBI_CR_TRANSMIT_MODE	(0x1	  << 15)
#define DBI_CR_FORMAT		(12)
#define DBI_CR_FORMAT_MASK	(0x7	  << DBI_CR_FORMAT)
#define DBI_CR_INTERFACE	(8)
#define DBI_CR_INTERFACE_MASK	(0x7	  << DBI_CR_INTERFACE)
#define DBI_CR_SRC_FORMAT	(4)
#define DBI_CR_SRC_FORMAT_MASK	(0xf	  << DBI_CR_SRC_FORMAT)
#define DBI_CR_VI_SRC_RGB16		(0x1	  << 0)//RGB source type 0:RGB32,1:RGB16

#define DBI_CR1_SOFT_TX_MODULE  (0x1	  << 31)
#define DBI_CR1_EN_MODEL    (29)           //0:always on mode	1:software trigger mode	  2:timer trigger mode	  3:te trigger mode
#define DBI_CR1_RGB666_FMT  (26)           //2 Data line RGB666 format 0:normal format  1 special format for ILITEK 2 special format for new vision
#define DBI_CR1_RXCLK_INV	(0x1	  << 25)
#define DBI_CR1_CLK_AUTO	(0x1	  << 24)
#define DBI_CR1_DCX_DATA	(0x1	  << 22)/*DCX data value: 0:DCX value equal to 0;1 DCX value equal to 1*/
#define DBI_CR1_RGB16_PIXEL0_IN_MSB  (0x1	  << 21)/*RGB16 data source select :Pixel0 in MSB*/

// #define DBI_CR1_            (0x1	  << 29)
#define DBI_CR2_HRDY_BYP    (0x1	  << 31)/*AHB read bypass*/
#define DBI_CR2_SDI_PIN	(0x1	  << 6)
#define DBI_CR2_DCX_PIN	(0x1	  << 5)
#define DBI_CR2_TE_ENABLE	(0x1	  << 0)
#define DBI_CR2_DMA_ENABLE	(0x1	  << 15)
#define DBI_INT_STA_MASK	(0x7f|(0x7f<<8))
#define DBI_INT_FIFO_EMPTY	(0x1 << 14) /*DBI FIFO is empty*/
#define DBI_INT_FIFO_FULL   (0x1 << 13) /*DBI FIFO is full*/
#define DBI_INT_TIMER_INT   (0x1 << 12) /* timer enable*/
#define DBI_INT_RD_DONE	    (0x1 << 11) /*All of data has been read*/
#define DBI_INT_TE_INT      (0x1 << 10) /* te enable*/
#define DBI_INT_STA_FRAME	(0x1 << 9) /* fram Transfer Completed */
#define DBI_INT_LINE_DONE   (0x1 << 8) /*A line of video data has been send*/
#define DBI_INT_FIFO_EMPTY_EN	(0x1 << 6) /* fram Transfer Completed En*/
#define DBI_INT_FIFO_FULL_EN   (0x1 << 5) /* dbi fifo full*/
#define DBI_TIMER_INT_EN	(0x1 << 4) /* timer interrupt*/
#define DBI_INT_RD_DONE_EN   (0x1 << 3) /* Read done interrupt enable*/
#define DBI_TE_INT_EN	(0x1 << 2) /* TE interrupt*/
#define DBI_INT_FRAM_DONE_EN	(0x1 << 1) /* fram Transfer Completed En*/
#define DBI_INT_LINE_DONE_EN	(0x1 << 0) /* Enable line done interrupt*/

#define SUNXI_SPI_DRQ_RX(ch) (DRQSRC_SPI0_RX + ch)
#define SUNXI_SPI_DRQ_TX(ch) (DRQDST_SPI0_TX + ch)


#endif /* __COMMON_DBI_I_H__ */
