#ifndef _AW_G2D_H_
#define _AW_G2D_H_

#include "g2d_driver_enh.h"

#define G2D_PIC_CHANNEL_3 3
#define G2D_PIC_CHANNEL_2 2
#define G2D_PIC_CHANNEL_1 1


typedef struct AwG2dT
{
	int        g2dFd;
	int        g2dInfoNum;
	struct mixer_para* g2dInfo;
} AwG2d;

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

typedef struct AwG2dParamT
{
	int g2dTotalNum;
	ScaleInfo* scaleinfo;
} AwG2dParam;

AwG2d* AwG2dCreate();
int  AwG2dInit(AwG2d* self, AwG2dParam param);
int  AwG2dProcess(AwG2d* self);
void AwG2dDestroy(AwG2d* self);
int AwG2dCopy(AwG2d *self, void *phy_dst, ImageAddr phy_src, unsigned int width,
	      unsigned int height, int format);

#endif
