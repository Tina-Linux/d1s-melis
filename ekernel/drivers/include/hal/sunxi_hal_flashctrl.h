/**
 * @file  hal_flashctrl.h
 * @author  XRADIO IOT WLAN Team
 */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_FLASHCTRL_H_
#define _DRIVER_CHIP_HAL_FLASHCTRL_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
  #define   __I     volatile             /* !< Defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /* !< Defines 'read only' permissions                 */
#endif
#define     __O     volatile             /* !< Defines 'write only' permissions                */
#define     __IO    volatile             /* !< Defines 'read / write' permissions              */

#if (CONFIG_CHIP_ARCH_VER == 3)
#define FLASH_XIP_OPT_READ //debug r128
//#define FLASH_XIP_OPT_WRITE /* close for not stable */
//#define FLASH_XIP_OPT_ERASR
#endif

// #define FLASH_DMA_TRANSFER_MIN_SIZE     (64)
#define FLASH_DMA_TRANSFER_MIN_SIZE     (64000) //no dma

//#define FLASH_ISPR_MASK_NUM             howmany(MAX_IRQn, 32)
#define FLASH_ISPR_MASK_NUM             32

#define FLASH_SWITCH_OUT_MS             2
#define FLASH_CHECK_BUSY_US             200
#define FLASH_BUSY_TIMEOUT_US           2000000


#ifdef	FLASH_XIP_OPT_ERASR
#define FLASH_REERASE_CHECK
#endif

#ifdef FLASH_XIP_OPT_WRITE
#define FLASH_REWRITE_CHECK
#endif

#define FLASHCTRL_ARCH_V2		(CONFIG_CHIP_ARCH_VER > 1)

typedef enum {
	FLASH_ADDR_XIP_IDX        = 0,
	FLASH_ADDR_PSRAM_IDX      = 1,
	FLASH_ADDR_USER_RD_IDX    = 2,
	FLASH_ADDR_TZ_XIP_IDX     = 3,
	FLASH_ADDR_TZ_PSRAM_IDX   = 4,
	FLASH_ADDR_FIELD_NUM      = 6
} FLASHC_BIAS_INDEX;

typedef struct {
	__IO uint32_t START_ADDR;       /* , Address offset: N * 0x4 + 0x00        */
	__IO uint32_t END_ADDR;         /* , Address offset: N * 0x4 + 0x04        */
	__IO uint32_t BIAS_ADDR;        /* , Address offset: N * 0x4 + 0x08        */
	__I  uint32_t RESERVE0C;        /* , Address offset: N * 0x4 + 0x0C        */
} FADDR_T;

typedef struct {
	__IO uint32_t MEM_COM_CONFG;                    /* , Address offset: 0x000 */
	__IO uint32_t SQPI_CTRL_COM_CONFG;              /* , Address offset: 0x004 */
	__IO uint32_t CACHE_RLVT_CONFG;                 /* , Address offset: 0x008 */
	__IO uint32_t MEM_AC_CHR_TIMING_CONFG;          /* , Address offset: 0x00C */
	__IO uint32_t CBUS_RD_OPRT_CONFG;               /* , Address offset: 0x010 */
	__IO uint32_t CBUS_WR_OPRT_CONFG;               /* , Address offset: 0x014 */
	__IO uint32_t CBUS_RD_DUMMY_DATA_TOP_HALF;      /* , Address offset: 0x018 */
	__IO uint32_t CBUS_RD_DUMMY_DATA_BUTT_HALF;     /* , Address offset: 0x01C */
	__IO uint32_t CBUS_WR_DUMMY_DATA_TOP_HALF;      /* , Address offset: 0x020 */
	__IO uint32_t CBUS_WR_DUMMY_DATA_BUTT_HALF;     /* , Address offset: 0x024 */
	__IO uint32_t CBUS_IO_SW_WAIT_TIME;             /* , Address offset: 0x028 */
	__IO uint32_t SBUS_RW_OPRT_CONFG;               /* , Address offset: 0x02C */
	__IO uint32_t SBUS_ADDR_CONFG;                  /* , Address offset: 0x030 */
	__IO uint32_t SBUS_DUMMY_DATA_TOP_HALF;         /* , Address offset: 0x034 */
	__IO uint32_t SBUS_DUMMY_DATA_BUTT_HALF;        /* , Address offset: 0x038 */
	__IO uint32_t SBUS_IO_SW_WAIT_TIME;             /* , Address offset: 0x03C */
	__IO uint32_t SBUS_WR_DATA_BYTE_NUM;            /* , Address offset: 0x040 */
	__IO uint32_t SBUS_RD_DATA_BYTE_NUM;            /* , Address offset: 0x044 */
	__IO uint32_t SBUS_START_SEND_REG;              /* , Address offset: 0x048 */
	__IO uint32_t FIFO_TRIGGER_LEVEL;               /* , Address offset: 0x04C */
	__I  uint32_t FIFO_STATUS_REG;                  /* , Address offset: 0x050 */
	__IO uint32_t INT_ENABLE_REG;                   /* , Address offset: 0x054 */
	__IO uint32_t INT_STATUS_REG;                   /* , Address offset: 0x058 */
	__IO uint32_t XIP_WARP_MODE_EXE_IDCT;           /* , Address offset: 0x05C */
	__I  uint32_t MEM_CTRL_DEBUG_STATE;             /* , Address offset: 0x060 */
	__I  uint32_t DEBUG_CNT_SBUS_WR;                /* , Address offset: 0x064 */
	__I  uint32_t DEBUG_CNT_SBUS_RD;                /* , Address offset: 0x068 */
	__IO uint32_t NOP_INSTRUCTION;                  /* , Address offset: 0x06C */
	__IO uint32_t CBUS_RW_EN_WTIME;                 /* , Address offset: 0x070 */
	__I  uint32_t DEBUG_CNT_CBUS_WR;                /* , Address offset: 0x074 */
	__I  uint32_t DEBUG_CNT_CBUS_RD;                /* , Address offset: 0x078 */
	__IO uint32_t RESERVE7C;
	     FADDR_T  FLASH_ADDR[FLASH_ADDR_FIELD_NUM]; /* , Address offset: 0x080~0x0DC */
	__IO uint32_t RESERVEC0[32 - (FLASH_ADDR_FIELD_NUM * 4)]; /* , Address offset: 0x0E0~0x0FC */
	__IO uint32_t SBUS_WR_DATA_REG;                 /* , Address offset: 0x100 */
	__IO uint32_t RESERVE104[63];
	__I  uint32_t SBUS_RD_DATA_REG;                 /* , Address offset: 0x200 */

#if 0
	/* NOTE: the same with psram ctrl define, change that define too if necessary */
	     uint32_t RESERVE204[383];
	__IO uint32_t PMEM_COM_CFG;                     /* , Address offset: 0x800 */
	__IO uint32_t POPI_CTRL_COM_CFG;                /* , Address offset: 0x804 */
	__IO uint32_t PCACHE_RLVT_CFG;                  /* , Address offset: 0x808 */
	__IO uint32_t RESERVE80C;
	__IO uint32_t PC_RD_OPRT_CFG;                   /* , Address offset: 0x810 */
	__IO uint32_t PC_WD_OPRT_CFG;                   /* , Address offset: 0x814 */
	__IO uint32_t PC_RD_DUMMY_DATA_H;               /* , Address offset: 0x818 */
	__IO uint32_t PC_RD_DUMMY_DATA_L;               /* , Address offset: 0x81C */
	__IO uint32_t PC_WD_DUMMY_DATA_H;               /* , Address offset: 0x820 */
	__IO uint32_t PC_WD_DUMMY_DATA_L;               /* , Address offset: 0x824 */
	__IO uint32_t PC_IO_SW_WAIT_TIME;               /* , Address offset: 0x828 */
	__IO uint32_t RESERVE82C[16];
	__IO uint32_t PSRAM_FORCE_CFG;                  /* , Address offset: 0x86C */
	__IO uint32_t PSRAM_COM_CFG;                    /* , Address offset: 0x870 */
#endif
} OPI_MEM_CTRL_T;

