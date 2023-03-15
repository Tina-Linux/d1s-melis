/*
 * kernel/workqueue.c - generic async execution with shared worker pool
 *
 * This is the generic async execution mechanism.  Work items as are
 * executed in process context.  The worker pool is shared and
 * automatically managed.  There is one worker pool for each CPU and
 * one extra for works which are better served by workers which are
 * not bound to any specific CPU.
 *
 */
#include <stdio.h>

#include "delay.h"
#include "xr_types.h"
#include "atomic.h"

#include "workqueue.h"
#include "kernel.h"

#include "threads.h"

#define WORKQUEUE_SIGNAL_USE_SEM /* use semaphore to implement signal */

//#define FSM_CHECK

//#define FSM_LOGD printf
#define FSM_LOGW printf
#define FSM_LOGE printf
//#define FSM_LOGN printf

#define FSM_LOGD(x...)
#define FSM_LOGN(x...)

#define WORK_TASK_NUM 1

#define WORK_WAIT_TIME (5 * HZ)  /* 5S */

/* Thread define */
#ifdef __MELIS__
#define FSM_TASK_STACK (1024*8)
#elif (defined(__CONFIG_OS_RTTHREAD))
#define FSM_TASK_STACK (1024*8)
#elif (defined(__CONFIG_OS_YUNOS) || defined(CONFIG_OS_NUTTX))
#define FSM_TASK_STACK (1024*8)
#else
#ifdef __CONFIG_WLAN_SMALL_STACK_SIZE
#define FSM_TASK_STACK (1024*1)
#else
#define FSM_TASK_STACK (1024*2)
#endif
#endif

struct work_task {
	u32 flg; /* 0:not used, 1:used, 2:wait to be removed  */
	xr_drv_thread_t _thread;
	u32 flg_exit; /* 1:should exit */
#ifdef WORKQUEUE_SIGNAL_USE_SEM
	xr_drv_sema_t _wq_semp;
#endif /* WORKQUEUE_SIGNAL_USE_SEM */
#ifdef FSM_CHECK
	u32 _st_pe; /* for stack check */
#endif
};

struct work_task _fsm_ws[WORK_TASK_NUM];

static spinlock_t fsm_lock;

static struct work_struct xr_wq_head;

#if ((defined FSM_DEBUG) || (defined FSM_CHECK))
#define FSM_BUG_ON(v) \
	do { if (v) { \
		FSM_LOGN("BUG at %s:%d! %d\n", __func__, __LINE__, \
		         (unsigned int)(v)); while (1);}} while (0)

static void parse_works(struct work_struct *work_head)
{
	struct work_struct *wq;

	FSM_LOGN("%s(%p)", work_head->name, work_head);
	list_for_each_entry(wq, &work_head->entry, entry) {
		FSM_LOGN("-->%s(%p)", wq->name, wq);
	}
	FSM_LOGN("\n");
}

void print_works(void)
{
	parse_works(&xr_wq_head);
}
#endif
#ifdef CONFIG_OS_NUTTX_THTREAD
static void work_task_fn(int argc, FAR char *argv[])
{
	struct work_task *p_task =(struct work_task *)argv[0];
#else
static void work_task_fn(void *arg)
{
	struct work_task *p_task =(struct work_task *)arg;
#endif
	struct work_struct *wq;
	bool run;
	unsigned long flags;

	while (1) {
#ifdef WORKQUEUE_SIGNAL_USE_SEM
		xr_drv_sema_wait(&(p_task->_wq_semp), XR_DRV_WAIT_FOREVER);
#else
		xr_thread_signal_wait(0x02, XR_DRV_WAIT_FOREVER);
#endif
		if (p_task->flg_exit) {
			FSM_LOGN("%s thread exit\n", __func__);
			p_task->flg_exit = 0;
			break;
		}

		xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
		while (!list_empty(&xr_wq_head.entry)) {
			wq = list_first_entry(&xr_wq_head.entry, struct work_struct, entry);
			if (wq->flg == WORK_STRUCT_PENDING) {
				wq->flg = WORK_STRUCT_RUNNING;
				run = true;
			} else
				run = false;
			list_del(&wq->entry);
			xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
			if (run && wq->func)
				wq->func(wq);  /* exe this work */
			xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
			if (wq->flg == WORK_STRUCT_RUNNING)
				wq->flg = WORK_STRUCT_STATIC;
			else
				FSM_LOGD("%s, work:%s state change %u\n", __func__,
#ifdef FSM_DEBUG
				wq->name,
#else
				"",
#endif
				wq->flg);
		}
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
#ifdef FSM_CHECK
		{
			int i;
			for (i = 0; i < WORK_TASK_NUM; i++) {
				if (_fsm_ws[i]._st_pe) {
					FSM_LOGE("task%d stack overflow, %x\n", i, _fsm_ws[i]._st_pe);
					FSM_BUG_ON(_fsm_ws[i]._st_pe);
				}
			}
		}
#endif
		xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
		p_task->flg = 2;
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
	}
	xr_drv_thread_destroy(&p_task->_thread);
}

void fsm_start(void *argument)
{
	int i;
	bool find = false;
	unsigned long flags = 0;

	xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
	for (i = 0; i < WORK_TASK_NUM; i++) {
		if (_fsm_ws[i].flg == 2) {
			_fsm_ws[i].flg = 0;
		}
		if (_fsm_ws[i].flg == 0) {
			_fsm_ws[i].flg = 1;
			find = true;
			break;
		}
	}
	xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);

	if (find) {
#ifdef WORKQUEUE_SIGNAL_USE_SEM
		xr_drv_sema_release(&_fsm_ws[i]._wq_semp);
#else
		xr_thread_signal_set(&_fsm_ws[i]._thread, 0x02);
#endif
	} else {
#ifdef WORKQUEUE_SIGNAL_USE_SEM
		xr_drv_sema_release(&_fsm_ws[0]._wq_semp);
#else
		xr_thread_signal_set(&_fsm_ws[0]._thread, 0x02); /* exe on [0] */
#endif
	}
}

