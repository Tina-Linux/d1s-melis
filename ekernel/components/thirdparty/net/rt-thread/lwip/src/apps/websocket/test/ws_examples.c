#include <aos/aos.h>
#include <stddef.h>
#include <string.h>
#include <websocket.h>

#define TAG "wsclient_example"

void websocket_test()
{
    int ret;
    unsigned char *buf;
    buf = aos_zalloc(1024);
    if (!buf) {
        return;
    }

    websocket_config_t config = {
        // .url = "ws://echo.websocket.org",
        // .url = "wss://echo.websocket.org",
        .host = "echo.websocket.org",
        .port = 443,
        .transport_type = WS_TRANSPORT_OVER_SSL,
        .timeout_ms = 5000
    };
    websocket_handle_t ws = websocket_create(&config);

    if (!ws) {
        LOGE(TAG, "ws create failed.");
        goto _exit;
    }

    snprintf((char *)buf, 1024, "%s", "hello websocket!");
    if ((ret = websocket_write(ws, buf, strlen((char *)buf), 5000, WS_SEND_TYPE_TEXT)) < 0) {
        LOGE(TAG, "ws write error, %d", ret);
        goto _exit;
    }

    if ((ret = websocket_read(ws, buf, 1024, 5000)) < 0) {
        LOGE(TAG, "ws read error, %d", ret);
        goto _exit;
    }
    buf[ret > 1023 ? 1023 : ret] = 0;
    LOGD(TAG, "ret: %d", ret);
    LOGD(TAG, "====>\n%s\n", buf);

_exit:
    aos_free(buf);
    websocket_destroy(ws);
}