/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_hal_DBI.h
 *
 *    Description:  DBI HAL definition.
 *
 *        Version:  FreeRTOS
 *         Create:  2022-2-10 11:11:56
 *       Revision:  none
 *         Author:  libairong@allwinnertech.com
 *   Organization:  SWC-BPD
 *  Last Modified:  2022-3-21 16:02:11
 *
 * ===========================================================================================
 */
#ifndef SUNXI_HAL_DBI_H
#define SUNXI_HAL_DBI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sunxi_hal_common.h"
#include <hal_gpio.h>
#include <hal_dma.h>
#include <hal_sem.h>
#include <hal_clk.h>
#include <dbi/common_dbi.h>
#include <dbi/platform_dbi.h>

#define DBI_WRITE  0
#define DBI_READ 1

typedef enum
{
    HAL_DBI_MASTER_0 = 0, /**< dbi master port 0 */
    HAL_DBI_MASTER_1 = 1, /**< dbi master port 1 */
    HAL_DBI_MASTER_MAX,   /**< dbi master max port number\<invalid\> */
} hal_dbi_master_port_t;

typedef enum
{
    DBI_MODE_ALLWAYS_ON = 0,//0:always on mode
    DBI_MODE_SOFTWARE_TRIGGER = 1,//1:software trigger mode
    DBI_MODE_TIMER_TRIGGER = 2,//2:timer trigger mode
    DBI_MODE_TE_TRIGGER = 3,//3:te trigger mode
} hal_dbi_en_mode_t;

typedef enum
{
    HAL_DBI_RGB666_FMT_NORMAL = 0,//0:normal format
    HAL_DBI_RGB666_FMT_ILITEK = 1,//1 special format for ILITEK
    HAL_DBI_RGB666_FMT_NEW_VISION = 2,//2 special format for new vision  
} hal_dbi_rgb666_format_t;

typedef enum  
{
	DBI_RGB111 = 0,
	DBI_RGB444 = 1,
	DBI_RGB565 = 2,
	DBI_RGB666 = 3,
	DBI_RGB888 = 4,
} dbi_output_data_format_t;

typedef enum  {
	L3I1 = 0,
	L3I2 = 1,
	L4I1 = 2,
	L4I2 = 3,
	D2LI = 4,
}dbi_interface_t;

typedef enum {
	DBI_SRC_RGB = 0,
	DBI_SRC_RBG = 1,
	DBI_SRC_GRB = 2,
	DBI_SRC_GBR = 3,
	DBI_SRC_BRG = 4,
	DBI_SRC_BGR = 5,
	/* following definition only for rgb565
	 * to change the RGB order in two byte(16 bit).
	 * format:R(5bit)--G_1(3bit)--G_0(3bit)--B(5bit)
	 * G_0 mean the low 3 bit of G component
	 * G_1 mean the high 3 bit of G component
	 *  */
	DBI_SRC_GRBG_0 = 6,
	DBI_SRC_GRBG_1 = 7,
	DBI_SRC_GBRG_0 = 8,
	DBI_SRC_GBRG_1 = 9,
}dbi_source_format_t;

