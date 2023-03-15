#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <console.h>
#include <errno.h>
#include <tinatest.h>
#include <sys/vfs.h>
#include "../../source/flashc/hal_flash.h"
#include "../../source/flashc/hal_flashctrl.h"
#include <hal_flashc_enc.h>
#include <hal_cache.h>

#define KB ((unsigned long long)1024)
#define MB (KB * 1024)
#define GB (KB * 1024)

#define VERSION "v0.1.0"
#define COMPILE "Compiled in " __DATE__ " at " __TIME__

extern FlashBoardCfg *get_flash_cfg(int devNum);
extern void FC_WrapMode(bool enable);
struct flashc_task {
	char *file;
	char *dir;
	char *path;
	unsigned int avg;
	unsigned long long size;
	unsigned long long r_ms;
	unsigned long long w_ms;
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t clk;
	int line;
	int num;
	int opt;
	struct sys {
		uint32_t free_ddr;
		uint32_t total_ddr;
		uint32_t total_flash;
	} sys;
};

static unsigned long long str_to_bytes(const char *str)
{
	unsigned long long size;
	char c;

	c = str[strlen(str) - 1];
	size = atoi(str);

	if (size == 0)
		return 0;
	switch (c) {
		case '0'...'9': return size;
		case 'k':
		case 'K': return size * KB;
		case 'm':
		case 'M': return size * MB;
		case 'g':
		case 'G': return size * GB;
		default:
		return 0;
	}
}

static int randbuf(char *buf, int len)
{
	int x;

	srand((unsigned int)time(NULL));
	while (len) {
		int l = min(len, (int)sizeof(int));

		x = rand();
		memcpy(buf, &x, l);

		len -= l;
		buf += l;
	}
	return 0;
}

static inline uint32_t get_total_flash(const char *dir)
{
	struct statfs sfs;

	if (!dir)
		return 0;

	if (statfs(dir, &sfs) < 0) {
		printf("statfs %s failed\n", dir);
		return 0;
	}

	return sfs.f_bsize * sfs.f_blocks;
}

static inline uint32_t get_free_flash(const char *dir)
{
	struct statfs sfs;

	if (!dir)
		return 0;

	if (statfs(dir, &sfs) < 0)
		return 0;

	/* get free by f_bavail rather than f_bfree */
	return sfs.f_bsize * sfs.f_bavail;
}

static void show_help(void)
{
	printf("Usage: flashc [-h] <-d select test items> [-s start addr] [-e end addr]\n");
	printf("\n");
	printf("\t-h : show this massage and exit\n");
	printf("\t-d # : select test items\n");
	printf("\t   0 : spi model 0/1/2/3 test\n");
	printf("\t   1 : spi std/dual/quad test\n");
	printf("\t   2 : erase/write/read speed test\n");
	printf("\t   4 : xip continuous/wrap read test\n");
	printf("\t   5 : spi flash test\n");
	printf("\t-s # : set end addr\n");
	printf("\t-e # : set num\n");
	printf("\n");
}

static int nor_erase_64KB(unsigned int addr, unsigned int size)
{
	HAL_Status status = HAL_ERROR;
	status = HAL_Flash_Open(0, 5000);
	if (status != HAL_OK) {
		printf("open %d fail\n", 0);
		return status;
	}

	status = HAL_Flash_Erase(0 , FLASH_ERASE_64KB, addr, size / 65536);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	HAL_Flash_Close(0);
	if (status != HAL_OK) {
		printf("close %d fail\n", 0);
		return status;
	}

	return status;
}

static int nor_erase(unsigned int addr, unsigned int size)
{
	HAL_Status status = HAL_ERROR;
	status = HAL_Flash_Open(0, 5000);
	if (status != HAL_OK) {
		printf("open %d fail\n", 0);
		return status;
	}

	status = HAL_Flash_Erase(0 , FLASH_ERASE_4KB, addr, size / 4096);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	HAL_Flash_Close(0);
	if (status != HAL_OK) {
		printf("close %d fail\n", 0);
		return status;
	}
	return status;
}

