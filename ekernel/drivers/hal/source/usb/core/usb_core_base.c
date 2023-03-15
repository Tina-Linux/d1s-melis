/*
*******************************************************************************
*                                              usb host module
*
*                             Copyright(C), 2006-2008, SoftWinners Co., Ltd.
*                               All Rights Reserved
*
* File Name :
*
* Author : GLHuang(HoLiGun)
*
* Version : 1.0
*
* Date : 2008.07.xx
*
* Description :
*
* History :
*******************************************************************************
*/

#include "usb_gen_hcd.h"
#include "mod_devicetable.h"


static uint32_t usb_host_is_run_flag = 0;

#define USB_HOST_IS_RUN_TRUE		0x11
#define USB_HOST_IS_RUN_FAIL		0x22

// usb host开启
void usb_core_enable(void)
{
	usb_host_is_run_flag = USB_HOST_IS_RUN_TRUE;
}
// usb host 禁止
void usb_core_disable(void)
{
	usb_host_is_run_flag = USB_HOST_IS_RUN_FAIL;
}
//查询usb host是否开启
// return    :   非0为enable
uint8_t usb_core_is_enabled(void)
{
	return (usb_host_is_run_flag == USB_HOST_IS_RUN_TRUE) ? 0x01 : 0x00;
}

/**
 * usb_host_alloc_virt_dev - usb device constructor (usbcore-internal)
 * @parent: hub to which device is connected; null to allocate a root hub
 * @bus: bus used to access the device
 * @port1: one-based index of port; ignored for root hubs
 * Context: !in_interrupt ()
 *
 * Only hub drivers (including virtual root hub drivers for host
 * controllers) should ever call this.
 *
 * This call may not be used in a non-sleeping context.
 */
/* 创建usb_device结构 */
struct usb_device *usb_host_alloc_virt_dev(struct usb_device *parent,
					   struct usb_bus *bus, uint32_t port1)
{
	struct usb_device *dev = NULL;
	int32_t i = 0;

	if (bus == NULL) {
		__err("PANIC : usb_host_alloc_virt_dev()  : input == NULL");
		return NULL;
	}

	// dev =  hal_malloc(sizeof(*dev));
	dev = hal_malloc(sizeof(struct usb_device));

	if (!dev) {
		__err("PANIC : usb_host_alloc_virt_dev()  : dev == NULL");
		return NULL;
	}

	memset(dev, 0, sizeof(struct usb_device));
	dev->state = USB_STATE_ATTACHED;

	INIT_LIST_HEAD(&dev->ep0.urb_list);
	dev->ep0.desc.bLength = USB_DT_ENDPOINT_SIZE;
	dev->ep0.desc.bDescriptorType = USB_DT_ENDPOINT;
	/* ep0 maxpacket comes later, from device descriptor */
	dev->ep_in[0] = dev->ep_out[0] = &dev->ep0;
	/* Save readable and stable topology id, distinguishing devices
	 * by location for diagnostics, tools, driver model, etc.  The
	 * string is a path along hub ports, from the root.  Each device's
	 * dev->devpath will be stable until USB is re-cabled, and hubs
	 * are often labeled with these port numbers.  The bus_id isn't
	 * as stable:  bus->busnum changes easily from modprobe order,
	 * cardbus or pci hotplugging, and so on.
	 */
	/*
	if ((!parent)) {
	    dev->devpath [0] = '0';

	    dev->dev.parent = bus->controller;
	    sprintf (&dev->dev.bus_id[0], "usb%d", bus->busnum);
	} else {
	    //match any labeling on the hubs; it's one-based
	    if (parent->devpath [0] == '0')
		snprintf (dev->devpath, sizeof dev->devpath,
		    "%d", port1);
	    else
		snprintf (dev->devpath, sizeof dev->devpath,
		    "%s.%d", parent->devpath, port1);

	    dev->dev.parent = &parent->dev;
	    sprintf (&dev->dev.bus_id[0], "%d-%s",
		bus->busnum, dev->devpath);

	    // hub driver sets up TT records
	}
    */
	dev->bus = bus;
	dev->parent = parent;
	//?INIT_LIST_HEAD(&dev->filelist);

	dev->usb_virt_dev_semi = hal_sem_create(1);

	//=================================
	//初始化virt_sub_dev部分
	for (i = 0; i < USB_MAX_VIRT_SUB_DEV_NR; i++) {
		dev->virt_sub_dev_array[i].usb_virt_sub_dev_semi = hal_sem_create(1);
	}

