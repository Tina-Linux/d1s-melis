/*************************************************************************
	> File Name: http_server.c
	> Author: flyranchao
	> Mail: flyranchao@allwinnertech.com
	> Created Time: 2020年03月23日 星期一 11时24分36秒
 ************************************************************************/

#include<stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#define DEFAULTIP "127.0.0.1"
#define DEFAULTPORT "80"
#define DEFAULTBACK "10"
#define DEFAULTDIR "/home"
#define DEFAULTLOG "/tmp/das-server.log"

void prterrmsg(char *msg);
#define prterrmsg(msg)        { perror(msg); abort();  }
void wrterrmsg(char *msg);
#define wrterrmsg(msg)        { fputs(msg, logfp); fputs(strerror(errno), logfp);fflush(logfp); abort();  }

void prtinfomsg(char *msg);
#define prtinfomsg(msg)        { fputs(msg, stdout);   }
void wrtinfomsg(char *msg);
#define wrtinfomsg(msg)        {  fputs(msg, logfp); fflush(logfp); }

#define MAXBUF        2048

char buffer[MAXBUF + 1];
char *host = 0;
char *port = 0;
char *back = 0;
char *dirroot = 0;
char *logdir = 0;
unsigned char daemon_y_n = 0;
FILE *logfp;

#define MAXPATH        150

/*----------------------------------------
 * *--- dir_up - 查找dirpath所指目录的上一级目录
 * *----------------------------------------
 * */
char *dir_up(char *dirpath)
{
    static char Path[MAXPATH];
    int len;

    strcpy(Path, dirpath);
    len = strlen(Path);
    if (len > 1 && Path[len - 1] == '/')
	len--;
    while (Path[len - 1] != '/' && len > 1)
	len--;
    Path[len] = 0;
    return Path;

}

 /*------------------------------------------------------
 * *--- AllocateMemory - 分配空间并把d所指的内容复制
 * *------------------------------------------------------
 * */
void AllocateMemory(char **s, int l, char *d)
{
    *s = malloc(l + 1);
    bzero(*s, l + 1);
    memcpy(*s, d, l);

}

 /************关于本文档********************************************
  * *filename: das-server.c
  * *purpose: 这是在Linux下用C语言写的目录访问服务器，支持目录浏览和文件下载
  * *wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
  * Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
  * *date time:2007-01-26 19:32
  * *Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
  * * 但请遵循GPL
  * *Thanks to: Google.com
  * *Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
  * * 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
  * *********************************************************************/
  /*------------------------------------------------------
   * *--- GiveResponse - 把Path所指的内容发送到client_sock去
   * *-------------------如果Path是一个目录，则列出目录内容
   * *-------------------如果Path是一个文件，则下载文件
   * *------------------------------------------------------
   * */
