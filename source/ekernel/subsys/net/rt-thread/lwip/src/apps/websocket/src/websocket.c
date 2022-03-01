#include "websocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include "aos/kernel.h"
//#include "aos/log.h"
//#include "aos/debug.h"
#include "transport_ws.h"
#include "transport_tcp.h"
#ifdef CONFIG_USING_TLS
#include "transport_ssl.h"
#endif

#define TAG "websocket"

#define DEFAULT_WS_PORT     (80)
#define DEFAULT_WSS_PORT    (443)

struct websocket {
    char                        *host;
    int                         port;
    char                        *path;
    char                        *protocol;
    int                         timeout_ms;
    int                         buffer_size;
    transport_handle_t          ws;
#if defined(CONFIG_USING_TLS)
    transport_handle_t          ssl;
#endif
    transport_handle_t          tcp;
};
typedef struct websocket websocket_t;

websocket_handle_t websocket_create(websocket_config_t *config)
{
    char *ptr;
    char *host;
    int host_len;
    int port;
    int transport_type;
    websocket_handle_t websocket = NULL;
    transport_handle_t ws = NULL;

    aos_check_return_null(config);
    websocket = aos_zalloc(sizeof(websocket_t));
    aos_check_return_null(websocket);

    transport_type = config->transport_type;
    if (config->url != NULL) {
        if (strncmp(config->url, "ws://", 5) == 0) {
            host = (char *)(config->url + 5);
            port = DEFAULT_WS_PORT;
            transport_type = WS_TRANSPORT_OVER_TCP;
        } else if (strncmp(config->url, "wss://", 6) == 0) {
            host = (char *)(config->url + 6);
            port = DEFAULT_WSS_PORT;
            transport_type = WS_TRANSPORT_OVER_SSL;
        } else {
            goto _fail;
        }
        ptr = strstr(host, "/");
        if (!ptr) {
            host_len = strlen(host);
            websocket->path = strdup("/");
            websocket->host = strdup(host);
        } else {
            host_len = ptr - host;
            char *p = aos_malloc(host_len + 1);
            if (!p) {
                goto _fail;
            }
            memcpy(p, host, host_len);
            p[host_len] = 0;
            websocket->path = strdup(host + host_len);
            websocket->host = strdup(p);
            aos_free(p);
        }
        websocket->port = port;
    } else if (config->host != NULL) {
        websocket->host = strdup(config->host);
        websocket->port = config->port;
        websocket->path = strdup("/");
    } else {
        goto _fail;
    }

    if (config->protocol != NULL) {
        websocket->protocol = strdup(config->protocol);
    }

    if (config->buffer_size == 0) {
        websocket->buffer_size = DEFAULT_WS_BUF_SIZE;
    }

    websocket->timeout_ms = config->timeout_ms;

    LOGD(TAG, "ws## %s, %d, %s, %s", websocket->host, websocket->port, websocket->path, websocket->protocol);

    if (transport_type == WS_TRANSPORT_OVER_SSL) {
#if defined(CONFIG_USING_TLS)
        transport_handle_t ssl = transport_ssl_init();
        if (ssl == NULL) {
            goto _fail;
        }
        ws = transport_ws_init(ssl, websocket->path, websocket->protocol, WS_SEND_TYPE_BIN, websocket->buffer_size);
        websocket->ssl = ssl;

        if (config->use_global_ca_store == true) {
            transport_ssl_enable_global_ca_store(ssl);
        } else if (config->cert_pem) {
            transport_ssl_set_cert_data(ssl, config->cert_pem, strlen(config->cert_pem));
        }

        if (config->client_cert_pem) {
            transport_ssl_set_client_cert_data(ssl, config->client_cert_pem, strlen(config->client_cert_pem));
        }

        if (config->client_key_pem) {
            transport_ssl_set_client_key_data(ssl, config->client_key_pem, strlen(config->client_key_pem));
        }
#endif
    } else {
        transport_handle_t tcp = transport_tcp_init();
        if (!tcp) {
            goto _fail;
        }
        ws = transport_ws_init(tcp, websocket->path, websocket->protocol, WS_SEND_TYPE_BIN, websocket->buffer_size);
        websocket->tcp = tcp;
    }
    if (ws == NULL) {
        goto _fail;
    }
    websocket->ws = ws;

    if (transport_connect(ws, websocket->host, websocket->port, websocket->timeout_ms) != 0) {
        LOGE(TAG, "ws conn error");
        goto _fail;
    }

    LOGD(TAG, "ws init and connect ok");
    return websocket;
_fail:
    LOGE(TAG, "ws conn e, need destroy");
    websocket_destroy(websocket);
    return NULL;
}

int websocket_write(websocket_handle_t client, const unsigned char *buffer, size_t len, uint32_t timeout_ms, ws_send_type_t send_type)
{
    transport_ws_set_sendtype(client->ws, send_type);
    return transport_write(client->ws, (const char *)buffer, len, timeout_ms);
}

int websocket_read(websocket_handle_t client, unsigned char *buffer, size_t len, uint32_t timeout_ms)
{
    return transport_read(client->ws, (char *)buffer, len, timeout_ms);
}

int websocket_destroy(websocket_handle_t client)
{
    if (client) {

        if (client->host) {
            aos_free(client->host);
        }
        if (client->path) {
            aos_free(client->path);
        }
        if (client->protocol) {
            aos_free(client->protocol);
        }
        if (client->ws)
            transport_close(client->ws);
#if defined(CONFIG_USING_TLS)
        if (client->ssl) {
            transport_destroy(client->ssl);
        }
#endif
        if (client->tcp) {
            transport_destroy(client->tcp);
        }
        if (client->ws) {
            transport_destroy(client->ws);
        }
        aos_free(client);
    }
    return 0;
}