	return dev;
}

void usb_host_free_virt_dev(struct usb_device *virt_dev)
{
	int32_t i = 0;
	uint8_t err = 0;

	if (virt_dev == NULL) {
		__err("PANIC : usb_host_free_virt_dev() : input == NULL");
		return;
	}

	// --<1>--释放semi
	// 释放virt_sub_dev部分
	for (i = 0; i < USB_MAX_VIRT_SUB_DEV_NR; i++) {
		if (virt_dev->virt_sub_dev_array[i].usb_virt_sub_dev_semi) {
			hal_sem_delete(virt_dev->virt_sub_dev_array[i].usb_virt_sub_dev_semi);
		} else {
			__err("PANIC : usb_host_free_virt_dev() : virt_sub_dev`s semi == NULL");
			return;
		}
	}

	if (virt_dev->usb_virt_dev_semi) {
		hal_sem_delete(virt_dev->usb_virt_dev_semi);
	} else {
		__err("PANIC : usb_host_free_virt_dev() : virt_dev`s semi == NULL");
		return;
	}

	//--<2>--释放string
	if (virt_dev->manufacturer) {
		free(virt_dev->manufacturer);
		virt_dev->manufacturer = NULL;
	} else {
		__err("ERR: parameter is NULL, can't free");
	}

	if (virt_dev->product) {
		free(virt_dev->product);
		virt_dev->product = NULL;
	} else {
		__err("ERR: parameter is NULL, can't free");
	}

	if (virt_dev->serial) {
		free(virt_dev->serial);
		virt_dev->serial = NULL;
	} else {
		__err("ERR: parameter is NULL, can't free");
	}

	//--<3>--释放,所有相关描述符
	if (virt_dev->config) {
		int j = 0;
		int k = 0;
		int l = 0;
		// int m = 0;

		for (j = 0; j < virt_dev->descriptor.bNumConfigurations; j++) {
			for (k = 0; k < virt_dev->config[j].desc.bNumInterfaces; k++) {
				for (l = 0; l < virt_dev->config[j].intf_cache[k]->num_altsetting; l++) {
					//for(m = 0; m < virt_dev->config[j].intf_cache[k].altsetting_array[l].desc.bNumEndpoints; m++){
					if (virt_dev->config[j].intf_cache[k]->altsetting_array[l].endpoint) {
						free(virt_dev->config[j].intf_cache[k]->altsetting_array[l].endpoint);
						virt_dev->config[j].intf_cache[k]->altsetting_array[l].endpoint = NULL;
					} else {
						__err("ERR: parameter is NULL, can't free");
					}
					//}
				}

				if (virt_dev->config[j].string) {
					free(virt_dev->config[j].string);
					virt_dev->config[j].string = NULL;
				}

				if (virt_dev->config[j].intf_cache[k]->altsetting_array) {
					free(virt_dev->config[j].intf_cache[k]->altsetting_array);
					virt_dev->config[j].intf_cache[k]->altsetting_array = NULL;
				} else {
					__err("ERR: parameter is NULL, can't free");
				}

				if (virt_dev->config[j].intf_cache[k]) {
					free(virt_dev->config[j].intf_cache[k]);
					virt_dev->config[j].intf_cache[k] = NULL;
				} else {
					__err("ERR: parameter is NULL, can't free");
				}

				/*--在usb_disable_device()中已经删除了
				DMSG_INFO("new_interfaces[n]--free-- = %x",virt_dev->config[j].interface[k]);
				if(virt_dev->config[j].interface[k]){
					free(virt_dev->config[j].interface[k]);
					virt_dev->config[j].interface[k] = NULL;
				}
				*/
			}

			// free
			// free(virt_dev->config->intf_cache);
			// virt_dev->config->intf_cache = NULL;
		}

		// free
		if (virt_dev->config) {
			free(virt_dev->config);
			virt_dev->config = NULL;
		} else {
			__err("ERR: parameter is NULL, can't free");
		}
	}

	for (i = 0; i < virt_dev->descriptor.bNumConfigurations; i++) {
		if (virt_dev->rawdescriptors[i]) {
#if ENABLE_DMA_ALLOC_COHERENT
			dma_free_coherent(virt_dev->rawdescriptors[i]);
#else
			hal_free_align(virt_dev->rawdescriptors[i]);
#endif
			virt_dev->rawdescriptors[i] = NULL;
		} else {
			__err("ERR: parameter is NULL, can't free");
		}
	}

	if (virt_dev->rawdescriptors) {
		free(virt_dev->rawdescriptors);
		virt_dev->rawdescriptors = NULL;
	} else {
		__err("ERR: parameter is NULL, can't free");
	}

	//--<4>--释放自己
	free(virt_dev);
	// virt_dev = NULL;
	return;
}

