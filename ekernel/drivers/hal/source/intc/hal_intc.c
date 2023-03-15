#include <stdint.h>
#include <hal_interrupt.h>
#include <hal_intc.h>

#include "intc.h"

#ifdef CONFIG_COMPONENTS_PM
#include "pm_syscore.h"
extern struct syscore_ops g_intc_pm_ops;
#endif

static hal_irqreturn_t intc_dispatcher(void *arg)
{
	interrupt_entry();
	return HAL_IRQ_OK;
}

hal_intc_status_t hal_intc_init(uint32_t irq_no)
{
	hal_intc_status_t ret = HAL_INTC_STATUS_OK;

	/* initialize interrupt manager */
	interrupt_init();

	/* register intc main dispatcher handler */
	if (hal_request_irq((int32_t)irq_no, intc_dispatcher, "intc", NULL) < 0) {
		ret = HAL_INTC_STATUS_FAIL;
		goto end;
	}

#ifdef CONFIG_COMPONENTS_PM
	int pm_ret;
	pm_ret = pm_syscore_register(&g_intc_pm_ops);
	if (pm_ret)
		printf("WARNING: INTC PM syscore ops registers failed\n");
#endif

	hal_enable_irq(irq_no);

end:
	return ret;
}
