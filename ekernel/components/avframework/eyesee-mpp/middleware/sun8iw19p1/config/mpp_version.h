/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpp_version.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/07/25
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/

#ifndef _MPP_VERSION_H_
#define _MPP_VERSION_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MPP_REPO_MODULE_NAME "MPP-Platform"
#define MPP_REPO_TAG "V1.0 Release"
#define MPP_REPO_BRANCH "v5-dev"
#define MPP_REPO_COMMIT "4eed413532ad87725895a8cb1ba75e22fc328233"
#define MPP_REPO_DATE "20170725"
#define MPP_RELEASE_AUTHOR "jenkins"

static inline void MPPLogVersionInfo(void)
{

    alogd("\n"
        ">>>>>>>>>>>>>>>>>>>>>>>>>>>>> Media Process Platform<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
        "modName : %s\n"
        "tag   : %s\n"
        "branch: %s\n"
        "commit: %s\n"
        "date  : %s\n"
        "author: %s\n",
        MPP_REPO_MODULE_NAME, MPP_REPO_TAG, MPP_REPO_BRANCH, MPP_REPO_COMMIT, MPP_REPO_DATE, MPP_RELEASE_AUTHOR);
}

#ifdef __cplusplus
}
#endif

#endif  /* _MPP_VERSION_H_ */

