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

#include <errno.h>
#include <typedef.h>
#include <hal_mem.h>
#include <sunxi_hal_geth.h>
#include <lwip/pbuf.h>
#include <netif/ethernet.h>
#include <sunxi_hal_mii.h>
#include <sunxi_hal_miiphy.h>

static struct hal_geth_dma_desc_t *dma_desc_tx;
static struct hal_geth_dma_desc_t *dma_desc_rx;
static char *rx_packet;
static char *rx_handle_buf;

static uint32_t used_type = EXT_PHY;
static uint32_t phy_addr = 0x1f;

static struct rt_geth_device rt_geth_dev;

static uint32_t tx_delay = 0;
static uint32_t rx_delay = 0;

#if 0
static inline void dma_free_coherent(void *addr)
{
    void *malloc_ptr = NULL;
	
    if (!addr)
        return;
    malloc_ptr = *(uint32_t *)((uint32_t *)addr - 1);
    /* krhino_mm_free(malloc_ptr); */
    hal_free(malloc_ptr);
}

static inline void *dma_alloc_coherent(size_t size)
{
    void *fake_ptr = NULL;
    void *malloc_ptr = NULL;

    /* malloc_ptr = krhino_mm_alloc(size + 64); */
    malloc_ptr = hal_malloc(size + 64);
    if ((uint32_t)malloc_ptr & 0x3) {
        printf("error: krhino_mm_alloc not align to 4 byte\r\n");
    }
    fake_ptr = (uint32_t)(malloc_ptr + 64) & (~63);
    *(uint32_t *)((uint32_t *)fake_ptr - 1) = malloc_ptr;

    return fake_ptr;
}
#endif

static uint32_t geth_phy_read(const char *devname, int phy_addr, uint8_t reg)
{
	struct eth_device *dev;
	uint32_t value;

	//dev = eth_get_dev_by_name(devname);
	value = geth_mdio_read(rt_geth_dev.iobase, phy_addr, reg);
	
	return value;
}

static uint32_t geth_phy_write(const char *devname, int phy_addr, uint8_t reg, uint16_t data)
{
	struct eth_device *dev;

	//dev = eth_get_dev_by_name(devname);
	geth_mdio_write(rt_geth_dev.iobase, phy_addr, reg, data);

	return 0;
}

static rt_err_t rt_geth_xmit(rt_device_t dev, struct pbuf *p)
{
	rt_err_t ret = RT_ERROR;
	uint32_t value;
	uint32_t xmit_stat;
	uint32_t frame_len;
	hal_geth_dma_desc_t *tx_p = dma_desc_tx;
	int tmo = 0;

	/* Get transmit status */
	xmit_stat = hal_readl(rt_geth_dev.iobase + GETH_TX_DMA_STA) & 0x7;

	/* Wait the Prev packet compled and timeout flush it */
	while (tx_p->desc0.tx.own || (xmit_stat != 0x00
			&& xmit_stat != 0x06)) {
		if (tmo < 50) {
			usleep(100);
		} else {
			break;
		}
	}

	frame_len = p->len;
	/* Configure transmit dma descriptor */
	tx_p->desc0.all = 0x80000000;   /* Set Own */
	tx_p->desc1.all = 0x61000000;
#ifdef CONFIG_HARD_CHECKSUM
	tx_p->desc1.all |= (0x3 << 27); /* CIC Full */
#endif
	tx_p->desc1.all |= (((1 << 11) - 1) & frame_len);
	tx_p->desc2 = (uint32_t *)(p->payload);

	//flush_cache((long unsigned int)packet, length);
	/* flush Transmit FIFO */
	geth_flush_tx(rt_geth_dev.iobase);

	/* Enable transmit and Poll transmit */
	geth_start_tx(rt_geth_dev.iobase);
	geth_tx_poll(rt_geth_dev.iobase);

	return ret;
}

