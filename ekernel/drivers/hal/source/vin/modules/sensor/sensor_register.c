#include "sensor_register.h"

struct sensor_cfg_array sensor_array[] = {
#ifdef CONFIG_SENSOR_GC4663_MIPI
        {"gc4663_mipi", &gc4663_core},
#endif
#ifdef CONFIG_SENSOR_GC2053_MIPI
        {"gc2053_mipi", &gc2053_core},
#endif
#ifdef CONFIG_SENSOR_SC2355_MIPI
        {"sc2355_mipi", &sc2355_core},
#endif
#ifdef CONFIG_SENSOR_SC5336_MIPI
        {"sc5336_mipi", &sc5336_core},
#endif
};

struct sensor_fuc_core *find_sensor_func(char *sensor_name)
{
        int i;

        if (!ARRAY_SIZE(sensor_array)) {
                vin_err("%s:sensor_array is NULL\n", __func__);
                return NULL;
        }

	for (i = 0; i < ARRAY_SIZE(sensor_array); i++) {
                if (!strncmp(sensor_name, sensor_array[i].sensor_name, 6)) {
                        vin_print("%s: find %s sensor core function!\n", __func__, sensor_name);
                        return sensor_array[i].sensor_core;
                }
        }

        return NULL;
}
