#include <stdlib.h>
#include <stdio.h>
#include <cli_console.h>
#include <stdint.h>

#define GDB_CONSOLEBUF_SIZE 1024

int check_gdb_connected(void);
void gdb_msg_write(const char *s, int len);

static int gdb_console_write(const char *s, size_t count, void *private_data)
{
    if (!check_gdb_connected())
    {
        return count;
    }

    gdb_msg_write(s, count);
    return count;
}

int gdb_printf(const char *fmt, ...)
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
    return length;
}

int gdb_console_read(void *buf, size_t len, void *privata_data)
{
    gdb_printf("not supprt read function for gdb_console\n");
    while (1);
    return 1;
}

static int gdb_console_init(void *private_data)
{
    return 1;
}

static int gdb_console_deinit(void *private_data)
{
    return 1;
}

static device_console gdb_console =
{
    .name = "gdb-console",
    .write = gdb_console_write,
    .read = gdb_console_read,
    .init = gdb_console_init,
    .deinit = gdb_console_deinit
};