static int rx_status(hal_geth_dma_desc_t *p)
{
	int ret = good_frame;

	if (p->desc0.rx.last_desc == 0)
		ret = discard_frame;

	if (p->desc0.rx.frm_type && (p->desc0.rx.chsum_err
			|| p->desc0.rx.ipch_err))
		ret = discard_frame;

	if (p->desc0.rx.err_sum)
		ret = discard_frame;

	if (p->desc0.rx.len_err)
		ret = discard_frame;

	if (p->desc0.rx.mii_err)
		ret = discard_frame;

	return ret;
}

static struct pbuf *rt_geth_recv(rt_device_t dev)
{
	uint32_t len, value, recv_stat;
	struct pbuf *p, *q;
	hal_geth_dma_desc_t *rx_p = (void *)dma_desc_rx;

	if (rx_p->desc0.rx.own)
		return RT_NULL;

	/*
	 * FIXME: to prevent the Rx buffer that we are handling
	 * from being overwrited.
	 */
	memcpy(rx_handle_buf, rx_packet, 2048);

	recv_stat = hal_readl(rt_geth_dev.iobase + GETH_INT_STA);
	if (!(recv_stat & 0x2300))
		goto fill;

	hal_writel(0x3F00, rt_geth_dev.iobase + GETH_INT_STA);

	recv_stat = rx_status(rx_p);
	if (recv_stat != discard_frame) {
		if (recv_stat != llc_snap)
			len = (rx_p->desc0.rx.frm_len - 4);
		else
			len = rx_p->desc0.rx.frm_len;

		//pkt_hex_dump("RX", (void *)rx_packet, 64);
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p != NULL) {
			p->payload = (void *)rx_packet;
			p->len = len;
		}
		//flush_cache((long unsigned int)rx_packet, 2048);
		//ethernet_input(p, netif);
		return p;
	} else {
		/* Just need to clear 64 bits header */
		memset(rx_packet, 0, 64);
	}

fill:
	rx_p->desc0.all = 0x80000000;
	rx_p->desc1.all = 0x81000000;
	rx_p->desc1.all |= ((1 << 11) - 1);
	rx_p->desc2 = (void *)rx_packet;

	recv_stat = hal_readl(rt_geth_dev.iobase + GETH_RX_DMA_STA) & 0x07;
	/* Enable receive and poll it */
	value = hal_readl(rt_geth_dev.iobase + GETH_RX_CTL1);
	if (recv_stat == 0x00)
		value |= 0x40000000;
	else
		value |= 0x80000000;
	hal_writel(value, rt_geth_dev.iobase + GETH_RX_CTL1);

	return RT_NULL;
}

static void geth_set_hwaddr(unsigned char *addr)
{
	geth_set_mac_addr(rt_geth_dev.iobase, addr, 0);
}

static void geth_get_hwaddr(struct eth_device *dev)
{	
}

static void geth_pinctrl_init(void)
{
	drv_gpio_pinmux_set_function(DRV_GPIO_PD1, 5); 
	drv_gpio_pinmux_set_function(DRV_GPIO_PD2, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD3, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD4, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD5, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD6, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD7, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD8, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD18, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD20, 5);
	drv_gpio_pinmux_set_function(DRV_GPIO_PD21, 5);
}

static int geth_phy_init(struct eth_device *dev)
{
	uint32_t value;
	uint16_t phy_val;
	int i;

	for (i = 0; i < 0x1f; i++) {
		value = (geth_phy_read(dev, i, MII_PHYSID1)
							& 0xffff) << 16;
		value |= (geth_phy_read(dev, i, MII_PHYSID2) & 0xffff);

		if ((value & 0x1fffffff) == 0x1fffffff) {
			usleep(10000);
			continue;
		}

		phy_addr = i;
		break;
	}

	if (phy_addr == 0x1f) {
		GETH_ERR("No PHY device!\n");
		return -1;
	}
	
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val | BMCR_RESET);
	while (geth_phy_read(dev, phy_addr, MII_BMCR) & BMCR_RESET)
		;

	/* Reset phy chip */
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	geth_phy_write(dev, phy_addr, MII_BMCR, (phy_val & ~BMCR_PDOWN));
	while (geth_phy_read(dev, phy_addr, MII_BMCR) & BMCR_PDOWN)
		;

	/* Wait BMSR_ANEGCOMPLETE be set */
	while (!(geth_phy_read(dev, phy_addr, MII_BMSR) & BMSR_ANEGCOMPLETE)) {
		if (i > 4) {
			printf("Warning: Auto negotiation timeout!\n");
			break;
		}
		usleep(500*1000);
		i++;
	}

