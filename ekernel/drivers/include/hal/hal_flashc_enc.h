/* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.

 * Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
 * the the People's Republic of China and other countries.
 * All Allwinner Technology Co.,Ltd. trademarks are used with permission.

 * DISCLAIMER
 * THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
 * IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
 * IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
 * ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
 * ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
 * COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
 * YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.


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

#ifndef __HAL_FLASHC_ENC_H__
#define __HAL_FLASHC_ENC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Flashc_Enc_Cfg {
	uint8_t ch;
	uint8_t used;
	uint8_t enable;
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t key_0;
	uint32_t key_1;
	uint32_t key_2;
	uint32_t key_3;
} Flashc_Enc_Cfg;

/* for debug */
void printf_enc_config(const Flashc_Enc_Cfg *enc_cfg);

/* for flashc module */
int hal_flashc_enc_init(uint32_t max_addr, uint8_t start_ch);

/* user api */
int hal_flashc_set_enc(const Flashc_Enc_Cfg *enc_set);
int hal_flashc_enc_enable(const Flashc_Enc_Cfg *enc_set);
int hal_flashc_enc_disable(const Flashc_Enc_Cfg *enc_set);
int hal_flashc_enc_alloc_ch(void);
int hal_flashc_enc_free_ch(const Flashc_Enc_Cfg *enc_set);

/* other api */
int hal_flashc_enc_enable_ch(uint8_t ch);
int hal_flashc_enc_disable_ch(uint8_t ch);
int hal_flashc_enc_set_key(const uint32_t *aes_key, uint8_t ch);
int hal_flashc_enc_set_addr(uint32_t start_addr, uint32_t end_addr, uint8_t ch);

Flashc_Enc_Cfg *get_flashc_enc_cfg(void);
#ifdef __cplusplus
}
#endif

#endif /* __HAL_FLASHC_ENC_H__*/
