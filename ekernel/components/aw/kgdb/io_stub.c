#include "gdb_stub.h"

void gdb_uart_putc(char c);
int gdb_uart_getc(void);

struct gdb_io gdb_io_ops =
{
    .read_char = gdb_uart_getc,
    .write_char = gdb_uart_putc,
};