/**
 * queue_work - queue work on a workqueue
 * @wq: workqueue to use
 * @work: work to queue
 *
 * Returns 0 if @work was already on a queue, non-zero otherwise.
 *
 */
int queue_work(struct work_struct *wq, struct work_struct *work)
{
	int ret = 0;
	unsigned long flags = 0;

	xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
	if (work->flg == WORK_STRUCT_PENDING) {
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
		return ret;
	}
	work->flg = WORK_STRUCT_PENDING;
	list_add_tail(&work->entry, &xr_wq_head.entry);
	xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
	ret = 1;
	fsm_start(work);

	return ret;
}

/**
 * cancel_work_sync - cancel a work and wait for it to finish
 * @work: the work to cancel
 *
 * Cancel @work and wait for its execution to finish.  This function
 * can be used even if the work re-queues itself or migrates to
 * another workqueue.  On return from this function, @work is
 * guaranteed to be not pending or executing on any CPU.
 *
 * cancel_work_sync(&delayed_work->work) must not be used for
 * delayed_work's.  Use cancel_delayed_work_sync() instead.
 *
 * The caller must ensure that the workqueue on which @work was last
 * queued can't be destroyed before this function returns.
 *
 * RETURNS:
 * %true if @work was pending, %false otherwise.
 */
bool cancel_work_sync(struct work_struct *work)
{
	int i = 0;
	unsigned long flags = 0;

	xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
	if (work->flg == WORK_STRUCT_PENDING) {
		list_del(&work->entry);
		work->flg = WORK_STRUCT_STATIC;
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
		return 1;
	}
	while (WORK_STRUCT_RUNNING == work->flg) {
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
		xr_drv_msleep(5);
		xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
		if (i++ > WORK_WAIT_TIME/5) {
			FSM_LOGW("%s waiting tmo!\n", __func__);
			i = 0;
		}
#ifdef FSM_DEBUG
		FSM_LOGW("%s,%d waiting %s\n", __func__, __LINE__, work->name);
#endif
	}
	xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);

	return 0;
}

/**
 * mod_timer - modify a timer's timeout
 * @timer: the timer to be modified
 * @expires: new timeout in jiffies
 *
 * mod_timer() is a more efficient way to update the expire field of an
 * active timer (if the timer is inactive it will be activated)
 *
 * mod_timer(timer, expires) is equivalent to:
 *
 *     del_timer(timer); timer->expires = expires; add_timer(timer);
 *
 * Note that if there are multiple unserialized concurrent users of the
 * same timer, then mod_timer() is the only safe way to modify the timeout,
 * since add_timer() cannot modify an already running timer.
 *
 * The function returns whether it has modified a pending timer or not.
 * (ie. mod_timer() of an inactive timer returns 0, mod_timer() of an
 * active timer returns 1.)
 */
int mod_timer(struct timer_list *timer, unsigned long expires)
{
	if (expires) {
		timer->expires = expires + jiffies;
		return xr_drv_timer_change_period(&(timer->timer), msecs_to_jiffies(expires));
	} else {
		return xr_drv_timer_stop(&(timer->timer));
	}
}
void queue_delayed_cbc(void *argument)
{
	struct work_struct *work;
	unsigned long flags = 0;

	if (!argument)
		return ;

	work = (struct work_struct *)argument;
	xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
	if (WORK_STRUCT_PENDING == work->flg) {
		list_add_tail(&work->entry, &xr_wq_head.entry);
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
		fsm_start(work);
	} else {
		xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
	}
}

