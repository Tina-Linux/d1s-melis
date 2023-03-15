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
#include <stdio.h>
#include "interrupt.h"
#include <log.h>

static unsigned int timer_reload_val;
uint32_t g_irq_arg[3] = { 0 };

#define ARCH_TIMER_CTRL_ENABLE          (1 << 0)
#define ARCH_TIMER_CTRL_IT_MASK         (1 << 1)
#define ARCH_TIMER_CTRL_IT_STAT         (1 << 2)

static unsigned int read_cntfrq(void)
{
    unsigned int cntfrq;
    asm volatile("mrc  p15, 0, %0, c14, c0, 0" : "=r"(cntfrq));
    return cntfrq;
}

static void write_cntp_tval(unsigned int cntp_tval)
{
    asm volatile("mcr p15, 0, %0, c14, c2, 0" :: "r"(cntp_tval));
}

static void write_cntp_ctl(unsigned int cntp_ctl)
{
    asm volatile("mcr p15, 0, %0, c14, c2, 1" ::"r"(cntp_ctl));
    asm volatile("isb");
}

static unsigned int read_cntp_ctl(void)
{
    unsigned int val = 0;
    asm volatile("mrc p15, 0, %0, c14, c2, 1" : "=r"(val));
    asm volatile("isb");
    return val;
}

static void set_next_event(unsigned int reload)
{
    unsigned int ctl;

    ctl = read_cntp_ctl();
    ctl |= ARCH_TIMER_CTRL_ENABLE;
    ctl &= ~ARCH_TIMER_CTRL_IT_MASK;
    write_cntp_tval(reload);
    write_cntp_ctl(ctl);
}

static hal_irqreturn_t platform_tick(void *para)
{
    unsigned int ctl;

    ctl = read_cntp_ctl();

    if (ctl & ARCH_TIMER_CTRL_IT_STAT)
    {
        //mask now.
        ctl |= ARCH_TIMER_CTRL_IT_MASK;
        write_cntp_ctl(ctl); /* disable timer */

        void awos_arch_tick_increase(void);
        awos_arch_tick_increase();

        set_next_event(timer_reload_val);
        return HAL_IRQ_OK;
    }

    return HAL_IRQ_OK;
}

void arm_generic_timer_init(int irq, unsigned int freq_override)
{
    unsigned int cntfrq = 0, *rate = NULL;
    int ret, len, irqnum;
    uint32_t intsize = 0;

    {
        //ns:0 secuirty mode. ns:1 normal mode
        int ns = 0;

        if (freq_override == 0)
        {
            cntfrq = read_cntfrq(); /* default: 24MHz */
            if (!cntfrq)
            {
                cntfrq = 24000000;
            }
        }
        else
        {
            cntfrq = freq_override;
        }

        //normal world, irqnum is 30
        if (ns == 1)
        {
            irqnum = 30;
        }
        else
        {
            irqnum = 29;
        }
    }

    timer_reload_val = cntfrq / CONFIG_HZ;
    set_next_event(timer_reload_val);

    ret = hal_request_irq(irqnum, platform_tick, "tick", NULL);
    if (ret != 0)
    {
        __err("fatal error, request irq failure.");
        while (1);
        return;
    }
}
