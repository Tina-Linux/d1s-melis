/*
*********************************************************************************************************
*                                                   eMOD
*                                           the Easy Module System
*                                          drv_touchpanel sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : drv_touchpanel.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/
#ifndef _DRV_TOUCHPANEL_H_
#define _DRV_TOUCHPANEL_H_

#include "typedef.h"
//#include "cfgs.h"

#define    TP_DEFAULT_MSG_PERTIME           (3)
#define    TP_DEFAULT_OFFSET                (12)//(16)

#define    EV_TP_PRESS_START                0xf0    //按下的起点
#define    EV_TP_PRESS_MOVE                 0xf1    //按下时候在滑动
#define    EV_TP_PRESS_HOLD                 0xf2    //按下时候不动, 保留

#define    EV_TP_PINCH_IN                   0xf4   //  zoom in 
#define    EV_TP_PINCH_OUT                  0xf5   //  zoom out
#define    EV_TP_PINCH_HOLD                 0xf6   //  hold


/************************触摸屏动作*************************
*
*以下同样处于msg_type数据项中，默认出现一个抬起消息
*
*************************************************************/
#define    EV_TP_ACTION_NULL                0x00    //空白动作或无法识别的动作
#define    EV_TP_ACTION_CLICK               0x01    //触摸屏单击
#define    EV_TP_ACTION_DOWN                0x02    //触摸屏下滑--used
#define    EV_TP_ACTION_UP                  0x12    //触摸屏上滑--used
#define    EV_TP_ACTION_LEFT                0x03    //触摸屏左滑--used
#define    EV_TP_ACTION_RIGHT               0x13    //触摸屏右滑--used
#define    EV_TP_ACTION_ANTICLOCKWISE       0x04    //触摸屏逆时针滑动
#define    EV_TP_ACTION_CLOCKWISE           0x14    //触摸屏顺时针滑动
#define    EV_TP_ACTION_LD2RU               0x05    //触摸屏斜线滑动,左下到右上
#define    EV_TP_ACTION_RU2LD               0x15    //触摸屏斜线滑动,右上到左下
#define    EV_TP_ACTION_LU2RD               0x06    //触摸屏反斜线滑动,左上到右下
#define    EV_TP_ACTION_RD2LU               0x16    //触摸屏反斜线滑动,右下到左上

typedef struct __EV_TP_MSG
{
    __u8   msg_pattern;     //消息种类，用于和输入设备兼容，用户暂时不关心,保留
    __u8   msg_type;        //消息类型，区分是按下的起点，滑动，按下不动，或者是抬起时候的手势
    __s16  xpoint;          //当前点的X坐标值
    __s16  ypoint;          //当前点的Y坐标值
    __u16  speed_dir;       //抬起时候移动的方向
    __u16  speed_val;       //抬起时候速度的大小, 速度分级:0-6

} __ev_tp_msg_t;

typedef struct __EV_TP_POS
{
    __u16  disp_xpos;       //屏幕对应的x,y坐标值
    __u16  disp_ypos;
} __ev_tp_pos_t;

