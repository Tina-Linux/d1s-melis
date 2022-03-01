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
