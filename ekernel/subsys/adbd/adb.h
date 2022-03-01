/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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
#ifndef __ADB_H
#define __ADB_H

#include "adb_queue.h"
#include "adb_ev.h"
#include "adb_rb.h"
#include "misc.h"
#include <assert.h>
#include <cli_console.h>

#define ADBD_VERSION    "AW-V1.1.0"

#if 0

#define adbd_debug(fmt, args...)    \
    printf("[ADBD-DEBUG][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)
#else
#define adbd_debug(fmt, args...)
#endif

#ifndef unlikely
#define unlikely(x)             __builtin_expect ((x), 0)
#endif

extern int g_adbd_debug_mask;
#define adbd_info(fmt, args...) \
    do { \
        if (unlikely(g_adbd_debug_mask)) \
            printf("[ADBD-INFO][%s] line:%d " fmt "\n", \
                   __func__, __LINE__, ##args); \
    } while (0)
#define adbd_info_raw(fmt, args...) \
    do { \
        if (unlikely(g_adbd_debug_mask)) \
            printf( fmt , ##args); \
    } while (0)

#define adbd_err(fmt, args...)  \
    printf("[ADBD-ERROR][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)

#define fatal(msg) \
    do {\
        printf("[ADBD-FATAL][%s] line:%d %s \n", __func__, __LINE__,msg);\
        assert(0);\
    } while (0)

#define ADBD_RECORD_ALIVE_THREAD

#define USB_ADB_EP0 (0)
#define USB_ADB_IN  (1)
#define USB_ADB_OUT (2)

#define MAX_PAYLOAD (4096)

#define A_SYNC 0x434e5953
#define A_CNXN 0x4e584e43
#define A_OPEN 0x4e45504f
#define A_OKAY 0x59414b4f
#define A_CLSE 0x45534c43
#define A_WRTE 0x45545257
#define A_AUTH 0x48545541


#define A_VERSION 0x01000000        // ADB protocol version

#define ADB_VERSION_MAJOR 1         // Used for help/version information
#define ADB_VERSION_MINOR 0         // Used for help/version information

#define ADB_SERVER_VERSION    31    // Increment this when we want to force users to start a new adb server

#define DUMPMAX 32
typedef struct usb_handle usb_handle;
typedef struct aservice aservice;
typedef struct atransport atransport;
typedef struct amessage amessage;
typedef struct apacket apacket;
typedef struct sync_xfer sync_xfer;
typedef struct socket_xfer socket_xfer;

struct amessage
{
    unsigned int command;
    unsigned int arg0;
    unsigned int arg1;
    unsigned int data_length;
    unsigned int data_check;
    unsigned int magic;
};

struct apacket
{
    amessage msg;
    unsigned char data[MAX_PAYLOAD];
};

typedef enum transport_type
{
    kTransportUsb,
    kTransportLocal,
    kTransportAny,
    kTransportHost,
} transport_type;

struct atransport
{
    int (*read_from_remote)(apacket *p, atransport *t);
    int (*write_to_remote)(apacket *p, atransport *t);
    void (*close)(atransport *t);
    void (*kick)(atransport *t);

    transport_type type;

    usb_handle *usb;

    unsigned sync_token;
    int connection_state;
    int online;

    adb_queue to_remote;
};

#define CS_ANY       -1
#define CS_OFFLINE    0
#define CS_BOOTLOADER 1
#define CS_DEVICE     2
#define CS_HOST       3
#define CS_RECOVERY   4
#define CS_NOPERM     5 /* Insufficient permissions to communicate with the device */
#define CS_SIDELOAD   6


enum
{
    ADB_SERVICE_TYPE_UNKNOWN = 0,
    ADB_SERVICE_TYPE_SHELL,
    ADB_SERVICE_TYPE_SYNC,
    ADB_SERVICE_TYPE_SOCKET,
};

struct adb_rb
{
    unsigned char *buffer;
    unsigned int length;
    volatile unsigned int start, end, isfull;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    adb_ev *ev;
	shell_xfer_t *xfer;
    /* lock */
};

struct shell_xfer
{
    adb_schd_t schd;
    adb_ev *ev_handle;
    adb_rb *rb_read_from_pc;
    adb_rb *rb_write_by_shell;
    adb_rb *rb_write_by_adb;
    cli_console *console;
    int recv_close;
};

struct sync_xfer
{
    adb_queue_ev *queue_recv;
    adb_queue queue_send;
    adb_schd_t schd;
    apacket *p;
    unsigned int offset;
};

struct socket_xfer
{
    adb_schd_t schd;
    int s;
    int recv_close;
};

struct aser_cookie
{
    adb_thread_t tid;
    char command_name[32];
};

struct aservice
{
    atransport *transport;
    void (*func)(void *handle, void *cookie);
    int service_type;
    union
    {
        shell_xfer_t shell_handle;
        sync_xfer sync_handle;
        socket_xfer socket_handle;
    } xfer_handle;
    struct aser_cookie cookie;

    unsigned int localid;
    unsigned int remoteid;

    int (*enqueue)(struct aservice *ser, apacket *pkt);
    int (*ready)(struct aservice *ser);

    int has_send_close;
    adb_thread_t service_tid;
    struct list_head list;
};


void print_apacket(const char *label, apacket *p);
void handle_packet(apacket *p, atransport *t);
apacket *get_apacket(void);
void put_apacket(apacket *p);
void send_apacket(apacket *p, atransport *t);
void send_write(const char *buf, int size, unsigned int local, unsigned int remote, atransport *t);
void send_write_combine(const char *buf1, int size1,
                        const char *buf2, int size2,
                        unsigned int local, unsigned int remote, atransport *t);
void send_close(unsigned int local, unsigned int remote, atransport *t);
void send_ready(unsigned int local, unsigned int remote, atransport *t);

int check_header(apacket *p);
int check_data(apacket *p);


int init_usb_transport(atransport *t, usb_handle *h, int state);
int deinit_usb_transport(atransport *t, usb_handle *h, int state);
aservice *adb_service_create(const char *name, unsigned int remoteid, atransport *t);
void adb_service_destroy(unsigned int localid);
void adb_service_destroy_all(void);
aservice *find_adb_service(unsigned int localid);

int usb_write(usb_handle *h, const void *data, int len);
int usb_read(usb_handle *h, void *data, int len);

int usb_init(void);
int usb_exit(void);
void register_usb_transport(usb_handle *usb, const char *serial, const char *devpath, unsigned writeable);
void unregister_usb_transport(usb_handle *usb, const char *serial, const char *devpath, unsigned writeable);

#ifdef CONFIG_COMPONENTS_ADBD_SOCKETS
#define ADB_SUPPORT_SOCKET_API
#endif

#ifdef ADB_SUPPORT_SOCKET_API
int socket_loopback_client(int port);
void socket_recv_service(void *xfer_handle, void *cookie);
int socket_enqueue(aservice *ser, apacket *p);
int socket_ready(aservice *ser);
#endif

//#define MEM_DEBUG
#ifdef MEM_DEBUG
void *malloc_wrapper(size_t size);
void *calloc_wrapper(size_t nmemb, size_t size);
void *realloc_wrapper(void *ptr, size_t size);
char *strdup_wrapper(const char *s);
void free_wrapper(void *ptr);
#define adb_malloc(size)                malloc_wrapper(size)
#define adb_calloc(num, size)               calloc_wrapper(num, size)
#define adb_realloc(ptr, size)              realloc_wrapper(ptr, size)
#define adb_strdup(s)                   strdup_wrapper(s)
//#define adb_scandir(dirp,namelist,filter,compar)  scandir_wrapper(dirp,namelist,filter,compar)
#define adb_free(ptr)                   free_wrapper(ptr)
void memleak_print(void);
#else
#define adb_malloc(size)                malloc(size)
#define adb_calloc(num, size)               calloc(num, size)
#define adb_realloc(ptr, size)              realloc(ptr, size)
#define adb_strdup(s)                   strdup(s)
//#define adb_scandir(dirp,namelist,filter,compar)  scandir(dirp,namelist,filter,compar)
#define adb_free(ptr)                   free(ptr)
#endif

#define ADB_THREAD_HIGH_PRIORITY    (20)
#define ADB_THREAD_NORMAL_PRIORITY  (21)


#endif /* __ADB_H */