#define OPI_MEM_CTRL ((OPI_MEM_CTRL_T *)FLASH_CTRL_BASE)

/* MEM_COM_CONFG: Memory Controller Common Configuration, 0x00 */
#define FLASHC_CBUS_RW_ENABLE_SHIFT                     0
#define FLASHC_CBUS_RW_ENABLE_MASK                      (0x1U << FLASHC_CBUS_RW_ENABLE_SHIFT)
#define FLASHC_CBUS_RW_ENABLE_BIT                       (0x1U << FLASHC_CBUS_RW_ENABLE_SHIFT)
#define FLASHC_XIP_DEBUG_SHIFT                          1
#define FLASHC_XIP_DEBUG_MASK                           (0x1U << FLASHC_XIP_DEBUG_SHIFT)
#define FLASHC_XIP_ENABLE_SHIFT                         2
#define FLASHC_XIP_ENABLE_MASK                          (0x1U << FLASHC_XIP_ENABLE_SHIFT)
#define FLASHC_XIP_ENABLE_BIT                           (0x1U << FLASHC_XIP_ENABLE_SHIFT)
#define FLASHC_WRAP_AROUND_ENABLE_SHIFT                 3
#define FLASHC_WRAP_AROUND_ENABLE_MASK                  (0x1U << FLASHC_WRAP_AROUND_ENABLE_SHIFT)
#if (CONFIG_CHIP_ARCH_VER == 2)
#define FLASHC_FLASH_PSRAM_SELECT_SHIFT                 4
#define FLASHC_FLASH_PSRAM_SELECT_MASK                  (0x1U << FLASHC_FLASH_PSRAM_SELECT_SHIFT)
#elif (CONFIG_CHIP_ARCH_VER == 3)
#define FLASHC_ARBITER_EN_SHIFT                         4
#define FLASHC_ARBITER_EN_MASK                          (0x1U << FLASHC_ARBITER_EN_SHIFT)
#endif
#define FLASHC_REV_FIFO_RESET_SHIFT                     5
#define FLASHC_REV_FIFO_RESET_MASK                      (0x1U << FLASHC_REV_FIFO_RESET_SHIFT)
#define FLASHC_TRAN_FIFO_RESET_SHIFT                    6
#define FLASHC_TRAN_FIFO_RESET_MASK                     (0x1U << FLASHC_TRAN_FIFO_RESET_SHIFT)
#define FLASHC_SBUS_HREADY_WAITTIME_OUT_ENABLE_SHIFT    7
#define FLASHC_SBUS_HREADY_WAITTIME_OUT_ENABLE_MASK     (0x1U << FLASHC_SBUS_HREADY_WAITTIME_OUT_ENABLE_SHIFT)
#define FLASHC_SBUS_HREADY_ADJ_WAITTIME_OUT_SHIFT       8
#define FLASHC_SBUS_HREADY_ADJ_WAITTIME_OUT_MASK        (0xFFU<< FLASHC_SBUS_HREADY_ADJ_WAITTIME_OUT_SHIFT)
#if (CONFIG_CHIP_ARCH_VER == 2)
#define FLASHC_IO7_VACANCY_OUT_SHIFT                    16
#define FLASHC_IO7_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO7_VACANCY_OUT_SHIFT)
#define FLASHC_IO6_VACANCY_OUT_SHIFT                    18
#define FLASHC_IO6_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO6_VACANCY_OUT_SHIFT)
#define FLASHC_IO5_VACANCY_OUT_SHIFT                    20
#define FLASHC_IO5_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO5_VACANCY_OUT_SHIFT)
#define FLASHC_IO4_VACANCY_OUT_SHIFT                    22
#define FLASHC_IO4_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO4_VACANCY_OUT_SHIFT)
#endif
#define FLASHC_IO3_VACANCY_OUT_SHIFT                    24
#define FLASHC_IO3_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO3_VACANCY_OUT_SHIFT)
#define FLASHC_IO2_VACANCY_OUT_SHIFT                    26
#define FLASHC_IO2_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO2_VACANCY_OUT_SHIFT)
#define FLASHC_IO1_VACANCY_OUT_SHIFT                    28
#define FLASHC_IO1_VACANCY_OUT_MASK                     (0x3U << FLASHC_IO1_VACANCY_OUT_SHIFT)
#define FLASHC_ADDR_SIZE_MODE                           30
#define FLASHC_ADDR_SIZE_MODE_MASK                      (0x3U << FLASHC_ADDR_SIZE_MODE)

/* SQPI_CTRL_COM_CONFG: OPI Controller Common Configuration, 0x04 */
#define FLASHC_SPI_CPHA_CTRL_SHIFT                      0
#define FLASHC_SPI_CPHA_CTRL_MASK                       (0x1U << FLASHC_SPI_CPHA_CTRL_SHIFT)
#define FLASHC_SPI_CPOL_CTRL_SHIFT                      1
#define FLASHC_SPI_CPOL_CTRL_MASK                       (0x1U << FLASHC_SPI_CPOL_CTRL_SHIFT)
typedef enum {
	FC_SCLK_Mode0 = 0 << FLASHC_SPI_CPHA_CTRL_SHIFT, /* 0:Phase 0(Leading edge for sample data, 0:Active high polarity) */
	FC_SCLK_Mode1 = 1 << FLASHC_SPI_CPHA_CTRL_SHIFT, /* 1:Phase 1(Leading edge for setup data,  0:Active high polarity) */
	FC_SCLK_Mode2 = 2 << FLASHC_SPI_CPHA_CTRL_SHIFT, /* 0:Phase 0(Leading edge for sample data, 1:Active low polarity) */
	FC_SCLK_Mode3 = 3 << FLASHC_SPI_CPHA_CTRL_SHIFT, /* 1:Phase 0(Leading edge for setup data,  1:Active low polarity) */
} FC_Sclk_Mode;
#define FLASHC_FIRST_RCV_BIT_SLT_SHIFT                  4
#define FLASHC_FIRST_RCV_BIT_SLT_MASK                   (0x1U << FLASHC_FIRST_RCV_BIT_SLT_SHIFT)
typedef enum {
	FC_TCTRL_FBS_MSB = 0 << FLASHC_FIRST_RCV_BIT_SLT_SHIFT,
	FC_TCTRL_FBS_LSB = 1 << FLASHC_FIRST_RCV_BIT_SLT_SHIFT
} FC_TCTRL_Fbs;
#define FLASHC_FLASH_CS_POL_SHIFT                       8
#define FLASHC_FLASH_CS_POL_MASK                        (0x1U << FLASHC_FLASH_CS_POL_SHIFT)
typedef enum {
	FC_TCTRL_CS_LOW_ENABLE = 0 << FLASHC_FLASH_CS_POL_SHIFT,
	FC_TCTRL_CS_HIGH_ENABLE = 1 << FLASHC_FLASH_CS_POL_SHIFT
} FC_Cs;
#define FLASHC_WAIT_HALF_CYCLE_SHIFT                    12
#define FLASHC_WAIT_HALF_CYCLE_MASK                     (0x3U << FLASHC_WAIT_HALF_CYCLE_SHIFT)

#define FLASHC_CS_OUTPUT_SEL_SHIFT                      15
#define FLASHC_CS_OUTPUT_SEL_MASK                       (0x3U << FLASHC_CS_OUTPUT_SEL_SHIFT)
#define FLASHC_FLASHCS0_PSRAMCS1                        (0 << FLASHC_CS_OUTPUT_SEL_SHIFT)
#define FLASHC_FLASHCS1_PSRAMCS0                        (1 << FLASHC_CS_OUTPUT_SEL_SHIFT)
#define FLASHC_FLASH_CS_ALL                             (2 << FLASHC_CS_OUTPUT_SEL_SHIFT)
#define FLASHC_PSRAM_CS_ALL                             (3 << FLASHC_CS_OUTPUT_SEL_SHIFT)

