#ifndef __USB_OS_PLATFORM_H__
#define __USB_OS_PLATFORM_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <script.h>

#include "hal_cfg.h"
#include "hal_osal.h"
#include "hal_clk.h"
#include "hal_gpio.h"
#include "hal_reset.h"

#include "sunxi_hal_usb.h"
#include "sunxi_hal_common.h"

#include "error.h"
#include "endian.h"
#include "bitops.h"

#ifdef CONFIG_KERNEL_FREERTOS
#include "usb_rtos.h"
#else
#include "usb_melis.h"
#endif

/*-------------------------------------------------------------*/

#define gfp_t		unsigned int
#define dma_addr_t	unsigned long

/*-------------------------------------------------------------*/

#define ENABLE_DMA_ALLOC_COHERENT		1

#define USB_DESC_MALLOC_ALIGN_SIZE		64
#define USB_OHCI_HCCA_MALLOC_ALIGN_SIZE		256
#define USB_EHCI_PERIODIC_MALLOC_ALIGN_SIZE	4096

/*-------------------------------------------------------------*/
/* usb hci moduble debug */

int hal_usb_hcd_debug_get(void);
#define EHCI_DEBUG_COLOR_OFF			"\033[0m"
#define EHCI_DEBUG_COLOR_GRAY			"\033[0;37m"
#define EHCI_DEBUG_OFFSET(x)			"\033[" #x "D\033[" #x "C"
#define EHCI_DEBUG_PRINTF(format, args...)                                     \
	do {                                                                   \
		if (hal_usb_hcd_debug_get())                                   \
			printf("[%s:%d]" "%s" format "\n\r",                   \
			    __func__, __LINE__, EHCI_DEBUG_OFFSET(40), ##args);\
	} while(0)

#define EHCI_DEBUG_LINE_BREAK()              \
	do {                                 \
		if (hal_usb_hcd_debug_get()) \
			printf("\n\r");       \
	} while (0)

/*-------------------------------------------------------------*/
/* SOC platform */

#if defined(CONFIG_ARCH_SUN8IW18P1)
#include "../platform/sun8iw18/usb_sun8iw18.h"
#endif
#if defined(CONFIG_ARCH_SUN8IW19)
#include "../platform/sun8iw19/usb_sun8iw19.h"
#endif
#if defined(CONFIG_ARCH_SUN8IW20) || defined(CONFIG_SOC_SUN20IW1)
#include "../platform/sun20iw1/usb_sun20iw1.h"
#endif
#if defined(CONFIG_ARCH_SUN20IW2)
#include "../platform/sun20iw2/usb_sun20iw2.h"
#endif

struct platform_usb_config {
	unsigned char *name;
	unsigned int pbase;
	unsigned int irq;
	unsigned int ohci_clk;
	unsigned int usb_clk;
	unsigned int usb_rst;
	unsigned int phy_clk;
	unsigned int phy_rst;
};

typedef struct usb_gpio_config {
	unsigned int valid;
	unsigned int gpio;
} usb_gpio_config_t;

struct platform_usb_port_config {
	unsigned int enable;
	unsigned int port_type;
	unsigned int detect_type;
	unsigned int detect_mode;
	usb_gpio_config_t id; /* usb id pin info */
	usb_gpio_config_t det_vbus; /* usb det_vbus pin info */
	usb_gpio_config_t drv_vbus[USB_MAX_CONTROLLER_COUNT]; /* usb drv_vbus pin info */
};

struct platform_usb_config *platform_get_ehci_table(void);
struct platform_usb_config *platform_get_ohci_table(void);
struct platform_usb_config *platform_get_otg_table(void);
struct platform_usb_port_config *platform_get_port_table(void);

/*-------------------------------------------------------------*/
/* USB dma alloc/free */

static inline void *usb_dma_malloc(uint32_t size, dma_addr_t *p_addr, uint32_t align)
{
	void *v_addr = NULL;

	/* should be 32 byte-align for QH and QTD */
#ifdef CONFIG_DMA_COHERENT_HEAP
	v_addr = dma_coherent_heap_alloc_align(size, align);
#else
	v_addr = dma_alloc_coherent_align(size, align);
#endif
	if (v_addr) {
		*p_addr = __va_to_pa((unsigned long)v_addr);
		memset(v_addr, 0, size);
		return v_addr;
	}

	hal_log_err("hal_malloc failed\n");
	return NULL;
}

static inline void usb_dma_free(void *v_addr, dma_addr_t p_addr)
{
	if (v_addr) {
#ifdef CONFIG_DMA_COHERENT_HEAP
		dma_coherent_heap_free_align(v_addr);
#else
		dma_free_coherent_align(v_addr);
#endif
	}
}


#endif //__USB_OS_PLATFORM_H__
