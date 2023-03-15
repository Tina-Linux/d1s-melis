
#ifndef _SAMPLE_VIRVI2VO_H_
#define _SAMPLE_VIRVI2VO_H_

#include <plat_type.h>
#include <tsemaphore.h>
#include <mm_comm_vo.h>
#include <mpi_sys.h>
#include <mpi_clock.h>
#include <mpi_vo.h>

#define MAX_FILE_PATH_SIZE (256)

typedef struct SampleVIRVI2VOCmdLineParam
{
    char mConfigFilePath[MAX_FILE_PATH_SIZE];
}SampleVIRVI2VOCmdLineParam;

typedef struct SampleVIRVI2VOConfig
{
    int mDevNum;
    int mCaptureWidth;
    int mCaptureHeight;
    int mDisplayWidth;
    int mDisplayHeight;
    PIXEL_FORMAT_E mPicFormat; //MM_PIXEL_FORMAT_YUV_PLANAR_420
    int mFrameRate;
    int mTestDuration;  //unit:s, 0 mean infinite
    VO_INTF_TYPE_E mDispType;
    VO_INTF_SYNC_E mDispSync;
}SampleVIRVI2VOConfig;

typedef struct SampleVIRVI2VOContext
{
    SampleVIRVI2VOCmdLineParam mCmdLinePara;
    SampleVIRVI2VOConfig mConfigPara;

    cdx_sem_t mSemExit;
    int mUILayer;

    MPP_SYS_CONF_S mSysConf;
    VO_DEV mVoDev;
    VO_LAYER mVoLayer;
    VO_VIDEO_LAYER_ATTR_S mLayerAttr;
    VO_CHN mVOChn;
#if 0
    CLOCK_CHN mClockChn;
    CLOCK_CHN_ATTR_S mClockChnAttr;
#endif
    VI_DEV mVIDev;
    VI_CHN mVIChn;
}SampleVIRVI2VOContext;
int initSampleVIRVI2VOContext();
int destroySampleVIRVI2VOContext();

#endif  /* _SAMPLE_VIRVI2VO_H_ */

