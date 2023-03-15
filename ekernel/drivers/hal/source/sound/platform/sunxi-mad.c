/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <semphr.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <hal_timer.h>
//#ifdef CONFIG_COMPONENTS_PM
#include <pm_task.h>
#include <pm_wakesrc.h>
#include <pm_rpcfunc.h>
#include <pm_devops.h>
#include <pm_wakecnt.h>
//#endif
#include "irqs.h"
#include "sunxi-mad.h"

#ifdef CONFIG_SND_SUNXI_MAD_DEBUG
QueueHandle_t mad_sleep;
void sunxi_mad_schd_wakeup(SemaphoreHandle_t semaphore);
#endif

/****************************************************************************/
struct sunxi_mad_sram_size {
	unsigned int src_chan_num;
	unsigned int standby_chan_num; /* no used */
	unsigned int size;
	unsigned int sram_store_th;
	/* align dma block size */
	unsigned int ahb1_rx_th;
	unsigned int ahb1_tx_th;
};

/*
 * for alignment data when overflow.
 * Tips: sram dma width is 32bit.
 */
#if 0
static const struct sunxi_mad_sram_size mad_sram_size[] = {
	{1, 1, 128, 64, 64, 64},
	{2, 2, 128, 64, 64, 64},
	{3, 3, 120, 60, 60, 60},	/* 120 % (3 * 2 * 2) == 0 */
	{4, 4, 128, 64, 64, 64},	/* 128 % (4 * 2) == 0 */
	{5, 5, 120, 60, 60, 60},	/* 120 % (5 * 2 * 2) == 0 */
	{6, 6, 120, 60, 60, 60},	/* 120 % (6 * 2) == 0 */
	{7, 7, 112, 56, 56, 56},	/* 112 % (7 * 2 * 2) == 0 */
	{8, 8, 128, 64, 64, 64},	/* 128 % (8 * 2) == 0 */
};
#else
static const struct sunxi_mad_sram_size mad_sram_size[] = {
	{1, 1, 64, 32, 32, 32},
	{2, 2, 64, 32, 32, 32},
	{3, 3, 60, 30, 30, 30},	/* 120 % (3 * 2 * 2) == 0 */
	{4, 4, 64, 32, 32, 32},	/* 128 % (4 * 2) == 0 */
	{5, 5, 60, 30, 30, 30},	/* 120 % (5 * 2 * 2) == 0 */
	{6, 6, 60, 30, 30, 30},	/* 120 % (6 * 2) == 0 */
	{7, 7, 56, 28, 28, 28},	/* 112 % (7 * 2 * 2) == 0 */
	{8, 8, 64, 32, 32, 32},	/* 128 % (8 * 2) == 0 */
};
#endif

/****************************************************************************/
/*
 * UNIT: audio frames
 * defalut_val:[0x5]
 */
static int sram_wake_back_da_val = 0x5;

/****************************************************************************/
/* lpsd ad sync frame */
static int lpsd_ad_sync = 0x20;

/* lpsd th(0x0-0xFFFF) */
static int lpsd_th = 0x4b0;

/* speed deep on lpsd clk(0x0-0xFF) */
static int lpsd_rrun = 0x91;

/* speed deep on lpsd clk(0x0-0xFF) */
static int lpsd_rstop = 0xaa;

/* frames(0x0-0xFFFF) */
static int lpsd_ecnt = 2;//default: 0x32;

/****************************************************************************/

struct sunxi_mad_info *sunxi_mad;
static hal_spinlock_t sunxi_mad_lock;

unsigned int snd_mad_read(struct sunxi_mad_info *mad_info, unsigned int reg)
{
	unsigned int val = 0;
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&sunxi_mad_lock);
	val = snd_readl(mad_info->mem_base + reg);
	hal_spin_unlock_irqrestore(&sunxi_mad_lock, __cpsr);
	snd_info("reg=0x%x, val=0x%x\n", reg, val);
	return val;
}

int snd_mad_write(struct sunxi_mad_info *mad_info, unsigned int reg, unsigned int val)
{
	int ret = 0;
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&sunxi_mad_lock);
	ret = snd_writel(val, mad_info->mem_base + reg);
	hal_spin_unlock_irqrestore(&sunxi_mad_lock, __cpsr);
	snd_info("reg=0x%x, val=0x%x\n", reg, val);
	return ret;
}

int snd_mad_update_bits(struct sunxi_mad_info *mad_info, unsigned int reg,
				unsigned int mask, unsigned int value)
{
	int ret = 0;
	unsigned int old, new;
	bool change;
	uint32_t __cpsr;

	__cpsr = hal_spin_lock_irqsave(&sunxi_mad_lock);
	old = snd_readl(mad_info->mem_base + reg);
	new = (old & ~mask) | (value & mask);
	change = old != new;
	if (change)
		ret = snd_writel(new, mad_info->mem_base + reg);
	hal_spin_unlock_irqrestore(&sunxi_mad_lock, __cpsr);
	snd_info("reg=0x%x, mask=0x%x, val=0x%x, change=%u\n", reg, mask, value, change);
	return ret;
}

/****************************************************************************/
int sunxi_mad_bind_set(enum mad_path_sel path_sel, bool enable)
{
	switch (path_sel) {
	case MAD_PATH_CODEC:
	case MAD_PATH_CODECADC:
		sunxi_mad->mad_bind.mad_bind_codecadc = enable;
	break;
	case MAD_PATH_DMIC:
		sunxi_mad->mad_bind.mad_bind_dmic = enable;
	break;
	case MAD_PATH_I2S0:
		sunxi_mad->mad_bind.mad_bind_i2s0 = enable;
	break;
	case MAD_PATH_I2S1:
		sunxi_mad->mad_bind.mad_bind_i2s1 = enable;
	break;
	case MAD_PATH_I2S2:
		sunxi_mad->mad_bind.mad_bind_i2s2 = enable;
	break;
	case MAD_PATH_NONE:
	default:
		return -EINVAL;
	break;
	}

	return 0;
}