typedef struct
{
	u32 cmd_typ;//Command type : 0:write command, 1 read command
	u32 write_dump;//Write Command dummy cycles
	u32 output_data_sequence;//Output data sequence: 0 MSB first,1 LSB first
	u32 rgb_seq;//Output RGB sequence
	u32 transmit_mode;//Transmit Mode:0 conmmand /Parameter
	dbi_output_data_format_t dbi_output_data_format;//output Data Format
	dbi_interface_t dbi_interface;//DBI interface 
	dbi_source_format_t dbi_source_format;
	u32 dump_value;//dummy cycl value,output value during cycle
	u32 rgb_bit_order;//RGB bit order
	u32 epos;//Element position  only for RGB32 Data Format
	u32 rgb_src_typ;//RGB Source type :0 RGB 32,1 RGB16

	hal_dbi_en_mode_t dbi_en_mode;//DBI enable mode select:0 always on mode;1software trgger mode;2 timer trgger mode; 3 te trigger mode
	u32 rgb666_format;//2 Ddata line RGB666 format
	u32 dbi_rx_clk_inv;//DBI rx clock inverse:0 、1
	u32 dbi_output_clk_mode;//DBI output clock mode
	u32 dbi_clk_output_inv;// DBI clock output inverse
	u32 dcx_data;//DCX data value: 0:DCX value equal to 0;1 DCX value equal to 1
	u32 rgb16_pix0_post;//RGB 16 data source sel:1 pixell in hight,0 pixell in low
	u32 read_msb_or_lsb;//bit order of read data:0:read data is hight,1 read data is low
	u32 read_dump;//read comman dummy cycles
	u32 read_num;//Read data number of bytes

	u32 ahb_ready_bypass;//AHB ready bypass
	u32 sdi_out_select;//DBI SDI pin output select: 0 ouput WRX,1 output DCX
	u32 dcx_select;//DBI DCX pin function select:0 DBI DBX function,1 WRX
	u32 sdi_pin_select;//DBI SDI pin function select : 0,DBI_SDI or WRX,1:DBI_TE,2DBI_DCX
	u32 te_deb_select;//TE debounce function select :0 debounce,1 no-debounce
	u32 te_edge_select;// TE edge trgger select : 0 TE rising edge,1 falling edge
	u32 te_en;//te enable:0 disable;1 enable

	u32 v_size;
	u32 h_size;
	u32 clock_frequency;
} hal_dbi_config_t;

typedef struct
{
    const uint8_t *tx_buf; /**< Data buffer to send, */
    u32 tx_len; /**< The total number of bytes to send. */
    u32
    tx_single_len; /**< The number of bytes to send in single mode. */
    uint8_t *rx_buf;   /**< Received data buffer, */
    u32 rx_len;   /**< The valid number of bytes received. */
    uint8_t tx_nbits : 3;  /**< Data buffer to send in nbits mode */
    uint8_t rx_nbits : 3;  /**< Data buffer to received in nbits mode */
    uint8_t dummy_byte;    /**< Flash send dummy byte, default 0*/
#define SPI_NBITS_SINGLE 0x01  /* 1bit transfer */
#define SPI_NBITS_DUAL 0x02    /* 2bit transfer */
#define SPI_NBITS_QUAD 0x04    /* 4bit transfer */
    uint8_t bits_per_word; /**< transfer bit_per_word */
} hal_dbi_master_transfer_t;

typedef struct di_dma
{
    struct dma_slave_config config;
    struct sunxi_dma_chan *chan;
} dbi_dma_t;

typedef enum dbi_mode_type
{
    DBI_MODE_TYPE_RX,
    DBI_MODE_TYPE_TX,
    DBI_MODE_TYPE_NULL,
} dbi_mode_type_t;

typedef struct sunxi_dbi
{
	int8_t result : 2;
#define SPI_XFER_READY 0
#define SPI_XFER_OK 1
#define SPI_XFER_FAILED -1

    bool sem;
    uint16_t irqnum;
    unsigned long base;
    dbi_mode_type_t mode_type;

    hal_clk_t pclk; /* PLL clock */
    hal_clk_t bus_clk; /* BUS clock */
    hal_clk_t mclk; /* spi module clock */
	struct reset_control *reset;

    gpio_pin_t *pin;

    dbi_dma_t dma_rx;
    dbi_dma_t dma_tx;

    char *align_dma_buf;
#define ALIGN_DMA_BUF_SIZE (4096 + 64)

    hal_sem_t xSemaphore_tx;
    hal_sem_t xSemaphore_rx;

    hal_dbi_master_port_t port;

    // For user config
    hal_dbi_config_t dbi_reg_cfg;
    hal_dbi_master_transfer_t* transfer;
} sunxi_dbi_t;
// enum dbi_src_seq {
// 	DBI_SRC_RGB = 0,
// 	DBI_SRC_RBG = 1,
// 	DBI_SRC_GRB = 2,
// 	DBI_SRC_GBR = 3,
// 	DBI_SRC_BRG = 4,
// 	DBI_SRC_BGR = 5,
// 	/* following definition only for rgb565
// 	 * to change the RGB order in two byte(16 bit).
// 	 * format:R(5bit)--G_1(3bit)--G_0(3bit)--B(5bit)
// 	 * G_0 mean the low 3 bit of G component
// 	 * G_1 mean the high 3 bit of G component
// 	 *  */
// 	DBI_SRC_GRBG_0 = 6,
// 	DBI_SRC_GRBG_1 = 7,
// 	DBI_SRC_GBRG_0 = 8,
// 	DBI_SRC_GBRG_1 = 9,
// };
// enum dbi_out_seq {
// 	DBI_OUT_RGB = 0,
// 	DBI_OUT_RBG = 1,
// 	DBI_OUT_GRB = 2,
// 	DBI_OUT_GBR = 3,
// 	DBI_OUT_BRG = 4,
// 	DBI_OUT_BGR = 5,
// };

