#ifndef _SUNXI_HAL_LSPSRAM_H_
#define _SUNXI_HAL_LSPSRAM_H_

struct psram_chip;

int platform_psram_chip_config(void);
struct psram_chip *psram_chip_get_para(void);
void psram_suspend(struct psram_chip *psram_para);
int psram_resume(struct psram_chip *psram_para);

#endif
