#include "timer.h"

#ifdef CONFIG_ON_NUTTX
#if 0
static void test_timer_handler_once(int signo, FAR siginfo_t * info,
                            FAR void *ucontext)
{
	char *str = (char *)info->si_value.sival_ptr;
	NX_TIMER_LOGE("%s\n",str);
}
#endif

int nx_timer_delete(nx_timer_t *timer)
{
	sigset_t mask;

	timer_delete(timer->handle);

	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask, NULL);
	return 0;
}

int nx_timer_stop(nx_timer_t *timer)
{
	int ret;
	struct itimerspec timers;

	if(timer == NULL) {
		NX_TIMER_LOGE("timer handle invaild.\n");
		return -1;
	}

	timers.it_value.tv_sec = MAX_TIME_DELAY;
	timers.it_value.tv_nsec = MAX_TIME_DELAY;

	timers.it_interval.tv_sec = MAX_TIME_DELAY;
	timers.it_interval.tv_nsec = MAX_TIME_DELAY;

	ret = timer_settime(timer->handle, 0, &timers, NULL);

	if (ret != OK) {
	    NX_TIMER_LOGE("timer_settime() failed:%d\n", ret);
	    return ret;
	}

	return ret;

}

int nx_timer_start(nx_timer_t *timer)
{
	int ret;
	struct itimerspec timers;

	if(timer == NULL) {
		NX_TIMER_LOGE("timer handle invaild.\n");
		return -1;
	}

	timers.it_value.tv_sec = timer->init_time_ms / 1000;
	timers.it_value.tv_nsec = (timer->init_time_ms % 1000) * 1000 * 1000;

	if(timer->type == NX_TIMER_PERIODIC) {
		timers.it_interval.tv_sec = timer->init_time_ms / 1000;
		timers.it_interval.tv_nsec = (timer->init_time_ms % 1000) * 1000 * 1000;
	}else {
		timers.it_interval.tv_sec = 0;
		timers.it_interval.tv_nsec = 0;
	}

	ret = timer_settime(timer->handle, 0, &timers, NULL);

	if (ret != OK) {
	    NX_TIMER_LOGE("timer_settime() failed:%d\n", ret);
	    return ret;
	}

	return ret;

}

int nx_timer_change_period(nx_timer_t *timer, nx_time_t ms)
{
	int ret;
	struct itimerspec timers;

	if(timer == NULL) {
		NX_TIMER_LOGE("timer handle invaild.\n");
		return -1;
	}
	timers.it_value.tv_sec = ms / 1000;
	timers.it_value.tv_nsec = (ms % 1000) * 1000 * 1000;

	if(timer->type == NX_TIMER_PERIODIC) {
		timers.it_interval.tv_sec = ms / 1000;
		timers.it_interval.tv_nsec = (ms % 1000) * 1000 * 1000;
	}else {
		timers.it_interval.tv_sec = 0;
		timers.it_interval.tv_nsec = 0;
	}

	ret = timer_settime(timer->handle, 0, &timers, NULL);

	if (ret != OK) {
	    NX_TIMER_LOGE("timer_settime() failed:%d\n", ret);
	    return ret;
	}

	return ret;

}

int nx_timer_create(nx_timer_t *timer, nx_timer_type_t type,
		nx_timer_cb_t handler, void *ptr_param, nx_time_t ms)
{

	int ret;
	sigset_t mask;
	struct sigaction sa;
	struct sigevent sev;
	timer_t timerid;

	/* Check argument. */
	if (handler == NULL) {
	    return -1;
	}

	sigemptyset(&mask);
	sigaddset(&mask, MY_TIMER_SIGNAL);

	ret = sigprocmask(SIG_UNBLOCK, &mask, NULL);
	if (ret != OK) {
	    NX_TIMER_LOGE("sigprocmask() failed:%d\n", ret);
	    return -1;
	}

	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);
	sigdelset(&sa.sa_mask, MY_TIMER_SIGNAL);
	ret = sigaction(MY_TIMER_SIGNAL, &sa, NULL);

	if (ret != OK) {
	    NX_TIMER_LOGE("sigaction() failed:%d\n", ret);
	    return -1;
	}

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = MY_TIMER_SIGNAL;
	//sev.sigev_value.sival_int = int_param;
	sev.sigev_value.sival_ptr = ptr_param;

	ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
	if (ret != OK) {
	    NX_TIMER_LOGE("timer_create() failed:%d\n", ret);
	    return -1;
	}

	timer->handle = timerid;
	timer->type = type;
	timer->init_time_ms = ms;

	return 0;
}
int nx_timer_is_active(nx_time_t *timer)
{
	return 0;
}

#endif
