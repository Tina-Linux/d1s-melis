#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <hal_mem.h>

#include <unistd.h>

#include <fcntl.h>
#include <dfs_posix.h>

#include "../hal/source/g2d_rcq/g2d_driver.h"
static int init_flag;
extern unsigned long dma_coherent_alloc(unsigned long *virt_addr, unsigned long size);
extern void dma_coherent_free(unsigned long virt_addr);

extern void hal_dcache_clean(unsigned long vaddr_start, unsigned long size); //D¡ä¨¨?
extern void hal_dcache_invalidate(unsigned long vaddr_start, unsigned long size);//?¨¢¨¨?

typedef struct Crop {
	int x;
	int y;
	int width;
	int height;
} Crop;

typedef struct ScaleTaget {
	int sfd;
	int format;
	int channel;
    int w;
	int h;
    int addr;
} ScaleTaget;

typedef struct ImageAddr {
    unsigned int addr1;
    unsigned int addr2;
    unsigned int addr3;
} ImageAddr;


typedef struct ScaleInfo {
	int use_phy_addr;
	ScaleTaget src;
	ScaleTaget dst;
	Crop crop_info;
} ScaleInfo;

typedef struct _AwG2dParamT
{
	int g2dTotalNum;
	ScaleInfo* scaleinfo;
} Aw_G2dParam;

typedef struct _AwG2dT
{
	int        g2dFd;
	int        g2dInfoNum;
	struct mixer_para *g2dInfo;
} Aw_G2d;

extern unsigned long dma_coherent_alloc(unsigned long *virt_addr, unsigned long size);
extern void dma_coherent_free(unsigned long virt_addr);

#define SRC_X 0
#define SRC_Y 0
#define SRC_W 555
#define SRC_H 333

#define DST_X 0
#define DST_Y 0
#define DST_W 555
#define DST_H	333
#define SRC_FORMAT 0
#define DST_FORMAT 0

int fillrect(void)
{
   	g2d_fillrect_h info;
    char *buf1 = NULL;
    int size = 0;
    int ii=0;
    int ret = -1;

	FILE* file1;
	printf("------------fillrect------------\n");

    rt_device_t dev = rt_device_find("g2d");
	if (!dev){
		printf("can not find device\n");
		ret = -1;
		return -1;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		return -1;
	}




    memset(&info, 0, sizeof(g2d_fillrect_h));
    info.dst_image_h.bbuff = 1;
    info.dst_image_h.format = G2D_FORMAT_ARGB8888;
    info.dst_image_h.color = 0xffee0055;
    info.dst_image_h.width = 800;
    info.dst_image_h.height = 480;

    info.dst_image_h.clip_rect.x = 100;
    info.dst_image_h.clip_rect.y = 100;
    info.dst_image_h.clip_rect.w = SRC_W;
    info.dst_image_h.clip_rect.h = SRC_H;

    printf("clip_rect:%d,%d,%d,%d\n",info.dst_image_h.clip_rect.x,info.dst_image_h.clip_rect.y,
        info.dst_image_h.clip_rect.w,info.dst_image_h.clip_rect.h);

    info.dst_image_h.alpha = 255;
    info.dst_image_h.mode = G2D_GLOBAL_ALPHA;
    info.dst_image_h.use_phy_addr = 1;

    size = info.dst_image_h.width*info.dst_image_h.height*4;

    buf1 = (char *)hal_malloc(size);
    if(buf1==NULL)
    {
        printf("buf1 malloc fail!\n");
    }

	memset(buf1, 0, size);

#if 1
        	file1 = fopen("e:\\11.dat", "rb+");
	if (file1 == NULL)
		printf("err in fopen\n");
	ret = fread((void*)buf1, size, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);
#endif
   //  hal_dcache_clean(buf1, size);
    info.dst_image_h.laddr[0] = (__u32)__va_to_pa((unsigned long)buf1);

	printf("info.dst_image_h.laddr[0]=%p\n",info.dst_image_h.laddr[0]);

    ret = dev->control(dev, G2D_CMD_FILLRECT_H, &info);
  //  hal_dcache_invalidate(buf1, size);

    	file1 = fopen("e:\\fill_argb.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");


		printf("[]ret:%d\n",ret);
	ret = fwrite(buf1,size, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);
    rt_device_close(dev);

    printf("-------ret:%d---------\n",ret);
    return ret;
}


int fillrect_1(const char **argv,Aw_G2d* g2d)
{
    g2d_fillrect_h info;
  //  char *buf1 = NULL;
    int size = 0;
    int ii=0;
    int ret = -1;
    FILE* rfp=NULL,*wfp = NULL;
    unsigned long rbuf;
     char *wbuf=NULL;
    unsigned long phy_addr1 = 0, phy_addr2 = 0;

	printf("start open\n");

    printf("argv:");
    for(ii=0;ii<11;ii++)
    {
        printf("%s,",argv[ii]);
    }
    printf("\n");


    memset(&info, 0, sizeof(g2d_fillrect_h));
    info.dst_image_h.bbuff = 1;
    info.dst_image_h.format = atoi(argv[1]);//g2d_fmt_enh
    info.dst_image_h.color = 0xffee0055;
    info.dst_image_h.width = atoi(argv[3]);
    info.dst_image_h.height = atoi(argv[4]);

    info.dst_image_h.clip_rect.x = atoi(argv[5]);
    info.dst_image_h.clip_rect.y = atoi(argv[6]);
    info.dst_image_h.clip_rect.w = atoi(argv[7]);
    info.dst_image_h.clip_rect.h = atoi(argv[8]);

    printf("clip_rect:%d,%d,%d,%d\n",info.dst_image_h.clip_rect.x,info.dst_image_h.clip_rect.y,
        info.dst_image_h.clip_rect.w,info.dst_image_h.clip_rect.h);

    info.dst_image_h.alpha = atoi(argv[9]);
    info.dst_image_h.mode = atoi(argv[10]);
    info.dst_image_h.use_phy_addr = 1;

    size = info.dst_image_h.width*info.dst_image_h.height*4;

    rfp =fopen("/mnt/E/11.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    phy_addr1=dma_coherent_alloc(&rbuf,size);
    if(phy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)rbuf,0,size);
    fread((void *)rbuf,1,size,rfp);

    info.dst_image_h.laddr[0] = phy_addr1;

    ret = ioctl(g2d->g2dFd, G2D_CMD_FILLRECT_H, &info);
    if(ret != 0)
    {
        printf("\n[err]G2D_CMD_FILLRECT_H err!\n");
        ret = -1;
        goto __err;

    }
    printf("-----1-------\n");
    wfp =fopen("/mnt/E/fill.dat", "wb");
    if(wfp==NULL)
    {
        printf("\n[err]wfp==NULL\n");
        ret = -1;
        goto __err;
    }

    wbuf = malloc(size);
    memcpy(wbuf,(void *)rbuf,size);
    printf("-----2-------\n");


    fwrite((void *)wbuf,1,size,wfp);
    free(wbuf);

    printf("ret : %d\n",ret);
__err:
    if(rbuf)
    {
        dma_coherent_free(rbuf);
        rbuf = NULL;
    }
    if(rfp)
    {
        fclose(rfp);
    }
    if(wfp)
    {
        fclose(wfp);
    }
    printf("fillrect_1 end ! ret : %d\n",ret);
    return ret;
}


Aw_G2d* G2dCreate(void)
{
    printf("AwG2dCreate!\n");
    Aw_G2d* p = (Aw_G2d*)malloc(sizeof(Aw_G2d));
    if(p == NULL)
        return NULL;

    p->g2dFd = open("/dev/g2d",O_RDWR);
    if(p->g2dFd == -1) {
        printf("open g2d device fail!\n");
        free(p);
        return NULL;
    }
    p->g2dInfo = NULL;
    return p;
}

#if 0
int G2dInit(Aw_G2d* self, Aw_G2dParam param)
{
    struct mixer_para* g2d_info = NULL;
    ScaleInfo* scale_info = param.scaleinfo;
    int fb_width, fb_height;

    if(self == NULL)
        return -1;

    self->g2dInfoNum = param.g2dTotalNum;
    if(self->g2dInfo == NULL)
    {
        g2d_info = (struct mixer_para*)malloc(sizeof(*g2d_info)*param.g2dTotalNum);
        if(g2d_info == NULL)
        {
            printf("malloc failed!\n");
            return -1;
        }
        self->g2dInfo = g2d_info;
    }else{
        printf("g2d info is not null please check\n");
    }
    //for scale
    for(int idx=0; idx<param.g2dTotalNum; idx++){

        g2d_info[idx].flag_h = G2D_BLT_NONE_H;
        g2d_info[idx].op_flag = OP_BITBLT;

		g2d_info[idx].src_image_h.format = (g2d_fmt_enh)scale_info[idx].src.format;
		g2d_info[idx].dst_image_h.format = (g2d_fmt_enh)scale_info[idx].dst.format;
    	g2d_info[idx].src_image_h.width  = scale_info[idx].src.w;
    	g2d_info[idx].src_image_h.height = scale_info[idx].src.h;
    	g2d_info[idx].src_image_h.mode = G2D_GLOBAL_ALPHA;// G2D_PIXEL_ALPHA;

    	g2d_info[idx].src_image_h.alpha = 0x0;
    	g2d_info[idx].src_image_h.color = 0xFFFFFFFF;
        g2d_info[idx].dst_image_h.alpha = 0x0;
    	g2d_info[idx].dst_image_h.color = 0xFFFFFFFF;

    	g2d_info[idx].src_image_h.clip_rect.x = scale_info[idx].crop_info.x;
    	g2d_info[idx].src_image_h.clip_rect.y = scale_info[idx].crop_info.y;
    	g2d_info[idx].src_image_h.clip_rect.w = scale_info[idx].crop_info.width;
    	g2d_info[idx].src_image_h.clip_rect.h = scale_info[idx].crop_info.height;
    	g2d_info[idx].src_image_h.align[0] = 0;
    	g2d_info[idx].src_image_h.align[1] = 0;
    	g2d_info[idx].src_image_h.align[2] = 0;

    	g2d_info[idx].dst_image_h.width  = scale_info[idx].dst.w;
    	g2d_info[idx].dst_image_h.height = scale_info[idx].dst.h;
    	g2d_info[idx].dst_image_h.mode = G2D_GLOBAL_ALPHA; //G2D_PIXEL_ALPHA;

    	g2d_info[idx].dst_image_h.clip_rect.x = 0;
    	g2d_info[idx].dst_image_h.clip_rect.y = 0;
    	g2d_info[idx].dst_image_h.clip_rect.w = scale_info[idx].dst.w;
    	g2d_info[idx].dst_image_h.clip_rect.h = scale_info[idx].dst.h;

    	g2d_info[idx].dst_image_h.align[0] = 0;
    	g2d_info[idx].dst_image_h.align[1] = 0;
    	g2d_info[idx].dst_image_h.align[2] = 0;

    	g2d_info[idx].src_image_h.laddr[0] = scale_info[idx].src.addr;
    	g2d_info[idx].dst_image_h.laddr[0] = scale_info[idx].dst.addr;

    	fb_width = g2d_info[idx].src_image_h.width;
    	fb_height = g2d_info[idx].src_image_h.height;

    	if((g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV420UVC_U1V1U0V0) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV420_PLANAR) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV420UVC_V1U1V0U0)) {

    		g2d_info[idx].src_image_h.laddr[1] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].src_image_h.laddr[2] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height*5/4);
    	}

    	if((g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV422UVC_V1U1V0U0) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV422_PLANAR) ||
    		(g2d_info[idx].src_image_h.format == G2D_FORMAT_YUV422UVC_V1U1V0U0)) {

    		g2d_info[idx].src_image_h.laddr[1] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].src_image_h.laddr[2] = (int)(g2d_info[idx].src_image_h.laddr[0] + fb_width*fb_height*3/2);
    	}

    	fb_width = g2d_info[idx].dst_image_h.width;
    	fb_height = g2d_info[idx].dst_image_h.height;
    	if((g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV420UVC_U1V1U0V0) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV420_PLANAR) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV420UVC_V1U1V0U0)) {

    		g2d_info[idx].dst_image_h.laddr[1] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].dst_image_h.laddr[2] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height*5/4);
    	}

    	if((g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV422UVC_V1U1V0U0) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV422UVC_U1V1U0V0) ||
    	   (g2d_info[idx].dst_image_h.format == G2D_FORMAT_YUV422_PLANAR)) {

    		g2d_info[idx].dst_image_h.laddr[1] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height);
    		g2d_info[idx].dst_image_h.laddr[2] = (int)(g2d_info[idx].dst_image_h.laddr[0] + fb_width*fb_height*3/2);

    	}
    	g2d_info[idx].src_image_h.use_phy_addr = 1;
    	g2d_info[idx].dst_image_h.use_phy_addr = 1;
