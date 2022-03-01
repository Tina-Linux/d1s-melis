/*
 * File      : telnet.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-04-01     Bernard      first version
 * 2018-01-25     armink       Fix it on RT-Thread 3.0+
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <ringbuffer.h>

#if defined(RT_USING_DFS_NET) || defined(SAL_USING_POSIX)
#include <sys/socket.h>
#else
#include <lwip/sockets.h>
#endif /* SAL_USING_POSIX */

#if defined(RT_USING_POSIX)
#include <dfs_posix.h>
#include <dfs_poll.h>
#include <libc.h>
static int dev_old_flag;
#endif

#include <finsh.h>
#include <msh.h>
#include <shell.h>

#include <cli_console.h>

#define TELNET_PORT         23
#define TELNET_BACKLOG      5
#define RX_BUFFER_SIZE      256
#define TX_BUFFER_SIZE      4096

#define ISO_nl              0x0a
#define ISO_cr              0x0d

#define STATE_NORMAL        0
#define STATE_IAC           1
#define STATE_WILL          2
#define STATE_WONT          3
#define STATE_DO            4
#define STATE_DONT          5
#define STATE_CLOSE         6

#define TELNET_IAC          255
#define TELNET_WILL         251
#define TELNET_WONT         252
#define TELNET_DO           253
#define TELNET_DONT         254

struct telnet_session
{
    struct rt_ringbuffer rx_ringbuffer;
    struct rt_ringbuffer tx_ringbuffer;

    rt_mutex_t rx_ringbuffer_lock;
    rt_mutex_t tx_ringbuffer_lock;

    struct rt_device device;
    rt_int32_t server_fd;
    rt_int32_t client_fd;

    /* telnet protocol */
    rt_uint8_t state;
    rt_uint8_t echo_mode;

    rt_sem_t read_notice;
    cli_console *console;
};

/* process tx data */
static void send_to_client(struct telnet_session* telnet)
{
    rt_size_t length;
    rt_uint8_t tx_buffer[32];

    while (1)
    {
        rt_memset(tx_buffer, 0, sizeof(tx_buffer));
        rt_mutex_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
        /* get buffer from ringbuffer */
        length = rt_ringbuffer_get(&(telnet->tx_ringbuffer), tx_buffer, sizeof(tx_buffer));
        rt_mutex_release(telnet->tx_ringbuffer_lock);

        /* do a tx procedure */
        if (length > 0)
        {
            send(telnet->client_fd, tx_buffer, length, 0);
        }
        else break;
    }
}

/* send telnet option to remote */
static void send_option_to_client(struct telnet_session* telnet, rt_uint8_t option, rt_uint8_t value)
{
    rt_uint8_t optbuf[4];

    optbuf[0] = TELNET_IAC;
    optbuf[1] = option;
    optbuf[2] = value;
    optbuf[3] = 0;

    rt_mutex_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
    rt_ringbuffer_put(&telnet->tx_ringbuffer, optbuf, 3);
    rt_mutex_release(telnet->tx_ringbuffer_lock);

    send_to_client(telnet);
}

