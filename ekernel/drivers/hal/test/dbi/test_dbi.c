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
#include <time.h>
#include <sys/time.h>

#include <hal_log.h>
#include <hal_cmd.h>
#include <sunxi_hal_dbi.h>

#define TEST_READ 0
#define TEST_WRITE 1
#define DBI_POS_BUF		(0x55000000)
#define DBI_GIRL_BUF    (0x56000000)
#define DBI_HOUSE_BUF   (0x57000000)

#define ARGV_OPT "rw:c:"

#define PRINT_TIME_USED(func)                     \
    do {                                             \
        struct timeval tv;                        \
        int64_t start, end;                          \
        gettimeofday(&tv, NULL);                   \
	start = tv.tv_sec * 1000000 + tv.tv_usec;                       \
        func;                                      \
        gettimeofday(&tv, NULL);                   \
	end = tv.tv_sec * 1000000 + tv.tv_usec;                         \
        printf("function " #func " used usec : %ld \n", end - start);    \
    } while (0);

u32 clk_freq = 1000000;

void dbi_init(hal_dbi_config_t *cfg)
{
#if 1
    memset(cfg, 0, sizeof(hal_dbi_config_t));

    cfg->v_size = 320;
    cfg->h_size = 240;
    cfg->dbi_output_clk_mode = 0;
    cfg->dbi_interface = D2LI;
    cfg->clock_frequency = clk_freq;
    cfg->dbi_output_data_format = DBI_RGB565;
    cfg->rgb_src_typ = 1;
    cfg->ahb_ready_bypass = 1;
#endif
}

int send_cmd_by_dbi(hal_dbi_master_port_t port, hal_dbi_config_t *cfg, void *buf, u32 size)
{
    int ret;
    hal_dbi_master_transfer_t tr;

    /* Set command mode. */
    cfg->dbi_output_data_format = DBI_RGB111;
    cfg->dbi_interface = D2LI;
    cfg->output_data_sequence = 0;

    // 决定发送的方式/内容
    cfg->cmd_typ = DBI_WRITE;
    cfg->transmit_mode = 0;
    cfg->dcx_data = 0;

    cfg->dbi_en_mode = 0;
    cfg->dbi_output_clk_mode = 0;

    // 第一步
    hal_dbi_init(port, cfg);

    // 第二步
    tr.tx_buf = buf;
    tr.tx_len = size;
    tr.rx_buf = NULL;
    tr.rx_len = 0;

    ret = hal_dbi_xfer(port, &tr);
    // 第三步, 释放资源，函数外调用
    return 0;
}

int send_para_by_dbi(hal_dbi_master_port_t port, hal_dbi_config_t *cfg, void *buf, u32 size)
{
    int ret;
    hal_dbi_master_transfer_t tr;

    /* Set command mode. */
    cfg->dbi_output_data_format = DBI_RGB111;
    cfg->dbi_interface	= D2LI;
    cfg->output_data_sequence = 0;

    // 决定发送的方式/内容
    cfg->cmd_typ = DBI_WRITE;
    cfg->transmit_mode = 0;
    cfg->dcx_data = 1;

    cfg->dbi_en_mode = 0;
    cfg->dbi_output_clk_mode = 0;

    // 第一步
    hal_dbi_init(port, cfg);

    // 第二步
    tr.tx_buf = buf;
    tr.tx_len = size;
    tr.rx_buf = NULL;
    tr.rx_len = 0;

    ret = hal_dbi_xfer(port, &tr);
    // 第三步, 释放资源，函数外调用
    return 0;
}

// ARGB8888
#define BUF_SIZE 320 * 240 * 2

int send_pixel_by_dbi(hal_dbi_master_port_t port, hal_dbi_config_t *cfg, void *buf, u32 size)
{
    int ret;
    hal_dbi_master_transfer_t tr;
    dbi_master_status_t init_ret;

#if 0
    void *fbuf = 0; 
	int bpp = 4, j = 0;
	unsigned char red[4] = {0xff,0xff,0x0,0x0};
	fbuf = malloc(BUF_SIZE);

	for (j = 0; j < BUF_SIZE / bpp; ++j) {
		memcpy(fbuf+j*bpp,red, bpp);
	}

#else
    // void *fbuf = malloc(BUF_SIZE); 
    void *fbuf = dma_alloc_coherent(BUF_SIZE); 
    printf("fbuf : %p \n", fbuf);
#endif

#if 1
    /* Set command mode. */
    cfg->dbi_output_data_format = DBI_RGB565;
    cfg->dbi_interface	= D2LI;
    cfg->output_data_sequence = 0;

    cfg->rgb_src_typ = 1;

    // 决定发送的方式/内容
    cfg->cmd_typ = DBI_WRITE;
    cfg->transmit_mode = 1;
    cfg->dcx_data = 1;

//    cfg->dbi_en_mode = 0;
    cfg->dbi_en_mode = 0;
    cfg->dbi_output_clk_mode = 0;

    cfg->te_en = 0;

    // 第一步
    init_ret = hal_dbi_init(port, cfg);
    if (init_ret != 0) {
        printf("DBI init failed! \n");
	return -1;
    }
#endif

    // 第二步
    tr.tx_buf = fbuf;
    tr.tx_len = BUF_SIZE;
    tr.rx_buf = NULL;
    tr.rx_len = 0;

    printf("fbuf : %p, tr.tx_buf: %p \n", fbuf, tr.tx_buf);
    printf("Preparing to test ... \n");
    PRINT_TIME_USED(sleep(2));

    printf("hal_dbi_xfer start \n");
    int i;
    int success_count = 0;
    for (i = 0; i < 3; i++) {
	printf("i : %d \n", i);
        PRINT_TIME_USED(ret = hal_dbi_xfer(port, &tr));

	if (ret != 0)
	    printf("failed! ret : %d \n", ret);

	if (ret == 0)
	    success_count += 1;
    }
    // ret = hal_dbi_xfer(port, &tr);
    // 第三步, 释放资源，函数外调用
    printf("hal_dbi_xfer finish! success_count :%d \n", success_count);
    // free(fbuf);
    return 0;
}

static int cmd_test_dbi(int argc, const char **argv)
{
    hal_dbi_master_port_t  port;
    hal_dbi_config_t cfg;
    // char tbuf[10]={0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20};
    char tbuf[]={0x8f};
    char rbuf[10];
    char rw = 1;
    int c;
    int i;
    int cmd_or_data;

    if (argc < 3)
    {
        printf("Usage:");
        printf("\tdbi <port> <-r|-w optarg>");
        return -1;
    }

    printf("Run dbi test \n");

    port = strtol(argv[1], NULL, 0);
    printf("port : %d \n", port);

    while ((c = getopt(argc, (char *const *)argv, ARGV_OPT)) != -1)
    {
        printf("getopt: %c, optarg : %s \n",c, optarg);
        switch (c)
        {
            case 'r':
                rw = TEST_READ;
                break;
            case 'w': {
                rw = TEST_WRITE;
		cmd_or_data = strtol(optarg, NULL, 10);
                printf("cmd_or_data : %d \n", cmd_or_data);
		break;

            }
            case 'c': {
		clk_freq = strtol(optarg, NULL, 10);
                printf("clk_freq : %u \n", clk_freq);
		break;

            }
        }
    }

    // defauft config
    dbi_init(&cfg);

    //cfg.rgb16_pix0_post=1;
    printf("dbi init OK \n");
    if (rw == TEST_READ)
    {
        hal_dbi_read(port,rbuf, 0x4);
        printf("rbuf: %s \n", rbuf);
    }
    else if (rw == TEST_WRITE)
    {
	if (0 == cmd_or_data) {
            printf("dbi test write cmd\n");
    		send_cmd_by_dbi(port, &cfg, tbuf,sizeof(tbuf));
	} else if (1 == cmd_or_data) {
            printf("dbi test write data\n");
            // send_cmd_by_dbi(port, &cfg, tbuf,sizeof(tbuf));
            send_para_by_dbi(port, &cfg, tbuf, sizeof(tbuf));
	} else {
	    printf("dbi test write pixels data \n");
	    send_pixel_by_dbi(port, &cfg, tbuf, sizeof(tbuf));
	}
    } else
	goto out;

    printf("dbi always on \n");
    // hal_dbi_deinit(port);

out:
    printf("dbi test finish ok\n");
    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_dbi, hal_dbi, dbi hal APIs tests)
