/*
****************************************************************************************************
*                                               MELIS
*                               the Easy Portable/Player Develop Kits
*                                           UART Driver
*
*                           (c) Copyright 2011-2014, All winners Co,Ld.
*                                       All Rights Reserved
*
* File    : drv_uart.h
* By      : james.deng
* Version : 1.0.0
* Date    : 2011-12-25
* Descript:
* Update  : <date>          <author>            <version>           <notes>
*           2011-12-25      james.deng          1.0.0               build the file.
****************************************************************************************************
*/

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#define UART_PORT_COUNT                 4
#define UART_DMA_MODE                   0

/*
    support baudrate:
    300,
    600,
    1200,
    2400,
    4800,
    9600,
    19200,
    38400,
    57600,
    115200,
    230400,
    576000,
    921600,
    1000000,
    1500000,
    3000000,
    4000000,
*/

// The Parameter of uart
typedef struct __UART_PARA
{
    uint32_t    nEvenParity;        // 0: Odd       1: Even
    uint32_t    nParityEnable;      // 0: disable   1: enable
    uint32_t    nStopBit;           // 0: 1 stopbit 1: 1.5 stopbit when DLS=0,else 2 bits
    uint32_t    nDataLen;           // 0: 5bit      1: 6bit     2: 7bit     3: 8bit
} __uart_para_t;

// Uart FIFO type
typedef enum __UART_FIFO_TYPE
{
    UART_FIFO_TYPE_RX,
    UART_FIFO_TYPE_TX
} __uart_fifo_type_t;


typedef enum __UART_CMD
{
    UART_CMD_SET_PARA,
    UART_CMD_SET_BAUDRATE,
    UART_CMD_FLUSH
} __uart_cmd_t;

#endif // __DRV_UART_H__
