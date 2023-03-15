#include "xr_drv_mem.h"
#include <stdio.h>

#if XR_DRV_MEM_TRACE

#define HEAP_MEM_DBG_ENABLE 	0
#define HEAP_MEM_DBG_MIN_SIZE	100
#define HEAP_MEM_MAX_CNT	64
#define HEAP_SYSLOG 		printf
#define HEAP_MEM_ERR		printf

#if HEAP_MEM_DBG_ENABLE
#define HEAP_MEM_DBG(fmt, arg...)	HEAP_SYSLOG("[xrdrv heap] "fmt, ##arg)
#else
#define HEAP_MEM_DBG(...)		do{} while(0)
#endif


struct heap_mem {
	void *ptr;
	size_t size;
};

static struct heap_mem g_mem[HEAP_MEM_MAX_CNT];

static int g_mem_entry_cnt = 0;
static int g_mem_entry_cnt_max = 0;
static int g_mem_empty_idx = 0; // beginning idx to do the search for new one

static size_t g_mem_sum = 0;
static size_t g_mem_sum_max = 0;

void xr_drv_heap_info(void)
{
	HEAP_SYSLOG("<<< lmac driver heap info >>>\n"
		    "g_mem_sum       %u (%u KB)\n"
		    "g_mem_sum_max   %u (%u KB)\n"
		    "g_mem_entry_cnt %u, max %u\n",
		    g_mem_sum, g_mem_sum / 1024,
		    g_mem_sum_max, g_mem_sum_max / 1024,
		    g_mem_entry_cnt, g_mem_entry_cnt_max);

	int i, j = 0;
	for (i = 0; i < HEAP_MEM_MAX_CNT; ++i) {
		if (g_mem[i].ptr != 0) {
			HEAP_SYSLOG("%03d. %03d, %p, %u\n",
				    ++j, i, g_mem[i].ptr, g_mem[i].size);
		}
	}
}

void *xr_drv_malloc(size_t size)
{
	void *ptr = malloc(size);

	if (ptr) {
		if (size > HEAP_MEM_DBG_MIN_SIZE) {
			HEAP_MEM_DBG("m (%p, %u)\n", ptr, size);
		}
		int i;
		for (i = g_mem_empty_idx; i < HEAP_MEM_MAX_CNT; ++i) {
			if (g_mem[i].ptr == 0) {
				/* add new entry */
				g_mem[i].ptr = ptr;
				g_mem[i].size = size;
				g_mem_entry_cnt++;
				g_mem_empty_idx = i + 1;
				g_mem_sum += size;
				if (g_mem_sum > g_mem_sum_max)
					g_mem_sum_max = g_mem_sum;
				if (g_mem_entry_cnt > g_mem_entry_cnt_max)
					g_mem_entry_cnt_max = g_mem_entry_cnt;
				break;
			}
		}
		if (i >= HEAP_MEM_MAX_CNT) {
			HEAP_MEM_ERR("heap memory count exceed %d\n",
				     HEAP_MEM_MAX_CNT);
		}
	} else {
		HEAP_MEM_ERR("heap memory exhausted!\n");
	}

	return ptr;
}

void xr_drv_free(void *ptr)
{
	if (ptr) {
		int i;
		for (i = 0; i < HEAP_MEM_MAX_CNT; ++i) {
			if (g_mem[i].ptr != ptr)
				continue;

			/* delete the old entry */
			if (g_mem[i].size > HEAP_MEM_DBG_MIN_SIZE) {
				HEAP_MEM_DBG("f (%p, %u)\n",
					     g_mem[i].ptr, g_mem[i].size);
			}
			g_mem_sum -= g_mem[i].size;
			g_mem[i].ptr = 0;
			g_mem[i].size = 0;
			g_mem_entry_cnt--;
			if (i < g_mem_empty_idx)
				g_mem_empty_idx = i;
			break;
		}
		if (i >= HEAP_MEM_MAX_CNT) {
			HEAP_MEM_ERR("heap memory entry (%p) missed\n", ptr);
		}
	}

	free(ptr);
}

#else /* XR_DRV_MEM_TRACE */

void xr_drv_heap_info(void)
{
	printf("XR_DRV_MEM_TRACE is false\n");
}

#endif /* XR_DRV_MEM_TRACE */
