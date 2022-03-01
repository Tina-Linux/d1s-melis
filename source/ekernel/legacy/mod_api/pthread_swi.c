/*
 * ===========================================================================================
 *
 *       Filename:  swi.c
 *
 *    Description:  syscall entry definition.
 *
 *        Version:  Melis3.0
 *         Create:  2018-04-24 11:11:07
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-27 18:45:37
 *
 * ===========================================================================================
 */

#include <typedef.h>
#include <rtthread.h>
#include <kconfig.h>
#include <mod_defs.h>
#include <rthw.h>
#include <kapi.h>
#include <debug.h>
#include <log.h>

#include "pthread.h"
#include "semaphore.h"
#include "clock_time.h"

extern int usleep(unsigned int usecs);
extern int msleep(unsigned int msecs);
extern void sleep(int seconds);
/*SWIHandler_PTHREAD_t*/
const unsigned long SWIHandler_entity_PTHREAD[] =
{
    (unsigned long)pthread_attr_destroy,
    (unsigned long)pthread_attr_init,
    (unsigned long)pthread_attr_setdetachstate,
    (unsigned long)pthread_attr_getdetachstate,
    (unsigned long)pthread_attr_setschedpolicy,
    (unsigned long)pthread_attr_getschedpolicy,
    (unsigned long)pthread_attr_setschedparam,
    (unsigned long)pthread_attr_getschedparam,
    (unsigned long)pthread_attr_setstacksize,
    (unsigned long)pthread_attr_getstacksize,
    (unsigned long)pthread_attr_setstackaddr,
    (unsigned long)pthread_attr_getstackaddr,
    (unsigned long)pthread_attr_setstack,
    (unsigned long)pthread_attr_getstack,
    (unsigned long)pthread_attr_setguardsize,
    (unsigned long)pthread_attr_getguardsize,
    (unsigned long)pthread_attr_setscope,
    (unsigned long)pthread_attr_getscope,
    (unsigned long)pthread_system_init,
    (unsigned long)pthread_create,
    (unsigned long)pthread_detach,
    (unsigned long)pthread_join,
    (unsigned long)pthread_exit,
    (unsigned long)pthread_once,
    (unsigned long)pthread_cleanup_pop,
    (unsigned long)pthread_cleanup_push,
    (unsigned long)pthread_cancel,
    (unsigned long)pthread_testcancel,
    (unsigned long)pthread_setcancelstate,
    (unsigned long)pthread_setcanceltype,
    (unsigned long)pthread_atfork,
    (unsigned long)pthread_kill,
    (unsigned long)pthread_self,
    (unsigned long)pthread_mutex_init,
    (unsigned long)pthread_mutex_destroy,
    (unsigned long)pthread_mutex_lock,
    (unsigned long)pthread_mutex_unlock,
    (unsigned long)pthread_mutex_trylock,
    (unsigned long)pthread_mutexattr_init,
    (unsigned long)pthread_mutexattr_destroy,
    (unsigned long)pthread_mutexattr_gettype,
    (unsigned long)pthread_mutexattr_settype,
    (unsigned long)pthread_mutexattr_setpshared,
    (unsigned long)pthread_mutexattr_getpshared,
    (unsigned long)pthread_condattr_destroy,
    (unsigned long)pthread_condattr_init,
    (unsigned long)pthread_condattr_getclock,
    (unsigned long)pthread_condattr_setclock,
    (unsigned long)pthread_cond_init,
    (unsigned long)pthread_cond_destroy,
    (unsigned long)pthread_cond_broadcast,
    (unsigned long)pthread_cond_signal,
    (unsigned long)pthread_cond_wait,
    (unsigned long)pthread_cond_timedwait,
    (unsigned long)pthread_rwlockattr_init,
    (unsigned long)pthread_rwlockattr_destroy,
    (unsigned long)pthread_rwlockattr_getpshared,
    (unsigned long)pthread_rwlockattr_setpshared,
    (unsigned long)pthread_rwlock_init,
    (unsigned long)pthread_rwlock_destroy,
    (unsigned long)pthread_rwlock_rdlock,
    (unsigned long)pthread_rwlock_tryrdlock,
    (unsigned long)pthread_rwlock_timedrdlock,
    (unsigned long)pthread_rwlock_timedwrlock,
    (unsigned long)pthread_rwlock_unlock,
    (unsigned long)pthread_rwlock_wrlock,
    (unsigned long)pthread_rwlock_trywrlock,
    (unsigned long)pthread_spin_init,
    (unsigned long)pthread_spin_destroy,
    (unsigned long)pthread_spin_lock,
    (unsigned long)pthread_spin_trylock,
    (unsigned long)pthread_spin_unlock,
    (unsigned long)pthread_barrierattr_destroy,
    (unsigned long)pthread_barrierattr_init,
    (unsigned long)pthread_barrierattr_getpshared,
    (unsigned long)pthread_barrierattr_setpshared,
    (unsigned long)pthread_barrier_destroy,
    (unsigned long)pthread_barrier_init,
    (unsigned long)pthread_barrier_wait,
    (unsigned long)pthread_getspecific,
    (unsigned long)pthread_setspecific,
    (unsigned long)pthread_key_create,
    (unsigned long)pthread_key_delete,
    (unsigned long)sem_close,
    (unsigned long)sem_destroy,
    (unsigned long)sem_getvalue,
    (unsigned long)sem_init,
    (unsigned long)sem_open,
    (unsigned long)sem_post,
    (unsigned long)sem_timedwait,
    (unsigned long)sem_trywait,
    (unsigned long)sem_unlink,
    (unsigned long)sem_wait,
    (unsigned long)clock_settime,
    (unsigned long)clock_gettime,
    (unsigned long)clock_getres,
    (unsigned long)usleep,
    (unsigned long)msleep,
    (unsigned long)sleep,
};
