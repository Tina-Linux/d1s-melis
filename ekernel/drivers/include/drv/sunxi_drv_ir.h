#ifndef SUNXIKBD_DEBUG_H
#define SUNXIKBD_DEBUG_H

#include "../sunxi_hal_ir.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ADDR_NUM  (32)
#define RC_MAP_SUNXI "rc_map_sunxi"

struct sunxi_ir_data {
	struct rc_dev *rcdev;
	uint32_t ir_protocols;
	uint32_t ir_addr_cnt;
	uint32_t ir_addr[MAX_ADDR_NUM];
};

int init_sunxi_ir_map(void);
#ifdef CONFIG_SUNXI_KEYMAPPING_SUPPORT
int init_sunxi_ir_map_ext(void *addr, int num);
#endif

#ifdef __cplusplus
}
#endif

#endif
