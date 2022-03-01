//*****************************************************************************
//  All Winner Micro, All Right Reserved. 2006-2008 Copyright (c)
//
//  File:               ss_test.c
//
//  Description:  This file implements basic test for SS (Security System)
//
//  History:
//              2009/04/09      Berg Xing       0.10    Initial version
//
//*****************************************************************************
#include "ss_sys.h"
#include "ss_hal.h"
#include "ss_reg.h"

//*****************************************************************************
//  void ss_aesdec(unsigned long* text_in, unsigned long* text_out, unsigned long text_size)
//  Description:
//
//  Arguments:      unsigned long* text_in  Pointer to input buffer
//                unsigned long* text_out Pointer to output buffer
//                unsigned long text_size Text size in BYTE unit. It must be 16-bytes aligned.
//
//  Return Value:   0-fail, 1-success
//*****************************************************************************
unsigned long ss_aesdec(unsigned long *text_in, unsigned long *text_out, unsigned long text_size)
{
    unsigned long i, j;
    unsigned long io_buf[16];
    unsigned long key_size = 256;

    //system open for SS
    ss_sys_open()   ;
    //SS engine init
    ss_init();

    //set SS debug flag
    ss_debug();

    //set encrypt mode
    io_buf[0] = 0x0;            //AES mode
    io_buf[1] = 0x1;            //decryption
    io_buf[2] = key_size;
    io_buf[3] = 0x0;            //ECB mode
    io_buf[4] = 0x0;
    ss_io_ctl(SSIO_SETUP_AES_DES, io_buf);

    //set decrypt key
    io_buf[0] = key_size / 32;
    for (i = 0; i < (key_size / 32); i++)
    {
        io_buf[i + 1] = 4 * i;
        io_buf[i + 1] |= (4 * i + 1) << 8;
        io_buf[i + 1] |= (4 * i + 2) << 16;
        io_buf[i + 1] |= (4 * i + 3) << 24;
    }
    ss_io_ctl(SSIO_SET_KEY, io_buf);

    //start SS
    ss_io_ctl(SSIO_START, io_buf);

    //send cipher text to SS
    i = 0;
    j = 0;
    do
    {
        if (ss_get_rxfifo_room_size() > 0)
        {
            ss_send_data(text_in[i]);
            i++;
        }
        if (ss_get_txfifo_data_cnt() > 0)
        {
            ss_receive_data(&text_out[j]);
            j++;
        }
    } while (i < (text_size / 4));

    if (j < (text_size / 4))
    {
        do
        {
            if (ss_get_txfifo_data_cnt() > 0)
            {
                ss_receive_data(&text_out[j]);
                j++;
            }
        } while (j < (text_size / 4));
    }

    //SS engie exit
    ss_exit();
    //system close for SS
    ss_sys_close();

    return 1;
}