static int nor_write(unsigned int addr, char *buf, unsigned int size)
{
	HAL_Status status = HAL_ERROR;
	status = HAL_Flash_Open(0, 5000);
	if (status != HAL_OK) {
		printf("open %d fail\n", 0);
		return status;
	}

	status = HAL_Flash_Write(0 , addr, buf, size);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	HAL_Flash_Close(0);
	if (status != HAL_OK) {
		printf("close %d fail\n", 0);
		return status;
	}
	return status;
}

static int nor_read(unsigned int addr, char *buf, unsigned int size)
{
	HAL_Status status = HAL_ERROR;
	status = HAL_Flash_Open(0, 5000);
	if (status != HAL_OK) {
		printf("open %d fail\n", 0);
		return status;
	}

	status = HAL_Flash_Read(0 , addr, buf, size);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	HAL_Flash_Close(0);
	if (status != HAL_OK) {
		printf("close %d fail\n", 0);
		return status;
	}
	return status;
}

static int flash_api_test(void)
{
	u8 read_buf[128], write_buf[128], i;
	uint32_t addr = 0, size = sizeof(write_buf);
	int ret;
	HAL_Status status = HAL_ERROR;

	for(i = 0; i < size ;i++)
	write_buf[i] = i;

	status = HAL_Flash_Open(0, 5000);
	if (status != HAL_OK) {
	printf("open %d fail\n", 0);
		return status;
	}

	status = HAL_Flash_Erase(0 , FLASH_ERASE_4KB, addr, 1);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	status = HAL_Flash_Write(0 , addr, write_buf, size);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	memset(read_buf, 0, size);
	status = HAL_Flash_Read(0 , addr, read_buf, size);
	if (status != HAL_OK) {
		printf("erase %d fail\n", 0);
		return status;
	}

	HAL_Flash_Close(0);
	if (status != HAL_OK) {
		printf("close %d fail\n", 0);
		return status;
	}

	ret = memcmp(write_buf, read_buf, size);
	if (ret != 0) {
		printf("\tresult: err\n");
	} else {
		printf("\tresult: ok\n");
	}

	return ret;
}

