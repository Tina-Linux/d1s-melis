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

#ifndef _DRIVER_CHIP_HAL_PRCM_H_
#define _DRIVER_CHIP_HAL_PRCM_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Bitwise operation
 */
#define HAL_BIT(pos)                        (1U << (pos))

#define HAL_SET_BIT(reg, mask)              ((reg) |= (mask))
#define HAL_CLR_BIT(reg, mask)              ((reg) &= ~(mask))
#define HAL_GET_BIT(reg, mask)              ((reg) & (mask))
#define HAL_GET_BIT_VAL(reg, shift, vmask)  (((reg) >> (shift)) & (vmask))

#define HAL_MODIFY_REG(reg, clr_mask, set_mask) \
    ((reg) = (((reg) & (~(clr_mask))) | (set_mask)))

#ifndef HAL_ASSERT_PARAM
#define HAL_ASSERT_PARAM(x)  do { if (!(x)) while (1); } while (0)
#endif

/*
 * Macros for accessing LSBs of a 32-bit register (little endian only)
 */
#define HAL_REG_32BIT(reg_addr)  (*((__IO uint32_t *)(reg_addr)))
#define HAL_REG_16BIT(reg_addr)  (*((__IO uint16_t *)(reg_addr)))
#define HAL_REG_8BIT(reg_addr)   (*((__IO uint8_t  *)(reg_addr)))


#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */



/* auto generate name by macro __LINE__ */
#define RESERVED_HELPER(a,b)  a##b
#define RESERVED_HELP(a, b)   RESERVED_HELPER(a, b)
#define RESERVED              RESERVED_HELP(RES, __LINE__)


#ifdef CONFIG_PLATFORM_FPGA
#define LOSC_CLOCK          (32000U)
#else
#define LOSC_CLOCK          (32768U)
#endif

#define SYS_LFCLOCK         LOSC_CLOCK


/* prcm system module control */
typedef struct {
    __IO uint32_t DIG_BG_CTRL;              /* offset: 0x0000 Digital Bandgap Control Register */
    __IO uint32_t DCDC_CTRL0;               /* offset: 0x0004 DCDC control register */
    __IO uint32_t DCDC_CTRL1;               /* offset: 0x0008 DCDC control register */
    __IO uint32_t DCDC_LDO_MODE_SW_SEL;     /* offset: 0x000c DCDC/LDO Work Mode control */

         uint32_t RESERVED[4];

    __IO uint32_t RTC_LDO_CTRL;             /* offset: 0x0020 RTC LDO control register */
    __IO uint32_t EXT_LDO_CTRL;             /* offset: 0x0024 EXT LDO control register */
    __IO uint32_t TOP_LDO_CTRL;             /* offset: 0x0028 TOP LDO control register */
         uint32_t RESERVED[5];

    __IO uint32_t AON_LDO_CTRL;             /* offset: 0x0040 AON LDO control register */
    __IO uint32_t APP_LDO_CTRL;             /* offset: 0x0044 APP LDO control register */
//    __IO uint32_t RV_LDO_CTRL;              /* offset: 0x0048 RV LDO control register */
         uint32_t RESERVED[1];
    __IO uint32_t DSP_LDO_CTRL;             /* offset: 0x004c DSP LDO control register */

         uint32_t RESERVED[4];

    __IO uint32_t VBAT_MON_CTRL;            /* offset: 0x0060 VBAT Monitor control register */
    __IO uint32_t USB_BIAS_CTRL;            /* offset: 0x0064 USB Bias control register */
         uint32_t RESERVED[6];

    __IO uint32_t SYS_LFCLK_CTRL;           /* offset: 0x0080 System Low Frequency clock control*/
    __IO uint32_t SYS_LFCLK_STATUS;         /* offset: 0x0084 System Low Frequency clock status*/
         uint32_t RESERVED[14];

    __IO uint32_t RST_DBC_CTRL;             /* offset: 0x00c0 RESET pin Debounce control */
//    __IO uint32_t SYS_RST_SRC;              /* offset: 0x00c4 system reset source register */
         uint32_t RESERVED[1];
    __IO uint32_t BOOT_MEDIA_RECORD;        /* offset: 0x00c8 system reset source register */
         uint32_t RESERVED[29];

    __IO uint32_t SYS_RCOSC_FREQ_DET;       /* offset: 0x0140, system RCOSC frequnency clock */
    __IO uint32_t BLE_RCOSC_CALIB_CTRL0;    /* offset: 0x0144, ble RCOSC calibration control register0 */
    __IO uint32_t BLE_RCOSC_CALIB_CTRL1;    /* offset: 0x0148, ble RCOSC calibration control register1 */
    __IO uint32_t BLE_CLK32K_SWITCH0;       /* offset: 0x014c, ble clk 32k switch register0 */

    __IO uint32_t BLE_CLK32K_SWITCH1;       /* offset: 0x0150, ble clk 32k switch register1 */
         uint32_t RESERVED[11];


    __IO uint32_t BT_WLAN_CONN_MODE_CTRL;   /* offset: 0x0180, bt/wlan connect mode control.*/
         uint32_t RESERVED[1];
    __I  uint32_t CFG_IO_STATUS;            /* offset: 0x0188, config io status. */
    __IO uint32_t WLAN_HIF_OV_CTRL;         /* offset: 0x018c, wlan HIF override control register. */

    __IO uint32_t SRAM_BIST_CFG;            /* offset: 0x0190, ble clk 32k switch register0 */
    __IO uint32_t FLASH_ENCRYPT_AES_NONCE0; /* offset: 0x0194, flash encrypt module AES nonce low bit register */
    __IO uint32_t FLASH_ENCRYPT_AES_NONCE1; /* offset: 0x0198, flash encrypt module AES nonce hig bit register */
         uint32_t RESERVED[9];

    __IO uint32_t AR800A_BOOT_FLAG;         /* offset: 0x01c0, AR800A boot flag. */
    __IO uint32_t AR800A_BOOT_ADDR;         /* offset: 0x01c4, AR800A boot addr. */
    __IO uint32_t AR800A_BOOT_ARG;          /* offset: 0x01c8, AR800A boot argument. */
    __IO uint32_t DSP_BOOT_FLAG;            /* offset: 0x01cc, DSP boot flag. */

    __IO uint32_t DSP_BOOT_ADDR;            /* offset: 0x01d0, DSP boot addr. */
    __IO uint32_t DSP_BOOT_ARG;             /* offset: 0x01d4, DSP boot argument. */
    __IO uint32_t RV_BOOT_FLAG;             /* offset: 0x01d8, RV boot flag. */
    __IO uint32_t RV_BOOT_ADDR;             /* offset: 0x01dc, RV boot addr. */

    __IO uint32_t RV_BOOT_ARG;              /* offset: 0x01e0, RV boot argument. */
         uint32_t RESERVED[7];

    __IO uint32_t SYSTEM_PRIV_REG0T7[8];     /* offset: 0x0200~0x021c, system private register. */

         uint32_t RESERVED[4];

    __IO uint32_t SYSTEM_FAST_BOOT_INFO_REG; /* offset: 0x0230, system fast boot media information register. */
} PRCM_T;

#define PRCM_BASE      0x40050000
#define PRCM           ((PRCM_T *)PRCM_BASE)


typedef struct {
         uint32_t RESERVED[16];

    __IO uint32_t DCXO_STABLE_REF_TIME;     /* offset: 0x0040 DCXO stable reference time register */
    __IO uint32_t DPLL_STABLE_REF_TIME;     /* offset: 0x0044 DPLL stable reference time register */
    __IO uint32_t LDO_STABLE_REF_TIME;      /* offset: 0x0048 LDO  stable reference time register */
    __IO uint32_t DIGITAL_SWITCH_REF_TIME;  /* offset: 0x004C Digital Switch reference time register */

    __IO uint32_t BANDGAP_STABLE_REF_TIME;  /* offset: 0x0050 Band Gap stable reference time register */
    __IO uint32_t DCDC_STABLE_REF_TIME;     /* offset: 0x0054 DCDC stable reference time register */
         uint32_t RESERVED[2];

         uint32_t RESERVED[40];             /* 0x60~0x100 ~ 160 Bytes*/

    __IO uint32_t SYS_LOW_POWER_CTRL;       /* offset: 0x0100 system low-power control register */
    __IO uint32_t SYS_LOW_POWER_STATUS;     /* offset: 0x0104 system low-power status register */
    __IO uint32_t RTC_WUP_TIMER_CNT;        /* offset: 0x0108 RTC wakeup timer counter register */
    __IO uint32_t RTC_WUP_TIMER_CTRL;       /* offset: 0x010c RTC wakeup timer control register */

    __IO uint32_t RTC_IO_WAKE_EN;           /* offset: 0x0110 RTC IO wakeup enable register */
    __IO uint32_t RTC_IO_WAKE_DEB_CLK;      /* offset: 0x0114 RTC IO wakeup debounce clock register */
    __IO uint32_t RTC_IO_WAKE_ST;           /* offset: 0x0118 RTC IO wakeup status register */
    __IO uint32_t RTC_IO_HOLD_CTRL;         /* offset: 0x011c RTC IO hold control register */

    __IO uint32_t RTC_IO_WUP_GEN;           /* offset: 0x0120 RTC IO wakeup global enable register */
    __IO uint32_t RTC_IO_WUP_DEB_CYCLES0;   /* offset: 0x0124 RTC IO wakeup debouce clock cycles 0 register */
    __IO uint32_t RTC_IO_WUP_DEB_CYCLES1;   /* offset: 0x0128 RTC IO wakeup debouce clock cycles 1 register */
         uint32_t RESERVED[1];

         uint32_t RESERVED[20];             /* 0x130~0x180 ~ 80 Bytes*/

    __IO uint32_t VDD_APP_PWROFF_GATING;    /* offset: 0x0180 VDD_APP Power Off Gating register */
    __IO uint32_t POWERCTRL_CFG_REG;        /* offset: 0x0184 Power control configuration register */

} PMU_T;

