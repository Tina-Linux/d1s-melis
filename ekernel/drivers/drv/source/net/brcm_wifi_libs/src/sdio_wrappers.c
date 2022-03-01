#include <hal_sdhost.h>

struct mmc_host * HAL_SDC_Init(struct mmc_host *host)
{
	return hal_sdc_init(host);
}

struct mmc_host *HAL_SDC_Create(uint32_t sdc_id, SDC_InitTypeDef *param)
{
	return hal_sdc_create(sdc_id, param);
}
