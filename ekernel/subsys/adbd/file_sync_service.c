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
#include "adb_queue.h"
#include "adb_shell.h"
#include "file_sync_service.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <aw_dirent.h>

//#define SUPPORT_SPIFFS

static void sync_wait_ready(aservice *ser)
{
	sync_xfer *xfer = &ser->xfer_handle.sync_handle;

	adbd_debug("");
	adb_schd_wait(xfer->schd);
	adbd_debug("");
}

/* used for detect CLSE event */
static int sync_wait_ready_and_error(aservice *ser)
{
	sync_xfer *xfer = &ser->xfer_handle.sync_handle;
	int event;
	event = adb_schd_wait(xfer->schd);
	if (event & ADB_SCHD_ERROR) {
		adbd_info("detect schd do_recv exit");
		return -1;
	}
	return 0;
}

static int fail_message(const char *reason, aservice *ser)
{
	syncmsg msg;
	int len = strlen(reason);

	adbd_debug("sync: failure: %s", reason);

	msg.data.id = ID_FAIL;
	msg.data.size = len;

#if 0
	sync_wait_ready(ser);
	send_write((const char *)&msg.data, sizeof(msg.data), ser->localid, ser->remoteid, ser->transport);
	sync_wait_ready(ser);
	send_write((const char *)reason, len, ser->localid, ser->remoteid, ser->transport);
#else
	if (sync_wait_ready_and_error(ser) < 0)
		return -1;
	send_write_combine((const char *)&msg.data, sizeof(msg.data),
				(const char *)reason, len,
				ser->localid, ser->remoteid, ser->transport);
#endif
	return 0;
}

static int fail_errno(aservice *ser)
{
	/*adbd_err("errno=%d, str=%s\n", errno, strerror(errno));*/
	return fail_message(strerror(errno), ser);
}

static int sync_read_packet(char *buf, unsigned int len, aservice *ser)
{
	int size = 0;
	sync_xfer *xfer = &ser->xfer_handle.sync_handle;
	int event = 0;

	adbd_debug("len:%d, offset:%d, p=%p\n", len, xfer->offset, xfer->p);
	if (!xfer->p) {
		adb_dequeue_event(xfer->queue_recv, &event, (void **)&xfer->p, 0);
		if (event & ADB_EV_EXIT) {
			adbd_info("receive exit event. p=%p", xfer->p);
			goto err;
		}
		if (!(event & ADB_EV_READ)) {
			adbd_info("event=0x%x, no read bit?, p=%p", event, xfer->p);
		}
		if (!xfer->p) {
			adbd_err("dequeue packet is null\n");
			return -1;
		}
	}
	if (len < xfer->p->msg.data_length - xfer->offset)
		size = len;
	else
		size = xfer->p->msg.data_length - xfer->offset;
	adbd_debug("data len:%d, copy size:%d, p=%p\n", xfer->p->msg.data_length, size, xfer->p);
	memcpy(buf, xfer->p->data + xfer->offset, size);
	xfer->offset += size;
	if (xfer->offset >= xfer->p->msg.data_length) {
		adbd_debug("p=%p, offset=%u, data len=%u, free apacket",
			xfer->p, xfer->offset, xfer->p->msg.data_length);
		xfer->offset = 0;
		put_apacket(xfer->p);
		xfer->p = NULL;
	}
	if (size < len) {
		adbd_debug("expect len:%u, actual size:%u", len, size);
		sync_read_packet(buf + size, len - size, ser);
	}
	return len;
err:
	adbd_debug("exit event...");
	if (xfer->p) {
		xfer->offset = 0;
		put_apacket(xfer->p);
		xfer->p = NULL;
	}
	return -1;
}

static int mkdirs(const char *path)
{
	fatal("no support now");
	return 0;
}

