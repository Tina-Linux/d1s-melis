/*#####################################################################
# File Describe:download_mult.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年03月16日 星期一 15时51分38秒
#====================================================================*/

#include <stdio.h>//printf
#include <string.h>//字符串处理
//#include <lwip/sockets.h>//套接字
#include <lwip/inet.h>//ip地址处理
#include <fcntl.h>//open系统调用
#include <unistd.h>//write系统调用
#include <lwip/netdb.h>//查询DNS
#include <stdlib.h>//exit函数
#include <sys/types.h>
#include <sys/stat.h>//stat系统调用获取文件大小
#include <pthread.h>
#include <sys/time.h>//获取下载时间
#include <finsh.h>
#undef write
#undef read
struct resp_header_m//保持相应头信息
{
    int status_code;//HTTP/1.1 '200' OK
    char content_type[128];//Content-Type: application/gzip
    long content_length;//Content-Length: 11683079
    char file_name[256];
};

struct resp_header_m resp_m;//全剧变量以便在多个进程中使用

void parse_url_m(const char *url, const char **newurl_p, char *domain_m, int *port, char *file_name)
{
    /*通过url解析出域名, 端口, 以及文件名*/
    int i, j = 0;
    int start = 0;
    *port = 80;
    char *patterns[] = {"http://", "https://", NULL};

    for (int i = 0; patterns[i]; i++)//分离下载地址中的http协议
        if (strncmp(url, patterns[i], strlen(patterns[i])) == 0)
            start = strlen(patterns[i]);

    //解析域名, 这里处理时域名后面的端口号会保留
    for (i = start; url[i] != '/' && url[i] != '\0'; i++, j++)
        domain_m[j] = url[i];
    domain_m[j] = '\0';

    *newurl_p = url + i + 1;
    printf("start:%d\n", start);
    printf("url:%s\n newurl:%s\n", url, *newurl_p);
    //解析端口号, 如果没有, 那么设置端口为80
    char *pos = strstr(domain_m, ":");
    if (pos)
        sscanf(pos, ":%d", port);

    //删除域名端口号
    for (int i = 0; i < (int)strlen(domain_m); i++)
    {
        if (domain_m[i] == ':')
        {
            domain_m[i] = '\0';
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

struct resp_header_m get_resp_header_m(const char *response)
{
    /*获取响应头的信息*/
    struct resp_header_m resp_m;

    memset(&resp_m, 0x00, sizeof(resp_m));
    char *pos = strstr(response, "HTTP/");
    if (pos)//获取返回代码
        sscanf(pos, "%*s %d", &resp_m.status_code);

    pos = strstr(response, "Content-Type:");
    if (pos)//获取返回文档类型
        sscanf(pos, "%*s %10s", resp_m.content_type);

    pos = strstr(response, "Content-Length:");
    if (pos)//获取返回文档长度
        sscanf(pos, "%*s %ld", &resp_m.content_length);

    return resp_m;
}

void get_ip_addr_mult(char *domain_m, char *ip_addr)
{
    printf("domain:%s\n",domain_m);
    /*通过域名得到相应的ip地址*/
    struct hostent *host = gethostbyname(domain_m);//此函数将会访问DNS服务器
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


void progressBar_m(long cur_size, long total_size)
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

unsigned long get_file_size(const char *filename)
{
    //通过系统调用直接得到文件的大小
    struct stat buf;
    if (stat(filename, &buf) < 0)
        return 0;
    return (unsigned long) buf.st_size;
}

void download_mult(int client_socket, char *file_name, long content_length)
{
    /*下载文件函数*/
    long hasrecieve = 0;//记录已经下载的长度
    struct timeval t_start, t_end;//记录一次读取的时间起点和终点, 计算速度
    int mem_size = 4096;//缓冲区大小8K
    int buf_len = mem_size;//理想状态每次读取8K大小的字节流
    int len;
    char rtos_file_name[256+6];
    strcpy(rtos_file_name, "/data/");
    strcpy(rtos_file_name+6, resp_m.file_name);
    printf("rtos_file_name: %s ^_^\n\n",rtos_file_name);

    //创建文件描述符
    int fd = open(rtos_file_name, O_CREAT | O_WRONLY, S_IRWXG | S_IRWXO | S_IRWXU);
    if (fd < 0)
    {
        printf("Create file failed\n");
	    return;
    }

    char *buf = (char *) malloc(mem_size * sizeof(char));

    //从套接字流中读取文件流
    long diff = 0;
    int prelen = 0;
    double speed = 0;

    while (hasrecieve < resp_m.content_length)
    {
        gettimeofday(&t_start, NULL ); //获取开始时间
        len = lwip_read(client_socket, buf, buf_len);
        write(fd, buf, len);
        gettimeofday(&t_end, NULL ); //获取结束时间

        hasrecieve += len;//更新已经下载的长度

        //计算速度
        if (t_end.tv_usec - t_start.tv_usec >= 0 &&  t_end.tv_sec - t_start.tv_sec >= 0)
            diff += 1000000 * ( t_end.tv_sec - t_start.tv_sec ) + (t_end.tv_usec - t_start.tv_usec);//us

        if (diff >= 1000000)//当一个时间段大于1s=1000000us时, 计算一次速度
        {
            speed = (double)(hasrecieve - prelen) / (double)diff * (1000000.0 / 1024.0);
            prelen = hasrecieve;//清零下载量
            diff = 0;//清零时间段长度
        }

        progressBar_m(hasrecieve, resp_m.content_length);

        if (hasrecieve == resp_m.content_length)
            break;
    }
    free(buf);
    close(fd);
}

int cmd_wgetm(int argc, char const *argv[])
{
    /* 命令行参数: 接收两个参数, 第一个是下载地址, 第二个是文件的保存位置和名字, 下载地址是必须的, 默认下载到当前目录
     * 示例: ./download http://www.baidu.com baidu.html
     */
    char url[2048] = "127.0.0.1";//设置默认地址为本机,
    const char *newurl;
    char domain_m[64] = {0};//远程主机地址
    char ip_addr[16] = {0};//远程主机IP地址
    int port = 80;//远程主机端口, http默认80端口
    char file_name[256] = {0};//下载文件名

    if (argc == 1)
    {
        printf("Input a valid URL please\n");
        return -1;
    }
    else
        strcpy(url, argv[1]);

    puts("1: Parsing url...");
    parse_url_m(url, &newurl, domain_m, &port, file_name);//从url中分析出主机名, 端口号, 文件名
    printf("url:%s\n newurl:%s\n", url, newurl);

    if (argc == 3)
    {
        printf("\tyou have download the file : %s\n", argv[2]);
        strcpy(file_name, argv[2]);
    }

    puts("2: Get ip address...");
    get_ip_addr_mult(domain_m, ip_addr);//调用函数同访问DNS服务器获取远程主机的IP
    if (strlen(ip_addr) == 0)
    {
        printf("can not get ip address\n");
        return 0;
    }

    puts("\n>>>>Detail<<<<");
	printf("\tURL: %s\n", newurl);
    printf("\tDOMAIN: %s\n", domain_m);
    printf("\tIP: %s\n", ip_addr);
    printf("\tPORT: %d\n", port);
    printf("\tFILENAME: %s\n\n", file_name);

    //设置http请求头信息
    char header[2560] = {0};
    sprintf(header, \
            "GET %s HTTP/1.1\r\n"\
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
            "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
            "Host: %s\r\n"\
            "Connection: keep-alive\r\n"\
            "\r\n"\
        ,newurl, domain_m);

    printf("%s\n%d\n", header, (int) strlen(header));
    puts("3: create socket...");
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0)
    {
        printf("invalid socket descriptor: %d\n", client_socket);
       return -1;
    }

    //创建IP地址结构体
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    addr.sin_port = htons(port);

    //连接远程主机
    puts("4: Connect server...");
    int res = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
    if (res == -1)
    {
        printf("connect failed, return: %d\n", res);
        return -1;
    }

    puts("5: Send request...");
    lwip_write(client_socket, header, strlen(header));//write系统调用, 将请求header发送给服务器


    int mem_size = 4096;
    int length = 0;
    int len;
    char *buf = (char *) malloc(mem_size * sizeof(char));
    char *response = (char *) malloc(mem_size * sizeof(char));

    //每次单个字符读取响应头信息
    puts("6: Response header:...");
    while ((len = lwip_read(client_socket, buf, 1)) != 0)
    {
        if (length + len > mem_size)
        {
            //动态内存申请, 因为无法确定响应头内容长度
            mem_size *= 2;
            char * temp = (char *) realloc(response, sizeof(char) * mem_size);
            if (temp == NULL)
            {
                printf("malloc mem failed\n");
		goto fail;
            }
            response = temp;
        }

        buf[len] = '\0';
        strcat(response, buf);

        //找到响应头的头部信息
        int flag = 0;
        for (int i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++);
        if (flag == 4)//连续两个换行和回车表示已经到达响应头的头尾, 即将出现的就是需要下载的内容
            break;

        length += len;
    }

  
    printf("\n>>>>Response header:<<<<\n%s\n", response);
    printf("\n>>>>http header success:<<<<\n");
    resp_m = get_resp_header_m(response);
    strcpy(resp_m.file_name, file_name);
    printf("\tHTTP code: %d\n", resp_m.status_code);
    if (resp_m.status_code != 200)
    {
        printf("can not download status: %d\n", resp_m.status_code);
	goto fail;
    }
    printf("\tHTTP file type: %s\n", resp_m.content_type);
    printf("\tHTTP body length: %ld字节\n\n", resp_m.content_length);


    printf("7: start download...\n");
    download_mult(client_socket, file_name, resp_m.content_length);
    printf("8: close socket\n");

    if (resp_m.content_length == get_file_size(file_name))
        printf("\nfile %s download success! ^_^\n\n", file_name);
    else
    {
        remove(file_name);
        printf("\nfile download failed retry!\n\n");
    }
    shutdown(client_socket, 2);//关闭套接字的接收和发送
    if (buf)
	free(buf);
    return 0;
fail:
    if (buf)
	free(buf);
    return -1;

}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_wgetm, __cmd_wgetm, download by mult thread);
FINSH_FUNCTION_EXPORT(wgetm, download by mult thread);
