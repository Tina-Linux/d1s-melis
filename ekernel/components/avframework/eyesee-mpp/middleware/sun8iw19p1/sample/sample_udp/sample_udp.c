#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sched.h>
#include <assert.h>
#include "lwip/tcpip.h"

//#define _SERVER_IP          "192.168.31.135"
#define _SERVER_PORT        8088
#define PACKET_MAX_SIZE    1472

static long long getNowUs(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000ll + tv.tv_usec;
}

static void *RecvThread(void *pArg)
{
    int fd, len;
    struct sockaddr_in ar;
    socklen_t al = sizeof (struct sockaddr_in);
    unsigned char msg[PACKET_MAX_SIZE];
    unsigned char msg_end[8] = "END";
    int *seq = (int *)msg;
    int seq_last = -1;
    int seq_base = -1;
    int i = 0;
    char *ip = "127.0.0.1";
    int port = _SERVER_PORT;
    int lost = 0;
    int total_lost = 0;
    int interval_time_us = 5000000;    // 5s
    long long last_time_us = 0;
    int socket_buffer_size = 64*1024;

    if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket IPPROTO_UDP failed\n");
        exit(EXIT_FAILURE);
    }

    // bind local ip address and port
    bzero(&ar, sizeof(ar));
    ar.sin_family = AF_INET;
    ar.sin_port = htons(port);
    ar.sin_addr.s_addr = inet_addr(ip);
    if (bind(fd, (struct sockaddr *)&ar, al) == -1)
    {
        perror("bind ip failed\n");
        exit(EXIT_FAILURE);
    }

    printf("udp server start [%d][%s:%hd] ...\n", fd, inet_ntoa(ar.sin_addr), _SERVER_PORT);

    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &socket_buffer_size, sizeof(socket_buffer_size)) < 0)
    {
        printf("setsockopt(SO_RECVBUF) failed\n");
    }
    len = sizeof(socket_buffer_size);
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &socket_buffer_size, &len) < 0)
    {
        printf("getsockopt(SO_RCVBUF) failed\n");
    }
    else
    {
        printf("get recv buf size: %d KB\n", socket_buffer_size/1024);
    }

    last_time_us = getNowUs();

    while (1)
    {
        long long recv_tm_us = getNowUs();
        len = recvfrom(fd, msg, PACKET_MAX_SIZE, 0, (struct sockaddr *)&ar, &al);
        //printf("recv req: %d, len: %d, use %lld(us)\n", *seq, len, getNowUs() - recv_tm_us);
        if (len != PACKET_MAX_SIZE)
        {
            perror("recvfrom failed\n");
            break;
        }

        if (!strncmp(msg, msg_end, strlen(msg_end)))
        {
            printf("total lost rate: %f%% , lost %d packages in total %d\n",
                (float)(total_lost *100) / (float)seq_last, total_lost, seq_last);
            seq_base = -1;
            seq_last = -1;
            total_lost = 0;
            break;
        }

        if (seq_base == -1)
        {
            last_time_us = getNowUs();
            seq_base = *seq;
        }

        if ((seq_last != -1) && (*seq - seq_last != 1))
        {
            lost += *seq - seq_last - 1;
            total_lost += *seq - seq_last - 1;
            //printf("lost package seq: %d, last: %d, lost: %d\n", *seq, seq_last, *seq - seq_last - 1);
        }
        //printf("[%s:%hd] -> len: %d, seq: %u\n", inet_ntoa(ar.sin_addr), ntohs(ar.sin_port), len, *seq);

        seq_last = *seq;
        if (getNowUs() - last_time_us >= interval_time_us)
        {
            last_time_us = getNowUs();
            printf("lost rate: %f%% , lost %d packages in total %d\n",
                (float)(lost *100) / (float)(seq_last - seq_base), lost, (seq_last - seq_base));
            seq_base = -1;
            lost = 0;
        }
    }

    close(fd);

    printf("%s exit\n",__func__);
    return NULL;
}

