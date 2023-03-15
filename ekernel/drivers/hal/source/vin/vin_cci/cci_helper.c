/*
 * cci_helper.c
 *
 * Copyright (c) 2018 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zequn Zheng <zequnzhengi@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <hal_timer.h>
#include <sunxi_hal_twi.h>

#include "../vin.h"
#include "../platform/platform_cfg.h"
#include "cci_helper.h"

//u64 v4l2_get_timestamp(void)
//{
//
//}

static int cci_read_a8_d8(int id, unsigned char addr, unsigned char *value)
{
	unsigned char data[2];
	twi_msg_t msg[2];
	int slave_addr;
	twi_port_t port;
	int ret;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	data[0] = addr;
	data[1] = 0xee;
	/*
	 * Send out the register address...
	 */
	msg[0].addr = slave_addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &data[0];
	/*
	 * ...then read back the result.
	 */
	msg[1].addr = slave_addr;
	msg[1].flags = TWI_M_RD;
	msg[1].len = 1;
	msg[1].buf = &data[1];

	ret = hal_twi_xfer(port, msg, 2);
	if (ret >= 0) {
		*value = data[1];
		ret = 0;
	}
	return ret;
}

static int cci_write_a8_d8(int id, unsigned char addr, unsigned char value)
{
	twi_msg_t msg;
	unsigned char data[2];
	int slave_addr;
	twi_port_t port;
	int ret;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	data[0] = addr;
	data[1] = value;

	msg.addr = slave_addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = data;

	ret = hal_twi_xfer(port, &msg, 1);
	if (ret >= 0)
		ret = 0;
	return ret;
}

static int cci_read_a8_d16(int id, unsigned char addr, unsigned short *value)
{
	unsigned char data[3];
	twi_msg_t msg[2];
	int slave_addr;
	twi_port_t port;
	int ret;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	data[0] = addr;
	data[1] = 0xee;
	data[2] = 0xee;
	/*
	* Send out the register address...
	*/
	msg[0].addr = slave_addr;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &data[0];
	/*
	* ...then read back the result.
	*/
	msg[1].addr = slave_addr;
	msg[1].flags = TWI_M_RD;
	msg[1].len = 2;
	msg[1].buf = &data[1];

	ret = hal_twi_xfer(port, msg, 2);
	if (ret >= 0) {
		*value = data[1] * 256 + data[2];
		ret = 0;
	}
	return ret;
}

static int cci_write_a8_d16(int id, unsigned short addr, unsigned short value)
{
	twi_msg_t msg;
	unsigned char data[3];
	int slave_addr;
	twi_port_t port;
	int ret;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	data[0] = addr;
	data[1] = (value & 0xff00) >> 8;
	data[2] = (value & 0x00ff);

	msg.addr = slave_addr;
	msg.flags = 0;
	msg.len = 3;
	msg.buf = data;

	ret = hal_twi_xfer(port, &msg, 1);
	if (ret >= 0)
		ret = 0;
	return ret;
}

static int cci_read_a16_d8(int id, unsigned short addr, unsigned char *value)
{
	unsigned char data[3];
	twi_msg_t msg[2];
	int slave_addr;
	twi_port_t port;
	int ret;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	data[0] = (addr & 0xff00) >> 8;
	data[1] = (addr & 0x00ff);
	data[2] = 0xee;

	/*
	* Send out the register address...
	*/
	msg[0].addr = slave_addr;
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = &data[0];
	/*
	* ...then read back the result.
	*/
	msg[1].addr = slave_addr;
	msg[1].flags = TWI_M_RD;
	msg[1].len = 1;
	msg[1].buf = &data[2];

	ret = hal_twi_xfer(port, msg, 2);
	if (ret >= 0) {
		*value = data[2];
		ret = 0;
	}
	return ret;
}

