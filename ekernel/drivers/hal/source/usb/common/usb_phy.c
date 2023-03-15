/*
********************************************************************************************************************
*                             Copyright(C), 2006-2021, AllWinners Co., Ltd.
*                                           All Rights Reserved
*
* File Name : usb_phy.c
*
* Author :
*
* Version : 1.0
*
* Date : 2022.9.15
*
* Description :
* phy的控制目前一共两种：
* 1.直接读写818/418寄存器，主要针对外购phy
* 2.通过VC总线读写，主要针对自研phy
* 但是也会有例外，没有明确区分手段，每个项目不同，具体参考《正在维护平台的使用情况》，或咨询设计人员
* tips：查看spec的phy ctrl和status寄存器看是否有操作vc总线的位也可大致区分
*
* History :
*
********************************************************************************************************************
*/

#include "usb_phy.h"
#include "usb_os_platform.h"

static int bit_offset(uint32_t mask)
{
	int offset = 0;

	for (offset = 0; offset < 32; offset++)
		if ((mask == 0) || (mask & (0x01 << offset)))
			break;
	return offset;
}

/**
 * @brief for register write
 * 	1.PHY's paraments range is 0x0~0x3ff.
 * 	2.PHY's paraments:
 * 		bit[9:8]: TXRESTUNE
 * 		bit[7:6]: TXPREEMPAMPTUNE;
 * 		bit[5:4]: TXRISETUNE;
 * 		bit[3:0]: TXVREFTUNE;
 *
 * @param phy_vbase
 * @param data
 * @return int
 */
int usb_phy_write(unsigned long phy_vbase, int data)
{
	int reg_value = 0;

	reg_value = hal_readl(phy_vbase + USB_PHY_OFFSET_TUNE);

	/*clear TXPREEMPAMPTUNE + TXRESTUNE + TXRISETUNE + TXVREFTUNE */
	reg_value &= ~(USB_PHY_TUNE_TXPREEMPAMPTUNE
		     | USB_PHY_TUNE_TXRESTUNE
		     | USB_PHY_TUNE_TXRISETUNE
		     | USB_PHY_TUNE_TXVREFTUNE);

	/*set TXPREEMPAMPTUNE*/
	reg_value |= ((data & USB_PHY_RANGE_TXPREEMPAMPTUNE)
		      >> bit_offset(USB_PHY_RANGE_TXPREEMPAMPTUNE))
		     << bit_offset(USB_PHY_TUNE_TXPREEMPAMPTUNE);

	/*set TXRESTUNE*/
	reg_value |= ((data & USB_PHY_RANGE_TXRESTUNE)
		      >> bit_offset(USB_PHY_RANGE_TXRESTUNE))
		     << bit_offset(USB_PHY_TUNE_TXRESTUNE);

	/*set TXRISETUNE*/
	reg_value |= ((data & USB_PHY_RANGE_TXRISETUNE)
		      >> bit_offset(USB_PHY_RANGE_TXRISETUNE))
		     << bit_offset(USB_PHY_TUNE_TXRISETUNE);

	/*set TXVREFTUNE*/
	reg_value |= ((data & USB_PHY_RANGE_TXVREFTUNE)
		      >> bit_offset(USB_PHY_RANGE_TXVREFTUNE))
		     << bit_offset(USB_PHY_TUNE_TXVREFTUNE);

	hal_writel(reg_value, (phy_vbase + USB_PHY_OFFSET_TUNE));

	return 0;
}

/**
 * @brief for register read
 *
 * @param phy_vbase
 * @return int
 */
