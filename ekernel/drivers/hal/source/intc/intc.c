/*
*********************************************************************************************************
*                                                AR100 SYSTEM
*                                     AR100 Software System Develop Kits
*                                              interrupt  module
*
*                                    (c) Copyright 2012-2016, Sunny China
*                                             All Rights Reserved
*
* File    : intc.c
* By      : Sunny
* Version : v1.0
* Date    : 2012-5-3
* Descript: interrupt controller module.
* Update  : date                auther      ver     notes
*           2012-5-3 13:25:40   Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include "intc_i.h"
#include "platform-intc.h"

struct intc_regs *pintc_regs;

#ifdef CONFIG_COMPONENTS_PM
#include "pm_syscore.h"

#define INTC_BASE_ADDR_REG_ADDR	(SUNXI_R_INTC_PBASE + 0x04)
#define INTC_NMI_CTRL_REG_ADDR	(SUNXI_R_INTC_PBASE + 0x0C)

#define INTC_PENDING_REG_BASE	(SUNXI_R_INTC_PBASE + 0x10)
#define INTC_ENABLE_REG_BASE	(SUNXI_R_INTC_PBASE + 0x40)
#define INTC_MASK_REG_BASE	(SUNXI_R_INTC_PBASE + 0x50)
#define INTC_FORCE_REG_BASE	(SUNXI_R_INTC_PBASE + 0x70)
#define INTC_PRIORITY_REG_BASE	(SUNXI_R_INTC_PBASE + 0x80)
#define INTC_GROUP_CONFIG_REG_BASE	(SUNXI_R_INTC_PBASE + 0xC0)


#define INTC_GENERAL_CTRL_REG_NUM 4
#define INTC_PRIORITY_REG_NUM 8
//static uint32_t s_intc_vector_offset_reg_bak;
static uint32_t s_intc_base_addr_reg_bak;
static uint32_t s_intc_nmi_ctrl_reg_bak;

static uint32_t s_intc_pending_reg_bak[INTC_GENERAL_CTRL_REG_NUM];
static uint32_t s_intc_enable_reg_bak[INTC_GENERAL_CTRL_REG_NUM];
static uint32_t s_intc_mask_reg_bak[INTC_GENERAL_CTRL_REG_NUM];
static uint32_t s_intc_force_reg_bak[INTC_GENERAL_CTRL_REG_NUM];
static uint32_t s_intc_priority_reg_bak[INTC_PRIORITY_REG_NUM];
static uint32_t s_intc_group_config_reg_bak[INTC_GENERAL_CTRL_REG_NUM];

static int intc_suspend(void *data, suspend_mode_t mode)
{
	int i;

	s_intc_base_addr_reg_bak = readl(INTC_BASE_ADDR_REG_ADDR);
	s_intc_nmi_ctrl_reg_bak = readl(INTC_NMI_CTRL_REG_ADDR);

	for (i = 0; i < INTC_GENERAL_CTRL_REG_NUM; i++)
	{
		s_intc_pending_reg_bak[i] = readl(INTC_PENDING_REG_BASE + i * 4);
		s_intc_enable_reg_bak[i] = readl(INTC_ENABLE_REG_BASE + i * 4);
		s_intc_mask_reg_bak[i] = readl(INTC_MASK_REG_BASE + i * 4);
		s_intc_force_reg_bak[i] = readl(INTC_FORCE_REG_BASE + i * 4);
		s_intc_group_config_reg_bak[i] = readl(INTC_GROUP_CONFIG_REG_BASE + i * 4);
	}

	for (i = 0; i < INTC_PRIORITY_REG_NUM; i++)
	{
		s_intc_priority_reg_bak[i] = readl(INTC_PRIORITY_REG_BASE + i * 4);
	}

	return 0;
}

static void intc_resume(void *data, suspend_mode_t mode)
{
	int i;

	writel(s_intc_base_addr_reg_bak, INTC_BASE_ADDR_REG_ADDR);
	writel(s_intc_nmi_ctrl_reg_bak, INTC_NMI_CTRL_REG_ADDR);

	for (i = 0; i < INTC_GENERAL_CTRL_REG_NUM; i++)
	{
		writel(s_intc_pending_reg_bak[i], INTC_PENDING_REG_BASE + i * 4);
		writel(s_intc_enable_reg_bak[i], INTC_ENABLE_REG_BASE + i * 4);
		writel(s_intc_mask_reg_bak[i], INTC_MASK_REG_BASE + i * 4);
		writel(s_intc_force_reg_bak[i], INTC_FORCE_REG_BASE + i * 4);
		writel(s_intc_group_config_reg_bak[i], INTC_GROUP_CONFIG_REG_BASE + i * 4);
	}

	for (i = 0; i < INTC_PRIORITY_REG_NUM; i++)
	{
		s_intc_priority_reg_bak[i] = readl(INTC_PRIORITY_REG_BASE + i * 4);
	}
}

struct syscore_ops g_intc_pm_ops = {
	.name = "intc_syscore_ops",
	.suspend = intc_suspend,
	.resume = intc_resume,
};
#endif /* CONFIG_COMPONENTS_PM */

