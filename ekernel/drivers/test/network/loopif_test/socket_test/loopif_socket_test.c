/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
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
#include <rtthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <lwip/sockets.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "arch/sys_arch.h"

#define BUFF_SIZE 64

void loop_if_server(void *unuse)
{
    char buff[BUFF_SIZE];
    int ret,i;
    int server_sockfd, client_sockfd;
    int server_len;
    socklen_t client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*  Name the socket.  */

    server_address.sin_family = AF_INET;
//    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");;
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    ret = listen(server_sockfd, 5);
/*  Create a connection queue and wait for clients.  */

    printf("server waiting\n");

/*  Accept a connection.  */

	client_len = sizeof(client_address);
	client_sockfd = accept(server_sockfd,
	(struct sockaddr *)&client_address, &client_len);

	printf("[%s,%d],enter\n",__func__,__LINE__);


    while(1) {
        char ch;
        lwip_read(client_sockfd, buff, BUFF_SIZE);
	for(i=0; i<BUFF_SIZE; i++) {
		buff[i]=i;
		//printf("===%s-%d buf:%d===\n", __func__, __LINE__,buff[i]);
	}
	hex_dump(__func__,16,buff,BUFF_SIZE);
	printf("=====================================================\n");
    }
    close(client_sockfd);
	while(1);
}

void loop_if_client(void *unuse)
{
    int sockfd;
    int len,ret;
    struct sockaddr_in address;
    int result;
    int i;
    int time = 0;
    char ch = '1';
    char buff[BUFF_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*  Name the socket, as agreed with the server.  */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9734);
    len = sizeof(address);

    printf("[%s,%d],enter\n",__func__,__LINE__);
/*  Now connect our socket to the server's socket.  */

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if(result == -1) {
	printf("[%s,%d],enter\n",__func__,__LINE__);
        perror("oops: client");
		goto end;
    }
	while(1) {
		time++;
		for(i=0; i<BUFF_SIZE; i++) {
			buff[i]=i;
			//printf("===%s-%d buf:%d===\n", __func__, __LINE__,buff[i]);
		}
		printf("\n=========[%s %d] send total: %d=========\n", __func__, time, BUFF_SIZE);
		ret = lwip_write(sockfd, buff, BUFF_SIZE);
		sleep(2);
	}
    close(sockfd);
end:
	while(1);
}
void cmd_loop_server(void)
{
	rt_thread_t ret;
	printf("Creating loop_if_server_main task\n");
	ret = rt_thread_create("loop_server", loop_if_server, RT_NULL, 1024*4+BUFF_SIZE, 3, 20);
	RT_ASSERT(ret != RT_NULL);
	rt_thread_startup(ret);
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_loop_server, __cmd_loop_server, loopserver);

void cmd_loop_client(void)
{
	rt_thread_t ret;
	printf("Creating loop_if_client_main task\n");
	ret = rt_thread_create("loop_client", loop_if_client, RT_NULL, 1024*4+BUFF_SIZE, 3, 20);
	RT_ASSERT(ret != RT_NULL);
	rt_thread_startup(ret);
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_loop_client, __cmd_loop_client, loopclient);