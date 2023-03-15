/*
 * =====================================================================================
 *   Copyright (c)  Allwinner Technology Co. Ltd.
 *   All rights reserved.
 *
 *   Filename:  ISE_lib_gdc.h
 *   Description:
 *   Version:  1.0
 *   Created:  08/08/2019 04:18:11 PM
 *   Author:  Gan Qiuye(ganqiuye@allwinnertech.com)
 * =====================================================================================
 */
#ifndef _ISE_LIB_GDC_H_
#define _ISE_LIB_GDC_H_

#include "ISE_common.h"
#include "stdint.h"

#ifndef DISTORTORDER_FISHEYE
#define DISTORTORDER_FISHEYE       (5)
#endif

#ifndef DISTORTORDER_WIDEANGLE_RA
#define DISTORTORDER_WIDEANGLE_RA  (3)
#endif

#ifndef DISTORTORDER_WIDEANGLE_TA
#define DISTORTORDER_WIDEANGLE_TA  (2)
#endif

#ifndef DISTORTORDER_FISHEYE_K
#define DISTORTORDER_FISHEYE_K     (4)
#endif

#ifndef PI
#define PI  (3.1415926f)
#endif

#ifndef DEG2RAD
#define DEG2RAD(angle) ((angle)*PI/180.0f)
#endif

//#ifndef DBL_EPSILON
//#define DBL_EPSILON    (2.2204460492503131e-16)
//#endif
//
//#ifndef FLT_EPSILON
//#define FLT_EPSILON    (1.192092896e-07F)
//#endif

#ifndef DIM_PROJECTIONMAT
#define DIM_PROJECTIONMAT    (9)
#endif
//安装方式
typedef enum _eMount_Type_{

    Mount_Top,

    Mount_Wall,

    Mount_Bottom
}eMount_Type;
//矫正方式
typedef enum _eWarpType_ {

    Warp_LDC,

    Warp_Pano180,

    Warp_Pano360,

    Warp_Normal,

    Warp_Fish2Wide,

    Warp_Perspective,

    Warp_BirdsEye,

	Warp_Ptz4In1, //7

    Warp_User
}eWarpType;

typedef enum _ePerspFunc_ {
    Persp_Only,
    Persp_LDC
}ePerspFunc;

typedef enum _eLensDistModel_ {
    DistModel_WideAngle,//广角镜头
    DistModel_FishEye//鱼眼镜头
}eLensDistModel;

typedef struct VEC2D
{
    float x;
    float y;

}sVec2D;

typedef struct VEC3D
{
    float x;
    float y;
    float z;

}sVec3D;

typedef struct ROTATEPARA
{
    float roll;//相机坐标系绕自身X 轴正向逆时针旋转的角度。
    float pitch;//相机坐标系绕自身Y 轴正向逆时针旋转的角度。
    float yaw;//相机坐标系绕自身Z 轴正向逆时针旋转的角度。
}sRotPara;

typedef struct WARPPARA
{
    int radialDistortCoef;    //[-255,255] 径向畸变调节系数。
    int fanDistortCoef;       //[-255,255]
    int trapezoidDistortCoef; //[-255,255] 梯形畸变调节系数。
}sWarpPara;

//LDC 主要支持桶型畸变校正与枕型畸变校正
typedef struct LENSDISTORTIONPARA
{
    //镜头参数列表
    float fx;//(0, W] 焦距值在水平方向的标量
    float fy;//(0, H] 焦距值在竖直方向的标量
    float cx;//(0, W] 主点横坐标
    float cy;//(0, H] 主点纵坐标

    float fx_scale;//新焦距值在水平方向的标量
    float fy_scale;//新焦距值在竖直方向的标量
    float cx_scale;//新主点横坐标
    float cy_scale;//新主点纵坐标

    //畸变系数列表
    eLensDistModel distModel;//畸变模型
    float distCoef_wide_ra[DISTORTORDER_WIDEANGLE_RA];//广角径向畸变系数
    float distCoef_wide_ta[DISTORTORDER_WIDEANGLE_TA];//广角切向畸变系数
    float distCoef_fish_k[DISTORTORDER_FISHEYE_K];//鱼眼畸变系数

    //LDC 校正参数列表
    int zoomH;//[0,200] 水平视场角的缩放尺度，小于100 放大，大于100 缩小。
    int zoomV;//[0,200] 垂直视场角的缩放尺度，小于100 放大，大于100 缩小。
    int centerOffsetX;//[-W,W] 图像中心点相对物理中心点的水平偏移。
    int centerOffsetY;//[-H,H] 图像中心点相对物理中心点的竖直偏移。
    int rotateAngle; //[0,360]//[0,360] 图像顺时针旋转的角度。

    ePerspFunc perspFunc;//
    float perspectiveProjMat[DIM_PROJECTIONMAT];//
}sLensDistortPara;

typedef struct FISHEYELENSPARA
{
    int width;//输出图像宽       宽等于输入图像宽           需注意宽度配置需为32 的整数倍
    int height;//输出图像高 高度为输入图像的一半
    float cx;//中心点x 坐标；[0, width]
    float cy;//中心点y 坐标；[0, width]
    float radius;//鱼眼图像半径；[0, width]
    float fov;//鱼眼镜头视场角；(0,220]
    float k[DISTORTORDER_FISHEYE];//畸变系数；[-4096,4096]
    int k_order; //<=5//包含的畸变系数数目；[1,5]
}sFishEyeLensPara;

