/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA,
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MAT
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

#ifndef _PSRAM_H
#define _PSRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal_time.h>
#include <hal_def.h>
#include <stddef.h>
#include "xradio_hal_debug.h"
#include "sunxi_hal_common.h"
#include "hal_ccm.h"

/*
typedef enum
{
	HAL_OK      = 0,
	HAL_ERROR   = -1,
	HAL_BUSY    = -2,
	HAL_TIMEOUT = -3,
	HAL_INVALID = -4
} HAL_Status;

#ifdef __cplusplus
  #define   __I     volatile
#else
  #define   __I     volatile const
#endif
#define     __O     volatile
#define     __IO    volatile
*/
//--------PSRAM ADDR---------
extern uint8_t __PSRAM_BASE[];
extern uint8_t __PSRAM_END[];
extern uint8_t __PSRAM_LENGTH[];
extern uint8_t __psram_start__[];
extern uint8_t __psram_end__[];
extern uint8_t __psram_data_start__[];
extern uint8_t __psram_data_end__[];
extern uint8_t __psram_bss_start__[];
extern uint8_t __psram_bss_end__[];
extern uint8_t __DMAHEAP_PSRAM_BASE[];
extern uint8_t __DMAHEAP_PSRAM_END[];
extern uint8_t __DMAHEAP_PSRAM_LENGTH[];

//#define PSRAM_START_ADDR        ((uint32_t)(__PSRAM_BASE))
//#define PSRAM_END_ADDR          ((uint32_t)(__PSRAM_END))
//#define PSRAM_LENGTH            ((uint32_t)(__PSRAM_LENGTH))
#define PSRAM_START_ADDR	    0x08000000
#define PSRAM_END_ADDR		    0x087fffff

#define DMAHEAP_PSRAM_BASE          ((uint32_t)(__DMAHEAP_PSRAM_BASE))
#define DMAHEAP_PSRAM_END           ((uint32_t)(__DMAHEAP_PSRAM_END))
#define DMAHEAP_PSRAM_LENGTH        ((uint32_t)(__DMAHEAP_PSRAM_LENGTH))

//--------PSRAM OPI Command set-----------
#define Sync_Read                0x00
#define Sync_Write               0x80
#define Sync_Burst_Read          0x20
#define Sync_Burst_Write         0xA0
#define Mode_Reg_Read            0x40
#define Mode_Reg_Write           0xC0
#define Global_Reaet             0xFF

//--------OPI Mode Register Address---------
#define MR0                     0x00
#define MR1                     0x01
#define MR2                     0x02
#define MR3                     0x03
#define MR4                     0x04
#define MR5                     0x05
#define MR6                     0x06
#define MR7                     0x07

//--------------define by myself------------------
#define S_READ                          0x00
#define S_FAST_READ                     0x01
#define S_FAST_READ_QUAD                0x02
#define S_WRITE                         0x03
#define S_QAUD_WRITE                    0x04
#define Q_FAST_READ                     0x05
#define Q_FAST_READ_QUAD                0x06
#define Q_WRITE                         0x07
#define O_SYNC_READ                     0x08
#define O_SYNC_WRITE                    0x09
#define O_SYNC_BURST_READ               0x0A
#define O_SYNC_BURST_WRITE              0x0B

/*  chip information  */
#define PSRAM_CHIP_WB		0
#define PSRAM_CHIP_OPI_APS32    1
#define PSRAM_CHIP_OPI_APS64    2
#define PSRAM_CHIP_MAX          3

/*
 * PSRAM initialization parameters
 */
typedef struct {
	uint32_t p_type;
	uint32_t freq;          /*!< PSRAM Chip working frequency */
} PSRAMChip_InitParam;

struct psram_chip {
	uint8_t			id;
	uint8_t			type;

	uint8_t                 cbus_rcmd;
	uint8_t                 cbus_wcmd;
	uint8_t			mf_id;
	uint8_t                 kgd;

	uint32_t                die;

	uint32_t		buswidth;
	uint32_t                wrap_len;
	uint32_t                capacity;
	uint32_t                freq;
	char                    *name;
	struct psram_ctrl       *ctrl;
};

#define PSRAM_DATA_WRITE_BYTE   (1 << 0)
#define PSRAM_DATA_WRITE_SHORT  (1 << 1)
#define PSRAM_DATA_WRITE_WORD   (1 << 2)
#define PSRAM_DATA_WRITE_MASK   (0x07 << 0)
#define PSRAM_DATA_READ_BYTE    (1 << 4)
#define PSRAM_DATA_READ_SHORT   (1 << 5)
#define PSRAM_DATA_READ_WORD    (1 << 6)
#define PSRAM_DATA_READ_MASK    (0x07 << 4)

#define PSRAM_ADDR_PRESENT      (1 << 0)

struct psram_command {
	uint32_t opcode;
	uint32_t addr;
	uint8_t *resp;
	uint32_t rw_cfg;
	uint32_t flags;
};

struct psram_data {
	uint32_t blksz;
	uint32_t blocks;
	uint32_t flags;
	uint32_t rw_cfg;
	uint8_t *buff;
};

struct psram_request {
	struct psram_command 	cmd;
	struct psram_data	data;
};

int32_t psram_init(struct psram_chip *chip, struct psram_ctrl *ctrl, PSRAMChip_InitParam *param);
int32_t psram_deinit(struct psram_chip *chip);
int32_t Psram_Read_Mr(struct psram_chip *chip, uint32_t mreg);
int32_t Psram_Read_Id(struct psram_chip *chip);
int32_t Psram_Read_Info(struct psram_chip *chip);
int32_t Psram_Read_die_sta(struct psram_chip *chip);
void psram_idbus_op_cmd(struct psram_chip *chip, uint32_t opcmd);

int32_t psram_read_16byte_test(struct psram_chip *chip);
int32_t psram_set_write_latency(struct psram_chip *chip, uint32_t p_type, uint32_t wlc);
int32_t psram_set_read_latency(struct psram_chip *chip, uint32_t fixed);
int32_t psram_set_drv(struct psram_chip *chip, uint32_t drv);
int32_t psram_sw_reset(struct psram_chip *chip);
void dump_sta(void);
int platform_psram_init(void);
#endif