static int handle_send_file(char *path, mode_t mode, aservice *ser)
{
	int fd;
	int total_size;
	int drain_packet = 0;
	char *buffer = adb_malloc(SYNC_DATA_MAX);

	if (!buffer)
		fatal("no memory");

	adbd_debug("");
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
	if (fd < 0 && errno == ENOENT) {
		mkdirs(path);
		fd = open(path, O_WRONLY | O_CREAT | O_EXCL, mode);
	}
	if (fd < 0 && errno == EEXIST) {
		fd = open(path, O_WRONLY, mode);
	}
	if (fd < 0) {
		fail_errno(ser);
		fd = -1;
	}

	for (;;) {
		syncmsg msg;
		int size;

		adbd_debug("");

		size = sync_read_packet((char *)&msg.data, sizeof(msg.data), ser);
		if (size < 0) {
			adbd_debug("sync_read_packet return %d\n", size);
			goto err;
		}
		total_size = msg.data.size;
		adbd_info("msg data return size=%d, data size:0x%x\n", size, total_size);
		if (size > SYNC_DATA_MAX) {
			fail_message("oversize data message", ser);
			drain_packet = 1;
			continue;
		}
		if (msg.data.id != ID_DATA) {
			adbd_debug("");
			if (msg.data.id == ID_DONE) {
				adbd_debug("");
				break;
			}
			fail_message("invalid data message", ser);
			drain_packet = 1;
			continue;
		}
		size = sync_read_packet(buffer, total_size, ser);
		if (size < 0) {
			adbd_debug("sync_read_packet return %d\n", size);
			goto err;
		}
		if (drain_packet)
			continue;
		adbd_info("write %d bytes data, please wait...", total_size);
	#if 1
		if (fd < 0)
			continue;
		size = write(fd, buffer, total_size);
		adbd_info("write return %d\n", size);
		if (size < 0) {
			int saved_errno = errno;
			/* errno may not actual err number, so assign here */
			errno = -size;
			adbd_err("write failed, return %d, errno=%d", size, errno);
			fail_errno(ser);
			errno = saved_errno;
			close(fd);
			fd = -1;
			unlink(path);
			drain_packet = 1;
			continue;
		}
	#endif
	}
	if (fd > 0) {
		syncmsg msg;
		/* TODO timestamp */
		close(fd);
		adbd_info("write finish..send ok");
		msg.status.id = ID_OKAY;
		msg.status.msglen = 0;
		send_write((const char *)&msg.status, sizeof(msg.status), ser->localid, ser->remoteid, ser->transport);
		sync_wait_ready(ser);
	}

	if (buffer)
		adb_free(buffer);
	adbd_debug("");
	return 0;
err:
	if (fd > 0)
		close(fd);

	if (buffer)
		adb_free(buffer);
	unlink(path);
	return -1;
}

static int do_send(char *path, aservice *ser)
{
	int ret;
	char *tmp;
	mode_t mode = 644;

	tmp = strrchr(path, ',');
	if (tmp) {
		*tmp = 0;
		errno = 0;
		mode = strtoul(tmp + 1, NULL, 0);
		mode &= 0777;
	}

	unlink(path);

	mode |= ((mode >> 3) & 0070);
	mode |= ((mode >> 3) & 0007);
	adbd_debug("send %s", path);
	ret = handle_send_file(path, mode, ser);
	adbd_debug("");
	return ret;
}



static int do_recv(char *path, aservice *ser)
{
	syncmsg msg;
	int fd, r;
	char *buffer = adb_malloc(SYNC_DATA_MAX);

	if (!buffer)
		fatal("no memory");

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		fail_errno(ser);
		goto err;
	}

	msg.data.id = ID_DATA;
	for (;;) {
		int i;
		char *data = buffer;

		adbd_debug("read data...");
		r = read(fd, buffer, SYNC_DATA_MAX);
		if (r <= 0) {
			adbd_info("read return %d", r);
			if (r == 0) break;
			if (errno == EINTR) continue;
			r = fail_errno(ser);
			goto err;
		}
		msg.data.size = r;
		if (sync_wait_ready_and_error(ser) < 0)
			goto err;
		send_write((const char *)&msg.data, sizeof(msg.data),
			ser->localid, ser->remoteid, ser->transport);
		for (i = 0; i < r/MAX_PAYLOAD; i++) {
			if (sync_wait_ready_and_error(ser) < 0)
				goto err;
			send_write((const char *)data, MAX_PAYLOAD, ser->localid,
				ser->remoteid, ser->transport);
			data += MAX_PAYLOAD;
		}
		r = r%MAX_PAYLOAD;
		if (r != 0) {
			if (sync_wait_ready_and_error(ser) < 0)
				goto err;
			send_write((const char *)data, r, ser->localid,
				ser->remoteid, ser->transport);
		}
		adbd_debug("do recv loop");
	}
	close(fd);
	fd = -1;

	msg.data.id = ID_DONE;
	msg.data.size = 0;
	if (sync_wait_ready_and_error(ser) < 0)
		goto err;
	send_write((const char *)&msg.data, sizeof(msg.data), ser->localid, ser->remoteid, ser->transport);
	if (buffer)
		adb_free(buffer);
	return 0;
err:
	if (fd > 0)
		close(fd);
	if (buffer)
		adb_free(buffer);
	return -1;
}

