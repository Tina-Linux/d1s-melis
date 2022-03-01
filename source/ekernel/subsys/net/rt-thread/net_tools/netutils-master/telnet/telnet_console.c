/*
 * =====================================================================================
 *
 *       Filename:  telnet_console.c
 *
 *    Description:  telnet console for multi-console
 *
 *        Version:  1.0
 *        Created:  2020年06月22日 14时29分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zeng Zhijin
 *   Organization:
 *
 * =====================================================================================
 */
#include <cli_console.h>
#include <stdint.h>
#include <libc.h>

extern rt_size_t telnet_session_read(void *sesssion, rt_off_t pos, void* buffer, rt_size_t size);
extern rt_size_t telnet_session_write (void *session, rt_off_t pos, const void* buffer, rt_size_t size);

static int telnet_console_write(const void * buf, size_t len, void * private_data)
{
    return telnet_session_write(private_data, 0, buf, len);
}

static int telnet_console_read(void * buf, size_t len, void * private_data)
{
    return telnet_session_read(private_data, 0, buf, len);
}

static int telnet_console_init(void * private_data)
{
    return 1;
}

static int telnet_console_deinit(void * private_data)
{
    return 1;
}

device_console telnet_console =
{
    .name = "telnet-console",
    .write = telnet_console_write,
    .read = telnet_console_read,
    .init = telnet_console_init,
    .deinit = telnet_console_deinit
};
