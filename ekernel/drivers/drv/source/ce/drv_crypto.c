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
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <rtdef.h>
#include <hwcrypto.h>
#include <hw_symmetric.h>
#include <sunxi_drv_crypto.h>

#include <melis/init.h>

#include <sunxi_hal_ce.h>

struct aw_hwcrypto_device
{
    struct rt_hwcrypto_device dev;
    struct rt_mutex mutex;
};

static rt_err_t _aes_crypt(struct hwcrypto_symmetric *symmetric_ctx, struct hwcrypto_symmetric_info *symmetric_info)
{
	struct aw_hwcrypto_device *_hwcrypto = (struct aw_hwcrypto_device *)symmetric_ctx->parent.device;
	unsigned char *in = NULL, *out = NULL;
	int ret = 0;
	crypto_aes_req_ctx_t *aes_req_ctx = NULL;
	/*hardware crypto can't update iv, so we to update it.*/
	rt_uint8_t tmp_iv[16] = {0};

	if ((symmetric_ctx->key_bitlen != AES_KEYSIZE_128)
		&& (symmetric_ctx->key_bitlen != AES_KEYSIZE_192)
		&& (symmetric_ctx->key_bitlen != AES_KEYSIZE_256)) {
		pr_err("key length is %d, invalid\n", symmetric_ctx->key_bitlen);
		return -RT_EINVAL;
	}

	if ((symmetric_info->length % AES_BLOCK_SIZE) != 0) {
		pr_err("data len is %d, not block align\n", symmetric_info->length);
		return -RT_EINVAL;
	}

	if ((symmetric_info->in == NULL)
		|| (symmetric_info->out == NULL)
		|| (symmetric_ctx->iv == NULL)
		|| (symmetric_ctx->key == NULL)) {
		pr_err("input buf is NULL\n");
		return -RT_EINVAL;
	}
	
	if (((rt_uint32_t)symmetric_info->in % ADDR_ALIGN_SIZE) != 0) {
		pr_err("malloc input buf\n");
        in = rt_malloc(symmetric_info->length);
        if (in) {
            memcpy(in, symmetric_info->in, symmetric_info->length);
        } else {
			pr_err("malloc input buf fail\n");
            goto error;
        }
    }
	
	if (((rt_uint32_t)symmetric_info->out % ADDR_ALIGN_SIZE) != 0) {
		pr_err("malloc output buf\n");
		out = rt_malloc(symmetric_info->length);
        if (out == NULL) {
			pr_err("malloc outbuf fail\n");
            goto error;
        }
	}

	/*init aes_req_ctx_t*/
	aes_req_ctx = rt_malloc(sizeof(crypto_aes_req_ctx_t));
	if (aes_req_ctx == NULL) {
		pr_err("malloc aes_req_ctx fail\n");
		goto error;
	}
	memset(aes_req_ctx, 0x0, sizeof(crypto_aes_req_ctx_t));
	if (in != NULL) {
		aes_req_ctx->src_buffer = in;
	} else {
		aes_req_ctx->src_buffer = (unsigned char *)symmetric_info->in;
		/*ce_print_hex((char *)aes_req_ctx->src_buffer, symmetric_info->length, (char *)aes_req_ctx->src_buffer);*/
	}
	if (out != NULL) {
		aes_req_ctx->dst_buffer = out;
	} else {
		aes_req_ctx->dst_buffer = (unsigned char *)symmetric_info->out;
	}
	
	aes_req_ctx->src_length = symmetric_info->length;
	
	if (symmetric_info->mode == HWCRYPTO_MODE_ENCRYPT) {
		aes_req_ctx->dir = CRYPTO_DIR_ENCRYPT;
	} else {
		aes_req_ctx->dir = CRYPTO_DIR_DECRYPT;
	}

    switch (symmetric_ctx->parent.type & (HWCRYPTO_MAIN_TYPE_MASK | HWCRYPTO_SUB_TYPE_MASK))
    {
    case HWCRYPTO_TYPE_AES_ECB:
        aes_req_ctx->mode = AES_MODE_ECB;
        break;
    case HWCRYPTO_TYPE_AES_CBC:
        aes_req_ctx->mode = AES_MODE_CBC;
        break;
    default :
		pr_err("don't support this crypto type\n");
        goto error;
    }

	if(aes_req_ctx->dir == CRYPTO_DIR_DECRYPT && aes_req_ctx->mode == AES_MODE_CBC)
	{
		/*if AES-CBC decrypt, we try to save input_data to 'tmp_iv', then update iv when
		 * calculate finished.*/
		memcpy(tmp_iv, aes_req_ctx->src_buffer, 16);
	}

	aes_req_ctx->iv = (unsigned char *)symmetric_ctx->iv;
	/*ce_print_hex((char *)aes_req_ctx->iv, symmetric_info->length, (char *)aes_req_ctx->iv);*/
	aes_req_ctx->key = (unsigned char *)symmetric_ctx->key;
	aes_req_ctx->key_length = symmetric_ctx->key_bitlen >> 3;

#if defined(HWCRYPTO_LOCK)
    rt_mutex_take(&_hwcrypto->mutex, RT_WAITING_FOREVER);
#endif
	ret = do_aes_crypto(aes_req_ctx);
	if (ret < 0) {
		pr_err("hal_do_aes_crypto fail %d\n", ret);
        goto error;
	}
 
#if defined(HWCRYPTO_LOCK)
    rt_mutex_release(&_hwcrypto->mutex);
#endif

	/* update iv*/
	if (aes_req_ctx->mode == AES_MODE_CBC) {
		if (aes_req_ctx->dir == CRYPTO_DIR_ENCRYPT) {
			/*if AES-CBC encrypt, we try to save output_data to 'tmp_iv'.*/
			memcpy(aes_req_ctx->iv, aes_req_ctx->dst_buffer, 16);
		}
		else {
			 /* if AES-CBC decrypt, we update iv by tmp_iv. */
			memcpy(aes_req_ctx->iv, tmp_iv, 16);
		}
	}

    if (out) {
        memcpy(symmetric_info->out, aes_req_ctx->dst_buffer, symmetric_info->length);
        rt_free(out);
    }
	
	if (in) {
		rt_free(in);
	}

	if (aes_req_ctx) {
		rt_free(aes_req_ctx);
	}
    return RT_EOK;

error:
	if (in) {
		rt_free(in);
	}
	if (out) {
		rt_free(out);
	}
	if (aes_req_ctx) {
		rt_free(aes_req_ctx);
	}
	return -RT_ERROR;
}



