/*
 * ===========================================================================================
 *
 *       Filename:  csp_uart.c
 *
 *    Description:  uart csp impl.
 *
 *        Version:  Melis3.0
 *         Create:  2020-06-23 18:37:54
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-02 14:37:57
 *
 * ===========================================================================================
 */

#include <../sbi/sbi.h>

void sbi_putc(void *port, int c)
{
    sbi_console_putchar(c);
}

int uart_put_buffer(const char *buf, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        sbi_console_putchar(buf[i]);
    }

    return i;
}

int uart_get_buffer(char *buf, int count)
{
    int i, c;

    void delay_10insn(void);
    for (i = 0; i < count; i++)
    {
retry:
        c = sbi_console_getchar();
        if (c < 0)
        {
            delay_10insn();
            delay_10insn();
            delay_10insn();
            goto retry;
        }

        buf[i] = c;
    }

    return i;
}
