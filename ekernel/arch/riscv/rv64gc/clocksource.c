#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <rtthread.h>
#include <ktimer.h>
#include <timex.h>

#define TIMER_FREQ    (24000000)

static uint32_t arch_timer_rate;

static inline uint32_t arch_timer_get_cntfrq(void)
{
    uint32_t val = TIMER_FREQ;
    return val;
}

static inline uint64_t arch_counter_get_cntpct(void)
{
    uint64_t cval;

    cval = get_cycles64();
    return cval;
}

uint64_t (*arch_timer_read_counter)(void) = arch_counter_get_cntpct;
static cycle_t arch_counter_read(struct clocksource *cs)
{
    return arch_timer_read_counter();
}

static struct clocksource arch_timer_clocksource =
{
    .name   = "arch_sys_counter",
    .read   = arch_counter_read,
    .mask   = CLOCKSOURCE_MASK(56),
};

int64_t ktime_get(void)
{
    struct clocksource *cs = &arch_timer_clocksource;

    double ns_per_ticks = cs->mult / (1 << cs->shift);

    ns_per_ticks = NS_PER_TICK;
    uint64_t arch_counter = cs->read(cs);

    return arch_counter * ns_per_ticks;
}

int do_gettimeofday(struct timespec64 *ts)
{
    if (ts == NULL)
    {
        return -1;
    }

    int64_t nsecs = ktime_get();

    ts->tv_sec  = nsecs / NSEC_PER_SEC;
    ts->tv_nsec = nsecs % NSEC_PER_SEC;

    return 0;
}

void udelay(unsigned int us)
{
    uint64_t start, target;
    uint64_t frequency;

    frequency = arch_timer_get_cntfrq();
    start = arch_counter_get_cntpct();
    target = frequency / 1000000ULL * us;

    while (arch_counter_get_cntpct() - start <= target) ;
}

int usleep(unsigned int usecs)
{
    int tickDiv = 1000 * (1000 / CONFIG_HZ);
    int ticks   = usecs / tickDiv;
    int msecs   = usecs % tickDiv;

    if (ticks)
    {
        rt_thread_delay(ticks);
    }
    if (msecs)
    {
        udelay(usecs);
    }
    return 0;
}

int msleep(unsigned int msecs)
{
    usleep(msecs*1000);
    return 0;
}

void sleep(int seconds)
{
    msleep(seconds * CONFIG_HZ * 1000);
}

void timekeeping_init(void)
{
    struct clocksource *cs = &arch_timer_clocksource;

    cs->shift = 24;
    cs->mult = 699050667;
    cs->arch_timer_rate = arch_timer_get_cntfrq();
    if (cs->arch_timer_rate == 0)
    {
        cs->arch_timer_rate = TIMER_FREQ;
    }

    return;
}

void timestamp(char *tag)
{
    struct timespec64 ts;
    do_gettimeofday(&ts);

    printf("[TSM]: %*.*s]:sec %ld, nsec %d.\n", 12, 12, tag, ts.tv_sec, ts.tv_nsec);
}

