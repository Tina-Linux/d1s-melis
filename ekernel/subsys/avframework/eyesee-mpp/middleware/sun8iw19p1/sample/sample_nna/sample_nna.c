#include "sample_nna.h"

int test_fun = 0;

int box_num = 0;
struct nna_fd_bbox box[200] = {0};
draw_box xybox[200] = {0};
struct osd_xybox
{   
    int osd_box_num;
    draw_box box[200];
};
struct osd_xybox *hb_xy = NULL;

static test_info_t test_info;
#define NNA_BUF_SIZE 2*1024*1024
#define SRC_BUF_SIZE 2*1024*1024
#define ALL_BUF_SIZE NNA_BUF_SIZE


unsigned int NNA_DDR_PHYADDR = 0;
unsigned int IMAGE_DDR_PHYADDR = 0;
unsigned int SCALE_DDR_PHYADDR = 0;
unsigned int NNA_DDR_VIRADDR = 0;
unsigned int IMAGE_DDR_VIRADDR = 0;
unsigned int SCALE_DDR_VIRADDR = 0;


#define SIZE_IMG_1080GRAY    (1920*1080)
#define TEST_NNAIRQ 0
#define TEST_BATCH_G2D 1
#define G2D_PROCESS_NUM 10

int mpp_init()
{
	// 初始化MPP
	int ret = 0;
    MPP_SYS_CONF_S mSysConf;
    memset(&mSysConf, 0, sizeof(MPP_SYS_CONF_S));
    mSysConf.nAlignWidth = 32;
    AW_MPI_SYS_SetConf(&mSysConf);
    AW_MPI_SYS_Init();
    if (ret < 0)
    {
        printf("sys Init failed!\n");
        return -1;
    }
    printf("sys init succeed!\n");
	return 0;
}

int mpp_destroy()
{
	AW_MPI_SYS_Exit();
	return 0;
}