static int speed_test(struct flashc_task *task)
{
	#define TEST_ADDR 0x900000
	//#define TEST_SIZE 4096
	#define TEST_SIZE 0x100000
	//#define BUF_SIZE  65536
	#define BUF_SIZE  4096
	#define TEST_DBUG_3 1
	int i, j, k = 0,ret;
	HAL_Status status = HAL_ERROR;

	unsigned long long  r_start_ms = 0, r_end_ms = 0, r_ms = 0;
	unsigned long long  w_start_ms = 0, w_end_ms = 0, w_ms = 0;
	unsigned long long  e_start_ms = 0, e_end_ms = 0, e_ms = 0;
	unsigned long long  e_64_start_ms = 0, e_64_end_ms = 0, e_64_ms = 0;
	unsigned long long  e_w_start_ms = 0, e_w_end_ms = 0, e_w_ms = 0;
	struct timeval w_start = {0}, w_end = {0};
	struct timeval r_start = {0}, r_end = {0};
	struct timeval e_start = {0}, e_end = {0};
	struct timeval e_64_start = {0}, e_64_end = {0};
	struct timeval e_w_start = {0}, e_w_end = {0};
	uint32_t test_addr = task->start_addr;
	uint32_t size, test_size = task->end_addr - task->start_addr;
	__attribute__((aligned(64))) u8 write_buf[BUF_SIZE];
	__attribute__((aligned(64))) u8 read_buf[BUF_SIZE];
	FlashBoardCfg *cfg = get_flash_cfg(0);

	size = test_size;
	printf("\terase/write/read speed test(non-file system)\n");
	printf("\terase/write/read size:%.2fkb\n", (double)size/1024);
	printf("\tmode: 0x%02X freq: %dMHz\n", cfg->mode, cfg->clk/1000/1000);
	printf("\n");

	memset(write_buf, 0, BUF_SIZE);
	randbuf(write_buf, BUF_SIZE);
	
	ret = gettimeofday(&e_64_start, NULL);
	nor_erase_64KB(test_addr, test_size);
	ret = gettimeofday(&e_64_end, NULL);

	//erase test
	printf("\terase ...\n");
	ret = gettimeofday(&e_w_start, NULL);
	ret = gettimeofday(&e_start, NULL);
	status = nor_erase(test_addr, test_size);
	if(status != HAL_OK) {
		printf("erase err!!!\n");
		goto out;
	}
	ret = gettimeofday(&e_end, NULL);
	printf("\terase ok\n");

	//write test
	printf("\twrite ...\n");
	ret = gettimeofday(&w_start, NULL);
	size = test_size;
	j = 0;
	while (size > 0) {
		status = nor_write(test_addr + j*BUF_SIZE, write_buf, BUF_SIZE);
		if(status != HAL_OK) {
			printf("write err!!!\n");
			goto out;
		}

		j++;
		size -= BUF_SIZE;
	}
	ret = gettimeofday(&w_end, NULL);
	ret = gettimeofday(&e_w_end, NULL);
	printf("\twrite ok\n");

	//read test
	printf("\tread ...\n");
	ret = gettimeofday(&r_start, NULL);
	j = 0;
	size = test_size;
	while (size > 0) {
		status = nor_read(test_addr + j*BUF_SIZE, read_buf, BUF_SIZE);
		if(status != HAL_OK) {
			printf("read err!!!\n");
			goto out;
		}

		j++;
		size -= BUF_SIZE;
	}
	ret = gettimeofday(&r_end, NULL);
	printf("\tread ok\n");

	//check...
	printf("\tcheck ...\n");
	j = 0;
	size = test_size;
	while (size > 0) {
		memset(read_buf, 0, BUF_SIZE);
		status = nor_read(test_addr + j*BUF_SIZE, read_buf, BUF_SIZE);
		if(status != HAL_OK) {
			printf("read err!!!\n");
			goto out;
		}
		ret = memcmp(write_buf, read_buf, BUF_SIZE);
		if (ret != 0) {
			printf("check err!!!\n");
			#if TEST_DBUG_3
			printf("j = %d\n", j);
			if (k == 0) {
				k = 1;
				for(i = 0; i < sizeof(read_buf) ;i++) {
					printf("0x%02x ",read_buf[i]);
					if(i%20 == 0)
						printf("\n");
				}
				printf("\n ");
				printf("\n ");
				for(i = 0; i < sizeof(write_buf) ;i++) {
					printf("0x%02x ",write_buf[i]);
					if(i%20 == 0)
						printf("\n");
				}
				printf("\n ");
				goto out;
			}
			#endif
			goto out;
		}
		j++;
		size -= BUF_SIZE;
	}
	printf("\tcheck ok\n");

	e_start_ms = e_start.tv_sec * 1000 + e_start.tv_usec / 1000;
	e_end_ms = e_end.tv_sec * 1000 + e_end.tv_usec / 1000;
	e_64_start_ms = e_64_start.tv_sec * 1000 + e_64_start.tv_usec / 1000;
	e_64_end_ms = e_64_end.tv_sec * 1000 + e_64_end.tv_usec / 1000;

	w_start_ms = w_start.tv_sec * 1000 + w_start.tv_usec / 1000;
	w_end_ms = w_end.tv_sec * 1000 + w_end.tv_usec / 1000;
	r_start_ms = r_start.tv_sec * 1000 + r_start.tv_usec / 1000;
	r_end_ms = r_end.tv_sec * 1000 + r_end.tv_usec / 1000;
	r_end_ms = r_end.tv_sec * 1000 + r_end.tv_usec / 1000;
	e_w_start_ms = e_w_start.tv_sec * 1000 + e_w_start.tv_usec / 1000;
	e_w_end_ms = e_w_end.tv_sec * 1000 + e_w_end.tv_usec / 1000;

	e_ms = e_end_ms - e_start_ms;
	e_64_ms = e_64_end_ms - e_64_start_ms;
	w_ms = w_end_ms - w_start_ms;
	r_ms = r_end_ms - r_start_ms;
	r_ms = r_end_ms - r_start_ms;
	e_w_ms = e_w_end_ms - e_w_start_ms;

	size = test_size;
	printf("\n");
	printf("\terase(4k) test OK (%.2f KB/s)\n", ((double)size / 1024) / ((double)e_ms / 1000));
	printf("\terase(64k) test OK (%.2f KB/s)\n", ((double)size / 1024) / ((double)e_64_ms / 1000));
	printf("\twrite test OK     (%.2f KB/s)\n", ((double)size / 1024) / ((double)w_ms / 1000));
	printf("\terase && write    (%.2f KB/s)\n", ((double)size / 1024) / ((double)e_w_ms / 1000));
	printf("\tread test OK      (%.2f KB/s)\n", ((double)size / 1024) / ((double)r_ms / 1000));
	ret = 0;
	printf("\tresult: ok\n");
	return ret;
out:
	printf("\tresult: err\n");
	return ret;
}

