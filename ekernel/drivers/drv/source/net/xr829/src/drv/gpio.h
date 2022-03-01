#ifndef _XR_GPIO_H_
#define _XR_GPIO_H_

void xr_wlan_irq_disable(void);

void xr_wlan_irq_enable(void);
void xr_wlan_irq_subscribe(void *func);
void xr_wlan_irq_unsubscribe(void *func);

#endif /* _XR_GPIO_H_ */
