/*
 * workqueue.h --- work queue handling for Linux.
 */

#ifndef _LINUX_WORKQUEUE_H
#define _LINUX_WORKQUEUE_H

#include <stdio.h>
#include "xr_types.h"

#include "semaphore.h"

#include "timer.h"

#include "sys/list.h"
#include "spinlock.h"

//#define FSM_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

enum {
	WORK_STRUCT_STATIC      = (0),       /* work item is init state */
	WORK_STRUCT_PENDING     = (1),       /* work item is pending */
	WORK_STRUCT_RUNNING     = (2),       /* work item is running */
	//WORK_STRUCT_WAITCLEAN   = (3),       /* wait to clean */
};

struct work_struct;

typedef void (*work_func_t)(struct work_struct *work);

struct work_struct {
	u32 flg;
	struct list_head entry;
	void *arg;
	work_func_t func;
	spinlock_t lock;
#ifdef FSM_DEBUG
	char name[48];
#endif
};

struct timer_list {
	XR_DRV_Timer_t timer;
	long expires;
};

struct delayed_work {
	struct work_struct work;
	struct timer_list timer;
};

#define TIMER_INITIALIZER(_timer, _expires) {		\
		(_timer)->expires = (_expires),		\
	}

/*
 * initialize a work item's function pointer
 */
#define PREPARE_WORK(_work, _func)				\
	do {							\
		(_work)->func = (_func);			\
	} while (0)

#define PREPARE_DELAYED_WORK(_work, _func)			\
	PREPARE_WORK(&(_work)->work, (_func))

/*
 * initialize all of a work item in one go
 *
 * NOTE! No point in using "atomic_long_set()": using a direct
 * assignment of the work data initializer allows the compiler
 * to generate better code.
 */
