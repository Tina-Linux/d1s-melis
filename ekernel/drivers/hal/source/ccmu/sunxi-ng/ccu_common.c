// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2016 Maxime Ripard
 *
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 */
#include "ccu.h"
#include "ccu_common.h"
#include "ccu_gate.h"
#include "ccu_reset.h"
#include <hal_log.h>
#include <hal_timer.h>
#include <stdlib.h>
#ifdef CONFIG_COMPONENTS_PM
#include <pm_syscore.h>
#endif


/* FIXME: use udelay provided by OS */
static void __clk_udelay(u32 ns)
{
    u32 i;
    ns *= 100;
    for (i = 0; i < ns; i ++)
    {
        ;
    }
}

void ccu_helper_wait_for_lock(struct ccu_common *common, u32 lock)
{
    unsigned long addr;
    u32 reg, loop = 5000;

    if (!lock)
    {
        return;
    }

    if (common->features & CCU_FEATURE_LOCK_REG)
    {
        addr = common->base + common->lock_reg;
    }
    else
    {
        addr = common->base + common->reg;
    }

    while (--loop)
    {
        reg = readl(addr);
        if (reg & lock)
        {
            __clk_udelay(20);
            break;
        }
        __clk_udelay(1);
    }
    if (!loop)
    {
        hal_log_warn("ccu wait for lock failed\n");
    }
}

int ccu_common_init(unsigned long reg, const struct sunxi_ccu_desc *desc)
{
    struct ccu_reset *reset;
    int i;
    int ret;

    if (!desc)
    {
        return 0;
    }

    for (i = 0; i < desc->num_ccu_clks; i++)
    {
        struct ccu_common *cclk = desc->ccu_clks[i];

        if (!cclk)
        {
            continue;
        }

        cclk->base = reg;
    }

    for (i = 0; i < desc->hw_clks->num; i++)
    {
        struct clk_hw *hw = desc->hw_clks->hws[i];
        const char *name;

        if (!hw)
        {
            continue;
        }

        name = hw->init->name;
        hw->type = desc->clk_type;
        hw->id = i;
        ret = clk_hw_register(hw);
        if (ret)
        {
            printf("Couldn't register clock %d - %s\n", i, name);
            goto err_clk_unreg;
        }

    }

    reset = (struct ccu_reset *)malloc(sizeof(*reset));
    if (!reset)
    {
        hal_log_err("can't malloc reset struct!\n");
        ret = -1;
        goto err_clk_unreg;
    }

    memset(reset, 0, sizeof(*reset));
    reset->base = reg;
    reset->reset_map = desc->resets;
    reset->rcdev.ops = &ccu_reset_ops;
    reset->rcdev.type = desc->reset_type;
    reset->rcdev.nr_resets = desc->num_resets;

    ret = reset_control_register(&reset->rcdev);
    if (ret)
    {
        goto err_rst_unreg;
    }

    return ret;

err_rst_unreg:

    free(reset);

err_clk_unreg:
    while (i-- >= 0)
    {
        struct clk_hw *hw = desc->hw_clks->hws[i];

        clk_hw_unregister(hw);
    }

    return ret;
}

void set_reg(unsigned long addr, u32 val, u8 bw, u8 bs)
{
    u32 mask = (1UL << bw) - 1UL;
    u32 tmp = 0;

    tmp = readl(addr);
    tmp &= ~(mask << bs);

    writel(tmp | ((val & mask) << bs), addr);
}

void set_reg_key(unsigned long addr,
                 u32 key, u8 kbw, u8 kbs,
                 u32 val, u8 bw, u8 bs)
{
    u32 mask = (1UL << bw) - 1UL;
    u32 kmask = (1UL << kbw) - 1UL;
    u32 tmp = 0;

    tmp = readl(addr);
    tmp &= ~(mask << bs);

    writel(tmp | ((val & mask) << bs) | ((key & kmask) << kbs), addr);
}

