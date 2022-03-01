//*****************************************************************************
//  All Winner Micro, All Right Reserved. 2006-2008 Copyright (c)
//
//  File:               ss_hal.c
//
//  Description:  This file implements basic functions for SS (Security System)
//
//  History:
//              2009/04/09      Berg Xing       0.10    Initial version
//
//*****************************************************************************
#include "ss_reg.h"
#include "ss_hal.h"

//*****************************************************************************
//  void ss_init(void)
//  Description:    Start SS
//
//  Arguments:      none
//
//  Return Value:   None
//*****************************************************************************
void ss_init(void)
{
}
//*****************************************************************************
//  void ss_exit(void)
//  Description:    Disable SS
//
//  Arguments:      none
//
//  Return Value:   None
//*****************************************************************************
void ss_exit(void)
{
    unsigned long reg_val;

    reg_val = ss_read_w(SS_CTL);
    reg_val &= ~0x1;
    ss_write_w(SS_CTL, reg_val);
}
//*****************************************************************************
//  unsigned long ss_io_ctl(unsigned long io_type, unsigned long* io_data)
//  Description:    Multi IO control function for SS
//
//  Arguments:      unsigned long io_type
//                unsigned long* io_data
//
//  Return Value:   None
//*****************************************************************************
unsigned long ss_io_ctl(unsigned long io_type, unsigned long *io_data)
{
    unsigned long i, k;
    unsigned long reg_val;

    switch (io_type)
    {
        case SSIO_SET_KEY:
            k = *io_data;           //key number in word unit
            for (i = 0; i < k; i++)
            {
                ss_write_w(SS_KEY + 4 * i, *(io_data + i + 1));
            }
            break;
        case SSIO_SET_IV:
            k = *io_data;           //IV number in word unit
            if (k > 4)
            {
                for (i = 0; i < 4; i++)
                {
                    ss_write_w(SS_IV + 4 * i, *(io_data + i + 1));
                }
                ss_write_w(SS_CNT, *(io_data + 5));
            }
            else
            {
                for (i = 0; i < k; i++)
                {
                    ss_write_w(SS_IV + 4 * i, *(io_data + i + 1));
                }
            }
            break;
        case SSIO_SET_CNT:
            k = *io_data;           //CNT number in word unit
            for (i = 0; i < k; i++)
            {
                ss_write_w(SS_CNT + 4 * i, *(io_data + i + 1));
            }
            break;
        case SSIO_SETUP_AES_DES:
            reg_val = ss_read_w(SS_CTL);
            reg_val &= ~(0x7 << 4);
            reg_val |= (*io_data & 0x7) << 4;   //0: AES, 1: DES, 2: 3DES
            reg_val &= ~(0x1 << 7);
            if (*(io_data + 1))
            {
                reg_val |= 0x1 << 7;    //decryption
            }
            reg_val &= ~(0x3 << 8);
            if (*(io_data + 2) == 256)
            {
                reg_val |= 0x2 << 8;    //256 bits key
            }
            else if (*(io_data + 2) == 192)
            {
                reg_val |= 0x1 << 8;    //192 bits key
            }
            else
            {
                reg_val |= 0x0 << 8;    //128 bits key
            }
            reg_val &= ~(0x3 << 12);
            reg_val |= (*(io_data + 3) & 0x3) << 12;    //operation mode(ECB, CBC, CNT)
            reg_val &= ~(0x3 << 10);
            if (*(io_data + 4) == 64)
            {
                reg_val |= 0x2 << 10;    //64 bits CNT width for CNT mode
            }
            else if (*(io_data + 4) == 32)
            {
                reg_val |= 0x1 << 10;    //32 bits CNT width for CNT mode
            }
            else
            {
                reg_val |= 0x0 << 10;    //16 bits CNT width for CNT mode
            }
            ss_write_w(SS_CTL, reg_val);
            break;
        case SSIO_SETUP_SHA1_MD5:
            reg_val = ss_read_w(SS_CTL);
            reg_val &= ~(0x7 << 4);
            if ((*io_data & 0x7) == 0)
            {
                reg_val |= 0x3 << 4;    //SHA1
            }
            else if ((*io_data & 0x7) == 1)
            {
                reg_val |= 0x4 << 4;    //MD5
            }
            reg_val &= ~(0x1 << 14);
            if (*(io_data + 1))
            {
                reg_val |= 0x1 << 14;    //IV steady of its constants
            }
            ss_write_w(SS_CTL, reg_val);
            break;
        case SSIO_START:
            reg_val = ss_read_w(SS_CTL);
            reg_val |= 0x1;
            ss_write_w(SS_CTL, reg_val);
            break;
        case SSIO_STOP:
            reg_val = ss_read_w(SS_CTL);
            reg_val &= ~0x1;
            ss_write_w(SS_CTL, reg_val);
            break;
        case SSIO_ENABLE_DMA:
            reg_val = ss_read_w(SS_ICSR);
            reg_val |= 0x1 << 4;
            ss_write_w(SS_ICSR, reg_val);
            break;
        case SSIO_DISABLE_DMA:
            reg_val = ss_read_w(SS_ICSR);
            reg_val &= ~(0x1 << 4);
            ss_write_w(SS_ICSR, reg_val);
            break;
        case SSIO_TX_INT:
            reg_val = ss_read_w(SS_ICSR);
            if (*(io_data))
            {
                reg_val |= 0x1;
            }
            else
            {
                reg_val &= ~0x1;
            }
            ss_write_w(SS_ICSR, reg_val);
            break;
        case SSIO_RX_INT:
            reg_val = ss_read_w(SS_ICSR);
            if (*(io_data))
            {
                reg_val |= 0x1 << 2;
            }
            else
            {
                reg_val &= ~(0x1 << 2);
            }
            ss_write_w(SS_ICSR, reg_val);
            break;
        case SSIO_RX_TRIGGER_LEVEL:
            reg_val = ss_read_w(SS_FCSR);
            reg_val &= ~(0x1f << 8);
            reg_val |= ((*io_data - 1) & 0x1f) << 8;
            ss_write_w(SS_FCSR, reg_val);
            break;
        case SSIO_TX_TRIGGER_LEVEL:
            reg_val = ss_read_w(SS_FCSR);
            reg_val &= ~0x1f;
            reg_val |= (*io_data - 1) & 0x1f;
            ss_write_w(SS_FCSR, reg_val);
            break;

        default:
            break;
    }
    return (1);
}

