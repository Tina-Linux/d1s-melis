#ifndef __DRV_CRYPTO_H__
#define __DRV_CRYPTO_H__

#include <sunxi_hal_ce.h>
//#define HWCRYPTO_LOCK

#define AES_BLOCK_SIZE		16

/*sunxi crypto cfg*/
#define ADDR_ALIGN_SIZE		0x4

#define AES_MODE_ECB	0
#define AES_MODE_CBC	1

#define CRYPTO_DIR_ENCRYPT	0
#define CRYPTO_DIR_DECRYPT	1

int aw_hw_crypto_device_init(void);

#endif /* __DRV_CRYPTO_H__ */
