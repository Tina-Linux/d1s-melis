/*
 * =====================================================================================
 *
 *       Filename:  sunxi_drv_efuse.h
 *
 *    Description:  efuse
 *
 *        Version:  1.0
 *        Created:  2019-07-19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangwei
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef __EFUSE_MAP_H__
#define __EFUSE_MAP_H__




/*reference this struct when call api:sunxi_efuse_write*/
typedef struct
{
	#define SUNXI_KEY_NAME_LEN	64
	char name[SUNXI_KEY_NAME_LEN];
	unsigned int len; /*byte*/
	unsigned int res;
	unsigned char  *key_data;
}
efuse_key_info_t;



/*  breif:: sunxi_efuse_write : unify api to write Efuse key map.
* OTP bit one time burn feature: this API only care the unburned bits of key_buf->data
* So it is strongly suggested you read the value first,then set the unburned bits you want.
* In param:key_name
* In param:rd_buf:array to store the keydata.You must take note of no overflow.
* In param:key_bit_len: key bit len, must be 8bit align.
* Return val: 0==success ;negetive value is err number:ref efuse_err_e)
*/
int hal_efuse_write(char *key_name, unsigned char *key_data, size_t key_bit_len);

/* breif:: sunxi_efuse_read: unify api to read Efuse key map.
*In param:key_name
*In param:rd_buf:array to store the keydata.You must take note of no overflow.
*In param:key_bit_len: key bit len, must be 8bit align.
*return value : (>0) means the real lenth of read data;
*other value means err number:Please reference efuse_err_e to get detail.
*/
int hal_efuse_read(char *key_name, unsigned char *key_data, size_t key_bit_len);

int hal_set_security_mode(void);

int hal_get_security_mode(void);
#endif /*__EFUSE_MAP_H__*/
