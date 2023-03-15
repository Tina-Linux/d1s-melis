/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "adb.h"
#include <lwip/inet.h>
#include "lwip/sockets.h"
#ifdef ADB_SUPPORT_SOCKET_API

int lwip_close(int);

int socket_enqueue(aservice *ser, apacket *p)
{
    int ret;
    socket_xfer *xfer = &ser->xfer_handle.socket_handle;
    adbd_debug("");
    /*adb_enqueue(xfer->queue_recv, (void **)&p, 100);*/
    adbd_debug("data:%s\n", p->data);
    ret = send(xfer->s, p->data, p->msg.data_length, 0);
    put_apacket(p);
    if (ret >= 0)
    {
        return 0;
    }
    return -1;
}

int socket_ready(aservice *ser)
{
    socket_xfer *xfer = &ser->xfer_handle.socket_handle;

    adbd_debug("");
    adb_schd_wakeup(xfer->schd, ADB_SCHD_READ);
    return 0;
}

static void socket_wait_ready(aservice *ser)
{
    socket_xfer *xfer = &ser->xfer_handle.socket_handle;

    adbd_debug("");
    adb_schd_wait(xfer->schd);
    adbd_debug("");
}

/* for debug */
static uint64_t record_recv_len = 0;
uint64_t adb_get_record_recv_len(void)
{
    return record_recv_len;
}

#define LISTEN_LENGTH   (20)

int socket_loopback_client(int port)
{
    struct sockaddr_in addr;
    int s, enable;
    printf("now in:%s\n", __FUNCTION__);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    /*addr.sin_addr.s_addr = htonl(INADDR_ANY);*/

    adbd_info("port:%d", port);
    s = socket(AF_INET, SOCK_STREAM, 0);
    printf("create socket success\n");
    if (s < 0)
    {
        adbd_err("create socket failed, errno=%d(%s)", errno, strerror(errno));
        return -1;
    }
    enable = 1;
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        adbd_err("connect failed, errno=%d(%s)", errno, strerror(errno));
        lwip_close(s);
        return -1;
    }
    record_recv_len = 0;
    return s;
}

void socket_recv_service(void *xfer_handle, void *cookie)
{
    fd_set fds;
    int ret;
    socket_xfer *xfer = (socket_xfer *)xfer_handle;
    aservice *ser = container_of((void *)xfer, struct aservice, xfer_handle);

#if 1
    char *buffer;
    struct timeval timeout;
    int size;

    buffer = adb_malloc(MAX_PAYLOAD);
    if (!buffer)
    {
        fatal("no memory");
    }

    while (1)
    {
        FD_ZERO(&fds);
        FD_SET(xfer->s, &fds);

        timeout.tv_sec  = 0;
        timeout.tv_usec = 200000;
        adbd_debug("select...");
        ret = lwip_select(xfer->s + 1, &fds, NULL, NULL, &timeout);
        adbd_debug("select return %d", ret);
        if (ret < 0)
        {
            adbd_err("select failed, errno=%d\n", errno);
            break;
        }
        else if (ret == 0)
        {
            adbd_debug("select timeout...");
            if (xfer->recv_close != 0)
            {
                break;
            }
            continue;
        }
        if (!FD_ISSET(xfer->s, &fds))
        {
            adbd_err("unknow fd.");
            break;
        }
        adbd_debug("");
        memset(buffer, 0, MAX_PAYLOAD);
        size = lwip_recv(xfer->s, buffer, MAX_PAYLOAD, MSG_DONTWAIT);
        adbd_debug("size=%d\n", size);
        if (xfer->recv_close)
        {
            adbd_info("recv_close=%d, break loop", xfer->recv_close);
            break;
        }
        if (size <= 0)
        {
            adbd_info("recv return %d, recv_close=%d", size, xfer->recv_close);
            break;
        }
        else if (size > 0)
        {
            record_recv_len += size;
            socket_wait_ready(ser);
            send_write((const char *)buffer, size, ser->localid,
                       ser->remoteid, ser->transport);
        }
        adbd_debug("");
    }
    adb_free(buffer);
    adbd_info("socket recv service exit...");
    //pthread_exit(NULL);
#else
    char buf[128];
    int size;

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        size = lwip_read(xfer->s, buf, sizeof(buf));
        if (size < 0)
        {
            adbd_err("lwip_read failed, return %d\n", size);
            break;
        }
        adbd_info("size=%d\n", size);
        if (size > 0 && !xfer->recv_close)
        {
            adbd_info("data=%s\n", buf);
            send_write((const char *)buf, size, ser->localid,
                       ser->remoteid, ser->transport);
            socket_wait_ready(ser);
        }
    }
    pthread_exit(NULL);
