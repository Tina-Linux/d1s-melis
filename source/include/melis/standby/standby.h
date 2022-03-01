/*
 * for standby head file
 */

#ifndef __MELIS_STANDBY_
#define __MELIS_STANDBY_

#include <errno.h>
enum dev_notify_type {
	DEV_NOTIFY_SUSPEND,
	DEV_NOTIFY_RESUME
};

#define STANDBY_NORMAL 1
#define STANDBY_TEST   2

struct dev_pm;

#ifdef CONFIG_STANDBY

int standby_enter(int level);

struct dev_pm *register_pm_dev_notify(int (*suspend)(void *),
				      int (*resume)(void *), void *d);
void unregister_pm_dev_notify(struct dev_pm *pm);

extern void (*arch_enter_standby)(int);
extern void (*arch_core_suspend)(void);
extern void (*arch_core_resume)(void);

#else

int inline standby_enter(int level)
{
	return 0;
}
struct dev_pm inline *register_pm_dev_notify(int (*suspend)(void *),
					     int (*resume)(void *), void *d)
{
	return (void *)(-EINVAL);
}
void inline unregister_pm_dev_notify(struct dev_pm *pm)
{
	return;
}

#endif

#endif /* __MELIS_STANDBY_ */