#define PMU_BASE      0x40051400
#define PMU           ((PMU_T *)PMU_BASE)



/*
 * bit field definition of PRCM->DCDC_CTRL0
 */

#define PRCM_DCDC_VOLT_SHIFT	        21   /* R/W */
#define PRCM_DCDC_VOLT_MASK		(0x3FU << PRCM_DCDC_VOLT_SHIFT)
#define PRCM_DCDC_VOLT_VMASK		(0x3FU)
#define PRCM_DCDC_VOLT_MAX_MV		1000
#define PRCM_DCDC_VOLT_MIN_MV		2500
#define PRCM_DCDC_VOLT_VAL2MV(_x)	(((_x) * 25) + 1000)
#define PRCM_DCDC_VOLT_MV2VAL(_x)	(((_x) - 1000) / 25)

#define PRCM_TOPLDO_OFF_DCDC_ON_BIT     HAL_BIT(9)
#define PRCM_SYS_STANDBY_DCDC_OFF_BIT   HAL_BIT(8)
#define PRCM_DCDC_PWM_SEL_EN_BIT        HAL_BIT(7)

#define PRCM_DCDC_PWM_SRC_SHIFT         4   /* R/W */
#define PRCM_DCDC_PWM_SRC_MASK          (0x7U << PRCM_DCDC_PWM_SRC_SHIFT)
#define PRCM_DCDC_PWM_SRC_VMASK         (0x7U)
typedef enum {
	PRCM_DCDC_PWM_SRC_CLK_RFIPDPLL  = (0x0U << PRCM_DCDC_PWM_SRC_SHIFT),
	PRCM_DCDC_PWM_SRC_CLK_RFIPDCXO  = (0x1U << PRCM_DCDC_PWM_SRC_SHIFT),
	PRCM_DCDC_PWM_SRC_WLANPHYEN     = (0x2U << PRCM_DCDC_PWM_SRC_SHIFT),
	PRCM_DCDC_PWM_SRC_APPCPUEN      = (0x3U << PRCM_DCDC_PWM_SRC_SHIFT),
	PRCM_DCDC_PWM_SRC_WLANCPUEN     = (0x4U << PRCM_DCDC_PWM_SRC_SHIFT),
	PRCM_DCDC_PWM_SRC_BLERFEN       = (0x5U << PRCM_DCDC_PWM_SRC_SHIFT),
	/*others: PRCM_DCDC_PWM_SRC_BLERFEN*/
} PRCM_DCDCPwmSrc;

#define PRCM_OVR_DCDC_DETECT_BIT        HAL_BIT(2)
#define PRCM_DCDC_DETECT_VALUE_BIT      HAL_BIT(1)
#define PRCM_DCDC_DETECT_BIT            HAL_BIT(0) /* RO */

/*
 * bit field definition of PRCM->DCDC_CTRL1
 */
#define PRCM_DCDC_OCP_SHIFT             29
#define PRCM_DCDC_OCP_MASK              (0x7U << PRCM_DCDC_OCP_SHIFT)
#define PRCM_DCDC_OCP_VMASK             (0x7U)

#define PRCM_DCDC_BGTR_SHIFT            24
#define PRCM_DCDC_BGTR_MASK             (0x1FU << PRCM_DCDC_BGTR_SHIFT)
#define PRCM_DCDC_BGTR_VMASK            (0x1FU)

#define PRCM_DCDC_FTR_SHIFT             16
#define PRCM_DCDC_FTR_MASK              (0x7FU << PRCM_DCDC_FTR_SHIFT)
#define PRCM_DCDC_FTR_VMASK             (0x7FU)

#define PRCM_DCDC_TEST_ITEM_SEL_SHIFT   8
#define PRCM_DCDC_TEST_ITEM_SEL_MASK    (0xFFU << PRCM_DCDC_TEST_ITEM_SEL_SHIFT)
#define PRCM_DCDC_TEST_ITEM_SEL_VMASK   (0xFFU)

#define PRCM_DCDC_TEST_MODE_SHIFT       4
#define PRCM_DCDC_TEST_MODE_MASK        (0xFU << PRCM_DCDC_TEST_MODE_SHIFT)
#define PRCM_DCDC_TEST_MODE_VMASK       (0xFU)

#define PRCM_DCDC_BYPASS_SHIFT          2
#define PRCM_DCDC_BYPASS_MASK           (0x3U << PRCM_DCDC_BYPASS_SHIFT)
#define PRCM_DCDC_BYPASS_VMASK          (0x3U)

#define PRCM_DCDC_WORK_MODE_BIT         HAL_BIT(0)  /* RO */

/*
 * bit field definition of PRCM->DCDC_LDO_MODE_SW_SEL
 */
#define PRCM_DSP_LDO_LQ_MODE_BIT                HAL_BIT(7)
#define PRCM_APP_LDO_LQ_MODE_BIT                HAL_BIT(6)
#define PRCM_AON_LDO_LQ_MODE_BIT                HAL_BIT(5)
#define PRCM_LDO_MODE_SW_OVERIDE_BIT            HAL_BIT(4)
#define PRCM_DCDC_PFM_MODE_BIT                  HAL_BIT(1)
#define PRCM_DCDC_MODE_SW_OVERIDE_BIT           HAL_BIT(0)


/*
 * bit field definition of PRCM->RTC_LDO_CTRL
 */
