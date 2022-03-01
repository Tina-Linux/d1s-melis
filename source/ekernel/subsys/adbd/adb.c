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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "adb.h"

int g_adbd_debug_mask = 0;

void send_ready(unsigned int local, unsigned int remote, atransport *t)
{
	adbd_debug("");

	apacket *p = get_apacket();
	p->msg.command = A_OKAY;
	p->msg.arg0 = local;
	p->msg.arg1 = remote;
	send_apacket(p, t);
}

void send_close(unsigned int local, unsigned int remote, atransport *t)
{
	adbd_debug("");

	apacket *p = get_apacket();
	p->msg.command = A_CLSE;
	p->msg.arg0 = local;
	p->msg.arg1 = remote;
	send_apacket(p, t);
}

static size_t fill_connect_data(char *buf, size_t bufsize)
{
	static const char *cnxn_props[] = {
                "ro.product.name",
                "ro.product.model",
                "ro.product.device",
        };
	const char *adb_device_banner = "device";
        size_t remaining = bufsize, len;

	adbd_debug("remaining:%u", remaining);
        len = snprintf(buf, remaining, "%s::", adb_device_banner);
        remaining -= len;
        buf += len;

	adbd_debug("remaining:%u", remaining);
        len = snprintf(buf, remaining, "%s=%s;", cnxn_props[0], "Tina");
        remaining -= len;
        buf += len;

	adbd_debug("remaining:%u", remaining);
        len = snprintf(buf, remaining, "%s=%s;", cnxn_props[1], "sun8iw18");
        remaining -= len;
        buf += len;

	adbd_debug("remaining:%u", remaining);
        len = snprintf(buf, remaining, "%s=%s;", cnxn_props[1], "R328");
        remaining -= len;
        buf += len;

        return bufsize - remaining + 1;
}

static void send_connect(atransport *t)
{
	apacket *p = NULL;

	adbd_debug("");
	p = get_apacket();
	p->msg.command = A_CNXN;
        p->msg.arg0 = A_VERSION;
        p->msg.arg1 = MAX_PAYLOAD;
        p->msg.data_length = fill_connect_data((char *)p->data, sizeof(p->data));

	adbd_debug("data length:%u\n", p->msg.data_length);
        send_apacket(p, t);
}

static void handle_online(atransport *t)
{
	t->online = 1;
}

#if 0
void print_apacket(const char *label, apacket *p)
{
	#if 0
	printf("%s %c%c\n", label,
                ((char*)(&p->msg.command))[0],
                ((char*)(&p->msg.command))[1]);
	#endif
	return;
}
#else
void print_apacket(const char *label, apacket *p)
{
	char *x, *tag;
	unsigned int count;
	amessage *msg = &p->msg;

	switch (msg->command) {
	case A_SYNC: tag = "SYNC"; break;
	case A_CNXN: tag = "CNXN"; break;
	case A_OPEN: tag = "OPEN"; break;
	case A_OKAY: tag = "OKAY"; break;
	case A_CLSE: tag = "CLSE"; break;
	case A_WRTE: tag = "WRTE"; break;
	case A_AUTH: tag = "AUTH"; break;
	default: tag = "????";
		printf("command:0x%08x\n", msg->command);
		break;
	}
	adbd_info_raw("%s: %s %08x %08x %04x ,check:0x%08x, magic:0x%08x \"",
		label, tag, msg->arg0, msg->arg1, msg->data_length,
		msg->data_check, msg->magic);
	/*return;*/
	count = msg->data_length;
	x = (char *)(p->data);
	if (count > DUMPMAX) {
		count = DUMPMAX;
		tag = "\n";
	} else {
		tag = "\"\n";
	}
	while (count-- > 0) {
		if ((*x >= ' ') && (*x < 127))
			adbd_info_raw("%c", *x);
		else
			adbd_info_raw(".");
		x++;
	}
	adbd_info_raw("%s", tag);

#if 0
	count = msg->data_length;
	if (count > DUMPMAX) {
		count = DUMPMAX;
		tag = "\n";
	} else {
		tag = "\"\n";
	}
	x = (char *)(p->data) + (msg->data_length - count);
	while (count-- > 0) {
	if ((*x >= ' ') && (*x < 127))
		printf("%c", *x);
	else
		printf(".");
		x++;
	}
	printf("%s", tag);
#endif
#if 0
	count = msg->data_length;
	while (count-- > 0) {
		printf("0x%02x ", *x);
	if (count%16 == 0)
		printf("\n");
		x++;
	}
	printf("%s", tag);
#endif
	return;
}
#endif

