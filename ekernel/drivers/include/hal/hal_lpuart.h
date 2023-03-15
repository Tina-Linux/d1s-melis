/**
 * @file  hal_lpuart.h
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
#ifndef SUNXI_HAL_LPUART_H
#define SUNXI_HAL_LPUART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <hal_queue.h>
#include "sunxi_hal_common.h"
#include "hal_interrupt.h"
#ifdef CONFIG_COMPONENTS_PM
#include <pm_devops.h>
#endif
/*
 * include the platform uart header file.
 */
#include <lpuart/platform-lpuart.h>
#include "sdmmc/hal/rom_debug.h"

#define OS_WAIT_FOREVER         0xffffffffU /* Wait forever timeout value */
#define LPUART_RX_DATA_FLAG_BIT         (1U << 9)
#define LPUART_RX_DATA_CMP_FLAG_BIT     (1U << 7)

/* This enum defines baud rate of the UART frame. */
typedef enum
{
    LPUART_BAUDRATE_9600 = 0,
    LPUART_BAUDRATE_MAX
} lpuart_baudrate_t;

/* This enum defines word length of the UART frame. */
typedef enum
{
    LPUART_WORD_LENGTH_4 = 0,
    LPUART_WORD_LENGTH_5,
    LPUART_WORD_LENGTH_6,
    LPUART_WORD_LENGTH_7,
    LPUART_WORD_LENGTH_8,
    LPUART_WORD_LENGTH_9,
} lpuart_word_length_t;

/* This enum defines smp bit of the UART frame. */
typedef enum
{
    LPUART_MSB_BIT_0 = 0,
    LPUART_MSB_BIT_1,
} lpuart_msb_bit_t;

/* This enum defines parity of the LPUART frame. */
typedef enum
{
    LPUART_PARITY_NONE = 0,
    LPUART_PARITY_ODD,
    LPUART_PARITY_EVEN,
    LPUART_PARITY_SPACE,
    LPUART_PARITY_MARK
} lpuart_parity_t;

/* This struct defines UART configure parameters. */
typedef struct
{
    lpuart_baudrate_t baudrate;
    lpuart_word_length_t word_length;
    lpuart_msb_bit_t msb_bit;
    lpuart_parity_t parity;
} _lpuart_config_t;

typedef enum
{
    LPUART_0 = 0,
    LPUART_1 = 1,
    LPUART_MAX,
} lpuart_port_t;

typedef void (*lpuart_callback_t)(void *arg);

typedef struct
{
	void     *arg;
	uint32_t irqn;
	uint8_t  bind_uart;
	uint32_t rx_len;
	uint32_t timeout;
	hal_queue_t queue;
	lpuart_port_t     lpuart_port;
	lpuart_callback_t func;
#ifdef CONFIG_COMPONENTS_PM
	struct pm_device pm;
#endif
} lpuart_priv_t;

typedef enum
{
    HAL_LPUART_STATUS_ERROR_PARAMETER = -4,      /**< Invalid user input parameter. */
    HAL_LPUART_STATUS_ERROR_BUSY = -3,           /**< LPUART port is currently in use. */
    HAL_LPUART_STATUS_ERROR_UNINITIALIZED = -2,  /**< LPUART port has not been initialized. */
    HAL_LPUART_STATUS_ERROR = -1,                /**< LPUART driver detected a common error. */
    HAL_LPUART_STATUS_OK = 0                     /**< LPUART function executed successfully. */
} hal_lpuart_status_t;

/* LPUART->RX_CMP1,LPUART->RX_CMP2 R/W */
#define LPUART_RX_CMP_DATA0_POS         3
#define LPUART_RX_CMP_DATA1_POS         12
#define LPUART_RX_CMP_DATA2_POS         21
#define LPUART_RX_CMP_DATA3_POS         3
#define LPUART_RX_CMP_DATA4_POS         12