#define FLASHC_CLK_SEL_EN_SHIFT                         17
#define FLASHC_CLK_SEL_EN_MASK                          (0x1U << FLASHC_CLK_SEL_EN_SHIFT)
#define FLASHC_SINGLE_DEV_EN_SHIFT                      18
#define FLASHC_SINGLE_DEV_EN_MASK                       (0x1U << FLASHC_SINGLE_DEV_EN_SHIFT)
#define FLASHC_SINGLE_DEV_SEL_SHIFT                     19
#define FLASHC_SINGLE_DEV_SEL_MASK                      (0x1U << FLASHC_SINGLE_DEV_SEL_SHIFT)
typedef enum {
	FC_DEV_FLASH = 0 << FLASHC_SINGLE_DEV_SEL_SHIFT,
	FC_DEV_PSRAM = 1 << FLASHC_SINGLE_DEV_SEL_SHIFT,
} FC_Sel_Dev;

/* CACHE_RLVT_CONFG: Cache Relevant Configuration, 0x08 */
#define FLASHC_RD_CACHE_LINE_LEN_CONFG_SHIFT            0 /* read cache line */
#define FLASHC_RD_CACHE_LINE_LEN_CONFG_MASK             (0x3U << FLASHC_RD_CACHE_LINE_LEN_CONFG_SHIFT)
typedef enum {
	FC_RD_CACHE_LINE_LEN_8B = 0 << FLASHC_RD_CACHE_LINE_LEN_CONFG_SHIFT,
	FC_RD_CACHE_LINE_LEN_16B = 1 << FLASHC_RD_CACHE_LINE_LEN_CONFG_SHIFT,
	FC_RD_CACHE_LINE_LEN_32B = 2 << FLASHC_RD_CACHE_LINE_LEN_CONFG_SHIFT,
} FC_RdCacheLineLenCfg;
#define FLASHC_CBUS_WR_SIZE_SELECT_SHIFT                2
#define FLASHC_CBUS_WR_SIZE_SELECT_BIT                  (0x1U << FLASHC_CBUS_WR_SIZE_SELECT_SHIFT)
#define FLASHC_CBUS_HREADY_TIME_OUT_SHIFT               3
#define FLASHC_CBUS_HREADY_TIME_OUT_BIT                 (0x1U << FLASHC_CBUS_HREADY_TIME_OUT_SHIFT)
#define FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_SHIFT      4
#define FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_VMASK      (0xFFU)
#define FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_MASK       (FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_VMASK << FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_SHIFT)
#define FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT(wt)        (((wt) & FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_VMASK) << FLASHC_CBUS_HREADY_ADY_WAIT_TIME_OUT_SHIFT)

#if (CONFIG_CHIP_ARCH_VER == 3)
#define FLASHC_WT_CACHE_LINE_LEN_CONFG_SHIFT            12 /* write cache line */
#define FLASHC_WT_CACHE_LINE_LEN_CONFG_MASK             (0x3U << FLASHC_WT_CACHE_LINE_LEN_CONFG_SHIFT)
typedef enum {
	FC_WT_CACHE_LINE_LEN_8B = 0 << FLASHC_WT_CACHE_LINE_LEN_CONFG_SHIFT,
	FC_WT_CACHE_LINE_LEN_16B = 1 << FLASHC_WT_CACHE_LINE_LEN_CONFG_SHIFT,
	FC_WT_CACHE_LINE_LEN_32B = 2 << FLASHC_WT_CACHE_LINE_LEN_CONFG_SHIFT,
	FC_WT_CACHE_LINE_LEN_64B = 3 << FLASHC_WT_CACHE_LINE_LEN_CONFG_SHIFT,
} FC_WtCacheLneLenCfg;
#endif

/* MEM_AC_CHR_TIMING_CONFG: Memory AC Charachter Timing Configuration, 0x0C */
#define FLASHC_CBUS_SHSL_SHIFT                          0
#define FLASHC_CBUS_SHSL_MASK                           (0xFFU << FLASHC_CBUS_SHSL_SHIFT)
#define FLASHC_SBUS_SHSL_SHIFT                          8
#define FLASHC_SBUS_SHSL_MASK                           (0xFFU << FLASHC_SBUS_SHSL_SHIFT)
#define FLASHC_SPI_FLASH_CHSH_SHIFT                     16
#define FLASHC_SPI_FLASH_CHSH_MASK                      (0xFFU << FLASHC_SPI_FLASH_CHSH_SHIFT)
#define FLASHC_SPI_FLASH_SLCH_SHIFT                     24
#define FLASHC_SPI_FLASH_SLCH_MASK                      (0xFFU << FLASHC_SPI_FLASH_SLCH_SHIFT)

/*
 * Bit field definition of
 *     - CBUS_RD_OPRT_CONFG
 *     - CBUS_WR_OPRT_CONFG
 *     - SBUS_RW_OPRT_CONFG
 */
#define FLASHC_BUS_RW_CMD_SHIFT                         (24)
#define FLASHC_BUS_RW_CMD_VMASK                         (0x0FFU)
#define FLASHC_BUS_RW_CMD_MASK                          (FLASHC_BUS_RW_CMD_VMASK << FLASHC_BUS_RW_CMD_SHIFT)
#define FLASHC_BUS_RW_CMD(cmd)                          (((cmd) & FLASHC_BUS_RW_CMD_VMASK) << FLASHC_BUS_RW_CMD_SHIFT)

#define FLASHC_BUS_CMD_BIT_SHIFT                        (20)
#define FLASHC_BUS_CMD_BIT_MASK                         (0x7U << FLASHC_BUS_CMD_BIT_SHIFT)

#define FLASHC_BUS_ADDR_BIT_SHIFT                       (16)
#define FLASHC_BUS_ADDR_BIT_MASK                        (0x7U << FLASHC_BUS_ADDR_BIT_SHIFT)

#define FLASHC_BUS_DUMY_BIT_SHIFT                       (12)
#define FLASHC_BUS_DUMY_BIT_MASK                        (0x7U << FLASHC_BUS_DUMY_BIT_SHIFT)

#define FLASHC_BUS_DUMMY_WID_SHIFT                      (4)
#define FLASHC_BUS_DUMMY_WID_MASK                       (0x3FU << FLASHC_BUS_DUMMY_WID_SHIFT)

#define FLASHC_BUS_DATA_BIT_SHIFT                       (0)
#define FLASHC_BUS_DATA_BIT_MASK                        (0x7U << FLASHC_BUS_DATA_BIT_SHIFT)

/* CBUS_RD_OPRT_CONFG: Code-bus Read Operation Configuration, 0x10 */
#if (CONFIG_CHIP_ARCH_VER == 3)
#define	FLASHC_READ_DMA_EN                              HAL_BIT(23)
#endif

/* CBUS_WR_OPRT_CONFG: Code-bus Write Operation Configuration, 0x14 */

/* CBUS_RD_DUMMY_DATA_TOP_HALF: Code-bus Read Dummy Data Top Half, 0x18 */
#define FLASHC_RD_DUMMY_DATA_TOP_HALF_SHIFT             0
#define FLASHC_RD_DUMMY_DATA_TOP_HALF_MASK              (0xFFFFFFFFU << FLASHC_RD_DUMMY_DATA_TOP_HALF_SHIFT)

/* CBUS_RD_DUMMY_DATA_BUTT_HALF: Code-bus Read Dummy Data Buttom Half, 0x1C */
#define FLASHC_RD_DUMMY_DATA_BUTTOM_HALF_SHIFT          0
#define FLASHC_RD_DUMMY_DATA_BUTTOM_HALF_MASK           (0xFFFFFFFFU << FLASHC_RD_DUMMY_DATA_BUTTOM_HALF_SHIFT)