static void *NNA_Proc(void* pThreadData)
{
    #if 1
    UserConfig* pUserCfg = (UserConfig*)pThreadData;
    assert(pUserCfg != NULL);

    VirVi_Params *pVirViParam = (VirVi_Params*)&pUserCfg->stVirViParams[VirVi_NNA];
    assert(pVirViParam != NULL);

    NNA_Params *pNNAParams = (NNA_Params*)&pUserCfg->stNNAParams;
    assert(pNNAParams != NULL);

    printf("Start NNA Thread!  mViDev[%d] mViChn[%d]",pVirViParam->iViDev,pVirViParam->iViChn);

    int iYSize = pNNAParams->iPicWidth * pNNAParams->iPicHeight;
    int iFrameIdx = 0;
    hb_xy = (struct osd_xybox *)malloc(sizeof(struct osd_xybox));
    memset(hb_xy,0,sizeof(struct osd_xybox));

    while (iFrameIdx < pNNAParams->iFrameNum)
    {
        //cdx_sem_wait(&pUserCfg->mSemExit);
        // 获取一帧YUV数据
        VIDEO_FRAME_INFO_S stFrameInfo;
        if (SUCCESS == AW_MPI_VI_GetFrame(pVirViParam->iViDev, pVirViParam->iViChn, &stFrameInfo, 500))
        {
            // 获取VirVi数据成功
            int64_t time0 = CDX_GetSysTimeUsMonotonic();

            if(test_fun==0)
            {   
                //printf("\n--->HD NNA!!!\n");
                box_num = nna_hd_run((char *)stFrameInfo.VFrame.mPhyAddr[0],pNNAParams->iPicWidth,pNNAParams->iPicHeight,box,200);
                //printf("\n<---HD NNA!!!\n");
            }
            else
            {
                //printf("\n--->FD NNA!!!\n");
                box_num = nna_fd_run((char *)stFrameInfo.VFrame.mPhyAddr[0],pNNAParams->iPicWidth,pNNAParams->iPicHeight,box,200);
                //printf("\n<---FD NNA!!!\n");
            }
            int64_t timeIntervalUs = (CDX_GetSysTimeUsMonotonic() - time0);
            printf("----->NNA Body detect %lld[us] box_num=%d iFrameIdx=%d\n",
                        timeIntervalUs,box_num,iFrameIdx);
            if(box_num == 0)
            {
                pthread_mutex_lock(&g_stResult_lock);
                hb_xy->osd_box_num = box_num;
                pthread_mutex_unlock(&g_stResult_lock);
                // 释放YUV数据
                AW_MPI_VI_ReleaseFrame(pVirViParam->iViDev, pVirViParam->iViChn, &stFrameInfo);
            }
            else
            {
                pthread_mutex_lock(&g_stResult_lock);
                hb_xy->osd_box_num = box_num;
                for(int k=0; k<box_num; k++)
                {
                    printf("----->NNA box:%d score:%3d [%4d %4d %4d %4d]\n\n",k,box[k].score,
                        box[k].x1,box[k].y1,box[k].x2,box[k].y2);

                        xybox[k].x1 = box[k].x1;
                        xybox[k].y1 = box[k].y1;
                        xybox[k].x2 = box[k].x2;
                        xybox[k].y2 = box[k].y2;
                        
                        hb_xy->box[k] = xybox[k];
                }
                pthread_mutex_unlock(&g_stResult_lock);
                // 释放YUV数据
                AW_MPI_VI_ReleaseFrame(pVirViParam->iViDev, pVirViParam->iViChn, &stFrameInfo);
            }
        }
        else
        {// 获取VirVi数据失败
            alogw("dev = %d  chn = %d GetFrame Failed!", pVirViParam->iViDev, pVirViParam->iViChn);
            usleep(20 * 1000);
        }
        iFrameIdx++;
    }
    free(hb_xy);
    printf("NNA_Proc_exit!!\n");
    #endif
    return NULL;
}
static void *VENC_Proc(void* pThreadData)
{

#if 1 
    int res_boxnum=0;
    int ret = 0;
    int last_box_num = 0;
    UserConfig* pUserCfg = (UserConfig*)pThreadData;
    assert(pUserCfg != NULL);

    VirVi_Params *pVirViParam = (VirVi_Params*)&pUserCfg->stVirViParams[VirVi_VENC];
    assert(pVirViParam != NULL);

    VENC_Params *pVENCParams = (VENC_Params*)&pUserCfg->stVENCParams;
    assert(pVENCParams != NULL);
    assert(pVENCParams->mVideoEncoderFmt == PT_H264);

    printf("Start VEnc Thread!  mViDev[%d] mViChn[%d]\n",pVirViParam->iViDev,pVirViParam->iViChn);

    VENC_CHN mVeChnH264 = 0; // VENC_CHN_0
    create_venc(pVENCParams, mVeChnH264, PT_H264);
#ifdef SAVE_H264_FILE
    // 打开文件，写入SPS，PPS头
    FILE* fd_out = fopen(pVENCParams->szOutputFile, "wb");

    VencHeaderData pH264SpsPpsInfo={0};
    AW_MPI_VENC_GetH264SpsPpsInfo(mVeChnH264, &pH264SpsPpsInfo);
    if (pH264SpsPpsInfo.nLength)
    {
        fwrite(pH264SpsPpsInfo.pBuffer, 1, pH264SpsPpsInfo.nLength, fd_out);
    }
#endif
    VENC_PACK_S mpPack;
    VENC_STREAM_S vencFrame;
    memset(&vencFrame, 0, sizeof(VENC_STREAM_S));
    vencFrame.mpPack = &mpPack;
    vencFrame.mPackCount = 1;
    
    RGN_ATTR_S stRgnAttr;
    RGN_CHN_ATTR_S stRgnChnAttr;
    memset(&stRgnAttr, 0, sizeof(RGN_ATTR_S));
    memset(&stRgnChnAttr, 0, sizeof(RGN_CHN_ATTR_S));
    MPP_CHN_S viChn = {MOD_ID_VIU, pVirViParam->iViDev,pVirViParam->iViChn};
    stRgnAttr.enType = ORL_RGN;
  
    int iFrameIdx = 0;
    int ret_get;
    while (iFrameIdx < pVENCParams->iFrameNum)
    {
        
        // 获取一帧YUV数据
        VIDEO_FRAME_INFO_S stFrameInfo;
        ret_get = AW_MPI_VI_GetFrame(pVirViParam->iViDev, pVirViParam->iViChn, &stFrameInfo, 500);
        if (ret_get == SUCCESS)
        {                        
            stFrameInfo.VFrame.mWidth           = pVENCParams->srcWidth;
            stFrameInfo.VFrame.mHeight          = pVENCParams->srcHeight;
            stFrameInfo.VFrame.mOffsetLeft      = 0;
            stFrameInfo.VFrame.mOffsetTop       = 0;
            stFrameInfo.VFrame.mOffsetRight     = stFrameInfo.VFrame.mOffsetLeft + stFrameInfo.VFrame.mWidth;
            stFrameInfo.VFrame.mOffsetBottom    = stFrameInfo.VFrame.mOffsetTop + stFrameInfo.VFrame.mHeight;
            stFrameInfo.VFrame.mField           = VIDEO_FIELD_FRAME;
            stFrameInfo.VFrame.mVideoFormat     =
 VIDEO_FORMAT_LINEAR;
            stFrameInfo.VFrame.mCompressMode    = COMPRESS_MODE_NONE;
            pthread_mutex_lock(&g_stResult_lock);
            if(hb_xy->osd_box_num > 0)
            {
                last_box_num = res_boxnum;
                res_boxnum = hb_xy->osd_box_num;
             
                //先destroy上一次创建的RNG
                for(int k=0; k < last_box_num; k++)
                {
                    ret = AW_MPI_RGN_Destroy(k);
                    if(ret != SUCCESS)
                        aloge("fatal error! why destory region fail?");
                } 
                //新建新的RNG 
                for(int k=0; k < res_boxnum; k++)
                {
                    ret = AW_MPI_RGN_Create(k, &stRgnAttr);
                    if(ret != SUCCESS)
                    {
                        aloge("fatal error! why create ORL region fail?[0x%x]", ret);
                        break;
                    }
                    int iBodyLeft   = hb_xy->box[k].x1   * VI_CAPTURE_WIDTH  / NNA_CALC_WIDTH;
                    int iBodyTop    = hb_xy->box[k].y1   * VI_CAPTURE_HIGHT  / NNA_CALC_HIGHT;
                    int iBodyRight  = hb_xy->box[k].x2   * VI_CAPTURE_WIDTH  / NNA_CALC_WIDTH;
                    int iBodyBottom = hb_xy->box[k].y2   * VI_CAPTURE_HIGHT  / NNA_CALC_HIGHT;
                    int iBodyWidth  = iBodyRight - iBodyLeft;
                    int iBodyHeight = iBodyBottom - iBodyTop;
                    //偶数对齐
                    if((iBodyLeft % 2)==0)
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.X = iBodyLeft;
                    else
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.X = iBodyLeft+1;

                    if((iBodyTop % 2)==0)
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.Y = iBodyTop;
                    else
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.Y = iBodyTop+1;

                    if((iBodyWidth % 2)==0)
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.Width = iBodyWidth;
                    else
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.Width = iBodyWidth+1;

                    if((iBodyHeight % 2)==0)
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.Height = iBodyHeight;
                    else
                        stRgnChnAttr.unChnAttr.stOrlChn.stRect.Height = iBodyHeight+1;
                        
                    stRgnChnAttr.bShow = TRUE;
                    stRgnChnAttr.enType = ORL_RGN;
                    stRgnChnAttr.unChnAttr.stOrlChn.enAreaType = AREA_RECT;
                    stRgnChnAttr.unChnAttr.stOrlChn.mColor = 0xFF0000 >> ((k % 3)*8);
                    stRgnChnAttr.unChnAttr.stOrlChn.mThick = 1;
                    stRgnChnAttr.unChnAttr.stOrlChn.mLayer = k;
                    printf("=====>RNG box:%d[X:%d Y:%d W:%d H:%d]\n",res_boxnum,iBodyLeft,iBodyTop,iBodyWidth,iBodyHeight);
                    ret = AW_MPI_RGN_AttachToChn(k, &viChn, &stRgnChnAttr);
                    if(ret != SUCCESS)
                        aloge("fatal error! why attach to vi channel[%d,%d] fail?",pVirViParam->iViDev,pVirViParam->iViChn);
                            
                }
            }
            if(hb_xy->osd_box_num == 0)
            {
                for(int k=0; k < last_box_num; k++)
                {
                    ret = AW_MPI_RGN_Destroy(k);
                    if(ret != SUCCESS)
                        aloge("fatal error! why destory region fail?");
                } 
                last_box_num = 0;
                res_boxnum = 0;
            }
            pthread_mutex_unlock(&g_stResult_lock);
            printf("=====>VENC iFrameIdx:%d\n\n",iFrameIdx);
            // 送入编码通道
            AW_MPI_VENC_SendFrame(mVeChnH264, &stFrameInfo, 60);
            // 读取编码结果
            int ret = AW_MPI_VENC_GetStream(mVeChnH264, &vencFrame, 20);
            if (SUCCESS == ret)
            {
#ifdef SAVE_H264_FILE
                //保存H264文件
                if (vencFrame.mpPack->mLen0)
                {
                    fwrite(vencFrame.mpPack->mpAddr0, 1, vencFrame.mpPack->mLen0, fd_out);
                }
                if (vencFrame.mpPack->mLen1)
                {
                    fwrite(vencFrame.mpPack->mpAddr1, 1, vencFrame.mpPack->mLen1, fd_out);
                }
#endif
                AW_MPI_VENC_ReleaseStream(mVeChnH264, &vencFrame);
            }
            // 释放YUV数据
            AW_MPI_VI_ReleaseFrame(pVirViParam->iViDev, pVirViParam->iViChn, &stFrameInfo);
        }
        else
        {// 获取VirVi数据失败
            alogw("dev = %d  chn = %d GetFrame Failed!", pVirViParam->iViDev, pVirViParam->iViChn);
            usleep(20 * 1000);
        }
        iFrameIdx++;
        //cdx_sem_signal(&pUserCfg->mSemExit);
    }
    destroy_venc(mVeChnH264);
#ifdef SAVE_H264_FILE
    fclose(fd_out);
#endif
    printf("Venc_Proc_exit!!\n");
#endif
    return NULL;
}
int sysconfig_init(UserConfig* pUserCfg)
{
    
    // Initial global vipp map
    g_VIPP_map[VIPP_0].bInit      = 0;
    g_VIPP_map[VIPP_0].width      = VI_CAPTURE_WIDTH;
    g_VIPP_map[VIPP_0].height     = VI_CAPTURE_HIGHT;
    g_VIPP_map[VIPP_0].eformat    = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    g_VIPP_map[VIPP_0].frame_rate = 20;
    g_VIPP_map[VIPP_0].bMirror    = 0;
    g_VIPP_map[VIPP_0].bFlip      = 0;
    pUserCfg->stVirViParams[VirVi_VENC].iViDev = VIPP_0;
    pUserCfg->stVirViParams[VirVi_VENC].iViChn = VirViChn_0;
    
    g_VIPP_map[VIPP_1].bInit      = 0;
    g_VIPP_map[VIPP_1].width      = NNA_CALC_WIDTH;
    g_VIPP_map[VIPP_1].height     = NNA_CALC_HIGHT;
    g_VIPP_map[VIPP_1].eformat    = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    g_VIPP_map[VIPP_1].frame_rate = 20;
    g_VIPP_map[VIPP_1].bMirror    = 0;
    g_VIPP_map[VIPP_1].bFlip      = 0;
    // Initial VirVi params
    pUserCfg->stVirViParams[VirVi_NNA].iViDev  = VIPP_1;
    pUserCfg->stVirViParams[VirVi_NNA].iViChn  = VirViChn_0;


    pUserCfg->stNNAParams.ePixFmt    = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pUserCfg->stNNAParams.iPicWidth  = NNA_CALC_WIDTH;
    pUserCfg->stNNAParams.iPicHeight = NNA_CALC_HIGHT;
    pUserCfg->stNNAParams.iFrmRate   = 20;
    pUserCfg->stNNAParams.iFrameNum  = TEST_FRAME_NUM;

    // Inital VENC params
    pUserCfg->stVENCParams.iFrameNum        = TEST_FRAME_NUM;//测试帧数
    strcpy(pUserCfg->stVENCParams.szOutputFile, "/mnt/extsd/nna_g2d/out/nna.h264");  //输出文件
    pUserCfg->stVENCParams.srcWidth         = VI_CAPTURE_WIDTH;
    pUserCfg->stVENCParams.srcHeight        = VI_CAPTURE_HIGHT;
    pUserCfg->stVENCParams.srcSize          = VI_CAPTURE_WIDTH;
    pUserCfg->stVENCParams.srcPixFmt        = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420; //NV21

    pUserCfg->stVENCParams.dstWidth         = VI_CAPTURE_WIDTH;
    pUserCfg->stVENCParams.dstHeight        = VI_CAPTURE_HIGHT;
    pUserCfg->stVENCParams.dstSize          = VI_CAPTURE_WIDTH;
    pUserCfg->stVENCParams.dstPixFmt        = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420; //NV21

    pUserCfg->stVENCParams.mVideoEncoderFmt = PT_H264; 
    pUserCfg->stVENCParams.mField           = VIDEO_FIELD_FRAME;
    pUserCfg->stVENCParams.mVideoBitRate    = 4 * 1024 * 1024;
    pUserCfg->stVENCParams.mVideoFrameRate  = 20;
    pUserCfg->stVENCParams.maxKeyFrame      = 20;

    pUserCfg->stVENCParams.mEncUseProfile   = 2; // profile 0-low 1-mid 2-high
    pUserCfg->stVENCParams.mRcMode          = 0; 
    pUserCfg->stVENCParams.rotate           = ROTATE_NONE;

    // Config UserCfg
    pUserCfg->stThreadData[PROC_NNA].ProcessFunc   = NNA_Proc;
    pUserCfg->stThreadData[PROC_NNA].mThreadID     = 0;
    pUserCfg->stThreadData[PROC_NNA].pPrivateData  = NULL;

    pUserCfg->stThreadData[PROC_VENC].ProcessFunc  = VENC_Proc;
    pUserCfg->stThreadData[PROC_VENC].mThreadID    = 0;
    pUserCfg->stThreadData[PROC_VENC].pPrivateData = NULL;

    return 0;
}
// VirVi数据处理线程

