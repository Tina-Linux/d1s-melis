#ifndef __HW_SPIN_LOCK__
#define __HW_SPIN_LOCK__

#define SPINLOCK_CLI_UART_LOCK_BIT (0)

void hal_hwspinlock_init(void);
int hal_hwspinlock_put(int num);
int hal_hwspinlock_get(int num);
int hal_hwspinlock_check_taken(int num);

extern void hal_hwspin_lock(int num);
extern void hal_hwspin_unlock(int num);

#endif