/* CBUS_WR_DUMMY_DATA_TOP_HALF: Code-bus Write Dummy Data Top Half, 0x20 */
#define FLASHC_WR_DUMMY_DATA_TOP_HALF_SHIFT             0
#define FLASHC_WR_DUMMY_DATA_TOP_HALF_MASK              (0xFFFFFFFFU << FLASHC_WR_DUMMY_DATA_TOP_HALF_SHIFT)

/* CBUS_WR_DUMMY_DATA_BUTT_HALF: Code-bus Write Dummy Data Buttom Half, 0x24 */
#define FLASHC_WR_DUMMY_DATA_BUTTOM_HALF_SHIFT          0
#define FLASHC_WR_DUMMY_DATA_BUTTOM_HALF_MASK           (0xFFFFFFFFU << FLASHC_WR_DUMMY_DATA_BUTTOM_HALF_SHIFT)

/* CBUS_IO_SW_WAIT_TIME: Code-bus IO Switch Wait Time, 0x028 */
#if (CONFIG_CHIP_ARCH_VER == 2)
#define FLASHC_CBUS_DUMMY_WAIT_CYCLE_SHIFT              0
#define FLASHC_CBUS_DUMMY_WAIT_CYCLE_MASK               (0xFFU << FLASHC_CBUS_DUMMY_WAIT_CYCLE_SHIFT)
#define FLASHC_CBUS_ADDR_WAIT_CYCLE_SHIFT               8
#define FLASHC_CBUS_ADDR_WAIT_CYCLE_MASK                (0xFFU << FLASHC_CBUS_ADDR_WAIT_CYCLE_SHIFT)
#define FLASHC_CBUS_CMD_WAIT_CYCLE_SHIFT                16
#define FLASHC_CBUS_CMD_WAIT_CYCLE_MASK                 (0xFFU << FLASHC_CBUS_CMD_WAIT_CYCLE_SHIFT)
#endif
#define FLASHC_CBUS_LATENCY_WAIT_CYCLE_SHIFT            24
#define FLASHC_CBUS_LATENCY_WAIT_CYCLE_MASK             (0xFFU << FLASHC_CBUS_LATENCY_WAIT_CYCLE_SHIFT)

/* SBUS_RW_OPRT_CONFG: System-bus Read/Write Operation Configuration, 0x02C */
#if (CONFIG_CHIP_ARCH_VER == 3)
#define FLASHC_SBUS_DEV_SEL_SHIFT                       23
#define FLASHC_SBUS_DEV_SEL_MASK                        (0x1U << FLASHC_SBUS_DEV_SEL_SHIFT)
#define FLASHC_SBUS_DEV_SEL_FLASH                       (0x0U << FLASHC_SBUS_DEV_SEL_SHIFT)
#define FLASHC_SBUS_DEV_SEL_PSRAM                       (0x1U << FLASHC_SBUS_DEV_SEL_SHIFT)
#endif

/* SBUS_ADDR_CONFG: System-bus Address Configuration, 0x030 */
#define FLASHC_SBUS_ADDR_SHIFT                          0
#define FLASHC_SBUS_ADDR_MASK                           (0xFFFFFFFFU << FLASHC_SBUS_ADDR_SHIFT)

/* SBUS_DUMMY_DATA_TOP_HALF: System-bus Dummy Data Top half, 0x034 */
#define FLASHC_SBUS_DUMMY_TOP_HALF_SHIFT                0
#define FLASHC_SBUS_DUMMY_TOP_HALF_MASK                 (0xFFFFFFFFU << FLASHC_SBUS_DUMMY_TOP_HALF_SHIFT)

/* SBUS_DUMMY_DATA_BUTT_HALF: System-bus Dummy Data Buttom half, 0x038 */
#define FLASHC_SBUS_DUMMY_BUTTOM_HALF_SHIFT             0
#define FLASHC_SBUS_DUMMY_BUTTOM_HALF_MASK              (0xFFFFFFFFU << FLASHC_SBUS_DUMMY_BUTTOM_HALF_SHIFT)

/* SBUS_IO_SW_WAIT_TIME: System-bus IO Switch Wait Time, 0x03C */
#if (CONFIG_CHIP_ARCH_VER == 2)
#define FLASHC_SBUS_DUMMY_WAIT_CYCLE_SHIFT              0
#define FLASHC_SBUS_DUMMY_WAIT_CYCLE_MASK               (0xFFU << FLASHC_SBUS_DUMMY_WAIT_CYCLE_SHIFT)
#define FLASHC_SBUS_ADDR_WAIT_CYCLE_SHIFT               8
#define FLASHC_SBUS_ADDR_WAIT_CYCLE_MASK                (0xFFU << FLASHC_SBUS_ADDR_WAIT_CYCLE_SHIFT)
#define FLASHC_SBUS_CMD_WAIT_CYCLE_SHIFT                16
#define FLASHC_SBUS_CMD_WAIT_CYCLE_MASK                 (0xFFU << FLASHC_SBUS_CMD_WAIT_CYCLE_SHIFT)
#endif
#define FLASHC_SBUS_LATENCY_WAIT_CYCLE_SHIFT            24
#define FLASHC_SBUS_LATENCY_WAIT_CYCLE_MASK             (0xFFU << FLASHC_SBUS_LATENCY_WAIT_CYCLE_SHIFT)

/* SBUS_WR_DATA_BYTE_NUM: System-bus Write Data Byte Number, 0x040 */
#define FLASHC_SBUS_WR_BYTE_SHIFT                       0
#define FLASHC_SBUS_WR_BYTE_MASK                        (0xFFFFFFFFU << FLASHC_SBUS_WR_BYTE_SHIFT)

/* SBUS_RD_DATA_BYTE_NUM: Sytem-bus Read Data Byte Number, 0x044 */
#define FLASHC_SBUS_RD_BYTE_SHIFT                       0
#define FLASHC_SBUS_RD_BYTE_MASK                        (0xFFFFFFFFU << FLASHC_SBUS_RD_BYTE_SHIFT)

/* SBUS_START_SEND_REG: System-bus Start Send Register, 0x048 */
#define FLASHC_ENABLE_SBUS_SHIFT                        0
#define FLASHC_ENABLE_SBUS_MASK                         (0x1U << FLASHC_ENABLE_SBUS_SHIFT)

/* FIFO Trigger Level */
#define FLASHC_RD_FIFO_EMPTY_REQ_SHIFT                  0
#define FLASHC_RD_FIFO_EMPTY_REQ_MASK                   (0xFFU << FLASHC_RD_FIFO_EMPTY_REQ_SHIFT)
#define FLASHC_RD_FIFO_FULL_REQ_SHIFT                   8
#define FLASHC_RD_FIFO_FULL_REQ_MASK                    (0xFFU << FLASHC_RD_FIFO_FULL_REQ_SHIFT)
#define FLASHC_WR_FIFO_EMPTY_REQ_SHIFT                  16
#define FLASHC_WR_FIFO_EMPTY_REQ_MASK                   (0xFFU << FLASHC_WR_FIFO_EMPTY_REQ_SHIFT)
#define FLASHC_WR_FIFO_FULL_REQ_SHIFT                   24
#define FLASHC_WR_FIFO_FULL_REQ_MASK                    (0xFFU << FLASHC_WR_FIFO_FULL_REQ_SHIFT)

