#ifndef __MPP_VI__
#define __MPP_VI__
#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#include <utils/plat_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

#include "mm_common.h"
#include "media/mm_comm_vi.h"
#include "media/mpi_sys.h"
#include "media/mpi_vi.h"
#include "mpi_isp.h"

typedef enum _VIPP_INDEX
{
    VIPP_0 = 0,
    VIPP_1,
    VIPP_2,
    VIPP_3,
    VIPP_NUM,
}VIPP_INDEX;

typedef enum _CHN_INDEX
{
    VirViChn_0 = 0,
    VirViChn_1,
    VirViChn_2,
    VirViChn_3,
}CHN_INDEX;

typedef enum _STREAM_INDEX
{
	VirVi_VENC = 0,
	VirVi_NNA,
	VirVi_MAX,
}STREAM_INDEX;

typedef enum _PROC_INDEX
{
    PROC_VENC = 0,
    PROC_NNA,
	PROC_MAX,
}PROC_INDEX;

typedef struct _VIPP_Config
{
	int            bInit;
	int            width;
	int            height;
	PIXEL_FORMAT_E eformat;
	int            frame_rate;
    int            bMirror;
    int            bFlip;
}VIPP_Config;

typedef struct _VirVi_Params
{
	VI_DEV      iViDev;
    VI_CHN      iViChn;
}VirVi_Params;

int create_vi(VirVi_Params* pVirViParams);
int destroy_vi(VirVi_Params* pVirViParams);

extern VIPP_Config g_VIPP_map[VIPP_NUM];
#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
