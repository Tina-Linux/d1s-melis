#ifndef _LINUX_POLL_H
#define _LINUX_POLL_H

#include <dfs_poll.h>
#include <ipc/poll.h>

#define DEFAULT_POLLMASK POLLMASK_DEFAULT

/*
 * Return the set of events that the application wants to poll for.
 * This is useful for drivers that need to know whether a DMA transfer has
 * to be started implicitly on poll(). You typically only want to do that
 * if the application is actually polling for POLLIN and/or POLLOUT.
 */
static inline unsigned long poll_requested_events(struct rt_pollreq *p)
{
	return p ? p->_key : ~0UL;
}

#endif /* _LINUX_POLL_H */
