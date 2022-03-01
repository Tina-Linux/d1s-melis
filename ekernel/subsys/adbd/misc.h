/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
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
/*  
#ifndef __ADB_MISC_H
#define __ADB_MISC_H
*/
#ifndef __MISC_H
#define __MISC_H
#include <pthread.h>

typedef pthread_t	adb_thread_t;
typedef void* (*adb_thread_func_t)(void *arg);
#define ADB_THREAD_STACK_SIZE	(8*1024)

static inline int adb_thread_create(adb_thread_t *tid, adb_thread_func_t start,
					const char *name, void *arg, int priority)
{
	int ret;
	pthread_attr_t attr;
	struct sched_param sched;

	memset(&sched, 0, sizeof(sched));
	sched.sched_priority = priority;
	pthread_attr_init(&attr);
	pthread_attr_setschedparam(&attr, &sched);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(&attr, ADB_THREAD_STACK_SIZE);
	ret = pthread_create(tid, &attr, start, arg);
	if (ret < 0)
		return ret;
	pthread_setname_np(*tid, name);
	return ret;
}

static inline int adb_thread_create_detach(adb_thread_t *tid, adb_thread_func_t start,
					const char *name, void *arg, int priority)
{
	int ret;
	pthread_attr_t attr;
	struct sched_param sched;

	sched.sched_priority = priority;
	pthread_attr_init(&attr);
	pthread_attr_setschedparam(&attr, &sched);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, ADB_THREAD_STACK_SIZE);
	ret = pthread_create(tid, &attr, start, arg);
	if (ret < 0)
		return ret;
	pthread_setname_np(*tid, name);
	return ret;
}

static inline int adb_thread_wait(adb_thread_t tid, void **retval)
{
	return pthread_join(tid, retval);
}

int adb_thread_wait_timeout(adb_thread_t tid, void **retval, int timeout);


typedef struct adb_schd adb_schd;
typedef struct adb_schd* adb_schd_t;

struct adb_schd {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int event;
	int mask;
};

#define ADB_SCHD_READ		(0x0001)
#define ADB_SCHD_WRITE		(0x0002)
#define ADB_SCHD_ERROR		(0x0004)
#define ADB_SCHD_TIMEOUT	(0x0008)

adb_schd_t adb_schd_init(int mask);
int adb_schd_wait(adb_schd_t schd);
void adb_schd_wakeup(adb_schd_t schd, int event);
void adb_schd_release(adb_schd_t schd);

#endif
void record_alive_thread_add(const char *name);
void record_alive_thread_print(void);
