#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <hal_cmd.h>
#include "sunxi_hal_cir_tx.h"

#define NS_TO_US(nsec)		((nsec) / 1000)

#define NEC_NBITS		32
#define NEC_UNIT		562500 /* ns. Logic data bit pulse length */
#define NEC_HEADER_PULSE	(16 * NEC_UNIT)	/* 9ms. 16 * Logic data bit pulse length*/
#define NEC_HEADER_SPACE	(8 * NEC_UNIT)  /* 4.5ms */
#define NEC_BIT_PULSE		(1 * NEC_UNIT)
#define NEC_BIT_0_SPACE		(1 * NEC_UNIT)
#define NEC_BIT_1_SPACE		(3 * NEC_UNIT)
#define NEC_TRAILER_PULSE	(1 * NEC_UNIT)
#define NEC_TRAILER_SPACE	(10 * NEC_UNIT) /* even longer */

#define GPIO_IR_RAW_BUF_SIZE	128
#define DEFAULT_DUTY_CYCLE	33
#define DEFAULT_CARRIER_FREQ	38000

#define LIRC_MODE2_PULSE	0x01000000
#define LIRC_MODE2_SPACE	0x00000000

#define LIRC_VALUE_MASK		0x00FFFFFF
#define LIRC_MODE2_MASK		0xFF000000

#define LIRC_PULSE(val)	(((val)&LIRC_VALUE_MASK) | LIRC_MODE2_PULSE)
#define LIRC_SPACE(val) (((val)&LIRC_VALUE_MASK) | LIRC_MODE2_SPACE)

uint32_t tx_raw_buf[GPIO_IR_RAW_BUF_SIZE];

static int nec_modulation_byte(uint32_t *buf, uint8_t code)
{
	int i = 0;
	uint8_t mask = 0x01;

	while (mask) {
		if (code & mask) {
			/* bit 1 */
			*(buf + i) = LIRC_PULSE(NS_TO_US(NEC_BIT_PULSE));
			*(buf + i + 1) = LIRC_SPACE(NS_TO_US(NEC_BIT_1_SPACE));
		} else {
			/* bit 0 */
			*(buf + i) = LIRC_PULSE(NS_TO_US(NEC_BIT_PULSE));
			*(buf + i + 1) = LIRC_SPACE(NS_TO_US(NEC_BIT_0_SPACE));
		}
		mask <<= 1;
		i += 2;
	}
	return i;
}

static int ir_lirc_transmit_ir(uint32_t *raw_buf, size_t n)
{
	int ret, count;

	count = n / sizeof(unsigned int);
	if (count > 1024 || count % 2 == 0) {
		return -1;
	}
}

static int nec_ir_encode(uint32_t *raw_buf, uint32_t key_code)
{
	uint8_t address, reverse_address, command, reverse_command;
	uint32_t *head_p, *data_p, *stop_p;

	address = (key_code >> 24) & 0xff;
	reverse_address = (key_code >> 16) & 0xff;
	command = (key_code >> 8) & 0xff;
	reverse_command = (key_code >> 0) & 0xff;

	/* head bit */
	head_p = raw_buf;
	*(head_p) = LIRC_PULSE(NS_TO_US(NEC_HEADER_PULSE));
	*(head_p + 1) = LIRC_PULSE(NS_TO_US(NEC_HEADER_SPACE));

	/* data bit */
	data_p = raw_buf + 2;
	nec_modulation_byte(data_p, address);

	data_p += 16;
	nec_modulation_byte(data_p, reverse_address);

	data_p += 16;
	nec_modulation_byte(data_p, command);

	data_p += 16;
	nec_modulation_byte(data_p, reverse_command);

	/* stop bit */
	stop_p = data_p + 16;
	*(stop_p) = LIRC_PULSE(NS_TO_US(NEC_TRAILER_PULSE));
	*(stop_p + 1) = LIRC_PULSE(NS_TO_US(NEC_TRAILER_SPACE));

	return ((NEC_NBITS + 2) * 2 - 1);
}

int cmd_test_cir_tx(int argc, char **argv)
//int cmd_test_cir_tx(int argc, char **argv)
{
	int key_code = 0x04fb13ec;
	int i, size;
	int count = 67;
	struct sunxi_cir_tx_t *cir_tx;

	hal_cir_tx_init(cir_tx);

	hal_cir_tx_set_duty_cycle(DEFAULT_DUTY_CYCLE);
	hal_cir_tx_set_carrier(DEFAULT_CARRIER_FREQ);

	size = nec_ir_encode(tx_raw_buf, key_code);
	for (i = 0; i < size; i++) {
		printf("%d ", *(tx_raw_buf + i) & 0x00FFFFFF);
		if ((i + 1) % 8 == 0) {
			printf("\n");
		}
	}
	printf("\n");

	for (i = 0; i < size; i++)
		tx_raw_buf[i] = (tx_raw_buf[i] & 0x00FFFFFF);

	hal_cir_tx_xmit(tx_raw_buf, count);
	printf("end test!\n");

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_cir_tx, hal_cir_tx, irtx)
