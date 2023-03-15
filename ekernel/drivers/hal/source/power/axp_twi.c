/*
 * Copyright (C) 2020 allwinnertech Ltd.
 */

#include <sunxi_hal_power.h>
#include <sunxi_hal_power_private.h>
#include <sunxi_hal_twi.h>

int hal_power_twi_init(struct power_dev *rdev)
{
	unsigned char port = TWI_PORT(rdev->flag);
	unsigned short addr = AXP_ADDR(rdev->flag);

	hal_twi_init(port);
	hal_twi_control(port, I2C_SLAVE, &addr);

	return 0;
}

int hal_power_byte_read(struct power_dev *rdev, u8 reg, u8 *reg_val)
{
	unsigned char port = TWI_PORT(rdev->flag);
	int ret;

	ret = hal_twi_read(port, reg, reg_val, 1);
	if (ret > 0)
		return 0;

	return ret;
}

int hal_power_byte_bulk_read(struct power_dev *rdev, u8 reg, void *reg_val, int val_count)
{
	unsigned char port = TWI_PORT(rdev->flag);
	int ret;

	ret = hal_twi_read(port, reg, reg_val, val_count);
	if (ret > 0)
		return 0;

	return ret;
}

int hal_power_byte_write(struct power_dev *rdev, u8 reg, u8 reg_val)
{
	/* hal_twi_write bug workaround */
	unsigned char port = TWI_PORT(rdev->flag);
	twi_msg_t msg;
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = reg_val;
	msg.flags = 0;
	msg.addr = AXP_ADDR(rdev->flag);
	msg.len = 2;
	msg.buf = buf;

	return hal_twi_control(port, I2C_RDWR, &msg);
}

int hal_power_byte_update(struct power_dev *rdev, u8 reg, u8 val, u8 mask)
{
	u8 reg_val = 0;
	int ret = 0;

	ret = hal_power_byte_read(rdev, reg, &reg_val);
	if (ret)
		goto out;

	if ((reg_val & mask) != val) {
		reg_val = (reg_val & ~mask) | (val & mask);
		ret = hal_power_byte_write(rdev, reg, reg_val);
	}

out:
	return ret;
}
