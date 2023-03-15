/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY'S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS'SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY'S TECHNOLOGY.
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
#include <stdlib.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <sunxi_hal_sysctl.h>

static int cmd_test_sysctl(int argc, char **argv)
{
	uint32_t func, data[4];
	uint32_t value;
    if (argc < 2)
    {
        hal_log_info("Usage: hal_sysctl func [data]\n");
        return -1;
    }

    hal_log_info("Run sysctl hal layer test case\n");

    func = strtol(argv[1], NULL, 0);
    data[0] = strtol(argv[2], NULL, 0);

	switch (func) {
		case 0:
			HAL_SYSCTL_SetSipFlashTestMapMode(data[0]);
			break;
		case 1:
			HAL_SYSCTL_RamUseAsBT(data[0]);
			break;
		case 2:
			HAL_SYSCTL_RamUseAsCSI(data[0]);
			break;
		case 3:
			HAL_SYSCTL_RomSecureSel(data[0]);
			break;
		case 4:
			data[1] = strtol(argv[3], NULL, 0);
			data[2] = strtol(argv[4], NULL, 0);
			data[3] = strtol(argv[5], NULL, 0);
			HAL_SYSCTL_SetPsensorControl(data[0], data[1], data[2],  data[3]);
			break;
		case 5:
			data[1] = strtol(argv[3], NULL, 0);
			data[2] = strtol(argv[4], NULL, 0);
			value = HAL_SYSCTL_GetPsensor(data[0], data[1], data[2]);
			hal_log_info("Get Psensor value = %d\n", value);
			break;
		case 6:
			data[1] = strtol(argv[3], NULL, 0);
			HAL_SYSCTL_SetDbgData(data[0], data[1]);
			value = HAL_SYSCTL_GetDegData(data[0]);
			hal_log_info("HAL_SYSCTL_GetDegData value = %x\n", value);
			break;
		default:
			hal_log_info("default func\n");
			break;
	}

    hal_log_info("hal_sysctl test finish\n");
	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_sysctl, hal_sysctl, sysctl_hal_APIs_tests)
