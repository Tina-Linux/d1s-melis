#include <stdlib.h>

#define GDB_CONSOLEBUF_SIZE 1024

void gdb_console_write(const char *s, unsigned count, void *private_data)
{
    if (!gdb_connected)
    {
        return;
    }

    gdb_msg_write(s, count);
}

void gdb_printf(const char *fmt, ...)
{
    va_list args;
    size_t length;
    char gdb_log_buf[GDB_CONSOLEBUF_SIZE];

    memset(gdb_log_buf, 0, GDB_CONSOLEBUF_SIZE);

    va_start(args, fmt);
    length = vsnprintf(gdb_log_buf, sizeof(gdb_log_buf) - 1, fmt, args);
    if (length > GDB_CONSOLEBUF_SIZE - 1)
    {
        length = GDB_CONSOLEBUF_SIZE - 1;
    }
    gdb_console_write(gdb_log_buf, length, NULL);
    va_end(args);
}