static void *SendThread(void *pArg)
{
    int fd, len;
    struct sockaddr_in ar;
    socklen_t al = sizeof (struct sockaddr_in);
    unsigned char msg[PACKET_MAX_SIZE];
    unsigned char msg_end[8] = "END";
    int *p_seq = (int *)msg;
    int seq = 0;
    int i = 0;
    char *ip = "127.0.0.1";
    int port = _SERVER_PORT;
    int size = PACKET_MAX_SIZE;
    long long test_time = 300*1000*1000; // 300s
    long long begin_time_us = 0;
    int bitrate = 10*1024*1024; // 10Mbps
    int framerate = 30; // 30fps
    int frame_count = 0;
    int frame_pkg_count = 0;
    int interval_time_us = 33000; // 33ms
    int interval_count = 0;
    int ret = 0;

    frame_pkg_count = (bitrate >> 3) / framerate / size;
    interval_time_us = 1000000 / framerate;
    printf("bitrate: %d(bps), frame_pkg_count: %d, interval_time: %d(us), test_time: %d(s)\n",
        bitrate, frame_pkg_count, interval_time_us, test_time/(1000*1000));

    if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket IPPROTO_UDP failed\n");
        exit(EXIT_FAILURE);
    }

#if 0 /* lwip socket is not support SO_SNDBUF. */
    int opt_val = 0;
    socklen_t opt_len = sizeof(opt_val);
    if (0 > getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &opt_val, &opt_len))
    {
        perror("getsockopt SO_SNDBUF fail\n");
    }
    printf("get send buf size: %d KB\n", opt_val/1024);
#endif

    bzero(&ar, sizeof(ar));
    ar.sin_family = AF_INET;
    ar.sin_port = htons(port);
    ar.sin_addr.s_addr = inet_addr(ip);

    begin_time_us = getNowUs();

    while (1)
    {
        *p_seq = seq++;
        if (seq % 1000 == 0)
        {
            printf(".");
            fflush(stdout);
        }
        ret = sendto(fd, msg, size, 0, (struct sockaddr *)&ar, al);
        //printf("send req: %d, len: %d\n", *p_seq, ret);
        if (-1 == ret)
        {
            perror("sendto fail\n");
        }
        if (interval_count++ >= frame_pkg_count)
        {
            frame_count++;
            interval_count = 0;
            long long last_time = getNowUs();
            usleep(interval_time_us);
            //printf("sleep %dms\n", interval_time_us/1000);
        }
        //usleep(10000);
        if (getNowUs() - begin_time_us >= test_time)
        {
            printf("test time %llds is up, stop send data.\n", test_time/(1000000));
            break;
        }
    }
    printf("\nsend total package count: %d, frame: %d\n",  seq, frame_count);

    ret = sendto(fd, msg_end, strlen(msg_end), 0, (struct sockaddr *)&ar, al);
    if (-1 == ret)
    {
        perror("sendto fail\n");
    }

    close(fd);

    printf("%s exit\n",__func__);
    return NULL;
}

static int main_sample_udp(int argc, char **argv)
{
    int ret = 0;
    pthread_t recv_thid = 0;
    pthread_t send_thid = 0;

    /* must be init tcpip before test socket. */
    tcpip_init(NULL, NULL);

    ret = pthread_create(&recv_thid, NULL, RecvThread, NULL);
    if (ret < 0)
    {
        printf("pthread_create RecvThread failed\n");
        return -1;
    }

    ret = pthread_create(&send_thid, NULL, SendThread, NULL);
    if (ret < 0)
    {
        printf("pthread_create SendThread failed\n");
        return -1;
    }

    printf("wait for exit\n");
    pthread_join(send_thid, NULL);
    pthread_join(recv_thid, NULL);

    printf("%s test result: %s", argv[0], ((0 == ret) ? "success" : "fail"));
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(main_sample_udp, __cmd_sample_udp, udp sample);
