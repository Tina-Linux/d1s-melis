

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_WS_BUF_SIZE 2048

typedef struct websocket *websocket_handle_t;


typedef enum {
    WS_SEND_TYPE_BIN = 0,
    WS_SEND_TYPE_TEXT
} ws_send_type_t;

/**
 * @brief      websocket transport
 */
typedef enum {
    WS_TRANSPORT_UNKNOWN = 0x0,   /*!< Unknown */
    WS_TRANSPORT_OVER_TCP,        /*!< Transport over tcp */
    WS_TRANSPORT_OVER_SSL,        /*!< Transport over ssl */
} websocket_transport_t;

/**
 * @brief websocket configuration
 */
typedef struct {
    const char                  *url;                /*!< ws URL, the information on the URL is most important, it overrides the other fields below, if any */
    const char                  *host;               /*!< Domain or IP as string */
    int                         port;                /*!< Port to connect, default is (80 or 443) */
    const char                  *path;               /*!< ws Path, if not set, default is `/` */
    const char                  *protocol;           /*!< ws Protocol */
    const char                  *cert_pem;           /*!< SSL server certification, PEM format as string, if the client requires to verify server */
    const char                  *client_cert_pem;    /*!< SSL client certification, PEM format as string, if the server requires to verify client */
    const char                  *client_key_pem;     /*!< SSL client key, PEM format as string, if the server requires to verify client */
    int                         timeout_ms;               /*!< Network timeout in milliseconds */
    int                         buffer_size;              /*!< ws buffer size (both send and receive) */
    websocket_transport_t       transport_type;           /*!< websocket transport type, see `websocket_transport_t` */
    bool                        use_global_ca_store;      /*!< Use a global ca_store for all the connections in which this bool is set. */
} websocket_config_t;


/**
 * @brief      start a websocket session
 *
 * @param[in]  config   The configurations, see `websocket_config_t`
 *
 * @return
 *     - `websocket_handle_t`
 *     - NULL if any errors
 */
websocket_handle_t websocket_create(websocket_config_t *config);

/**
 * @brief     write data to websocket connection
 *
 * @param[in]  client  The websocket handle
 * @param[in]  buffer  The buffer
 * @param[in]  len     The buffer length
 * @param      timeout_ms     network timeout with in millisecond
 * @param      send_type      see `ws_send_type_t`
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data written
 */
int websocket_write(websocket_handle_t client, const unsigned char *buffer, size_t len, uint32_t timeout_ms, ws_send_type_t send_type);

/**
 * @brief     read data from websocket connection
 *
 * @param[in]  client  The websocket handle
 * @param[in]  buffer  The buffer
 * @param[in]  len     The buffer length
 * @param      timeout_ms     network timeout with in millisecond
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data read
 */
int websocket_read(websocket_handle_t client, unsigned char *buffer, size_t len, uint32_t timeout_ms);

/**
 * @brief      This function must be the last function to call for an session.
 *
 * @param[in]  client  The websocket handle
 *
 * @return
 *     - (-1) if any errors
 *     - (0) if ok
 */
int websocket_destroy(websocket_handle_t client);



#ifdef __cplusplus
}
#endif

#endif // ifndef WEBSOCKET_H