#endif


}


/*#define ADB_FORWARD_TEST*/
#ifdef ADB_FORWARD_TEST

#define ADB_DEVICE_PORT (22222)
#define BUFFER_SIZE     (4096)

#include <../components/thirdparty/sona_audioaef/sona_config.h>
#include <aw-alsa-lib/pcm.h>

static void *sona_instance;
static int sona_init()
{
    if (sona_instance)
    {
        return 0;
    }
    sona_config_t *config = NULL;
    config = sona_config_find("C1R_3138_TL");
    if (!config)
    {
        printf("sona config not found\n");
        return -1;
    }
    sona_instance = audioaef_create_fromBuffer(48000, 48000, config->start,
                    config->end - config->start,
                    snd_pcm_format_physical_width(SND_PCM_FORMAT_S16_LE),
                    1024);
    if (!sona_instance)
    {
        printf("sona init failed\n");
        return -1;
    }
    printf("sona init success\n");
    return 0;
}

static int sona_process(char *src, char *dst, int size, int channels, snd_pcm_format_t format)
{
    int in_bits = snd_pcm_format_physical_width(format);
    int max_frames = 1024;
    int frames = size / (channels * in_bits / 8);
    int residue_bytes = in_bits / 8 * channels * frames;
    int process_bytes_max = in_bits / 8 * max_frames;
    int process_bytes = 0;
    while (residue_bytes > 0)
    {
        process_bytes = process_bytes_max < residue_bytes ?
                        process_bytes_max : residue_bytes;
        int ret = audioaef_process(sona_instance, src,
                                   process_bytes, channels, in_bits, dst, channels);
        if (ret < 0)
        {
            printf("Error in audioaef_process. "
                   "Maybe max_frames not large enough? "
                   "(max_frames: %d, process_bytes: %d)\n",
                   max_frames, process_bytes);
            return ret;
        }
        src += process_bytes;
        dst += process_bytes;
        residue_bytes -= process_bytes;
    }
    return size;
}
static int g_frame_loop = 160;
static int g_stop_xfer;
int cmd_sonatest(int argc, char *argv[])
{
    int c;
    optind = 0;
    while ((c = getopt(argc, argv, "f:s")) != -1)
    {
        switch (c)
        {
            case 'f':
                g_frame_loop = atoi(optarg);
                printf("set g_frame_loop=%d\n", atoi(optarg));
                break;
            case 's':
                printf("set stop flag\n");
                g_stop_xfer = 1;
                break;
        }
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_sonatest, st, sona test);
static int sona_send_data(int sock, const char *path)
{
    int ret = -1;
    int channels = 1;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    int frame_bytes = g_frame_loop * channels * snd_pcm_format_physical_width(format) / 8;
    void *in_data = NULL, *out_data = NULL;
    int size, fd;


    in_data = malloc(frame_bytes);
    if (!in_data)
    {
        printf("no memory\n");
        goto err;
    }
    out_data = malloc(frame_bytes);
    if (!out_data)
    {
        printf("no memory\n");
        goto err;
    }
    while (!g_stop_xfer)
    {
        fd = open(path, O_RDONLY);
        if (fd < 0)
        {
            printf("fd isn't exist\n");
            return -1;
        }
        while (1)
        {
            static int size_array[] =
            {
                2508, 2048, 368, 92, 676, 1832,
                2048, 458, 1346, 1162, 1512, 996
            };
            static int size_count = 0;

            if (size_count == sizeof(size_array) / sizeof(int))
            {
                size_count = 0;
            }
            size = size_array[size_count++];
            size = read(fd, in_data, frame_bytes);
            if (size <= 0)
            {
                /*printf("read return %d\n", size);*/
                break;
            }
            /*printf("input size=%d\n", size);*/
            size = sona_process(in_data, out_data, size, channels, format);
            /*printf("output size=%d\n", size);*/
            send(sock, out_data, size, 0);
        }
        close(fd);
    }

err:
    if (in_data)
    {
        free(in_data);
    }
    if (out_data)
    {
        free(out_data);
    }
    if (fd)
    {
        close(fd);
    }
    return ret;
}

static int server_recv(int s)
{
    struct sockaddr_in cli_addr;
    int ret = 0;
    int cli_s;
    socklen_t length = sizeof(cli_addr);
    char buffer[BUFFER_SIZE];

    printf("prepare accept:\n");
    cli_s = accept(s, (struct sockaddr *)&cli_addr, &length);
    if (cli_s < 0)
    {
        printf("accept failed\n");
        return -1;
    }
#if 0
    memset(buffer, 0, sizeof(buffer));
    length = recv(cli_s, buffer, sizeof(buffer), 0);
    if (length < 0)
    {
        printf("receive failed\n");
        ret = -1;
        goto err;
    }
    printf("recv data, len=%d\n", length);
    printf("data:%s\n", buffer);
#endif

#if 1
#define UPLOAD_START    "-->C1R_SOCKET_UPLOAD_START"
#define UPLOAD_END  "-->C1R_SOCKET_UPLOAD_END"
    g_stop_xfer = 0;
    sona_init();
    send(cli_s, UPLOAD_START, strlen(UPLOAD_START), 0);
    sona_send_data(cli_s, "/data/test.pcm");
    send(cli_s, UPLOAD_END, strlen(UPLOAD_END), 0);
#else
    memset(buffer, 1, sizeof(buffer));
    send(cli_s, buffer, BUFFER_SIZE, 0);
#endif

    /*sleep(1);*/
err:
    lwip_close(cli_s);
    return ret;
}

static int cmd_adbforward(int argc, char *argv[])
{
    int s, ret;
    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    /*addr.sin_addr.s_addr = htons(INADDR_ANY);                               */
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (argc == 1)
    {
        printf("[%s] line:%d port:%d\n", __func__, __LINE__, ADB_DEVICE_PORT);
        addr.sin_port = htons(ADB_DEVICE_PORT);
    }
    else
    {
        printf("[%s] line:%d port:%s\n", __func__, __LINE__, argv[1]);
        addr.sin_port = htons(atoi(argv[1]));
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        printf("create socket failed\n");
        return -1;
    }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0)
    {
        printf("bind failed\n");
        goto err;
    }
    if (listen(s, LISTEN_LENGTH))
    {
        printf("listen failed\n");
        goto err;
    }
    while (1)
    {
        if (server_recv(s) < 0)
        {
            break;
        }
    }
err:
    if (s > 0)
    {
        lwip_close(s);
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_adbforward, aft, adb forward test);
#endif /* ADB_FORWARD_TEST */

#define UPLOAD_START    "-->C1R_SOCKET_UPLOAD_START"
#define UPLOAD_END  "-->C1R_SOCKET_UPLOAD_END"

typedef struct adb_forward
{
    pthread_t tid;
    int port;
    int local_sock, data_sock;
    int state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    //dlist_t list;
    //struct hlist_node list;
    //struct hlist_node *next, * *pprev;
    struct list_head list;
    struct list_head *next, *prev;
    void *send_buf;
    unsigned int send_size;
    sem_t sem;

} adb_forward_t;

enum
{
    ADB_FORWARD_THREAD_UNKNOWN = 0,
    ADB_FORWARD_THREAD_ACCEPT,
    ADB_FORWARD_THREAD_INIT,
    ADB_FORWARD_THREAD_RUNNING,
    ADB_FORWARD_THREAD_FINISH,
    ADB_FORWARD_THREAD_EXIT,
};

static struct list_head gAdbForwardThreadList = LIST_HEAD_INIT(gAdbForwardThreadList);//AOS_DLIST_INIT

static adb_forward_t *adb_forward_find(int port)
{
    adb_forward_t *a = NULL;
    list_for_each_entry(a, &gAdbForwardThreadList, list)  //dlist_for_each_entry
    {
        if (a->port == port)
        {
            return a;
        }
    }
    return NULL;
}

static void *adb_forward_thread(void *arg)
{
    int ret;
    adb_forward_t *a = arg;
    struct sockaddr_in cli_addr;
    socklen_t length = sizeof(cli_addr);
    struct timeval timeout;
    fd_set fds;

    while (1)
    {
        ret = listen(a->local_sock, LISTEN_LENGTH);
        if (ret != 0)
        {
            printf("listen failed, return %d\n", ret);
            goto err;
        }
retry_accept:
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        setsockopt(a->local_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        a->state = ADB_FORWARD_THREAD_ACCEPT;
        a->data_sock = accept(a->local_sock,
                              (struct sockaddr *)&cli_addr, &length);
        if (a->data_sock <= 0)
        {
            adbd_debug("accept failed, return %d\n", a->data_sock);
            if (a->state != ADB_FORWARD_THREAD_ACCEPT)
            {
                break;
            }
            goto retry_accept;
        }
        a->state = ADB_FORWARD_THREAD_INIT;
        pthread_mutex_lock(&a->mutex);

        while (a->state != ADB_FORWARD_THREAD_FINISH &&
               a->state != ADB_FORWARD_THREAD_EXIT)
        {
            pthread_cond_wait(&a->cond, &a->mutex);
        }
        /* send upload end flag */
        send(a->data_sock, UPLOAD_END, strlen(UPLOAD_END), 0);
        if (a->state == ADB_FORWARD_THREAD_EXIT)
        {
            goto exit_state;
        }
        lwip_close(a->data_sock);
        /* clear file_adb info */
        a->data_sock = -1;
        a->state = ADB_FORWARD_THREAD_UNKNOWN;
        pthread_mutex_unlock(&a->mutex);
    }
err:
    pthread_mutex_lock(&a->mutex);
exit_state:
    a->state = ADB_FORWARD_THREAD_EXIT;
    if (a->data_sock > 0)
    {
        lwip_close(a->data_sock);
    }
    if (a->local_sock > 0)
    {
        lwip_close(a->local_sock);
    }
    a->data_sock = -1;
    a->local_sock = -1;
    pthread_mutex_unlock(&a->mutex);

    //pthread_exit(NULL);
    return 0;
}

static void adb_forward_set_state(adb_forward_t *a, int state)
{
    pthread_mutex_lock(&a->mutex);
    a->state = state;
    pthread_cond_signal(&a->cond);
    pthread_mutex_unlock(&a->mutex);
}

extern int lwip_socket(int domain, int type, int protocol);
int adb_forward_create(int port)
{
    adb_forward_t *a = NULL;
    struct sockaddr_in addr;
    char name[32];
    int s;
    if (port < 0)
    {
        printf("unknown port=%d\n", port);
        return -1;
    }
    a = adb_forward_find(port);
    if (a != NULL)
    {
        printf("adb forward thread with port:%d, already exist\n", port);
        return 0;
    }

    a = calloc(1, sizeof(adb_forward_t));
    if (!a)
    {
        printf("no memory\n");
        return -1;
    }
    a->port = port;
    a->state = ADB_FORWARD_THREAD_UNKNOWN;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(port);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        printf("create socket failed\n");
        free(a);
        return -1;
    }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("bind socket failed\n");
        lwip_close(s);
        free(a);
        return -1;
    }
    a->local_sock = s;
    snprintf(name, sizeof(name), "adb_forward_%d", port);
    pthread_mutex_init(&a->mutex, NULL);
    pthread_cond_init(&a->cond, NULL);
    sem_init(&a->sem, 0, 1);
    adb_thread_create(&a->tid, adb_forward_thread, name, (void *)a, ADB_THREAD_NORMAL_PRIORITY);
    list_add(&a->list, &gAdbForwardThreadList);
    return 0;
}