void nna_hw_reset_aw(void)
{
    printf("nna reset aw\n");
	if(ioctl(test_info.nna_fd, NNA_CMD_SET_CLOCK,NNA_CLOCK_300M) < 0)
    {
        printf("[%d][%s][%s]NNA_CMD_ENABLE_IRQ failure! \n", __LINE__,
               __FILE__, __FUNCTION__);
    }
	if(ioctl(test_info.nna_fd, NNA_CMD_RESET_NNA) < 0)
    {
        printf("[%d][%s][%s]NNA_CMD_ENABLE_IRQ failure! \n", __LINE__,
               __FILE__, __FUNCTION__);
    }
}


void nna_hw_reset(void )
{
	#define MAP_SIZE (64*1024UL)
	#define MAP_MASK (MAP_SIZE - 1)
	#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
			  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
	
	unsigned char * reg_base = (unsigned char *)0x03001000;
	unsigned int offset = 0x06EC; 
	unsigned int iRegVal = 0;

	int dev_fd = open("/dev/mem", O_RDWR | O_NDELAY);
	if (dev_fd < 0) {
		printf("# open(/dev/mem) failed.");
		return ;
	}
	reg_base = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0x03001000 );

	*(volatile unsigned int *)(reg_base + 0x06E0)= 0xC1000002;
	if(munmap(reg_base, MAP_SIZE) == -1)
	{
		printf("failure munmap!\n");
	}
	reg_base = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0x03001000);
	//clock reset
	*(volatile unsigned int *)(reg_base + 0x06EC)= 0;
	*(volatile unsigned int *)(reg_base + 0x06EC)= 0x10001;
	if(munmap(reg_base, MAP_SIZE) == -1)
	{
		printf("failure munmap!\n");
	}
	if(dev_fd)
	{
		close(dev_fd);
	}
}