static const struct hwcrypto_symmetric_ops aes_ops =
{
    .crypt = _aes_crypt,
};

static rt_err_t _crypto_create(struct rt_hwcrypto_ctx *ctx)
{
    rt_err_t res = RT_EOK;
    void *contex;

    switch (ctx->type & HWCRYPTO_MAIN_TYPE_MASK)
    {
    case HWCRYPTO_TYPE_AES :
        ctx->contex = RT_NULL;
        ((struct hwcrypto_symmetric *)ctx)->ops = &aes_ops;
        break;
    default:
        res = -RT_ERROR;
        break;
    }

    return res;
}

static void _crypto_destroy(struct rt_hwcrypto_ctx *ctx)
{
    rt_free(ctx->contex);
}

static rt_err_t _crypto_clone(struct rt_hwcrypto_ctx *des, const struct rt_hwcrypto_ctx *src)
{
    rt_err_t res = RT_EOK;

    switch (src->type & HWCRYPTO_MAIN_TYPE_MASK)
    {
    case HWCRYPTO_TYPE_AES:
    case HWCRYPTO_TYPE_RC4:
    case HWCRYPTO_TYPE_RNG:
    case HWCRYPTO_TYPE_CRC:
    case HWCRYPTO_TYPE_BIGNUM:
        break;
    case HWCRYPTO_TYPE_MD5:
    case HWCRYPTO_TYPE_SHA1:
        break;
    default:
        res = -RT_ERROR;
        break;
    }

    return res;
}

static void _crypto_reset(struct rt_hwcrypto_ctx *ctx)
{
    switch (ctx->type & HWCRYPTO_MAIN_TYPE_MASK)
    {
    case HWCRYPTO_TYPE_AES:
    case HWCRYPTO_TYPE_RC4:
    case HWCRYPTO_TYPE_RNG:
    case HWCRYPTO_TYPE_CRC:
        break;
    default:
        break;
    }
}

static const struct rt_hwcrypto_ops _ops =
{
    .create = _crypto_create,
    .destroy = _crypto_destroy,
    .copy = _crypto_clone,
    .reset = _crypto_reset,
};

int aw_hw_crypto_device_init(void)
{
	static unsigned char is_initialized  = 0;
    static struct aw_hwcrypto_device _crypto_dev;

	/** Prevent duplicate initialization **/
	if (is_initialized)
		return 0;

	pr_debug("rt_hwcrypto init.\n");
	is_initialized = 1;

    _crypto_dev.dev.ops = &_ops;
    _crypto_dev.dev.id = 0;
    _crypto_dev.dev.user_data = &_crypto_dev;

    if (rt_hwcrypto_register(&_crypto_dev.dev,
                             RT_HWCRYPTO_DEFAULT_NAME) != RT_EOK)
    {
		pr_err("rt_hwcrypto_register fail\n");
        return -1;
    }
	if (sunxi_ce_init() < 0) {
		pr_err("hal_ce_init fail\n");
		return -1;
	}
    /*rt_mutex_init(&_crypto_dev.mutex, RT_HWCRYPTO_DEFAULT_NAME, RT_IPC_FLAG_FIFO);*/
    return 0;
}

//INIT_DEVICE_EXPORT(aw_hw_crypto_device_init);
device_initcall(aw_hw_crypto_device_init);
