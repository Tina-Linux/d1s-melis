#include <kernel/os/os_errno.h>
rt_err_t rt_get_errno(void);
void rt_set_errno(rt_err_t error);

int XR_OS_GetErrno(void)
{
	return (int)rt_get_errno();
}

void XR_OS_SetErrno(int err)
{
	rt_set_errno((rt_err_t)err);
}


