#include <kgdb_util.h>

static const char hexchars[] = "0123456789abcdef";
void *volatile gdb_mem_fault_handler = (void *)0;

long probe_kernel_write(void *dst, void *src, size_t size)
{
    int i = 0;
    char *dst_ptr = (char *)dst;
    char *src_ptr = (char *)src;

    gdb_mem_fault_handler = && err;

    for (i = 0; i < size; i++)
    {
        *(dst_ptr++) = *(src_ptr++);
    }

    gdb_mem_fault_handler = (void *)0;
    return 0;
err:
    gdb_mem_fault_handler = (void *)0;
    return -1;
}

int hex(char ch)
{
    if ((ch >= 'a') && (ch <= 'f'))
    {
        return ch - 'a' + 10;
    }
    if ((ch >= '0') && (ch <= '9'))
    {
        return ch - '0';
    }
    if ((ch >= 'A') && (ch <= 'F'))
    {
        return ch - 'A' + 10;
    }
    return -1;
}

char tohex(char c)
{
    return hexchars[c & 15];
}

/*
 * Copy the binary array pointed to by buf into mem.  Fix $, #, and
 * 0x7d escaped with 0x7d.  Return a pointer to the character after
 * the last byte written.
 */
int gdb_ebin2mem(char *buf, char *mem, int count)
{
    int err = 0;
    char c;

    while (count-- > 0)
    {
        c = *buf++;
        if (c == 0x7d)
        {
            c = *buf++ ^ 0x20;
        }
        err = probe_kernel_write(mem, &c, 1);
        if (err)
        {
            break;
        }

        mem++;
    }

    return err;
}

/*
 * Convert the hex array pointed to by buf into binary to be placed in mem.
 * Return a pointer to the character AFTER the last byte written.
 * May return an error.
 */
int gdb_hex2mem(char *buf, char *mem, int count)
{
    char *tmp_raw;
    char *tmp_hex;

    tmp_raw = buf + count * 2;

    tmp_hex = tmp_raw - 1;
    while (tmp_hex >= buf)
    {
        tmp_raw--;
        *tmp_raw = hex(*tmp_hex--);
        *tmp_raw |= hex(*tmp_hex--) << 4;
    }

    return probe_kernel_write(mem, tmp_raw, count);
}
/*
 * Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null). May return an error.
 */
int gdb_mem2hex(char *mem, char *buf, int count)
{
    char *tmp = mem;
    char ch;

    gdb_mem_fault_handler = && err;

    while (count > 0)
    {
        ch = *(tmp++);
        *(buf++) = tohex((ch >> 4) & 0xf);
        *(buf++) = tohex(ch & 0xf);

        count--;
    }
    *buf = 0;

    gdb_mem_fault_handler = (void *)0;
    return 0;
err:
    gdb_mem_fault_handler = (void *)0;
    return -1;
}

/*
 * While we find nice hex chars, build a long_val.
 * Return number of chars processed.
 */
int gdb_hex2long(char **ptr, unsigned long *long_val)
{
    int hex_val;
    int num = 0;
    int negate = 0;

    *long_val = 0;

    if (**ptr == '-')
    {
        negate = 1;
        (*ptr)++;
    }
    while (**ptr)
    {
        hex_val = hex(**ptr);
        if (hex_val < 0)
        {
            break;
        }

        *long_val = (*long_val << 4) | hex_val;
        num++;
        (*ptr)++;
    }

    if (negate)
    {
        *long_val = -*long_val;
    }

    return num;
}