int sunxi_mad_bind_get(enum mad_path_sel path_sel, bool *enable)
{
	bool val;

	switch (path_sel) {
	case MAD_PATH_CODEC:
	case MAD_PATH_CODECADC:
		val = sunxi_mad->mad_bind.mad_bind_codecadc;
	break;
	case MAD_PATH_DMIC:
		val = sunxi_mad->mad_bind.mad_bind_dmic;
	break;
	case MAD_PATH_I2S0:
		val = sunxi_mad->mad_bind.mad_bind_i2s0;
	break;
	case MAD_PATH_I2S1:
		val = sunxi_mad->mad_bind.mad_bind_i2s1;
	break;
	case MAD_PATH_I2S2:
		val = sunxi_mad->mad_bind.mad_bind_i2s2;
	break;
	case MAD_PATH_NONE:
	default:
		*enable = false;
		return -EINVAL;
	break;
	}

	*enable = val;
	return 0;
}

void sunxi_mad_lpsd_init(void)
{
	snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_WAKE_BACK_DATA,
		sram_wake_back_da_val);

	snd_mad_write(sunxi_mad, SUNXI_MAD_LPSD_AD_SYNC_FC, lpsd_ad_sync);

	/*enable lpsd DC offset*/
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_LPSD_CH_MASK,
			0x1 << MAD_LPSD_DCBLOCK_EN,
			0x1 << MAD_LPSD_DCBLOCK_EN);

	snd_mad_write(sunxi_mad, SUNXI_MAD_LPSD_TH, lpsd_th);

	snd_mad_write(sunxi_mad, SUNXI_MAD_LPSD_RRUN, lpsd_rrun);

	snd_mad_write(sunxi_mad, SUNXI_MAD_LPSD_RSTOP, lpsd_rstop);

	snd_mad_write(sunxi_mad, SUNXI_MAD_LPSD_ECNT, lpsd_ecnt);
}

void sunxi_sram_ahb1_threshole_init(void)
{
	int i = 0;

	/*config sunxi_mad_ahb1_rx_th_reg*/
	for (i = 0; i < ARRAY_SIZE(mad_sram_size); i++) {
		if (mad_sram_size[i].src_chan_num ==
				sunxi_mad->mad_priv.audio_src_chan_num) {
			snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_AHB1_RX_TH,
				mad_sram_size[i].ahb1_rx_th);
			snd_print("MAD_SRAM_AHB1_RX_TH:%dkB\n",
					mad_sram_size[i].ahb1_rx_th);

			snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_AHB1_TX_TH,
				mad_sram_size[i].ahb1_tx_th);
			snd_print("MAD_SRAM_AHB1_TX_TH:%dkB\n",
					mad_sram_size[i].ahb1_tx_th);
			break;
		}
	}
}

void sunxi_mad_sram_set_reset_bit(void)
{
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			1 << SRAM_RST, 1 << SRAM_RST);
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			1 << SRAM_RST, 0 << SRAM_RST);
}

int sunxi_mad_sram_set_reset_flag(enum sunxi_mad_sram_reset_flag reset_flag)
{
	sunxi_mad->sram_reset_flag = reset_flag;

	return 0;
}

enum sunxi_mad_sram_reset_flag sunxi_mad_sram_get_reset_flag(void)
{
	return sunxi_mad->sram_reset_flag;
}

void sunxi_mad_sram_init(void)
{
	int i = 0;

	snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_POINT, 0x00);

	for (i = 0; i < ARRAY_SIZE(mad_sram_size); i++) {
		if (mad_sram_size[i].src_chan_num ==
				sunxi_mad->mad_priv.audio_src_chan_num) {
			snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_SIZE,
				mad_sram_size[i].size);
			snd_print("MAD_SRAM_SIZE:%dkB\n", mad_sram_size[i].size);

			snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_STORE_TH,
					mad_sram_size[i].sram_store_th);
			snd_print("MAD_SRAM_STORE_TH:%dkB\n",
					mad_sram_size[i].sram_store_th);
			break;
		}
	}

	/*config sunxi_mad_sram_sec_region_reg, non-sec*/
	snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_SEC_REGION_REG, 0x0);

	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			1 << SRAM_RST, 1 << SRAM_RST);
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			1 << SRAM_RST, 0 << SRAM_RST);

	sunxi_mad_sram_set_reset_flag(SUNXI_MAD_SRAM_RESET_IDLE);
}

struct sunxi_mad_info *sunxi_mad_get_mad_info(void)
{
	return sunxi_mad;
}

void sunxi_mad_clk_enable(bool val)
{
	if (val)
		hal_clock_enable(sunxi_mad->clk.mad_clk);
	else
		hal_clock_disable(sunxi_mad->clk.mad_clk);
}

void sunxi_mad_ad_clk_enable(bool val)
{
	if (val)
		hal_clock_enable(sunxi_mad->clk.mad_ad_clk);
	else
		hal_clock_disable(sunxi_mad->clk.mad_ad_clk);
}

void sunxi_mad_cfg_clk_enable(bool val)
{
	if (val)
		hal_clock_enable(sunxi_mad->clk.mad_cfg_clk);
	else
		hal_clock_disable(sunxi_mad->clk.mad_cfg_clk);
}

void sunxi_lpsd_clk_enable(bool val)
{
	if (val)
		hal_clock_enable(sunxi_mad->clk.lpsd_clk);
	else
		hal_clock_disable(sunxi_mad->clk.lpsd_clk);
}