//		printf("idx: %d dst use phy:%d\n",idx,g2d_info[idx].src_image_h.use_phy_addr);
   }
   return 0;
}
#endif
int G2dProcess(Aw_G2d* self)
{
    unsigned long arg[6] = {0};

    arg[0] = (unsigned long )(self->g2dInfo);
    arg[1] = self->g2dInfoNum;
    if (ioctl(self->g2dFd, G2D_CMD_MIXER_TASK, (arg)) < 0)
    {
        //printf("[%d][%s][%s]G2D_CMD_MIXER_TASK failure!\n", __LINE__,
         //      __FILE__, __FUNCTION__);
        return -1;
    }

    printf("[%d][%s][%s]G2D_CMD_MIXER_TASK successfull!\n", __LINE__,
           __FILE__, __FUNCTION__);


    return 0;
}

int scaler(const char **argv,Aw_G2d* aw_g2d)
{
   // Aw_G2d* aw_g2d;
    Aw_G2dParam info;
    int i,ii;
  //  FILE* rfp=NULL,*wfp = NULL;
  //  char *rbuf = NULL,*wbuf=NULL;
    int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    int crop_x,crop_y,crop_w,crop_h;
    int size;
    int ret = -1;
    int fb_width, fb_height;

    unsigned long srcphy_addr1 = 0, srcphy_addr2 = 0,srcphy_addr3 = 0;
    unsigned long dstphy_addr1 = 0, dstphy_addr2 = 0,dstphy_addr3 = 0;
    unsigned long rbuf_y=0;
        char *wbuf = NULL;
        char *rbuf = NULL;
    unsigned long dstbuf1=0,dstbuf2=0,dstbuf3=0;
    FILE* rfp=NULL,*wfp = NULL;
    FILE* rfp1=NULL,*wfp1 = NULL,*wfp2 = NULL;
    unsigned long rbuf_u=0,rbuf_v=0;
    unsigned long arg[6] = {0};

    int size_scaler;
    struct mixer_para info2;

    printf("-----------scaler-----------\n");
#if 1
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);

  //  crop_x = atoi(argv[7]);
   // crop_y = atoi(argv[8]);
   // crop_w = atoi(argv[9]);
   // crop_h = atoi(argv[10]);


    crop_x = 0;
    crop_y = 0;
    crop_w = srcw;
    crop_h = srch;

  ////////////////////////////////////////////////////////////

      rfp =fopen("/mnt/E/y.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    size = srcw*srch;
    srcphy_addr1=dma_coherent_alloc(&rbuf_y,size);
    if(srcphy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)rbuf_y,0,size);
    fread((void *)rbuf_y,1,size,rfp);


    rfp1 =fopen("/mnt/E/uv.dat", "r+");
    if(rfp1==NULL)
    {
        printf("\n[err]rfp1==NULL\n");
        ret = -1;
        goto __err;
    }
    //size = srcw*srch*4;
    srcphy_addr2=dma_coherent_alloc(&rbuf_u,size/4);
    if(srcphy_addr2 == 0)
    {
        ret = -1;
        goto __err;

    }

    srcphy_addr3=dma_coherent_alloc(&rbuf_v,size/4);
    if(srcphy_addr3 == 0)
    {
        ret = -1;
        goto __err;

    }

    memset((void *)rbuf_u,0,size/4);
    memset((void *)rbuf_v,0,size/4);
    fread((void *)rbuf_v,1,size/4,rfp1);
    fread((void *)rbuf_u,1,size/4,rfp1);

    size_scaler = dstw*dsth;

    dstphy_addr1=dma_coherent_alloc(&dstbuf1,size_scaler);
    if(dstphy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf1,0,size_scaler);

    dstphy_addr2=dma_coherent_alloc(&dstbuf2,size_scaler/4);
    if(dstphy_addr2 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf2,0,size_scaler/4);

    dstphy_addr3=dma_coherent_alloc(&dstbuf3,size_scaler/4);
    if(dstphy_addr3 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf3,0,size_scaler/4);
    printf("----444444-------\n");


////////////////////////////////////////////////////////////

#if 0
//////////////////////////////////////////////////////////////////
    rfp =fopen("/mnt/E/11.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    size = srcw*srch*4;
   // rbuf = malloc(size);
    srcphy_addr1=dma_coherent_alloc(&rbuf,size);
    if(rbuf == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)rbuf,0,size);
    fread((void *)rbuf,1,size,rfp);

   // wbuf = malloc(size);
   size_scaler = dstw*dsth*4;
    dstphy_addr1=dma_coherent_alloc(&dstbuf1,size_scaler);
    if(dstphy_addr1 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf1,0,size_scaler);
  ////////////////////////////////////////////////////////////////////////////
 #endif

    #if 0
    memset(&info,0,sizeof(Aw_G2dParam));
    info.scaleinfo = (ScaleInfo*)malloc(sizeof(*(info.scaleinfo)));
    if(info.scaleinfo == NULL)
    {
        ret = -1;
        goto __err;

    }
    info.g2dTotalNum = 1;
    for(int i=0; i<1; i++)
    {
        //printf("cut x:%d y:%d w:%d h:%d\n",x1,y1,cutw,cuth);
        info.scaleinfo[i].use_phy_addr = 1;
        info.scaleinfo[i].src.addr = srcphy_addr1;//__va_to_pa((uint32_t)rbuf);;
        info.scaleinfo[i].src.w  = srcw;
        info.scaleinfo[i].src.h  = srch;
        info.scaleinfo[i].src.format = srcformat;
        info.scaleinfo[i].dst.addr  = dstphy_addr1;//__va_to_pa((uint32_t)wbuf);;
        info.scaleinfo[i].dst.w    = dstw;
        info.scaleinfo[i].dst.h    = dsth;
        info.scaleinfo[i].dst.format = dstformat;

        info.scaleinfo[i].crop_info.x = crop_x;
        info.scaleinfo[i].crop_info.y = crop_y;
        info.scaleinfo[i].crop_info.width = crop_w;
        info.scaleinfo[i].crop_info.height = crop_h;

    }
    #endif
   // ret = G2dInit(aw_g2d,info);
   // if(ret != 0)
  //  {
      //  printf("G2dInit fail\n");
       // goto __err;
  //  }

    memset(&info2,0,sizeof(struct mixer_para));

           info2.flag_h = G2D_BLT_NONE_H;
           info2.op_flag = OP_BITBLT;

   info2.src_image_h.bbuff =1;
    info2.dst_image_h.bbuff =1;
           info2.src_image_h.format = srcformat;
           info2.dst_image_h.format = dstformat;
           info2.src_image_h.width  = srcw;
           info2.src_image_h.height = srch;
           info2.src_image_h.mode = G2D_GLOBAL_ALPHA;// G2D_PIXEL_ALPHA;


           info2.src_image_h.clip_rect.x = 0;
           info2.src_image_h.clip_rect.y = 0;
           info2.src_image_h.clip_rect.w = srcw;
           info2.src_image_h.clip_rect.h = srch;
           info2.src_image_h.align[0] = 0;
           info2.src_image_h.align[1] = 0;
           info2.src_image_h.align[2] = 0;

           info2.dst_image_h.width  = dstw;
           info2.dst_image_h.height = dsth;
           info2.dst_image_h.mode = G2D_GLOBAL_ALPHA; //G2D_PIXEL_ALPHA;

           info2.dst_image_h.clip_rect.x = 0;
           info2.dst_image_h.clip_rect.y = 0;
           info2.dst_image_h.clip_rect.w = dstw;
           info2.dst_image_h.clip_rect.h = dsth;

           info2.dst_image_h.align[0] = 0;
           info2.dst_image_h.align[1] = 0;
           info2.dst_image_h.align[2] = 0;

           info2.src_image_h.laddr[0] = srcphy_addr1;
           info2.src_image_h.laddr[1] = srcphy_addr2;
           info2.src_image_h.laddr[2] = srcphy_addr3;

           info2.dst_image_h.laddr[0] = dstphy_addr1;
           info2.dst_image_h.laddr[1] = dstphy_addr2;
           info2.dst_image_h.laddr[2] = dstphy_addr3;


           info2.src_image_h.use_phy_addr = 1;
           info2.dst_image_h.use_phy_addr = 1;
   //      printf("idx: %d dst use phy:%d\n",idx,g2d_info[idx].src_image_h.use_phy_addr);

       aw_g2d->g2dInfoNum = 1;


       printf("----4444------\n");

        arg[0] = (unsigned long )(&info2);
    arg[1] = 1;
       ret = ioctl(aw_g2d->g2dFd, G2D_CMD_MIXER_TASK, (arg));
   // ret = G2dProcess(aw_g2d);
    if(ret != 0)
    {
        printf("G2dProcess fail\n");
        goto __err;
    }
       printf("-----5555-------\n");
    wfp =fopen("/mnt/E/scaler_y.dat", "wb");
    if(wfp==NULL)
    {
        printf("\n[err]wfp==NULL\n");
        ret = -1;
        goto __err;
    }
