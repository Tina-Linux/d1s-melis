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
#ifndef	__SUN8IW20_DMIC_H_
#define	__SUN8IW20_DMIC_H_

#define	SUNXI_DMIC_MEMBASE (0x02031000)

/*------------------------ CLK CONFIG FOR NORMAL ---------------------------*/
#define SUNXI_DMIC_CLK_PLL_AUDIO	CLK_PLL_AUDIO0
#define SUNXI_DMIC_CLK_DMIC		CLK_DMIC
#define SUNXI_DMIC_CLK_BUS		CLK_BUS_DMIC
#define SUNXI_DMIC_CLK_RST		RST_BUS_DMIC

/*------------------------ PIN CONFIG FOR NORMAL ---------------------------*/



/*------------------------ PIN CONFIG FOR D1 EVB -----------------------*/
dmic_gpio_t g_dmic_gpio = {
	.clk	= {GPIOE(17), 6},
	.din0	= {GPIOB(11), 2},
	.din1	= {GPIOB(10), 2},
	.din2	= {GPIOD(17), 4},
	.din3	= {GPIOG(26), -1},	/* no use */
};

/*------------------------ CLK CONFIG FOR SUN8IW20 ---------------------------*/
struct sunxi_dmic_clk {
	struct reset_control *rstclk;

	hal_clk_t pllclk;
	hal_clk_t moduleclk;
	hal_clk_t busclk;
};

static inline int snd_sunxi_dmic_clk_enable(struct sunxi_dmic_clk *clk)
{
	int ret;

	ret = hal_reset_control_deassert(clk->rstclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_deassert rstclk failed.\n");
		goto err_dmic_rstclk_deassert;
	}

	ret = hal_clock_enable(clk->busclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_enable busclk failed.\n");
		goto err_dmic_busclk_enable;
	}
	ret = hal_clock_enable(clk->pllclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_enable pllclk failed.\n");
		goto err_dmic_pllclk_enable;
	}
	ret = hal_clock_enable(clk->moduleclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic clk_enable moduleclk failed.\n");
		goto err_dmic_moduleclk_enable;
	}

	return HAL_CLK_STATUS_OK;

err_dmic_moduleclk_enable:
	hal_clock_disable(clk->pllclk);
err_dmic_pllclk_enable:
err_dmic_busclk_enable:
err_dmic_rstclk_deassert:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_dmic_clk_disable(struct sunxi_dmic_clk *clk)
{
	hal_clock_disable(clk->busclk);
	hal_clock_disable(clk->moduleclk);
	hal_clock_disable(clk->pllclk);

	hal_reset_control_assert(clk->rstclk);
}

static inline int snd_sunxi_dmic_clk_init(struct sunxi_dmic_clk *clk)
{
	int ret;
	hal_reset_type_t reset_type = HAL_SUNXI_RESET;
	hal_clk_type_t clk_type = HAL_SUNXI_CCU;

	clk->pllclk = hal_clock_get(clk_type, SUNXI_DMIC_CLK_PLL_AUDIO);
	clk->moduleclk = hal_clock_get(clk_type, SUNXI_DMIC_CLK_DMIC);
	clk->busclk = hal_clock_get(clk_type, SUNXI_DMIC_CLK_BUS);
	clk->rstclk = hal_reset_control_get(reset_type, SUNXI_DMIC_CLK_RST);

	ret = hal_clk_set_parent(clk->moduleclk, clk->pllclk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("clk clk_set_parent failed.\n");
		goto err_dmic_moduleclk_set_parent;
	}

	ret = snd_sunxi_dmic_clk_enable(clk);
	if (ret != HAL_CLK_STATUS_OK) {
		snd_err("dmic snd_sunxi_dmic_clk_enable failed.\n");
		goto err_clk_enable;
	}

	return HAL_CLK_STATUS_OK;

err_clk_enable:
err_dmic_moduleclk_set_parent:
	return HAL_CLK_STATUS_ERROR;
}

static inline void snd_sunxi_dmic_clk_exit(struct sunxi_dmic_clk *clk)
{
	snd_sunxi_dmic_clk_disable(clk);

	hal_clock_put(clk->busclk);
	hal_clock_put(clk->moduleclk);
	hal_clock_put(clk->pllclk);

	hal_reset_control_put(clk->rstclk);
}

static inline int snd_sunxi_dmic_clk_set_rate(struct sunxi_dmic_clk *clk, int stream,
					      unsigned int freq_in, unsigned int freq_out)
{
	int ret;

	(void)stream;
	(void)freq_in;

	ret = hal_clk_set_rate(clk->pllclk, freq_out);
	if (ret < 0) {
		snd_err("set pllclk %u failed\n", freq_out);
		return HAL_CLK_STATUS_ERROR;
	}

	return HAL_CLK_STATUS_OK;
}

#endif /* __SUN8IW20_DMIC_H_ */
