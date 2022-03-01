#ifndef _SS_HAL_H
#define _SS_HAL_H


//macro define for SS IO operation
#define SSIO_SET_KEY            0x00
#define SSIO_SET_IV             0x01
#define SSIO_SET_CNT            0x02
#define SSIO_SETUP_AES_DES      0x03
#define SSIO_SETUP_SHA1_MD5     0x04
#define SSIO_START              0x05
#define SSIO_STOP               0x06
#define SSIO_ENABLE_DMA         0x07
#define SSIO_DISABLE_DMA        0x08
#define SSIO_TX_INT             0x09
#define SSIO_RX_INT             0x0a
#define SSIO_TX_TRIGGER_LEVEL   0x0b
#define SSIO_RX_TRIGGER_LEVEL   0x0c


extern void ss_init(void);
extern void ss_exit(void);
extern void ss_start(void);
extern unsigned long ss_io_ctl(unsigned long io_type, unsigned long *io_data);
extern unsigned long ss_get_rxfifo_room_size(void);
extern unsigned long ss_get_txfifo_data_cnt(void);
extern void ss_send_data(unsigned long data);
extern void ss_receive_data(unsigned long *data);
extern void ss_debug(void);

#endif  //_SS_HAL_H

