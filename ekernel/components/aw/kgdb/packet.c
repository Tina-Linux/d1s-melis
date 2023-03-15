/*
 * GDB stub packet process.
 *
 * Migarte form linux to rt-thread by Wzyy2
 * Original edition : KGDB stub
 *
 * File      : gdb_stub.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-07-04     Wzyy2      first version
 * 2019-12-25     ZengZhijin refactor to support hw breakpoint/watchpoint
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * KGDB stub.
 *
 * Maintainer: Jason Wessel <jason.wessel@windriver.com>
 *
 * Copyright (C) 2000-2001 VERITAS Software Corporation.
 * Copyright (C) 2002-2004 Timesys Corporation
 * Copyright (C) 2003-2004 Amit S. Kale <amitkale@linsyssoft.com>
 * Copyright (C) 2004 Pavel Machek <pavel@suse.cz>
 * Copyright (C) 2004-2006 Tom Rini <trini@kernel.crashing.org>
 * Copyright (C) 2004-2006 LinSysSoft Technologies Pvt. Ltd.
 * Copyright (C) 2005-2008 Wind River Systems, Inc.
 * Copyright (C) 2007 MontaVista Software, Inc.
 * Copyright (C) 2008 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * Contributors at various stages not listed above:
 *  Jason Wessel ( jason.wessel@windriver.com )
 *  George Anzinger <george@mvista.com>
 *  Anurekh Saxena (anurekh.saxena@timesys.com)
 *  Lake Stevens Instrument Division (Glenn Engel)
 *  Jim Kingdon, Cygnus Support.
 *
 * Original KGDB stub: David Grothe <dave@gcom.com>,
 * Tigran Aivazian <tigran@sco.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <gdb_stub.h>
#include <breakpoint.h>
#include "kgdb_util.h"

extern struct gdb_arch arch_gdb_ops;

struct gdb_state
{
    int signo;
    int pass_exception;
} gs;

/* flag for connected to host gdb */
static int gdb_connected;

/* Storage for the registers, in GDB format. */
static unsigned long gdb_regs[(NUMREGBYTES + sizeof(unsigned long) - 1) / sizeof(unsigned long)];

char remcom_in_buffer[BUFMAX];
char remcom_out_buffer[BUFMAX];

static char gdbmsgbuf[BUFMAX + 1];

int check_gdb_connected(void)
{
    return gdb_connected;
}

static int write_mem_msg(int binary)
{
    char *ptr = &remcom_in_buffer[1];
    unsigned long addr;
    unsigned long length;
    int err;

    if (gdb_hex2long(&ptr, &addr) > 0 && *(ptr++) == ',' &&
        gdb_hex2long(&ptr, &length) > 0 && *(ptr++) == ':')
    {
        if (kgdb_permit_data_access(addr, length))
        {
            return -1;
        }

        if (binary)
        {
            err = gdb_ebin2mem(ptr, (char *)addr, length);
        }
        else
        {
            err = gdb_hex2mem(ptr, (char *)addr, length);
        }

        if (err)
        {
            return err;
        }

        if (CACHE_FLUSH_IS_SAFE)
        {
            gdb_flush_icache_range(addr, addr + length);
        }
        return 0;
    }

    return -1;
}

/*
 * Send the packet in buffer.
 * Check for gdb connection if asked for.
 */
static void put_packet(char *buffer)
{
    unsigned char checksum;
    int count;
    char ch;

    /*
     * $<packet info>#<checksum>.
     */
    while (1)
    {
        gdb_io_ops.write_char('$');
        checksum = 0;
        count = 0;

        while ((ch = buffer[count]))
        {
            gdb_io_ops.write_char(ch);
            checksum += ch;
            count++;
        }

        gdb_io_ops.write_char('#');
        gdb_io_ops.write_char(tohex((checksum >> 4) & 0xf));
        gdb_io_ops.write_char(tohex(checksum & 0xf));

        /* Now see what we get in reply. */
        ch = gdb_io_ops.read_char();

        /* If we get an ACK, we are done. */
        if (ch == '+')
        {
            return;
        }

        /*
         * If we get the start of another packet, this means
         * that GDB is attempting to reconnect.  We will NAK
         * the packet being sent, and stop trying to send this
         * packet.
         */
        if (ch == '$')
        {
            gdb_io_ops.write_char('-');
            if (gdb_io_ops.flush)
            {
                gdb_io_ops.flush();
            }
            return;
        }
    }
}

