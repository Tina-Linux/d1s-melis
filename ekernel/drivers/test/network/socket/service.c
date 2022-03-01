/*************************************************************************
	> File Name: ser.c
	> Author: 
	> Mail: 
	> Created Time: 2019年09月20日 星期五 17时08分55秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
//#include<netinet/in.h>

int cmd_server(int argc, char ** argv[])
{
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);

    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serv_sock, 20);

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    char str[] = "hello socket...";
    while(1){
        lwip_write(clnt_sock, str, sizeof(str));
        sleep(1);
    }
    close(clnt_sock);
    close(serv_sock);

    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cmd_server, __cmd_server, server);
