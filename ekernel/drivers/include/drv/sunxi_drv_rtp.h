#ifndef SUNXI_DRV_RTP_H
#define SUNXI_DRV_RTP_H

#include "../hal/sunxi_hal_tpadc.h"

int sunxi_rtp_init(void);

int drv_tpadc_adc_init(void);
int drv_tpadc_adc_channel_init(tp_channel_id channel);
int drv_get_tpadc_adc_data(tp_channel_id channel);
int drv_tpadc_register_callback(tpadc_usercallback_t callback);
#ifdef CONFIG_STANDBY
int sunxi_tpadc_resume(void);
int sunxi_tapdc_suspend(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* For debug */
#define RTP_DEBUG
#ifdef  RTP_DEBUG
#define RTP_INFO(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)
#else
#define RTP_INFO(fmt, arg...) do {}while(0)
#endif

#define RTP_ERR(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)

#ifdef __cplusplus
}
#endif

#endif