/* scan for the sequence $<data>#<checksum> */
static void get_packet(char *buffer)
{
    unsigned char checksum;
    unsigned char xmitcsum;
    int count;
    char ch;

    do
    {
        /*
         * Spin and wait around for the start character, ignore all
         * other characters:
         */
        while ((ch = (gdb_io_ops.read_char())) != '$');
        gdb_connected = 1;
        checksum = 0;
        xmitcsum = -1;

        count = 0;

        /*
         * now, read until a # or end of buffer is found:
         */
        while (count < (BUFMAX - 1))
        {
            ch = gdb_io_ops.read_char();
            if (ch == '#')
            {
                break;
            }
            checksum = checksum + ch;
            buffer[count] = ch;
            count = count + 1;
        }
        buffer[count] = 0;

        if (ch == '#')
        {
            xmitcsum = hex(gdb_io_ops.read_char()) << 4;
            xmitcsum += hex(gdb_io_ops.read_char());

            if (checksum != xmitcsum)
            {
                /* failed checksum */
                gdb_io_ops.write_char('-');
            }
            else
            {
                /* successful transfer */
                gdb_io_ops.write_char('+');
            }
            if (gdb_io_ops.flush)
            {
                gdb_io_ops.flush();
            }
        }
    } while (checksum != xmitcsum);
}

static void error_packet(char *pkt, int error)
{
    error = -error;
    pkt[0] = 'E';
    pkt[1] = tohex((error / 10));
    pkt[2] = tohex((error % 10));
    pkt[3] = '\0';
}

void gdb_msg_write(const char *s, int len)
{
    char *bufptr;
    int wcount;
    int i;

    /* 'O'utput */
    gdbmsgbuf[0] = 'O';

    /* Fill and send buffers... */
    while (len > 0)
    {
        bufptr = gdbmsgbuf + 1;

        /* Calculate how many this time */
        if ((len << 1) > (BUFMAX - 2))
        {
            wcount = (BUFMAX - 2) >> 1;
        }
        else
        {
            wcount = len;
        }

        /* Pack in hex chars */
        for (i = 0; i < wcount; i++)
        {
            *(bufptr++) = tohex((s[i] >> 4) & 0xf);
            *(bufptr++) = tohex(s[i] & 0xf);
        }
        *bufptr = '\0';

        /* Move up */
        s += wcount;
        len -= wcount;

        /* Write packet */
        put_packet(gdbmsgbuf);
    }
}

static int gdb_io_ready(int print_wait)
{
    if (gdb_connected)
    {
        return 1;
    }
    if (print_wait)
    {
        printf("GDB: Waiting for remote debugger\n");
    }
    return 1;
}

static void gdb_cmd_status(struct gdb_state *gs)
{
    /*
     * We know that this packet is only sent
     * during initial connect.  So to be safe,
     * we clear out our breakpoints now in case
     * GDB is reconnecting.
     */
    /* remove_all_break_watch_points(); */

    remcom_out_buffer[0] = 'S';
    remcom_out_buffer[1] = tohex((gs->signo >> 4) & 0xf);
    remcom_out_buffer[2] = tohex(gs->signo & 0xf);
    remcom_out_buffer[3] = 0;
}

static void gdb_cmd_memread(struct gdb_state *gs)
{
    char *ptr = &remcom_in_buffer[1];
    unsigned long length;
    unsigned long addr;
    int err;

    if (gdb_hex2long(&ptr, &addr) > 0 && *ptr++ == ',' &&
        gdb_hex2long(&ptr, &length) > 0)
    {
        if (kgdb_permit_data_access(addr, length))
        {
            error_packet(remcom_out_buffer, -1);
            return ;
        }
        err = gdb_mem2hex((char *)addr, remcom_out_buffer, length);
        if (err)
        {
            error_packet(remcom_out_buffer, err);
        }
    }
    else
    {
        error_packet(remcom_out_buffer, -1);
    }
}

static void gdb_cmd_memwrite(struct gdb_state *gs)
{
    int err = write_mem_msg(0);

    if (err)
    {
        error_packet(remcom_out_buffer, err);
    }
    else
    {
        strcpy(remcom_out_buffer, "OK");
    }
}

