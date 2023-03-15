INC_PATH := -I drivers/hal/source/usb/common
INC_PATH += -I drivers/hal/source/usb/include

subdir-ccflags-y += $(INC_PATH)

obj-$(CONFIG_DRIVERS_USB) += common/
obj-$(CONFIG_USB_HOST) += core/
obj-$(CONFIG_USB_HOST) += host/
obj-$(CONFIG_USB_DEVICE) += udc/
obj-$(CONFIG_USB_GADGET) += gadget/
obj-$(CONFIG_USB_MANAGER) += manager/
obj-$(CONFIG_USB_STORAGE) += storage/
obj-$(CONFIG_USB_HID) += hid/
obj-$(CONFIG_USB_CAMERA) += uvc/
#obj-$(CONFIG_USB_CARPLAY_HOST) += carplay_host/

obj-$(CONFIG_ARCH_SUN8IW18P1) += platform/sun8iw18/
obj-$(CONFIG_ARCH_SUN8IW19) += platform/sun8iw19/
obj-$(CONFIG_ARCH_SUN8IW20) += platform/sun20iw1/
obj-$(CONFIG_ARCH_SUN20IW1) += platform/sun20iw1/

obj-$(CONFIG_ARCH_SUN20IW2) += platform/sun20iw2/
