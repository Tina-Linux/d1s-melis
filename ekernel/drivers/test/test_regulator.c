/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
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
#include <getopt.h>
#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sunxi_hal_regulator.h>

static struct regulator_dev regulator;

#define REGULATOR_ENABLE 1
#define REGULATOR_DISABLE 2
#define REGULATOR_SETVOL 3
#define REGULATOR_GETVOL 4

static int cmd_regulator(int argc, const char **argv)
{
    int cur_vol = 0, tar_vol = 0;
    char regulator_type, regulator_id;
    int c, state = REGULATOR_GETVOL;

    if (argc < 4)
    {
        printk("Usage:\n");
        printk("\tregulator [regulator type] [regulator id] -e\n");
        printk("\t                                          -d\n");
        printk("\t                                          -s [vol]\n");
        printk("\t                                          -g\n");
        return -1;
    }

    regulator_type = strtol(argv[1], NULL, 0);
    regulator_id = strtol(argv[2], NULL, 0);

    while ((c = getopt(argc, (char *const *)argv, "degs")) != -1)
    {
        switch (c)
        {
        case 'e':
            state = REGULATOR_ENABLE;
            break;
        case 's':
            state = REGULATOR_SETVOL;
            tar_vol = strtol(argv[4], NULL, 0);
            break;
        case 'd':
            state = REGULATOR_DISABLE;
            break;
        case 'g':
            state = REGULATOR_GETVOL;
            break;
        default:
            state = REGULATOR_GETVOL;
            break;
        }
    }

    hal_regulator_get(REGULATOR_GET(regulator_type, regulator_id), &regulator);

    switch (state) {
    case REGULATOR_ENABLE:
        hal_regulator_enable(&regulator);
        break;
    case REGULATOR_DISABLE:
        hal_regulator_disable(&regulator);
        break;
    case REGULATOR_GETVOL:
        hal_regulator_get_voltage(&regulator, &cur_vol);
        printk("cur_vol:%d\n", cur_vol);
        break;
    case REGULATOR_SETVOL:
        hal_regulator_set_voltage(&regulator, tar_vol);
        break;
    default:
        break;
    }

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_regulator, __cmd_regulator, regulator test code);