#ifdef DISABLE_AUTONEG
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	phy_val &= ~BMCR_ANENABLE;
	phy_val &= ~BMCR_SPEED1000;
	phy_val |= BMCR_SPEED100;
	phy_val |= BMCR_FULLDPLX;
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val);
#endif

	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	value = hal_readl(rt_geth_dev.iobase + GETH_BASIC_CTL0);
	if (phy_val & BMCR_FULLDPLX)
		value |= 0x01;
	else
		value &= ~0x01;

	/* Default is 1000Mbps */
	value &= ~0xc;
	if (phy_val & BMCR_SPEED100)
		value |= 0x0c;
	else if (!(phy_val & BMCR_SPEED1000))
		value |= 0x08;
	hal_writel(value, rt_geth_dev.iobase + GETH_BASIC_CTL0);

#if 0
	/* PHY layer Loopback */
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	phy_val |= BMCR_LOOPBACK;
	geth_phy_write(dev, phy_addr, MII_BMCR, phy_val);
	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	printf("REG0: %08x\n", phy_val);

	/* MAC layer Loopback */
	value = hal_readl(rt_geth_dev.iobase + GETH_BASIC_CTL0);
	value |= 0x02;
	hal_writel(reg_val, rt_geth_dev.iobase + GETH_BASIC_CTL0);
#endif

	phy_val = geth_phy_read(dev, phy_addr, MII_BMCR);
	printf("%s   Speed: %dMbps, Mode: %s, Loopback: %s\n",
				"eth0", ((phy_val & BMCR_SPEED1000) ? 1000 :
				((phy_val & BMCR_SPEED100) ? 100 : 10)),
				(phy_val & BMCR_FULLDPLX) ? "Full duplex" : "Half duplex",
				(phy_val & BMCR_LOOPBACK) ? "YES" : "NO");

	return 0;
}

static int geth_dma_desc_init(void)
{
	/* Initialize dma descriptor */
	dma_desc_tx = (struct hal_geth_dma_desc_t *)hal_malloc(sizeof(struct hal_geth_dma_desc_t));
    if (!dma_desc_tx) {
		GETH_ERR("alloc tx dma desc fail!\n");
		goto err;
	}

	dma_desc_rx = (struct hal_geth_dma_desc_t *)hal_malloc(sizeof(struct hal_geth_dma_desc_t));
    if (!dma_desc_rx) {
		GETH_ERR("alloc rx dma desc fail!\n");
		goto err;
	}

	rx_packet = hal_malloc(2048);
	if (!rx_packet) {
		GETH_ERR("alloc rx packet buffer fail!\n");
		goto err;
	}

	rx_handle_buf = hal_malloc(2048);
	if (!rx_handle_buf) {
		GETH_ERR("alloc rx handle buffer fail!\n");
		goto err;
	}
	
	return 0;
	
err:
	free(rx_handle_buf);
	free(rx_packet);
	free(dma_desc_rx);
	free(dma_desc_tx);
	
	return -ENOMEM;
}

static void geth_dma_desc_free(void)
{
	free(rx_handle_buf);
	free(rx_packet);
	free(dma_desc_rx);
	free(dma_desc_tx);
}