//	hal_dcache_invalidate(dstbuf1, size_scaler);
//	hal_dcache_invalidate(dstbuf2, size_scaler/4);
	//hal_dcache_invalidate(dstbuf3, size_scaler/4);


    wbuf = malloc(size_scaler);
    memcpy(wbuf,(void *)dstbuf1,size_scaler);
    printf("-----2-------\n");
    printf("[wbuf]%x,%x,%x,%x\n",wbuf[0],wbuf[1],wbuf[2],wbuf[3]);
    fwrite((void *)wbuf,1,size_scaler,wfp);
    free(wbuf);

    wfp1 =fopen("/mnt/E/scaler_u.dat", "wb");
       if(wfp1==NULL)
       {
           printf("\n[err]wfp1==NULL\n");
           ret = -1;
           goto __err;
       }

       wbuf = malloc(size_scaler/4);
       memcpy(wbuf,(void *)dstbuf2,size_scaler/4);
       printf("--1---2-------\n");
       fwrite((void *)wbuf,1,size_scaler/4,wfp1);

       free(wbuf);


       wfp2 =fopen("/mnt/E/scaler_v.dat", "wb");
       if(wfp2==NULL)
       {
           printf("\n[err]wfp2==NULL\n");
           ret = -1;
           goto __err;
       }
       wbuf = malloc(size_scaler/4);
       memcpy(wbuf,(void *)dstbuf3,size_scaler/4);
       printf("--2---2-------\n");
       fwrite((void *)wbuf,1,size_scaler/4,wfp2);

       free(wbuf);


    printf("-----33-------\n");

#endif
  printf("[YG] scaler success!,ret:%d\n",ret);
__err:

    if(rbuf_y)
    {
        dma_coherent_free((unsigned long)rbuf_y);
        rbuf_y = NULL;
    }
    if(rbuf_u)
    {
        dma_coherent_free((unsigned long)rbuf_u);
        rbuf_u = NULL;
    }
        if(rbuf_v)
    {
        dma_coherent_free((unsigned long)rbuf_v);
        rbuf_v = NULL;
    }
    if(dstbuf1)
    {
        dma_coherent_free((unsigned long)dstbuf1);
        dstbuf1 = NULL;
    }
    if(dstbuf2)
    {
        dma_coherent_free((unsigned long)dstbuf2);
        dstbuf2 = NULL;
    }

    if(dstbuf3)
    {
        dma_coherent_free((unsigned long)dstbuf3);
        dstbuf3= NULL;
    }
    if(rfp)
    {
        fclose(rfp);
    }
    if(rfp1)
    {
        fclose(rfp1);
    }
    if(wfp)
    {
        fclose(wfp);
    }

        if(wfp1)
    {
        fclose(wfp1);
    }
            if(wfp2)
    {
        fclose(wfp2);
    }


    printf("[YG] scaler end!,ret:%d\n",ret);
    return ret;

}

int rotate_argb(const char **argv,Aw_G2d* aw_g2d)
{
    g2d_blt_h   blit;
    int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    int crop_x,crop_y,crop_w,crop_h;
    int size;
    int ret = -1;
    int nRotation = 0;
    unsigned long srcphy_addr1 = 0, srcphy_addr2 = 0;
    unsigned long dstphy_addr1 = 0, dstphy_addr2 = 0;
    unsigned long rbuf;
        char *wbuf;
    unsigned long dstbuf1,dstbuf2;
    FILE* rfp=NULL,*wfp = NULL;

    printf("-----------rotate start-------------\n");
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);

  //  crop_x = atoi(argv[7]);
  //  crop_y = atoi(argv[8]);
  //  crop_w = atoi(argv[9]);
  //  crop_h = atoi(argv[10]);
    nRotation = atoi(argv[7]);


    printf("----nRotation:%d-----------\n",nRotation);

    memset(&blit, 0, sizeof(g2d_blt_h));
    switch(nRotation)
    {
        case 90:
            blit.flag_h = G2D_ROT_90;
            break;
        case 180:
            blit.flag_h = G2D_ROT_180;
            break;
        case 270:
            blit.flag_h = G2D_ROT_270;
            break;
       case 200:
            blit.flag_h = G2D_ROT_H;
            break;
       case 300:
            blit.flag_h = G2D_ROT_V;
            break;
        default:
            printf("fatal error! rotation[%d] is invalid!\n", nRotation);
            blit.flag_h = G2D_BLT_NONE_H;
            break;
    }
  ////////////////////////////////////////////////////////////

      rfp =fopen("/mnt/E/11.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    size = srcw*srch*4;
    srcphy_addr1=dma_coherent_alloc(&rbuf,size);
    if(srcphy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)rbuf,0,size);
    fread((void *)rbuf,1,size,rfp);

    dstphy_addr1=dma_coherent_alloc(&dstbuf1,size);
    if(dstphy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf1,0,size);


////////////////////////////////////////////////////////////

    blit.src_image_h.bbuff = 1;
    blit.src_image_h.use_phy_addr = 1;
    blit.src_image_h.color = 0xff;
    blit.src_image_h.format = srcformat;//g2d_fmt_enh
    blit.src_image_h.laddr[0] = srcphy_addr1;//pSrc->VFrame.mPhyAddr[0];
    blit.src_image_h.laddr[1] = srcphy_addr2;//pSrc->VFrame.mPhyAddr[1];
    blit.src_image_h.width = srcw;
    blit.src_image_h.height = srch;
    blit.src_image_h.align[0] = 0;
    blit.src_image_h.align[1] = 0;
    blit.src_image_h.align[2] = 0;
    blit.src_image_h.clip_rect.x = 0;
    blit.src_image_h.clip_rect.y = 0;
    blit.src_image_h.clip_rect.w = srcw;
    blit.src_image_h.clip_rect.h = srch;
    blit.src_image_h.gamut = G2D_BT709;
    blit.src_image_h.bpremul = 0;
    blit.src_image_h.alpha = 0xff;
    blit.src_image_h.mode = G2D_GLOBAL_ALPHA;

    blit.dst_image_h.bbuff = 1;
    blit.dst_image_h.use_phy_addr = 1;
    blit.dst_image_h.color = 0xff;
    blit.dst_image_h.format = dstformat;
    blit.dst_image_h.laddr[0] = dstphy_addr1;//pDes->VFrame.mPhyAddr[0];
    blit.dst_image_h.laddr[1] = dstphy_addr2;//pDes->VFrame.mPhyAddr[1];
    blit.dst_image_h.width = dstw;
    blit.dst_image_h.height = dsth;
    blit.dst_image_h.align[0] = 0;
    blit.dst_image_h.align[1] = 0;
    blit.dst_image_h.align[2] = 0;
    blit.dst_image_h.clip_rect.x = 0;
    blit.dst_image_h.clip_rect.y = 0;
    blit.dst_image_h.clip_rect.w = dstw;
    blit.dst_image_h.clip_rect.h = dsth;
    blit.dst_image_h.gamut = G2D_BT709;
    blit.dst_image_h.bpremul = 0;
    blit.dst_image_h.alpha = 0xff;
    blit.dst_image_h.mode = G2D_GLOBAL_ALPHA;

    ret = ioctl(aw_g2d->g2dFd, G2D_CMD_BITBLT_H, (unsigned long)&blit);
    if(ret < 0)
    {
        printf("fatal error! bit-block(image) transfer failed\n");
       // system("cd /sys/class/sunxi_dump;echo 0x14A8000,0x14A8100 > dump;cat dump");
        ret = -1;
    }
/////////////////////////////////////////////
    printf("-----1-------\n");
    wfp =fopen("/mnt/E/rotate.dat", "wb");
    if(wfp==NULL)
    {
        printf("\n[err]wfp==NULL\n");
        ret = -1;
        goto __err;
    }

    wbuf = malloc(size);
    memcpy(wbuf,(void *)dstbuf1,size);
    printf("-----2-------\n");
    fwrite((void *)wbuf,1,size,wfp);
    free(wbuf);
     printf("-----3-------\n");
/////////////////////////////////////////
__err:
    if(rbuf)
    {
        dma_coherent_free((unsigned long)rbuf);
        rbuf = NULL;
    }
    if(dstbuf1)
    {
        dma_coherent_free((unsigned long)dstbuf1);
        dstbuf1 = NULL;
    }
    if(rfp)
    {
        fclose(rfp);
    }
    if(wfp)
    {
        fclose(wfp);
    }
    printf("fillrect_1 end ! ret : %d\n",ret);
    return ret;

}

int rotate_yuv(const char **argv,Aw_G2d* aw_g2d)
{
    g2d_blt_h   blit;
    int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    int crop_x,crop_y,crop_w,crop_h;
    int size;
    int ret = -1;
    int nRotation = 0;
    unsigned long srcphy_addr1 = 0, srcphy_addr2 = 0,srcphy_addr3 = 0;
    unsigned long dstphy_addr1 = 0, dstphy_addr2 = 0,dstphy_addr3 = 0;
    unsigned long rbuf_y;
        char *wbuf;
    unsigned long dstbuf1,dstbuf2,dstbuf3;
    FILE* rfp=NULL,*wfp = NULL;
    FILE* rfp1=NULL,*wfp1 = NULL,*wfp2 = NULL;
    unsigned long rbuf_u,rbuf_v;

    printf("-----------rotate start-------------\n");
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);

  //  crop_x = atoi(argv[7]);
  //  crop_y = atoi(argv[8]);
  //  crop_w = atoi(argv[9]);
  //  crop_h = atoi(argv[10]);
    nRotation = atoi(argv[7]);


    printf("----nRotation:%d-----------\n",nRotation);

    memset(&blit, 0, sizeof(g2d_blt_h));
    switch(nRotation)
    {
        case 90:
            blit.flag_h = G2D_ROT_90;
            break;
        case 180:
            blit.flag_h = G2D_ROT_180;
            break;
        case 270:
            blit.flag_h = G2D_ROT_270;
            break;
        default:
            printf("fatal error! rotation[%d] is invalid!\n", nRotation);
            blit.flag_h = G2D_BLT_NONE_H;
            break;
    }
  ////////////////////////////////////////////////////////////

      rfp =fopen("/mnt/E/y.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    size = srcw*srch;
    srcphy_addr1=dma_coherent_alloc(&rbuf_y,size);
    if(srcphy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)rbuf_y,0,size);
    fread((void *)rbuf_y,1,size,rfp);


    rfp1 =fopen("/mnt/E/uv.dat", "r+");
    if(rfp1==NULL)
    {
        printf("\n[err]rfp1==NULL\n");
        ret = -1;
        goto __err;
    }
    //size = srcw*srch*4;
    srcphy_addr2=dma_coherent_alloc(&rbuf_u,size/4);
    if(srcphy_addr2 == 0)
    {
        ret = -1;
        goto __err;

    }

    srcphy_addr3=dma_coherent_alloc(&rbuf_v,size/4);
    if(srcphy_addr3 == 0)
    {
        ret = -1;
        goto __err;

    }

    memset((void *)rbuf_u,0,size/4);
    memset((void *)rbuf_v,0,size/4);
    fread((void *)rbuf_u,1,size/4,rfp1);
    fread((void *)rbuf_v,1,size/4,rfp1);

    dstphy_addr1=dma_coherent_alloc(&dstbuf1,size);
    if(dstphy_addr1 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf1,0,size);

    dstphy_addr2=dma_coherent_alloc(&dstbuf2,size/4);
    if(dstphy_addr2 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf2,0,size/4);

    dstphy_addr3=dma_coherent_alloc(&dstbuf3,size/4);
    if(dstphy_addr3 == 0)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)dstbuf3,0,size/4);
    printf("----444444-------\n");


