#ifndef __SAMPLE_FACE_DETECT__
#define __SAMPLE_FACE_DETECT__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <time.h>
#include <utils/plat_log.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "mpp_vi.h"
#include "mpp_venc.h"
#include "osd_helper.h"
#include "nna.h"
#include "media/mpi_sys.h"
#include "mm_comm_sys.h"
#include "mm_common.h"
#include "tmessage.h"
#include "tsemaphore.h"

#include "show.h"
#include "NNARegister.h"
#include "ion_alloc.h"
#include "nna_driver.h"
#include "awMemory.h"
#include "aw_g2d.h"
#include "image_utils.h"
#include "timing.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define TEST_FRAME_NUM      	(800)  // 测试帧数
#define SAVE_H264_FILE                 // 保存H264文件
#define VI_CAPTURE_WIDTH		(1920)	
#define VI_CAPTURE_HIGHT		(1080)

#define NNA_CALC_WIDTH			(1920)
#define NNA_CALC_HIGHT			(1080)

#define MAX(x, y)   ((x) > (y) ? (x) : (y))
#define MIN(x, y)   ((x) < (y) ? (x) : (y))

pthread_mutex_t          g_stResult_lock = PTHREAD_MUTEX_INITIALIZER;

typedef enum MPP_STATETYPE
{
    MPP_StateIdle = 0X70000000,
    MPP_StateFilled,
    MPP_StateMax = 0X7FFFFFFF
} MPP_STATETYPE;

typedef struct _Picture_Params
{
	MPP_STATETYPE              ePicState;
	VIDEO_FRAME_INFO_S         stVideoFrame;
	pthread_mutex_t            lockPicture;

}Picture_Params;


typedef void* (*ProcessFuncType)(void *pThreadData);
typedef struct _THREAD_DATA
{
    ProcessFuncType ProcessFunc;
    pthread_t   mThreadID;
    void*       pPrivateData;
    
}THREAD_DATA;

typedef struct _NNA_Params
{
    int            iFrameNum;
    
    PIXEL_FORMAT_E ePixFmt;
    int            iPicWidth;
    int            iPicHeight;
    int            iFrmRate;
    
    AW_HANDLE      pEVEHd;
}NNA_Params;

typedef struct _UserConfig
{
    VirVi_Params   stVirViParams[VirVi_MAX];
    VENC_Params    stVENCParams;
    NNA_Params     stNNAParams;
    THREAD_DATA    stThreadData[PROC_MAX];
    cdx_sem_t      mSemExit;
}UserConfig;


typedef struct _test_info_t
{
    AwG2d* aw_g2d;
    struct AwMemOpsS* aw_mem;

    int nna_fd;
    int scale_num;
    unsigned int nna_buf_phy;
    unsigned char* nna_buf_vir;
    unsigned int nna_buf_size;
    
    unsigned int src_buf_phy;
    unsigned char* src_buf_vir;
    unsigned int src_buf_size;

} test_info_t;
extern int64_t CDX_GetSysTimeUsMonotonic();
#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif

