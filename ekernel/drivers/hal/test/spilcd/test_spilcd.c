/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <hal_cache.h>
#include <hal_mem.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <hal_lcd_fb.h>

static uint32_t width;
static uint32_t height;
static long int screensize = 0;
static uint32_t line_length;
static char *fbsmem_start = 0;

static void lcdfb_fb_init(uint32_t yoffset, struct fb_info *p_info)
{
	p_info->screen_base = fbsmem_start;
	p_info->var.xres = width;
	p_info->var.yres = height;
	p_info->var.xoffset = 0;
	p_info->var.yoffset = yoffset;
	p_info->fix.line_length = line_length;
}

int show_rgb(unsigned int sel)
{
	int i = 0, ret = -1;
	struct fb_info fb_info;
	int bpp = 4;
	unsigned char color[4] = {0xff,0x0,0xff,0x0};

	width = bsp_disp_get_screen_width(sel);
	height = bsp_disp_get_screen_height(sel);

	line_length = width * bpp;
	screensize = line_length * height;

    fbsmem_start = dma_alloc_coherent(screensize);

	hal_log_info("width = %d, height = %d, screensize = %d, fbsmem_start = %x\n",
			width, height, screensize, fbsmem_start);

	memset(fbsmem_start, 0, screensize);

	for (i = 0; i < screensize / bpp; ++i) {
		memcpy(fbsmem_start+i*bpp, color, bpp);
	}

	lcdfb_fb_init(0, &fb_info);
	hal_dcache_clean((unsigned long)fbsmem_start, screensize);
	bsp_disp_lcd_set_layer(sel, &fb_info);

	dma_free_coherent(fbsmem_start);
	return ret;
}

static int cmd_test_spilcd(int argc, char **argv)
{
    uint8_t ret;

    hal_log_info("Run spilcd hal layer test case\n");

    ret = show_rgb(0);

    hal_log_info("spilcd test finish\n");

    return ret;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_spilcd, test_spilcd, spilcd hal APIs tests)