typedef enum {
	LPUART_RX_CMP_DATA_NUM_1 	= 1,
	LPUART_RX_CMP_DATA_NUM_2,
	LPUART_RX_CMP_DATA_NUM_3,
	LPUART_RX_CMP_DATA_NUM_4,
	LPUART_RX_CMP_DATA_NUM_5,
	LPUART_RX_CMP_DATA_NUM_MAX 	= 5,
} LPUART_RX_CMP_DATA_NUM;

#define LPUART_RX_CMP_LEN_SHIFT         0

/* Exported functions --------------------------------------------------------*/


/**
 * @brief Initialize the LPUART
 * @retval int32_t, HAL_OK on success
 */
int32_t hal_lpuart_init(lpuart_port_t lpuart_port);

/**
 * @brief DeInitialize the specified LPUART
 * @param[in] lpuartID ID of the specified LPUART
 * @retval int32_t, HAL_OK on success
 */
int32_t hal_lpuart_deinit(lpuart_port_t lpuart_port);

int32_t hal_lpuart_control(lpuart_port_t lpuart_port, int cmd, void *args);
/**
 * @brief Receive an amount of data in interrupt mode
 * @param[in] lpuartID ID of the specified LPUART
 * @param[out] buf Pointer to the data buffer
 * @param[in] size The maximum number of bytes to be received.
 *                 The actual received bytes can be less than this.
 * @param[in] msec Timeout value in millisecond to receive data.
 *                 HAL_WAIT_FOREVER for no timeout.
 * @return Number of bytes received, -1 on error
 *
 * @note This function is not thread safe. If using the LPUART receive series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
int32_t hal_lpuart_receive(int32_t dev, uint8_t *data, uint32_t num);

/**
 * @brief setup rx data compare function
 * @param[in] lpuartID ID of the specified LPUART
 * @param[in] cmp_len the length of compare data
 * @param[in] cmp_data buffer point to compare data
 * @retval int32_t, HAL_OK on success
 *
 * @note
 */
int32_t hal_lpuart_rx_cmp(lpuart_port_t lpuart_port, uint8_t cmp_len, uint8_t *cmp_data);
int32_t hal_lpuart_rx_cmp(lpuart_port_t lpuart_port, uint8_t cmp_len, uint8_t *cmp_data);
int32_t hal_lpuart_enable_rx_data(lpuart_port_t lpuart_port, lpuart_callback_t cb, void *arg);
int32_t hal_lpuart_disable_rx_data(lpuart_port_t lpuart_port);

/**
 * @brief Enable receive compare callback function for the specified LPUART
 * @param[in] cb The LPUART receive ready callback function
 * @param[in] arg Argument of the LPUART receive ready callback function
 * @retval int32_t, HAL_OK on success
 *
 * @note To handle receive data externally, use this function to enable the
 *       receive ready callback function, then receive and process the data in
 *       the callback function.
 * @note If the receive ready callback function is enabled, all other receive
 *       series functions cannot be used to receive data.
 * @note This function is not thread safe. If using the LPUART receive series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
int32_t hal_lpuart_enable_rx_cmp(lpuart_port_t lpuart_port, lpuart_callback_t cb, void *arg);

/**
 * @brief Disable receive compare callback function for the specified LPUART
 * @param[in] lpuartID ID of the specified LPUART
 * @retval int32_t, HAL_OK on success
 */
int32_t hal_lpuart_disable_rx_cmp(lpuart_port_t lpuart_port);

/**
 * @brief Set PM mode to be bypassed
 * @param[in] lpuartID ID of the specified LPUART
 * @param[in] mode Bit mask of PM mode to be bypassed
 * @retval int32_t, HAL_OK on success
 */
int32_t HAL_LPUART_SetBypassPmMode(lpuart_port_t lpuart_port, uint8_t mode);
static hal_irqreturn_t lpuart_irq_handler(void *dev_id);


#ifdef __cplusplus
}
#endif

#endif
