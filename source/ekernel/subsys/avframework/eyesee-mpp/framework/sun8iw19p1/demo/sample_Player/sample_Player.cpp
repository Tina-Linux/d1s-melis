//#define LOG_NDEBUG 0
#define LOG_TAG "sample_Player"
#include <utils/plat_log.h>

#include <sys/stat.h>
#include <pthread.h>

#include <cstdio>
#include <csignal>
#include <iostream>

#include <sys/prctl.h>

#include <hwdisplay.h>
#include <confparser.h>
#include <mpi_sys.h>
#include <mpi_vo.h>
#include <mpi_vdec.h>

#include "sample_Player_config.h"
#include "sample_Player.h"

using namespace std;
using namespace EyeseeLinux;

static SamplePlayerContext *pSamplePlayerContext = NULL;

static void handle_exit()
{
    alogd("user want to exit!");
    if(pSamplePlayerContext!=NULL)
    {
        cdx_sem_up(&pSamplePlayerContext->mSemExit);
    }
}

void EyeseePlayerListener::onCompletion(EyeseePlayer *pMp)
{
    alogd("receive onCompletion message!");
    cdx_sem_up(&mpOwner->mSemExit);
}

bool EyeseePlayerListener::onError(EyeseePlayer *pMp, int what, int extra)
{
    alogd("receive onError message![%d][%d]", what, extra);    //what: EyeseeLinux::EyeseePlayer::MEDIA_ERROR_UNKNOWN, extra: 
    switch(what)
    {
        case EyeseeLinux::EyeseePlayer::MEDIA_ERROR_IO:
            break;
        default:
            break;
    }
    return true;
}

void EyeseePlayerListener::onVideoSizeChanged(EyeseePlayer *pMp, int width, int height)
{
    alogd("receive onVideoSizeChanged message!size[%dx%d]", width, height);
}

bool EyeseePlayerListener::onInfo(EyeseePlayer *pMp, int what, int extra)
{
    alogd("receive onInfo message! media_info_type[%d] extra[%d]", what, extra);    //what: EyeseeLinux::EyeseePlayer::MEDIA_INFO_UNKNOWN
    switch(what)
    {
        case EyeseeLinux::EyeseePlayer::MEDIA_INFO_VIDEO_RENDERING_START:
            break;
        default:
            break;
    }
    return true;
}

void EyeseePlayerListener::onSeekComplete(EyeseePlayer *pMp)
{
    alogd("receive onSeekComplete message!");
}

EyeseePlayerListener::EyeseePlayerListener(SamplePlayerContext *pOwner)
    : mpOwner(pOwner)
{
}

SamplePlayerContext::SamplePlayerContext()
    :mPlayerListener(this)
{
    cdx_sem_init(&mSemExit, 0);
    mUILayer = HLAY(2, 0);
    mVoDev = -1;
    mVoLayer = -1;
    mpPlayer = NULL;
    mLoopNum = 0;
}

SamplePlayerContext::~SamplePlayerContext()
{
    cdx_sem_deinit(&mSemExit);
    if(mpPlayer!=NULL)
    {
        aloge("fatal error! EyeseePlayer is not destruct!");
    }
}

status_t SamplePlayerContext::ParseCmdLine(int argc, char *argv[])
{
    alogd("this program path:[%s], arg number is [%d]", argv[0], argc);
    status_t ret = NO_ERROR;
    int i=1;
    while(i < argc)
    {
        if(!strcmp(argv[i], "-path"))
        {
            if(++i >= argc)
            {
                std::string errorString;
                errorString = "fatal error! use -h to learn how to set parameter!!!";
                cout<<errorString<<endl;
                ret = -1;
                break;
            }
            mCmdLinePara.mConfigFilePath = argv[i];
        }
        else if(!strcmp(argv[i], "-h"))
        {
            std::string helpString;
            helpString += "CmdLine param example:\n";
            helpString += "\t run -path /home/sample_Player.conf\n";
            cout<<helpString<<endl;
            ret = 1;
            break;
        }
        else
        {
            std::string ignoreString;
            ignoreString += "ignore invalid CmdLine param:[";
            ignoreString += argv[i];
            ignoreString += ", type -h to get how to set parameter!";
            cout<<ignoreString<<endl;
        }
        i++;
    }
    return ret;
}

