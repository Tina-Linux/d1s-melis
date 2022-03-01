#ifndef SUNXI_DRV_SPINOR_H
#define SUNXI_DRV_SPINOR_H

#include <stdint.h>
#include <blkpart.h>
#include <sunxi_hal_common.h>
#include <sunxi_hal_spinor.h>

typedef struct sunxi_hal_driver_spinor
{
    sunxi_hal_version_t (*get_version)(int32_t dev);
    sunxi_hal_spinor_capabilities_t (*get_capabilities)(void);
    int32_t (*initialize)(sunxi_hal_spinor_signal_event_t cb_event);
    int32_t (*uninitialize)(void);
    int32_t (*power_control)(sunxi_hal_power_state_e state);
    int32_t (*read_data)(uint32_t addr, void *data, uint32_t cnt);
    int32_t (*program_data)(uint32_t addr, const void *data, uint32_t cnt);
    int32_t (*erase_sector)(uint32_t addr, uint32_t len);
    int32_t (*erase_chip)(void);
    sunxi_hal_spinor_status_t (*get_status)(void);
    sunxi_hal_spinor_info *(*get_info)(void);
    void (* signal_event)(uint32_t event);
    int32_t (*control)(int32_t dev, uint32_t command, uint32_t arg);
} sunxi_hal_driver_spinor_t;

int sunxi_driver_spinor_init(void);
int sunxi_driver_spinor_deinit(void);

#endif  /*SUNXI_DRV_SPINOR_H*/
