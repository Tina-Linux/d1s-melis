/*****************************************************************************
**
**  Name:           host_gpio.h
**
**  Description:    This is the wrapper file for Hardware Abstraction 
**                  layer (HAL) and OS Abstraction Layer (OSAL) of MHD, 
**                  Broadcom's WiFi host driver.
**
**  Copyright (c) 2018, Broadcom Inc., All Rights Reserved.
**  Broadcom WCC. Proprietary and confidential.
**
*****************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/* config oob pin as interrupt source, rising edge trigger */
void host_oob_interrupt_config( void );
/* enable oob interrupt */
void host_oob_interrupt_enable( void );
/* enable oob interrupt */
void host_oob_interrupt_disable( void );
/* clear oob interrupt */
void host_oob_interrupt_clear( void );

/* config wl_reg_on pin as gpio pin */
void host_wl_regon_config( int is_init );
/* pull high wl_reg_on */
void host_wl_regon_up( void );
/* pull low wl_reg_on */
void host_wl_regon_down( void );

uint32_t host_sdc_index_config( void );

#ifdef __cplusplus
} /* extern "C" */
#endif