static int spi_flash_test(struct flashc_task *task)
{
	#define BUF_SIZE_5  4096
	#define TEST_DBUG_5 1
	int i, j, k, ret;
	HAL_Status status = HAL_ERROR;
	__attribute__((aligned(64))) u8 write_buf[BUF_SIZE_5];
	__attribute__((aligned(64))) u8 read_buf[BUF_SIZE_5];
	uint32_t test_addr = task->start_addr;
	uint32_t size, test_size = task->end_addr - task->start_addr;

	size = test_size;
	printf("\tspi flash test\n");
	printf("\tstart addr:0x%x\n", task->start_addr);
	printf("\tend addr:0x%x\n", task->end_addr);
	printf("\n");

	memset(write_buf, 0, BUF_SIZE_5);
	randbuf(write_buf, BUF_SIZE_5);

	//erase test
	printf("\terase ...\n");
	status = nor_erase(test_addr, test_size);
	if(status != HAL_OK) {
		printf("erase err!!!\n");
		goto out;
	}
	printf("\terase ok\n");

	//write test
	printf("\twrite ...\n");
	size = test_size;
	j = 0;
	while (size > 0) {
		status = nor_write(test_addr + j*BUF_SIZE_5, write_buf, BUF_SIZE_5);
		if(status != HAL_OK) {
			printf("write err!!!\n");
			goto out;
		}

		j++;
		size -= BUF_SIZE_5;
	}
	printf("\twrite ok\n");

	//read test
	printf("\tread ...\n");
	j = 0;
	size = test_size;
	while (size > 0) {
		status = nor_read(test_addr + j*BUF_SIZE_5, read_buf, BUF_SIZE_5);
		if(status != HAL_OK) {
			printf("read err!!!\n");
			goto out;
		}

		j++;
		size -= BUF_SIZE_5;
	}
	printf("\tread ok\n");

	//check...
	printf("\tcheck ...\n");
	j = 0;
	size = test_size;
	while (size > 0) {
		memset(read_buf, 0, BUF_SIZE_5);
		status = nor_read(test_addr + j*BUF_SIZE_5, read_buf, BUF_SIZE_5);
		if(status != HAL_OK) {
			printf("read err!!!\n");
			goto out;
		}
		ret = memcmp(write_buf, read_buf, BUF_SIZE_5);
		if (ret != 0) {
			printf("check err!!!\n");
			#if TEST_DBUG_5
			printf("j = %d\n", j);
			if (k == 0) {
				k = 1;
				for(i = 0; i < sizeof(read_buf) ;i++) {
					printf("0x%02x ",read_buf[i]);
					if(i%20 == 0)
						printf("\n");
				}
				printf("\n ");
				printf("\n ");
				for(i = 0; i < sizeof(write_buf) ;i++) {
					printf("0x%02x ",write_buf[i]);
					if(i%20 == 0)
						printf("\n");
				}
				printf("\n ");
				goto out;
			}
			#endif
			goto out;
		}
		j++;
		size -= BUF_SIZE_5;
	}
	printf("\tcheck ok\n");

	ret = 0;
	printf("\tresult: ok\n");
	return ret;
out:
	printf("\tresult: err\n");
	return ret;
}

