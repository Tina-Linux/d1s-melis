#include <stdio.h>

#ifndef _ASM_GENERIC_BUG_H
#define _ASM_GENERIC_BUG_H

#ifndef BUG_ON
#define BUG_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	if(__ret_warn_on)                                               \
		printf("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
	__ret_warn_on;							\
})
#endif

#ifndef WARN_ON
#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	__ret_warn_on;							\
})
#endif

#ifndef WARN
#define WARN(condition, format...) ({					\
	int __ret_warn_on = !!(condition);				\
	__ret_warn_on;							\
})
#endif

#endif
