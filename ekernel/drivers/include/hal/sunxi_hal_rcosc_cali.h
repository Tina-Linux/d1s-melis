/**
 * @file  hal_rcosc_cali.h
 * @author  XRADIO IOT WLAN Team
 */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_RCOSC_CALI_H_
#define _DRIVER_CHIP_HAL_RCOSC_CALI_H_

#include <sunxi_hal_prcm.h>
#include <hal_status.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAL_ASSERT_PARAM
#define HAL_ASSERT_PARAM(exp)                                           \
    do {                                                                \
        if (!(exp)) {                                                   \
            printf("Invalid param at %s:%d\n", __func__, __LINE__); \
        }                                                               \
    } while (0)
#endif

#define RCOCALI_CNT_TARGET_MASK         (0x0FFFF)
#define RCOCALI_RCO_DIVIDEND_MASK       (0xFFFFFFFF)
#define RCOCALI_DCXO_CNT_MASK           (0x3FFFFF)

#define RCOCALI_QUOTIENT_INVALID_FLAG   (0x1<<0)
#define RCOCALI_DENOMINATOR_INVALID_FLAG (0x1<<1)
#define RCOCALI_PARAM_ERR_CLR           (0x1<<2)
#define RCOCALI_MONITOR_EN              (0x1<<4)
#define RCOCALI_ABNORMAL_FLAG_CLR       (0x1<<5)
#define RCOCALI_LONG_LEVEL_FOUND        (0x1<<6)
#define RCOCALI_GLITCH_FOUND            (0x1<<7)
#define RCOCALI_GLITCH_MAX_WIDTH_MASK	(0x7F<<8)
#define RCOCALI_LEVEL_MAX_WIDTH_MASK	(0xFFFF<<16)

#define RCOCALI_INT_CLR                 (0x1<<1)
#define RCOCALI_INT_EN                  (0x1<<0)

typedef struct {
	__IO uint32_t CNT_TARGET;       /* , Address offset: 0x000 */
	__IO uint32_t DIVIDEND;         /* , Address offset: 0x004 */
	__O  uint32_t DCXO_CNT;         /* , Address offset: 0x008 */
	__IO uint32_t ABNORMAL_MONITOR; /* , Address offset: 0x00C */
	__IO uint32_t INTERRUPT;        /* , Address offset: 0x010 */
} RCOCALI_CTRL_T;

#define RCOSC_CALI_CTRL_BASE	(0x4004C800)

#define RCOCALI_CTRL    ((RCOCALI_CTRL_T *)RCOSC_CALI_CTRL_BASE)

/**
 * @brief RcoscCali initialization parameters
 */
typedef struct {
    uint32_t cnt_n;
    uint32_t out_clk;
} RCOCALI_InitParam;

/**
 * @brief RcoscCali configure parameters
 */
typedef struct {
    PRCM_RCOSC_WkModSel mode;
    PRCM_RCOSC_SCALE_PHASE2_WkTimes phase2_times;
	PRCM_RCOSC_SCALE_PHASE3_WkTimes phase3_times;
	uint8_t phase1_num;
	uint8_t phase2_num;
	uint16_t wup_time;
} RCOCALI_ConfigParam;

hal_status_t HAL_RcoscCali_Init(const RCOCALI_InitParam *param);
hal_status_t HAL_RcoscCali_Init(const RCOCALI_InitParam *param);
hal_status_t HAL_RcoscCali_DeInit(void);
void HAL_RcoscCali_Start();

hal_status_t hal_get_rccal_output_freq(uint32_t *rccal_clk_freq);
hal_status_t hal_get_rc_lf_freq(uint32_t *rc_lf_freq);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_RCOSC_CALI_H_ */