void nna_hw_resize(
    unsigned int dst_addr,
    int dstw,
    int dsth,
    unsigned int src_addr,
    int srcw,
    int srch,
    unsigned int *box_list,     //x1 y1 x2 y2
    int box_num
    )
{
    printf("nna_resize!\n");
    unsigned char *bbuf     = (unsigned char *)malloc(SIZE_IMG_1080GRAY);
    unsigned char *cutbuf   = (unsigned char *)malloc(SIZE_IMG_1080GRAY);
    unsigned char *srcbuf   = (unsigned char *)malloc(SIZE_IMG_1080GRAY);
    unsigned char *p_img=(unsigned char *)src_addr;
    int x1,y1,x2,y2;
    char debug_file[128]={0};
    //char * debug_file = NULL;
    
    printf("dst_addr:%x,dstw:%d,dsth:%d,src_addr:%x,srcw:%d,srch:%d,box_num:%d\n",
            dst_addr,dstw,dsth,src_addr,srcw,srch,box_num);

    xramcpy((off_t)srcbuf, src_addr, srcw*srch, 2);

    for(int i=0; i<box_num; i++)    {
        x1= box_list[i*4+0];
        y1= box_list[i*4+1];
        x2= box_list[i*4+2];
        y2= box_list[i*4+3];
        nna_img_cut_c1(srcbuf, srcw, srch, cutbuf, x1, y1, x2, y2);
        nna_img_resize_c1(cutbuf, x2-x1+1, y2-y1+1, bbuf, dstw, dsth);
        xramcpy(dst_addr+dstw*dsth*i, (off_t)bbuf,dstw*dsth, 1);
        //if(i<box_num)
        //{
        //    debug_file = "/mnt/extsd/nna_g2d/out/scale_1.jpg";
        //}
        printf("ssss-->:%s %d\n",__FUNCTION__,__LINE__);
        sprintf(debug_file,"/mnt/extsd/nna_g2d/out/scale_%d.jpg",test_info.scale_num);
        printf("<---ssss:%s %s %d\n",debug_file,__FUNCTION__,__LINE__);
        test_info.scale_num ++;
        EncodeImageToFile(debug_file,dstw,dsth,1,bbuf);
    }
    free(bbuf);
    free(cutbuf);
    free(srcbuf);
}
    