status_t SamplePlayerContext::loadConfig()
{
    int ret;
    char *ptr;
    std::string& ConfigFilePath = mCmdLinePara.mConfigFilePath;
    if(ConfigFilePath.empty())
    {
        alogd("user not set config file. use default test parameter!");
        mConfigPara.mFilePath = "/mnt/E/test.mp4";
        mConfigPara.mPicFormat = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        mConfigPara.mVdecOutputMaxWidth = 0;
        mConfigPara.mVdecOutputMaxHeight = 0;
        mConfigPara.mDisplayWidth = 480;
        mConfigPara.mDisplayHeight = 640; 
        mConfigPara.mbForbidAudio = false;
        mConfigPara.mRotation = ROTATE_NONE;
        mConfigPara.mbSetVideolayer = true;
        mConfigPara.mIntfType = VO_INTF_LCD;
        mConfigPara.mIntfSync = VO_OUTPUT_1080P60;
        mConfigPara.mVeFreq = 0;
        mConfigPara.mbLoop = false;
        return SUCCESS;
    }
    CONFPARSER_S stConfParser;
    ret = createConfParser(ConfigFilePath.c_str(), &stConfParser);
    if(ret < 0)
    {
        aloge("load conf fail");
        return UNKNOWN_ERROR;
    }
    mConfigPara.mFilePath = GetConfParaString(&stConfParser, SAMPLE_PLAYER_KEY_FILE_PATH, NULL);
    char *pStrPixelFormat = (char*)GetConfParaString(&stConfParser, SAMPLE_PLAYER_KEY_PIC_FORMAT, NULL); 
    if(!strcasecmp(pStrPixelFormat, "yu12"))
    {
        mConfigPara.mPicFormat = MM_PIXEL_FORMAT_YUV_PLANAR_420;
    }
    else if(!strcasecmp(pStrPixelFormat, "yv12"))
    {
        mConfigPara.mPicFormat = MM_PIXEL_FORMAT_YVU_PLANAR_420;
    }
    else if(!strcasecmp(pStrPixelFormat, "nv21"))
    {
        mConfigPara.mPicFormat = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    }
    else if(!strcasecmp(pStrPixelFormat, "nv12"))
    {
        mConfigPara.mPicFormat = MM_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    }
    else
    {
        aloge("fatal error! conf file pic_format is [%s]?", pStrPixelFormat);
        mConfigPara.mPicFormat = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    }
    mConfigPara.mVdecOutputMaxWidth = GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_VDEC_OUTPUT_MAX_WIDTH, 0); 
    mConfigPara.mVdecOutputMaxHeight = GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_VDEC_OUTPUT_MAX_HEIGHT, 0); 
    mConfigPara.mDisplayWidth = GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_DISPLAY_WIDTH, 0); 
    mConfigPara.mDisplayHeight = GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_DISPLAY_HEIGHT, 0); 
    mConfigPara.mbForbidAudio = (bool)GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_FORBID_AUDIO, 0); 
    mConfigPara.mRotation = (ROTATE_E)GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_ROTATION, 0); 
    mConfigPara.mbSetVideolayer = (bool)GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_SET_VIDEOLAYER, 0);
    char *pStrIntfType = (char*)GetConfParaString(&stConfParser, SAMPLE_PLAYER_KEY_INTERFACE_TYPE, NULL); 
    if(!strcasecmp(pStrIntfType, "lcd"))
    {
        mConfigPara.mIntfType = VO_INTF_LCD;
    }
    else if(!strcasecmp(pStrIntfType, "hdmi"))
    {
        mConfigPara.mIntfType = VO_INTF_HDMI;
    }
    else if(!strcasecmp(pStrIntfType, "cvbs"))
    {
        mConfigPara.mIntfType = VO_INTF_CVBS;
    }
    else
    {
        aloge("fatal error! conf file interface type is [%s]?", pStrIntfType);
        mConfigPara.mIntfType = VO_INTF_LCD;
    }
    char *pStrIntfSync = (char*)GetConfParaString(&stConfParser, SAMPLE_PLAYER_KEY_INTERFACE_SYNC, NULL); 
    if(!strcasecmp(pStrIntfSync, "480I") || !strcasecmp(pStrIntfSync, "NTSC"))
    {
        mConfigPara.mIntfSync = VO_OUTPUT_NTSC;
    }
    else if(!strcasecmp(pStrIntfSync, "1080P60"))
    {
        mConfigPara.mIntfSync = VO_OUTPUT_1080P60;
    }
    else if(!strcasecmp(pStrIntfSync, "PAL"))
    {
        mConfigPara.mIntfSync = VO_OUTPUT_PAL;
    }
    else
    {
        aloge("fatal error! conf file interface type is [%s]?", pStrIntfSync);
        mConfigPara.mIntfSync = VO_OUTPUT_NTSC;
    }
    mConfigPara.mVeFreq = GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_VE_FREQ, 0); 
    mConfigPara.mbLoop = (bool)GetConfParaInt(&stConfParser, SAMPLE_PLAYER_KEY_LOOP, 0); 
    destroyConfParser(&stConfParser);
    return SUCCESS;
}