/* process rx data */
static void process_rx(struct telnet_session* telnet, rt_uint8_t *data, rt_size_t length)
{
    rt_size_t index;

    for (index = 0; index < length; index++)
    {
        switch (telnet->state)
        {
        case STATE_IAC:
            if (*data == TELNET_IAC)
            {
                rt_mutex_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
                /* put buffer to ringbuffer */
                rt_ringbuffer_putchar(&(telnet->rx_ringbuffer), *data);
                rt_mutex_release(telnet->rx_ringbuffer_lock);

                telnet->state = STATE_NORMAL;
            }
            else
            {
                /* set telnet state according to received package */
                switch (*data)
                {
                case TELNET_WILL:
                    telnet->state = STATE_WILL;
                    break;
                case TELNET_WONT:
                    telnet->state = STATE_WONT;
                    break;
                case TELNET_DO:
                    telnet->state = STATE_DO;
                    break;
                case TELNET_DONT:
                    telnet->state = STATE_DONT;
                    break;
                default:
                    telnet->state = STATE_NORMAL;
                    break;
                }
            }
            break;

            /* don't option */
        case STATE_WILL:
        case STATE_WONT:
            send_option_to_client(telnet, TELNET_DONT, *data);
            telnet->state = STATE_NORMAL;
            break;

            /* won't option */
        case STATE_DO:
        case STATE_DONT:
            send_option_to_client(telnet, TELNET_WONT, *data);
            telnet->state = STATE_NORMAL;
            break;

        case STATE_NORMAL:
            if (*data == TELNET_IAC)
            {
                telnet->state = STATE_IAC;
            }
            else if (*data != '\r') /* ignore '\r' */
            {
                rt_mutex_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
                /* put buffer to ringbuffer */
                rt_ringbuffer_putchar(&(telnet->rx_ringbuffer), *data);
                rt_mutex_release(telnet->rx_ringbuffer_lock);
                rt_sem_release(telnet->read_notice);
            }
            break;
        }
        data++;
    }


#if !defined(RT_USING_POSIX)
    rt_size_t rx_length;
    rt_mutex_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
    /* get total size */
    rx_length = rt_ringbuffer_data_len(&telnet->rx_ringbuffer);
    rt_mutex_release(telnet->rx_ringbuffer_lock);

    /* indicate there are reception data */
    if ((rx_length > 0) && (telnet->device.rx_indicate != RT_NULL))
    {
        telnet->device.rx_indicate(&telnet->device, rx_length);
    }
#endif

    return;
}

/* client close */
static void client_close(struct telnet_session* telnet)
{
#if 0
    /* set console */
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
    /* set finsh device */
#if defined(RT_USING_POSIX)
    ioctl(libc_stdio_get_console(), F_SETFL, (void *) dev_old_flag);
    libc_stdio_set_console(RT_CONSOLE_DEVICE_NAME, O_RDWR);
#else
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif /* RT_USING_POSIX */
#endif

    cli_console_task_destory(telnet->console);

    rt_sem_release(telnet->read_notice);

    /* close connection */
    closesocket(telnet->client_fd);

#if 0
    /* restore shell option */
    finsh_set_echo(telnet->echo_mode);
#endif

    rt_kprintf("telnet: resume console to %s\n", RT_CONSOLE_DEVICE_NAME);
}

/* RT-Thread Device Driver Interface */
static rt_err_t telnet_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t telnet_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t telnet_close(rt_device_t dev)
{
    return RT_EOK;
}

#if 0
static rt_size_t telnet_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_size_t result;

    rt_sem_take(telnet->read_notice, RT_WAITING_FOREVER);

    /* read from rx ring buffer */
    rt_mutex_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
    result = rt_ringbuffer_get(&(telnet->rx_ringbuffer), buffer, size);
    if (result == 0)
    {
        /**
         * MUST return unless **1** byte for support sync read data.
         * It will return empty string when read no data
         */
        *(char *) buffer = '\0';
        result = 1;
    }
    rt_mutex_release(telnet->rx_ringbuffer_lock);

    return result;
}

static rt_size_t telnet_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    const rt_uint8_t *ptr;

    ptr = (rt_uint8_t*) buffer;

    rt_mutex_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
    while (size)
    {
        if (*ptr == '\n')
            rt_ringbuffer_putchar(&telnet->tx_ringbuffer, '\r');

        if (rt_ringbuffer_putchar(&telnet->tx_ringbuffer, *ptr) == 0) /* overflow */
            break;
        ptr++;
        size--;
    }
    rt_mutex_release(telnet->tx_ringbuffer_lock);

    /* send data to telnet client */
    send_to_client(telnet);

    return (rt_uint32_t) ptr - (rt_uint32_t) buffer;
}
#endif

