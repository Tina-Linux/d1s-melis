/*#####################################################################
# File Describe:dns.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年05月13日 星期三 14时57分57秒
#====================================================================*/
#include <stdio.h>
#include <rtthread.h>
#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/mem.h>
#include <lwip/icmp.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/inet.h>
#include <lwip/inet_chksum.h>
#include <lwip/ip.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <finsh.h>

int cmd_libdns_test(int argc, char * const argv[])
{
    printf("use: libdns www.baiud.com dns`s ip\n");
    printf("eg: libdns www.baiud.com 114.114.114.114\n");
    CURLcode libres;
    CURL *curl;
    ip_addr_t target_addr;
    struct addrinfo hint, *res = NULL;
    struct sockaddr_in *h = NULL;
    struct in_addr ina;

    memset(&hint, 0, sizeof(hint));
    /* convert URL to IP */
    if (lwip_getaddrinfo(argv[1], NULL, &hint, &res) != 0)
    {
        rt_kprintf("unknown host %s\n", argv[1]);
        return -RT_ERROR;
    }
    memcpy(&h, &res->ai_addr, sizeof(struct sockaddr_in *));
    memcpy(&ina, &h->sin_addr, sizeof(ina));
    lwip_freeaddrinfo(res);
    if (inet_aton(inet_ntoa(ina), &target_addr) == 0)
    {
        rt_kprintf("unknown host %s\n", argv[1]);
        return -RT_ERROR;
    }
        rt_kprintf("host %s\n", inet_ntoa(ina));

    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        printf("curl_global_init() failed\n");
       return -1;
    }

    curl = curl_easy_init();
    if (!curl) {
        printf("curl_easy_init() failed\n");
        curl_global_cleanup();
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, inet_ntoa(ina));
    curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, argv[2]);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    libres = curl_easy_perform(curl);

test_cleanup:

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return (int)libres;
}

FINSH_FUNCTION_EXPORT(cmd_libdns_test, libcurl test for https);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_libdns_test, __cmd_libdns, libcurl test for https);
