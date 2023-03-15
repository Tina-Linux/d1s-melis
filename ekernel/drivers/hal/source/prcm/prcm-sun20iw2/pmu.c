#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prcm.h"

/*
 * bit field definition of PMU->DCXO_STABLE_REF_TIME
 */

/* Base on LFCLK*n */
uint32_t HAL_PMU_GetDCXOStableRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->DCXO_STABLE_REF_TIME, DCXO_STABLE_REFTIME_SHIFT, \
			DCXO_STABLE_REFTIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetDCXOStableRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->DCXO_STABLE_REF_TIME, DCXO_STABLE_REFTIME_MASK,
			(val & DCXO_STABLE_REFTIME_VMASK) << DCXO_STABLE_REFTIME_SHIFT);
}


/*
 * bit field definition of PMU->DPLL_STABLE_REF_TIME
 */

/* Base on LFCLK*n */
uint32_t HAL_PMU_GetDPLLStableRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->DPLL_STABLE_REF_TIME, DPLL_STABLE_REFTIME_SHIFT, \
			DPLL_STABLE_REFTIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetDPLLStableRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->DPLL_STABLE_REF_TIME, DPLL_STABLE_REFTIME_MASK,
			(val & DPLL_STABLE_REFTIME_VMASK) << DPLL_STABLE_REFTIME_SHIFT);
}


/*
 * bit field definition of PMU->LDO_STABLE_REF_TIME
 */
/* Base on HFCLK*n */
uint32_t HAL_PMU_GetLDOSettlingHFclkTime(void)
{
	return HAL_GET_BIT_VAL(PMU->LDO_STABLE_REF_TIME, LDO_SETTLING_HFCLK_TIME_SHIFT, \
			LDO_SETTLING_HFCLK_TIME_VMASK);
}

/* Base on HFCLK*n */
void HAL_PMU_SetLDOSettlingHFclkTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->LDO_STABLE_REF_TIME, LDO_SETTLING_HFCLK_TIME_MASK,
			(val & LDO_SETTLING_HFCLK_TIME_VMASK) << LDO_SETTLING_HFCLK_TIME_SHIFT);
}

/* Base on LFCLK*n */
uint32_t HAL_PMU_GetRFIPLDODIGStableRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->LDO_STABLE_REF_TIME, RFIP_LDO_DIG_STABLE_REFTIME_SHIFT, \
			RFIP_LDO_DIG_STABLE_REFTIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetRFIPLDODIGStableRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->LDO_STABLE_REF_TIME, RFIP_LDO_DIG_STABLE_REFTIME_MASK,
			(val & RFIP_LDO_DIG_STABLE_REFTIME_VMASK) << RFIP_LDO_DIG_STABLE_REFTIME_SHIFT);
}


/* Base on LFCLK*n */
uint32_t HAL_PMU_GetLDOSettlingLFclkTime(void)
{
	return HAL_GET_BIT_VAL(PMU->LDO_STABLE_REF_TIME, LDO_SETTLING_LFCLK_TIME_SHIFT, \
			LDO_SETTLING_LFCLK_TIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetLDOSettlingLFclkTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->LDO_STABLE_REF_TIME, LDO_SETTLING_LFCLK_TIME_MASK,
			(val & LDO_SETTLING_LFCLK_TIME_VMASK) << LDO_SETTLING_LFCLK_TIME_SHIFT);
}



/*
 * bit field definition of PMU->DIGITAL_SWITCH_REF_TIME
 */

/* Base on HOSC *n */
uint32_t HAL_PMU_GetGigitalResetupRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->DIGITAL_SWITCH_REF_TIME, DIGITAL_RESETUP_REFTIME_SHIFT, \
			DIGITAL_RESETUP_REFTIME_VMASK);
}

/* Base on HOSC *n */
void HAL_PMU_SetGigitalResetupRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->DIGITAL_SWITCH_REF_TIME, DIGITAL_RESETUP_REFTIME_MASK,
			(val & DIGITAL_RESETUP_REFTIME_VMASK) << DIGITAL_RESETUP_REFTIME_SHIFT);
}