void nna_hw_resize_aw
(
    unsigned int dst_addr, //物理地址
    int dstw,
    int dsth,
    unsigned int src_addr, //物理地址
    int srcw,
    int srch,
    unsigned int *box_list,     //x1 y1 x2 y2
    int box_num
    )
{

		//printf("nna_hw_resize!\n");
		AwG2dParam g2d_param;
	
		int x1,y1,x2,y2;
		int cutw, cuth;
		int tmp_num =0;
		unsigned int tmp_dst = dst_addr;
	
		g2d_param.g2dTotalNum = 0;
		g2d_param.scaleinfo = NULL;
	
		g2d_param.scaleinfo = (ScaleInfo*)malloc(sizeof(*(g2d_param.scaleinfo))*box_num);
		if(g2d_param.scaleinfo == NULL){
			printf("malloc param space failed\n");
			return;
		}
		for(int i=0; i<box_num; i++)
		{
			x1= box_list[tmp_num+0];
			y1= box_list[tmp_num+1];
			x2= box_list[tmp_num+2];
			y2= box_list[tmp_num+3];
			tmp_num += 4;
			cutw = x2 - x1 + 1;
			cuth = y2 - y1 + 1;
			//printf("cut x:%d y:%d w:%d h:%d\n",x1,y1,cutw,cuth);
			g2d_param.scaleinfo[i].use_phy_addr = 1;
			g2d_param.scaleinfo[i].src.addr = src_addr;
			g2d_param.scaleinfo[i].src.w  = srcw;
			g2d_param.scaleinfo[i].src.h  = srch;
			g2d_param.scaleinfo[i].src.format = G2D_FORMAT_Y8;
			g2d_param.scaleinfo[i].dst.addr  = tmp_dst;
			g2d_param.scaleinfo[i].dst.w	= dstw;
			g2d_param.scaleinfo[i].dst.h	= dsth;
			g2d_param.scaleinfo[i].dst.format = G2D_FORMAT_Y8;
			tmp_dst += dstw*dsth;
	
			g2d_param.scaleinfo[i].crop_info.x = x1;
			g2d_param.scaleinfo[i].crop_info.y = y1;
			g2d_param.scaleinfo[i].crop_info.width = cutw;
			g2d_param.scaleinfo[i].crop_info.height = cuth;
	
		}
		g2d_param.g2dTotalNum = box_num;
		AwG2dInit(test_info.aw_g2d,g2d_param);
		AwG2dProcess(test_info.aw_g2d);
		free(g2d_param.scaleinfo);
}