rt_size_t telnet_session_read(struct telnet_session *telnet, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_size_t result;

    rt_sem_take(telnet->read_notice, RT_WAITING_FOREVER);

    /* read from rx ring buffer */
    rt_mutex_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
    result = rt_ringbuffer_get(&(telnet->rx_ringbuffer), buffer, size);
    if (result == 0)
    {
        /**
         * MUST return unless **1** byte for support sync read data.
         * It will return empty string when read no data
         */
        *(char *) buffer = '\0';
        result = 1;
    }
    rt_mutex_release(telnet->rx_ringbuffer_lock);

    return result;
}

rt_size_t telnet_session_write (struct telnet_session *telnet, rt_off_t pos, const void* buffer, rt_size_t size)
{
    const rt_uint8_t *ptr;

    ptr = (rt_uint8_t*) buffer;

    rt_mutex_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
    while (size)
    {
        if (*ptr == '\n')
            rt_ringbuffer_putchar(&telnet->tx_ringbuffer, '\r');

        if (rt_ringbuffer_putchar(&telnet->tx_ringbuffer, *ptr) == 0) /* overflow */
            break;
        ptr++;
        size--;
    }
    rt_mutex_release(telnet->tx_ringbuffer_lock);

    /* send data to telnet client */
    send_to_client(telnet);

    return (rt_uint32_t) ptr - (rt_uint32_t) buffer;
}

rt_err_t telnet_control(rt_device_t dev, int cmd, void *args)
{
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
    static struct rt_device_ops _ops = {
        telnet_init,
        telnet_open,
        telnet_close,
        telnet_read,
        telnet_write,
        telnet_control
    };
#endif

typedef pthread_t telnet_thread_t;
typedef void* (*telnet_thread_func_t)(void *arg);
#define ADB_THREAD_STACK_SIZE	(8*1024)

typedef struct _telnet_shell
{
    telnet_thread_t cli_tid;
    telnet_thread_t ser_tid;
    telnet_thread_t recv_tid;
    struct telnet_session* session;
} telnet_shell_t;

static struct telnet_session * init_telnet_session(void)
{
    rt_uint8_t *ptr;
    struct telnet_session *telnet = rt_malloc(sizeof(struct telnet_session));
    if (telnet == RT_NULL)
    {
        rt_kprintf("telnet: no memory\n");
        return NULL;
    }

    /* init ringbuffer */
    ptr = rt_malloc(RX_BUFFER_SIZE);
    if (ptr)
    {
        rt_ringbuffer_init(&telnet->rx_ringbuffer, ptr, RX_BUFFER_SIZE);
    }
    else
    {
        rt_kprintf("telnet: no memory\n");
        return NULL;
    }
    ptr = rt_malloc(TX_BUFFER_SIZE);
    if (ptr)
    {
        rt_ringbuffer_init(&telnet->tx_ringbuffer, ptr, TX_BUFFER_SIZE);
    }
    else
    {
        rt_kprintf("telnet: no memory\n");
        return NULL;
    }
    /* create tx ringbuffer lock */
    telnet->tx_ringbuffer_lock = rt_mutex_create("telnet_tx", RT_IPC_FLAG_FIFO);
    /* create rx ringbuffer lock */
    telnet->rx_ringbuffer_lock = rt_mutex_create("telnet_rx", RT_IPC_FLAG_FIFO);

    telnet->read_notice = rt_sem_create("telnet_rx", 0, RT_IPC_FLAG_FIFO);
    return telnet;
}

static void destory_telnet_session(struct telnet_session *session)
{
    free(session->rx_ringbuffer.buffer_ptr);
    free(session->tx_ringbuffer.buffer_ptr);
    rt_mutex_delete(session->rx_ringbuffer_lock);
    rt_mutex_delete(session->tx_ringbuffer_lock);
    rt_sem_delete(session->read_notice);
    free(session);
}

static void* telnet_console_service_thread(void *arg)
{
    telnet_shell_t *xshell = (arg);
    struct telnet_session* session = xshell->session;
    pthread_t cli_tid = (pthread_t)(xshell->cli_tid);
    pthread_join(cli_tid, NULL);
    cli_console_destory(session->console);
    destory_telnet_session(session);
    free(xshell);
    return NULL;
}

static void* telnet_console_recv_thread(void *arg)
{
#define RECV_BUF_LEN 64
    struct telnet_session* telnet = arg;
    rt_uint8_t recv_buf[RECV_BUF_LEN];
    rt_int32_t recv_len = 0;

    while (1)
    {
        /* try to send all data in tx ringbuffer */
        send_to_client(telnet);
        /* do a rx procedure */
        if ((recv_len = recv(telnet->client_fd, recv_buf, RECV_BUF_LEN, 0)) > 0)
        {
            process_rx(telnet, recv_buf, recv_len);
        }
        else
        {
            /* close connection */
            client_close(telnet);
            break;
        }
    }
    return NULL;
}

static int telnet_thread_create(telnet_thread_t *tid, telnet_thread_func_t start,
					const char *name, void *arg, int priority)
{
    int ret;
    pthread_attr_t attr;
    struct sched_param sched;

    memset(&sched, 0, sizeof(sched));
    sched.sched_priority = priority;
    pthread_attr_init(&attr);
    pthread_attr_setschedparam(&attr, &sched);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&attr, ADB_THREAD_STACK_SIZE);
    ret = pthread_create(tid, &attr, start, arg);
    if (ret < 0)
        return ret;
    pthread_setname_np(*tid, name);
	return ret;
}

