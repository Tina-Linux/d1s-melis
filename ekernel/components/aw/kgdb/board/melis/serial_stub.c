#include <stddef.h>
#include <stdint.h>

#define APB_USART0_BASE                           (0x05000000)
#define APB_USART1_BASE                           (0x05000400)
#define APB_USART2_BASE                           (0x05000800)
#define APB_USART3_BASE                           (0x05000c00)

#define UART_RHB (0x00)
#define UART_RBR (0x00)         /* receive buffer register */
#define UART_THR (0x00)         /* transmit holding register */
#define UART_DLL (0x00)         /* divisor latch low register */
#define UART_DLH (0x04)         /* diviso latch high register */
#define UART_IER (0x04)         /* interrupt enable register */
#define UART_IIR (0x08)         /* interrupt identity register */
#define UART_FCR (0x08)         /* FIFO control register */
#define UART_LCR (0x0c)         /* line control register */
#define UART_MCR (0x10)         /* modem control register */
#define UART_LSR (0x14)         /* line status register */
#define UART_MSR (0x18)         /* modem status register */
#define UART_SCH (0x1c)         /* scratch register */
#define UART_USR (0x7c)         /* status register */
#define UART_TFL (0x80)         /* transmit FIFO level */
#define UART_RFL (0x84)         /* RFL */
#define UART_HALT (0xa4)        /* halt tx register */
#define UART_RS485 (0xc0)       /* RS485 control and status register */

int32_t esSIOS_putchar(char data);

static uint32_t sunxi_uart_port[] =
{
    APB_USART0_BASE, APB_USART1_BASE, APB_USART2_BASE, APB_USART3_BASE
};

static uint8_t gdb_SIOS_getchar(void)
{
    int ch = -1;
    int devid = 0;
    volatile uint32_t *rec_buf;
    volatile uint32_t *sta;
    volatile uint32_t *fifo;

    rec_buf = (uint32_t *)(sunxi_uart_port[devid] + UART_RHB);
    sta = (uint32_t *)(sunxi_uart_port[devid] + UART_USR);
    fifo = (uint32_t *)(sunxi_uart_port[devid] + UART_RFL);

    while (!(*fifo & 0x1ff));

    /* Receive Data Available */
    if (*sta & 0x08)
    {
        ch = *rec_buf & 0xff;
    }

    return ch;
}

int gdb_uart_getc(void)
{
    int ch;

    ch = -1;
    do
    {
        ch = gdb_SIOS_getchar();
    } while (ch == -1);

    return ch;
}

void gdb_uart_putc(char c)
{
    esSIOS_putchar(c);
}
