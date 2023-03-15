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
#include <getopt.h>
#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sunxi_hal_spi.h>
#include <sunxi_drv_spi.h>


static hal_spi_master_config_t cfg =
{
    .clock_frequency = 10*1000*1000,//10MHz
    .slave_port = HAL_SPI_MASTER_SLAVE_0,
    .cpha = HAL_SPI_MASTER_CLOCK_PHASE0,
    .cpol = HAL_SPI_MASTER_CLOCK_POLARITY0,
    //.bit_order = HAL_SPI_MASTER_LSB_FIRST,//LSB
};//SPI configure

static int cmd_drv_spi(int argc, const char **argv)
{
    char tbuf[10]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char rbuf[10];
    hal_spi_master_transfer_t tr;
    tr.tx_buf = tbuf;
    tr.tx_len = 10;
    tr.rx_buf = rbuf;
    tr.rx_len = 10;
    tr.tx_single_len = tr.tx_len;
    tr.dummy_byte = 0;
    tr.rx_nbits = tr.tx_nbits = SPI_NBITS_SINGLE;
    rt_device_t fd;

    fd = rt_device_find("spi1");
    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
    rt_device_control(fd, SPI_CONFIG, &cfg);
    rt_device_write((rt_device_t)fd, 0, tbuf, 10);
    rt_device_read((rt_device_t)fd, 0, tbuf, 10);
    printf("SPI1 cmd receive  data is %s \n", rbuf);
    tbuf[0]=0xAA;tbuf[1]=0xBB;
    rt_device_control(fd, SPI_WRITE_READ, &tr);
    rt_device_close((rt_device_t)fd);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_drv_spi, __cmd_drvspi, rtthread spi test code);


#define TEST_READ 0
#define TEST_WRITE 1
static int cmd_hal_spi(int argc, const char **argv)
{
    hal_spi_master_port_t  port;
    hal_spi_master_config_t cfg;
    char tbuf[10]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char rbuf[10];
    char rw = 1;
    int c;

    if (argc < 3)
    {
        printk("Usage:\n");
        printk("\tspi <port> <-r|-w> \n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    while ((c = getopt(argc, (char *const *)argv, "r:w")) != -1)
    {
        switch (c)
        {
            case 'r':
                rw = TEST_READ;
                break;
            case 'w':
                rw = TEST_WRITE;
                break;
        }
    }

    cfg.clock_frequency = 50;
    cfg.slave_port = HAL_SPI_MASTER_SLAVE_0;
    cfg.cpha = HAL_SPI_MASTER_CLOCK_PHASE0;
    cfg.cpol = HAL_SPI_MASTER_CLOCK_POLARITY0;
    hal_spi_init(port, &cfg);
    //hal_spi_control(port, I2C_SLAVE, &addr);
    if (rw == TEST_READ)
    {
        hal_spi_read(port, rbuf, 10);
        printk("rbuf: %s\n", rbuf);
    }
    else if (rw == TEST_WRITE)
    {
        hal_spi_write(port, tbuf, 10);
    }
    hal_spi_deinit(port);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_spi, __cmd_spi, hal spi test code);

