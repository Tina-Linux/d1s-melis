#ifndef _XR_SDIO_H_
#define _XR_SDIO_H_

#ifdef __MELIS__

#include "xr_types.h"
#include "arch/suniv/bsp/bsp_sdc.h"
#include "arch/suniv/bsp/bsp_sd.h"
struct mmc_card;
typedef struct mmc_card xr_drv_sdio_t;
unsigned char sdio_readb(xr_drv_sdio_t *card, unsigned int func_num, unsigned int addr, int *err_ret);
void sdio_writeb(xr_drv_sdio_t *card, unsigned int func_num, const unsigned char b, unsigned int addr, int *err_ret);
int sdio_memcpy_fromio(xr_drv_sdio_t *card, unsigned int func_num, void *dst, unsigned int addr, int count);
int sdio_memcpy_toio(xr_drv_sdio_t *card, unsigned int func_num, unsigned int addr, const void *src, int count);
int sdio_set_block_size(xr_drv_sdio_t *card, unsigned int fn_num, unsigned blksz);
unsigned int sdio_align_size(xr_drv_sdio_t *func, unsigned int sz);
#define sdio_lock(l) do {} while(0)
#define sdio_unlock(l) do {} while(0)
struct mmc_card *BSP_SDC_GetCard(__u32 card_no);

#elif (defined(__CONFIG_ARCH_STM32))

#include "sdio_api.h"

#elif (defined(__CONFIG_CHIP_XRADIO))
#include "driver/chip/sdmmc/hal_sdhost.h"
#include "driver/chip/sdmmc/sdio.h"
#include "driver/chip/sdmmc/sdmmc.h"

struct mmc_card;
typedef struct mmc_card xr_drv_sdio_t;

#elif (defined(CONFIG_OS_RTTHREAD))
#include "drivers/sdio.h"

#define FN(x)                           (x)
#define FN0                             FN(0)
#define FN1                             FN(1)
#define SDIO_CCCR_IENx                  0x04    /* Function/Master Interrupt Enable */

#define sdio_lock(l) do {} while(0)
#define sdio_unlock(l) do {} while(0)

typedef struct rt_mmcsd_card xr_drv_sdio_t;

#define sdio_claim_host(host)
#define sdio_release_host(host)

unsigned char sdio_readb(xr_drv_sdio_t *card, unsigned int func_num, unsigned int addr, int *err_ret);
void sdio_writeb(xr_drv_sdio_t *card, unsigned int func_num, const unsigned char b, unsigned int addr, int *err_ret);
int sdio_memcpy_fromio(xr_drv_sdio_t *card, unsigned int func_num, void *dst, unsigned int addr, int count);
int sdio_memcpy_toio(xr_drv_sdio_t *card, unsigned int func_num, unsigned int addr, const void *src, int count);
unsigned int sdio_align_size(xr_drv_sdio_t *card, unsigned int sz);

#define xr_sdio_set_block_size(c, fn, sz) sdio_set_block_size(c->sdio_function[fn], sz)
#define xr_sdio_enable_func(c, fn) sdio_enable_func(c->sdio_function[fn])
#define xr_sdio_disable_func(c, fn) sdio_disable_func(c->sdio_function[fn])
#define xr_sdio_align_size(c, sz) sdio_align_size(NULL, sz)



#elif (defined(CONFIG_OS_YUNOS))
//#include "sdmmc/card.h"
//#include "sdmmc/sdio.h"
struct mmc_card;
struct sdio_func;
typedef struct mmc_card xr_drv_sdio_t;

#define FN(x)                           (x)
#define FN0                             FN(0)
#define FN1                             FN(1)
#define FN2                             FN(2)
#define FN3                             FN(3)
#define FN4                             FN(4)
#define FN5                             FN(5)
#define FN6                             FN(6)
#define FN7                             FN(7)
#define SDIO_CCCR_IENx                  0x04    /* Function/Master Interrupt Enable */

