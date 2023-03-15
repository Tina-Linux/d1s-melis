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
#include <hal_log.h>
#include <hal_cmd.h>
#include <hal_timer.h>
#include <hal_lpuart.h>
#include <hal_uart.h>

/* find a free uart_port or pc com as source */
#define UART_TEST UART_1
#define TEST_LEN 5

static void cmd_usage(void)
{
	printf("Usage:\n"
		"\t hal_lpuart <port> <baudrate>\n");
}

void test_recv_data(lpuart_port_t port)
{
	printf("enter recv data test\n");
	hal_lpuart_enable_rx_data(port, NULL, NULL);
	/* use uart as source */
	hal_uart_init(UART_TEST);
	hal_uart_send(UART_TEST, "a", 1);
	/* use pc com as source */
	printf("enter\n");
	hal_sleep(5);
	hal_lpuart_disable_rx_data(port);
}

static void compare_callback(void *arg)
{
	printf("data compare success!\n");
}

void test_cmp_data(lpuart_port_t port)
{
	printf("enter cmp data test\n");
	char cmp[TEST_LEN + 1] = "abcde";

	if (hal_lpuart_init(port)) {
		printf("lpuart %d not inited\n", port);
		return;
	}

	hal_lpuart_rx_cmp(port, TEST_LEN, cmp);
	hal_lpuart_enable_rx_cmp(port, compare_callback, NULL);
	/* use uart as source, stop bit of uart should be 2 */
	hal_uart_init(UART_TEST);
	hal_uart_send(UART_TEST, cmp, TEST_LEN);
	/* use pc com as source */
	printf("enter abcde\n");
	hal_sleep(5);
	hal_lpuart_disable_rx_cmp(port);
}

void lpuart_reset_multiplex()
{
	lpuart_multiplex(LPUART_0, UART_0);
	lpuart_multiplex(LPUART_1, UART_1);
}

int cmd_test_lpuart(int argc, char **argv)
{
	if (argc != 3) {
		cmd_usage();
		return -1;
	}

	lpuart_port_t port;
	uint32_t baudrate;

	port = strtol(argv[1], NULL, 0);
	baudrate = strtol(argv[2], NULL, 0);

	if (hal_lpuart_init(port) != SUNXI_HAL_OK) {
		printf("Fail to init lpuart\n");
		return -1;
	}

	if (port == 0) {
		lpuart_multiplex(LPUART_0, UART_TEST);
	} else if (port == 1) {
		lpuart_multiplex(LPUART_1, UART_TEST);
	}
	test_recv_data(port);
	test_cmp_data(port);

	lpuart_reset_multiplex();

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_lpuart, hal_lpuart, lpuart hal APIs tests)