int usb_phy_read(unsigned long phy_vbase)
{
	int data = 0;
	int reg_value = 0;

	reg_value = hal_readl(phy_vbase + USB_PHY_OFFSET_TUNE);

	/*read TXPREEMPAMPTUNE*/
	data |= ((reg_value & USB_PHY_TUNE_TXPREEMPAMPTUNE)
		 >> bit_offset(USB_PHY_TUNE_TXPREEMPAMPTUNE))
		<< bit_offset(USB_PHY_RANGE_TXPREEMPAMPTUNE);

	/*set TXRESTUNE*/
	data |= ((reg_value & USB_PHY_TUNE_TXRESTUNE)
		 >> bit_offset(USB_PHY_TUNE_TXRESTUNE))
		<< bit_offset(USB_PHY_RANGE_TXRESTUNE);

	/*set TXRISETUNE*/
	data |= ((reg_value & USB_PHY_TUNE_TXRISETUNE)
		 >> bit_offset(USB_PHY_TUNE_TXRISETUNE))
		<< bit_offset(USB_PHY_RANGE_TXRISETUNE);

	/*set TXVREFTUNE*/
	data |= ((reg_value & USB_PHY_TUNE_TXVREFTUNE)
		 >> bit_offset(USB_PHY_TUNE_TXVREFTUNE))
		<< bit_offset(USB_PHY_RANGE_TXVREFTUNE);

	printf("bit[3:0]VREF = 0x%x; bit[5:4]RISE = 0x%x; bit[7:6]PREEMPAMP = 0x%x; bit[9:8]RES = 0x%x\n",
	       (data & USB_PHY_RANGE_TXVREFTUNE) >> bit_offset(USB_PHY_RANGE_TXVREFTUNE),
	       (data & USB_PHY_RANGE_TXRISETUNE) >> bit_offset(USB_PHY_RANGE_TXRISETUNE),
	       (data & USB_PHY_RANGE_TXPREEMPAMPTUNE) >> bit_offset(USB_PHY_RANGE_TXPREEMPAMPTUNE),
	       (data & USB_PHY_RANGE_TXRESTUNE) >> bit_offset(USB_PHY_RANGE_TXRESTUNE));

	return data;
}

/**
 * @brief for VC bus write
 *
 * @param phy_vbase phy base register address
 * @param addr the address need to write
 * @param data the date need to write
 * @param len  write data bit length
 * @return int
 */