/*
关于TP的屏幕校准

    Tp的屏幕校准command只有一条，即DRV_TP_CMD_ADJUST，要完成这个功能则需要连续使用四次这条命令
    需要一个数据结构  __ev_tp_pos_t  my_pos;

    调用命令 eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 0, (void *)&my_pos);

            在数据结构my_pos的disp_xpos和disp_ypos中填入屏幕坐标点，在 aux 中填写校准步骤
            调用此命令后，将等待用户对step1-4中提示的点进行点击，直到用户点击后才返回
            建议APP以此点为中心，绘出一些标志性图标，如圆圈，十字等，并提示用户对该点进行点击。

    以上，是对一个点进行校准的标准步骤。通常情况下，屏幕校准应该有四个校准点，并且应该是
    呈矩形分布的四个点。当对这样的四个点依次进行上面的校准方法后，驱动就可以获取到足够的数据，
    完成正确的计算。驱动对于这四个点有要求，第一个和第二个点的x坐标相同，第二个和第三个的y坐标相同，
    第三个和第四个的x坐标相同，第四个和第三个的y坐标相同,这样就呈现一个矩形状态
            adjust_step的顺序一定要正确，在驱动中会做校准，即顺序依次是1，2，3，4，并且与坐标值的变化对应

    以800×480屏幕校准简单教程
        step1:
            通知驱动准备校准TP，驱动开始做相应的工作
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 0, 0);
            执行完毕后立即返回
        step2:
            my_pos.disp_xpos = 100;
            my_pos.disp_ypos = 100;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 1, (void *)&my_pos);    //等待用户点击，点击后返回
            //用户点击后返回，这里不判断成功或失败
        step3:
            my_pos.disp_xpos = 700;
            my_pos.disp_ypos = 100;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 2, (void *)&my_pos);    //等待用户点击，点击后返回
            //用户点击后返回，这里不判断成功或失败
        step4:
            my_pos.disp_xpos = 700;
            my_pos.disp_ypos = 400;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 3, (void *)&my_pos);    //等待用户点击，点击后返回
            //用户点击后返回，这里不判断成功或失败
        step5:
            my_pos.disp_xpos = 100;
            my_pos.disp_ypos = 400;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 4, (void *)&my_pos);    //等待用户点击，校准后返回
            //这里已经取得四个数据，驱动首先判断数据时候有效，成功则开始校准。用户需要判断返回值，如果成功，
            //则退出校准，如果失败，应该返回step2重新开始读取数据

*/

/*
新的5点校准
    5点校准是在原来4点的基础上做的改进，主要是为了解决触摸屏在安装的时候由于某些原有没有安装正确，出现了
    倾斜等的问题。

    Tp的屏幕校准command只有一条，即DRV_TP_CMD_ADJUST，要完成这个功能则需要连续使用五次这条命令
    需要一个数据结构  __ev_tp_pos_t  my_pos;

    调用命令 eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 0, (void *)&my_pos);

            在数据结构my_pos的disp_xpos和disp_ypos中填入屏幕坐标点，在 aux 中填写校准步骤
            调用此命令后，将等待用户对step1-4中提示的点进行点击，直到用户点击后才返回
            建议APP以此点为中心，绘出一些标志性图标，如圆圈，十字等，并提示用户对该点进行点击。

    以上，是对一个点进行校准的标准步骤。通常情况下，屏幕校准应该有五个校准点，并且应该是
    呈矩形分布的四个点，以及矩形中心的一个点。当对这样的五个点依次进行上面的校准方法后，驱动就可以获取到足够的数据，
    完成正确的计算。驱动对于这五个点有要求，第一个和第二个点的x坐标相同，第二个和第三个的y坐标相同，
    第三个和第四个的x坐标相同，第四个和第三个的y坐标相同,这样就呈现一个矩形状态。第五个点出现在屏幕的中间。
            adjust_step的顺序一定要正确，在驱动中会做校准，即顺序依次是1，2，3，4，5，并且与坐标值的变化对应


特别说明：
    当校准到其中的时候，由于某些原因不希望继续向下校准，可以调用以下的函数，通知触摸屏驱动，可以实现退出驱动的
    校准部分，恢复驱动的点触功能
    eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 6, (void *)&my_pos);
    当调了以上的函数以后，如果需要重新进行校准，需要全部重新来。

    以800×480屏幕校准简单教程
        step1:
            通知驱动准备校准TP，驱动开始做相应的工作
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 0, 0);
            执行完毕后立即返回
        step2:
            my_pos.disp_xpos = 100;
            my_pos.disp_ypos = 100;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 1, (void *)&my_pos);    //等待用户点击，点击后返回
            //用户点击，驱动会判断当前的点击是否正确，如果正确返回成功，否则返回失败
            //应用程序应该在收到失败的时候重新发送这个命令以及同样的参数
        step3:
            my_pos.disp_xpos = 700;
            my_pos.disp_ypos = 100;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 2, (void *)&my_pos);    //等待用户点击，点击后返回
            //用户点击，驱动会判断当前的点击是否正确，如果正确返回成功，否则返回失败
            //应用程序应该在收到失败的时候重新发送这个命令以及同样的参数
        step4:
            my_pos.disp_xpos = 700;
            my_pos.disp_ypos = 400;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 3, (void *)&my_pos);    //等待用户点击，点击后返回
            //用户点击，驱动会判断当前的点击是否正确，如果正确返回成功，否则返回失败
            //应用程序应该在收到失败的时候重新发送这个命令以及同样的参数
        step5:
            my_pos.disp_xpos = 100;
            my_pos.disp_ypos = 400;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 4, (void *)&my_pos);    //等待用户点击，校准后返回
            //用户点击，驱动会判断当前的点击是否正确，如果正确返回成功，否则返回失败
            //应用程序应该在收到失败的时候重新发送这个命令以及同样的参数
        step5:
            my_pos.disp_xpos = 400;
            my_pos.disp_ypos = 250;
            eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 4, (void *)&my_pos);    //等待用户点击，校准后返回
            //用户点击，驱动会判断当前的点击是否正确，如果失败返回失败
            //应用程序应该在收到失败的时候重新发送这个命令以及同样的参数
            //这里已经取得五个数据，驱动首先判断数据时候有效，成功则开始校准，否则返回-2.用户需要判断返回值，如果成功，
            //则退出校准，如果是-2，应该返回step2重新开始读取数据

*/

