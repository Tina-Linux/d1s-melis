#ifndef __USB_PHY_H__
#define __USB_PHY_H__

#define USB_PHY_OFFSET_CTRL		0x10
#define USB_PHY_OFFSET_TUNE		0x18
#define USB_PHY_OFFSET_STATUS		0x24

#define USB_PHY_CTRL_VC_CLK		(0x1 << 0)	//bit0
#define USB_PHY_CTRL_VC_EN		(0x1 << 1)	//bit1
#define USB_PHY_CTRL_VC_DI		(0x1 << 7)	//bit7
#define USB_PHY_CTRL_VC_ADDR		(0xff << 8)	//bit8-15
#define USB_PHY_STATUS_VC_DO		(0x1 << 0)	//bit0
#define USB_PHY_TUNE_TXPREEMPAMPTUNE	(0x3 << 0)	//bit0-1
#define USB_PHY_TUNE_TXRESTUNE		(0x3 << 2)	//bit2-3
#define USB_PHY_TUNE_TXRISETUNE		(0x3 << 4)	//bit4-5
#define USB_PHY_TUNE_TXVREFTUNE		(0xf << 8)	//bit8-11

#define USB_PHY_EFUSE_ADDR		0x03006218
#define USB_PHY_EFUSE_ADJUST		(0x1<<16)	//bit16
#define USB_PHY_EFUSE_MODE		(0x1<<17)	//bit17
#define USB_PHY_EFUSE_RES		(0xf<<18)	//bit18-21
#define USB_PHY_EFUSE_VERF_COM		(0x7<<22)	//bit22-24
#define USB_PHY_EFUSE_IREF_USB0TX	(0x7<<22)	//bit22-24
#define USB_PHY_EFUSE_IREF_USB1TX	(0x7<<25)	//bit25-27

#define USB_PHY_RANGE_TXVREFTUNE	(0xf<<0)	//bit0-3
#define USB_PHY_RANGE_TXRISETUNE	(0x3<<4)	//bit4-5
#define USB_PHY_RANGE_TXPREEMPAMPTUNE	(0x3<<6)	//bit6-7
#define USB_PHY_RANGE_TXRESTUNE		(0x3<<8)	//bit8-9

int usb_phy_write(unsigned long phy_vbase, int data);
int usb_phy_read(unsigned long phy_vbase);
int usb_phy_tp_write(unsigned long phy_vbase, int addr, int data, int len);
int usb_phy_tp_read(unsigned long phy_vbase, int addr, int len);

void usb_phy_init(unsigned long phy_vbase, unsigned int usbc_no);
void usb_new_phy_driverlevel_show(unsigned long phy_vbase);
void usb_new_phy_driverlevel_adjust(unsigned long phy_vbase, int driverlevel);

#endif //__USB_PHY_H__
