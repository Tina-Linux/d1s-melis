/*
 * ===========================================================================================
 *
 *       Filename:  syscall.c
 *
 *    Description:  pthread syscall
 *
 *        Version:  Melis3.0
 *         Create:  2018-09-07 11:15:41
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2019-02-19 09:35:59
 *
 * ===========================================================================================
 */
#include <kapi.h>
#include <syscall.h>
#include <kconfig.h>
#include <pthread.h>
#include <semaphore.h>

int pthread_attr_destroy(pthread_attr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_attr_destroy), (long)attr);
    return result;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_attr_init), (long)attr);
    return result;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int state)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setdetachstate), (long)attr, (long)state);
    return result;
}

int pthread_attr_getdetachstate(pthread_attr_t const *attr, int *state)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_getdetachstate), (long)attr, (long)state);
    return result;
}

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setschedpolicy), (long)attr, (long)policy);
    return result;
}

int pthread_attr_getschedpolicy(pthread_attr_t const *attr, int *policy)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_getschedpolicy), (long)attr, (long)policy);
    return result;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, struct sched_param const *param)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setschedparam), (long)attr, (long)param);
    return result;
}

int pthread_attr_getschedparam(pthread_attr_t const *attr, struct sched_param *param)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_getschedparam), (long)attr, (long)param);
    return result;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stack_size)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setstacksize), (long)attr, (long)stack_size);
    return result;
}

int pthread_attr_getstacksize(pthread_attr_t const *attr, size_t *stack_size)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_getstacksize), (long)attr, (long)stack_size);
    return result;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stack_addr)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setstackaddr), (long)attr, (long)stack_addr);
    return result;
}

int pthread_attr_getstackaddr(pthread_attr_t const *attr, void **stack_addr)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_getstackaddr), (long)attr, (long)stack_addr);
    return result;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stack_base, size_t stack_size)
{
    int result ;
    result = (int)__syscall3((long)SYSCALL_PTD(pthread_attr_setstack), (long)attr, (long)stack_base, (long)stack_size);
    return result;
}

int pthread_attr_getstack(pthread_attr_t const *attr, void **stack_base, size_t *stack_size)
{
    int result ;
    result = (int)__syscall3((long)SYSCALL_PTD(pthread_attr_getstack), (long)attr, (long)stack_base, (long)stack_size);
    return result;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guard_size)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setguardsize), (long)attr, (long)guard_size);
    return result;
}

int pthread_attr_getguardsize(pthread_attr_t const *attr, size_t *guard_size)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_getguardsize), (long)attr, (long)guard_size);
    return result;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_attr_setscope), (long)attr, (long)scope);
    return result;
}

int pthread_attr_getscope(pthread_attr_t const *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_attr_getscope), (long)attr);
    return result;
}

int pthread_system_init(void)
{
    int result ;
    result = (int)__syscall0((long)SYSCALL_PTD(pthread_system_init));
    return result;
}

int pthread_create(pthread_t *tid, const pthread_attr_t *attr, void *(*start)(void *), void *arg)
{
    int result ;
    result = (int)__syscall4((long)SYSCALL_PTD(pthread_create), (long)tid, (long)attr, (long)start, (long)arg);
    return result;
}

int pthread_detach(pthread_t thread)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_detach), (long)thread);
    return result;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_join), (long)thread, (long)value_ptr);
    return result;
}

void pthread_exit(void *value_ptr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_exit), (long)value_ptr);
    return;
}

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_once), (long)once_control, (long)init_routine);
    return result;
}

void pthread_cleanup_pop(int execute)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_cleanup_pop), (long)execute);
    return;
}

void pthread_cleanup_push(void (*routine)(void *), void *arg)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_cleanup_push), (long)routine, (long)arg);
    return;
}

int pthread_cancel(pthread_t thread)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_cancel), (long)thread);
    return result;
}

pthread_t pthread_self(void)
{
    pthread_t result ;
    result = (pthread_t)__syscall0((long)SYSCALL_PTD(pthread_self));
    return result;
}

void pthread_testcancel(void)
{
    __u32 result ;
    result = (__u32)__syscall0((long)SYSCALL_PTD(pthread_testcancel));
    return;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_setcancelstate), (long)state, (long)oldstate);
    return result;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_setcanceltype), (long)type, (long)oldtype);
    return result;
}

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
    int result ;
    result = (int)__syscall3((long)SYSCALL_PTD(pthread_atfork), (long)prepare, (long)parent, (long)child);
    return result;
}

int pthread_kill(pthread_t thread, int sig)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_kill), (long)thread, (long)sig);
    return result;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_mutex_init), (long)mutex, (long)attr);
    return result;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_mutex_destroy), (long)mutex);
    return result;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_mutex_lock), (long)mutex);
    return result;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_mutex_unlock), (long)mutex);
    return result;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_mutex_trylock), (long)mutex);
    return result;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_mutexattr_init), (long)attr);
    return result;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)\
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_mutexattr_destroy), (long)attr);
    return result;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_mutexattr_gettype), (long)attr, (long)type);
    return result;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_mutexattr_settype), (long)attr, (long)type);
    return result;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int  pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_mutexattr_setpshared), (long)attr, (long)pshared);
    return result;
}