void sunxi_mad_module_clk_enable(bool val)
{
	if (val) {
		hal_clock_enable(sunxi_mad->clk.mad_clk);
		hal_clock_enable(sunxi_mad->clk.mad_ad_clk);
		hal_clock_enable(sunxi_mad->clk.mad_cfg_clk);
	} else {
		hal_clock_disable(sunxi_mad->clk.mad_cfg_clk);
		hal_clock_disable(sunxi_mad->clk.mad_ad_clk);
		hal_clock_disable(sunxi_mad->clk.mad_clk);
	}
}

void sunxi_mad_standby_chan_sel(unsigned int num)
{
	snd_print("standby_chan_sel: %d\n", num);
	sunxi_mad->mad_priv.standby_chan_sel = num;
}

void sunxi_lpsd_chan_sel(unsigned int num)
{
	snd_print("lpsd_chan_sel: %d\n", num);
	sunxi_mad->mad_priv.lpsd_chan_sel = num;
}

/*
 * should be called before the sunxi_mad_sram_init.
 */
void sunxi_mad_audio_src_chan_num(unsigned int num)
{
	snd_print("audio_src_chan_num:%d\n", num);
	sunxi_mad->mad_priv.audio_src_chan_num = num;
}

#if 0
static void sunxi_mad_int_info_show(void)
{
	unsigned int val = 0;

	val = snd_mad_read(sunxi_mad, SUNXI_MAD_STA);
	snd_err("SUNXI_MAD_STA:0x%x\n", val);
	if (((val >> MAD_STATE) & 0xF) == 0)
		snd_err("MAD_STATE: ---> IDLE\n");
	else if ((val >> MAD_STATE) & 0x1)
		snd_err("MAD_STATE: ---> WAIT\n");
	else if ((val >> (MAD_STATE + 1)) & 0x1)
		snd_err("MAD_STATE: ---> RUN\n");
	else if ((val >> (MAD_STATE + 2)) & 0x1)
		snd_err("MAD_STATE: ---> NORMAL\n");

	val = snd_mad_read(sunxi_mad, SUNXI_MAD_DEBUG);
	snd_err("SUNXI_MAD_DEBUG:0x%x\n", val);
	snd_mad_write(sunxi_mad, SUNXI_MAD_DEBUG, 0x7F);
}
#endif

void sunxi_mad_set_lpsd_req(bool enable)
{
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
		0x1 << MAD_REQ_INT_MASK, enable << MAD_REQ_INT_MASK);
}

void sunxi_mad_set_data_req(bool enable)
{
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
		0x1 << DATA_REQ_INT_MASK, enable << DATA_REQ_INT_MASK);
}

void sunxi_lpsd_int_stat_clr(void)
{
	unsigned int val = 0;
	/* must clear the wake req flag */
	val = snd_mad_read(sunxi_mad, SUNXI_MAD_INT_ST_CLR);
	if (val & (0x1 << WAKE_INT)) {
		val &= ~(0x1 << DATA_REQ_INT);
		val |= (0x1 << WAKE_INT);
		snd_mad_write(sunxi_mad, SUNXI_MAD_INT_ST_CLR, val);
	}
}

void sunxi_mad_sram_chan_params(unsigned int mad_channels)
{
	unsigned int reg_val = 0;

	snd_print("mad_channels:%d\n", mad_channels);
	reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_SRAM_CH_MASK);
	/*config mad sram receive audio channel num*/
	reg_val &= ~(0x1f << MAD_SRAM_CH_NUM);
	reg_val |= mad_channels << MAD_SRAM_CH_NUM;

	/* open mad sram receive channels */
	reg_val &= ~(0xffff << MAD_SRAM_CH_MASK);
	reg_val |= ((1 << mad_channels) - 1) << MAD_SRAM_CH_MASK;

	snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_CH_MASK, reg_val);
}

static int sunxi_mad_sram_chan_com_config(unsigned int audio_src_chan_num,
				unsigned int mad_standby_chan_sel, bool enable)
{
	unsigned int chan_ch = 0;
	unsigned int mad_sram_chan_num = 0;
	unsigned int temp_val = 0;

	/*transfer to mad_standby channels*/
	switch (mad_standby_chan_sel) {
	case 0:
		mad_sram_chan_num = audio_src_chan_num;
		break;
	case 1:
		mad_sram_chan_num = 2;
		break;
	case 2:
		mad_sram_chan_num = 3;
		break;
	case 3:
		mad_sram_chan_num = 4;
		break;
	default:
		mad_sram_chan_num = 2;
		break;
	}

	/* Read data at start */
	temp_val = snd_mad_read(sunxi_mad, SUNXI_MAD_SRAM_CH_MASK);
	/* mad sram receive channels */
	temp_val &= ~(0x1F << MAD_SRAM_CH_NUM);
	temp_val &= ~(0xFFFF << MAD_SRAM_CH_MASK);
	if (enable) {
		temp_val |= audio_src_chan_num << MAD_SRAM_CH_NUM;
		temp_val |= ((1 << audio_src_chan_num) - 1) << MAD_SRAM_CH_MASK;
	} else {
		temp_val |= mad_sram_chan_num << MAD_SRAM_CH_NUM;
		temp_val |= ((1 << mad_sram_chan_num) - 1) << MAD_SRAM_CH_MASK;
	}

	/* config mad_sram channel change */
	if (mad_standby_chan_sel == 0) {
		chan_ch = MAD_CH_COM_NON;
	} else if (mad_standby_chan_sel == 1) {
		switch (audio_src_chan_num) {
		case 2:
			chan_ch = MAD_CH_COM_NON;
			break;
		case 4:
			chan_ch = MAD_CH_COM_2CH_TO_4CH;
			break;
		case 6:
			chan_ch = MAD_CH_COM_2CH_TO_6CH;
			break;
		case 8:
			chan_ch = MAD_CH_COM_2CH_TO_8CH;
			break;
		default:
			snd_err("unsupported mad_sram channels!\n");
			return -EINVAL;
		}
	} else if (mad_standby_chan_sel == 2) {
		chan_ch = MAD_CH_COM_NON;
	} else if (mad_standby_chan_sel == 3) {
		switch (audio_src_chan_num) {
		case 4:
			chan_ch = MAD_CH_COM_NON;
			break;
		case 6:
			chan_ch = MAD_CH_COM_4CH_TO_6CH;
			break;
		case 8:
			chan_ch = MAD_CH_COM_4CH_TO_8CH;
			break;
		default:
			snd_err("unsupported mad_sram channels!\n");
			return -EINVAL;
		}
	} else {
		snd_err("mad_standby channels isn't set up!\n");
		return -EINVAL;
	}
	temp_val &= ~(0xF << MAD_CH_COM_NUM);
	temp_val |= chan_ch << MAD_CH_COM_NUM;

	/*
	 * Enable mad_sram channel CHANGE_EN
	 * when DMA interpolation process finish, the CHANGE_EN bit will be set
	 * to 0 automaticallly.
	 */
	if (enable && (chan_ch != MAD_CH_COM_NON))
		temp_val |= 0x1 << MAD_CH_CHANGE_EN;
	else
		temp_val &= ~(0x1 << MAD_CH_CHANGE_EN);

	/* Write the value Once! */
	if (chan_ch != MAD_CH_COM_NON)
		snd_mad_write(sunxi_mad, SUNXI_MAD_SRAM_CH_MASK, temp_val);

	temp_val = 0;
	temp_val = snd_mad_read(sunxi_mad, SUNXI_MAD_SRAM_PRE_DSIZE);
	snd_print("SUNXI_MAD_SRAM_PRE_DSIZE:0x%x\n", temp_val);

	return 0;
}