typedef struct PTZPARA
{
    int pan;//[-90,90] 视场角在水平方向的旋转角度。
    int tilt;//[-90,90] 视场角在垂直方向的旋转角度。
    int zoomH;//[0,100] 水平视场角大小的百分比。
    int zoomV;//[0,100] 垂直视场角大小的百分比。
}sPtzPara;
typedef struct FISHEYEPARA
{
	sPtzPara ptz;
    int scale;//缩放因子，当输出分辨率非典型值时，配置该参数。
    int innerRadius;//[0,radius]校正内圈半径。

    eMount_Type mountMode;//安装方式
    sRotPara rotPara;//相机坐标系及旋转角
    sFishEyeLensPara lensParams;//镜头参数
}sFishEyePara;
//鸟瞰投影算法需搭配畸变校正使用
typedef struct BIRDSEYEVIEWPARA
{
    sRotPara rotPara;//相机坐标系及旋转角

    int offsetX;//[-W,W] 鸟瞰图中心相对物理中心的水平偏移量
    int offsetY;//[-H,H] 鸟瞰图中心相对物理中心的竖直偏移量
    int zoomH;//鸟瞰图水平方向的缩放系数
    int zoomV;//鸟瞰图竖直方向的缩放系数
    float mountHeight;//(0,n] 相机实际安装相距地面的高度（米）
    //输出鸟瞰图宽为输入宽的40%，高度相同 需注意宽度配置需为32的整数倍
    int outWidth;//
    int outHeight;//

    float roiDist_ahead;//
    float roiDist_left;//
    float roiDist_right;//
    float roiDist_bottom;//

    float projMat_birds2src[DIM_PROJECTIONMAT];//
    float projMat_src2birds[DIM_PROJECTIONMAT];//
    float projMat_vehicle2birds[DIM_PROJECTIONMAT];//
    float projMat_birds2vehicle[DIM_PROJECTIONMAT];//

}sBirdsEyePara;

typedef struct RECTIFYPARA
{
    eWarpType warpMode;//矫正方式

    sWarpPara warpPara;//畸变系数
    sLensDistortPara LensDistPara;//LDC矫正参数
    sFishEyePara fishEyePara;//鱼眼矫正参数
    sBirdsEyePara birdsEyePara;//鸟瞰矫正参数
	sPtzPara ptz4In1Para[4];
}sRectifyPara;

typedef enum IMAGE_FORMAT
{
    PLANE_YUV420  = 0x0,
    PLANE_YVU420  = 0x01,
    PLANE_YUV420p = 0x02,
    PLANE_YUV422  = 0x03,
    PLANE_YVU422  = 0x04,
    PLANE_YUV422p = 0x05,
    PLANE_RGB     = 0x06,
    PLANE_RGBp    = 0x07
}IMAGE_FORMAT;

//! 图像数据结构体
typedef struct UserImageHeader
{
    int32_t width;          // 图像长度
    int32_t height;         // 图像宽度
    //int32_t channel;

    uint8_t *pdata[3];       // 数组指针，分别指向Y、U、V存储数据地址
    int32_t stride[3];       // Y、U、V数据的存储宽度
    int32_t pixels_step[3];  // Y、U、V数据存储步长

    IMAGE_FORMAT img_format;
}IMGHeader;

//*the callback, you can comply it in your code for "warpMode == Warp_User"
//*the userData is defined by your own struct, can be NULL
typedef void (*cbRectifyPixel)(void* userData, sVec2D srcPos, sVec2D *dstPos, sRectifyPara rectPara);

typedef struct _ISE_CFG_PARA_GDC_{
    IMGHeader       srcImage;//输入图像参数
    IMGHeader       birdsEyeImage;//鸟瞰输出图像参数
    IMGHeader       undistImage;//畸变矫正输出图像参数
    sRectifyPara    rectPara;//矫正参数
    char            reserved[32];
}ISE_CFG_PARA_GDC;
//图像数据地址
typedef struct _ISE_PROCIN_PARA_GDC_{
    unsigned int        in_luma_phy_Addr;//输入图像的亮度分量物理地址 Y
    void*               in_luma_mmu_Addr;//输入图像的亮度分量虚拟地址 Y
    unsigned int        in_chroma_phy_Addr;//输入图像的色度分量物理地址 UV
    void*               in_chroma_mmu_Addr;//输入图像的色度分量虚拟地址 UV
    char                reserved[32];
}ISE_PROCIN_PARA_GDC;

typedef struct _ISE_PROCOUT_PARA_GDC_{
    unsigned int        out_luma_phy_Addr[1+MAX_SCALAR_CHNL];//输出Y 物理
    void*               out_luma_mmu_Addr[1+MAX_SCALAR_CHNL];//输出Y 虚拟
    unsigned int        out_chroma_u_phy_Addr[1+MAX_SCALAR_CHNL];//输出U 物理
    void*               out_chroma_u_mmu_Addr[1+MAX_SCALAR_CHNL];//输出U 虚拟
    unsigned int        out_chroma_v_phy_Addr[1+MAX_SCALAR_CHNL];//输出V 物理
    void*               out_chroma_v_mmu_Addr[1+MAX_SCALAR_CHNL];//输出V 虚拟
    char                reserved[32];
}ISE_PROCOUT_PARA_GDC;


// 接口函数
int ISE_Create_GDC(ISE_HANDLE_GDC *handle);

void ISE_SetFrq_GDC(ISE_HANDLE_GDC *handle, int freMHz);

int ISE_SetCallBack_GDC(ISE_HANDLE_GDC *handle, cbRectifyPixel callback, void* userData);

int  ISE_SetAttr_GDC(ISE_HANDLE_GDC *handle, ISE_CFG_PARA_GDC gdc_cfg);

int ISE_Proc_GDC(
    ISE_HANDLE_GDC  *handle,
    ISE_PROCIN_PARA_GDC  *ise_procin,
    ISE_PROCOUT_PARA_GDC *ise_procout,
    eWarpType warpMode);

int ISE_Destroy_GDC(ISE_HANDLE_GDC *handle);

#endif
