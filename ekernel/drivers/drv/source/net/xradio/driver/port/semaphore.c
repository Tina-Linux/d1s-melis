#ifdef CONFIG_OS_NUTTX

#include "semaphore.h"

#define XR_SEM_LOGE sinfo
#define XR_SEM_LOGI sinfo
#define XR_SEM_LOGD sinfo
#define XR_SEM_LOGW sinfo

int snd_schd_timeout(xr_drv_sema_t *sem, XR_DRV_Time_t ms)
{
	int ret;
	struct timespec tp;

	clock_gettime(CLOCK_REALTIME, &tp);

	tp.tv_sec += ms/1000;
	tp.tv_nsec += ms%1000*1000;

	XR_SEM_LOGI("schd=%p, set timeout : %lu, %lu\n", sem, tp.tv_sec, tp.tv_nsec);
	ret = nxsem_timedwait(sem, (const struct timespec *)&tp);

	XR_SEM_LOGE("sem_timedwait exit...ret=%d\n", ret);
	if (ret == -ETIMEDOUT)
		return 0;
	else if (ret != 0) {
		XR_SEM_LOGE("sem_timedwait fail, return %d\n", ret);
		return ret;
	}
	return 0;
}
#endif