static void sunxi_mad_lpsd_chan_enable(unsigned int lpsd_chan_sel, bool enable)
{
	unsigned int reg_val = 0;

	reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_LPSD_CH_MASK);

	reg_val &= ~(0xFFFF << MAD_LPSD_CH_MASK);
	reg_val &= ~(0x1 << MAD_LPSD_CH_NUM);
	if (enable) {
		/*transfer to mad_standby lpsd sel channels*/
		reg_val |= 1 << lpsd_chan_sel;
		/*config LPSD receive audio channel num: 1 channel*/
		reg_val |= 0x1 << MAD_LPSD_CH_NUM;
	}

	/*
	 * Tips:
	 *	The lpsd chan num and the channel mask should be setup
	 * at the same time.
	 */
	snd_mad_write(sunxi_mad, SUNXI_MAD_LPSD_CH_MASK, reg_val);
}

static void sunxi_mad_irq_status_clear(struct sunxi_mad_info *sunxi_mad)
{
	unsigned int reg_val = 0;

	for (;;) {
		reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_INT_ST_CLR);
		if (reg_val & (0x1 << DATA_REQ_INT)) {
			/* clear data int state */
			reg_val &= ~(0x1 << WAKE_INT);
			reg_val |= (0x1 << DATA_REQ_INT);
			snd_mad_write(sunxi_mad, SUNXI_MAD_INT_ST_CLR, reg_val);
		} else
			break;
	}
}

static void sunxi_lpsd_irq_status_clear(struct sunxi_mad_info *sunxi_mad)
{
	unsigned int reg_val = 0;

	for (;;) {
		reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_INT_ST_CLR);
		if (reg_val & (0x1 << WAKE_INT)) {
			/* clear data int state */
			reg_val &= ~(0x1 << DATA_REQ_INT);
			reg_val |= (0x1 << WAKE_INT);
			snd_mad_write(sunxi_mad, SUNXI_MAD_INT_ST_CLR, reg_val);
		} else
			break;
	}
}

static hal_irqreturn_t sunxi_lpsd_irq_handle(void *data)
{
	struct sunxi_mad_info *sunxi_mad = data;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	unsigned int pvItemToQueue = 0;
	int ret = 0;

	snd_print("Start.\n");
	sunxi_mad->sram_rd_point = snd_mad_read(sunxi_mad, SUNXI_MAD_SRAM_RD_POINT);

	sunxi_mad->wakeup_flag |= SUNXI_MAD_WAKEUP_LPSD_IRQ;

	/* disable the wake req */
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
			0x1 << MAD_REQ_INT_MASK, 0x0 << MAD_REQ_INT_MASK);

	sunxi_lpsd_irq_status_clear(sunxi_mad);

#if 0
	snd_err("\n");
	/* FIXME:not use for wakeup */
	if (!(sunxi_mad->wakeup_flag & SUNXI_MAD_WAKEUP_USE)) {
		sunxi_lpsd_irq_status_clear(sunxi_mad);
		snd_err("SUNXI_MAD_WAKEUP_USE is off.\n");
		return;
	}
#endif

	if (sunxi_mad->status != SUNXI_MAD_SUSPEND) {
		sunxi_lpsd_irq_status_clear(sunxi_mad);
		snd_err("device was not be suspended!\n");
		return HAL_IRQ_ERR;
	}

//	if (device_may_wakeup(sunxi_mad->dev)) {
		/*
		 * FIXME: wakeup interrupt when suspending.
		 * the app should setup cmd example:
		 * cat /sys/power/wakeup_count
		 * echo xxx > /sys/power/wakeup_count
		 */
//		pm_stay_awake(sunxi_mad->dev);
//		pm_wakeup_event(sunxi_mad->dev, 0);
//		pm_relax(sunxi_mad->dev);
		printf(SNDRV_LOG_COLOR_BLUE "SRAM_RD_POINT=0x%x\n" SNDRV_LOG_COLOR_NONE,
			sunxi_mad->sram_rd_point);
