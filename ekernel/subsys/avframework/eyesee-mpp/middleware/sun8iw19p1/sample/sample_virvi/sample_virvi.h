
#ifndef _SAMPLE_VIRVI_H_
#define _SAMPLE_VIRVI_H_

#include <plat_type.h>
#include <tsemaphore.h>

#define MAX_FILE_PATH_SIZE  (256)
typedef struct awVirVi_PrivCap_S {
    pthread_t thid;
    VI_DEV Dev;
    VI_CHN Chn;
    int s32MilliSec;
    VIDEO_FRAME_INFO_S pstFrameInfo;
    void *mpContext;    //SampleVirViContext*

    int mRawStoreNum;
    int mJpegStoreNum;

    VENC_CHN mChn;
    VENC_CHN_ATTR_S mChnAttr;
    VENC_PARAM_JPEG_S mJpegParam;
    VENC_EXIFINFO_S mExifInfo;
    VENC_STREAM_S mOutStream;
    VENC_JPEG_THUMB_BUFFER_S mJpegThumbBuf;
    cdx_sem_t mSemFrameBack;
    unsigned int mCurFrameId;
} VirVi_Cap_S;
int initVirVi_Cap_S(VirVi_Cap_S *pThiz);
int destroyVirVi_Cap_S(VirVi_Cap_S *pThiz);

typedef struct SampleVirViCmdLineParam
{
    char mConfigFilePath[MAX_FILE_PATH_SIZE];
}SampleVirViCmdLineParam;

typedef struct SampleVirViConfig
{
    int AutoTestCount;
    int GetFrameCount;
    int DevNum;
    int PicWidth;
    int PicHeight;
    int FrameRate;
    unsigned int PicFormat; //V4L2_PIX_FMT_NV21M
    enum v4l2_colorspace mColorSpace;
    int mRawStoreCount;	//the picture number of storing. 0 means don't store pictures.
	int mRawStoreInterval;	//n: store one picture of n pictures.
	int mJpegStoreCount;
    int mJpegStoreInterval;	//n: store one picture of n pictures.
	char mStoreDirectory[MAX_FILE_PATH_SIZE];   //e.g.: /mnt/extsd
    
}SampleVirViConfig;

typedef struct SampleVirViContext
{
    SampleVirViCmdLineParam mCmdLinePara;
    SampleVirViConfig mConfigPara;

    VirVi_Cap_S privCap[MAX_VIPP_DEV_NUM][MAX_VIR_CHN_NUM];

}SampleVirViContext;

#endif  /* _SAMPLE_VIRVI_H_ */