static int spi_flash_read_write(void)
{
	#define TEST_SIZE_1 512
	int ret = -1;
	uint8_t *read_buf, *write_buf;
	HAL_Status status = HAL_ERROR;

	write_buf = malloc(TEST_SIZE_1);
	if (!write_buf) {
		printf("malloc failed - %s\n", strerror(errno));
		goto out;
	}

	read_buf = malloc(TEST_SIZE_1);
	if (!read_buf) {
		printf("malloc failed - %s\n", strerror(errno));
		goto out;
	}

	status = nor_erase(0x600000, 4096);
	if(status != HAL_OK) {
		printf("erase err!!!\n");
		goto out;
	}

	randbuf(write_buf, TEST_SIZE_1);
	status = nor_write(0x600000, write_buf, TEST_SIZE_1);
	if(status != HAL_OK) {
		printf("write err!!!\n");
		goto out;
	}
	memset(read_buf, 0, TEST_SIZE_1);
	status = nor_read(0x600000, read_buf, TEST_SIZE_1);
	if(status != HAL_OK) {
		printf("read err!!!\n");
		goto out;
	}
	ret = memcmp(write_buf, read_buf, TEST_SIZE_1);
	if (ret != 0) {
		printf("check err!!!\n");
		goto out;
	}

	ret = 0;
out:
	free(read_buf);
	free(write_buf);
	return ret;
}

int xip_read_data(uint32_t addr, uint8_t *data, uint32_t len, uint8_t index)
{
	uint32_t _fc_icache_startAddr;
	uint32_t icache_bias;
	uint32_t icache_src;

	_fc_icache_startAddr = 0x10000000 + addr;
	icache_bias = FlashCBUS_AddrRequire(index, _fc_icache_startAddr,addr, len);
	icache_src = _fc_icache_startAddr + addr - icache_bias;

	hal_dcache_invalidate(icache_src, len);
	memcpy(data, (uint8_t *)((uintptr_t)(icache_src)), len);

	return 0;
}

static int write_test_data(uint32_t addr, uint8_t *data, uint32_t len)
{
	int ret = -1;
	HAL_Status status = HAL_ERROR;
	status = nor_erase(addr, len >= 4096 ? len : 4096);
	if(status != HAL_OK) {
		printf("erase err!!!\n");
		goto out;
	}

	status = nor_write(addr, data, len);
	if(status != HAL_OK) {
		printf("write err!!!\n");
		goto out;
	}

	ret = 0;
out:
	return ret;
}

