/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY'S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS'SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY'S TECHNOLOGY.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <hal_log.h>
#include <hal_cmd.h>
#include <sunxi_hal_dbi.h>

#define TEST_READ 0
#define TEST_WRITE 1
#define DBI_POS_BUF		(0x55000000)
#define DBI_GIRL_BUF    (0x56000000)
#define DBI_HOUSE_BUF   (0x57000000)

static int cmd_test_dbi(int argc, const char **argv)
{
    hal_dbi_master_port_t  port;
    hal_dbi_config_t cfg;
    char tbuf[10]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char rbuf[10];
    char rw = 1;
    int c;
    int i;
    int cmd;

    if (argc < 3)
    {
        hal_log_info("Usage:");
        hal_log_info("\tdbi <port> <-r|-w>");
        return -1;
    }

    hal_log_info("Run dbi test");

    port = strtol(argv[1], NULL, 0);
    while ((c = getopt(argc, (char *const *)argv, "r:w")) != -1)
    {
        printf("getopt: %c\n",c);
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
    cfg.v_size=320;
    cfg.h_size=4;
    cfg.dbi_output_clk_mode=1;
    cfg.dbi_interface=L4I1;
    cfg.clock_frequency=5000000;
    cfg.dbi_output_data_format=DBI_RGB444;
    cfg.rgb_src_typ=1;
    cfg.ahb_ready_bypass=1;
    //cfg.rgb16_pix0_post=1;
    hal_dbi_init(port, &cfg);
    hal_log_info("dbi init OK");
    if (rw == TEST_READ)
    {
        //hal_dbi_read(port,8, rbuf, 0x4);
        hal_log_info("rbuf: %s", rbuf);
    }
    else if (rw == TEST_WRITE)
    {
        hal_dbi_send_cmd(port,&tbuf,sizeof(tbuf));
        //hal_dbi_send_para(port,&tbuf,sizeof(tbuf));
    }
    hal_dbi_deinit(port);
    hal_log_info("dbi test finish ok");
    return 0;
}

// FINSH_FUNCTION_EXPORT_CMD(cmd_test_dbi, hal_dbi, dbi hal APIs tests)
