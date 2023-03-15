#ifndef _LINUX_FS_H
#define _LINUX_FS_H

#include <melis/typedef.h>

struct file {
	struct dfs_fd *dfd;

	u32    f_flags; //O_RDWR | O_NONBLOCK
	__s64  f_pos;
	int    f_mode;
	int    f_dev;

	void   *private_data;   //struct v4l2_fh*
	int    f_fd;
};

#endif /* _LINUX_FS_H */
