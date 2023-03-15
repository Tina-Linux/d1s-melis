/*#####################################################################
# File Describe:test_http.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年03月21日 星期六 10时46分46秒
#====================================================================*/
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "user_http.h"
#include <unistd.h>
//#include <ioctl.h>
#include <finsh.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//上次一次的ip
char gethostipfirst;
char ip[15];

/*解析url,只保留路径＋文件名*/

void parse_url_for(const char *url, const char **newurl_p, char *domain, int *port, char *file_name)
{
    /*通过url解析出域名, 端口, 以及文件名*/
    int i, j = 0;
    int start = 0;
    *port = 80;
    char *patterns[] = {"http://", "https://", NULL};

    for (int i = 0; patterns[i]; i++)
        if (strncmp(url, patterns[i], strlen(patterns[i])) == 0)
            start = strlen(patterns[i]);

    //解析域名, 这里处理时域名后面的端口号会保留
    for (i = start; url[i] != '/' && url[i] != '\0'; i++, j++)
        domain[j] = url[i];
    domain[j] = '\0';

    *newurl_p = url + i + 1;
    printf("start:%d\n", start);
    printf("url:%s\nnewurl:%s\n", url, *newurl_p);

    //解析端口号, 如果没有, 那么设置端口为80
    char *pos = strstr(domain, ":");
    if (pos)
        sscanf(pos, ":%d", port);

    //删除域名端口号

    for (int i = 0; i < (int)strlen(domain); i++)
    {
        if (domain[i] == ':')
        {
            domain[i] = '\0';
            break;
        }
    }

    //获取下载文件名
    j = 0;
    for (int i = start; url[i] != '\0'; i++)
    {
        if (url[i] == '/')
        {
            if (i !=  strlen(url) - 1)
                j = 0;
            continue;
        }
        else
            file_name[j++] = url[i];
    }
    file_name[j] = '\0';
}


// 创建socket
int create_socket()
{
    printf("===1.%s->%d===\n", __func__, __LINE__);
    int mysfd = socket(AF_INET, SOCK_STREAM, 0);
    if(mysfd < 0){
        printf("create_socket error\r\n");
        return -1;
    }

    return mysfd;
}

/**
 * 域名解析
 * hostname：域名
 * hostip：  解析出的ip
 * time  ：  解析尝试次数
 */

int hostnametoip(char *hostname, char *hostip, int time)
{
    int err = -1;
    int cnt = 0;
    ip_addr_t rm_add;


    //如果域名解析出的ip相同则不再进行域名解析
    if(gethostipfirst == 1)
    {
        sprintf(hostip, "%s", ip);
    }else{
        printf("ip is no right\r\n");
        while(err != 0)
        {
            err = dns_gethostbyname(hostname, &rm_add, NULL, LWIP_DNS_ADDRTYPE_IPV4); //IPV4 only
            printf("dns:%d\r\n", err);
            sleep(1);
            cnt++;
            if(cnt > time){
              printf("get addr error\r\n");
              return -2;
            }
        }

        sprintf(hostip, "%s", ipaddr_ntoa(&rm_add));
        sprintf(ip,     "%s", ipaddr_ntoa(&rm_add));
        printf("hostnametoip - ip:%s\r\n", ip);
        gethostipfirst = 1;

    }



    return 0;
}

