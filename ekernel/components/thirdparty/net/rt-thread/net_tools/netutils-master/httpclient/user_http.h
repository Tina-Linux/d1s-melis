#ifndef USER_HTTP_H
#define USER_HTTP_H



//请求方式
typedef enum {
    GET,
    POST
}RequestMethod_e;

//http版本
#define HTTP1_1 "HTTP/1.1"
#define HTTP2_0 "HTTP/2.0"


//hear头
#define HTTP_DEFAULT_HEADER         "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:63.0) Gecko/20100101 Firefox/63.0\r\n"   \
                                    "Connection: keep-alive\r\n" \
                                    "Cache-Control: no-cache\r\n"           \
                                    "Content-Type: application/json\r\n"   \
                                    "Accept: */*"
#define SOCKET_DEFAUT_MAX_SIZE 2048

int create_socket(void);
int hostnametoip(char *hostname, char *hostip, int time);
int socket_timeout(int sockfd, int s, fd_set *rfds, fd_set *wfds, fd_set *efds);
int create_connect(int sockfd, char *host, int port);
int socket_recv(int sockfd, char *data, int s);
int socket_send(int sockfd, char *data, int len, int s);
int http(char *host, int port, char *url, int timeout, RequestMethod_e method, char *content, char *response);


#endif