int usb_phy_tp_write(unsigned long phy_vbase, int addr, int data, int len)
{
	int j = 0;
	int temp = 0;
	int dtmp = data;

	/*VC_EN enable*/
	temp = hal_readb(phy_vbase + USB_PHY_OFFSET_CTRL);
	temp |= USB_PHY_CTRL_VC_EN;
	hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

	for (j = 0; j < len; j++) {
		/*ensure VC_CLK low*/
		temp = hal_readb(phy_vbase + USB_PHY_OFFSET_CTRL);
		temp &= ~USB_PHY_CTRL_VC_CLK;
		hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

		/*set write address*/
		temp = hal_readl(phy_vbase + USB_PHY_OFFSET_CTRL);
		temp &= ~USB_PHY_CTRL_VC_ADDR;//clear
		temp |= ((addr + j) << bit_offset(USB_PHY_CTRL_VC_ADDR));  // write
		hal_writel(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

		/*write data to VC_DI*/
		temp = hal_readb(phy_vbase + USB_PHY_OFFSET_CTRL);
		temp &= ~USB_PHY_CTRL_VC_DI;//clear
		temp |= ((dtmp & 0x01) << bit_offset(USB_PHY_CTRL_VC_DI));  // write
		hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

		/*set VC_CLK high*/
		temp |= USB_PHY_CTRL_VC_CLK;
		hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

		/*right move one bit*/
		dtmp >>= 1;
	}

	/*set VC_CLK low*/
	temp &= ~USB_PHY_CTRL_VC_CLK;
	hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

	/*VC_EN disable*/
	temp = hal_readb(phy_vbase + USB_PHY_OFFSET_CTRL);
	temp &= ~USB_PHY_CTRL_VC_EN;
	hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

	return 0;
}

/**
 * @brief for VC bus read
 *
 * @param phy_vbase phy base register address
 * @param addr the address need to read
 * @param len read data bit length
 * @return int
 */
int usb_phy_tp_read(unsigned long phy_vbase, int addr, int len)
{
	int temp = 0;
	int i = 0;
	int j = 0;
	int ret = 0;

	/*VC_EN enable*/
	temp = hal_readb(phy_vbase + USB_PHY_OFFSET_CTRL);
	temp |= USB_PHY_CTRL_VC_EN;
	hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

	for (j = len; j > 0; j--) {
		/*set write address*/
		temp = hal_readl(phy_vbase + USB_PHY_OFFSET_CTRL);
		temp &= ~USB_PHY_CTRL_VC_ADDR;//clear
		temp |= ((addr + j - 1) << bit_offset(USB_PHY_CTRL_VC_ADDR));  // write
		hal_writel(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

		/*delsy 1us*/
		hal_udelay(1);

		/*read data from VC_DO*/
		temp = hal_readb(phy_vbase + USB_PHY_OFFSET_STATUS);
		ret <<= 1;
		ret |= temp & USB_PHY_STATUS_VC_DO;

	}

	/*VC_EN disable*/
	temp = hal_readb(phy_vbase + USB_PHY_OFFSET_CTRL);
	temp &= ~USB_PHY_CTRL_VC_EN;
	hal_writeb(temp, phy_vbase + USB_PHY_OFFSET_CTRL);

	return ret;
}

void usb_new_phy_driverlevel_show(unsigned long phy_vbase)
{
	printf("driver level value : %d\n", usb_phy_tp_read(phy_vbase, 0x60, 0x0E) & 0xf);
}

void usb_new_phy_driverlevel_adjust(unsigned long phy_vbase, int driverlevel)
{
	int value = 0;
	if (driverlevel < 0 || driverlevel > 15) {
		printf("driver evel error value  : %d!! level range[0-15]\n", driverlevel);
		return;
	}

	value = usb_phy_tp_read(phy_vbase, 0x60, 0x0E);
	value = (value & (~0x0f)) | driverlevel;
	usb_phy_tp_write(phy_vbase, 0x60, value, 0x0E);
	printf("driver level set success : %d\n", usb_phy_tp_read(phy_vbase, 0x60, 0x0E) & 0xf);
}

#if defined(CONFIG_ARCH_SUN8IW20) || defined(CONFIG_SOC_SUN20IW1)
void usb_phy_init(unsigned long phy_vbase, unsigned int usbc_no)
{
	int value = 0;
	int efuse = 0;

	efuse = hal_readl(USB_PHY_EFUSE_ADDR);

	printf("phy_vbase : 0x%lx, usbc_no : %x, efuse : 0x%x\n", phy_vbase, usbc_no, efuse);

	if (efuse & USB_PHY_EFUSE_ADJUST) {
		if (efuse & USB_PHY_EFUSE_MODE)  // iref
		{
			usb_phy_tp_write(phy_vbase, 0x60, 0x1, 0x01);

			switch (usbc_no) {
			case 0:
				value = (efuse & USB_PHY_EFUSE_IREF_USB0TX) >> 22;
				usb_phy_tp_write(phy_vbase, 0x61, value, 0x03);
				break;

			case 1:
				value = (efuse & USB_PHY_EFUSE_IREF_USB1TX) >> 25;
				usb_phy_tp_write(phy_vbase, 0x61, value, 0x03);
				break;

			default:
				printf("usb %d not exist!\n", usbc_no);
				break;
			}

			value = (efuse & USB_PHY_EFUSE_RES) >> 18;//电阻软件校准??
			usb_phy_tp_write(phy_vbase, 0x44, value, 0x04);
		} else { // vref
			usb_phy_tp_write(phy_vbase, 0x60, 0x0, 0x01);

			value = (efuse & USB_PHY_EFUSE_RES) >> 18;//电阻软件校准??
			usb_phy_tp_write(phy_vbase, 0x44, value, 0x04);

			value = (efuse & USB_PHY_EFUSE_VERF_COM) >> 22;
			usb_phy_tp_write(phy_vbase, 0x36, value, 0x03);
		}
	}
}
#elif defined(CONFIG_ARCH_SUN20IW2)
void usb_phy_init(unsigned long phy_vbase, unsigned int usbc_no)
{
	/* 0xC - 0x1, enable calibration */
	usb_phy_tp_write(phy_vbase, 0xC, 0x1, 1);
	/* 0x20~0x21 - 0x3, adjust amplitude */
	usb_phy_tp_write(phy_vbase, 0x20, 0x3, 2);
	usb_phy_tp_write(phy_vbase, 0x3, 0x0, 2);

	/* 0x1a~0x1f, ro, read calibration value */
	printf("calibration finish, val:0x%x, usbc_no:%d\n",
		     usb_phy_tp_read(phy_vbase, 0x1a, 6), usbc_no);
}
#else
void usb_phy_init(unsigned long phy_vbase, unsigned int usbc_no)
{
	printf("using default phy\n");
}
#endif