/*
*********************************************************************************************************
*                                           INTERRUPT INIT
*
* Description:  initialize interrupt.
*
* Arguments  :  none.
*
* Returns    :  OK if initialize succeeded, others if failed.
*
* Note       :
*********************************************************************************************************
*/
s32 intc_init(void)
{
	pintc_regs = (struct intc_regs *)(SUNXI_R_INTC_PBASE);

	/*initialize interrupt controller */
	pintc_regs->enable = 0x0;
	pintc_regs->mask = 0x0;
	pintc_regs->pending = 0xffffffff;

	pintc_regs->enable1 = 0x0;
	pintc_regs->mask1 = 0x0;
	pintc_regs->pending1 = 0xffffffff;

	pintc_regs->enable2 = 0x0;
	pintc_regs->mask2 = 0x0;
	pintc_regs->pending2 = 0xffffffff;
	return OK;
}

/*
*********************************************************************************************************
*                                         INTERRUPT EXIT
*
* Description:  exit interrupt.
*
* Arguments  :  none.
*
* Returns    :  OK if exit succeeded, others if failed.
*
* Note       :
*********************************************************************************************************
*/
s32 intc_exit(void)
{
	pintc_regs = NULL;

	return OK;
}

/*
*********************************************************************************************************
*                                           ENABLE INTERRUPT
*
* Description:  enable a specific interrupt.
*
* Arguments  :  intno   : the source number of interrupt to which we want to enable.
*
* Returns    :  OK if enable interrupt succeeded, others if failed.
*
* Note       :
*********************************************************************************************************
*/
s32 intc_enable_interrupt(u32 intno)
{
	/*intno can't beyond then IRQ_SOURCE_MAX */
	/* ASSERT(intno < IRQ_SOUCE_MAX); */

	/*
	 * NMI interrupt should clear before enable.
	 * by sunny at 2012-6-12 19:30:22.
	 */
	if (intno == SUNXI_RINTC_IRQ_NMI) {
		pintc_regs->pending = (1 << intno);
	}

	/*enable interrupt which number is intno */
	if (intno <= 31)
		pintc_regs->enable |= (1 << intno);
	else if (intno > 31 && intno <= 63)
		pintc_regs->enable1 |= (1 << (intno - 32));
	else
		pintc_regs->enable2 |= (1 << (intno - 64));

	return OK;
}

/*
*********************************************************************************************************
*                                           DISABLE INTERRUPT
*
* Description:  disable a specific interrupt.
*
* Arguments  :  intno  : the source number of interrupt which we want to disable.
*
* Returns    :  OK if disable interrupt succeeded, others if failed.
*
* Note       :
*********************************************************************************************************
*/
s32 intc_disable_interrupt(u32 intno)
{
	/*intno can't beyond then IRQ_SOURCE_MAX */
	/* ASSERT(intno < IRQ_SOUCE_MAX); */

	/*enable interrupt which number is intno */
	if (intno <= 31)
		pintc_regs->enable &= ~(1 << intno);
	else if (intno > 31 && intno <= 63)
		pintc_regs->enable1 &= ~(1 << (intno - 32));
	else
		pintc_regs->enable2 &= ~(1 << (intno - 64));

	return OK;
}

/*
*********************************************************************************************************
*                                   GET CURRENT INTERRUPT
*
* Description: get the source number of current interrupt.
*
* Arguments  : none.
*
* Returns    : the source number of current interrupt.
*
* Note       :
*********************************************************************************************************
*/
u32 intc_get_current_interrupt(void)
{
	volatile u32 interrupt;

	interrupt = (u32) ((pintc_regs->vector) >> 2);

	return interrupt;
}

s32 intc_set_mask(u32 intno, u32 mask)
{
	/* intno can't beyond then IRQ_SOURCE_MAX */
	/* ASSERT(intno < IRQ_SOUCE_MAX); */

	/* enable interrupt which number is intno */
	if (intno <= 31) {
		pintc_regs->mask &= ~(1 << intno);
		pintc_regs->mask |= (mask << intno);
	} else if (intno > 31 && intno <= 63) {
		pintc_regs->mask1 &= ~(1 << (intno - 32));
		pintc_regs->mask1 |= (mask << intno);
	} else {
		pintc_regs->mask2 &= ~(1 << (intno - 64));
		pintc_regs->mask2 |= (mask << intno);
	}

	return OK;
}

s32 intc_set_group_config(u32 grp_irq_num, u32 mask)
{
	if (grp_irq_num <= 31) {
		pintc_regs->group_config0 &= ~(1 << grp_irq_num);
		pintc_regs->group_config0 |= (mask << grp_irq_num);
	} else if (grp_irq_num > 31 && grp_irq_num <= 63) {
		pintc_regs->group_config1 &= ~(1 << (grp_irq_num - 32));
		pintc_regs->group_config1 |= (mask << grp_irq_num);
	} else if (grp_irq_num > 63 && grp_irq_num <= 95) {
		pintc_regs->group_config2 &= ~(1 << (grp_irq_num - 64));
		pintc_regs->group_config2 |= (mask << grp_irq_num);
	} else if (grp_irq_num > 95 && grp_irq_num <= 127) {
		pintc_regs->group_config3 &= ~(1 << (grp_irq_num - 96));
		pintc_regs->group_config3 |= (mask << grp_irq_num);
	}

	return OK;
}
