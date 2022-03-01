/*#####################################################################
# File Describe:download.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年03月16日 星期一 14时36分20秒
#====================================================================*/

#include <stdio.h>
#include <string.h>
//#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <lwip/netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <finsh.h>
#undef write
#undef read
struct resp_header//保持相应头信息
{
    int status_code;//HTTP/1.1 '200' OK
    char content_type[128];//Content-Type: application/gzip
    long content_length;//Content-Length: 11683079
    char file_name[256];
};

struct resp_header resp;//全剧变量以便在多个进程中使用

void parse_url(const char *url, const char **newurl_p, char *domain, int *port, char *file_name)
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
    printf("url:%s\n newurl:%s\n", url, *newurl_p);

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

struct resp_header get_resp_header(const char *response)
{
    /*获取响应头的信息*/
    struct resp_header resp;

    memset(&resp, 0x00, sizeof(resp));
    char *pos = strstr(response, "HTTP/");
    if (pos)
        sscanf(pos, "%*s %d", &resp.status_code);//返回状态码

    pos = strstr(response, "Content-Type:");//返回内容类型
    if (pos)
        sscanf(pos, "%*s %10s", resp.content_type);

    pos = strstr(response, "Content-Length:");//内容的长度(字节)
    if (pos)
        sscanf(pos, "%*s %ld", &resp.content_length);

    return resp;
}

void get_ip_addr(char *domain, char *ip_addr)
{
    printf("domain:%s\n",domain);
    /*通过域名得到相应的ip地址*/
    struct hostent *host = gethostbyname(domain);
    if (!host)
    {
        ip_addr = NULL;
        return;
    }

    for (int i = 0; host->h_addr_list[i]; i++)
    {
        strcpy(ip_addr, inet_ntoa( * (struct in_addr*) host->h_addr_list[i]));
        break;
    }
}


void progressBar(long cur_size, long total_size)
{
    /*用于显示下载进度条*/
    float percent = (float) cur_size / total_size;
    const int numTotal = 50;
    int numShow = (int)(numTotal * percent);

    if (numShow == 0)
        numShow = 1;

    if (numShow > numTotal)
        numShow = numTotal;

    char sign[51] = {0};
    memset(sign, '=', numTotal);

    printf("\r%.2f%%\t[%-*.*s] %.2f/%.2fMB",
		percent * 100,
		numTotal,
		numShow,
		sign,
		cur_size / 1024.0 / 1024.0,
		total_size / 1024.0 / 1024.0);
    fflush(stdout);

    if (numShow == numTotal)
        printf("\nDownload 100.00%%\n");
}

void * download(void * socket_d)
{
    /*下载文件函数, 放在线程中执行*/
    int client_socket = *(int *) socket_d;
    int length = 0;
    int mem_size = 4096;//mem_size might be enlarge, so reset it
    int buf_len = mem_size;//read 4k each time
    int len;
    char rtos_file_name[256+6];
    strcpy(rtos_file_name, "/data/");
    strcpy(rtos_file_name+6, resp.file_name);
    printf("rtos_file_name: %s ^_^\n\n",rtos_file_name);

    //创建文件描述符
    int fd = open(rtos_file_name, O_CREAT | O_WRONLY, S_IRWXG | S_IRWXO | S_IRWXU);
    if (fd < 0)
    {
        printf("Create file failed\n");
	return NULL;
    }

    char *buf = (char *) malloc(mem_size * sizeof(char));

    //从套接字中读取文件流
    while (length < resp.content_length && (len = lwip_read(client_socket, buf, buf_len)) != 0 )
    {
        write(fd, buf, len);
        length += len;
        progressBar(length, resp.content_length);
    }

    if (length == resp.content_length)
        printf("\nDownload successful ^_^\n\n");
    else
        printf("\nLength %d resp.content_length:%ld\n\n", length, resp.content_length);
    if (buf)
	    free(buf);
    if (fd >= 0)
	    close(fd);

    return NULL;

}

int cmd_wgets(int argc, char ** argv)
{
    /*
        test url:
        1. https://nodejs.org/dist/v4.2.3/node-v4.2.3-linux-x64.tar.gz
        2. http://img.ivsky.com/img/tupian/pre/201312/04/nelumbo_nucifera-009.jpg
    */
    char url[2048] = "127.0.0.1";
    const char *newurl;
    char domain[64] = {0};
    char ip_addr[16] = {0};
    int port = 80;
    char file_name[256] = {0};

    if (argc == 1)
    {
        printf("Input a valid URL please\n");
        return -1;
    }
    else
        strcpy(url, argv[1]);

    puts("1: Parsing url...");
    parse_url(url, &newurl, domain, &port, file_name);
    printf("url:%s\n newurl:%s\n", url, newurl);

    if (argc == 3)
        strcpy(file_name, argv[2]);

    puts("2: Get ip address...");
    get_ip_addr(domain, ip_addr);
    if (strlen(ip_addr) == 0)
    {
        printf("can not get ip address\n");
        return 0;
    }

    puts("\n>>>>Detail<<<<");
	printf("URL: %s\n", newurl);
    printf("DOMAIN: %s\n", domain);
    printf("IP: %s\n", ip_addr);
    printf("PORT: %d\n", port);
    printf("FILENAME: %s\n\n", file_name);

    //设置http请求头信息
    char header[2560] = {0};
    sprintf(header, \
            "GET %s HTTP/1.1\r\n"\
            "Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
            "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
            "Host:%s\r\n"\
            "Connection:close\r\n"\
            "\r\n"\
	    ,newurl, domain);

    printf("%s\n%d\n", header, (int) strlen(header));

    //创建套接字
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0)
    {
        printf("invalid socket descriptor: %d\n", client_socket);
        return -1;
    }

    //创建地址结构体
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    addr.sin_port = htons(port);

    //连接服务器
    puts("3: Connect server...");
    int res = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
    if (res == -1)
    {
        printf("connect failed, return: %d\n", res);
        return -1;
    }

    puts("4: Send request...");//向服务器发送下载请求
    lwip_write(client_socket, header, strlen(header));

    int mem_size = 4096;
    int length = 0;
    int len;
    char *buf = (char *) malloc(mem_size * sizeof(char));
    char *response = (char *) malloc(mem_size * sizeof(char));

    //每次单个字符读取响应头信息, 仅仅读取的是响应部分的头部, 后面单独开线程下载
    while ((len = lwip_read(client_socket, buf, 1)) != 0)
    {
        if (length + len > mem_size)
        {
            //动态内存申请, 因为无法确定响应头内容长度
            mem_size *= 2;
            char * temp = (char *) realloc(response, sizeof(char) * mem_size);
            if (temp == NULL)
            {
                printf("realloc failed\n");
		goto fail;
            }
            response = temp;
        }

        buf[len] = '\0';
        strcat(response, buf);

        //找到响应头的头部信息, 两个"\n\r"为分割点
        int flag = 0;
        for (int i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++);
        if (flag == 4)
            break;

        length += len;
    }

    printf("\n>>>>Response header:<<<<\n%s\n", response);

    resp = get_resp_header(response);
    strcpy(resp.file_name, file_name);

    printf("5: Start thread to download...\n");
    /*开新的线程下载文件*/
    pthread_t download_thread;
    pthread_create(&download_thread, NULL, download, (void *) &client_socket);
    pthread_join(download_thread, NULL);

    if (buf)
	free(buf);
    return 0;
fail:
    if (buf)
	free(buf);
    return -1;

}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_wgets, __cmd_wgets, download by single thread);
FINSH_FUNCTION_EXPORT(wgets, download by single thread);