/**
 * usb_match_id - find first usb_device_id matching device or interface
 * @interface: the interface of interest
 * @id: array of usb_device_id structures, terminated by zero entry
 *
 * usb_match_id searches an array of usb_device_id's and returns
 * the first one matching the device or interface, or null.
 * This is used when binding (or rebinding) a driver to an interface.
 * Most USB device drivers will use this indirectly, through the usb core,
 * but some layered driver frameworks use it directly.
 * These device tables are exported with MODULE_DEVICE_TABLE, through
 * modutils and "modules.usbmap", to support the driver loading
 * functionality of USB hotplugging.
 *
 * What Matches:
 *
 * The "match_flags" element in a usb_device_id controls which
 * members are used.  If the corresponding bit is set, the
 * value in the device_id must match its corresponding member
 * in the device or interface descriptor, or else the device_id
 * does not match.
 *
 * "driver_info" is normally used only by device drivers,
 * but you can create a wildcard "matches anything" usb_device_id
 * as a driver's "modules.usbmap" entry if you provide an id with
 * only a nonzero "driver_info" field.  If you do this, the USB device
 * driver's probe() routine should use additional intelligence to
 * decide whether to bind to the specified interface.
 *
 * What Makes Good usb_device_id Tables:
 *
 * The match algorithm is very simple, so that intelligence in
 * driver selection must come from smart driver id records.
 * Unless you have good reasons to use another selection policy,
 * provide match elements only in related groups, and order match
 * specifiers from specific to general.  Use the macros provided
 * for that purpose if you can.
 *
 * The most specific match specifiers use device descriptor
 * data.  These are commonly used with product-specific matches;
 * the USB_DEVICE macro lets you provide vendor and product IDs,
 * and you can also match against ranges of product revisions.
 * These are widely used for devices with application or vendor
 * specific bDeviceClass values.
 *
 * Matches based on device class/subclass/protocol specifications
 * are slightly more general; use the USB_DEVICE_INFO macro, or
 * its siblings.  These are used with single-function devices
 * where bDeviceClass doesn't specify that each interface has
 * its own class.
 *
 * Matches based on interface class/subclass/protocol are the
 * most general; they let drivers bind to any interface on a
 * multiple-function device.  Use the USB_INTERFACE_INFO
 * macro, or its siblings, to match class-per-interface style
 * devices (as recorded in bDeviceClass).
 *
 * Within those groups, remember that not all combinations are
 * meaningful.  For example, don't give a product version range
 * without vendor and product IDs; or specify a protocol without
 * its associated class and subclass.
 */
