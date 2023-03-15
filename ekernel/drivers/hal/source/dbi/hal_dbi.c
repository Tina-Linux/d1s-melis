/*
 * Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 *
 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.
 *
 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY¡¯S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR
 * MPEGLA, ETC.)
 * IN ALLWINNERS¡¯SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT
 * TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY¡¯S TECHNOLOGY.
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
#include <stdio.h>
#include <stdlib.h>
#include <hal_interrupt.h>
#include <hal_cache.h>
#include <hal_mem.h>
#include <hal_osal.h>
#include <hal_log.h>
#include <hal_gpio.h>
#include <hal_dma.h>
#include <hal_reset.h>

#include "platform_dbi.h"

#ifdef CONFIG_DRIVER_SYSCONFIG
#include <script.h>
#include <hal_cfg.h>
#endif

#define SPI_ALIGN(x, a) __ALIGN_KERNEL((x), (a))

#if (0)
#define CONFIG_SUNXI_DBI_CPU_XFER_ONLY
#endif

#define DBI_ERR(fmt, arg...) printf("%s()%d " fmt, __func__, __LINE__, ##arg)

#if 1
#define DBI_INFO(fmt, arg...)                                                  \
    do {                                                                   \
    } while (0);
#define DBI_DBG(fmt, arg...)                                                  \
    do {                                                                   \
    } while (0);
#else
#define DBI_INFO(fmt, arg...) printf("%s()%d " fmt, __func__, __LINE__, ##arg)
#define DBI_DBG(fmt, arg...) printf("%s()%d " fmt, __func__, __LINE__, ##arg)
#endif

static sunxi_dbi_t g_sunxi_dbi[HAL_DBI_MASTER_MAX];
struct sunxi_dbi_params_t g_sunxi_dbi_params[] = {
    DBI0_PARAMS,
    DBI1_PARAMS,
#ifdef DBI2_PARAMS
    DBI2_PARAMS,
#endif
};

#ifdef SIP_DBI0_PARAMS
struct sunxi_dbi_params_t g_sunxi_dbi_params_sip[] = {
    SIP_DBI0_PARAMS,
};
#endif

u32 frame_flag = 0;
u32 line_flag = 0;

/* delay internal read sample point */
static void spi_sample_delay(uint32_t sdm, uint32_t sdc, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_TC_REG);
    uint32_t org_val = reg_val;

    if (sdm)
    {
        reg_val |= SPI_TC_SDM;
    }
    else
    {
        reg_val &= ~SPI_TC_SDM;
    }

    if (sdc)
    {
        reg_val |= SPI_TC_SDC;
    }
    else
    {
        reg_val &= ~SPI_TC_SDC;
    }

    if (reg_val != org_val)
    {
        hal_writel(reg_val, dbi->base + SPI_TC_REG);
    }
}
/* soft reset spi controller */
static void dbi_soft_reset(sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_CTRL_REG);
    reg_val |= SPI_CR_SRST;
    hal_writel(reg_val, dbi->base + SPI_CTRL_REG);
}

/* enable dma irq */
static void dbi_enable_dma_irq(uint32_t bitmap, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_FIFO_CTL_REG);
    bitmap &= SPI_FIFO_CTL_DRQEN_MASK;
    reg_val |= bitmap;
    hal_writel(reg_val, dbi->base + SPI_FIFO_CTL_REG);
}

/* disable dma irq */
static void dbi_disable_dma_irq(uint32_t bitmap, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_FIFO_CTL_REG);
    bitmap &= SPI_FIFO_CTL_DRQEN_MASK;
    reg_val &= ~bitmap;
    hal_writel(reg_val, dbi->base + SPI_FIFO_CTL_REG);
}