/*
特别说明：
    当校准到其中的时候，由于某些原因不希望继续向下校准，可以调用以下的函数，通知触摸屏驱动，可以实现退出驱动的
    校准部分，恢复驱动的点触功能
    eLIBs_fioctrl(hdle, DRV_TP_CMD_ADJUST, 6, (void *)&my_pos);
    当调了以上的函数以后，如果需要重新进行校准，需要全部重新来。

*/
typedef enum __DRV_TOUCHPANEL_CMD
{
    DRV_TP_CMD_NULL       = 0,
    DRV_TP_CMD_REG,                    //挂载回调函数，如果前面存在回调函数，会自动替换前一个回调函数
    DRV_TP_CMD_UNREG,                  //卸载回调函数，卸载后，如果前面存在回调函数，会自动使用前一个回调函数
    DRV_TP_CMD_ADJUST,                 //屏幕校准命令，参见上面说明
    DRV_TP_CMD_SET_OFFSET_INFO,        //设置像素偏差，x或者y方向超过这个值认为是一次move  aux = 偏差值，默认是前面的宏 TP_DEFAULT_OFFSET
    DRV_TP_CMD_GET_OFFSET_INFO,        //获取像素偏差
    DRV_TP_CMD_SET_MSG_PERTIME,        //设置发送间隔，采样多少次发送一个消息，aux = 发送间隔，默认是前面的宏 TP_DEFAULT_MSG_PERTIME
    DRV_TP_CMD_GET_MSG_PERTIME,        //获取发送间隔
    DRV_TP_CMD_SET_WORKMODE,           //设置工作模式，设置普通模式，以及快速模式 aux = {_drv_tp_workmode_t}，参见下方数据结构和说明
    DRV_TP_CMD_

} __drv_touchpanel_cmd_t;


typedef enum _DRV_TP_WORKMODE
{
    DRV_TP_MODE_NORMAL,
    DRV_TP_MODE_FAST,
    DRV_TP_MODE_DUAL,
    DRV_TP_MODE_XXX,
}
_drv_tp_workmode_t;
/*
    名词解释  ：普通模式，快速模式, 两点模式

    使用这样的场合，是为了解决应用场景的不同
    单点模式:
    普通模式适用于用户通常的操作；
    快速模式，适用于输入法的识别；
    两点模式:
    适用于放大缩小的应用场景，比如图片的放大或缩小，地图的放大或缩小等。

*/

#endif  /*_DRV_TOUCHPANEL_H_*/