const struct usb_device_id *usb_match_id(struct usb_interface *intf,
						   const struct usb_device_id *id)
{
	struct usb_host_interface *virt_intf = NULL;
	struct usb_device *dev = NULL;

	/* proc_connectinfo in devio.c may call us with id == NULL. */
	if (id == NULL) {
		hal_log_err("ERR: usb_match_id: input error");
		return NULL;
	}

	virt_intf = intf->cur_altsetting;

	if (virt_intf == NULL) {
		hal_log_err("ERR: usb_match_id: virt_intf == NULL");
		return NULL;
	}

	dev = interface_to_usbdev(intf);

	if (dev == NULL) {
		hal_log_err("ERR: usb_match_id: dev == NULL");
		return NULL;
	}

	/* It is important to check that id->driver_info is nonzero,
       since an entry that is all zeroes except for a nonzero
       id->driver_info is the way to create an entry that
       indicates that the driver want to examine every
       device and interface. */
	for (; id->idVendor || id->bDeviceClass || id->bInterfaceClass || id->driver_info; id++) {
		/*
		hal_log_info("usb match id, match_flags = %x, bInterfaceClass= (%x, %x)",
			   id->match_flags, id->bInterfaceClass,
			   virt_intf->desc.bInterfaceClass);
	*/
		if ((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR)
		    && id->idVendor != le16_to_cpu(dev->descriptor.idVendor)) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT)
		    && id->idProduct != le16_to_cpu(dev->descriptor.idProduct)) {
			continue;
		}

		/* No need to test id->bcdDevice_lo != 0, since 0 is never
		 *greater than any unsigned number. */
		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO)
		    && (id->bcdDevice_lo > le16_to_cpu(dev->descriptor.bcdDevice))) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI)
		    && (id->bcdDevice_hi < le16_to_cpu(dev->descriptor.bcdDevice))) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS)
		    && (id->bDeviceClass != dev->descriptor.bDeviceClass)) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS)
		    && (id->bDeviceSubClass != dev->descriptor.bDeviceSubClass)) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL)
		    && (id->bDeviceProtocol != dev->descriptor.bDeviceProtocol)) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_INT_CLASS)
		    && (id->bInterfaceClass != virt_intf->desc.bInterfaceClass)) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_INT_SUBCLASS)
		    && (id->bInterfaceSubClass != virt_intf->desc.bInterfaceSubClass)) {
			continue;
		}

		if ((id->match_flags & USB_DEVICE_ID_MATCH_INT_PROTOCOL)
		    && (id->bInterfaceProtocol != virt_intf->desc.bInterfaceProtocol)) {
			continue;
		}

		hal_log_info("usb match id suceessfull");
		return id;
	}

	// hal_log_info("usb match id failed, bDeviceClass = %x, bInterfaceClass = %x",
	// 	     dev->descriptor.bDeviceClass, virt_intf->desc.bInterfaceClass);
	return NULL;
}

// return    :   0       //成功
//      :   其余    //失败
int32_t _usb_core_func_drv_probe_interface(struct usb_host_virt_sub_dev *sub_dev)
{
	struct usb_interface *intf = NULL;
	struct usb_driver *func_drv = NULL;
	const struct usb_device_id *id = NULL;
	int32_t error = -ENODEV;
	struct usb_device *virt_dev = NULL;

	if (!sub_dev || !(sub_dev->sub_dev_interface)) {
		__err("PANIC : _usb_core_func_drv_probe_interface() : input fail,sub_dev = 0x%x ,");
		return -ENODEV;
	}

	intf = sub_dev->sub_dev_interface;
	func_drv = sub_dev->func_drv;
	// hal_log_info("----func_drv->func_drv_name = %s", func_drv->func_drv_name);

	if (!func_drv->probe) {
		__err("PANIC : _usb_core_func_drv_probe_interface() : func_drv->probe == NULL");
		return error;
	}

	/* FIXME we'd much prefer to just resume it ... */
	virt_dev = interface_to_usbdev(intf);

	if (virt_dev == NULL) {
		__err("PANIC : virt_dev == NULL");
		return error;
	}

	if (virt_dev->state == USB_STATE_SUSPENDED) {
		__err("PANIC : virt_dev->state == USB_STATE_SUSPENDED");
		return -EHOSTUNREACH;
	}
	id = usb_match_id(intf, func_drv->match_table);

	if (id) {
		intf->condition = USB_INTERFACE_BINDING;
		error = func_drv->probe(intf, id);
		intf->condition = error ? USB_INTERFACE_UNBOUND : USB_INTERFACE_BOUND;
		return error;
	}

	return -ENODEV;	 //失败
}

int32_t _usb_core_func_drv_disconncet_interface(struct usb_host_virt_sub_dev *dev)
{
	struct usb_interface *intf = NULL;
	struct usb_driver *driver = NULL;

	if (dev == NULL) {
		__err("ERR; invalid argment");
		return -1;
	}

	intf = dev->sub_dev_interface;

	if (intf == NULL) {
		__err("ERR; invalid argment");
		return -1;
	}

	if (!intf->virt_sub_dev) {
		__err("ERR; invalid argment");
		return -1;
	}

	driver = intf->virt_sub_dev->func_drv;

	if (driver == NULL) {
		__err("ERR; invalid argment");
		return -1;
	}

	intf->condition = USB_INTERFACE_UNBINDING;
	/* release all urbs for this interface */
	usb_disable_interface(interface_to_usbdev(intf), intf);

	/* 调用driver的disconnect,如msc_disconnect() */
	if (driver && driver->disconnect) {
		driver->disconnect(intf);
	}

	/* reset other interface state */
	usb_set_interface(interface_to_usbdev(intf),
			  intf->altsetting[0].desc.bInterfaceNumber, 0);
	usb_set_intfdata(intf, NULL);
	intf->condition = USB_INTERFACE_UNBOUND;
	return 0;
}