//		snd_err("SRAM_RD_POINT:0x%x.\n", sunxi_mad->sram_rd_point);
//	} else {
//		sunxi_lpsd_irq_status_clear(sunxi_mad);
//		snd_err("device was not wakeup.\n");
//	}

	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
		0x1 << KEY_WORD_OK, 0x1 << KEY_WORD_OK);

	if (sunxi_mad->suspend_flag & SUNXI_MAD_STANDBY_SRAM_MEM) {
		sunxi_mad_dma_type(SUNXI_MAD_DMA_IO);
	}

	sunxi_mad_sram_chan_com_config(sunxi_mad->mad_priv.audio_src_chan_num,
			sunxi_mad->mad_priv.standby_chan_sel, true);

#ifdef SUNXI_MAD_DATA_INT_USE
	/* enable the data req and wake req */
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
		0x1 << DATA_REQ_INT_MASK, 0x1 << DATA_REQ_INT_MASK);
#else
	pvItemToQueue = SUNXI_MAD_LPSD_IRQ_WORK;
	ret = xQueueSendToBackFromISR(sunxi_mad->irq_queue, (void *)&pvItemToQueue,
					&xHigherPriorityTaskWoken);
	if (ret == errQUEUE_FULL) {
		snd_err("xQueueSendToBackFromISR failed.\n");
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif

	pm_wakecnt_inc();
	snd_print("Stop.\n");

	return HAL_IRQ_OK;
}

static hal_irqreturn_t sunxi_mad_irq_handle(void *data)
{
	struct sunxi_mad_info *sunxi_mad = data;
	unsigned int val = 0;
	int ret = 0;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	unsigned int pvItemToQueue = 0;

	snd_print("Start.\n");

	sunxi_mad->wakeup_flag |= SUNXI_MAD_WAKEUP_MAD_IRQ;

	val = snd_mad_read(sunxi_mad, SUNXI_MAD_INT_ST_CLR);
	if (val & (0x1 << DATA_REQ_INT)) {
		/* disable the data req and wake req */
		snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
			0x1 << DATA_REQ_INT_MASK, 0x0 << DATA_REQ_INT_MASK);

		pvItemToQueue = SUNXI_MAD_DATA_IRQ_WORK;
		ret = xQueueSendToBackFromISR(sunxi_mad->irq_queue, (void *)&pvItemToQueue,
					&xHigherPriorityTaskWoken);
		if (ret == errQUEUE_FULL) {
			snd_err("xQueueSendToBackFromISR failed.\n");
		}
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		/* clear data int state */
		sunxi_mad_irq_status_clear(sunxi_mad);
	}

	snd_print("Stop.\n");

	return HAL_IRQ_OK;
}

void sunxi_sram_dma_config(struct sunxi_dma_params *capture_dma_param)
{
	capture_dma_param->dma_addr = (void *)MAD_SRAM_DMA_SRC_ADDR;
	capture_dma_param->dma_drq_type_num = DRQSRC_MAD_RX;
}

void sunxi_mad_dma_type(enum sunxi_mad_dma_type dma_type)
{
	/* config sunxi_mad_sram dma type should be before DMA_EN */
	switch (dma_type) {
	case SUNXI_MAD_DMA_MEM:
	default:
		snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL, 0x1 << DMA_TYPE, 0x0 << DMA_TYPE);
		break;
	case SUNXI_MAD_DMA_IO:
		snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL, 0x1 << DMA_TYPE, 0x1 << DMA_TYPE);
		break;
	}
}

void sunxi_mad_dma_enable(bool enable)
{
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL, 0x1 << DMA_EN, enable << DMA_EN);
}

int sunxi_mad_open(void)
{
	sunxi_mad->status = SUNXI_MAD_OPEN;
	return 0;
}

int sunxi_mad_enable(bool enable)
{
	u32 reg_val = 0;
	unsigned int mad_is_worked = 1; /*not work*/

	/*open MAD_EN*/
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			0x1 << MAD_EN, enable << MAD_EN);

	if (enable) {
		/* if mad is working well */
		reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_STA);
		reg_val |= ~(0x1 << MAD_RUN);
		mad_is_worked = ~reg_val;
		if (mad_is_worked) {
			snd_err("mad isn't working right!\n");
			return -EBUSY;
		}
	}

	return 0;
}

void sunxi_mad_set_go_on_sleep(bool enable)
{
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			0x1 << GO_ON_SLEEP, enable << GO_ON_SLEEP);
}

int sunxi_mad_close(void)
{
	sunxi_mad_enable(false);
	/*
	 * when sram used for optee at suspend,
	 * mad should set the io type to memcpy.
	 */
	sunxi_mad_dma_type(SUNXI_MAD_DMA_MEM);

	sunxi_mad->audio_src_path = 0;
	sunxi_mad->mad_priv.audio_src_chan_num = 0;
	sunxi_mad->mad_priv.standby_chan_sel = 0;
	sunxi_mad->mad_priv.lpsd_chan_sel = 0;

	sunxi_mad->status = SUNXI_MAD_CLOSE;

	return 0;
}

int sunxi_mad_hw_params(unsigned int mad_channels, unsigned int sample_rate)
{
	snd_print("mad_channels: %d, sample_rate:%d\n",
		mad_channels, sample_rate);

	/*config mad sram audio source channel num*/
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_SRAM_CH_MASK,
		0x1f << MAD_AD_SRC_CH_NUM, mad_channels << MAD_AD_SRC_CH_NUM);

	sunxi_mad_sram_chan_params(mad_channels);

	/* keep lpsd running in 16kHz */
	if (sample_rate == 16000)
		snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			0x1 << AUDIO_DATA_SYNC_FRC, 0x0 << AUDIO_DATA_SYNC_FRC);
	else if (sample_rate == 48000)
		snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			0x1 << AUDIO_DATA_SYNC_FRC, 0x1 << AUDIO_DATA_SYNC_FRC);
	else
		return -EINVAL;

	sunxi_mad->status = SUNXI_MAD_PARAMS;

	return 0;
}

