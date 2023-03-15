/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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