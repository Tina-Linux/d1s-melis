#include "platform_usb.h"
#include <sunxi_hal_common.h>

#define SUNXI_USB_OFFSET_PHY_CTRL		0x10
#define SUNXI_USB_OFFSET_PHY_STATUS		0x24

#define SUNXI_USB_PHY_EFUSE_ADDR		0x03006218
#define SUNXI_USB_PHY_EFUSE_ADJUST		0x10000		//bit16
#define SUNXI_USB_PHY_EFUSE_MODE		0x20000		//bit17
#define SUNXI_USB_PHY_EFUSE_RES			0x3C0000	//bit18-21
#define SUNXI_USB_PHY_EFUSE_COM			0x1C00000	//bit22-24
#define SUNXI_USB_PHY_EFUSE_USB0TX		0x1C00000	//bit22-24
#define SUNXI_USB_PHY_EFUSE_USB1TX		0xE000000	//bit25-27

static int usb_new_phyx_tp_write(uint32_t phy_vbase, int addr, int data, int len)
{
	int temp = 0;
	int j = 0;
	int dtmp = 0;
	
	/*device: 0x410(phy_ctl)*/
	dtmp = data;
	for (j = 0; j < len; j++) 
	{
		temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp |= (0x1 << 1);
		hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);

		hal_writeb(addr + j, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL + 1);
		temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp &= ~(0x1 << 0);
		hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp &= ~(0x1 << 7);
		temp |= (dtmp & 0x1) << 7;
		hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp |= (0x1 << 0);
		hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp &= ~(0x1 << 0);
		hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		
		temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		temp &= ~(0x1 << 1);
		hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
		dtmp >>= 1;
	}
	return 0;
}

static int usb_new_phyx_tp_read(uint32_t phy_vbase, int addr, int len)
{
	int temp = 0;
	int i = 0;
	int j = 0;
	int ret = 0;
	
	temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
	temp |= (0x1 << 1);
	hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
	
	for (j = len; j > 0; j--)
	{
		hal_writeb((addr + j - 1), phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL + 1);
		for (i = 0; i < 0x4; i++);
		temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_STATUS);
		ret <<= 1;
		ret |= (temp & 0x1);
	}
	
	temp = hal_readb(phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
	temp &= ~(0x1 << 1);
	hal_writeb(temp, phy_vbase + SUNXI_USB_OFFSET_PHY_CTRL);
	return ret;

}

static void usb_new_phy_print(uint32_t phy_vbase)
{
	printf("[%s][%d] reg 0x%x : 0x%x\n", __func__, __LINE__, 0x1C, usb_new_phyx_tp_read(phy_vbase, 0x1C, 0x03));
	printf("[%s][%d] reg 0x%x : 0x%x\n", __func__, __LINE__, 0x30, usb_new_phyx_tp_read(phy_vbase, 0x30, 0x0D));
	printf("[%s][%d] reg 0x%x : 0x%x\n", __func__, __LINE__, 0x60, usb_new_phyx_tp_read(phy_vbase, 0x60, 0x0E));
	printf("[%s][%d] reg 0x%x : 0x%x\n\n", __func__, __LINE__, 0x40, usb_new_phyx_tp_read(phy_vbase, 0x40, 0x08));
}

void usb_new_phy_driverlevel_adjust(uint32_t phy_vbase, int driverlevel)
{
	int value = 0;

	value = usb_new_phyx_tp_read(phy_vbase, 0x60, 0x0E);
	value = (value & (~0x0f)) | driverlevel;
	usb_new_phyx_tp_write(phy_vbase, 0x60, value, 0x0E);
}

 /*phy develop by self*/
#if defined(CONFIG_ARCH_SUN8IW20) || defined(CONFIG_SOC_SUN20IW1)
void usb_new_phy_init(uint32_t phy_vbase, uint32_t usbc_no)
{
	int value = 0;
	int efuse = 0;


	efuse = hal_readl(SUNXI_USB_PHY_EFUSE_ADDR);
	printf("phy_vbase : 0x%x, usbc_no : %x, efuse : 0x%x\n", phy_vbase, usbc_no, efuse);

	usb_new_phy_print(phy_vbase);
	usb_new_phyx_tp_write(phy_vbase, 0x1C, 0x0, 0x03);

	if(efuse & SUNXI_USB_PHY_EFUSE_ADJUST)
	{
		if(efuse & SUNXI_USB_PHY_EFUSE_MODE)//iref
		{
			usb_new_phyx_tp_write(phy_vbase, 0x60, 0x1, 0x01);

			switch(usbc_no)
			{
			case 0:
				value = (efuse & SUNXI_USB_PHY_EFUSE_USB0TX) >> 22;
				usb_new_phyx_tp_write(phy_vbase, 0x61, value, 0x03);
				break;

			case 1:
				value = (efuse & SUNXI_USB_PHY_EFUSE_USB0TX) >> 25;
				usb_new_phyx_tp_write(phy_vbase, 0x61, value, 0x03);
				break;

			default:
				printf("usb %d not exist!\n", usbc_no);
				break;
			}

			value = (efuse & SUNXI_USB_PHY_EFUSE_RES) >> 18;
			usb_new_phyx_tp_write(phy_vbase, 0x44, value, 0x04);
		}
		else//vref
		{
			usb_new_phyx_tp_write(phy_vbase, 0x60, 0x0, 0x01);

			value = (efuse & SUNXI_USB_PHY_EFUSE_RES) >> 18;
			usb_new_phyx_tp_write(phy_vbase, 0x44, value, 0x04);

			value = (efuse & SUNXI_USB_PHY_EFUSE_COM) >> 22;
			usb_new_phyx_tp_write(phy_vbase, 0x36, value, 0x03);
		}
	}
	usb_new_phy_print(phy_vbase);
}
#else
void usb_new_phy_init(uint32_t phy_vbase, uint32_t usbc_no)
{
	printf("[%s][%d] using default phy\n", __func__, __LINE__);
}
#endif