static void gdb_cmd_binwrite(struct gdb_state *gs)
{
    int err = write_mem_msg(1);

    if (err)
    {
        error_packet(remcom_out_buffer, err);
    }
    else
    {
        strcpy(remcom_out_buffer, "OK");
    }
}

static void gdb_cmd_query(struct gdb_state *gs)
{
    /* nothing,because we have no thread support */
}

static void gdb_cmd_getregs(struct gdb_state *gs)
{
    char len = sizeof(long);

    gdb_get_register((unsigned long *)gdb_regs);

    if (remcom_in_buffer[0] == 'p')
    {
        /*get one registers*/
        char *p = &remcom_in_buffer[1];
        unsigned long regno = 0;

        if (gdb_hex2long(&p, &regno))
        {
            gdb_mem2hex(((char *)gdb_regs) + regno * len, remcom_out_buffer, len);
            return;
        }
        else
        {
            strcpy(remcom_out_buffer, "INVALID");
            return;
        }
    }
    else
    {
        /*get all registers*/
        gdb_mem2hex((char *)gdb_regs, remcom_out_buffer, NUMREGBYTES);
    }
}

static void gdb_cmd_setregs(struct gdb_state *gs)
{
    char len = sizeof(long);

    if (remcom_in_buffer[0] == 'P')
    {
        /*set one registers*/
        char *p = &remcom_in_buffer[1];
        unsigned long regno = 0;

        if (gdb_hex2long(&p, &regno) && *p++ == '=')
        {
            gdb_get_register((unsigned long *)gdb_regs);
            gdb_hex2mem(p, ((char *)gdb_regs) + regno * len, len);
            gdb_put_register(gdb_regs);
            strcpy(remcom_out_buffer, "OK");
        }
        return;
    }
    else
    {
        /*set all registers*/
        gdb_hex2mem(&remcom_in_buffer[1], (char *)gdb_regs, NUMREGBYTES);

        gdb_put_register(gdb_regs);
        strcpy(remcom_out_buffer, "OK");
    }
}

/* Handle the 'D' or 'k', detach or kill packets */
static void gdb_cmd_detachkill(struct gdb_state *gs)
{
    int error;

    if (remcom_in_buffer[0] == 'D')
    {
        error = remove_all_break_watch_points();
        if (error < 0)
        {
            error_packet(remcom_out_buffer, error);
        }
        else
        {
            strcpy(remcom_out_buffer, "OK");
            gdb_connected = 0;
        }
        put_packet(remcom_out_buffer);
    }
    else
    {
        remove_all_break_watch_points();
        gdb_connected = 0;
    }
}

static void gdb_cmd_break(struct gdb_state *gs)
{
    char *bpt_type = &remcom_in_buffer[1];
    char *ptr = &remcom_in_buffer[2];
    unsigned long addr;
    unsigned long length;
    int error = 0;

    if (arch_gdb_ops.set_hw_breakpoint && *bpt_type >= '1')
    {
        /* Unsupported */
        if (*bpt_type > '4')
        {
            return;
        }
    }
    if (*(ptr++) != ',')
    {
        error_packet(remcom_out_buffer, -1);
        return;
    }
    if (!gdb_hex2long(&ptr, &addr))
    {
        error_packet(remcom_out_buffer, -1);
        return;
    }
    if (*(ptr++) != ',' ||
        !gdb_hex2long(&ptr, &length))
    {
        error_packet(remcom_out_buffer, -1);
        return;
    }
    if (remcom_in_buffer[0] == 'Z' && *bpt_type == '0')
    {
        error = gdb_set_sw_break(addr, length);
    }
    else if (remcom_in_buffer[0] == 'z' && *bpt_type == '0')
    {
        error = gdb_remove_sw_break(addr, length);
    }
    else if (remcom_in_buffer[0] == 'Z' && *bpt_type == '1')
    {
        error = gdb_set_hw_break(addr);
    }
    else if (remcom_in_buffer[0] == 'z' && *bpt_type == '1')
    {
        error = gdb_remove_hw_break(addr);
    }
    else if (remcom_in_buffer[0] == 'Z'
             && (*bpt_type - '0') > BP_HARDWARE_BREAKPOINT)
    {
        error = gdb_set_hw_watch(addr, *bpt_type - '0');
    }
    else if (remcom_in_buffer[0] == 'z'
             && (*bpt_type - '0') > BP_HARDWARE_BREAKPOINT)
    {
        error = gdb_remove_hw_watch(addr);
    }

    if (error == 0)
    {
        strcpy(remcom_out_buffer, "OK");
    }
    else
    {
        error_packet(remcom_out_buffer, error);
    }
}

