/*
 * Gadget Function Driver for MTP
 *
 * Copyright (C) 2010 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _F_MTP_H
#define _F_MTP_H

#include <fcntl.h>
//#include "sunxi_drv_usbd.h"

typedef struct mtp_file_range
{
    /* file descriptor for file to transfer */
    ES_FILE         *fp;
    /* offset in file for start of transfer */
    long long offset;
    /* number of bytes to transfer */
    int64_t     length;
    /* MTP command ID for data header,
     * used only for MTP_SEND_FILE_WITH_HEADER
     */
    uint16_t    command;
    /* MTP transaction ID for data header,
     * used only for MTP_SEND_FILE_WITH_HEADER
     */
    uint32_t    transaction_id;
}mtp_file_range_t;

struct mtp_event
{
    /* size of the event */
    size_t      length;
    /* event data to send */
    void        *data;
};
typedef enum __USBD_MTP_CMD_SET           // Éý¼¶ÃüÁîÁÐ±í(IOCtrol)
{
    MTP_SEND_FILE = 0x01,
    MTP_RECEIVE_FILE,
    MTP_SEND_EVENT,
    MTP_SEND_FILE_WITH_HEADER,    
	MTP_EXIT
} __usbd_mtp_cmd_set_t;

#endif /* _F_MTP_H */
