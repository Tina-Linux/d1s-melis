
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "ring_buffer.h"

int ring_buf_init(Ring_buf_manager *buf_mgr)
{
	pthread_mutex_init(&buf_mgr->mutex, NULL);

	buf_mgr->buf = (unsigned char * ) malloc(buf_mgr->buf_size);

	printf("buf = %p, size = %d\n", buf_mgr->buf, buf_mgr->buf_size);

	if (buf_mgr->buf == NULL)
	{
		printf("malloc for ring buf failed\n");
		return -1;
	}
	else
	{
		buf_mgr->buf_start = buf_mgr->buf;
		buf_mgr->buf_end = buf_mgr->buf + buf_mgr->buf_size - 1;
		buf_mgr->write_addr = buf_mgr->buf;
		buf_mgr->read_addr = buf_mgr->buf;
		buf_mgr->valid_size = 0;
	}

	return 0;
}

int ring_buf_destroy(Ring_buf_manager *buf_mgr)
{
	pthread_mutex_destroy(&buf_mgr->mutex);

	printf("buf = %p, size = %d\n", buf_mgr->buf, buf_mgr->buf_size);

	if (buf_mgr->buf != NULL)
	{
		free(buf_mgr->buf);
		buf_mgr->buf = NULL;
	}

	buf_mgr->buf_start = NULL;
	buf_mgr->buf_end = NULL;
	buf_mgr->write_addr = NULL;
	buf_mgr->read_addr = NULL;
	buf_mgr->valid_size = 0;

	return 0;
}

int ring_buf_write_data(Ring_buf_manager *buf_mgr, char *pdata, int len)
{
	int buf_empty_size = buf_mgr->buf_size - buf_mgr->valid_size;

	if (buf_empty_size < len)
	{
		printf("buffer is full: len = %d, empty_size = %d, total_size = %d\n",
			len, buf_empty_size, buf_mgr->buf_size);
		return -1;
	}

	pthread_mutex_lock(&buf_mgr->mutex);

	if (len <= (buf_mgr->buf_end - buf_mgr->write_addr + 1))
	{
		memcpy(buf_mgr->write_addr, pdata, len);
		buf_mgr->write_addr += len;
	}
	else
	{
		int tail_size = buf_mgr->buf_end - buf_mgr->write_addr + 1;
		memcpy(buf_mgr->write_addr, pdata, tail_size);
		memcpy(buf_mgr->buf_start, pdata + tail_size, len - tail_size);
		buf_mgr->write_addr = buf_mgr->buf_start + (len - tail_size);
	}

	if (buf_mgr->write_addr > buf_mgr->buf_end)
	{
		buf_mgr->write_addr = buf_mgr->buf_start;
    }
	buf_mgr->valid_size += len;

	//printf("write: len = %d, valid_size = %d, total_size = %d, write_addr = %p, diff = %d\n",
	//	len, buf_mgr->valid_size, buf_mgr->buf_size,
	//	buf_mgr->write_addr, buf_mgr->buf_end - buf_mgr->write_addr);

	pthread_mutex_unlock(&buf_mgr->mutex);

	return 0;
}

int ring_buf_read_data(Ring_buf_manager *buf_mgr, char *pdata, int len)
{

	if (len > buf_mgr->valid_size)
	{
		printf("valid_size is too short .\n");
		return -1;
	}

	int tail_size = buf_mgr->buf_end - buf_mgr->read_addr + 1;
}

int ring_buf_read_update(Ring_buf_manager *buf_mgr, int len)
{
	pthread_mutex_lock(&buf_mgr->mutex);

	buf_mgr->valid_size -= len;
	buf_mgr->read_addr += len;

	//xm_logd("diff read_addr = %d\n", buf_mgr->buf_end - buf_mgr->read_addr);

	if (buf_mgr->read_addr > buf_mgr->buf_end)
		buf_mgr->read_addr = buf_mgr->buf_start;

	pthread_mutex_unlock(&buf_mgr->mutex);
	return 0;
}

int ring_buf_read_update_and_copy(Ring_buf_manager *buf_mgr, int len, unsigned char* out_buf)
{
	pthread_mutex_lock(&buf_mgr->mutex);
    if (buf_mgr->valid_size < len)
    {
        printf("buffer data is not enough for read, valid_size = %d, but nead read len = %d\n", 
                buf_mgr->valid_size, len);
        return -1;
    }

    if (len <= (buf_mgr->buf_end - buf_mgr->read_addr + 1))
    {
	    memcpy(out_buf, buf_mgr->read_addr, len);
	    buf_mgr->read_addr += len;
	}
	else
	{
	    int tail_data_size = buf_mgr->buf_end - buf_mgr->read_addr + 1;
        memcpy(out_buf, buf_mgr->read_addr, tail_data_size);
        memcpy(out_buf+tail_data_size, buf_mgr->buf_start, len - tail_data_size);
        buf_mgr->read_addr = buf_mgr->buf_start + len - tail_data_size;
	}

	if (buf_mgr->read_addr > buf_mgr->buf_end)
	{
		buf_mgr->read_addr = buf_mgr->buf_start;
    }
	buf_mgr->valid_size -= len;

	pthread_mutex_unlock(&buf_mgr->mutex);
	return 0;
}

int ring_buf_get_valid_size(Ring_buf_manager *buf_mgr)
{
	int valid_size = 0;
	pthread_mutex_lock(&buf_mgr->mutex);

	valid_size = buf_mgr->valid_size;

	pthread_mutex_unlock(&buf_mgr->mutex);

	return valid_size;
}

int ring_buf_reset(Ring_buf_manager *buf_mgr)
{
	pthread_mutex_lock(&buf_mgr->mutex);

	buf_mgr->buf_start = buf_mgr->buf;
	buf_mgr->buf_end = buf_mgr->buf + buf_mgr->buf_size - 1;
	buf_mgr->write_addr = buf_mgr->buf;
	buf_mgr->read_addr = buf_mgr->buf;
	buf_mgr->valid_size = 0;

	pthread_mutex_unlock(&buf_mgr->mutex);

	return 0;
}