#ifdef CONFIG_COMPONENTS_PM

static LIST_HEAD(ccu_reg_cache_list);

struct sunxi_clock_reg_cache {
	struct list_head node;
	void  *reg_base;
	struct ccu_reg_dump *rdump;
	unsigned int rd_num;
	const struct ccu_reg_dump *rsuspend;
	unsigned int rsuspend_num;
};

static void ccu_save(void  *base, struct ccu_reg_dump *rd,
		    unsigned int num_regs)
{
	for (; num_regs > 0; --num_regs, ++rd)
		rd->value = readl(base + rd->offset);
}

static void ccu_restore(void  *base,
			const struct ccu_reg_dump *rd,
			unsigned int num_regs)
{
	for (; num_regs > 0; --num_regs, ++rd)
		writel(rd->value, base + rd->offset);
}

static struct ccu_reg_dump *ccu_alloc_reg_dump(struct ccu_common **rdump,
					       unsigned long nr_rdump)
{
	struct ccu_reg_dump *rd;
	unsigned int i;

	rd = malloc(nr_rdump * sizeof(*rd));
	if (!rd)
		return NULL;
	memset(rd, 0, sizeof(*rd));
	for (i = 0; i < nr_rdump; ++i) {
		struct ccu_common *ccu_clks = rdump[i];

		rd[i].offset = ccu_clks->reg;
	}

	return rd;
}

int ccu_suspend(void *data, suspend_mode_t mode)
{
	struct sunxi_clock_reg_cache *reg_cache;

	list_for_each_entry(reg_cache, &ccu_reg_cache_list, node) {
		ccu_save(reg_cache->reg_base, reg_cache->rdump,
			 reg_cache->rd_num);
		ccu_restore(reg_cache->reg_base, reg_cache->rsuspend,
			    reg_cache->rsuspend_num);
	}
	printf("ccmu suspend end\n");
	return 0;
}

void ccu_resume(void *data, suspend_mode_t mode)
{
	struct sunxi_clock_reg_cache *reg_cache;

	list_for_each_entry(reg_cache, &ccu_reg_cache_list, node)
		ccu_restore(reg_cache->reg_base, reg_cache->rdump,
				reg_cache->rd_num);
	printf("ccmu resume end\n");
}

static struct syscore_ops ccmu_syscore_ops = {
	.name = "ccmu_syscore",
	.suspend = ccu_suspend,
	.resume = ccu_resume,
	.common_syscore = COMMON_SYSCORE,
};

void sunxi_ccu_sleep_init(void *reg_base,
			  struct ccu_common **rdump,
			  unsigned long nr_rdump,
			  const struct ccu_reg_dump *rsuspend,
			  unsigned long nr_rsuspend)
{
	struct sunxi_clock_reg_cache *reg_cache;

	reg_cache = malloc(sizeof(*reg_cache));
	if (!reg_cache) {
		hal_log_warn("could not allocate register reg_cache.\n");
		return;
	}
	memset(reg_cache, 0, sizeof(*reg_cache));
	reg_cache->rdump = ccu_alloc_reg_dump(rdump, nr_rdump);

	if (!reg_cache->rdump) {
		hal_log_warn("could not allocate register dump storage.\n");
		free(reg_cache);
		return;
	}

	if (list_empty(&ccu_reg_cache_list))
		pm_syscore_register(&ccmu_syscore_ops);

	reg_cache->reg_base = reg_base;
	reg_cache->rd_num = nr_rdump;
	reg_cache->rsuspend = rsuspend;
	reg_cache->rsuspend_num = nr_rsuspend;
	list_add_tail(&reg_cache->node, &ccu_reg_cache_list);
}
#else
void sunxi_ccu_sleep_init(void *reg_base,
			  struct ccu_common **rdump,
			  unsigned long nr_rdump,
			  const struct ccu_reg_dump *rsuspend,
			  unsigned long nr_rsuspend)
{}
#endif