/* FIFO_STATUS_REG: FIFO Status Register */
#define FLASHC_RD_FIFO_COUNTER_SHIFT                    0
#define FLASHC_RD_FIFO_COUNTER_MASK                     (0xFFU <<FLASHC_RD_FIFO_COUNTER_SHIFT)
#define FLASHC_RD_FIFO_COUNTER_VMASK                    (0xFFU)
#define FLASHC_WR_FIFO_COUNTER_SHIFT                    8
#define FLASHC_WR_FIFO_COUNTER_MASK                     (0xFFU <<FLASHC_WR_FIFO_COUNTER_SHIFT)
#define FLASHC_WR_FIFO_COUNTER_VMASK                    (0xFFU)
#define FLASHC_RD_BUFF_COUNTER_SHIFT                    16
#define FLASHC_RD_BUFF_COUNTER_MASK                     (0x7U << FLASHC_RD_BUFF_COUNTER_SHIFT)
#define FLASHC_RD_BUFF_COUNTER_VMASK                    (0x7U)
#define FLASHC_RD_BUFF_RD_STATUS_SHIFT                  19
#define FLASHC_RD_BUFF_RD_STATUS_MASK                   (0x1U << FLASHC_RD_BUFF_RD_STATUS_SHIFT)
#define FLASHC_WR_BUFF_COUNTER_VMASK                    (0x7U)
#define FLASHC_WR_BUFF_COUNTER_SHIFT                    20
#define FLASHC_WR_BUFF_COUNTER_MASK                     (0x7U << FLASHC_WR_BUFF_COUNTER_SHIFT)
#define FLASHC_RD_BUFF_RD_STATUS_VMASK                  (0x1U)
#define FLASHC_WR_BUFF_RD_STATUS_SHIFT                  23
#define FLASHC_WR_BUFF_RD_STATUS_MASK                   (0x1U << FLASHC_WR_BUFF_RD_STATUS_SHIFT)
#define FLASHC_WR_BUFF_RD_STATUS_VMASK                  (0x1U)

/* Interrupt Enable Register */
#define FLASHC_RD_FIFO_REQ_INT_ENABLE_SHIFT             0
#define FLASHC_RD_FIFO_REQ_INT_ENABLE_MASK              (0x1U << FLASHC_RD_FIFO_REQ_INT_ENABLE_SHIFT)
#define FLASHC_RD_FIFO_EMPTY_INT_ENABLE_SHIFT           1
#define FLASHC_RD_FIFO_EMPTY_INT_ENABLE_MASK            (0x1U << FLASHC_RD_FIFO_EMPTY_INT_ENABLE_SHIFT)
#define FLASHC_RD_FIFO_FULL_INT_ENABLE_SHIFT            2
#define FLASHC_RD_FIFO_FULL_INT_ENABLE_MASK             (0x1U << FLASHC_RD_FIFO_FULL_INT_ENABLE_SHIFT)
#define FLASHC_WR_FIFO_REQ_INT_ENABLE_SHIFT             4
#define FLASHC_WR_FIFO_REQ_INT_ENABLE_MASK              (0x1U << FLASHC_WR_FIFO_REQ_INT_ENABLE_SHIFT)
#define FLASHC_WR_FIFO_EMPTY_INT_ENABLE_SHIFT           5
#define FLASHC_WR_FIFO_EMPTY_INT_ENABLE_MASK            (0x1U << FLASHC_WR_FIFO_EMPTY_INT_ENABLE_SHIFT)
#define FLASHC_WR_FIFO_FULL_INT_ENABLE_SHIFT            6
#define FLASHC_WR_FIFO_FULL_INT_ENABLE_MASK             (0x1U << FLASHC_WR_FIFO_FULL_INT_ENABLE_SHIFT)
#define FLASHC_RD_FIFO_OVERFLOW_INT_ENABLE_SHIFT        8
#define FLASHC_RD_FIFO_OVERFLOW_INT_ENABLE_MASK         (0x1U << FLASHC_RD_FIFO_OVERFLOW_INT_ENABLE_SHIFT)
#define FLASHC_RD_FIFO_UNDERFLOW_INT_ENABLE_SHIFT       9
#define FLASHC_RD_FIFO_UNDERFLOW_INT_ENABLE_MASK        (0x1U << FLASHC_RD_FIFO_UNDERFLOW_INT_ENABLE_SHIFT)
#define FLASHC_WR_FIFO_OVERFLOW_INT_ENABLE_SHIFT        10
#define FLASHC_WR_FIFO_OVERFLOW_INT_ENABLE_MASK         (0x1U << FLASHC_WR_FIFO_OVERFLOW_INT_ENABLE_SHIFT)
#define FLASHC_WR_FIFO_UNDERFLOW_INT_ENABLE_SHIFT       11
#define FLASHC_WR_FIFO_UNDERFLOW_INT_ENABLE_MASK        (0x1U << FLASHC_WR_FIFO_UNDERFLOW_INT_ENABLE_SHIFT)
#define FLASHC_TRAN_COMPLETED_INT_ENABLE_SHIFT          12
#define FLASHC_TRAN_COMPLETED_INT_ENABLE_MASK           (0x1U << FLASHC_TRAN_COMPLETED_INT_ENABLE_SHIFT)
#define FLASHC_SBUS_HREADY_TIME_OUT_INT_ENABLE_SHIFT    13
#define FLASHC_SBUS_HREADY_TIME_OUT_INT_ENABLE_MASK     (0x1U << FLASHC_SBUS_HREADY_TIME_OUT_INT_ENABLE_SHIFT)
#if (CONFIG_CHIP_ARCH_VER == 3)
#define FLASHC_PSRAM_LATENCY_TIME_OUT_INT_ENABLE_SHIFT  14
#define FLASHC_PSRAM_LATENCY_TIME_OUT_INT_ENABLE_MASK   (0x1U << FLASHC_PSRAM_LATENCY_TIME_OUT_INT_ENABLE_SHIFT)
#define FLASHC_DMA_ADDR_NO_CONFIG_INT_ENABLE_SHIFT      15
#define FLASHC_DMA_ADDR_NO_CONFIG_INT_ENABLE_MASK       (0x1U << FLASHC_DMA_ADDR_NO_CONFIG_INT_ENABLE_SHIFT)
#define FLASHC_START_SEND_HW_CLEAR_INT_ENABLE_SHIFT     16
#define FLASHC_START_SEND_HW_CLEAR_INT_ENABLE_MASK      (0x1U << FLASHC_START_SEND_HW_CLEAR_INT_ENABLE_SHIFT)
#define FLASHC_CBUS_READ_NOP_INT_ENABLE_SHIFT           17
#define FLASHC_CBUS_READ_NOP_INT_ENABLE_MASK            (0x1U << FLASHC_CBUS_READ_NOP_INT_ENABLE_SHIFT)
#endif