static int dir_is_exist(const char *path)
{
	DIR *d;
	struct dirent *de;
	const char *data_dir = "/data/";
	int data_dir_len = strlen(data_dir);
	char *ignore_data = NULL;
	char buf[256];
	int len;

	len = strlen(path);
	if (len >= sizeof(buf) - 1)
		return -1;
	strcpy(buf, path);
	if (buf[len - 1] != '/') {
		buf[len++] = '/';
		buf[len] = '\0';
	}
	adbd_info("buf=%s\n", buf);
	if (strncmp(buf, data_dir, data_dir_len))
		return -1;
	if (len == data_dir_len)
		return 0;

	ignore_data = buf + data_dir_len;
	adbd_info("ignore_data=%s\n", ignore_data);
	d = opendir(data_dir);
	if (!d)
		return -1;
	while ((de = readdir(d))) {
		if (!strncmp(de->d_name, ignore_data, strlen(ignore_data))) {
			closedir(d);
			return 0;
		}
	}
	closedir(d);
	return -1;
}

static int do_stat(const char *path, aservice *ser)
{
	syncmsg msg;
	struct stat st;

	msg.stat.id = ID_STAT;
	adbd_debug("stat %s", path);
	if (stat(path, &st)) {
#ifdef SUPPORT_SPIFFS
		if (!dir_is_exist(path)) {
			adbd_info("");
			msg.stat.mode = S_IFDIR;
			msg.stat.size = 0x1000;
			msg.stat.time = 0;
		} else {
			adbd_info("");
			msg.stat.mode = 0;
			msg.stat.size = 0;
			msg.stat.time = 0;
		}
#else
		adbd_info("");
		msg.stat.mode = 0;
		msg.stat.size = 0;
		msg.stat.time = 0;
#endif
	} else {
		if (S_ISBLK(st.st_mode))
			st.st_mode = S_IFREG;
		msg.stat.mode = st.st_mode;
		msg.stat.size = st.st_size;
		msg.stat.time = st.st_mtime;
	}
	if (sync_wait_ready_and_error(ser) < 0)
		return -1;
	send_write((const char *)&msg.stat, sizeof(msg.stat), ser->localid, ser->remoteid, ser->transport);
	return 0;
}

#ifdef SUPPORT_SPIFFS
static int do_list(const char *path, aservice *ser)
{
	DIR *d;
	struct dirent *de;
	struct stat st;
	int len;
	char tmp[256];
	char lastdir[128];
	char *fname;
	int is_data_dir = 0;
	const char *data_dir = "/data/";
	syncmsg msg;

	adbd_info("path=%s\n", path);
	len = strlen(path);
	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, path, len);
	if (tmp[len - 1] != '/')
		tmp[len++] = '/';
	fname = tmp + len;

	msg.dent.id = ID_DENT;

	memset(lastdir, 0, sizeof(lastdir));

	if (!strcmp(path, data_dir)) {
		is_data_dir = 1;
		adbd_info("is data directory\n");
	}
	d = opendir(data_dir);
	if (!d)
		goto done;
	while ((de = readdir(d))) {
		int len;
		char *ptr = NULL;
		char cur_name[256];

		len = strlen(de->d_name);
		if (len > 128)
			continue;
		if (!is_data_dir) {
			ptr = tmp + strlen(data_dir);
			len = fname - ptr;
			if (strncmp(de->d_name, ptr, len) != 0)
				continue;
			snprintf(cur_name, sizeof(cur_name), "%s", de->d_name + len);
			len = strlen(cur_name);
		} else {
			snprintf(cur_name, sizeof(cur_name), "%s", de->d_name);
			strcpy(cur_name, de->d_name);
		}
		if ((ptr = strchr(cur_name, '/')) != NULL) {
			/*
			 * /data/123/ and /data/1234/
			 * dir 123 will list first
			 * */
			len = ptr - cur_name;
			if (!strncmp(cur_name, lastdir, len))
				continue;
			strncpy(lastdir, cur_name, len);
			msg.dent.mode = S_IFDIR;
			msg.dent.size = 0x1000;
			msg.dent.time = 0;
			msg.dent.namelen = len;
			adbd_info("send dir %s, namelen=%d\n", lastdir, len);
#if 0
			sync_wait_ready(ser);
			send_write((const char *)&msg.dent, sizeof(msg.dent),
					ser->localid, ser->remoteid, ser->transport);
			sync_wait_ready(ser);
			send_write(lastdir, len, ser->localid, ser->remoteid, ser->transport);
#else
			sync_wait_ready(ser);
			send_write_combine((const char *)&msg.dent, sizeof(msg.dent),
						lastdir, len,
						ser->localid, ser->remoteid, ser->transport);
#endif
		} else {
			strcpy(fname, cur_name);
			if (stat(tmp, &st) != 0)
				continue;
			msg.dent.mode = st.st_mode;
			msg.dent.size = st.st_size;
			msg.dent.time = st.st_mtime;
			msg.dent.namelen = len;
			adbd_info("send file %s, mode=0x%x, namelen=%d\n", cur_name, st.st_mode, len);
#if 0
			sync_wait_ready(ser);
			send_write((const char *)&msg.dent, sizeof(msg.dent),
					ser->localid, ser->remoteid, ser->transport);
			sync_wait_ready(ser);
			send_write(cur_name, len, ser->localid, ser->remoteid, ser->transport);
#else
			sync_wait_ready(ser);
			send_write_combine((const char *)&msg.dent, sizeof(msg.dent),
						cur_name, len,
						ser->localid, ser->remoteid, ser->transport);
#endif
		}
	}

	closedir(d);