void handle_packet(apacket *p, atransport *t)
{
        adbd_debug(" %c%c%c%c\n",
                ((char*)(&p->msg.command))[0],
                ((char*)(&p->msg.command))[1],
                ((char*)(&p->msg.command))[2],
                ((char*)(&p->msg.command))[3]);
        print_apacket("recv", p);

	switch (p->msg.command) {
        case A_SYNC:
                break;
        case A_CNXN:
                /*parse_banner((char *)p->data);*/
		handle_online(t);
		send_connect(t);
                break;
        case A_AUTH:
                break;
        case A_OPEN:
		if (t->online) {
			char *name = (char *)p->data;
			aservice *ser;
#ifdef ADBD_RECORD_ALIVE_THREAD
			record_alive_thread_print();
#endif
			name[p->msg.data_length > 0 ? p->msg.data_length - 1 : 0] = 0;
			ser = adb_service_create(name, p->msg.arg0, t);
			if (!ser || ser->localid <=0) {
				send_close(0, p->msg.arg0, t);
			} else {
				send_ready(ser->localid, p->msg.arg0, t);
				ser->ready(ser);
			}
		}
        break;
        case A_OKAY:
		if (t->online) {
			aservice *ser;
			if ((ser = find_adb_service(p->msg.arg1))) {
				ser->ready(ser);
			}
		}
                break;
        case A_CLSE:
		if (t->online) {
#if 0
				aservice *ser;
				if ((ser = find_adb_service(p->msg.arg1))) {
					ser->close(ser);
				}
#else
				adb_service_destroy(p->msg.arg1);
#endif
		}
                break;
	case A_WRTE:
		if (t->online) {
			int remoteid = p->msg.arg0;
			int localid = p->msg.arg1;
			aservice *ser;

			if ((ser = find_adb_service(localid))) {
#if 0
				if (ser->enqueue(ser, p) == 0) {
					adbd_debug("Enqueue service packet");
					send_ready(localid, remoteid, t);
				}
#else
				adbd_debug("Enqueue service packet, p=%p", p);
				send_ready(localid, remoteid, t);
				ser->enqueue(ser, p);
#endif
				return;
			}
		}
                break;
        default:
                adbd_info("unknown command...0x%08x\n", p->msg.command);
                break;
	}
	#ifdef MEM_DEBUG
	if (p->msg.command == A_CLSE) {
		put_apacket(p);
		memleak_print();
		return ;
	}
	#endif
	put_apacket(p);
}

void put_apacket(apacket *p)
{
	adb_free(p);
}

apacket *get_apacket(void)
{
	apacket *p = adb_malloc(sizeof(apacket));
	if (!p)
		fatal("no memory");
	/*memset(p, 0, sizeof(apacket) - MAX_PAYLOAD);*/
	memset(p, 0, sizeof(amessage));
	return p;
}

static inline void adbd_version(void)
{
	printf("adbd version:%s, compiled on: %s %s\n", ADBD_VERSION, __DATE__, __TIME__);
}

#ifdef ADBD_SWITCH_WITH_KV
static int adbd_switch_kv_check(void)
{
	char *adbd_key = "adbd_sw";
	int sw = 1, len;

	len = sizeof(sw);
	ret = aos_kv_get(adbd_key, (void *)&sw, &len);
	if (ret != 0) {
		sw = 1;
		aos_kv_set(adbd_key, (void *)&sw, sizeof(sw), 1);
	}
	if (!sw)
		return -1;
	return 0;
}

#else

static int adbd_switch_file_check(void)
{
	const char *adb_switch_file = "/data";// /data/factory/newborn
	if (!access(adb_switch_file, F_OK))
		return 0;
	return -1;
}
#endif

static int adbd_switch_check(void)
{
#ifdef ADBD_SWITCH_WITH_KV
	return adbd_switch_kv_check();
#else
	return adbd_switch_file_check();
#endif
}

int adbd_main(void)
{
	int ret = 0;
	int sw = 1, len;

	adbd_version();
//	if (adbd_switch_check() != 0) {
//		printf("adbd is forbidden...\n");
//		return -1;
//	}

	/* TODO: auth,transport_port */
	ret = usb_init();
	if (ret != 0) {
		printf("adbd service init failed,it may already init, please use directly.\n");
		return -1;
	}

	printf("adbd service init successful\n");
	return ret;
}
int adbd_exit(void)
{
	return usb_exit();
}
static void usage(void)
{
	printf("Usgae: adbd [option]\n");
	printf("-v,          adbd version\n");
	printf("-d,          adbd debug option\n");
	printf("-h,          adbd help\n");
	printf("\n");
}

int adbd_init(int argc, char **argv)
{
	int ret = 0, c;

	optind = 0;
	while ((c = getopt(argc, argv, "vd:h")) != -1) {
		switch (c) {
		case 'v':
			adbd_version();
			return 0;
		case 'd':
			g_adbd_debug_mask = atoi(optarg);
			return 0;
		case 'h':
		default:
			usage();
			return 0;
		}
	}

	ret = adbd_main();

	return ret;
}
#if 0
int cmd_test(int argc, char *argv[])
{
	int ret;
	char *adbd_key = "adbd_sw";
	int sw, len;

	aos_kv_init();
	if (argc == 2) {
		sw = atoi(argv[1]);
		printf("set sw=%d\n", sw);
		ret = aos_kv_set(adbd_key, (void *)&sw, sizeof(sw), 1);
		printf("set return %d\n", ret);
		return 0;
	}
	len = sizeof(sw);
	ret = aos_kv_get(adbd_key, (void *)&sw, &len);
	if (ret != 0) {
		printf("get failed\n");
		return -1;
	}
	printf("adbd_sw = %d\n", sw);
}
FINSH_FUNCTION_EXPORT_CMD(cmd_test, test, test);
#endif
#if 0
static int cmd_test(int argc, char *argv[])
{
	printf("sleep forever\n");
	while (1) {
		aos_msleep(500);
		/*printf("active...\n");*/
	}
	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_test, test, test);
#endif