static int rt_geth_init(rt_device_t dev)
{
	uint32_t value;
	
	/* Enable clock */
	geth_clk_enable();
	
	/* Pinctrl init */
	geth_pinctrl_init();
	
	/* Init phy */
	geth_phy_init(dev);

	/* MAC controller soft reset */
	value = geth_mac_reset(rt_geth_dev.iobase);
	if (!value)
		GETH_INFO("Gmac controller soft reset success\n");

	/* MAC controller initialize */
	geth_mac_init(rt_geth_dev.iobase);
	/* Frame filter */
	geth_set_filter(rt_geth_dev.iobase);
	/* Burst should be 8 */
	value = hal_readl(rt_geth_dev.iobase + GETH_BASIC_CTL1);
	value |= (8 << 24);
	hal_writel(value, rt_geth_dev.iobase + GETH_BASIC_CTL1);

	/* Seth hardware address */

	/* Disable all interrupt of dma */
	geth_int_disable(rt_geth_dev.iobase);

	value = geth_dma_desc_init();
	if(value < 0) {
		GETH_ERR("Gmac dma desc init fail!\n");
		return -1;
	}
	
	memset((void *)dma_desc_tx, 0, sizeof(hal_geth_dma_desc_t));
	memset((void *)dma_desc_rx, 0, sizeof(hal_geth_dma_desc_t));

	dma_desc_tx->desc3 = (uint32_t *)dma_desc_tx;
	dma_desc_rx->desc3 = (uint32_t *)dma_desc_rx;

	hal_writel((uint32_t)dma_desc_tx, rt_geth_dev.iobase + GETH_TX_DESC_LIST);
	hal_writel((uint32_t)dma_desc_rx, rt_geth_dev.iobase + GETH_RX_DESC_LIST);
	
	/* start tx & rx */
	geth_start_tx(rt_geth_dev.iobase);
	geth_start_rx(rt_geth_dev.iobase);
	
	/* Enable transmit & receive */
	geth_mac_enable(rt_geth_dev.iobase);

	return 0;
}

static void rt_geth_uninitialize(rt_device_t dev)
{
	geth_mac_disable(rt_geth_dev.iobase);
	geth_clk_disable();
}

static rt_err_t rt_geth_open(rt_device_t dev, rt_uint16_t oflag)
{
	GETH_INFO("gmac open\n");
	return RT_EOK;
}

static rt_err_t rt_geth_close(rt_device_t dev)
{
	GETH_INFO("gmac close\n");
	return RT_EOK;
}

static rt_err_t rt_geth_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
	GETH_INFO("gmac read\n");
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static rt_err_t rt_geth_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
	GETH_INFO("gmac write\n");
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static rt_err_t rt_geth_control(rt_device_t dev, int cmd, void *args)
{
	switch(cmd)
	{
	case NIOCTL_GADDR:
		/* get mac address */
		if(args) {
			rt_memcpy(args, rt_geth_dev.dev_addr, 6);
		} else {
			return -RT_ERROR;
		}
		break;

	default :
		break;
	}

	return RT_EOK;
}

const sunxi_hal_driver_geth_t sunxi_hal_geth_driver =
{
	.initialize = rt_geth_init,
	.uninitialize = rt_geth_uninitialize,
	.send = rt_geth_xmit,
	.recv = rt_geth_recv,
};

void rt_geth_driver_init(void)
{
	rt_err_t state = RT_EOK;
	
	rt_geth_dev.iobase = IOBASE;
	rt_geth_dev.phy_interface = PHY_INTERFACE_MODE_MII;
	random_ether_addr(rt_geth_dev.dev_addr);
	
	rt_geth_dev.parent.parent.init = rt_geth_init;
	rt_geth_dev.parent.parent.open = rt_geth_open;
	rt_geth_dev.parent.parent.close = rt_geth_close;
	rt_geth_dev.parent.parent.read = rt_geth_read;
	rt_geth_dev.parent.parent.write = rt_geth_write;
	rt_geth_dev.parent.parent.control = rt_geth_control;
	rt_geth_dev.parent.parent.user_data = RT_NULL;
	
	rt_geth_dev.parent.eth_rx = rt_geth_recv;
	rt_geth_dev.parent.eth_tx = rt_geth_xmit;
	
	/* register eth device */
	state = eth_device_init(&(rt_geth_dev.parent), "e0");
	if (RT_EOK == state) {
		GETH_INFO("gmac device init success\n");
	} else {
		GETH_ERR("gmac device init failed: %d\n", state);
	}

	miiphy_register("e0", geth_phy_read, geth_phy_write);

	return 0;
}
