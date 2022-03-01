#include <cli_console.h>
#include <stdint.h>
#include <kconfig.h>
#include <libc.h>
#include <sunxi_drv_uart.h>

rt_device_t fd;

int uart_console_write(const void * buf, size_t len, void * privata_data)
{
    char    * output_buf = (char *)buf;
    int     i = 0;
    char    data;

    while(i < len)
    {
        if (*(output_buf + i) == '\n')
        {
            data = '\r';
            rt_device_write((rt_device_t)fd, 0, &data, 1);
        }
        rt_device_write((rt_device_t)fd, 0, output_buf + i, 1);

        i++;
    }
    return i;
}

/*TODO:  because getchar need 1024 bytes return when going through _read_r, change len = 1*/
int uart_console_read(void * buf, size_t len, void * privata_data)
{
    return rt_device_read((rt_device_t)fd, 0, buf, 1);
}

void sunxi_driver_uart_init(void);
static int uart_console_init(void * private_data)
{
    int uart_port;
    _uart_config_t uart_config;
    char *uart_name = "uartn";

#if defined(CONFIG_ARM)
    /*nothing to do. ARMv7 platform has implimented 'sunxi_driver_uart_init()' function before, 
     *please check the file sysinit.c 'serial_init()' function.
    */
#elif defined(CONFIG_RISCV)
    sunxi_driver_uart_init();
#endif

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.word_length = UART_WORD_LENGTH_8;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.parity      = UART_PARITY_NONE;
    if (esCFG_GetKeyValue("uart_para", "uart_debug_port", (int32_t *)&uart_port, 1) != EPDK_OK)
    {
        uart_port = 0;
    }
    sprintf(uart_name, "uart%d", uart_port);

    fd = rt_device_find(uart_name);
    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
#if defined(CONFIG_ARM)
#elif defined(CONFIG_RISCV)
    rt_device_control(fd, 0, &uart_config);
#endif

    return 1;
}

static int uart_console_deinit(void * private_data)
{
    return 1;
}

static device_console uart_console =
{
    .name       = "uart-console",
    .write      = uart_console_write,
    .read       = uart_console_read,
    .init       = uart_console_init,
    .deinit     = uart_console_deinit
};

cli_console cli_uart_console =
{
    .i_list         = {0},
    .name           = "cli-uart",
    .dev_console    = &uart_console,
    .init_flag      = 0,
    .exit_flag      = 0,
    .alive          = 1,
    .private_data   = NULL,
    .cli_tag        = {0},
    .cli_tag_len    = 0,
    .task_list      = {0},
    .finsh_callback = NULL,
    .start_callback = NULL,
};