//*****************************************************************************
//  unsigned long ss_get_rxfifo_room_size(void)
//  Description:    Get RX FIFO empty room in word unit
//                              The 32 words RX FIFO is used for sending data (plaintext/ciphertext)
//                to SS engine
//  Arguments:      None
//
//
//  Return Value:   RX FIFO empry room size in word unit
//*****************************************************************************
unsigned long ss_get_rxfifo_room_size(void)
{
    unsigned long reg_val;

    reg_val = ss_read_w(SS_FCSR);
    reg_val = reg_val >> 24;
    reg_val &= 0x3f;

    return reg_val;
}

//*****************************************************************************
//  unsigned long ss_get_txfifo_data_cnt(void)
//  Description:    Get TX FIFO available data counter in word unit
//                              The 32 words TX FIFO is used for receiving data (plaintext/ciphertext)
//                from SS engine
//  Arguments:      None
//
//
//  Return Value:   TX FIFO available data counter in word unit
//*****************************************************************************
unsigned long ss_get_txfifo_data_cnt(void)
{
    unsigned long reg_val;

    reg_val = ss_read_w(SS_FCSR);
    reg_val = reg_val >> 16;
    reg_val &= 0x3f;

    return reg_val;
}

//*****************************************************************************
//  void ss_send_data(unsigned long data)
//  Description:
//                              Sending one word data (plaintext/ciphertext) to SS engine
//  Arguments:      None
//
//
//  Return Value:   void
//*****************************************************************************
void ss_send_data(unsigned long data)
{
    ss_write_w(SS_RXFIFO, data);
}
//*****************************************************************************
//  void ss_send_data(unsigned long data)
//  Description:
//                              Sending one word data (plaintext/ciphertext) to SS engine
//  Arguments:      None
//
//
//  Return Value:   void
//*****************************************************************************
void ss_receive_data(unsigned long *data)
{
    *data = ss_read_w(SS_TXFIFO);
}


void ss_debug(void)
{
    unsigned long reg_val;

    reg_val = ss_read_w(SS_CTL);
    reg_val |= ((unsigned long)0x1 << 31);
    ss_write_w(SS_CTL, reg_val);
}