int sunxi_mad_audio_source_sel(enum mad_path_sel path_sel, unsigned int enable)
{
	char *path_str = NULL;
	unsigned int path_sel_map = 0;

	switch (path_sel) {
	case MAD_PATH_NONE:
		path_str = "No-Audio";
		path_sel_map = 0;
	break;
	case MAD_PATH_CODEC:
	case MAD_PATH_CODECADC:
		path_str = "Codec-Input";
		path_sel_map = 2;
	break;
	case MAD_PATH_DMIC:
		path_str = "DMIC-Input";
		path_sel_map = 3;
	break;
	case MAD_PATH_I2S0:
		path_str = "I2S0-Input";
		path_sel_map = 4;
	break;
	case MAD_PATH_I2S1:
		path_str = "I2S1-Input";
		path_sel_map = 4;
	break;
	case MAD_PATH_I2S2:
		path_str = "I2S2-Input";
		path_sel_map = 4;
	break;
	default:
		path_str = "Error-Input";
		path_sel_map = 0;
		return -EINVAL;
	break;
	}
	snd_print("%s, path_sel_map -> %u\n", path_str, path_sel_map);

	if (enable) {
		if ((path_sel_map >= 1) && (path_sel_map <= 5)) {
			snd_mad_update_bits(sunxi_mad,
					SUNXI_MAD_AD_PATH_SEL,
					MAD_AD_PATH_SEL_MASK,
					path_sel_map << MAD_AD_PATH_SEL);
			sunxi_mad->audio_src_path = path_sel;
		} else
			return -EINVAL;
	} else {
		snd_mad_update_bits(sunxi_mad, SUNXI_MAD_AD_PATH_SEL,
			MAD_AD_PATH_SEL_MASK, 0 << MAD_AD_PATH_SEL);
	}

	return 0;
}

static void sunxi_mad_show_all_regs(struct sunxi_mad_info *sunxi_mad)
{
	unsigned int reg_val[4] = {0};
	int reg_offset = 0;

	printf("================= sunxi mad all regs ================\n");
	for (reg_offset = 0; reg_offset < 0x6c; reg_offset += 0x10) {
		reg_val[0] = snd_mad_read(sunxi_mad, reg_offset + 0x0);
		reg_val[1]= snd_mad_read(sunxi_mad, reg_offset + 0x4);
		reg_val[2]= snd_mad_read(sunxi_mad, reg_offset + 0x8);
		reg_val[3]= snd_mad_read(sunxi_mad, reg_offset + 0xc);
		printf("0x%02x-0x%02x: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			reg_offset, reg_offset+0xc,
			reg_val[0], reg_val[1], reg_val[2], reg_val[3]);
	}
	printf("=====================================================\n");
}

/*for ASOC cpu_dai*/
int sunxi_mad_suspend_external(void)
{
	if (sunxi_mad->status == SUNXI_MAD_SUSPEND) {
		snd_err("sunxi mad has suspend!\n");
		return 0;
	}

	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL, 0x1 << DMA_EN, 0x0 << DMA_EN);

	/*
	 * config sunxi_mad_sram as memory
	 * eg: because optee need use the sram.
	 * the sram should stop to data.
	 */
	if (sunxi_mad->suspend_flag & SUNXI_MAD_STANDBY_SRAM_MEM) {
		sunxi_mad_dma_type(SUNXI_MAD_DMA_MEM);
		sunxi_mad_sram_chan_params(0);
	} else {
		sunxi_mad_sram_chan_com_config(sunxi_mad->mad_priv.audio_src_chan_num,
					       sunxi_mad->mad_priv.standby_chan_sel, false);
	}

#ifndef SUNXI_LPSD_CLK_ALWAYS_ON
	sunxi_lpsd_clk_enable(true);
#endif

	sunxi_lpsd_irq_status_clear(sunxi_mad);

	sunxi_mad_lpsd_init();
	sunxi_mad_lpsd_chan_enable(sunxi_mad->mad_priv.lpsd_chan_sel, true);

	sunxi_lpsd_irq_status_clear(sunxi_mad);

	/* disable the data req and enable wake req */
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
			0x1 << DATA_REQ_INT_MASK, 0x0 << DATA_REQ_INT_MASK);
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
			0x1 << MAD_REQ_INT_MASK, 0x1 << MAD_REQ_INT_MASK);

	sunxi_mad_enable(true);
	sunxi_mad_set_go_on_sleep(true);

	sunxi_mad->status = SUNXI_MAD_SUSPEND;

	sunxi_mad_show_all_regs(sunxi_mad);

	printf(SNDRV_LOG_COLOR_BLUE "sunxi mad is working right!\n" SNDRV_LOG_COLOR_NONE);

	return 0;
}