////////////////////////////////////////////////////////////

    blit.src_image_h.bbuff = 1;
    blit.src_image_h.use_phy_addr = 1;
    blit.src_image_h.color = 0xff;
    blit.src_image_h.format = srcformat;//g2d_fmt_enh
    blit.src_image_h.laddr[0] = srcphy_addr1;//pSrc->VFrame.mPhyAddr[0];
    blit.src_image_h.laddr[1] = srcphy_addr2;//pSrc->VFrame.mPhyAddr[1];
    blit.src_image_h.laddr[2] = srcphy_addr3;//pSrc->VFrame.mPhyAddr[1];
    blit.src_image_h.width = srcw;
    blit.src_image_h.height = srch;
    blit.src_image_h.align[0] = 0;
    blit.src_image_h.align[1] = 0;
    blit.src_image_h.align[2] = 0;
    blit.src_image_h.clip_rect.x = 0;
    blit.src_image_h.clip_rect.y = 0;
    blit.src_image_h.clip_rect.w = srcw;
    blit.src_image_h.clip_rect.h = srch;
    blit.src_image_h.gamut = G2D_BT709;
    blit.src_image_h.bpremul = 0;
    blit.src_image_h.alpha = 0xff;
    blit.src_image_h.mode = G2D_GLOBAL_ALPHA;

    blit.dst_image_h.bbuff = 1;
    blit.dst_image_h.use_phy_addr = 1;
    blit.dst_image_h.color = 0xff;
    blit.dst_image_h.format = dstformat;
    blit.dst_image_h.laddr[0] = dstphy_addr1;//pDes->VFrame.mPhyAddr[0];
    blit.dst_image_h.laddr[1] = dstphy_addr2;//pDes->VFrame.mPhyAddr[1];
    blit.dst_image_h.laddr[2] = dstphy_addr3;
    blit.dst_image_h.width = dstw;
    blit.dst_image_h.height = dsth;
    blit.dst_image_h.align[0] = 0;
    blit.dst_image_h.align[1] = 0;
    blit.dst_image_h.align[2] = 0;
    blit.dst_image_h.clip_rect.x = 0;
    blit.dst_image_h.clip_rect.y = 0;
    blit.dst_image_h.clip_rect.w = dstw;
    blit.dst_image_h.clip_rect.h = dsth;
    blit.dst_image_h.gamut = G2D_BT709;
    blit.dst_image_h.bpremul = 0;
    blit.dst_image_h.alpha = 0xff;
    blit.dst_image_h.mode = G2D_GLOBAL_ALPHA;
    printf("-----6666-------\n");

    ret = ioctl(aw_g2d->g2dFd, G2D_CMD_BITBLT_H, (unsigned long)&blit);
    if(ret < 0)
    {
        printf("fatal error! bit-block(image) transfer failed\n");
       // system("cd /sys/class/sunxi_dump;echo 0x14A8000,0x14A8100 > dump;cat dump");
        ret = -1;
    }
/////////////////////////////////////////////
    printf("-----1-------\n");
    wfp =fopen("/mnt/E/rotate_y.dat", "wb");
    if(wfp==NULL)
    {
        printf("\n[err]wfp==NULL\n");
        ret = -1;
        goto __err;
    }

    wbuf = malloc(size);
    memcpy(wbuf,(void *)dstbuf1,size);
    printf("-----2-------\n");
    fwrite((void *)wbuf,1,size,wfp);
    free(wbuf);

    wfp1 =fopen("/mnt/E/rotate_u.dat", "wb");
    if(wfp1==NULL)
    {
        printf("\n[err]wfp1==NULL\n");
        ret = -1;
        goto __err;
    }

    wbuf = malloc(size/4);
    memcpy(wbuf,(void *)dstbuf2,size/4);
    printf("--1---2-------\n");
    fwrite((void *)wbuf,1,size/4,wfp1);

    free(wbuf);


    wfp2 =fopen("/mnt/E/rotate_v.dat", "wb");
    if(wfp2==NULL)
    {
        printf("\n[err]wfp2==NULL\n");
        ret = -1;
        goto __err;
    }
    wbuf = malloc(size/4);
    memcpy(wbuf,(void *)dstbuf3,size/4);
    printf("--2---2-------\n");
    fwrite((void *)wbuf,1,size/4,wfp2);

    free(wbuf);
     printf("--3---3-------\n");
/////////////////////////////////////////
__err:

    printf("-----555-------\n");

    if(rbuf_y)
    {
        dma_coherent_free((unsigned long)rbuf_y);
        rbuf_y = NULL;
    }
    if(rbuf_u)
    {
        dma_coherent_free((unsigned long)rbuf_u);
        rbuf_u = NULL;
    }
        if(rbuf_v)
    {
        dma_coherent_free((unsigned long)rbuf_v);
        rbuf_v = NULL;
    }
    if(dstbuf1)
    {
        dma_coherent_free((unsigned long)dstbuf1);
        dstbuf1 = NULL;
    }
    if(dstbuf2)
    {
        dma_coherent_free((unsigned long)dstbuf2);
        dstbuf2 = NULL;
    }

    if(dstbuf3)
    {
        dma_coherent_free((unsigned long)dstbuf3);
        dstbuf3= NULL;
    }
    if(rfp)
    {
        fclose(rfp);
    }
    if(rfp1)
    {
        fclose(rfp1);
    }
    if(wfp)
    {
        fclose(wfp);
    }

        if(wfp1)
    {
        fclose(wfp1);
    }
            if(wfp2)
    {
        fclose(wfp2);
    }
    printf("fillrect_1 end ! ret : %d\n",ret);
    return ret;

}