/* telnet server thread entry */
static void telnet_thread(void* parameter)
{
    struct sockaddr_in addr;
    socklen_t addr_size;
#if 0
    if ((telnet->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("telnet: create socket failed\n");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(TELNET_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));
    if (bind(telnet->server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1)
    {
        rt_kprintf("telnet: bind socket failed\n");
        return;
    }

    if (listen(telnet->server_fd, TELNET_BACKLOG) == -1)
    {
        rt_kprintf("telnet: listen socket failed\n");
        return;
    }

    /* register telnet device */
    telnet->device.type     = RT_Device_Class_Char;
#ifdef RT_USING_DEVICE_OPS
    telnet->device.ops = &_ops;
#else    
    telnet->device.init     = telnet_init;
    telnet->device.open     = telnet_open;
    telnet->device.close    = telnet_close;
    telnet->device.read     = telnet_read;
    telnet->device.write    = telnet_write;
    telnet->device.control  = telnet_control;
#endif

    /* no private */
    telnet->device.user_data = RT_NULL;

    /* register telnet device */
    rt_device_register(&telnet->device, "telnet", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);
#else
    rt_int32_t server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("telnet: create socket failed\n");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(TELNET_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(addr.sin_zero), 0, sizeof(addr.sin_zero));
    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1)
    {
        rt_kprintf("telnet: bind socket failed\n");
        return;
    }

    if (listen(server_fd, TELNET_BACKLOG) == -1)
    {
        rt_kprintf("telnet: listen socket failed\n");
        return;
    }
#endif

    while (1)
    {
        rt_kprintf("telnet: waiting for connection\n");
        rt_int32_t client_fd;

        /* grab new connection */
        if ((client_fd = accept(server_fd, (struct sockaddr *) &addr, &addr_size)) == -1)
        {
            continue;
        }
#if 0
        rt_kprintf("telnet: new telnet client(%s:%d) connection, switch console to telnet...\n", inet_ntoa(addr.sin_addr), addr.sin_port);
        /* process the new connection */
        /* set console */
        rt_console_set_device("telnet");
        /* set finsh device */
#if defined(RT_USING_POSIX)
        /* backup flag */
        dev_old_flag = ioctl(libc_stdio_get_console(), F_GETFL, (void *) RT_NULL);
        /* add non-block flag */
        ioctl(libc_stdio_get_console(), F_SETFL, (void *) (dev_old_flag | O_NONBLOCK));
        /* set tcp shell device for console */
        libc_stdio_set_console("telnet", O_RDWR);
        /* resume finsh thread, make sure it will unblock from last device receive */
        rt_thread_t tid = rt_thread_find(FINSH_THREAD_NAME);
        if (tid)
        {
            rt_thread_resume(tid);
            rt_schedule();
        }
#else
        /* set finsh device */
        finsh_set_device("telnet");
#endif /* RT_USING_POSIX */

        /* set init state */
        telnet->state = STATE_NORMAL;

        telnet->echo_mode = finsh_get_echo();
        /* disable echo mode */
        finsh_set_echo(0);
        /* output RT-Thread version and shell prompt */
#ifdef FINSH_USING_MSH
        msh_exec("version", strlen("version"));
#endif
        rt_kprintf(FINSH_PROMPT);
#else
        telnet_shell_t *tel_shell;
        struct telnet_session * telnet;

        tel_shell = calloc(sizeof(telnet_shell_t), 1);
        if (tel_shell == NULL)
        {
            rt_kprintf("alloc memory failed! %s, %d\n", __func__, __LINE__);
            continue;
        }

        telnet = init_telnet_session();
        if (telnet == NULL)
        {
            free(tel_shell);
            continue;
        }
        telnet->server_fd = server_fd;
        telnet->client_fd = client_fd;
        telnet->state = STATE_NORMAL;

        tel_shell->session = telnet;

        rt_kprintf("telnet: new telnet client(%s:%d) connection \n", inet_ntoa(addr.sin_addr), addr.sin_port);

        extern device_console telnet_console;
        telnet->console = cli_console_create(&telnet_console,
            "telnet-cli-console", (void *)telnet);
        cli_console_task_create(telnet->console, &(tel_shell->cli_tid), 8 * 1024, 21);
        telnet_thread_create(&(tel_shell->recv_tid), telnet_console_recv_thread, "telnet-recv", telnet, 20);
        telnet_thread_create(&(tel_shell->ser_tid), telnet_console_service_thread, "telnet-ser", tel_shell, 20);
#endif
    }
}

/* telnet server */
void telnet_server(void)
{
#if 0
    if (telnet == RT_NULL)
    {
        rt_uint8_t *ptr;

        telnet = rt_malloc(sizeof(struct telnet_session));
        if (telnet == RT_NULL)
        {
            rt_kprintf("telnet: no memory\n");
            return;
        }
        /* init ringbuffer */
        ptr = rt_malloc(RX_BUFFER_SIZE);
        if (ptr)
        {
            rt_ringbuffer_init(&telnet->rx_ringbuffer, ptr, RX_BUFFER_SIZE);
        }
        else
        {
            rt_kprintf("telnet: no memory\n");
            return;
        }
        ptr = rt_malloc(TX_BUFFER_SIZE);
        if (ptr)
        {
            rt_ringbuffer_init(&telnet->tx_ringbuffer, ptr, TX_BUFFER_SIZE);
        }
        else
        {
            rt_kprintf("telnet: no memory\n");
            return;
        }
        /* create tx ringbuffer lock */
        telnet->tx_ringbuffer_lock = rt_mutex_create("telnet_tx", RT_IPC_FLAG_FIFO);
        /* create rx ringbuffer lock */
        telnet->rx_ringbuffer_lock = rt_mutex_create("telnet_rx", RT_IPC_FLAG_FIFO);

        telnet->read_notice = rt_sem_create("telnet_rx", 0, RT_IPC_FLAG_FIFO);

        tid = rt_thread_create("telnet", telnet_thread, RT_NULL, 4096, 1, 5);
        if (tid != RT_NULL)
        {
            rt_thread_startup(tid);
            rt_kprintf("Telnet server start successfully\n");
        }
    }
    else
    {
        rt_kprintf("telnet: server already running\n");
    }
#else
    static rt_thread_t tid = 0;
    if (tid == RT_NULL)
    {
        tid = rt_thread_create("telnet", telnet_thread, RT_NULL, 4096, 1, 5);
        if (tid != RT_NULL)
        {
            rt_thread_startup(tid);
            rt_kprintf("Telnet server start successfully\n");
        }
    }
    else
    {
        rt_kprintf("telnet: server already running\n");
    }

#endif
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(telnet_server, startup telnet server);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(telnet_server, startup telnet server)
#endif /* FINSH_USING_MSH */
#endif /* RT_USING_FINSH */
