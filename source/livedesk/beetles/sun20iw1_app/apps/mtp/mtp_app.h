#ifndef __MTP_APP_H__
#define __MTP_APP_H__
#include "elibs_stdio.h"
typedef enum {
	MTP_STATE_INIT = 0,
	MTP_STATE_ADD,
	MTP_STATE_DEL,
	MTP_STATE_EXIT
}Mtp_Command_t;
extern void usb_uevent_handle(Mtp_Command_t command, unsigned char partname);
#endif