enum dbi_te_en {
	DBI_TE_DISABLE = 0,
	DBI_TE_RISING_EDGE = 1,
	DBI_TE_FALLING_EDGE = 2,
};

// typedef struct {
// 	enum dbi_src_seq	dbi_src_sequence;
// 	enum dbi_out_seq	dbi_out_sequence;
// 	char dbi_rgb_bit_order;
// 	char dbi_rgb32_alpha_pos;
// 	char dbi_rgb16_pixel_endian;
// 	char dbi_format; /*DBI OUT format*/
// 	char dbi_interface;
// 	u16	 dbi_mode;
// 	char dbi_clk_out_mode;
// 	u16			dbi_video_v;
// 	u16			dbi_video_h;
// 	enum dbi_te_en          dbi_te_en;
// 	void			(*dbi_vsync_handle)(unsigned long data);
// 	char			dbi_read_bytes;
// }hal_dbi_config;

typedef enum
{
    DBI_MASTER_ERROR = -6,       /**< DBI master function error occurred. */
    DBI_MASTER_ERROR_NOMEM = -5, /**< DBI master request mem failed. */
    DBI_MASTER_ERROR_TIMEOUT = -4, /**< DBI master xfer timeout. */
    DBI_MASTER_ERROR_BUSY = -3,    /**< DBI master is busy. */
    DBI_MASTER_ERROR_PORT = -2,    /**< DBI master invalid port. */
    DBI_MASTER_INVALID_PARAMETER = -1,       /**< DBI master invalid input parameter. */
    DBI_MASTER_OK = 0 /**< DBI master operation completed successfully. */
} dbi_master_status_t;

dbi_master_status_t hal_set_dbi_config(hal_dbi_master_port_t port,hal_dbi_config_t *cfg);
dbi_master_status_t hal_dbi_init(hal_dbi_master_port_t port,hal_dbi_config_t *cfg);
void dbi_send_cmd(hal_dbi_master_port_t port,u8 cmd);
dbi_master_status_t hal_dbi_read(hal_dbi_master_port_t port,void *buf, uint32_t size);
void dbi_send_two_cmd(hal_dbi_master_port_t port,u8 cmd1, u8 cmd2);
void dbi_send_para(hal_dbi_master_port_t port,u8 para);
dbi_master_status_t dbi_send_pixel(hal_dbi_master_port_t port, u32 bufaddr);
dbi_master_status_t hal_dbi_deinit(hal_dbi_master_port_t port);


// Code editted by Gilbert
dbi_master_status_t hal_dbi_send_cmd(hal_dbi_master_port_t port,void *buf,u32 size);
dbi_master_status_t hal_dbi_send_para(hal_dbi_master_port_t port,void *buf,u32 size);
dbi_master_status_t hal_dbi_xfer(hal_dbi_master_port_t port, hal_dbi_master_transfer_t *transfer);
#ifdef __cplusplus
}
#endif
#endif
