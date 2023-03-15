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
#include <stdint.h>
#include <stdbool.h>
#include <io.h>
#include <csr.h>

static uint32_t clint_time_hart_count;
static volatile void *clint_time_base;
static volatile uint64_t *clint_time_val;
static volatile uint64_t *clint_time_cmp;

unsigned int current_hartid(void)
{
	return (uint32_t)csr_read(mhartid);
}

#if __riscv_xlen != 32
static uint64_t clint_time_rd64(volatile uint64_t *addr)
{
	return readq_relaxed(addr);
}

static void clint_time_wr64(uint64_t value, volatile uint64_t *addr)
{
	writeq_relaxed(value, addr);
}
#endif

static uint64_t clint_time_rd32(volatile uint64_t *addr)
{
	uint32_t lo, hi;

	do {
		hi = readl_relaxed((uint32_t *)addr + 1);
		lo = readl_relaxed((uint32_t *)addr);
	} while (hi != readl_relaxed((uint32_t *)addr + 1));

	return ((uint64_t)hi << 32) | (uint64_t)lo;
}

static void clint_time_wr32(uint64_t value, volatile uint64_t *addr)
{
	uint32_t mask = -1U;

	writel_relaxed(value & mask, (void *)(addr));
	writel_relaxed(value >> 32, (void *)(addr) + 0x04);
}

static uint64_t (*clint_time_rd)(volatile uint64_t *addr) = clint_time_rd32;
static void (*clint_time_wr)(uint64_t value, volatile uint64_t *addr) = clint_time_wr32;

uint64_t clint_timer_value(void)
{
	/* Read CLINT Time Value */
	return clint_time_rd(clint_time_val);
}

void clint_timer_event_stop(void)
{
	uint32_t target_hart = current_hartid();

	if (clint_time_hart_count <= target_hart)
		return;

	/* Clear CLINT Time Compare */
	clint_time_wr(-1ULL, &clint_time_cmp[target_hart]);
}

void clint_timer_event_start(uint64_t next_event)
{
	uint32_t target_hart = current_hartid();

	if (clint_time_hart_count <= target_hart)
		return;

	/* Program CLINT Time Compare */
	clint_time_wr(next_event, &clint_time_cmp[target_hart]);
}

int clint_warm_timer_init(void)
{
	uint32_t target_hart = current_hartid();

	if (clint_time_hart_count <= target_hart || !clint_time_base)
		return -1;

	/* Clear CLINT Time Compare */
	clint_time_wr(-1ULL, &clint_time_cmp[target_hart]);

	return 0;
}

int clint_cold_timer_init(unsigned long base, uint32_t hart_count,
			  bool has_64bit_mmio)
{
	/* Figure-out CLINT Time register address */
	clint_time_hart_count		= hart_count;
	clint_time_base			= (void *)base;
	clint_time_val			= (uint64_t *)(clint_time_base + 0xbff8);
	clint_time_cmp			= (uint64_t *)(clint_time_base + 0x4000);

	/* Override read/write accessors for 64bit MMIO */
#if __riscv_xlen != 32
	if (has_64bit_mmio) {
		clint_time_rd		= clint_time_rd64;
		clint_time_wr		= clint_time_wr64;
	}
#endif

	return 0;
}