/* Handle the 'C' signal / exception passing packets */
static int gdb_cmd_exception_pass(struct gdb_state *gs)
{
    /* C09 == pass exception
     * C15 == detach gdb, pass exception
     */
    if (remcom_in_buffer[1] == '0' && remcom_in_buffer[2] == '9')
    {
        gs->pass_exception = 1;
        remcom_in_buffer[0] = 'c';
    }
    else if (remcom_in_buffer[1] == '1' && remcom_in_buffer[2] == '5')
    {
        gs->pass_exception = 1;
        remcom_in_buffer[0] = 'D';
        remove_all_break_watch_points();
        gdb_connected = 0;
        return 1;
    }
    else
    {
        error_packet(remcom_out_buffer, -1);
        return 0;
    }

    /* Indicate fall through */
    return -1;
}


/*more about packet in https://www.sourceware.org/gdb/current/onlinedocs/gdb/Packets.html#Packets*/
static int process_packet(char *pkt)
{
    int status = 0;
    int tmp;

    status = gdb_arch_handle_exception(remcom_in_buffer, remcom_out_buffer);

    remcom_out_buffer[0] = 0;
    switch (pkt[0])
    {
        case '?':       /* gdb serial status */
            gdb_cmd_status(&gs);
            break;
        case 'q':       /* query command */
            gdb_cmd_query(&gs);
            break;
        case 'p':       /* return the value of  a single CPU register */
        case 'g':       /* return the value of the CPU registers */
            gdb_cmd_getregs(&gs);
            break;
        case 'P':       /* set the value of a single CPU registers - return OK */
        case 'G':       /* set the value of the CPU registers - return OK */
            gdb_cmd_setregs(&gs);
            break;
        case 'm':       /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
            gdb_cmd_memread(&gs);
            break;
        case 'X':       /* XAA..AA,LLLL: Write LLLL escaped binary bytes at address AA.AA*/
            gdb_cmd_binwrite(&gs);
            break;
        case 'M':       /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
            gdb_cmd_memwrite(&gs);
            break;
        case 'D':       /* Debugger detach */
        case 'k':       /* Debugger detach via kill */
            gdb_cmd_detachkill(&gs);
            break;
        case 'C':       /* Exception passing */
            tmp = gdb_cmd_exception_pass(&gs);
            if (tmp > 0)
            {
                process_packet(remcom_in_buffer);
            }
            if (tmp == 0)
            {
                break;
            }
        case 'z':       /* Break point remove */
        case 'Z':       /* Break point set */
            gdb_cmd_break(&gs);
            break;
        case 'H':       /* task related */
            break;
        case 'T':       /* Query thread status */
            break;
        case 'b':       /* bBB...  Set baud rate to BB... */
            break;
        case 's':       /* sAA..AA    step form address AA..AA (optional) */
        case 'c':       /* cAA..AA    Continue at address AA..AA (optional) */
            gdb_activate_all_break_watch_points();
            break;
    }

    if (!status)
    {
        return -1;
    }

exit:
    put_packet(remcom_out_buffer);
    return 0;
}

int gdb_process_exception(void)
{
    int status;

    do
    {
        get_packet(remcom_in_buffer);
        status = process_packet(remcom_in_buffer);
    } while (status == 0);

    return status < 0 ? 0 : 1;
}

int gdb_handle_exception(int signo, void *regs)
{
    int error;

    gs.signo = signo;

    if (!gdb_io_ready(1))
    {
        error = 1;
        return error;
    }

    gdb_deactivate_all_break_watch_points();
    gdb_set_register(regs);

    memset(remcom_out_buffer, 0, sizeof(remcom_out_buffer));

    if (gdb_connected)
    {
        char *ptr;

        gdb_io_ops.write_char('\n');
        /* Reply to host that an exception has occurred */
        ptr = remcom_out_buffer;
        *ptr++ = 'T';
        *ptr++ = tohex((gs.signo >> 4) & 0xf);
        *ptr++ = tohex(gs.signo & 0xf);
        put_packet(remcom_out_buffer);
    }
    gs.pass_exception = 0;

    while (gdb_process_exception());

    error = gs.pass_exception;

    return error;
}