/* Base on HFCLK*n */
uint32_t HAL_PMU_GetGigitalMTcmosSwSettlingTime(void)
{
	return HAL_GET_BIT_VAL(PMU->DIGITAL_SWITCH_REF_TIME, DIGITAL_MTCMOS_SW_SETTLING_TIME_SHIFT, \
			DIGITAL_MTCMOS_SW_SETTLING_TIME_VMASK);
}

/* Base on HFCLK*n */
void HAL_PMU_SetGigitalMTcmosSwSettlingTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->DIGITAL_SWITCH_REF_TIME, DIGITAL_MTCMOS_SW_SETTLING_TIME_MASK,
			(val & DIGITAL_MTCMOS_SW_SETTLING_TIME_VMASK) << DIGITAL_MTCMOS_SW_SETTLING_TIME_SHIFT);
}


/*
 * bit field definition of PMU->BANDGAP_STABLE_REF_TIME
 */

/* Base on LFCLK*n */
uint32_t HAL_PMU_GetBandgapLDOStableRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->BANDGAP_STABLE_REF_TIME, BANDGAP_LDO_STABLE_REFTIME_SHIFT, \
			BANDGAP_LDO_STABLE_REFTIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetBandgapLDOStableRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->BANDGAP_STABLE_REF_TIME, BANDGAP_LDO_STABLE_REFTIME_MASK,
			(val & BANDGAP_LDO_STABLE_REFTIME_VMASK) << BANDGAP_LDO_STABLE_REFTIME_SHIFT);
}


/*
 * bit field definition of PMU->DCDC_STABLE_REF_TIME
 */

/* Base on LFCLK*n */
uint32_t HAL_PMU_GetLDOToPWMModeStableRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->DCDC_STABLE_REF_TIME, LDO_TO_PWMMODE_STABLE_REFTIME_SHIFT, \
			LDO_TO_PWMMODE_STABLE_REFTIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetLDOToPWMModeStableRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->DCDC_STABLE_REF_TIME, LDO_TO_PWMMODE_STABLE_REFTIME_MASK,
			(val & LDO_TO_PWMMODE_STABLE_REFTIME_VMASK) << LDO_TO_PWMMODE_STABLE_REFTIME_SHIFT);
}

/* Base on LFCLK*n */
uint32_t HAL_PMU_GetOtherWorkSwitchStableRefTime(void)
{
	return HAL_GET_BIT_VAL(PMU->DCDC_STABLE_REF_TIME, OTHER_WORKMODE_SWITCH_STABLE_REFTIME_SHIFT, \
			OTHER_WORKMODE_SWITCH_STABLE_REFTIME_VMASK);
}

/* Base on LFCLK*n */
void HAL_PMU_SetOtherWorkSwitchStableRefTime(uint32_t val)
{
	HAL_MODIFY_REG(PMU->DCDC_STABLE_REF_TIME, OTHER_WORKMODE_SWITCH_STABLE_REFTIME_MASK,
			(val & OTHER_WORKMODE_SWITCH_STABLE_REFTIME_VMASK) << OTHER_WORKMODE_SWITCH_STABLE_REFTIME_SHIFT);
}



/*
 * bit field definition of PMU->SYS_LOW_POWER_CTRL
 */

void HAL_PMU_SetDSPWakeupActive(uint8_t active)
{
	if (active) {
		HAL_SET_BIT(PMU->SYS_LOW_POWER_CTRL, DSP_WUP_ENABLE_BIT);
	}  else {
		HAL_CLR_BIT(PMU->SYS_LOW_POWER_CTRL, DSP_WUP_ENABLE_BIT);
	}
}

void HAL_PMU_SetRVWakeupActive(uint8_t active)
{
	if (active) {
		HAL_SET_BIT(PMU->SYS_LOW_POWER_CTRL, RV_WUP_ENABLE_BIT);
	}  else {
		HAL_CLR_BIT(PMU->SYS_LOW_POWER_CTRL, RV_WUP_ENABLE_BIT);
	}
}


