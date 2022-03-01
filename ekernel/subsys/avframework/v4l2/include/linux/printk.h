#ifndef __KERNEL_PRINTK__
#define __KERNEL_PRINTK__

#include <melis/log.h>

//#include <stdarg.h>
//#include <linux/init.h>
#include <linux/kern_levels.h>
//#include <linux/linkage.h>
//#include <linux/cache.h>

/*
 * Like KERN_CONT, pr_cont() should only be used when continuing
 * a line with no newline ('\n') enclosed. Otherwise it defaults
 * back to KERN_DEFAULT.
 */
/*
#define pr_cont(fmt, ...) \
	printk(KERN_CONT fmt, ##__VA_ARGS__)
*/

#define pr_cont pr_debug

#endif