/**
 * queue_delayed_work - queue work on a workqueue after delay
 * @wq: workqueue to use
 * @dwork: delayable work to queue
 * @delay: number of jiffies to wait before queueing
 *
 * Returns 0 if @work was already on a queue, non-zero otherwise.
 */
int queue_delayed_work(struct work_struct *wqd_head,
                       struct delayed_work *dwork, unsigned long delay)
{
	unsigned long flags = 0;

	if (delay == 0)
		return queue_work(&xr_wq_head, &dwork->work);

	xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
	dwork->work.flg = WORK_STRUCT_PENDING;
	xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);
	mod_timer(&dwork->timer, delay);

	return 1;
}

/**
 * cancel_delayed_work_sync - cancel a delayed work and wait for it to finish
 * @dwork: the delayed work cancel
 *
 * This is cancel_work_sync() for delayed works.
 *
 * RETURNS:
 * %true if @dwork was pending, %false otherwise.
 */
bool cancel_delayed_work_sync(struct delayed_work *dwork)
{
	mod_timer(&dwork->timer, 0);

	/* there is no need to list_del(&work->entry) by call cancel_work_sync,
	 * but do this is harmless */
	return cancel_work_sync(&dwork->work);
}

bool cancel_delayed_work(struct delayed_work *dwork)
{
	int ret = 0;
	unsigned long flags = 0;

	mod_timer(&dwork->timer, 0);

	xr_spin_lock_irqsave(&xr_wq_head.lock, flags);
	if (dwork->work.flg == WORK_STRUCT_PENDING)
		ret = 1;
	dwork->work.flg = WORK_STRUCT_STATIC;
	xr_spin_unlock_irqrestore(&xr_wq_head.lock, flags);

	return ret;
}

/**
 * flush_workqueue - ensure that any scheduled work has run to completion.
 * @wq: workqueue to flush
 *
 * Forces execution of the workqueue and blocks until its completion.
 * This is typically used in driver shutdown handlers.
 *
 * We sleep until all works which were queued on entry have been handled,
 * but we are not livelocked by new incoming ones.
 */
void flush_workqueue(struct work_struct *wq)
{
	while (!list_empty(&wq->entry) || (wq->flg != WORK_STRUCT_STATIC)) {
		xr_drv_msleep(5);
		FSM_LOGW("%s,%d waiting...\n", __func__, __LINE__);
	}
}

static void fsm_test_cbc(struct work_struct *work)
{
	FSM_LOGW("%s,%d arg:%p\n", __func__, __LINE__, work);
}

struct work_struct *create_singlethread_workqueue(const char *name)
{
	int i;

	INIT_WORK(&xr_wq_head, fsm_test_cbc);
	FSM_LOGN("%s @ %p\n", name, &xr_wq_head);
#ifdef FSM_DEBUG
	strncpy(xr_wq_head.name, name, 48);
#endif

	spin_lock_init(&fsm_lock);
	for (i = 0; i < WORK_TASK_NUM; i++) {
		_fsm_ws[i].flg = 0;
		_fsm_ws[i].flg_exit = 0;

#ifdef WORKQUEUE_SIGNAL_USE_SEM
		xr_drv_sema_init(&_fsm_ws[i]._wq_semp, 0);
#endif
		xr_drv_thread_init(&_fsm_ws[i]._thread,
		                   "workqueue",
		                   work_task_fn,
		                   &_fsm_ws[i],
		                   XR_DRV_THREAD_PRIO_DRV_WORK,
		                   FSM_TASK_STACK);
	}
	return &xr_wq_head;
}

void destroy_singlethread_workqueue(struct work_struct *wq)
{
	int i;

	for (i = 0; i < WORK_TASK_NUM; i++) {
		_fsm_ws[i].flg_exit = 1;

#ifdef WORKQUEUE_SIGNAL_USE_SEM
		xr_drv_sema_release(&_fsm_ws[i]._wq_semp);
		while (xr_drv_thread_valid(&_fsm_ws[i]._thread))
			xr_drv_msleep(1);
		xr_drv_sema_destroy(&_fsm_ws[i]._wq_semp);
#else
		xr_thread_signal_set(&_fsm_ws[i]._thread, 0x02); /* exe on [0] */
		while (xr_drv_thread_valid(&_fsm_ws[i]._thread))
			xr_drv_msleep(1);
#endif
	}
	spin_lock_exit(&fsm_lock);
	DESTROY_WORK(&xr_wq_head);
}

#ifdef FSM_TEST
void fsm_test(void *arg)
{


}
#endif