void HAL_PMU_SetLowPowerMode(PMU_LowPowerModeSel mode)
{
	if (mode) {
		HAL_SET_BIT(PMU->SYS_LOW_POWER_CTRL, LOWPOWER_MODE_SEL_BIT);
	} else {
		HAL_CLR_BIT(PMU->SYS_LOW_POWER_CTRL, LOWPOWER_MODE_SEL_BIT);
	}
}

uint32_t HAL_PMU_GetLowPowerMode(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_CTRL, LOWPOWER_MODE_SEL_MASK);
}


/*
 * bit field definition of PMU->SYS_LOW_POWER_STATUS
 */

uint32_t HAL_PMU_IsDSPSleepStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, DSP_SLEEP_BIT);
}

uint32_t HAL_PMU_IsDSPAliveStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, DSP_ALIVE_BIT);
}


uint32_t HAL_PMU_IsRVSleepStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, RV_SLEEP_BIT);
}

uint32_t HAL_PMU_IsRVAliveStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, RV_ALIVE_BIT);
}

uint32_t HAL_PMU_IsAPPSleepStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, APP_SLEEP_BIT);
}

uint32_t HAL_PMU_IsAPPDeepSleepStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, APP_DEEPSLEEP_BIT);
}

uint32_t HAL_PMU_IsAPPAliveStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, APP_ALIVE_BIT);
}


uint32_t HAL_PMU_IsWLANSleepStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, WLAN_SLEEP_BIT);
}

uint32_t HAL_PMU_IsWLANStandbyStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, WLAN_STANDBY_BIT);
}

uint32_t HAL_PMU_IsWLANAliveStatus(void)
{
	return !!HAL_GET_BIT(PMU->SYS_LOW_POWER_STATUS, WLAN_ALIVE_BIT);
}



/*
 * bit field definition of PMU->RTC_WUP_TIMER_CNT
 */

uint32_t HAL_PMU_IsWakeupTimerEnabled(void)
{
	return !!HAL_GET_BIT(PMU->RTC_WUP_TIMER_CNT, PMU_WAKE_TIMER_EN_BIT);
}

void HAL_PMU_WakeupTimerEnable(void)
{
	HAL_SET_BIT(PMU->RTC_WUP_TIMER_CNT, PMU_WAKE_TIMER_EN_BIT);
}

void HAL_PMU_WakeupTimerDisable(void)
{
	HAL_CLR_BIT(PMU->RTC_WUP_TIMER_CNT, PMU_WAKE_TIMER_EN_BIT);
}

uint32_t HAL_PMU_WakeupTimerGetCurrentValue(void)
{
	return (PMU->RTC_WUP_TIMER_CNT & PMU_WAKE_TIMER_CUR_VAL_MASK);
}


/*
 * bit field definition of PMU->RTC_WUP_TIMER_CTRL
 */

uint32_t HAL_PMU_WakeupTimerIsPending(void)
{
	return !!HAL_GET_BIT(PMU->RTC_WUP_TIMER_CTRL, PMU_WAKE_TIMER_PENDING_BIT);
}

void HAL_PMU_WakeupTimerClearPending(void)
{
	HAL_SET_BIT(PMU->RTC_WUP_TIMER_CTRL, PMU_WAKE_TIMER_PENDING_BIT);
}

void HAL_PMU_SetWakeupTimerValue(uint32_t val)
{
	HAL_MODIFY_REG(PMU->RTC_WUP_TIMER_CTRL, PMU_WAKE_TIMER_CMP_VAL_MASK, \
			val & PMU_WAKE_TIMER_CMP_VAL_MASK);
}

uint32_t HAL_PMU_GetWakeupTimerValue(void)
{
	return (PMU->RTC_WUP_TIMER_CTRL & PMU_WAKE_TIMER_CMP_VAL_MASK);
}