/**
 * usb_altnum_to_altsetting - get the altsetting structure with a given
 *  alternate setting number.
 * @intf: the interface containing the altsetting in question
 * @altnum: the desired alternate setting number
 *
 * This searches the altsetting array of the specified interface for
 * an entry with the correct bAlternateSetting value and returns a pointer
 * to that entry, or null.
 *
 * Note that altsettings need not be stored sequentially by number, so
 * it would be incorrect to assume that the first altsetting entry in
 * the array corresponds to altsetting zero.  This routine helps device
 * drivers avoid such mistakes.
 *
 * Don't call this function unless you are bound to the intf interface
 * or you have locked the device!
 */
struct usb_host_interface *usb_altnum_to_altsetting(struct usb_interface *intf, int32_t altnum)
{
	int i;

	for (i = 0; i < intf->num_altsetting; i++) {
		if (intf->altsetting[i].desc.bAlternateSetting == altnum) {
			return &intf->altsetting[i];
		}
	}

	return NULL;
}

/**
 * usb_lock_device - acquire the lock for a usb device structure
 * @udev: device that's being locked
 *
 * Use this routine when you don't hold any other device locks;
 * to acquire nested inner locks call down(&udev->serialize) directly.
 * This is necessary for proper interaction with usb_lock_all_devices().
 */
void usb_lock_device(struct usb_device *virt_dev)
{
	uint8_t err = 0;
	// down_read(&usb_all_devices_rwsem);
	// USB_OS_SEMI_PEND(virt_dev->usb_virt_dev_semi, 0, &err);
	hal_sem_wait(virt_dev->usb_virt_dev_semi);
}

/**
 * usb_unlock_device - release the lock for a usb device structure
 * @udev: device that's being unlocked
 *
 * Use this routine when releasing the only device lock you hold;
 * to release inner nested locks call up(&udev->serialize) directly.
 * This is necessary for proper interaction with usb_lock_all_devices().
 */
void usb_unlock_device(struct usb_device *virt_dev)
{
	// USB_OS_SEMI_POST(virt_dev->usb_virt_dev_semi);
	hal_sem_post(virt_dev->usb_virt_dev_semi);
	// up_read(&usb_all_devices_rwsem);
}

void *usb_buffer_alloc(struct usb_device *dev, uint32_t size, uint32_t mem_flags, void *dma)
{
	return hcd_ops_buffer_alloc(dev->bus, size, mem_flags, dma);
}

/**
 * usb_buffer_free - free memory allocated with usb_buffer_alloc()
 * @dev: device the buffer was used with
 * @size: requested buffer size
 * @addr: CPU address of buffer
 * @dma: DMA address of buffer
 *
 * This reclaims an I/O buffer, letting it be reused.  The memory must have
 * been allocated using usb_buffer_alloc(), and the parameters must match
 * those provided in that allocation request.
 */
void usb_buffer_free(struct usb_device *dev, uint32_t size, void *addr, void *dma)
{
	hcd_ops_buffer_free(dev->bus, size, addr, dma);
}

/**
 * usb_ifnum_to_if - get the interface object with a given interface number
 * @dev: the device whose current configuration is considered
 * @ifnum: the desired interface
 *
 * This walks the device descriptor for the currently active configuration
 * and returns a pointer to the interface with that particular interface
 * number, or null.
 *
 * Note that configuration descriptors are not required to assign interface
 * numbers sequentially, so that it would be incorrect to assume that
 * the first interface in that descriptor corresponds to interface zero.
 * This routine helps device drivers avoid such mistakes.
 * However, you should make sure that you do the right thing with any
 * alternate settings available for this interfaces.
 *
 * Don't call this function unless you are bound to one of the interfaces
 * on this device or you have locked the device!
 */
struct usb_interface *usb_ifnum_to_if(struct usb_device *dev, uint32_t ifnum)
{
	struct usb_host_config *config = dev->actconfig;
	int i;

	if (!config) {
		__err("ERR: input = NULL");
		return NULL;
	}

	for (i = 0; i < config->desc.bNumInterfaces; i++) {
		if (config->interface[i]->altsetting[0].desc.bInterfaceNumber == ifnum) {
			return config->interface[i];
		}
	}

	return NULL;
}