int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_mutexattr_getpshared), (long)attr, (long)pshared);
    return result;
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_condattr_destroy), (long)attr);
    return result;
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_condattr_init), (long)attr);
    return result;
}

int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_condattr_getclock), (long)attr, (long)clock_id);
    return result;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_condattr_setclock), (long)attr, (long)clock_id);
    return result;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_cond_init), (long)cond, (long)attr);
    return result;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_cond_destroy), (long)cond);
    return result;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_cond_broadcast), (long)cond);
    return result;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_cond_signal), (long)cond);
    return result;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_cond_wait), (long)cond, (long)mutex);
    return result;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    int result ;
    result = (int)__syscall3((long)SYSCALL_PTD(pthread_cond_timedwait), (long)cond, (long)mutex, (long)abstime);
    return result;
}

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlockattr_init), (long)attr);
    return result;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlockattr_destroy), (long)attr);
    return result;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_rwlockattr_getpshared), (long)attr, (long)pshared);
    return result;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_rwlockattr_setpshared), (long)attr, (long)pshared);
    return result;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_rwlock_init), (long)rwlock, (long)attr);
    return result;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlock_destroy), (long)rwlock);
    return result;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlock_rdlock), (long)rwlock);
    return result;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlock_tryrdlock), (long)rwlock);
    return result;
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_rwlock_timedrdlock), (long)rwlock, (long)abstime);
    return result;
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_rwlock_timedwrlock), (long)rwlock, (long)abstime);
    return result;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlock_unlock), (long)rwlock);
    return result;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlock_wrlock), (long)rwlock);
    return result;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_rwlock_trywrlock), (long)rwlock);
    return result;
}

int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_spin_init), (long)lock, (long)pshared);
    return result;
}

int pthread_spin_destroy(pthread_spinlock_t *lock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_spin_destroy), (long)lock);
    return result;
}

int pthread_spin_lock(pthread_spinlock_t *lock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_spin_lock), (long)lock);
    return result;
}

int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_spin_trylock), (long)lock);
    return result;
}

int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_spin_unlock), (long)lock);
    return result;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_barrierattr_destroy), (long)attr);
    return result;
}

int pthread_barrierattr_init(pthread_barrierattr_t *attr)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_barrierattr_init), (long)attr);
    return result;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_barrierattr_getpshared), (long)attr, (long)pshared);
    return result;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_barrierattr_setpshared), (long)attr, (long)pshared);
    return result;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_barrier_destroy), (long)barrier);
    return result;
}

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count)
{
    int result ;
    result = (int)__syscall3((long)SYSCALL_PTD(pthread_barrier_init), (long)barrier, (long)attr, (long)count);
    return result;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_barrier_wait), (long)barrier);
    return result;
}

void *pthread_getspecific(pthread_key_t key)
{
    void *result ;
    result = (void *)__syscall1((long)SYSCALL_PTD(pthread_getspecific), (long)key);
    return result;
}

int pthread_setspecific(pthread_key_t key, const void *value)

{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_setspecific), (long)key, (long)value);
    return result;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(pthread_key_create), (long)key, (long)destructor);
    return result;
}

int pthread_key_delete(pthread_key_t key)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(pthread_key_delete), (long)key);
    return result;
}

int sem_close(sem_t *sem)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(sem_close), (long)sem);
    return result;
}

int sem_destroy(sem_t *sem)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(sem_destroy), (long)sem);
    return result;
}

int sem_getvalue(sem_t *sem, int *sval)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(sem_getvalue), (long)sem, (long)sval);
    return result;
}

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    int result ;
    result = (int)__syscall3((long)SYSCALL_PTD(sem_init), (long)sem, (long)pshared, (long)value);
    return result;
}

sem_t *sem_open(const char *name, int oflag, ...)
{
    sem_t *result ;
    result = (sem_t *)__syscall2((long)SYSCALL_PTD(sem_open), (long)name, (long)oflag);
    return result;
}

int sem_post(sem_t *sem)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(sem_post), (long)sem);
    return result;
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(sem_timedwait), (long)sem, (long)abs_timeout);
    return result;
}

int sem_trywait(sem_t *sem)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(sem_trywait), (long)sem);
    return result;
}

int sem_unlink(const char *name)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(sem_unlink), (long)name);
    return result;
}

int sem_wait(sem_t *sem)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(sem_wait), (long)sem);
    return result;
}

int clock_settime(clockid_t clockid, const struct timespec *tp)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(clock_settime), (long)clockid, (long)tp);
    return result;
}

int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(clock_gettime), (long)clockid, (long)tp);
    return result;
}

int clock_getres(clockid_t clockid, struct timespec *res)
{
    int result ;
    result = (int)__syscall2((long)SYSCALL_PTD(clock_getres), (long)clockid, (long)res);
    return result;
}

int usleep (useconds_t __useconds)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(usleep), (long)__useconds);
    return result;
}

int msleep(unsigned int msecs)
{
    int result ;
    result = (int)__syscall1((long)SYSCALL_PTD(msleep), (long)msecs);
    return result;
}

unsigned sleep (unsigned int __seconds)
{
    __syscall1((long)SYSCALL_PTD(sleep), (long)__seconds);
    return 0;
}