/*
 * bit field definition of PMU->RTC_IO_WAKE_EN
 * bit field definition of PMU->RTC_IO_WAKE_DEB_CLK
 * bit field definition of PMU->RTC_IO_WAKE_ST
 * bit field definition of PMU->RTC_IO_HOLD_CTRL
 * bit field definition of PMU->RTC_IO_WUP_GEN
 * bit field definition of PMU->RTC_IO_WUP_DEB_CYCLES0
 * bit field definition of PMU->RTC_IO_WUP_DEB_CYCLES1
 */

void HAL_PMU_WakeupIOEnable(PMU_WakeIO ioMask)
{
	HAL_SET_BIT(PMU->RTC_IO_WAKE_EN, ioMask << PMU_WAKEUP_IOx_EN_SHIFT);
}

void HAL_PMU_WakeupIODisable(PMU_WakeIO ioMask)
{
	HAL_CLR_BIT(PMU->RTC_IO_WAKE_EN, ioMask << PMU_WAKEUP_IOx_EN_SHIFT);
}

void HAL_PMU_EnableWakeupIOx(uint8_t ioIndex, uint8_t enable)
{
	HAL_ASSERT_PARAM(ioIndex < WAKEUP_IO_MAX);

	if (enable)
		HAL_SET_BIT(PMU->RTC_IO_WAKE_EN, 1 << ioIndex);
	else
		HAL_CLR_BIT(PMU->RTC_IO_WAKE_EN, 1 << ioIndex);
}

void HAL_PMU_WakeupIOSetRisingEvent(PMU_WakeIO ioMask)
{
	HAL_SET_BIT(PMU->RTC_IO_WAKE_EN, ioMask << PMU_WAKEUP_IOx_MODE_SHIFT);
}

void HAL_PMU_WakeupIOSetFallingEvent(PMU_WakeIO ioMask)
{
	HAL_CLR_BIT(PMU->RTC_IO_WAKE_EN, ioMask << PMU_WAKEUP_IOx_MODE_SHIFT);
}

uint32_t HAL_PMU_WakeupIOGetEventStatus(void)
{
	/*note: the func will return all WKUPIO event.*/
	return (PMU->RTC_IO_WAKE_ST & WAKEUP_IO_MASK);
}

uint32_t HAL_PMU_WakeupIOClearEventStatus(void)
{
	/*note: the func will clear all WKUPIO event.*/
	return (PMU->RTC_IO_WAKE_ST |= WAKEUP_IO_MASK);
}

int HAL_PMU_WakeupIOGetEventDetected(PMU_WakeIO ioMask)
{
	return !!HAL_GET_BIT(PMU->RTC_IO_WAKE_ST, ioMask);
}

void HAL_PMU_WakeupIOClearEventDetected(PMU_WakeIO ioMask)
{
	HAL_SET_BIT(PMU->RTC_IO_WAKE_ST, ioMask);
}

void HAL_PMU_WakeupIOEnableCfgHold(PMU_WakeIO ioMask)
{
	HAL_SET_BIT(PMU->RTC_IO_HOLD_CTRL, ioMask);
}

void HAL_PMU_WakeupIODisableCfgHold(PMU_WakeIO ioMask)
{
	HAL_CLR_BIT(PMU->RTC_IO_HOLD_CTRL, ioMask);
}

void HAL_PMU_WakeupIOEnableGlobal(void)
{
	HAL_SET_BIT(PMU->RTC_IO_WUP_GEN, PMU_WAKE_IO_GLOBAL_EN_BIT);
}

void HAL_PMU_WakeupIODisableGlobal(void)
{
	HAL_CLR_BIT(PMU->RTC_IO_WUP_GEN, PMU_WAKE_IO_GLOBAL_EN_BIT);
}