static int four_xip_ranges_test(void)
{
	#define TEST_SIZE_2 1024
	#define TEST_DBUG_2 0
	int i, j, ret = -1;
	uint8_t *read_buf, *write_buf;
	uint32_t addr = 0x300000;

	HAL_Status status = HAL_ERROR;

	printf("\tconfigure four xip ranges test\n");
	printf("\n");

	write_buf = malloc(TEST_SIZE_2);
	if (!write_buf) {
		printf("malloc failed - %s\n", strerror(errno));
		goto out;
	}

	read_buf = malloc(TEST_SIZE_2);
	if (!read_buf) {
		printf("malloc failed - %s\n", strerror(errno));
		goto out;
	}

	for (i= 1; i < 4; i++) {
		randbuf(write_buf, TEST_SIZE_2);
		printf("\twrite data, addr: 0x%x, len: 0x%x\n", addr, TEST_SIZE_2);
		write_test_data(addr, write_buf, TEST_SIZE_2);
		#if TEST_DBUG_2
		for(j = 0; j < TEST_SIZE_2 ;j++) {
			printf("0x%02x ",write_buf[j]);
			if(j%20 == 0)
				printf("\n");
		}
		printf("\n ");
		#endif
		printf("\txip read data, addr: 0x%x, len: 0x%x\n", addr, TEST_SIZE_2);
		xip_read_data(addr, read_buf, TEST_SIZE_2, i);
		#if TEST_DBUG_2
		for(j = 0; j < TEST_SIZE_2 ;j++) {
			printf("0x%02x ",read_buf[j]);
			if(j%20 == 0)
				printf("\n");
		}
		printf("\n ");
		#endif
		ret = memcmp(write_buf, read_buf, TEST_SIZE_2);
		if (ret == 0) {
			printf("\tresult: ok\n");
		} else {
			printf("\tresult: err\n");
		}

		addr += 0x100000;
	}

	ret = 0;
out:
	return ret;
}

static int set_spi_line(uint8_t line)
{
	struct FlashDev *dev = getFlashDev(0);
	if (line == 1) {
		dev->rmode = FLASH_READ_FAST_MODE;
	} else if (line == 2) {
		dev->rmode = FLASH_READ_DUAL_IO_MODE;
	} else if (line == 4) {
		dev->rmode = FLASH_READ_QUAD_IO_MODE;
	}
}

static int spi_line_test(void)
{
	int ret = 0;
	uint8_t i, line;
	line = 0;

	printf("\tspi line std/dual/quad test\n");
	printf("\n");
	for (i = 0; i < 3; i++) {
		if (i == 0) {
			line = 1;
			printf("\tset:  spi std\n");
		} else if (i == 1) {
			line = 2;
			printf("\tset:  spi dual\n");
		} else if (i == 2){
			line = 4;
			printf("\tset:  spi quad\n");
		}
		set_spi_line(line);

		printf("\twrite/read the spi flash...\n");
		ret = spi_flash_read_write();
		printf("\tcmd(bit24~31):0x%08x\n", *((u32 *)(0x4000B000+0x2C)));
		if (ret ==0)
			printf("\tresult: ok\n");
		else
			printf("\tresult: err\n");
		printf("\n");
	}

out:
	return ret;
}

static int set_spi_mode(uint8_t mode)
{
	struct FlashDev *dev = getFlashDev(0);

	if (mode == 0) {
		mode = FC_SCLK_Mode0;
	} else if (mode == 1) {
		mode = FC_SCLK_Mode0;
		//mode = FC_SCLK_Mode1;
	} else if (mode == 2) {
		//mode = FC_SCLK_Mode2;
		mode = FC_SCLK_Mode3;
	} else if (mode == 3) {
		mode = FC_SCLK_Mode3;
	} 
	else {
		return -1;
	}

	HAL_Flashc_ResetCCMU();
	HAL_Flashc_ConfigCCMU(96*1000*1000);
	HAL_Flashc_EnableCCMU(dev->chip->flash_ctrl);
	FC_SetFlash(FC_TCTRL_CS_LOW_ENABLE, FC_TCTRL_FBS_MSB, mode);
	FC_Sbus_ResetFIFO(1, 1);
	HAL_Flashc_SetCacheLineLen(FC_RD_CACHE_LINE_LEN_32B, FC_WT_CACHE_LINE_LEN_32B);
	HAL_Flashc_DisableCCMU(dev->chip->flash_ctrl);
	printf("\tmode(bit0~1):0x%08x\n", *((u32 *)(0x4000B000+0x04)));
	return 0;
}

