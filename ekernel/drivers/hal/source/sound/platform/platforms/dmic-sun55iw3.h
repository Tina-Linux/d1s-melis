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
#ifndef	__SUN55IW3_DMIC_H_
#define	__SUN55IW3_DMIC_H_

#define	SUNXI_DMIC_MEMBASE (0x07111000)

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/
dmic_gpio_t g_dmic_gpio = {
	.clk	= {GPIOF(9), 2},
	.din0	= {GPIOF(28), 5},
	.din1	= {GPIOG(30), 5},
	.din2	= {GPIOG(28), 5},
	.din3	= {GPIOG(26), 5},
};

/*------------------------ CLK CONFIG FOR SUN20IW2 ---------------------------*/
struct sunxi_dmic_clk {
	struct reset_control *clk_rst;		/* AON_RESET: RST_DMIC */
	hal_clk_t clk_bus;			/* AON_CCU: CLK_BUS_DMIC */
};

static inline int snd_sunxi_dmic_clk_enable(struct sunxi_dmic_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	ret = hal_reset_control_deassert(clk->clk_rst);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_rst clk_deassert failed.\n");
		goto err_deassert_clk_rst;
	}
	ret = hal_clock_enable(clk->clk_bus);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_bus enable failed.\n");
		goto err_enable_clk_bus;
	}

	return HAL_CLK_STATUS_OK;

err_enable_clk_bus:
	hal_reset_control_assert(clk->clk_rst);
err_deassert_clk_rst:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_dmic_clk_disable(struct sunxi_dmic_clk *clk)
{
	snd_print("\n");

	hal_clock_disable(clk->clk_bus);
	hal_reset_control_assert(clk->clk_rst);

	return;
}

static inline int snd_sunxi_dmic_clk_init(struct sunxi_dmic_clk *clk)
{
	int ret;

	snd_print("\n");

	/* rst & bus */
	clk->clk_rst = hal_reset_control_get(HAL_SUNXI_DSP_RESET, RST_BUS_DSP_DMIC);
	if (!clk->clk_rst) {
		snd_err("dmic clk_rst hal_reset_control_get failed\n");
		goto err_get_clk_rst;
	}
	clk->clk_bus = hal_clock_get(HAL_SUNXI_DSP, CLK_BUS_DSP_DMIC);
	if (!clk->clk_bus) {
		snd_err("dmic clk_bus hal_clock_get failed\n");
		goto err_get_clk_bus;
	}

	/* note: Enable and then set the freq to avoid clock lock errors */
	ret = snd_sunxi_dmic_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic snd_sunxi_dmic_clk_enable failed.\n");
		goto err_clk_enable;
	}

	return HAL_CLK_STATUS_OK;

err_clk_enable:
	hal_clock_put(clk->clk_bus);
err_get_clk_bus:
	hal_reset_control_put(clk->clk_rst);
err_get_clk_rst:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_dmic_clk_exit(struct sunxi_dmic_clk *clk)
{
	snd_print("\n");

	snd_sunxi_dmic_clk_disable(clk);
	hal_clock_put(clk->clk_bus);
	hal_reset_control_put(clk->clk_rst);

	return;
}

static inline int snd_sunxi_dmic_clk_set_rate(struct sunxi_dmic_clk *clk, int stream,
					      unsigned int freq_in, unsigned int freq_out)
{
	return HAL_CLK_STATUS_OK;
}

#endif /* __SUN55IW3_DMIC_H_ */