int create_connect(int sockfd, char *host, int port)
{
    printf("===2.%s->%d===\n", __func__, __LINE__);
    //域名解析
    char hostip[15];
    struct sockaddr_in cliaddr;
    unsigned long ul;
    fd_set fds;

    // 设置非阻塞
    ul = 1;
    //ioctl(sockfd, FIONBIO, &ul);
    //printf("%s->%d host=%s ===\n", __func__, __LINE__, host);
    int re = hostnametoip(host, hostip, 4);
    if(re < 0)
    {
        gethostipfirst = 1;
        printf("get hostip error\r\n");
    }
    //建立连接
    memset(&cliaddr, 0, sizeof(struct sockaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(port);
    cliaddr.sin_addr.s_addr= inet_addr(hostip);

    // 客户端连接
    printf("conenct conenct conenct\r\n");
    re = connect(sockfd, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr));
    if(re >= 0){
        return 1;
    }

    // 超时监听
    re = socket_timeout(sockfd, 1, NULL, &fds, NULL);
    if(re <= 0){
        close(sockfd);
        return -2;
    }

    // 设置为阻塞
    ul = 0;
    //ioctl(sockfd, FIONBIO, &ul);


    return re;

}




int socket_timeout(int sockfd, int s, fd_set *rfds, fd_set *wfds, fd_set *efds)
{
    int re = 0;
    struct timeval tm;

    int len;
    int error = -1;

    if(rfds != NULL){
        FD_ZERO(rfds);
        FD_SET(sockfd, rfds);
    }

    if(wfds != NULL){
        FD_ZERO(wfds);
        FD_SET(sockfd, wfds);
    }

    if(efds != NULL){
        FD_ZERO(efds);
        FD_SET(sockfd, efds);
    }

    if(s == 0){
        re = select(sockfd + 1, rfds, wfds, efds, NULL);
    }else{
        tm.tv_sec = s;
        tm.tv_usec = 0;
        re = select(sockfd + 1, rfds, wfds, efds, &tm);
    }

    if(re > 0)
    {
        //表示数据可读
        len = sizeof(int);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
        if(error == 0){
            re = 1;         // 正常
        }else{
            printf("select timeout");
            re = -2;        // select阻塞,此时不能进行读写操作
        }
    }
    else if(re == 0)
    {
        //socket超时
        re = 0;

    }
    else if(re == -1)
    {
        //select发生错误
        re = -1;
    }


    return re;
}





int socket_send(int sockfd, char *data, int len, int s)
{
    printf("===4.%s->%d===\n", __func__, __LINE__);
    int re = 0;
    int err = 0;
    int send = 0;
    int send_count = 0;
    int end_count = len;
    fd_set write_fds;

    while(end_count)
    {
        re = socket_timeout(sockfd, s, NULL, &write_fds, NULL);
	printf("===%s->%d re = %d===\n", __func__, __LINE__, re);
        if(re < 0){
            err = -1;       // 超时
	    printf("===%s->%d send timeout===\n", __func__, __LINE__);
            break;
        }

        if(FD_ISSET(sockfd, &write_fds)){
            printf("===%s->%d===\n", __func__, __LINE__);
            printf("%s\r\n", data);
            send = lwip_write(sockfd, data+send_count, end_count);
            if(send == 0){
                err = -2;          // 没有发送任何内容
                printf("===%s->%d  send nothing===\n", __func__, __LINE__);
                break;
            }

            if(send < 0){
                switch(errno)
                {
                    case EINTR:{
                        err = -3;       // 未知错误
                    }break;
                    case ENOSPC:{
                        err = -4;       // 内存空间不足
                    }break;
                    default: {
                        err = -5;       // write() 错误
                    }break;
                }
                break;
            }

            send_count  = send_count + send;
            end_count   = len - send_count;
        }
	sleep(2);
    }

    if(err < 0){
        return err;
    }

    printf("===%s->%d send successed ===\n", __func__, __LINE__);
    return send_count;
}





int socket_recv(int sockfd, char *response, int s)
{
    printf("===5.%s->%d ===\n", __func__, __LINE__);
    int re = 0;
    int len = 0;
    fd_set read_fds;

    char buffer[SOCKET_DEFAUT_MAX_SIZE];

    memset(buffer, 0, SOCKET_DEFAUT_MAX_SIZE);

    while(1)
    {

        re = socket_timeout(sockfd, s, &read_fds, NULL, NULL);
	printf("===%s->%d re = %d===\n", __func__, __LINE__, re);
        if(re < 0){
            re = -1;            // select异常
	    printf("===%s->%d recv timeout===\n", __func__, __LINE__);
            break;
        }

        if(FD_ISSET(sockfd, &read_fds)){
            len = lwip_read(sockfd, buffer, SOCKET_DEFAUT_MAX_SIZE);
            printf("%s->%d buffer:%s", __func__, __LINE__, buffer);
            if(len < 0){
                printf("read error");
                re = -2;        // read异常
                break;
            }else{
                break;
            }

        }
	sleep(1);
    }

    if(re < 0){
        return re;
    }
    printf("\r\n");

    memcpy(response, buffer, len);

    return len;
}


int makehttphead(char *host, int port, char *url, RequestMethod_e method, char *content, char *headpack)
{
    printf("===3.%s->%d===\n", __func__, __LINE__);
    char request_method[128];
    const char *newurl;
    char file_name[256] = {0};
    int j = 0;

    parse_url_for(url, &newurl, host, &port, file_name);

    //url检测,content检测
    if (content == NULL)
    {
            return -1;
    }

    //请求行：请求方法，请求url，请求协议版
    if(method == GET)
    {

        if(url == NULL){
		return -1;
        }else{
		j = sprintf(headpack, "GET %s %s\n", newurl, HTTP1_1);
        }
    }
    else if(method == POST)
    {
        if(url == NULL){
		return -1;
        }else{
		j = sprintf(headpack, "POST %s %s\n", url, HTTP1_1);
        }
    }
    //请求头： HTTP_DEFAULT_HEADER
    j += sprintf(headpack + j, "%s\r\n", HTTP_DEFAULT_HEADER);
    j += sprintf(headpack + j, "Host: %s:%d\r\n", host, port);
    j += sprintf(headpack + j, "Content-Length: %d\r\n\r\n", strlen(content));
    //请求数据
    j += sprintf(headpack + j , "%s\r\n", content);

    printf("%s\n", headpack);

    return 0;
}



/**
 * host         ：域名
 * port         ：端口
 * url          ：page
 * timeout      ：超时时间
 * method       : 使用哪种协议
 * content      : 内容
 * response     ：响应
 *
 */


int http(char *host, int port, char *url, int timeout, RequestMethod_e method, char *content, char *response)
{
    printf("--------------------------- 0.start http ---------------------------------------------\r\n");
    char headpack[4096];
    memset(headpack, 0, 4096);


//1.创建socket

    int myfd = create_socket();
    if(myfd < 0)
    {
        return -1;
    }

//2 connect
    int re = create_connect(myfd, host, port);
    if(re <= 0)
    {
        //如果连接失败，清空标志位
        close(myfd);
        gethostipfirst = 0;
        return -2;
    }else
    {
//3.makehead 配置http头

        int res = makehttphead(host, port, url, method, content, headpack);
        if (res < 0)
        {
            printf("data pack error \r\n");
        }

//4.发送
        int len = socket_send(myfd, headpack, strlen(headpack), timeout);
        printf("===%s->%d len:%d===\n", __func__, __LINE__,len);
        if(len <= 0){
            printf("socket_send\r\n");
            return -3;
        }
//5.接收:  注意response需要在函数外释放
        len = socket_recv(myfd, response, timeout);
        printf("===%s->%d len:%d===\n", __func__, __LINE__,len);
        if(len <= 0){
            printf("socket_recv error\r\n");
            return -4;
        }

    }

    if (myfd >= 0)
	close(myfd);

    printf("--------------------------- 6.end http ---------------------------------------------\r\n");
    return 0;

}
int cmd_httpclient_test(int argc, char ** argv)
{
  char host[] = "192.168.31.6";
  int  port   = 80;
  char url[]  = "http://192.168.31.6/henrisk/ranchao/test.txt";
  char data[] = "{\"data\":\"1\"}a,b,c,d,e,f";
  char http_re[1024];
  int rc;

  if (argc == 1){
      printf("Input a valid argv[1]:host argv[2]:url please\n");
      printf("default valid host:192.168.31.6 url:'http://192.168.31.6/ranchao/test.txt'\n");
  }
  else{
	if (argc == 2){
		strcpy(host, argv[1]);
	}else{
		strcpy(host, argv[1]);
		strcpy(url, argv[2]);
	}
  }
  while(1)
  {
    memset(http_re, 0, 1024);

    //uxHighWaterMark = uxTaskGetStackHighWaterMark( Tcp_TaskHandle );
    //printf("Tcp_TaskHandle remains %d%%\r\n",(uxHighWaterMark*100/4096));

    rc = http(host, port, url, 5, GET, data, http_re);
    if(rc < 0){
            printf("http post failed %d \r\n",rc);
            //return NULL;异常处理
    }else{

            if(http_re != NULL)
            {
		printf("--------- Http Response %s -------- \r\n", url);
		printf("%d:%s\r\n",strlen(http_re),http_re);;
            }else{
		printf("http_re post failed \n");
            }
    }

  }
  return 0;
}

//FINSH_FUNCTION_EXPORT(cmd_httpclient_test, httpclient_test http test);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_httpclient_test, __cmd_httpclient_test, http test);