void nna_soft_wait_event(void)
{
    //wait nna interrupt event
    //printf("nna_soft_wait_event!\n");
    nna_wait_event_done();
}

static int enable_nna_irq(test_info_t *pTestInfo)
{
    if(pTestInfo == NULL)
    {
        printf("pTestInfo pointer is null!");
        return -1;
    }

    if(ioctl(pTestInfo->nna_fd, NNA_CMD_ENABLE_IRQ) < 0)
    {
        printf("[%d][%s][%s]NNA_CMD_ENABLE_IRQ failure!\n", __LINE__,
               __FILE__, __FUNCTION__);
        return -1;
    }

    return 0;
}

static int disable_nna_irq(test_info_t *pTestInfo)
{
    if(pTestInfo == NULL)
    {
        printf("pTestInfo pointer is null!");
        return -1;
    }

    if(ioctl(pTestInfo->nna_fd, NNA_CMD_DISABLE_IRQ) < 0)
    {
        printf("[%d][%s][%s]NNA_CMD_DISABLE_IRQ failure! error:%s\n", __LINE__,
               __FILE__, __FUNCTION__, strerror(errno));
        return -1;
    }

    return 0;
}

int main(int argc,char**agrv)
{    

    if(argc == 2)
        test_fun = 1;
//人脸
    else
        test_fun = 0;//人形
    
    int ret = 0;
    MemParameter mem_parm;
    MemBufInfo nna_buf[10];
	
    printf("sample nna buile time = %s, %s.\r\n", __DATE__, __TIME__);
    memset(&test_info, 0, sizeof(test_info_t));
    memset(&mem_parm, 0, sizeof(mem_parm));
#if TEST_BATCH_G2D
    test_info.aw_g2d = AwG2dCreate();
    if(test_info.aw_g2d == NULL)
    {
        printf("AwG2dCreate failed.\n");
        return -1;
    }
#endif
    test_info.aw_mem = GetMemAdapterOpsS();
    if(test_info.aw_mem == NULL)
    {
        printf("GetMemAdapterOpsS failed.\n");
        goto NNA_EXIT;
    }

    mem_parm.iommuFlag = 1;
    if(AwMemOpen(test_info.aw_mem,mem_parm) != 0)
    {
        printf("AwMemOpen failed.\n");
        goto NNA_EXIT;
    }

    //TODO: we should use cmd to control this function
#if TEST_NNAIRQ
    if((test_info.nna_fd = open("/dev/nna",O_RDWR)) == -1) {
        printf("open nna device fail!\n");
        goto NNA_EXIT;
    }
    enable_nna_irq(&test_info);
#endif

   //alloc 6M continuous phy address for nna
    nna_buf[0].bufferSize = ALL_BUF_SIZE;

    if(AwMemPalloc(test_info.aw_mem,nna_buf,1) != 0)
    {
        printf("AwMemPalloc failed. size:%d\n",nna_buf[0].bufferSize);
        goto NNA_EXIT;
    }
	//nna buf used by g2d  2M
	test_info.nna_buf_phy = (unsigned int)nna_buf[0].phyAddr;
    test_info.nna_buf_vir = (unsigned char*)nna_buf[0].virAddr;
    test_info.nna_buf_size  = NNA_BUF_SIZE;

    nna_init(NNA_BASE_ADDRESS);
    nna_setup_buffer(test_info.nna_buf_phy,(unsigned int)test_info.nna_buf_vir,test_info.nna_buf_size);
    nna_setup_reset_handler(nna_hw_reset);//设置复位功能回调函数
	nna_reset();
    nna_setup_wait_event_handler(nna_soft_wait_event);//设置等待NNA完成事件回调函数
#if TEST_BATCH_G2D
    nna_setup_resize_handler(nna_hw_resize_aw);//设置缩放功能回调函数
#else
    nna_setup_resize_handler(nna_hw_resize);
#endif

    printf("NNA MEM ALLOC SUCCEED!!\n\n");

    // 启动MPP
    mpp_init();
    
    UserConfig stUserCfg;
    memset(&stUserCfg, 0, sizeof(UserConfig));
    sysconfig_init(&stUserCfg);
    
    //cdx_sem_init(&stUserCfg.mSemExit,0);

    create_vi(stUserCfg.stVirViParams);
    // Create Process thread
    for (int ProcIndex = 0; ProcIndex < PROC_MAX; ProcIndex++)
    {
        // 启动数据采集线程
        ret = pthread_create(&stUserCfg.stThreadData[ProcIndex].mThreadID
                            ,NULL
                            ,stUserCfg.stThreadData[ProcIndex].ProcessFunc
                            ,(void*)&stUserCfg
                            );
        if (ret)
        {
            printf("chn 0 Save YUV Thread exit! ret[%d]",ret);
            abort();
        }
    }
    // 等待所有线程结束
    for (int ProcIndex = 0; ProcIndex < PROC_MAX; ProcIndex++)
    {
        pthread_join(stUserCfg.stThreadData[ProcIndex].mThreadID, (void*)&ret);
        printf("Thread %d Exit!", ProcIndex);
    }
    destroy_vi(stUserCfg.stVirViParams);

    // 销毁MPP
    mpp_destroy();
    nna_close();
    pthread_mutex_destroy(&g_stResult_lock);
    //cdx_sem_deinit(&stUserCfg.mSemExit);

NNA_EXIT:
#if TEST_NNAIRQ
        if(test_info.nna_fd > 0){
            disable_nna_irq(&test_info);
            close(test_info.nna_fd);
        }
#endif
        if(test_info.aw_mem != NULL)
        {
            AwMemPfree(test_info.aw_mem, nna_buf, 1);
            AwMemClose(test_info.aw_mem);
        }
#if TEST_BATCH_G2D
        if(test_info.aw_g2d != NULL)
            AwG2dDestroy(test_info.aw_g2d);
#endif
    printf("sample_face_detect exit!\n");
    return 0;
}