static int cci_write_a16_d8(int id, unsigned short addr, unsigned char value)
{
	twi_msg_t msg;
	unsigned char data[3];
	int slave_addr;
	twi_port_t port;
	int ret;

	slave_addr = global_sensors[id].sensor_twi_addr >> 1;
	port = global_sensors[id].sensor_twi_id;

	data[0] = (addr & 0xff00) >> 8;
	data[1] = (addr & 0x00ff);
	data[2] = value;

	msg.addr = slave_addr;
	msg.flags = 0;
	msg.len = 3;
	msg.buf = data;

	ret = hal_twi_xfer(port, &msg, 1);
	if (ret >= 0)
		ret = 0;
	return ret;
}

static int cci_write(int id, addr_type addr, data_type value)
{
	int ret;

	if (global_sensors[id].addr_width == CCI_BITS_16 && global_sensors[id].data_width == CCI_BITS_8)
		ret = cci_write_a16_d8(id, addr, value);
	else if (global_sensors[id].addr_width == CCI_BITS_8 && global_sensors[id].data_width == CCI_BITS_8)
		ret = cci_write_a8_d8(id, addr, value);
	else {
		vin_err("%s:Now not support addr_width/%d and data_width/%d\n",
				__func__, global_sensors[id].addr_width, global_sensors[id].data_width);
		ret = -1;
	}

	if (ret) {
		vin_log(VIN_LOG_CCI, "at %s error, addr_width = %d , data_width = %d!\n",
				__func__, global_sensors[id].addr_width, global_sensors[id].data_width);
		return -1;
	}

	return ret;
}

static int cci_read(int id, addr_type addr, data_type *value)
{
	int ret;
	*value = 0;

	if (global_sensors[id].addr_width == CCI_BITS_16 && global_sensors[id].data_width == CCI_BITS_8)
		ret = cci_read_a16_d8(id, addr, (unsigned char *)value);
	else if (global_sensors[id].addr_width == CCI_BITS_8 && global_sensors[id].data_width == CCI_BITS_8)
		ret = cci_read_a8_d8(id, addr, (unsigned char *)value);
	else {
		vin_err("%s:Now not support addr_width/%d and data_width/%d\n",
				__func__, global_sensors[id].addr_width, global_sensors[id].data_width);
		ret = -1;
	}

	if (ret) {
		vin_log(VIN_LOG_CCI, "%s error! addr_width = %d , data_width = %d!\n ",\
			__func__, global_sensors[id].addr_width, global_sensors[id].data_width);
		return -1;
	}

	return ret;
}


int sensor_read(int id, addr_type reg, data_type *value)
{
	int ret = 0, cnt = 0;

	ret = cci_read(id, reg, value);
	while ((ret != 0) && (cnt < 2)) {
		ret = cci_read(id, reg, value);
		cnt++;
	}
	if (cnt > 0)
		vin_warn("[%s]sensor read retry = %d\n", global_sensors[id].sensor_name, cnt);

	return ret;
}

int sensor_write(int id, addr_type reg, data_type value)
{
	int ret = 0, cnt = 0;

	ret = cci_write(id, reg, value);
	while ((ret != 0) && (cnt < 2)) {
		ret = cci_write(id, reg, value);
		cnt++;
	}
	if (cnt > 0)
		vin_warn("[%s]sensor write retry = %d\n", global_sensors[id].sensor_name, cnt);

	return ret;
}

int sensor_write_array(int id, struct regval_list *regs, int array_size)
{
	int ret = 0, i = 0;
	//data_type rval = 0;


	if (!regs)
		return -1;

	while (i < array_size) {
		if (regs->addr == REG_DLY) {
			hal_msleep(regs->data);
		} else {
			ret = sensor_write(id, regs->addr, regs->data);
			if (ret < 0) {
				vin_err("%s sensor write array error!\n", global_sensors[id].sensor_name);
				return -1;
			}
			//sensor_read(regs->addr, &rval);
			//vin_print("addr0x%x write data is 0x%x, read data is 0x%x\n", regs->addr, regs->data, rval);
		}
		i++;
		regs++;
	}
	return 0;
}