/* Interrupt Status Register */
#define FLASHC_RD_FIFO_REQ_INT_FLAG_SHIFT               0
#define FLASHC_RD_FIFO_REQ_INT_FLAG_MASK                (0x1U << FLASHC_RD_FIFO_REQ_INT_FLAG_SHIFT)
#define FLASHC_RD_FIFO_EMPTY_INT_FLAG_SHIFT             1
#define FLASHC_RD_FIFO_EMPTY_INT_FLAG_MASK              (0x1U << FLASHC_RD_FIFO_EMPTY_INT_FLAG_SHIFT)
#define FLASHC_RD_FIFO_FULL_INT_FLAG_SHIFT              2
#define FLASHC_RD_FIFO_FULL_INT_FLAG_MASK               (0x1U << FLASHC_RD_FIFO_FULL_INT_FLAG_SHIFT)
#define FLASHC_WR_FIFO_REQ_INT_FLAG_SHIFT               4
#define FLASHC_WR_FIFO_REQ_INT_FLAG_MASK                (0x1U << FLASHC_WR_FIFO_REQ_INT_FLAG_SHIFT)
#define FLASHC_WR_FIFO_EMPTY_INT_FLAG_SHIFT             5
#define FLASHC_WR_FIFO_EMPTY_INT_FLAG_MASK              (0x1U << FLASHC_WR_FIFO_EMPTY_INT_FLAG_SHIFT)
#define FLASHC_WR_FIFO_FULL_INT_FLAG_SHIFT              6
#define FLASHC_WR_FIFO_FULL_INT_FLAG_MASK               (0x1U << FLASHC_WR_FIFO_FULL_INT_FLAG_SHIFT)
#define FLASHC_RD_FIFO_OVERFLOW_INT_FLAG_SHIFT          8
#define FLASHC_RD_FIFO_OVERFLOW_INT_FLAG_MASK           (0x1U << FLASHC_RD_FIFO_OVERFLOW_INT_FLAG_SHIFT)
#define FLASHC_RD_FIFO_UNDERFLOW_INT_FLAG_SHIFT         9
#define FLASHC_RD_FIFO_UNDERFLOW_INT_FLAG_MASK          (0x1U << FLASHC_RD_FIFO_UNDERFLOW_INT_FLAG_SHIFT)
#define FLASHC_WR_FIFO_OVERFLOW_INT_FLAG_SHIFT          10
#define FLASHC_WR_FIFO_OVERFLOW_INT_FLAG_MASK           (0x1U << FLASHC_WR_FIFO_OVERFLOW_INT_FLAG_SHIFT)
#define FLASHC_WR_FIFO_UNDERFLOW_INT_FLAG_SHIFT         11
#define FLASHC_WR_FIFO_UNDERFLOW_INT_FLAG_MASK          (0x1U << FLASHC_WR_FIFO_UNDERFLOW_INT_FLAG_SHIFT)
#define FLASHC_TRAN_COMPLETED_INT_FLAG_SHIFT            12
#define FLASHC_TRAN_COMPLETED_INT_FLAG_MASK             (0x1U << FLASHC_TRAN_COMPLETED_INT_FLAG_SHIFT)
#define FLASHC_SBUS_HREADY_TIME_OUT_INT_FLAG_SHIFT      13
#define FLASHC_SBUS_HREADY_TIME_OUT_INT_FLAG_MASK       (0x1U << FLASHC_SBUS_HREADY_TIME_OUT_INT_FLAG_SHIFT)
#if (CONFIG_CHIP_ARCH_VER == 3)
#define FLASHC_PSRAM_LATENCY_TIME_OUT_INT_FLAG_SHIFT    14
#define FLASHC_PSRAM_LATENCY_TIME_OUT_INT_FLAG_MASK     (0x1U << FLASHC_PSRAM_LATENCY_TIME_OUT_INT_FLAG_SHIFT)
#define FLASHC_DMA_ADDR_NO_CONFIG_INT_FLAG_SHIFT        15
#define FLASHC_DMA_ADDR_NO_CONFIG_INT_FLAG_MASK         (0x1U << FLASHC_DMA_ADDR_NO_CONFIG_INT_FLAG_SHIFT)
#define FLASHC_START_SEND_HW_CLEAR_INT_FLAG_SHIFT       16
#define FLASHC_START_SEND_HW_CLEAR_INT_FLAG_MASK        (0x1U << FLASHC_START_SEND_HW_CLEAR_INT_FLAG_SHIFT)
#define FLASHC_CBUS_READ_NOP_INT_FLAG_SHIFT             17
#define FLASHC_CBUS_READ_NOP_INT_FLAG_MASK              (0x1U << FLASHC_CBUS_READ_NOP_INT_FLAG_SHIFT)
#endif

/* XIP/Wrap Mode Executed Indication*/
#define FLASHC_XIP_NOT_EFFECT_MEM_SHIFT                 0
#define FLASHC_XIP_NOT_EFFECT_MEM_MASK                  (0x1U << FLASHC_XIP_NOT_EFFECT_MEM_SHIFT)
#define FLASHC_WRAP_NOT_EFFECT_MEM_SHIFT                1
#define FLASHC_WRAP_NOT_EFFECT_MEM_MASK                 (0x1U << FLASHC_WRAP_NOT_EFFECT_MEM_SHIFT)

/* Memory Controller Debug state */
#define FLASHC_MEM_CTRL_STATUE_DEBUG_SHIFT              0
#define FLASHC_MEM_CTRL_STATUE_DEBUG_MASK               (0xFU << FLASHC_MEM_CTRL_STATUE_DEBUG_SHIFT)
#define FLASHC_MEM_CTRL_STATUE_DEBUG_VMASK              (0xFU)

/*Memory Controller SBUS Debug Count*/
#define FLASHC_SBUS_WR_COUNT_SHIFT                      0
#define FLASHC_SBUS_WR_COUNT_MASK                       (0xFFFF << FLASHC_SBUS_WR_COUNT_SHIFT)
#define FLASHC_SBUS_RD_COUNT_SHIFT                      16
#define FLASHC_SBUS_RD_COUNT_MASK                       (0xFFFF << FLASHC_SBUS_RD_COUNT_SHIFT)

/*Memory Controller CBUS Debug Count*/
#define FLASHC_CBUS_WR_COUNT_SHIFT                      0
#define FLASHC_CBUS_WR_COUNT_MASK                       (0xFFFF << FLASHC_CBUS_WR_COUNT_SHIFT)
#define FLASHC_CBUS_RD_COUNT_SHIFT                      16
#define FLASHC_CBUS_RD_COUNT_MASK                       (0xFFFF << FLASHC_CBUS_RD_COUNT_SHIFT)

#if (CONFIG_CHIP_ARCH_VER == 3)
/*CBUS Read Write enable wait cycle*/
#define FLASHC_CBUS_RW_EN_WAIT_SHIFT                    0
#define FLASHC_CBUS_RW_EN_WAIT_MASK                     (0xFF << FLASHC_CBUS_RW_EN_WAIT_SHIFT)
#endif


/*PSRAM Common Configure Register*/
#define FLASHC_CMD_HOLD_HALF_CYCLE_SHIFT                0
#define FLASHC_CMD_HOLD_HALF_CYCLE_MASK                 (0x1 << FLASHC_CMD_HOLD_HALF_CYCLE_SHIFT)
#define FLASHC_WORK_DOUBLE_CLK_EDGE_SHIFT               1
#define FLASHC_WORK_DOUBLE_CLK_EDGE_MASK                (0x1 << FLASHC_WORK_DOUBLE_CLK_EDGE_SHIFT)
#define FLASHC_OUTPUT_CLK_SGNL_STATE_SHIFT              2
#define FLASHC_OUTPUT_CLK_SGNL_STATE_MASK               (0x1 << FLASHC_OUTPUT_CLK_SGNL_STATE_SHIFT)
#define FLASHC_DQS_WORKS_WR_OPT_SHIFT                   3
#define FLASHC_DQS_WORKS_WR_OPT_MASK                    (0x1 << FLASHC_DQS_WORKS_WR_OPT_SHIFT)
#define FLASHC_SEND_DUMMY_DQS_WR_SHIFT                  4
#define FLASHC_SEND_DUMMY_DQS_WR_MASK                   (0x1 << FLASHC_SEND_DUMMY_DQS_WR_SHIFT)
#define FLASHC_SEDN_DUMMY_DM_WR_SHIFT                   5
#define FLASHC_SEDN_DUMMY_DM_WR_MASK                    (0x1 << FLASHC_SEDN_DUMMY_DM_WR_SHIFT)
#define FLASHC_CLK_ENABLE_STATE_SHIFT                   6
#define FLASHC_CLK_ENABLE_STATE_MASK                    (0x1 << FLASHC_CLK_ENABLE_STATE_SHIFT)
#define FLASHC_ENABLE_CLK_STOPPED_SHIFT                 7
#define FLASHC_ENABLE_CLK_STOPPED_MASK                  (0x1 << FLASHC_ENABLE_CLK_STOPPED_SHIFT)
#define FLASHC_MIN_WR_CYCLE_SHIFT                       8
#define FLASHC_MIN_WR_CYCLE_MASK                        (0x3 << FLASHC_MIN_WR_CYCLE_SHIFT)
#define FLASHC_DUMMY_NUM_SHIFT                          10
#define FLASHC_DUMMY_NUM_MASK                           (0x3 << FLASHC_DUMMY_NUM_SHIFT)
#define FLASHC_DQS_RD_WAIT_CYCLE_SHIFT                  12
#define FLASHC_DQS_RD_WAIT_CYCLE_MASK                   (0x3 << FLASHC_DQS_RD_WAIT_CYCLE_SHIFT)
#define FLASHC_MAX_CE_LOW_CYCLE_SHIFT                   16
#define FLASHC_MAX_CE_LOW_CYCLE_MASK                    (0xFFF << FLASHC_MAX_CE_LOW_CYCLE_SHIFT)
#define FLASHC_MAX_PUSH_OUT_RD_LATENCY_SHIFT            28
#define FLASHC_MAX_PUSH_OUT_RD_LATENCY_MASK             (0xF << FLASHC_MAX_PUSH_OUT_RD_LATENCY_SHIFT)