/*Dy¡Áa¨°?o¨®¨®D?¨¬¨¬?*/
int rotate_yuv_new(int argc, const char **argv)
{
    g2d_blt_h   blit;
    int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    int crop_x,crop_y,crop_w,crop_h;
    int size;
    int ret = -1;
    int nRotation = 0;
    unsigned long srcphy_addr1 = 0, srcphy_addr2 = 0,srcphy_addr3 = 0;
    unsigned long dstphy_addr1 = 0, dstphy_addr2 = 0,dstphy_addr3 = 0;
    unsigned long rbuf_y;
        char *wbuf;
    unsigned long dstbuf1,dstbuf2,dstbuf3;
    FILE* rfp=NULL,*wfp = NULL;
    FILE* rfp1=NULL,*wfp1 = NULL,*wfp2 = NULL;
    unsigned long rbuf_u,rbuf_v;
    int src_w=0,src_h =0;
    int ii;

	unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;

    printf("-----------rotate_yuv_new start-------------\n");
    printf("argv(%d):\n",argc);
    for(ii=0;ii<argc;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");
    rt_device_t dev = rt_device_find("g2d");

	if (!dev){
		printf("can not find device\n");
		ret = -1;
		goto __err;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto __err;
	}

    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);

  //  crop_x = atoi(argv[7]);
  //  crop_y = atoi(argv[8]);
  //  crop_w = atoi(argv[9]);
  //  crop_h = atoi(argv[10]);
    nRotation = atoi(argv[7]);

    printf("----nRotation:%d-----------\n",nRotation);

    memset(&blit, 0, sizeof(g2d_blt_h));
    switch(nRotation)
    {
        case 90:
            blit.flag_h = G2D_ROT_90;
            break;
        case 180:
            blit.flag_h = G2D_ROT_180;
            break;
        case 270:
            blit.flag_h = G2D_ROT_270;
            break;
        case 200:
            blit.flag_h = G2D_ROT_H;
            break;
         case 300:
            blit.flag_h = G2D_ROT_V;
            break;
        default:
            printf("fatal error! rotation[%d] is invalid!\n", nRotation);
            blit.flag_h = G2D_BLT_NONE_H;
            break;
    }


  ////////////////////////////////////////////////////////////

      rfp =fopen("/mnt/E/y.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    size = srcw*srch;
     buf1 = hal_malloc(size);
    //srcphy_addr1=dma_coherent_alloc(&rbuf_y,size);
    if(buf1 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf1,0,size);
    fread((void *)buf1,1,size,rfp);

    rfp1 =fopen("/mnt/E/uv.dat", "r+");
    if(rfp1==NULL)
    {
        printf("\n[err]rfp1==NULL\n");
        ret = -1;
        goto __err;
    }
    //size = srcw*srch*4;
  //  srcphy_addr2=dma_coherent_alloc(&rbuf_u,size/4);
  buf2 = hal_malloc(size/4);
    if(buf2 == NULL)
    {
        ret = -1;
        goto __err;

    }

   // srcphy_addr3=dma_coherent_alloc(&rbuf_v,size/4);
     buf3 = hal_malloc(size/4);
    if(buf3 == NULL)
    {
        ret = -1;
        goto __err;

    }

    memset((void *)buf2,0,size/4);
    memset((void *)buf3,0,size/4);
    fread((void *)buf2,1,size/4,rfp1);
    fread((void *)buf3,1,size/4,rfp1);

    hal_dcache_clean(buf1, size);
    hal_dcache_clean(buf2, size/4);
    hal_dcache_clean(buf3, size/4);


  //  dstphy_addr1=dma_coherent_alloc(&dstbuf1,size);
    buf4 = hal_malloc(size);
    if(buf4 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf4,0,size);

    buf5 = hal_malloc(size/4);
    if(buf5 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf5,0,size/4);

    buf6 = hal_malloc(size/4);
    if(buf6 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf6,0,size/4);

    printf("----444444-------\n");


////////////////////////////////////////////////////////////

    blit.src_image_h.bbuff = 1;
    blit.src_image_h.use_phy_addr = 1;
    blit.src_image_h.color = 0xff;
    blit.src_image_h.format = srcformat;//g2d_fmt_enh
    blit.src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
    blit.src_image_h.laddr[1] = __va_to_pa((uint32_t)buf2);
    blit.src_image_h.laddr[2] = __va_to_pa((uint32_t)buf3);
    blit.src_image_h.width = srcw;
    blit.src_image_h.height = srch;
    blit.src_image_h.align[0] = 0;
    blit.src_image_h.align[1] = 0;
    blit.src_image_h.align[2] = 0;
    blit.src_image_h.clip_rect.x = 0;
    blit.src_image_h.clip_rect.y = 0;
    blit.src_image_h.clip_rect.w = srcw;
    blit.src_image_h.clip_rect.h = srch;
    blit.src_image_h.gamut = G2D_BT709;
    blit.src_image_h.bpremul = 0;
    blit.src_image_h.alpha = 0xff;
    blit.src_image_h.mode = G2D_GLOBAL_ALPHA;

    blit.dst_image_h.bbuff = 1;
    blit.dst_image_h.use_phy_addr = 1;
    blit.dst_image_h.color = 0xff;
    blit.dst_image_h.format = dstformat;
    blit.dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);
    blit.dst_image_h.laddr[1] = __va_to_pa((uint32_t)buf5);
    blit.dst_image_h.laddr[2] = __va_to_pa((uint32_t)buf6);
    blit.dst_image_h.width = dstw;
    blit.dst_image_h.height = dsth;
    blit.dst_image_h.align[0] = 0;
    blit.dst_image_h.align[1] = 0;
    blit.dst_image_h.align[2] = 0;
    blit.dst_image_h.clip_rect.x = 0;
    blit.dst_image_h.clip_rect.y = 0;
    blit.dst_image_h.clip_rect.w = dstw;
    blit.dst_image_h.clip_rect.h = dsth;
    blit.dst_image_h.gamut = G2D_BT709;
    blit.dst_image_h.bpremul = 0;
    blit.dst_image_h.alpha = 0xff;
    blit.dst_image_h.mode = G2D_GLOBAL_ALPHA;
    printf("-----6666-------\n");

    hal_dcache_clean(buf4, size);
    hal_dcache_clean(buf5, size/4);
    hal_dcache_clean(buf6, size/4);

    hal_dcache_invalidate(buf4, size);
    hal_dcache_invalidate(buf5, size/4);
    hal_dcache_invalidate(buf6, size/4);

	ret = dev->control(dev, G2D_CMD_BITBLT_H, (unsigned long)&blit);
	if (ret){
		printf("g2d G2D_CMD_BITBLT_H fail\n");
		ret = -1;
		goto __err;
		}
	else {
		printf("G2D_CMD_BITBLT_H ok\n");

	}


    hal_dcache_invalidate(buf4, size);
    hal_dcache_invalidate(buf5, size/4);
    hal_dcache_invalidate(buf6, size/4);
/////////////////////////////////////////////
    printf("-----1-------\n");
    wfp =fopen("/mnt/E/rotate_y.dat", "wb");
    if(wfp==NULL)
    {
        printf("\n[err]wfp==NULL\n");
        ret = -1;
        goto __err;
    }

    printf("-----2-------\n");
    fwrite((void *)buf4,1,size,wfp);

    wfp1 =fopen("/mnt/E/rotate_v.dat", "wb");
    if(wfp1==NULL)
    {
        printf("\n[err]wfp1==NULL\n");
        ret = -1;
        goto __err;
    }

    printf("--1---2-------\n");
    fwrite((void *)buf5,1,size/4,wfp1);



    wfp2 =fopen("/mnt/E/rotate_u.dat", "wb");
    if(wfp2==NULL)
    {
        printf("\n[err]wfp2==NULL\n");
        ret = -1;
        goto __err;
    }

    printf("--2---2-------\n");
    fwrite((void *)buf6,1,size/4,wfp2);

     printf("--3---3-------\n");
    hal_free(buf1);
    hal_free(buf2);
	hal_free(buf3);
	hal_free(buf4);
    hal_free(buf5);
    hal_free(buf6);
    rt_device_close(dev);
/////////////////////////////////////////
__err:

    printf("-----555-------\n");

    if(rfp)
    {
        fclose(rfp);
    }
    if(rfp1)
    {
        fclose(rfp1);
    }
    if(wfp)
    {
        fclose(wfp);
    }

        if(wfp1)
    {
        fclose(wfp1);
    }
            if(wfp2)
    {
        fclose(wfp2);
    }
    printf("fillrect_1 end ! ret : %d\n",ret);
    return ret;

}

