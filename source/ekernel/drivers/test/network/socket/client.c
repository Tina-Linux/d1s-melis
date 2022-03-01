/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2019年09月20日 星期五 17时20分55秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int cmd_client(int argc, char ** agrv[])
{
    int i;
    i = 0;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.31.6");
    serv_addr.sin_port = htons(1234);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char buffer[40];
    while(1){
	i++;
        lwip_read(sock, buffer, sizeof(buffer)-1);
        printf("==================================================\n");
	printf("Times = %d,Message from service:%s\n", i, buffer);
        printf("==================================================\n\n");
        sleep(1);
    }
    close(sock);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_client, __cmd_client, client);