void GiveResponse(FILE * client_sock, char *Path)
{
    struct dirent *dirent;
    struct stat info;
    char Filename[MAXPATH];
    DIR *dir;
    int fd, len, ret, rtt;
    char *p, *realPath, *realFilename, *nport;

    /* 获得实际工作目录或文件 */
    len = strlen(dirroot) + strlen(Path) + 1;
    realPath = malloc(len + 1);
    bzero(realPath, len + 1);
    sprintf(realPath, "%s/%s", dirroot, Path);
    //sprintf(realPath, "%s", Path);
    printf("===%s->%d dirroot:%s Path:%s===\n", __func__, __LINE__, dirroot, Path);
    printf("===%s->%drealPath:%s===\n", __func__, __LINE__, realPath);

    /* 获得实际工作端口 */
    len = strlen(port) + 1;
    nport = malloc(len + 1);
    bzero(nport, len + 1);
    sprintf(nport, ":%s", port);

    rtt = stat(realPath, &info);
    printf("===%s->%drtt:%d===\n", __func__, __LINE__, rtt);
 /* 获得实际工作目录或文件的信息以判断是文件还是目录 */
    //if (stat(realPath, &info)) {
    if (rtt != 0) {
        printf("===%s->%drealPath:%s===\n", __func__, __LINE__, realPath);
	fprintf(client_sock,
		"HTTP/1.1 200 OK\r\nServer: DAS by ZhouLifa\r\nConnection: close\r\n\r\n<html><head><title>%d - %s</title></head>"
		"<body><font size=+4>Linux 下目录访问服务器</font><br><hr width=\"100%%\"><br><center>"
		"<table border cols=3 width=\"100%%\">", errno,
		strerror(errno));
	fprintf(client_sock,
		"</table><font color=\"CC0000\" size=+2>请向管理员咨询为何出现如下错误提示：\n%s %s</font></body></html>",
		Path, strerror(errno));
	goto out;
    }
    //printf("===%s->%dclient_socket:%p===\n", __func__, __LINE__, client_sock);

    /* 处理浏览文件请求，即下载文件 */
    if (S_ISREG(info.st_mode)) {
        //printf("===%s->%d download file===\n", __func__, __LINE__);
	fd = open(realPath, O_RDONLY);
	len = lseek(fd, 0, SEEK_END);
	p = (char *) malloc(len + 1);
	bzero(p, len + 1);
	lseek(fd, 0, SEEK_SET);
	ret = read(fd, p, len);
	close(fd);
	fprintf(client_sock,
		"HTTP/1.1 200 OK\r\nServer: DAS by ZhouLifa\r\nConnection: keep-alive\r\nContent-type: application/*\r\nContent-Length:%d\r\n\r\n",
		len);
	fwrite(p, len, 1, client_sock);
	free(p);
	//printf("===%s->%d===\n", __func__, __LINE__);

    } else if (S_ISDIR(info.st_mode)) {
	/* 处理浏览目录请求 */
        //printf("===%s->%d request dir ===\n", __func__, __LINE__);
	dir = opendir(realPath);
	fprintf(client_sock,
		"HTTP/1.1 200 OK\r\nServer: DAS by ZhouLifa\r\nConnection: close\r\n\r\n<html><head><title>%s</title></head>"
		"<body><font size=+4>Linux 下目录访问服务器</font><br><hr width=\"100%%\"><br><center>"
		"<table border cols=3 width=\"100%%\">", Path);
	fprintf(client_sock,
		"<caption><font size=+3>目录 %s</font></caption>\n",
		Path);
	fprintf(client_sock,
		"<tr><td>名称</td><td>大小</td><td>修改时间</td></tr>\n");
	if (dir == 0) {
	    fprintf(client_sock,
		    "</table><font color=\"CC0000\" size=+2>%s</font></body></html>",
		    strerror(errno));
	    return;
	}
	/* 读取目录里的所有内容 */
	while ((dirent = readdir(dir)) != 0) {
	    if (strcmp(Path, "/") == 0)
		sprintf(Filename, "/%s", dirent->d_name);
	    else
		sprintf(Filename, "%s/%s", Path, dirent->d_name);
	    fprintf(client_sock, "<tr>");
	    len = strlen(dirroot) + strlen(Filename) + 1;
	    realFilename = malloc(len + 1);
	    bzero(realFilename, len + 1);
	    sprintf(realFilename, "%s/%s", dirroot, Filename);
	    if (stat(realFilename, &info) == 0) {
		if (strcmp(dirent->d_name, "..") == 0)
		    fprintf(client_sock,
			    "<td><a href=\"http://%s%s%s\">(parent)</a></td>",
			    host, atoi(port) == 80 ? "" : nport,
			    dir_up(Path));
		else
		    fprintf(client_sock,
			    "<td><a href=\"http://%s%s%s\">%s</a></td>",
			    host, atoi(port) == 80 ? "" : nport, Filename,
			    dirent->d_name);
		if (S_ISDIR(info.st_mode))
		    fprintf(client_sock, "<td>目录</td>");
		else if (S_ISREG(info.st_mode))
		    fprintf(client_sock, "<td>%d</td>", info.st_size);
		else if (S_ISLNK(info.st_mode))
		    fprintf(client_sock, "<td>链接</td>");
		else if (S_ISCHR(info.st_mode))
		    fprintf(client_sock, "<td>字符设备</td>");
		else if (S_ISBLK(info.st_mode))
		    fprintf(client_sock, "<td>块设备</td>");
		else if (S_ISFIFO(info.st_mode))
		    fprintf(client_sock, "<td>FIFO</td>");
		else if (S_ISSOCK(info.st_mode))
		    fprintf(client_sock, "<td>Socket</td>");
		else
		    fprintf(client_sock, "<td>(未知)</td>");
		fprintf(client_sock, "<td>%s</td>", ctime(&info.st_ctime));
	    }
	    fprintf(client_sock, "</tr>\n");
	    free(realFilename);

	}
	fprintf(client_sock, "</table></center></body></html>");
        //printf("===%s->%d===\n", __func__, __LINE__);

    } else {
        //printf("===%s->%d can not found===\n", __func__, __LINE__);
	/* 既非常规文件又非目录，禁止访问 */
	fprintf(client_sock,
		"HTTP/1.1 200 OK\r\nServer: DAS by ZhouLifa\r\nConnection: close\r\n\r\n<html><head><title>permission denied</title></head>"
		"<body><font size=+4>Linux 下目录访问服务器</font><br><hr width=\"100%%\"><br><center>"
		"<table border cols=3 width=\"100%%\">");
	fprintf(client_sock,
		"</table><font color=\"CC0000\" size=+2>你访问的资源'%s'被禁止访问，请联系管理员解决！</font></body></html>",
		Path);
    }
  out:
    //printf("===%s->%d===\n", __func__, __LINE__);
    free(realPath);
    free(nport);
}

   /*------------------------------------------------------
    * *--- getoption - 分析取出程序的参数
    * *------------------------------------------------------
    * */