done:
	msg.dent.id = ID_DONE;
	msg.dent.mode = 0;
	msg.dent.size = 0;
	msg.dent.time = 0;
	msg.dent.namelen = 0;
	sync_wait_ready(ser);
	send_write((const char *)&msg.dent, sizeof(msg.dent), ser->localid, ser->remoteid, ser->transport);
	return 0;
}

#else

static int do_list(const char *path, aservice *ser)
{
	DIR *d;
	struct dirent *de;
	struct stat st;
	int len;
	char tmp[256 + 128];
	char *fname;
	syncmsg msg;

	len = strlen(path);
	if (len > 256)
		goto done;
	memcpy(tmp, path, len);
	tmp[len] = '/';
	fname = tmp + len + 1;

	msg.dent.id = ID_DENT;

	adbd_debug("path=%s", path);
	d = opendir(path);
	if (!d)
		goto done;
	while ((de = readdir(d))) {
		int len = strlen(de->d_name);
		adbd_debug("name:%s", de->d_name);

		if (len > 128)
			continue;
		strcpy(fname, de->d_name);
		if (!stat(tmp, &st)) {
			msg.dent.mode = st.st_mode;
			msg.dent.size = st.st_size;
			msg.dent.time = st.st_mtime;
			msg.dent.namelen = len;
			if (sync_wait_ready_and_error(ser) < 0) {
				closedir(d);
				return -1;
			}
			send_write_combine((const char *)&msg.dent, sizeof(msg.dent),
						de->d_name, len,
						ser->localid, ser->remoteid, ser->transport);
		}
	}

	closedir(d);
done:
	msg.dent.id = ID_DONE;
	msg.dent.mode = 0;
	msg.dent.size = 0;
	msg.dent.time = 0;
	msg.dent.namelen = 0;
	if (sync_wait_ready_and_error(ser) < 0)
		return -1;
	send_write((const char *)&msg.dent, sizeof(msg.dent), ser->localid, ser->remoteid, ser->transport);
	return 0;
}
#endif

void file_sync_service(void *xfer_handle, void *cookie)
{
	sync_xfer *xfer = (sync_xfer *)xfer_handle;
	aservice *ser = container_of((void *)xfer, struct aservice, xfer_handle);
	syncmsg msg;
	char name[256];
	unsigned int namelen;
	int ret;
	for (;;) {
		adbd_info("sync: waiting for command");

		ret = sync_read_packet((char *)&msg.req, sizeof(msg.req), ser);
		if (ret < 0)
			goto err;
		namelen = msg.req.namelen;
		adbd_info("namelen:%d", namelen);
		memset(name, 0, sizeof(name));
		if (namelen >= sizeof(name)) {
			adbd_err("invalid namelen");
			goto err;
		} else if (namelen > 0 && namelen < sizeof(name)) {
			ret = sync_read_packet(name, namelen, ser);
			if (ret < 0)
				goto err;
			name[namelen] = 0;
			msg.req.namelen = 0;
		}

		adbd_info("sync: req:0x%x", msg.req.id);

		switch (msg.req.id) {
		case ID_STAT:
			if(do_stat(name, ser)) goto err;
			break;
		case ID_LIST:
			if(do_list(name, ser)) goto err;
			break;
		case ID_SEND:
			if(do_send(name, ser)) goto err;
			break;
		case ID_RECV:
			if(do_recv(name, ser)) goto err;
			break;
		case ID_QUIT:
			adbd_debug("");
			goto err;
		default:
			fatal("unknown command");
			goto err;
		}
	}
err:
	if (!ser->has_send_close) {
		ser->has_send_close = 1;
		adbd_info("\n");
		send_close(ser->localid, ser->remoteid, ser->transport);
	}
	adbd_info("sync: done");
	return;
}
