/*
 * =====================================================================================
 *
 *       Filename:  isptool_terminal.c
 *
 *    Description:  terminal for isp tools communication script
 *
 *        Version:  1.0
 *        Created:  2020年10月09日
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  chenweiming
 *   Organization:
 *
 * =====================================================================================
 */
#include <cli_console.h>
#include <stdint.h>
#include <libc.h>

int isptool_terminal_init();

int isptool_console_write(const void * buf, size_t len, void * private_data);
int isptool_console_read(void * buf, size_t len, void * private_data);

int isptool_console_init(void * private_data);
int isptool_console_deinit(void * private_data);

