#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <hwcrypto.h>
#include <hw_symmetric.h>
#include <sunxi_drv_crypto.h>
#include <debug.h>
#include <log.h>

#ifndef MIN
#define MIN(a, b) (a > b ? b : a)
#endif

#define HEXDUMP_LINE_CHR_CNT 16
int sunxi_hexdump(const unsigned char *buf, int bytes)
{
    unsigned char line[HEXDUMP_LINE_CHR_CNT] = {0};
    int addr;

    for (addr = 0; addr < bytes; addr += HEXDUMP_LINE_CHR_CNT)
    {
        int len = MIN(bytes - addr, HEXDUMP_LINE_CHR_CNT), i;

        memcpy(line, buf + addr, len);
        memset(line + len, 0, HEXDUMP_LINE_CHR_CNT - len);

        /* print addr */
        printf("0x%.8X: ", addr);
        /* print hex */
        for (i = 0; i < HEXDUMP_LINE_CHR_CNT; i++)
        {
            if (i < len)
            {
                printf("%.2X ", line[i]);
            }
            else
            {
                printf("   ");
            }
        }
        /* print char */
        printf("|");
        for (i = 0; i < HEXDUMP_LINE_CHR_CNT; i++)
        {
            if (i < len)
            {
                if (line[i] >= 0x20 && line[i] <= 0x7E)
                {
                    printf("%c", line[i]);
                }
                else
                {
                    printf(".");
                }
            }
            else
            {
                printf(" ");
            }
        }
        printf("|\n");
    }
    return 0;
}

