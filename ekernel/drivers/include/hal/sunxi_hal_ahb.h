/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.


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

#ifndef __HAL_AHB_H
#define __HAL_AHB_H

#include <stdio.h>

#define ahb_err(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)

/* AHB HUSTER ids */
enum ahb_huster {
	AHB_CPU0    = 0,    /* CPU0 bandwidth */
	AHB_CPU1,           /* CPU1 bandwidth */
	AHB_RISCV,            /* RISCV */
	AHB_DSP,          /* DSP */
	AHB_CE,           /* CE */
	AHB_DMA0,         /* DMA0 */
	AHB_DMA1,        /* DMA1 */
	AHB_CSI,            /* CSI */
	AHB_ST0,           /* ST0 */
	AHB_ST1,           /* ST1 */
};

typedef enum{
	HAL_AHB_STATUS_BUSY_CHANNEL = -4,
	HAL_AHB_STATUS_ERROR_PARAMETER = -3,
	HAL_AHB_STATUS_ERROR_CHANNEL = -2,
	HAL_AHB_STATUS_ERROR = -1,
	HAL_AHB_STATUS_OK = 0
}hal_ahb_status_t;

hal_ahb_status_t hal_ahb_huster_get_value(enum ahb_huster type, unsigned int *value);
hal_ahb_status_t hal_ahb_huster_enable(void);
hal_ahb_status_t hal_ahb_disable_id_chan(enum ahb_huster type);

uint32_t ahb_get_sum_total_read(void);
uint32_t ahb_get_sum_total_write(void);
uint32_t ahb_get_max_total_read(void);
uint32_t ahb_get_max_total_write(void);

#endif
