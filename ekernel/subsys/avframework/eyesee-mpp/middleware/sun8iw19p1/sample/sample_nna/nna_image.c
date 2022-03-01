/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : sample_face_detect.c
  Version       : V6.0
  Author        : Allwinner BU3-XIAN Team
  Created       :
  Last Modified : 2017/11/30
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/
#include "sample_nna.h"

int64_t g_last_eve_time = 0;
int64_t g_last_vo_time = 0;
int64_t g_exit_flag = 0;

int box_num = 0;
struct nna_fd_bbox box[200] = {0};
draw_box xybox[200] = {0};

test_info_t test_info;
unsigned int NNA_DDR_PHYADDR = 0;
unsigned int IMAGE_DDR_PHYADDR = 0;
unsigned int SCALE_DDR_PHYADDR = 0;
unsigned int NNA_DDR_VIRADDR = 0;
unsigned int IMAGE_DDR_VIRADDR = 0;
unsigned int SCALE_DDR_VIRADDR = 0;
MemBufInfo nna_buf[10];

#define TEST_FACEDET    0
#define SIZE_IMG_1080GRAY    (1920*1080)
#define TEST_NNAIRQ 0
#define TEST_BATCH_G2D 1
#define G2D_PROCESS_NUM 10

#define DISPALIGN(value, align) ((align==0)?(unsigned long)value:((((unsigned long)value) + ((align) - 1)) & ~((align) - 1)))

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
//	printf( "\n# nna_reset:clock div:2" );
	reg_base = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0x03001000 );

	  //printf( "\n# nna_reset1" ); 
	*(volatile unsigned int *)(reg_base + 0x06E0)= 0xC1000002;
//	usleep(2000);
	//printf( "\n# nna_reset2" ); 
	if(munmap(reg_base, MAP_SIZE) == -1)
	{
		printf("failure munmap!\n");
	}
	//printf( "\n# nna_reset3" ); 
	reg_base = (unsigned char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0x03001000);
	//clock reset
	*(volatile unsigned int *)(reg_base + 0x06EC)= 0;
//	usleep(2000);
	*(volatile unsigned int *)(reg_base + 0x06EC)= 0x10001;
//	usleep(2000);
	//printf( "\n# nna_reset4" ); 
	if(munmap(reg_base, MAP_SIZE) == -1)
	{
		printf("failure munmap!\n");
	}
	//printf( "\n# nna_reset5" ); 
	if(dev_fd)
	{
		close(dev_fd);
	}
	//printf( "\n# nna_reset6" ); 
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
    g2d_param.g2dTotalNum = 0;
    int tmp_num =0;
    char src_file[50]={'\0'};
    char dst_file[50]={'\0'};
    unsigned int tmp_dst = dst_addr;
    unsigned char* dst_addr_vir = NULL;
    unsigned char* dst_vir_tmp = NULL;
    unsigned char* src_addr_vir = NULL;

    dst_addr_vir = (unsigned char*)AwMemGetVirtualAddressCpu(test_info.aw_mem,(void *)dst_addr);
    if(dst_addr_vir == NULL){
        printf("we can not get dst virAddr\n");
        return;
    }

    dst_vir_tmp = dst_addr_vir;

    src_addr_vir = (unsigned char*)AwMemGetVirtualAddressCpu(test_info.aw_mem,(void *)src_addr);
    if(src_addr_vir == NULL){
        printf("we can not get src virAddr\n");
        return;
    }

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
        g2d_param.scaleinfo[i].dst.w    = dstw;
        g2d_param.scaleinfo[i].dst.h    = dsth;
        g2d_param.scaleinfo[i].dst.format = G2D_FORMAT_Y8;
        tmp_dst += dstw*dsth;

        g2d_param.scaleinfo[i].crop_info.x = x1;
        g2d_param.scaleinfo[i].crop_info.y = y1;
        g2d_param.scaleinfo[i].crop_info.width = cutw;
        g2d_param.scaleinfo[i].crop_info.height = cuth;

    }
    g2d_param.g2dTotalNum = box_num;
    AwG2dInit(test_info.aw_g2d,g2d_param);
    if(AwG2dProcess(test_info.aw_g2d) == 0){
        /*
        for(int z=0; z<box_num;z++){
            sprintf(dst_file,"/mnt/nna/out/scale_%d.jpg",test_info.scale_num);
            test_info.scale_num ++;
            EncodeImageToFile(dst_file,dstw,dsth,1,dst_vir_tmp);
            dst_vir_tmp += dstw*dsth;
        }
        test_info.scale_num =0;*/
    }
    //printf("g2d run time:%lld ms\n",(get_sys_timeUs()-detect_time)/1000);
    free(g2d_param.scaleinfo);

}