/*PSRAM Latency Configure Register*/
#define FLASHC_SBUS_WR_LATENCY_SHIFT                    0
#define FLASHC_SBUS_WR_LATENCY_MASK                     (0xFF << FLASHC_SBUS_WR_LATENCY_SHIFT)
#define FLASHC_CBUS_WR_LATENCY_SHIFT                    8
#define FLASHC_CBUS_WR_LATENCY_MASK                     (0xFF << FLASHC_CBUS_WR_LATENCY_SHIFT)
#define FLASHC_RD_LATENCY_TIMEOUT_SHIFT                 16
#define FLASHC_RD_LATENCY_TIMEOUT_MASK                  (0xFF << FLASHC_RD_LATENCY_TIMEOUT_SHIFT)

/*PSRAM Timing Configure Register*/
#define FLASHC_ADQ_OUTPUT_DELAY_CYCLE_SHIFT             0
#define FLASHC_ADQ_OUTPUT_DELAY_CYCLE_MASK              (0x3 << FLASHC_ADQ_OUTPUT_DELAY_CYCLE_SHIFT)
#define FLASHC_CLK_OUTPUT_DELAY_CYCLE_SHIFT             4
#define FLASHC_CLK_OUTPUT_DELAY_CYCLE_MASK              (0x3 << FLASHC_CLK_OUTPUT_DELAY_CYCLE_SHIFT)
#define FLASHC_CS_OUTPUT_DELAY_CYCL_SHIFT               8
#define FLASHC_CS_OUTPUT_DELAY_CYCL_MASK                (0x3 << FLASHC_CS_OUTPUT_DELAY_CYCL_SHIFT)
#define FLASHC_DM_OUTPUT_DELAY_CYCL_SHIFT               12
#define FLASHC_DM_OUTPUT_DELAY_CYCL_MASK                (0x3 << FLASHC_DM_OUTPUT_DELAY_CYCL_SHIFT)
#define FLASHC_DQS_OUTPUT_DELAY_CYCL_SHIFT              16
#define FLASHC_DQS_OUTPUT_DELAY_CYCL_MASK               (0x3 << FLASHC_DQS_OUTPUT_DELAY_CYCL_SHIFT)
#define FLASHC_DQS_IUTPUT_DELAY_CYCL_SHIFT              20
#define FLASHC_DQS_IUTPUT_DELAY_CYCL_MASK               (0x3 << FLASHC_DQS_IUTPUT_DELAY_CYCL_SHIFT)

/* Address Field0~7 Start Postion Configuration */
#define FLASHC_ADDR_FIELD_START_SHIFT                   0
#define FLASHC_ADDR_FIELD_START_MASK                    (0xFFFFFF0U << FLASHC_ADDR_FIELD_START_SHIFT)

/* Address Field0~7 End Postion Configuration */
#define FLASHC_ADDR_FIELD_END_SHIFT                     0
#define FLASHC_ADDR_FIELD_END_MASK                      (0xFFFFFF0U << FLASHC_ADDR_FIELD_END_SHIFT)

/* Address Field0~7 Bias Configuration */
#define FLASHC_ADDR_FIELD_BIAS_SHIFT                    0
#define FLASHC_ADDR_FIELD_BIAS_MASK                     (0xFFFFFFFU << FLASHC_ADDR_FIELD_BIAS_SHIFT)
#define FLASHC_ADDR_FIELD_BIAS_EN_SHIFT                 31
#define FLASHC_ADDR_FIELD_BIAS_EN_MASK                  (0x1U << FLASHC_ADDR_FIELD_BIAS_EN_SHIFT)

/* System-bus Write Data Register */
#define FLASHC_SBUS_WR_DATA_SHIFT                       0
#define FLASHC_SBUS_WR_DATA_MASK                        (0xFFFFFFFFU << FLASHC_SBUS_WR_DATA_SHIFT)

/* System-bus Read Data Register */
#define FLASHC_SBUS_RD_DATA_SHIFT                       0
#define FLASHC_SBUS_RD_DATA_MASK                        (0xFFFFFFFFU << FLASHC_SBUS_RD_DATA_SHIFT)

/*
 * bit field definition of PSRAMC->COMMON_CFG 0x00
 */
#if (CONFIG_CHIP_ARCH_VER == 3)
#define PSRAMC_CLEAR_CACHE_BUF_SHIFT    (4)
#define PSRAMC_CLEAR_CACHE_BUF_MASK     (1 << PSRAMC_CLEAR_CACHE_BUF_SHIFT)
#define PSRAMC_CLEAR_CACHE_BUF_BIT      (1 << PSRAMC_CLEAR_CACHE_BUF_SHIFT)
#define PSRAMC_FORCE_CACHE_TOUT_SHIFT   (0)
#define PSRAMC_FORCE_CACHE_TOUT_MASK    (1 << PSRAMC_FORCE_CACHE_TOUT_SHIFT)
#define PSRAMC_FORCE_CACHE_TOUT_BIT     (1 << PSRAMC_FORCE_CACHE_TOUT_SHIFT)
#endif

typedef enum {
	FC_CYCLEBITS_0,
	FC_CYCLEBITS_1,
	FC_CYCLEBITS_2,
	FC_CYCLEBITS_4
} FC_CycleBits;

typedef struct {
	uint8_t cs_begin;               /*!< cs active to valid clk edge setup minimum time */
	uint8_t cs_over;                /*!< valid clk edge to cs active hold minimum time */
	uint8_t cs_deselect;            /*!< cs minimum deselect time after read */
	uint8_t cmd_over;
	uint8_t addr_over;
	uint8_t dummy_over;
	uint8_t data;                   /*!< delay n half cycle */
} Flash_Ctrl_DelayCycle;

typedef enum {
	XIP_MODE_NORMAL,
	XIP_MODE_FAST,
	XIP_MODE_DUAL_O,
	XIP_MODE_DUAL_IO,
	XIP_MODE_QUAD_O,
	XIP_MODE_QUAD_IO
} XIP_Mode;

typedef struct XIP_Instruction {
	uint8_t cmd;                    /*!< command */
	FC_CycleBits cmd_line;          /*!< line mode of command */
	FC_CycleBits addr_line;         /*!< line mode of address */
	FC_CycleBits dummy_line;        /*!< line mode of dummy */
	FC_CycleBits data_line;         /*!< line mode of data */
	uint32_t dum_btyes;             /*!< dummy length */
//	uint32_t dummyh;
//	uint32_t dummyl;
} XIP_Instruction;

typedef struct {
	uint32_t addr;                  /*!< XIP code started address in flash */
	uint32_t freq;                  /*!< flash working frequency */
	Flash_Ctrl_DelayCycle delay;    /*!< board delay config */
	XIP_Instruction ins;            /*!< XIP read instruction */
	bool cont_mode;                 /*!< continue mode or not */
} XIP_Config;

typedef struct Flashc_Config {
	uint32_t freq;                  /*!< flash working frequency */
//	uint32_t t_shsl_ns;             /*!< flash t_shsl parameter. for calculate the cs delay. */
	uint32_t ispr_mask[FLASH_ISPR_MASK_NUM];
	uint32_t switch_out_ms;
	uint32_t check_busy_us;
	uint32_t busy_timeout_us;
	uint8_t optimize_mask; /* FLASH_OPTIMIZE_XX */
#if (CONFIG_CHIP_ARCH_VER == 3)
	uint32_t cs_mode;
#endif
} Flashc_Config;