static int spi_mode_test(void)
{
	int ret = 0;
	uint8_t i, mode;
	mode = 0;

	printf("\tspi mode 0/1/2/3 test\n");
	printf("\n");
	for (i = 0; i < 4; i++, mode++) {
		printf("\ttest: spi mode %d\n", mode);
		printf("\tset:  spi mode %d\n", mode);
		set_spi_mode(mode);

		printf("\twrite/read the spi flash...\n");
		ret = spi_flash_read_write();
		if (ret ==0)
			printf("\tresult: ok\n");
		else
			printf("\tresult: err\n");
		printf("\n");
	}

out:
	return ret;
}

static int spi_flash_test_1(void)
{
	#define BUF_SIZE_5  4096
	#define TEST_DBUG_5 1
	int i, j, k = 0, ret;
	HAL_Status status = HAL_ERROR;
	__attribute__((aligned(64))) u8 write_buf[BUF_SIZE_5];
	__attribute__((aligned(64))) u8 read_buf[BUF_SIZE_5];
	uint32_t test_addr = 0x20000;
	uint32_t size, test_size = BUF_SIZE_5;

	size = test_size;

	//read test
	printf("\tread ...\n");
	j = 0;
	size = test_size;
	while (size > 0) {
		status = nor_read(test_addr + j*BUF_SIZE_5, read_buf, BUF_SIZE_5);
		if(status != HAL_OK) {
			printf("read err!!!\n");
			goto out;
		}
			#if TEST_DBUG_5
			printf("j = %d\n", j);
			if (k == 0) {
				k = 1;
				for(i = 0; i < sizeof(read_buf) ;i++) {
					printf("0x%02x ",read_buf[i]);
					if(i%20 == 0)
						printf("\n");
				}
				printf("\n ");
			}
			#endif
		j++;
		size -= BUF_SIZE_5;
	}
	printf("\tread ok\n");

	ret = 0;
	printf("\tresult: ok\n");
	return ret;
out:
	printf("\tresult: err\n");
	return ret;
}




static int begin(struct flashc_task *task)
{
	int avg = 1;
	int ret;

	while (avg-- > 0) {
		if (task->opt ==0) {
			spi_mode_test();
		} else if (task->opt ==1) {
			spi_line_test();
		} else if (task->opt ==2) {
			speed_test(task);
		} else if (task->opt ==3) {
			four_xip_ranges_test();
		} else if (task->opt ==5) {
			spi_flash_test(task);
		} else if (task->opt ==6) {
			spi_flash_test_1();
		}
	}

	return 0;
}

int flashc_main(int argc, char **argv)
{
	int opts = 0;
	int ret = 0;
	struct flashc_task task;

	printf("%s@%p\n", __func__, flashc_main);
	memset(&task, 0, sizeof(task));

	optind = 0;
	while ((opts = getopt(argc, argv, ":hd:t:s:e:")) != EOF) {
		switch (opts) {
		case 'h': show_help(); goto out;
		case 'd': {
			task.opt = atoi(optarg);
			printf("task->opt: %d\n", task.opt);
			break;
			}
		case 's': {
			task.start_addr = str_to_bytes(optarg);
			printf("task->start_addr %x\n", task.start_addr);
			break;
			}
		case 'e': {
			task.end_addr = str_to_bytes(optarg);
			printf("task->end_addr %x\n", task.end_addr);
			break;
			}
		case 't': {
			task.line = atoi(optarg);
			printf("task->num %d\n", task.num);
			break;
			}
		case '?':
			printf("invalid option %c\n", optopt);
			goto out;
		case ':':
			printf("option -%c requires an argument\n", optopt);
			show_help();
			goto out;
		}
	}

	if (!task.num)
        task.num = 1;

	if (!task.start_addr) {
		task.start_addr = 0x700000;
	} 

	if (!task.end_addr) {
		task.end_addr = 0x800000;
	}

	task.sys.total_flash = get_total_flash(task.dir);

	ret = begin(&task);
	free(task.path);
out:
	return ret;
}
FINSH_FUNCTION_EXPORT_CMD(flashc_main, flashc, flash controller test case);
