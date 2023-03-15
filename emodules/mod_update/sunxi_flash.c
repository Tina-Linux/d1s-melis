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
#include "sunxi_flash.h"
#include "sunxi_update.h"
#include "unistd.h"
#define CONFIG_SPINOR_LOGICAL_OFFSET	48*1024

__s32 sunxi_boot_storage_type(void) {
    int ret = -1, value = 0;	

	ret = esCFG_GetKeyValue("card_product", "card_product_used", (int32_t *)&value, 1);
	if (ret == 0 && value) {
		ret = esCFG_GetKeyValue("card_product", "card_product_storage", (int32_t *)&value, 1);
		return value;
	}

	ret = esCFG_GetKeyValue("target", "storage_type", (int32_t *)&value, 1);
	return value;
}

__s32 sunxi_gey_storage_logic_offset(void) {
	int ret = 0, storage_type = 0;
	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		return CONFIG_SPINOR_LOGICAL_OFFSET;
	}
	return EPDK_FAIL;
}

__s32 sunxi_flash_size(void) {
	int fd = -1;
	int ret = 0,storage_type = 0;
	unsigned char device_name[64] = {0};
	unsigned int total_bytes = 0;

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

	fd = open(device_name, O_RDONLY);
	if (fd < 0) {
		eLIBs_printf("can not find device:%s\n", device_name);
		return EPDK_FAIL;
	}	
	ret = ioctl(fd, BLOCK_DEVICE_CMD_GET_TOTAL_SIZE, &total_bytes);
	if (ret < 0) {
		eLIBs_printf("device:%s BLOCK_DEVICE_CMD_GET_TOTAL_SIZE fail!\n", device_name);
		close(fd);
		return EPDK_FAIL;
	}
#ifdef UPDATE_DEBUG
	eLIBs_printf("device_name:%s total_bytes:%d !\n", device_name, total_bytes);
#endif
    close(fd);
	return total_bytes/512;
}
__s32 sunxi_flash_read(blk_dev_get_data_t *get_data_request) {
	int fd = -1;
    int ret = 0, storage_type = 0;
	unsigned char device_name[64] = {0};

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

    fd = open(device_name, O_RDONLY);
	if (fd < 0) {
        eLIBs_printf("can not find device:%s\n", device_name);
        return EPDK_FAIL;
    }
//    ret = ioctl(fd, BLOCK_DEVICE_CMD_USR_PHY_READ, get_data_request);
//	if (ret < 0) {
//        eLIBs_printf("%s BLOCK_DEVICE_CMD_USR_PHY_READ Fail!\n", device_name);
//		close(fd);
//        return EPDK_FAIL;
//	}
	lseek(fd, get_data_request->addr, SEEK_SET);
	read(fd, get_data_request->buff, sizeof(char) * get_data_request->len);
	close(fd);
	return ret;
}
__s32 sunxi_flash_logic_read(blk_dev_get_data_t *get_data_request) {
	int fd = -1;
    int ret = 0, storage_type = 0;
	unsigned char device_name[64] = {0};

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
		get_data_request->addr += CONFIG_SPINOR_LOGICAL_OFFSET;
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

    fd = open(device_name, O_RDONLY);
	if (fd < 0) {
        eLIBs_printf("can not find device:%s\n", device_name);
        return EPDK_FAIL;
    }

	lseek(fd, get_data_request->addr, SEEK_SET);
	read(fd, get_data_request->buff, sizeof(char) * get_data_request->len);
	close(fd);
	return ret;
}

__s32 sunxi_flash_write(blk_dev_get_data_t *get_data_request) {
	int fd = -1;
    int ret = 0, storage_type = 0;
	unsigned char device_name[64] = {0};

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

    fd = open(device_name, O_WRONLY);
	if (fd < 0) {
        eLIBs_printf("can not find device:%s\n", device_name);
        return EPDK_FAIL;
    }

	lseek(fd, get_data_request->addr, SEEK_SET);
	write(fd, get_data_request->buff, sizeof(char) * get_data_request->len);
	close(fd);
	return ret;
}
__s32 sunxi_flash_logic_write(blk_dev_get_data_t *get_data_request) {
	int fd = -1;
    int ret = 0, storage_type = 0;
	unsigned char device_name[64] = {0};

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
		get_data_request->addr += CONFIG_SPINOR_LOGICAL_OFFSET;
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

    fd = open(device_name, O_WRONLY);
	if (fd < 0) {
        eLIBs_printf("can not find device:%s\n", device_name);
        return EPDK_FAIL;
    }
//    ret = ioctl(fd, BLOCK_DEVICE_CMD_USR_PHY_WRITE, get_data_request);
//	if (ret < 0) {
//        eLIBs_printf("%s BLOCK_DEVICE_CMD_USR_PHY_WRITE Fail!\n", device_name);
//		close(fd);
//        return EPDK_FAIL;
//	}
	lseek(fd, get_data_request->addr, SEEK_SET);
	write(fd, get_data_request->buff, sizeof(char) * get_data_request->len);
	close(fd);
	return ret;
}

__s32 sunxi_flash_erase(blk_dev_erase_t *blk_dev_erase) {
	int fd = -1;
    int ret = 0, storage_type = 0;
	unsigned char device_name[64] = {0};

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

    fd = open(device_name, O_RDONLY);
	if (fd < 0) {
        eLIBs_printf("can not find device:%s\n", device_name);
        return EPDK_FAIL;
    }
    ret = ioctl(fd, BLOCK_DEVICE_CMD_ERASE_SECTOR, blk_dev_erase);
	if (ret < 0) {
        eLIBs_printf("%s BLOCK_DEVICE_CMD_ERASE_SECTOR Fail!\n", device_name);
		close(fd );
        return EPDK_FAIL;
	}
	close(fd);
	return ret;
}
__s32 sunxi_flash_logic_erase(blk_dev_erase_t *blk_dev_erase) {
	int fd = -1;
    int ret = 0, storage_type = 0;
	unsigned char device_name[64] = {0};

	storage_type = sunxi_boot_storage_type();
	if (storage_type == STORAGE_NOR) {
		eLIBs_strcpy(device_name, "/dev/spinor");
		blk_dev_erase->addr += CONFIG_SPINOR_LOGICAL_OFFSET;
	} else {
		eLIBs_printf("storage_type:%d not support!\n", storage_type);
		return EPDK_FAIL;
	}

    fd = open(device_name, O_RDONLY);
	if (fd < 0) {
        eLIBs_printf("can not find device:%s\n", device_name);
        return EPDK_FAIL;
    }
    ret = ioctl(fd, BLOCK_DEVICE_CMD_ERASE_SECTOR, blk_dev_erase);
	if (ret < 0) {
        eLIBs_printf("%s BLOCK_DEVICE_CMD_ERASE_SECTOR Fail!\n", device_name);
		close(fd );
        return EPDK_FAIL;
	}
	close(fd);
	return ret;
}

