#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <console.h>
#include <tinatest.h>
#include <sys/vfs.h>
#include <part_efi.h>
#include "../../source/flashc/hal_flash.h"
#include "../../source/flashc/hal_flashctrl.h"
#include <hal_flashc_enc.h>

#define KB ((unsigned long long)1024)
#define MB (KB * 1024)
#define GB (KB * 1024)

#define VERSION "v0.1.0"
#define COMPILE "Compiled in " __DATE__ " at " __TIME__

/*
static const Flashc_Enc_Cfg flashc_enc_cfg[] = {
#if defined(CONFIG_XIP) && defined(FLASH_XIP_OPT_READ)
#if defined(FLASH_XIP_ENC)
	{
		.used = 1,
		.enable = 0,
		.start_addr = 0x391000+0x80,//xip start offset in flash ,see sys_partition_xip.fex
		.end_addr = 0x391000+0x69880,
		.key_0 = 0x0,
		.key_1 = 0x0,
		.key_2 = 0x0,
		.key_3 = 0x0,
	},
	{
		.used = 1,
		.enable = 0,
		.start_addr = FLASH_XIP_START_ADDR,//xip start addr
		.end_addr = FLASH_XIP_START_ADDR+0x69880,
		.key_0 = 0x0,
		.key_1 = 0x0,
		.key_2 = 0x0,
		.key_3 = 0x0,
	},
#endif
	{
		.used = 0,
		.enable = 0,
		.start_addr = 0x900000,
		.end_addr = 0xA00000,
		.key_0 = 0x0,
		.key_1 = 0x0,
		.key_2 = 0x0,
		.key_3 = 0x0,
	},
	{
		.used = 0,
		.enable = 0,
		.start_addr = 0x10900000,
		.end_addr = 0x10A00000,
		.key_0 = 0x0,
		.key_1 = 0x0,
		.key_2 = 0x0,
		.key_3 = 0x0,
	},
#else
	{
		.used = 0,
		.enable = 0,
		.start_addr = 0x900000,
		.end_addr = 0xA00000,
		.key_0 = 0x12345678,
		.key_1 = 0x12345678,
		.key_2 = 0x12345678,
		.key_3 = 0x12345678,
	},
#endif
};
*/
struct enctask {
    char *file;
    char *dir;
    char *path;
    unsigned int avg;
    unsigned long long size;
    unsigned long long r_ms;
    unsigned long long w_ms;
    unsigned long long start_addr;
    unsigned long long end_addr;
    int num;
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
    size = atoll(str);
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
    printf("    Usage: flashenc [-s start addr] [-h] [-e end addr] [-n select test items]\n");
    printf("\n");
    printf("\t-h : show this massage and exit\n");
    printf("\t-s : # : set start addr\n");
    printf("\t-e # : set end addr\n");
    printf("\t-n # : set select test items\n");
    printf("\n");
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

#define TEST_SIZE 4096
#define BUF_SIZE  1024
static int flash_enc_test(struct enctask *task)
{
    #define TEST_DBUG 0

    int i, j, k = 0, ret = -1;
    unsigned long long addr;
    __attribute__((aligned(64))) u8 write_buf[BUF_SIZE];
    __attribute__((aligned(64))) u8 read_buf[BUF_SIZE];
    Flashc_Enc_Cfg enc_set;
    Flashc_Enc_Cfg *enc_cfg = get_flashc_enc_cfg();
    HAL_Status status = HAL_ERROR;

    memset(enc_cfg, 0, sizeof(Flashc_Enc_Cfg)*6);
    #if TEST_DBUG
	printf_enc_config(enc_cfg);
    #endif

    printf("\tflash_enc model test\n");
    printf("\n");

    addr = 0x600000;

loop:
    enc_set.ch = hal_flashc_enc_alloc_ch();
    if (enc_set.ch < 0) {
	printf("err: alloc channel failed.\n");
	goto out;
    }
    enc_set.start_addr  = addr;
    enc_set.end_addr    = addr + 0x100000 - 0x100;
    enc_set.key_0 = 0x12345678;
    enc_set.key_1 = 0x12345678;
    enc_set.key_2 = 0x12345678;
    enc_set.key_3 = 0x12345678;
    enc_set.enable      = 1;
    hal_flashc_set_enc(&enc_set);
    printf("\tstart_addr:0x%x ,end_addr:0x%x ,ch:%d \n", 
        enc_set.start_addr, enc_set.end_addr, enc_set.ch);

    randbuf(write_buf, BUF_SIZE);
    /*
	for (i = 0; i < BUF_SIZE; i++) {
		write_buf[i] = i;
	}
    */

    printf("\twrite ...\n");
    status = nor_erase(addr, TEST_SIZE);
    if(status != HAL_OK) {
	printf("erase err!!!\n");
	goto out;
    }

    status = nor_write(addr, write_buf, BUF_SIZE);
    if(status != HAL_OK) {
	printf("write err!!!\n");
	goto out;
    }

    printf("\tread ...\n");	
    memset(read_buf, 0, BUF_SIZE);
    status = nor_read(addr, read_buf, BUF_SIZE);
    if(status != HAL_OK) {
	printf("read err!!!\n");
	goto out;
    }

#if TEST_DBUG
    for(j = 0; j < BUF_SIZE ;j++) {
	printf("0x%02x ",write_buf[j]);
	if(j%20 == 0)
		printf("\n");
    }
    printf("\n ");

    for(j = 0; j < BUF_SIZE ;j++) {
	printf("0x%02x ",read_buf[j]);
	if(j%20 == 0)
	    printf("\n");
    }
    printf("\n ");
#endif

    printf("\tcheck...\n");
    ret = memcmp(write_buf, read_buf, BUF_SIZE);
    if (ret != 0) {
	printf("check err!!!\n");
	goto out;
    }

    printf("\tdisable flash enc\n");
    hal_flashc_enc_disable(&enc_set);

    printf("\tread ...\n");	
    memset(read_buf, 0, BUF_SIZE);
    status = nor_read(addr, read_buf, BUF_SIZE);
    if(status != HAL_OK) {
	printf("read err!!!\n");
	goto out;
    }

#if TEST_DBUG
    for(j = 0; j < BUF_SIZE ;j++) {
	printf("0x%02x ",read_buf[j]);
	if(j%20 == 0)
	printf("\n");
    }
    printf("\n ");
#endif

    printf("\tcheck...\n");
    ret = memcmp(write_buf, read_buf, BUF_SIZE);
    if (ret == 0) {
        printf("check err!!!\n");
	goto out;
    }

    //hal_flashc_enc_enable(&enc_set);
    printf("\tresult: ok\n");
    printf("\n");

    addr += 0x100000;
	k++;
#if defined(CONFIG_XIP)
    if (k <3) {
        goto loop;
    }
#else
    if (k <6) {
        goto loop;
    }
#endif
    /*
    printf("\tfree...\n");
    enc_set.ch = 0;
    for(j = 0; j < 6 ;j++) {
        hal_flashc_enc_free_ch(&enc_set);
        enc_set.ch++;
    }
    */
    #if TEST_DBUG
	printf_enc_config(enc_cfg);
    #endif

    ret = 0;
    return ret;
out:
    printf("\tresult: err\n");
    return ret;
}

#ifdef CONFIG_COMPONENT_SECURE_STORAGE
extern Flashc_Enc_Cfg ss_enc_set;

static int nor_get_gpt(char *buf, int len)
{
    if (len < GPT_TABLE_SIZE) {
        printf("buf too small for gpt\n");
        return -1;
    }

    return nor_read(GPT_ADDRESS, buf, GPT_TABLE_SIZE);
}

static int get_secret_part_info(uint32_t *start_addr, uint32_t *size)
{
    int ret;
    char *gpt_buf;
    struct nor_flash *nor;
    struct gpt_part *gpt_part;
    struct part *part;
    int start_sector;
    int sectors;

#ifdef CONFIG_FLASHC_CPU_XFER_ONLY
    gpt_buf = malloc(GPT_TABLE_SIZE);
#else
    gpt_buf = dma_alloc_coherent(GPT_TABLE_SIZE);
#endif
    if (!gpt_buf) {
        ret = -1;
        goto err;
    }
    memset(gpt_buf, 0, GPT_TABLE_SIZE);

    ret = nor_get_gpt(gpt_buf, GPT_TABLE_SIZE);
    if (ret) {
        printf("get gpt from nor flash failed - %d\n", ret);
        ret = -1;
        goto err;
    }
    ret = get_part_info_by_name(gpt_buf, "secret", &start_sector, &sectors);
    if (ret < 0) {
        printf("get secret part failed - %d\n", ret);
	goto err;
    }
err:
#ifdef CONFIG_FLASHC_CPU_XFER_ONLY
    free(gpt_buf);
#else
    dma_free_coherent(gpt_buf);
#endif
    //printf("start_sector: 0x%0x sectors: 0x%0x\n", start_sector, sectors);
    *start_addr = start_sector * 512;
    *size = sectors * 512;
    return 0;
}

static int sstorage_test(struct enctask *task)
{
    #define TEST_DBUG 0

    int i, j, k = 0, ret = -1;
    uint32_t size;
    uint32_t addr;

    __attribute__((aligned(64))) u8 write_buf[BUF_SIZE];
    __attribute__((aligned(64))) u8 read_buf[BUF_SIZE];

    Flashc_Enc_Cfg *enc_cfg = get_flashc_enc_cfg();
    HAL_Status status = HAL_ERROR;

    #if TEST_DBUG
	printf_enc_config(enc_cfg);
    #endif

    printf("\tsstorage_test: secure storage test\n");
	printf("\n");

    get_secret_part_info(&addr, &size);

    printf("\t/secret start addr:0x%x ,size:0x%x\n", addr, size);
    addr += 0x1000;
    addr &= 0xFFFFF000;

    randbuf(write_buf, BUF_SIZE);
    /*
	for (i = 0; i < BUF_SIZE; i++) {
		write_buf[i] = i;
	}
    */

    printf("\terase ...\n");
    status = nor_erase(addr, TEST_SIZE);
    if(status != HAL_OK) {
	printf("erase err!!!\n");
	goto out;
    }
    printf("\twrite ...\n");
    status = nor_write(addr, write_buf, BUF_SIZE);
    if(status != HAL_OK) {
	printf("write err!!!\n");
	goto out;
    }

    printf("\tread ...\n");
    memset(read_buf, 0, BUF_SIZE);
    status = nor_read(addr, read_buf, BUF_SIZE);
    if(status != HAL_OK) {
	    printf("read err!!!\n");
	    goto out;
    }

#if TEST_DBUG
    for(j = 0; j < BUF_SIZE ;j++) {
	printf("0x%02x ",write_buf[j]);
	if(j%20 == 0)
	     printf("\n");
    }
    printf("\n ");

    for(j = 0; j < BUF_SIZE ;j++) {
	printf("0x%02x ",read_buf[j]);
	if(j%20 == 0)
	    printf("\n");
    }
    printf("\n ");
#endif

    printf("\tcheck...\n");
    ret = memcmp(write_buf, read_buf, BUF_SIZE);
    if (ret != 0) {
	printf("check err!!!\n");
	goto out;
    }

    printf("\tdisable flash enc\n");
    hal_flashc_enc_disable(&ss_enc_set);

    printf("\tread ...\n");
    memset(read_buf, 0, BUF_SIZE);
    status = nor_read(addr, read_buf, BUF_SIZE);
    if(status != HAL_OK) {
	printf("read err!!!\n");
	goto out;
    }

#if TEST_DBUG
    for(j = 0; j < BUF_SIZE ;j++) {
	printf("0x%02x ",read_buf[j]);
	if(j%20 == 0)
	    printf("\n");
    }
    printf("\n ");
#endif

    printf("\tcheck...\n");
    ret = memcmp(write_buf, read_buf, BUF_SIZE);
    if (ret == 0) {
        printf("check err!!!\n");
	goto out;
    }

    hal_flashc_enc_enable(&ss_enc_set);
    printf("\tresult: ok\n");
    printf("\n");

    #if TEST_DBUG
	printf_enc_config(enc_cfg);
    #endif

    ret = 0;
    return ret;
out:
    printf("\tresult: err\n");
    return ret;
}

static int begin(struct enctask *task)
{
    int avg = 1;
    int ret;

    while (avg-- > 0) {
        if (task->num == 1)
           sstorage_test(task);
        else
	   flash_enc_test(task);
    }

    return 0;
}
#else
static int begin(struct enctask *task)
{
    int avg = 1;
    int ret;

    while (avg-- > 0) {
	flash_enc_test(task);
    }

    return 0;
}

#endif

int flashenc_main(int argc, char **argv)
{
    int opts = 0;
    int ret = 0;
    struct enctask task;

    printf("%s@%p\n", __func__, flashenc_main);
    memset(&task, 0, sizeof(task));

    optind = 0;
    while ((opts = getopt(argc, argv, ":hs:e:n:f:")) != EOF) {
        switch (opts) {
        case 'h': show_help(); goto out;
        case 's': {
		task.start_addr = str_to_bytes(optarg);
		printf("task->start_addr %llu\n", task.start_addr);
                break;
              }
        case 'e': {
                task.end_addr = str_to_bytes(optarg);
		printf("task->end_addr %llu\n", task.end_addr);
                break;
              }
        case 'n': {
		task.num = atoi(optarg);
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
        task.start_addr = 0x300000;
    }

    if (!task.end_addr) {
	task.end_addr = 0x400000;
    }

    task.sys.total_flash = get_total_flash(task.dir);

    ret = begin(&task);
    free(task.path);
out:
    return ret;
}
FINSH_FUNCTION_EXPORT_CMD(flashenc_main, flashenc, test flash enc module);