int adb_forward_destroy(int port)
{
    adb_forward_t *a = NULL;

    a = adb_forward_find(port);
    if (!a)
    {
        printf("adb forward thread with port:%d, isn't exist\n", port);
        return -1;
    }

    adb_forward_set_state(a, ADB_FORWARD_THREAD_EXIT);
    adb_thread_wait(a->tid, NULL);
    list_del(&a->list);
    pthread_mutex_destroy(&a->mutex);
    pthread_cond_destroy(&a->cond);
    sem_destroy(&a->sem);
    free(a);
    return 0;
}

int adb_forward_end(int port)
{
    adb_forward_t *a = NULL;

    a = adb_forward_find(port);
    if (!a)
    {
        printf("adb forward thread with port:%d, isn't exist\n", port);
        return -1;
    }

    adb_forward_set_state(a, ADB_FORWARD_THREAD_FINISH);
    return 0;
}

int adb_forward_send(int port, void *data, unsigned len)
{
    int err;
    adb_forward_t *a = NULL;

    a = adb_forward_find(port);
    printf("port=%d, adb forward thread state=%d\n", port, a->state);

    if (!a)
    {
        printf("adb forward thread with port:%d, isn't exist\n", port);
        return -1;
    }

    pthread_mutex_lock(&a->mutex);
    switch (a->state)
    {
        case ADB_FORWARD_THREAD_INIT:
            err = send(a->data_sock, UPLOAD_START, strlen(UPLOAD_START), 0);
            printf("init ret=%d\n", err);
            a->state = ADB_FORWARD_THREAD_RUNNING;
	    //break;
        case ADB_FORWARD_THREAD_RUNNING:
            err = send(a->data_sock, data, len, 0);
            printf("ruinning ret=%d\n", err);
            break;
        default:
            err = -1;
            break;
    }

    pthread_mutex_unlock(&a->mutex);
    return err;

}



static void usage(void)
{
    printf("Usage: aft [option]\n");
    printf("-p,          port\n");
    printf("-c,          create adb forward thread with port\n");
    printf("-s,          send msg\n");
    printf("-f,          transfer finish, send upload end flag\n");
    printf("-e,          exit adb forward thread with port\n");
    printf("\n");
}

static int cmd_adbforward(int argc, char *argv[])
{
    int c, port = -1;

    optind = 0;
    while ((c = getopt(argc, argv, "p:s:fech")) != -1)
    {
        switch (c)
        {
            case 'p':
                port = atoi(optarg);
                break;
            case 'c':
                if (adb_forward_create(port))
                {
                    printf("adb forward init thread failed\n");
                }
                else
                {
                    printf("adb forward init with port:%d success\n", port);
                }
                return 0;
            case 'f':
                adb_forward_end(port);
                break;
            case 'e':
                adb_forward_destroy(port);
                break;
            case 's':
            {
                int ret;
                ret = adb_forward_send(port, optarg, strlen(optarg));
                printf("adb_forward_send return %d\n", ret);
            }
            break;
            case 'h':
            default:
                usage();
                return -1;
        }
    }
    return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_adbforward, af, adb forward);
#endif /* ADB_SUPPORT_SOCKET_API */
