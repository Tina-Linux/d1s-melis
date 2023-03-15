/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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