void getoption(int argc, char **argv)
{
    int c, len;
    char *p = 0;

    opterr = 0;
    while (1) {
	int option_index = 0;
	static struct option long_options[] = {
	    {"host", 1, 0, 0},
	    {"port", 1, 0, 0},
	    {"back", 1, 0, 0},
	    {"dir", 1, 0, 0},
	    {"log", 1, 0, 0},
	    {"daemon", 0, 0, 0},
	    {0, 0, 0, 0}

	};
	/* 本程序支持如一些参数：
	 ** --host IP地址 或者 -H IP地址
	 ** --port 端口 或者 -P 端口
	 ** --back 监听数量 或者 -B 监听数量
	 ** --dir 网站根目录 或者 -D 网站根目录
	 ** --log 日志存放路径 或者 -L 日志存放路径
	 ** --daemon 使程序进入后台运行模式
	 **/
	c = getopt_long(argc, argv, "H:P:B:D:L", long_options,
			&option_index);
	if (c == -1 || c == '?')
	    break;
	if (optarg)
	    len = strlen(optarg);
	else
	    len = 0;
	if ((!c && !(strcasecmp(long_options[option_index].name, "host")))
	    || c == 'H')
	    p = host = malloc(len + 1);
	else if ((!c
		  &&
		  !(strcasecmp(long_options[option_index].name, "port")))
		 || c == 'P')
	    p = port = malloc(len + 1);
	else if ((!c
		  &&
		  !(strcasecmp(long_options[option_index].name, "back")))
		 || c == 'B')
	    p = back = malloc(len + 1);
	else if ((!c
		  && !(strcasecmp(long_options[option_index].name, "dir")))
		 || c == 'D')
	    p = dirroot = malloc(len + 1);
	else if ((!c
		  && !(strcasecmp(long_options[option_index].name, "log")))
		 || c == 'L')
	    p = logdir = malloc(len + 1);
	else if ((!c
		  &&
		  !(strcasecmp
		    (long_options[option_index].name, "daemon")))) {
	    daemon_y_n = 1;
	    continue;

	} else
	    break;
	bzero(p, len + 1);
	memcpy(p, optarg, len);

    }
    //printf("===%s->%d ip:%s port:%s===\n", __func__, __LINE__,host, port);

}