/*for ASOC cpu_dai*/
int sunxi_mad_resume_external(void)
{
	uint32_t __cpsr = 0;
	int ret = 0;
	unsigned int pvItemToQueue = 0;

	snd_print("Start.\n");

	/* disable the wake req */
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
			0x1 << MAD_REQ_INT_MASK, 0x0 << MAD_REQ_INT_MASK);

	if ((sunxi_mad->wakeup_flag & SUNXI_MAD_WAKEUP_LPSD_IRQ) ||
		(sunxi_mad->wakeup_flag & SUNXI_MAD_WAKEUP_MAD_IRQ)) {
		snd_err("sunxi mad has wakeup irq!\n");
		return 0;
	}

	__cpsr = hal_spin_lock_irqsave(&sunxi_mad_lock);

	if (sunxi_mad->status == SUNXI_MAD_RESUME) {
		snd_err("sunxi mad has resume!\n");
		hal_spin_unlock_irqrestore(&sunxi_mad_lock, __cpsr);
		return 0;
	}
	sunxi_mad->status = SUNXI_MAD_RESUME;

	hal_spin_unlock_irqrestore(&sunxi_mad_lock, __cpsr);

	/* not a lpsd interrupt resume */
	if (sunxi_mad->suspend_flag & SUNXI_MAD_STANDBY_SRAM_MEM) {
		sunxi_mad_dma_type(SUNXI_MAD_DMA_IO);
	}

	sunxi_mad_sram_chan_com_config(sunxi_mad->mad_priv.audio_src_chan_num,
			sunxi_mad->mad_priv.standby_chan_sel, true);

	pvItemToQueue = SUNXI_MAD_NULL_IRQ_WORK;
	ret = xQueueSendToBack(sunxi_mad->irq_queue, (void *)&pvItemToQueue,
				pdMS_TO_TICKS(1 * 100UL));
	if (ret == errQUEUE_FULL) {
		snd_err("xQueueSendToBack failed.\n");
	}

	snd_print("Stop.\n");
	return 0;
}

#if defined(CONFIG_ARCH_SUN8IW18P1)
static void sunxi_mad_sram_set_bmode(bool mode)
{
	unsigned int reg = 0;

	reg = readl(SRAM_BMODE_CTRL_REG);
	if (mode)
		reg |= (0x1 << MAD_SRAM_BMODE_CTRL);
	else
		reg &= ~(0x1 << MAD_SRAM_BMODE_CTRL);
	writel(reg, SRAM_BMODE_CTRL_REG);
}
#endif

static void sunxi_mad_work_resume(struct sunxi_mad_info *sunxi_mad)
{
	unsigned int break_flag = 0;
	unsigned int __cpsr = 0;
	unsigned int reg_val = 0;

	__cpsr = hal_spin_lock_irqsave(&sunxi_mad_lock);

	sunxi_mad->status = SUNXI_MAD_RESUME;

	hal_spin_unlock_irqrestore(&sunxi_mad_lock, __cpsr);

	snd_print("Start.\n");

	/* disable the wake req */
	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_INT_MASK,
			0x1 << MAD_REQ_INT_MASK, 0x0 << MAD_REQ_INT_MASK);

	sunxi_lpsd_irq_status_clear(sunxi_mad);

	/* it maybe need 200-800ms */
	for (;;) {
/*
		reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_STA);
		if (((reg_val & (0x3 << MAD_LPSD_STAT)) == 0) &&
			((break_flag & 0x1) == 0x0)) {
			sunxi_mad_lpsd_chan_enable(0, false);
			break_flag |= 0x1 << 0x0;
		}
*/
		reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_SRAM_CH_MASK);
		reg_val = (reg_val >> MAD_CH_CHANGE_EN) & 0x1;
		if ((reg_val == 0) && ((break_flag & 0x2) != 0x2))
			break_flag |= 0x2;

//		if (break_flag == 0x3)
		if (break_flag & 0x2)
			break;
		hal_msleep(10);
	}

/*#define CONFIG_SUNXI_AUDIO_DEBUG*/
#ifdef CONFIG_SUNXI_AUDIO_DEBUG
	/* SUNXI_MAD_DMA_TF_SIZE[0x4C] */
	reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_DMA_TF_SIZE);
	snd_err("SUNXI_MAD_DMA_TF_SIZE:0x%x\n", reg_val);

	reg_val = snd_mad_read(sunxi_mad, SUNXI_MAD_RD_SIZE);
	snd_err("KEY_WORD_OK: MAD_RD_SIZE[0x%x]:0x%x.\n",
			SUNXI_MAD_RD_SIZE, reg_val);
#endif

	snd_mad_update_bits(sunxi_mad, SUNXI_MAD_CTRL,
			0x1 << CPUS_RD_DONE, 0x0 << CPUS_RD_DONE);

#ifndef SUNXI_LPSD_CLK_ALWAYS_ON
	sunxi_mad_lpsd_chan_enable(0, false);
	sunxi_lpsd_clk_enable(false);
#endif

	sunxi_mad_enable(false);

	if (sunxi_mad->wakeup_flag & SUNXI_MAD_WAKEUP_LPSD_IRQ)
		sunxi_mad->wakeup_flag &= ~SUNXI_MAD_WAKEUP_LPSD_IRQ;

	if (sunxi_mad->wakeup_flag & SUNXI_MAD_WAKEUP_MAD_IRQ)
		sunxi_mad->wakeup_flag &= ~SUNXI_MAD_WAKEUP_MAD_IRQ;

#ifdef CONFIG_SND_SUNXI_MAD_DEBUG
	sunxi_mad_schd_wakeup(mad_sleep);
#endif
	snd_print("Stop.\n");
}

static void sunxi_mad_irq_task(void *pvParameters)
{
	unsigned int receive_val;
	BaseType_t xStatus;
	struct sunxi_mad_info *sunxi_mad = pvParameters;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(1 * 1000UL);

	for( ;; ) {
		if(uxQueueMessagesWaiting(sunxi_mad->irq_queue) != 0) {
			snd_print("Queue should have been empty!\r\n");
		}

		xStatus = xQueueReceive(sunxi_mad->irq_queue, &receive_val, xTicksToWait);
		if( xStatus == pdPASS ) {
			snd_print("Received = %d\n", receive_val);
			switch (receive_val) {
			case SUNXI_MAD_DATA_IRQ_WORK:
			case SUNXI_MAD_LPSD_IRQ_WORK:
				sunxi_mad_work_resume(sunxi_mad);
				break;
			}
		}
	}
}

