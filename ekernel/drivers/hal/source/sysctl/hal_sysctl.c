
/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.


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

#include <stdio.h>
#include <stdint.h>
#include <hal_log.h>
#include <hal_time.h>
#include <sunxi_hal_common.h>
#include <sunxi_hal_sysctl.h>

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

#define HAL_ASSERT_PARAM(exp)                                           \
    do {                                                                \
        if (!(exp)) {                                                   \
            hal_log_info("Invalid param at %s:%d\n", __func__, __LINE__); \
        }                                                               \
    } while (0)


void HAL_SYSCTL_SetSipFlashTestMapMode(uint32_t en)
{
    if (en)
        HAL_SET_BIT(SYSCTL->SIP_TEST_MAP, SYSCTL_SIP_TEST_MAP_WRITE_EN | SYSCTL_SIP_FLASH_TEST_MAP_EN);
    else
        SYSCTL->SIP_TEST_MAP = SYSCTL_SIP_TEST_MAP_WRITE_EN;
}


void HAL_SYSCTL_RamUseAsBT(uint32_t en)
{
    if (en)
        HAL_SET_BIT(SYSCTL->RAM_REMAP_REG, SYSCTL_BT_RAM_REMAP_MASK);
    else
        HAL_CLR_BIT(SYSCTL->RAM_REMAP_REG, SYSCTL_BT_RAM_REMAP_MASK);
}

void HAL_SYSCTL_RamUseAsCSI(uint32_t en)
{
    if (en)
        HAL_SET_BIT(SYSCTL->RAM_REMAP_REG, SYSCTL_CSI_RAM_REMAP_MASK);
    else
        HAL_CLR_BIT(SYSCTL->RAM_REMAP_REG, SYSCTL_CSI_RAM_REMAP_MASK);
}

void HAL_SYSCTL_RomSecureSel(uint32_t en)
{
    if (en)
        HAL_SET_BIT(SYSCTL->RAM_REMAP_REG, SYSCTL_ROM_SECURE_SEL_MASK);
    else
        HAL_CLR_BIT(SYSCTL->RAM_REMAP_REG, SYSCTL_ROM_SECURE_SEL_MASK);
}


void HAL_SYSCTL_SetPsensorControl(uint32_t PsensorId, uint32_t OSCSelect,
		uint32_t ps_n, uint32_t en)
{
    SYSCTL_PsensorId id;
    SYSCTL_OSCSelect osc_sel;
    switch (PsensorId) {
		case 0:
			id = SYSCTL_PSENSOR_0;
			break;
		case 1:
			id = SYSCTL_PSENSOR_1;
			break;
		default:
			HAL_ASSERT_PARAM(id < 2);
			break;
    }
    switch (OSCSelect) {
		case 0:
			osc_sel = SYSCTL_OSC_DISABLE;
			break;
		case 1:
			osc_sel = SYSCTL_OSC_RVT_CASECODE;
			break;
		case 2:
			osc_sel = SYSCTL_OSC_LVT_CASECODE;
			break;
		case 3:
			osc_sel = SYSCTL_OSC_RVT_NORMAL;
			break;
		default:
			HAL_ASSERT_PARAM(osc_sel < 4);
			break;
    }
    HAL_ASSERT_PARAM(ps_n < 8);

    if (en)
        SYSCTL->PS_CTL_REG = id | osc_sel | SYSCTL_PS_N_PRD_VAL(ps_n) | SYSCTL_PS_EN_MASK;
    else
        SYSCTL->PS_CTL_REG = id | osc_sel | SYSCTL_PS_N_PRD_VAL(ps_n);
}

void HAL_SYSCTL_WaitPsensorRdyAndClean(void)
{
    while (!(SYSCTL->PS_CTL_REG & SYSCTL_CLK250M_CNT_RDY_MASK))
        ;
    HAL_SET_BIT(SYSCTL->PS_CTL_REG, SYSCTL_CLK250M_CNT_RDY_MASK);
}

uint32_t HAL_SYSCTL_GetPsensorCnt(void)
{
    return SYSCTL->PS_CNT_REG;
}


void HAL_SYSCTL_SetDbgData(uint32_t id, uint32_t data)
{
    HAL_ASSERT_PARAM(id < 6);
    SYSCTL->GENRAL_DBG_REG[id] = data;
}

uint32_t HAL_SYSCTL_GetDegData(uint32_t id)
{
    HAL_ASSERT_PARAM(id < 6);
    return SYSCTL->GENRAL_DBG_REG[id];
}


uint32_t HAL_SYSCTL_GetPsensor(uint32_t PsensorId, uint32_t OSCSelect,
		uint32_t ps_n)
{
    uint32_t value;
    HAL_SYSCTL_SetPsensorControl(PsensorId, OSCSelect, ps_n, 1);

    hal_usleep(1);
    HAL_SYSCTL_WaitPsensorRdyAndClean();
    HAL_SET_BIT(SYSCTL->PS_CTL_REG, SYSCTL_CLK250M_CNT_RDY_MASK); // clear ready bit

    value = HAL_SYSCTL_GetPsensorCnt();
    HAL_CLR_BIT(SYSCTL->PS_CTL_REG, SYSCTL_PS_EN_MASK); // disable psensor

    return value;
}