void HAL_PMU_SetWakeupIOxDebSrc(uint8_t ioIndex, PMU_WakeIODebClkSrc src)
{
	HAL_ASSERT_PARAM(ioIndex < WAKEUP_IO_MAX);

	switch (src) {
	case PMU_WAKEUP_IOX_DEB_CLK0_SRC:
		HAL_SET_BIT(PMU->RTC_IO_WAKE_DEB_CLK, 1 << ioIndex);
		break;
	case PMU_WAKEUP_IOX_DEB_CLK1_SRC:
		HAL_CLR_BIT(PMU->RTC_IO_WAKE_DEB_CLK, 1 << ioIndex);
		break;
	default:
		break;
	}
}

void HAL_PMU_SetWakeupDebClk0(uint8_t val)
{
	HAL_MODIFY_REG(PMU->RTC_IO_WAKE_DEB_CLK, PMU_WKAEUP_DEB_CLK0_MASK, val << PMU_WKAEUP_DEB_CLK0_SHIFT);
}

void HAL_PMU_SetWakeupDebClk1(uint8_t val)
{
	HAL_MODIFY_REG(PMU->RTC_IO_WAKE_DEB_CLK, PMU_WKAEUP_DEB_CLK1_MASK, val << PMU_WKAEUP_DEB_CLK1_SHIFT);
}

void HAL_PMU_SetWakeupIOxDebounce(uint8_t ioIndex, uint8_t val)
{
	HAL_ASSERT_PARAM(ioIndex < WAKEUP_IO_MAX);

	if (ioIndex < 8) {
		HAL_MODIFY_REG(PMU->RTC_IO_WUP_DEB_CYCLES0,
		               PMU_WAKEUP_IO0T7_DEDOUNCE_CYCLE_MASK(ioIndex),
		               val << PMU_WAKEUP_IO0T7_DEDOUNCE_CYCLE_SHIFT(ioIndex));
	} else if (ioIndex < WAKEUP_IO_MAX) {
		HAL_MODIFY_REG(PMU->RTC_IO_WUP_DEB_CYCLES1,
		               PMU_WAKEUP_IO8T15_DEDOUNCE_CYCLE_MASK(ioIndex - 8),
		               val << PMU_WAKEUP_IO8T15_DEDOUNCE_CYCLE_SHIFT(ioIndex - 8));
	}
}


PMU_WakeIO HAL_PMU_WakeupIOxIndex2Mask(uint8_t ioIndex)
{
	HAL_ASSERT_PARAM(ioIndex < WAKEUP_IO_MAX);

	return 1 << ioIndex;
}

uint8_t HAL_PMU_WakeupIOxMask2Index(PMU_WakeIO ioMask)
{
	uint8_t ioIndex;
	switch (ioMask) {
	case HAL_BIT(9):
		ioIndex = 9;
		break;
	case HAL_BIT(8):
		ioIndex = 8;
		break;
	case HAL_BIT(7):
		ioIndex = 7;
		break;
	case HAL_BIT(6):
		ioIndex = 6;
		break;
	case HAL_BIT(5):
		ioIndex = 5;
		break;
	case HAL_BIT(4):
		ioIndex = 4;
		break;
	case HAL_BIT(3):
		ioIndex = 3;
		break;
	case HAL_BIT(2):
		ioIndex = 2;
		break;
	case HAL_BIT(1):
		ioIndex = 1;
		break;
	case HAL_BIT(0):
		ioIndex = 0;
		break;
	default:
		/* Force Halt, because it is inVaild. */
		HAL_ASSERT_PARAM(0);
		break;
	}
	return ioIndex;
}



/*
 * bit field definition of PMU->VDD_APP_PWROFF_GATING
 */

/*
 * Note:
 * 1, if it is 1, the system will disable IO, and the IO state depends on the pull up and down.
 * 2, to set 0 after AON_LDO and APP_LDO are opened and stabilized.
 * 3, to set 1 before AON_LDO opened.
 */
void HAL_PMU_SetDDRPhyPochEnable(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->VDD_APP_PWROFF_GATING, DDR_PHY_POCH_BIT);
	}  else {
		HAL_CLR_BIT(PMU->VDD_APP_PWROFF_GATING, DDR_PHY_POCH_BIT);
	}
}

