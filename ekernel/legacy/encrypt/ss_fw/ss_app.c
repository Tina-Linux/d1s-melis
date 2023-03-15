/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
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

