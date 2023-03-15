/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __SCSI_H
#define __SCSI_H
#include "usb_slave_msc.h"
#define MAX_COMMAND_SIZE        16       // Length of a SCSI Command Data Block

/* SCSI commands that we recognize */
#define SC_FORMAT_UNIT                  0x04
#define SC_INQUIRY                      0x12
#define SC_MODE_SELECT_6                0x15
#define SC_MODE_SELECT_10               0x55
#define SC_MODE_SENSE_6                 0x1a
#define SC_MODE_SENSE_10                0x5a
#define SC_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1e
#define SC_READ_6                       0x08
#define SC_READ_10                      0x28
#define SC_READ_12                      0xa8
#define SC_READ_CAPACITY                0x25
#define SC_READ_FORMAT_CAPACITIES       0x23
#define SC_RELEASE                      0x17
#define SC_REQUEST_SENSE                0x03
#define SC_RESERVE                      0x16
#define SC_SEND_DIAGNOSTIC              0x1d
#define SC_START_STOP_UNIT              0x1b
#define SC_SYNCHRONIZE_CACHE            0x35
#define SC_TEST_UNIT_READY              0x00
#define SC_VERIFY                       0x2f
#define SC_WRITE_6                      0x0a
#define SC_WRITE_10                     0x2a
#define SC_WRITE_12                     0xaa

/* SCSI Sense Key/Additional Sense Code/ASC Qualifier values */
#define SS_NO_SENSE                             0
#define SS_COMMUNICATION_FAILURE                0x040800
#define SS_INVALID_COMMAND                      0x052000
#define SS_INVALID_FIELD_IN_CDB                 0x052400
#define SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE   0x052100
#define SS_LOGICAL_UNIT_NOT_SUPPORTED           0x052500
#define SS_MEDIUM_NOT_PRESENT                   0x023a00
#define SS_MEDIUM_REMOVAL_PREVENTED             0x055302
#define SS_NOT_READY_TO_READY_TRANSITION        0x062800
#define SS_RESET_OCCURRED                       0x062900
#define SS_SAVING_PARAMETERS_NOT_SUPPORTED      0x053900
#define SS_UNRECOVERED_READ_ERROR               0x031100
#define SS_WRITE_ERROR                          0x030c02
#define SS_WRITE_PROTECTED                      0x072700

#define SS_RESET_CMD_PHASE_ERR                  0x064a00
#define SS_RESET_DATA_PHASE_ERR                 0x064b00

#define SK(x)       ((u8) ((x) >> 16))  // Sense Key byte, etc.
#define ASC(x)      ((u8) ((x) >> 8))
#define ASCQ(x)     ((u8) (x))


#define   backing_file_is_open(curlun)   ((curlun)->filp != NULL)


#define USB_BULK_CS_WRAP_LEN    13
#define USB_BULK_CS_SIG         0x53425355  // Spells out 'USBS'
#define USB_STATUS_PASS         0
#define USB_STATUS_FAIL         1
#define USB_STATUS_PHASE_ERROR  2

int get_next_command(struct msc_slave_dev *fsg);
int do_scsi_command(struct msc_slave_dev *fsg);
int finish_reply(struct msc_slave_dev *fsg);
#endif /* __MSC_H */