int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    int sock_fd, addrlen;
    /* 获得程序工作的参数，如 IP 、端口、监听数、网页根目录、目录存放位置等 */
    getoption(argc, argv);

    if (!host) {
	addrlen = strlen(DEFAULTIP);
	AllocateMemory(&host, addrlen, DEFAULTIP);

    }
    if (!port) {
	addrlen = strlen(DEFAULTPORT);
	AllocateMemory(&port, addrlen, DEFAULTPORT);

    }
    if (!back) {
	addrlen = strlen(DEFAULTBACK);
	AllocateMemory(&back, addrlen, DEFAULTBACK);

    }
    if (!dirroot) {
	addrlen = strlen(DEFAULTDIR);
	AllocateMemory(&dirroot, addrlen, DEFAULTDIR);

    }
    if (!logdir) {
	addrlen = strlen(DEFAULTLOG);
	AllocateMemory(&logdir, addrlen, DEFAULTLOG);

    }

    printf
	("host=%s port=%s back=%s dirroot=%s logdir=%s %s是后台工作模式(进程ID：%d)\n",
	 host, port, back, dirroot, logdir, daemon_y_n ? "" : "不",
	 getpid());

    /* fork() 两次处于后台工作模式下 */
    if (daemon_y_n) {
	if (fork())
	    exit(0);
	if (fork())
	    exit(0);
	close(0), close(1), close(2);
	logfp = fopen(logdir, "a+");
	if (!logfp)
	    exit(0);

    }

    /* 处理子进程退出以免产生僵尸进程 */
    signal(SIGCHLD, SIG_IGN);

    /* 创建 socket */
    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	if (!daemon_y_n) {
	    prterrmsg("socket()");

	} else {
	    wrterrmsg("socket()");

	}

    }

    /* 设置端口快速重用 */
    addrlen = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &addrlen,
	       sizeof(addrlen));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(host);
    addrlen = sizeof(struct sockaddr_in);
    /* 绑定地址、端口等信息 */
    if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0) {
	if (!daemon_y_n) {
	    prterrmsg("bind()");

	} else {
	    wrterrmsg("bind()");

	}

    }

    /* 开启临听 */
    if (listen(sock_fd, atoi(back)) < 0) {
	if (!daemon_y_n) {
	    prterrmsg("listen()");

	} else {
	    wrterrmsg("listen()");

	}

    }
    while (1) {
	int len;
	int new_fd;
	addrlen = sizeof(struct sockaddr_in);
	/* 接受新连接请求 */
	new_fd = accept(sock_fd, (struct sockaddr *) &addr, &addrlen);
	if (new_fd < 0) {
	    if (!daemon_y_n) {
		prterrmsg("accept()");

	    } else {
		wrterrmsg("accept()");

	    }
	    break;

	}
	bzero(buffer, MAXBUF + 1);
	sprintf(buffer, "连接来自于: %s:%d\n",
		inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	if (!daemon_y_n) {
	    prtinfomsg(buffer);

	} else {
	    wrtinfomsg(buffer);

	}
    //printf("===%s->%d===\n", __func__, __LINE__);
	/* 产生一个子进程去处理请求，当前进程继续等待新的连接到来 */
	if (!fork()) {
	    bzero(buffer, MAXBUF + 1);
	    if ((len = recv(new_fd, buffer, MAXBUF, 0)) > 0) {
		FILE *ClientFP = fdopen(new_fd, "w");
		if (ClientFP == NULL) {
		    if (!daemon_y_n) {
			prterrmsg("fdopen()");

		    } else {
			prterrmsg("fdopen()");

		    }

		} else {
		    char Req[MAXPATH + 1] = "";
		    scanf(buffer, "GET %s HTTP", Req);
		    bzero(buffer, MAXBUF + 1);
		    sprintf(buffer, "请求取文件: \"%s\"\n", Req);
		    if (!daemon_y_n) {
			prtinfomsg(buffer);

		    } else {
			wrtinfomsg(buffer);

		    }
		    /* 处理用户请求 */
		    printf("===%s->%d Req:%s===\n", __func__, __LINE__,Req);
		    GiveResponse(ClientFP, Req);
		    fclose(ClientFP);

		}

	    }
	    exit(0);

	}
	close(new_fd);

    }
    close(sock_fd);
    return 0;

}
