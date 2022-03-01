#ifndef __ASM_GENERIC_PAGE_H
#define __ASM_GENERIC_PAGE_H

#include <rtdef.h>

#ifndef PAGE_SHIFT
#define PAGE_SHIFT  12
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE   RT_MM_PAGE_SIZE
#endif
#ifndef PAGE_MASK
#define PAGE_MASK   RT_MM_PAGE_MASK
#endif

#endif /* __ASM_GENERIC_PAGE_H */
