
#ifndef __SENSOR_REGISTER__H__
#define __SENSOR_REGISTER__H__

#include "camera.h"


struct sensor_cfg_array {
	char sensor_name[20];
	struct sensor_fuc_core *sensor_core;
};

struct sensor_fuc_core *find_sensor_func(char *sensor_name);

#endif
