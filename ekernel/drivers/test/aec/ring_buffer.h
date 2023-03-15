#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <pthread.h>

typedef struct Ring_buf_manager
{
    pthread_mutex_t    mutex;
	unsigned char*     buf;
	unsigned char*     buf_start;
	unsigned char*     buf_end;

	unsigned char*     write_addr;
	unsigned char*     read_addr;
	int				   valid_size;

	int				   buf_size;
	int				   read_align_size;

}Ring_buf_manager;


int ring_buf_init(Ring_buf_manager *buf_mgr);

int ring_buf_destroy(Ring_buf_manager *buf_mgr);

int ring_buf_write_data(Ring_buf_manager *buf_mgr, char *pdata, int len);

int ring_buf_read_update(Ring_buf_manager *buf_mgr, int len);

int ring_buf_get_valid_size(Ring_buf_manager *buf_mgr);

int ring_buf_reset(Ring_buf_manager *buf_mgr);

int ring_buf_read_update_and_copy(Ring_buf_manager *buf_mgr, int len, unsigned char* out_buf);

#endif