#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ringbuffer.h>
#include <sys/socket.h>
#include <msh.h>
#include "isptool_terminal.h"

#define ISPTOOL_TERMINAL_PORT     32
#define RX_BUFFER_SIZE            256
#define TX_BUFFER_SIZE            4096

#define STATE_NORMAL        0
#define STATE_IAC           1
#define STATE_WILL          2
#define STATE_WONT          3
#define STATE_DO            4
#define STATE_DONT          5
#define STATE_CLOSE         6

int isptool_console_init(void *private_data)
{
    return 1;
}

int isptool_console_deinit(void *private_data)
{
    return 1;
}

int isptool_console_write(const void *buf, size_t len, void *private_data)
{
    return 1;
}

int isptool_console_read(void *buf, size_t len, void *private_data)
{
    return 1;
}

device_console isptool_console =
{
    .name = "isptool_console",
    .write = isptool_console_write,
    .read = isptool_console_read,
    .init = isptool_console_init,
    .deinit = isptool_console_deinit
};

void *finsh_thread_entry(void *Parameter);
static cli_console *isptool_terminal_handle = NULL;

int isptool_terminal_init()
{
#if 1
    if(!isptool_terminal_handle){
        isptool_terminal_handle = cli_console_create(&isptool_console, "isptool-cli-console",NULL);
    }
    if(!cli_console_check_invalid(isptool_terminal_handle))
    {
        return -1;
    }
    cli_console_init(isptool_terminal_handle);
//    adb_thread_create(tid, (adb_thread_func_t)finsh_thread_entry, "adb-shell",
//            (void *)xfer->console, ADB_THREAD_NORMAL_PRIORITY);

    int ret;
    pthread_attr_t attr;
    struct sched_param sched;
    pthread_t isptool_tid;
    memset(&sched, 0, sizeof(sched));
    sched.sched_priority = 21;
    pthread_attr_init(&attr);
    pthread_attr_setschedparam(&attr, &sched);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&attr, 8*1024);
    ret = pthread_create(&isptool_tid, &attr, finsh_thread_entry,isptool_terminal_handle);
    if (ret < 0)
        return ret;
    pthread_setname_np(isptool_tid, "isptool-shell");
    return ret;


    isptool_terminal_handle->task = NULL;
    isptool_terminal_handle->alive = 1;
#endif
    return 0;
}
