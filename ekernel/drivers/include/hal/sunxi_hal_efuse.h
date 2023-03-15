/*
**********************************************************************************************************************
*
*						           the Embedded Secure Bootloader System
*
*
*						       Copyright(C), 2006-2014, Allwinnertech Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      :
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/

#ifndef __SUNXI_SID_H__
#define __SUNXI_SID_H__

#define CHIP_VER_A		0x0
#define CHIP_VER_B		0x1
#define CHIP_VER_C		0x2
#define CHIP_VER_D		0x3

typedef enum efuse_err
{
	EFUSE_ERR_ARG = -1,
	EFUSE_ERR_KEY_NAME_WRONG = -2,
	EFUSE_ERR_KEY_SIZE_TOO_BIG = -3,
	EFUSE_ERR_PRIVATE = -4,
	EFUSE_ERR_ALREADY_BURNED = -5,
	EFUSE_ERR_READ_FORBID = -6,
	EFUSE_ERR_BURN_TIMING = -7,
	EFUSE_ERR_NO_ACCESS = -8,
	EFUSE_ERR_INVALID_ROTPK = -9,
}efuse_err_e;

/* internal struct */
typedef struct efuse_key_map_new{
	#define SUNXI_KEY_NAME_LEN	64
	char name[SUNXI_KEY_NAME_LEN];	/* key_name */
	int offset;	/* key_addr offset */
	int size;	 /* unit: bit */
	int rd_fbd_offset;	/* key can read or not */
	int burned_flg_offset;	/* key has burned or not */
	int sw_rule;
}efuse_key_map_new_t;


int hal_efuse_write(char *key_name, unsigned char *key_data, size_t key_bit_len);
int hal_efuse_read(char *key_name, unsigned char *key_data, size_t key_bit_len);
int hal_efuse_read_ext(uint32_t start_bit, uint32_t bit_num, uint8_t *data);
int hal_efuse_set_security_mode(void);
int hal_efuse_get_security_mode(void);
int hal_efuse_get_chipid(unsigned char *buffer);
int hal_efuse_get_thermal_cdata(unsigned char *buffer);
int hal_efuse_get_chip_ver(void);
int hal_get_module_param_from_sid(uint32_t *dst, uint32_t offset, uint32_t len);

efuse_key_map_new_t *efuse_search_key_by_name(const char *key_name);
#endif    /*  #ifndef __EFUSE_H__  */
