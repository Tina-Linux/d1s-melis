
#ifndef __GADGET_H__
#define __GADGET_H__

#include <aw_list.h>
#include <usb/usb_gadget.h>
#include <usb/ch9.h>
#include <hal_sem.h>

#if 0
#define gadget_info(fmt, args...)	\
    printf("[GADGET-INFO][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)

#define gadget_debug(fmt, args...)	\
    printf("[GADGET-DEBUG][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)
#else
#define gadget_info(fmt, args...)
#define gadget_debug(fmt, args...)
#endif

#define gadget_err(fmt, args...)	\
    printf("[GADGET-ERROR][%s] line:%d " fmt "\n", __func__, __LINE__, ##args)

//typedef sem_t usb_schd_t;

typedef struct {
	unsigned int length;
	char buffer;
} usb_queue_t;

struct usb_function_driver {
	const char *name;
	struct list_head list;
	hal_sem_t read_schd;
	hal_sem_t read_finish_schd;
	hal_sem_t write_finish_schd;
	hal_sem_t write_schd;
	uint8_t *ep_addr;
	usb_queue_t *read_queue;
	usb_queue_t *write_queue;
	void *config_desc;

	int (*desc_init)(struct usb_function_driver *);
	int (*desc_deinit)(struct usb_function_driver *);
	int (*standard_req)(struct usb_ctrlrequest *);
	int (*class_req)(struct usb_ctrlrequest *);

	int read_size;

	int enabled;

	uint16_t *strings[USB_GADGET_MAX_IDX];
};


int usb_gadget_function_register(struct usb_function_driver *newf);
int usb_gadget_function_unregister(struct usb_function_driver *fd);
int usb_gadget_string_set(struct usb_function_driver *ugf, char *str, unsigned int idx);



#endif /* __GADGET_H__ */