void HAL_PMU_SetDDRPhyIOLatch0Enable(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->VDD_APP_PWROFF_GATING, DDR_PHY_IO_LATCH0_BIT);
	}  else {
		HAL_CLR_BIT(PMU->VDD_APP_PWROFF_GATING, DDR_PHY_IO_LATCH0_BIT);
	}
}

void HAL_PMU_SetDDRPhyIOLatch1Enable(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->VDD_APP_PWROFF_GATING, DDR_PHY_IO_LATCH1_BIT);
	}  else {
		HAL_CLR_BIT(PMU->VDD_APP_PWROFF_GATING, DDR_PHY_IO_LATCH1_BIT);
	}
}


/*
 * bit field definition of PMU->POWERCTRL_CFG_REG
 */

void HAL_PMU_SetDSPLDOOvrShutDown(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, DSP_LDO_SHUT_DOWN_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, DSP_LDO_SHUT_DOWN_BIT);
	}
}

void HAL_PMU_SetBTRstHighLevelWakeup(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, BT_RSTN_HIGHLVEL_WAKEUP_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, BT_RSTN_HIGHLVEL_WAKEUP_BIT);
	}
}

void HAL_PMU_SetWlanIrqHighLevelWakeup(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, WLAN_IRQ_HIGHLVEL_WAKEUP_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, WLAN_IRQ_HIGHLVEL_WAKEUP_BIT);
	}
}

void HAL_PMU_SetIgnoreWicWakeup(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, IGNORE_WICWAKEUP_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, IGNORE_WICWAKEUP_BIT);
	}
}

void HAL_PMU_SetIgnoreWicNack(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, IGNORE_WICENACK_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, IGNORE_WICENACK_BIT);
	}
}

void HAL_PMU_SetWakeupMode(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, WAKEUP_MODE_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, WAKEUP_MODE_BIT);
	}
}

void HAL_PMU_SetRtcAlarmWakeupToPowerCtrl(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, RTC_ALARM_WUP_TO_POWERCTRL_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, RTC_ALARM_WUP_TO_POWERCTRL_BIT);
	}
}

void HAL_PMU_SetRcoscCalibStartSrc(PMU_RcoscCalibStartSrc src)
{
	HAL_MODIFY_REG(PMU->POWERCTRL_CFG_REG, \
			RCOSC_CALIB_START_SRC_MASK, src);
}

void HAL_PMU_SetCpuSlpNoRetention(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, CPU_SLEEP_NO_RETENTION_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, CPU_SLEEP_NO_RETENTION_BIT);
	}
}

void HAL_PMU_SetBTSlpRetentionOvr(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, BT_SLEEP_RETENTION_OVR_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, BT_SLEEP_RETENTION_OVR_BIT);
	}
}

void HAL_PMU_SetBLESlpTimerIrqToPowerctrl(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, BLE_SLEEP_TIMER_IRQ_TO_POWER_CTRL_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, BLE_SLEEP_TIMER_IRQ_TO_POWER_CTRL_BIT);
	}
}

void HAL_PMU_SetWupTimerIrqToPowerCtrl(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, WUPTIMER_IRQ_TO_POWERCTRL_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, WUPTIMER_IRQ_TO_POWERCTRL_BIT);
	}
}

void HAL_PMU_SetWupIOToPowerCtrl(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, WUPIO_IRQ_TO_POWERCTRL_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, WUPIO_IRQ_TO_POWERCTRL_BIT);
	}
}

void HAL_PMU_SetBTRstnAsWupPin(uint8_t enable)
{
	if (enable) {
		HAL_SET_BIT(PMU->POWERCTRL_CFG_REG, BT_RSTN_WAKEUPPIN_BIT);
	}  else {
		HAL_CLR_BIT(PMU->POWERCTRL_CFG_REG, BT_RSTN_WAKEUPPIN_BIT);
	}
}