static void* SampleDetectInputThread(void* pThreadData)
{
    SamplePlayerContext* pContext = (SamplePlayerContext*) pThreadData;
    int ret;
    alogd("detect input thread is running!");
    int nOldCancelType = 0;
    ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &nOldCancelType);
    if(ret != 0)
    {
        aloge("fatal error! pthread setcancelstate fail[%d]", ret);
    }
    alogd("set cancel type:%d, old:%d", PTHREAD_CANCEL_ASYNCHRONOUS, nOldCancelType);
    prctl(PR_SET_NAME, (unsigned long)"demo_Player_DetectInput", 0, 0, 0);
    while(1)
    {
        //alogd("before getchar.");
        char ch = getchar();
        alogd("after getchar[%c][0x%x].", ch, ch);
        if ( ch == 'q' || ch == '\03' )
        {
            handle_exit();
            alogd("detect input thread is exit.");
            return 0;
        }
        else
        {
        }
        //alogd("before testcancel");
        pthread_testcancel();
        //alogd("after testcancel");
    }
    alogd("fatal error! detect input thread is exit2.");
    return (void*)0;
}

int main_demo_Player(int argc, char *argv[])
{
    int result = 0;
    void *pValue = NULL;
    float leftVolume = 0, rightVolume = 0;
    VO_LAYER hlay = 0;
    cout<<"hello, sample_Player!"<<endl;
    SamplePlayerContext *pContext = new SamplePlayerContext;
    pSamplePlayerContext = pContext;
    //parse command line param
    if(pContext->ParseCmdLine(argc, argv) != NO_ERROR)
    {
        //aloge("fatal error! command line param is wrong, exit!");
        result = -1;
        goto _err0;
    }
    //parse config file.
    if(pContext->loadConfig() != SUCCESS)
    {
        aloge("fatal error! no config file or parse conf file fail");
        result = -1;
        goto _err0;
    }
    //register process function for SIGINT, to exit program.
    //if (signal(SIGINT, handle_exit) == SIG_ERR)
    //    perror("can't catch SIGSEGV");
    result = pthread_create(&pContext->mDetectInputThreadId, NULL, SampleDetectInputThread, pContext);
    if (result != 0)
    {
        aloge("fatal error! pthread create fail[0x%x]", result);
        goto _err0;
    }
    alogd("create detect input thread:[0x%lx]", pContext->mDetectInputThreadId);
    alogd("    Please enter 'q' or Ctrl-C to quit top command.");

    //init mpp system
    memset(&pContext->mSysConf, 0, sizeof(MPP_SYS_CONF_S));
    pContext->mSysConf.nAlignWidth = 32;
    AW_MPI_SYS_SetConf(&pContext->mSysConf);
    AW_MPI_SYS_Init();
    AW_MPI_VDEC_SetVEFreq(MM_INVALID_CHN, pContext->mConfigPara.mVeFreq);

    pContext->mpPlayer = new EyeseePlayer;
    pContext->mVoDev = 0;
    AW_MPI_VO_Enable(pContext->mVoDev);
    VO_PUB_ATTR_S stPubAttr;
    AW_MPI_VO_GetPubAttr(0, &stPubAttr);
    stPubAttr.enIntfType = pContext->mConfigPara.mIntfType;
    stPubAttr.enIntfSync = pContext->mConfigPara.mIntfSync;
    AW_MPI_VO_SetPubAttr(0, &stPubAttr);
    AW_MPI_VO_AddOutsideVideoLayer(pContext->mUILayer);
    AW_MPI_VO_CloseVideoLayer(pContext->mUILayer);//close ui layer.
    hlay = 0;
    while(hlay < VO_MAX_LAYER_NUM)
    {
        if(SUCCESS == AW_MPI_VO_EnableVideoLayer(hlay))
        {
            break;
        }
        hlay++;
    }
    if(hlay >= VO_MAX_LAYER_NUM)
    {
        aloge("fatal error! enable video layer fail!");
    }
    AW_MPI_VO_GetVideoLayerAttr(hlay, &pContext->mLayerAttr);
    pContext->mLayerAttr.stDispRect.X = 0;
    pContext->mLayerAttr.stDispRect.Y = 0;
    pContext->mLayerAttr.stDispRect.Width = pContext->mConfigPara.mDisplayWidth;
    pContext->mLayerAttr.stDispRect.Height = pContext->mConfigPara.mDisplayHeight;
    AW_MPI_VO_SetVideoLayerAttr(hlay, &pContext->mLayerAttr);
    pContext->mVoLayer = hlay;
    alogd("requestSurface hlay=%d", pContext->mVoLayer);
    pContext->mpPlayer->setOnCompletionListener(&pContext->mPlayerListener);
    pContext->mpPlayer->setOnErrorListener(&pContext->mPlayerListener);
    pContext->mpPlayer->setOnVideoSizeChangedListener(&pContext->mPlayerListener);
    pContext->mpPlayer->setOnInfoListener(&pContext->mPlayerListener);
    pContext->mpPlayer->setOnSeekCompleteListener(&pContext->mPlayerListener);
    if(pContext->mConfigPara.mbForbidAudio)
    {
        pContext->mpPlayer->grantPlayAudio(false);
    }
    pContext->mpPlayer->setDataSource(pContext->mConfigPara.mFilePath);
    //mCMP->setAudioStreamType(AUDIO_STREAM_MUSIC/*AudioManager.STREAM_MUSIC*/);
    if(pContext->mConfigPara.mbSetVideolayer)
    {
        pContext->mpPlayer->setDisplay(pContext->mVoLayer);
    }
    pContext->mpPlayer->setOutputPixelFormat(pContext->mConfigPara.mPicFormat);
    pContext->mpPlayer->enableScaleMode(true, pContext->mConfigPara.mVdecOutputMaxWidth, pContext->mConfigPara.mVdecOutputMaxHeight);
    pContext->mpPlayer->setRotation(pContext->mConfigPara.mRotation);
    pContext->mpPlayer->setLooping(pContext->mConfigPara.mbLoop);
    pContext->mpPlayer->prepare();
    pContext->mpPlayer->start();

    leftVolume = 0;
    rightVolume = 0;
    pContext->mpPlayer->getVolume(&leftVolume, &rightVolume);
    alogd("[0]get volume:left[%f], right[%f]", leftVolume, rightVolume);
//    int i;
//    for(i=0; i<5; i++)
//    {
//        leftVolume = rightVolume = 1.0;
//        pContext->mpPlayer->setVolume(leftVolume, rightVolume);
//        alogd("[1]set volume:left[%f], right[%f]", leftVolume, rightVolume);
//        pContext->mpPlayer->getVolume(&leftVolume, &rightVolume);
//        alogd("[1]get volume:left[%f], right[%f]", leftVolume, rightVolume);
//        sleep(10);
//        leftVolume = rightVolume = 0.1;
//        pContext->mpPlayer->setVolume(leftVolume, rightVolume);
//        alogd("[2]set volume:left[%f], right[%f]", leftVolume, rightVolume);
//        pContext->mpPlayer->getVolume(&leftVolume, &rightVolume);
//        alogd("[2]get volume:left[%f], right[%f]", leftVolume, rightVolume);
//        sleep(10);
//    }

    //sleep(10);
    cdx_sem_down(&pContext->mSemExit);
    //close player
    pContext->mpPlayer->stop();
    pContext->mpPlayer->reset();
    delete pContext->mpPlayer;
    pContext->mpPlayer = NULL;
    //close vo
    AW_MPI_VO_DisableVideoLayer(pContext->mVoLayer);
    pContext->mVoLayer = -1;
    AW_MPI_VO_RemoveOutsideVideoLayer(pContext->mUILayer);
    AW_MPI_VO_Disable(pContext->mVoDev);
    pContext->mVoDev = -1;
    //exit mpp system
    AW_MPI_SYS_Exit();

    result = pthread_cancel(pContext->mDetectInputThreadId);
    alogd("post cancel signal to pthread[0x%lx] join, ret[0x%x]", pContext->mDetectInputThreadId, result);
    result = pthread_join(pContext->mDetectInputThreadId, &pValue);
    alogd("pthread[0x%lx] join, ret[0x%x], pValue[%p]", pContext->mDetectInputThreadId, result, pValue);

    delete pContext;
    pSamplePlayerContext = NULL;
    cout<<"bye, sample_Player!"<<endl;
    return result;

_err0:
    delete pContext;
    pSamplePlayerContext = NULL;
    return result;
}

FINSH_FUNCTION_EXPORT_ALIAS(main_demo_Player, __cmd_demo_Player, test mpp framework player);

