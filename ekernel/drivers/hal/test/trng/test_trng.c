#include <stdio.h>
#include <string.h>
#include <hal_log.h>
#include <hal_cmd.h>
#include <sunxi_hal_trng.h>

static int cmd_test_trng(int argc, const char **argv)
{
	int ret = 0;
	uint32_t random[4] = {0};

	if (argc != 2) {
		printf("Parameter number Error!\n");
		printf("Usage: hal_trng <crc|xor>\n");
		return -1;
	}

	if (strcmp(argv[1], "crc") == 0)
		ret = HAL_TRNG_Extract(0, random);
	else if (strcmp(argv[1], "xor") == 0)
		ret = HAL_TRNG_Extract(1, random);
	else {
		printf("Parameter Error!\n");
		printf("Usage: hal_trng <crc|xor>\n");
		ret = -1;
		return ret;
	}

	if (ret) {
		printf("trng extract failed: %d\n", ret);
		return ret;
	}

	printf("trng result: 0x%08x 0x%08x 0x%08x 0x%08x\n", random[0], random[1], random[2], random[3]);

	return ret;
}

FINSH_FUNCTION_EXPORT_CMD(cmd_test_trng, hal_trng, trng hal APIs tests)