static int cmd_ce_test(int argc, const char **argv)
{
	int ret = -1;
	int i = 0, j = 0;
	struct rt_hwcrypto_ctx *ctx = NULL;
	rt_uint8_t out[16] __attribute__((aligned(32)))= {0x0};
	rt_uint8_t tmp[16] __attribute__((aligned(32)))= {0x0};
	hwcrypto_type  type ;
	rt_uint32_t   keybits;

	rt_uint8_t in[16] __attribute__((aligned(32)))= {
		0xEE, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};
	rt_uint8_t key[32] __attribute__((aligned(32)))= {
		0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
		0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
		0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
		0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
	};
	rt_uint8_t iv[16] __attribute__((aligned(32)))= {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};
	/*void hal_dcache_clean(unsigned long vaddr_start, unsigned long size);*/

	/*hal_dcache_clean(in, sizeof(in));*/
	/*hal_dcache_clean(key, sizeof(key));*/
	/*hal_dcache_clean(iv, sizeof(iv));*/

	rt_uint8_t result[2][3][16] __attribute__((aligned(32))) = {
		/* HWCRYPTO_TYPE_AES_ECB */
		{
			/*128 bits*/
			{0xD1,0x40,0x54,0xF9,0xF5,0x27,0x32,0x04,0x2F,0x15,0x65,0x26,0x03,0x36,0x14,0x7C},
			/*192 bits*/
			{0x5E,0x64,0x7C,0xFD,0x6E,0xC6,0x92,0xED,0xE9,0x71,0x81,0x7D,0x84,0x78,0x89,0x46},
			/*256 bits*/
			{0x72,0x6C,0x64,0xC1,0x5E,0xC8,0xBF,0x42,0x37,0xA2,0xCF,0x81,0x0B,0xC2,0x35,0x96},
		},
		/* HWCRYPTO_TYPE_AES_CBC */
		{
			/*128 bits*/
			{0xD4,0x72,0x14,0x04,0xF9,0x85,0x4A,0x69,0x10,0xCE,0x0B,0x6F,0xB6,0xCE,0xA4,0xC0},
			/*192 bits*/
			{0x44,0x5D,0x09,0x91,0xED,0xEE,0xE9,0xB9,0xF2,0x73,0x7D,0xCF,0xE9,0x7E,0xD6,0x48},
			/*256 bits*/
			{0x60,0xBA,0x28,0x9D,0x6B,0x81,0xD2,0x97,0xF1,0xAF,0x1C,0xE2,0xB5,0xF0,0xFD,0x48},
		},
	};

	printf("Enter test_ce \n");
	printf("in buf 0x%08x\n", (rt_uint32_t)in);
	sunxi_hexdump(in, sizeof(in));
	printf("iv buf 0x%08x\n", (rt_uint32_t)iv);
	sunxi_hexdump(iv, sizeof(iv));
	printf("key buf 0x%08x\n", (rt_uint32_t)key);
	sunxi_hexdump(key, sizeof(key));
	printf("out buf 0x%08x\n", (rt_uint32_t)out);
	printf("\n");

#if 0
	ret = aw_hw_crypto_device_init();
	if (ret < 0) {
		printf("Error: aw_hw_crypto_device_init fail \n");
 		software_break();
		return -1;
	}
#endif

	/* for HWCRYPTO_TYPE_AES_ECB  HWCRYPTO_TYPE_AES_CBC */
	for (i = 0; i < 2; i++)
	{
		/* for keybit in 128 192 256*/
		for (j = 0; j < 3; j++)
		{
			/*void hal_dcache_clean_all(void);*/
			/*hal_dcache_clean_all();*/
			switch (i) {
				case 0:
					type = HWCRYPTO_TYPE_AES_ECB;
					printf("Info: select AES_ECB.\n");
					break;
				case 1:
					type = HWCRYPTO_TYPE_AES_CBC;
					printf("Info: select AES_CBC.\n");
					break;
				default:
					printf("Error: type is Incorrect.\n");
					return -1;
			}

			keybits = 128 + j * 64;
			printf("Info: select keybits: %d.\n", keybits);
			
			printf("------------------------------- \n");

			ctx = rt_hwcrypto_symmetric_create(rt_hwcrypto_dev_default(), type);
			if (ctx == NULL) {
				printf("Error: rt_hwcrypto_symmetric_create fail \n");
				software_break();
				return -1;
			}

			ret = rt_hwcrypto_symmetric_setkey(ctx, key, keybits);
			if (ret < 0) {
				printf("Error: rt_hwcrypto_symmetric_setkey fail \n");
				software_break();
				return -1;
			}

			if( type == HWCRYPTO_TYPE_AES_CBC) {
				ret = rt_hwcrypto_symmetric_setiv(ctx, iv, sizeof(iv));
				if (ret < 0) {
					printf("Erorr: rt_hwcrypto_symmetric_setiv fail \n");
					software_break();
					return -1;
				}
			}
			//void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size);

			//software_break();
			printf ("Info: select encrypt\n");
			ret = rt_hwcrypto_symmetric_crypt(ctx, HWCRYPTO_MODE_ENCRYPT, 16, in, out);
			if (ret < 0) {
				printf("Error: rt_hwcrypto_symmetric_crypt encrypt fail \n");
				software_break();
				return -1;
			}

			printf ("Info: out:\n");
			//hal_dcache_invalidate(out, 16);
			sunxi_hexdump(out, 16);

			if (memcmp(result[i][j], out, 16)) {
				printf ("Info: %s line %d, aes test encrypt fail\n", __func__, __LINE__);
				software_break();
			}
			else{
				printf ("Info: aes test encrypt pass\n");
			}

			if( type == HWCRYPTO_TYPE_AES_CBC)
			{
				memset(out,0,16);
				ret = rt_hwcrypto_symmetric_getiv(ctx, out, 16);
				if (ret < 0) {
					printf("Erorr: rt_hwcrypto_symmetric_getiv fail \n");
					software_break();
					return -1;
				}
				printf ("Info: iv:\n");
				sunxi_hexdump(out, 16);
			}

			printf("\n");

			/* DECRYPT */
			if( type == HWCRYPTO_TYPE_AES_CBC)
			{
				ret = rt_hwcrypto_symmetric_setiv(ctx, iv, sizeof(iv));
				if (ret < 0) {
					printf("Erorr: rt_hwcrypto_symmetric_setiv fail \n");
					software_break();
					return -1;
				}
			}


			printf ("Info: select decrypt.\n");
			ret = rt_hwcrypto_symmetric_crypt(ctx, HWCRYPTO_MODE_DECRYPT, 16, result[i][j], out);
			if (ret < 0) {
				printf("Error: rt_hwcrypto_symmetric_crypt fail \n");
				software_break();
				return -1;
			}
			//hal_dcache_invalidate(out, 16);
			printf ("Info: out:\n");	
			sunxi_hexdump(out, 16);

			if (memcmp(in, out, 16)) {
				printf ("Info: aes test decrypt fail\n");
				software_break();
			}
			else{
				printf ("Info: aes test decrypt pass\n");
			}

			if( type == HWCRYPTO_TYPE_AES_CBC) {
				memset(out,0,16);
				ret = rt_hwcrypto_symmetric_getiv(ctx, out, 16);
				if (ret < 0) {
					printf("Erorr: rt_hwcrypto_symmetric_getiv fail \n");
					software_break();
					return -1;
				}
				printf ("Info: iv:\n");
				sunxi_hexdump(out, 16);
			}
			rt_hwcrypto_symmetric_destroy(ctx);
			ctx = NULL;
			printf("\n");
		}
	}

	return 0;
}

int cmd_ce_burnin_test(int argc, char **argv)
{
    	const char *argvv = "burnin";
	static int counter = 0;
	void rt_memory_info(rt_uint32_t *total,
    			rt_uint32_t *used,
    			rt_uint32_t *max_used);
	rt_uint32_t total, used, max_used;

    	while(1)
	{
	    cmd_ce_test(1, &argvv);
	    rt_thread_delay(1);
	    rt_memory_info(&total, &used, &max_used);
	    __log("pass times %d 0x%08lx, 0x%08lx, 0x%08lx.", counter ++, used, max_used, total);
	}

	return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_ce_test, __cmd_ce_test, ce_test);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_ce_burnin_test, __cmd_ce_burnin_test, ce_burnin_test);