#define sdio_lock(l) do {} while(0)
#define sdio_unlock(l) do {} while(0)
/***************************** Alios Function ********************************/
extern int sdio_set_block_size(struct mmc_card *card, unsigned int fn_num, unsigned int blksz);
extern int sdio_enable_func(struct mmc_card *card, unsigned int func_num);
extern int sdio_disable_func(struct mmc_card *card, unsigned int func_num);
//unsigned int sdio_align_size(struct sdio_func *func, unsigned int sz);
extern unsigned char
sdio_readb(struct mmc_card *card, unsigned int func_num, unsigned int addr,
           int *err_ret);
extern void
sdio_writeb(struct mmc_card *card, unsigned int func_num, const unsigned char b,
            unsigned int addr, int  *err_ret);
extern int
sdio_memcpy_toio(struct mmc_card *card, unsigned int func_num, unsigned int addr,
                 const void *src, int count);
extern int
sdio_memcpy_fromio(struct mmc_card *card, unsigned int func_num, void *dst,
                   unsigned int addr, int count);

inline static unsigned int sdio_align_size(xr_drv_sdio_t *func, unsigned int sz)
{
	/* Do a first check with the controller, in case it
	 * wants to increase the size up to a point where it
	 * might need more than one block.
	 */
	if (sz && sz & 3) {
		sz &= ~0x03;
		sz += 0x04;
	}

	/* The controller is simply incapable of transferring the size
	 * we want in decent manner, so just return the original size.
	 */
	return sz;
}


/*
 * xr_sdio_set_block_size - xradio set the block size of an SDIO function
 * @c: xr_drv_sdio_t
 * @fn:
 * @sz: new block size
 */
#define xr_sdio_set_block_size(c, fn, sz) sdio_set_block_size(c, fn, sz)

#define xr_sdio_enable_func(c, fn)	sdio_enable_func(c, fn)

#define xr_sdio_disable_func(c, fn)	sdio_disable_func(c, fn)

#define xr_sdio_align_size(c, sz) sdio_align_size(c, sz)

xr_drv_sdio_t *xradio_sdio_detect(unsigned int id, int enable);
void xradio_sdio_deinit(xr_drv_sdio_t *card);

#elif (defined(CONFIG_OS_NUTTX))
#include <sdmmc/hal_sdhost.h>
#include <sdmmc/card.h>
#include <sdmmc/sdio.h>
#include <sdmmc/sdmmc.h>
#include <sdmmc/sys/sys_debug.h>
struct mmc_card;
struct sdio_func;
typedef struct mmc_card xr_drv_sdio_t;
#define CONFIG_SDC_ID 1

#define sdio_lock(l) sdio_claim_host(l)
#define sdio_unlock(l) sdio_release_host(l)
inline static unsigned int nx_sdio_align_size(xr_drv_sdio_t *func, unsigned int sz)
{
	/* Do a first check with the controller, in case it
	 * wants to increase the size up to a point where it
	 * might need more than one block.
	 */
	if (sz && sz & 3) {
		sz &= ~0x03;
		sz += 0x04;
	}

	/* The controller is simply incapable of transferring the size
	 * we want in decent manner, so just return the original size.
	 */
	return sz;
}
/*
 * xr_sdio_set_block_size - xradio set the block size of an SDIO function
 * @c: xr_drv_sdio_t
 * @fn:
 * @sz: new block size
 */
#define xr_sdio_set_block_size(c, fn, sz) sdio_set_block_size(c, fn, sz)

#define xr_sdio_enable_func(c, fn)	sdio_enable_func(c, fn)

#define xr_sdio_disable_func(c, fn)	sdio_disable_func(c, fn)

#define xr_sdio_align_size nx_sdio_align_size

xr_drv_sdio_t *xradio_sdio_detect(unsigned int id, int enable);
void xradio_sdio_deinit(xr_drv_sdio_t *card);


#endif /* __MELIS__ */
#ifndef CONFIG_OS_NUTTX
extern void SDIO_PowerON(xr_drv_sdio_t *card);
extern void SDIO_PowerOFF(xr_drv_sdio_t *card);
extern int SDIO_Initialize_Cards(xr_drv_sdio_t *card);
extern int SDIO_DeInitialize_Cards(xr_drv_sdio_t *card);
#endif
#endif /* _XR_SDIO_H_ */