/* reset fifo */
static void dbi_reset_fifo(sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_FIFO_CTL_REG);
    reg_val |= (SPI_FIFO_CTL_RX_RST | SPI_FIFO_CTL_TX_RST);
    /* Set the trigger level of RxFIFO/TxFIFO. */
    reg_val &= ~(SPI_FIFO_CTL_RX_LEVEL | SPI_FIFO_CTL_TX_LEVEL);
    reg_val |= (0x20 << 16) | 0x20;
    // reg_val |= (0x1 << 30);
    hal_writel(reg_val, dbi->base + SPI_FIFO_CTL_REG);
}
static void dbi_set_rx_trig(uint32_t val, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_FIFO_CTL_REG);

    reg_val &= ~SPI_FIFO_CTL_RX_LEVEL;
    reg_val |= val & SPI_FIFO_CTL_RX_LEVEL;
    hal_writel(reg_val, dbi->base + SPI_FIFO_CTL_REG);
}
static void dbi_set_tx_trig(uint32_t val, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + SPI_FIFO_CTL_REG);

    reg_val &= ~SPI_FIFO_CTL_TX_LEVEL;
    reg_val |= (val << 16) & SPI_FIFO_CTL_TX_LEVEL;
    hal_writel(reg_val, dbi->base + SPI_FIFO_CTL_REG);
}
static s32 dbi_tx_fifo_empty(sunxi_dbi_t* dbi)
{
	u32 reg_val;
	reg_val = hal_readl(dbi->base + DBI_INT_REG);
	if(reg_val & DBI_INT_FIFO_EMPTY)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
static void dbi_clear_read_int(sunxi_dbi_t* dbi)
{
	u32 reg_val;
	reg_val = hal_readl(dbi->base + DBI_INT_REG);
	reg_val |= DBI_INT_RD_DONE;
	hal_writel(reg_val, dbi->base + DBI_INT_REG);
}
static s32 dbi_rx_valid(sunxi_dbi_t* dbi)
{
	u32 reg_val;
	reg_val = hal_readl(dbi->base + DBI_INT_REG);
	if(reg_val & DBI_INT_RD_DONE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
static void dbi_clear_empty_int(sunxi_dbi_t* dbi)
{
	u32 reg_val;
	reg_val = hal_readl(dbi->base + DBI_INT_REG);
	reg_val |= DBI_INT_FIFO_EMPTY;
	hal_writel(reg_val, dbi->base + DBI_INT_REG);
}
static void dbi_enable(sunxi_dbi_t* dbi)
{
	u32 reg_val;
	reg_val = hal_readl(dbi->base + SPI_CTRL_REG);
	reg_val |= SPI_CR_DBI_EN;
	hal_writel(reg_val, dbi->base + SPI_CTRL_REG);
}

static void set_master(sunxi_dbi_t* dbi)
{
	u32 reg_val;

	reg_val = hal_readl(dbi->base + SPI_CTRL_REG);
	reg_val |= (0x1 << 1);
	hal_writel(reg_val, dbi->base + SPI_CTRL_REG);

}

static void dbi_disable(sunxi_dbi_t* dbi)
{
	u32 reg_val;
	reg_val = hal_readl(dbi->base + SPI_CTRL_REG);
	reg_val &= ~SPI_CR_DBI_EN;
	hal_writel(reg_val, dbi->base + SPI_CTRL_REG);
}
static void dbi_enable_irq(uint32_t bitmap, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + DBI_INT_REG);
    bitmap &= DBI_INT_STA_MASK;
    reg_val |= bitmap;
    hal_writel(reg_val, dbi->base + DBI_INT_REG);
}
static void dbi_disable_irq(uint32_t bitmap, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + DBI_INT_REG);

    bitmap &= DBI_INT_STA_MASK;
    reg_val &= ~bitmap;
    hal_writel(reg_val, dbi->base + DBI_INT_REG);
}
/* clear irq pending */
static void dbi_clr_irq_pending(uint32_t bitmap, sunxi_dbi_t *dbi)
{
    uint32_t reg_val = hal_readl(dbi->base + DBI_INT_REG);
    reg_val &= DBI_INT_STA_MASK;
    reg_val |= bitmap;
    hal_writel(reg_val, dbi->base + DBI_INT_REG);
}
static dbi_master_status_t hal_dbi_register_config(sunxi_dbi_t* dbi)
{
	u32 reg,reg_val;

	/* Set SDBI working on dbi mode */
	reg_val = hal_readl(dbi->base + SPI_CTRL_REG);
	reg_val |= SPI_CR_SELECT_DBI_MODE;
	hal_writel(reg_val, dbi->base + SPI_CTRL_REG);

	/* 1.config dbi DBI_CTRL_REG0 */
	reg_val = 0;

	if (dbi->dbi_reg_cfg.cmd_typ) {
		reg_val |= DBI_CR_READ;
		dbi->mode_type = DBI_MODE_TYPE_RX;
	}
	else
	{
		reg_val &= ~DBI_CR_READ;
		dbi->mode_type = DBI_MODE_TYPE_TX;
	}

	/* output data sequence */
    if(dbi->dbi_reg_cfg.output_data_sequence)
	{
		reg_val |= DBI_CR_LSB_FIRST;
	}
	else
	{
		reg_val &= ~DBI_CR_LSB_FIRST;
	}

	dbi_irq_disable(dbi->port, DBI_MODE_TYPE_TX);
	/* transmit video */
	if(dbi->dbi_reg_cfg.transmit_mode)
	{
		int line_size = dbi->dbi_reg_cfg.h_size;
		reg_val |= DBI_CR_TRANSMIT_MODE;

		if (line_size < BULK_DATA_BOUNDARY)
			dbi_enable_irq(DBI_INT_LINE_DONE_EN, dbi);  /* enable line int */

		switch(dbi->dbi_reg_cfg.dbi_en_mode) {
		case DBI_MODE_ALLWAYS_ON:
			dbi_enable(dbi);
			dbi_enable_irq(DBI_INT_FRAM_DONE_EN, dbi);
			break;
		case DBI_MODE_SOFTWARE_TRIGGER:
			dbi_enable_irq(DBI_TE_INT_EN, dbi);  /* enable te int */
			break;
		case DBI_MODE_TIMER_TRIGGER:
			dbi_enable_irq(DBI_TIMER_INT_EN, dbi);  /* enable timer int */

			reg_val = hal_readl(dbi->base + DBI_TIMER_REG);  /* dbi tmier value, clock source is SCLK */
			reg_val &= ~(0x7fffffff << 0);
			reg_val |= (0x38A40 << 0);  /* timer_value = SCLK/frame_rate - one_pixel_clock_cycle * one_frame_pixel_value */
			reg_val &= ~(0x1u << 31);
			reg_val |= (0x1u << 31);
			hal_writel(reg_val, dbi->base + DBI_TIMER_REG);
			break;
		case DBI_MODE_TE_TRIGGER:
			dbi_enable_irq(DBI_TE_INT_EN, dbi);/* enable te int */
			break;
		}
	}
	else
	{
		reg_val &= ~DBI_CR_TRANSMIT_MODE;
		dbi_irq_enable(dbi->port, DBI_MODE_TYPE_TX);
	}

	/* output data format */
	reg_val &= ~(DBI_CR_FORMAT_MASK);
	if(dbi->dbi_reg_cfg.dbi_output_data_format == DBI_RGB111)
	{
		reg_val &= ~(0x7 << DBI_CR_FORMAT);
	}
	else
	{
		reg_val |= ((dbi->dbi_reg_cfg.dbi_output_data_format) << DBI_CR_FORMAT);
	}

	/* dbi interface select */
	reg_val &= ~(DBI_CR_INTERFACE_MASK);
	if (dbi->dbi_reg_cfg.dbi_interface == L3I1)
	{
		reg_val &= ~((0x7) << DBI_CR_INTERFACE);
	}
	else
	{
		reg_val |= ((dbi->dbi_reg_cfg.dbi_interface) << DBI_CR_INTERFACE);
	}
	/* source format */
	reg_val &= ~(DBI_CR_SRC_FORMAT_MASK);
	if (dbi->dbi_reg_cfg.dbi_source_format == DBI_SRC_RGB)
	{
		reg_val &= ~((0xf) << DBI_CR_SRC_FORMAT);
	}
	else
	{
		reg_val |= ((dbi->dbi_reg_cfg.dbi_source_format) << DBI_CR_SRC_FORMAT);
	}
	/* RGB bit order */
	if (dbi->dbi_reg_cfg.rgb_bit_order == 1)
	{
		reg_val |= ((0x1) << 2);
	}
	else
	{
		reg_val &= ~((0x1) << 2);
	}
	if (dbi->dbi_reg_cfg.epos)
	{
		reg_val |= ((0x1) << 1);
	}
	else
	{
		reg_val &= ~((0x1) << 1);
	}
	/* RGB16 or RGB32 */
	if(dbi->dbi_reg_cfg.rgb_src_typ)
	{
		reg_val |= DBI_CR_VI_SRC_RGB16;
	}
	else
	{
		reg_val &= ~DBI_CR_VI_SRC_RGB16;
	}
	hal_writel(reg_val, dbi->base + DBI_CTRL_REG0);

	/* 2. config dbi DBI_CTRL_REG1 */
	set_dcx(dbi->port, dbi->dbi_reg_cfg.dcx_data);

	config_dbi_data_src_sel(dbi->port, dbi->dbi_reg_cfg.rgb16_pix0_post);

	/* DBI enable mode select:0 always on mode;1software trgger mode;2 timer trgger mode; 3 te trigger mode */
	config_dbi_enable_mode(dbi->port, dbi->dbi_reg_cfg.dbi_en_mode);

	/* config dbi clock mode */
	config_dbi_output_clk_mode(dbi->port, dbi->dbi_reg_cfg.dbi_output_clk_mode);

	/* 3. config dbi DBI_CTRL_REG2 */
	reg_val = 0;
	if(dbi->dbi_reg_cfg.ahb_ready_bypass)
	{
		reg_val |= DBI_CR2_HRDY_BYP;
	}
	else
	{
		reg_val &= ~DBI_CR2_HRDY_BYP;
	}
	if (dbi->dbi_reg_cfg.dbi_interface == D2LI) {
		reg_val |= DBI_CR2_DCX_PIN;
		reg_val &= ~DBI_CR2_SDI_PIN;
	}
	else
	{
		reg_val &= ~DBI_CR2_DCX_PIN;
		reg_val |= DBI_CR2_SDI_PIN;
	}

	if(dbi->dbi_reg_cfg.te_en) {
		// Only TE and non-TE dbi mode are supported.
		set_dbi_en_mode(dbi->port, DBI_MODE_TE_TRIGGER);

		/*te enable*/
		reg_val |= 0x1;
		if (dbi->dbi_reg_cfg.te_edge_select == DBI_TE_FALLING_EDGE)
			reg_val |= (0x1 << 1); //1 TE falling edge
		else
			reg_val &= ~(0x1 << 1); //0 TE rising edge
	} else
		reg_val &= ~(0x3 << 0); // te disable

	hal_writel(reg_val, dbi->base + DBI_CTRL_REG2);

	reg_val = 0;
	reg_val=(dbi->dbi_reg_cfg.h_size & 0x7ff) | ((dbi->dbi_reg_cfg.v_size & 0x7ff) << 16);
	hal_writel(reg_val, dbi->base + DBI_SIZE_REG);

	// default config
	default_config(dbi->port);

	return DBI_MASTER_OK;

}
static dbi_master_status_t dbi_cpu_write(sunxi_dbi_t *dbi)
{
    uint32_t len = dbi->transfer->tx_len;
    const uint8_t *buf = dbi->transfer->tx_buf;
    //uint32_t poll_time = 0xff;
    if (NULL == buf)
    {
        DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
        return DBI_MASTER_INVALID_PARAMETER;
    }

    for (; len > 0; --len)
    {
        hal_writeb(*buf++, dbi->base + DBI_TXFIFO);
    }
    dbi_clear_empty_int(dbi);
    // *(volatile unsigned int *)0x04026014 = 0x00000012;

    return DBI_MASTER_OK;
}
static dbi_master_status_t dbi_cpu_read(sunxi_dbi_t *dbi)
{
    uint32_t len = dbi->transfer->rx_len;
    uint8_t *buf = dbi->transfer->rx_buf;
    uint32_t poll_time = 0xff;
    uint32_t i;

    if (NULL == buf)
    {
        DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
        return DBI_MASTER_INVALID_PARAMETER;
    }
    dbi_clear_read_int(dbi);
    dbi_enable(dbi);
    for (i = 0; i < len; i++)
    {
        while (!dbi_rx_valid(dbi) && poll_time--)
            ;
        if (poll_time <= 0)
        {
            DBI_ERR("[dbi%d] cpu receive data time out!\n",
                    dbi->port);
            DBI_ERR("[dbi%d] dump reg:\n", dbi->port);
            return DBI_MASTER_ERROR_TIMEOUT;
        }

        *(buf + i) = hal_readb(dbi->base + DBI_RXFIFO);
    }
    dbi_disable(dbi);

    return DBI_MASTER_OK;
}

#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
static dbi_master_status_t dbi_dma_tx_config(struct sunxi_dbi *dbi)
{
    hal_dma_chan_status_t ret;
    uint32_t flags = 0;
    uint32_t len = dbi->transfer->tx_len;
    uint8_t const *buf = dbi->transfer->tx_buf;
    struct dma_slave_config *config = &dbi->dma_tx.config;

    if (NULL == buf)
    {
        DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
        return DBI_MASTER_INVALID_PARAMETER;
    }

    if (len <= ALIGN_DMA_BUF_SIZE)
    {
        memcpy(dbi->align_dma_buf, buf, len);
        buf = dbi->align_dma_buf;
    }
    else
    {
        /* buffer on DMA must align to cache line */
        if (len & (64 - 1))
        {
            DBI_ERR("[dbi%d] tx buf or len not align to 64\n",
                    dbi->port);
            return DBI_MASTER_INVALID_PARAMETER;
        }
    }
    hal_dcache_clean((unsigned long)buf, SPI_ALIGN(len, 64));

    ret = hal_dma_chan_request(&dbi->dma_tx.chan);
    if (ret == HAL_DMA_CHAN_STATUS_BUSY)
    {
        DBI_ERR("[dbi%d] request dma_rx failed\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    dbi_set_tx_trig(0xFF, dbi);
    dbi_enable_dma_irq(SPI_FIFO_CTL_TX_DRQEN, dbi);

    config->direction = DMA_MEM_TO_DEV;
    config->dst_addr = dbi->base + DBI_TXFIFO;
    config->src_addr = (unsigned long)buf;

    if (len % DMA_SLAVE_BUSWIDTH_4_BYTES)
    {
        config->src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
        config->dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
        config->dst_maxburst = DMA_SLAVE_BURST_16;
        config->src_maxburst = DMA_SLAVE_BURST_16;
    }
    else
    {
        config->src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        config->dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        config->dst_maxburst = DMA_SLAVE_BURST_8;
        config->src_maxburst = DMA_SLAVE_BURST_8;
    }

    config->slave_id =
        sunxi_slave_id(SUNXI_SPI_DRQ_TX(dbi->port), DRQSRC_SDRAM);

    DBI_INFO("[dbi%d] config:\n"
             "  direction: %d\n"
             "  src_addr: 0x%08lx,"
             "  dst_addr: 0x%08lx\n"
             "  src_addr_width: %d,"
             "  dst_addr_width: %d\n"
             "  src_maxburst: %lu,"
             "  dst_maxburst: %lu\n"
             "  slave_id: 0x%08lx\n",
             dbi->port, config->direction, config->src_addr,
             config->dst_addr, config->src_addr_width,
             config->dst_addr_width, config->src_maxburst,
             config->dst_maxburst, config->slave_id);

    return DBI_MASTER_OK;
}

static void spi_dma_tx_callback(void *para)
{
    sunxi_dbi_t *dbi = (sunxi_dbi_t *)para;

    DBI_INFO("[dbi%d] DMA TX callback function\n", dbi->port);
    if (hal_sem_post(dbi->xSemaphore_tx))
    {
        DBI_ERR("[dbi%d] xSemaphoreGive failed.\n", dbi->port);
    }
}

static dbi_master_status_t dbi_dma_tx_submit(struct sunxi_dbi *dbi)
{
    hal_dma_status_t ret;
    uint32_t len = dbi->transfer->tx_len;
    struct dma_slave_config *config = &dbi->dma_tx.config;
    struct sunxi_dma_chan *chan = dbi->dma_tx.chan;

    ret = hal_dma_slave_config(chan, config);
    if (ret)
    {
        DBI_ERR("[dbi%d] dma slave config failed! return %d\n",
                dbi->port, ret);
        return DBI_MASTER_ERROR;
    }

    ret = hal_dma_prep_device(chan, config->dst_addr, config->src_addr, len,
                              config->direction);
    if (ret)
    {
        DBI_ERR("[dbi%d] dma prep device failed! return %d\n",
                dbi->port, ret);
        return DBI_MASTER_ERROR;
    }

    chan->callback = spi_dma_tx_callback;
    chan->callback_param = dbi;

    ret = hal_dma_start(chan);
    if (ret)
    {
        DBI_ERR("[dbi%d] dma start error! return %d\n", dbi->port,
                ret);
        return DBI_MASTER_ERROR;
    }
    return DBI_MASTER_OK;
}
static void dbi_dma_rx_callback(void *para)
{
    sunxi_dbi_t *dbi = (sunxi_dbi_t *)para;

    if (hal_sem_post(dbi->xSemaphore_rx))
    {
        DBI_ERR("[dbi%d] xSemaphoreGive failed.\n", dbi->port);
    }
}

static dbi_master_status_t dbi_dma_rx_config(struct sunxi_dbi *dbi)
{
    hal_dma_chan_status_t ret;
    uint32_t flags = 0;
    uint32_t len = dbi->transfer->rx_len, size = 0;
    uint8_t *buf = dbi->transfer->rx_buf;
    struct dma_slave_config *config = &dbi->dma_rx.config;

    if (NULL == buf)
    {
        DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
        return DBI_MASTER_INVALID_PARAMETER;
    }

    if (len <= ALIGN_DMA_BUF_SIZE)
    {
        buf = dbi->align_dma_buf;
        memset(buf, 0, ALIGN_DMA_BUF_SIZE);
    }
    else
    {
        DBI_ERR("[dbi%d] rx size over dma align buf size\n",
                dbi->port);
        /* buffer on DMA must align to cache line */
        if ((unsigned long)buf & (64 - 1) || len & (64 - 1))
        {
            DBI_ERR("[dbi%d] rx buf or len not align to 64\n",
                    dbi->port);
            return DBI_MASTER_INVALID_PARAMETER;
        }
    }
    hal_dcache_clean_invalidate((unsigned long)buf, SPI_ALIGN(len, 64));
    dbi_enable_dma_irq(SPI_FIFO_CTL_RX_DRQEN, dbi);

    ret = hal_dma_chan_request(&dbi->dma_rx.chan);
    if (ret == HAL_DMA_CHAN_STATUS_BUSY)
    {
        DBI_ERR("[dbi%d] request dma_rx failed\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    config->direction = DMA_DEV_TO_MEM;
    config->dst_addr = (unsigned long)buf;
    config->src_addr = dbi->base + DBI_RXFIFO;

    if (len % DMA_SLAVE_BUSWIDTH_4_BYTES)
    {
        config->src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
        config->dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;
        config->dst_maxburst = DMA_SLAVE_BURST_16;
        config->src_maxburst = DMA_SLAVE_BURST_16;
    }
    else
    {
        config->src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        config->dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        config->dst_maxburst = DMA_SLAVE_BURST_8;
        config->src_maxburst = DMA_SLAVE_BURST_8;
    }

    config->slave_id =
        sunxi_slave_id(DRQDST_SDRAM, SUNXI_SPI_DRQ_RX(dbi->port));

    DBI_INFO("[dbi%d] config:\n"
             "  direction: %d\n"
             "  src_addr: 0x%08lx,"
             "  dst_addr: 0x%08lx\n"
             "  src_addr_width: %d,"
             "  dst_addr_width: %d\n"
             "  src_maxburst: %lu,"
             "  dst_maxburst: %lu\n"
             "  slave_id: 0x%08lx\n",
             dbi->port, config->direction, config->src_addr,
             config->dst_addr, config->src_addr_width,
             config->dst_addr_width, config->src_maxburst,
             config->dst_maxburst, config->slave_id);

    return DBI_MASTER_OK;
}

static dbi_master_status_t dbi_dma_rx_submit(struct sunxi_dbi *dbi)
{
    hal_dma_status_t ret;
    uint32_t flags = 0;
    uint32_t len = dbi->transfer->rx_len, size = 0;
    struct sunxi_dma_chan *chan = dbi->dma_rx.chan;
    struct dma_slave_config *config = &dbi->dma_rx.config;

    ret = hal_dma_slave_config(chan, config);
    if (ret)
    {
        DBI_ERR("[dbi%d] dma slave config failed! return %d\n",
                dbi->port, ret);
        return DBI_MASTER_ERROR;
    }

    ret = hal_dma_prep_device(chan, config->dst_addr, config->src_addr, len,
                              config->direction);
    if (ret)
    {
        DBI_ERR("[dbi%d] dma prep device failed! return %d\n",
                dbi->port, ret);
        return DBI_MASTER_ERROR;
    }

    chan->callback = dbi_dma_rx_callback;
    chan->callback_param = dbi;

    ret = hal_dma_start(chan);
    if (ret)
    {
        DBI_ERR("[dbi%d] dma start error! return %d\n", dbi->port,
                ret);
        return DBI_MASTER_ERROR;
    }

    return DBI_MASTER_OK;
}
static dbi_master_status_t dbi_dma_tx_complete(sunxi_dbi_t *dbi)
{
    uint32_t flags = 0;
    uint32_t timeout = 0x7fffffff;
    struct sunxi_dma_chan *chan = dbi->dma_tx.chan;
    hal_dma_status_t dma_ret;
    dbi_master_status_t ret = 0;
    int xResult;

    xResult = hal_sem_timedwait(dbi->xSemaphore_tx, 5000);
    if (xResult == 0)
    {
        if (SPI_XFER_OK == dbi->result)
        {
            DBI_INFO("ok\n");
            ret = DBI_MASTER_OK;
        }
        else if (SPI_XFER_FAILED == dbi->result)
        {
            DBI_ERR("[dbi%d] xfer failed...\n", dbi->port);
            DBI_INFO("[dbi%d] dump reg:\n", dbi->port);
            ret = DBI_MASTER_ERROR;
        }
    }
    else
    {
        DBI_ERR("[dbi%d] dma xfer timeout\n", dbi->port);
        DBI_INFO("[dbi%d] dump reg:\n", dbi->port);

        dbi->result = SPI_XFER_FAILED;

        if (hal_sem_post(dbi->xSemaphore_tx))
        {
            DBI_ERR("[dbi%d] xSemaphoreGive failed.\n", dbi->port);
        }

        ret = DBI_MASTER_ERROR_TIMEOUT;
    }

end:

    dma_ret = hal_dma_stop(chan);
    if (dma_ret)
    {
        DBI_ERR("[dbi%d] dma stop error! ret %d\n", dbi->port,
                dma_ret);
        return DBI_MASTER_ERROR;
    }

    dma_ret = hal_dma_chan_free(chan);
    if (dma_ret)
    {
        DBI_ERR("[dbi%d] free dma_tx failed, ret %d\n", dbi->port,
                dma_ret);
        return DBI_MASTER_ERROR;
    }

    return ret;
}
static dbi_master_status_t dbi_dma_rx_complete(sunxi_dbi_t *dbi)
{
    uint32_t flags = 0;
    uint32_t len = dbi->transfer->rx_len, size = 0;
    uint8_t *buf = dbi->transfer->rx_buf;
    struct sunxi_dma_chan *chan = dbi->dma_rx.chan;
    hal_dma_status_t dma_ret;
    dbi_master_status_t ret;
    int  xResult;

    xResult = hal_sem_timedwait(dbi->xSemaphore_rx, 100);
    if (xResult != 0)
    {
        DBI_ERR("[dbi%d] dma xfer timeout\n", dbi->port);
        DBI_INFO("[dbi%d] dump reg:\n", dbi->port);

        dbi->result = SPI_XFER_FAILED;

        ret = DBI_MASTER_ERROR_TIMEOUT;
        goto end;
    }

    hal_dcache_invalidate((unsigned long)dbi->align_dma_buf, SPI_ALIGN(len, 64));
    dbi->result = SPI_XFER_OK;
    if (len <= ALIGN_DMA_BUF_SIZE)
    {
        memcpy(buf, dbi->align_dma_buf, len);
    }
    ret = DBI_MASTER_OK;
    DBI_INFO("ok\n");

end:
    dma_ret = hal_dma_stop(chan);
    if (dma_ret)
    {
        DBI_ERR("[dbi%d] dma stop error! ret %d\n", dbi->port,
                dma_ret);
        ret = DBI_MASTER_ERROR;
    }

    dma_ret = hal_dma_chan_free(chan);
    if (dma_ret)
    {
        DBI_ERR("[dbi%d] free dma_rx failed, ret %d\n", dbi->port,
                dma_ret);
        return DBI_MASTER_ERROR;
    }

    return ret;
}

#endif

static dbi_master_status_t dbi_transfer(sunxi_dbi_t *dbi)
{
	dbi_enable(dbi);
    switch (dbi->mode_type)
    {

        case DBI_MODE_TYPE_RX:
        {
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            /* >64 use DMA transfer, or use cpu */
            if (dbi->transfer->rx_len > BULK_DATA_BOUNDARY)
            {
                DBI_INFO("[dbi%d] rx by dma\n", dbi->port);
                /* For Rx mode, the DMA end(not TC flag) is real end. */
                if (dbi_dma_rx_config(dbi))
                {
                    return DBI_MASTER_ERROR;
                }
                if (dbi_dma_rx_submit(dbi))
                {
                    return DBI_MASTER_ERROR;
                }
	        dbi_irq_enable(dbi->port, DBI_MODE_TYPE_RX);
            }
            else
            {
#endif
                DBI_DBG("[dbi%d] rx by cpu\n", dbi->port);
                dbi_cpu_read(dbi);
	        dbi_irq_enable(dbi->port, DBI_MODE_TYPE_RX);
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            }
#endif
            break;
        }
        case DBI_MODE_TYPE_TX:
        {
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            /* >64 use DMA transfer, or use cpu */
            if (dbi->transfer->tx_len > BULK_DATA_BOUNDARY)
            {
				DBI_DBG("dbi->transfer->tx_len : %d \n", dbi->transfer->tx_len);
                DBI_DBG("[dbi%d] tx by dma\n", dbi->port);
                dbi->sem = 1;
                if (dbi_dma_tx_config(dbi))
                {
                    return DBI_MASTER_ERROR;
                }
                if (dbi_dma_tx_submit(dbi))
                {
                    return DBI_MASTER_ERROR;
                }

            }
            else
            {
#endif
                DBI_DBG("[dbi%d] tx by cpu\n", dbi->port);
                dbi_cpu_write(dbi);
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            }
#endif
            break;
        }
        default:
        {
            DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
            return DBI_MASTER_INVALID_PARAMETER;
        }
    }

	return DBI_MASTER_OK;
}
static enum hal_irqreturn dbi_irq_handler(void *ptr)
{
	u32 reg_val, i;
	u32 int_status, int_en_status;
	sunxi_dbi_t *dbi = (sunxi_dbi_t *)ptr;
	int_status = hal_readl(dbi->base + DBI_INT_REG);

	/* frame done int */
	if((int_status & DBI_INT_STA_FRAME) && (int_status & DBI_INT_FRAM_DONE_EN))
	{
		frame_flag = 1;
       		dbi_clr_irq_pending(DBI_INT_STA_FRAME,dbi);
		dbi->result = SPI_XFER_OK;

		/*
		if (hal_sem_post(dbi->xSemaphore_tx))
		{
			DBI_ERR("[dbi%d] xSemaphoreGive failed.\n", dbi->port);
		}
		*/
	}
	/* fifo empty int */
	if((int_status & DBI_INT_FIFO_EMPTY) && (int_status & DBI_INT_FIFO_EMPTY_EN))
	{
		dbi_clr_irq_pending(DBI_INT_FIFO_EMPTY,dbi);
		dbi_irq_disable(dbi->port, DBI_MODE_TYPE_TX);

	}
	/* fifo full int */
	if((int_status & DBI_INT_FIFO_FULL) && (int_status & DBI_INT_FIFO_FULL_EN))
	{
		dbi_clr_irq_pending(DBI_INT_FIFO_FULL,dbi);
	}

	/* timer int */
	if((int_status & DBI_INT_TIMER_INT) && (int_status & DBI_TIMER_INT_EN))
	{
		dbi_clr_irq_pending(DBI_INT_TIMER_INT,dbi);
	}

	/* read done int */
	if((int_status & DBI_INT_RD_DONE) && (int_status & DBI_INT_RD_DONE_EN))
	{
		dbi_clr_irq_pending(DBI_INT_RD_DONE,dbi);
	}

	/* te int */
	if((int_status & DBI_INT_TE_INT) && (int_status & DBI_TE_INT_EN))
	{
	        dbi_clr_irq_pending(DBI_INT_TE_INT,dbi);
		if(dbi->dbi_reg_cfg.dbi_en_mode == DBI_MODE_SOFTWARE_TRIGGER)
		{
			reg_val = hal_readl(dbi->base + DBI_CTRL_REG1);	/* enable software trigger, auto clear to '0' */
			reg_val |= DBI_CR1_SOFT_TX_MODULE;
			hal_writel(reg_val, dbi->base + DBI_CTRL_REG1);
		}
	}

	/* line done int */
	if((int_status & DBI_INT_LINE_DONE) && (int_status & DBI_INT_LINE_DONE_EN))
	{
		line_flag = 1;
		dbi_clr_irq_pending(DBI_INT_LINE_DONE,dbi);
		dbi->result = SPI_XFER_OK;
	}
	return 0;
}
static dbi_master_status_t dbi_pinctrl_init(sunxi_dbi_t *dbi)
{
    if (dbi->port >= HAL_DBI_MASTER_MAX)
    {
        DBI_ERR("[dbi%d] invalid port\n", dbi->port);
        return DBI_MASTER_INVALID_PARAMETER;
    }
	DBI_DBG("dbi port :%d\n", dbi->port);

#ifdef CONFIG_DRIVER_SYSCONFIG
    user_gpio_set_t gpio_cfg[6] = {0};
    gpio_pin_t dbi_pin;
    int count, i, ret;
    char dbi_name[16];

    sprintf(dbi_name, "spi%d", dbi->port);
    count = Hal_Cfg_GetGPIOSecKeyCount(dbi_name);
    if (!count) {
        DBI_ERR("[dbi%d] not support in sys_config\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    Hal_Cfg_GetGPIOSecData(dbi_name, gpio_cfg, count);

    for (i = 0; i < count; i++) {

		dbi_pin = (gpio_cfg[i].port - 1) * PINS_PER_BANK + gpio_cfg[i].port_num;
        ret = hal_gpio_pinmux_set_function(dbi_pin, gpio_cfg[i].mul_sel);
		if (ret) {
            DBI_ERR("[dbi%d] PIN%u set function failed! return %d\n",
                dbi->port, dbi_pin, ret);
			return DBI_MASTER_ERROR;
        }

		ret = hal_gpio_set_driving_level(dbi_pin, gpio_cfg[i].drv_level);
        if (ret) {
            DBI_ERR("[dbii%d] PIN%u set driving level failed! return %d\n",
                dbi->port, dbi_pin, ret);
			return DBI_MASTER_ERROR;
        }

		if (gpio_cfg[i].pull) {
            ret = hal_gpio_set_pull(dbi_pin, gpio_cfg[i].pull);
			if (ret) {
		        DBI_ERR("[dbii%d] PIN%u set driving level failed! return %d\n",
			        dbi->port, dbi_pin, ret);
				return DBI_MASTER_ERROR;
	        }
		}
    }

    return ret;
#else
    uint8_t i;
    uint32_t flags = 0;
    int ret = DBI_MASTER_OK;
    struct sunxi_dbi_params_t *para;

    para = &g_sunxi_dbi_params[dbi->port];

    dbi->pin = malloc(sizeof(gpio_pin_t) * 6);
    dbi->pin[0] = para->gpio_clk;
    dbi->pin[1] = para->gpio_mosi;
    dbi->pin[2] = para->gpio_cs0;
    dbi->pin[3] = para->gpio_miso;
    dbi->pin[4] = para->gpio_wp;
    dbi->pin[5] = para->gpio_hold;

    for (i = 0; i < para->gpio_num; i++)
    {
        ret = hal_gpio_pinmux_set_function(dbi->pin[i],para->mux);
        if (ret)
        {
            DBI_ERR("[dbi%d] PIN%u set function failed! return %d\n",
                dbi->port, dbi->pin[i], ret);
            ret = DBI_MASTER_ERROR;
	        goto err;
        }
        ret = hal_gpio_set_driving_level(dbi->pin[i],
			para->driv_level);
        if (ret)
        {
            DBI_ERR("[dbi%d] PIN%u set driving level failed! return %d\n",
                dbi->port, dbi->pin[i], ret);
            ret = DBI_MASTER_ERROR;
	        goto err;
        }
        if (i == 2) //CS
        {
            ret = hal_gpio_set_pull(dbi->pin[i], GPIO_PULL_UP);
        }
        else
        {
            ret = hal_gpio_set_pull(dbi->pin[i], GPIO_PULL_DOWN_DISABLED);
        }
    }

err:
    free(dbi->pin);
#endif
    return ret;
}

static dbi_master_status_t dbi_clk_init(sunxi_dbi_t *dbi, u32 mode_clk)
{
    unsigned long rate;
    hal_clk_status_t ret;
    hal_reset_type_t reset_type = HAL_SUNXI_RESET;
    u32  reset_id;
    hal_clk_type_t clk_type = HAL_SUNXI_CCU;
#if defined(SUNXI_CLK_TYPE_PCLK_SPI)
    hal_clk_type_t clk_type_pclk = SUNXI_CLK_TYPE_PCLK_SPI;
#else
    hal_clk_type_t clk_type_pclk = HAL_SUNXI_CCU;
#endif

    hal_clk_id_t clk_id;
    hal_clk_id_t clk_bus_id;
    /* hal_clk_t clk; */

    switch (dbi->port)
    {
	    case 0:
		    clk_id = SUNXI_CLK_SPI(0);
		    clk_bus_id = SUNXI_CLK_BUS_SPI(0);
		    reset_id = SUNXI_CLK_RST_SPI(0);
		    break;
	    case 1:
		    clk_id = SUNXI_CLK_SPI(1);
		    clk_bus_id = SUNXI_CLK_BUS_SPI(1);
		    reset_id = SUNXI_CLK_RST_SPI(1);
		    break;
#ifdef SPI2_PARAMS
	    case 2:
		    clk_id = SUNXI_CLK_SPI(2);
		    clk_bus_id = SUNXI_CLK_BUS_SPI(2);
		    reset_id = SUNXI_CLK_RST_SPI(2);
		    break;
#endif
	    default:
		    DBI_ERR("dbi%d is invalid\n", dbi->port);
		    return DBI_MASTER_INVALID_PARAMETER;
    }

    dbi->reset = hal_reset_control_get(reset_type, reset_id);
    hal_reset_control_deassert(dbi->reset);

    dbi->mclk = hal_clock_get(clk_type, clk_id);
    dbi->bus_clk = hal_clock_get(clk_type, clk_bus_id);
    dbi->pclk = hal_clock_get(clk_type_pclk, SUNXI_CLK_PLL_SPI);

    ret = hal_clk_set_parent(dbi->mclk, dbi->pclk);
    if (ret)
    {
        DBI_ERR("[dbi%d] clk set parent failed! return %d\n",
                dbi->port, ret);
        return DBI_MASTER_ERROR;
    }

    rate = mode_clk;

    ret = hal_clk_set_rate(dbi->mclk, rate);
    if (ret)
    {
        DBI_ERR("[dbi%d] clk set rate failed! return %d\n", dbi->port,
                ret);
        return DBI_MASTER_ERROR;
    }

    rate = hal_clk_get_rate(dbi->mclk);
    if (!rate)
    {
        DBI_ERR("[dbi%d] clk get rate failed! return %ld\n", dbi->port,
                rate);
        return DBI_MASTER_ERROR;
    }

	if (hal_clock_is_enabled(dbi->bus_clk) != HAL_CLK_STATUS_ENABLED) {
	    ret = hal_clock_enable(dbi->bus_clk);
		if (ret) {
			DBI_ERR("[dbi%d] couldn't enable mlck! return %d\n", dbi->port,
					ret);
			return DBI_MASTER_ERROR;
		}
	}

	if (hal_clock_is_enabled(dbi->mclk) != HAL_CLK_STATUS_ENABLED) {
	    ret = hal_clock_enable(dbi->mclk);
		if (ret) {
			DBI_ERR("[dbi%d] couldn't enable mlck! return %d\n", dbi->port,
					ret);
			return DBI_MASTER_ERROR;
		}
	}

    return DBI_MASTER_OK;
}

static dbi_master_status_t dbi_clk_exit(sunxi_dbi_t *dbi)
{
    hal_clk_status_t ret;

    ret = hal_clock_disable(dbi->mclk);
    ret = hal_clock_disable(dbi->bus_clk);
    if (ret)
    {
        DBI_ERR("[dbi%d] couldn't disable mlck! return %d\n",
                dbi->port, ret);
        return DBI_MASTER_ERROR;
    }

    hal_clock_put(dbi->mclk);
    hal_clock_put(dbi->bus_clk);

    hal_reset_control_assert(dbi->reset);
    hal_reset_control_put(dbi->reset);

    return DBI_MASTER_OK;
}

static dbi_master_status_t dbi_cpu_complete(sunxi_dbi_t *dbi)
{
    uint32_t timeout = 0xffff;

    while (!dbi->result && timeout--)
        ;

    if (!dbi_tx_fifo_empty(dbi)) {
	printf("Datas had not been sent. \n");
	return DBI_MASTER_ERROR;
    }

    if (timeout <= 0)
    {
        DBI_ERR("[dbi%d] xfer timeout\n", dbi->port);
        DBI_INFO("[dbi%d] dump reg:\n", dbi->port);
        return DBI_MASTER_ERROR_TIMEOUT;
    }
    else if (SPI_XFER_FAILED == dbi->result)
    {
        DBI_ERR("[dbi%d] xfer failed...\n", dbi->port);
        DBI_INFO("[dbi%d] dump reg:\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    return DBI_MASTER_OK;
}


/*
 * < 64 : cpu ;  >= 64 : dma
 * wait for done completion in this function, wakup in the irq hanlder
 */
dbi_master_status_t hal_dbi_xfer(hal_dbi_master_port_t port,
        hal_dbi_master_transfer_t *transfer)
{
    uint32_t flags = 0;
    dbi_master_status_t ret = 0;
    sunxi_dbi_t *dbi = &g_sunxi_dbi[port];

    if (NULL == transfer)
    {
        DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
        ret = DBI_MASTER_INVALID_PARAMETER;
        goto end;
    }

    DBI_DBG("[dbi%d] tl=%lu rl=%lu, \n", dbi->port, transfer->tx_len,
             transfer->rx_len);

    if ((!transfer->tx_buf && !transfer->rx_buf) ||
        (!transfer->tx_len && !transfer->rx_buf))
    {
        DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
        ret = DBI_MASTER_INVALID_PARAMETER;
        goto end;
    }

    dbi->result = SPI_XFER_READY;
    dbi->transfer = transfer;

    dbi_disable_dma_irq(SPI_FIFO_CTL_DRQEN_MASK, dbi);
    dbi_reset_fifo(dbi);

    set_chip_select_control(port);
    set_master(dbi);

    if (is_master_mode(port)) {
	// Set master burst counter
	set_master_counter(port, transfer->tx_len);
    }

    if (dbi_transfer(dbi))
    {
        ret = DBI_MASTER_ERROR;
        goto end;
    }

    switch (dbi->mode_type)
    {
        case DBI_MODE_TYPE_RX:
        {
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            /* >64 use DMA transfer, or use cpu */
            if (dbi->transfer->rx_len > BULK_DATA_BOUNDARY)
            {
                if (dbi_dma_rx_complete(dbi))
                {
                    ret = DBI_MASTER_ERROR;
                    goto end;
                }
            }
            else
            {
#endif
                if (dbi_cpu_complete(dbi))
                {
                    ret = DBI_MASTER_ERROR;
                    goto end;
                }
                else
                {
                    ret = DBI_MASTER_OK;
                }
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            }
#endif
            break;
        }
        case DBI_MODE_TYPE_TX:
        {
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            /* >64 use DMA transfer, or use cpu */
            if (dbi->transfer->tx_len > BULK_DATA_BOUNDARY)
            {
                if (dbi_dma_tx_complete(dbi))
                {
                    ret = DBI_MASTER_ERROR;
                    goto end;
                }
            }
            else
            {
#endif
                if (dbi_cpu_complete(dbi))
                {
                    ret = DBI_MASTER_ERROR;
                    goto end;
                }
                else
                {
                    ret = DBI_MASTER_OK;
                }
#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
            }
#endif
            break;
        }
        default:
        {
            DBI_ERR("[dbi%d] invalid parameter\n", dbi->port);
            ret = DBI_MASTER_INVALID_PARAMETER;
        }
    }
end:
    return ret;
}
static int sunxi_get_spic_clk(sunxi_dbi_t *dbi)
{
    unsigned long rate;

    rate = hal_clk_get_rate(dbi->mclk);
    if (!rate)
        DBI_ERR("[dbi%d] clk get rate failed! return %ld\n", dbi->port, rate);
    return rate;
}
dbi_master_status_t hal_dbi_hw_config(hal_dbi_master_port_t port, hal_dbi_config_t *dbi_config)
{
    uint sclk_freq = 0;
    unsigned long clock_frequency;
    sunxi_dbi_t *dbi = &g_sunxi_dbi[port];
    struct sunxi_dbi_params_t *para;

    if (NULL == dbi_config)
    {
        DBI_ERR("[dbi%d] invalid parameter\n", port);
        return DBI_MASTER_INVALID_PARAMETER;
    }

    para = &g_sunxi_dbi_params[port];

    dbi->base = para->reg_base;
    dbi->port = port;
    dbi->mode_type = DBI_MODE_TYPE_NULL;

    if (dbi_config->clock_frequency)
        clock_frequency = dbi_config->clock_frequency;
    else
        clock_frequency = SPI_MOD_CLK;

    if (clock_frequency > SPI_MAX_FREQUENCY)
    {
        DBI_ERR("[dbi%d] invalid parameter! max_frequency is 100MHZ\n",
                dbi->port);
    }
    else
    {
        DBI_DBG("[dbi%d] clock_frequency = %ldHZ\n", dbi->port,
                 clock_frequency);
    }

    if (clock_frequency >= SPI_HIGH_FREQUENCY)
    {
        spi_sample_delay(0, 1, dbi);
    }
    else if (clock_frequency <= SPI_LOW_FREQUENCY)
    {
        spi_sample_delay(1, 0, dbi);
    }
    else
    {
        spi_sample_delay(0, 0, dbi);
    }


    dbi_soft_reset(dbi);

    sclk_freq = sunxi_get_spic_clk(dbi);
    if (!sclk_freq)
    {
        DBI_INFO("spi clk error ! \n");
    }
    else if (sclk_freq != clock_frequency)
    {
        dbi_clk_exit(dbi);
        if (dbi_clk_init(dbi, clock_frequency))
        {
            DBI_ERR("[dbi%d] init clk error\n", dbi->port);
            return DBI_MASTER_ERROR;
        }
    }
    /* reset fifo */
    dbi_reset_fifo(dbi);

    return DBI_MASTER_OK;
}

dbi_master_status_t hal_set_dbi_config(hal_dbi_master_port_t port,hal_dbi_config_t *cfg)
{
    sunxi_dbi_t *dbi = &g_sunxi_dbi[port];
    struct sunxi_dbi_params_t *para;
    if (port >= HAL_DBI_MASTER_MAX)
    {
        DBI_ERR("[dbi%d] invalid port\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    // base config, it can not be changed!
    para = &g_sunxi_dbi_params[port];
    dbi->base = para->reg_base;
    dbi->irqnum = para->irq_num;
    dbi->port = port;
    dbi->mode_type = DBI_MODE_TYPE_NULL;
    // user config
    memcpy(&(dbi->dbi_reg_cfg), cfg, sizeof(hal_dbi_config_t));
    dbi_dev_set_base(port, para->reg_base);
    if (hal_dbi_hw_config(port, cfg))
    {
        DBI_ERR("[dbi%d] hw config error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    if(hal_dbi_register_config(dbi))
    {
        DBI_ERR("[dbi%d] init register error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    return DBI_MASTER_OK;
}

dbi_master_status_t hal_dbi_init(hal_dbi_master_port_t port, hal_dbi_config_t *cfg)
{
    sunxi_dbi_t *dbi = &g_sunxi_dbi[port];
    struct sunxi_dbi_params_t *para;

    // print param
#if 0
    printf(
       "%u cmd_typ;               \n"
        "%u write_dump;            \n"
        "%u output_data_sequence;  \n"
        "%u rgb_seq;               \n"
        "%u transmit_mode;         \n"
        "%d dbi_output_data_format;\n"
        "%d dbi_interface;         \n"
        "%d dbi_source_format;     \n"
        "%u dump_value;            \n"
        "%u rgb_bit_order;         \n"
        "%u epos;                  \n"
        "%u rgb_src_typ;           \n"
        "%d dbi_en_mode;           \n"
        "%u rgb666_format;         \n"
        "%u dbi_rx_clk_inv;        \n"
        "%u dbi_output_clk_mode;   \n"
        "%u dbi_clk_output_inv;    \n"
        "%u dcx_data;              \n"
        "%u rgb16_pix0_post;       \n"
        "%u read_msb_or_lsb;       \n"
        "%u read_dump;             \n"
        "%u read_num;              \n"
        "%u ahb_ready_bypass;      \n"
        "%u sdi_out_select;        \n"
        "%u dcx_select;            \n"
        "%u sdi_pin_select;        \n"
        "%u te_deb_select;         \n"
        "%u te_edge_select;        \n"
        "%u te_en;                 \n"
        "%u v_size;                \n"
        "%u h_size;                \n"
        "%u clock_frequency;       \n"
        ,
        cfg->cmd_typ,
        cfg->write_dump,
        cfg->output_data_sequence,
        cfg->rgb_seq,
        cfg->transmit_mode,
        cfg->dbi_output_data_format,
        cfg->dbi_interface,
        cfg->dbi_source_format,
        cfg->dump_value,
        cfg->rgb_bit_order,
        cfg->epos,
        cfg->rgb_src_typ,
        cfg->dbi_en_mode,
        cfg->rgb666_format,
        cfg->dbi_rx_clk_inv,
        cfg->dbi_output_clk_mode,
        cfg->dbi_clk_output_inv,
        cfg->dcx_data,
        cfg->rgb16_pix0_post,
        cfg->read_msb_or_lsb,
        cfg->read_dump,
        cfg->read_num,
        cfg->ahb_ready_bypass,
        cfg->sdi_out_select,
        cfg->dcx_select,
        cfg->sdi_pin_select,
        cfg->te_deb_select,
        cfg->te_edge_select,
        cfg->te_en,
        cfg->v_size,
        cfg->h_size,
        cfg->clock_frequency);
#endif

    if (port >= HAL_DBI_MASTER_MAX)
    {
        DBI_ERR("[dbi%d] invalid port\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    // base config, it can not be changed!
    para = &g_sunxi_dbi_params[port];
    dbi->base = para->reg_base;
    dbi->irqnum = para->irq_num;
    dbi->port = port;
    dbi->mode_type = DBI_MODE_TYPE_NULL;

    // user config
    memcpy(&(dbi->dbi_reg_cfg), cfg, sizeof(hal_dbi_config_t));

    dbi_dev_set_base(port, para->reg_base);

    if (hal_request_irq(dbi->irqnum, dbi_irq_handler, "dbi-ctrl", dbi) < 0)
    {
        DBI_ERR("[dbi%d] request irq error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    hal_enable_irq(dbi->irqnum);
    if (dbi_pinctrl_init(dbi))
    {
        DBI_ERR("[dbi%d] init pinctrl error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    if (dbi_clk_init(dbi, SPI_MAX_FREQUENCY))
    {
        DBI_ERR("[dbi%d] init clk error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    dbi_soft_reset(dbi);


    if (hal_dbi_hw_config(port, cfg))
    {
        DBI_ERR("[dbi%d] hw config error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }

    if(hal_dbi_register_config(dbi))
	{
		DBI_ERR("[dbi%d] init register error\n", dbi->port);
        return DBI_MASTER_ERROR;
	}

#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
    dbi->xSemaphore_tx = hal_sem_create(0);
    if (dbi->xSemaphore_tx == NULL)
    {
        DBI_ERR("[dbi%d] creating semaphore_tx failed.\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    dbi->xSemaphore_rx = hal_sem_create(0);
    if (dbi->xSemaphore_rx == NULL)
    {
        DBI_ERR("[dbi%d] creating semaphore_rx failed.\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    dbi->align_dma_buf = dma_alloc_coherent(ALIGN_DMA_BUF_SIZE);
    if (!dbi->align_dma_buf)
    {
        DBI_ERR("[dbi%d] alloc memory failed\n", dbi->port);
        return DBI_MASTER_ERROR_NOMEM;
    }
    DBI_DBG("[dbi%d] DMA xfer enable\n", dbi->port);
#else
    DBI_DBG("[dbi%d] CPU xfer only\n", dbi->port);
#endif
    return DBI_MASTER_OK;
}

dbi_master_status_t hal_dbi_deinit(hal_dbi_master_port_t port)
{
    uint8_t i;
    sunxi_dbi_t *dbi = &g_sunxi_dbi[port];

    hal_disable_irq(dbi->irqnum);

#ifndef CONFIG_SUNXI_DBI_CPU_XFER_ONLY
    dma_free_coherent(dbi->align_dma_buf);
    hal_sem_delete(dbi->xSemaphore_tx);
    hal_sem_delete(dbi->xSemaphore_rx);
#endif
    if (dbi_clk_exit(dbi))
    {
        DBI_ERR("[dbi%d] exit clk error\n", dbi->port);
        return DBI_MASTER_ERROR;
    }
    hal_free_irq(dbi->irqnum);

    return DBI_MASTER_OK;
}

dbi_master_status_t hal_dbi_send_cmd(hal_dbi_master_port_t port, void *buf, u32 size)
{
    u32 reg_val = 0;
    dbi_master_status_t ret;
    hal_dbi_master_transfer_t tr;
    sunxi_dbi_t* dbi = &g_sunxi_dbi[port];

//    reg_val = hal_readl(dbi->base + DBI_CTRL_REG0);
//    reg_val &= ~DBI_CR_READ;/* Write command */
//    reg_val &= ~DBI_CR_TRANSMIT_MODE;/* 0:command/parameter	1:video */
//    hal_writel(reg_val,dbi->base + DBI_CTRL_REG0);

    dbi->mode_type = DBI_MODE_TYPE_TX;

    tr.tx_buf = buf;
    tr.tx_len = size;
    tr.rx_buf = NULL;
    tr.rx_len = 0;

    ret = hal_dbi_xfer(port, &tr);
    return ret;
}

dbi_master_status_t hal_dbi_send_para(hal_dbi_master_port_t port, void *buf, u32 size)
{
    u32 reg_val = 0;
    dbi_master_status_t ret;
    hal_dbi_master_transfer_t tr;
    sunxi_dbi_t* dbi = &g_sunxi_dbi[port];

    reg_val = hal_readl(dbi->base + DBI_CTRL_REG0);
    reg_val &= ~DBI_CR_READ;/* Write command */
    reg_val &= ~DBI_CR_TRANSMIT_MODE;/* 0:command/parameter	1:video */
    hal_writel(reg_val,dbi->base + DBI_CTRL_REG0);

    dbi->mode_type = DBI_MODE_TYPE_TX;

    tr.tx_buf = buf;
    tr.tx_len = size;
    tr.rx_buf = NULL;
    tr.rx_len = 0;

    ret = hal_dbi_xfer(port, &tr);
    return ret;
}
dbi_master_status_t hal_dbi_read(hal_dbi_master_port_t port,void *buf, uint32_t size)
{
    u32 reg_val = 0;
    dbi_master_status_t ret;
    hal_dbi_master_transfer_t tr;
    sunxi_dbi_t* dbi = &g_sunxi_dbi[port];

    reg_val = hal_readl(dbi->base + DBI_CTRL_REG0);
    reg_val |= DBI_CR_READ;/* Read command */
    reg_val &= ~DBI_CR_TRANSMIT_MODE;/* 0:command/parameter	1:video */
    hal_writel(reg_val,dbi->base + DBI_CTRL_REG0);

    dbi->mode_type = DBI_MODE_TYPE_RX;

    tr.tx_buf = NULL;
    tr.tx_len = 0;
    tr.rx_buf = buf;
    tr.rx_len = size;

    DBI_INFO("dbi[%d] read data,len is %ld \n", port, size);
    ret = hal_dbi_xfer(port, &tr);
    return ret;
}
dbi_master_status_t hal_dbi_send_line_pixel(hal_dbi_master_port_t port, const void *buf)
{
    u32 reg_val;
    dbi_master_status_t ret;
	hal_dbi_master_transfer_t tr;
    sunxi_dbi_t* dbi = &g_sunxi_dbi[port];
	memset(&tr, 0, sizeof(hal_dbi_master_transfer_t));
    u32 line_size = dbi->dbi_reg_cfg.h_size;

    reg_val=hal_readl(dbi->base + DBI_CTRL_REG0);
    reg_val &= ~DBI_CR_READ;/* Write command */
    reg_val |= DBI_CR_TRANSMIT_MODE;/* 0:command/parameter	1:video */
    hal_writel(reg_val, dbi->base + DBI_CTRL_REG0);

    reg_val=hal_readl(dbi->base + DBI_CTRL_REG1);
    reg_val |= DBI_CR1_DCX_DATA;/* 0:command	1:data */
    hal_writel(reg_val, dbi->base + DBI_CTRL_REG1);
    if(line_size < BULK_DATA_BOUNDARY)
	{
        dbi_enable_irq(DBI_INT_LINE_DONE_EN,dbi);/* enable line int */

		switch(dbi->dbi_reg_cfg.dbi_en_mode)
		{
			case DBI_MODE_ALLWAYS_ON:
				dbi_enable(dbi);
				break;
			case DBI_MODE_SOFTWARE_TRIGGER:
                dbi_enable_irq(DBI_TE_INT_EN,dbi);/* enable te int */
				break;
			case DBI_MODE_TIMER_TRIGGER:
                dbi_enable_irq(DBI_TIMER_INT_EN,dbi);/* enable timer int */

				reg_val = hal_readl(dbi->base + DBI_TIMER_REG);	/* dbi tmier value, clock source is SCLK */
				reg_val &= ~(0x7fffffff << 0);
				reg_val |= (0x38A40 << 0);			/* timer_value = SCLK/frame_rate - one_pixel_clock_cycle * one_frame_pixel_value */
				reg_val &= ~(0x1u << 31);
				reg_val |= (0x1u << 31);
				hal_writel(reg_val, dbi->base + DBI_TIMER_REG);
				break;
			case DBI_MODE_TE_TRIGGER:
                dbi_enable_irq(DBI_TE_INT_EN,dbi);/* enable te int */
				break;

		}
    }

    tr.tx_buf = buf;
    if(dbi->dbi_reg_cfg.rgb_src_typ & DBI_CR_VI_SRC_RGB16)
    {
        tr.tx_len = line_size*2;
    }
    else
    {
        tr.tx_len = line_size*4;
    }

	tr.rx_buf = NULL;
	tr.rx_len = 0;
	tr.dummy_byte = 0;
	dbi->mode_type = DBI_MODE_TYPE_TX;
	ret = hal_dbi_xfer(port, &tr);
    return ret;

}