int rotate_argb_new(int argc, const char **argv)
{
    g2d_blt_h   blit;
    int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    int crop_x,crop_y,crop_w,crop_h;
    int size;
    int ret = -1;
    int nRotation = 0;
    unsigned long srcphy_addr1 = 0, srcphy_addr2 = 0,srcphy_addr3 = 0;
    unsigned long dstphy_addr1 = 0, dstphy_addr2 = 0,dstphy_addr3 = 0;
    unsigned long rbuf_y;
        char *wbuf;
    unsigned long dstbuf1,dstbuf2,dstbuf3;
    FILE* rfp=NULL,*wfp = NULL;
    FILE* rfp1=NULL,*wfp1 = NULL,*wfp2 = NULL;
    unsigned long rbuf_u,rbuf_v;
    int src_w=0,src_h =0;
    int src_align=0,dst_align=0;
    u8 * ttt;

    int ii;
	unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;

    printf("-----------rotate_argb_new start-------------\n");
    printf("argv(%d):\n",argc);
    for(ii=0;ii<argc;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");
    rt_device_t dev = rt_device_find("g2d");

	if (!dev){
		printf("can not find device\n");
		ret = -1;
		goto __err;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto __err;
	}


    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    src_align = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dst_align = atoi(argv[6]);


  if(srcw % 8 != 0)
  {
      src_w = (srcw / 8 + 1) * 8;
  }
  else
  {
      src_w = srcw;
  }

  if(srch % 8 != 0)
  {
      src_h = (srch / 8 + 1) * 8;
  }
  else
  {
      src_h = srch;
  }
 // src_h = srch;

  //  crop_x = atoi(argv[7]);
  //  crop_y = atoi(argv[8]);
  //  crop_w = atoi(argv[9]);
  //  crop_h = atoi(argv[10]);
    nRotation = atoi(argv[7]);

    printf("----nRotation:%d-,%d,%d----------\n",nRotation,src_w,src_h);

    memset(&blit, 0, sizeof(g2d_blt_h));
    switch(nRotation)
    {
        case 0:
            blit.flag_h = G2D_ROT_0;
            break;
        case 90:
            blit.flag_h = G2D_ROT_90;
            break;
        case 180:
            blit.flag_h = G2D_ROT_180;
            break;
        case 270:
            blit.flag_h = G2D_ROT_270;
            break;
        case 200:
            blit.flag_h = G2D_ROT_H;
            break;
         case 300:
            blit.flag_h = G2D_ROT_V;
            break;
        default:
            printf("fatal error! rotation[%d] is invalid!\n", nRotation);
            blit.flag_h = G2D_BLT_NONE_H;
            break;
    }


  ////////////////////////////////////////////////////////////

      rfp =fopen("/mnt/E/11.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    size = src_w*src_h*4;
     buf1 = hal_malloc(srcw*srch*4);
    //srcphy_addr1=dma_coherent_alloc(&rbuf_y,size);
    if(buf1 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf1,0,srcw*srch*4);
   fread((void *)buf1,1,srcw*srch*4,rfp);
#if 0
ttt = (u8 *)buf1;
for(ii=0;ii<srch;ii++)
{
    //fread((void *)(buf1+src_w*4*ii),1,srcw*4,rfp);
        fread((void *)ttt,333*4,1,rfp);
	ttt+=src_w*4;
}
#endif
    hal_dcache_clean(buf1, srcw*srch*4);
    hal_dcache_invalidate(buf1, srcw*srch*4);

  fclose(rfp);

  //  dstphy_addr1=dma_coherent_alloc(&dstbuf1,size);
    buf4 = hal_malloc(size);
    if(buf4 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf4,0,size);

    printf("----444444-------\n");


////////////////////////////////////////////////////////////

    blit.src_image_h.bbuff = 1;
    blit.src_image_h.use_phy_addr = 1;
    blit.src_image_h.color = 0xff;
    blit.src_image_h.format = 0;//g2d_fmt_enh
    blit.src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
    blit.src_image_h.laddr[1] = 0;//__va_to_pa((uint32_t)buf2);
    blit.src_image_h.laddr[2] = 0;//__va_to_pa((uint32_t)buf3);
    blit.src_image_h.width = srcw;
    blit.src_image_h.height = srcw;
    blit.src_image_h.align[0] = src_align;
    blit.src_image_h.align[1] = src_align;
    blit.src_image_h.align[2] = src_align;
    blit.src_image_h.clip_rect.x = 0;
    blit.src_image_h.clip_rect.y = 0;
    blit.src_image_h.clip_rect.w = srcw;
    blit.src_image_h.clip_rect.h = srch;
    blit.src_image_h.gamut = G2D_BT709;
    blit.src_image_h.bpremul = 0;
    blit.src_image_h.alpha = 0xff;
    blit.src_image_h.mode = G2D_GLOBAL_ALPHA;

    blit.dst_image_h.bbuff = 1;
    blit.dst_image_h.use_phy_addr = 1;
    blit.dst_image_h.color = 0xff;
    blit.dst_image_h.format = 0;
    blit.dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);
    blit.dst_image_h.laddr[1] = 0;//__va_to_pa((uint32_t)buf5);
    blit.dst_image_h.laddr[2] = 0;//__va_to_pa((uint32_t)buf6);
    blit.dst_image_h.width = dstw;
    blit.dst_image_h.height = dsth;
    blit.dst_image_h.align[0] = dst_align;
    blit.dst_image_h.align[1] = dst_align;
    blit.dst_image_h.align[2] = dst_align;
    blit.dst_image_h.clip_rect.x = 0;
    blit.dst_image_h.clip_rect.y = 0;
    blit.dst_image_h.clip_rect.w = dstw;
    blit.dst_image_h.clip_rect.h = dsth;
    blit.dst_image_h.gamut = G2D_BT709;
    blit.dst_image_h.bpremul = 0;
    blit.dst_image_h.alpha = 0xff;
    blit.dst_image_h.mode = G2D_GLOBAL_ALPHA;
    printf("-----6666-------\n");

    hal_dcache_clean(buf4, size);


    hal_dcache_invalidate(buf4, size);

	ret = dev->control(dev, G2D_CMD_BITBLT_H, (unsigned long)&blit);
	if (ret){
		printf("g2d G2D_CMD_BITBLT_H fail\n");
		ret = -1;
		goto __err;
		}
	else {
		printf("G2D_CMD_BITBLT_H ok\n");

	}

    hal_dcache_invalidate(buf4, size);

/////////////////////////////////////////////
    printf("-----1-------\n");
    wfp =fopen("/mnt/E/rotate_argb.dat", "wb");
    if(wfp==NULL)
    {
        printf("\n[err]wfp==NULL\n");
        ret = -1;
        goto __err;
    }

    printf("-----2-------\n");
    fwrite((void *)buf4,1,size,wfp);

     printf("--3---3-------\n");
    hal_free(buf1);

	hal_free(buf4);

    rt_device_close(dev);
/////////////////////////////////////////
__err:

    printf("-----555-------\n");

    if(rfp)
    {
     //   fclose(rfp);
    }
    if(rfp1)
    {
       // fclose(rfp1);
    }
    if(wfp)
    {
        fclose(wfp);
    }

        if(wfp1)
    {
        fclose(wfp1);
    }
            if(wfp2)
    {
       // fclose(wfp2);
    }
    printf("fillrect_1 end ! ret : %d\n",ret);
    return ret;

}

#if 0
int blend(const char **argv)
{
    int ret = -1;
    g2d_bld blend;
    int bld_cmd;
    g2d_image_enh src_image_h,dst_image_h;
        int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    unsigned long srcphy_addr1 = 0, srcphy_addr2 = 0,srcphy_addr3 = 0;
    unsigned long dstphy_addr1 = 0, dstphy_addr2 = 0,dstphy_addr3 = 0;
    unsigned long rbuf;
        char *wbuf;
    unsigned long dstbuf1,dstbuf2,dstbuf3;
    FILE* rfp=NULL,*wfp = NULL;
    FILE* rfp1=NULL,*wfp1 = NULL,*wfp2 = NULL;
    int size=0,dstsize=0;

	unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;
    printf("-----------blend start-------------\n");

    memset(&src_image_h,0,sizeof(g2d_image_enh));
    memset(&dst_image_h,0,sizeof(g2d_image_enh));
    memset(&blend,0,sizeof(g2d_bld));

    srcw = atoi(argv[1]);
   srch = atoi(argv[2]);
  srcformat = atoi(argv[3]);

   dstw = atoi(argv[4]);
  dsth = atoi(argv[5]);
   dstformat = atoi(argv[6]);


    rt_device_t dev = rt_device_find("g2d");

	if (!dev){
		printf("can not find device\n");
		ret = -1;
		goto __err;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto __err;
	}


    size = srcw*srch*4;
    dstsize = dstw*dsth*4;
    rfp =fopen("/mnt/E/11.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
     buf1 = hal_malloc(size);
    //srcphy_addr1=dma_coherent_alloc(&rbuf_y,size);
    if(buf1 == NULL)
    {
        ret = -1;
        goto __err;

    }
    memset((void *)buf1,0,size);
    fread((void *)buf1,1,size,rfp);
    fclose(rfp);

     buf4 = hal_malloc(dstsize);
    //srcphy_addr1=dma_coherent_alloc(&rbuf_y,size);
    if(buf4 == NULL)
    {
        ret = -1;
        goto __err;

    }

    rfp =fopen("/mnt/E/22.dat", "r+");
    if(rfp==NULL)
    {
        printf("\n[err]rfp==NULL\n");
        ret = -1;
        goto __err;
    }
    memset((void *)buf4,0,dstsize);
    fread((void *)buf4,1,dstsize,rfp);

    blend.bld_cmd = atoi(argv[7]);//4


    blend.src_image_h.bbuff = 1;
    blend.src_image_h.alpha = 111;//atoi(argv[2]);//(rand() % (0xff + 1));
    blend.src_image_h.color = 0xee8899;
    blend.src_image_h.format = srcformat;
    blend.src_image_h.mode = 1;
    blend.src_image_h.use_phy_addr = 1;
    blend.src_image_h.width = srcw;
    blend.src_image_h.height = srch;
    blend.src_image_h.clip_rect.x = 0;
    blend.src_image_h.clip_rect.y = 0;
    blend.src_image_h.clip_rect.w = srcw;
    blend.src_image_h.clip_rect.h = srch;
    blend.src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
    blend.src_image_h.laddr[1] = 0;
    blend.src_image_h.laddr[2] = 0;

    blend.dst_image_h.bbuff = 1;
    blend.dst_image_h.alpha = 111;//atoi(argv[3]);//(rand() % (0xff + 1));
    blend.dst_image_h.color = 0xee8899;
    blend.dst_image_h.format = dstformat;
    blend.dst_image_h.mode = 1;
    blend.dst_image_h.use_phy_addr = 1;
    blend.dst_image_h.width = dstw;
    blend.dst_image_h.height = dsth;
    blend.dst_image_h.clip_rect.x = 0;
    blend.dst_image_h.clip_rect.y = 0;
    blend.dst_image_h.clip_rect.w = dstw;
    blend.dst_image_h.clip_rect.h = dsth;
    blend.dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);
    blend.dst_image_h.laddr[1] = 0;
    blend.dst_image_h.laddr[2] = 0;
   // blend.src_image_h = src_image_h;
   // blend.dst_image_h = dst_image_h;

   printf("-alpha:%d,%d-------\n", blend.src_image_h.alpha, blend.dst_image_h.alpha);

     hal_dcache_clean(buf1, size);
    hal_dcache_invalidate(buf1, size);
    hal_dcache_clean(buf4, dstsize);
     hal_dcache_invalidate(buf4, dstsize);


    blend.ck_para.min_color = 0x11555555;
    blend.ck_para.max_color = 0xeeaaaaaa;
   // if(ioctl(aw_g2d->g2dFd , G2D_CMD_BLD_H ,(unsigned long)(&blend)) < 0)
   ret = dev->control(dev, G2D_CMD_BLD_H, (unsigned long)(&blend));
   if(ret<0)
    {
        printf("[%d][%s][%s]G2D_CMD_BLD_H failure!\n",
            __LINE__, __FILE__,__FUNCTION__);
    		 goto __err;
    }

        hal_dcache_invalidate(buf4, dstsize);

        wfp1 =fopen("/mnt/E/blend.dat", "wb");
       if(wfp1==NULL)
       {
           printf("\n[err]wfp1==NULL\n");
           ret = -1;
           goto __err;
       }

       printf("--1---2-------\n");
       fwrite((void *)buf4,1,dstsize,wfp1);
       free(buf1);
       free(buf4);

__err:

    if(rfp)
    {
        fclose(rfp);
    }
    if(wfp1)
    {
        fclose(wfp1);
    }
    printf("blend end ! ret : %d\n",ret);


}
#endif


int scaler_yuv2argb(int argc, const char **argv)
{
    int ret;
	unsigned long arg[6];
	unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;
	char *temp;
	FILE* file1;
	int fb_width, fb_height;

	g2d_blt_h info;
	struct mixer_para info2[2];
	g2d_blt_h blit_para;
	int srcw,srch,srcformat,dstw,dsth,dstformat;
	unsigned long i;
    int srcsize,dstsize;
    int src_w=0,src_h =0;
    int ii;
	printf("hello g2d_test\n");

        printf("-----------scaler_yuv2argb-----------\n");
    printf("argv(%d):\n",argc);
    for(ii=0;ii<argc;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);


	rt_device_t dev = rt_device_find("g2d");

	if (!dev){
		printf("can not find device\n");
		ret = -1;
		goto out;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto error;
	}

    if(srcw % 16 != 0)
    {
        src_w = (srcw / 16 + 1) * 16;
    }
    else
    {
        src_w = srcw;
    }
    if(srch % 16 != 0)
    {
        src_h = (srch / 16 + 1) * 16;
    }
    else
    {
        src_h=srch;

    }
     printf("src_w:%d,src_h:%d\n",src_w,src_h);
    srcsize = src_w*src_h;
    dstsize = dstw*dsth*4;
    buf1 = hal_malloc(srcsize);
    buf2 = hal_malloc(srcsize/2);
	//buf3 = hal_malloc(srcsize/4);

    buf4 = hal_malloc(dstsize);
   // buf5 = hal_malloc(dstsize/4);
  //  buf6 = hal_malloc(dstsize/4);
	memset(buf1, 0, srcsize);
    memset(buf2, 0, srcsize/2);
  //  memset(buf3, 0, srcsize/4);

	memset(buf4, 0, dstsize);
    //memset(buf5, 0, dstsize/4);
   // memset(buf6, 0, dstsize/4);

    memset(&info2, 0, 2*sizeof(struct mixer_para));
    	info2[0].src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
        info2[0].src_image_h.laddr[1] = __va_to_pa((uint32_t)buf2);
        info2[0].src_image_h.laddr[2] =0;// __va_to_pa((uint32_t)buf3);

	info2[0].dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);
    info2[0].dst_image_h.laddr[1] = 0;//__va_to_pa((uint32_t)buf5);
    info2[0].dst_image_h.laddr[2] =0;// __va_to_pa((uint32_t)buf6);

//	info2[0].src_image_h.laddr[0] = (int)(info2[0].src_image_h.laddr[0] );
//	info2[0].src_image_h.laddr[1] = (int)(info2[0].src_image_h.laddr[0] + srcsize);
	//info2[0].src_image_h.laddr[2] = (int)(info2[0].src_image_h.laddr[0] + srcsize*5/4);
	info2[0].src_image_h.use_phy_addr = 1;
	//info2[0].dst_image_h.laddr[0] = (int)(info2[0].dst_image_h.laddr[0] );
	//info2[0].dst_image_h.laddr[1] = (int)(info2[0].dst_image_h.laddr[0] + dstsize);
	//info2[0].dst_image_h.laddr[2] = (int)(info2[0].dst_image_h.laddr[0] + dstsize*5/4);

	info2[0].dst_image_h.use_phy_addr = 1;
	info2[0].flag_h = G2D_BLT_NONE_H;
	info2[0].op_flag = OP_BITBLT;
	info2[0].src_image_h.format = srcformat;
	info2[0].src_image_h.width = src_w;
	info2[0].src_image_h.height = src_h;

	info2[0].src_image_h.clip_rect.x = 0;
	info2[0].src_image_h.clip_rect.y = 0;
    	info2[0].src_image_h.clip_rect.w = srcw;
	info2[0].src_image_h.clip_rect.h = srch;


	info2[0].dst_image_h.format = dstformat;
	info2[0].dst_image_h.width = dstw;
	info2[0].dst_image_h.height = dsth;

	info2[0].dst_image_h.clip_rect.x = 0;
	info2[0].dst_image_h.clip_rect.y = 0;
	info2[0].dst_image_h.clip_rect.w = dstw;
	info2[0].dst_image_h.clip_rect.h = dsth;

        printf("-----1111---------\n");

    	file1 = fopen("/mnt/E/y.dat", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)buf1, srcsize, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

    file1 = fopen("/mnt/E/uv.dat", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
//	ret = fread((void*)(buf3), srcsize/2, 1, file1);
    ret = fread((void*)(buf2), srcsize/4, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

    hal_dcache_clean(buf1, srcsize);
    hal_dcache_clean(buf2, srcsize/2);
  //  hal_dcache_clean(buf3, srcsize/4);

    hal_dcache_clean(buf4, dstsize);
    hal_dcache_invalidate(buf4, dstsize);

    printf("start control\n");
    	arg[0] = (unsigned long)(info2);
	arg[1] = 1;
	ret = dev->control(dev, G2D_CMD_MIXER_TASK, (arg));
	if (ret){
		printf("g2d G2D_CMD_MIXER_TASK fail\n");
		ret = -1;
		goto error;
		}
	else {
		printf("G2D_CMD_MIXER_TASK ok\n");

	}
    hal_dcache_invalidate(buf4, dstsize);
    //hal_dcache_invalidate(buf5, dstsize/4);
   // hal_dcache_invalidate(buf6, dstsize/4);

	file1 = fopen("/mnt/E/scaler_yuv2argb.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite(buf4,dstsize, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);

    hal_free(buf1);
    hal_free(buf2);
	//free(buf3);
	hal_free(buf4);
    hal_free(buf5);
    hal_free(buf6);
out:
    return 0;
error:
    return ret;


}

int scaler_argb2yuv(int argc, const char **argv)
{
    int ret;
    unsigned long arg[6];
    unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;
    char *temp;
    FILE* file1;
    int fb_width, fb_height;

    g2d_blt_h info;
    struct mixer_para info2[2];
    g2d_blt_h blit_para;
    int srcw,srch,srcformat,dstw,dsth,dstformat;
    unsigned long i;
    int srcsize,dstsize;
    int src_w=0,src_h =0;
    int ii;
    printf("hello g2d_test\n");

        printf("-----------scaler_argb2yuv-----------\n");
    printf("argv(%d):\n",argc);
    for(ii=0;ii<argc;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);


    rt_device_t dev = rt_device_find("g2d");

    if (!dev){
        printf("can not find device\n");
        ret = -1;
        goto out;
        }
    if (init_flag == 0){
        dev->init(dev);
        init_flag = 1;
        }
    printf("start open\n");

    ret = dev->open(dev,0);
    if (ret){
        printf("g2d open fail\n");
        ret = -1;
        goto error;
    }

    src_w = srcw;
    src_h = srch;


    printf("src_w:%d,src_h:%d\n",src_w,src_h);
    srcsize = src_w*src_h*4;
    dstsize = dstw*dsth;
    buf1 = hal_malloc(srcsize);

    buf4 = hal_malloc(dstsize);
    buf5 = hal_malloc(dstsize/4);
    buf6 = hal_malloc(dstsize/4);
    memset(buf1, 0, srcsize);

    memset(buf4, 0, dstsize);
    memset(buf5, 0, dstsize/4);
    memset(buf6, 0, dstsize/4);

    memset(&info2, 0, 2*sizeof(struct mixer_para));
        info2[0].src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
        info2[0].src_image_h.laddr[1] = 0;
        info2[0].src_image_h.laddr[2] = 0;

    info2[0].dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);
    info2[0].dst_image_h.laddr[1] = __va_to_pa((uint32_t)buf5);
    info2[0].dst_image_h.laddr[2] = __va_to_pa((uint32_t)buf6);

//  info2[0].src_image_h.laddr[0] = (int)(info2[0].src_image_h.laddr[0] );
//  info2[0].src_image_h.laddr[1] = (int)(info2[0].src_image_h.laddr[0] + srcsize);
    //info2[0].src_image_h.laddr[2] = (int)(info2[0].src_image_h.laddr[0] + srcsize*5/4);
    info2[0].src_image_h.use_phy_addr = 1;
    //info2[0].dst_image_h.laddr[0] = (int)(info2[0].dst_image_h.laddr[0] );
    //info2[0].dst_image_h.laddr[1] = (int)(info2[0].dst_image_h.laddr[0] + dstsize);
    //info2[0].dst_image_h.laddr[2] = (int)(info2[0].dst_image_h.laddr[0] + dstsize*5/4);

    info2[0].dst_image_h.use_phy_addr = 1;
    info2[0].flag_h = G2D_BLT_NONE_H;
    info2[0].op_flag = OP_BITBLT;
    info2[0].src_image_h.format = srcformat;
    info2[0].src_image_h.width = src_w;
    info2[0].src_image_h.height = src_h;

    info2[0].src_image_h.clip_rect.w = srcw;
    info2[0].src_image_h.clip_rect.h = srch;


    info2[0].dst_image_h.format = dstformat;
    info2[0].dst_image_h.width = dstw;
    info2[0].dst_image_h.height = dsth;

    info2[0].dst_image_h.clip_rect.w = dstw;
    info2[0].dst_image_h.clip_rect.h = dsth;

        printf("-----1111---------\n");

        file1 = fopen("/mnt/E/11.dat", "rb+");
    if (file1 == NULL)
        printf("err in fopen");
    ret = fread((void*)buf1, srcsize, 1, file1);
    printf("fread,ret=%d\n", ret);
    fclose(file1);
    hal_dcache_clean(buf1, srcsize);

    hal_dcache_clean(buf4, dstsize);
    hal_dcache_clean(buf5, dstsize/4);
    hal_dcache_clean(buf6, dstsize/4);

    hal_dcache_invalidate(buf4, dstsize);
    hal_dcache_invalidate(buf5, dstsize/4);
    hal_dcache_invalidate(buf6, dstsize/4);

    printf("start control\n");
        arg[0] = (unsigned long)(info2);
    arg[1] = 1;
    ret = dev->control(dev, G2D_CMD_MIXER_TASK, (arg));
    if (ret){
        printf("g2d G2D_CMD_MIXER_TASK fail\n");
        ret = -1;
        goto error;
        }
    else {
        printf("G2D_CMD_MIXER_TASK ok\n");

    }

    hal_dcache_invalidate(buf4, dstsize);
    hal_dcache_invalidate(buf5, dstsize/4);
    hal_dcache_invalidate(buf6, dstsize/4);

    file1 = fopen("/mnt/E/scaler_y.dat", "wb+");
    if (file1 == NULL)
        printf("err in fopen");
    ret = fwrite(buf4,dstsize, 1, file1);
    printf("fwrite ,ret=%d\n",ret);
    fclose(file1);

    file1 = fopen("/mnt/E/scaler_v.dat", "wb+");
    if (file1 == NULL)
        printf("err in fopen");
    ret = fwrite((buf5),dstsize/4, 1, file1);
    printf("fwrite ,ret=%d\n",ret);
    fclose(file1);

        file1 = fopen("/mnt/E/scaler_u.dat", "wb+");
    if (file1 == NULL)
        printf("err in fopen");
    ret = fwrite((buf6),dstsize/4, 1, file1);
    printf("fwrite ,ret=%d\n",ret);
    fclose(file1);
    hal_free(buf1);
    hal_free(buf4);
    hal_free(buf5);
    hal_free(buf6);
out:
    return 0;
error:
    return ret;
}

int scaler_yuv2yuv(int argc, const char **argv)
{
    int ret;
	unsigned long arg[6];
	unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;
	char *temp;
	FILE* file1;
	int fb_width, fb_height;

	g2d_blt_h info;
	struct mixer_para info2[2];
	g2d_blt_h blit_para;
	int srcw,srch,srcformat,dstw,dsth,dstformat;
	unsigned long i;
    int srcsize,dstsize;
    int src_w=0,src_h =0;
    int ii;
	printf("hello g2d_test\n");
    printf("argv(%d):\n",argc);
    for(ii=0;ii<argc;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");
        printf("-----------scaler_yuv2yuv-----------\n");
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);


	rt_device_t dev = rt_device_find("g2d");

	if (!dev){
		printf("can not find device\n");
		ret = -1;
		goto out;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto error;
	}

    if(srcw % 16 != 0)
    {
        src_w = (srcw / 16 + 1) * 16;
    }
    else
    {
        src_w = srcw;
    }
    if(srch % 16 != 0)
    {
        src_h = (srch / 16 + 1) * 16;
    }
    else
    {
        src_h = srch;

    }


     printf("src_w:%d,src_h:%d\n",src_w,src_h);
    srcsize = src_w*src_h;
    dstsize = dstw*dsth;
    buf1 = hal_malloc(srcsize);
    buf2 = hal_malloc(srcsize/4);
	buf3 = hal_malloc(srcsize/4);

    buf4 = hal_malloc(dstsize);
    buf5 = hal_malloc(dstsize/4);
    buf6 = hal_malloc(dstsize/4);
	memset(buf1, 0, srcsize);
    memset(buf2, 0, srcsize/4);
    memset(buf3, 0, srcsize/4);

	memset(buf4, 0x80, dstsize);
    memset(buf5, 0x80, dstsize/4);
    memset(buf6, 0x80, dstsize/4);

    memset(&info2, 0, 2*sizeof(struct mixer_para));
    	info2[0].src_image_h.laddr[0] = __va_to_pa((uint32_t)buf1);
        info2[0].src_image_h.laddr[1] = __va_to_pa((uint32_t)buf2);
        info2[0].src_image_h.laddr[2] = __va_to_pa((uint32_t)buf3);

	info2[0].dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);
    info2[0].dst_image_h.laddr[1] = __va_to_pa((uint32_t)buf5);
    info2[0].dst_image_h.laddr[2] = __va_to_pa((uint32_t)buf6);

//	info2[0].src_image_h.laddr[0] = (int)(info2[0].src_image_h.laddr[0] );
//	info2[0].src_image_h.laddr[1] = (int)(info2[0].src_image_h.laddr[0] + srcsize);
	//info2[0].src_image_h.laddr[2] = (int)(info2[0].src_image_h.laddr[0] + srcsize*5/4);
	info2[0].src_image_h.use_phy_addr = 1;
	//info2[0].dst_image_h.laddr[0] = (int)(info2[0].dst_image_h.laddr[0] );
	//info2[0].dst_image_h.laddr[1] = (int)(info2[0].dst_image_h.laddr[0] + dstsize);
	//info2[0].dst_image_h.laddr[2] = (int)(info2[0].dst_image_h.laddr[0] + dstsize*5/4);

	info2[0].dst_image_h.use_phy_addr = 1;
	info2[0].flag_h = G2D_BLT_NONE_H;
	info2[0].op_flag = OP_BITBLT;
	info2[0].src_image_h.format = srcformat;
	info2[0].src_image_h.width = src_w;
	info2[0].src_image_h.height = src_h;

	info2[0].src_image_h.clip_rect.w = srcw;
	info2[0].src_image_h.clip_rect.h = srch;


	info2[0].dst_image_h.format = dstformat;
	info2[0].dst_image_h.width = dstw;
	info2[0].dst_image_h.height = dsth;

	info2[0].dst_image_h.clip_rect.x = (dstw-666)/2;
	info2[0].dst_image_h.clip_rect.y = (dsth-888)/2;
	info2[0].dst_image_h.clip_rect.w = 666;
	info2[0].dst_image_h.clip_rect.h = 888;

        printf("-----1111---------\n");

    	file1 = fopen("/mnt/E/y.dat", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)buf1, srcsize, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

    file1 = fopen("/mnt/E/uv.dat", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)(buf2), srcsize/4, 1, file1);
    ret = fread((void*)(buf3), srcsize/4, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

    hal_dcache_clean(buf1, srcsize);
    hal_dcache_clean(buf2, srcsize/4);
    hal_dcache_clean(buf3, srcsize/4);

    hal_dcache_clean(buf4, dstsize);
    hal_dcache_clean(buf5, dstsize/4);
    hal_dcache_clean(buf6, dstsize/4);

    hal_dcache_invalidate(buf4, dstsize);
    hal_dcache_invalidate(buf5, dstsize/4);
    hal_dcache_invalidate(buf6, dstsize/4);

    printf("start control\n");
    	arg[0] = (unsigned long)(info2);
	arg[1] = 1;
	ret = dev->control(dev, G2D_CMD_MIXER_TASK, (arg));
	if (ret){
		printf("g2d G2D_CMD_MIXER_TASK fail\n");
		ret = -1;
		goto error;
		}
	else {
		printf("G2D_CMD_MIXER_TASK ok\n");

	}

    hal_dcache_invalidate(buf4, dstsize);
    hal_dcache_invalidate(buf5, dstsize/4);
    hal_dcache_invalidate(buf6, dstsize/4);

	file1 = fopen("/mnt/E/scaler_y.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite(buf4,dstsize, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);

#if 0
   	file1 = fopen("/mnt/E/scaler_v.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite((buf5),dstsize/4, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);

       	file1 = fopen("/mnt/E/scaler_u.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite((buf6),dstsize/4, 1, file1);
		printf("fwrite ,ret=%d\n",ret);
	fclose(file1);
#else
   	file1 = fopen("/mnt/E/scaler_uv.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite((buf5),dstsize/4, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	//fclose(file1);

     //  	file1 = fopen("/mnt/E/scaler_u.dat", "wb+");
	//if (file1 == NULL)
//		printf("err in fopen");
	ret = fwrite((buf6),dstsize/4, 1, file1);
		printf("fwrite ,ret=%d\n",ret);
	fclose(file1);
#endif

    hal_free(buf1);
    hal_free(buf2);
	hal_free(buf3);
	hal_free(buf4);
    hal_free(buf5);
    hal_free(buf6);
out:
    return 0;
error:
    return ret;
}

int scaler_argb2argb(int argc, const char **argv)
{
    int ret;
	unsigned long arg[6];
	unsigned long *buf1 = NULL,*buf2 = NULL,*buf3 = NULL,*buf4 = NULL,*buf5 = NULL,*buf6 = NULL;
	char *temp;
	FILE* file1;
	int fb_width, fb_height;

	g2d_blt_h info;
	struct mixer_para info2[2];
	g2d_blt_h blit_para;
	int srcw,srch,srcformat,dstw,dsth,dstformat;
	unsigned long i;
    int srcsize,dstsize;
    int src_w;
    int ii;
	printf("hello g2d_test\n");
    printf("argv(%d):\n",argc);
    for(ii=0;ii<argc;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");
        printf("-----------scaler_argb2argb-----------\n");
    srcw = atoi(argv[1]);
    srch = atoi(argv[2]);
    srcformat = atoi(argv[3]);

    dstw = atoi(argv[4]);
    dsth = atoi(argv[5]);
    dstformat = atoi(argv[6]);


	rt_device_t dev = rt_device_find("g2d");

	if (!dev){
		printf("can not find device\n");
		ret = -1;
		goto out;
		}
	if (init_flag == 0){
		dev->init(dev);
		init_flag = 1;
		}
	printf("start open\n");

	ret = dev->open(dev,0);
	if (ret){
		printf("g2d open fail\n");
		ret = -1;
		goto error;
	}

    if(srcw % 16 != 0)
    {
        src_w = (srcw / 16 + 1) * 16;
    }
    else
    {
        src_w = srcw;
    }

    srcsize = srcw*srch;
    dstsize = dstw*dsth;
	buf3 = hal_malloc(src_w*srch*4);
	buf4 = hal_malloc(dstsize*4);
	memset(buf3, 0, srcsize*4);
	memset(buf4, 0, dstsize*4);

    memset(&info2, 0, 2*sizeof(struct mixer_para));
    	info2[0].src_image_h.laddr[0] = __va_to_pa((uint32_t)buf3);
	info2[0].dst_image_h.laddr[0] = __va_to_pa((uint32_t)buf4);

	//info2[0].src_image_h.laddr[0] = (int)(info2[0].src_image_h.laddr[0] );
	//info2[0].src_image_h.laddr[1] = (int)(info2[0].src_image_h.laddr[0] + srcsize);
	//info2[0].src_image_h.laddr[2] = (int)(info2[0].src_image_h.laddr[0] + srcsize*5/4);
	info2[0].src_image_h.use_phy_addr = 1;
	//info2[0].dst_image_h.laddr[0] = (int)(info2[0].dst_image_h.laddr[0] );
	//info2[0].dst_image_h.laddr[1] = (int)(info2[0].dst_image_h.laddr[0] + dstsize);
	//info2[0].dst_image_h.laddr[2] = (int)(info2[0].dst_image_h.laddr[0] + dstsize*5/4);

   // src_w = (srcw%4)== 0?srcw:(srcw / 4 + 1) * 4;

	info2[0].dst_image_h.use_phy_addr = 1;
	info2[0].flag_h = G2D_BLT_NONE_H;
	info2[0].op_flag = OP_BITBLT;
	info2[0].src_image_h.format = srcformat;
	info2[0].src_image_h.width = srcw;
	info2[0].src_image_h.height = srch;

    printf("[YG]src_w=%d\n", src_w);

	info2[0].src_image_h.clip_rect.w = srcw;
	info2[0].src_image_h.clip_rect.h = srch;


	info2[0].dst_image_h.format = dstformat;
	info2[0].dst_image_h.width = dstw;
	info2[0].dst_image_h.height = dsth;

	info2[0].dst_image_h.clip_rect.x = 0;
	info2[0].dst_image_h.clip_rect.y = 0;
	info2[0].dst_image_h.clip_rect.w = dstw;
	info2[0].dst_image_h.clip_rect.h = dsth;


    	file1 = fopen("/mnt/E/11.dat", "rb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fread((void*)buf3, srcsize*4, 1, file1);
	printf("fread,ret=%d\n", ret);
	fclose(file1);

    hal_dcache_clean(buf3, srcsize*4);

   hal_dcache_clean(buf4, dstsize*4);
    hal_dcache_invalidate(buf4, dstsize*4);
    printf("start control\n");
    	arg[0] = (unsigned long)(info2);
	arg[1] = 1;
	ret = dev->control(dev, G2D_CMD_MIXER_TASK, (arg));
	if (ret){
		printf("g2d G2D_CMD_MIXER_TASK fail\n");
		ret = -1;
		goto error;
		}
	else {
		printf("G2D_CMD_MIXER_TASK ok\n");

	}
	    printf("end control 11\n");

    hal_dcache_invalidate(buf4, dstsize*4);
	file1 = fopen("/mnt/E/scaler_argb.dat", "wb+");
	if (file1 == NULL)
		printf("err in fopen");
	ret = fwrite(buf4,dstsize*4, 1, file1);
	printf("fwrite ,ret=%d\n",ret);
	fclose(file1);

	hal_free(buf3);
	hal_free(buf4);
out:
    return 0;
error:
    return ret;


}

static int cmd_g2d(int argc, const char **argv)
{
    Aw_G2d* aw_g2d;
    Aw_G2dParam info;
    int i,ii;
    FILE* rfp=NULL;
    char *rbuf = NULL;
    int srcw,srch,srcformat;
    int dstw,dsth,dstformat;
    int crop_x,crop_y,crop_w,crop_h;
    int size;

    printf("hello g2d_test\n");
    printf("argv:");
    for(ii=0;ii<11;ii++)
    {
        printf("%s,\n",argv[ii]);
    }
    printf("\n");

  //fillrect(argv);
 // scaler_yuv(argv);
 //scaler_argb2argb(argv);
 // scaler_yuv2argb(argv);
//  scaler_argb2yuv(argv);
 //scaler_yuv2yuv(argv);
 //rotate_yuv_new(argv);

  //rotate_argb_new(argv);
  //blend(argv);


 //   scaler_argb(argv);
#if 0
    aw_g2d = G2dCreate();
    if(aw_g2d == NULL)
    {
        printf("G2dCreate fail\n");
        return -1;
    }

    blend(argv,aw_g2d);
   // fillrect_1(argv,aw_g2d);
 //   scaler(argv,aw_g2d);
   //rotate_yuv(argv,aw_g2d);
  // rotate_argb(argv,aw_g2d);
#endif

}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_g2d, __cmd_g2d, g2d);

FINSH_FUNCTION_EXPORT_ALIAS(rotate_argb_new, __cmd_rotate_argb_new,rotate_argb_new);
FINSH_FUNCTION_EXPORT_ALIAS(rotate_yuv_new, __cmd_rotate_yuv_new, rotate_yuv_new);
FINSH_FUNCTION_EXPORT_ALIAS(scaler_yuv2argb, __cmd_scaler_yuv2argb, scaler_yuv2argb);
FINSH_FUNCTION_EXPORT_ALIAS(scaler_argb2yuv, __cmd_scaler_argb2yuv, scaler_argb2yuv);
FINSH_FUNCTION_EXPORT_ALIAS(scaler_yuv2yuv, __cmd_scaler_yuv2yuv, scaler_yuv2yuv);
FINSH_FUNCTION_EXPORT_ALIAS(scaler_argb2argb, __cmd_scaler_argb2argb, scaler_argb2argb);


