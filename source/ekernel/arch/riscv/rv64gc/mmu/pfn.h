#ifndef __PFN_H_
#define __PFN_H_

#ifndef __ASSEMBLY__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint64_t val;
} pfn_t;
#endif

#define PFN_ALIGN(x)    (((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)
#define PFN_UP(x)   (((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x) ((x) >> PAGE_SHIFT)
#define PFN_PHYS(x) ((phys_addr_t)(x) << PAGE_SHIFT)
#define PHYS_PFN(x) ((unsigned long)((x) >> PAGE_SHIFT))

#endif
