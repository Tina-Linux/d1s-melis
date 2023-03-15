/*
* Copyright (c) 2008-2018 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : isp_version.h
* Description :
* History :
* Author  : zhaowei <zhaowei@allwinnertech.com>
* Date    : 2018/02/08
*
*/

#ifndef _ISP_VERSION_H_
#define _ISP_VERSION_H_

#include <isp_debug.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISP_VERSION 521
#define REPO_TAG "isp-500-520-v2.00"
#define REPO_BRANCH "libisp-dev"
#define REPO_COMMIT "6da89c6ce2127012750856c2a41d27793dd7c33b"
#define REPO_DATE "Tue Jun 15 11:35:29 2021 +0800"
#define RELEASE_AUTHOR "<zhenghanjie@allwinnertech.com>"

#define FROM_REPO_BRANCH "isp521-ipc"
#define FROM_REPO_COMMIT "feb055b740a1b50de2ef77e258e8425909f78753"
#define FROM_REPO_DATE "Mon Apr 26 17:06:44 2021 +0800"

static inline void isp_version_info(void)
{
	ISP_PRINT(">>>>>>>>>>>>>>>>>>>> ISP VERSION INFO <<<<<<<<<<<<<<<<<<<\n"
		"IPCORE: ISP%d\n"
		"branch: %s\n"
		"commit: %s\n"
		"date  : %s\n"
		"author: %s\n"
		"--------------------------------------------------------\n\n",
		ISP_VERSION, REPO_BRANCH, REPO_COMMIT, REPO_DATE, RELEASE_AUTHOR);
}

#ifdef __cplusplus
}
#endif

#endif

