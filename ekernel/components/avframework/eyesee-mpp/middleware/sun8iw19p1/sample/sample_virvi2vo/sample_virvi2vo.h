
#ifndef _SAMPLE_VIRVI2VO_H_
#define _SAMPLE_VIRVI2VO_H_

#include <plat_type.h>
#include <tsemaphore.h>
#include <mm_comm_vo.h>
#include <mpi_sys.h>
#include <mpi_clock.h>
#include <mpi_vo.h>
#include <mpi_isp.h>

#define MAX_FILE_PATH_SIZE (256)

typedef struct SampleVIRVI2VOCmdLineParam
{
    char mConfigFilePath[MAX_FILE_PATH_SIZE];
}SampleVIRVI2VOCmdLineParam;

typedef struct VIPP2VOConfig
{
    int mDevNum;
    int mCaptureWidth;
    int mCaptureHeight;
    int mDisplayX;
    int mDisplayY;
    int mDisplayWidth;
    int mDisplayHeight;
    int mLayerNum;
}VIPP2VOConfig;
typedef struct VIPP2VOLinkInfo
{
    VI_DEV mVIDev;
    VI_CHN mVIChn;

    VO_LAYER mVoLayer;
    VO_VIDEO_LAYER_ATTR_S mLayerAttr;
    VO_CHN mVOChn;
}VIPP2VOLinkInfo;

#define VIPP2VO_NUM (3)
typedef struct SampleVIRVI2VOConfig
{
    VIPP2VOConfig mVIPP2VOConfigArray[VIPP2VO_NUM];
    BOOL mbAddUILayer;
    VO_LAYER mTestUILayer;
    int mUIDisplayWidth;
    int mUIDisplayHeight;
    
    PIXEL_FORMAT_E mPicFormat; //MM_PIXEL_FORMAT_YUV_PLANAR_420
    int mFrameRate;
    int mTestDuration;  //unit:s, 0 mean infinite
    VO_INTF_TYPE_E mDispType;
    VO_INTF_SYNC_E mDispSync;

    ISP_CFG_MODE mIspWdrSetting;
}SampleVIRVI2VOConfig;

typedef struct SampleVIRVI2VOContext
{
    SampleVIRVI2VOCmdLineParam mCmdLinePara;
    SampleVIRVI2VOConfig mConfigPara;

    pthread_t mDetectInputThreadId;

    cdx_sem_t mSemExit;
    VO_LAYER mUILayer;
    VO_VIDEO_LAYER_ATTR_S mUILayerAttr;
    VO_LAYER mTestUILayer;
    VO_CHN mTestVOChn;
    VO_VIDEO_LAYER_ATTR_S mTestUILayerAttr;
    VIDEO_FRAME_INFO_S mTestUIFrame;

    MPP_SYS_CONF_S mSysConf;
    ISP_DEV mIspDev;
    VO_DEV mVoDev;
    
#if 0
    CLOCK_CHN mClockChn;
    CLOCK_CHN_ATTR_S mClockChnAttr;
#endif
    VIPP2VOLinkInfo mLinkInfoArray[VIPP2VO_NUM];
}SampleVIRVI2VOContext;
static int initSampleVIRVI2VOContext(SampleVIRVI2VOContext *pContext);
static int destroySampleVIRVI2VOContext(SampleVIRVI2VOContext *pContext);

#endif  /* _SAMPLE_VIRVI2VO_H_ */