typedef struct Flashc_Delay {
	uint32_t t_shsl_ns;             /*!< flash t_shsl parameter. for calculate the cs delay. */
} Flashc_Delay;

typedef struct FC_InstructionField {
	uint8_t *pdata;                 /*!< instruction field: data */
	uint32_t len;                   /*!< instruction field: data len */
	FC_CycleBits line;              /*!< instruction field: line mode */
} FC_InstructionField;

typedef enum Flashc_Commands {
	FC_CMD_ENABLE_32BITADDR_MODE = 0,
	FC_CMD_CONFIG_OPTIMIZE,
	FC_CMD_CONFIG_RESET_MASK,
} Flashc_Commands;

typedef enum {
	FC_CREATE,
	FC_INIT,
	FC_OPEN,
	FC_TRANS, /* no use now */
	FC_CLOSE,
	FC_DEINIT,
	FC_DESTORY,
} FC_OPERATE;

struct flash_controller {
	uint8_t externAddr_on;
	uint8_t xip_on;
	uint8_t ccmu_on;
	uint8_t pin_inited;
	uint8_t sbusing;
	uint8_t ref;
#define FLASH_OPTIMIZE_READ     HAL_BIT(0)
#define FLASH_OPTIMIZE_WRITE    HAL_BIT(1)
#define FLASH_OPTIMIZE_ERASE    HAL_BIT(2)
	uint8_t optimize_mask;
	uint8_t xip_continue;
#if (CONFIG_CHIP_ARCH_VER == 3)
	uint32_t cs_mode;
#endif
	hal_sem_t dmaSem;
	uint32_t irqsaveflag;
	uint8_t resetMask;
	int8_t cacheWriteThroughIdx;
	uint8_t pm_xip;
	uint8_t suspending;
	XIP_Config pm_ibus_cfg;
	Flashc_Config pm_sbus_cfg;
#ifdef CONFIG_PM
	struct soc_device_driver flashc_drv;
	struct soc_device flashc_dev;
#endif
};

typedef void (*flashc_caback)(struct flash_controller *ctrl, FC_OPERATE op, uint32_t step);

/**
 * @brief Initialize Flash controller IBUS driver (XIP).
 * @param cfg:
 *        @arg cfg->addr: Started address of XIP code in Flash.
 *        @arg cfg->freq: Flash working frequency.
 *        @arg cfg->delay: Delay of hardware.
 *        @arg cfg->ins: Instruction of XIP reading
 *        @arg cfg->cont_mode: Enable continue mode in reading or not.
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Xip_Init(struct flash_controller *ctrl, XIP_Config *cfg);

/**
 * @brief Deinitialize Flash controller IBUS (XIP).
 * @param None
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Xip_Deinit(struct flash_controller *ctrl);

/**
 * @internal
 * @brief Flash controller IBUS (XIP) Enable without Pin initialization.
 * @note Most for Flash controller SBUS. It will resume system schedule.
 * @param None
 * @retval None
 */
void HAL_Flashc_Xip_RawEnable(struct flash_controller *ctrl);

/**
 * @internal
 * @brief Flash controller IBUS (XIP) Enable without Pin deinitialization.
 * @note Most for Flash controller SBUS. It will suspend system schedule.
 * @param None
 * @retval None
 */
void HAL_Flashc_Xip_RawDisable(struct flash_controller *ctrl);

/**
 * @brief Initialize Flash controller SBUS.
 * @param cfg:
 * 	   @arg cfg->freq: Flash working frequency.
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Init(struct flash_controller *ctrl, const Flashc_Config *cfg);

/**
 * @brief Deinitialize Flash controller SBUS.
 * @param None
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Deinit(struct flash_controller *ctrl);

void HAL_Flashc_RegisterCb(flashc_caback cb);

/**
 * @brief Open flash controller SBUS.
 * @note At the same time, it will disable XIP and suspend schedule.
 * @param None
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Open(struct flash_controller *ctrl);

/**
 * @brief Close flash controller SBUS.
 * @param None
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Close(struct flash_controller *ctrl);

/**
 * @brief Flash controller ioctl.
 * @note op : arg
 *       nothing support for now.
 * @param op: ioctl command.
 * @param arg: ioctl arguement
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Ioctl(struct flash_controller *ctrl, uint32_t op, void *arg);

/**
 * @brief Write or read flash by flash controller SBUS.
 * @note Send a instruction in command + address + dummy + write or read data.
 * @param cmd: Command of instruction.
 *        @arg cmd->pdata: The data is filled with in this field.
 *        @arg cmd->len: The data len of this field.
 *        @arg cmd->line: The number of line transfering this field data.
 * @param addr: Address of instruction
 * @param dummy: Dummy of instruction
 * @param data: Data of instruction
 * @param dma: Transfer data by DMA or not.
 * @retval hal_status_t: The status of driver.
 */
hal_status_t HAL_Flashc_Transfer(struct flash_controller *ctrl, int write,
                               FC_InstructionField *cmd, FC_InstructionField *addr,
                               FC_InstructionField *dummy, FC_InstructionField *data, bool dma);

struct flash_controller *HAL_Flashc_Create(uint32_t id);
hal_status_t HAL_Flashc_Destory(struct flash_controller *ctrl);
int HAL_Flashc_IncRef(struct flash_controller *ctrl);
int HAL_Flashc_DecRef(struct flash_controller *ctrl);
void HAL_Flashc_SetDbgMask(uint8_t dbg_mask);
//int __FC_DebugCheck(int state);

/**
 * @brief Delay realization in Flash controller IBUS (XIP).
 * @note Delay can be system sleep while it's not in XIP, but must be a while
 *       delay without system interface while it's in XIP.
 * @param us: delay time in microsecond.
 * @retval None
 */
void HAL_Flashc_Delay(struct flash_controller *ctrl, unsigned int us);

void FC_GetDelayCycle(Flash_Ctrl_DelayCycle *delay, uint32_t freq);

void HAL_Flashc_ResetCCMU(void);
bool HAL_Flashc_ConfigCCMU(uint32_t clk);
void HAL_Flashc_EnableCCMU(struct flash_controller *ctrl);
void FC_Sbus_TransmitDelay(Flash_Ctrl_DelayCycle *delay);
void FC_SetFlash(FC_Cs cs, FC_TCTRL_Fbs fbs, FC_Sclk_Mode mode);
void FC_Sbus_ResetFIFO(bool tx, bool rx);
void FC_Cbus_RW_Enable(bool enable);

void HAL_Flashc_DisableCCMU(struct flash_controller *ctrl);

void HAL_Flashc_PinInit(struct flash_controller *ctrl);
void FC_Ibus_TransmitDelay(Flash_Ctrl_DelayCycle *delay);
void HAL_Flashc_PinDeinit(struct flash_controller *ctrl);
void HAL_Flashc_DUMP_CbusCnt(void);

uint32_t FC_GetXipUserEndAddr(void);
int32_t FlashCBUS_AddrRequire(uint8_t field, uint32_t sAddr, uint32_t bias, uint32_t userLen);
int32_t PsramCBUS_AddrRequire(uint8_t field, uint32_t sAddr, uint32_t bias, uint32_t userLen);
#if (CONFIG_CHIP_ARCH_VER == 3)
void HAL_Flashc_SetCacheLineLen(FC_RdCacheLineLenCfg rlen, FC_WtCacheLneLenCfg wlen);
void FC_SbusPrepare(void);
void FC_SbusFinish(void);

void HAL_Flashc_CBUS_Dma_Enable(uint32_t en);
void HAL_FlashCrypto_Init(uint8_t *nonce);
void HAL_FlashCrypto_DeInit(void);
int32_t FlashCryptoRequest(uint32_t startAddr, uint32_t endAddr, uint8_t *key);
#if FLASH_CRYPTO_DISABLE_FUNC_EN
void FlashCryptoRelease(uint32_t channel);
#endif

void FC_Crypto_Enable(uint8_t *key);
void FC_Clr_Crypto_Infor(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_FLASHCTRL_H_ */