void nna_soft_wait_event(void)
{
    //wait nna interrupt event
    printf("nna_soft_wait_event!\n");
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


int main(int argc, char *argv[])
{
    
    printf("sample_nna buile time = %s, %s.\r\n", __DATE__, __TIME__);
    int ret = -1;
    char *szfile = NULL;
    FILE *file = NULL;
    char jpeg_path[100]={'0'};
    unsigned char *inputImage = NULL;
    unsigned char *decode_img = NULL;
    int Channels = 0 ;
    int Width = 0;
    int Height = 0;
    MemParameter mem_parm;

    memset(&test_info, 0, sizeof(test_info_t));
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
   //alloc 2M for input image
    nna_buf[0].bufferSize = 6*1024*1024;
    nna_buf[1].bufferSize = 2*1024*1024;

    if(AwMemPalloc(test_info.aw_mem,nna_buf,2) != 0)
    {
        printf("AwMemPalloc failed. size:%d\n",nna_buf[0].bufferSize);
        goto NNA_EXIT;
    }

    NNA_DDR_PHYADDR = (unsigned int)nna_buf[0].phyAddr;
    NNA_DDR_VIRADDR = (unsigned int)nna_buf[0].virAddr;

    decode_img = (unsigned char *)nna_buf[1].virAddr;

    nna_init(NNA_BASE_ADDRESS);
    //nna_setup_buffer(NNA_DDR_PHYADDR,1024*1024*4);
    nna_setup_buffer(NNA_DDR_PHYADDR,NNA_DDR_VIRADDR,1024*1024*4);
    nna_setup_reset_handler(nna_hw_reset);

    nna_reset();
    nna_setup_wait_event_handler(nna_soft_wait_event);
#if TEST_BATCH_G2D
    nna_setup_resize_handler(nna_hw_resize_aw);
#else
    nna_setup_resize_handler(nna_hw_resize);
#endif
    IMAGE_DDR_PHYADDR = NNA_DDR_PHYADDR + 1024*1024*4; //启始4MB空间
    SCALE_DDR_PHYADDR = NNA_DDR_PHYADDR + 1024*1024*2; //启始4MB空间

    IMAGE_DDR_VIRADDR = NNA_DDR_VIRADDR + 1024*1024*4; //启始4MB空间
    SCALE_DDR_VIRADDR = NNA_DDR_VIRADDR + 1024*1024*2; //启始4MB空间

    printf("NNA MEM ALLOC SUCCEED!!\n\n");

    /*--------------------------------*/
    printf("\n\nTest input is image!!!n\n");
    szfile = argv[1];
    //this file content iamge name list
    file = fopen(szfile, "r");
    if(file == NULL){
        printf("szfile file is NULL\n");
        //goto NNA_EXIT;
    }
	if(file != NULL){
			    while(feof(file) == 0)
			    {
			        fscanf(file, "%99s\n", jpeg_path);
			        printf("jpg filename = %s\n", jpeg_path);
			        //start nna test
			        inputImage = DecodeImageFromFile(jpeg_path, &Width, &Height, &Channels);
			        printf("W:%d H:%d CH:%d\n",Width, Height, Channels);    
			        if (inputImage == NULL || Channels != 3){
			            printf("read input data failed! inputImage:%p,channel:%d",inputImage,Channels);
			            //goto NNA_EXIT;
			        }
			        printf("aaa:%s %d\n",__FUNCTION__,__LINE__);
			        //double startTime = now();
			        xrgb_to_c1(decode_img, inputImage, Width, Height);

			        printf("bbb:%s %d\n",__FUNCTION__,__LINE__);

			        AwMemCopy(test_info.aw_mem,(void *)IMAGE_DDR_VIRADDR,(void *)decode_img,Width*Height);

			        EncodeImageToFile("/mnt/extsd/nna_g2d/out/src_pic.jpg", Width, Height, 1, (unsigned char *)IMAGE_DDR_VIRADDR);
#if TEST_FACEDET
			        printf("nna_fd_run-->:%s %d\n",__FUNCTION__,__LINE__);
			        box_num = nna_fd_run((char *)IMAGE_DDR_PHYADDR,  Width,  Height, box,  200);
			        printf("<--nna_fd_run:%s %d\n",__FUNCTION__,__LINE__);
#else
			        box_num = nna_hd_run((char *)IMAGE_DDR_PHYADDR,  Width,  Height, box,  200);
#endif
			        //double nDetectTime = calcElapsed(startTime, now());

			        printf("NNA FD face num: %d \n", box_num);
			        for(int i=0; i<box_num; i++)
			        {
			            printf("box:%d score:%3d [%4d %4d %4d %4d]\n",i,box[i].score,box[i].x1,box[i].y1,box[i].x2,box[i].y2);
			            xybox[i].x1 = box[i].x1;
			            xybox[i].y1 = box[i].y1;
			            xybox[i].x2 = box[i].x2;
			            xybox[i].y2 = box[i].y2;
			        }
			        //printf("face detection cost time:%lld\n",nDetectTime);
			        if(box_num > 0){
			            draw_face_box(inputImage, Width, Channels,xybox, box_num,5);
			            EncodeImageToFile("/mnt/extsd/nna_g2d/out/faceresult.jpg", Width, Height, Channels, inputImage);
			        }
			        free(inputImage);
			    fclose(file);
			    //NNA_END();
			    printf("face detection success!!!\n");
			}
		}
NNA_EXIT:
#if TEST_NNAIRQ
        if(test_info.nna_fd > 0){
            disable_nna_irq(&test_info);
            close(test_info.nna_fd);
        }
#endif
        if(test_info.aw_mem != NULL)
        {
            AwMemPfree(test_info.aw_mem, nna_buf, 2);
            AwMemClose(test_info.aw_mem);
        }
#if TEST_BATCH_G2D
        if(test_info.aw_g2d != NULL)
            AwG2dDestroy(test_info.aw_g2d);
#endif

    return 0;
}

