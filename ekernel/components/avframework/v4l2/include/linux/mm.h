#ifndef _LINUX_MM_H
#define _LINUX_MM_H

#include <linux/kernel.h>

#ifndef PAGE_ALIGN
/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr) ALIGN(addr, RT_MM_PAGE_SIZE)
#endif


#endif /* _LINUX_MM_H */
