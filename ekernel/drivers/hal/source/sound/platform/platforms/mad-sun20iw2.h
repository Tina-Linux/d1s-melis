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
#ifndef	__SUN20IW2_MAD_H_
#define	__SUN20IW2_MAD_H_

#include <hal_clk.h>
#include <hal_reset.h>

#if defined(CONFIG_CORE_DSP0)
#define MAD_WAKE_IRQn		(RINTC_IRQ_MASK | 59)
#define MAD_DATA_REQ_IRQn	(RINTC_IRQ_MASK | 60)
#endif

/* memory mapping */
#define MAD_BASE (0x4004E000)
#define MAD_SRAM_DMA_SRC_ADDR (0x04100000)	/* 64k */

#undef MAD_CLK_ALWAYS_ON
#define SUNXI_LPSD_CLK_ALWAYS_ON
#undef SUNXI_MAD_DATA_INT_USE
#define SUNXI_MAD_SRAM_SUSPEND_RESET
/* 64k bytes */
#define MAD_SRAM_SIZE_VALUE 0x40

/*------------------------ CLK CONFIG FOR SUN20IW2 ---------------------------*/
struct sunxi_mad_clk {
	/* audio pll --> mad(24.576MHz)
	 * CLK_PLL_AUDIO --> CLK_PLL_AUDIO1X -----> CLK_AUDPLL_HOSC_SEL --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_MAD_LPSD
	 *               `-> CLK_PLL_AUDIO2X -----^
	 *                   CLK_HOSC ------------^
	 *                   CLK_DAUDIO_MCLK_IN --^
	 *
	 * audio pll --> mad(22.5792MHz)
	 * CLK_DPLL1 --> CLK_CK1_AUD --> CLK_CK1_AUD_DIV --> CLK_CODEC_ADC_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_MAD_LPSD
	 *
	 * audio pll -> mad(8.192MHz)
	 * CLK_AUD_RCO_DIV --> CLK_CODEC_ADC_SEL1 --> CLK_MAD_LPSD
	 *
	 * mad_apb & mad_ahb
	 * CLK_APB --> CLK_MAD_APB_GATE
	 * CLK_AHB_DIV --> CLK_MAD_AHB_GATE
	 */
	struct reset_control *clk_rst_lpsd;	/* AON_RESET: RST_LPSD */
	struct reset_control *clk_rst_mad;	/* AON_RESET: RST_MAD */
	struct reset_control *clk_rst_madcfg;	/* AON_RESET: RST_MADCFG */

	hal_clk_t clk_rc_hf_en;			/* R_CCU  : RC_HF_EN           -> 8.192M          */
	hal_clk_t clk_aud_rco_div;		/* AON_CCU: CLK_AUD_RCO_DIV    -> 8.192M          */
	hal_clk_t clk_adc_sel1;			/* AON_CCU: CLK_CODEC_ADC_SEL1 */

	/* module */
	hal_clk_t clk_mad_lpsd;			/* AON_CCU: CLK_MAD_LPSD */
	hal_clk_t clk_mad_apb_gate;		/* AON_CCU: CLK_MAD_APB_GATE */
	hal_clk_t clk_mad_ahb_gate;		/* AON_CCU: CLK_MAD_AHB_GATE */

	hal_clk_t mad_clk;	/* --> clk_mad_apb_gate */
	hal_clk_t mad_cfg_clk;	/* --> clk_mad_ahb_gate */
	hal_clk_t mad_ad_clk;	/* --> clk_mad_ahb_gate */
	hal_clk_t lpsd_clk;	/* --> clk_mad_lpsd */
};

int snd_sunxi_mad_clk_enable(struct sunxi_mad_clk *clk);
void snd_sunxi_mad_clk_disable(struct sunxi_mad_clk *clk);
int snd_sunxi_mad_clk_init(struct sunxi_mad_clk *clk);
void snd_sunxi_mad_clk_exit(struct sunxi_mad_clk *clk);
int snd_sunxi_mad_clk_set_rate(struct sunxi_mad_clk *clk, int stream,
			       unsigned int freq_in, unsigned int freq_out);

#endif	/* __SUN20IW2_MAD_H_ */