#define PRCM_RTC_LDO_SLEEP_VOLT_SHIFT       4
#define PRCM_RTC_LDO_SLEEP_VOLT_MASK        (0x3U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT)
#define PRCM_RTC_LDO_SLEEP_VOLT_VMASK       (0x3U)
typedef enum {
	PRCM_RTC_LDO_SLEEP_VOLT_1075MV	=       (0x0U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_975MV	=       (0x1U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_875MV	=       (0x2U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_775MV	=       (0x3U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_675MV	=       (0x4U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_575MV	=       (0x5U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_1175MV	=       (0x6U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
	PRCM_RTC_LDO_SLEEP_VOLT_1275MV	=       (0x7U << PRCM_RTC_LDO_SLEEP_VOLT_SHIFT),
} PRCM_RTCLDOSlpVolt;

#define PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT       0
#define PRCM_RTC_LDO_ACTIVE_VOLT_MASK        (0x3U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT)
#define PRCM_RTC_LDO_ACTIVE_VOLT_VMASK       (0x3U)
typedef enum {
	PRCM_RTC_LDO_ACTIVE_VOLT_1075MV	=       (0x0U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_975MV	=       (0x1U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_875MV	=       (0x2U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_775MV	=       (0x3U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_675MV	=       (0x4U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_575MV	=       (0x5U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_1175MV	=       (0x6U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
	PRCM_RTC_LDO_ACTIVE_VOLT_1275MV	=       (0x7U << PRCM_RTC_LDO_ACTIVE_VOLT_SHIFT),
} PRCM_RTCLDOActVolt;


/*
 * bit field definition of PRCM->EXT_LDO_CTRL
 */

#define PRCM_EXT_LDO_LQB_MODE_BIT       HAL_BIT(24)
#define PRCM_EXT_LDO_BYPASS_BIT         HAL_BIT(23)
#define PRCM_EXT_LDO_SW_TRIM_SHIFT      21
#define PRCM_EXT_LDO_SW_TRIM_MASK       (0x3U << PRCM_EXT_LDO_SW_TRIM_SHIFT)
#define PRCM_EXT_LDO_SW_TRIM_VMASK      (0x3U)
#define PRCM_EXT_LDO_SW_MODE_BIT        HAL_BIT(20)

#define PRCM_EXT_LDO_VOLT_SHIFT         4
#define PRCM_EXT_LDO_VOLT_MASK          (0x3U << PRCM_EXT_LDO_VOLT_SHIFT)
typedef enum {
	PRCM_EXT_LDO_3V3                = 0,
	PRCM_EXT_LDO_3V1                = 1,
	PRCM_EXT_LDO_2V8                = 2,
	PRCM_EXT_LDO_2V5                = 3,
} PRCM_EXTLDOVolt;

#define PRCM_EXT_LDO_MODE_SHIFT         0 /* R/W */
#define PRCM_EXT_LDO_MODE_MASK          (0x3U << PRCM_EXT_LDO_MODE_SHIFT)
typedef enum {
	PRCM_EXTLDO_ALWAYS_OFF  =       (0x0U << PRCM_EXT_LDO_MODE_SHIFT),
	PRCM_EXTLDO_SAME_TOPLDO =       (0x1U << PRCM_EXT_LDO_MODE_SHIFT),
	PRCM_EXTLDO_ALWAYS_ON   =       (0x2U << PRCM_EXT_LDO_MODE_SHIFT),
	/* other: PRCM_EXTLDO_ALWAYS_ON */
} PRCM_ExtLDOMode;



/*
 * bit field definition of PRCM->TOP_LDO_CTRL
 */
/* NOT TODO */

/*
 * bit field definition of PRCM->AON_LDO_CTRL
 */

#define PRCM_AON_LDO_TESTPEAK_BIT       HAL_BIT(19)
#define PRCM_AON_LDO_STARTICC_BIT       HAL_BIT(18)  /* RO */
#define PRCM_AON_LDO_TESTICC_BIT        HAL_BIT(17)

#define PRCM_AON_LDO_WORK_VOLT_SHIFT           9
#define PRCM_AON_LDO_WORK_VOLT_MASK            (0x1FU << PRCM_AON_LDO_WORK_VOLT_SHIFT)
#define PRCM_AON_LDO_WORK_VOLT_VMASK           (0x1FU)
#define PRCM_AON_LDO_WORK_VOLT_MAX_MV          1375
#define PRCM_AON_LDO_WORK_VOLT_MIN_MV          600
#define PRCM_AON_LDO_WORK_VOLT_VAL2MV(_x)      (((_x) * 25) + 600)
#define PRCM_AON_LDO_WORK_VOLT_MV2VAL(_x)      (((_x) - 600) / 25)

#define PRCM_AON_LDO_SLEEP_VOLT_SHIFT      4
#define PRCM_AON_LDO_SLEEP_VOLT_MASK       (0x1FU << PRCM_AON_LDO_SLEEP_VOLT_SHIFT)
#define PRCM_AON_LDO_SLEEP_VOLT_VMASK      (0x1FU)
#define PRCM_AON_LDO_SLEEP_VOLT_MAX_MV     1375
#define PRCM_AON_LDO_SLEEP_VOLT_MIN_MV     600
#define PRCM_AON_LDO_SLEEP_VOLT_VAL2MV(_x) (((_x) * 25) + 600)
#define PRCM_AON_LDO_SLEEP_VOLT_MV2VAL(_x) (((_x) - 600) / 25)

#define PRCM_AON_LDO_LQB_MODE_BIT       HAL_BIT(1)   /* RO */
#define PRCM_AON_LDO_EN_BIT             HAL_BIT(0)   /* RO */


/*
 * bit field definition of PRCM->APP_LDO_CTRL
 */

#define PRCM_APP_LDO_TESTPEAK_BIT       HAL_BIT(19)
#define PRCM_APP_LDO_STARTICC_BIT       HAL_BIT(18)  /* RO */
#define PRCM_APP_LDO_TESTICC_BIT        HAL_BIT(17)

#define PRCM_APP_LDO_WORK_VOLT_SHIFT           9
#define PRCM_APP_LDO_WORK_VOLT_MASK            (0x1FU << PRCM_APP_LDO_WORK_VOLT_SHIFT)
#define PRCM_APP_LDO_WORK_VOLT_VMASK           (0x1FU)
#define PRCM_APP_LDO_WORK_VOLT_MAX_MV          1375
#define PRCM_APP_LDO_WORK_VOLT_MIN_MV          600
#define PRCM_APP_LDO_WORK_VOLT_VAL2MV(_x)      (((_x) * 25) + 600)
#define PRCM_APP_LDO_WORK_VOLT_MV2VAL(_x)      (((_x) - 600) / 25)

#define PRCM_APP_LDO_SLEEP_VOLT_SHIFT      4
#define PRCM_APP_LDO_SLEEP_VOLT_MASK       (0x1FU << PRCM_APP_LDO_SLEEP_VOLT_SHIFT)
#define PRCM_APP_LDO_SLEEP_VOLT_VMASK      (0x1FU)
#define PRCM_APP_LDO_SLEEP_VOLT_MAX_MV     1375
#define PRCM_APP_LDO_SLEEP_VOLT_MIN_MV     600
#define PRCM_APP_LDO_SLEEP_VOLT_VAL2MV(_x) (((_x) * 25) + 600)
#define PRCM_APP_LDO_SLEEP_VOLT_MV2VAL(_x) (((_x) - 600) / 25)

#define PRCM_APP_LDO_LQB_MODE_BIT       HAL_BIT(1)   /* RO */
#define PRCM_APP_LDO_EN_BIT             HAL_BIT(0)   /* RO */



/*
 * bit field definition of PRCM->DSP_LDO_CTRL
 */

#define PRCM_DSP_LDO_TESTPEAK_BIT       HAL_BIT(19)
#define PRCM_DSP_LDO_STARTICC_BIT       HAL_BIT(18)  /* RO */
#define PRCM_DSP_LDO_TESTICC_BIT        HAL_BIT(17)

#define PRCM_DSP_LDO_WORK_VOLT_SHIFT           9
#define PRCM_DSP_LDO_WORK_VOLT_MASK            (0x1FU << PRCM_DSP_LDO_WORK_VOLT_SHIFT)
#define PRCM_DSP_LDO_WORK_VOLT_VMASK           (0x1FU)
#define PRCM_DSP_LDO_WORK_VOLT_MAX_MV          1375
#define PRCM_DSP_LDO_WORK_VOLT_MIN_MV          600
#define PRCM_DSP_LDO_WORK_VOLT_VAL2MV(_x)      (((_x) * 25) + 600)
#define PRCM_DSP_LDO_WORK_VOLT_MV2VAL(_x)      (((_x) - 600) / 25)

#define PRCM_DSP_LDO_SLEEP_VOLT_SHIFT      4
#define PRCM_DSP_LDO_SLEEP_VOLT_MASK       (0x1FU << PRCM_DSP_LDO_SLEEP_VOLT_SHIFT)
#define PRCM_DSP_LDO_SLEEP_VOLT_VMASK      (0x1FU)
#define PRCM_DSP_LDO_SLEEP_VOLT_MAX_MV     1375
#define PRCM_DSP_LDO_SLEEP_VOLT_MIN_MV     600
#define PRCM_DSP_LDO_SLEEP_VOLT_VAL2MV(_x) (((_x) * 25) + 600)
#define PRCM_DSP_LDO_SLEEP_VOLT_MV2VAL(_x) (((_x) - 600) / 25)

#define PRCM_DSP_LDO_LQB_MODE_BIT       HAL_BIT(1)   /* RO */
#define PRCM_DSP_LDO_EN_BIT             HAL_BIT(0)   /* RO */


/*
 * bit field definition of PRCM->VBAT_MON_CTRL
 */

#define PRCM_VBAT_MON_VSEL_SHIFT           4
#define PRCM_VBAT_MON_VSEL_MASK            (0x1FU << PRCM_VBAT_MON_VSEL_SHIFT)
#define PRCM_VBAT_MON_VSEL_VMASK           (0x1FU)
#define PRCM_VBAT_MON_VSEL_MAX_MV          7700
#define PRCM_VBAT_MON_VSEL_MIN_MV          1400
#define PRCM_VBAT_MON_VSEL_VAL2MV(_x)      (((_x) * 100) + 1400)
#define PRCM_VBAT_MON_VSEL_MV2VAL(_x)      (((_x) - 1400) / 100)

#define PRCM_VBAT_LOW_VOL_MODE_SEL_SHIFT   2
#define PRCM_VBAT_LOW_VOL_MODE_SEL_MASK    (0x1U << PRCM_VBAT_MON_VSEL_SHIFT)
#define PRCM_VBAT_LOW_VOL_MODE_SEL_BIT     HAL_BIT(2)
typedef enum {
	PRCM_VBAT_LOW_VOL_MODE_IRQ   =     (0x0U << PRCM_EXT_LDO_MODE_SHIFT),
	PRCM_VBAT_LOW_VOL_MODE_RESET =     (0x1U << PRCM_EXT_LDO_MODE_SHIFT),
} PRCM_VbatLowVolMode;

#define PRCM_VBAT_MON_SW_MODE_BIT          HAL_BIT(1)
#define PRCM_VBAT_MON_EN_BIT               HAL_BIT(0)


/*
 * bit field definition of PRCM->USB_BIAS_CTRL
 */

#define PRCM_USB_BIAS_TRIM_SHIFT           4
#define PRCM_USB_BIAS_TRIM_MASK            (0x3F << PRCM_USB_BIAS_TRIM_SHIFT)
#define PRCM_USB_BIAS_TRIM_VMASK           (0x3F)

#define PRCM_USB_BIAS_EN_BIT               HAL_BIT(0)


/*
 * bit field definition of PRCM->SYS_LFCLK_CTRL
 */

#define PRCM_LFCLK_EXT32K_EN_BIT        HAL_BIT(31)     /* R/W */
#define PRCM_LFCLK_INTER32K_EN_BIT      HAL_BIT(30)     /* R/W */
#define PRCM_LFCLK_INTER_RC_HF_EN_BIT   HAL_BIT(29)     /* R/W */

#define PRCM_SYS_32K_SEL_SHIFT		28              /* R/W */
#define PRCM_SYS_32K_SEL_MASK		(0x1U << PRCM_SYS_32K_SEL_SHIFT)
typedef enum {
        PRCM_SYS_32K_SEL_LFCLK		= (0x0U << PRCM_SYS_32K_SEL_SHIFT),
        PRCM_SYS_32K_SEL_RCCAL		= (0x1U << PRCM_SYS_32K_SEL_SHIFT)
} PRCM_SYS32KSrc;

#define PRCM_BLE_32K_SEL_SHIFT    27              /* R/W */
#define PRCM_BLE_32K_SEL_MASK     (0x1U << PRCM_BLE_32K_SEL_SHIFT)
typedef enum {
        PRCM_BLE_32K_SEL_LFCLK = (0x0U << PRCM_BLE_32K_SEL_SHIFT),
        PRCM_BLE_32K_SEL_RCCAL = (0x1U << PRCM_BLE_32K_SEL_SHIFT)
} PRCM_BLE32KSrc;

#define PRCM_SYSRTC_32K_SEL_SHIFT    25              /* R/W */
#define PRCM_SYSRTC_32K_SEL_MASK     (0x3U << PRCM_SYSRTC_32K_SEL_SHIFT)
typedef enum {
        PRCM_SYSRTC_32K_SEL_LFCLK    = (0x0U << PRCM_SYSRTC_32K_SEL_SHIFT),
        PRCM_SYSRTC_32K_SEL_RCCAL    = (0x1U << PRCM_SYSRTC_32K_SEL_SHIFT),
        PRCM_SYSRTC_32K_SEL_DIV32K   = (0x2U << PRCM_SYSRTC_32K_SEL_SHIFT),
        PRCM_SYSRTC_32K_SEL_RESERVED = (0x3U << PRCM_SYSRTC_32K_SEL_SHIFT)
} PRCM_SYSRTC32KSrc;

#define PRCM_LFCLK_BASE_SRC_SHIFT       24              /* R/W */
#define PRCM_LFCLK_BASE_SRC_MASK        (0x1U << PRCM_LFCLK_BASE_SRC_SHIFT)
typedef enum {
        PRCM_LFCLK_BASE_SRC_INTER32K    = (0x0U << PRCM_LFCLK_BASE_SRC_SHIFT),
        PRCM_LFCLK_BASE_SRC_EXT32K      = (0x1U << PRCM_LFCLK_BASE_SRC_SHIFT)
} PRCM_LFCLKBaseSrc;

#define PRCM_RCO_HF_TRIM_SHIFT		16
#define PRCM_RCO_HF_TRIM_MASK		(0xFFU << PRCM_RCO_HF_TRIM_SHIFT)

#define PRCM_PAD_CLK_OUT_FACTOR_M_SHIFT 3
#define PRCM_PAD_CLK_OUT_FACTOR_M_MASK  (0x1FFFU << PRCM_PAD_CLK_OUT_FACTOR_M_SHIFT)
#define PRCM_PAD_CLK_OUT_FACTOR_M_VAL(v) ( ((v) & 0x1FFFU) << PRCM_PAD_CLK_OUT_FACTOR_M_SHIFT)

#define PRCM_PAD_CLK_OUT_SOURCE_SHIFT   1
#define PRCM_PAD_CLK_OUT_SOURCE_MASK    (0x3U << PRCM_PAD_CLK_OUT_SOURCE_SHIFT)
typedef enum {
        PRCM_PAD_CLK_OUT_LFCLK_RC       = (0x0U << PRCM_PAD_CLK_OUT_SOURCE_SHIFT),
        PRCM_PAD_CLK_OUT_LFCLK_CRYSTAL  = (0x1U << PRCM_PAD_CLK_OUT_SOURCE_SHIFT),
        PRCM_PAD_CLK_OUT_LFCLK_RCO_CALIB = (0x2U << PRCM_PAD_CLK_OUT_SOURCE_SHIFT),
        PRCM_PAD_CLK_OUT_HFCLK_CRYSTAL  = (0x3U << PRCM_PAD_CLK_OUT_SOURCE_SHIFT),
} PRCM_PadClkOutSource;

#define PRCM_PAD_CLK_OUT_EN_BIT         HAL_BIT(0)

/*
 * bit field definition of PRCM->SYS_LFCLK_STATUS
 */

#define PRCM_SYS_LFCLK_STATUS_READY_BIT	HAL_BIT(0)

typedef enum {
        PRCM_LFCLK_MODULE_SYS,          /* power ctrl, cpu, bus, lpuart0/1, gpadc, keyboard */
        PRCM_LFCLK_MODULE_WKTIMER,      /* wakeup timer */
        PRCM_LFCLK_MODULE_WLAN,         /* wlan */
        PRCM_LFCLK_MODULE_RTC,          /* rtc, free running timer */
        PRCM_LFCLK_MODULE_MAX,
} PRCM_LFClkModule;

typedef enum {
        PRCM_LFCLK_SRC_LOSC,            /* external, 32.768K */
        PRCM_LFCLK_SRC_RCOSC,           /* rcosc/32, 20~50K */
        PRCM_LFCLK_SRC_RCCAL,           /* rcosc calibration, about 32K */
        PRCM_LFCLK_SRC_DIV32K,          /* from 32M or HOSC division, 32K */
        PRCM_LFCLK_SRC_MAX,
} PRCM_LFClkSrc;

/*
 * bit field definition of PRCM->RST_DBC_CTRL
 */
#define PRCM_CFG_DBCOUNCE_CLK_DLY_SHIFT	8
#define PRCM_CFG_DBCOUNCE_CLK_DLY_MASK	(0xFU << PRCM_CFG_DBCOUNCE_CLK_DLY_SHIFT)

#define PRCM_CFG_DBCOUNCE_CLK_FREQ_SEL_SHIFT	0
#define PRCM_CFG_DBCOUNCE_CLK_FREQ_SEL_MASK	(0xFU << PRCM_CFG_DBCOUNCE_CLK_FREQ_SEL_SHIFT)

/*
 * bit field definition of PRCM->BOOT_MEDIA_RECORD
 */


/*
 * bit field definition of PRCM->SYS_RCOSC_FREQ_DET
 */
#define PRCM_RCOSC_CALIB_FREQ_SHIFT	8	/* R */
#define PRCM_RCOSC_CALIB_FREQ_VMASK	0xFFFFFU

#define PRCM_RCOSC_CALIB_EN_BIT		HAL_BIT(0)	/* R/W */

/*
 * bit field definition of
 *     - PRCM->BLE_RCOSC_CALIB_CTRL0 (R/W)
 *     - PRCM->BLE_RCOSC_CALIB_CTRL1 (R/W)
 *     - PRCM->BLE_CLK32K_SWITCH_REG0 (R/W)
 *     - PRCM->BLE_CLK32K_SWITCH_REG1 (R/W)
 */
/* bit field definition of PRCM->BLE_RCOSC_CALIB_CTRL0 0x0144 */
#define PRCM_BLE_RCOSC_CALIB_EN_BIT             HAL_BIT(29)
#define PRCM_BLE_RCOSC_CALIB_RST_PUL_BIT        HAL_BIT(28) /* write 1 to reset */
#define PRCM_RCOSC_CALIB_CS_SHIFT               24
#define PRCM_RCOSC_CALIB_CS_MASK                (0x7U << PRCM_RCOSC_CALIB_CS_SHIFT)
typedef enum {
	PRCM_RCOSC_CALIB_CS_IDLE                = (0x0U << PRCM_RCOSC_CALIB_CS_SHIFT),
	PRCM_RCOSC_CALIB_CS_WAIT_DCXO_READY     = (0x1U << PRCM_RCOSC_CALIB_CS_SHIFT),
	PRCM_RCOSC_CALIB_CS_WAIT_CAL_FINISH     = (0x3U << PRCM_RCOSC_CALIB_CS_SHIFT),
	PRCM_RCOSC_CALIB_CS_CAL_FINISH          = (0x6U << PRCM_RCOSC_CALIB_CS_SHIFT),
	PRCM_RCOSC_CALIB_CS_WAIT_TIMER          = (0x2U << PRCM_RCOSC_CALIB_CS_SHIFT),
} PRCM_RCOSC_CALIB_CurrentState;
#define PRCM_RCOSC_CALIB_SW_REQ_PUL_BIT         HAL_BIT(20)
#define PRCM_RCOSC_WK_TIME_EN_BIT               HAL_BIT(16)
#define PRCM_RCOSC_WK_TIME_SHIFT                0
#define PRCM_RCOSC_WK_TIME_MASK                 (0x01FFFU << PRCM_RCOSC_WK_TIME_SHIFT)
#define PRCM_RCOSC_WK_TIME_VAL(n)               (n << PRCM_RCOSC_WK_TIME_SHIFT)


/*
 * bit field definition of PRCM->BLE_RCOSC_CALIB_CTRL1 0x0148
 */
#define PRCM_RCOSC_SCALE_PHASE2_NUM_SHIFT       16
#define PRCM_RCOSC_SCALE_PHASE2_NUM_MASK        (0x0FU << PRCM_RCOSC_SCALE_PHASE2_NUM_SHIFT)
#define PRCM_RCOSC_SCALE_PHASE1_NUM_SHIFT       12
#define PRCM_RCOSC_SCALE_PHASE1_NUM_MASK        (0x0FU << PRCM_RCOSC_SCALE_PHASE1_NUM_SHIFT)
#define PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT 8
#define PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_MASK  (0x07U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT)
typedef enum {
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_10     = (0x0U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_16     = (0x1U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_20     = (0x2U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_24     = (0x3U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_32     = (0x4U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_40     = (0x5U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_64     = (0x6U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_128    = (0x7U << PRCM_RCOSC_SCALE_PHASE3_WK_TIMES_SHIFT),
} PRCM_RCOSC_SCALE_PHASE3_WkTimes;
#define PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT  4
#define PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_MASK   (0x07U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT)
typedef enum {
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_2      = (0x0U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_4      = (0x1U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_6      = (0x2U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_8      = (0x3U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_10     = (0x4U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_12     = (0x5U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_14     = (0x6U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
	PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_16     = (0x7U << PRCM_RCOSC_SCALE_PHASE2_WK_TIMES_SHIFT),
} PRCM_RCOSC_SCALE_PHASE2_WkTimes;
#define PRCM_RCOSC_NORMAL_WK_TIMES_SEL_SHIFT    1
#define PRCM_RCOSC_NORMAL_WK_TIMES_SEL_MASK     (0x01U << PRCM_RCOSC_NORMAL_WK_TIMES_SEL_SHIFT)
typedef enum {
	PRCM_RCOSC_NORMAL_WK_TIMES_SEL_PHASE2   = (0x0U << PRCM_RCOSC_NORMAL_WK_TIMES_SEL_SHIFT),
	PRCM_RCOSC_NORMAL_WK_TIMES_SEL_PHASE3   = (0x1U << PRCM_RCOSC_NORMAL_WK_TIMES_SEL_SHIFT),
} PRCM_RCOSC_NormalWkTimesSel;
#define PRCM_RCOSC_WK_MODE_SEL_SHIFT            0
#define PRCM_RCOSC_WK_MODE_SEL_MASK             (0x01U << PRCM_RCOSC_WK_MODE_SEL_SHIFT)
typedef enum {
	PRCM_RCOSC_WK_MODE_SEL_SCALE            = (0x1U << PRCM_RCOSC_WK_MODE_SEL_SHIFT),
	PRCM_RCOSC_WK_MODE_SEL_NORMAL           = (0x0U << PRCM_RCOSC_WK_MODE_SEL_SHIFT),
} PRCM_RCOSC_WkModSel;

/* bit field definition of PRCM->BLE_CLK32K_SWITCH0 0x014c */
#define PRCM_CLK32K_SW_FORCE_EN_BIT             HAL_BIT(28) /* 28 bit */
#define PRCM_CLK32K_SW_FORCE_DOWN_BIT           HAL_BIT(29) /* 29 bit */
#define PRCM_CLK32K_SW_FORCE_READY_BIT          HAL_BIT(30) /* 30 bit */

#define PRCM_BLE_CLK32K_DIV_SHIFT               (16)
#define PRCM_BLE_CLK32K_DIV_MASK                (0x01FF << PRCM_BLE_CLK32K_DIV_SHIFT)
#define PRCM_BLE_CLK32K_DIV_HALFCYCLE_32M       (499) /* 32M / 32K / 2 - 1 */
#define PRCM_BLE_CLK32K_DIV_VALUE(val)          (((val) << PRCM_BLE_CLK32K_DIV_SHIFT) & PRCM_BLE_CLK32K_DIV_MASK)

#define PRCM_BLE_CLK32K_DIV_CLK_EN_BIT          HAL_BIT(4)
//#define PRCM_SYS_CLK32K_RCO_CALIB_SEL_BIT       HAL_BIT(3)

//#define PRCM_BLE_CLK32K_SEL_SHIFT               (2)
//#define PRCM_BLE_CLK32K_SEL_MASK                (0x01U << PRCM_BLE_CLK32K_SEL_SHIFT)
//typedef enum {
//	PRCM_BLE_CLK32K_SEL_RCOSCORLFCLK            = (0x0U << PRCM_BLE_CLK32K_SEL_SHIFT),
//	PRCM_BLE_CLK32K_SEL_RCCAL                   = (0x1U << PRCM_BLE_CLK32K_SEL_SHIFT),
//} PRCM_BLE_CLK32K_Sel;

#define PRCM_BLE_CLK32K_DIV_SRC_SEL_SHIFT       (1)
#define PRCM_BLE_CLK32K_DIV_SRC_SEL_MASK        (0x01U << PRCM_BLE_CLK32K_DIV_SRC_SEL_SHIFT)
typedef enum {
	PRCM_BLE_CLK32K_DIV_SRC_SEL_HFCLK       = (0x0U << PRCM_BLE_CLK32K_DIV_SRC_SEL_SHIFT),
	PRCM_BLE_CLK32K_DIV_SRC_SEL_32M         = (0x1U << PRCM_BLE_CLK32K_DIV_SRC_SEL_SHIFT),
} PRCM_BLE_CLK32K_DivSrcSel;

#define PRCM_BLE_CLK32K_AUTO_SW_EN_BIT          HAL_BIT(0)

/*
 * bit field definition of PRCM->BLE_CLK32K_SWITCH1 0x0150
 */
#define PRCM_BLE_CLK32K_SW_OFFSET_DOWN_SHIFT    (16)
#define PRCM_BLE_CLK32K_SW_OFFSET_DOWN_VMASK    (0x07FF)
#define PRCM_BLE_CLK32K_SW_OFFSET_ON_SHIFT      (0)
#define PRCM_BLE_CLK32K_SW_OFFSET_ON_VMASK      (0x07FF)

/*
 * bit field definition of PRCM->BT_WLAN_CONN_MODE_CTRL  0x180
 */
#define PRCM_WLAN_RESET_BIT		HAL_BIT(17)	/* R */
#define PRCM_BLE_RESET_BIT		HAL_BIT(16)	/* R */

#define PRCM_WLAN_SHARE_SRAM_SHIFT      (1)
#define PRCM_WLAN_SHARE_SRAM_MASK       (0x1 << PRCM_WLAN_SHARE_SRAM_SHIFT)
typedef enum {
	PRCM_WLAN_SHARE_NONE = (0x0 << PRCM_WLAN_SHARE_SRAM_SHIFT),
	PRCM_WLAN_SHARE_128K = (0x1 << PRCM_WLAN_SHARE_SRAM_SHIFT),
} PRCM_WLAN_ShareSramType;

#define PRCM_CONN_BOOT_FLAG_BIT         HAL_BIT(0)


/*
 * bit field definition of PRCM->CFG_IO_STATUS 0x0188
 */
#define PRCM_FLASH_SIP_EN_BIT           HAL_BIT(0) /* R */


/*
 * bit field definition of PRCM->WLAN_HIF_OV_CTRL 0x18c
 */
#define PRCM_WLAN_IRQ_OVR_HIF_BIT               HAL_BIT(7)
#define PRCM_WLAN_WUP_OVR_HIF_BIT               HAL_BIT(6)
#define PRCM_WLAN_RESET_CPU_OVR_HIF_BIT         HAL_BIT(5)
#define PRCM_WLAN_DISABLE_CPU_CLK_OVR_HIF_BIT   HAL_BIT(4)
#define PRCM_WLAN_WUP_BIT                       HAL_BIT(2)
#define PRCM_WLAN_RESET_CPU_BIT                 HAL_BIT(1)
#define PRCM_WLAN_DISABLE_CPU_CLK_BIT           HAL_BIT(0)

/*
 * bit field definition of PRCM->SRAM_BIST_CFG 0x190
 */
#if 0
#define PRCM_CLK32K_SW_FORCE_EN_BIT             HAL_BIT(24) /* 24 bit */
#define PRCM_CLK32K_SW_FORCE_DOWN_BIT           HAL_BIT(26) /* 26 bit */
#define PRCM_CLK32K_SW_FORCE_READY_BIT          HAL_BIT(27) /* 27 bit */
#endif

/*
 * bit field definition of PRCM->FLASH_ENCRYPT_AES_NONCE0   0x194
 * bit field definition of PRCM->FLASH_ENCRYPT_AES_NONCE1   0x198
 */
// NULL


/*
 * bit field definition of PRCM->AR800A_BOOT_FLAG 0x1c0
 * bit field definition of PRCM->AR800A_BOOT_ADDR 0x1c4
 * bit field definition of PRCM->AR800A_BOOT_ARG 0x1c8
 * bit field definition of PRCM->DSP_BOOT_FLAG 0x1cc
 * bit field definition of PRCM->DSP_BOOT_ADDR 0x1d0
 * bit field definition of PRCM->DSP_BOOT_ARG 0x1d4
 * bit field definition of PRCM->RV_BOOT_FLAG 0x1d8
 * bit field definition of PRCM->RV_BOOT_ADDR 0x1dc
 * bit field definition of PRCM->RV_BOOT_ARG 0x1e0
 */
#define PRCM_CPU_BOOT_FLAG_WR_LOCK		(0x429BU << 16)	/* W */

#define PRCM_CPU_BOOT_FLAG_SHIFT		0	/* R/W */
#define PRCM_CPU_BOOT_FLAG_MASK			(0xFU << PRCM_CPU_BOOT_FLAG_SHIFT)
typedef enum {
	PRCM_CPU_TYPE_INDEX_AR800A = 0,
	PRCM_CPU_TYPE_INDEX_DSP       ,
	PRCM_CPU_TYPE_INDEX_RSICV
} PRCM_CPUTypeIndex;

typedef enum {
	PRCM_CPU_BOOT_FROM_COLD_RESET	= (0U << PRCM_CPU_BOOT_FLAG_SHIFT),
	PRCM_CPU_BOOT_FROM_DEEPSLEEP	= (1U << PRCM_CPU_BOOT_FLAG_SHIFT),
	PRCM_CPU_BOOT_FROM_SYS_UPDATE	= (2U << PRCM_CPU_BOOT_FLAG_SHIFT),
	PRCM_CPU_BOOT_FROM_RECOVERY	= (3U << PRCM_CPU_BOOT_FLAG_SHIFT)
} PRCM_CPUBootFlag;


/*
 * bit field definition of PRCM->SYSTEM_PRIV_REG0T7, 0x200~0x21c
 */
#define PRCM_SYSTEM_PRIV_DATA_ID_NUM 8

/*
 * bit field definition of SYSTEM_FAST_BOOT_INFO_REG 0x0230
 */
// NULL


// PMU REGLIST

/*
 * bit field definition of PMU->DCXO_STABLE_REF_TIME
 */
#define DCXO_STABLE_REFTIME_SHIFT         0   /* R/W */
#define DCXO_STABLE_REFTIME_MASK          (0x1FFU << DCXO_STABLE_REFTIME_SHIFT)
#define DCXO_STABLE_REFTIME_VMASK         (0x1FFU)


/*
 * bit field definition of PMU->DPLL_STABLE_REF_TIME
 */
#define DPLL_STABLE_REFTIME_SHIFT         0   /* R/W */
#define DPLL_STABLE_REFTIME_MASK          (0xFFU << DPLL_STABLE_REFTIME_SHIFT)
#define DPLL_STABLE_REFTIME_VMASK         (0xFFU)



/*
 * bit field definition of PMU->LDO_STABLE_REF_TIME
 */

#define LDO_SETTLING_HFCLK_TIME_SHIFT         16   /* R/W */
#define LDO_SETTLING_HFCLK_TIME_MASK          (0xFFFFU << LDO_SETTLING_HFCLK_TIME_SHIFT)
#define LDO_SETTLING_HFCLK_TIME_VMASK         (0xFFFFU)

#define RFIP_LDO_DIG_STABLE_REFTIME_SHIFT     8   /* R/W */
#define RFIP_LDO_DIG_STABLE_REFTIME_MASK      (0xFU << RFIP_LDO_DIG_STABLE_REFTIME_SHIFT)
#define RFIP_LDO_DIG_STABLE_REFTIME_VMASK     (0xFU)

#define LDO_SETTLING_LFCLK_TIME_SHIFT         0   /* R/W */
#define LDO_SETTLING_LFCLK_TIME_MASK          (0xFFU << LDO_SETTLING_LFCLK_TIME_SHIFT)
#define LDO_SETTLING_LFCLK_TIME_VMASK         (0xFFU)




/*
 * bit field definition of PMU->DIGITAL_SWITCH_REF_TIME
 */
#define DIGITAL_RESETUP_REFTIME_SHIFT         16   /* R/W */
#define DIGITAL_RESETUP_REFTIME_MASK          (0x3FFU << DIGITAL_RESETUP_REFTIME_SHIFT)
#define DIGITAL_RESETUP_REFTIME_VMASK         (0x3FFU)

#define DIGITAL_MTCMOS_SW_SETTLING_TIME_SHIFT 0   /* R/W */
#define DIGITAL_MTCMOS_SW_SETTLING_TIME_MASK  (0x3FFU << DIGITAL_MTCMOS_SW_SETTLING_TIME_SHIFT)
#define DIGITAL_MTCMOS_SW_SETTLING_TIME_VMASK (0x3FFU)


/*
 * bit field definition of PMU->BANDGAP_STABLE_REF_TIME
 */
#define BANDGAP_LDO_STABLE_REFTIME_SHIFT         0   /* R/W */
#define BANDGAP_LDO_STABLE_REFTIME_MASK          (0xFFU << BANDGAP_LDO_STABLE_REFTIME_SHIFT)
#define BANDGAP_LDO_STABLE_REFTIME_VMASK         (0xFFU)

/*
 * bit field definition of PMU->DCDC_STABLE_REF_TIME
 */

#define LDO_TO_PWMMODE_STABLE_REFTIME_SHIFT      16   /* R/W */
#define LDO_TO_PWMMODE_STABLE_REFTIME_MASK       (0x3FFU << LDO_TO_PWMMODE_STABLE_REFTIME_SHIFT)
#define LDO_TO_PWMMODE_STABLE_REFTIME_VMASK      (0x3FFU)


#define OTHER_WORKMODE_SWITCH_STABLE_REFTIME_SHIFT      0   /* R/W */
#define OTHER_WORKMODE_SWITCH_STABLE_REFTIME_MASK       (0x3FFU << OTHER_WORKMODE_SWITCH_STABLE_REFTIME_SHIFT)
#define OTHER_WORKMODE_SWITCH_STABLE_REFTIME_VMASK      (0x3FFU)


/*
 * bit field definition of PMU->SYS_LOW_POWER_CTRL
 */

#define DSP_WUP_ENABLE_BIT                       HAL_BIT(12)
#define RV_WUP_ENABLE_BIT                        HAL_BIT(8)
#define LOWPOWER_MODE_SEL_BIT                    HAL_BIT(0)
#define LOWPOWER_MODE_SEL_SHIFT                  0
#define LOWPOWER_MODE_SEL_MASK                  (0x1U  << LOWPOWER_MODE_SEL_SHIFT)
typedef enum {
	PMU_LOWPOWER_MODE_STANDBY    = (0x0U << LOWPOWER_MODE_SEL_SHIFT),
	PMU_LOWPOWER_MODE_HIBERNATE  = (0x1U << LOWPOWER_MODE_SEL_SHIFT),
} PMU_LowPowerModeSel;



/*
 * bit field definition of PMU->SYS_LOW_POWER_STATUS
 */

#define DSP_SLEEP_BIT                            HAL_BIT(13)  /* RO */
#define DSP_ALIVE_BIT                            HAL_BIT(12)  /* RO */
#define RV_SLEEP_BIT                             HAL_BIT(9)  /* RO */
#define RV_ALIVE_BIT                             HAL_BIT(8)  /* RO */
#define APP_SLEEP_BIT                            HAL_BIT(6)  /* RO */
#define APP_DEEPSLEEP_BIT                        HAL_BIT(5)  /* RO */
#define APP_ALIVE_BIT                            HAL_BIT(4)  /* RO */
#define WLAN_SLEEP_BIT                           HAL_BIT(2)  /* RO */
#define WLAN_STANDBY_BIT                         HAL_BIT(1)  /* RO */
#define WLAN_ALIVE_BIT                           HAL_BIT(0)  /* RO */


/*
 * bit field definition of PMU->RTC_WUP_TIMER_CNT
 */
#define PMU_WAKE_TIMER_EN_BIT                    HAL_BIT(31)	/* R/W */
#define PMU_WAKE_TIMER_CUR_VAL_MASK              0x7FFFFFFFU	/* R */


/*
 * bit field definition of PMU->RTC_WUP_TIMER_CTRL
 */
#define PMU_WAKE_TIMER_PENDING_BIT               HAL_BIT(31)	/* R/W */
#define PMU_WAKE_TIMER_CMP_VAL_MASK              0x7FFFFFFFU	/* R/W */


/*
 * bit field definition of PMU->RTC_IO_WAKE_EN
 * bit field definition of PMU->RTC_IO_WAKE_DEB_CLK
 * bit field definition of PMU->RTC_IO_WAKE_ST
 * bit field definition of PMU->RTC_IO_HOLD_CTRL
 * bit field definition of PMU->RTC_IO_WUP_GEN
 * bit field definition of PMU->RTC_IO_WUP_DEB_CYCLES0
 * bit field definition of PMU->RTC_IO_WUP_DEB_CYCLES1
 */

typedef enum {
	PMU_WAKE_IO_0 = HAL_BIT(0),
	PMU_WAKE_IO_1 = HAL_BIT(1),
	PMU_WAKE_IO_2 = HAL_BIT(2),
	PMU_WAKE_IO_3 = HAL_BIT(3),
	PMU_WAKE_IO_4 = HAL_BIT(4),
	PMU_WAKE_IO_5 = HAL_BIT(5),
	PMU_WAKE_IO_6 = HAL_BIT(6),
	PMU_WAKE_IO_7 = HAL_BIT(7),
	PMU_WAKE_IO_8 = HAL_BIT(8),
	PMU_WAKE_IO_9 = HAL_BIT(9),
#define WAKEUP_IO_NUM	10
#define WAKEUP_IO_MAX	10
} PMU_WakeIO;


#define WAKEUP_IO_MASK                      ((1 << WAKEUP_IO_MAX) - 1)
#define PMU_WAKEUP_IOx_MODE_SHIFT           16
#define PMU_WAKEUP_IOx_EN_SHIFT             0
#define PMU_WAKE_IO_GLOBAL_EN_BIT          HAL_BIT(0)

#define PMU_WKAEUP_DEB_CLK1_SHIFT            28
#define PMU_WKAEUP_DEB_CLK1_MASK             (0xFU << PMU_WKAEUP_DEB_CLK1_SHIFT)
#define PMU_WKAEUP_DEB_CLK0_SHIFT            24
#define PMU_WKAEUP_DEB_CLK0_MASK             (0xFU << PMU_WKAEUP_DEB_CLK0_SHIFT)
#define PMU_WAKEUP_IOX_DEB_CLK_SRC_SHIFT     0
#define PMU_WAKEUP_IOX_DEB_CLK_SRC_MASK      (0x3U << PMU_WAKEUP_IOX_DEB_CLK_SRC_SHIFT)
typedef enum {
	PMU_WAKEUP_IOX_DEB_CLK0_SRC = 0U,
	PMU_WAKEUP_IOX_DEB_CLK1_SRC = 1U,
} PMU_WakeIODebClkSrc;

#define PMU_WAKEUP_IO0T7_DEDOUNCE_CYCLE_SHIFT(n)  (0 + (n * 4))
#define PMU_WAKEUP_IO0T7_DEDOUNCE_CYCLE_MASK(n)   (0xFU << PMU_WAKEUP_IO0T7_DEDOUNCE_CYCLE_SHIFT(n))
#define PMU_WAKEUP_IO8T15_DEDOUNCE_CYCLE_SHIFT(n) (0 + (n * 4))
#define PMU_WAKEUP_IO8T15_DEDOUNCE_CYCLE_MASK(n)  (0xFU << PMU_WAKEUP_IO8T15_DEDOUNCE_CYCLE_SHIFT(n))



/*
 * bit field definition of PMU->VDD_APP_PWROFF_GATING
 */

#define DDR_PHY_POCH_BIT                  HAL_BIT(2)
#define DDR_PHY_IO_LATCH0_BIT             HAL_BIT(1)
#define DDR_PHY_IO_LATCH1_BIT             HAL_BIT(0)


/*
 * bit field definition of PMU->POWERCTRL_CFG_REG
 */

#define DSP_LDO_SHUT_DOWN_BIT             HAL_BIT(14)
#define BT_RSTN_HIGHLVEL_WAKEUP_BIT       HAL_BIT(13)
#define WLAN_IRQ_HIGHLVEL_WAKEUP_BIT      HAL_BIT(12)
#define IGNORE_WICWAKEUP_BIT              HAL_BIT(11)
#define IGNORE_WICENACK_BIT               HAL_BIT(10)
#define WAKEUP_MODE_BIT                   HAL_BIT(9)
#define RTC_ALARM_WUP_TO_POWERCTRL_BIT    HAL_BIT(8)

#define RCOSC_CALIB_START_SRC_SHIFT       6
#define RCOSC_CALIB_START_SRC_MASK        (0x3FU << RCOSC_CALIB_START_SRC_SHIFT)
#define RCOSC_CALIB_START_SRC_VMASK       (0x3FU)
typedef enum {
	PMU_RCOSC_CALIB_START_SRC_APP_SLEEP      = (0x0U << RCOSC_CALIB_START_SRC_SHIFT),
	PMU_RCOSC_CALIB_START_SRC_WLAN_SLEEP     = (0x1U << RCOSC_CALIB_START_SRC_SHIFT),
	PMU_RCOSC_CALIB_START_SRC_APP_WLAN_SLEEP = (0x2U << RCOSC_CALIB_START_SRC_SHIFT),
	PMU_RCOSC_CALIB_START_SRC_LDO1_SHUTDOWN  = (0x3U << RCOSC_CALIB_START_SRC_SHIFT),
} PMU_RcoscCalibStartSrc;

#define CPU_SLEEP_NO_RETENTION_BIT            HAL_BIT(5)
#define BT_SLEEP_RETENTION_OVR_BIT            HAL_BIT(4)
#define BLE_SLEEP_TIMER_IRQ_TO_POWER_CTRL_BIT HAL_BIT(3)
#define WUPTIMER_IRQ_TO_POWERCTRL_BIT         HAL_BIT(2)
#define WUPIO_IRQ_TO_POWERCTRL_BIT            HAL_BIT(1)
#define BT_RSTN_WAKEUPPIN_BIT                 HAL_BIT(0)






/*
 * ******************************************************************
 * function
 * ******************************************************************
 */

uint32_t HAL_PRCM_GetDCDCVoltage(void);
int HAL_PRCM_SetDCDCVoltage(uint32_t volt);

void HAL_PRCM_SetDCDCOffFlowAONLDOEnable(uint8_t enable);
uint32_t HAL_PRCM_GetDCDCOffFlowAONLDOStatus(void);

void HAL_PRCM_SetDCDCOffWhenSYSStandbyEnable(uint8_t enable);
uint32_t HAL_PRCM_GetDCDCOffWhenSYSStandbyStatus(void);

void HAL_PRCM_SetDCDCPwmSelEnable(uint8_t enable);
uint32_t HAL_PRCM_IsDCDCPwmSelEnabled(void);
void HAL_PRCM_SelectDCDCPwmSrc(PRCM_DCDCPwmSrc src);

uint32_t HAL_PRCM_GetOvrDCDCDetctStatus(void);
void HAL_PRCM_SetOvrDCDCDetctEnable(uint8_t enable);
uint32_t HAL_PRCM_GetOvrDCDCDetctValueStatus(void);
uint32_t HAL_PRCM_SetOvrDCDCDetctValueEnable(uint8_t enable);
uint32_t HAL_PRCM_GetDCDCDetctStatus(void);

uint32_t HAL_PRCM_GetDCDCWorkMode(void);

void HAL_PRCM_SetDSPLDOLQModeEnable(uint8_t enable);
void HAL_PRCM_SetAPPLDOLQModeEnable(uint8_t enable);
void HAL_PRCM_SetAONLDOLQModeEnable(uint8_t enable);
void HAL_PRCM_SetLDOModeSWSelEnable(uint8_t enable);

void HAL_PRCM_SetDCDCPFMModeEnable(uint8_t enable);
void HAL_PRCM_SetDCDCModeSWSelEnable(uint8_t enable);

void HAL_PRCM_SetRTCLDOSlpVolt(PRCM_RTCLDOSlpVolt volt);
void HAL_PRCM_SetRTCLDOActVolt(PRCM_RTCLDOActVolt volt);

void HAL_PRCM_SetEXTLDOSwTrim(uint32_t trim);
void HAL_PRCM_SetEXTLDOSwModeEnable(uint8_t enable);
void HAL_PRCM_SelectEXTLDOVolt(PRCM_EXTLDOVolt volt);
void HAL_PRCM_SelectEXTLDOMode(PRCM_ExtLDOMode mode);

uint32_t HAL_PRCM_GetAONLDOWorkVoltage(void);
int HAL_PRCM_SetAONLDOWorkVoltage(uint32_t volt);
uint32_t HAL_PRCM_GetAONLDOSlpVoltage(void);
int HAL_PRCM_SetAONLDOSlpVoltage(uint32_t volt);
uint32_t HAL_PRCM_GetAONLDOLQMode(void);
int HAL_PRCM_IsAONLDOEnabled(void);

uint32_t HAL_PRCM_GetAPPLDOWorkVoltage(void);
int HAL_PRCM_SetAPPLDOWorkVoltage(uint32_t volt);
uint32_t HAL_PRCM_GetAPPLDOSlpVoltage(void);
int HAL_PRCM_SetAPPLDOSlpVoltage(uint32_t volt);
uint32_t HAL_PRCM_GetAPPLDOLQMode(void);
int HAL_PRCM_IsAPPLDOEnabled(void);

uint32_t HAL_PRCM_GetDSPLDOWorkVoltage(void);
int HAL_PRCM_SetDSPLDOWorkVoltage(uint32_t volt);
uint32_t HAL_PRCM_GetDSPLDOSlpVoltage(void);
int HAL_PRCM_SetDSPLDOSlpVoltage(uint32_t volt);
uint32_t HAL_PRCM_GetDSPLDOLQMode(void);
int HAL_PRCM_IsDSPLDOEnabled(void);

uint32_t HAL_PRCM_GetVBATMonLowVoltage(void);
int HAL_PRCM_SetVBATMonLowVoltage(uint32_t volt);
void HAL_PRCM_SetVBATLowVoltMode(PRCM_VbatLowVolMode mode);
void HAL_PRCM_SetVBATMonSwModeEnable(uint8_t enable);
void HAL_PRCM_SetVBATMonitorEnable(uint8_t enable);

void HAL_PRCM_SetUSBBiasTrim(uint32_t trim);
void HAL_PRCM_SetUSBBiasEnable(uint8_t enable);

void HAL_PRCM_SetPadClkOut(uint8_t enable);
void HAL_PRCM_SetPadClkOutSource(PRCM_PadClkOutSource source);
void HAL_PRCM_SetPadClkOutFactorM(uint16_t value);
void HAL_PRCM_SetLFCLKBaseSource(PRCM_LFCLKBaseSrc src);
uint32_t HAL_PRCM_GetInter32KFreq(void);
uint32_t HAL_PRCM_EnableInter32KCalib(void);
uint32_t HAL_PRCM_DisableInter32KCalib(void);
int HAL_PRCM_IsInter32KCalibEnabled(void);
uint32_t HAL_PRCM_GetBaseLFClock(void);
void HAL_PRCM_SetCLK32kDiv(uint32_t en, PRCM_BLE_CLK32K_DivSrcSel sel);
void HAL_PRCM_SetCLK32kAutoSw(uint32_t en);
void HAL_PRCM_SetRcoCalib(uint32_t en, PRCM_RCOSC_WkModSel mode, PRCM_RCOSC_NormalWkTimesSel sel, uint32_t phase, uint32_t wk_time_en, uint32_t wk_time);


// PRCM->BT_WLAN_CONN_MODE_CTRL  0x180
uint32_t HAL_PRCM_GetWLANRstStatus(void);
uint32_t HAL_PRCM_GetBLERstStatus(void);
void HAL_PRCM_SetWlanSramShare(PRCM_WLAN_ShareSramType type);
uint32_t HAL_PRCM_GetConnectModeFlag(void);

// PRCM->CFG_IO_STATUS 0x0188
int HAL_PRCM_IsFlashSip(void);
void HAL_PRCM_SetFlashExt(uint8_t ext);


// PRCM->WLAN_HIF_OV_CTRL 0x18c
void HAL_PRCM_EnableWlanCPUClk(uint8_t enable);
void HAL_PRCM_ReleaseWlanCPUReset(void);
void HAL_PRCM_ForceWlanCPUReset(void);
void HAL_PRCM_WakeUpWlan(uint8_t wakeup);
void HAL_PRCM_EnableWlanCPUClkOvrHIF(void);
void HAL_PRCM_DisableWlanCPUClkOvrHIF(void);
void HAL_PRCM_ReleaseWlanCPUOvrHIF(void);
void HAL_PRCM_ResetWlanCPUOvrHIF(void);
void HAL_PRCM_EnableWlanWUPOvrHIF(void);
void HAL_PRCM_DisableWlanWUPOvrHIF(void);
void HAL_PRCM_EnableWlanIRQOvrHIF(void);
void HAL_PRCM_DisableWlanIRQOvrHIF(void);


uint32_t HAL_PRCM_GetRcoscCalSwitchLoseClk();
void HAL_PRCM_SetCLK32KSwitchRCCal(uint8_t enable);


// PRCM->FLASH_ENCRYPT_AES_NONCE0   0x194
// PRCM->FLASH_ENCRYPT_AES_NONCE1   0x198
void HAL_PRCM_SetFlashCryptoNonce(uint8_t *nonce);



// PRCM->AR800A_BOOT_FLAG 0x1c0
// PRCM->AR800A_BOOT_ADDR 0x1c4
// PRCM->AR800A_BOOT_ARG 0x1c8
// PRCM->DSP_BOOT_FLAG 0x1cc
// PRCM->DSP_BOOT_ADDR 0x1d0
// PRCM->DSP_BOOT_ARG 0x1d4
// PRCM->RV_BOOT_FLAG 0x1d8
// PRCM->RV_BOOT_ADDR 0x1dc
// PRCM->RV_BOOT_ARG 0x1e0
void HAL_PRCM_SetCPUBootFlag(PRCM_CPUTypeIndex cpu, PRCM_CPUBootFlag flag);
uint32_t HAL_PRCM_GetCPUBootFlag(PRCM_CPUTypeIndex cpu);
void HAL_PRCM_SetCPUBootAddr(PRCM_CPUTypeIndex cpu, uint32_t addr);
uint32_t HAL_PRCM_GetCPUBootAddr(PRCM_CPUTypeIndex cpu);
void HAL_PRCM_SetCPUBootArg(PRCM_CPUTypeIndex cpu, uint32_t arg);
uint32_t HAL_PRCM_GetCPUBootArg(PRCM_CPUTypeIndex cpu);

// PRCM->SYSTEM_PRIV_REG0T7, 0x200~0x21c
void HAL_PRCM_SetSystemPrivateData(uint32_t id, uint32_t data);
uint32_t HAL_PRCM_GetSystemPrivateData(uint32_t id);


//  PMU FUNCTION

uint32_t HAL_PMU_GetDCXOStableRefTime(void);
void HAL_PMU_SetDCXOStableRefTime(uint32_t val);
uint32_t HAL_PMU_GetDPLLStableRefTime(void);
void HAL_PMU_SetDPLLStableRefTime(uint32_t val);
uint32_t HAL_PMU_GetLDOSettlingHFclkTime(void);
void HAL_PMU_SetLDOSettlingHFclkTime(uint32_t val);
uint32_t HAL_PMU_GetRFIPLDODIGStableRefTime(void);
void HAL_PMU_SetRFIPLDODIGStableRefTime(uint32_t val);
uint32_t HAL_PMU_GetLDOSettlingLFclkTime(void);
void HAL_PMU_SetLDOSettlingLFclkTime(uint32_t val);
uint32_t HAL_PMU_GetGigitalResetupRefTime(void);
void HAL_PMU_SetGigitalResetupRefTime(uint32_t val);
uint32_t HAL_PMU_GetGigitalMTcmosSwSettlingTime(void);
void HAL_PMU_SetGigitalMTcmosSwSettlingTime(uint32_t val);
uint32_t HAL_PMU_GetBandgapLDOStableRefTime(void);
void HAL_PMU_SetBandgapLDOStableRefTime(uint32_t val);
uint32_t HAL_PMU_GetLDOToPWMModeStableRefTime(void);
void HAL_PMU_SetLDOToPWMModeStableRefTime(uint32_t val);
uint32_t HAL_PMU_GetOtherWorkSwitchStableRefTime(void);
void HAL_PMU_SetOtherWorkSwitchStableRefTime(uint32_t val);
void HAL_PMU_SetDSPWakeupActive(uint8_t active);
void HAL_PMU_SetRVWakeupActive(uint8_t active);
void HAL_PMU_SetLowPowerMode(PMU_LowPowerModeSel mode);
uint32_t HAL_PMU_GetLowPowerMode(void);
uint32_t HAL_PMU_IsDSPSleepStatus(void);
uint32_t HAL_PMU_IsDSPAliveStatus(void);
uint32_t HAL_PMU_IsRVSleepStatus(void);
uint32_t HAL_PMU_IsRVAliveStatus(void);
uint32_t HAL_PMU_IsAPPSleepStatus(void);
uint32_t HAL_PMU_IsAPPDeepSleepStatus(void);
uint32_t HAL_PMU_IsAPPAliveStatus(void);
uint32_t HAL_PMU_IsWLANSleepStatus(void);
uint32_t HAL_PMU_IsWLANStandbyStatus(void);
uint32_t HAL_PMU_IsWLANAliveStatus(void);
uint32_t HAL_PMU_IsWakeupTimerEnabled(void);
void HAL_PMU_WakeupTimerEnable(void);
void HAL_PMU_WakeupTimerDisable(void);
uint32_t HAL_PMU_WakeupTimerGetCurrentValue(void);
uint32_t HAL_PMU_WakeupTimerIsPending(void);
void HAL_PMU_WakeupTimerClearPending(void);
void HAL_PMU_SetWakeupTimerValue(uint32_t val);
uint32_t HAL_PMU_GetWakeupTimerValue(void);
void HAL_PMU_WakeupIOEnable(PMU_WakeIO ioMask);
void HAL_PMU_WakeupIODisable(PMU_WakeIO ioMask);
void HAL_PMU_EnableWakeupIOx(uint8_t ioIndex, uint8_t enable);
void HAL_PMU_WakeupIOSetRisingEvent(PMU_WakeIO ioMask);
void HAL_PMU_WakeupIOSetFallingEvent(PMU_WakeIO ioMask);
uint32_t HAL_PMU_WakeupIOGetEventStatus(void);
uint32_t HAL_PMU_WakeupIOClearEventStatus(void);
int HAL_PMU_WakeupIOGetEventDetected(PMU_WakeIO ioMask);
void HAL_PMU_WakeupIOClearEventDetected(PMU_WakeIO ioMask);
void HAL_PMU_WakeupIOEnableCfgHold(PMU_WakeIO ioMask);
void HAL_PMU_WakeupIODisableCfgHold(PMU_WakeIO ioMask);
void HAL_PMU_WakeupIOEnableGlobal(void);
void HAL_PMU_WakeupIODisableGlobal(void);
void HAL_PMU_SetWakeupIOxDebSrc(uint8_t ioIndex, PMU_WakeIODebClkSrc src);
void HAL_PMU_SetWakeupDebClk0(uint8_t val);
void HAL_PMU_SetWakeupDebClk1(uint8_t val);
void HAL_PMU_SetWakeupIOxDebounce(uint8_t ioIndex, uint8_t val);
PMU_WakeIO HAL_PMU_WakeupIOxIndex2Mask(uint8_t ioIndex);
uint8_t HAL_PMU_WakeupIOxMask2Index(PMU_WakeIO ioMask);
void HAL_PMU_SetDDRPhyPochEnable(uint8_t enable);
void HAL_PMU_SetDDRPhyIOLatch0Enable(uint8_t enable);
void HAL_PMU_SetDDRPhyIOLatch1Enable(uint8_t enable);
void HAL_PMU_SetDSPLDOOvrShutDown(uint8_t enable);
void HAL_PMU_SetBTRstHighLevelWakeup(uint8_t enable);
void HAL_PMU_SetWlanIrqHighLevelWakeup(uint8_t enable);
void HAL_PMU_SetIgnoreWicWakeup(uint8_t enable);
void HAL_PMU_SetIgnoreWicNack(uint8_t enable);
void HAL_PMU_SetWakeupMode(uint8_t enable);
void HAL_PMU_SetRtcAlarmWakeupToPowerCtrl(uint8_t enable);
void HAL_PMU_SetRcoscCalibStartSrc(PMU_RcoscCalibStartSrc src);
void HAL_PMU_SetCpuSlpNoRetention(uint8_t enable);
void HAL_PMU_SetBTSlpRetentionOvr(uint8_t enable);
void HAL_PMU_SetBLESlpTimerIrqToPowerctrl(uint8_t enable);
void HAL_PMU_SetWupTimerIrqToPowerCtrl(uint8_t enable);
void HAL_PMU_SetWupIOToPowerCtrl(uint8_t enable);
void HAL_PMU_SetBTRstnAsWupPin(uint8_t enable);



#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_PRCM_H_ */

