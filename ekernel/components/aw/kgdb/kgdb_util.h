#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

int hex(char ch);
char tohex(char c);
int gdb_ebin2mem(char *buf, char *mem, int count);
int gdb_hex2mem(char *buf, char *mem, int count);
int gdb_mem2hex(char *mem, char *buf, int count);
int gdb_hex2long(char **ptr, unsigned long *long_val);

long probe_kernel_write(void *dst, void *src, size_t size);

#endif  /*UTIL_H*/