#ifdef FSM_DEBUG
#define INIT_WORK(_work, _func)                                 \
	do {                                                    \
		(_work)->arg = NULL;                            \
		(_work)->flg = WORK_STRUCT_STATIC;              \
		PREPARE_WORK((_work), &(_func));                \
		INIT_LIST_HEAD(&(_work)->entry);                \
		spin_lock_init(&(_work)->lock);                 \
		strcpy((_work)->name, #_func);                  \
	} while (0)
extern void print_works(void);
#else
#define INIT_WORK(_work, _func)                         \
	do {                                                \
		(_work)->arg = NULL;                            \
		(_work)->flg = WORK_STRUCT_STATIC;              \
		PREPARE_WORK((_work), &(_func));                \
		INIT_LIST_HEAD(&(_work)->entry);                \
		spin_lock_init(&(_work)->lock);                 \
	} while (0)
#endif
#define DESTROY_WORK(_work)                                     \
	do {                                                    \
		spin_lock_exit(&(_work)->lock);                 \
	} while (0)

static XR_INLINE int init_timer(struct timer_list *timer, void (*func)(void *), void *arg)
{
	return xr_drv_timer_init(&(timer->timer), XR_DRV_TIMER_ONCE, func, arg, XR_DRV_WAIT_FOREVER);
}

/**
 * del_timer - deactive a timer.
 * @timer: the timer to be deactivated
 *
 * del_timer() deactivates a timer - this works on both active and inactive
 * timers.
 *
 * The function returns whether it has deactivated a pending timer or not.
 * (ie. del_timer() of an inactive timer returns 0, del_timer() of an
 * active timer returns 1.)
 */
static XR_INLINE int del_timer(struct timer_list *timer)
{
	return xr_drv_timer_destroy(&(timer->timer));
}

#define del_timer_sync(t) del_timer(t)

static XR_INLINE int timer_pending(struct timer_list *timer)
{
	return xr_drv_timer_active(&(timer->timer));
}
extern void queue_delayed_cbc(void *argument);

#define INIT_DELAYED_WORK(_dwork, _func)                        \
	do {							\
		INIT_WORK(&(_dwork)->work, (_func));		\
		init_timer(&(_dwork)->timer, (&queue_delayed_cbc), &(_dwork)->work); \
	} while (0)
#define DESTROY_DELAYED_WORK(_dwork)                            \
	do {                                                    \
		del_timer(&(_dwork)->timer);                    \
		DESTROY_WORK(&(_dwork)->work);                  \
	} while (0)

struct work_struct *create_singlethread_workqueue(const char *name);
void destroy_singlethread_workqueue(struct work_struct *wq);

/**
 * work_pending - Find out whether a work item is currently pending
 * @work: The work item in question
 */
#define work_pending(work) (WORK_STRUCT_PENDING == (work)->flg)

#define work_runing(work) (WORK_STRUCT_RUNNING == (work)->flg)

/**
 * delayed_work_pending - Find out whether a delayable work item is currently
 * pending
 * @work: The work item in question
 */
#define delayed_work_pending(w) work_pending(&(w)->work)

/*
 * Kill off a pending schedule_delayed_work().  Note that the work callback
 * function may still be running on return from cancel_delayed_work(), unless
 * it returns 1 and the work doesn't re-arm itself. Run flush_workqueue() or
 * cancel_work_sync() to wait on it.
 */
bool cancel_delayed_work(struct delayed_work *work);
bool cancel_work_sync(struct work_struct *work);

extern int queue_work(struct work_struct *wq, struct work_struct *work);
extern int queue_delayed_work(struct work_struct *wq,
			struct delayed_work *work, unsigned long delay);

#define init_waitqueue_head(q)                          \
	do {                                            \
		spin_lock_init(&q->lock);               \
		INIT_LIST_HEAD(&q->task_list);          \
	} while (0)

#define deinit_waitqueue_head(q)                        \
	do {                                            \
		spin_lock_exit(&q->lock);               \
		INIT_LIST_HEAD(&q->task_list);          \
	} while (0)

extern void flush_workqueue(struct work_struct *wq);

extern bool cancel_delayed_work_sync(struct delayed_work *dwork);

int mod_timer(struct timer_list *timer, unsigned long expires);

/**
 * wake_up - wake up threads blocked on a waitqueue.
 * @q: the waitqueue
 * @mode: which threads
 * @nr_exclusive: how many wake-one or wake-many threads to wake up
 * @key: is directly passed to the wakeup function
 *
 * It may be assumed that this function implies a write memory barrier before
 * changing the task state if and only if any tasks are woken up.
 */
static __always_inline void wake_up(xr_drv_sema_t *q)
{
#if 0
	if (down_trylock(q))
		;
#endif
	xr_drv_sema_release(q);
}

/**
 * wait_event_timeout - sleep until a condition gets true or a timeout elapses
 * @wq: the waitqueue to wait on
 * @condition: a C expression for the event to wait for
 * @timeout: timeout, in jiffies
 *
 * The process is put to sleep (TASK_UNINTERRUPTIBLE) until the
 * @condition evaluates to true. The @condition is checked each time
 * the waitqueue @wq is woken up.
 *
 * wake_up() has to be called after changing any variable that could
 * change the result of the wait condition.
 *
 * The function returns 0 if the @timeout elapsed, and the remaining
 * jiffies if the condition evaluated to true before the timeout elapsed.
 */
#if 1
#define ___wait_cond_timeout(condition)					\
({									\
	bool __cond = (condition);					\
	if (__cond && !__ret)						\
		__ret = 1;						\
	__cond || !__ret;						\
})

/* referenced from linux-3.4
 * Returns:
 * The function returns 0 if the @timeout elapsed, and the remaining
 * jiffies if the condition evaluated to true before the timeout elapsed.
 */
#define __wait_event_timeout(q, condition, timeout)	\
({							\
	long __tmo = timeout;				\
	long __ret = jiffies + (timeout);		\
	int __condition;				\
	for (;;) {					\
		xr_drv_sema_wait((q), (XR_DRV_Time_t)(__tmo));	\
		__condition = (condition);			\
		if (__condition) {				\
			if ((timeout) == XR_DRV_WAIT_FOREVER) {	\
				__ret = 1;			\
			} else {				\
				__ret = __ret - jiffies;	\
				if (__ret <= 0)			\
					__ret = 1;		\
			}					\
			break;					\
		} else {					\
			if ((timeout) != XR_DRV_WAIT_FOREVER) {	\
				__tmo = __ret - (long)jiffies;	\
				if (__tmo <= 0) {		\
					__ret = 0;		\
					break;			\
				}				\
			}					\
		}						\
	}							\
	__ret;							\
})

/**
 * wait_event_timeout - sleep until a condition gets true or a timeout elapses.
 * NOTE: always return 0 if timeout is 0.
 * @wq: the waitqueue to wait on
 * @condition: a C expression for the event to wait for
 * @timeout: timeout, in jiffies
 *
 * The process is put to sleep (TASK_UNINTERRUPTIBLE) until the
 * @condition evaluates to true. The @condition is checked each time
 * the waitqueue @wq is woken up.
 *
 * wake_up() has to be called after changing any variable that could
 * change the result of the wait condition.
 *
 * Returns:
 * 0 if the @condition evaluated to %false after the @timeout elapsed,
 * 1 if the @condition evaluated to %true after the @timeout elapsed,
 * or the remaining jiffies (at least 1) if the @condition evaluated
 * to %true before the @timeout elapsed.
 */
#define wait_event_timeout(wq, condition, timeout)			\
({									\
	long __ret = timeout;						\
	if (!___wait_cond_timeout(condition))				\
		__ret = __wait_event_timeout(wq, condition, timeout);	\
	else								\
		xr_drv_sema_wait((wq), 0);				\
	__ret;								\
})

#define wait_event_interruptible_timeout(q, condition, timeout) wait_event_timeout(q, condition, timeout)

#define wait_event_interruptible(wq, condition) wait_event_interruptible_timeout(wq, condition, XR_DRV_WAIT_FOREVER)

#else
/* referenced from linux-3.10
 * Returns:
 * 0 if the @condition evaluated to %false after the @timeout elapsed,
 * 1 if the @condition evaluated to %true after the @timeout elapsed,
 * the remaining jiffies (at least 1) if the @condition evaluated
 * to %true before the @timeout elapsed, or -%ERESTARTSYS if it was
 * interrupted by a signal.
 */
#define wait_event_timeout(q, condition, timeout)		\
({								\
	signed long __temp = 0; 				\
	unsigned long __ret = jiffies + (timeout);		\
	int __condition = (condition);				\
	if ((__condition)) {					\
		__temp = xr_drv_sema_wait((q), 0);		\
	} else {						\
		__temp = xr_drv_sema_wait((q), (timeout));	\
		__condition = (condition);			\
	}							\
	if (__condition || __temp > 0) {			\
		if ((timeout) == XR_DRV_WAIT_FOREVER) { 		\
			__ret = 1;				\
		} else {					\
			__temp = __ret - jiffies;		\
			__ret = __temp <= 0 ? 1 : __temp;	\
		}						\
	} else {						\
		__ret = 0;					\
	}							\
	__ret;							\
})

#define wait_event_interruptible_timeout(q, condition, timeout) wait_event_timeout(q, condition, timeout)

#define wait_event_interruptible(wq, condition) \
({	unsigned long ___ret;								\
	do { \
		___ret = wait_event_interruptible_timeout(wq, condition, XR_DRV_WAIT_FOREVER);\
		if (condition) \
			break; \
	} while (1); \
	___ret; \
})
#endif

#ifdef __cplusplus
}
#endif

#endif