#ifdef CONFIG_SND_SUNXI_MAD_DEBUG
int sunxi_mad_schd_timeout(SemaphoreHandle_t semaphore, long ms)
{
	BaseType_t ret;
	const TickType_t timeout = ms / portTICK_PERIOD_MS;

	if (!semaphore)
		return -1;
	snd_print("\n");
	ret = xSemaphoreTake(semaphore, timeout);
	if (ret == pdPASS) {
		snd_print("\n");
		return 0;
	}
	snd_print("\n");
	return -1;
}

void sunxi_mad_schd_wakeup(SemaphoreHandle_t semaphore)
{
	BaseType_t ret;
	BaseType_t taskwoken = pdTRUE;

	if (!semaphore)
		return;
	snd_print("\n");
	ret = xSemaphoreGive(semaphore);
	if (ret == pdPASS) {
		portYIELD_FROM_ISR(taskwoken);
		snd_print("\n");
		return;
	}
	snd_print("\n");
}
#endif

int sunxi_mad_platform_probe()
{
	int ret = 0;

	snd_print("\n");
	if (sunxi_mad != NULL) {
		sunxi_mad->ref_count++;
		snd_print("\n");
		return 0;
	}

	sunxi_mad = snd_malloc(sizeof(struct sunxi_mad_info));
	if (!sunxi_mad) {
		snd_err("no memory\n");
		return -ENOMEM;
	}
	sunxi_mad->ref_count++;

#if defined(CONFIG_ARCH_SUN8IW18P1)
	sunxi_mad_sram_set_bmode(MAD_SRAM_BMODE_NORMAL);
#endif

	/* clk */
	ret = snd_sunxi_mad_clk_init(&sunxi_mad->clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("snd_sunxi_mad_clk_init failed.\n");
		goto err_mad_set_clock;
	}

	/* mem base */
	sunxi_mad->mem_base = (void *)MAD_BASE;

	/*request irq*/
	sunxi_mad->lpsd_irq = MAD_WAKE_IRQn;
	ret = hal_request_irq(sunxi_mad->lpsd_irq, sunxi_lpsd_irq_handle, "lpsd", sunxi_mad);
	if (ret < 0) {
		snd_err("lpsd irq request failed.\n");
		goto err_lpsd_irq_request;
	}
	ret = pm_set_wakeirq(MAD_WAKE_IRQn);
	if (ret) {
		snd_err("pm_set_wakeirq for mad-lpsd failed\n");
	}
	ret = hal_enable_irq(sunxi_mad->lpsd_irq);
	if (ret < 0) {
		snd_err("lpsd irq enable failed.\n");
		goto err_lpsd_irq_enable;
	}

	sunxi_mad->mad_irq = MAD_DATA_REQ_IRQn;
	ret = hal_request_irq(sunxi_mad->mad_irq, sunxi_mad_irq_handle, "mad", sunxi_mad);
	if (ret < 0) {
		snd_err("mad irq request failed.\n");
		goto err_mad_irq_request;
	}
	ret = hal_enable_irq(sunxi_mad->mad_irq);
	if (ret < 0) {
		snd_err("mad irq enable failed.\n");
		goto err_mad_irq_enable;
	}

	sunxi_mad->irq_queue = xQueueCreate(2, sizeof(unsigned int));
	if (sunxi_mad->irq_queue == NULL) {
		snd_err("sunxi mad create irq queue failed.\n");
		ret =  -EFAULT;
		goto err_mad_irq_queue_create;
	}
	ret = xTaskCreate(sunxi_mad_irq_task, "sunxi mad task", 1024,
			(void *)sunxi_mad, 31, &sunxi_mad->pxMadIrqTask);
	if (ret != pdPASS) {
		snd_err("sunxi mad create irq task failed.\n");
		ret =  -EFAULT;
		goto err_mad_irq_task_create;
	}
	ret = pm_task_register(sunxi_mad->pxMadIrqTask, PM_TASK_TYPE_PM);
	if (ret) {
		snd_err("pm_task_register for mad-lpsd irq task failed\n");
	}

#ifdef CONFIG_SND_SUNXI_MAD_DEBUG
	mad_sleep = xSemaphoreCreateBinary();
	if (!mad_sleep) {
		snd_err("semaphore mad_sleep create failed\n");
		ret =  -EFAULT;
		goto err_mad_sleep_semaphore_create;
	}
#endif
	snd_print("\n");

	return 0;

#ifdef CONFIG_SND_SUNXI_MAD_DEBUG
err_mad_sleep_semaphore_create:
	vTaskDelete(sunxi_mad->pxMadIrqTask);
#endif
err_mad_irq_task_create:
	vQueueDelete(sunxi_mad->irq_queue);
err_mad_irq_queue_create:
err_mad_irq_enable:
err_mad_irq_request:
	hal_disable_irq(sunxi_mad->lpsd_irq);
err_lpsd_irq_enable:
err_lpsd_irq_request:
err_mad_set_clock:
	snd_sunxi_mad_clk_exit(&sunxi_mad->clk);
	snd_free(sunxi_mad);
	return ret;
}

int sunxi_mad_platform_remove()
{
	/* struct sunxi_mad_info *sunxi_mad = platform->private_data; */

	snd_print("\n");
	if (!sunxi_mad) {
		snd_err("sunxi_mad is null.\n");
		return 0;
	} else if (--sunxi_mad->ref_count != 0) {
		snd_err("sunxi_mad is ref_count:%d.\n", sunxi_mad->ref_count);
		return 0;
	}

#ifdef CONFIG_SND_SUNXI_MAD_DEBUG
	vSemaphoreDelete(mad_sleep);
#endif

	vQueueDelete(sunxi_mad->irq_queue);
	vTaskDelete(sunxi_mad->pxMadIrqTask);

	hal_disable_irq(sunxi_mad->lpsd_irq);
	hal_disable_irq(sunxi_mad->mad_irq);

	snd_sunxi_mad_clk_exit(&sunxi_mad->clk);

	snd_free(sunxi_mad);
	sunxi_mad = NULL;
	return 0;
}
