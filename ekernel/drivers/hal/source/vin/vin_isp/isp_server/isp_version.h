/*
 * linux-4.9/drivers/media/platform/sunxi-vin/vin-isp/isp_server/isp_version.h
 *
 * Copyright (C) 2018 zhao wei.
 * Copyright (C) 2022 zheng zequn
 *
 * Authors: zhengzequn <zequnzheng@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ISP_VERSION_H_
#define _ISP_VERSION_H_

#include "include/isp_debug.h"

#if defined CONFIG_ARCH_SUN20IW3
#define ISP_VERSION 600
#define REPO_TAG "isp-600-600-v1.00"
#endif
#define REPO_BRANCH "libisp-dev"
#define REPO_COMMIT "dc238b7298b76018320b2101c61312dcf9850335"
#define REPO_DATE "Wed Nov 16 15:35:34 2022 +0800"
#define RELEASE_AUTHOR "<mayifei@allwinnertech.com>"

#define FROM_REPO_BRANCH "libisp-dev"
#define FROM_REPO_COMMIT "dc238b7298b76018320b2101c61312dcf9850335"
#define FROM_REPO_DATE "Wed Nov 16 15:35:34 2022 +0800"

static inline void isp_version_info(void)
{
	ISP_PRINT(">>>>>>>>>>>> ISP VERSION INFO >>>>>>>>>>>\n"
		"IPCORE: ISP%d\n"
		"branch: %s\n"
		"commit: %s\n"
		"date  : %s\n"
		"author: %s\n"
		"-------------------------------------------\n\n",
		ISP_VERSION, REPO_BRANCH, REPO_COMMIT, REPO_DATE, RELEASE_AUTHOR);
}

#endif
