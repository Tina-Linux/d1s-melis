#ifndef _LINUX_SEQ_FILE_H
#define _LINUX_SEQ_FILE_H

#include <string.h>
#include "xr_types.h"

struct seq_file {
	//char *buf;
	//size_t size;
	//size_t from;
	//size_t count;
	//loff_t index;
	//loff_t read_pos;
	//u64 version;
	//struct mutex lock;
	//const struct seq_operations *op;
	///int poll_event;
	void *private;
};

struct ntxr_file {
	/* needed for tty driver, and maybe others */
	void *private_data;
};

#endif
